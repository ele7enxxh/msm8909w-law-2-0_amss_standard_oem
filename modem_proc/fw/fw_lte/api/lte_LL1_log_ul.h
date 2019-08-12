/*!
  @file
  lte_LL1_log_ul.h

  @brief
  ul log packet data structures

  @detail
  !!! Please do not modify !!!
  This file was auto-generated on Fri May 14 11:46:16 2010.
  Command line used: perl w2h.pl log_ul ul 2 2 2 2 1 1
   Note: The params, [2 2 2 2 1 1], correspond to version numbers for
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_ul.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef LTE_LL1_LOG_UL_H
#define LTE_LL1_LOG_UL_H


/*==========================================================================*/
/*! @brief
 * 1 LTE LL1 PUSCH Tx Report (Log code ID: 0xB139)
 *
 *  This log packet gives important parameters related to the PUSCH transmit 
 *  report. The log record is generated once every 20 sub-frames or every 
 *  100ms, whichever occurs first. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_UL_PUSCH_TX_REPORT_VERSION 25
#define LTE_LL1_LOG_UL_NUMBER_OF_PUSCH_RECORDS_FIXED 20

/*! @brief pusch_tx_report number_of_pusch_records_fixed struct
 */
typedef struct
{
  //word 1
  uint16 current_sfn_sf                  ;  ///< System / Sub Frame Number, as
                                                         ///< 10*SFN + SF
  uint16 coding_rate                      ;  ///< PUSCH coding rate in Q10 (0,1)

  //word 2
  uint32 ul_control_mux_bitmask_ack_cqi_ri : 3;  ///< 0x0: No ACK/CQI/RI
                                                         ///< 0x1: ACK/CQI/RI exists
  uint32 frequency_hopping                : 2;  ///< 0 : Freq Hop disabled
                                                         ///< 1: Inter SF freq hopping
                                                         ///< 2 : Intra+Inter SF freq hopping
                                                         ///< 3 : Undefined
  uint32 redundancy_version               : 2;  ///< Range: 0 ~ 3
  uint32 mirror_hopping                   : 2;  ///< 0x0: Disabled for slot 0
                                                         ///< 0x1: Enabled for slot 0
                                                         ///< 0x2: Disabled for slot 1
                                                         ///< 0x3: Enabled for slot 1
  uint32 cyclic_shift_of_dmrs_symbols_slot_0 : 4;  ///< Cyclic shift of DMRS symbols for
                                                         ///< slot 1. Unit is samples.
                                                         ///< Range: 0 -11
  uint32 cyclic_shift_of_dmrs_symbols_slot_1 : 4;  ///< Cyclic shift of DMRS symbols for
                                                         ///< slot 1. Unit is samples.
                                                         ///< Range: 0 -11
  uint32 dmrs_root_slot_0                 : 11;  ///< Range: 1...1291
  uint32 ue_srs_on                        : 1;  ///< 0: OFF
                                                         ///< 1: ON
  uint32 reserved0                        : 3;   

  //word 3
  uint32 dmrs_root_slot_1                 : 11;  ///< Range: 1...1291
  uint32 start_resource_block_rb_for_slot_0 : 7;  ///< Range: 0...110
  uint32 start_resource_block_rb_for_slot_1 : 7;  ///< Range: 0...110
  uint32 number_of_rb                     : 7;  ///< Range: 0...110

  //word 4
  uint32 pusch_tb_size                    : 16;  ///< Transport block size in bytes
  uint32 num_ack_bits_nharq               : 3;  ///< Length of ACK NACK raw payload
  uint32 ack_payload                      : 4;  ///< ACK / NACK raw payload.
                                                         ///< Max length:
                                                         ///< TDD: 4 bits
                                                         ///< FDD: 2 bits non CA, 4 bits CA
  uint32 reserved1                         : 9;

  //word 5
  uint32 rate_matched_ack_bits            : 11;  ///< 504 bits

  uint32 num_ri_bits_nri                  : 2;  ///< 0: 0 RI bits
                                                         ///< 1: 1 RI bit
                                                         ///< 2: 2 RI bits
  uint32 ri_payload                       : 2;  ///< RI raw payload
  uint32 rate_matched_ri_bits             : 11;  ///< Range: 0 - 138
  uint32 pusch_mod_order                  : 2;  ///< Modulation order encoded as
                                                         ///< follows:
                                                         ///< 0: BPSK
                                                         ///< 1: QPSK
                                                         ///< 2: 16QAM
                                                         ///< 3: 64QAM
  uint32 reserved2                        : 4;
  
  //word 6
  uint32 pusch_digital_gain               : 8;  ///< Digital amplitude gain in dB
                                                         ///< (beta_pusch is int16. Are 8 bits
                                                         ///< sufficient?)
  uint32 srs_occasion                     : 1;  ///< 0: OFF
                                                         ///< 1: ON
  uint32 re_transmission_index            : 5;  ///< CURRENT_TX_NB
                                                         ///< 0: First transmission
                                                         ///< 1: Second transmission
                                                         ///< ...
                                                         ///< 7: Eighth transmission
  uint32 reserved3                        : 18; 

  //word 7
  uint32 pusch_transmit_power_dbm         : 10;  ///< Range: -50dBm to 23dBm, with 1dB
                                                         ///< stepsize
  uint32 num_cqi_bits_ncqi                : 8;  ///< Range:0~66 non-ca, 0-127 CA
  uint32 rate_matched_cqi_bits            : 14; ///< Range: 0~ 4959 non-ca, 0-8586 CA
  uint32 cqi_payload[4]                      ;  ///< CQI raw payload on PUSCH. Max
                                                ///< length is 96 bits for non-ca, 128 CA
  uint32 tx_resampler                     ; ///< TXR resampler value Q32 

}lte_LL1_log_ul_pusch_tx_report_pusch_records_s;


/*! @brief pusch_tx_report main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                             ///< Version = 2 for the log packet
                                                         ///< structure described below
  uint32 serving_cell_id                  : 9;  ///< Range: 0...504
  uint32 number_of_records                : 5;  ///< Range: 0..20
  uint32 reserved0                        : 10; 
  uint16 dispatch_sfn_sf                 ;  ///< System / Sub Frame Number, as
                                                         ///< 10*SFN + SF
  uint16 reserved1                       ; 
  lte_LL1_log_ul_pusch_tx_report_pusch_records_s lte_LL1_log_ul_pusch_tx_report_pusch_records[LTE_LL1_LOG_UL_NUMBER_OF_PUSCH_RECORDS_FIXED];
}lte_LL1_log_ul_pusch_tx_report_ind_struct;



/*==========================================================================*/
/*! @brief
 * 2 LTE LL1 PUCCH Tx Report (Log code ID: 0xB13C)
 *
 *  This log packet gives important parameters related to the PUCCH transmit 
 *  reports. The log record is generated once every 20 sub-frames or every 
 *  100ms, whichever occurs first. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_UL_PUCCH_TX_REPORT_VERSION 21
#define LTE_LL1_LOG_UL_NUMBER_OF_PUCCH_RECORDS_FIXED 20

/*! @brief pucch_tx_report number_of_pucch_records_fixed struct
 */
typedef struct
{
  uint16 current_sfn_sf                  ;  ///< System / Sub Frame Number, as
                                                         ///< 10*SFN + SF
  uint16 cqi_payload                     ;  ///< Raw CQI payload on PUCCH. Max length is 13 bits
  uint32 format                           : 3;  ///< 0x0: Format 1
                                                         ///< 0x1: Format 1a
                                                         ///< 0x2: Format 1b
                                                         ///< 0x3: Format 2
                                                         ///< 0x4: Format 2a
                                                         ///< 0x5: Format 2b
  uint32 start_rb_slot_0                  : 7;  ///< Range: 0...110
  uint32 start_rb_slot_1                  : 7;  ///< Range: 0...110
  uint32 srs_shorting_for_2nd_slot        : 1;  ///< 0: normal slot
                                                         ///< 1: shorten 2nd Slot
  uint32 ue_srs_on                        : 1;  ///< 0: OFF
                                                         ///< 1: ON
  uint32 dmrs_sequence_for_slot_0         : 5;  ///< index 0...29
  uint32 dmrs_sequence_for_slot_1         : 5;  ///< index 0...29
  uint32 ack_payload                      : 2;  ///< ACK / NACK raw payload.
  uint32 reserved0                        : 1; 
  uint8 cyclic_shift_sequence_for_each_symbol[14]    ;  ///< Time domain cyclic shifts per symbol per slot
  uint32 pucch_digital_gain               : 8;  ///< Digital amplitude gain in dB
  uint32 pucch_transmit_power_dbm         : 7;  ///< Range: -50dBm to 23dBm, with 1dB stepsize
  uint32 reserved1                        : 1; 

  uint32 tx_resampler                     ; ///< TXR resampler value Q32

  uint16 n_1_pucch                        ; 
  uint16 reserved2                        ;
}lte_LL1_log_ul_pucch_tx_report_pucch_records_s;


/*! @brief pucch_tx_report main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 2 for the log packet structure
                                                         ///< described below
  uint32 serving_cell_id                  : 9;  ///< Range: 0...504
  uint32 number_of_records                : 5;  ///< Range: 0..20
  uint32 reserved0                        : 10; 
  uint16 dispatch_sfn_sf                 ;  ///< System / Sub Frame Number, as
                                                         ///< 10*SFN + SF
  uint16 reserved1                       ; 
  lte_LL1_log_ul_pucch_tx_report_pucch_records_s lte_LL1_log_ul_pucch_tx_report_pucch_records[LTE_LL1_LOG_UL_NUMBER_OF_PUCCH_RECORDS_FIXED];
}lte_LL1_log_ul_pucch_tx_report_ind_struct;



/*==========================================================================*/
/*! @brief
 * 3 LTE LL1 SRS Tx Report (Log code ID: 0xB140)
 *
 *  This log packet gives important parameters related to the PUCCH power 
 *  control reports. The log record is generated once every 20 sub-frames or 
 *  every 100ms, whichever occurs first. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_UL_SRS_TX_REPORT_VERSION 2
#define LTE_LL1_LOG_UL_NUMBER_OF_SRS_RECORDS_FIXED 20

/*! @brief srs_tx_report number_of_srs_records_fixed struct
 */
typedef struct
{
  uint32 current_sfn_sf                   : 16;  ///< System / Sub Frame Number, as
                                                         ///< 10*SFN + SF
  uint32 ue_srs_enable                    : 1;  ///< 0: Enabled
                                                         ///< 1: Disabled
  uint32 cell_srs_enable                  : 1;  ///< 0: Enabled
                                                         ///< 1: Disabled
  uint32 start_rb                         : 7;  ///< Range: 0-110
  uint32 number_of_rb                     : 7;  ///< Range: 0-110
  uint32 zc_root                          : 11;  ///< Range: 1-1151
  uint32 cyclic_shift                     : 3;  ///< Range: 0-7
  uint32 reserved0                        : 2; 
  uint32 srs_transmit_power_dbm           : 7;  ///< Range: -50dBm to 23dBm, with 1dB stepsize
  uint32 tone_offset_enable               : 1;  ///< 0: Offset 0
                                                         ///< 1: Offset 1
  uint32 reserved1                        : 8; 
}lte_LL1_log_ul_srs_tx_report_srs_records_s;


/*! @brief srs_tx_report main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 2 for the log packet structure
                                                         ///< described below
  uint32 serving_cell_id                  : 9;  ///< Range: 0...504
  uint32 number_of_records                : 5;  ///< Range: 0..20
  uint32 reserved0                        : 10; 
  uint16 dispatch_sfn_sf                 ;  ///< System / Sub Frame Number, as
                                                         ///< 10*SFN + SF
  uint16 reserved1                       ; 
  lte_LL1_log_ul_srs_tx_report_srs_records_s lte_LL1_log_ul_srs_tx_report_srs_records[LTE_LL1_LOG_UL_NUMBER_OF_SRS_RECORDS_FIXED];
}lte_LL1_log_ul_srs_tx_report_ind_struct;



/*==========================================================================*/
/*! @brief
 * 4 LTE LL1 RACH Tx Report (Log code ID: 0xB144)
 *
 *  This log packet gives important parameters related to the RACH transmit 
 *  reports. The log record is generated once every RACH attempt. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_UL_RACH_TX_REPORT_VERSION 5

/*! @brief rach_tx_report main struct
 */
typedef struct
{
   uint8 version                         ;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 2 for the log packet structure
                                                         ///< described below
   uint8 reserved0                       ; 
  uint16 current_sfn_sf                  ;  ///< System / Sub Frame Number, as
                                                         ///< 10*SFN + SF
  uint32 reserved1                        : 8; 
  uint32 preamble_sequence                : 6;  ///< Range: 0-63
  uint32 cyclic_shift                     : 10;  ///< Range: 0-839
  uint32 preamble_format                  : 3;  ///< Range: 0-4
  uint32 reserved2                        : 5; 
  uint32 first_rb_for_prach_opportunity   : 7;  ///< Range: 0-110. n_ra_prb in 36.211 sec. 5.7.1
  uint32 reserved3                        : 1; 
  uint32 start_rb                         : 7;  ///< Range: 0-110
  uint32 prach_transmit_power_dbm         : 7;  ///< Range: -50dBm to 23dBm, with 1dB stepsize
  uint32 logical_root_q                   : 10;  ///< Range: 1-839

  uint32 tx_resampler                     ; ///< TXR resampler value Q32

}lte_LL1_log_ul_rach_tx_report_ind_struct;



/*==========================================================================*/
/*! @brief
 * 5 AGC Tx Report (Log code ID: 0xB146)
 *
 *  This log packet gives important parameters related to the TxAGC. The log 
 *  record is generated once every sub-frame
 *  Total size in words:  Main-struct: 3
 *                        Input sub-struct:  1 + 2*4 = 9
 *                        Return sub-struct: 9*4 = 36
 *                        Total: 48 words == 192bytes (worst-case)
 *                        
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_UL_AGC_TX_REPORT_VERSION 43


typedef struct
{
  uint32 slot_active                 : 1;  ///< Slot active/inactive
  uint32 number_of_rbs               : 7;  ///< Range: 0-110
  uint32 start_rb                    : 7;  ///< Range: 0-110
  uint32 lte_tx_pwr_db10             : 11; ///< Range: -1024 to 1023
  uint32 mod_type                    : 3;  ///< Modulation type: 0: BPSK, 1: QPSK,
                                           /// 2: 16_QAM, 3: 64_QAM 4: INVALID
  uint32 tx_ant_index                : 3;  ///< TX antenna index
  uint32 ustmr_action_time           : 24; ///< Range: 0: (2^23-1)
  uint32 reserved4                   : 8;
}lte_LL1_log_ul_fed_api_input_per_slot_s;

/*! @brief lte_LL1_log_ul_fed_api_input_param_s struct
 */
typedef struct
{
  uint32 afc_sf_active                    : 1;  ///< 0: FALSE, 1: TRUE
  uint32 afc_action_time                  : 24; ///< Range: 0: (2^23-1)
  uint32 drif_grp_idx                     : 1;  ///< Defines the DRIF group (A or B)
  uint32 reserved2                        : 6;
  lte_LL1_log_ul_fed_api_input_per_slot_s  lte_LL1_log_ul_fed_api_input_per_slot[4];
}lte_LL1_log_ul_fed_api_input_param_s;


/*! @brief lte_LL1_log_ul_fed_api_return_param_s struct
 */
typedef struct
{ 

  uint32 update_en    : 6;   ///<  Bit 0: Enable LOG_START/STOP TIME update
                             ///<  Bit 1: Enable CLUSTER_MASK update
                             ///<  Bit 2: Enable DTR_DAC_IREF update
                             ///<  Bit 3: Enable TX_DAC_CONFIG update
                             ///<  Bit 4: DRIF_LUT_0 update enable (DRIF)
                             ///<  Bit 5: DRIF_LUT_1 update enable (LUT)
                             ///<  Bit 31:6 Reserved
  uint32 block_idx     : 4;  ///< symbol number
  uint32 sample_idx    : 16; ///< sample count
  uint32 reserved5     : 6;  ///< Reserved
  uint32 ref_log_start_time;         ///< ustmr time to start tx sample capture
                                     ///< for FBRx/dbg/cal
  
  uint32 ref_log_stop_time;          ///< ustmr time to stop tx sample capture

  uint32 dac_iref_update_start_time; ///< ustmr time to start iref  update

  uint32 dac_iref_update_stop_time;  ///< ustmr time to stop iref update

  uint32 tx_dac_conf2_reg;           /// used for iref supdate

  uint32 tx_dac_conf3_reg;           /// used for iref update                           
                                  
  uint32 cluster_mask_fed;           ///< Mask indicating which rflm owned cluster needs 
                                     ///< update.  non-RF cluster bits are set to 0.  
  uint32 cluster_mask_fw;            ///< Cluster Mask provided by common-FW for Rotator DMA 

}lte_LL1_log_ul_dtr_cfg_param_s;

/*! @brief agc_tx_report main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version; range is 0 to 255
                                                         ///< Version - 2 for the log packet structure
                                                         ///< described below
  uint32 subframe_number                  : 4;  ///< Range is 0 to 9
  uint32 system_frame_number              : 10;  ///< Range is 0 to 1023
  uint32 ul_chan_type                     : 4;  ///< UL physical channel type
                                                ///< 0:PRACH, 1:PUSCH, 2:PUCCH, 3:SRS_ONLY, 4:CELL_SRS, < 
                                                ///< 5:SRS_UPPTS, 6:PUCCH_PUSCH, 7: DTX 8: CHAN_TYPE_COUNT 
                                                ///< 9:CMD_MAX 
  uint32 ul_bandwidth                     : 3;  ///< UL channel BW 
                                                ///< 0: BW_1.4, 1: BW_3, 2: BW_5, 3:BW_10,
                                                /// 4:BW_15, 5:BW_20, 6:BW_40 7: INVALID
  uint32 prach_format                     : 3;  ///< PRACH format 0:4
  uint32 pcc_cell_id                      : 3;  ///< Primary carrier cell-id, RFLM_LTE_MC_CELL0 -> RFLM_LTE_MC_CELL7
  uint32 ustmr_start_call_time            : 24; ///< FED-API call time from FW-->RFSW
  uint32 reserved0                        : 5;  ///< Reserved
  uint32 ustmr_return_call_time           : 24; ///< FED-API call-return time from RFSW-->FW
  uint32 iq_gain_backoff                  : 7;    ///< Range: 0dB to 10dB with 0.1dB stepsize
  uint32 reserved1                        : 1;  ///< Reserved
  lte_LL1_log_ul_fed_api_input_param_s lte_LL1_log_ul_fed_api_input_param;
  lte_LL1_log_ul_dtr_cfg_param_s lte_LL1_log_ul_dtr_cfg_param[4];

}lte_LL1_log_ul_agc_tx_report_ind_struct;



/*==========================================================================*/
/*! @brief
 * 6 LTE LL1 UpPts Tx Report (Log code ID: 0xB148)
 *
 *  This log packet gives important parameters related to the UpPts transmit 
 *  reports. The log record is generated once every UpPts attempt. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_UL_UPPTS_TX_REPORT_VERSION 1

/*! @brief uppts_tx_report main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 0 for the log packet structure
                                                         ///< described below
  uint32 current_sfn_sf                   : 16;  ///< System / Sub Frame Number, as
                                                         ///< 10*SFN + SF
  uint32 uppts_type                       : 2;  ///< 00: One SRS
                                                         ///< 01: Two SRS
                                                         ///< 10: RACH
                                                         ///< 11: Undefined
  uint32 preamble_format                  : 3;  ///< Range: 0-4
  uint32 reserved0                        : 3; 
  uint32 first_rb_for_prach_opportunity   : 7;  ///< Range: 0-110. n_ra_prb in 36.211 sec. 5.7.1
  uint32 reserved1                        : 1; 
  uint32 rach_srs0_start_rb               : 7;  ///< Range: 0-110
  uint32 srs1_start_rb                    : 7;  ///< Range: 0-110
  uint32 srs0_rach_cyclic_shift           : 10;  ///< Range: 0-139 for RACH; 0-7 for SRS
  uint32 uppts_transmit_power_dbm         : 7;  ///< Range: -50dBm to 23dBm, with 1dB stepsize
                                                         ///< 0x0: -50dBm ... 0x4A: 23dBm
  uint32 srs0_rach_logical_root_q         : 11;  ///< Range: 1-139 for RACH; 1-1151 for SRS
  uint32 srs1_cyclic_shift                : 3;  ///< Range: 0-7 for SRS1
  uint32 srs1_logical_root_q              : 11;  ///< Range: 1-1151
}lte_LL1_log_ul_uppts_tx_report_ind_struct;



#endif //LTE_LL1_LOG_UL_H
