#ifndef __DDRCC_CONFIG_H__
#define __DDRCC_CONFIG_H__
/**
 * @file  ddrcc_config.c
 * @brief  This is header file for ddrcc_config.c implementation for ddrcc
 *    static setting 
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     -------------------------------------------------------------
01/09/14   cchou      Initial version.
================================================================================
                   Copyright 2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/

/*==============================================================================
                                                             INCLUDES
==============================================================================*/
#include "HALhwio.h"

/*==============================================================================
                                           VARIABLE DEFINITIONS                     
==============================================================================*/
extern uint32 ddrcc_config[][2];    

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/ 
/* =============================================================================
 **  Function : EBI_DDRCC_Set_Config
 ** =============================================================================
 */
/**
 *    @details
 *     program EBI_DDRCC static settings 
 *
 *   @param[in] reg_base EBI DDRCC base address
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void EBI_DDRCC_Set_Config(uint32 reg_base);
 
#endif /* __DDRCC_CONFIG_H__ */
