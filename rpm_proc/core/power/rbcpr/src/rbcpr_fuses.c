/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "rbcpr_fuses.h"
#include "CoreVerify.h"
#include "string.h"

extern const rbcpr_bsp_fuse_config_t rbcpr_bsp_fuse_config;

static void rbcpr_fuses_individual_fuse_init(rbcpr_bsp_individual_fuse_config_t* rail_fuse_config)
{
    int bits_assigned = 0;
    uint32 cpr_fuse_bits = 0;
    
    for(int i=0; i<rail_fuse_config->number_of_partial_fuse_configs; i++)
    {
        const rbpcr_bsp_part_individual_fuse_config_t* part_fuse_config = &rail_fuse_config->partial_individual_fuse_configs[i];

        uint32 raw_fuse_value = *(uint32*)(part_fuse_config->fuse_address);

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
}


static void rbcpr_fuses_rail_init(rbcpr_bsp_fuse_rail_config_t* rail_fuse_config)
{
    for(int i=0; i<rail_fuse_config->number_of_fuses; i++)
    {
        rbcpr_fuses_individual_fuse_init(&rail_fuse_config->individual_fuse_configs[i]);
    }
}

void rbcpr_fuses_init(void)
{
    for(int i=0; i<rbcpr_bsp_fuse_config.number_of_fused_rails; i++)
    {
        rbcpr_fuses_rail_init(&rbcpr_bsp_fuse_config.rail_fuse_config[i]);
    }
}

int32 rbcpr_fuses_get_fuse_value(const char* rail_name, rbcpr_fuse_type fuse_type)
{
    if(RBCPR_FUSE_NO_FUSE==fuse_type)
        return 0;

    rbcpr_bsp_fuse_rail_config_t* rail_fuse_config = NULL;
    for(int i=0; i<rbcpr_bsp_fuse_config.number_of_fused_rails; i++)
    {
        if(!strcmp(rail_name, rbcpr_bsp_fuse_config.rail_fuse_config[i].rail_name))
        {
            rail_fuse_config = &rbcpr_bsp_fuse_config.rail_fuse_config[i];
            break;
        }
    }
    
    CORE_VERIFY_PTR(rail_fuse_config);  //Couldn't find the rail
    
    for(int i=0; i<rail_fuse_config->number_of_fuses; i++)
    {
        if(rail_fuse_config->individual_fuse_configs[i].fuse_type == fuse_type)
        {
            return rail_fuse_config->individual_fuse_configs[i].cpr_fuse_value;
        }
    }
    
    CORE_VERIFY(0); //Couldn't find the fuse
}

