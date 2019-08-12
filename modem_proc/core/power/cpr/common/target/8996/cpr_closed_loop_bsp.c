/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_closed_loop.h"
#include "cpr_defs.h"
#ifdef WINSIM
#define MODEM_TOP_BASE 0
#else
#include "msmhwiobase.h"
#endif


#define CPR3_BASE              (0x50000  | 0x60000000)
#define MX_CPR3_BASE           (0x54000  | 0x60000000)
#define MMSS_CPR_WRAPPER_BASE  (0x838000 | 0x60000000)
#define MSS_CPR_WRAPPER_BASE   (MODEM_TOP_BASE + 0x001a4000)

static cpr_closed_loop_controller_config cpr3_controller_config = {
    .thread_count = 1,
    .sensor_count = 73,
};

static cpr_closed_loop_controller_config mx_cpr3_controller_config = {
    .thread_count = 4,
    .sensor_count = 32,
    .sensors_to_bypass =        (const uint8[]){11,12,14,15},
    .sensors_to_bypass_count =  4,
};

static cpr_closed_loop_controller_config mss_cpr_controller_config = {
    .thread_count = 1,
    .sensor_count = 35,
};

static const cpr_closed_loop_rail_config_t cx_config = 
{
    .rail_id =                  CPR_RAIL_CX,
    .controller_config =        &cpr3_controller_config,
    .ref_clk_resource =         "gcc_rbcpr_cx_clk",
    .ahb_clk_resource =         "gcc_rbcpr_cx_ahb_clk",
    .hal_handle =               {CPR3_BASE, 0},
    .aging_sensor_ids =     (const uint8[]){12},
    .aging_sensor_count =       0,  // 0 = Don't use aging
    .step_quot =                16,
    .interrupt_id =             54,
    .up_threshold =             1,
    .dn_threshold =             2,
    .consecutive_up =           0,
    .consecutive_dn =           2,
};

static const cpr_closed_loop_rail_config_t mx_config = 
{
    .rail_id =                  CPR_RAIL_MX,
    .controller_config =        &mx_cpr3_controller_config,
    .ref_clk_resource =         "gcc_rbcpr_mx_clk",
    .ahb_clk_resource =         "gcc_rbcpr_mx_ahb_clk",
    .hal_handle =               {MX_CPR3_BASE, 0},
    .aging_sensor_ids =     (const uint8[]){12},
    .aging_sensor_count =       0,  // 0 = Don't use aging	
    .sensors_to_mask =          (const uint8[]){0,1,2,3,4,13,16,17,18,19,20,21,28,29,30,31},
    .sensors_to_mask_count =    16,
    .step_quot =                16,
    .interrupt_id =             17,
    .up_threshold =             1,
    .dn_threshold =             2,
    .consecutive_up =           0,
    .consecutive_dn =           2,
};

static const cpr_closed_loop_rail_config_t ddr_config = 
{
    .rail_id =                  CPR_RAIL_VDDA_EBI,
    .controller_config =        &mx_cpr3_controller_config,
    .ref_clk_resource =         "gcc_rbcpr_mx_clk",
    .ahb_clk_resource =         "gcc_rbcpr_mx_ahb_clk",
    .hal_handle =               {MX_CPR3_BASE, 1},
    .aging_sensor_ids =     (const uint8[]){12},
    .aging_sensor_count =       0,  // 0 = Don't use aging	
    .sensors_to_mask =          (const uint8[]){5,6,7,8,9,10,22,23,24,25,26,27},
    .sensors_to_mask_count =    12,
    .step_quot =                16,
    .interrupt_id =             23,
    .up_threshold =             1,
    .dn_threshold =             2,
    .consecutive_up =           0,
    .consecutive_dn =           2,
};

static const cpr_closed_loop_rail_config_t gfx_config = 
{
    .rail_id =                  CPR_RAIL_GFX,
    .controller_config =        &mss_cpr_controller_config,
    .ref_clk_resource =         "mmss_rbcpr_clk",
    .ahb_clk_resource =         "mmss_rbcpr_ahb_clk",
    .hal_handle =               {MMSS_CPR_WRAPPER_BASE, 0},
    .aging_sensor_ids =     (const uint8[]){12},
    .aging_sensor_count =       0,  // 0 = Don't use aging
    .step_quot =                16,
    .interrupt_id =             60,
    .up_threshold =             1,
    .dn_threshold =             2,
    .consecutive_up =           0,
    .consecutive_dn =           2,
};


static const cpr_closed_loop_rail_config_t mss_config =
{
    .rail_id = CPR_RAIL_MSS,
    .controller_config = &mss_cpr_controller_config,
    .ref_clk_resource = "clk_rbcpr_ref",
    .ahb_clk_resource = "clk_bus_rbcpr",
    .hal_handle = { MSS_CPR_WRAPPER_BASE, 0 },
    .aging_sensor_ids =     (const uint8[]){12},
    .aging_sensor_count =       0,  // 0 = Don't use aging
    .step_quot = 40,
    .interrupt_id = 144,
    .up_threshold = 1,
    .dn_threshold = 2,
    .consecutive_up = 0,
    .consecutive_dn = 2,
};


const cpr_closed_loop_config_t cpr_bsp_closed_loop_config =
{
    .closed_loop_config = (const cpr_closed_loop_rail_config_t*[])
    {
        &mss_config,
    },
    .closed_loop_config_count = 1,
};
