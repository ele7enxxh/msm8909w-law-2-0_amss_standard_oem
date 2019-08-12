#ifndef _EBI_SEQ_HEADER_
#define _EBI_SEQ_HEADER_
/**
 * @file ebi.h
 * @brief  This is header file for ebi.c implementation.
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
/*==============================================================================
                                                             INCLUDES
==============================================================================*/
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"
#include "ddrcc_config.h"
#include "bimc_scale.h"
/*==============================================================================
                                                             FUNCTIONS
==============================================================================*/    
/* =============================================================================
 **  Function : EBI1_PHY_CFG_DDR_CC_Pre_Init_Setup
 ** =============================================================================
 */
/**
 *   @brief
 *      one-time settings for EBI_DDRCC 
 *   @param[in]    reg_base   EBI reg_base  
 *   @param[in]    interface    Interface to set 
 *
 *   @dependencies None
 *
 *   @sideeffects  None 
 **/
void EBI1_PHY_CFG_DDR_CC_Pre_Init_Setup (uint32 reg_base, SDRAM_INTERFACE interface);

#endif /* __EBI_SEQ_HEADER__ */
