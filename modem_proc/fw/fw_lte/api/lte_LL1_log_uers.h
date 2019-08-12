/*!
  @file
  lte_LL1_log_uers.h

  @brief
  UERS log packet data structures 

  @detail
  !!! Please do not modify !!!
  This file was auto-generated on Wed Oct 21 13:01:18 2009.
  Command line used: perl w2h.pl log_cqi cqi 0 0 0 0 0 0
   Note: The params, [0 0 0 0 0 0], correspond to version numbers for
         each log message in the listed order in the ICD

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_uers.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef LTE_LL1_LOG_UERS_H
#define LTE_LL1_LOG_UERS_H


/*==========================================================================*/
/*! @brief
 * 1 UERS tone log (Log code ID:   0xB14F) 
 *  This log packet gives important parameters related to UERS
 *  channel estimation results in TxMode7.
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_UERS_TONES_VERSION 22
#define LTE_LL1_MAX_NUM_SNR_TONES_PER_SF 1 
#define LTE_LL1_MAX_NUM_RX_TONES_PER_SF 1

typedef struct
{
  uint32 uers_snr_rx0;  ///< UERS SNR for rx0
  uint32 uers_snr_rx1;  ///< UERS SNR for rx1

}uers_snr_log_t;


typedef struct
{
  uint32 uers_tonepwr_rx0; ///< UERS tonepower for rx0
  uint32 uers_tonepwr_rx1; ///< UERS tonepower for rx1

}uers_rxtonepwr_snr_log_t;

/*! @brief uers_tones_log main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Range: 0..255
  uint32 sub_frame_num                    : 4;  ///< Range 0..9
  uint32 frame_number                     :10;  ///< Range 0..1023
  uint32 num_symbols_slot0                : 2;  ///< Range 0..2
  uint32 num_symbols_slot1                : 2;  ///< Range 0..2
  uint32 cp_mode                          : 1;  ///< 0x1: EXTND_CP
                                                ///< 0x0: NORM_CP
  uint32 slot_num                         : 1;  ///< 0x0: Slot0
                                                ///< 0x1: Slot1
  uint32 filter_order                     : 3;  ///< Range: 1..6
  uint32 reserved0                        : 1;
                                                         
  uint32 num_uers_rbs                     : 7;  ///< Range: 0..99

  uint32 delay_estimation                 : 16; ///< Range: 0..65535, 32U13
  uint32 delay_index                      : 3;  ///< Range: 0..4
  uint32 doppler_index                    : 2;  ///< Range: 0..3
  uint32 cinr_index                       : 3;  ///< Range: 0..5
  uint32 reserved1                        : 1;

  uint32 alpha                            : 16; ///< Range: 2979..32768, 16U11
  uint32 reserved2                        : 16;

  uint16 beta[12]                         ;     ///< Range: 0..65535, 16U15

  uint32 uers_snr_rx0                     : 32; ///< Range: 0..16777216, 32U8
  uint32 uers_snr_rx1                     : 32; ///< Range: 0..16777216, 32U8
  uint32 uers_tonepwr_rx0                 : 32; ///< Range: 0..4294967296, 32U31
  uint32 uers_tonepwr_rx1                 : 32; ///< Range: 0..4294967296, 32U31

  uint32 cinr_boost_rx0                   : 32; ///< Range: 0..1048575, 32U31 
  uint32 cinr_boost_rx1                   : 32; ///< Range: 0..1048575, 32U31 

  //uint32 max_cer_loc                      : 9;  ///< Range: 0..511
  //uint32 ttl_phase_incr                   : 16; ///< Range: 0..65535

  //uint32 ptm_phase_incr                   : 16;  ///< Range: 0..65535 
  //uint32 ttl_phase_comp                   : 16;  ///< Range: 0..65535 

  //uint32 uers_com                         : 9;  ///< Range: 0..511 
  //uint32 ttl_res_1                        : 9;  ///< Range: 0..511 
  //uint32 ttl_res_2                        : 9;  ///< Range: 0..511
  //uint32 ttl_res_3                        : 5;  ///< Range: 0..511
  //uers_snr_log_t           uers_snr_log[LTE_LL1_MAX_NUM_SNR_TONES_PER_SF];
  //uers_rxtonepwr_snr_log_t uers_rxtonepwr_snr_log[LTE_LL1_MAX_NUM_RX_TONES_PER_SF];   
}lte_LL1_log_uers_tones_ind_struct;


#endif //LTE_LL1_LOG_UERS_H
