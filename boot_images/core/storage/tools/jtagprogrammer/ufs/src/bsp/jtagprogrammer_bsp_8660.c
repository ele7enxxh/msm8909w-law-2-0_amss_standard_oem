/**********************************************************************
* jsdcc_bsp_8660.c
*
* SDCC(Secure Digital Card Controller) driver BSP for TOOL.
*
* This file implements the SDCC driver BSP for the board in use for TOOL.
*
* Copyright (c) 2010
* Qualcomm Technologies Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
*
**********************************************************************/

/*=======================================================================
                        Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/jtagprogrammer/ufs/src/bsp/jtagprogrammer_bsp_8660.c#1 $
$DateTime: 2015/09/01 00:30:35 $

YYYY-MM-DD   who     what, where, why
----------------------------------------------------------------------
2012-03-05   ah      Commented out code failing for 8930
2011-02-11   mm      L4 Kernel Upgrade dependency
2010-06-08   rh      Not disable watchdog timer for 8660
2010-02-19   rh      Adding the BSP support for 8660
===========================================================================*/

/*==============================================================================
  Includes for this module.
  ----------------------------------------------------------------------------*/

#include "msm.h"
#include "jtagprogrammer_bsp.h"

/*==============================================================================
                               DEFINES
==============================================================================*/

#define BUS_SPEED_IN_MHZ      20
#define OPERATION_TIMEOUT_CNT 1000

/*==============================================================================
================================================================================
                         J S D C C    B S P
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
   /* Stub out function - use CMM script to configure PLL for 8660 */
}  /* sdcc_bsp_boot_config_pll */


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
   (void) arg_L;
   /* Stub out function - use CMM script to configure PLL for 8660 */
}  /* sdcc_bsp_boot_config_pll */


/******************************************************************************
* Name: jsdcc_bsp_tool_enable_pmic_ssbi_clk
*
* Description:
*    Setup the clock source for PMIC SSBI bus 
*    This function has undergone a slight change in 7x30, which also turn
*    on the power supply after enabling the SSBI clock source
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
   /* Stub out function - RUMI do not have PMIC */
}  /* jsdcc_bsp_tool_enable_pmic_ssbi_clk */

 
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
   // Watchdog timer is not accessable from Scorpion processor
}


#ifndef FEATURE_QUARTZ_3X 

/* Stub out functions - Required to compile the code */
/*
L4_Word_t int_lock_ext( void )
{
   return 0;
}
*/
void int_free_ext( void )
{
}
#endif
