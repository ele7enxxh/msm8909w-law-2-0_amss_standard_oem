#ifndef _BIMC_MC_SCMO_SEQ_HEADER_
#define _BIMC_MC_SCMO_SEQ_HEADER_
/**
 * @file bimc_mc_scmo.h
 * @brief  This is header file for bimc_mc_scmo.c implementation.
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
 **  Function : Post_Init_Setup 
 ** =============================================================================
 */
/** 
 *  @details
 *    post initilization for SCMO; updating number of banks, rows, columns, address base etc.
 **/
void SCMO_CFG_Post_Init_Setup( uint32 _inst_, uint32 index, uint32 num_banks, uint32 num_rows, uint32 num_cols, uint32 interleave_en, uint32 addr_base );

/** =============================================================================
 **  Function : Enable_Rank
 ** =============================================================================
 */
/** 
 *    @details
 *    Enable rank(s) in SCMO registers
 * 
 *    @param[in] chip_select  chip select to set
 **/
void SCMO_CFG_Enable_Rank( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 **  Function : Disable_Rank
 ** =============================================================================
 */
/** 
 *    @details
 *    Disable rank(s) in SCMO registers
 * 
 *    @param[in] chip_select  chip select to disable
 **/
void SCMO_CFG_Disable_Rank( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 **  Function : Disable_Rank_With_Flush
 ** =============================================================================
 */
/**
 *    @details
 *    used for enter DPD sequence
 * 
 *    @param[in] chip_select  chip select to set
 **/
void SCMO_CFG_Disable_Rank_With_Flush( uint32 _inst_, uint32 chip_select );

#endif
