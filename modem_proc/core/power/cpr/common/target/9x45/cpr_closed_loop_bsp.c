/*===========================================================================

  Copyright (c) 2014, 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  9x45 Closed Loop BSP
  ===========================================================================*/

#include "cpr_closed_loop.h"
#include "CoreVerify.h"
#include <string.h>
#include "cpr_image.h"
#include "cpr_enablement.h"
#ifdef WINSIM
#define MSS_RBCPR_WRAPPER_REG_BASE 0
#else
#include "msmhwiobase.h"
#endif
#include "cpr_qfprom.h"


static const cpr_closed_loop_rail_config_t mss_9x45_closed_loop_config = 
{
    .rail_id =                  CPR_RAIL_MSS,
    .ref_clk_resource =         "clk_rbcpr_ref",
    .ahb_clk_resource =         "clk_bus_rbcpr",
    .hal_handle =               {MSS_RBCPR_WRAPPER_REG_BASE},
    .aging_sensor_ids =         (const uint8[]){9},
    .aging_sensor_count =       1,   // 0 = Don't use aging
    .sensors_to_mask =          0,
    .sensors_to_mask_count =    0,
    .step_quot =                40,
    .interrupt_id =             144,
    .up_threshold =             1,
    .dn_threshold =             2,
    .consecutive_up =           0,
    .consecutive_dn =           2,
    .thermal_margin =         -13, //rounded 12.5 to 13
};


const cpr_closed_loop_config_t cpr_bsp_closed_loop_config =
{
    .closed_loop_config = (const cpr_closed_loop_rail_config_t*[])
    {
        &mss_9x45_closed_loop_config,
    },
    .closed_loop_config_count = 1,
};
