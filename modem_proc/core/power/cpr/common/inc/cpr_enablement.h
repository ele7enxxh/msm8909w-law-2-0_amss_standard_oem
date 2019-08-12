/*===========================================================================

  Copyright (c) 2015, 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef CPR_ENABLEMENT_H
#define CPR_ENABLEMENT_H

#include "comdef.h"
#include "cpr_defs.h"
#include "cpr_device_hw_version.h"

typedef enum
{
    CPR_OPEN_LOOP_VOLTAGE,          //This doubles as the Closed-Loop Ceiling also.
    CPR_CLOSED_LOOP_FLOOR_VOLTAGE,
} cpr_enablement_custom_voltage_calculation_type_t;

typedef uint32 (*cpr_custom_voltage_calculation_fn)(cpr_enablement_custom_voltage_calculation_type_t voltage_calculation_type, cpr_voltage_mode_t voltage_mode);

typedef enum
{
    CPR_ENABLEMENT_OPEN_LOOP_ID,
    CPR_ENABLEMENT_CLOSED_LOOP_ID,
    CPR_ENABLEMENT_GLOBAL_CEILING_VOLTAGE_ID,
    CPR_ENABLEMENT_READ_CONFIG_FROM_SMEM_ID,
    CPR_ENABLEMENT_HV_PARTS_ID,                //8996 only
} cpr_enablement_type_t;

typedef enum
{
    CPR_ENABLEMENT_STAGE_INIT_INITAL_VOLTAGE_CONFIG,
    CPR_ENABLEMENT_STAGE_INIT_CLOSED_LOOP,
} cpr_enablement_stage_t;

struct cpr_enablement_versioned_rail_config_t;
typedef void (*cpr_enablement_fn_t)(cpr_rail_id_t rail_id, const struct cpr_enablement_versioned_rail_config_t* rail_enablement_config, cpr_enablement_stage_t enablement_stage);
typedef int32 (*cpr_enablement_custom_static_margin_fn_t)(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode,
                                                          int32 static_margin_mv);

typedef struct
{
    cpr_enablement_type_t   enablement_type;
    cpr_enablement_fn_t     enablement_fn;
} cpr_enablement_init_params_t;

//Standard enablement configs available for all rails, all targets.
//Not all rails can support Closed-Loop, but all rails should support any of the Open-Loop configuration options.
extern const cpr_enablement_init_params_t CPR_ENABLE_OPEN_LOOP;
extern const cpr_enablement_init_params_t CPR_ENABLE_CLOSED_LOOP;
extern const cpr_enablement_init_params_t CPR_ENABLE_GLOBAL_CEILING_VOLTAGE; //This will enable the voltage at the SS voltage.
extern const cpr_enablement_init_params_t CPR_ENABLE_READ_CONFIG_FROM_SMEM;



typedef struct
{
  cpr_voltage_mode_t  voltage_mode;
  int32               static_margin_mv;
  cpr_enablement_custom_static_margin_fn_t custom_static_margin_fn;
  int32               aging_scaling_factor;
} cpr_enablement_supported_level_t;

typedef struct cpr_enablement_versioned_rail_config_t
{
    cpr_config_hw_version_range             hw_versions;
    const cpr_enablement_init_params_t*     enablement_init_params;
    cpr_enablement_supported_level_t*       supported_level;
    uint32                                  supported_level_count;
    cpr_custom_voltage_calculation_fn       custom_voltage_fn;        //Typically will be NULL.
    const cpr_voltage_mode_t*               mode_to_settle;
    uint32                                  mode_to_settle_count;
    uint32                                  aging_static_margin_limit;
} cpr_enablement_versioned_rail_config_t;

typedef struct
{
    cpr_rail_id_t                                   rail_id;
    const cpr_enablement_versioned_rail_config_t**  versioned_rail_config;
    uint32                                          versioned_rail_config_count;
} cpr_enablement_rail_config_t;

typedef struct
{
    const cpr_enablement_rail_config_t**    rail_enablement_config;
    uint32                                  rail_enablement_config_count;
} cpr_enablement_config_t;

//All BSPs must supply an instance of a cpr_bsp_enablement_config
extern const cpr_enablement_config_t cpr_bsp_enablement_config;

uint32 cpr_enablement_number_of_rails(void);

//Index must be between 0 and cpr_enablement_number_of_rails-1
//rail_enablement_config can be NULL if the index doesn't have a valid CPR enablement setting for this device.
void cpr_enablement_rail_info(uint32 index, cpr_rail_id_t* rail_id, const cpr_enablement_versioned_rail_config_t** rail_enablement_config);
const cpr_enablement_versioned_rail_config_t* cpr_enablement_find_versioned_rail_config(cpr_rail_id_t cpr_rail_id);
void cpr_enablement_from_xml(int32 rail_id, cpr_enablement_versioned_rail_config_t** rail_enablement_config);

#endif //CPR_ENABLEMENT_H
