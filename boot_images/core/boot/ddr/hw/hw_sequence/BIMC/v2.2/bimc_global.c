/**
 * @file bimc_global.c
 * @brief BIMC GLOBAL0 APIs
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     -------------------------------------------------------------
01/08/14   cchou      Code optimization. Add API description.
09/01/13   cchou      Initial version.
================================================================================
                   Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/***************************************************************************************************
  Auto-generated sequence C codes from Autoseq 2.2 12/3/2013
  User Name:cchou
  Input File:ddrss.blk
 ***************************************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2011, Qualcomm Technologies Incorporated. All rights reserved.

 All data and information contained in or disclosed by this document are confidential and
 proprietary information of Qualcomm Technologies Incorporated, and all rights therein are expressly
 reserved. By accepting this material, the recipient agrees that this material and the
 information contained therein are held in confidence and in trust and will not be used,
 copied, reproduced in whole or in part, nor its contents revealed in any manner to others
 without the express written permission of Qualcomm Technologies Incorporated.

 This technology was exported from the United States in accordance with the Export
 Administration Regulations. Diversion contrary to U. S. law prohibited.
 ***************************************************************************************************/
/*==============================================================================
                                                             INCLUDES
==============================================================================*/
#include "bimc_global.h"

/*==============================================================================
                                                             FUNCTIONS
==============================================================================*/   
/** =============================================================================
 *  Function : Update_Clock_Period 
 ** =============================================================================
 */
/**
 *    @details
 *    update DDR clock period and corresponding resolution; clock period is used for converting timing settings from ns to cycles
 **/
void BIMC_MISC_GLOBAL_CSR_Update_Clock_Period( uint32 _inst_, uint32 interface, uint32 resolution, uint32 clk_period )
{
  uint32 clk_period_updated = 0;

  ///  width checks below check for max values possible in each  resolution.
  if (resolution == CLK_PERIOD_RESOLUTION_100PS) {
    if (clk_period > 0xFF) {
      clk_period_updated = 0xFF;
    }
    else
    {
      clk_period_updated = clk_period;
    }
  }
  else
  {
    if (clk_period > 0x1FFFF) {
      clk_period_updated = 0x1FFFF;
    }
    else
    {
      clk_period_updated = clk_period;
    }
  }
  HWIO_OUTXFI(_inst_, BIMC_MISC_GLOBAL_CSR_DDR_CHN_CLK_PERIOD, interface, CLK_PERIOD_RESOLUTION, resolution);
  HWIO_OUTXFI(_inst_, BIMC_MISC_GLOBAL_CSR_DDR_CHN_CLK_PERIOD, interface, PERIOD, clk_period_updated);
}


/** =============================================================================
 *  Function : Get_Clock_Period
 ** =============================================================================
 */
/**
 *    @details
 *    get the current clock priod
 **/
uint32 BIMC_MISC_GLOBAL_CSR_Get_Clock_Period( uint32 _inst_, uint32 interface )
{

  return ((HWIO_INXFI(_inst_, BIMC_MISC_GLOBAL_CSR_DDR_CHN_CLK_PERIOD, interface, PERIOD)));
}


/** =============================================================================
 *  Function : Freq_Switch 
 ** =============================================================================
 */
/**
 *    @details
 *    set the HANDSHAKE_CTRL bit for clock switch
 **/
void BIMC_MISC_GLOBAL_CSR_Freq_Switch( uint32 _inst_, uint32 interface )
{
  HWIO_OUTXI(_inst_, BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_STAGE1_HANDSHAKE_CTRL, interface, 0x3);
  HWIO_OUTXI(_inst_, BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_STAGE2_HANDSHAKE_CTRL, interface, 0x3);
}

/** =============================================================================
 *  Function : Mode_Switch 
 ** =============================================================================
 */
/**
 *    @details
 *    switch the clock mode
 **/
void BIMC_MISC_GLOBAL_CSR_Mode_Switch( uint32 _inst_, uint32 interface, uint32 pll_mode )
{
  if (pll_mode) {
    HWIO_OUTXFI(_inst_, BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAL_CFG_2, interface, CAL_CLK_SWITCH_ENA, 1);
  }
  else
  {
    HWIO_OUTXFI(_inst_, BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAL_CFG_2, interface, CAL_CLK_SWITCH_ENA, 0);
  }
}


