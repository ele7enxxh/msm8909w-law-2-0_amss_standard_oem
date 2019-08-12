/*!
  @file
  lte_LL1_log_embms.h

  @brief
  EMBMS log packet data structures 

  @detail
  !!! Please do not modify !!!
  This file was auto-generated on Wed Oct 21 13:01:18 2009.
  Command line used: perl w2h.pl log_cqi cqi 0 0 0 0 0 0
   Note: The params, [0 0 0 0 0 0], correspond to version numbers for
         each log message in the listed order in the ICD

*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_embms.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef LTE_LL1_LOG_EMBMS_H
#define LTE_LL1_LOG_EMBMS_H

/*==========================================================================*/
/*! @brief
 * 4 Whitened matrices log (Log code ID: 0xB125)
 *
 *  This log packet gives parameters related to the eMBMS whitened channel 
 *  matrices. The log record is generated every sub-frame during LTE_CQI task.
 *  only when eMBMS is decoded for this sub-frame
 *  Note: Depends on whether CR is approved by VPE team. Otherwise, can't
 *  send this log.
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_MBSFN_WHITENED_MATRICES_LOG_VERSION 21
#define LTE_LL1_MAX_NUM_WHITENING_SAMPLES_MBSFN 100
/*! @brief whitened_matrices_log main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Range: 0..255
  uint32 start_system_sub_frame_number_sfn : 4; ///< Range 0..9
  uint32 start_system_frame_number        : 10; ///< Range 0..1023
  uint32 mbsfn_area_id                    : 8;  ///< Range 0..255
  uint32 reserved0                        : 2; 
  uint32 num_whiten_matrices_for_mbsfn    : 7;  ///< Range: 0..100
  uint32 number_of_rx_antennas            : 1;  ///< 0x0: 1 antenna
                                                ///< 0x1: 2 antennas
  uint32 reserved1                        : 20; 
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC  
  uint32 whitened_channel_matrices[LTE_LL1_MAX_NUM_WHITENING_SAMPLES_MBSFN];  
}lte_LL1_log_mbsfn_whitened_matrices_ind_struct;

/*==========================================================================*/
/*! @brief
 * 4 LTE LL1 eMBMS CER (Log code ID: 0xB127)
 *
 *  This log packet gives important parameters related to the eMBMS 
 *  channel energy response (CER). The log record is generated once every k*50 
 *  milliseconds in round robin fashion for each valid Tx-Rx pair, where 'k' is 
 *  configurable.
 *  Note: Even though DIME don't plan to decode eMBMS for SCell. It doesn't hurt
 *  to add carrier index for future expansion.
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_MBSFN_EMBMS_CER_VERSION 23
#define LTE_LL1_LOG_MBSFN_SCELL_NUMBER_OF_RECORDS_CER_SIZE 512

/*! @brief embms_cer scell_number_of_records_cer_size struct
 */
typedef struct
{
  union
  {
    struct
    {
      int16 re;
      int16 im;
    };

    uint32 cer_per_rx_tx_pair;
  };

}lte_LL1_log_mbsfn_embms_cer_scell_records_cer_size_s;


/*! @brief embms_cer main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 1 for the log packet structure
                                                         ///< described below
  uint32 mbsfn_area_id                    : 8;  ///< Range: 0...255
  uint32 sub_frame_number                 : 4;  ///< Sub-frame number (Range 0..9)
  uint32 system_frame_number              : 10; ///< System frame number (range 0..1023)
  uint32 reserved0                        : 2; 
  uint32 cer_size_p                       : 3;  ///< Encoded as:
                                                ///< 0x0: 128
                                                ///< 0x1: 256
                                                ///< 0x2: 512
                                                ///< 0x3: 512 (10MHz, first 512 of total 1024)
                                                ///< 0x5: 512 (20MHz, first 512 of total 2048)
                                                ///< 0x5-0x7: Reserved
  uint32 rx_antenna_number                : 1;  ///< Encoded as:
                                                ///< 0x0: antenna 0
                                                ///< 0x1: antenna 1
  uint32 configuration                    : 4; 
  uint32 reserved1                        : 10;
  uint32 carrier_index                    : 4;  ///< 0-PCC, 1-SCC  
  uint32 scell_number_of_records_cer_size : 10; ///< Range: 1..512
  lte_LL1_log_mbsfn_embms_cer_scell_records_cer_size_s lte_LL1_log_mbsfn_embms_cer_scell_records_cer_size[LTE_LL1_LOG_MBSFN_SCELL_NUMBER_OF_RECORDS_CER_SIZE];
}lte_LL1_log_mbsfn_embms_cer_ind_struct;

#endif //LTE_LL1_LOG_EMBMS_H
