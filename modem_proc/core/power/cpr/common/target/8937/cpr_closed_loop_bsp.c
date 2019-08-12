/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_closed_loop.h"
#include "cpr_defs.h"
#ifdef WINSIM
#define MSS_CPR_WRAPPER_BASE 0
#else
#include "msmhwiobase.h"
#define MSS_CPR_WRAPPER_BASE   (MSS_TOP_BASE + 0x001B1000)
#endif


cpr_closed_loop_rail_config_t mss_8937_closed_loop_config =
{
    .rail_id = CPR_RAIL_MSS,
    .ref_clk_resource = "clk_rbcpr_ref",
    .ahb_clk_resource = "clk_bus_rbcpr",
    .hal_handle = { MSS_CPR_WRAPPER_BASE, 0 },
    .aging_sensor_ids =     (const uint8[]){12},
    .aging_sensor_count =       0,  // 0 = Don't use aging
    .sensors_to_mask =          0,
    .sensors_to_mask_count =    0,
    .step_quot = 26,
    .interrupt_id = 61,
    .up_threshold = 0,
    .dn_threshold = 2,
    .consecutive_up = 0,
    .consecutive_dn = 2,
    .thermal_margin = 0, //0 to disable thermal changes. 
};


cpr_closed_loop_config_t cpr_bsp_closed_loop_config =
{
    .closed_loop_config = (cpr_closed_loop_rail_config_t*[])
    {
        &mss_8937_closed_loop_config,
    },
    .closed_loop_config_count = 1,
};
