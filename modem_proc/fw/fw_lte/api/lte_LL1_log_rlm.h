/*!
  @file
  lte_LL1_log_rlm.h

  @brief
  RLM log packet data structures

  @detail
  !!! Please do not modify !!!
  This file was auto-generated on Fri Feb 12 16:25:11 2010.
  Command line used: perl w2h.pl log_timing timing 1
   Note: The params, [1], correspond to version numbers for
         each log message in the listed order in the ICD

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_rlm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef LTE_LL1_LOG_RLM_H
#define LTE_LL1_LOG_RLM_H



/*==========================================================================*/
/*! @brief
 * 11 LTE LL1 Radio Link Monitoring (Log code ID: 0xB129) - internal
 *
 *  This log packet provides important parameters (SNR and BLER estimates) 
 *  related to the radio link status. The log record is generated once every 5 
 *  frames (in connected mode) or 5 drx cycles (in connected drx mode). 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_RLM_RADIO_LINK_MONITORING_VERSION 21
#define LTE_LL1_LOG_RLM_NUMBER_OF_SUBRECORDS 10
#define LTE_LL1_LOG_RLM_NUMBER_OF_RECORDS 5

/*! @brief radio_link_monitoring number_of_subrecords struct
 */
typedef struct
{
  uint32 sub_frame_number                 : 4;  ///< Sub-frame number of the current subrecord
                                                         ///< (Range: 0..9)
  uint32 system_frame_number              : 10;  ///< System frame number of the current subrecord
                                                         ///< (Range: 0..1023)
  uint32 reserved0                        : 2; 
  uint32 average_snr                      : 16;  ///< Per subframe average SNR in dB (16S8) for 10
                                                         ///< subframes.
                                                         ///< The actual SNR in dB = Average_SNR/256.
                                                         ///< The SNR range is (-128dB, 0dB).
  uint16 bler_for_out_of_sync            ;  ///< Per subframe instantaneous BLER for out of sync
                                                         ///< (OOS) mode (16U15) for 10 subframes.
                                                         ///< The actual BLER = BLER_for_OOS/32768.
                                                         ///< The BLER range is (0.00003, 1).
  uint16 bler_for_in_sync                ;  ///< Per subframe instantaneous BLER for in sync
                                                         ///< (IS) mode (16U15) for 10 subframes.
                                                         ///< The actual BLER = BLER_for_IS/32768.
                                                         ///< The BLER range is (0.00003, 1).
}lte_LL1_log_rlm_radio_link_monitoring_subrecords_s;


/*! @brief radio_link_monitoring number_of_records struct
 */
typedef struct
{
  uint32 cycle_start_sub_frame_number     : 4;  ///< Sub-frame number of the start of current cycle (Range:
                                                         ///< 0..9)
  uint32 cycle_start_system_frame_number  : 10;  ///< System frame number of the start of current cycle
                                                         ///< (Range: 0..1023)
  uint32 cycle_length                     : 11;  ///< Cycle length in unit of ms (always 10 for connected mode)                                                        
  uint32 reserved1                        : 7; 
  uint32 number_of_active_subframes       : 11;  ///< Number of active subframes within current cycle
  uint32 reserved2                        : 5; 
  uint32 filtered_bler_for_out_of_sync    : 16;  ///< Moving average filtered BLER for out of sync
                                                         ///< (OOS) mode (16U15).
                                                         ///< The actual BLER = Filtered_BLER_for_OOS/32768.
                                                         ///< The BLER range is (0.00003, 1).
  uint32 filtered_bler_for_in_sync        : 16;  ///< Moving average filtered BLER for in sync (IS)
                                                         ///< mode (16U15).
                                                         ///< The actual BLER = Filtered_BLER_for_IS/32768.
                                                         ///< The BLER range is (0.00003, 1).
  uint32 reserved3                        : 12; 
  uint32 number_of_subrecords             : 4;  ///< Range: 1..10. It indicates how many subredcords
                                                         ///< contain valid data. The remaining subrecords
                                                         ///< are populated with zeros.
  lte_LL1_log_rlm_radio_link_monitoring_subrecords_s lte_LL1_log_rlm_radio_link_monitoring_subrecords[LTE_LL1_LOG_RLM_NUMBER_OF_SUBRECORDS];
}lte_LL1_log_rlm_radio_link_monitoring_records_s;


/*! @brief radio_link_monitoring main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 2 for the log packet structure
                                                         ///< described below
  uint32 reserved0                        : 17; 
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint32 number_of_records                : 3;  ///< Range: 1..5
  lte_LL1_log_rlm_radio_link_monitoring_records_s lte_LL1_log_rlm_radio_link_monitoring_records[LTE_LL1_LOG_RLM_NUMBER_OF_RECORDS];
}lte_LL1_log_rlm_results_ind_struct;


#endif //LTE_LL1_LOG_RLM_H
