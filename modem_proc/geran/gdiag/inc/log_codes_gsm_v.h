#ifndef LOG_CODES_GSM_V_H
#define LOG_CODES_GSM_V_H
/*===========================================================================

                         Log Code Definitions

General Description
  This file contains log code definitions and is shared with the tools.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                             Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdiag/inc/log_codes_gsm_v.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
24/03/14   dg      CR626860 - New log packets by adding timeslot information
11/07/14   pjr     CR 692177 Added support for RFACCH 2.0
23/01/14   sp      CR588783 - Update log packet Id to map with APEX reqs
24/09/13   sp      CR 449651 - New log packets for Burst Metrics.
24/09/13   sp      CR482765 - Define new packet for EDGE Enhanced Log update.
13/03/12   ws      CR 330862 - Change dedicated mode F3 level's and add sacch burst
                   metrics and compact message metrics log packets.
15/01/08   ws      Added LOG_GPRS_ESAIC_METRICS_C
17/09/07   agv     Added I/Q logging to code base.
24/07/06   zyu     Added LOG_EGPRS_SRB_HARD_DECISIONS_C 0x522E
12/20/05   gfr     Added LOG_GSM_SAIC_METRICS_C
26/08/05   zyu     Resolve the issue of log code reuse. (They cannot be reused!)
                   LOG_UPLINK_TBF_DATA_BLOCK_COUNT_C is reallocated a new
                   code  0x523F.
                   Comments out lint errors: code repeated, and not implemented code.
25/08/05   zyu     Put in comments on current status of log codes wrt ICD and their HEX
                   code for mapping them, which will help for future log code allocations.
23/06/05   og      Code changes to accomodate the uplink tbf data block count
                   log packet.
03/14/05   yh      Added LOG_GSM_MDSP_APPS_C
20/09/04   zyu     Added 8 new log packets for EGPRS logging
09/20/04   gfr     Added LOG_GSM_MDSP_DEBUG_BUFFER_C
09/09/04   dp      Added LOG_GSM_TRANSMIT_BURST_METRICS_C
06/09/04   gfr     Added LOG_GSM_SCH_MSG_METRICS_VER2_C.
04/28/04   Rex     Added log code for new WMS packet LOG_WMS_RECEIVE_CB
02/17/04   gfr     Added log code for new AFC packet.
01/30/04   gfr     Added log code for mdsp scaling factors.
01/28/04   gw      Added log codes for GSM auxiliary measurements.
12/05/03   bm      Added logging support for MDSP commands issued,
                   and the corresponding pops
09/08/93   ws      Added DL PTCCH message and burst metrics
08/01/03   tb      Added more GtoW log codes.
07/07/03   ws      Added LOG_GPRS_RECEIVE_MSG_METRICS_A_VER2_C to
                   LOG_GPRS_RECEIVE_MSG_METRICS_D_VER2_C
03/27/03   gfr     Added LOG_GPRS_HW_SCHEDULE_C and LOG_GSM_MDSP_CMD_C
01/23/03   ATM     Added more suggested packets for systems
01/22/03   tb      added LOG_GSM_WCDMA_KNOWN_LIST_C
11/18/02   ATM     Added enhance PDM_ADJUST and SACCH_REPORT packets
11/05/02   WS      Added GPRS L1 log codes
10/07/02   HE      Added GPRS log codes.
09/06/02   ATM     Added USER code space
07/24/02   ATM     Added NCELL_BCCH_DECODE packet
07/11/02   ATM     Added GSTMR, PDM, NCELL acq packets for systems group
07/09/02   ATM     Added GPRS log code range
05/08/02   ATM     Split Rx_Metrics into Message/Burst packets
04/30/02   ATM     Added SERVING_CELL_POWER packet
04/17/02   ATM     Added RECEVIE_TRAFFIC_METRICS packet
04/02/02   ATM     Added CSDATA packets
03/29/02   ATM     Added NCELL packets
03/08/02   ATM     Added jachan's L2 packets and renamed some RR packets
02/04/02   ATM     Cleanup -- renamed codes to fit convention
01/24/02   ATM     Created log codes used by GSM

===========================================================================*/

#include "log_codes.h" // for LOG_GSM_BASE_C
#include "log_codes_gsm.h"
/* ======================================================================== */
// GSM general log codes
#define LOG_GSM_PLT_PKT_C                             (  0  + LOG_GSM_BASE_C) /* 0x5000 */
#define LOG_GSM_OBSOLETE_CFA_C                        (  1  + LOG_GSM_BASE_C) /* 0x5001 */
#define LOG_GSM_AGC_PARAMS_C                          (103  + LOG_GSM_BASE_C)/* 0x5067 */
#define LOG_GSM_AFC_PARAMS_C                          (104  + LOG_GSM_BASE_C)/* 0x5068 */
#define LOG_GSM_PC_DATA_C                             (106  + LOG_GSM_BASE_C)/* 0x506A */
#define LOG_GSM_RECEIVE_MSG_METRICS_C                 (107  + LOG_GSM_BASE_C)/* 0x506B */
#define LOG_GSM_MEASUREMENT_REPORT_C                  (109  + LOG_GSM_BASE_C)/* 0x506D */
#define LOG_GSM_SURROUNDING_CELL_INFO_C               (110  + LOG_GSM_BASE_C)/* 0x506E */
#define LOG_GSM_PDM_ADJ_C                             (116  + LOG_GSM_BASE_C) /* obsoleted by 120 */
#define LOG_GSM_NCELL_BCCH_DECODE_C                   (119  + LOG_GSM_BASE_C)
#define LOG_GSM_PDM_ADJUST_C                          (120  + LOG_GSM_BASE_C) /* obsoleted by 124 */
#define LOG_GSM_SCH_MSG_METRICS_C                     (132  + LOG_GSM_BASE_C) /* obsoleted by 135 *//* 0x507F */
#define LOG_GSM_MDSP_SCALING_FACTORS_C                (142  + LOG_GSM_BASE_C)
#define LOG_GSM_MDSP_APPS_C                           (144  + LOG_GSM_BASE_C)/* 0x5090 not in ICD*/
#define LOG_GSM_RECEIVE_SACCH_BURST_METRICS_C         (160  + LOG_GSM_BASE_C)/* 0x50A0 */
#define LOG_GSM_RECEIVE_MSG_METRICS_COMPACT_C         (161  + LOG_GSM_BASE_C)/* 0x50A1 */
#define LOG_GSM_RR_LINK_FAILURE_DETECTION_C           (301  + LOG_GSM_BASE_C) /* 0x512D *//*not in ICD*/
#define LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C            (317  + LOG_GSM_BASE_C) /* 0x513d *//* Not in ICD */
#define LOG_GSM_RR_IRAT_MEASUREMENTS_C                (318  + LOG_GSM_BASE_C) /* 0x513E */
#define LOG_GSM_RR_IRAT_PRIORITY_LIST_C               (319  + LOG_GSM_BASE_C) /* 0x513F */
/* ========================   GSM CS DATA PACKETS  ======================== */
#define LOG_GSM_CS_DATA_RX_C                          (400  + LOG_GSM_BASE_C) /* 0x5190 */
#define LOG_GSM_CS_DATA_TX_C                          (401  + LOG_GSM_BASE_C)
#define LOG_EGPRS_RLC_UL_STATS_C                      (519  + LOG_GSM_BASE_C) /* 0x5207 *//*not implemented*/
#define LOG_EGPRS_RLC_DL_STATS_C                      (520  + LOG_GSM_BASE_C) /* 0x5208 *//*not implemented*/
#define LOG_GPRS_RLC_UL_ACKNACK_PARAMS_C              (523  + LOG_GSM_BASE_C) /* 0x520B */
#define LOG_GPRS_RLC_DL_ACKNACK_PARAMS_C              (524  + LOG_GSM_BASE_C) /* 0x520C */

/* GERAN IQ LOGGING CODE */
#define LOG_MDSP_IQ_SAMPLES_C                         (559  + LOG_GSM_BASE_C) /* 0x522F */
#define LOG_GPRS_HW_SCHEDULE_C                        (570  + LOG_GSM_BASE_C) /* 0x523A *//*not in ICD*/
#define LOG_GPRS_DLPTCCH_BURST_METRICS_C              (593  + LOG_GSM_BASE_C) /* 0x5251 */
#define LOG_GPRS_DLPTCCH_MSG_METRICS_C                (594  + LOG_GSM_BASE_C) /* 0x5252 */

#define LOG_GSM_RECEIVE_MSG_METRICS_RFACCH_C          (609  + LOG_GSM_BASE_C) /* 0x5261 */

#define LOG_GPRS_EDGE_ENHANCED_C                      (105  + LOG_GSM_BASE_C) /* 0x5069 - Edge enhanced Hdr Logging*/

#define LOG_GPRS_LAST_C                               LOG_GPRS_RECEIVE_MSG_METRICS_D_C /*Clearly it is not. Not used anywhere*/

#define LOG_GSM_RECEIVE_BURST_METRICS_VER2_C               (106  + APICOPY__LOG_GSM_BASE_C)/* 0x506A */
#define LOG_GPRS_RECEIVE_BURST_METRICS_A_VER2_C            (596  + APICOPY__LOG_GSM_BASE_C) /* 0x5254 */
#define LOG_GPRS_RECEIVE_BURST_METRICS_B_VER2_C            (597  + APICOPY__LOG_GSM_BASE_C) /* 0x5255 */
#define LOG_GPRS_RECEIVE_BURST_METRICS_C_VER2_C            (598  + APICOPY__LOG_GSM_BASE_C) /* 0x5256 */
#define LOG_GPRS_RECEIVE_BURST_METRICS_D_VER2_C            (599  + APICOPY__LOG_GSM_BASE_C) /* 0x5257 */

#define LOG_GSM_TRANSMIT_BURST_METRICS_VER1_C              (137  + APICOPY__LOG_GSM_BASE_C)/* 0x5089 */

#define LOG_GSM_SACCH_RPT_C                          (0x410 + LOG_GSM_BASE_C) /* formalized as 314 *//* 0x5410 */

/* this value is used to increment the GSM log ID values */
/* which tells the diag tools that they are for Dual SIM */
#define LOG_GSM_DUAL_SIM_CODE_OFFSET  0x0A00

#endif /* LOG_CODES_GSM_V_H */
