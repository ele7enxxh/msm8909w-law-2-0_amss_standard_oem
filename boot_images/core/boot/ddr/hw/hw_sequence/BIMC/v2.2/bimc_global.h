#ifndef _BIMC_GLOBAL_SEQ_HEADER_
#define _BIMC_GLOBAL_SEQ_HEADER_
/**
 * @file bimc_global.h
 * @brief  This is header file for bimc_global.c implementation.
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
  Auto-generated sequence C codes from Autoseq 2.2 12/6/2013
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
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"
#include "bimc_struct.h"

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
 * 
 *    @param[in] interface    channel select to set
 *    @param[in] resolution   clock resolution; 1 for 1ps resolution where 0 for 100ps resolution
 *    @param[in] clk_period   clock period based the corresponding resolution
 **/
void BIMC_MISC_GLOBAL_CSR_Update_Clock_Period( uint32 _inst_, uint32 interface, uint32 resolution, uint32 clk_period );

/** =============================================================================
 *  Function : Get_Clock_Period
 ** =============================================================================
 */
/**
 *    @details
 *    get the current clock priod
 * 
 *    @param[in] interface    channel select to set
 **/
uint32 BIMC_MISC_GLOBAL_CSR_Get_Clock_Period( uint32 _inst_, uint32 interface );

/** =============================================================================
 *  Function : Freq_Switch 
 ** =============================================================================
 */
/**
 *    @details
 *    set the HANDSHAKE_CTRL bit for clock switch
 * 
 *    @param[in] interface    channel select to set
 **/
void BIMC_MISC_GLOBAL_CSR_Freq_Switch( uint32 _inst_, uint32 interface );

/** =============================================================================
 *  Function : Mode_Switch 
 ** =============================================================================
 */
/**
 *    @details
 *    switch the clock mode
 * 
 *    @param[in] interface    channel select to set
 *    @param[in] pll_mode     clock mode; 1 for pll mode, 0 for dynamic legacy mode
 **/
void BIMC_MISC_GLOBAL_CSR_Mode_Switch( uint32 _inst_, uint32 interface, uint32 pll_mode );

#endif
