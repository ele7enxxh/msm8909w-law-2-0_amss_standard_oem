#ifndef __WCN_COEX_MGR_H__
#define __WCN_COEX_MGR_H__
/*!
  @file wcn_coex_mgr.h

  @brief
   APIs exposed by the CXM for WCN-WWAN COEX

*/

/*=============================================================================

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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/wcn_coex_mgr.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/20/15   btl     Add interface support for LTE carrier aggregation
01/14/15   ckk     Add support for Tx antenna via WCI2 MWS_Connected Type 7
01/07/15   ckk     Added msg for RATs to report their layer 3 states
05/23/14   tak     GSM and TDSCDMA APIs
06/14/14   ckk     Added WCI2 type4 & type7[pwr_bit] support
11/25/13   tak	   Sticky bit driver APIs
02/12/13   ckk     Add COEX boot_params & active_policy message definitions
01/29/13   btl     Add LTE-specific idle/page_cycle params
01/16/13   ckk     Updates to separate WCN/WWAN - WWAN coexistence APIs
01/08/13   tak     Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "cxm.h"
#include <IxErrno.h>

/*=============================================================================

                       CONSTANTS, DEFINES & ENUMS

=============================================================================*/

#define CXM_NUM_TDS_SLOTS             9
#define COEX_MAX_HIGH_PRIO_FREQ       8
#define CXM_SMEM_TDS_TX_POWER_INVALID -640
#define COEX_MAX_ASYNC_LOGS           8

/*! @brief Indicates the CXM system-wide configuration for various WWAN-techs to follow */
typedef uint32 cxm_coex_sys_config_t;

/*! @brief */
typedef enum
{
  CXM_ACTION_UNINIT,
  CXM_ACTION_START,
  CXM_ACTION_STOP,
  CXM_ACTION_READ,
}cxm_action_e;

/*! @brief  WCN Tx state */
typedef enum
{
  CXM_WCN_TX_STATE_INACTIVE,
  CXM_WCN_TX_STATE_ACTIVE
}cxm_wcn_tx_state_e;

typedef enum
{
  CXM_WCN_TX_RSP_SUCCESS,                      /*!< operation was successful*/              
  CXM_WCN_TX_RSP_CLNT_ALREADY_ENABLED,         /*!< client enable called but client is already enabled*/   
  CXM_WCN_TX_RSP_CLNT_ALREADY_DISABLED,        /*!< client disable called but client is already disabled*/
  CXM_WCN_TX_RSP_CLNT_ALREADY_STARTED,         /*!< WLAN TX collection is already ongoing for this client when start is called*/
  CXM_WCN_TX_RSP_CLNT_ALREADY_STOPPED,         /*!< WLAN TX collection is not active but stop commend is issued*/
  CXM_WCN_TX_RSP_CLNT_NOT_ENABLED,             /*!< WLAN TX start is called while the client is not enabled*/
  CXM_WCN_TX_RSP_DISABLE_FAIL_TX_CHECK_ACTIVE, /*!< Tx collection is active so this client cannot be disbaled*/
  CXM_WCN_TX_RSP_INVALID_CLIENT,               /*!< client identifier is invalid*/
  CXM_WCN_TX_RSP_NOT_SUPPORTED                 /*!< WLAN TX sticky bit manager is not supported*/
}cxm_wcn_tx_rsp_codes_e;

typedef enum
{
  CXM_WCN_TX_CLIENT_GPS,
  CXM_WCN_TX_CLIENT_RF,
  CXM_WCN_TX_CLIENT_DIAG,
  CXM_WCN_TX_CLIENT_MAX
}cxm_wcn_tx_client_e;

typedef enum 
{
  CXM_LTE_CHAN_RESERVED, 
  CXM_LTE_CHAN_PUCCH, 
  CXM_LTE_CHAN_PUSCH, 
  CXM_LTE_CHAN_PRACH, 
  CXM_LTE_CHAN_SRS, 
  CXM_LTE_CHAN_SRS_PUCCH, 
  CXM_LTE_CHAN_SRS_PUSCH
}cxm_lte_channel_e;

typedef enum 
{
  CXM_SLOT_NONE,
  CXM_SLOT_RX, 
  CXM_SLOT_TX, 
  CXM_SLOT_MON 
} cxm_slot_type_e;

typedef enum 
{
  CXM_HIGH_PRIO_UNKNOWN,
  CXM_HIGH_PRIO_RACH,
  CXM_HIGH_PRIO_MEAS, 
  CXM_HIGH_PRIO_HO, 
  CXM_HIGH_PRIO_ACQ, 
  CXM_HIGH_PRIO_PAGE,
  CXM_HIGH_PRIO_SYS_UPDATE,
  CXM_HIGH_PRIO_FREQ_SCAN
} cxm_high_prio_oper_e;

/*! @brief enum to define operating mode of RATs */
typedef enum
{
  CXM_TECH_INACTIVE, /*!< RAT is either inactive or out-of-service or disabled */
  CXM_TECH_ACTIVE    /*!< RAT is in active state (transitioning between idle...connected */
} cxm_tech_op_mode_e;

/*! @brief enum to define various RAT's [layer 3] states (cummulative list) */
typedef enum
{
  CXM_TECH_L3_INACTIVE,
  CXM_TECH_L3_DISCONNECT,
  CXM_TECH_L3_ACQUISITION,
  CXM_TECH_L3_CONNECTING,
  CXM_TECH_L3_CONNECTION_ESTB,
  CXM_TECH_L3_IDLE,
  CXM_TECH_L3_CSFB_IDLE,
  CXM_TECH_L3_IDLE_NOT_CAMPED,
  CXM_TECH_L3_IDLE_CAMPED,
  CXM_TECH_L3_ACCESS,
  CXM_TECH_L3_CSFB_ACCESS,
  CXM_TECH_L3_RACH,
  CXM_TECH_L3_CONNECTED,
  CXM_TECH_L3_CELL_DCH,
  CXM_TECH_L3_CELL_FACH,
  CXM_TECH_L3_TRAFFIC,
  CXM_TECH_L3_CONNECTED_CS,
  CXM_TECH_L3_CONNECTED_PS,
  CXM_TECH_L3_CELL_PCH,
  CXM_TECH_L3_URA_PCH,
  CXM_TECH_L3_SUSPEND
} cxm_tech_l3_state_e;

/*! @brief enum to specify current antenna selection */
typedef enum
{
  CXM_TX_ANT_SEL_UNUSED,
  CXM_TX_ANT_SEL_X,
  CXM_TX_ANT_SEL_Y
} cxm_tx_ant_sel_e;

/*=============================================================================

                               DATA TYPES

=============================================================================*/

/*! @brief LTE specific sleep/wakeup state/parameters info set */
typedef struct
{
  boolean idle_flag;  /*!< LTE in idle DRX mode(True) or not(False)*/
  uint32  page_cycle; /*!< LTE periodic sleep interval in mSecs */
}cxm_lte_sleep_wakeup_info_s;

/*! @brief Union for all TECH specific data for sleep/wakeup */
typedef union
{
  cxm_lte_sleep_wakeup_info_s lte; /*!< LTE specific parameters */
}cxm_sleep_wakeup_tech_specific_params_u;

/*! @brief LTE specific info set for Tx advance notice */
typedef struct
{
  uint8             sfn;             /*!< LTE subframe number at which transmit is scheduled */
  boolean           transmit;        /*! True if transmit is scheduled  */
  uint32            ostmr_time;      /*! OSTMR time at which the LTE sub frame with the above SFN */
  uint32            ustmr_time;      /*! USTMR time at which the LTE sub frame with the above SFN */
  uint8             rbs;             /*! RB usage for the sub frame with above SFN */
  int16             tx_power;        /*! Tx power for the sub frame with above SFN */
  cxm_lte_channel_e channel_type;    /*! Channel type */ 
  int8              slot0_power;     /*! Tx Power in slot 1 */
  uint8             slot0_first_rb;  /*! First PRB allocation in slot 1 */
  uint8             slot0_last_rb;   /*! Last PRB allocation in slot 1 */
  int8              slot1_power;     /*! Tx Power in slot 2 */
  uint8             slot1_first_rb;  /*! First PRB allocation in slot 2 */
  uint8             slot1_last_rb;   /*! Last PRB allocation in slot 2 */
}cxm_lte_tx_adv_ntc_info_s;

/*! @brief Union for all TECH specific data for Tx advance notice */
typedef union
{
  cxm_lte_tx_adv_ntc_info_s lte; /*!< LTE specific parameters */
}cxm_tx_adv_ntc_tech_specific_params_u;

/*! @brief LTE specific info set for frame timing */
typedef struct
{
  uint32            dl_frame_time;   /*! DL time in USTMR for subframe 0*/
  uint32            ta;              /*! Timing advance in USTMR. WCN can use this to calculate 
                                         the UL frame timing*/
}cxm_lte_frame_timing_info_s;

/*! @brief TDS specific info set for frame timing. TDS frame timing is sent every 10ms */
typedef struct
{
  uint32            sfn;       /*! subframe number (0 to 8911)*/
  uint32            ustmr_val; /*! Corresponding ustmr value for above subframe*/  
  uint32            ta;        /*! Timing advance in USTMR. WCN can use this 
                                   to calculate the UL frame timing*/ 
}cxm_tds_frame_timing_info_s;

/*! @brief Union for all TECH specific data for frame timing */
typedef union
{
  cxm_lte_frame_timing_info_s lte; /*!< LTE specific parameters */
  cxm_tds_frame_timing_info_s tds; /*!< TDSCDMA specific parameters */
}cxm_frame_timing_tech_specific_params_u;

/*! @brief LTE specific info set for BLER metric request */
typedef struct
{
  uint32 tb_count;            /*!< LTE Transport blocks that have to be
                                     received prior to reporting BLER */
  uint32 tb_err_count_thresh; /*!< LTE Transport blocks error count that
                                     has to be meet for sending 
                                     the indication out */
}cxm_lte_bler_start_req_s;

/*! @brief Union for LTE BLER metrics request message */
typedef union
{
  cxm_lte_bler_start_req_s start_params; /*!< BLER start request specific parameters */
}cxm_lte_bler_metrics_req_u;

/*! @brief LTE specific info set for SINR metric request */
typedef struct
{
  uint32 alpha; /*!< First order filter co-efficient in Q8 format */
}cxm_lte_sinr_start_req_s;

/*! @brief LTE specific info set for SINR metric request */
typedef struct
{
  uint32 msg_id; /*!< COEX's MSG ID to include in response */
}cxm_lte_sinr_read_req_s;

typedef struct
{
  uint32 alpha; /*!< First order filter co-efficient in Q8 format */
} cxm_start_req_s;

typedef struct
{
  uint32 msg_id; /*!< COEX's MSG ID to include in response */
} cxm_read_req_s;

typedef union
{
  cxm_start_req_s start_params; /*!< Metrics start request specific parameters */
  cxm_read_req_s  read_params;  /*!< Metrics read request specific parameters */
} cxm_metric_req_u;

/*! @brief Union for LTE SINR metrics request message */
typedef union
{
  cxm_lte_sinr_start_req_s start_params; /*!< SINR start request specific parameters */
  cxm_lte_sinr_read_req_s  read_params;
}cxm_lte_sinr_metrics_req_u;

/*! @brief Parameters needed for filter running on LTE's Tx power */
typedef struct
{
  uint32 filter_alpha; /*!< alpha for Tx power filtering in Q16 format */
  uint16 threshold;    /*!< threshold to compare Tx power filter output against in mW */
}cxm_lte_tx_pwr_param_s;

/*! @brief LTE's filtered Tx power report in linear domain (mW), used for WCI2 Type7[pwr bit]*/
typedef struct
{
  boolean wci2_t7_tx_pwr_state; /*!< set TRUE if filtered Tx pwr > thld else FALSE
                                     NOTE: LTE only reports transitions 0->1 or 1->0 */
  uint16  filtered_tx_pwr_mW; /*!< output of LTE Tx power filter in linear domain (mW) */
}cxm_lte_pwr_rpt_s;

/*! @brief Union to hold Tx power report of various RATs */
typedef union
{
  cxm_lte_pwr_rpt_s lte; /*!< Holds LTE's power report in linear domain (mW) */
}cxm_tech_tx_pwr_rpt_u;

/*! @brief Info for one TDS slot */
typedef struct
{
  cxm_slot_type_e       slot_type;      /*! RX/TX/MON  */
  int16                 tx_power;       /*! Tx power for the sub frame with above SFN, unit:0.1dBm, Range:[-550,500], -640=INVALID*/
  cxm_slot_prio_e       priority;       /*! Relative priority of the TX/RX/MON activity */ 
  uint32                monitor_freq;   /*! If activity is monitoring, the freq that is being monitored, in MHz */  
} cxm_tds_slot_info_s;

/*! @brief Info for one TDS sfn containing 9 slots */
typedef struct
{
  uint16               sfn;                      /*! TDS subframe number*/
  cxm_tds_slot_info_s  slot[CXM_NUM_TDS_SLOTS];  /*! TDS slots, 0=TS0, 1=DwPTS,2=UpPTS, 3=TS1,4=TS2, ..., 8=TS6 */ 
} cxm_tds_sfn_adv_ntc_info_s;

typedef union
{
  cxm_tds_sfn_adv_ntc_info_s tds; /*!< TDS specific parameters */
} cxm_activity_adv_ntc_tech_specific_params_u;

typedef struct
{
  cxm_wcn_tx_client_e clnt;
  uint32              status;
  uint32              last_rep_tx;
  uint32              last_rsp_code;
} cxm_async_log_wcn_tx_s;

typedef union
{
  cxm_rx_tx_activity_info_u fw_activity;
  cxm_async_log_wcn_tx_s    wcn_tx;
}cxm_async_log_paylds_u;

typedef struct
{
  uint32                  log_id;
  cxm_async_log_paylds_u  payld;
}cxm_async_log_data_s;

/*!@brief structure to hold RAT's [layer 3] state */
typedef struct
{
  cxm_tech_op_mode_e       mode;  /*!< WWAN tech's operating mode (active/inactive) */
  cxm_tech_l3_state_e      state; /*!< WWAN tech's layer 3 (RRC/RR/CP) state */
}cxm_tech_l3_state_s;

/*=============================================================================

                              MESSAGE PAYLOADS

	The idea here is to define payloads which can be used by any underlying
    layers into the MSGR message definitions

=============================================================================*/

/*! @brief Generic message used by WWAN techs to request information */
typedef struct
{
  msgr_hdr_struct_type msg_hdr; /*!< msgr header containing msg_id */
}cxm_coex_generic_req_s;

/*! @brief Message sent by CxM controller to all WWAN techs at boot to configure. This 
    message is deprecated since the boot message in cxm_fw_msg.h is now used.
    @note  This message is version controlled as it is also used by WWAN tech LLs */
typedef struct
{
  msgr_hdr_struct_type  msg_hdr;     /*!< msgr header containing msg_id */
  cxm_coex_sys_config_t sys_config;  /*!< system-wide config for WWAN techs */
  cxm_boot_config_v01_s boot_config; /*!< system-wide boot params for WWAN techs */
}cxm_coex_boot_params_v01_ind_s;

/*! @brief Message sent by CxM controller to LTE with current CxM policy to follow */
typedef struct
{
  msgr_hdr_struct_type    msg_hdr;              /*!< msgr header containing msg_id */
  cxm_active_config_v01_s active_config;        /*!< DO NOT USE -- iface transition only, to be removed */
  cxm_active_config_v02_s active_cfg;           /*!< current active cxm polcy for LTE */
  uint32                  tx_bw_filter_alpha;   /*!< alpha for RB (LTE bw) filtering in Q8 format */
  cxm_lte_tx_pwr_param_s  tx_pwr_filter_params; /*!< params for Tx power filtering */
}cxm_coex_active_policy_lte_ind_s;

/*! @brief Message sent by CxM controller to WWAN techs with current CxM policy to follow */
typedef struct
{
  msgr_hdr_struct_type    msg_hdr;            /*!< msgr header containing msg_id */
  cxm_active_config_v01_s active_config;      /*!< current active cxm polcy */
  uint32                  tx_bw_filter_alpha; /*!< alpha for RB filtering in Q8 format */ 
}cxm_coex_active_policy_ind_s;

/*! @brief Message used by WWAN techs to send sleep or wakeup notifications */
typedef struct
{
  msgr_hdr_struct_type                    msg_hdr;           /*!< msgr header containing msg_id */
  cxm_tech_type                           tech_id;           /*!< WWAN Technology */
  boolean                                 is_going_to_sleep; /*!< Sleep(True) or Wakeup(False) */
  uint32                                  duration;          /*!< Duration to above event in mSecs */
  cxm_sleep_wakeup_tech_specific_params_u params;            /*!< Tech specific params for sleep/wakeup */
}cxm_coex_tech_sleep_wakeup_duration_ind_s;

/*! @brief Message used by WWAN tech(s) to report current WCN requested denial/blanking stats */
typedef struct
{
  msgr_hdr_struct_type         msg_hdr; /*!< msgr header containing msg_id */
  cxm_wcn_txfrmdnl_stats_v01_s stats;   /*!< current WCN requested Tx frame denial/blanking report */
}cxm_coex_wcn_txfrndnl_report_ind_s;
typedef struct
{
  msgr_hdr_struct_type         msg_hdr; /*!< msgr header containing msg_id */
  cxm_tech_type                tech_id; /*!< WWAN Technology */
  cxm_wcn_txfrmdnl_stats_v01_s stats;   /*!< current WCN requested Tx frame denial/blanking report */
}cxm_coex_tech_tx_frm_dnl_report_ind_s;

/*! @brief Message used by WWAN techs to advance notifications of Tx(s) */
typedef struct
{
  msgr_hdr_struct_type                  msg_hdr; /*!< msgr header containing msg_id */
  cxm_tech_type                         tech_id; /*!< WWAN Technology */
  cxm_tx_adv_ntc_tech_specific_params_u params;  /*!< Tech specific params for tx adv notice */
}cxm_coex_tech_tx_adv_ntc_ind_s;

/*! @brief Message used by WWAN-LTE tech as a response to CXM's Tx pwr lmt conditions check query */
typedef struct
{
  msgr_hdr_struct_type msg_hdr;     /*!< msgr header containing msg_id */
  uint32               rb_count;    /*!< count of RB values filtered */
  uint32               rb_filtered; /*!< filtered RB values in Q8 Format */  
  boolean              rrc_state;   /*!< RRC state info */
  int16                dl_pathloss; /*!< DL path loss in dB */
}cxm_coex_tx_pwr_lmt_lte_cndtns_rsp_s;

/*! @brief Message used by WWAN-LTE tech to inform CXM controller of change in LTE RRC state */
typedef struct
{
  msgr_hdr_struct_type msg_hdr;     /*!< msgr header containing msg_id */
  boolean              rrc_state;   /*!< RRC state info */
}cxm_coex_tx_pwr_lmt_lte_cndtns_ind_s;

/*! @brief Message used by CXM controller to request WWAN-LTE tech to start/stop collecting BLER metrics 
    @note  Message sender must provide appropriate 'payload' associate to the action (if any)
             if 'action' == START, then sender MUST provide valid 'tb_count' & 'tb_err_count_thresh' values in 'payload'
             if 'action' == STOP, then 'payload' is DON'T CARE */
typedef struct
{
  msgr_hdr_struct_type       msg_hdr;  /*!< msgr header containing msg_id */
  cxm_action_e               action;   /*!< whether to START/STOP metric collection */
  cxm_lte_bler_metrics_req_u payload;  /*!< associated payload for the type of action */
}cxm_coex_metrics_lte_bler_req_s;

/*! @brief Message used by WWAN-LTE tech to report collected BLER metrics */
typedef struct
{
  msgr_hdr_struct_type msg_hdr;               /*!< msgr header containing msg_id */
  uint32               tb_count;              /*!< Transport blocks sent */
  uint32               tb_err_count;          /*!< Transport blocks error count */
  uint32               tb_count_bt;           /*!< Transport blocks sent for only bluetooth active bucket*/
  uint32               tb_err_count_bt;       /*!< Transport blocks error count for only bluetooth active bucket*/
  uint32               tb_count_wifi;         /*!< Transport blocks sent for only WIFI active bucket*/
  uint32               tb_err_count_wifi;     /*!< Transport blocks error count for only WIFI active bucket*/
  uint32               tb_count_bt_wifi;      /*!< Transport blocks sent for bluetooth and WIFI active bucket*/
  uint32               tb_err_count_bt_wifi;  /*!< Transport blocks error count for bluetooth and WIFI active bucket*/
  uint32               tb_count_lte_only;     /*!< Transport blocks sent for only LTE active bucket*/
  uint32               tb_err_count_lte_only; /*!< Transport blocks error count for only LTE active bucket*/
}cxm_coex_metrics_lte_bler_ind_s;

/*! @brief Message used by CXM controller to request WWAN-LTE tech to start/read/stop collecting SINR metrics 
    @note  Message sender must provide appropriate 'payload' associate to the action (if any)
             if 'action' == START, then sender MUST provide 'alpha' in 'payload'
             if 'action' == READ, then sender MUST provide valid 'msg_id' in 'payload'
             if 'action' == STOP, then 'payload' is DON'T CARE */
typedef struct
{
  msgr_hdr_struct_type       msg_hdr; /*!< msgr header containing msg_id */
  cxm_action_e               action;  /*!< whether to START/READ/STOP metric collection */
  cxm_lte_sinr_metrics_req_u payload; /*!< associated payload for the type of action */
  cxm_carrier_e              carrier; /*!< carrier for which to perform this action */
}cxm_coex_metrics_lte_sinr_req_s;

/*! @brief Message used by CXM controller to request WWAN tech to start/read/stop collecting metrics 
    @note  Message sender must provide appropriate 'payload' associate to the action (if any)
             if 'action' == START, then sender MUST provide 'alpha' in 'payload'
             if 'action' == READ, then sender MUST provide valid 'msg_id' in 'payload'
             if 'action' == STOP, then 'payload' is DON'T CARE */
typedef struct
{
  msgr_hdr_struct_type       msg_hdr; /*!< msgr header containing msg_id */
  cxm_action_e               action;  /*!< whether to START/READ/STOP metric collection */
  cxm_metric_req_u           payload; /*!< associated payload for the type of action */
}cxm_coex_metrics_req_s;

/*! @brief Message used by WWAN-LTE tech to respond to 'read' request with collected SINR metrics -- per LTE carrier */
typedef struct
{
  msgr_hdr_struct_type msg_hdr;                   /*!< msgr header containing msg_id */
  uint32               msg_id;                    /*! MSG ID received in 'read' request */
  uint32               sinr_count;                /*! Count of sinr values filtered */
  uint32               sinr_count_bt;             /*! Count of sinr values filtered for only bluetooth active bucket*/
  uint32               sinr_count_wifi;           /*! Count of sinr values filtered for only WIFI active bucket*/
  uint32               sinr_count_bt_wifi;        /*! Count of sinr values filtered for bluetooth and WIFI active bucket*/
  uint32               sinr_count_lte_only;       /*! Count of sinr values filtered for only LTE active bucket*/
  int32                filtered_sinr_db;          /*! Filtered SINR value in db */
  int32                filtered_sinr_bt_db;       /*! Filtered SINR value in db for only bluetooth active bucket */
  int32                filtered_sinr_wifi_db;     /*! Filtered SINR value in db for only WIFI active bucket */
  int32                filtered_sinr_bt_wifi_db;  /*! Filtered SINR value in db for bluetooth and WIFI active bucket */
  int32                filtered_sinr_lte_only_db; /*! Filtered SINR value in db for only LTE active bucket */
  errno_enum_type      status;                    /*! Status if the read was successful */ 
  cxm_carrier_e        carrier;                   /*! Carrier ID received in the 'read' request */
}cxm_coex_metrics_lte_sinr_rsp_s;

typedef struct
{
  msgr_hdr_struct_type msg_hdr;                 /*!< msgr header containing msg_id */
  cxm_tech_type        tech_id;                 /*!< WWAN Technology */
  uint32               msg_id;                  /*! MSG ID received in 'read' request */
  uint32               metric_cnt;              /*! overall metric count */
  uint32               metric_cnt_bt;           /*! bt active count of metric */
  uint32               metric_cnt_wifi;         /*! wifi count of metric */
  uint32               metric_cnt_bt_wifi;      /*! bt and wifi count of metric */
  uint32               metric_cnt_mdm_only;     /*! modem only count of metric */
  int32                filt_metric;             /*! overall filtered metric */
  int32                filt_metric_bt;          /*! bt active filtered metric */
  int32                filt_metric_wifi;        /*! wifi active filtered metric */
  int32                filt_metric_bt_wifi;     /*! bt and wifi filtered metric */
  int32                filt_metric_mdm_only;    /*! modem only filtered metric */
  errno_enum_type      status;                  /*! Status if the read was successful */ 
}cxm_coex_metrics_rsp_s;

/*! @brief Message used by WWAN-LTE tech to respone to 'read' request with collected SINR metrics */
typedef struct
{
  msgr_hdr_struct_type                    msg_hdr; /*!< msgr header containing msg_id */
  cxm_tech_type                           tech_id; /*!< WWAN Technology */
  cxm_frame_timing_tech_specific_params_u params;  /*!< frame timing info for tech */
}cxm_coex_frame_timing_ind_s;

/*! @brief this message is used to inform of a high priority event and can be used in 5 ways:
1.) The start/end USTMR times are known - fill in these fields
2.) Start USTMR is know at the beginning of the event but the end is not known - fill in
    the start time and make end time 0xFFFFFFFF
3.) The end time eventually becomes known (it was not known at the beginning) - fill in 
    0xFFFFFFFF for the start time and fill the appropriate end time
4.) Fill start = end = 0xFFFFFFFF to indicate a 'start' event happening now
5.) Fill start = end = 0x00 to indicate an 'end' event happening now */
typedef struct
{
  msgr_hdr_struct_type  msg_hdr;                         /*!< msgr header containing msg_id */
  cxm_tech_type         tech_id;                         /*!< WWAN Technology */
  cxm_high_prio_oper_e  op_id;                           /*!< High Prio Operation */
  uint32                start;                           /*! Start time of the high priority event */
  uint32                end;                             /*! End time of the high prioriy event. */
  uint8                 num_freqs;                       /*! Number of high priority frequencies. */
  uint32                freq[COEX_MAX_HIGH_PRIO_FREQ];   /*! Frequency of high priority event */         
}cxm_coex_high_prio_ind_s;

/*! @brief this message is used to inform of RAT's Tx power report */
typedef struct
{
  msgr_hdr_struct_type  msg_hdr; /*!< msgr header containing msg_id */
  cxm_tech_type         tech_id; /*!< WWAN Technology */
  cxm_tech_tx_pwr_rpt_u report;  /*!< RAT's Tx power report */
}cxm_coex_tech_tx_pwr_rpt_ind_s;

/*! @brief Message used by WWAN techs to advance notifications of Tx(s) */
typedef struct
{
  msgr_hdr_struct_type                        msg_hdr; /*!< msgr header containing msg_id */
  cxm_tech_type                               tech_id; /*!< WWAN Technology */
  cxm_activity_adv_ntc_tech_specific_params_u params;  /*!< Tech specific params for tx adv notice */
}cxm_coex_tech_activity_adv_ntc_ind_s;

typedef struct
{
  msgr_hdr_struct_type     msg_hdr; /*!< msgr header containing msg_id */
  uint64                   ts;
  uint32                   event;   /*!< trace event */
  uint8                    num_logs;
  cxm_async_log_data_s     data[COEX_MAX_ASYNC_LOGS];
}cxm_coex_log_async_ind_s;

/*! @brief this message is used to inform of RAT's [layer 3] state */
typedef struct
{
  msgr_hdr_struct_type     msg_hdr;  /*!< msgr header containing msg_id */
  cxm_tech_type            tech;     /*!< WWAN tech ID */
  cxm_tech_l3_state_s      l3;       /*!< WWAN tech's current layer 3 state*/
}cxm_coex_tech_l3_state_ind_s;

/*! @brief this message is used to inform of MDM's Tx antenna selection */
typedef struct
{
  msgr_hdr_struct_type     msg_hdr;    /*!< msgr header containing msg_id */
  cxm_tx_ant_sel_e         tx_ant_sel; /*!< current Tx antenna in use */
}cxm_coex_tx_ant_sel_ind_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  cxm_wcn_tx_enable

===========================================================================*/
/*!
  @brief
    Power on the uart block. This must be called by clients
    before using cxm_wcn_tx_start and cxm_wcn_tx_stop. When WLAN TX is not
    needed for some time, cxm_wcn_tx_disable must be called.

  @return
    cxm_wcn_tx_rsp_codes_e
*/
/*=========================================================================*/
cxm_wcn_tx_rsp_codes_e cxm_wcn_tx_enable (
  cxm_wcn_tx_client_e client
);

/*===========================================================================

  FUNCTION:  cxm_wcn_tx_disable

===========================================================================*/
/*!
  @brief
    Power off the uart block. 

  @return
    cxm_wcn_tx_rsp_codes_e
*/
/*=========================================================================*/
cxm_wcn_tx_rsp_codes_e cxm_wcn_tx_disable (
  cxm_wcn_tx_client_e client
);

/*===========================================================================

  FUNCTION:  cxm_wcn_tx_start

===========================================================================*/
/*!
  @brief
    To start collecting WLAN tx state for this client.
 
  @return
    cxm_wcn_tx_rsp_codes_e
*/
/*=========================================================================*/
cxm_wcn_tx_rsp_codes_e cxm_wcn_tx_start (
  cxm_wcn_tx_client_e client
);

/*===========================================================================

  FUNCTION:  cxm_wcn_tx_stop

===========================================================================*/
/*!
  @brief
    To stop collecting WLAN tx state for this client and return the tx state for
    the period. cxm_wcn_tx_start must be called before calling this. For the
    tx_state parameter, pass in a pointer to a defined instance of
    cxm_wcn_tx_state_e.

  @return
    cxm_wcn_tx_rsp_codes_e
*/
/*=========================================================================*/
cxm_wcn_tx_rsp_codes_e cxm_wcn_tx_stop (
  cxm_wcn_tx_client_e client,
  cxm_wcn_tx_state_e *tx_state
);

/*===========================================================================

  FUNCTION:  cxm_set_tech_l3_state

===========================================================================*/
/*!
  @brief
    To set WWAN tech's layer3 state
 
  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type cxm_set_tech_l3_state (
  cxm_tech_type        tech,
  cxm_tech_l3_state_s *l3
);

/*===========================================================================

  FUNCTION:  cxm_set_tx_ant_sel

===========================================================================*/
/*!
  @brief
    Used to set transmit (tx) antenna selected
 
  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type cxm_set_tx_ant_sel (
  cxm_tx_ant_sel_e tx_ant_sel
);

/*=============================================================================

                        MSGR UMID Definitions

=============================================================================*/

/* @brief These indications are used for WCN - WWAN Coexistence mitigation
 */
enum
{
  /* Sent out by CXM controller to inform all WWAN techs of current configuration to follow */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_BOOT_PARAMS,
                    COEX_BOOT_PARAMS_ID, cxm_coex_boot_params_ind_s ),

  /* Used by WWAN techs to request for system's coexistence configuration */
  MSGR_DEFINE_UMID( MCS, CXM, REQ, COEX_BOOT_PARAMS,
                    COEX_BOOT_PARAMS_ID, cxm_coex_generic_req_s ),

  /* Sent out by CXM controller to inform WWAN-LTE tech of current coexistence policy to impose */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_ACTIVE_POLICY_LTE,
                    COEX_ACTIVE_POLICY_LTE_ID, cxm_coex_active_policy_lte_ind_s ),

  /* Used by WWAN-LTE tech to request current coexistence policy for LTE in place */
  MSGR_DEFINE_UMID( MCS, CXM, REQ, COEX_ACTIVE_POLICY_LTE,
                    COEX_ACTIVE_POLICY_LTE_ID, cxm_coex_generic_req_s ),

  /* Used by WWAN techs to indicate their upcoming 'sleep' or 'wakeup' notification */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TECH_SLEEP_WAKEUP,
                    COEX_TECH_SLEEP_WAKEUP_ID, cxm_coex_tech_sleep_wakeup_duration_ind_s ),

  /* Used by WWAN tech(s) to report current WCN requested Tx frame denial/blanking stats */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_WCN_TXFRMDNL_REPORT,
                    COEX_WCN_TXFRMDNL_REPORT_ID, cxm_coex_wcn_txfrndnl_report_ind_s ),
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TECH_TX_FRM_DNL_REPORT,
                    COEX_TECH_TX_FRM_DNL_REPORT_ID, cxm_coex_tech_tx_frm_dnl_report_ind_s ),

  /* Used by WWAN tech(s) to report next Tx frame number/id as advance notice */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TECH_TX_ADV_NTC,
                    COEX_TECH_TX_ADV_NTC_ID, cxm_coex_tech_tx_adv_ntc_ind_s ),

  /* Used by CXM controller to request for WWAN-LTE tech's conditions (to enforce Tx pwr lmts) */
  MSGR_DEFINE_UMID( MCS, CXM, REQ, COEX_TX_PWR_LMT_LTE_CNDTNS,
                    COEX_TX_PWR_LMT_LTE_CNDTNS_ID, cxm_coex_generic_req_s ),

  /* Used by WWAN-LTE tech as a response to CXM's Tx pwr lmt conditions check query */
  MSGR_DEFINE_UMID( MCS, CXM, RSP, COEX_TX_PWR_LMT_LTE_CNDTNS,
                    COEX_TX_PWR_LMT_LTE_CNDTNS_ID, cxm_coex_tx_pwr_lmt_lte_cndtns_rsp_s),

  /* Used by WWAN-LTE tech to inform CXM controller of change in LTE RRC state */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TX_PWR_LMT_LTE_CNDTNS,
                    COEX_TX_PWR_LMT_LTE_CNDTNS_ID, cxm_coex_tx_pwr_lmt_lte_cndtns_ind_s),

  /* Used by CXM controller to request WWAN-LTE tech to start/stop collecting BLER metrics */
  MSGR_DEFINE_UMID( MCS, CXM, REQ, COEX_METRICS_LTE_BLER,
                    COEX_METRICS_LTE_BLER_ID, cxm_coex_metrics_lte_bler_req_s ),

  /* Used by WWAN-LTE tech to report collected BLER metrics when threshold is crossed */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_METRICS_LTE_BLER,
                    COEX_METRICS_LTE_BLER_ID, cxm_coex_metrics_lte_bler_ind_s ),

  /* Used by CXM controller to request WWAN-LTE tech to start/read/stop collecting SINR metrics */
  MSGR_DEFINE_UMID( MCS, CXM, REQ, COEX_METRICS_LTE_SINR,
                    COEX_METRICS_LTE_SINR_ID, cxm_coex_metrics_lte_sinr_req_s ),

  /* Used by WWAN-LTE tech to report collected SINR metrics in response to 'read' request */
  MSGR_DEFINE_UMID( MCS, CXM, RSP, COEX_METRICS_LTE_SINR,
                    COEX_METRICS_LTE_SINR_ID, cxm_coex_metrics_lte_sinr_rsp_s ),

  /* Used by WWAN techs to report frame timing in ustmr */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TECH_FRAME_TIMING,
                    COEX_TECH_FRAME_TIMING_ID, cxm_coex_frame_timing_ind_s ),

  /* Used by WWAN tech(s) to report activity as advance notice */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TECH_ACTIVITY_ADV_NTC,
                    COEX_TECH_ACTIVITY_ADV_NTC_ID, cxm_coex_tech_activity_adv_ntc_ind_s ),

  /* Used by CXM controller to request WWAN-TDSCDMA tech to start/read/stop collecting metrics */
  MSGR_DEFINE_UMID( MCS, CXM, REQ, COEX_METRICS_TDSCDMA,
                    COEX_METRICS_TDSCDMA_ID, cxm_coex_metrics_req_s ),

  /* Used by CXM controller to request WWAN-GSM1 tech to start/read/stop collecting metrics */
  MSGR_DEFINE_UMID( MCS, CXM, REQ, COEX_METRICS_GSM1,
                    COEX_METRICS_GSM1_ID, cxm_coex_metrics_req_s ),

  /* Used by CXM controller to request WWAN-GSM2 tech to start/read/stop collecting metrics */
  MSGR_DEFINE_UMID( MCS, CXM, REQ, COEX_METRICS_GSM2,
                    COEX_METRICS_GSM2_ID, cxm_coex_metrics_req_s ),

  /* Used by CXM controller to request WWAN-GSM3 tech to start/read/stop collecting metrics */
  MSGR_DEFINE_UMID( MCS, CXM, REQ, COEX_METRICS_GSM3,
                    COEX_METRICS_GSM3_ID, cxm_coex_metrics_req_s ),

  /* Used by WWAN tech to report collected metrics in response to 'read' request */
  MSGR_DEFINE_UMID( MCS, CXM, RSP, COEX_TECH_METRICS,
                    COEX_TECH_METRICS_ID, cxm_coex_metrics_rsp_s ),

  /* Sent out by CXM controller to inform WWAN-TDSCDMA tech of current coexistence policy to impose */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_ACTIVE_POLICY_TDSCDMA,
                    COEX_ACTIVE_POLICY_TDSCDMA_ID, cxm_coex_active_policy_ind_s ),

  /* Sent out by CXM controller to inform WWAN-GSM1 tech of current coexistence policy to impose */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_ACTIVE_POLICY_GSM1,
                    COEX_ACTIVE_POLICY_GSM1_ID, cxm_coex_active_policy_ind_s ),

  /* Sent out by CXM controller to inform WWAN-GSM2 tech of current coexistence policy to impose */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_ACTIVE_POLICY_GSM2,
                    COEX_ACTIVE_POLICY_GSM2_ID, cxm_coex_active_policy_ind_s ),

  /* Sent out by CXM controller to inform WWAN-GSM3 tech of current coexistence policy to impose */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_ACTIVE_POLICY_GSM3,
                    COEX_ACTIVE_POLICY_GSM3_ID, cxm_coex_active_policy_ind_s ),

  /* Sent out by WWAN techs to CXM to indicate a high priority event */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_HIGH_PRIORITY,
                    COEX_HIGH_PRIORITY_ID, cxm_coex_high_prio_ind_s ),

  /* Sent out by WWAN techs to CXM to inform of RAT Tx power stats */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TECH_TX_PWR_REPORT,
                    COEX_TECH_TX_PWR_REPORT_ID, cxm_coex_tech_tx_pwr_rpt_ind_s ),

  /* Sent out by CXM when logging is done in another task's context */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_LOG_ASYNC,
                    COEX_LOG_ASYNC_ID, cxm_coex_log_async_ind_s ),

  /* Sent out by WWAN techs to report current layer3 (RRC/CP/RR) states */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TECH_L3_STATE,
                    COEX_TECH_L3_STATE_ID, cxm_coex_tech_l3_state_ind_s ),

  /* Sent to CXM to inform of modem's current Tx antenna selection */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TX_ANT_SEL,
                    COEX_TX_ANT_SEL_ID, cxm_coex_tx_ant_sel_ind_s ),
};

#endif /* __WCN_COEX_MGR_H__ */
