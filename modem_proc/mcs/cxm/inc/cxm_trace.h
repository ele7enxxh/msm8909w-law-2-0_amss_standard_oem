#ifndef CXM_TRACE_H
#define CXM_TRACE_H
/*!
  @file
  cxm_trace.h

  @brief
  APIs for CXM's debug trace logging

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/cxm_trace.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/15   ckk     Add support for Tx antenna via WCI2 MWS_Connected Type 7
08/14/14   cdh     Diag log packet functionality added
24/10/13   tak     Added functions for returning counter info for diag
12/20/12   btl     Port over
10/31/12   ckk     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <atomic_ops.h>
#include <IxErrno.h>
#include <comdef.h>
#include <stdarg.h>
#include "cxm_log_types.h"
#include "log.h"

/*===========================================================================

                       DATA TYPES AND MACROS

===========================================================================*/
/* Number of events in circular buffer
 * Assume 4 events/ms * 1000 ms/s * 0.5 s = 2000 */
#define CXM_TRACE_NUM_OF_EVENTS  1024

/*! @brief CXM trace events (last updated on 03/22/2013) */
typedef enum
{
  CXM_TRC_INVALID,              /* [0, 0, 0] */
  CXM_TRC_TSK_INIT,             /* [0, 0, 0] */
  CXM_TRC_TSK_DEINIT,           /* [0, 0, 0] */

  CXM_TRC_MSGR_RCV_MSG,                /* [0, 0, MSGR_UMID] */
  CXM_TRC_MSGR_SND_MSG,                /* [0, 0, MSGR_UMID] */
  CXM_TRC_MSGR_WWAN_TECH_STATE_INFO,   /* [WWAN_TECH_ID, IF_STATE_CHANGED, #_INFO_SETS] */
  CXM_TRC_MSGR_WWAN_TECH_SLEEP_WAKEUP, /* [WWAN_TECH_ID, IS_GOING_TO_SLEEP, DURATION] */
  CXM_TRC_MSGR_LTE_SFN,       /* [LTE_ADV_TX_SFN, 0, 0] */
  CXM_TRC_MSGR_LTE_BLER,      /* [0, TB_ERR_CNT, TB_CNT] */
  CXM_TRC_MSGR_LTE_SINR,      /* [SYS_ST, SINR_VALID, SINR] */
  CXM_TRC_MSGR_LTE_TX_PWR_LMT,/* [PATH_LOSS, FILTERED_RB_Q8, RRC_STATE] */
  CXM_TRC_MSGR_LTE_RRC_STATE, /* [RRC_STATE, CTLR_PWR_LMT_STATE, WCI2_PWR_LMT_SATE] */
  CXM_TRC_MSGR_WCN_RPT,       /* [ERR_TYPE, #_OF_WCN_REQs, 0] */

  CXM_TRC_QMI_SVC_INIT,           /* [SVC_ID, SVC_IDL_MAJOR_VER, SVC_IDL_MINOR_VER] */
  CXM_TRC_QMI_SVC_DEINIT,         /* [SVC_ID, QMI_estatus, 0] */
  CXM_TRC_QMI_SVC_OPEN,           /* [NAME, SVC_ID, SVC_QMI_HANDLE] */
  CXM_TRC_QMI_SVC_CLOSE,          /* [SVC_ID, RETVAL, 0] */
  CXM_TRC_QMI_CLNT_CONNECT_CB,    /* [SVC_ID, CLNT_INDX, RETVAL] */
  CXM_TRC_QMI_CLNT_DISCONNECT_CB, /* [SVC_ID, CLNT_PTR, 0] */
  CXM_TRC_QMI_SVC_HNDL_REQ_CB,    /* [SVC_ID, MSG_ID, RETVAL] */
  CXM_TRC_QMI_SND_IND,            /* [SVC_ID, MSG_ID, #_OF_RCVRs] */
  CXM_TRC_QMI_SND_RESP,           /* [MSG_ID, CLNT_PTR, RETVAL] */
  CXM_TRC_QMI_PUSH_DEF_REQ,       /* [SVC_ID, DEF_QID, RETVAL] */
  CXM_TRC_QMI_POP_DEF_REQ,        /* [SVC_ID, DEF_QID, RETVAL] */

  CXM_TRC_QMI_HNDL_REG_IND_REQ,   /* [CLNT_PTR, CLNT_IND_MAP, RETVAL] */
  CXM_TRC_QMI_SET_POLICY_REQ,     /* [POLICY, 0, 0] */
  CXM_TRC_QMI_STRT_BLER_REQ,      /* [TB_CNT, THLD_ERR_TB_CNT, 0] */
  CXM_TRC_QMI_STRT_SINR_REQ,      /* [APLHA, 0, 0] */
  CXM_TRC_QMI_LTE_TX_PWR_LMT_SUCC,/* [TX_PWR_LMT_SUCC_STATE, SENT_MASK, DROP_COUNT] */
  CXM_TRC_QMI_LTE_TX_PWR_LMT_FAIL,/* [CTLR_TX_PWR_LMT_FAIL_STATE, WCI2_TX_PWR_LMT_FAIL_STATE, DROP_COUNT] */
  CXM_TRC_QMI_TX_SUBFRAME_DENIAL, /* [TX_SUBFRAME_DENIAL_REASON, SENT_MASK, DROP_COUNT] */
  CXM_TRC_UART_RCV_MSG,           /* [BYTE, FIFO_INDX, RX_FIFO_PTR] */
  CXM_TRC_UART_SND_MSG,           /* [MSG_TYPE, DATA1, [DATA2]] */
  CXM_TRC_QMI_UPDATE_WLAN_CONN,   /* [CONN_INFO_HNDLE, CONN_INFO_STATE, CONN_INFO_MODE] */
  CXM_TRC_QMI_UPDATE_WLAN_HP,     /* [HP_INFO_ID, HP_INFO_STATE, HP_INFO_BAND_FREQ] */
  CXM_TRC_COEX_CONFLICT_START,    /* [lte_freq, wlan_freq, rb_tbl_index] */
  CXM_TRC_COEX_CONFLICT_END,      /* [0 <- later, pointer to conflict?, 0, 0] */
  CXM_TRC_SET_LTE_TX_PWR_LMT,     /* [NEW_LIMIT, OLD_LIMIT, RF_RETVAL] */
  CXM_TRC_QMI_SET_SLP_NOTFY_THRESH, /* [tech, off_period_thresh, 0] */
  CXM_TRC_QMI_SND_WWAN_STATE_IND, /* [retval, 0, 0] */
  CXM_TRC_ERR_FATAL,              /* [MSG_TYPE, DATA1, [DATA2]] */

  CXM_TRC_ANT_RELEASE_AND_REQUEST, /* [client_id, req_antenna, req_durn] */
  CXM_TRC_ANT_GRANT_CB,            /* [0, grant_return, duration] */
  CXM_TRC_ANT_UNLOCK_CB,           /* [event, sclk_delta, unlock_by_sclk] */

  CXM_TRC_SMDL_RCV_NOT,            /* [retval, 0, 0] */
  CXM_TRC_SMDL_RCV_MSG,            /* [retval, 0, 0] */
  CXM_TRC_SMDL_SND_MSG,            /* [retval, 0, 0] */

  CXM_TRC_WCI2_RX_CB,              /* [status, 0, 0] */
  CXM_TRC_WCI2_PROC_RX,            /* [BYTE, TYPE, 0] */
  CXM_TRC_ANT_IRAT_CB              /* [IRAT_TECH, 0, DUR] */

} cxm_trace_event_types_e;

/*! @brief CXM trace event record/structure 
    @note    Current structure size is 16 bytes & is packed tight */
typedef struct
{
  uint8  event; /*!< 8-bit event ID (cxm_trace_event_types_e) */
  uint8  a8;    /*!< 1st of 3 opaque payloads (8-bit) */
  uint16 b16;   /*!< 2nd of 3 opaque payloads (16-bit) */
  uint32 c32;   /*!< 3rd of 3 opaque payloads (32-bit) */
  uint64 ts;    /*!< timestamp in slowclock ticks */
} cxm_trace_event_log_s;

/*! @brief CXM trace structure */
typedef struct
{
  /*!< Sequence number of the log entry in the table 
       Note: Last location written to in circular "events" trace buffer
             can be calculated by [seq_num % CXM_TRACE_NUM_OF_EVENTS]
  */
  atomic_word_t         seq_num; 
  /*! Circular "events" trace buffer */
  cxm_trace_event_log_s events[ CXM_TRACE_NUM_OF_EVENTS ];
  /*!< Version number to help keep script in step */
  uint8                 version_number;
} cxm_trace_event_s;

/* Helper macros for determining number of variable arguments to CXM_TRACE */
#define VA_NUM_ARGS(...) VA_NUM_ARGS__(__VA_ARGS__,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1)
#define VA_NUM_ARGS__(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,N,...) (uint8)(N)

/* Macro for cxm trace and log packet debug systems.  The CXM_TRACE
   macro accepts AT LEAST 5 arguments and NO MORE THAN 20 total arguments.
   The first (MANDATORY) argument after c32 is a flag specifying whether
   to perform simple trace buffer logging, diag log packetting, or a 
   combination.  When log packets are desired, the next 1 to 15 arguments
   are enumerated values corresponding to the specific structures for 
   capturing in log packets.

   Example usage:
   CXM_TRACE( CXM_TRC_TSK_INIT, 0, 0, 0, CXM_TRC_AND_PKT_EN,
             CXM_LOG_PARAMS_NO_TABLES, CXM_LOG_PARAMS_PORT_TABLE, CXM_LOG_PARAMS_VICTIM_TBL );
   CXM_TRACE( CXM_TRC_QMI_UPDATE_WLAN_SCAN, request->scan_info.id,
             request->scan_info.state, request->scan_info.band.freq,
             CXM_TRC_EN );
*/
#define CXM_TRACE(event_id, a8, b16, c32, ...) \
  cxm_trace_event((cxm_trace_event_types_e)event_id, (uint8)(a8), (uint16)(b16), (uint32)(c32), VA_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

/* CXM counters enum -- list of all counters.
 * NOTE: if more are added, add them at the end */
typedef enum
{
  CXM_CNT_POLICY_RESET,
  CXM_CNT_POLICY_UPDATES,
  CXM_CNT_UART_RX,
  CXM_CNT_UART_RX_WCI2_TYPE1,
  CXM_CNT_UART_RX_WCI2_TYPE6,
  CXM_CNT_UART_RX_WCI2_TYPE_UNSUPPORTED,
  CXM_CNT_UART_TX,
  CXM_CNT_UART_TX_WCI2_TYPE1,
  CXM_CNT_UART_TX_WCI2_TYPE3,
  CXM_CNT_UART_TX_WCI2_TYPE6,
  CXM_CNT_UART_TX_WCI2_TYPE_UNSUPPORTED,
  CXM_CNT_LTE_ML1_ADVANCE_NOTICE_RECVD,
  CXM_CNT_LTE_ML1_SLEEP_DURN_RECVD,
  CXM_CNT_CONTR_PWR_LMT_REQ,
  CXM_CNT_CONTR_PWR_LMT_COND_FAIL,
  CXM_CNT_CONTR_PWR_LMT_HONORED,
  CXM_CNT_WCI2_PWR_LMT_REQ,
  CXM_CNT_WCI2_PWR_LMT_COND_FAIL,
  CXM_CNT_WCI2_PWR_LMT_HONORED,
  CXM_CNT_LATE_LTE_TX_MSG,
  CXM_CNT_TX_ADV_TRANSMIT,
  CXM_CNT_PLCY_RSRVD_FOR_CTRLR,
  
  CXM_CNT_MAX_COUNTERS /* placeholder - always last */
} cxm_counters_e;

/* CXM counter instance structure */
typedef struct
{
  uint32 count;
  uint32 last_value;
  uint32 last_tstamp;
} cxm_counter_type_s;

/* Masks to determine which log types to use */
#define CXM_TRC_EN         ((uint8)0x01)
#define CXM_LOG_PKT_EN     ((uint8)0x02)
#define CXM_ASYNC_PKT_EN   ((uint8)0x04)
#define CXM_TRC_AND_PKT_EN ( CXM_TRC_EN | CXM_LOG_PKT_EN )

/* Maximum number of packets in one log data array before pushing out */
#define CXM_LOW_PRIO_MAX_PACKETS  15
#define CXM_MED_PRIO_MAX_PACKETS  10
#define CXM_HIGH_PRIO_MAX_PACKETS 3
#define CXM_SMEM_DATA_MAX_PACKETS 8

/* Define log packet version and subcode */
#define CXM_LOG_PKT_VERSION (uint8)1
#define CXM_LOG_PKT_SUBCODE (uint8)1

/* Enumeration of low prio log packet payloads */
typedef enum 
{
  CXM_LOG_MIN_LOW_PRIO = 0x0000FFFF,         /* 32-bit offset */
  CXM_LOG_STATE,                             // 0x0000
  CXM_LOG_CONTROLLER_PWR_LMT_STATE,          // 0x0001
  CXM_LOG_WCI2_PWR_LMT_STATE,                // 0x0002
  CXM_LOG_METRICS_LTE_BLER_STARTED,          // 0x0003
  CXM_LOG_METRICS_LTE_SINR_STARTED,          // 0x0004
  CXM_LOG_METRICS_GSM1_SINR_STARTED,         // 0x0005
  CXM_LOG_METRICS_GSM2_SINR_STARTED,         // 0x0006
  CXM_LOG_METRICS_GSM3_SINR_STARTED,         // 0x0007
  CXM_LOG_METRICS_TDS_NRRWBND_NOISE_STARTED, // 0x0008
  CXM_LOG_ANTENNA_STATE,                     // 0x0009
  CXM_LOG_WCN_TX_STATE,                      // 0x000A
  CXM_LOG_LTE_TX_ADV,                        // 0x000B
  CXM_LOG_TECH_METRICS_READ,                 // 0x000C
  CXM_LOG_LTE_BLER_READ,                     // 0x000D
  CXM_LOG_HIGH_PRIO_INFO,                    // 0x000E
  CXM_LOG_MAX_LOW_PRIO,                      /* Current maximum */
  CXM_LOG_UNSIGNED32_LP = 0x0001FFFE         /* Force uint32 type */
} cxm_low_prio_log_type_e;

/* Enumeration of med prio log packet payloads */
typedef enum
{
  CXM_LOG_MIN_MED_PRIO = 0x0001FFFF,         /* 32-bit offset */
  CXM_LOG_LTE_WWAN_STATE_INFO,               // 0x0000
  CXM_LOG_TDS_WWAN_STATE_INFO,               // 0x0001
  CXM_LOG_GSM1_WWAN_STATE_INFO,              // 0x0002
  CXM_LOG_ONEX_WWAN_STATE_INFO,              // 0x0003
  CXM_LOG_HDR_WWAN_STATE_INFO,               // 0x0004
  CXM_LOG_WCDMA_WWAN_STATE_INFO,             // 0x0005
  CXM_LOG_GSM2_WWAN_STATE_INFO,              // 0x0006
  CXM_LOG_GSM3_WWAN_STATE_INFO,              // 0x0007
  CXM_LOG_GLOBAL_POLICY_PARAMS,              // 0x0008
  CXM_LOG_MDM_STATE,                         // 0x0009
  CXM_LOG_WCI2_DATA_RECV,                    // 0x000A
  CXM_LOG_MAX_MED_PRIO,                      /* Current maximum */
  CXM_LOG_UNSIGNED32_MP = 0x0003FFFE         /* Force uint32 type */
} cxm_med_prio_log_type_e;

/* Enumeration of high prio log packet payloads */
typedef enum
{
  CXM_LOG_MIN_HIGH_PRIO = 0x0003FFFF,        /* 32-bit offset */
  CXM_LOG_LTE_POLICY_INFO,                   // 0x0000
  CXM_LOG_GSM1_POLICY_INFO,                  // 0x0001
  CXM_LOG_GSM2_POLICY_INFO,                  // 0x0002
  CXM_LOG_GSM3_POLICY_INFO,                  // 0x0003
  CXM_LOG_TDS_POLICY_INFO,                   // 0x0004
  CXM_LOG_BAND_FILTER_INFO,                  // 0x0005
  CXM_LOG_PARAMS_NO_TABLES,                  // 0x0006
  CXM_LOG_PARAMS_VICTIM_TBL,                 // 0x0007
  CXM_LOG_PARAMS_PORT_TABLE,                 // 0x0008
  CXM_LOG_MAX_HIGH_PRIO,                     /* Current maximum */
  CXM_LOG_UNSIGNED32_HP = 0x0007FFFE         /* Force uint32 type */
} cxm_high_prio_log_type_e;

/* Enumeration of smem data plane log packet payloads */
typedef enum
{
  CXM_LOG_MIN_SMEM_DATA = 0x0007FFFF,        /* 32-bit offset */
  CXM_LOG_SMEM_LTE_SLEEP_WAKEUP,             // 0x0000
  CXM_LOG_SMEM_LTE_FRAME_TIMING,             // 0x0001
  CXM_LOG_SMEM_LTE_TX_ADV,                   // 0x0002
  CXM_LOG_SMEM_LTE_HIGH_PRIO,                // 0x0003
  CXM_LOG_SMEM_TDS_SLEEP_WAKEUP,             // 0x0004
  CXM_LOG_SMEM_TDS_FRAME_TIMING,             // 0x0005
  CXM_LOG_SMEM_TDS_ACT_ADV,                  // 0x0006
  CXM_LOG_SMEM_TDS_HIGH_PRIO,                // 0x0007
  CXM_LOG_SMEM_GSM1_SLEEP_WAKEUP,            // 0x0008
  CXM_LOG_SMEM_GSM1_HIGH_PRIO,               // 0x0009
  CXM_LOG_SMEM_GSM1_TIMELINE,                // 0x000A
  CXM_LOG_SMEM_GSM2_SLEEP_WAKEUP,            // 0x000B
  CXM_LOG_SMEM_GSM2_HIGH_PRIO,               // 0x000C
  CXM_LOG_SMEM_GSM2_TIMELINE,                // 0x000D
  CXM_LOG_SMEM_GSM3_SLEEP_WAKEUP,            // 0x000E
  CXM_LOG_SMEM_GSM3_HIGH_PRIO,               // 0x000F
  CXM_LOG_SMEM_GSM3_TIMELINE,                // 0x0010
  CXM_LOG_SMEM_TDS_FW_DATA,                  // 0x0011
  CXM_LOG_SMEM_GSM1_FW_DATA,                 // 0x0012
  CXM_LOG_SMEM_GSM2_FW_DATA,                 // 0x0013
  CXM_LOG_SMEM_GSM3_FW_DATA,                 // 0x0014
  CXM_LOG_SMEM_TDS_FW_DATA_V2         ,      // 0x0015
  CXM_LOG_SMEM_GSM1_FW_DATA_V2,              // 0x0016
  CXM_LOG_SMEM_GSM2_FW_DATA_V2,              // 0x0017
  CXM_LOG_SMEM_GSM3_FW_DATA_V2,              // 0x0018
  CXM_LOG_SMEM_GSM1_TIMELINE_V2,             // 0x0019
  CXM_LOG_SMEM_GSM2_TIMELINE_V2,             // 0x001A
  CXM_LOG_SMEM_GSM3_TIMELINE_V2,             // 0x001B
  CXM_LOG_MAX_SMEM_DATA,                     /* Current maximum */
  CXM_LOG_UNSIGNED32_SM = 0x000FFFFE         /* Force uint32 type */
} cxm_smem_data_log_type_e;

/* Macro to use only 16 least significant bits */
#define CXM_INDX( e_code ) (uint32)((e_code) & 0xFFFF)
/* Macros to check which log to use */
#define CXM_LOG_LP( e_code ) (uint32)((e_code) & (CXM_LOG_MIN_LOW_PRIO + 1))
#define CXM_LOG_MP( e_code ) (uint32)((e_code) & (CXM_LOG_MIN_MED_PRIO + 1))
#define CXM_LOG_HP( e_code ) (uint32)((e_code) & (CXM_LOG_MIN_HIGH_PRIO + 1))
#define CXM_LOG_SM( e_code ) (uint32)((e_code) & (CXM_LOG_MIN_SMEM_DATA + 1))

/* All types associated with low priority cxm log packets */
typedef PACK(union)
{
  coex_state_type_pack                 log_state;
  coex_power_limit_state_e             log_controller_pwr_lmt_state;
  coex_power_limit_state_e             log_wci2_pwr_lmt_state;
  atomic_word_t_pack                   log_metrics_lte_bler_started;
  atomic_word_t_pack                   log_metrics_lte_sinr_started;
  atomic_word_t_pack                   log_metrics_gsm1_sinr_started;
  atomic_word_t_pack                   log_metrics_gsm2_sinr_started;
  atomic_word_t_pack                   log_metrics_gsm3_sinr_started;
  atomic_word_t_pack                   log_metrics_tds_nrrwbnd_noise_started;
  cxm_antenna_state_s_pack             log_antenna_state;
  cxm_wcn_tx_info_s_pack               log_wcn_tx_state;
  cxm_lte_tx_adv_s_pack                log_lte_tx_adv;
  cxm_tech_metrics_read_s_pack         log_tech_metrics_read;
  cxm_lte_bler_read_s_pack             log_lte_bler_read;
  cxm_high_prio_s_pack                 log_high_prio_info;
} cxm_low_prio_log_data_u;

/* ADT for data payloads of different types, augmented with timestamp */
typedef PACK(struct)
{
  uint64                               ts;
  uint8                                event;
  cxm_low_prio_log_type_e              low_prio_type_id;
  cxm_low_prio_log_data_u              payload;
} cxm_low_prio_log_data_struct_type;

/* Structure for holding logged data prior to pushing through diag */
typedef PACK(struct)
{
  uint8                                num_packets;
  cxm_low_prio_log_data_struct_type    log_data[CXM_LOW_PRIO_MAX_PACKETS];
} cxm_low_prio_log_struct_type;

/* Top level structure to be passed to diag log packet api */
typedef PACK(struct)
{
  log_hdr_type                         hdr;
  uint8                                version;
  uint8                                subcode;
  cxm_low_prio_log_struct_type         log_struct;
} cxm_low_prio_log_data_type;

/* All types associated with med priority cxm log packets */
typedef PACK(union)
{
  coex_wwan_tech_info_pack             log_lte_wwan_state_info;
  coex_wwan_tech_info_pack             log_tds_wwan_state_info;
  coex_wwan_tech_info_pack             log_gsm1_wwan_state_info;
  coex_wwan_tech_info_pack             log_onex_wwan_state_info;
  coex_wwan_tech_info_pack             log_hdr_wwan_state_info;
  coex_wwan_tech_info_pack             log_wcdma_wwan_state_info;
  coex_wwan_tech_info_pack             log_gsm2_wwan_state_info;
  coex_wwan_tech_info_pack             log_gsm3_wwan_state_info;
  coex_global_policy_params_s_pack     log_global_policy_params;
  cxm_mdm_state_s_pack                 log_mdm_state;
  uint8                                log_wci2_data_recv;
} cxm_med_prio_log_data_u;

/* ADT for data payloads of different types, augmented with timestamp */
typedef PACK(struct)
{
  uint64                               ts;
  uint8                                event;
  cxm_med_prio_log_type_e              med_prio_type_id;
  cxm_med_prio_log_data_u              payload;
} cxm_med_prio_log_data_struct_type;

/* Structure for holding logged data prior to pushing through diag */
typedef PACK(struct)
{
  uint8                                num_packets;
  cxm_med_prio_log_data_struct_type    log_data[CXM_MED_PRIO_MAX_PACKETS];
} cxm_med_prio_log_struct_type;

/* Top level structure to be passed to diag log packet api */
typedef PACK(struct)
{
  log_hdr_type                         hdr;
  uint8                                version;
  uint8                                subcode;
  cxm_med_prio_log_struct_type         log_struct;
} cxm_med_prio_log_data_type;

/* All types associated with high priority cxm log packets */
typedef PACK(union)
{
  coex_tech_policy_info_pack           log_lte_policy_info;
  coex_tech_policy_info_pack           log_gsm1_policy_info;
  coex_tech_policy_info_pack           log_gsm2_policy_info;
  coex_tech_policy_info_pack           log_gsm3_policy_info;
  coex_tech_policy_info_pack           log_tds_policy_info;
  coex_band_filter_s_pack              log_band_filter_info;
  /* config params excluding victim_tbl and port_table */
  coex_config_params_v8_no_tables      log_params_no_tables;
  /* each victim_tbl is only one quarter of full size */
  coex_config_params_v8_victim_tbl     log_params_victim_tbl;
  /* port_table of config params */
  cxm_port_table_v01_s_pack            log_params_port_table;
} cxm_high_prio_log_data_u;

/* ADT for data payloads of different types, augmented with timestamp */
typedef PACK(struct)
{
  uint64                               ts;
  uint8                                event;
  cxm_high_prio_log_type_e             high_prio_type_id;
  cxm_high_prio_log_data_u             payload;
} cxm_high_prio_log_data_struct_type;

/* Structure for holding logged data prior to pushing through diag */
typedef PACK(struct)
{
  uint8                                num_packets;
  cxm_high_prio_log_data_struct_type   log_data[CXM_HIGH_PRIO_MAX_PACKETS];
} cxm_high_prio_log_struct_type;

/* Top level structure to be passed to diag log packet api */
typedef PACK(struct)
{
  log_hdr_type                         hdr;
  uint8                                version;
  uint8                                subcode;
  cxm_high_prio_log_struct_type        log_struct;
} cxm_high_prio_log_data_type;

/* All types associated with smem log packets */
typedef PACK(union)
{
  /* lte smem params */
  cxm_smem_sleep_wakeup_s_pack         log_smem_lte_sleep_wakeup;
  cxm_smem_lte_frame_timing_s_pack     log_smem_lte_frame_timing;
  cxm_smem_lte_tx_adv_s_pack           log_smem_lte_tx_adv;
  cxm_smem_high_prio_s_pack            log_smem_lte_high_prio;

  /* tds smem params */
  cxm_smem_sleep_wakeup_s_pack         log_smem_tds_sleep_wakeup;
  cxm_smem_tds_frame_timing_s_pack     log_smem_tds_frame_timing;
  cxm_smem_tds_act_adv_s_pack          log_smem_tds_act_adv;
  cxm_smem_high_prio_s_pack            log_smem_tds_high_prio;

  /* gsm1 smem params */
  cxm_smem_sleep_wakeup_s_pack         log_smem_gsm1_sleep_wakeup;
  cxm_smem_high_prio_s_pack            log_smem_gsm1_high_prio;
  /* Timeline is only one eighth of total size */
  cxm_smem_gsm_timeline                log_smem_gsm1_timeline;

  /* gsm2 smem params */
  cxm_smem_sleep_wakeup_s_pack         log_smem_gsm2_sleep_wakeup;
  cxm_smem_high_prio_s_pack            log_smem_gsm2_high_prio;
  /* Timeline is only one eighth of total size */
  cxm_smem_gsm_timeline                log_smem_gsm2_timeline;

  /* gsm3 smem params */
  cxm_smem_sleep_wakeup_s_pack         log_smem_gsm3_sleep_wakeup;
  cxm_smem_high_prio_s_pack            log_smem_gsm3_high_prio;
  /* Timeline is only one eighth of total size */
  cxm_smem_gsm_timeline                log_smem_gsm3_timeline;

  /* fw data for tds and gsm */
  cxm_smem_fw_data                     log_smem_fw_data;
} cxm_smem_log_data_u;

/* ADT for smem data plane log packets, with timestamp */
typedef PACK(struct)
{
  uint64                               ts;
  uint8                                event;
  cxm_smem_data_log_type_e             smem_data_type_id;
  cxm_smem_log_data_u                  payload;
} cxm_smem_log_data_struct_type;

/* Structure for holding logged data prior to pushing through diag */
typedef PACK(struct)
{
  uint8                                num_packets;
  cxm_smem_log_data_struct_type        log_data[CXM_SMEM_DATA_MAX_PACKETS];
} cxm_smem_log_struct_type;

/* Top level structure to be passed to diag log packet api */
typedef PACK(struct)
{
  log_hdr_type                         hdr;
  uint8                                version;
  uint8                                subcode;
  cxm_smem_log_struct_type             log_struct;
} cxm_smem_log_data_type;

/*===========================================================================

  FUNCTION:  cxm_trace_init

===========================================================================*/
/*!
  @brief
    To initialize CXM's debug trace/event logging interface

  @return
    None
*/
/*=========================================================================*/
void cxm_trace_init (
  void
);

/*===========================================================================

  FUNCTION:  cxm_trace_event

===========================================================================*/
/*!
  @brief
    To log CXM event into the trace buffer

  @return
    None
*/
/*=========================================================================*/
void cxm_trace_event
(
  cxm_trace_event_types_e event,   /*!< CXM event type */
  uint8                   a8,    /*!< 1st of 3 opaque payloads (8-bit) */
  uint16                  b16,   /*!< 2nd of 3 opaque payloads (16-bit) */
  uint32                  c32,   /*!< 3rd of 3 opaque payloads (32-bit) */
  uint8                   nargs, /*!< Parameter for number of variable args*/
  ...  /*!< Variable argument list including MANDATORY "flag" parameter */
  /*!< uint32             flag,     log packetting and trace enable flag */
  /*!< uint32             pyld_e1   first optional log pkt payload enum */
  /*!<  ...                                                             */
  /*!< uint32             pyld_e15  final optional log pkt patload enum */
);

/*===========================================================================

  FUNCTION:  cxm_counter_event

===========================================================================*/
/*!
  @brief
    To increment a CXM counter corresponding to some event. Also log the 
    timestamp and value of that event

  @detail
    NOT threadsafe. please call only from within the cxm task for now.

  @return
    None
*/
/*=========================================================================*/
void cxm_counter_event
(
  cxm_counters_e counter,    /*!< Unique counter ID */
  uint32         last_value  /*!< Specific to the event that the counter 
                                 refers to. This is meant to be the value of
                                 the event we are counting */
);

/*===========================================================================

  FUNCTION:  cxm_counter_reset

===========================================================================*/
/*!
  @brief
    Reset specified counter. Exceptions: If value passed is 
    CXM_CNT_MAX_COUNTERS, will reset ALL counters. If CXM_CNT_POLICY_RESET 
    is passed in, will reset all counters EXCEPT the policy counters.

  @return
    None
*/
/*=========================================================================*/
void cxm_counter_reset
(
  cxm_counters_e counter
);

/*===========================================================================

  FUNCTION:  cxm_counter_log

===========================================================================*/
/*!
  @brief
    Logs all CXM counters with F3s

  @return
    None
*/
/*=========================================================================*/
void cxm_counter_log
(
  void
);

/*===========================================================================

  FUNCTION:  cxm_get_lte_counters

===========================================================================*/
/*!
  @brief
    Gets all counters and LTE related FW counters

  @return
    None
*/
/*=========================================================================*/
void cxm_get_lte_counters
(
  uint32 fw_counters[],
  uint32 counters[]
);

/*===========================================================================

  FUNCTION:  cxm_pack_low_prio_log_packet_payload

===========================================================================*/
/*!
  @brief
    Helper function to create a deep copy of payload structures, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_low_prio_log_packet_payload
(
  cxm_low_prio_log_data_u *dest,
  cxm_low_prio_log_type_e pyld_e
);

/*===========================================================================

  FUNCTION:  cxm_pack_med_prio_log_packet_payload

===========================================================================*/
/*!
  @brief
    Helper function to create a deep copy of payload structures, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_med_prio_log_packet_payload
(
  cxm_med_prio_log_data_u *dest,
  cxm_med_prio_log_type_e pyld_e
);

/*===========================================================================

  FUNCTION:  cxm_pack_victim_tbl

===========================================================================*/
/*!
  @brief
    Inline helper function to create a deep copy of victim_tbl, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_victim_tbl
(
  uint32 start_indx,
  cxm_high_prio_log_data_u *dest
);

/*===========================================================================

  FUNCTION:  cxm_pack_high_prio_log_packet_payload

===========================================================================*/
/*!
  @brief
    Helper function to create a deep copy of payload structures, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_high_prio_log_packet_payload
(
  cxm_high_prio_log_data_u *dest,
  cxm_high_prio_log_type_e pyld_e
);

/*===========================================================================

  FUNCTION:  cxm_pack_gsm_timeline

===========================================================================*/
/*!
  @brief
    Inline helper function to create a deep copy of gsm timeline, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_gsm_timeline
(
  uint32 start_indx,
  uint32 end_idx,
  cxm_smem_log_data_u *dest,
  cxm_smem_data_log_type_e pyld_e
);

/*===========================================================================

  FUNCTION:  cxm_pack_smem_log_packet_payload

===========================================================================*/
/*!
  @brief
    Helper function to create a deep copy of payload structures, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_smem_log_packet_payload
(
  cxm_smem_log_data_u *dest,
  cxm_smem_data_log_type_e pyld_e
);

/*===========================================================================

  FUNCTION:  cxm_log_new_data

===========================================================================*/
/*!
  @brief
    Takes one of the enumerated log packet payload datatypes and creates a
    new entry in the appropriate log.

  @return
    None
*/
/*=========================================================================*/
void cxm_log_new_data
(
  uint32 pyld_e
);

/*===========================================================================

  FUNCTION:  cxm_send_log

===========================================================================*/
/*!
  @brief
    Sends the specified log to diag and resets the log.

  @return
    None
*/
/*=========================================================================*/
void cxm_send_log
(
  uint16 log_code
);

/*=============================================================================

  FUNCTION:  coex_handle_log_async_ind

=============================================================================*/
/*!
    @brief
    Handle logs performed outside CXM context

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_log_async_ind (
  void *rcv_msgr_msg_ptr
);


#endif /* CXM_TRACE_H */

