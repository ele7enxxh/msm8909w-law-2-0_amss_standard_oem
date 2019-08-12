/*!
  @file
  lte_mac_log.h

  @brief
  This header file contains the variou defines, enums, and other data
  structure types which are necessary for logging binary packets via DIAG.


*/

/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/src/lte_mac_log.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/15/15   sb      CR 803880: MAC Changes for UDC feature
03/28/14   mg      CR632129: Print the MAC MCE payload in the log.
10/18/13   ta      CR 553489: Reorg of log defs
04/17/12   ru      CR 342859: Added eMBMS logging changes  
07/28/11   st      Fixed wrong reporting of the BSR by the UE
01/12/11   bn      Removed Extraneous comma at the end of enum define
08/23/10   bn      Used Overwrite DB for UL TB log to prevent deadlock
08/17/10   bn      Defined the log structure to have explicit field
06/23/10   bn      Suppress lint warnings related to Anonymous union 
06/22/10   bn      Enhanced MAC buffer status and RACH attempt log packets
03/12/10   bn      Added in the periodic flush timer for UL TB log
09/19/09   bn      Consolidated all MAC timer event in one util function
09/16/09   bn      Added in support for new MAC events
09/01/09   bn      Matched up the RACH attempt log MSG Bitmask with ICD
08/26/09   bn      Added prach_fre_offset,prach index mask and hdr size to 10
07/26/09   bn      Fixed UL QoS log
06/15/09   bn      Fixed the SFN field in DL/UL TB log packet
06/11/09   bn      Added in config reason logging for RLF
05/15/08   bn      Initial version 
09/24/08   yg      MAC Log packet and subpacket definitions 
===========================================================================*/

#ifndef LTE_MAC_LOG_H
#define LTE_MAC_LOG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <genlog.h>
#include "lte_log_codes.h"
#include "lte_mac_log_ext.h"
#include "lte_mac_common.h"
/*lint -e657*/
/*lint -e658*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Log SubIds of MAC */
typedef enum
{
  LTE_MAC_LOG_SUB_ID_CFG_TYPE,      /*<  Subpkt Id for MAC Config Type log packet */
  LTE_MAC_LOG_SUB_ID_DL_CFG,        /*<  Subpkt Id for MAC DL Config log packet */
  LTE_MAC_LOG_SUB_ID_UL_CFG,        /*<  Subpkt Id for MAC UL Config log packet */
  LTE_MAC_LOG_SUB_ID_RACH_CFG,      /*<  Subpkt Id for MAC RACH Config log packet */
  LTE_MAC_LOG_SUB_ID_LC_CFG,        /*<  Subpkt Id for MAC LC Config log packet */
  LTE_MAC_LOG_SUB_ID_RACH_REASON,   /*<  Subpkt Id for MAC RACH reason log packet */
  LTE_MAC_LOG_SUB_ID_RACH_ATTEMPT,  /*<  Subpkt Id for MAC RACH attempt log packet */
  LTE_MAC_LOG_SUB_ID_DL_TB,         /*<  Subpkt Id for DL Transport Block log packet */
  LTE_MAC_LOG_SUB_ID_UL_TB,         /*<  Subpkt Id for UL Transport Block log packet */
  LTE_MAC_LOG_SUB_ID_UL_QOS,        /*<  Subpkt Id for UL QOS log packet */
  LTE_MAC_LOG_SUB_ID_BUF_STATUS,    /*<  Subpkt Id for Buffer Status log packet */
  LTE_MAC_LOG_SUB_ID_UL_TX_STATS,   /*<  Subpkt Id for UL Tx Statistics log packet */
  LTE_MAC_LOG_SUB_ID_EMBMS_STATS,   /*<  Subpkt Id for eMBMS Statistics log packet */
  LTE_MAC_LOG_SUB_ID_EMBMS_CFG,     /*<  Subpkt Id for MAC eMBMS Config log packet */
  LTE_MAC_LOG_SUB_ID_MAX
} lte_mac_log_sub_id_e;

/*! @brief L2 configuration reason
*/
typedef enum
{
  LTE_MAC_LOG_CFG_NORMAL,        /*!< regular configuration */
  LTE_MAC_LOG_CFG_HO,            /*!< handover */
  LTE_MAC_LOG_CFG_RELEASE,       /*!< connection release */
  LTE_MAC_LOG_CFG_RADIO_FAILURE, /*!< Radio Failure */
  LTE_MAC_LOG_EMBMS_CFG          /*!< eMBMS configuration*/
} lte_mac_log_cfg_reason_e;

/*! @brief Enum for RACH reason */
typedef enum
{
  LTE_MAC_LOG_RACH_REASON_CONNECTION_REQ,  /*<  Connection Request */
  LTE_MAC_LOG_RACH_REASON_RLF,             /*<  Radio Link Failure */
  LTE_MAC_LOG_RACH_REASON_UL_DATA,         /*<  Uplink Data Arrival */
  LTE_MAC_LOG_RACH_REASON_DL_DATA,         /*<  Downlink Data Arrival */
  LTE_MAC_LOG_RACH_REASON_HO               /*<  Handover */
} lte_mac_log_rach_reason_e;

/*! @brief Enum for RACH result */
typedef enum
{
  LTE_MAC_LOG_RACH_RESULT_SUCCESS,          /*<  Success */
  LTE_MAC_LOG_RACH_RESULT_FAIL_AT_MESG2,    /*<  Failure at MESG2 */
  LTE_MAC_LOG_RACH_RESULT_FAIL_AT_MESG4_CTT,/*<  Failure at MESG4 due CT timer */
  LTE_MAC_LOG_RACH_RESULT_FAIL_AT_MESG4,    /*<  Failure at MESG4 due to CR */
  LTE_MAC_LOG_RACH_RESULT_ABORTED           /*<  Aborted */
} lte_mac_log_rach_result_e;

/*! @brief defined invalid SFN value */
#define LTE_MAC_LOG_INVALID_SFN_VALUE       0xFFFF

/*! @brief defined bit mask to indicate DL configuration is present
*/
#define LTE_MAC_LOG_DL_CFG_BM       0x01

/*! @brief defined bit mask to indicate RACH configuration is present
*/
#define LTE_MAC_LOG_RACH_CFG_BM     0x02

/*! @brief defined bit mask to indicate UL configuration is present
*/
#define LTE_MAC_LOG_UL_CFG_BM       0x04

/*! @brief defined bit mask to indicate LC Info configuration is present
*/
#define LTE_MAC_LOG_LC_INFO_CFG_BM  0x08

/*! @brief defined bit mask to indicate eMBMS configuration is present 
*/
#define LTE_MAC_LOG_EMBMS_CFG_BM    0x10

/*! @brief 
    Bitmask for MESG1 of RACH attempt log packet
*/
#define LTE_MAC_LOG_RACH_ATTEMPT_MESG1_MASK 0x1

/*! @brief 
    Bitmask for MESG2 of RACH attempt log packet
*/
#define LTE_MAC_LOG_RACH_ATTEMPT_MESG2_MASK 0x2

/*! @brief 
    Bitmask for MESG3 of RACH attempt log packet
*/
#define LTE_MAC_LOG_RACH_ATTEMPT_MESG3_MASK 0x4

/*! @brief 
    Frequency of logging for DL Transport Block log packet
*/
#define LTE_MAC_LOG_DL_TB_LOG_TIMER_VAL 50

/*! @brief 
    Maximum length of MAC header in log packets
*/
#define LTE_MAC_LOG_MAC_HDR_MAX_LEN 32

/*! @brief 
    Maximum number of DL TB samples in DL Transport Block log packet
*/
#define LTE_MAC_LOG_MAX_DL_TB_SAMPLES 30

/*! @brief 
    Maximum number of UL TB samples in UL Transport Block log packet
*/
#define LTE_MAC_LOG_MAX_UL_TB_SAMPLES 25

/*! @brief 
    Maximum Flush timer for UL Transport Block log packet
*/
#define LTE_MAC_LOG_UL_TB_LOG_FLUSH_TIMER  (4 * LTE_MAC_LOG_MAX_UL_TB_SAMPLES)

/*! @brief 
    Maximum number of UL QOS samples in UL QOS log packet
*/
#define LTE_MAC_LOG_MAX_UL_QOS_SAMPLES 40

/*! @brief 
    Maximum number of Buffer Status samples in Buffer Status log packet
*/
#define LTE_MAC_LOG_MAX_BUF_STATUS_SAMPLES 40 

/*! @brief 
    Number of ms over which UL TX stats is accumulated
*/
#define LTE_MAC_LOG_UL_TX_LOG_PERIOD 100

/*! @brief Define BSR/SR info/request mask event
*/
#define LTE_MAC_LOG_PER_TIMER_EXP_EVT_BM  0x01 /*!< Periodic BSR timer expired*/
#define LTE_MAC_LOG_HIGH_DATA_ARR_EVT_BM  0x02 /*!< High data arrival */
#define LTE_MAC_LOG_RET_TIMER_EXP_EVT_BM  0x04 /*!< Retxed BSR timer expired */
#define LTE_MAC_LOG_BSR_REQUEST_EVT_BM    0x08 /*!< BSR request */
#define LTE_MAC_LOG_SR_REQUEST_EVT_BM     0x10 /*!< SR request */

/*! @brief 
    Computes the size of the variable of type lte_mac_log_dl_tb_info_per_sample_s.
    The struct is of variable size. 
*/
#define lte_mac_log_dl_tb_info_per_sample_s__real_size(val) \
(FPOS(lte_mac_log_dl_tb_info_per_sample_s, mac_hdr) + (val)->mac_hdr_len_logged*sizeof((val)->mac_hdr[0]))

/*! @brief 
    Computes the size of the variable of type lte_mac_log_ul_tb_info_per_sample_s.
    The struct is of variable size. 
*/
#define lte_mac_log_ul_tb_info_per_sample_s__real_size(val) \
(FPOS(lte_mac_log_ul_tb_info_per_sample_s, mac_hdr) + (val)->mac_hdr_len_logged*sizeof((val)->mac_hdr[0]))

/*! @brief 
    Computes the size of the variable of type lte_mac_log_ul_qos_info_per_sample_s.
    The struct is of variable size. 
*/
#define lte_mac_log_ul_qos_info_per_sample_s__real_size(val) \
(FPOS(lte_mac_log_ul_qos_info_per_sample_s, lc) + (val)->num_active_lc*sizeof((val)->lc[0]))

/*! @brief 
    Computes the size of the variable of type lte_mac_log_ul_buf_status_info_per_sample_s.
    The struct is of variable size. 
*/
#define lte_mac_log_ul_buf_status_info_per_sample_s__real_size(val) \
(FPOS(lte_mac_log_ul_buf_status_info_per_sample_s, lc) + (val)->num_active_lc*sizeof((val)->lc[0]))

#ifdef _WIN32
    #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*! Length of area pmch combo field for embms statistics logging*/
#define LTE_MAC_LOG_AREA_PMCH_MAX 5

/*! Counter limit for eMBMS statistics packet commit*/
#define LTE_MAC_LOG_EMBMS_STATS_COMMIT_CNTR 8

/*! Macro to define the max number of lcid's logged in eMBMS stats*/
#define LTE_MAC_EMBMS_LOG_LC 8

/*! Macro to define an empty slot in the logging arrays for eMBMS*/
#define LTE_MAC_LOG_ARRAY_EMPTY 0xFFFF

/*! @brief Structure for DL TB info in DL Transport block Log sub packet "scratchpad" 
*/
typedef PACK(struct)
{
  uint16     sfn;                     /*!< Subframe number */
  uint8      rnti_type;               /*!< rnti type */
  uint8      harq_id_or_area_id;      /*!< HARQ Id or AREA Id for eMBMS*/
  uint16     pmch_id;                 /*!< PMCH Id*/ 
  uint16     dl_grant;                /*!< Downlink grant */
  uint8      num_rlc_pdus;            /*!< Number of RLC PDUs in the MAC PDU */
  uint16     num_padding_bytes;       /*!< Number of Padding bytes in the MAC PDU */
  uint8      mac_hdr_len_logged;      /*!< Actual length of MAC header logged */
  uint8      mac_hdr[LTE_MAC_LOG_MAC_HDR_MAX_LEN]; /*!< Leading portion of MAC header of the MAC PDU */
} lte_mac_log_dl_tb_info_per_sample_s;

/*! @brief Structure for DL Transport block Log sub packet "scratchpad" 
*/
typedef PACK(struct)
{
  uint8               num_samples;      /*!< number of DL TB samples */
  lte_mac_log_dl_tb_info_per_sample_s  tb_info[LTE_MAC_LOG_MAX_DL_TB_SAMPLES];   /*!< DL transport blocks */
} lte_mac_log_sub_pkt_dl_tb_s;

/*! @brief Structure for DL embms statistics Log sub packet */
typedef PACK(struct)
{
  uint16     area_id;                          /*!< Area Id, size is uint16 to make sure the struc
                                                     is on 4 bytes boundary*/
  uint16     pmch_id;                          /*!< Pmch Id*/
  uint32     num_embms_data_pdu;               /*!< Number of received data pdu*/
  uint32     num_embms_dropped_data_pdu;       /*!< Number of dropped data pdu*/
  uint32     num_embms_ctrl_pdu;               /*!< Number of received ctrl pdu*/
  uint32     num_total_tb_bytes;               /*!< Number of total tb bytes*/
  uint32     num_total_padding_bytes;          /*!< Number of total padding bytes*/
  uint32     num_total_msi_bytes;              /*!< Number of total msi bytes*/
  uint32     reserved1;                        /*!< Reserved*/
  uint32     reserved2;                        /*!< Reserved*/
  uint32     num_lcid_bytes[LTE_MAC_EMBMS_LOG_LC]; /*!< Number of bytes per lc_id*/
} lte_mac_log_embms_sub_pkt_stats_s;

/*! @brief Structure for DL embms statistics Log packet */
typedef PACK(struct)
{
  uint32      total_num_dropped_pkts;           /*!< Total number of dropped packets*/
  lte_mac_log_embms_sub_pkt_stats_s area_pmch_combo[LTE_MAC_LOG_AREA_PMCH_MAX];  
} lte_mac_log_embms_stats_s;

/*! @brief Structure for UL TB info in UL Transport block Log sub packet "scratchpad" 
*/
typedef PACK(struct)
{
  uint8      harq_id;                 /*!< HARQ Id*/
  uint8      rnti_type;               /*!< rnti type */
  uint16     sfn;                     /*!< Subframe number */
  uint16     ul_grant;                /*!< Uplink grant */
  uint8      num_rlc_pdus;            /*!< Number of RLC PDUs in the MAC PDU */
  uint16     num_padding_bytes;       /*!< Number of Padding bytes in the MAC PDU */
  uint8      bsr_event;               /*!< Specifies if the BSR  event is of type - a) Regular b) Periodic c) robustnes or none*/
  uint8      bsr_triggered;           /*!< which BSR is sent out, see lte_mac_qos_bsr_triggered_e */
  uint8      mac_hdr_len_logged;      /*!< Actual length of MAC header logged */
  uint8      mac_hdr[LTE_MAC_LOG_MAC_HDR_MAX_LEN]; /*!< Leading portion of MAC header of the MAC PDU */
} lte_mac_log_ul_tb_info_per_sample_s;

/*! @brief Structure for UL Transport block Log sub packet "scratchpad" 
*/
typedef PACK(struct)
{
  uint8               num_samples;      /*!< number of UL TB samples */
  lte_mac_log_ul_tb_info_per_sample_s  tb_info[LTE_MAC_LOG_MAX_UL_TB_SAMPLES];   /*!< UL transport blocks */
} lte_mac_log_sub_pkt_ul_tb_s;

/*! @brief Structure for LC Info in UL QOS Log sub packet "scratchpad" 
*/
typedef PACK(struct)
{
  uint8               lc_id;                /*!< Logical channel ID of each channel */
  uint16              lower_edge_allotment; /*!< MAC provides RLC with a range for the allotment */
  uint16              upper_edge_allotment; /*!< This is over and above the PBR  */
  uint16              grant_utilized;       /*!< Grant actually utilized by the RLC while building PDUs  */
} lte_mac_log_ul_qos_lc_info_s;

/*! @brief Structure for UL QOS Sample in UL QOS Log sub packet "scratchpad" 
*/
typedef PACK(struct)
{
  uint16              sfn;                     /*!< Sub/System Frame Number */
  uint8               num_active_lc;           /*!< Number of active logical channels */
  lte_mac_log_ul_qos_lc_info_s lc[LTE_MAX_LC]; /*!< Active LC Infos */
} lte_mac_log_ul_qos_info_per_sample_s;

/*! @brief Structure for UL QOS Log sub packet "scratchpad" 
*/
typedef PACK(struct)
{
  uint8               num_samples;          /*!< Number of samples collected in this log packet */
  lte_mac_log_ul_qos_info_per_sample_s qos[LTE_MAC_LOG_MAX_UL_QOS_SAMPLES]; /*!< QOS Samples */
} lte_mac_log_sub_pkt_ul_qos_s;

/*! @brief Structure for LC Info in MAC Buffer Status Log sub packet "scratchpad" 
*/
typedef PACK(struct)
{
  uint8     lc_id;                /*!< Logical channel ID */
  uint8     priority;             /*!< Priority of this logical channel */
  uint32    new_uncomp_data_bytes;  /*!< Number of new uncompressed data bytes available for transmission  */
  uint32    new_comp_data_bytes;  /*!< Number of new compressed data bytes available for transmission  */
  uint32    retx_data_bytes;      /*!< Number of bytes available for retransmission */
  uint16    ctrl_data_bytes;      /*!< Number of bytes of control data available for transmission */
} lte_mac_log_lc_buf_info_s;

/*! @brief Structure for Buffer Status sample MAC Buffer 
    Status Log sub packet "scratchpad" 
*/
typedef PACK(struct)
{
  uint16              sfn;                     /*!< Sub/System Frame Number */
  uint8               num_active_lc;        /*!< Number of active logical channels */
  lte_mac_log_lc_buf_info_s lc[LTE_MAX_LC]; /*!< Active LC Infos */
} lte_mac_log_ul_buf_status_info_per_sample_s;

/*! @brief Structure for MAC Buffer Status Log sub packet "scratchpad" 
*/
typedef PACK(struct)
{
  /*! Number of samples collected in this log packet */
  uint8      num_samples;  
  /*! Buffer Status Samples */
  lte_mac_log_ul_buf_status_info_per_sample_s buf_status[LTE_MAC_LOG_MAX_BUF_STATUS_SAMPLES];
} lte_mac_log_sub_pkt_ul_buf_status_s;

/*! @brief Structure for Timing Advance Event
*/
typedef PACK(struct)
{
  /*! Timer value for which the newly received timing adjustment is valid*/
  uint16   timer_value; 
  /*! amount of timing adjustment in 0.5 us */
  uint8    timing_advance; 
} lte_mac_log_ta_evt_s;

/*! @brief Structure for SPS Deactivated Event
*/
typedef PACK(struct)
{
  uint8  num_padding_bsr;/*!< Number of padding BSRs sent by the UE*/
} lte_mac_log_sps_deactivated_evt_s;

/*! @brief Structure for RACH Start Event
*/
typedef PACK(struct)
{
  uint8 rach_cause;     /*!< cause for RACH procedure. See lte_mac_log_rach_reason_e type */
  uint8 rach_contention;/*!< 1=contention-based. 0=contention-free */
} lte_mac_log_rach_start_evt_s;

/*! @brief RACH result event enummeration type
*/
typedef enum
{
  LTE_MAC_LOG_RACH_RES_EVT_ABORTED,/*!< RACH is aborted */
  LTE_MAC_LOG_RACH_RES_EVT_SUCCESS,/*!< RACH is success */
  LTE_MAC_LOG_RACH_RES_EVT_PROBLEM,/*!< RACH is having problem*/
  LTE_MAC_LOG_RACH_RES_EVT_FAILED  /*!< RACH is failed*/
} lte_mac_log_reach_result_evt_e;

/*! @brief Structure for RACH Access Result Event
*/
typedef PACK(struct)
{
  uint8 rach_result;  /*!<aborted=0,successful=1,problem=2*/
} lte_mac_log_rach_result_evt_s;

/*! @brief Structure for RACH RAID MATCH Event
*/
typedef PACK(struct)
{
  uint8 match_result;  /*!< Not Matched=0,matched=1*/
} lte_mac_log_rach_raid_match_evt_s;

/*! @brief Structure for MAC Reset Event
*/
typedef PACK(struct)
{
  uint8 cause;  /*!< cause of mac reset */
} lte_mac_log_mac_reset_evt_s;

/*! @brief Structure for BSR/SR request Event
*/
typedef PACK(struct)
{
  uint8 cause_mask;  /*!< cause of BSR/SR request */
} lte_mac_log_bsr_sr_request_evt_s;

/*! @brief MAC timer type enummeration type
*/
typedef enum
{
  LTE_MAC_LOG_TA_TIMER,      /*!< Time Advance timer*/
  LTE_MAC_LOG_RA_TIMER,      /*!< RACH RA timer */
  LTE_MAC_LOG_CT_TIMER,      /*!< RACH contention timer */
  LTE_MAC_LOG_BACKOFF_TIMER  /*!< RACH backoff timer */
} lte_mac_log_timer_type_evt_e;

/*! @brief MAC timer action enummeration type
*/
typedef enum
{
  LTE_MAC_LOG_TIMER_START,  /*!< timer start*/
  LTE_MAC_LOG_TIMER_STOP,   /*!< timer stop */
  LTE_MAC_LOG_TIMER_EXPIRED /*!< timer expired*/
} lte_mac_log_timer_action_evt_e;

/*! @brief Structure for MAC timer Event
*/
typedef PACK(struct)
{
  uint8 type;  /*!< timer type */
  uint8 action;/*!< action: start, stop or expired */
} lte_mac_log_mac_timer_evt_s;

/*! @brief Structure for UL Transport log sub packet buffer 
*/

typedef PACK(struct) 
{
  union 
  {
    lte_mac_log_sub_pkt_ul_tb_s  log_pkt ALIGN(4);
    uint8                        buf[sizeof(lte_mac_log_sub_pkt_ul_tb_s)] ALIGN(4);
    /*! Buffer for accumulation of UL transport blocks */
  };
  uint16   size; /*! Current size of buffer */
} lte_mac_log_sub_pkt_ul_tb_buf_s ;

/*! @brief Structure for DL Transport log sub packet buffer 
*/
typedef PACK(struct)
{
  union 
  {
    lte_mac_log_sub_pkt_dl_tb_s  log_pkt ALIGN(4);
    /*! Buffer for accumulation of DL transport blocks */
    uint8                        buf[sizeof(lte_mac_log_sub_pkt_dl_tb_s)] ALIGN(4);
  };
  /*! Current size of buffer for accumulation of DL transport blocks */
  uint16                         size;
} lte_mac_log_sub_pkt_dl_tb_buf_s;

/*! @brief Structure for UL QoS sub packet buffer 
*/
typedef PACK(struct)
{
  union 
  {
    lte_mac_log_sub_pkt_ul_qos_s  log_pkt ALIGN(4);
    /*! Buffer for accumulation of UL QOS info */
    uint8                         buf[sizeof(lte_mac_log_sub_pkt_ul_qos_s)] ALIGN(4);
  };
  /*! Current size of buffer for accumulation of UL QOS info */
  uint16                          size;
} lte_mac_log_sub_pkt_ul_qos_buf_s;

/*! @brief Structure for UL Buffer Status sub packet buffer 
*/
typedef PACK(struct)
{
  union 
  {
    lte_mac_log_sub_pkt_ul_buf_status_s  log_pkt ALIGN(4);
    /*! Buffer for accumulation of UL Buffer Status info */
    uint8 buf[sizeof(lte_mac_log_sub_pkt_ul_buf_status_s)] ALIGN(4);
  };
  /*! Current size of buffer for accumulation of UL Buffer Status info */
  uint16  size;
} lte_mac_log_sub_pkt_ul_buf_status_buf_s;

#ifdef _WIN32
   #pragma pack(pop) // Revert alignment to what it was previously
#endif

/*! @brief Structure for MAC Log
*/
typedef struct
{
  genlog_s genlog;             /*!< GENLOG */
  /*! flush timer count for UL TB log*/
  uint8   ul_tb_log_flush_timer_cnt; 
  boolean dl_tb_log_enabled;   /*! Locally cached value. Whether DL TB log is enabled */
  boolean ul_tb_log_enabled;   /*! Locally cached value. Whether UL TB log is enabled */
  boolean ul_qos_log_enabled;  /*! Locally cached value. Whether UL QOS log is enabled */
  boolean buffer_status_log_enabled;  /*! Locally cached value. Whether Buffer Status log is enabled */
  boolean ul_tx_stats_log_enabled;  /*! Locally cached value. Whether UL TX Stats log is enabled */
  uint32  embms_stats_counter;   /*! eMBMS stats counter*/

  lte_mac_log_sub_pkt_cfg_type_s clipboard_cfg_type;
  lte_mac_log_sub_pkt_dl_cfg_s clipboard_dl_cfg;
  lte_mac_log_sub_pkt_ul_cfg_s clipboard_ul_cfg;
  lte_mac_log_sub_pkt_rach_cfg_s clipboard_rach_cfg;
  lte_mac_log_sub_pkt_lc_cfg_s clipboard_lc_cfg;
  lte_mac_log_sub_pkt_embms_cfg_s clipboard_embms_cfg;
  lte_mac_log_sub_pkt_rach_reason_s clipboard_rach_reason;
  lte_mac_log_sub_pkt_rach_attempt_s clipboard_rach_attempt;
  lte_mac_log_sub_pkt_dl_tb_buf_s    clipboard_dl_tb;
  lte_mac_common_dbuf_s clipboard_ul_tb_dbuf;
  lte_mac_log_sub_pkt_ul_tb_buf_s clipboard_ul_tb_buf1;
  lte_mac_log_sub_pkt_ul_tb_buf_s clipboard_ul_tb_buf2;
  lte_mac_log_sub_pkt_ul_tb_buf_s* current_clipboard_ul_tb;
  lte_mac_log_sub_pkt_ul_qos_buf_s clipboard_ul_qos;
  lte_mac_log_sub_pkt_ul_buf_status_buf_s clipboard_ul_buf_status;
  lte_mac_log_ul_tx_stats_s clipboard_ul_tx_stats;
  lte_mac_log_embms_stats_s clipboard_embms_stats;

  uint8   num_ul_tb_samples_to_flush;
} lte_mac_log_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_mac_log_get_log_data

===========================================================================*/
/*!
  @brief
  This function returns the global data for MAC log

  @details
  This function returns the global data for MAC log
 
  @return
  lte_mac_log_s*
*/
/*=========================================================================*/
extern lte_mac_log_s* lte_mac_log_get_log_data(void);

/*===========================================================================

  FUNCTION:  lte_mac_log_subpkt_dl_tb_get_curr_sample

===========================================================================*/
/*!
  @brief
  This functions is called to get the current transport block sample in the
  DL transport block log packet "scratchpad"

  @details
  This function returns the current transport block sample in the
  DL transport block log packet "scratchpad"
 
  @return
  lte_mac_log_dl_tb_info_per_sample_s*
*/
/*=========================================================================*/
extern lte_mac_log_dl_tb_info_per_sample_s* lte_mac_log_subpkt_dl_tb_get_curr_sample
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subpkt_dl_tb_curr_sample_done

===========================================================================*/
/*!
  @brief
  This functions is called to indicate that populating the current transport block
  sample in the DL transport block log packet "scratchpad" is done

  @details
  This functions is called to indicate that populating the current transport block
  sample in the DL transport block log packet "scratchpad" is done. This also updates
  the total size of the "scratchpad", and advances the buffer pointer into the
  "scratchpad" to the next sample.
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subpkt_dl_tb_curr_sample_done
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_pkt_dl_tb_commit

===========================================================================*/
/*!
  @brief
  This functions is called to log the DL transport block log packet

  @details
  This functions is called to log the DL transport block log packet.
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_pkt_dl_tb_commit
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_embms_stats_pkt_commit

===========================================================================*/
/*!
  @brief
  This functions is called to log the mac embms statistics log

  @details
  This functions is called to log the mac embms statistics log
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_embms_stats_pkt_commit
(
  void
);
/*===========================================================================

  FUNCTION:  lte_mac_log_subpkt_ul_tb_get_curr_sample

===========================================================================*/
/*!
  @brief
  This functions is called to get the current transport block sample in the
  UL transport block log packet "scratchpad"

  @details
  This function returns the current transport block sample in the
  UL transport block log packet "scratchpad"
 
  @return
  lte_mac_log_ul_tb_info_per_sample_s*
*/
/*=========================================================================*/
extern lte_mac_log_ul_tb_info_per_sample_s* lte_mac_log_subpkt_ul_tb_get_curr_sample
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subpkt_ul_tb_curr_sample_done

===========================================================================*/
/*!
  @brief
  This functions is called to indicate that populating the current transport block
  sample in the UL transport block log packet "scratchpad" is done

  @details
  This functions is called to indicate that populating the current transport block
  sample in the UL transport block log packet "scratchpad" is done. This also updates
  the total size of the "scratchpad", and advances the buffer pointer into the
  "scratchpad" to the next sample.
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subpkt_ul_tb_curr_sample_done
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_flush_ul_tb_log

===========================================================================*/
/*!
  @brief
  This functions is called to flush all the current samples of the UL TB
  log.

  @details
  None
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_flush_ul_tb_log
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_pkt_ul_tb_commit

===========================================================================*/
/*!
  @brief
  This functions is called to log the UL transport block log packet

  @details
  This functions is called to log the UL transport block log packet.
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_pkt_ul_tb_commit
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subpkt_ul_qos_get_curr_sample

===========================================================================*/
/*!
  @brief
  This functions is called to get the current QOS  sample in the
  UL QOS log packet "scratchpad"

  @details
  This function returns the current QOS sample in the
  UL QOS log packet "scratchpad"
 
  @return
  lte_mac_log_ul_qos_info_per_sample_s*
*/
/*=========================================================================*/
extern lte_mac_log_ul_qos_info_per_sample_s* lte_mac_log_subpkt_ul_qos_get_curr_sample
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subpkt_ul_qos_curr_sample_done

===========================================================================*/
/*!
  @brief
  This functions is called to indicate that populating the current QOS
  sample in the UL QOS log packet "scratchpad" is done

  @details
  This functions is called to indicate that populating the current QOS
  sample in the UL QOS log packet "scratchpad" is done. This also updates
  the total size of the "scratchpad", and advances the buffer pointer into the
  "scratchpad" to the next sample.
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subpkt_ul_qos_curr_sample_done
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_pkt_ul_qos_commit

===========================================================================*/
/*!
  @brief
  This functions is called to log the UL QOS log packet

  @details
  This functions is called to log the UL QOS log packet.
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_pkt_ul_qos_commit
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subpkt_ul_buf_status_get_curr_sample

===========================================================================*/
/*!
  @brief
  This functions is called to get the current Buffer Status  sample in the
  UL Buffer Status log packet "scratchpad"

  @details
  This function returns the current Buffer Status sample in the
  UL Buffer Status log packet "scratchpad"
 
  @return
  lte_mac_log_ul_buf_status_info_per_sample_s*
*/
/*=========================================================================*/
extern lte_mac_log_ul_buf_status_info_per_sample_s* lte_mac_log_subpkt_ul_buf_status_get_curr_sample
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_subpkt_ul_buf_status_curr_sample_done

===========================================================================*/
/*!
  @brief
  This functions is called to indicate that populating the current UL
  Buffer Status sample in the UL Buffer Status log packet "scratchpad" is done

  @details
  This functions is called to indicate that populating the current Buffer Status
  sample in the UL Buffer Status log packet "scratchpad" is done. This also updates
  the total size of the "scratchpad", and advances the buffer pointer into the
  "scratchpad" to the next sample.
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_subpkt_ul_buf_status_curr_sample_done
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_pkt_ul_buf_status_commit

===========================================================================*/
/*!
  @brief
  This functions is called to log the UL Buffer Status log packet

  @details
  This functions is called to log the UL Buffer Status log packet.
 
  @return
*/
/*=========================================================================*/
extern void lte_mac_log_pkt_ul_buf_status_commit
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_log_timer_event

===========================================================================*/
/*!
  @brief
  Send out the MAC timers event such as stop, start or expired

  @return
*/
/*=========================================================================*/
extern void lte_mac_log_timer_event
(
  lte_mac_log_timer_type_evt_e   type,  /*!< timer type*/
  lte_mac_log_timer_action_evt_e action /*!< timer action */
);
#endif /* LTE_MAC_LOG_H */
