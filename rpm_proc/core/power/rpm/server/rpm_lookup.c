/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <stdbool.h>
#include <alloca.h>
#include "rpm_config.h"
#include "CoreVerify.h"
#include <string.h>

#define CEILING_DIV(a,b) (((a)+((b)-1))/(b))

// The index a resource is stored at in the 'resources' array above is
// determined by a minimal perfect hash.  That is, when we need to find the
// ResourceData for a particular resource's (type, id) pair, we hash the
// pair and lookup the appropriate index in resources.
//
// Obviously, coming up with a perfect hash for these resources is a bit
// tricky.  We don't know all the possible (type, id) pairs up front, which
// makes the normal apporaches to minimal perfect hashing more difficult.
// Instead we trade startup time for a more time-complex hash table
// construction algorithm.  This is imperfect, but as long as we continue
// to assume that we aren't allowed to have a priori knowledge of (type,
// id) pairs *and* cannot consume any extra memory, we need to trade off
// something, and the only remaining quantity is boot speed.
//
// The way the hash algorithm does the lookup is that it first performs a
// very short MurmurHash3-derived hash of the (type, id) pair with a
// starting seed of 0.  Because we have 2 32-bit input values and wish to
// provide a single 32-bit output value, the hash is quick.
//
// After this first round of hashing it is possible for there to be
// collisions between various (type, id) pairs.  To resolve the collisions
// we use 2-level lookup scheme: the first hash is used to look up a seed
// value for which no collisions exist.
//
// In the first level table, a value of '0' means that no resource has ever hit
// that has index before.  During lookup, if a 0 is found in the first level
// hash, we know that resource does not exist.  When inserting a new resource,
// a value of '0' either means "no collision" or possible "the first
// collision" and it is necessary to check the second level table and check
// if that slot is occupied.
//
// If the first level table has a negative value, that means this is the only
// resource which resolves to this particular hash index.  Therefore, the index
// at which this resource is stored is *directly* at the index (-value - 1).
// If the value is positive, then there was a collision for this resource in
// the past.  The value in the first level table is a seed value for which, if
// the hash of (type, id) is repeated starting from that seed, there will be no
// collision when using the rehashed value to index the second level table.
//
// When inserting a new value, this is all very wishy washy.  This
// algorithm deviates from minimal perfect hashing literature in
// significant ways, and it is possible for the modified hash construction
// algorithm to fail, or take prohibitve computation time (as the time
// complexity of inserting is very unfavorable when finding collisions).
// However, the assumption is that number of resources is relatively small, the
// hash space is generally favorable, and new resources are added infrequently,
// so it should be possible to tweak the algorithm by small amounts if an
// undesireable state is found in production.

#include "rpm_hash.h"
#define resource_hash(seed,type,id) rpm_hash64s((seed),(type),(id))

static inline bool is_correct_resource(ResourceData *r, rpm_resource_type resource, unsigned id)
{
    return (r->id == id && rpm->classes[r->class_idx].type == resource);
}

ResourceData *rpmserver_lookup_resource(rpm_resource_type resource, unsigned id)
{
    unsigned resource_slots = rpm->supported_resources;
    unsigned first_idx = resource_hash(0, resource, id) % resource_slots;
    int16_t  first_val = rpm->resource_seeds[first_idx];

    // If the first level table contains 0, we've never inserted anything at
    // this idx before.  The lookup has definitely failed and there is no such
    // resource.
    if(!first_val)
        return 0;

    // If the first level table contains a negative number, then we have a
    // single resource that hashed to this idx in the past.  It is stored
    // directly at (-first_val - 1).  We need to check if that resource
    // matches, and if so we've found the right one.
    unsigned      second_idx;
    ResourceData *r;

    if(first_val < 0)
    {
        second_idx = -first_val - 1;
        r          = &(rpm->resources[second_idx]);
    }
    else
    {
        second_idx = resource_hash(first_val, resource, id) % resource_slots;
        r          = &(rpm->resources[second_idx]);
    }

    // If everything matches, we found it.  Otherwise, it must not exist.
    if(is_correct_resource(r, resource, id))
        return r;
    else
        return 0;
}

static bool slot_is_usable(unsigned resource_idx, uint32_t *collision_bitfield)
{
    unsigned resource_slots = rpm->supported_resources;

    ResourceData *r = &(rpm->resources[resource_idx]);
    
    // If this slot is empty, it won't have a current state pointer.
    // We can always use an empty slot.
    if(!r->current_state)
        return true;

    // We can freely move any resource which has no first-level collisions.
    ResourceClassData *rc = &(rpm->classes[r->class_idx]);
    unsigned first_idx = resource_hash(0, rc->type, r->id) % resource_slots;
    int16_t  first_val = rpm->resource_seeds[first_idx];
    if(first_val < 0)
        return true;

    // If this resource is already in our collision set, then we can use it
    // (since we've essentially already "reserved" the space with an existing
    // resource from the set we're relocating now).
    if(collision_bitfield[resource_idx/32] & (1 << (resource_idx % 32)))
        return true;

    return false;
}

static bool relocate_resource(unsigned resource_idx)
{
    unsigned resource_slots = rpm->supported_resources;

    ResourceData *r = &(rpm->resources[resource_idx]);

    // If this table entry is empty, we shouldn't be "relocating" it.
    CORE_VERIFY(r->current_state);

    ResourceClassData *rc = &(rpm->classes[r->class_idx]);
    unsigned first_idx = resource_hash(0, rc->type, r->id) % resource_slots;
    int16_t  first_val = rpm->resource_seeds[first_idx];

    // We beleive there's a resource here, so the top level entry better not be 0.
    CORE_VERIFY(first_val);

    // If there are top-level collisions on this resource, we can't simply
    // relocate it.  Most likely this means that the resource is also part of
    // the collision set we're reordering, and we'll handle this with a swap.
    if(first_val > 0)
        return false;

    // Find a new empty slot and swap this one in.
    for(unsigned i = 0; i < resource_slots; ++i)
    {
        ResourceData *slot = &(rpm->resources[i]);

        if(!slot->current_state)
        {
            // Found an empty slot, dump it here.
            memcpy(slot, r, sizeof(ResourceData));
            memset(r, 0, sizeof(ResourceData));
            rpm->resource_seeds[first_idx] = -(i + 1);
            return true;
        }
    }

    // Didn't find any free slots to move the resource to.  Shouldn't
    // happen--higher level should check that the table isn't full before
    // inserting.
    abort();
}

// This function's job is to find enough room in the hash table for this new
// (type, id) pair.  If we're lucky every hash will end up unique at the first
// level and we won't have to do anything too complicated.  Otherwise, there's
// a bit of magic involved, as we may have to move stuff around to fit it all.
ResourceData *rpmserver_insert_resource(rpm_resource_type resource, unsigned id)
{
    unsigned resource_slots = rpm->supported_resources;
    unsigned first_idx = resource_hash(0, resource, id) % resource_slots;
    int16_t  first_val = rpm->resource_seeds[first_idx];

    // Check the lucky case first (no collisions).  Cross your fingers!
    if(first_val == 0)
    {
        // No collision.  We can put this resource wherever we like.
        for(unsigned i = 0; i < resource_slots; ++i)
        {
            ResourceData *r = &(rpm->resources[i]);

            if(!r->current_state)
            {
                // Found an empty slot, dump it here.
                rpm->resource_seeds[first_idx] = -(i + 1);
                return r;
            }
        }

        // Shouldn't get here unless we run out of slots, which should be
        // checked at a higher level.
        abort();
    }

    // Ok, we got unlucky and need to shift stuff around a bit to get things in
    // the right place.  First, let's track which things collide at this index.
    unsigned  bitfield_indices   = CEILING_DIV(resource_slots, 32);
    unsigned  bitfield_size      = bitfield_indices * sizeof(uint32_t);
    uint32_t *collision_bitfield = alloca(bitfield_size);
    memset(collision_bitfield, 0, bitfield_size);

    unsigned  collisions = 0;
    for(unsigned i = 0; i < resource_slots; ++i)
    {
        ResourceData *r   = &(rpm->resources[i]);
        if(r->current_state)
        {
            ResourceClassData *rc  = &(rpm->classes[r->class_idx]);
            unsigned           idx = resource_hash(0, rc->type, r->id) % resource_slots;
            if(idx == first_idx)
            {
                collision_bitfield[i / 32] |= (1 << (i % 32));
                collisions++;
            }
        }
    }

    // Now, start looking for a seed value for which these resources can be
    // made to not collide with each other, or any non-easily-moved resource.
    uint32_t *allocated_bitfield = alloca(bitfield_size); // bitfield of where the colliding resources have been allocated to
    for(unsigned seed = 1; seed <= 65535; ++seed)
    {
        memset(allocated_bitfield, 0, bitfield_size);

        // Check the new resource first.
        unsigned second_idx = resource_hash(seed, resource, id) % resource_slots;
        if(!slot_is_usable(second_idx, collision_bitfield))
            continue;
        allocated_bitfield[second_idx/32] |= (1 << (second_idx % 32));

        bool clean = true;
        for(unsigned res_idx = 0; res_idx < resource_slots; ++res_idx)
        {
            if(collision_bitfield[res_idx/32] & (1 << (res_idx % 32)))
            {
                ResourceData      *r  = &(rpm->resources[res_idx]);
                ResourceClassData *rc = &(rpm->classes[r->class_idx]);
                second_idx = resource_hash(seed, rc->type, r->id) % resource_slots;
                if(!slot_is_usable(second_idx, collision_bitfield) ||
                   (allocated_bitfield[second_idx/32] & (1 << (second_idx % 32))))
                {
                    clean = false;
                    break;
                }
                allocated_bitfield[second_idx/32] |= (1 << (second_idx % 32));
            }
        }
        if(!clean)
            continue;

        // We found a working seed that resolves all collisions.  Go through
        // and move the resources into their new homes.
        rpm->resource_seeds[first_idx] = seed;

        // First, anything that was already in the table.
        for(unsigned res_idx = 0; res_idx < resource_slots; ++res_idx)
        {
            if(collision_bitfield[res_idx/32] & (1 << (res_idx % 32)))
            {
                ResourceData      *old_r = &(rpm->resources[res_idx]);
                ResourceClassData *rc    = &(rpm->classes[old_r->class_idx]);

                second_idx = resource_hash(seed, rc->type, old_r->id) % resource_slots;

                // It may be that we get to keep this resource right where it is.
                if(second_idx == res_idx)
                    continue;

                ResourceData      *new_r = &(rpm->resources[second_idx]);

                // If this destination table entry is empty, we can just use it.  Or...
                // If this destination table entry is direct-mapped, we can relocate it.
                if(!new_r->current_state ||       // empty entries have no state buffer
                   relocate_resource(second_idx)) // returns true if relocate succeeds
                {
                    memcpy(new_r, old_r, sizeof(ResourceData));
                    memset(old_r, 0, sizeof(ResourceData));
                    continue;
                }

                // Otherwise, the destination table entry must *also* be in our collision set.
                //      In this case, swap the entries, and redo this entry again.
                ResourceData temp;
                memcpy(&temp, new_r, sizeof(ResourceData));
                memcpy(new_r, old_r, sizeof(ResourceData));
                memcpy(old_r, &temp, sizeof(ResourceData));
                --res_idx;
            }
        }

        // Lastly, the new resource.  Since we moved all the in-table resources
        // to their destinations, the place this one lands should be guaranteed
        // to be either empty or relocatable.
        second_idx = resource_hash(seed, resource, id) % resource_slots;
        ResourceData *final_dest = &(rpm->resources[second_idx]);
        if(final_dest->current_state && !relocate_resource(second_idx))
            abort();
        return final_dest;
    }

    // We failed to find a working seed that resolves the collisions.  This
    // doesn't necessarily mean that there is no way to resolve them, but this
    // situation should be uncommon enough that it doesn't make sense to
    // include a more sophisticated (read: larger) algorithm for doing so.
    // Fail, and fail hard, and wait for a human being on the RPM team to
    // determine the right way to resolve the situation.
    abort();
}

