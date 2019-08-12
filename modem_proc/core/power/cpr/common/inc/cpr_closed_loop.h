/*===========================================================================

  Copyright (c) 2014, 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef CPR_CLOSED_LOOP_H
#define CPR_CLOSED_LOOP_H

#include <stdbool.h>
#include "comdef.h"
#include "HAL_cpr.h"
#include "cpr_defs.h"
#include "cpr_target_quotients.h"
#include "cpr_enablement.h"

typedef struct    //new for v3 CPR
{
    boolean         initialized;
    uint32          sensor_count;
    uint32          thread_count;
    const uint8*    sensors_to_bypass;
    uint8           sensors_to_bypass_count;
} cpr_closed_loop_controller_config;

typedef struct
{
    cpr_rail_id_t   rail_id;
    const char*     ref_clk_resource;
    const char*     ahb_clk_resource;
    HAL_cpr_rail_s  hal_handle;
    const uint8*    sensors_to_mask;
    uint8           sensors_to_mask_count;
    const uint8*    aging_sensor_ids;
    uint8           aging_sensor_count;
    uint8           step_quot;
    uint8           interrupt_id;
    uint8           up_threshold;
    uint8           dn_threshold;
    uint8           consecutive_up;
    uint8           consecutive_dn;
    int32           thermal_margin;
    
    cpr_closed_loop_controller_config* const controller_config;
    
} cpr_closed_loop_rail_config_t;

typedef struct
{
    cpr_closed_loop_rail_config_t**     closed_loop_config;
    uint32                                  closed_loop_config_count;
} cpr_closed_loop_config_t;

//Settings that are tied to the HW version.
typedef struct
{
  cpr_voltage_mode_t        voltage_mode;
  uint32                    voltage_ceil;
  uint32                    voltage_floor;
  cpr_target_quotient_set_t rosc_targets;
} cpr_corner_params_t;

typedef struct
{
    uint16 post_switch_counter;
    uint16 cpr_voltage_change_cnt;
    uint16 cpr_voltage_hit_floor;
    uint16 cpr_voltage_hit_ceiling;
    uint16 step_up_count;
    uint16 step_down_count;
    uint16 ignored_readings;
} cpr_closed_loop_rail_log;

typedef struct cpr_image_closed_loop_rail_s* cpr_image_closed_loop_rail_t;
typedef struct cpr_closed_loop_rail_s* cpr_closed_loop_rail_t;

typedef struct cpr_closed_loop_rail_s
{
    cpr_rail_id_t                           rail_id;
    const cpr_closed_loop_rail_config_t*    rail_config;
    cpr_image_rail_voltage_control_handle_t voltage_control_handle;
    uint32                                  pmic_step_size;
    int32                                   aging_percentage;
    uint16                                  age_min;
    uint16                                  age_max;
    cpr_image_closed_loop_rail_t            image_rail_state;
    bool                                    block_enabled;
    bool                                    cpr_handler_in_progress;
    cpr_corner_params_t*                    target_params;
    uint8                                   target_params_count;
    const cpr_corner_params_t*              current_target_params;
    cpr_closed_loop_rail_log                rail_log;
    cpr_closed_loop_rail_t                  next_link;
} cpr_closed_loop_rail_s;

extern cpr_closed_loop_rail_t cpr_closed_loop_rail_root;

void cpr_enable_closed_loop(cpr_rail_id_t rail_id, const cpr_enablement_versioned_rail_config_t* rail_enablement_config);

//Must be defined by the bsp config
extern  cpr_closed_loop_config_t cpr_bsp_closed_loop_config;

//To be implemented in the HW specific (v2 vs v3) part of the Closed-Loop driver.
void cpr_init_rail_hw(cpr_closed_loop_rail_t rail);
void cpr_settle_voltage(cpr_closed_loop_rail_t rail, const cpr_enablement_versioned_rail_config_t* rail_enablement_config);
void cpr_enable_closed_loop_feedback(cpr_closed_loop_rail_t rail);
void cpr_pause_cpr(cpr_closed_loop_rail_t rail);
void cpr_prepare_cpr(cpr_closed_loop_rail_t rail, const cpr_corner_params_t* corner_params, uint32 rail_uv);
void cpr_restart_cpr(cpr_closed_loop_rail_t rail);
void cpr_aging_poll(cpr_closed_loop_rail_t rail);
void cpr_configure_roscs(cpr_closed_loop_rail_t rail, const cpr_corner_params_t* corner_params);
void rbcpr_prepare_for_sleep(void);
void rbcpr_exit_sleep(void);

#endif //CPR_CLOSED_LOOP_H
