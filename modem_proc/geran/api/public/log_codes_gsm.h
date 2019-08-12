#ifndef LOG_CODES_GSM_H
#define LOG_CODES_GSM_H

/*===========================================================================

                         Log Code Definitions

General Description
  This file contains log code definitions and is shared with the tools.

Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                             Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/public/log_codes_gsm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/14   ws      CR702510 Add support for gprs message metrics E log packet
23/09/14   jj      CR 728792  Discrete Fourier transform (DFT) spur metric log packet
19-08-14   pjr     CR711506 Support for AMR Partial Decode and DTX detection FR19667
09-09-14   jk      CR697308:GL1 changes to support HLin LLin sawless feature
11-07-14   ws      CR690995 Hybrid AEQ log packet definition and implementation
10-07-14   jk      CR688587:GL1 changes to support LIF feature
21-07-14   jk      CR660801:Support for logging VAMOS log packet 0x5099
18-06-14   cos     CR682031 - GL1 control algorithm and changes for supporting AEQ PHASE 4
19-12-13   jk      CR503359 - GL1 changes to support Rx diversity
24-10-13   jj      CR 497390 LOG_GPRS_RECEIVE_BURST_METRICS_E_C_type
02-07-13   pjr     CR507542: Added LOG_GSM_COEX_PARAMS_C
10-09-08   tjw     Added LOG_GSM_AEQ_SCH_METRICS_C
09-02-26   tjw     Added LOG_GSM_NCELL_ASYNC_METRICS_C
09-02-11   tjw     Initial revision created by "splitting" modem/geran/gdiag/inc/log_codes_gsm.h

===========================================================================*/
/* APICOPY__LOG_GSM_BASE_C is defined in log_codes.h, define a copy here, explicitly
   and then check it where the header file can be included (i.e. NOT in an API file
   like this) */
#define APICOPY__LOG_GSM_BASE_C 0x5000

/* ========================   GSM LAYER1 PACKETS   ======================== */
#define LOG_GSM_POWER_SCAN_C                          (100  + APICOPY__LOG_GSM_BASE_C) /* 0x5064 */
#define LOG_GSM_FCCH_DECODE_C                         (101  + APICOPY__LOG_GSM_BASE_C)/* 0x5065 */
#define LOG_GSM_SCH_DECODE_C                          (102  + APICOPY__LOG_GSM_BASE_C)/* 0x5066 */
#define LOG_GSM_RECEIVE_BURST_METRICS_C               (108  + APICOPY__LOG_GSM_BASE_C)/* 0x506C */
#define LOG_GSM_PAGING_DATA_C                         (111  + APICOPY__LOG_GSM_BASE_C)/* 0x506F */
#define LOG_GSM_NCELL_ENGINE_MODE_C                   (112  + APICOPY__LOG_GSM_BASE_C)/* 0x5070 */
#define LOG_GSM_NCELL_BA_LIST_C                       (113  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_SERVING_CELL_POWER_C                  (114  + APICOPY__LOG_GSM_BASE_C)

/* QUICK PACKETS FOR SYSTEMS 07/11/2002 */
#define LOG_GSM_GSTMR_ADJ_C                           (115  + APICOPY__LOG_GSM_BASE_C)/* 0x5073 */
#define LOG_GSM_NCELL_ACQ_C                           (117  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_TXLEV_TA_C                            (118  + APICOPY__LOG_GSM_BASE_C)

/* New Message Metrics w/ both hard and soft RxQual */
#define LOG_GSM_RECEIVE_MSG_METRICS_ENHANCED_C        (121  + APICOPY__LOG_GSM_BASE_C)/* 0x5079 */

/* Packets for auxiliary L1 meausrement reports */
#define LOG_GSM_SERVING_AUX_MEAS_C                    (122  + APICOPY__LOG_GSM_BASE_C)/* 0x507A */
#define LOG_GSM_NCELL_AUX_MEAS_C                      (123  + APICOPY__LOG_GSM_BASE_C)

/* New AFC log with PDM and rotator values */
#define LOG_GSM_AFC_ADJUST_C                          (124  + APICOPY__LOG_GSM_BASE_C)/* 0x507C */

/* New NCELL async rx metrics log */
#define LOG_GSM_NCELL_ASYNC_METRICS_C                 (125  + APICOPY__LOG_GSM_BASE_C)/* 0x507D */

/* QUICK PACKETS2 FOR SYSTEMS 12/23/2002 */
#define LOG_GSM_MON_BURST_C                           (130  + APICOPY__LOG_GSM_BASE_C)/* 0x5082 */
#define LOG_GSM_SCH_BURST_METRICS_C                   (131  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_BCCH_BURST_METRICS_C                  (133  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_BCCH_MSG_METRICS_C                    (134  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_SCH_MSG_METRICS_VER2_C                (135  + APICOPY__LOG_GSM_BASE_C)/* 0x5087 */

/* Transmit burst metrics */
#define LOG_GSM_TRANSMIT_BURST_METRICS_C              (136  + APICOPY__LOG_GSM_BASE_C)/* 0x5088 */

/* MDSP command log */
#define LOG_GSM_MDSP_CMD_C                            (140  + APICOPY__LOG_GSM_BASE_C) /* 0x508C */
#define LOG_GSM_GL1_HW_CMD_C                          (141  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_MDSP_DEBUG_BUFFER_C                   (143  + APICOPY__LOG_GSM_BASE_C)/* 0x508F not in ICD*/
#define LOG_GSM_SAIC_METRICS_C                        (145  + APICOPY__LOG_GSM_BASE_C)/* DEPRECATED 0x5091 not in ICD*/
#define LOG_GPRS_ESAIC_METRICS_C                      (146  + APICOPY__LOG_GSM_BASE_C)/* DEPRECATED 0x5092 not in ICD*/
#define LOG_GSM_AEQ_SCH_METRICS_C                     (147  + APICOPY__LOG_GSM_BASE_C)/* DEPRECATED0x5093 not in ICD*/
#define LOG_GSM_COEX_PARAMS_C                         (148  + APICOPY__LOG_GSM_BASE_C)/* 0x5094 not in ICD*/
#define LOG_GSM_DFT_SPUR_METRICS_C                    (149  + APICOPY__LOG_GSM_BASE_C) /*0x5095 assigned*/
#define LOG_GSM_RxD_METRICS_C                         (150  + APICOPY__LOG_GSM_BASE_C) /*0x5096 assigned */
#define LOG_GSM_EPD_METRICS_C                         (151  + APICOPY__LOG_GSM_BASE_C) /*0x5097 assigned */
#define LOG_GSM_VAMOS_METRICS_C                       (153  + APICOPY__LOG_GSM_BASE_C)/*0x5099 assigned */
#define LOG_GSM_METRICS_HLLL_STATE_C                  (154  + APICOPY__LOG_GSM_BASE_C) /*0x509A Assigned*/
#define LOG_GSM_SAIC_METRICS_V2_C                     (155  + APICOPY__LOG_GSM_BASE_C)/* 0x509B */
#define LOG_GPRS_ESAIC_METRICS_V2_C                   (156  + APICOPY__LOG_GSM_BASE_C)/* 0x509C */
#define LOG_GSM_AEQ_SCH_METRICS_V2_C                  (157  + APICOPY__LOG_GSM_BASE_C)/* 0x509D */
#define LOG_GSM_AEQ4_METRICS_C                        (158  + APICOPY__LOG_GSM_BASE_C) /*0x509E assigned */ 
#define LOG_GSM_LIF_METRICS_C                         (159  + APICOPY__LOG_GSM_BASE_C) /*0X509F assigned */
#define LOG_GSM_AMR_PD_DTX_BURST_METRICS_C            (162  + APICOPY__LOG_GSM_BASE_C)/* 0x50A2 */
#define LOG_GSM_AMR_PD_DTX_MESSAGE_METRICS_C          (163  + APICOPY__LOG_GSM_BASE_C)/* 0x50A3 */


/* ========================   GSM LAYER2 PACKETS   ======================== */
#define LOG_GSM_L2_STATE_C                            (200  + APICOPY__LOG_GSM_BASE_C) /* 0x50C8 */
#define LOG_GSM_L2_TRANSMISSION_STATUS_C              (201  + APICOPY__LOG_GSM_BASE_C) /* 0x50C9 */
#define LOG_GSM_L2_OUTSTANDING_FRAME_C                (202  + APICOPY__LOG_GSM_BASE_C) /* 0x50CA */

/* ========================   GSM LAYER3 PACKETS   ======================== */
#define LOG_GSM_RR_STATE_C                            (300  + APICOPY__LOG_GSM_BASE_C) /* 0x512C */
#define LOG_GSM_RR_PROTOCOL_ERROR_C                   (302  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_SIGNALING_MESSAGE_C                (303  + APICOPY__LOG_GSM_BASE_C)/* 0x512F */
#define LOG_GSM_RR_CELL_RESELECTION_PARAMS_C          (304  + APICOPY__LOG_GSM_BASE_C)/* 0x5130 */
#define LOG_GSM_RR_RACH_CONTROL_PARAMS_C              (305  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C           (306  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_CELL_OPTIONS_C                     (307  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_CELL_INFO_C                        (308  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_CHANNEL_CONFIGURATION_C            (309  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_CIPHERING_MODE_C                   (310  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_CELL_RESELECTION_MEASMNTS_C        (311  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_DSC_COUNTER_C                      (312  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_S_COUNTER_C                        (313  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_SACCH_RPT_C                        (314  + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_RR_AVAILABLE_PLMN_LIST_C              (315  + APICOPY__LOG_GSM_BASE_C) /* 0x513B *//*not in ICD*/
#define LOG_GSM_RR_BA_LIST_C                          (316  + APICOPY__LOG_GSM_BASE_C)/* 0x513C */
#define LOG_GSM_RR_FAKE_CELL_DATA_C                   (317  + APICOPY__LOG_GSM_BASE_C)/* 0x513D */


/* ===========================   GPRS PACKETS  ============================ */
#define LOG_GPRS_INDICATORS_C                         (500  + APICOPY__LOG_GSM_BASE_C) /* 0x51F4 */
#define LOG_GPRS_GENERAL_PARAMS_C                     (501  + APICOPY__LOG_GSM_BASE_C) /* 0x51F5 */
#define LOG_GPRS_CELL_OPTIONS_C                       (502  + APICOPY__LOG_GSM_BASE_C) /* 0x51F6 */
#define LOG_GPRS_POWER_CONTROL_PARAMS_C               (503  + APICOPY__LOG_GSM_BASE_C) /* 0x51F7 */
#define LOG_GPRS_MOBILE_ALLOCATION_C                  (504  + APICOPY__LOG_GSM_BASE_C) /* 0x51F8 */
#define LOG_GPRS_PBCCH_DESCRIPTION_C                  (505  + APICOPY__LOG_GSM_BASE_C) /* 0x51F9 */
#define LOG_GPRS_GRR_STATE_C                          (506  + APICOPY__LOG_GSM_BASE_C) /* 0x51FA */
#define LOG_GPRS_RR_CELL_RESELECTION_PARAMS_C         (507  + APICOPY__LOG_GSM_BASE_C) /* 0x51FB */
#define LOG_GPRS_RR_CELL_RESELECTION_MEASMNTS_C       (508  + APICOPY__LOG_GSM_BASE_C) /* 0x51FC */
#define LOG_GPRS_RR_PSI1_C                            (509  + APICOPY__LOG_GSM_BASE_C) /* 0x51FD */
#define LOG_GPRS_RR_PSI2_C                            (510  + APICOPY__LOG_GSM_BASE_C) /* 0x51FE */
#define LOG_GPRS_RR_PSI3_C                            (511  + APICOPY__LOG_GSM_BASE_C) /* 0x51FF */

#define LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C           (512  + APICOPY__LOG_GSM_BASE_C) /* 0x5200 */
#define LOG_GPRS_RLC_UL_EVENT_COUNTS_C                (513  + APICOPY__LOG_GSM_BASE_C) /* 0x5201 */
#define LOG_GPRS_RLC_UL_STATS_C                       (514  + APICOPY__LOG_GSM_BASE_C) /* 0x5202 */

#define LOG_GPRS_RLC_UL_CONFIG_C                      (515  + APICOPY__LOG_GSM_BASE_C) /* 0x5203 */
#define LOG_GPRS_RLC_DL_CONFIG_C                      (516  + APICOPY__LOG_GSM_BASE_C) /* 0x5204 */

/* ========================  New items for EGPRS ========================= */
#define LOG_EGPRS_RLC_UL_HEADER_C                     (517  + APICOPY__LOG_GSM_BASE_C) /* 0x5205 */
#define LOG_EGPRS_RLC_DL_HEADER_C                     (518  + APICOPY__LOG_GSM_BASE_C) /* 0x5206 */
/* =====================  End of New items for EGPRS ====================== */

#define LOG_EGPRS_RLC_EPUAN_C                         (521  + APICOPY__LOG_GSM_BASE_C) /* 0x5209 */

#define LOG_GPRS_RLC_DL_STATS_C                       (522  + APICOPY__LOG_GSM_BASE_C) /* 0x520A */
#define LOG_GPRS_RLC_UL_RELEASE_IND_C                 (525  + APICOPY__LOG_GSM_BASE_C) /* 0x520D */
#define LOG_GPRS_RLC_DL_RELEASE_IND_C                 (526  + APICOPY__LOG_GSM_BASE_C) /* 0x520E */
#define LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C         (527  + APICOPY__LOG_GSM_BASE_C) /* 0x520F */
#define LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C         (528  + APICOPY__LOG_GSM_BASE_C) /* 0x5210 */

#define LOG_EGPRS_RLC_EPDAN_C                         (529  + APICOPY__LOG_GSM_BASE_C) /* 0x5211 */

#define LOG_GPRS_LLC_ME_INFO_C                        (530  + APICOPY__LOG_GSM_BASE_C) /* 0x5212 */
#define LOG_GPRS_LLC_SAPI_STATES_C                    (531  + APICOPY__LOG_GSM_BASE_C) /* 0x5213 */
#define LOG_GPRS_LLC_XID_INFO_C                       (532  + APICOPY__LOG_GSM_BASE_C) /* 0x5214 */
#define LOG_GPRS_LLC_PDU_STATS_C                      (533  + APICOPY__LOG_GSM_BASE_C) /* 0x5215 */
#define LOG_GPRS_LLC_PERIODIC_STATS_C                 (534  + APICOPY__LOG_GSM_BASE_C) /* 0x5216 */
#define LOG_GPRS_LLC_READY_TIMER_STATUS_C             (535  + APICOPY__LOG_GSM_BASE_C) /* 0x5217 */
#define LOG_GPRS_LLC_SEND_UI_FRAME_C                  (536  + APICOPY__LOG_GSM_BASE_C) /* 0x5218 *//*not in ICD*/
#define LOG_GPRS_LLC_RECV_UI_FRAME_C                  (537  + APICOPY__LOG_GSM_BASE_C) /* 0x5219 *//*not in ICD*/
#define LOG_GPRS_LLC_READY_TIMER_STATUS_VER2_C        (538  + APICOPY__LOG_GSM_BASE_C) /* 0x521A */


#define LOG_GPRS_SNDCP_ACTIVE_PDP_CONTEXTS_C          (540  + APICOPY__LOG_GSM_BASE_C) /* 0x521C */
#define LOG_GPRS_SNDCP_PDP_CONTEXT_INFO_C             (541  + APICOPY__LOG_GSM_BASE_C) /* 0x521D */
#define LOG_GPRS_SNDCP_STATES_C                       (542  + APICOPY__LOG_GSM_BASE_C) /* 0x521E */
#define LOG_GPRS_SNDCP_PDU_STATS_C                    (543  + APICOPY__LOG_GSM_BASE_C) /* 0x521F */
#define LOG_GPRS_SNDCP_UL_TCP_HDR_C                   (544  + APICOPY__LOG_GSM_BASE_C) /* 0x5220 */
#define LOG_GPRS_SNDCP_DL_TCP_HDR_C                   (545  + APICOPY__LOG_GSM_BASE_C) /* 0x5221 */

#define LOG_GPRS_RLC_UL_STATISTICS_C                  (546  + APICOPY__LOG_GSM_BASE_C) /* 0x5222 */
#define LOG_GPRS_RLC_DL_STATISTICS_C                  (547  + APICOPY__LOG_GSM_BASE_C) /* 0x5223 */

#define LOG_GPRS_MAC_SIGNALLING_MESSAGE_C             (550  + APICOPY__LOG_GSM_BASE_C) /* 0x5226 */
#define LOG_GPRS_MAC_STATE_C                          (551  + APICOPY__LOG_GSM_BASE_C) /* 0x5227 */
#define LOG_GPRS_MAC_UL_TBF_ESTABLISH_C               (552  + APICOPY__LOG_GSM_BASE_C) /* 0x5228 */
#define LOG_GPRS_MAC_UL_TBF_RELEASE_C                 (553  + APICOPY__LOG_GSM_BASE_C) /* 0x5229 */
#define LOG_GPRS_MAC_DL_TBF_ESTABLISH_C               (554  + APICOPY__LOG_GSM_BASE_C) /* 0x522A */
#define LOG_GPRS_MAC_DL_TBF_RELEASE_C                 (555  + APICOPY__LOG_GSM_BASE_C) /* 0x522B */
#define LOG_EGPRS_MAC_UL_ACKNACK_C                    (556  + APICOPY__LOG_GSM_BASE_C) /* 0x522C */
#define LOG_EGPRS_MAC_DL_ACKNACK_C                    (557  + APICOPY__LOG_GSM_BASE_C) /* 0x522D */
#define LOG_EGPRS_SRB_HARD_DECISIONS_C                (558  + APICOPY__LOG_GSM_BASE_C) /* 0x522E */


#define LOG_GPRS_SM_GMM_OTA_MESSAGE_C                 (560  + APICOPY__LOG_GSM_BASE_C) /* 0x5230 */
#define LOG_GPRS_AIR_INTERFACE_SUMMARY_C              (561  + APICOPY__LOG_GSM_BASE_C) /* 0x5231 */

#define LOG_GPRS_AGC_C                                (562  + APICOPY__LOG_GSM_BASE_C) /* 0x5232 */
#define LOG_GPRS_TIMING_ADVANCE_C                     (563  + APICOPY__LOG_GSM_BASE_C) /* 0x5233 */
#define LOG_GPRS_POWER_CONTROL_C                      (564  + APICOPY__LOG_GSM_BASE_C) /* 0x5234 */
#define LOG_GPRS_TRANSFER_SUMMARY_C                   (565  + APICOPY__LOG_GSM_BASE_C) /* 0x5235 */
//#define LOG_HOPPING_FREQUENCY_ARFCN_C             (566  + APICOPY__LOG_GSM_BASE_C) /* 0x5236 *//*NOT IMPLEMENTED*/
#define LOG_GPRS_TX_SCHEDULED_RESULTS_C               (567  + APICOPY__LOG_GSM_BASE_C) /* 0x5237 */

/* This is used in both GPRS and EGPRS */
#define LOG_UPLINK_TBF_DATA_BLOCK_COUNT_C             (575  + APICOPY__LOG_GSM_BASE_C) /* 0x523F */

#define LOG_GPRS_RECEIVE_BURST_METRICS_A_C            (581  + APICOPY__LOG_GSM_BASE_C) /* 0x5245 */
#define LOG_GPRS_RECEIVE_BURST_METRICS_B_C            (582  + APICOPY__LOG_GSM_BASE_C) /* 0x5246 */
#define LOG_GPRS_RECEIVE_BURST_METRICS_C_C            (583  + APICOPY__LOG_GSM_BASE_C) /* 0x5247 */
#define LOG_GPRS_RECEIVE_BURST_METRICS_D_C            (584  + APICOPY__LOG_GSM_BASE_C) /* 0x5248 */
#define LOG_GPRS_RECEIVE_BURST_METRICS_E_C            (585  + APICOPY__LOG_GSM_BASE_C) /* 0x5249 */
#define LOG_GPRS_RECEIVE_MSG_METRICS_B_C              (586  + APICOPY__LOG_GSM_BASE_C) /* 0x524A */
#define LOG_GPRS_RECEIVE_MSG_METRICS_C_C              (587  + APICOPY__LOG_GSM_BASE_C) /* 0x524B */
#define LOG_GPRS_RECEIVE_MSG_METRICS_D_C              (588  + APICOPY__LOG_GSM_BASE_C) /* 0x524C */
#define LOG_GPRS_RECEIVE_MSG_METRICS_A_VER2_C         (589  + APICOPY__LOG_GSM_BASE_C) /* 0x524D */
#define LOG_GPRS_RECEIVE_MSG_METRICS_B_VER2_C         (590  + APICOPY__LOG_GSM_BASE_C) /* 0x524E */
#define LOG_GPRS_RECEIVE_MSG_METRICS_C_VER2_C         (591  + APICOPY__LOG_GSM_BASE_C) /* 0x524F */
#define LOG_GPRS_RECEIVE_MSG_METRICS_D_VER2_C         (592  + APICOPY__LOG_GSM_BASE_C) /* 0x5250 */
#define LOG_GPRS_RECEIVE_MSG_METRICS_E_VER2_C         (595  + APICOPY__LOG_GSM_BASE_C) /* 0x5253 */

/* ==============   GSM MODE WCDMA MEASUREMENT PACKETS   ================== */
#define LOG_GSM_WCDMA_KNOWN_LIST_C                   (600 + APICOPY__LOG_GSM_BASE_C)  /* 0x5258 */
#define LOG_GSM_WCDMA_FILL_SAMPLE_RAM_C              (601 + APICOPY__LOG_GSM_BASE_C)
#define LOG_GSM_WCDMA_DED_CELL_LIST_C                (602 + APICOPY__LOG_GSM_BASE_C)

/* ==============   GSM MODE WCDMA MEASUREMEMTS REPORT FROM RR ============ */
#define LOG_GSM_DED_3G_MEASUREMENTS_C                (603 + APICOPY__LOG_GSM_BASE_C)/* 0x525B */

/* ============== GSM to WCDMA Reselection Rejected at WCDMA ============= */
#define LOG_GSM_IDLE_MODE_3G_RESELECT_REJ_C          (604 + APICOPY__LOG_GSM_BASE_C)/* 0x525C */

/* ==============   GSM/GPRS INTERRAT PACKETS            ================== */
#define LOG_GPRS_RR_3G_MEASUREMENT_PARAMS_C           (610  + APICOPY__LOG_GSM_BASE_C) /* 0x5262 */
#define LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C     (611  + APICOPY__LOG_GSM_BASE_C) /* 0x5263 */

/* =====================   GERAN COMMON LOG PACKETS   ===================== */
#define LOG_GERAN_F3_MESSAGES_C                       (612  + APICOPY__LOG_GSM_BASE_C) /* 0x5264: Not Implemented */

/* ======================================================================== */
/* User defined codes (starting at bit 1024, or 0x5400, incrementing by 0x10) */
#define LOG_GSM_FN_ADJ_C                             (0x400 + APICOPY__LOG_GSM_BASE_C)/* 0x5400 */
#define LOG_GSM_NEXT_USR_CODE_C                      (0x420 + APICOPY__LOG_GSM_BASE_C)/* 0x5420 */

/* ========================       WMS PACKETS      ======================== */
#define LOG_WMS_RECEIVE_CB                           (900  + APICOPY__LOG_GSM_BASE_C) /* 0x5384 */

/* ======================================================================== */
/* Last GSM code
   - Must be beyond the GPRS range or GPRS logging will not work */
#define LOG_GSM_LAST_C                               (LOG_GSM_NEXT_USR_CODE_C + 0x0A00)

#endif /* LOG_CODES_GSM_H */
