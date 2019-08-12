#include <assert.h>
#include <string.h>

#include "rpm_hash.h"

// Hashing algorithms used here derived from http://code.google.com/p/smhasher/wiki/MurmurHash3.

#define __rol(val, shift) __ror((val), 32 - (shift))

static const uint32_t c1 = 0xcc9e2d51;
static const uint32_t c2 = 0x1b873593;

static uint32_t fmix(uint32_t h) __pure
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

// This is essentially a fully unrolled MumurHash3_x86_32 implementation.
uint32_t rpm_hash64s(uint32_t seed, uint32_t val1, uint32_t val2) __pure
{
    uint32_t h1 = seed;

    // Body - first 4 bytes
    uint32_t k1 = val1;
    k1 *= c1;
    k1 = __rol(k1, 15);
    k1 *= c2;
    h1 ^= k1;
    h1 = __rol(h1, 13);
    h1 = h1 * 5 + 0xe6546b64;

    // Body - second 4 bytes
    k1 = val2;
    k1 *= c1;
    k1 = __rol(k1, 15);
    k1 *= c2;
    h1 ^= k1;
    h1 = __rol(h1, 13);
    h1 = h1 * 5 + 0xe6546b64;

    // Finalization (fmix inlined)
    h1 ^= 8;
    return fmix(h1);
}

// An implementation of MumurHash3_x86_32.
uint32_t rpm_hash(uint32_t length, const void *data)
{
    if(8 == length)
    {
        const unsigned *words = (const unsigned *)data;
        return rpm_hash64(words[0], words[1]);
    }

    const uint8_t  *bytes   = (const uint8_t*)data;
    const int       nblocks = length / 4;
    const uint32_t *blocks  = (const uint32_t *)(bytes + 4*nblocks);

    uint32_t h1 = 0;

    for(int i = -nblocks; i; i++)
    {
        uint32_t k1 = blocks[i];
        k1 *= c1;
        k1 = __rol(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        h1 = __rol(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    const uint8_t *tail = (const uint8_t *)(bytes + 4*nblocks);
    uint32_t k1 = 0;

    switch(length & 3)
    {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
                k1 *= c1; k1 = __rol(k1, 15); k1 *= c2; h1 ^= k1;
    }

    h1 ^= length;
    return fmix(h1);
}




uint32_t rpm_resource_hash(uint32_t hash, ResourceData *r, uint32_t length, const void *oldValue, const void *newValue)
{
    uint32_t rotation = r - rpm->resources; // get the index of this resource

    if(oldValue)
    {
        uint32_t oldHash = rpm_hash(length, oldValue);
        hash ^= __ror(oldHash, rotation);
    }

    if(newValue)
    {
        uint32_t newHash = rpm_hash(length, newValue);
        hash ^= __ror(newHash, rotation);
    }

    return hash;
}



