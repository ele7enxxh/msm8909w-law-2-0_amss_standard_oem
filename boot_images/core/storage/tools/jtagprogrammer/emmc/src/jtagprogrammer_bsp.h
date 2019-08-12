#ifndef __JSDCC_BSP_H
#define __JSDCC_BSP_H
/******************************************************************************
 * jsdcc_bsp.h
 *
 * This file provides SDCC abstraction for dependent drivers for Non-OS(BOOT).
 *
 * Copyright (c) 2009
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/jtagprogrammer/emmc/src/jtagprogrammer_bsp.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
2009-05-05   rh      Initial release
=============================================================================*/


/******************************************************************************
* Name: jsdcc_bsp_tool_config_pll
*
* Description:
*    Setup the PLL for default frequency required so sdcc_bsp_boot_config_clock
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
void jsdcc_bsp_tool_config_pll(void);

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
void jsdcc_bsp_tool_tune_pll(uint32 arg_L);

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
void jsdcc_bsp_tool_enable_pmic_ssbi_clk(void);

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
void jsdcc_bsp_kill_watchdog(void);


#endif /* ifndef __JSDCC_BSP_H */

