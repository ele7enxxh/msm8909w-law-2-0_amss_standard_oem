/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef CPR_DEFS_H
#define CPR_DEFS_H

typedef enum
{
    CPR_VOLTAGE_MODE_OFF,
    CPR_VOLTAGE_MODE_RETENTION,
    CPR_VOLTAGE_MODE_SVS_MIN,
    CPR_VOLTAGE_MODE_SVS2,
    CPR_VOLTAGE_MODE_SVS,
    CPR_VOLTAGE_MODE_SVS_L1,
    CPR_VOLTAGE_MODE_NOMINAL,
    CPR_VOLTAGE_MODE_NOMINAL_L1,
    CPR_VOLTAGE_MODE_TURBO,
    CPR_VOLTAGE_MODE_SUPER_TURBO,
    CPR_VOLTAGE_MODE_SUPER_TURBO_NO_CPR,
    CPR_VOLTAGE_MODE_COUNT  
} cpr_voltage_mode_t;

typedef enum
{
    // Core rails
    CPR_RAIL_MX     = 0x100,
    CPR_RAIL_CX,
    CPR_RAIL_MSS,
    CPR_RAIL_GFX,
    CPR_RAIL_VDDA_EBI,
    
    // APC rails
    CPR_RAIL_APC0   = 0x200,
    CPR_RAIL_APC1,
    
    // SSC rails
    CPR_RAIL_SSC_MX = 0x300,
    CPR_RAIL_SSC_CX,
    
} cpr_rail_id_t;

// Typenames used around the driver.
typedef struct cpr_image_rail_voltage_control_handle_s* cpr_image_rail_voltage_control_handle_t;

#endif //CPR_DEFS_H
