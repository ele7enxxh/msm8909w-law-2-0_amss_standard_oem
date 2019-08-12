#ifndef LOG_CODES_TDSCDMA_H
#define LOG_CODES_TDSCDMA_H

/*===========================================================================
testing
                         Log Code Definitions

General Description
  This file contains log code definitions for TDSCDMA.

Copyright (c) 2003-2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                             Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/log_codes_tdscdma.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $
   
when       who     what, where, why
--------   ---    ----------------------------------------------------------
10/05/11   mkh    Reverting as LOG_TDSCDMA_LAST_C already defined by DIAG 
09/14/11   mkh    Changed L2L3 log pkt ids
09/13/11   mkh    Fixed L2L3 log pkt id issue
07/26/11   bw	  Adjusted L1 log codes	
06/22/11   mkh    Added L2/L3 log packets' definitions
06/17/11   bw     Initial version log code definitions

===========================================================================*/

#include <log_codes.h>
#include "log_msgs_tdscdma.h"

/* ========================   LAYER 1 PACKETS   ======================== */

/*! @brief L1 log code base
    LOG_TDSCDMA_L1_BASE_C = 0xD000

    DO NOT CHANGE LOG PACKETS' LOG CODES. USE NEW LOGCODES INSTEAD.
*/
#define LOG_TDSL1_BASE_C                                  (LOG_TDSCDMA_BASE_C + 0x00)

//SRCH MEAS log packets
#define TDSCDMA_SYNCDL_MID_DETECT_LOG_PACKET                 (LOG_TDSL1_BASE_C + 0x00)

#define TDSCDMA_MEASUREMENT_RSCP_LOG_PACKET                  (LOG_TDSL1_BASE_C + 0x01)

#define TDSCDMA_MEASUREMENT_ISCP_LOG_PACKET                  (LOG_TDSL1_BASE_C + 0x02)

#define TDSCDMA_GAP_DETECT_LOG_PACKET                        (LOG_TDSL1_BASE_C + 0x03)

#define TDSCDMA_CELL_RESELECT_CFG_LOG_PACKET                 (LOG_TDSL1_BASE_C + 0x04)

#define TDSCDMA_MEASUREMENT_INTERNAL_CFG_LOG_PACKET          (LOG_TDSL1_BASE_C + 0x05)

#define TDSCDMA_MEASUREMENT_QUALITY_CFG_LOG_PACKET           (LOG_TDSL1_BASE_C + 0x06)

#define TDSCDMA_MEASUREMENT_INTERNAL_RESULT_LOG_PACKET       (LOG_TDSL1_BASE_C + 0x07)

#define TDSCDMA_MEASUREMENT_SCHEDULE_LOG_PACKET              (LOG_TDSL1_BASE_C + 0x08)

#define TDSCDMA_CELL_RESEL_RANK_LOG_PACKET                   (LOG_TDSL1_BASE_C + 0x09)

#define TDSCDMA_MEASUREMENT_ACCUMULATED_RSCP_LOG_PACKET      (LOG_TDSL1_BASE_C + 0x0A)

#define TDSCDMA_MEASUREMENT_ACCUMULATED_ISCP_LOG_PACKET      (LOG_TDSL1_BASE_C + 0x0B)

#define TDSCDMA_MEASUREMENT_QUALITY_RESULT_LOG_PACKET        (LOG_TDSL1_BASE_C + 0x0C)

#define TDSCDMA_MEASUREMENT_CFG_LOG_PACKET                   (LOG_TDSL1_BASE_C + 0x0D)

#define TDSCDMA_MEASUREMENT_GSM_RESULT_LOG_PACKET				     (LOG_TDSL1_BASE_C + 0x0E)

#define TDSCDMA_MEASUREMENT_GSM_TIMING_AND_INTERMEDIATE_RESULT_LOG_PACKET        (LOG_TDSL1_BASE_C + 0x0F)

//DL log packets
#define TDSCDMA_L1_DL_TRCH_CFG_LOG_PACKET                       (LOG_TDSL1_BASE_C + 0x10)
#define TDSCDMA_L1_DL_TFC_CFG_LOG_PACKET                        (LOG_TDSL1_BASE_C + 0x11)
#define TDSCDMA_L1_DL_COMMON_PHYCHAN_CFG_LOG_PACKET             (LOG_TDSL1_BASE_C + 0x12)
#define TDSCDMA_L1_DL_DPCH_CFG_LOG_PACKET                       (LOG_TDSL1_BASE_C + 0x13)
#define TDSCDMA_L1_DL_FMO_CFG_LOG_PACKET                        (LOG_TDSL1_BASE_C + 0x14)
#define TDSCDMA_L1_DL_FPACH_DECODE_RESULT_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x15)
#define TDSCDMA_L1_DL_BLER_LOG_PACKET                           (LOG_TDSL1_BASE_C + 0x16)
#define TDSCDMA_L1_DL_FW_MIDAMBLE_CFG_CMD_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x17)
#define TDSCDMA_L1_DL_FW_PICH_CFG_CMD_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x18)
#define TDSCDMA_L1_DL_FW_PCCPCH_CFG_CMD_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x19)
#define TDSCDMA_L1_DL_FW_SCCPCH_CFG_CMD_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x1A)
#define TDSCDMA_L1_DL_FW_DPCH_CFG_CMD_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x1B)
#define TDSCDMA_L1_DL_FW_RX_TIME_SYNC_CFG_CMD_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x1C)
#define TDSCDMA_L1_DL_FW_CCTRCH_CFG_CMD_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x1D)
#define TDSCDMA_L1_DL_FW_PICH_DECODE_RESULT_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x1E)
#define TDSCDMA_L1_DL_FW_DPCH_PCCPCH_SCCPCH_DECODE_RESULT_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x1F)
//UL log packets
#define TDSCDMA_L1_UL_DCH_CFG_LOG_PACKET                        (LOG_TDSL1_BASE_C + 0x20)
#define TDSCDMA_L1_UL_DCH_GAIN_FACTOR_CFG_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x21)
#define TDSCDMA_L1_UL_TFC_CFG_LOG_PACKET                        (LOG_TDSL1_BASE_C + 0x22)
#define TDSCDMA_L1_UL_DPCH_CFG_LOG_PACKET                       (LOG_TDSL1_BASE_C + 0x23)
#define TDSCDMA_L1_UL_RACH_CFG_LOG_PACKET                       (LOG_TDSL1_BASE_C + 0x24)
#define TDSCDMA_L1_UL_PRACH_FPACH_CFG_LOG_PACKET                (LOG_TDSL1_BASE_C + 0x25)
#define TDSCDMA_L1_UL_RANDOM_ACCESS_CFG_LOG_PACKET              (LOG_TDSL1_BASE_C + 0x26)
#define TDSCDMA_L1_UL_FW_UPPCH_FPACH_CFG_CMD_LOG_PACKET         (LOG_TDSL1_BASE_C + 0x27)
#define TDSCDMA_L1_UL_FW_PRACH_CFG_CMD_LOG_PACKET               (LOG_TDSL1_BASE_C + 0x28)
#define TDSCDMA_L1_UL_FW_DPCH_CFG_CMD_LOG_PACKET                (LOG_TDSL1_BASE_C + 0x29)

//RM log packets
#define TDSCDMA_L1_TF_POOL_PARAM_LOG_PACKET                     (LOG_TDSL1_BASE_C + 0x2D)
#define TDSCDMA_L1_TFC_POOL_PARAM_LOG_PACKET                    (LOG_TDSL1_BASE_C + 0x2F)

// HSDPA related log packets
#define TDSCDMA_L1_HSDPA_CFG_LOG_PACKET                         (LOG_TDSL1_BASE_C + 0x30)
#define TDSCDMA_L1_HSDPA_DECODE_STATUS_LOG_PACKET               (LOG_TDSL1_BASE_C + 0x31)
#define TDSCDMA_L1_HSDPA_HSSCCH_STAT_LOG_PACKET                 (LOG_TDSL1_BASE_C + 0x32)
#define TDSCDMA_L1_HSDPA_HARQ_STAT_LOG_PACKET                   (LOG_TDSL1_BASE_C + 0x33)
#define TDSCDMA_L1_HSDPA_MAC_HS_HDR_LOG_PACKET                  (LOG_TDSL1_BASE_C + 0x34)
#define TDSCDMA_L1_HSDPA_OL_CQI_LOG_PACKET                      (LOG_TDSL1_BASE_C + 0x35)

// HSUPA log packets
#define TDSCDMA_L1_UPA_EAGCH_CHAN_INFO_LOG_PACKET               (LOG_TDSL1_BASE_C + 0x40)
#define TDSCDMA_L1_UPA_SCHED_EHICH_CHAN_INFO_LOG_PACKET         (LOG_TDSL1_BASE_C + 0x41)
#define TDSCDMA_L1_UPA_NON_SCHED_EHICH_CHAN_INFO_LOG_PACKET     (LOG_TDSL1_BASE_C + 0x42) 
#define TDSCDMA_L1_UPA_NON_SCHED_GRANT_INFO_LOG_PACKET          (LOG_TDSL1_BASE_C + 0x43) 
#define TDSCDMA_L1_UPA_ETFC_RESTRICT_INFO_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x44) 
#define TDSCDMA_L1_UPA_HARQ_DB_INFO_LOG_PACKET                  (LOG_TDSL1_BASE_C + 0x45)
#define TDSCDMA_L1_UPA_EDL_DEC_AND_SG_INFO_LOG_PACKET           (LOG_TDSL1_BASE_C + 0x46)
#define TDSCDMA_L1_UPA_PWR_CTL_INFO_LOG_PACKET                  (LOG_TDSL1_BASE_C + 0x47)
#define TDSCDMA_L1_UPA_L1_STATS_INFO_LOG_PACKET                 (LOG_TDSL1_BASE_C + 0x48)
#define TDSCDMA_L1_UPA_EUL_CFG_LOG_PACKET                       (LOG_TDSL1_BASE_C + 0x49)

// SRCH DRX log packets
#define TDSCDMA_L1_DRX_CYCLE_LOG_PACKET                         (LOG_TDSL1_BASE_C + 0x50)
#define TDSCDMA_SCELL_TRACKING_STATUS_LOG_PACKET                (LOG_TDSL1_BASE_C + 0x51)
#define TDSCDMA_TS0_CELL_UPDATE_LOG_PACKET                      (LOG_TDSL1_BASE_C + 0x52) 
#define TDSCDMA_TS0_CELL_TABLE_LOG_PACKET                       (LOG_TDSL1_BASE_C + 0x53) 
#define TDSCDMA_NON_TS0_CELL_UPDATE_LOG_PACKET                  (LOG_TDSL1_BASE_C + 0x54)
#define TDSCDMA_NON_TS0_CELL_TABLE_LOG_PACKET                   (LOG_TDSL1_BASE_C + 0x55)
#define TDSCDMA_ACCUMULATE_TS0_CELL_TABLE_LOG_PACKET                       (LOG_TDSL1_BASE_C + 0x56) 
#define TDSCDMA_ACCUMULATE_NON_TS0_CELL_TABLE_LOG_PACKET                   (LOG_TDSL1_BASE_C + 0x57)
#define TDSCDMA_TS0_CELL_UPDATE_LOG_SHORTEN_PACKET                      (LOG_TDSL1_BASE_C + 0x58) 

#define TDSCDMA_IRAT_RSCP_LOG_PACKET                      (LOG_TDSL1_BASE_C + 0x5F)
//Absolute Priority based cell reselection log packets
#define TDSCDMA_MEASUREMENT_SIB19_CFG_LOG_PACKET                       (LOG_TDSL1_BASE_C + 0x59)
#define TDSCDMA_MEASUREMENT_EUTRA_CELLS_LOG_PACKET                     (LOG_TDSL1_BASE_C + 0x5A)
#define TDSCDMA_ABSOLUTE_PRIORITY_CELL_RANK_LOG_PACKET                 (LOG_TDSL1_BASE_C + 0x5B)

//OLPC log packets
#define TDSDLLOG_OLPC_CONFIG_LOG_PKT                            (LOG_TDSL1_BASE_C + 0x60)
#define TDSDLLOG_OLPC_UPDATE_LOG_PKT                            (LOG_TDSL1_BASE_C + 0x61)

#define TDSCDMA_L1_DL_SYNC_DETEC_LOG_PACKET                     (LOG_TDSL1_BASE_C + 0x62)
#define TDSCDMA_L1_INT_PCCPCH_LOG_PACKET                        (LOG_TDSL1_BASE_C + 0x63)

//tdsl1m profiling log packet
#define TDSCDMA_L1_L1M_PROFILING_LOG_PACKET                        (LOG_TDSL1_BASE_C + 0x64)

#define TDSCDMA_INTER_SYS_MEASUREMENT_CFG_LOG_PACKET                   (LOG_TDSL1_BASE_C + 0x65)
#define TDSCDMA_INTER_SYS_EUTRA_MEASUREMENT_CFG_LOG_PACKET            (LOG_TDSL1_BASE_C + 0x66)
#define TDSCDMA_INTER_SYS_EUTRA_IDLE_INTERVAL_CFG_LOG_PACKET					(LOG_TDSL1_BASE_C + 0x67)
#define TDSCDMA_DMO_CFG_LOG_PACKET										  (LOG_TDSL1_BASE_C + 0x68) 
#define TDSCDMA_T2L_TFW_GAP_CFG_LOG_PACKET															(LOG_TDSL1_BASE_C + 0x69)
#define TDSCDMA_T2L_EUTRA_MEASUREMENT_RESULT_LOG_PACKET								(LOG_TDSL1_BASE_C + 0x6A)
#define TDSCDMA_T2L_TIME_MEAS_SRCH_REQ_LOG_PACKET											(LOG_TDSL1_BASE_C + 0x6B)
#define TDSCDMA_T2L_EUTRA_MEASUREMENT_ACCUMULATE_RESULT_LOG_PACKET   (LOG_TDSL1_BASE_C + 0x6C)
#define TDSCDMA_RXD_LOG_PACKET									(LOG_TDSL1_BASE_C + 0x6D)

//DSDS L1 log packet
#define TDSCDMA_L1_DSDS_QTA_REPORT_LOG_PACKET                        (LOG_TDSL1_BASE_C + 0x6E)

//new added UL log packet
#define TDSCDMA_L1_UL_FW_UpPCH_TX_PWR_TIMING_CFG_LOG_PACKET        (LOG_TDSL1_BASE_C + 0x6F)
#define TDSCDMA_L1_UL_FW_DPCH_TX_PWR_TIMING_CFG_LOG_PACKET         (LOG_TDSL1_BASE_C + 0x70)
#define TDSCDMA_L1_UL_ERUCCH_SI_DATA_LOG_PACKET                     (LOG_TDSL1_BASE_C + 0x71)
#define TDSCDMA_L1_UL_FW_ERUCCH_CFG_CMD_LOG_PACKET                  (LOG_TDSL1_BASE_C + 0x72)

#define TDSCDMA_L1_MCPM_POWER_REPORTING_LOG_PACKET               (LOG_TDSL1_BASE_C + 0x73)

/* ========================   LAYER 2 / LAYER 3 PACKETS   ======================= */
   /*! @brief   TDSCDMA is assigned log packets from 0xD000 - 0xD0FF
   L2/L3 is assigned log packets from 0xD080 - 0xD0FF
   RLC uses 0xD080 - 0xD0AF from the assigned ones (D08*, D09*, D0A*)
   MAC uses 0xD0B0 - 0xD0DF from the assigned ones (D0B*, D0C*, D0D*)
   RRC uses 0xD0E0 - 0xD0FF from the assigned ones (D0E*, D0F*) 
    
   LOG_TDSCDMA_BASE_C = 0xD000 
   */
/* =============================================================================== */

/* ========================   LAYER 2 PACKETS   ======================== */
/* DO NOT CHANGE LOG PACKETS' LOG CODES. USE NEW LOGCODES INSTEAD. */

/*! @brief RLC packets */

#define TDSCDMA_UL_RLC_STATE_LOG_PACKET                    (LOG_TDSCDMA_BASE_C + 0x080) 
#define TDSCDMA_UL_RLC_TM_LOG_PACKET                       (LOG_TDSCDMA_BASE_C + 0x081)   
#define TDSCDMA_UL_RLC_UM_LOG_PACKET                       (LOG_TDSCDMA_BASE_C + 0x082)   
#define TDSCDMA_DL_RLC_STATE_LOG_PACKET                    (LOG_TDSCDMA_BASE_C + 0x083)   
#define TDSCDMA_DL_RLC_TM_LOG_PACKET                       (LOG_TDSCDMA_BASE_C + 0x084) 
#define TDSCDMA_DL_RLC_UM_LOG_PACKET                       (LOG_TDSCDMA_BASE_C + 0x085)  
#define TDSCDMA_DL_RLC_AM_LOG_PACKET                       (LOG_TDSCDMA_BASE_C + 0x086)   
#define TDSCDMA_UL_RLC_AM_LOG_PACKET                       (LOG_TDSCDMA_BASE_C + 0x087)
#define TDSCDMA_DL_RLC_AM_USR_PLANE_PDU_LOG_PACKET         (LOG_TDSCDMA_BASE_C + 0x088)
#define TDSCDMA_DL_RLC_AM_SIG_PLANE_PDU_LOG_PACKET         (LOG_TDSCDMA_BASE_C + 0x089)   
#define TDSCDMA_UL_RLC_AM_STAT_LOG_PACKET                  (LOG_TDSCDMA_BASE_C + 0x08A)
#define TDSCDMA_UL_RLC_AM_USR_PLANE_PDU_LOG_PACKET         (LOG_TDSCDMA_BASE_C + 0x08B)
#define TDSCDMA_UL_RLC_AM_SIG_PLANE_PDU_LOG_PACKET         (LOG_TDSCDMA_BASE_C + 0x08C)
#define TDSCDMA_UL_RLC_AM_NAK_PDU_LOG_PACKET               (LOG_TDSCDMA_BASE_C + 0x08D)   
#define TDSCDMA_DL_RLC_AM_NAK_PDU_LOG_PACKET               (LOG_TDSCDMA_BASE_C + 0x08E)   
#define TDSCDMA_DL_RLC_AM_STAT_LOG_PACKET                  (LOG_TDSCDMA_BASE_C + 0x08F)  
#define TDSCDMA_UL_RLC_CIPH_CFG_LOG_PACKET                 (LOG_TDSCDMA_BASE_C + 0x090) 
#define TDSCDMA_DL_RLC_CIPH_CFG_LOG_PACKET                 (LOG_TDSCDMA_BASE_C + 0x091) 
#define TDSCDMA_DL_RLC_PDU_CIPHER_LOG_PACKET               (LOG_TDSCDMA_BASE_C + 0x092)  
#define TDSCDMA_UL_RLC_PDU_CIPHER_LOG_PACKET               (LOG_TDSCDMA_BASE_C + 0x093)
#define TDSCDMA_DL_FLEXIBLE_RLC_AM_PDU_LOG_PACKET          (LOG_TDSCDMA_BASE_C + 0x094)
#define TDSCDMA_UL_RLC_UM_USR_PLANE_PDU_LOG_PACKET         (LOG_TDSCDMA_BASE_C + 0x095)
#define TDSCDMA_DL_RLC_UM_USR_PLANE_PDU_LOG_PACKET         (LOG_TDSCDMA_BASE_C + 0x096)

/*! @brief MAC packets */

#define TDSCDMA_MAC_UL_TVM_REPORT_LOG_PACKET               (LOG_TDSCDMA_BASE_C + 0x0B0)
#define TDSCDMA_MAC_UL_TVM_CFG_LOG_PACKET                  (LOG_TDSCDMA_BASE_C + 0x0B1)
#define TDSCDMA_UL_MAC_CFG_LOG_PACKET                      (LOG_TDSCDMA_BASE_C + 0x0B2)
#define TDSCDMA_DL_MAC_CFG_LOG_PACKET                      (LOG_TDSCDMA_BASE_C + 0x0B3)
#define TDSCDMA_MAC_RACH_CFG_LOG_PACKET                    (LOG_TDSCDMA_BASE_C + 0x0B4)
#define TDSCDMA_MAC_UL_TFCS_CFG_LOG_PACKET                 (LOG_TDSCDMA_BASE_C + 0x0B5) 
#define TDSCDMA_MAC_HS_CONFIG_LOG_PACKET                   (LOG_TDSCDMA_BASE_C + 0x0B6)
#define TDSCDMA_MAC_HS_STATUS_REPORT_LOG_PACKET            (LOG_TDSCDMA_BASE_C + 0x0B7)
#define TDSCDMA_MAC_HS_RESET_LOG_PACKET                    (LOG_TDSCDMA_BASE_C + 0x0B8)
#define TDSCDMA_MAC_HS_REASSEMBLY_LOG_PACKET               (LOG_TDSCDMA_BASE_C + 0x0B9)
#define TDSCDMA_MAC_HS_UL_CONFIG_LOG_PACKET                (LOG_TDSCDMA_BASE_C + 0x0BA)
#define TDSCDMA_MAC_HS_UL_HEADER_STATUS_LOG_PACKET         (LOG_TDSCDMA_BASE_C + 0x0BB)
#define TDSCDMA_MAC_HS_UL_ETFCI_STATUS_LOG_PACKET          (LOG_TDSCDMA_BASE_C + 0x0BC)

/* ========================   LAYER 3 PACKETS   ======================== */
/* DO NOT CHANGE LOG PACKETS' LOG CODES. USE NEW LOGCODES INSTEAD.*/

/*! @brief RRC Packets */

#define TDSCDMA_RRC_MODES_AND_STATES_LOG_PACKET            (LOG_TDSCDMA_BASE_C + 0x0E0)
#define TDSCDMA_RRC_PROTOCOL_ERRORS_LOG_PACKET             (LOG_TDSCDMA_BASE_C + 0x0E1)
#define TDSCDMA_RRC_CELL_ID_LOG_PACKET                     (LOG_TDSCDMA_BASE_C + 0x0E2)
#define TDSCDMA_RRC_SIGNALLING_MESSAGE_LOG_PACKET          (LOG_TDSCDMA_BASE_C + 0x0E3)
#define TDSCDMA_RRC_RB_RATE_INFO_LOG_PACKET                (LOG_TDSCDMA_BASE_C + 0x0E4)
#define TDSCDMA_RRC_ACQ_INFO_LOG_PACKET                    (LOG_TDSCDMA_BASE_C + 0x0E5)
#define TDSCDMA_RRC_DSDS_LTA_LOG_PACKET                    (LOG_TDSCDMA_BASE_C + 0x0E6)
/*============================ LOG CODE LIMIT CHECK ===================== */

//#define LOG_TDSCDMA_LAST_C   ((uint16) 0xD0FF)

#endif /* LOG_CODES_TDSCDMA_INT_H */

