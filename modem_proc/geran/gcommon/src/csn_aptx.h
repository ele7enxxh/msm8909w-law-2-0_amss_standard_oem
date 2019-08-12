/*===========================================================================

                             CSN.1 ENCODER DECODER

                            P R I V A T E   T Y P E S

DESCRIPTION
  The following declarations are for use with the CSN.1 encoder/decoder.

Copyright (c) 2007-2013 Qualcomm Technologies, Inc.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/src/csn_aptx.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07-06-15   sjw     Mainlined the refactored CSN files. Feature def is not required.
06-06-02   tjw     Created from //depot/asic/sandbox/projects/edtm/core/gcommon/csn.apt#1.

===========================================================================*/

#ifndef CSN_APTX_H
#define CSN_APTX_H

/* Include files */

/* ASSUME "customer.h" is included in the source file */
/* ASSUME "comdef.h" is included in the source file   */




/*===========================================================================

                        Constant Declarations

===========================================================================*/


/*===========================================================================

  CSN Field length constants

===========================================================================*/

#define ALLOCATION_BITMAP_LEN_SPECIFIED           0
#define ALLOCATION_BITMAP_LEN_UNSPECIFIED         1
#define CSN_8PSK_CV_BEP_LEN                       3
#define CSN_8PSK_M_BEP_LEN                        5
#define CSN_A5_4_LEN                              1
#define CSN_A5_5_LEN                              1
#define CSN_A5_6_LEN                              1
#define CSN_A5_7_LEN                              1
#define CSN_ABSOLUTE_INDEX_START_EMR_LEN          7
#define CSN_ABSOLUTE_LEN                          16
#define CSN_ACC_CONTR_CLASS_LEN                   16
#define CSN_ACCESS_BURST_TYPE_LEN                 1
#define CSN_ACCESS_CAPABILITY_LEN                 8
#define CSN_ACCESS_TECH_TYPE_LEN                  4
#define CSN_ACCESS_TECHNOLOGIES_REQ_LEN           4
#define CSN_ACCESS_TYPE_LEN                       2
#define CSN_ADDITIONAL_MSG_TYPE_LEN               1
#define CSN_ALLOC_BITMAP_LEN                      8
#define CSN_ALLOC_BITMAP_LENGTH_LEN               7
#define CSN_ALLOCATION_TYPE_LEN                   2
#define CSN_ALPHA_LEN                             4
#define CSN_ARAC_RETRAN_REQ_LEN                   1
#define CSN_ARFCN_INDEX_LIST_LEN                  6
#define CSN_ARFCN_LEN                             10
#define CSN_ASSOCIATED_RADIO_CAP_1_LEN            4
#define CSN_ASSOCIATED_RADIO_CAP_2_LEN            4
#define CSN_ATT_LEN                               1
#define CSN_AUX_TD_POWER_LEVEL_LEN                2
#define CSN_BA_USED_LEN                           1
#define CSN_BAND_IND_LEN                          1
#define CSN_BAND_INDICATOR_LEN                    1
#define CSN_BANDWIDTH_FDD_LEN                     3
#define CSN_BANDWIDTH_TDD_LEN                     3
#define CSN_BCC_LEN                               3
#define CSN_BCCH_CHNG_MRK_LEN                     3
#define CSN_BCCH_FREQ_NCELL                       5
#define CSN_BCCH_TYPE_LEN                         1
#define CSN_BEP_MEAN_LEN                          4
#define CSN_BEP_PERIOD_LEN                        4
#define CSN_BEP_PERIOD2_LEN                       4
#define CSN_BLOCK_PERIODS_LEN                     1
#define CSN_BOW                                   1
#define CSN_BS_AG_BLKS_RES_LEN                    3
#define CSN_BS_CV_MAX_LEN                         4
#define CSN_BS_PA_MFRMS_LEN                       3
#define CSN_BS_PAG_BLKS_RES_LEN                   4
#define CSN_BS_PBCCH_BLKS_LEN                     2
#define CSN_BS_PCC_REL_LEN                        1
#define CSN_BS_PRACH_BLKS_LEN                     4
#define CSN_BSIC_LEN                              6
#define CSN_BSIC_N_LEN                            6
#define CSN_BSS_PAGING_COORDINATION_LEN           1
#define CSN_BTS_PWR_CTRL_MODE_LEN                 1
#define CSN_C_VALUE_LEN                           6
#define CSN_C31_HYST_LEN                          1
#define CSN_C32_QUAL_LEN                          1
#define CSN_CALL_PRIORITY_LEN                     3
#define CSN_CAUSE_LEN                             4
#define CSN_CBQ_LEN                               1
#define CSN_CCCH_CONF_LEN                         3
#define CSN_CCN_ACTIVE_LEN                        1
#define CSN_CD_LENGTH_LEN                         5
#define CSN_CDMA2000_FREQUENCY_BAND_LEN           5
#define CSN_CDMA2000_FREQUENCY_LEN                11
#define CSN_CDMA2000_RADIO_ACCESS_TECH_CAP_LEN    1
#define CSN_CELL_BAR_ACCESS_2_LEN                 1
#define CSN_CELL_ID_LEN                           8
#define CSN_CELL_IDENTITY_LEN                     16
#define CSN_CELL_PARAM_PTR_LEN                    2
#define CSN_CELL_RESELECT_OFFSET_LEN              6
#define CSN_CHAN_CODING_CMD_LEN                   2
#define CSN_CHAN_TYPE_TDMA_OFFSET_LEN             5
#define CSN_CHANGE_MARK_1_LEN                     2
#define CSN_CHANGE_MARK_2_LEN                     2
#define CSN_CHANGE_MARK_LEN                       2
#define CSN_CHANNEL_CODING_COMMAND_LEN            2
#define CSN_CHANNEL_NEEDED_LEN                    2
#define CSN_CHANNEL_TYPE_LEN                      5
#define CSN_CN3_LEN                               2
#define CSN_CN4_LEN                               2
#define CSN_CONTAINER_ID_LEN                      2
#define CSN_CONTAINER_INDEX_LEN                   5
#define CSN_CONTENTION_RES_TLLI_LEN               32
#define CSN_CONTROL_ACK_LEN                       1
#define CSN_CRBB_LEN_LEN                          7
#define CSN_CTRL_ACK_LEN                          1
#define CSN_CTRL_ACK_TYPE_LEN                     1
#define CSN_DOWNLINK_CONTROL_TIMESLOT_LEN         3
#define CSN_DOWNLINK_TFI_ASSIGNMENT_LEN           5
#define CSN_DOWNLINK_TFI_LEN                      5
#define CSN_DRX_TIMER_MAX_LEN                     3
#define CSN_DTM_EGPRS_MULTI_SLOT_SUB_CLASS_LEN    2
#define CSN_DTM_ENHANCEMENTS_CAPABILITY_LEN       1
#define CSN_DTM_GPRS_MULTI_SLOT_SUB_CLASS_LEN     2
#define CSN_DTM_SUPPORT_LEN                       1
#define CSN_DTX_LEN                               2
#define CSN_DWNLINK_RELEASE_LEN                   1
#define CSN_DWNLINK_TA_INDEX_LEN                  4
#define CSN_DWNLINK_TA_TS_NUM_LEN                 3
#define CSN_DWNLINK_TFI_LEN                       5
#define CSN_DWNLK_CTRL_TS_LEN                     3
#define CSN_DWNLK_RLC_MODE_LEN                    1
#define CSN_DWNLK_TFI_ASSIGNMENT_LEN              5
#define CSN_DWNLK_TS_ALLOC_LEN                    8
#define CSN_EC_LEN                                1
#define CSN_ECSC_LEN                              1
#define CSN_EDGE_MULTI_SLOT_CLASS_LEN             5
#define CSN_EDGE_RF_POWER_CAP_1_LEN               2
#define CSN_EDGE_RF_POWER_CAP_2_LEN               2
#define CSN_EGPRS_BEP_PRD2_LEN                    4
#define CSN_EGPRS_CHANNEL_CODING_COMMAND_LEN      4
#define CSN_EGPRS_LINK_QUAL_LEN                   2
#define CSN_EGPRS_MCS_MODE_LEN                    4
#define CSN_EGPRS_PACKET_CHANNEL_REQUEST_LEN      1
#define CSN_EGPRS_WINDOW_SIZE_LEN                 5
#define CSN_EMLPP_PRIORITY_LEN                    3
#define CSN_EOW_LEN                               1
#define CSN_EXC_ACC_LEN                           1
#define CSN_EXT_DYNAMIC_ALLOC_LEN                 1
#define CSN_EXT_LENGTH_LEN                        6
#define CSN_EXT_MEAS_ORDR_LEN                     2
#define CSN_EXT_RA_LEN                            5
#define CSN_EXT_REPORTING_PERIOD_LEN              3
#define CSN_EXT_REPORTING_TYPE_LEN                2
#define CSN_EXT_UTBF_NO_DATA_LEN                  1
#define CSN_EXTENDED_MEASUREMENT_CAP_LEN          1
#define CSN_EXTENDED_RA_LEN                       5
#define CSN_FAI_LEN                               1
#define CSN_FDD_ARFCN_INDEX_LEN                   3
#define CSN_FDD_ARFCN_LEN                         14
#define CSN_FDD_BANDWIDTH_LEN                     3
#define CSN_FDD_INDIC0_LEN                        1
#define CSN_FDD_SCRAMBLING_CODE_LEN               9
#define CSN_FINAL_ACK_IND_LEN                     1
#define CSN_FINAL_ALLOC_LEN                       1
#define CSN_FRAME_NUMBER_LEN                      16
#define CSN_FREQ_DIFF_LENGTH_LEN                  3
#define CSN_FREQ_N_LEN                            6
#define CSN_GAMMA_LEN                             5
#define CSN_GAMMA_TN_LEN                          5
#define CSN_GMSK_CV_BEP_LEN                       3
#define CSN_GMSK_M_BEP_LEN                        5
#define CSN_GPRS_CELL_OPTIONS_OPTIONAL_EXTENSION_INFO_LENGTH_LEN  6
#define CSN_GPRS_CELL_RESEL_HYST_LEN              3
#define CSN_GPRS_MS_TMC_LEN                       5
#define CSN_GPRS_MS_TXPWR_MAX_CCH_LEN             5
#define CSN_GPRS_PENALTY_TIME_LEN                 5
#define CSN_GPRS_RESELECT_OFFSET_LEN              5
#define CSN_GPRS_RXLEV_ACCESS_MIN_LEN             6
#define CSN_GPRS_TEMP_OFFSET_LEN                  3
#define CSN_GROUP_CALL_REF_LEN                    36
#define CSN_GSM_BAND_LEN                          4
#define CSN_GSM400_ASSOCIATED_RADIO_CAP_LEN       4
#define CSN_GSM400_BANDS_SUPPORTED_LEN            2
#define CSN_GSM850_ASSOCIATED_RADIO_CAP_LEN       4
#define CSN_HALF_DUPLEX_LEN                       1
#define CSN_HALF_DUPLEX_MODE_LEN                  1
#define CSN_HCS_THR_LEN                           5
#define CSN_HSN_LEN                               6
#define CSN_I_LEVEL_LEN                           4
#define CSN_I_LEVEL_TN_BITMAP_LEN                 8
#define CSN_I_LEVEL_TN_LEN                        6
#define CSN_I_LEVEL_TN_PR_LEN                     4
#define CSN_INDEX_START_3G_LEN                    7
#define CSN_INFO_TYPE_4_LEN                       4
#define CSN_INFO_TYPE_5_LEN                       5
#define CSN_INFO_TYPE_6_LEN                       6
#define CSN_INT_FREQUENCY_LEN                     5
#define CSN_INT_MEAS_CHN_LIST_LEN                 1
#define CSN_INTRFRNCE_SRVG_CELL_LEN               6
#define CSN_LAI_LEN                               8
#define CSN_LARGE_CELL_OP_LEN                     1
#define CSN_LB_MS_TXPWR_MAX_CCH_LEN               5
#define CSN_LEN_MA_FREQ_LIST_LEN                  4
#define CSN_LENGTH_IN_OCTETS_LEN                  8
#define CSN_LENGTH_OF_FREQ_PARAMS_LEN             6
#define CSN_LENGTH_OF_MI_LEN                      4
#define CSN_LENGTH_RFL_DATA_LEN                   4
#define CSN_LINK_QUALITY_MEASUREMENT_MODE_LEN     2
#define CSN_LLC_PDU_TYPE_LEN                      1
#define CSN_LSA_ID_LEN                            23
#define CSN_LSA_OFFSET_LEN                        3
#define CSN_LST_INDEX_LEN                         7
#define CSN_MA_FREQ_LIST_LEN                      8
#define CSN_MA_LENGTH_LEN                         6
#define CSN_MA_NUMBER_IND_LEN                     1
#define CSN_MA_NUMBER_LEN                         4
#define CSN_MAC_MODE_LEN                          2
#define CSN_MAC_MODE_SUPPORT_LEN                  1
#define CSN_MAIO_2_LEN                            6
#define CSN_MAIO_LEN                              6
#define CSN_MAX_LAPDM_LEN                         3
#define CSN_MAX_RETRANS_LEN                       2
#define CSN_MCC_LEN                               12
#define CSN_MEAS_ORDER_LEN                        1
#define CSN_MEASURE_BITMAP_LEN                    8
#define CSN_MEASURE_INTERVAL_LEN                  5
#define CSN_MEASUREMENT_BITMAP_LEN                8
#define CSN_MEASUREMENT_INTERVAL_LEN              5
#define CSN_MEASUREMENT_STARTING_TIME_LEN         16
#define CSN_MESS_REC_LEN                          2
#define CSN_MESSAGE_TYPE_LEN                      6
#define CSN_MNC_LEN                               12
#define CSN_MOBILE_IDENTITY_LEN                   8
#define CSN_MODULATION_CAP_LEN                    1
#define CSN_MODULATION_LEN                        1
#define CSN_MODULUS_LEN                           4
#define CSN_MS_POSITIONING_METHOD_CAP_LEN         5
#define CSN_MS_POSITIONING_METHOD_LEN             5
#define CSN_MS_TXPWR_MAX_CCCH_LEN                 5
#define CSN_MS_TXPWR_MAX_LEN                      5
#define CSN_MSCR_LEN                              1
#define CSN_MSG_TYPE_LEN                          6
#define CSN_MULT_TBF_CAP_LEN                      1
#define CSN_MULTI_SLOT_CAP_LEN                    5
#define CSN_MULTIBAND_RPRT_LEN                    2
#define CSN_MULTIBAND_SUPPORTED_LEN               3
#define CSN_MULTIPLE_TBF_CAPABILITY_LEN           1
#define CSN_N_3G_LEN                              3
#define CSN_N_AVG_I_LEN                           4
#define CSN_N1_LEN                                2 /* 2 bits, values 0..3  */
#define CSN_N2_LEN                                5 /* 5 bits, values 0..31 */
#define CSN_N3G_LEN                               3
#define CSN_NC_MODE_LEN                           1
#define CSN_NCC_LEN                               3
#define CSN_NCC_PERMITTED_LEN                     8
#define CSN_NCH_POSITION_LEN                      5
#define CSN_NECI_LEN                              1
#define CSN_NIB_CCCH_LEN                          4
#define CSN_NLN_LEN                               2
#define CSN_NLN_PCH_LEN                           2
#define CSN_NLN_SACCH_LEN                         2
#define CSN_NLN_STATUS_LEN                        1
#define CSN_NMO_LEN                               2
#define CSN_NON_GSM_INFO_LEN                      2
#define CSN_NR_OF_FDD_CELLS_LEN                   5
#define CSN_NR_OF_FREQ_LEN                        5
#define CSN_NR_OF_TDD_CELLS_LEN                   5
#define CSN_NR_REMAINING_CELLS_LEN                4
#define CSN_NUM_EXT_MEAS_LEN                      5
#define CSN_NUM_NC_MEAS_LEN                       3
#define CSN_NUM_OF_RADIO_BLOCKS_ALLOCATED_LEN     2
#define CSN_NUMBER_CDMA2000_CELLS_LEN             5
#define CSN_NUMBER_CELLS_LEN                      7
#define CSN_NUMBER_OF_BLKS_ALLOC_LEN              2
#define CSN_NW_EXT_TBF_LEN                        1
#define CSN_P0_LEN                                4
#define CSN_PACKET_EXT_TIMING_ADV_LEN             2
#define CSN_PACKET_PSI3_QUATER_GPRS_REP_PRIORITY_NUMBER_CELLS_LEN               7
#define CSN_PACKET_PSI3_QUATER_GPRS_REP_PRIORITY_REP_PRIORITY_LEN               1
#define CSN_PACKET_PSI3_QUATER_THREE_G_NCELLS_DESC_ABSOLUTE_INDEX_START_EMR_LEN 7
#define CSN_PACKET_PSI3_QUATER_THREE_G_NCELLS_DESC_INDEX_START_3G_LEN           7
#define CSN_PAGE_CH_RESTRUCT_LEN                  1
#define CSN_PAGE_MODE_LEN                         2
#define CSN_PAN_DEC_LEN                           3
#define CSN_PAN_INC_LEN                           3
#define CSN_PAN_MAX_LEN                           3
#define CSN_PB_LEN                                4
#define CSN_PBCCH_CHNG_MRK_LEN                    3
#define CSN_PBCCH_LOC_LEN                         2
#define CSN_PC_MEAS_CHAN_LEN                      1
#define CSN_PCS1900_ASSOCIATED_RADIO_CAP_LEN      4
#define CSN_PD_LEN                                3
#define CSN_PEAK_THR_CLASS_LEN                    4
#define CSN_PENALTY_TIME_LEN                      5
#define CSN_PERSISTENCE_LEN                       4
#define CSN_PFC_FEATURE_MODE_LEN                  1
#define CSN_PFI_LEN                               7
#define CSN_PILOT_PN_OFFSET_LEN                   9
#define CSN_PKT_EST_CAUSE_LEN                     2
#define CSN_PKT_EXT_TA_LEN                        2
#define CSN_PMO_USED_LEN                          1
#define CSN_PO_LEN                                4
#define CSN_POLLING_LEN                           1
#define CSN_POWER_OFFSET_LEN                      2
#define CSN_PR_MODE_LEN                           1
#define CSN_PRIO_THR_LEN                          3
#define CSN_PRIORITY_ACCESS_THR_LEN               3
#define CSN_PRIORITY_CLASS_LEN                    3
#define CSN_PRIORITY_LEN                          3
#define CSN_PSI_CHNG_FLD_LEN                      4
#define CSN_PSI_CNT_HR_LEN                        4
#define CSN_PSI_CNT_LR_LEN                        6
#define CSN_PSI_STATUS_IND_LEN                    1
#define CSN_PSI1_REPEAT_PERIOD_LEN                4
#define CSN_PSI13_CHNG_MRK_LEN                    2
#define CSN_PSI2_CHNG_MRK_LEN                     2
#define CSN_PSI2_COUNT_LEN                        3
#define CSN_PSI2_INDEX_LEN                        3
#define CSN_PSI3_BIS_COUNT_LEN                    4
#define CSN_PSI3_BIS_INDEX_LEN                    4
#define CSN_PSI3_CHANGE_MARK_LEN                  2
#define CSN_PSI3_QUATER_COUNT_LEN                 4
#define CSN_PSI3_QUATER_INDEX_LEN                 4
#define CSN_PSI3QUATER_BROADCAST_LEN              1
#define CSN_PSI3TER_BROADCAST_LEN                 1
#define CSN_PSI4_CHNG_MRK_LEN                     2
#define CSN_PSI4_COUNT_LEN                        3
#define CSN_PSI4_INDEX_LEN                        3
#define CSN_PSI5_CHNG_MRK_LEN                     2
#define CSN_PSI5_COUNT_LEN                        3
#define CSN_PSI5_INDEX_LEN                        3
#define CSN_PSI8_BROADCAST_LEN                    1
#define CSN_PSIX_CHNG_MRK_LEN                     2
#define CSN_PSIX_COUNT_LEN                        4
#define CSN_PTMSI_LEN                             32
#define CSN_PWRC_LEN                              1
#define CSN_QOF_LEN                               2
#define CSN_QOF1_LEN                              2
#define CSN_QOF2_LEN                              2
#define CSN_R_SUPPORT_LEN                         3
#define CSN_RA_COLOUR_LEN                         3
#define CSN_RA_RESEL_HYST_LEN                     3
#define CSN_RAC_LEN                               8
#define CSN_RADIO_LNK_TO_LEN                      4
#define CSN_RADIO_PRIORITY_LEN                    2
#define CSN_RANDOM_ACCESS_INFO_LEN                11
#define CSN_RANDOM_ACCESS_RETRY_LEN               1
#define CSN_RECEIVED_BLK_BTMP_LEN                 8
#define CSN_REDUCED_MA_BIT_LENGTH_LEN             7
#define CSN_RELATIVE_LEN                          13
#define CSN_RELATIVE_POSITION_LEN                 1
#define CSN_REMOVED_THREE_G_CELL_INDEX_LEN        7
#define CSN_REP_PRIORITY_LEN                      1
#define CSN_REP_QUALITY_LEN                       6
#define CSN_REP_QUANT_LEN                         6
#define CSN_REPEAT_PERIOD_LEN                     4
#define CSN_RESEGMENT_LEN                         1
#define CSN_RFL_LEN                               8
#define CSN_RFL_NUM_LEN                           4
#define CSN_RFL_NUMBER_LIST_LEN                   4
#define CSN_RLC_DATA_BLKS_GRANTED_LEN             8
#define CSN_RLC_MODE_LEN                          1
#define CSN_RLC_OCTET_CNT_LEN                     16
#define CSN_RR_ALLOC_BITMAP_LENGTH_LEN            5
#define CSN_RXLEV_ACCESS_LEN                      6
#define CSN_RXLEV_N_LEN                           6
#define CSN_RXLEV_NCELL                           6
#define CSN_RXLEV_SRVG_CELL_LEN                   6
#define CSN_RXQUAL_LEN                            3
#define CSN_S_LEN                                 4
#define CSN_SAME_RA_LEN                           1
#define CSN_SCC_LEN                               1
#define CSN_SERVING_CELL_DATA_INTERFERENCE        6
#define CSN_SERVING_CELL_DATA_RXLEV               6
#define CSN_SGSNR_LEN                             1
#define CSN_SHORT_LSA_ID_LEN                      9
#define CSN_SI_CHNG_FIELD_LEN                     4
#define CSN_SI13_CHANGE_MARK_LEN                  2
#define CSN_SI13_LOC_LEN                          1
#define CSN_SI13_POSITION_LEN                     1
#define CSN_SI2N_SUPPORT_LEN                      2
#define CSN_SI2QUATER_POSITION_LEN                1
#define CSN_SI2TER_LEN                            1
#define CSN_SIGN_VAR_LEN                          6
#define CSN_SIX_CHNG_MRK_LEN                      3
#define CSN_SIX_COUNT_LEN                         4
#define CSN_SIX_MSG_TYPE_LEN                      8
#define CSN_SM_VALUE_LEN                          4
#define CSN_SMS_VALUE_LEN                         4
#define CSN_SPGC_CCCH_SUP_LEN                     1
#define CSN_SR3_PILOT_POWER1_LEN                  3
#define CSN_SR3_PILOT_POWER2_LEN                  3
#define CSN_SR3_PRIM_PILOT_LEN                    2
#define CSN_SSN_LEN                               11
#define CSN_START_FREQ_LEN                        10
#define CSN_STATUS_MESS_TYPE_LEN                  6
#define CSN_STRT_SEQ_NUM_LEN                      7
#define CSN_T_AVG_T_LEN                           5
#define CSN_T_AVG_W_LEN                           5
#define CSN_T_RESEL_LEN                           3
#define CSN_T3168_LEN                             3
#define CSN_T3192_LEN                             3
#define CSN_T3212_LEN                             8
#define CSN_TA_VALID_LEN                          1
#define CSN_TA_VALUE_LEN                          6
#define CSN_TBF_CAUSE_LEN                         3
#define CSN_TBF_EST_LEN                           1
#define CSN_TBF_RELEASE_CAUSE_LEN                 4
#define CSN_TBF_STARTING_TIME_LEN                 16
#define CSN_TD_MODE_LEN                           2
#define CSN_TD_POWER_LEVEL_LEN                    3
#define CSN_TDD_ARFCN_INDEX_LEN                   3
#define CSN_TDD_ARFCN_LEN                         14
#define CSN_TDD_BANDWIDTH_LEN                     3
#define CSN_TDD_CELL_PARAM_LEN                    7
#define CSN_TDD_INDIC0_LEN                        1
#define CSN_TEMPORARY_OFFSET_LEN                  3
#define CSN_TFI_ASSIGNMENT_LEN                    5
#define CSN_THREE_G_BA_USED_LEN                   1
#define CSN_THREE_G_CELL_DIFF_LENGTH_LEN          3
#define CSN_THREE_G_CELL_LIST_INDEX_LEN           7
#define CSN_THREE_G_ECSR_LEN                      1
#define CSN_TIMESLOT_ALLOC_LEN                    8
#define CSN_TIMESLOT_NUMBER_LEN                   3
#define CSN_TIMESLOTS_AVAILABLE_LEN               8
#define CSN_TIMING_ADV_INDEX_LEN                  4
#define CSN_TIMING_ADV_TIMESLOT_LEN               3
#define CSN_TIMING_ADV_VALUE_LEN                  6
#define CSN_TIMING_ADVANCE_INDEX_LEN              4
#define CSN_TLLI_BLK_CHAN_CODING_LEN              1
#define CSN_TLLI_BLOCK_CHANNEL_CODING_LEN         1
#define CSN_TLLI_LEN                              32
#define CSN_TMSI_LEN                              32
#define CSN_TN_LEN                                3
#define CSN_TQI_LEN                               16
#define CSN_TS_ALLOC_LEN                          8
#define CSN_TS_OVERRIDE_LEN                       8
#define CSN_TSC_LEN                               3
#define CSN_TX_INT_LEN                            4
#define CSN_TX_INTEGER_LEN                        4
#define CSN_TYPE_OF_ACK_LEN                       1
#define CSN_UCS2_TREATMENT_LEN                    1
#define CSN_UMTS_FDD_RADIO_ACCESS_TECH_CAP_LEN    1
#define CSN_UMTS_TDD_RADIO_ACCESS_TECH_CAP_LEN    1
#define CSN_UP_CTRL_ACK_LEN                       2
#define CSN_UPLINK_RELEASE_LEN                    1
#define CSN_UPLINK_TA_INDEX_LEN                   4
#define CSN_UPLINK_TA_TS_NUM_LEN                  3
#define CSN_UPLINK_TFI_ASSIGNMENT_LEN             5
#define CSN_UPLINK_TFI_LEN                        5
#define CSN_UPLINK_TS_ALLOC_LEN                   8
#define CSN_USF_GRANULARITY_LEN                   1
#define CSN_USF_LEN                               3
#define CSN_USF_TN_LEN                            3
#define CSN_VBS_VGCS_INBAND_NOTIF_LEN             1
#define CSN_VBS_VGCS_INBAND_PAGINGS_LEN           1
#define CSN_WAIT_IND_LEN                          8
#define CSN_WAIT_IND_SZ_LEN                       1
#define CSN_WALSH_LEN_A_LEN                       3
#define CSN_WALSH_LEN_B_LEN                       3
#define CSN_WALSH_LEN_C_LEN                       3
#define CSN_WALSH_LENGTH1_LEN                     3
#define CSN_WALSH_LENGTH2_LEN                     3
#define CSN_WHERE_LEN                             3
#define CSN_ESTABLISHMENT_CAUSE_LEN               8
#define CSN_CONTAINER_LENGTH_LEN                  8
#define CSN_UPLINK_CONTROL_TIMESLOT_LEN           3
#define CSN_EDTM_REALLOC_TYPE_LEN                 2
#define CSN_EXTENDED_TIMING_ADVANCE_LEN           2
#define CSN_TN_RRBP_LEN                           3
#define CSN_GRNTI_EXT_LEN                         4
#define CSN_CTRL_ACK_EXTENSION_LEN                9

#endif /* CSN_APTX_H */
