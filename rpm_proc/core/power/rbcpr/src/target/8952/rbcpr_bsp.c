/*===========================================================================
  	
  	FILE:         rbcpr_bsp.c


	DESCRIPTION:  Per target rbcpr bsp configurations
  	
=============================================================================
	
Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/rbcpr/src/target/8952/rbcpr_bsp.c#1 $
$Date: 2015/02/25 $
	 
when       who     what, where, why
--------   ---     --------------------------------------------------------
  
===========================================================================
Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.
===========================================================================*/

 /* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "rbcpr.h"
#include "rbcpr_comdef.h"
#include "HAL_rbcpr_qfprom.h"
#include "railway.h"
#include "Chipinfo.h"

 /* -----------------------------------------------------------------------
**                     		DATA
** ----------------------------------------------------------------------- */

//For 8936, the selection of which config to use (based on HW version) is done in the SBL.
const rbcpr_supported_hw_array_type rbcpr_supported_hw_array;

const rbcpr_bsp_rail_const_info rbcpr_rail_const_info[] =
{
    {   // VDDCX
        .ref_clk_resource       = "gcc_rbcpr_clk",
        .ahb_clk_resource       = "gcc_rbcpr_ahb_clk",
        .rail_name              = "vddcx",
        .interrupt_id           = 62,   // top_cpr_irq[0]
        .idle_clocks            = 15,
        .step_quot              = 26,
        .pmic_step_size_uv      = 12500,
        .up_threshold           = 0,
        .dn_threshold           = 2,
        .consecutive_up         = 0,
        .consecutive_dn         = 2,
        .gcnt                   = 19,
        .count_mode = HAL_RBCPR_STAGGERED,
        .efuse_target_voltage_multipler = 10000,
    },
};

rbcpr_bsp_type rbcpr_bsp_data =
{
    .rails = (rbcpr_bsp_rail_info[])
    {
        {   // VDDCX
            .rail_const_info = &rbcpr_rail_const_info[0],
        },
    },
    .num_rails = 1,
};
