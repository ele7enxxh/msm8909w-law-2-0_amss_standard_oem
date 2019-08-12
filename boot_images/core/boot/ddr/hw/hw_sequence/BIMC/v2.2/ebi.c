/**
 * @file  ebi.c 
 * @brief EBI PHY
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
#include "ebi.h"

#ifndef SEQ_EBI1_PHY_CFG_CH1_DDR_CC_OFFSET
#define SEQ_EBI1_PHY_CFG_CH1_DDR_CC_OFFSET SEQ_EBI1_PHY_CFG_CH0_DDR_CC_OFFSET
#endif

#define REG_OFFSET_DDRCC(INTERFACE) ((INTERFACE == SDRAM_INTERFACE_0) ? \
SEQ_EBI1_PHY_CFG_CH0_DDR_CC_OFFSET : \
    SEQ_EBI1_PHY_CFG_CH1_DDR_CC_OFFSET)
    
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
 **/
void EBI1_PHY_CFG_DDR_CC_Pre_Init_Setup (uint32 reg_base, SDRAM_INTERFACE interface)
{
  EBI_DDRCC_Set_Config( reg_base + REG_OFFSET_DDRCC(interface));
}

