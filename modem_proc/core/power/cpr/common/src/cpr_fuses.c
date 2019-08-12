/*===========================================================================

  Copyright (c) 2014, 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/
#include <stdbool.h>
#include "cpr_fuses.h"
#include "cpr_logging.h"
#include "CoreVerify.h"

//#define ENABLE_FUSE_LOGGING                  1


/**
* <!-- cpr_config_find_device_fuses -->
*
* @brief Run through the list of fuse configs and return the first one that matches the detected hardware version. 
*
*/
const cpr_bsp_fuse_versioned_config_t* cpr_config_find_device_fuses(const cpr_config_bsp_fuse_config_t* target_fuse_configs)
{
    for (int i = 0; i<target_fuse_configs->versioned_fuses_count; i++)
    {

        const cpr_bsp_fuse_versioned_config_t*  versioned_fuses = target_fuse_configs->versioned_fuses[i];

        //To do - factor out this hw version matching to a seperate function
        const cpr_config_hw_version_range* hw_versions = &versioned_fuses->hw_versions;

        if (cpr_device_hw_version_matches_this_device(hw_versions))
        {
            //CPR_LOGGING(1, "Using Fuse Config %d",i);
            return versioned_fuses;
        }
    }
    return NULL;
}



/**
* <!-- cpr_config_which_device_fuses_used -->
*
* @brief Run through the list of fuse configs and return the index of the first one that matches the detected hardware version.
*
*/
int32 cpr_config_which_device_fuses_used(const cpr_config_bsp_fuse_config_t* target_fuse_configs)
{
    for (int i = 0; i<target_fuse_configs->versioned_fuses_count; i++)
    {

        const cpr_bsp_fuse_versioned_config_t*  versioned_fuses = target_fuse_configs->versioned_fuses[i];

        //To do - factor out this hw version matching to a seperate function
        const cpr_config_hw_version_range* hw_versions = &versioned_fuses->hw_versions;

        if (cpr_device_hw_version_matches_this_device(hw_versions))
        {
            return i;
        }
    }
    return -1;
}



/**
* <!-- cpr_fuses_individual_fuse_init -->
*
* @brief 
*
*/
static void cpr_fuses_individual_fuse_init(cpr_bsp_individual_fuse_config_t* rail_fuse_config)
{
    int bits_assigned = 0;
    uint32 cpr_fuse_bits = 0;

#ifdef ENABLE_FUSE_LOGGING
    CPR_LOGGING(0, "cpr_fuses_individual_fuse_init");
#endif

    for(int i=0; i<rail_fuse_config->number_of_partial_fuse_configs; i++)
    {
        const cpr_bsp_part_individual_fuse_config_t* part_fuse_config = &rail_fuse_config->partial_individual_fuse_configs[i];
#ifdef ENABLE_FUSE_LOGGING
        CPR_LOGGING(1, "fuse type %d",rail_fuse_config->fuse_type);
#endif
#ifdef WINSIM
        /* Use 0 if it's a PC build for now */
        uint32 raw_fuse_value = 0;
#else
#ifdef ENABLE_FUSE_LOGGING
        CPR_LOGGING(2, "Reading fuse at  %x value is %x", part_fuse_config->fuse_address, *(uint32*)part_fuse_config->fuse_address);
#endif
        uint32 raw_fuse_value = *(uint32*)(part_fuse_config->fuse_address);
#endif
        //This will give us the bits we care about from the fuse in the lowest bits of bits_we_care_about.
        const uint32 bits_we_care_about = (raw_fuse_value&part_fuse_config->mask)>>part_fuse_config->offset;

        //Now need to know how many bits of the fuse this accounts for. Could use __clz if only I knew what it did on ARM64/Q6?
        uint32 number_of_bits_we_care_about = 0;
        uint32 mask_for_counting = part_fuse_config->mask >> part_fuse_config->offset;
        while (mask_for_counting)
        {
            number_of_bits_we_care_about++;
            mask_for_counting = mask_for_counting>>1;
        }

        //Move up the bits we've got previously to make way for the new bits we've parsed.
        cpr_fuse_bits = cpr_fuse_bits<<number_of_bits_we_care_about;

        //Now pull in the new bits we just parsed.
        cpr_fuse_bits |= bits_we_care_about;
        
        //Record the total number of bits in the fuse.
        bits_assigned+=number_of_bits_we_care_about;
    }

    rail_fuse_config->fuse_total_bits = bits_assigned;

    //We have the CPR fuse as a raw bitfied in cpr_fuse_bits. Now need to convert to a signed int based on the
    //highest bit of the fuse. If the highest bit is set then the fuse value is -ve, otherwise +ve
    
    //Create a mask for the value part of the fuse. This mask is to pull out the bottom (rail_fuse_config->fuse_total_bits-1) bits.
    const uint32 value_mask = (1<<(rail_fuse_config->fuse_total_bits-1))-1;

    rail_fuse_config->cpr_fuse_value = (int16)(cpr_fuse_bits&value_mask);
    
    //Now check the top bit to see if we need to flip the sign.
    if(cpr_fuse_bits&(1<<(rail_fuse_config->fuse_total_bits-1)))
    {
        rail_fuse_config->cpr_fuse_value = -rail_fuse_config->cpr_fuse_value;
    }
    //CPR_LOGGING(4, "Fuse total bits: %d  value mask: %x  fuse value: %d  sign mask: %x", rail_fuse_config->fuse_total_bits, value_mask, rail_fuse_config->cpr_fuse_value, (1 << (rail_fuse_config->fuse_total_bits - 1)));
}


/**
* <!-- cpr_fuses_rail_init -->
*
* @brief
*
*/
static void cpr_fuses_rail_init(const cpr_bsp_fuse_rail_config_t* rail_fuse_config)
{
    for(int i=0; i<rail_fuse_config->number_of_fuses; i++)
    {
        cpr_fuses_individual_fuse_init(&rail_fuse_config->individual_fuse_configs[i]);
    }
}


/**
* <!-- cpr_fuses_init -->
*
* @brief
*
*/
void cpr_fuses_init(void)
{
    static bool cpr_fuses_init_done = false;
    //find the appropriate fuses for this version of hardware
    const cpr_bsp_fuse_versioned_config_t* fuse_config = cpr_config_find_device_fuses(&cpr_bsp_fuse_config);
    

    if(cpr_fuses_init_done)
    {
        return;
    }
    cpr_fuses_init_done = true;


#ifdef ENABLE_FUSE_LOGGING	
    CPR_LOGGING(0, "cpr_fuses_init");
#endif

    if (fuse_config == NULL)
    {
        return;
    }

    for (int i = 0; i<fuse_config->number_of_fused_rails; i++)
    {
        cpr_fuses_rail_init(&(fuse_config->rail_fuse_config[i]));
    }
}



/**
* <!-- cpr_fuses_get_fuse_value -->
*
* @brief
*
*/
int32 cpr_fuses_get_fuse_value(cpr_rail_id_t rail_id, cpr_fuse_type fuse_type)
{
    const cpr_bsp_fuse_rail_config_t* rail_fuse_config = NULL;
    
    //find the appropriate fuses for this version of hardware
    const cpr_bsp_fuse_versioned_config_t* fuse_config = cpr_config_find_device_fuses(&cpr_bsp_fuse_config);

    cpr_fuses_init();

    if(CPR_FUSE_NO_FUSE==fuse_type)
        return 0;

    if (fuse_config==NULL)
        return 0;

    //find the rail
    for (int i = 0; i<fuse_config->number_of_fused_rails; i++)
    {
        if (rail_id == fuse_config->rail_fuse_config[i].rail_id)
        {
            rail_fuse_config = &fuse_config->rail_fuse_config[i];
            break;
        }
    }
    
    CORE_VERIFY_PTR(rail_fuse_config);  //Couldn't find the rail
    
    //find the fuse type
    for(int i=0; i<rail_fuse_config->number_of_fuses; i++)
    {
        if(rail_fuse_config->individual_fuse_configs[i].fuse_type == fuse_type)
        {
            return rail_fuse_config->individual_fuse_configs[i].cpr_fuse_value;
        }
    }
    
    return 0;
}

