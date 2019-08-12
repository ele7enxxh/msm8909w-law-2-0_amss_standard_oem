/*===========================================================================

  Copyright (c) 2015, 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/


#ifndef CPR_VOLTAGE_RANGES_H
#define CPR_VOLTAGE_RANGES_H

#include "comdef.h"
#include <stdint.h>
#include "cpr_fuses.h"
#include "cpr_device_hw_version.h"
#include "cpr_defs.h"


typedef struct
{
    cpr_voltage_mode_t          voltage_mode;
    uint32                      voltage_ceiling;
    uint32                      voltage_floor;
    uint32                      voltage_fuse_ref;
    uint32			voltage_max_floor_to_ceiling;					
    int32                       fuse_correction_factor;
    cpr_fuse_type               volt_fuse_type;
    cpr_fuse_type		offset_fuse_type;
} cpr_config_voltage_level_t;

typedef struct
{
    cpr_config_hw_version_range         hw_versions;
    const cpr_config_voltage_level_t*   voltage_level;
    uint32                              voltage_level_count;
} cpr_config_versioned_voltage_ranges_t;

typedef struct
{
    cpr_rail_id_t                                   rail_id;
    bool                                            disable_fused_floor;
    const cpr_config_versioned_voltage_ranges_t**   versioned_voltage_range;
    uint32                                          versioned_voltage_range_count;
} cpr_config_rail_voltage_ranges_t;

typedef struct
{
    const cpr_config_rail_voltage_ranges_t**   rail_voltage_ranges;
    uint32                                     rail_voltage_ranges_count;
} cpr_config_global_voltage_ranges_t;

typedef struct
{
    cpr_rail_id_t rail_id;
    uint32        pmic_step_size;
} cpr_config_misc_rail_info_t;

typedef struct
{
    const cpr_config_misc_rail_info_t*  rail_info;
    uint32                              rail_info_count;
    cpr_bsp_part_individual_fuse_config_t  cpr_rev_fuse;
} cpr_config_misc_info_t;

// All targets must define a cpr_bsp_voltage_ranges_config instance.
extern const cpr_config_global_voltage_ranges_t cpr_bsp_voltage_ranges_config;

// All targets must define a cpr_bsp_misc_info_config instance.
extern const cpr_config_misc_info_t             cpr_bsp_misc_info_config;

uint32 cpr_config_open_loop_voltage(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode);
uint32 cpr_config_global_ceiling_voltage(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode);
uint32 cpr_config_closed_loop_floor_voltage(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode);
uint32 cpr_config_round_up_to_pmic_step(cpr_rail_id_t rail_id, uint32 voltage);
uint32 cpr_config_pmic_step_size(cpr_rail_id_t rail_id);
const cpr_config_versioned_voltage_ranges_t* cpr_config_find_versioned_voltage_ranges(cpr_rail_id_t rail_id);
const cpr_config_voltage_level_t* cpr_config_find_voltage_level(const cpr_config_versioned_voltage_ranges_t* voltage_ranges, cpr_voltage_mode_t voltage_mode);
int32  cpr_offset_fuse_val(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode);


#endif  //CPR_VOLTAGE_RANGES_H
