/**********************************************************************
* jsdcc_bsp_7600.c
*
* SDCC(Secure Digital Card Controller) driver BSP for TOOL.
*
* This file implements the SDCC driver BSP for the board in use for TOOL.
* This BSP is used for 7200, 7500, 7600, 7625 and 7227
*
* Copyright (c) 2009 - 2010
* Qualcomm Technologies Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
*
**********************************************************************/

/*=======================================================================
                        Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/jsdcc/bsp/jsdcc_bsp_7600.c#1 $
$DateTime: 2015/09/01 00:30:35 $

YYYY-MM-DD   who     what, where, why
----------------------------------------------------------------------
2010-01-21   vj      Removed enabling of MPLL and have TCXO as clock source
2009-12-14   vj      Changed the PLL from PLL2 to MPLL
2009-12-01   vj      Added dummy APIs for compilation
2009-08-18   rh      Adding the BSP support for 7x27
===========================================================================*/

/*==============================================================================
  Includes for this module.
  ----------------------------------------------------------------------------*/
#include "jsdcc_param.h"
#include "jsdcc_bsp.h"
#include "msm.h"

/*==============================================================================
================================================================================
                           S D C C    B S P
==============================================================================*/

/******************************************************************************
* Name: jsdcc_bsp_tool_config_pll
*
* Description:
*    Setup the PLL for default frequency required so sdcc_boot_config_clock
*    can get the proper clock source.  
*
* Arguments:
*    None
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_tool_config_pll(void)
{
}  /* sdcc_bsp_boot_config_pll */

/******************************************************************************
* Name: jsdcc_bsp_tool_enable_pmic_ssbi_clk
*
* Description:
*    Setup the clock source for PMIC SSBI bus
*    
* Arguments:
*    None
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_tool_enable_pmic_ssbi_clk(void)
{
   if(!HWIO_INM(SBI_NS_REG, HWIO_FMSK(SBI_NS_REG, PMIC_SSBI_ROOT_ENA)))
   {
      HWIO_OUTM( SBI_NS_REG,
              HWIO_FMSK(SBI_NS_REG, PMIC_SSBI_CLK_BRANCH_ENA) |
              HWIO_FMSK(SBI_NS_REG, PMIC_SSBI_ROOT_ENA),
              HWIO_FMSK(SBI_NS_REG, PMIC_SSBI_CLK_BRANCH_ENA) |
              HWIO_FMSK(SBI_NS_REG, PMIC_SSBI_ROOT_ENA)
              );     
   }
}  /* sdcc_bsp_tool_enable_pmic_ssbi_clk */

/******************************************************************************
* Name: jsdcc_bsp_tool_tune_pll
*
* Description:
*    Change the output frequency of the PLL.  This function is used to
*    fine tune the output frequency of the PLL which is used mainly to
*    detect data corruption on a specific clock frequency.  
*
* Arguments:
*    arg_L - Multiplier settting for the PLL
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_tool_tune_pll(uint32 arg_L)
{
   (void)arg_L;
}

/******************************************************************************
* Name: jsdcc_bsp_kill_watchdog
*
* Description:
*    This function put the watchdog timer in freeze state
*    
* Arguments:
*    None
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_kill_watchdog(void)
{
}
