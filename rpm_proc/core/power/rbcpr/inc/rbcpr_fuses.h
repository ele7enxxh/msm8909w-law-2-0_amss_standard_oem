/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RBCPR_FUSES_H
#define RBCPR_FUSES_H

#include "comdef.h"
#include <stdint.h>

#define RBCPR_FUSE_MAPPING(base_register, field) \
    {   \
    .fuse_address   = HWIO_##base_register##_ADDR, \
    .offset         = HWIO_##base_register##_##field##_SHFT, \
    .mask           = HWIO_##base_register##_##field##_BMSK, \
    }


typedef enum    //Used only for the eFuses
{
    RBCPR_FUSE_NO_FUSE,
    RBCPR_FUSE_SVS2,
    RBCPR_FUSE_SVS,
	RBCPR_FUSE_SVS_PLUS,
    RBCPR_FUSE_NOMINAL,
    RBCPR_FUSE_TURBO,
} rbcpr_fuse_type;

typedef struct
{
    uintptr_t                                           fuse_address;
    uint32                                              offset;
    uint32                                              mask;
} rbpcr_bsp_part_individual_fuse_config_t;

// If a fuse is segmented across multiple registers, the partial_individual_fuse_configs
// must be listed in order from most significant to least significant bits.
//

typedef struct
{
    rbcpr_fuse_type                                     fuse_type;
    uint8                                               fuse_total_bits;
    uint8                                               number_of_partial_fuse_configs;
    int16                                               cpr_fuse_value;
    const rbpcr_bsp_part_individual_fuse_config_t*      partial_individual_fuse_configs;
} rbcpr_bsp_individual_fuse_config_t;

typedef struct
{
    char*                                               rail_name;
    uint32                                              number_of_fuses;
    rbcpr_bsp_individual_fuse_config_t*                 individual_fuse_configs;
} rbcpr_bsp_fuse_rail_config_t;

typedef struct
{
    rbcpr_bsp_fuse_rail_config_t*                       rail_fuse_config;
    uint32                                              number_of_fused_rails;
} rbcpr_bsp_fuse_config_t;


void rbcpr_fuses_init(void);

int32 rbcpr_fuses_get_fuse_value(const char* rail_name, rbcpr_fuse_type);

#endif //RBCPR_FUSES_H
