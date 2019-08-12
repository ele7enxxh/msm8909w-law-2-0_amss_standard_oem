/*!
  @file
  lte_LL1_log_cqi.h

  @brief
  cqi log packet data structures 

  @detail
  !!! Please do not modify !!!
  This file was auto-generated on Wed Oct 21 13:01:18 2009.
  Command line used: perl w2h.pl log_cqi cqi 0 0 0 0 0 0
   Note: The params, [0 0 0 0 0 0], correspond to version numbers for
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_csf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef LTE_LL1_LOG_CQI_H
#define LTE_LL1_LOG_CQI_H


/*==========================================================================*/
/*! @brief
 * 1 CSF Spectral Efficiency Log for TxMode 4, 5, 6 Rank 1 (Log code ID:
 *
 *  0xB149) 
 *  This log packet gives important parameters related to the subband (SB) 
 *  spectral efficiency table updates during the above mentioned transmission 
 *  modes and during Rank 2 of TxMode 3. The log record is generated every sub- 
 *  frame and will be used during CSF report generation. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_CSF_SPECTRAL_EFFICIENCY_FOR_TXMODE_4_5_6_RANK_1_VERSION 22

/*! @brief csf_spectral_efficiency_log_for_txmode_4_5_6_rank_1 main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Range: 0..255
  uint32 start_system_sub_frame_number_sfn : 4; ///< Range 0..9
  uint32 start_system_frame_number        : 10;  ///< Range 0..1023
  uint32 reserved0                        : 3;  
  uint32 num_whiten_matrices_for_csf      : 7;  ///< Range: 0..109
  uint32 cqi_step_size                    : 2;  ///< Range: 0..3
  uint32 number_of_tx_antennas            : 2;  ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas
                                                         ///< 0x2: 4 antennas
  uint32 number_of_rx_antennas            : 1;  ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas
  uint32 number_of_subbands               : 5;  ///< Range: 1..28
  uint32 precode_blk_idx_4tx              : 1;  ///< Range: 0 ..1
                                                         ///< 0 : PMIs from 0 to 7
                                                         ///< 1:  PMIs from 8 to 15
  uint32 tx_mode                      : 4;  ///< Range: 1..9
                                                         ///< 0: TM_INVALID
                                                         ///< 1: TM_SINGLE_ANT_PORT0
                                                         ///< 2: TM_TD_RANK1
                                                         ///< 3: TM_OL_SM
                                                         ///< 4: TM_CL_SM
                                                         ///< 5: TM_MU_MIMO
                                                         ///< 6: TM_CL_RANK1_PC
                                                         ///< 7: TM_SINGLE_ANT_PORT5
                                                         ///< 8: TM8
                                                         ///< 9: TM9
  uint32 reserved1                        : 13; 
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint16 spec_eff_r1_txmode456_and_r2_txmode3[200]    ;  ///< (CSF requirement)
                                                         ///< 2Tx:
                                                         ///< 25(SBs) x 4(PMIs) = 100 x 2(16bits) = 200 bytes
                                                         ///< 4Tx, 2Rx:
                                                         ///< 25(SBs) x 8(PMIs) = 200 x 2(16bits) = 400 bytes
                                                         ///< Max(200, 400) = 400bytes
                                                         ///< Note: for Rank 1 case
                                                         ///< Max num of PMIs for 2 Tx = 4 (out of 4)
                                                         ///< Max num of PMIs for 4 Tx = 16 (but only 8 are
                                                         ///< viewed each Subframe)
}lte_LL1_log_csf_spectral_efficiency_for_txmode_4_5_6_rank_1_ind_struct;



/*==========================================================================*/
/*! @brief
 * 2 CSF Spectral Efficiency Log for TxMode 4 Rank 2 (Log code ID: 0xB14a)
 *
 *  This log packet gives important parameters related to the subband (SB) 
 *  spectral efficiency table updates during the above mentioned transmission 
 *  modes. The log record is generated every sub-frame and will be used during 
 *  CSF report generation. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_CSF_SPECTRAL_EFFICIENCY_FOR_TXMODE_4_RANK_2_VERSION 22

/*! @brief csf_spectral_efficiency_log_for_txmode_4_rank_2 main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Range: 0..255
  uint32 start_system_sub_frame_number_sfn : 4; ///< Range 0..9
  uint32 start_system_frame_number        : 10;  ///< Range 0..1023
  uint32 reserved0                         : 3;  
  uint32 num_whiten_matrices_for_csf      : 7;  ///< Range: 0..109
  uint32 cqi_step_size                    : 2;  ///< {0, 1, 2, 3}
  uint32 number_of_tx_antennas            : 2;  ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas
                                                         ///< 0x2: 4 antennas
  uint32 number_of_rx_antennas            : 1;  ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas
  uint32 number_of_subbands               : 5;  ///< Range: 0..27
  uint32 precode_blk_idx_4tx              : 1;  ///< Range: 0 ..1
                                                         ///< 0 : PMIs from 0 to 7
                                                         ///< 1:  PMIs from 8 to 15
  uint32 tx_mode                      : 4;  ///< Range: 1..9
                                                         ///< 0: TM_INVALID
                                                         ///< 1: TM_SINGLE_ANT_PORT0
                                                         ///< 2: TM_TD_RANK1
                                                         ///< 3: TM_OL_SM
                                                         ///< 4: TM_CL_SM
                                                         ///< 5: TM_MU_MIMO
                                                         ///< 6: TM_CL_RANK1_PC
                                                         ///< 7: TM_SINGLE_ANT_PORT5
                                                         ///< 8: TM8
                                                         ///< 9: TM9
  uint32 reserved1                        : 13; 
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint16 spec_eff_r2_txmode4[400]                     ;  ///< 2Tx:
                                                         ///< 25(SBs) x 2(layers) x 2(PMIs) = 100 x 2(16bits) = 200
                                                         ///< bytes
                                                         ///< 4Tx, 2Rx:
                                                         ///< 25(SBs) x 2(layers) x 8(PMIs) = 400 x 2(16bits) = 800
                                                         ///< bytes
                                                         ///< Max(200, 800) = 800 bytes
                                                         ///< Note: for Rank 2 case
                                                         ///< Max num of PMIs for 2 Tx & 1 Tx = 2
                                                         ///< Max num of PMIs for 4 Tx = 16 (but only 8 are
                                                         ///< viewed each Subframe)
}lte_LL1_log_csf_spectral_efficiency_for_txmode_4_rank_2_ind_struct;



/*==========================================================================*/
/*! @brief
 * 3 CSF Spectral Efficiency Log for TxMode 1, 2, 7 (Log code ID: 0xB14b)
 *
 *  This log packet gives important parameters related to the subband (SB) 
 *  spectral efficiency table updates during the above mentioned transmission 
 *  modes and during Rank 1 of TxMode 3. The log record is generated every sub- 
 *  frame and will be used during CSF report generation. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_CSF_SPECTRAL_EFFICIENCY_FOR_TXMODE_1_2_7_VERSION 22

/*! @brief csf_spectral_efficiency_log_for_txmode_1_2_7 main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Range: 0..255
  uint32 start_system_sub_frame_number_sfn : 4; ///< Range 0..9
  uint32 start_system_frame_number        : 10;  ///< Range 0..1023
  uint32 reserved                          : 3;  
  uint32 num_whiten_matrices_for_csf      : 7;  ///< Range: 0..109
  uint32 cqi_step_size                    : 2;  ///< {0, 1, 2, 3}
  uint32 number_of_tx_antennas            : 2;  ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas
                                                         ///< 0x2: 4 antennas
  uint32 number_of_rx_antennas            : 1;  ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas
  uint32 number_of_subbands               : 5;  ///< Range: 0..27


  uint32 tx_mode                      : 4;  ///< Range: 1..9
                                                         ///< 0: TM_INVALID
                                                         ///< 1: TM_SINGLE_ANT_PORT0
                                                         ///< 2: TM_TD_RANK1
                                                         ///< 3: TM_OL_SM
                                                         ///< 4: TM_CL_SM
                                                         ///< 5: TM_MU_MIMO
                                                         ///< 6: TM_CL_RANK1_PC
                                                         ///< 7: TM_SINGLE_ANT_PORT5
                                                         ///< 8: TM8
                                                         ///< 9: TM9


  uint32 reserved0                        : 14; 
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint16 spec_eff_txmode127_and_r1_txmode3[28]        ;  ///< (CSF requirement)
                                                         ///< (16 bits per RB) x
                                                         ///< (Max_num_of_SubBands )
                                                         ///< Max_num_of_SubBands_4Tx_2Rx = 19
                                                         ///< Max_num_of_SubBands = 28 all other times
}lte_LL1_log_csf_spectral_efficiency_for_txmode_1_2_7_ind_struct;



/*==========================================================================*/
/*! @brief
 * 4 Whitened matrices log (Log code ID: 0xB14c)
 *
 *  This log packet gives important parameters related to the whitened channel 
 *  matrices. The log record is generated every sub-frame during LTE_CQI task.
 *  Note: This log packet is no longer available for DIME.
 *  Has submitted a CR to VPE team[ClearQuest] QCTDD00340712.
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_CSF_WHITENED_MATRICES_LOG_VERSION 21
#define LTE_LL1_MAX_NUM_WHITENING_SAMPLES 440
/*! @brief whitened_matrices_log main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Range: 0..255
  uint32 start_system_sub_frame_number_sfn : 4; ///< Range 0..9
  uint32 start_system_frame_number        : 10;  ///< Range 0..1023
  uint32 num_whiten_matrices_for_csf      : 7;  ///< Range: 0..109
  uint32 number_of_tx_antennas            : 2;  ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas
                                                         ///< 0x2: 4 antennas
  uint32 number_of_rx_antennas            : 1;  ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas
  uint32 cqi_step_size                    : 2;  ///< {0, 1, 2, 3}. pls read 54.3.15 SWI
                                                         /// < 0: N_2, 1:N_4
                                                         /// < 2: N_8, 3:N_16
  uint32 reserved0                        : 26; 
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint32 whitened_channel_matrices[LTE_LL1_MAX_NUM_WHITENING_SAMPLES];  ///< 32 bits per sample ( I & Q) x
                                                                        ///< (max_num_samples_RB = 8) x
                                                                        ///< (num_RBs_4Tx_2Rx = 55)
                                                                        ///< Max_num_RBs for 4 Tx, 2Rx = 55
                                                                        ///< For every other config, it is 110
}lte_LL1_log_csf_whitened_matrices_ind_struct;



/*==========================================================================*/
/*! @brief
 * 5 PUCCH CSF Log (Log code ID: 0xB14d)
 *
 *  This log packet gives important parameters related to the CSF log results 
 *  in PUCCH periodic reporting mode. The log record is generated on sub-frames 
 *  receiving CSF reporting request in periodic reporting mode from ML. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_CSF_PUCCH_VERSION 24   //23

/*! @brief pucch_csf_log main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Range: 0...255
  uint32 start_system_sub_frame_number_sfn : 4; ///< Range 0..9
  uint32 start_system_frame_number        : 10;  ///< Range 0..1023
  //uint32 reserved0                        : 3;   
  //uint32 pucch_reporting_mode             : 2;  ///< Range: 0..3
                                                         ///< 0: MODE_1_0
                                                         ///< 1: MODE_1_1
                                                         ///< 2: MODE 2_0
                                                         ///< 3: MODE 2_1
  //uint32 pucch_report_type                : 2;  ///< 0x0: type 1, Subband CQI feedback
                                                         ///< 0x1: type 2, Wideband CQI, PMI feedback
                                                         ///< 0x2: type 3, RI feedback
                                                         ///< 0x3: type 4, Wideband CQI feedback

  uint32 reserved0                        : 1;   
  uint32 pucch_reporting_mode             : 2;  ///< Range: 0..3
                                                         ///< 0: MODE_1_0
                                                         ///< 1: MODE_1_1
                                                         ///< 2: MODE 2_0
                                                         ///< 3: MODE 2_1
  uint32 pucch_report_type                : 4;  ///< Range: 0..9
                                                ///< 0x0: TYPE_1A_SBCQI_PMI2		
						///< 0x1: TYPE_1_SBCQI			
						///< 0x2: TYPE_2_WBCQI_PMI			
						///< 0x3: TYPE_3_RI				
						///< 0x4: TYPE_4_WBCQI			
						///< 0x5: TYPE_5_RI_WBPMI1			
						///< 0x6: TYPE_6_RI_PTI			
						///< 0x7: TYPE_2A_WBPMI			
						///< 0x8: TYPE_2B_WBCQI_WBPMI1_WBCQI2	
						///< 0x9: TYPE_2C_WBCQI_WBPMI1_WBPMI2_WBCQI2

  uint32 size_bwp                         : 3;  ///< Range: 0...4.
                                                         ///< This field is valid only when pucch_report_type = type 1  
  uint32 number_of_subbands               : 4;  ///< Range: 1..14
  uint32 bwp_index                        : 3;  ///< Range: 0...4
                                                         /// This field is valid only when pucch_report_type = type 1  
  uint32 rank_index                       : 1;  ///< Range: 0...1
                                                         ///< This field is valid only when pucch_report_type = type 3 
  uint32 subband_label                    : 2;  ///< Range: 0..3
                                                         ///< This field is valid only when pucch_report_type = type 1
  uint32 cqi_cw0                          : 4;  ///< Range: 0..15
                                                         ///< Wideband CQI for CW0 in type 2 & type 4 reporting
                                                         ///< Subband  CQI for CW0 in type 1 reporting
                                                         ///< This field is valid only when pucch_report_type = type 1 or type 2 or type4
  uint32 cqi_cw1                          : 4;  ///< Range: 0...15
                                                         ///< Wideband CQI for CW1 in type 2
                                                         ///< Subband  CQI for CW1 in type 1
                                                         ///< This field is valid when pucch_report_type = type 1 and pucch_reporting mode = 3 (Mode 2_1) and
                                                         ///< This field is valid when pucch_report_type = type 2 and pucch_reporting mode = 1 (Mode 1_1)
  uint32 wideband_pmi                     : 4;  ///< Range: 0...15 for 4 Tx, Rank 1 & 2
                                                         ///< Range: 0...1 for 2 Tx, Rank 2
                                                         ///< Range: 0...3 for 1 Tx, Rank 1
                                                         ///< This field is valid only when pucch_report_type = type 2
  
  
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint32 csf_tx_mode                      : 4;  ///< Range: 1..9
                                                         ///< 0: TM_INVALID
                                                         ///< 1: TM_SINGLE_ANT_PORT0
                                                         ///< 2: TM_TD_RANK1
                                                         ///< 3: TM_OL_SM
                                                         ///< 4: TM_CL_SM
                                                         ///< 5: TM_MU_MIMO
                                                         ///< 6: TM_CL_RANK1_PC
                                                         ///< 7: TM_SINGLE_ANT_PORT5
                                                         ///< 8: TM8
                                                         ///< 9: TM9

  uint32 pucch_reporting_submode         :2; ///< Range: 0..2
                                             ///< Invalid submode
                                             ///< submode 1
                                             ///< submode 2

  uint32 num_csirs_ports                 :4;  ///< Range: 1...8
                                              
  uint32 wideband_pmi_1                  :4; ///< Range: 0..15
                                             /// range 0-15, valid when num_csirs_ports=8

  uint32 pti                             :1;  ///< Range:0..1
                                              /// range 0..1, valid when num_csirs_ports=8 and pucch_report_type=TYPE_6_RI_PTI

}lte_LL1_log_csf_pucch_report_ind_struct;



/*==========================================================================*/
/*! @brief
 * 6 PUSCH CSF Log (Log code ID: 0xB14e)
 *
 *  This log packet gives important parameters related to the CSF log results 
 *  in PUSCH aperiodic reporting mode. The log record is generated on sub- 
 *  frames receiving CSF reporting request in aperiodic PUSCH  reporting mode 
 *  from ML. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_CSF_PUSCH_VERSION 23   //22

/*! @brief pusch_csf_log main struct
 */
typedef struct
{
  uint32 version                           : 8;  ///< Range: 0...255
  uint32 start_system_sub_frame_number_sfn : 4;  ///< Range 0..9
  uint32 start_system_frame_number         : 10;  ///< Range 0..1023
  //uint32 reserved0                         : 3;  
  uint32 pusch_reporting_mode             : 3;   ///< Range: 0..4
                                                          ///< 0: MODE_APERIODIC_RM12
                                                          ///< 1: MODE_APERIODIC_RM20
                                                          ///< 2: MODE_APERIODIC_RM22
                                                          ///< 3: MODE_APERIODIC_RM30
                                                          ///< 4: MODE_APERIODIC_RM31
  uint32 rank_index                       : 1;   ///< 0: Rank 1
                                                          ///< 1: Rank 2
  //uint32 reserved1                        : 3; 
  uint32 reserved1                        : 2;   
  uint32 wideband_pmi_1                  :4; ///< Range: 0..15
                                             /// range 0-15, valid when num_csirs_ports=8  uint32 number_of_subbands 
                                             ///end of first word

  uint32 number_of_subbands               : 5;   ///< Range: 1..28
  uint32 wideband_cqi_cw0                 : 4;   ///< Range: 0...15
                                                          ///< This field is valid for all pusch_reporting_modes
  uint32 wideband_cqi_cw1                 : 4;   ///< Range: 0...15
                                                          ///< This field is valid only when pusch_reporting_mode = 0 or 2 or 4 
                                                          ///< i.e, in reporting modes 1-2, 2-2 & 3-1
  uint32 subband_size_k                   : 4;   ///< [4, 4, 6, 8]  for reporting modes conforming to
                                                          ///< Table 7.2.1-3 of 36.213
                                                          ///< [2,2,3,4 ] for reporiting modes conforming to
                                                          ///< Table 7.2.1-5 of 36.213
  uint32 size_m                           : 3;   ///< [1,3,5,6]
                                                          ///< Table 7.2.1-5 of 36.213
                                                          ///< This field is valid only when pusch_reporting_mode = 1 or 2
                                                          ///< i.e, in reporting modes 2-0 and 2-2
  uint32 single_wb_pmi                    : 4;   ///< Range: 0...15 for 4 Tx, Rank1  & Rank2
                                                          ///< Range: 0...1 for 2 Tx, Rank2
                                                          ///< Range: 0...3 for 2 Tx, Rank1
                                                          ///< This field is valid only when pusch_reporting_mode = 2 or 4
                                                          ///< i.e, in reporting modes 2-2 & 3-1
  uint32 single_mb_pmi                    : 4;   ///< Range: 0...15 for 4 Tx, Rank1  & Rank2
                                                          ///< Range: 0...1 for 2 Tx, Rank2
                                                          ///< Range: 0...3 for 2 Tx, Rank1
                                                          ///< This field is valid only when pusch_reporting_mode = 2
                                                          ///< i.e, in reporting mode 2-2 
  uint32 csf_tx_mode                       : 4;  ///< Range: 0..9
                                                          ///< 0: TM_INVALID
                                                          ///< 1: TM_SINGLE_ANT_PORT0
                                                          ///< 2: TM_TD_RANK1
                                                          ///< 3: TM_OL_SM
                                                          ///< 4: TM_CL_SM
                                                          ///< 5: TM_MU_MIMO
                                                          ///< 6: TM_CL_RANK1_PC
                                                          ///< 7: TM_SINGLE_ANT_PORT5
                                                          ///< 8: TM8
                                                          ///< 9: TM9, end of second word


  uint32 subband_cqi_cw0_sb0              : 4;    ///< (4 bits per SubBand  x Max_num_SubBands)
                                                          ///< Max_num_SubBands = 14 for 2Tx
                                                          ///< Max_num_SubBands = 10 for 4Tx
                                                          ///< OR
                                                          ///< M_SubBand_CQI_CW0 in reporting modes 2-0 &  2-2
                                                          ///< Range: 0...15
                                                          ///< This field is valid only when pusch_reporting_mode = 1 or 2 or 3 or 4
                                                          ///< i.e, in reporting modes 2-0, 2-2, 3-0 & 3-1
  uint32 subband_cqi_cw0_sb1                  : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb2                  : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb3                  : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb4                  : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb5                  : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb6                  : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb7                  : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb8                  : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb9                  : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb10                 : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb11                 : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb12                 : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
  uint32 subband_cqi_cw0_sb13                 : 4;      ///< This field is valid only when pusch_reporting_mode = 3 or 4
    
  uint32 subband_cqi_cw1_sb0              : 4;       ///< (4 bits per SubBand  x Max_num_SubBands)
                                                             ///< Max_num_SubBands = 14 for 2Tx
                                                             ///< Max_num_SubBands = 10 for 4Tx
                                                             ///< OR
                                                             ///< M_SubBand_differential_CQI_CW1 in reporting mode 2-2
                                                             ///< Range: 0...15
                                                             ///< This field is valid only when pusch_reporting_mode = 2 or 4
                                                             ///< i.e, in reporting modes 2-2 & 3-1
  uint32 subband_cqi_cw1_sb1              : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb2              : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb3              : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb4              : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb5              : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb6              : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb7              : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb8              : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb9              : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb10             : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb11             : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb12             : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 subband_cqi_cw1_sb13             : 4;  ///< This field is valid only when pusch_reporting_mode = 4 
  uint32 reserved3                        : 16; 
  
  uint32 position_of_m_select_subbands_sb0     : 5;  ///< num_SubBands (max  = 28), 1st best subband among M
                                                             ///< This field is valid only when pusch_reporting_mode = 1 or 2
                                                             ///< i.e, in reporting modes 2-0 & 2-2
  uint32 position_of_m_select_subbands_sb1     : 5;  /// 2nd best subband among M
                                                             ///< This field is valid only when pusch_reporting_mode = 1 or 2 
                                                             ///< i.e, in reporting modes 2-0 & 2-2                           
  uint32 position_of_m_select_subbands_sb2     : 5;  /// 3rd best subband among M
                                                             ///< This field is valid only when pusch_reporting_mode = 1 or 2 
                                                             ///< i.e, in reporting modes 2-0 & 2-2                            
  uint32 position_of_m_select_subbands_sb3     : 5;  /// 4th best subband among M
                                                             ///< This field is valid only when pusch_reporting_mode = 1 or 2  
                                                             ///< i.e, in reporting modes 2-0 & 2-2                            
  uint32 position_of_m_select_subbands_sb4     : 5;  /// 5th best subband among M
                                                             ///< This field is valid only when pusch_reporting_mode = 1 or 2  
                                                             ///< i.e, in reporting modes 2-0 & 2-2                            
  uint32 position_of_m_select_subbands_sb5     : 5;  /// 6th best subband among M
                                                             ///< This field is valid only when pusch_reporting_mode = 1 or 2  
                                                             ///< i.e, in reporting modes 2-0 & 2-2                            
  uint32 reserved4                             : 2;
  uint32 multi_sb_pmi_sb0                      : 4;  ///< = max( 40, 14, 28)  bits total
                                                             ///< ( 4 pmi bits per SubBand x max_num_SubBands =
                                                             ///< 10) for 4 Tx
                                                             ///< (1 pmi bit per SubBand x max_num_SubBands = 14)
                                                             ///< for 1/2 Tx, Rank 2
                                                             ///< (2 pmi bits per SubBand x max_num_SubBands =
                                                             ///< 14) for 1/2 Tx, Rank 1
                                                             ///< This field is valid only when pusch_reporting_mode = 0  
                                                             ///< i.e, in reporting modes 1-2                            
  uint32 multi_sb_pmi_sb1                      : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb2                      : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb3                      : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb4                      : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb5                      : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb6                      : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb7                      : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb8                      : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb9                      : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb10                     : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb11                     : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb12                     : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 multi_sb_pmi_sb13                     : 4;  ///< This field is valid only when pusch_reporting_mode = 0
  uint32 carrier_index                         : 4; ///< 0-PCC, 1-SCC 
  //uint32 reserved5                             : 4;  
  uint32 num_csirs_ports                       : 4;  ///< Range: 1...8
}lte_LL1_log_csf_pusch_report_ind_struct;

#define LTE_LL1_CSF_MAX_NUM_LAYER                2
#define LTE_LL1_CSF_MAX_NUM_SUBBAND_TM9_LOG     18
#define LTE_LL1_CSF_MAX_NUM_PMI_TM9             16
#define LTE_LL1_CSF_CSIRS_SNE_MAX_NUM_TAPS_SC  128 ///< Maximum number of CSIRS SNE taps for single carrier

typedef struct
{
   uint32 rank1_dft_idx                                     : 5; ///< DFT
   uint32 rank1_cophase_idx                                 : 2; ///< Co-phase code book
   uint16 rank1_se[LTE_LL1_CSF_MAX_NUM_SUBBAND_TM9_LOG]        ; ///< #SB+1
} lte_LL1_rank1_precode_for_csf_se_tm9;

typedef struct
{
   uint32 rank2_dft_idx1                                                            : 5; ///< DFT for layer1
   uint32 rank2_dft_idx2                                                            : 5; ///< DFT for layer2
   uint32 rank2_cophase_idx                                                         : 1; ///< Co-phase code book
   uint16 rank2_se[LTE_LL1_CSF_MAX_NUM_LAYER][LTE_LL1_CSF_MAX_NUM_SUBBAND_TM9_LOG]     ; ///< #SB+1
} lte_LL1_rank2_precode_for_csf_se_tm9;

typedef struct
{
   uint16 w1_se_metric[LTE_LL1_CSF_MAX_NUM_LAYER*LTE_LL1_CSF_MAX_NUM_PMI_TM9]          ; ///< 16*2, 16: CSF_MAX_PMI, 2: CSF_MAX_LAYER
   uint32 w1_sel_idx[LTE_LL1_CSF_MAX_NUM_LAYER]                                        ; ///< Take bitmask form result buffer and store the location for "1"
} lte_LL1_w1_selection_for_csf_se_tm9;

#define LTE_LL1_LOG_CSF_SPECTRAL_EFFICIENCY_FOR_TXMODE_9_VERSION 22

typedef struct
{
  //WORD0
  uint32 version                          : 8;  ///< Range: 0..255
  uint32 sub_frame_num                    : 4;  ///< Range 0..9
  uint32 frame_num                        :10;  ///< Range 0..1023
  uint32 cqi_step_size                    : 2;  ///< {0, 1, 2, 3}. pls read 54.3.15 SWI
                                                         /// < 0: N_2, 1:N_4
                                                         /// < 2: N_8, 3:N_16
  uint32 num_whiten_matrices_for_csf      : 7;  ///< Range: 0..109
  uint32 reserved0                        : 1;

  //WORD1
  uint32 number_of_tx_antennas            : 2;  ///< 0x0: 1 antenna
                                                ///< 0x1: 2 antennas
                                                ///< 0x2: 4 antennas
  uint32 number_of_rx_antennas            : 1;  ///< 0x0: 1 antenna
                                                ///< 0x1: 2 antennas
  uint32 number_of_subbands               : 5;  ///< Range: 0..27
  uint32 tx_mode                          : 4;  ///< Range: 1..9
                                                         ///< 0: TM_INVALID
                                                         ///< 1: TM_SINGLE_ANT_PORT0
                                                         ///< 2: TM_TD_RANK1
                                                         ///< 3: TM_OL_SM
                                                         ///< 4: TM_CL_SM
                                                         ///< 5: TM_MU_MIMO
                                                         ///< 6: TM_CL_RANK1_PC
                                                         ///< 7: TM_SINGLE_ANT_PORT5
                                                         ///< 8: TM8
                                                         ///< 9: TM9
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint32 reserved1                        : 16;

  // Addtional TM9 logging parameters
  lte_LL1_rank1_precode_for_csf_se_tm9 rank1_precode[LTE_LL1_CSF_MAX_NUM_PMI_TM9];

  lte_LL1_rank2_precode_for_csf_se_tm9 rank2_precode[LTE_LL1_CSF_MAX_NUM_PMI_TM9];

  lte_LL1_w1_selection_for_csf_se_tm9 w1_selection;

}lte_LL1_log_csf_spectral_efficiency_for_txmode_9_ind_struct;

#define CSF_CSIRS_MAX_NUM_RX 2
#define CSF_CSIRS_MAX_NUM_TX 4
#define LTE_LL1_LOG_CSF_CSIRS_CHEST_FOR_TXMODE_9_VERSION 22

typedef struct
{
   // WORD0
  uint32 version                          : 8;  ///< Range: 0..255
  uint32 sub_frame_num                    : 4;  ///< Range 0..9
  uint32 frame_num                        :10;  ///< Range 0..1023
  uint32 reserved0                        :10;
                                                
  // WORD1    
  uint32 num_csirs_port                   : 4;  ///< {1, 2, 4, 8} 
                                                ///                                             
  uint32 number_of_tx_antennas            : 2;  ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas
                                                         ///< 0x2: 4 antennas
  uint32 number_of_rx_antennas            : 1;  ///< 0x0: 1 antenna
                                                         ///< 0x1: 2 antennas

  uint32 tx_mode                          : 4;  ///< Range: 1..9
                                                         ///< 0: TM_INVALID
                                                         ///< 1: TM_SINGLE_ANT_PORT0
                                                         ///< 2: TM_TD_RANK1
                                                         ///< 3: TM_OL_SM
                                                         ///< 4: TM_CL_SM
                                                         ///< 5: TM_MU_MIMO
                                                         ///< 6: TM_CL_RANK1_PC
                                                         ///< 7: TM_SINGLE_ANT_PORT5
                                                         ///< 8: TM8
                                                         ///< 9: TM9

  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 

  uint32 reserved1                        :17;

  // Addtional TM9 logging parameters
  uint32 noise_level;                                           ///< Reserved

  uint32 csirs_noise_est[CSF_CSIRS_MAX_NUM_RX][CSF_CSIRS_MAX_NUM_TX];              ///< CSIRS Noise in FW, one for each rx_tx pair; 2 RX, 4 TX for CSIRS

  uint32 csirs_sne[CSF_CSIRS_MAX_NUM_RX][LTE_LL1_CSF_CSIRS_SNE_MAX_NUM_TAPS_SC];   ///< Per RX; SC: max 128 taps per RX; CA: max 64 taps per RX 
                                                                                   ///< Read in 32S format from SNE buffer
}lte_LL1_log_csf_csirs_chest_for_txmode_9_ind_struct;

#endif //LTE_LL1_LOG_CQI_H
