/**
 * @file  bimc_config.h
 * @brief  This is header file for bimc_config_xxxx.c implementation, including the static setting,
 * read latency, write latency, MR values based on different frequency plans
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

#ifndef __BIMC_CONFIG_H__
#define __BIMC_CONFIG_H__
/*==============================================================================
                                                             INCLUDES
==============================================================================*/
#include "HALhwio.h"
#include "ddr_params.h"
#include "bimc_struct.h"

extern uint32 BIMC_NUM_BYTES;
extern uint32 BIMC_chip_id;

/*
extern uint32 PCDDR3_Freq_Param[][PCDDR3_NUM_PARAMS];    
extern uint32 LPDDR3_Freq_Param[][LPDDR3_NUM_PARAMS];

extern uint32 PCDDR3_bimc_global0_config[][2];
extern uint32 PCDDR3_bimc_scmo_config[][2];
extern uint32 PCDDR3_bimc_dpe_config[][2];
extern uint32 PCDDR3_bimc_shke_config[][2];

extern uint32 LPDDR3_bimc_global0_config[][2];
extern uint32 LPDDR3_bimc_scmo_config[][2];
extern uint32 LPDDR3_bimc_dpe_config[][2];
extern uint32 LPDDR3_bimc_shke_config[][2];
*/

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/ 
/* =============================================================================
 **  Function : BIMC_GLOBAL0_Set_Config
 ** =============================================================================
 */
/**
 *    @details
 *     program BIMC_GLOBAL0 static settings
 *
 *   @param[in] reg_base BIMC GLOBAL0 base address
 *   @param[in] interface  interface to set  
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void BIMC_GLOBAL0_Set_Config(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_SCMO_Set_Config
 ** =============================================================================
 */
/**
 *    @details
 *     program BIMC_SCMO static settings
 *
 *   @param[in] reg_base BIMC SCMO base address
 *   @param[in] interface  interface to set  
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void BIMC_SCMO_Set_Config(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_DPE_Set_Config
 ** =============================================================================
 */
/**
 *    @details
 *     program BIMC_DPE static settings
 *
 *   @param[in] reg_base BIMC DPE base address
 *   @param[in] interface  interface to set  
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void BIMC_DPE_Set_Config(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_SHKE_Set_Config
 ** =============================================================================
 */
/**
 *    @details
 *     program BIMC_SHKE static settings
 *
 *   @param[in] reg_base BIMC SHKE base address
 *   @param[in] interface  interface to set  
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void BIMC_SHKE_Set_Config(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : Get_BIMC_LUT
 ** =============================================================================
 */
/**
 *    @details
 *     obtain WL/ RL/ MR setting from BIMC LUT
 *
 *   @param[in]  interface    Interface to set 
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *   @param[in]  which_LUT      for PCDDR3, 
 *                                          which_LUT = 0, return  WL
 *                                          which_LUT = 1, return  RL 
 *                                          which_LUT = 2, return  MR0 
 *                                          which_LUT = 3, return  MR1_without_wrlvl 
 *                                          which_LUT = 4, return  MR1_with_wrlvl 
 *                                          which_LUT = 5, return  MR2
 *                                          which_LUT = 6, return clk_mode
 *                                          for LPDDR3, 
 *                                          which_LUT = 0, return  WL
 *                                          which_LUT = 1, return  RL 
 *                                          which_LUT = 2, return  MR1 
 *                                          which_LUT = 3, return  MR2_without_wrlvl 
 *                                          which_LUT = 4, return  MR2_with_wrlvl 
 *                                          which_LUT = 5, return  clk_mode
 *   @param[out]  return WL or RL or MR0/1/2 from LUT
*
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint32 
Get_BIMC_LUT(SDRAM_INTERFACE interface, uint32 clk_freq_in_khz, uint32 which_LUT);

#endif /* __BIMC_CONFIG_H__ */
