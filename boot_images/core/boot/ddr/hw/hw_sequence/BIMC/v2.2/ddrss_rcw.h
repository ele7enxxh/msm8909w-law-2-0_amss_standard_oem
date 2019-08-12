/**
 * @file ddrss_rcw.h
 * @brief  This is header file for ddrss_rcw.c implementation.
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     ------------------------------------------------------
10/28/14   sk         Changes for 8952 Chipset
01/21/14   cchou      Adding API for restore/store RCW results from/to SW
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

#include "bimc_scale.h"
#include "ddr_phy_ddrss.h"
#include "ddr_target.h"
/*==============================================================================
                                 FUNCTIONS
==============================================================================*/   
/* =============================================================================
 **  Function : DDRSS_RCW_training_Per_Byte
 ** =============================================================================
 */
/**
 *   @brief
 *      training rcw result for each byte
 *
 *   @param[in]  channel_select    channel to set
 *   @param[in]  chip_select         chip select to initilize
 *   @param[in]  which_ren_cfg     which freq configuration
 *   @param[in]  which_byte         which dq byte 
 *   @param[in]   pattern_mode       2: check both 4 PEs/NEs and DQCAL_COMPARE_BYTEx 
 *                                                1: check 4 PEs/NEs only 
 *                                                0: check non-zero PEs/NEs (for rank auto detection)
 *   @param[in]  max_loop_cnt       loop count for  DQ read check
 *   @param[out] result 1/0 if training passes/fails for the specific byte
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint32
DDRSS_RCW_training_Per_Byte( SDRAM_INTERFACE channel_select, uint32 chip_select, uint32 which_ren_cfg,uint32 which_byte, uint32 pattern_mode, uint32 max_loop_cnt);

/* =============================================================================
 **  Function : DDRSS_RCW_training
 ** =============================================================================
 */
/**
 *   @brief
 *      the top level function for rcw training
 *
 *   @param[in]  channel_select    channel to set
 *   @param[in]  chip_select         chip select to initilize
 *   @param[in]  which_byte         which dq byte 
 *   @param[in]  clk_freq_in_khz   clock frequency (in khz) to train 
 *   @param[in]  max_loop_cnt       loop count for  DQ read check
 *   @param[out] result 1/0 if training passes/fails for the rcw training
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint32 
DDRSS_RCW_training( SDRAM_INTERFACE channel_select, uint32 chip_select, uint32 pattern_mode, uint32 clk_freq_in_khz, uint32 max_loop_cnt);

/* =============================================================================
**  Function : DDRSS_REN_RCW_Set_Trained_Results 
** =============================================================================
*/
/**
*    @details
*     restore REN training results from SW to BIMC/PHY 
* 
*   @param[in]  interface    Interface to set 
*   @param[in]  dq_ren_training REN_results
*   @param[in]  *rcw_results rcw training data structure
*   @param[in]  *freq          match the freq plan
*   @param[in]   size          size of the frequency plan
**/
void DDRSS_REN_RCW_Set_Trained_Results (uint16 dq_ren_training[2][DDR_FREQ_MAX_NUM_LEVELS][8], 
                                        uint32 start_dly_byte0_3[2][DDR_FREQ_MAX_NUM_LEVELS], 
                                        uint32 start_dly_byte4_7[2][DDR_FREQ_MAX_NUM_LEVELS], 
                                        uint32 *freq, uint32 size);
/* =============================================================================
**  Function : DDRSS_REN_RCW_Get_Trained_Results
** =============================================================================
*/
/**
*    @details
*     SW gets RCW/REN training results from BIMC/PHY 
*
*   @param[in]  interface    Interface to set 
*   @param[in]  dq_ren_training REN_results
*   @param[in]  *rcw_results rcw training data structure
*   @param[in]  *freq          match the freq plan
*   @param[in]   size          size of the frequency plan
**/
void DDRSS_REN_RCW_Get_Trained_Results (uint16 dq_ren_training[2][DDR_FREQ_MAX_NUM_LEVELS][8], uint32
start_dly_byte0_3[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 start_dly_byte4_7[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 *freq, uint32 size);


