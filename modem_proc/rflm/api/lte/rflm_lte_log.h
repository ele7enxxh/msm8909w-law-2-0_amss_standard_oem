/*!
  @file
  rflm_lte_log.h

  @brief
  RF LOG API definitions for LTE to interact with SW and LM
  
  @detail
  This file will contain all definitios and declarations to be
  shared with SW and LM for LTE LM Logging
  
  @todo
 

*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/lte/rflm_lte_log.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/29/14   svi     Making FED build happy
01/28/14   svi     Initial Version - Support for LTE LM Logging


==============================================================================*/

#ifndef RFLM_LTE_LOG_H
#define RFLM_LTE_LOG_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "rflm.h"
#include "rflm_api_lte.h"
#include "rflm_lte.h"
#include "rflm_diag_log.h"
#include "log.h"

/*==============================================================================

                EXTERNAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/



/************************************************************/
/*            COMMON LM TO LTE LM CONVERSION                */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*! @brief RFLM LTE Internal log packet ID */
#define rflm_lte_diag_log_return_code_t rflm_diag_log_return_code_t

/*----------------------------------------------------------------------------*/



/************************************************************/
/*                LOG PACKET IDENTIFICATION                 */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*! @brief RFLM LTE Internal log packet ID */
#define RFLM_LTE_TXAGC_INTERNAL_LOG_PACKET_ID                0x184D

/*----------------------------------------------------------------------------*/
/*! @brief RFLM LTE External log packet ID */
#define RFLM_LTE_TXAGC_EXTERNAL_LOG_PACKET_ID                0x184C

/*----------------------------------------------------------------------------*/
/*! @brief RFLM LTE Internal TxAGC user ID */
#define RFLM_LTE_LOG_TXAGC_INTERNAL_PACKET_ID                0

/*----------------------------------------------------------------------------*/
/*! @brief RFLM LTE External TxAGC user ID */
#define RFLM_LTE_LOG_TXAGC_EXTERNAL_PACKET_ID                1

/*----------------------------------------------------------------------------*/
/*! @brief RFLM LTE Internal TxAGC tag size */
#define RFLM_LTE_LOG_TXAGC_INTERNAL_PACKET_TAG_SIZE_WORDS    0

/*----------------------------------------------------------------------------*/
/*! @brief RFLM LTE External TxAGC tag size */
#define RFLM_LTE_LOG_TXAGC_EXTERNAL_PACKET_TAG_SIZE_WORDS    0



/************************************************************/
/*                   LOG PACKET VERSIONS                    */
/************************************************************/


/*----------------------------------------------------------------------------*/
/*!@brief Enumeration indicating the TxAGC Internal Log Packet version */
typedef enum
{
  RFLM_LTE_TXAGC_INTERNAL_LOG_VER_1 = 1, /*!< TxAGC Internal Log Packet Version 1 */

} rflm_lte_agc_txagc_internal_version_type;

/*----------------------------------------------------------------------------*/
/*!@brief Enumeration indicating the TxAGC External Log Packet version */
typedef enum
{
  RFLM_LTE_TXAGC_EXTERNAL_LOG_VER_1 = 1, /*!< TxAGC External Log Packet Version 1 */

} rflm_lte_agc_txagc_external_version_type;

/*----------------------------------------------------------------------------*/



/************************************************************/
/*                   MISC DEFINITIONS                       */
/************************************************************/


/*----------------------------------------------------------------------------*/
/*! @brief RFLM LTE Log data to be filled in reserved fields */
#define RFLM_LTE_LOG_RESERVED_FIELD_DATA                     0

/*----------------------------------------------------------------------------*/



/*==============================================================================

              EXTERNAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/************************************************************/
/*                  LOG API RETURN STATUS                   */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Enumeration indicating error type for RFLM LTE Logging
*/
typedef enum
{
  RFLM_LTE_LOG_ERR_NONE,         /*!< RFLM LTE Logging Success */
  RFLM_LTE_LOG_DIAG_ALLCO_ERR,   /*!< RFLM LTE Logging Failure as Diag Lof Alloc Failed */
  RFLM_LTE_LOG_ERR,              /*!< RFLM LTE Logging Failure */
  RFLM_LTE_LOG_MAX_ERR           /*!< RFLM LTE Logging Invalid Status */

}rflm_lte_log_err_t;

/*==============================================================================

            EXTERNAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/************************************************************/
/*              LOG PACKET PAYLOAD DEFINITION               */
/************************************************************/

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  TxAGC External Log packet
*/
typedef struct
{

  /*********************************************************/
  /*!                 LOG PACKET SETTINGS                  */
  /*********************************************************/ 

  uint32 log_version             :  8; /*!< External Log Packet Version */
  uint32 log_info                : 24; /*!< Reserved */

  uint32 log_index               : 32; /*!< Log Index in Debug Structure */ 

  /*********************************************************/
  /*!               LOG SUB FRAME SETTINGS                 */
  /*********************************************************/ 

  uint32 frame_num               : 32; /*!< Frame Number */

  uint32 sf_number               :  8; /*!< Sub Frame Number */
  uint32 subframe_type           : 16; /*!< Sub Frame Type */    
  uint32 sf_info                 :  8; /*!< Reserved */  
  
  uint32 freq_hopping_enable     :  8; /*!< Freq Hopping is enabled */
  uint32 txpll_afc_update        :  8; /*!< AFC correction is done */
  uint32 enable_info             : 16; /*!< Reserved */  
  
  uint32 slot1_active            :  8; /*!< Active Slot - Slot 1 */
  uint32 slot2_active            :  8; /*!< Active Slot - Slot 2 */
  uint32 srs1_active             :  8; /*!< Active Slot - SRS 1 */
  uint32 srs2_active             :  8; /*!< Active Slot - SRS 2 */

  /*********************************************************/
  /*!                 LOG TXAGC SETTINGS                   */
  /*********************************************************/ 
  
  uint32 slot1_pa_state          :  8; /*!< PA Gain State - Slot 1 */
  uint32 slot2_pa_state          :  8; /*!< PA Gain State - Slot 2 */
  uint32 srs1_pa_state           :  8; /*!< PA Gain State - SRS 1 */
  uint32 srs2_pa_state           :  8; /*!< PA Gain State - SRS 2 */

  uint32 slot1_rgi_index         :  8; /*!< RGI Index - Slot 1 */
  uint32 slot2_rgi_index         :  8; /*!< RGI Index - Slot 2 */
  uint32 srs1_rgi_index          :  8; /*!< RGI Index - SRS 1 */
  uint32 srs2_rgi_index          :  8; /*!< RGI Index - SRS 2 */

  uint32 slot1_pwr_db10          : 32; /*!< Tx Power dB10 - Slot 1 */

  uint32 slot2_pwr_db10          : 32; /*!< Tx Power dB10 - Slot 2 */

  uint32 srs1_pwr_db10           : 32; /*!< Tx Power dB10 - SRS 1 */

  uint32 srs2_pwr_db10           : 32; /*!< Tx Power dB10 - SRS 2 */  

  /*********************************************************/
  /*!             LOG DIGITAL TXAGC SETTINGS               */
  /*********************************************************/ 

  /* ------- SLOT 1 -------- */

  uint32 env_scale_slot1                     : 32;

  uint32 iq_gain_slot1                       : 32;

  uint32 xpt_mode_slot1                      :  8;
  uint32 iref_slot1                          :  8;
  uint32 dpd_lut_lmem_idx_slot1              :  8;
  uint32 dpd_hw_tbl_idx_slot1                :  8;
  
  uint32 drif_group_slot1                    :  8;
  uint32 lin_tbl_idx_slot1                   :  8;
  uint32 first_sweep_lin_tbl_idx_slot1       :  8;
  uint32 lin_info_slot1                      :  8; /*!< Reserved */    
  
  uint32 do_fbrx_slot1                       : 32;
  uint32 fbrx_correction_slot1               : 32;
  uint32 pin_comp_slot1                      : 32;
  uint32 pout_comp_slot1                     : 32;
  uint32 dac_set_point_slot1                 : 32;  

  /* ------- SLOT 2 -------- */

  uint32 env_scale_slot2                     : 32;

  uint32 iq_gain_slot2                       : 32;

  uint32 xpt_mode_slot2                      :  8;
  uint32 iref_slot2                          :  8;
  uint32 dpd_lut_lmem_idx_slot2              :  8;
  uint32 dpd_hw_tbl_idx_slot2                :  8;
  
  uint32 drif_group_slot2                    :  8;
  uint32 lin_tbl_idx_slot2                   :  8;
  uint32 first_sweep_lin_tbl_idx_slot2       :  8;
  uint32 lin_info_slot2                      :  8; /*!< Reserved */    
  
  uint32 do_fbrx_slot2                       : 32;
  uint32 fbrx_correction_slot2               : 32;
  uint32 pin_comp_slot2                      : 32;
  uint32 pout_comp_slot2                     : 32;
  uint32 dac_set_point_slot2                 : 32;  

  /* ------- SRS 1 -------- */

  uint32 env_scale_srs1                      : 32;

  uint32 iq_gain_srs1                        : 32;

  uint32 xpt_mode_srs1                       :  8;
  uint32 iref_srs1                           :  8;
  uint32 dpd_lut_lmem_idx_srs1               :  8;
  uint32 dpd_hw_tbl_idx_srs1                 :  8;
  
  uint32 drif_group_srs1                     :  8;
  uint32 lin_tbl_idx_srs1                    :  8;
  uint32 first_sweep_lin_tbl_idx_srs1        :  8;
  uint32 lin_info_srs1                       :  8; /*!< Reserved */    
  
  uint32 do_fbrx_srs1                        : 32;
  uint32 fbrx_correction_srs1                : 32;
  uint32 pin_comp_srs1                       : 32;
  uint32 pout_comp_srs1                      : 32;
  uint32 dac_set_point_srs1                  : 32;  

  /* ------- SRS 2 -------- */

  uint32 env_scale_srs2                      : 32;

  uint32 iq_gain_srs2                        : 32;

  uint32 xpt_mode_srs2                       :  8;
  uint32 iref_srs2                           :  8;
  uint32 dpd_lut_lmem_idx_srs2               :  8;
  uint32 dpd_hw_tbl_idx_srs2                 :  8;
  
  uint32 drif_group_srs2                     :  8;
  uint32 lin_tbl_idx_srs2                    :  8;
  uint32 first_sweep_lin_tbl_idx_srs2        :  8;
  uint32 lin_info_srs2                       :  8; /*!< Reserved */    
  
  uint32 do_fbrx_srs2                        : 32;
  uint32 fbrx_correction_srs2                : 32;
  uint32 pin_comp_srs2                       : 32;
  uint32 pout_comp_srs2                      : 32;
  uint32 dac_set_point_srs2                  : 32;  
  
}rflm_lte_txagc_external_log_packet_t;

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  TxAGC Internal Log packet
*/
typedef struct
{

  /*********************************************************/
  /*!                 LOG PACKET SETTINGS                  */
  /*********************************************************/ 
  
  uint32 log_version                       :  8; /*!< Internal Log Packet Version */
  uint32 log_info                          : 24; /*!< Reserved */

  uint32 log_index                         : 32; /*!< Log Index in Debug Structure */ 

  /*********************************************************/
  /*!               LOG SUB FRAME SETTINGS                 */
  /*********************************************************/ 

  uint32 frame_num                         : 32; /*!< Frame Number */

  uint32 sf_number                         :  8; /*!< Sub Frame Number */
  uint32 subframe_type                     : 16; /*!< Sub Frame Type */    
  uint32 sf_info                           :  8; /*!< Reserved */  

  uint32 freq_hopping_enable               :  8; /*!< Freq Hopping is enabled */
  uint32 txpll_afc_update                  :  8; /*!< AFC correction is done */
  uint32 enable_info                       : 16; /*!< Reserved */  

  uint32 slot1_active                      :  8; /*!< Active Slot - Slot 1 */
  uint32 slot2_active                      :  8; /*!< Active Slot - Slot 2 */
  uint32 srs1_active                       :  8; /*!< Active Slot - SRS 1 */
  uint32 srs2_active                       :  8; /*!< Active Slot - SRS 2 */

  uint32 tx_pll_afc_action_time            : 32; /*!< TxAGC Action Time */
  
  uint32 tx_pll_afc_ccs_tq_data_offset     : 32; /*!< Tx AGC CCS TQ Data Offset */


  /*********************************************************/
  /*!                TIMING LOG FOR SLOT 1                 */
  /*********************************************************/ 

  uint32 tx_on_off_event_slot1                   : 16; /*!< Tx ON or OFF Event */
  uint32 tx_en_high_low_event_slot1              : 16; /*!< Tx EN HIGH or LOW Event */  

  uint32 tx_on_off_action_time_slot1             : 32; /*!< Tx ON or OFF Action Time */

  uint32 tx_on_off_ccs_tq_data_offset_slot1      : 32; /*!< Tx ON or OFF Event CCS TQ Data Offset */

  uint32 tx_en_action_time_slot1                 : 32; /*!< Tx EN High or Low Action Time */
  
  uint32 tx_en_ccs_tq_data_offset_slot1          : 32; /*!< Tx EN High or Low Event CCS TQ Data Offset */

  uint32 tx_on_off_enqueued_slot1                :  8; /*! < TX ON OFF was enqueued to CCS */
  uint32 tx_on_off_serviced_slot1                :  8; /*! < TX ON OFF was serviced by CCS */
  uint32 tx_en_high_low_enqueued_slot1           :  8; /*! < TX EN HIGH LOW was enqueued to CCS */ 
  uint32 tx_en_high_low_serviced_slot1           :  8; /*! < TX EN HIGH LOW was serviced by CCS */  

  uint32 txagc_action_time_slot1                 : 32; /*!< TxAGC Action Time */
  
  uint32 txagc_ccs_tq_data_offset_slot1          : 32; /*!< Tx AGC CCS TQ Data Offset */

  uint32 pa_on_non_trigger_action_time_slot1     : 32; /*!< PA ON NON Trigger Action Time */
  
  uint32 pa_on_trigger_action_time_slot1         : 32; /*!< PA ON Trigger Action Time */
  
  uint32 qpoet_non_trigger_action_time_slot1     : 32; /*!< QPOET NON Trigger Action Time */
  
  uint32 qpoet_trigger_action_time_slot1         : 32; /*!< QPOET Trigger Action Time */
  
  uint32 wtr_non_trigger_action_time_slot1       : 32; /*!< WTR NON Trigger Action Time */
  
  uint32 wtr_trigger_action_time_slot1           : 32; /*!< WTR Trigger Action Time */

  uint32 wtr_wxe_non_trigger_action_time_slot1   : 32; /*!< WTR WXE NON Trigger Action Time */
  
  uint32 wtr_wxe_trigger_action_time_slot1       : 32; /*!< WTR WXE Trigger Action Time */
  
  uint32 asm_non_trigger_action_time_slot1       : 32; /*!< ASM NON Trigger Action Time */
  
  uint32 asm_trigger_action_time_slot1           : 32; /*!< ASM Trigger Action Time */
    
  uint32 aux_asm_non_trigger_action_time_slot1   : 32; /*!< AUX ASM NON Trigger Action Time */
  
  uint32 aux_asm_trigger_action_time_slot1       : 32; /*!< AUX ASM Trigger Action Time */

  uint32 fbrx_non_trigger_action_time_slot1      : 32; /*!< FBRx NON Trigger Action Time */
  
  uint32 fbrx_trigger_action_time_slot1          : 32; /*!< FBRx Trigger Action Time */

  uint32 ant_swap_tx0_low_action_time_slot1      : 32; /*!< ANT SWAP Tx 0 Low Action Time */

  uint32 ant_swap_tx0_high_action_time_slot1     : 32; /*!< ANT SWAP Tx 0 High Action Time */

  uint32 ant_swap_tx1_low_action_time_slot1      : 32; /*!< ANT SWAP Tx 1 Low Action Time */

  uint32 ant_swap_tx1_high_action_time_slot1     : 32; /*!< ANT SWAP Tx 1 High Action Time */
  

  /*********************************************************/
  /*!                TIMING LOG FOR SLOT 2                 */
  /*********************************************************/ 

  uint32 tx_on_off_event_slot2                   : 16; /*!< Tx ON or OFF Event */
  uint32 tx_en_high_low_event_slot2              : 16; /*!< Tx EN HIGH or LOW Event */  

  uint32 tx_on_off_action_time_slot2             : 32; /*!< Tx ON or OFF Action Time */

  uint32 tx_on_off_ccs_tq_data_offset_slot2      : 32; /*!< Tx ON or OFF Event CCS TQ Data Offset */

  uint32 tx_en_action_time_slot2                 : 32; /*!< Tx EN High or Low Action Time */
  
  uint32 tx_en_ccs_tq_data_offset_slot2          : 32; /*!< Tx EN High or Low Event CCS TQ Data Offset */

  uint32 tx_on_off_enqueued_slot2                :  8; /*! < TX ON OFF was enqueued to CCS */
  uint32 tx_on_off_serviced_slot2                :  8; /*! < TX ON OFF was serviced by CCS */
  uint32 tx_en_high_low_enqueued_slot2           :  8; /*! < TX EN HIGH LOW was enqueued to CCS */ 
  uint32 tx_en_high_low_serviced_slot2           :  8; /*! < TX EN HIGH LOW was serviced by CCS */  
  
  uint32 txagc_action_time_slot2                 : 32; /*!< TxAGC Action Time */
  
  uint32 txagc_ccs_tq_data_offset_slot2          : 32; /*!< Tx AGC CCS TQ Data Offset */

  uint32 pa_on_non_trigger_action_time_slot2     : 32; /*!< PA ON NON Trigger Action Time */
  
  uint32 pa_on_trigger_action_time_slot2         : 32; /*!< PA ON Trigger Action Time */
  
  uint32 qpoet_non_trigger_action_time_slot2     : 32; /*!< QPOET NON Trigger Action Time */
  
  uint32 qpoet_trigger_action_time_slot2         : 32; /*!< QPOET Trigger Action Time */
  
  uint32 wtr_non_trigger_action_time_slot2       : 32; /*!< WTR NON Trigger Action Time */
  
  uint32 wtr_trigger_action_time_slot2           : 32; /*!< WTR Trigger Action Time */

  uint32 wtr_wxe_non_trigger_action_time_slot2   : 32; /*!< WTR WXE NON Trigger Action Time */
  
  uint32 wtr_wxe_trigger_action_time_slot2       : 32; /*!< WTR WXE Trigger Action Time */
  
  uint32 asm_non_trigger_action_time_slot2       : 32; /*!< ASM NON Trigger Action Time */
  
  uint32 asm_trigger_action_time_slot2           : 32; /*!< ASM Trigger Action Time */
    
  uint32 aux_asm_non_trigger_action_time_slot2   : 32; /*!< AUX ASM NON Trigger Action Time */
  
  uint32 aux_asm_trigger_action_time_slot2       : 32; /*!< AUX ASM Trigger Action Time */

  uint32 fbrx_non_trigger_action_time_slot2      : 32; /*!< FBRx NON Trigger Action Time */
  
  uint32 fbrx_trigger_action_time_slot2          : 32; /*!< FBRx Trigger Action Time */

  uint32 ant_swap_tx0_low_action_time_slot2      : 32; /*!< ANT SWAP Tx 0 Low Action Time */

  uint32 ant_swap_tx0_high_action_time_slot2     : 32; /*!< ANT SWAP Tx 0 High Action Time */

  uint32 ant_swap_tx1_low_action_time_slot2      : 32; /*!< ANT SWAP Tx 1 Low Action Time */

  uint32 ant_swap_tx1_high_action_time_slot2     : 32; /*!< ANT SWAP Tx 1 High Action Time */
  

  /*********************************************************/
  /*!                 TIMING LOG FOR SRS 1                 */
  /*********************************************************/ 

  uint32 tx_on_off_event_srs1                   : 16; /*!< Tx ON or OFF Event */
  uint32 tx_en_high_low_event_srs1              : 16; /*!< Tx EN HIGH or LOW Event */  

  uint32 tx_on_off_action_time_srs1             : 32; /*!< Tx ON or OFF Action Time */

  uint32 tx_on_off_ccs_tq_data_offset_srs1      : 32; /*!< Tx ON or OFF Event CCS TQ Data Offset */

  uint32 tx_en_action_time_srs1                 : 32; /*!< Tx EN High or Low Action Time */
  
  uint32 tx_en_ccs_tq_data_offset_srs1          : 32; /*!< Tx EN High or Low Event CCS TQ Data Offset */

  uint32 tx_on_off_enqueued_srs1                :  8; /*! < TX ON OFF was enqueued to CCS */
  uint32 tx_on_off_serviced_srs1                :  8; /*! < TX ON OFF was serviced by CCS */
  uint32 tx_en_high_low_enqueued_srs1           :  8; /*! < TX EN HIGH LOW was enqueued to CCS */ 
  uint32 tx_en_high_low_serviced_srs1           :  8; /*! < TX EN HIGH LOW was serviced by CCS */  

  uint32 txagc_action_time_srs1                 : 32; /*!< TxAGC Action Time */
  
  uint32 txagc_ccs_tq_data_offset_srs1          : 32; /*!< Tx AGC CCS TQ Data Offset */

  uint32 pa_on_non_trigger_action_time_srs1     : 32; /*!< PA ON NON Trigger Action Time */
  
  uint32 pa_on_trigger_action_time_srs1         : 32; /*!< PA ON Trigger Action Time */
  
  uint32 qpoet_non_trigger_action_time_srs1     : 32; /*!< QPOET NON Trigger Action Time */
  
  uint32 qpoet_trigger_action_time_srs1         : 32; /*!< QPOET Trigger Action Time */
  
  uint32 wtr_non_trigger_action_time_srs1       : 32; /*!< WTR NON Trigger Action Time */
  
  uint32 wtr_trigger_action_time_srs1           : 32; /*!< WTR Trigger Action Time */

  uint32 wtr_wxe_non_trigger_action_time_srs1   : 32; /*!< WTR WXE NON Trigger Action Time */
  
  uint32 wtr_wxe_trigger_action_time_srs1       : 32; /*!< WTR WXE Trigger Action Time */
  
  uint32 asm_non_trigger_action_time_srs1       : 32; /*!< ASM NON Trigger Action Time */
  
  uint32 asm_trigger_action_time_srs1           : 32; /*!< ASM Trigger Action Time */
    
  uint32 aux_asm_non_trigger_action_time_srs1   : 32; /*!< AUX ASM NON Trigger Action Time */
  
  uint32 aux_asm_trigger_action_time_srs1       : 32; /*!< AUX ASM Trigger Action Time */

  uint32 fbrx_non_trigger_action_time_srs1      : 32; /*!< FBRx NON Trigger Action Time */
  
  uint32 fbrx_trigger_action_time_srs1          : 32; /*!< FBRx Trigger Action Time */

  uint32 ant_swap_tx0_low_action_time_srs1      : 32; /*!< ANT SWAP Tx 0 Low Action Time */

  uint32 ant_swap_tx0_high_action_time_srs1     : 32; /*!< ANT SWAP Tx 0 High Action Time */

  uint32 ant_swap_tx1_low_action_time_srs1      : 32; /*!< ANT SWAP Tx 1 Low Action Time */

  uint32 ant_swap_tx1_high_action_time_srs1     : 32; /*!< ANT SWAP Tx 1 High Action Time */

  /*********************************************************/
  /*!                 TIMING LOG FOR SRS 2                 */
  /*********************************************************/ 

  uint32 tx_on_off_event_srs2                   : 16; /*!< Tx ON or OFF Event */
  uint32 tx_en_high_low_event_srs2              : 16; /*!< Tx EN HIGH or LOW Event */  

  uint32 tx_on_off_action_time_srs2             : 32; /*!< Tx ON or OFF Action Time */

  uint32 tx_on_off_ccs_tq_data_offset_srs2      : 32; /*!< Tx ON or OFF Event CCS TQ Data Offset */

  uint32 tx_en_action_time_srs2                 : 32; /*!< Tx EN High or Low Action Time */
  
  uint32 tx_en_ccs_tq_data_offset_srs2          : 32; /*!< Tx EN High or Low Event CCS TQ Data Offset */

  uint32 tx_on_off_enqueued_srs2                :  8; /*! < TX ON OFF was enqueued to CCS */
  uint32 tx_on_off_serviced_srs2                :  8; /*! < TX ON OFF was serviced by CCS */
  uint32 tx_en_high_low_enqueued_srs2           :  8; /*! < TX EN HIGH LOW was enqueued to CCS */ 
  uint32 tx_en_high_low_serviced_srs2           :  8; /*! < TX EN HIGH LOW was serviced by CCS */  

  uint32 txagc_action_time_srs2                 : 32; /*!< TxAGC Action Time */
  
  uint32 txagc_ccs_tq_data_offset_srs2          : 32; /*!< Tx AGC CCS TQ Data Offset */

  uint32 pa_on_non_trigger_action_time_srs2     : 32; /*!< PA ON NON Trigger Action Time */
  
  uint32 pa_on_trigger_action_time_srs2         : 32; /*!< PA ON Trigger Action Time */
  
  uint32 qpoet_non_trigger_action_time_srs2     : 32; /*!< QPOET NON Trigger Action Time */
  
  uint32 qpoet_trigger_action_time_srs2         : 32; /*!< QPOET Trigger Action Time */
  
  uint32 wtr_non_trigger_action_time_srs2       : 32; /*!< WTR NON Trigger Action Time */
  
  uint32 wtr_trigger_action_time_srs2           : 32; /*!< WTR Trigger Action Time */

  uint32 wtr_wxe_non_trigger_action_time_srs2   : 32; /*!< WTR WXE NON Trigger Action Time */
  
  uint32 wtr_wxe_trigger_action_time_srs2       : 32; /*!< WTR WXE Trigger Action Time */
  
  uint32 asm_non_trigger_action_time_srs2       : 32; /*!< ASM NON Trigger Action Time */
  
  uint32 asm_trigger_action_time_srs2           : 32; /*!< ASM Trigger Action Time */
    
  uint32 aux_asm_non_trigger_action_time_srs2   : 32; /*!< AUX ASM NON Trigger Action Time */
  
  uint32 aux_asm_trigger_action_time_srs2       : 32; /*!< AUX ASM Trigger Action Time */

  uint32 fbrx_non_trigger_action_time_srs2      : 32; /*!< FBRx NON Trigger Action Time */
  
  uint32 fbrx_trigger_action_time_srs2          : 32; /*!< FBRx Trigger Action Time */

  uint32 ant_swap_tx0_low_action_time_srs2      : 32; /*!< ANT SWAP Tx 0 Low Action Time */

  uint32 ant_swap_tx0_high_action_time_srs2     : 32; /*!< ANT SWAP Tx 0 High Action Time */

  uint32 ant_swap_tx1_low_action_time_srs2      : 32; /*!< ANT SWAP Tx 1 Low Action Time */

  uint32 ant_swap_tx1_high_action_time_srs2     : 32; /*!< ANT SWAP Tx 1 High Action Time */
  

}rflm_lte_txagc_internal_log_packet_t;

/*----------------------------------------------------------------------------*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
              API input/output function definitions -- TXAGC 
==============================================================================*/

/************************************************************/
/*           DIAG LOG SUBPACKET API DEFINITIONS             */
/************************************************************/

/*----------------------------------------------------------------------------*/
rflm_lte_log_err_t
rflm_lte_external_txagc_diag_log_subpacket
(
  rflm_lte_tech_data_s        *rflm_lte_dm_ptr,
  uint32                       log_index
);

/*----------------------------------------------------------------------------*/
rflm_lte_log_err_t
rflm_lte_internal_txagc_diag_log_subpacket
(
  rflm_lte_tech_data_s        *rflm_lte_dm_ptr,
  uint32                       log_index
);

/*----------------------------------------------------------------------------*/



#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_LTE_LOG_H */
