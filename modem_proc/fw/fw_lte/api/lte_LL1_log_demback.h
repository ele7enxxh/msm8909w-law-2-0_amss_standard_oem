/*!
  @file
  lte_LL1_log_demback.h

  @brief
  demback log packet data structures

  @detail
  !!! Please do not modify !!!
  This file was auto-generated on Thu Apr  7 17:22:18 2011.
  Command line used: perl w2h.pl log_demback demback 1 3 2 3 2
   Note: The params, [1 3 2 3 2], correspond to version numbers for
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_demback.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef LTE_LL1_LOG_DEMBACK_H
#define LTE_LL1_LOG_DEMBACK_H


/*==========================================================================*/
/*! @brief
 * 1 LTE LL1 PCFICH Decoding Results (Log code ID: 0xB12A)
 *
 *  This log packet gives important parameters related to the PCFICH decoding 
 *  results. The log record is generated once every 20 sub-frames or every 
 *  100ms, whichever occurs first. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_DEMBACK_PCFICH_DECODING_RESULTS_VERSION 23
#define LTE_LL1_LOG_DEMBACK_CFI_20MS_FIXED 20

/*! @brief pcfich_decoding_results cfi_20ms_fixed struct
 */
typedef struct
{
  uint64 sub_frame_offset                 : 8;  ///< Offset relative to the starting SFN
  uint64 pdcch_demap_status               : 10;  ///< Demap status
  uint64 pcfich_cfi                       : 2;  ///< CFI value
  uint64 cch_enable_mask                  : 18;  ///< Scheduling information for CCH demod
  uint64 reserved1                        : 10;  ///< Unused
  uint64 reserved0                        : 16; 
}lte_LL1_log_demback_pcfich_decoding_results_cfi_20ms_s;


/*! @brief pcfich_decoding_results main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 1 for the log packet structure
                                                         ///< described below
  uint32 serving_cell_id                  : 9;  ///< Range: 0...503
  uint32 number_of_records                : 5;  ///< Range: 0..20
  uint32 sub_frame_number                 : 4;  ///< Sub-frame number (Range 0..9)
  uint32 reserved0                        : 6; 
  uint32 system_frame_number              : 10;  ///< System frame number (range 0..1023)
  uint32 cfi                              : 2;  ///< CFI value
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint32 reserved1                        : 16; 
  lte_LL1_log_demback_pcfich_decoding_results_cfi_20ms_s lte_LL1_log_demback_pcfich_decoding_results_cfi_20ms[LTE_LL1_LOG_DEMBACK_CFI_20MS_FIXED];
}lte_LL1_log_demback_pcfich_decoding_results_ind_struct;



/*==========================================================================*/
/*! @brief
 * 2 LTE LL1 PHICH decoding results (LOG_CODE 0xB12C)
 *
 *  This log packet gives important parameters related to the PHICH 
 *  decoding results. The log record is generated once after accumulating 
 *  20 records or every 100 ms, whichever occurs first. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_DEMBACK_PHICH_DECODING_RESULTS_VERSION 4
#define LTE_LL1_LOG_DEMBACK_NUMBER_OF_RECORDS 20

/*! @brief phich_decoding_results number_of_records struct
 */
typedef struct
{
  uint32 subframe_number                  : 4;  ///< Subframe number; range is 0 to 9
  uint32 system_frame_number              :10;  ///< System frame number; range is 0 to 1023
  uint32 phich_enabled_0                  : 1;  ///< 1 -PHICH result 0 is valid (TRUE)
                                                         ///< 0 -PHICH result 0 is not valid
                                                         ///< FDD and TDD all UL/DLconfigurations
  uint32 phich_enabled_1                  : 1;  ///< 1 - for TDD UL/DL configuration0 and PUSCH
                                                         ///< transmission in SF4 or SF9. By spec, this  implies
                                                         ///< I_PHICH is true
                                                         ///< 0 - otherwise
  uint32 decoding_outcome_0               : 1;  ///< 1 - ACK
                                                         ///< 0 -NACK
                                                         ///< Valid only if PHICH Enabled 0 is TRUE
  uint32 decoding_outcome_1               : 1;  ///< 1 - ACK
                                                         ///< 0 -NACK
                                                         ///< Valid only if PHICH Enabled 1 is TRUE
  uint32 spreading_code_index_0           : 3;  ///< Range is 0 to 7
                                                ///< Valid only if PHICH Enabled 0 is TRUE
  uint32 spreading_code_index_1           : 3;  ///< Range is 0 to 7
                                                ///< Valid only if PHICH Enabled 1 is TRUE
  uint32 reserved0                        : 8; 

  uint32 phich_group_number_0             : 5;  ///< Range is 0 to 31
                                                         ///< Valid only if PHICH Enabled 0 is TRUE
  uint32 phich_group_number_1             : 5;  ///< Range is 0 to 31
                                                         ///< Valid only if PHICH Enabled 1 is TRUE
  uint32 llr_of_ack_bit_0                 :10;  ///< Absolute value of LLR bit 0
                                                         ///< Range is 0 to 1023
                                                         ///< Valid only if PHICH Enabled 0 is TRUE
  uint32 llr_of_ack_bit_1                 :10;  ///< Absolute value of LLR bit 1
                                                         ///< Range is 0 to 1023
                                                         ///< Valid only if PHICH Enabled 1 is TRUE
  uint32 reserved1                        : 2; 
}lte_LL1_log_demback_phich_decoding_results_records_s;


/*! @brief phich_decoding_results main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version; range is 0 to 255.
                                                         ///< Version - 2 for the log packet structure described
                                                         ///< below
  uint32 serving_cell_id                  : 9;  ///< Range is 0 to 503
  uint32 reserved0                        : 10; 
  uint32 number_of_records                : 5;  ///< Range is 0 to 20
  lte_LL1_log_demback_phich_decoding_results_records_s lte_LL1_log_demback_phich_decoding_results_records[LTE_LL1_LOG_DEMBACK_NUMBER_OF_RECORDS];
}lte_LL1_log_demback_phich_decoding_results_ind_struct;



/*==========================================================================*/
/*! @brief
 * 3 LTE LL1 PBCH Decoding Results (Log code ID: 0xB12E)
 *
 *  This log packet gives important parameters related to the PBCH decoding 
 *  results. The log record is generated once every PBCH decode attempt. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_DEMBACK_PBCH_DECODING_RESULTS_VERSION 2
#define LTE_LL1_LOG_DEMBACK_HYP_FIXED 12

/*! @brief pbch_decoding_results hyp_fixed struct
 */
typedef struct
{
   uint8 transmit_antenna_config          : 2;  ///< Encoded as:
                                                         ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas
                                                         ///< 0x2: 4 antennas
   uint8 sfn_offset_last_2_bits_of_cell_sfn : 2;  ///< Range: 0 to 7
   uint8 crc_pass_fail                    : 1;  ///< 0: Fail
                                                         ///< 1: Pass
   uint8 reserved0                        : 3; 
   uint32 energy_metric                   : 24;
}lte_LL1_log_demback_pbch_decoding_results_hyp_s;


/*! @brief pbch_decoding_results main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 1 for the log packet structure
                                                         ///< described below
  uint32 serving_cell_id                  : 9;  ///< Range: 0...503
  uint32 sub_frame_number                 : 4;  ///< Sub-frame number (Range 0..9)
  uint32 system_frame_number              : 10;  ///< System frame number (range 0..1023)
  uint32 reserved0                        : 1; 
  uint16 frequency_offset_applied        ;  ///< In units of Hz with 1Hz resolution. Range:
                                                         ///< -15kHz to +15kHz
                                                         ///< 0x0000: -15kHz ... 0x7531: +15kHz
  uint16 reserved1                       ; 
  lte_LL1_log_demback_pbch_decoding_results_hyp_s lte_LL1_log_demback_pbch_decoding_results_hyp[LTE_LL1_LOG_DEMBACK_HYP_FIXED];
  uint32 tx0_rx0_cir[32];
}lte_LL1_log_demback_pbch_decoding_results_ind_struct;



/*==========================================================================*/
/*! @brief
 * 4 LTE LL1 PDCCH Decoding Results (Log code ID: 0xB130)
 *
 *  This log packet gives important parameters related to the PDCCH decoding 
 *  results. The log record is generated once every sub-frame. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_DEMBACK_PDCCH_DECODING_RESULTS_VERSION 21
#define LTE_LL1_LOG_DEMBACK_NUMBER_OF_HYPOTHESIS 44

/*! @brief pdcch_decoding_results number_of_hypothesis struct
 */
typedef struct
{
  uint32 payload[2]                                   ;  ///< Raw payload
  uint32 aggregation_level                : 2;  ///< Number of control channel elements aggregated:
                                                         ///< 0x0: Agg1
                                                         ///< 0x1: Agg2
                                                         ///< 0x2: Agg4
                                                         ///< 0x3: Agg8
  uint32 candidate                        : 3;  ///< Candidate or offset is measured in units of
                                                         ///< aggregation level specified above. Range 0 to
                                                         ///< 7.
  uint32 search_space_type                : 1;  ///< 0: Common
                                                         ///< 1: User
  uint32 dci_format                       : 4;  ///< 0x0: Format 0
                                                         ///< 0x1: Format 1
                                                         ///< 0x2: Format 1A
                                                         ///< 0x3: Format 1B
                                                         ///< 0x4: Format 1C
                                                         ///< 0x5: Format 1D
                                                         ///< 0x6: Format 2
                                                         ///< 0x7: Format 2A
                                                         ///< 0x8: Format 3
                                                         ///< 0x9: Format 3A
                                                         ///< 0xA-0xF: Reserved.
  uint32 decode_status                    : 4;  ///< Indicates which one of 16 RNTI/UL Antenna
                                                         ///< configurations passed
                                                         ///< 0x0:  C_RNTI
                                                         ///< 0x1:  SPS_C_RNTI
                                                         ///< 0x2:  P_RNTI
                                                         ///< 0x3:  RA_RNTI
                                                         ///< 0x4:  T_C_RNTI
                                                         ///< 0x5:  SI_RNTI
                                                         ///< 0x6:  TPC_PUSCH_RNTI
                                                         ///< 0x7:  TPC_PUCCH_RNTI
                                                         ///< 0x8-0xF: Reserved.
  uint32 start_cce                        : 7;  ///< Starting CCE index (range 0..86)
  uint32 payload_size                     : 8;  ///< Payload size (with CRC bits)
  uint32 tail_match                       : 1;  ///< Check if the code block ending state matches
                                                         ///< the starting state
                                                         ///< 0: mismatch
                                                         ///< 1: match
  uint32 reserved0                        : 2; 
  uint32 prune_status                     : 11;  ///< 1: SUCCESS_DCI0,
                                                         ///< 2: SUCCESS_DCI1,
                                                         ///< 3: SUCCESS_DCI1A,
                                                         ///< 4: SUCCESS_DCI1C,
                                                         ///< 5: SUCCESS_DCI1B_1D,
                                                         ///< 6: SUCCESS_DCI2_2A,
                                                         ///< 7: SUCCESS_DCI3_3A,
                                                         ///< 8: TAIL_MISMATCH,
                                                         ///< 9: FAIL_SURVIVOR_SELECT,
                                                         ///< 10: PADDING_ERROR,
                                                         ///< 11: RB_ALLOC_IO_ERROR,
                                                         ///< 12: RB_ALLOC_ZERO_RB_ERROR_TYPE0,
                                                         ///< 13: RB_ALLOC_SET_NUM_ERROR_TYPE1,
                                                         ///< 14: RB_ALLOC_ZERO_RB_ERROR_TYPE1,
                                                         ///< 15: TBS_IO_ERROR,
                                                         ///< 16: MOD_ORDER_IO_ERROR,
                                                         ///< 17: DUPLICATE_HARQ_ID_ERROR,
                                                         ///< 18: UNKNOWN_DCI,
                                                         ///< 19: UNEXPECTED_PAYLOAD_SIZE,
                                                         ///< 20: UNEXPECTED_DCI_FOR_TPC,
                                                         ///< 21: TPC_PUSCH_PRUNE_DCI_3_3A,
                                                         ///< 22: BAD_PARAM_PDCCH_INTERPRET,
                                                         ///< 23: INTERPRET_ERROR,
                                                         ///< 24: BAD_RIV_DCI0,
                                                         ///< 25: RB_ALLOC_ERROR_DCI0,
                                                         ///< 26: INVALID_RB_NUM_DCI0,
                                                         ///< 27: MCS_ERROR_DCI0,
                                                         ///< 28: NDI_ERROR_DCI0,
                                                         ///< 29: CQI_ERROR_DCI0,
                                                         ///< 30: INVALID_RNTI_DCI1,
                                                         ///< 31: NUM_LAYERS_ERROR_DCI1,
                                                         ///< 32: BAD_RIV_DCI1A,
                                                         ///< 33: RB_ALLOC_ERROR_DCI1A,
                                                         ///< 34: INVALID_RB_NUM_DCI1A,
                                                         ///< 35: NUM_LAYERS_ERROR_DCI1A,
                                                         ///< 36: RETURN_DL_DATA_ARRIVAL_DCI1A,
                                                         ///< 37: INVALID_RNTI_DCI1B_1D,
                                                         ///< 38: BAD_RIV_DCI1B_1D,
                                                         ///< 39: RB_ALLOC_ERROR_DCI1B_1D,
                                                         ///< 40: INVALID_RB_NUM_DCI1B_1D,
                                                         ///< 41: NUM_LAYERS_ERROR_DCI1B_1D,
                                                         ///< 42: INVALID_RNTI_DCI1C,
                                                         ///< 43: BAD_RIV_DCI1C,
                                                         ///< 44: RB_ALLOC_ERROR_DCI1C,
                                                         ///< 45: NUM_LAYERS_ERROR_DCI1C,
                                                         ///< 46: INVALID_RNTI_DCI2_2A,
                                                         ///< 47: TB_EN_ERROR_DCI2_2A,
                                                         ///< 48: PMI_ERROR_DCI2_2A,
                                                         ///< 49: SPS_CRNTI_ERROR_DCI2_2A,
                                                         ///< 50: NUM_LAYERS_ERROR_DCI2_2A_TB1,
                                                         ///< 51: NUM_LAYERS_ERROR_DCI2_2A_TB2,
                                                         ///< 52: TBS_IO_ERROR_DCI2_2A_TB1,
                                                         ///< 53: TBS_IO_ERROR_DCI2_2A_TB2,
                                                         ///< 54: TBS_2LAYER_ERROR_DCI2_2A_TB1,
                                                         ///< 55: TBS_2LAYER_ERROR_DCI2_2A_TB2,
                                                         ///< 56: MOD_ORDER_ERROR_DCI2_2A_TB1,
                                                         ///< 57: MOD_ORDER_ERROR_DCI2_2A_TB2,
                                                         ///< 58: INVALID_RNTI_DCI0,
														                             ///< 59: INVALID_DL_GRANT_ON_MBSFN,
                                                         ///< 60: INVALID_HARQ_ID,
                                                         ///< 61: INVALID_DAI,
														                             ///< 62: SPS_C_RNTI_VALIDATION_FAIL
														                             ///< 63: SPS_PRUNE_AFTER_STORING
														                             ///< 64: FAIL_SER_ENGYMTRC_CHECK
                                                         ///< 65: PDCCH_DEBUG_PRUNE_DCI_SSFCFG
                                                         ///< 66: PDCCH_DEBUG_EXCEED_CAT3_TBLK_SIZE_LIMIT

  uint32 reserved1                        : 21; 
  int32 norm_energy_metric                : 32;  ///< Normalized energy metric 32S16,
												 ///< the field is signed and with a 
												 ///< Q scale factor of 16, to get the 
												 ///< actual value, divide this by 2^16  
  uint32 symbol_error_rate                : 32;  ///< symbol_error_rate 32U31, the field 
												 ///< is with a Q scale factor of 31, to 
												 ///< get the actual value, divide this 
												 ///< by 2^31
}lte_LL1_log_demback_pdcch_decoding_results_hypothesis_s;


/*! @brief pdcch_decoding_results main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 3 for the log packet structure
                                                         ///< described below
  uint32 sub_frame_number                 : 4;  ///< Sub-frame number (Range 0..9)
  uint32 system_frame_number              : 10;  ///< System frame number (range 0..1023)
  uint32 reserved0                        : 10; 
  uint32 reserved1                        : 16;
  uint32 demback_mode_sel                 : 4; /// demback mode select 
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint32 number_of_hypothesis             : 8;  ///< Range 0..43
  lte_LL1_log_demback_pdcch_decoding_results_hypothesis_s lte_LL1_log_demback_pdcch_decoding_results_hypothesis[LTE_LL1_LOG_DEMBACK_NUMBER_OF_HYPOTHESIS];
}lte_LL1_log_demback_pdcch_decoding_results_ind_struct;




/*==========================================================================*/
/*! @brief
 * 5 LTE LL1 PDSCH Decoding Results (Log code ID: 0xB132)
 *
 *  This log packet gives important parameters related to the PDSCH decoding 
 *  results. The log record is generated for each dl grant and
 *  log packet is delivered once every 20 sub-frames or every 
 *  100ms, whichever occurs first.
---------------------------------------------------------------------------*/

#define LTE_LL1_LOG_DEMBACK_PDSCH_DECODING_RESULTS_VERSION 24
#define LTE_LL1_LOG_DEMBACK_NUMBER_OF_STREAMS 2
#define LTE_LL1_LOG_DEMBACK_NUMBER_OF_CBLK    13
#define LTE_LL1_LOG_DEMBACK_NUMBER_OF_RECORDS 20
/*! @brief pdsch_decoding_results number_of_cblk struct 
 *  retrive from lte_tdec_cblk_status_word0_t and lte_tdec_cblk_status_word1_t
 */
 
 
typedef struct
{
  uint32 energy_metric                   : 21;
  uint32 iteration_num                   : 4;
  uint32 code_block_crc_pass             : 1;  ///< set if CB crc pass: 
  uint32 early_termination               : 1;  ///< set if decoding is early terminated
  uint32 harq_combine_enable             : 1;
  uint32 deint_decode_bypass             : 1;
  uint32 bitr_timeout                    : 1;  ///< @internal set if bit read times out 
  uint32 llrw_timeout                    : 1;  ///< @internal set if llr write time out
  uint32 llrr_timeout                    : 1;  ///< @internal set if llr read time out
 
}lte_LL1_log_demback_pdsch_decoding_results_cblk_s;

/*! @brief pdsch_decoding_results number_of_streams struct
 */
typedef struct
{
  uint32 transport_block_crc              : 1;  ///< 0: Fail
                                                ///< 1: Pass
  uint32 ndi                              : 1;  ///< NDI bit
  uint32 code_block_size_plus             : 13; ///< Unit: number of bits. Range: 0...8191
  uint32 num_code_block_plus              : 4;  ///< 1  to 13
  uint32 max_tdec_iter                    : 4;  ///< 8,7,6, max_iteration_num
  uint32 retransmission_number            : 3;  ///< 0x0: First transmissions
                                                         ///< 0x1: Second transmission
                                                         ///< ...
                                                         ///< 0x7: 8th transmission
  uint32 rvid                             : 2;  ///< 0: RVID 0 ... 0x3: RVID 3
  uint32 companding_stats                 : 2;  ///< 0x0: 3 bit LLR (soft bits)
                                                         ///< 0x1: 4 bit LLR
                                                         ///< 0x2: 6 bit LLR
  uint32 harq_combining                   : 1;  ///< 0: Harq not combine
                                                         ///< 1: Harq combine
  uint32 decob_tb_crc                     : 1;  ///< tb_crc_readout from HW Decob TB Status

  uint32 effective_code_rate              : 10;  ///< coderate x 1024.
                                                         ///< Range: 0x0000 = 0.0...0x3E8: 1.0
  uint32 num_re                           : 16;  
  uint32 bitw_timeout                     : 1;   ///< @internal set if bit write times out for the TB
  uint32 codeword_idx                     : 1; 
  uint32 last_pass_cb_idx                 : 4;

  lte_LL1_log_demback_pdsch_decoding_results_cblk_s lte_LL1_log_demback_pdsch_decoding_results_cblk_records[LTE_LL1_LOG_DEMBACK_NUMBER_OF_CBLK];
}lte_LL1_log_demback_pdsch_decoding_results_streams_s;


/*! @brief pdsch_decoding_results number_of_records struct
 */
typedef struct
{
  uint32 sub_frame_offset                 : 16;  ///< Offset relative to the starting SFN
  uint32 rnti                             : 16;  ///< Describes the RNTI

  uint32 harq_id                          : 4;  ///< 0 to 15
  uint32 rnti_type                        : 4;  ///< 0 - C_RNTI , 1 - SPS_C_RNTI, 2 - P_RNTI, 3 - RA_RNTI, 4 - T_C_RNTI, 5- SI_RNTI, 
                                                 ///< 6 - TPC_PUSCH_RNTI, 7 - TPC_PUCCH_RNTI, 8 -MBMS_RNTI
  uint32 system_information_msg_number    : 4;  ///< Only applicable for PDSCH carrying SIB
                                                ///< Range: 1 ~ 10
  uint32 system_information_mask          : 12; ///< Only applicable for PDSCH carrying SIB

  uint32 reserved4                        : 3;
  uint32 harq_log_status                  : 2;  ///< 0-Normal, 1-Reset, 2 - Overwritten
  uint32 codeword_swap                    : 1;
  uint32 number_of_streams                : 2;  ///< 1: 1 stream
                                                         ///< 2: 2 streams
  lte_LL1_log_demback_pdsch_decoding_results_streams_s lte_LL1_log_demback_pdsch_decoding_results_streams[LTE_LL1_LOG_DEMBACK_NUMBER_OF_STREAMS];
}lte_LL1_log_demback_pdsch_decoding_results_records_s;


/*! @brief pdsch_decoding_results main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 1 for the log packet structure
                                                         ///< described below
  uint32 serving_cell_id                  : 9;  ///< Range: 0...503. 
  uint32 starting_sub_frame_number        : 4;  ///< Sub-frame number (Range 0..9)
  uint32 starting_system_frame_number     : 10; ///< System frame number (range 0..1023)
  uint32 log_buf_activated_flag           : 1;  ///< @internal Whether log buf is used for logging or not

  uint32 ue_category                      : 4;
  uint32 num_dl_harq                      : 4;
  uint32 num_dl_harq_stored               : 4;  ///< @internal num_of_DL_harq emem is holding
  uint32 tm_mode                          : 4;

  uint32 reserved1                        : 7;  
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint32 number_of_records                : 5;  ///< Range: 0..20
  lte_LL1_log_demback_pdsch_decoding_results_records_s lte_LL1_log_demback_pdsch_decoding_results_records[LTE_LL1_LOG_DEMBACK_NUMBER_OF_RECORDS];
}lte_LL1_log_demback_pdsch_decoding_results_ind_struct;




/*==========================================================================*/
/*! @brief
 * 5 LTE LL1 demback PDSCH debug (Log code ID: 0xB12F) - Internal
 *
 *  This log packet gives important parameters related to the demback PDSCH debug information.
 *  The log is generated every subframe 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_DEMBACK_PDSCH_DEBUG_VERSION 22

typedef struct
{ 
  //Q6
  uint32 q6_ccs_cmd;  ///< q6 issue CCS Cmd ustmr
  uint32 g2_sync_point; ///< g2 sync point ustmr

  //from CCS
  uint32 ccs_first_dispatch ;  ///< ccs first cb dispatch ustmr
  uint32 ccs_q6_time ;   ///< ccs subframe done ustmr

  //from Q6
  uint32 ccs_q6_rsp_time;  ///< q6 result processing ustmr

  uint8  total_num_cb;     ///< total num of cb per subframe
  uint8  cmd_rsp_cnt_diff;   ///< q6 ccs cmd and response counter difference
  uint16 num_mismatch_tb_enable_and_tb_crc_valid; ///<num_mismatch of tb_enable in ccs cmd and tb valid in ccs response

  //derived
  uint32 g2_and_q6_ccs_cmd_time_diff_in_us; ///< (g2_sync_point - q6_ccs_cmd)/19200
  uint32 first_dispatch_and_g2_time_diff_in_us; ///< (ccs_first_dispatch-g2_sync_point)/19200
  uint32 ccs_proc_time_in_us;     ///< (ccs_q6_time-ccs_first_dispatch)/19200
  uint32 q6_rsp_delay_in_us;     ///< (ccs_q6_rsp_time - ccs_q6_time)/19200


  uint32 debug;  

}lte_LL1_log_demback_pdsch_debug_s;


/*! @brief lte_LL1_log_demback_pdsch_debug_struct main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                ///< Version = 1 for the log packet structure
                                                ///< described below
  uint32 sub_frame_number                 : 4;  ///< Sub-frame number (Range 0..9)
  uint32 system_frame_number              : 10; ///< System frame number (range 0..1023)
  uint32 reserved0                        : 10; 
  lte_LL1_log_demback_pdsch_debug_s       pdsch_debug;
}lte_LL1_log_demback_pdsch_debug_struct;


#endif //LTE_LL1_LOG_DEMBACK_H
