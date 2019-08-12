#ifndef __COEX_ALGOS_H__
#define __COEX_ALGOS_H__
/*!
  @file coex_algos.h

  @brief
  COEX algos & process function declarations
 
  @note
  These set of declarations are internal to the COEX libs

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/coex_algos.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/20/15   ckk     Update type7[rrc_c] using RAT layer 3 states
01/14/15   ckk     Add support for Tx antenna via WCI2 MWS_Connected Type 7
07/28/14   tak     Storage structures for per tech policy params
07/14/14   ckk     Added WCI2 type4 & type7[pwr_bit] support
03/01/13   tak     Moved to new interface structures owned by CXM
01/10/13   tak     Added CA and TDS support for WWAN update and sleep indication 
12/19/12   btl     Added support for band filtering
04/08/12   ckk     Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include <qmi_csi.h>
#include <atomic_ops.h>
#include "coexistence_service_v01.h"
#include <wci2_uart.h>
#include "npa.h"
#include "cxm_msgr.h"
#include "mcs_hwio.h"
#include "cxm.h"
#include <wci2_core.h>

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/
#define COEX_CONFLICT_MAX_TECHS 3
#define COEX_MAX_VICTIM_TBL_ENTRIES 32

#define CXM_CONFIG_VERSION_8     8 /* current version, only one supported here*/

/* channel avoidance only */
#define CXM_COEX_PROTOCOL_1  0x01
/*channel avoidance, WCI-2 */
#define CXM_COEX_PROTOCOL_3  0x03
/*WCI-2 with victim table*/
#define CXM_COEX_PROTOCOL_4  0x04
/* Control- & data-planes over WCI-2 */
#define CXM_COEX_PROTOCOL_5  0x05
/* Control- & data-planes over WCI-2, antenna sharing */
#define CXM_COEX_PROTOCOL_6  0x06
/* Data plane over SMEM with victim table */
#define CXM_COEX_PROTOCOL_8  0x08
/* Victim table, MCS-triggered LTE TX Adv */
#define CXM_COEX_PROTOCOL_9  0x09
/* Setup for type 0/1 operation */
#define CXM_COEX_PROTOCOL_10  0x0A

/* Data plane over SMEM  with SAWless RF card */
#define CXM_COEX_PROTOCOL_12  0x0C
/* Data plane over SMEM  with Suboptimal Filter RF card */
#define CXM_COEX_PROTOCOL_13  0x0D
/* Data plane over SMEM  with SAWless and Suboptimal Filter RF card */
#define CXM_COEX_PROTOCOL_14  0x0E

/*Masks for WLAN types*/
#define CXM_WLAN_CONN_TYPE        0x00000001
#define CXM_WLAN_SCAN_TYPE        0x00000002
#define CXM_WLAN_HIGH_PRIO_TYPE   0x00000004

/*max RB thresholds per conflict*/
#define CXM_MAX_RB_THRES_PER_CONF  6

/*read current time in USTMR*/
#if defined(HWIO_UNIV_STMR_MSTMR_IN)
  #define COEX_READ_USTMR_TIME() HWIO_UNIV_STMR_MSTMR_IN
#else
  #define COEX_READ_USTMR_TIME() 0
#endif
/*USTMR max count*/
#define COEX_USTMR_MAX             0x07FFFFFF 
/*Routine returns (A + B)*/
#define COEX_ADD_USTMR( a, b ) \
        ( ( (a) + (b) < (COEX_USTMR_MAX) ) ? ( (a) + (b) ) : ( (a) + (b) - (COEX_USTMR_MAX) ) )
/*Routine returns (A - B)*/
#define COEX_SUB_USTMR( a, b ) \
        ( ( (a) >= (b) ) ? ( (a) - (b) ) : ( (COEX_USTMR_MAX) + (a) - (b) ) )

/*100 us to ustmr ticks*/
#define COEX_CONVERT_100MICROSECS_USTMR  1920   

typedef enum
{
  COEX_WCI2_T7_CONN_ST_EVNT,
  COEX_WCI2_T7_PWR_ST_EVNT,
  COEX_WCI2_T7_TX_ANT_SEL_EVNT,
} coex_wci2_t7_event_e;

#define COEX_WCI2_T7_CONN_ST_EVNT_MSK    ((uint32)(1<<COEX_WCI2_T7_CONN_ST_EVNT))
#define COEX_WCI2_T7_PWR_ST_EVNT_MSK     ((uint32)(1<<COEX_WCI2_T7_PWR_ST_EVNT))
#define COEX_WCI2_T7_TX_ANT_SEL_EVNT_MSK ((uint32)(1<<COEX_WCI2_T7_TX_ANT_SEL_EVNT))
#define COEX_WCI2_T7_ALL_EVNT_MSK ((uint32)(COEX_WCI2_T7_CONN_ST_EVNT_MSK | \
                                            COEX_WCI2_T7_PWR_ST_EVNT_MSK) | \
                                            COEX_WCI2_T7_TX_ANT_SEL_EVNT_MSK)
#define COEX_WCI2_T7_NO_EVNT_MSK ((uint32)~COEX_WCI2_T7_ALL_EVNT_MSK)

typedef enum
{
  CXM_TECH_STATE_INACTIVE,
  CXM_TECH_STATE_IDLE,
  CXM_TECH_STATE_CONN
} cxm_tech_conn_state_e;

typedef enum
{
  CXM_TECH_STATE_SLEEP,
  CXM_TECH_STATE_AWAKE
} cxm_tech_sleep_state_e;

/* indicates whether any policy is currently active */
typedef enum
{
  COEX_POLICY_INACTIVE,
  COEX_POLICY_ACTIVE
} coex_policy_state_e;

/* storage for global policy params */
typedef struct
{
  /* link path-loss threshold to observe before deciding to enforce Power limit */
  float    link_path_loss_threshold;

  /* alpha value to apply to filtered rb threshold */
  float    rb_filter_alpha;

  /* filtered first-order RB usage count threshold for power limit */
  float    filtered_rb_threshold;

  /* Timeout value (in ms) for overall RAT conn state timer */
  uint32   t7_con_holdoff;
  
  /* Alpha coefficient for the first-order filter for the RAT (LTE) Tx power state */
  float    t7_pwr_alpha;
  
  /* Power threshold (in mW) to decide whether to send type 7 message */
  uint16_t t7_pwr_thresh;
  
  /* Timeout value (in milliseconds) for the timer that is set for cummulative RAT power state */
  uint32   t7_pwr_holdoff;
  
} coex_plcy_parms_s;

/* storage for lte related policy parameters coming from QMI */
typedef struct
{
  /* The AP's current selection of the APT table for the system's RF */
  coex_apt_table_enum_v01 apt_table;

  /* Controllers's LTE Tx power limit (in dBM) */
  float                   controller_tx_power_limit;

  /* WCI-2's LTE Tx power limit (in dBM) */
  float                   wci2_power_limit;

  /* timeout for WCI-2 power limit */
  uint16_t                wci2_tx_pwrlmt_timeout;

  /* timeout for cotroller power limit */
  uint16_t                controller_tx_pwrlmt_timeout;

  /* power threshold for TX adv notice */
  int16_t                 tx_power_threshold_for_adv_tx_notice;

  /* RB threshold for TX adv notice */
  uint8_t                 rb_threshold_for_adv_tx_notice;
} coex_lte_policy_params_s;

/* policy parameters specific to techs */
typedef union
{
  coex_lte_policy_params_s lte;
} coex_tech_spec_policy_params_u;

/* storage for policy parameters coming from QMI per tech */
typedef struct
{
  /* Power threshold (in dBM) to decide whether to react to WCI-2's WCN priority */
  int8_t                                       power_threshold;

  /* RB count threshold to decide if there is a need to react to WCI-2's WCN priority */
  uint32_t                                     rb_threshold;

  /* Maximum number of continuous Tx sub-frame denials */
  uint32_t                                     tx_continuous_subframe_denials_threshold;

  /* Tx subframe denials allowed in the system while reacting to WCI-2's WCN priority */
  coex_frame_denial_duty_cycle_params_type_v01 tx_subrame_denial_params;

  coex_tech_spec_policy_params_u               params;
} coex_tech_policy_params;

/* storage for the latest tech info from different techs*/
typedef struct _coex_wwan_tech_info 
{
  /*contains all the state info*/
  cxm_tech_data_s             state_data;

  /*band filter for this technology. 
  If an entry is TRUE, we send that band in state indication*/
  boolean                     tech_band_filter[CXM_MAX_SUPPORTED_LINK_SETS];

  /*the threshold with which to send sleep indications*/
  uint32                      sleep_thresh;

  /* timestamp of tech's next wakeup */
  uint64                      wakeup_tick;

  /* tech connected/idle/sleep state */
  cxm_tech_conn_state_e       conn_state;

  /* tech sleep/wake state */
  cxm_tech_sleep_state_e      sleep_state;

  /* tech's Layer 3 (RRC/CP/RR) state */
  cxm_tech_l3_state_s         l3;

  /* flag indicating that metrics have been started
    LTE/GSM - SINR
    TDS - Narrowband Noise */
  atomic_word_t               metrics_started;

  /* policy for this tech */
  coex_policy_config_mask_v01 policy;

  /* policy paramters */
  coex_tech_policy_params     plcy_parms;

  /* policy state for this tech */
  coex_policy_state_e         policy_state;

  /*victim table index of currently active conflict if any*/
  uint8                       active_conf_index;

} coex_wwan_tech_info;

/* linked list node for storing active wlan connections */
typedef struct _coex_wlan_conn_node_type
{
  coex_wlan_conn_info_type_v01 conn;
  struct _coex_wlan_conn_node_type *next;
} coex_wlan_conn_node_type;

/* linked list node for storing active wlan high prio events */
typedef struct _coex_wlan_hp_node_type
{
  coex_wlan_high_prio_info_type_v01 high_prio;
  struct _coex_wlan_hp_node_type *next;
} coex_wlan_hp_node_type;

/* coex storage for a lte/wlan conflict */
typedef struct
{
  uint32 wlan_freq;
  uint32 lte_freq;
} coex_lte_wlan_conflict_type;

/* aggregate storage for all lte/wlan conflicts */
typedef struct
{
  uint8 num_entries;
  coex_lte_wlan_conflict_type tbl[COEX_MAX_VICTIM_TBL_ENTRIES];
} coex_conflict_tbl_type;

/* state used for tracking when uart can be powered off */
typedef struct
{
  uint32           active_tech_mask;
  uint32           conn_tech_mask;
  uint32           tx_pwr_tech_mask;
  boolean          uart_en_state;
  uint32           uart_handle;
  cxm_tx_ant_sel_e tx_ant_sel;
} coex_state_type;

/* state for tracking power limiting through controller and 3rd party */
typedef enum
{
  COEX_PWR_LMT_OFF,
  COEX_PWR_LMT_PENDING,
  COEX_PWR_LMT_RRC_PROC_ONGOING,
  COEX_PWR_LMT_ACTIVE
} coex_power_limit_state_e;

/* TX Advanced line behavior when there is a late message */
typedef enum
{
  COEX_STATE_LAST,
  COEX_STATE_ON,
  COEX_STATE_OFF
} coex_late_tx_adv_toggle_type;

typedef struct
{
  /* Start frequency for the band in KHz */
  uint32 freq_start;
  /* Stop frequency for the band in KHz */
  uint32 freq_stop;
} coex_tech_band_s;

/* structure for storing band filters */
typedef struct
{
  /*band filtering will only take place if turned on*/
  boolean filter_on;

  uint32 ul_len;
  uint32 dl_len;

  coex_tech_band_s ul_filter[COEX_WWAN_MAX_BANDS_TO_MONITOR_V01];
  coex_tech_band_s dl_filter[COEX_WWAN_MAX_BANDS_TO_MONITOR_V01];

} coex_band_filter_s;

/* structure for storing SCAN frequency band filter */
typedef struct
{
  /* band filtering will only take place if turned on */
  boolean filter_on;

  /* filter setup via the QMI interface */
  uint32  bands_len;
  coex_band_type_v01 bands[COEX_WWAN_MAX_BANDS_TO_MONITOR_V01];

  /* processed filter start-stop frequencies */
  coex_tech_band_s processed_filter[COEX_WWAN_MAX_BANDS_TO_MONITOR_V01];

  /* last reported (if any) SCAN freq info */
  uint32 reported_filter_index;
  uint32 reported_scan_freq;
  cxm_tech_type reported_tech_id;
} coex_scan_freq_band_filter_s;

/* data structs and storage for throttling condition fail/success indications
 * sent to 1 per second */
#define COEX_THROTTLE_QMI_CONDITION_IND_PERIOD_MS 1000

#define COEX_WCN_PRIO_COND_FAIL_MASK_LEN     2
#define COEX_WCI2_PWRLMT_COND_FAIL_MASK_LEN  5
#define COEX_CONTR_PWRLMT_COND_FAIL_MASK_LEN 4
#define COEX_PWRLMT_COND_SUCCESS_MASK_LEN    2

typedef struct
{
  timetick_type last_sent_ts;
  uint16        drop_count;
} coex_throttle_count_s;

/* keep track of every bit in each of 4 condition masks */
typedef struct
{
  coex_throttle_count_s wcn_prio_fail[COEX_WCN_PRIO_COND_FAIL_MASK_LEN];
  coex_throttle_count_s wci2_pwrlmt_fail[COEX_WCI2_PWRLMT_COND_FAIL_MASK_LEN];
  coex_throttle_count_s contr_pwrlmt_fail[COEX_CONTR_PWRLMT_COND_FAIL_MASK_LEN];
  coex_throttle_count_s pwrlmt_success[COEX_PWRLMT_COND_SUCCESS_MASK_LEN];
} coex_throttle_condition_ind_s;

typedef struct
{
  boolean scan_enabled;    /* TRUE if WCN is doing paging */
  uint32  scan_interval;   /* WCN page scan interval */
  uint32  common_interval; /* calculated common interval between WCN and WWAN */
  uint64  last_msg_ts;     /* timestamp of last indication sent out */
} coex_wcn_wake_sync_s;

/* LTE Tx advanced received (for logging) */
typedef PACK(struct)
{
  uint8             sfn;             
  boolean           transmit;
  uint32            ustmr_time;      
  uint8             rbs;             
  int16             tx_power;        
} cxm_lte_tx_adv_s_pack;

/* Metric read values (for logging) */
typedef PACK(struct)
{
  cxm_tech_type     tech_id;
  uint32            metric_cnt;              
  uint32            metric_cnt_bt;           
  uint32            metric_cnt_wifi;         
  uint32            metric_cnt_bt_wifi;      
  uint32            metric_cnt_mdm_only;     
  int32             filt_metric;             
  int32             filt_metric_bt;          
  int32             filt_metric_wifi;        
  int32             filt_metric_bt_wifi;     
  int32             filt_metric_mdm_only;    
  errno_enum_type   status;                  
} cxm_tech_metrics_read_s_pack;

/* LTE BLER read value (for logging) */
typedef PACK(struct)
{
  uint32               tb_count;              
  uint32               tb_err_count;          
  uint32               tb_count_bt;           
  uint32               tb_err_count_bt;       
  uint32               tb_count_wifi;         
  uint32               tb_err_count_wifi;     
  uint32               tb_count_bt_wifi;      
  uint32               tb_err_count_bt_wifi;  
  uint32               tb_count_lte_only;     
  uint32               tb_err_count_lte_only; 
} cxm_lte_bler_read_s_pack;

/* WWAN high prio info (for logging) */
typedef PACK(struct)
{
  cxm_tech_type         tech_id;                         
  cxm_high_prio_oper_e  op_id;                           
  uint32                start;                           
  uint32                end;                             
  uint8                 num_freqs;                       
  uint32                freq[COEX_MAX_HIGH_PRIO_FREQ];   
} cxm_high_prio_s_pack;

/* struct containing all info (or pointers to info) related to global coex state
 * for quick [de]referencing when debugging */
typedef struct
{
  coex_state_type              *state;
  coex_power_limit_state_e     *cont_pwr_lmt_state;
  coex_power_limit_state_e     *wci2_pwr_lmt_state;
  atomic_word_t                *metrics_bler_started;
  coex_band_filter_s           *band_filter_info;  
  /* flag indicating that coex has been initialized. If this is 
   * ever removed, make sure this data structure is not optimized out */
  boolean                         coex_initialized;
  cxm_lte_tx_adv_s_pack         * log_lte_tx_adv;
  cxm_tech_metrics_read_s_pack  * log_tech_metrics_read;
  cxm_lte_bler_read_s_pack      * log_lte_bler_read;
  cxm_high_prio_s_pack          * log_high_prio_info;
  wci2_type7_type_s             * wci2_t7_info;
  uint8                         * recv_wci2_data;
} coex_state_info_type;

typedef struct
{
  coex_wwan_band_range_type_v01      wwan;
  coex_wcn_band_range_type_v01       wcn;

  coex_policy_config_mask_v01        mdm_policy;
  coex_wcn_policy_config_mask_v01    wcn_policy;

  coex_conflict_wcn_params_type_v01  wcn_params;
  coex_conflict_mdm_params_type_v01  mdm_params;
} coex_conflict_type;

typedef struct
{
  uint8 version;
  uint8 cxm_coex_protocol;
  uint8 num_conflicts;
  wci2_uart_baud_type_e baud;
  float link_path_loss_threshold;
  float filtered_rb_threshold;
  float rb_filter_alpha;             /* alpha value to apply to filtered rb threshold */
  coex_conflict_type victim_tbl[COEX_MAX_VICTIM_TBL_ENTRIES];  
  cxm_port_table_v01_s port_table;
  uint16 cxm_tx_active_adv_notice;   /* advanced notice timing in 100us */
  uint16 mdm_conn_ind_holdoff_time;  /* holdoff time for type 7[conn_state] timer (sec) */
  uint32 cxm_lte_sleep_notifi_thres; /* only send sleep notifications above this thresh */
  coex_late_tx_adv_toggle_type tx_adv_late_toggle;
} coex_config_params_v8;

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  coex_get_state_info_ptr

=============================================================================*/
/*!
    @brief
    Returns a pointer to the global coex_state_info struct for diag logging.

    @return
    coex_state_info_type *
*/
/*===========================================================================*/
coex_state_info_type* coex_get_state_info_ptr( void );

/*=============================================================================

  FUNCTION:  coex_get_wwan_state_info_ptr

=============================================================================*/
/*!
    @brief
    Returns a pointer to the global coex_wwan_state_info struct for diag
    logging.

    @return
    coex_wwan_tech_info *
*/
/*===========================================================================*/
coex_wwan_tech_info* coex_get_wwan_state_info_ptr( void );

/*=============================================================================

  FUNCTION:  coex_process_conflicts

=============================================================================*/
/*!
    @brief
    Searches through WWAN and WCN state data and determines if there is a
    conflict then carries out appropriate action

    @detail
    Searches through current state to determine if there is a conflict. The
    search is based on the tech passed in since it will only search for conflicts
    that may be associated with this tech. 

    @return
    none
*/
/*===========================================================================*/

void coex_process_conflicts (
  cxm_tech_type wwan_tech,
  cxm_wcn_tech_type wcn_tech
);

/*=============================================================================

  FUNCTION:  coex_resolve_conflict

=============================================================================*/
/*!
    @brief
    Given a WWAN band and WCN band determines if they are in conflict and
    carries out the appropriate action

    @detail
    For the given WWAN band and WCN band, the victim table is checked to
    see if they are in conflict. If a conflict is found, the appropriate
    action is carried out. There is a conflict if there is an intersection
    between both the WWAN and WCN bands with a pair in the victim table
 
    @return
    none
*/
/*===========================================================================*/

void coex_resolve_conflict (
  cxm_tech_type           wwan_tech,
  cxm_wcn_tech_type       wcn_tech,
  cxm_tech_link_info_s    *wwan_band,
  coex_band_type_v01      *wcn_band,
  uint32                  wcn_mode,
  coex_wwan_operating_dimension_v01  wwan_op_dim,
  uint64                  *conf_prio,
  uint8                   *conf_prio_id
);

/*=============================================================================

  FUNCTION:  set_coex_params

=============================================================================*/
/*!
    @brief
    Sets up paramters for coex.
	
    @detail
    Sets up the protocol used to determine algorithm behaviour and configures
    data plane ports
 
    @return
    none
*/
/*===========================================================================*/
void set_coex_params (
  void
);

/*=============================================================================

  FUNCTION:  coex_algos_init

=============================================================================*/
/*!
    @brief
    Initializes coex algorithm structures/states
	
    @detail
    Initializes coex algorithm and structures including GPIO or WCI-2 ports,
    policy information and boot parameters
 
    @return
    none
*/
/*===========================================================================*/
void coex_algos_init (
  void
);

/*=============================================================================

  FUNCTION:  coex_algos_deinit

=============================================================================*/
/*!
    @brief
    Deinitializes all COEX states as needed
 
    @return
    none
*/
/*===========================================================================*/
void coex_algos_deinit (
  void
);

/*=============================================================================

  FUNCTION:  coex_handle_indication_register_req

=============================================================================*/
/*!
    @brief
    Function to handle & respond to COEX_INDICATION_REGISTER_REQ message
 
    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_indication_register_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_get_wwan_state_req_v2

=============================================================================*/
/*!
    @brief
    Function to handle & respond to COEX_GET_WWAN_STATE_REQ message
 
    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_get_wwan_state_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_set_wlan_state_req

=============================================================================*/
/*!
    @brief
    This function handles the set wlan state req message from QMI.  If
    the message indicates valid info, the data is passed to a
    function to update the local coex wlan data with this new information.
    
 
    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_set_wlan_state_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_get_wlan_scan_state_req

=============================================================================*/
/*!
    @brief
    Given a scan id, the function locates the id in the coex wlan data (if
    it exists) and returns the data in that scan entry.
 
    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_get_wlan_hp_state_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_get_wlan_conn_state_req

=============================================================================*/
/*!
    @brief
    Given a conn handle, the function locates the handle in the coex wlan data
    (if it exists) and returns the data in that conn entry.
 
    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_get_wlan_conn_state_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_set_policy_req

=============================================================================*/
/*!
    @brief
    Function to handle client's request to set the current COEX policy to be
    followed. It processes the message and appropriately forwards the same to
    underlying layers.

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_set_policy_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_get_policy_req

=============================================================================*/
/*!
    @brief
    Function to handle client's query request of the current understanding
    of COEX policy followed by the service.

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_get_policy_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_lte_metric_bler_start_req

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to start collecting/
    collating LTE BLER metrics

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_lte_metric_bler_start_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_lte_metric_bler_stop_req

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to stop collecting LTE
    BLER metrics

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_lte_metric_bler_stop_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_lte_metric_sinr_start_req

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to start collecting/
    collating LTE SINR metrics

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_lte_metric_sinr_start_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_lte_metric_sinr_read_req

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to read current filter
    output for LTE SINR metrics

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_lte_metric_sinr_read_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_lte_metric_sinr_stop_req

=============================================================================*/
/*!
    @brief
    Function to handle teh client's query request to stop collecting/collating
    LTE SINR metrics

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_lte_metric_sinr_stop_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_set_band_filter_info_req

=============================================================================*/
/*!
    @brief
    Function to handle QMI req to set band filter info

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_set_band_filter_info_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_get_band_filter_info_req

=============================================================================*/
/*!
    @brief
    Function to handle QMI req to get band filter info

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_get_band_filter_info_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_get_wci2_mws_params_req

=============================================================================*/
/*!
    @brief
    Function to handle QMI req to query the type0 offset/jitter parameters

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_get_wci2_mws_params_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);


/*=============================================================================

  FUNCTION:  coex_handle_set_sleep_notification_thresh_req

=============================================================================*/
/*!
    @brief
    Function to handle QMI req to set sleep notification threshold for a tech

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_set_sleep_notification_thresh_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_get_sleep_notification_thresh_req

=============================================================================*/
/*!
    @brief
    Function to handle QMI req to get sleep notification threshold for a tech

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_get_sleep_notification_thresh_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_wcn_wake_sync_req

=============================================================================*/
/*!
    @brief
    Function to handle QMI req to sync BT and LTE page scans

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_wcn_wake_sync_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_get_conflict_params_req

=============================================================================*/
/*!
    @brief
    Function to handle QMI req to get the conflict table

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_get_conflict_params_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_tech_metric_start_req

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to start collecting/
    collating metrics for a tech

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_tech_metric_start_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_tech_metric_read_req

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to read current filter
    output for metrics

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_tech_metric_read_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_tech_metric_stop_req

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to stop collecting/collating
    metrics

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_tech_metric_stop_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_set_scan_freq_band_filter_info_req

=============================================================================*/
/*!
    @brief
    Function to handle QMI req to set SCAN freq band filter info

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_set_scan_freq_band_filter_info_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_handle_get_scan_freq_band_filter_info_req

=============================================================================*/
/*!
    @brief
    Function to handle QMI req to get SCAN freq band filter info

    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
qmi_csi_cb_error coex_handle_get_scan_freq_band_filter_info_req (
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void          *req_c_struct,
  unsigned int   req_c_struct_len,
  void          *service_cookie
);

/*=============================================================================

  FUNCTION:  coex_pwr_lmt_wdog_timer_handler

=============================================================================*/
/*!
    @brief
    Timer handler, called when wci2-based power limiting wdog timer expires.
    Called from cxm_task when timer signals task.

    @return void
*/
/*===========================================================================*/
void coex_pwr_lmt_wdog_timer_handler (
  void
);

/*===========================================================================

  FUNCTION:  cxm_req_boot_params

===========================================================================*/
/*!
  @brief
    Resend the boot params message. CFW will use this to ask for the boot
    params message. A function call is used here since CFW does not yet
    have a msgr message Q.

  @return
    none
*/
/*=========================================================================*/
void cxm_req_boot_params (
  void
);

/*=============================================================================

  FUNCTION:  coex_metrics_lte_sinr_initiate_req

=============================================================================*/
/*!
    @brief
    Start/stop/read SINR. Fill out params based on requested action.

    @return
    void
*/
/*===========================================================================*/
errno_enum_type coex_tech_metrics_initiate_req(
  coex_wwan_tech_v01 tech,     /* LTE, GSM1/2/3, TDS */
  cxm_action_e       action,   /* START/STOP/READ */
  uint32             alpha_q8, /* only for START */
  uint32             msg_id    /* only for READ */
);

/*=============================================================================

  FUNCTION:  coex_send_diag_policy

=============================================================================*/
/*!
    @brief
    Function to take policy from diag interface and send to LTE to
    simulate coex conditions.  Only policy is taken from diag...all supporting
    parameters stay at existing or default values.

    @detail

    @return
    none
*/
/*===========================================================================*/
void coex_send_diag_policy (
  uint64 policy
);

/*=============================================================================

  FUNCTION:  coex_wci2_type7_timer_handler

=============================================================================*/
/*!
    @brief
    Handle timer expiration for WCI-2 type 7 filtering. 

    @detail
    Trying to filter
    out extraneous fast transitions, so if timer expires, enough time has
    elapsed to say that this is not an "extraneous fast transition". Thus,
    send the type 7.

    @return
    void
*/
/*===========================================================================*/
void coex_wci2_type7_timer_handler(
  uint32 event_mask
);

/*=============================================================================

  FUNCTION:  coex_set_wci2_type7_filter_time

=============================================================================*/
/*!
    @brief
    Set timeout for type 7 filter timer

    @return
    void
*/
/*===========================================================================*/
void coex_set_wci2_type7_filter_time(
  uint8  event,
  uint32 time_ms
);

/*=============================================================================

  FUNCTION:  coex_algos_process_wci2_rx

=============================================================================*/
/*!
    @brief
    Process recieved WCI2 bytes

    @return
    void
*/
/*===========================================================================*/
void coex_algos_process_wci2_rx( void );

#endif /* __COEX_ALGOS_H__ */
