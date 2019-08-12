/*!
  @file
  coex_algos.c

  @brief
  This file contains all the message handlers as well CoEx algos (if any).


  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
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

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/coex_algos.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/10/16   tak     F3 reduction - removed QMI sleep/wakeup
01/20/15   ckk     Update type7[rrc_c] using RAT layer 3 states
01/14/15   ckk     Add support for type7[ant_tx]
12/02/14   ckk     Implement wci2 tx pwr limiting using type6[src]&[pl_req]
08/07/14   ckk     Handle wci2 type7 only if policy is enabled
07/28/14   tak     TDS/GSM Coex over SMEM
07/17/14   btl     Use updated WCI2 client interface
07/14/14   ckk     Added WCI2 type4 & type7[pwr_bit] support
07/10/14   btl     Update default GRFCs to BO/TH hw
05/28/14   btl     Featurize WCI2 subsystem when UART not present
05/16/14   tak     Basic WCI-2 protocol
05/05/14   ckk     Handling GSM2 & GSM3 RAT state updates over QMI
05/02/14   btl     Replace wakeup timer with a SLPC callback
04/25/14   btl     Type 6 filtering for WCI2 control + t6 reorg
03/25/14   btl     Type 7 filtering to eliminate short glitches
03/18/14   btl     Add LTE SINR support for QMB
02/26/14   btl     Add sending type 7 (WWAN TX ON/OFF)
02/19/14   ckk     Filter out too frequent LTE frame offset updates 
02/12/14   btl     Use SLPC to determine when WWAN techs do sleep/wakeup
01/20/14   btl     Filter out CXM WWAN State Info msgs with special link types
10/23/13   tak     Added TX advanced GRFC control, off state, center frequency
                   only conflict handling
07/18/13   tak     Removed dependency on NV file for DI 2.1 PL
07/15/13   tak     Added LTE subframe marker port and npa vote for bler/sinr
06/11/13   btl     Use cxm_task signal for timers instead of APC
06/10/13   btl     Throttle QMI succ/fail INDs to 1x per second each
05/28/13   tak     Support for 3-wire on 8974, power limiting via NV
05/22/13   ckk     React to LTE RRC state to update current power limiting state
03/01/13   tak     Remove dependency on LL1 messages and structures
02/13/13   tak     Added WWAN update and sleep indication support for all techs
02/04/13   btl     Add WCN<->WWAN page scan sync (featurized)
02/04/13   tak     Added control of GPIOs
01/10/13   tak     Added CA and TDS support for WWAN update and sleep indication 
12/19/12   btl     Port power limiting, band filtering, set APT table, WCN Prio
                   cond fail and consecutive subframe params, and trace logging
10/02/12   btl     Add metrics
09/06/12   cab     Removed hardcoded boot params, back to reading from nv
08/14/12   btl     Picked up missed coex msgr handlers
07/23/12   btl     Prevent sending duplicate WWAN state IND
07/19/12   cab     Updated for new LTE MSGR interface
07/18/12   cab     Updated NV versioning and protocols
07/13/12   cab     Update FW protocol versioning
07/13/12   cab     Fixes for messaging, loop index warnings, loop conditional
07/10/12   cab     Fix head pointer name in conn update
07/10/12   cab     Updated messaging, fix deletion from head of linked list
07/05/12   ckk     Correcly identify TDD frame structure
07/05/12   ejv     Add support for mqcsi layer.
04/23/12   ckk     Add support for WWAN state info REQ/RESP message
04/17/12   ckk     Move to new coex IDL
04/08/12   ckk     Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "coex_algos.h"
#include "mqcsi_conn_mgr.h"
#include <qmi_csi.h>
#include "cxm_msgr.h"
#include "cxm_utils.h"
#include <coexistence_service_v01.h>
#include "cxm_trace.h"
#include <lte_l1_types.h> /* to access LTE_L1_FRAME_STRUCTURE_FS2 */
#include <wcn_coex_mgr.h>
#include "cxm_wcn_tx.h"
#include "float.h"
#include "custmcs.h"
#include <coexistence_service_v02.h>
#include <slpc.h> /* to access slpc wakeup cb api */
#include "cxm_smem.h"
#include "cxm_fw_msg.h"
#include "cxm_intf_types.h"
#include <wci2_uart.h>
#include <wci2_core.h>
#include "coex_algos_v2.h"

#ifdef FEATURE_COEX_USE_NV
#include "coex_nv.h"
#endif

#ifdef FEATURE_LTE
#include "rflte_msg.h"
#endif /*FEATURE_LTE*/

#include <DDITimetick.h>
#include <DDIChipInfo.h>

#include "modem_fw_memmap.h" /*FW SHMEM*/
#include "fws.h"
#include "modem_fw_ipc.h"

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
/* high prio state values from QMI messaging */
#define COEX_WLAN_HIGH_PRIO_STOP  0x00
#define COEX_WLAN_HIGH_PRIO_START 0x01

/* connection state values from QMI messaging */
#define COEX_WLAN_CONN_DISABLED 0x00
#define COEX_WLAN_CONN_SETUP    0x01
#define COEX_WLAN_CONN_STEADY   0x02

/* connection mode values from QMI messaging */
#define COEX_WLAN_CONN_MODE_NONE             0x00
#define COEX_WLAN_CONN_MODE_STATION          0x01
#define COEX_WLAN_CONN_MODE_SOFTAP           0x02
#define COEX_WLAN_CONN_MODE_P2P_GROUP_OWNER  0x03
#define COEX_WLAN_CONN_MODE_P2P_CLIENT       0x04
#define COEX_WLAN_CONN_MODE_AMP              0x05

/* Max LTE Tx subframes */
#define COEX_MAX_LTE_TX_SUBF     10

/* recurring timer duration when sending boot params req to lte */
#define COEX_BOOT_REQ_RETRY_TIME_MS  1000

/* 1.0 in Q8 format = 1 << 8 */
#define COEX_LTE_METRICS_MAX_ALPHA 0x100

/* float neg infinity */
#define COEX_INVALID_SINR_VALUE 0xff800000

#define COEX_UART_JITTER_115200 87
#define COEX_UART_JITTER_2000000 5
#define COEX_UART_JITTER_3000000 4

#define MHZ_HZ_CONVERSION 1000000
#define MHZ_KHZ_CONVERSION 1000
#define KHZ_HZ_CONVERSION 1000

#define COEX_TIMETICKS_PER_MSEC 19200

/* policy max, min, default values */
#define COEX_POLICY_POWER_THRESH_MIN                        -70
#define COEX_POLICY_POWER_THRESH_MAX                         30
#define COEX_POLICY_POWER_THRESH_DEF                        -128
#define COEX_POLICY_RB_THRESH_MAX                            100
#define COEX_POLICY_CONT_SF_DENIALS_THRESH_MIN               2
#define COEX_POLICY_CONT_SF_DENIALS_THRESH_DEF               5
#define COEX_POLICY_CONT_SF_DENIALS_THRESH_MAX               8
#define COEX_POLICY_MAX_SF_DENIALS_MAX                       30
#define COEX_POLICY_MAX_SF_DENIALS_DEF                       10
#define COEX_POLICY_SF_DENIAL_WINDOW_MAX                     2000
#define COEX_POLICY_SF_DENIAL_WINDOW_DEF                     10
#define COEX_POLICY_TX_POWER_LIMIT_MIN                       16
#define COEX_POLICY_TX_POWER_LIMIT_MAX                       30
#define COEX_DEFAULT_LTE_POWER_LIMIT                         127
#define COEX_POLICY_LINK_PATH_LOSS_THRESH_MIN                0
#define COEX_POLICY_LINK_PATH_LOSS_THRESH_DEF                100
#define COEX_POLICY_LINK_PATH_LOSS_THRESH_MAX                100
#define COEX_POLICY_RB_FILTER_ALPHA_MIN                      0.0
#define COEX_POLICY_RB_FILTER_ALPHA_MAX                      1.0
#define COEX_POLICY_FILTERED_RB_THRESH_MIN                   0
#define COEX_POLICY_FILTERED_RB_THRESH_MAX                   100
#define COEX_POLICY_CONTR_PWRLMT_TIMEOUT_DEF                 150
#define COEX_POLICY_WCI2_PWRLMT_TIMEOUT_DEF                  150
#define COEX_POLICY_POWER_THRESH_FOR_TX_ADV_NTC_MIN_DB10    -700
#define COEX_POLICY_POWER_THRESH_FOR_TX_ADV_NTC_MAX_DB10     300
#define COEX_POLICY_POWER_THRESH_FOR_TX_ADV_NTC_DEF_DB10    -1280
#define COEX_POLICY_RB_THRESH_FOR_TX_ADV_NTC_MIN             0
#define COEX_POLICY_RB_THRESH_FOR_TX_ADV_NTC_MAX             100
#define COEX_POLICY_HOLDOFF_TIMER_RAT_CONN_STATE_MIN         0
#define COEX_POLICY_HOLDOFF_TIMER_RAT_CONN_STATE_DEF         90000
#define COEX_POLICY_HOLDOFF_TIMER_RAT_CONN_STATE_MAX         180000
#define COEX_POLICY_FILTER_ALPHA_RAT_PWR_STATE_MIN           0.0
#define COEX_POLICY_FILTER_ALPHA_RAT_PWR_STATE_MAX           1.0
#define COEX_POLICY_TX_PWR_THRESH_RAT_PWR_STATE_MIN          0
#define COEX_POLICY_TX_PWR_THRESH_RAT_PWR_STATE_DEF          1
#define COEX_POLICY_TX_PWR_THRESH_RAT_PWR_STATE_MAX          500
#define COEX_POLICY_HOLDOFF_TIMER_RAT_PWR_STATE_MIN          0
#define COEX_POLICY_HOLDOFF_TIMER_RAT_PWR_STATE_DEF          90000
#define COEX_POLICY_HOLDOFF_TIMER_RAT_PWR_STATE_MAX          180000

/* Defined to handle CUST_1's request for handle 90 sec delay in reporting
    not_connected and no_tx_pwr for WCI2 Type7 message byte */
#define COEX_SPL_PLCY_HLDFF_TMR_RAT_CONN_ST_DEF              90000
#define COEX_SPL_PLCY_HLDFF_TMR_RAT_PWR_ST_DEF               90000


#define COEX_WIRE_POLICIES ( CXM_POLICY_TOGGLE_FRAME_SYNC | \
                             CXM_POLICY_TOGGLE_TX_ACTIVE | \
                             CXM_POLICY_TOGGLE_RX_PRIORITY | \
                             CXM_POLICY_REACT_TO_WCN_PRIORITY )

#define COEX_WCI2_TYPE7_POLICIES ( CXM_POLICY_WCI2_SEND_TYPE7_MDM_CONN_STATE | \
                                   CXM_POLICY_WCI2_SEND_TYPE7_MDM_PWR_STATE )

/* defines for page scan wakeup sync between WCN and WWAN */
#define COEX_MAX_WWAN_WAKE_INTERVALS     4
#define COEX_MAX_WCN_WAKE_INTERVALS      4
#define COEX_WCN_WAKE_MIN_TIME_THRESH    20
#define COEX_WCN_WAKE_MAX_TIME_THRESH    60000

/*parameters for asserting tx active in advance*/
#define COEX_GRFC_ASSERT_PERIOD_USTMR    19200  /*1 ms*/
#define COEX_MAX_ADVANCED_TX_NOTIFI      40320  /*2.1 ms*/
#define COEX_MIN_ADVANCED_TX_NOTIFI      23040  /*1.2 ms*/
#define COEX_ADV_TIMING_TOLERANCE        1920   /*100 us*/

/* defines for WWAN TECH LTE */
#define COEX_TECH_LTE_FRAME_OFFSET_DIFF_THRESH  2 /* delta threshold frame offset
                                                       from last sent to trigger update */

#define COEX_BASIC_WCI2_MASK ~( COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN_V01 | \
                                COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE_V01 | \
                                COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT_V01 | \
                                COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 )

#define COEX_WCI2_T6_PL_MASK             0x03
#define COEX_WCI2_T6_PL_REQ_OFF          0x00
#define COEX_WCI2_T6_PL_REQ_ON           0x01
#define COEX_WCI2_T6_PL_SRC_BT           0x00
#define COEX_WCI2_T6_PL_SRC_WIFI         0x01
#define COEX_WCI2_T6_PL_REQ_BIT_SHIFT    0x01
#define COEX_WCI2_T6_PL_REQ_OFF_SRC_BT   ( (COEX_WCI2_T6_PL_REQ_OFF << \
                                            COEX_WCI2_T6_PL_REQ_BIT_SHIFT) | \
                                            COEX_WCI2_T6_PL_SRC_BT )
#define COEX_WCI2_T6_PL_REQ_ON_SRC_BT    ( (COEX_WCI2_T6_PL_REQ_ON << \
                                            COEX_WCI2_T6_PL_REQ_BIT_SHIFT) | \
                                            COEX_WCI2_T6_PL_SRC_BT )
#define COEX_WCI2_T6_PL_REQ_OFF_SRC_WIFI ( (COEX_WCI2_T6_PL_REQ_OFF << \
                                            COEX_WCI2_T6_PL_REQ_BIT_SHIFT) | \
                                            COEX_WCI2_T6_PL_SRC_WIFI )
#define COEX_WCI2_T6_PL_REQ_ON_SRC_WIFI  ( (COEX_WCI2_T6_PL_REQ_ON << \
                                            COEX_WCI2_T6_PL_REQ_BIT_SHIFT) | \
                                            COEX_WCI2_T6_PL_SRC_WIFI )

/* UART WCI2 receive */
void coex_algos_wci2_dir_read_cb( uint8* rx_types, uint8 status );
typedef struct
{
  uint8         bytes[8];
  atomic_word_t status;
} coex_wci2_rx_s;

STATIC coex_wci2_rx_s coex_wci2_rx;

/* max of two values */
#define COEX_MAX( a, b ) ( a > b ? a : b )
/* min of two values */
#define COEX_MIN( a, b ) ( a > b ? b : a )

/* assign src to dest within bounds min to max */
#define COEX_BOUND_AND_SET( valid, dest, src, min, max ) \
  if ( valid ) \
  { \
    if ( src < min ) \
    { \
      dest = min; \
    } \
    else if ( src > max ) \
    { \
      dest = max; \
    } \
    else \
    { \
      dest = src; \
    } \
  }

/*-table containing state and sleep information for all techs
  -the tech_id enum is used to index into this data structure at 
   runtime*/
coex_wwan_tech_info coex_wwan_state_info[CXM_TECH_MAX];

/* policy parameters */
coex_plcy_parms_s coex_plcy_parms;

/* linked list for active wlan connections */
STATIC coex_wlan_conn_node_type *coex_wlan_conn_head = NULL;

/* linked list for active wlan high prio */
STATIC coex_wlan_hp_node_type *coex_wlan_hp_head = NULL;

/* flag indicating that coex bler metrics have been started */
atomic_word_t coex_metrics_lte_bler_started;

STATIC coex_power_limit_state_e coex_controller_pwr_lmt_state = COEX_PWR_LMT_OFF;
STATIC coex_power_limit_state_e coex_wci2_pwr_lmt_state = COEX_PWR_LMT_OFF;
STATIC uint8 coex_wci2_pwr_lmt_src_req_status = 0x00;
STATIC boolean coex_check_power_limit_in_progress = FALSE;

/* timer when wci2 power limiting wdog timer expires */
STATIC timer_type coex_pwr_lmt_wdog_timer;

/* state used for tracking when uart can be powered off */
STATIC coex_state_type coex_state = 
       {
         0,                    // active_tech_mask
	   		 0,                    // conn_tech_mask
	   		 0,                    // tx_pwr_tech_mask
	   		 FALSE,                // uart_en_state
	   		 0,                    // uart_handle
	   		 CXM_TX_ANT_SEL_UNUSED // tx_ant_sel
	   	};

/* storage for band filtering info */
STATIC coex_band_filter_s coex_band_filter_info;
STATIC coex_scan_freq_band_filter_s coex_scan_freq_filter_info;

/*port mapping for wci-2*/
STATIC cxm_port_purpose_mapping_table_v01_s coex_wci2_port_mapping;

/* port mapping for SMEM data */
STATIC cxm_port_purpose_mapping_table_v01_s coex_smem_port_mapping;

/* port mapping for SMEM_V2 data */
STATIC cxm_port_purpose_mapping_table_v01_s coex_smem_v2_port_mapping;

STATIC coex_throttle_condition_ind_s coex_throttle_ind_info;

/* positions in these two index arrays are used as indices into the
 * common interval lookup table */
STATIC const uint16 coex_wwan_wake_intervals[COEX_MAX_WWAN_WAKE_INTERVALS] = {
  320, 640, 1280, 2560
};

STATIC const uint16 coex_wcn_wake_intervals[COEX_MAX_WCN_WAKE_INTERVALS] = {
  640,  1280, 1920, 2560
};

/* WWAN and WCN page intervals and the corresponding common interval
 * to adhere to (in msec) */
STATIC const uint16 
  coex_wcn_wake_interval_LUT[COEX_MAX_WWAN_WAKE_INTERVALS][COEX_MAX_WCN_WAKE_INTERVALS] = 
{
  /* WCN intvls 640,  1280, 1920, 2560 */
  /* ------------------------------------| WWAN intvls */
              { 640,  1280, 1920, 2560 }, /* 320  */
              { 640,  1280, 1920, 2560 }, /* 640  */
              { 1280, 1280, 3840, 2560 }, /* 1280 */
              { 2560, 2560, 7680, 2560 }  /* 2560 */
};

STATIC coex_wcn_wake_sync_s coex_wcn_wake_sync_info;

STATIC DalDeviceHandle *coex_DAL_handle = NULL;

/* structures used exclusively for logging */
STATIC cxm_lte_tx_adv_s_pack         cxm_tx_adv_log;
STATIC cxm_tech_metrics_read_s_pack  cxm_tech_metrics_log;
STATIC cxm_lte_bler_read_s_pack      cxm_lte_bler_log;
STATIC cxm_high_prio_s_pack          cxm_high_prio_log;
STATIC uint8                         cxm_recved_wci2_data_log;

STATIC wci2_type7_type_s coex_wci2_t7_state;

STATIC coex_state_info_type coex_state_info = 
{
  &coex_state,
  &coex_controller_pwr_lmt_state,
  &coex_wci2_pwr_lmt_state,
  &coex_metrics_lte_bler_started,
  &coex_band_filter_info,
  FALSE,
  &cxm_tx_adv_log,
  &cxm_tech_metrics_log,
  &cxm_lte_bler_log,
  &cxm_high_prio_log,
  &coex_wci2_t7_state,
  &cxm_recved_wci2_data_log
};

/*scheduled deassert time in ustmr for cxm feature to enable tx in advance*/
STATIC boolean coex_deassert_scheduled = FALSE;
STATIC uint32 coex_tx_deassert_time;

/*advnaced notification period in ustmr for cxm feature to enable tx in advance*/
STATIC uint32 coex_adv_notifi_period;

STATIC boolean coex_state_txadv_late = FALSE;

/*this structure is versioned; currently at version 8*/
coex_config_params_v8  coex_params;

/* COEX PROTOCOL -> COEX SYS BEHAVIOR MASK translation */
#define COEX_PROTOCOL_TRANSL_1 0x00 /* channel avoidance only */
#define COEX_PROTOCOL_TRANSL_3 ( CXM_SYS_BHVR_WCI2_DATA            | \
                                 CXM_SYS_BHVR_QMI_POLICY_CONTROL   | \
                                 CXM_SYS_BHVR_CXM_SENDS_POLICY     | \
                                 CXM_SYS_BHVR_CXM_SENDS_TYPE6      | \
                                 CXM_SYS_BHVR_QMI_METRICS)
#define COEX_PROTOCOL_TRANSL_4 ( CXM_SYS_BHVR_WCI2_DATA            | \
                                 CXM_SYS_BHVR_VICTIM_TABLE         | \
                                 CXM_SYS_BHVR_DISREGARD_RRC_PROC   | \
                                 CXM_SYS_BHVR_CXM_SENDS_POLICY     | \
                                 CXM_SYS_BHVR_CXM_ASSERTS_TX_ADV   | \
                                 CXM_SYS_BHVR_OFF_STATE_ENABLED    | \
                                 CXM_SYS_BHVR_TX_ADV_ALL_SUBFRAMES | \
                                 CXM_SYS_BHVR_CNTR_FRQ_CONFLICT)
/* QMB Control Plane over WCI2 */
#define COEX_PROTOCOL_TRANSL_5 ( CXM_SYS_BHVR_WCI2_DATA            | \
                                 CXM_SYS_BHVR_VICTIM_TABLE         | \
                                 CXM_SYS_BHVR_CXM_SENDS_POLICY     | \
                                 CXM_SYS_BHVR_WCI2_CONTROL         | \
                                 CXM_SYS_BHVR_TX_ADV_ALL_SUBFRAMES | \
                                 CXM_SYS_BHVR_CXM_SENDS_TYPE6)
/* QMB Control Plane over WCI2 + Antenna Sharing */
#define COEX_PROTOCOL_TRANSL_6 ( CXM_SYS_BHVR_WCI2_DATA            | \
                                 CXM_SYS_BHVR_VICTIM_TABLE         | \
                                 CXM_SYS_BHVR_CXM_SENDS_POLICY     | \
                                 CXM_SYS_BHVR_WCI2_CONTROL         | \
                                 CXM_SYS_BHVR_CXM_SENDS_TYPE6      | \
                                 CXM_SYS_BHVR_TX_ADV_ALL_SUBFRAMES | \
                                 CXM_SYS_BHVR_WLAN_ANT_SHARE)
/* Data plane over SMEM */
#define COEX_PROTOCOL_TRANSL_8 ( CXM_SYS_BHVR_SMEM_DATA            | \
                                 CXM_SYS_BHVR_VICTIM_TABLE         | \
                                 CXM_SYS_BHVR_CXM_SENDS_POLICY     | \
                                 CXM_SYS_BHVR_TX_ADV_ALL_SUBFRAMES | \
                                 CXM_SYS_BHVR_QMI_METRICS)
/* MCS asserts TX Adv + victim table, no type 6 */
#define COEX_PROTOCOL_TRANSL_9 ( CXM_SYS_BHVR_WCI2_DATA            | \
                                 CXM_SYS_BHVR_VICTIM_TABLE         | \
                                 CXM_SYS_BHVR_CXM_SENDS_POLICY     | \
                                 CXM_SYS_BHVR_DISREGARD_RRC_PROC   | \
                                 CXM_SYS_BHVR_CXM_ASSERTS_TX_ADV)
#define COEX_PROTOCOL_TRANSL_10 ( CXM_SYS_BHVR_WCI2_DATA           | \
                                  CXM_SYS_BHVR_QMI_POLICY_CONTROL  | \
                                  CXM_SYS_BHVR_CXM_SENDS_POLICY    | \
                                  CXM_SYS_BHVR_BASIC_WCI2)
/* Data plane over SMEM  with SAWless RF card */
#define COEX_PROTOCOL_TRANSL_12 ( CXM_SYS_BHVR_SMEM_DATA            | \
                                  CXM_SYS_BHVR_VICTIM_TABLE         | \
                                  CXM_SYS_BHVR_CXM_SENDS_POLICY     | \
                                  CXM_SYS_BHVR_TX_ADV_ALL_SUBFRAMES | \
                                  CXM_SYS_BHVR_QMI_METRICS          | \
                                  CXM_SYS_BHVR_GSM_TDS_HIGH_PRIO_MSG)
/* Data plane over SMEM  with Suboptimal Filter RF card */
#define COEX_PROTOCOL_TRANSL_13 ( CXM_SYS_BHVR_SMEM_DATA            | \
                                  CXM_SYS_BHVR_VICTIM_TABLE         | \
                                  CXM_SYS_BHVR_CXM_SENDS_POLICY     | \
                                  CXM_SYS_BHVR_TX_ADV_ALL_SUBFRAMES | \
                                  CXM_SYS_BHVR_QMI_METRICS          | \
                                  CXM_SYS_BHVR_LTE_HIGH_PRIO_MSG)
/* Data plane over SMEM  with SAWless and Suboptimal Filter RF card */
#define COEX_PROTOCOL_TRANSL_14 ( CXM_SYS_BHVR_SMEM_DATA            | \
                                  CXM_SYS_BHVR_VICTIM_TABLE         | \
                                  CXM_SYS_BHVR_CXM_SENDS_POLICY     | \
                                  CXM_SYS_BHVR_TX_ADV_ALL_SUBFRAMES | \
                                  CXM_SYS_BHVR_QMI_METRICS          | \
                                  CXM_SYS_BHVR_LTE_HIGH_PRIO_MSG    | \
                                  CXM_SYS_BHVR_GSM_TDS_HIGH_PRIO_MSG)

/*behavior of the system*/
cxm_sys_bhvr_t coex_sys_behavior_mask;

/*Shared memory with FW. Used for FW counters and information on TX advance grfc activity*/
cxm_fw_smem_s *cxm_fw_smem;

/* callbacks for sleep/wake */
void coex_slpc_notify_cb( slpc_id_type id, uint64 wakeup_tick, 
                          boolean update, boolean extension );
void coex_slpc_notify_wakeup_cb( slpc_id_type id, uint64 wakeup_tick);

void coex_send_diag_policy ( uint64 policy );

/*=============================================================================

                         INTERNAL FUNCTIONS

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
coex_state_info_type* coex_get_state_info_ptr( void )
{
  return &coex_state_info;
}

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
coex_wwan_tech_info* coex_get_wwan_state_info_ptr( void )
{
  return coex_wwan_state_info;
}

/*=============================================================================

  FUNCTION:  coex_slpc_to_cxm_id_transl

=============================================================================*/
/*!
    @brief
    convert slpc_id to cxm_tech_id

    @return
    cxm_tech_type
*/
/*===========================================================================*/
inline cxm_tech_type coex_slpc_to_cxm_id_transl( slpc_id_type slpc_id )
{
  cxm_tech_type cxm_id;
  /*-----------------------------------------------------------------------*/
  switch( slpc_id )
  {
    case SLPC_GSM:
      cxm_id = CXM_TECH_GSM1;
      break;

    case SLPC_1X:
      cxm_id = CXM_TECH_ONEX;
      break;

    case SLPC_HDR:
      cxm_id = CXM_TECH_HDR;
      break;

    case SLPC_WCDMA:
    case SLPC_WCDMA2:
      cxm_id = CXM_TECH_WCDMA;
      break;

    case SLPC_LTE:
      cxm_id = CXM_TECH_LTE;
      break;

    case SLPC_TDSCDMA:
      cxm_id = CXM_TECH_TDSCDMA;
      break;

    case SLPC_GSM2:
      cxm_id = CXM_TECH_GSM2;
      break;

    case SLPC_GSM3:
      cxm_id = CXM_TECH_GSM3;
      break;

    default:
      cxm_id = CXM_TECH_DFLT_INVLD;
      break;
  }

  return cxm_id;
}

/*=============================================================================

  FUNCTION:  coex_fatal_err_cb

=============================================================================*/
/*!
    @brief
    Gets called when err fatal occurs, to log fatal in cxm trace

    @detail

    @return
    none
*/
/*===========================================================================*/
void coex_fatal_err_cb( void )
{
  /*-----------------------------------------------------------------------*/
  cxm_trace_event( CXM_TRC_ERR_FATAL, 0, 0, 0, CXM_TRC_EN );

  return;
}

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
)
{
  /*-----------------------------------------------------------------------*/
  memset( &coex_params, 0, sizeof( coex_config_params_v8 ) );

  coex_params.version = CXM_CONFIG_VERSION_8;
  coex_params.baud = WCI2_UART_BAUD_3000000;
#ifdef FEATURE_COEX_USE_NV
  coex_params.cxm_coex_protocol = CXM_COEX_PROTOCOL_1; 
#else
  coex_params.cxm_coex_protocol = CXM_COEX_PROTOCOL_3;
#endif

  coex_params.port_table.num_ports = 5;
  coex_params.port_table.port_info[0].id = CXM_PORT_A;
  coex_params.port_table.port_info[0].type = CXM_PORT_TYPE_GRFC_BASED;
  coex_params.port_table.port_info[0].addr = 21;
  coex_params.port_table.port_info[0].assert_offset_us = -60;
  coex_params.port_table.port_info[0].deassert_offset_us = +10;
  coex_params.port_table.port_info[1].id = CXM_PORT_B;
  coex_params.port_table.port_info[1].type = CXM_PORT_TYPE_GRFC_BASED;
  coex_params.port_table.port_info[1].addr = 22;
  coex_params.port_table.port_info[1].assert_offset_us = -40;
  coex_params.port_table.port_info[1].deassert_offset_us = +40;
  coex_params.port_table.port_info[2].id = CXM_PORT_C;
  coex_params.port_table.port_info[2].type = CXM_PORT_TYPE_GRFC_BASED;
  coex_params.port_table.port_info[2].addr = 17;
  coex_params.port_table.port_info[2].assert_offset_us = 0;
  coex_params.port_table.port_info[2].deassert_offset_us = 0;
  coex_params.port_table.port_info[3].id = CXM_PORT_D;
  coex_params.port_table.port_info[3].type = CXM_PORT_TYPE_L2VIC_BASED;
#ifdef FEATURE_MCS_THOR
  coex_params.port_table.port_info[3].addr = 143;
#else
  coex_params.port_table.port_info[3].addr = 240;
#endif
  coex_params.port_table.port_info[3].assert_offset_us = 0;
  coex_params.port_table.port_info[3].deassert_offset_us = 0;
  coex_params.port_table.port_info[4].id = CXM_PORT_E;
  coex_params.port_table.port_info[4].type = CXM_PORT_TYPE_GRFC_BASED;
  coex_params.port_table.port_info[4].addr = 20;
  coex_params.port_table.port_info[4].assert_offset_us = 0;
  coex_params.port_table.port_info[4].deassert_offset_us = 0;

  return;
}

/*=============================================================================

  FUNCTION:  coex_algos_init

=============================================================================*/
/*!
    @brief
    Initializes coex algorithm structures/states
	
    @detail
    Initializes coex algorithm and structures including ports,
    policy information and boot parameters
 
    @return
    none
*/
/*===========================================================================*/
void coex_algos_init (
  void
)
{
  DALResult DAL_retval = DAL_SUCCESS;
  wci2_client_open_s uart_params;
  wci2_error_e       uart_retval;
  /* boot message received by CFW and tech L1s*/
  cxm_config_params_ind_s        cxm_config_ind;
#ifdef FEATURE_COEX_USE_NV
  int32 temp_Q8_conv;
  uint8 i = 0;
#endif
  /*-----------------------------------------------------------------------*/
#ifdef FEATURE_COEX_USE_NV
  /* initialize and/or retrieve coex NV data (victim table) */
  coex_nv_init(&coex_params);  
#else
  set_coex_params();
#endif

  /* ensure size of victim table is within bounds of allocated memory */
  CXM_ASSERT( coex_params.num_conflicts <= COEX_MAX_VICTIM_TBL_ENTRIES );

  /*Protocol to system behavior translation. This determines what 
  functionality is enabled in coex. */
  switch(coex_params.cxm_coex_protocol)
  {
    case CXM_COEX_PROTOCOL_3:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_3;
      break;
    case CXM_COEX_PROTOCOL_4:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_4;
      break;
    case CXM_COEX_PROTOCOL_5:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_5;
      break;
    case CXM_COEX_PROTOCOL_6:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_6;
      break;
    case CXM_COEX_PROTOCOL_8:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_8;
      break;
    case CXM_COEX_PROTOCOL_9:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_9;
      break;
    case CXM_COEX_PROTOCOL_10:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_10;
      break;
    case CXM_COEX_PROTOCOL_12:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_12;
      break;
    case CXM_COEX_PROTOCOL_13:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_13;
      break;
    case CXM_COEX_PROTOCOL_14:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_14;
      break;
    case CXM_COEX_PROTOCOL_1:
    default:
      coex_sys_behavior_mask = COEX_PROTOCOL_TRANSL_1;
      break;
  }

  CXM_MSG_1( MED, "Protocol received: ", coex_params.cxm_coex_protocol );

  /*Initialize shared memory between CXM and FW*/
  cxm_fw_smem = (cxm_fw_smem_s*) FW_SMEM_COEX_ADDR; 
  if( 0 != FW_SMEM_COEX_ADDR )
  {
    memset( cxm_fw_smem, 0, sizeof( cxm_fw_smem_s ) );
  }
  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_CXM_ASSERTS_TX_ADV) )
  {
    /*for WCI-2, CXM_PORT_C is for TX Active*/
    if( 0 != cxm_fw_smem )
    {
      cxm_fw_smem->grfc_task.grfc_num = coex_params.port_table.port_info[2].addr;
    }
    /*read the advanced notice for asserting TX active in 100microseconds*/
    coex_adv_notifi_period = coex_params.cxm_tx_active_adv_notice * COEX_CONVERT_100MICROSECS_USTMR;
  }

  /*Initialize wwan state info*/
  memset( &coex_wwan_state_info, 0, sizeof( coex_wwan_tech_info ) * CXM_TECH_MAX );

  /*read LTE sleep notification threshold from NV*/
  coex_wwan_state_info[CXM_TECH_LTE].sleep_thresh = coex_params.cxm_lte_sleep_notifi_thres;

  /*set up the port mappings*/
  coex_wci2_port_mapping.num_ports = 5;
  coex_wci2_port_mapping.port_purpose[0].id = CXM_PORT_A;
  coex_wci2_port_mapping.port_purpose[0].purpose = CXM_PORT_PURPOSE_FRAME_SYNC;
  coex_wci2_port_mapping.port_purpose[1].id = CXM_PORT_B;
  coex_wci2_port_mapping.port_purpose[1].purpose = CXM_PORT_PURPOSE_RX_ACTIVE;
  coex_wci2_port_mapping.port_purpose[2].id = CXM_PORT_C;
  coex_wci2_port_mapping.port_purpose[2].purpose = CXM_PORT_PURPOSE_TX_ACTIVE;
  coex_wci2_port_mapping.port_purpose[3].id = CXM_PORT_D;
  coex_wci2_port_mapping.port_purpose[3].purpose = CXM_PORT_PURPOSE_WCN_PRIORITY;
  coex_wci2_port_mapping.port_purpose[4].id = CXM_PORT_E;
  coex_wci2_port_mapping.port_purpose[4].purpose = CXM_PORT_PURPOSE_LTE_SUBFR_MARKER;

  coex_smem_port_mapping.num_ports = 3;
  coex_smem_port_mapping.port_purpose[0].id = CXM_PORT_A;
  coex_smem_port_mapping.port_purpose[0].purpose = CXM_PORT_PURPOSE_TX_ACTIVE;
  coex_smem_port_mapping.port_purpose[1].id = CXM_PORT_B;
  coex_smem_port_mapping.port_purpose[1].purpose = CXM_PORT_PURPOSE_RX_ACTIVE;
  coex_smem_port_mapping.port_purpose[2].id = CXM_PORT_C;
  coex_smem_port_mapping.port_purpose[2].purpose = CXM_PORT_PURPOSE_WCN_PRIORITY;

  coex_smem_v2_port_mapping.num_ports = 3;
  coex_smem_v2_port_mapping.port_purpose[0].id = CXM_PORT_D;
  coex_smem_v2_port_mapping.port_purpose[0].purpose = CXM_PORT_PURPOSE_TX_ACTIVE;
  coex_smem_v2_port_mapping.port_purpose[1].id = CXM_PORT_D;
  coex_smem_v2_port_mapping.port_purpose[1].purpose = CXM_PORT_PURPOSE_RX_ACTIVE;
  coex_smem_v2_port_mapping.port_purpose[2].id = CXM_PORT_C;
  coex_smem_v2_port_mapping.port_purpose[2].purpose = CXM_PORT_PURPOSE_WCN_PRIORITY;

  /* initialize COEX policy info for LTE*/
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.power_threshold = 
    COEX_POLICY_POWER_THRESH_DEF;
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_power_limit =
    COEX_DEFAULT_LTE_POWER_LIMIT;
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_power_limit = 
    COEX_DEFAULT_LTE_POWER_LIMIT;
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.apt_table = 
    COEX_APT_TABLE_DEFAULT_V01;
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.tx_continuous_subframe_denials_threshold = 
    COEX_POLICY_CONT_SF_DENIALS_THRESH_DEF;
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.tx_subrame_denial_params.max_allowed_frame_denials = 
    COEX_POLICY_MAX_SF_DENIALS_DEF;
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.tx_subrame_denial_params.frame_denial_window = 
    COEX_POLICY_SF_DENIAL_WINDOW_DEF;
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_tx_pwrlmt_timeout = 
    COEX_POLICY_WCI2_PWRLMT_TIMEOUT_DEF;
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_pwrlmt_timeout = 
    COEX_POLICY_CONTR_PWRLMT_TIMEOUT_DEF;
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.tx_power_threshold_for_adv_tx_notice = 
    COEX_POLICY_POWER_THRESH_FOR_TX_ADV_NTC_DEF_DB10;
  coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.rb_threshold_for_adv_tx_notice = 
    COEX_POLICY_RB_THRESH_FOR_TX_ADV_NTC_MIN;
  coex_plcy_parms.link_path_loss_threshold = COEX_POLICY_LINK_PATH_LOSS_THRESH_DEF;
  coex_plcy_parms.t7_con_holdoff = COEX_POLICY_HOLDOFF_TIMER_RAT_CONN_STATE_DEF;
  coex_plcy_parms.t7_pwr_alpha = COEX_POLICY_FILTER_ALPHA_RAT_PWR_STATE_MIN;
  coex_plcy_parms.t7_pwr_thresh = COEX_POLICY_TX_PWR_THRESH_RAT_PWR_STATE_DEF;
  coex_plcy_parms.t7_pwr_holdoff = COEX_POLICY_HOLDOFF_TIMER_RAT_PWR_STATE_DEF;

  /* initialize band filter info */
  coex_band_filter_info.ul_len = 0;
  coex_band_filter_info.dl_len = 0;
  coex_band_filter_info.filter_on = FALSE;

  /* send boot params to LTE, if it makes it..great..otherwise wait for req */
  cxm_config_ind.boot_config.static_sys_bhvr = coex_sys_behavior_mask;
  cxm_config_ind.boot_config.port_tbl = coex_params.port_table;

  cxm_msgr_send_msg( &cxm_config_ind.msg_hdr, 
                     MCS_CXM_COEX_CONFIG_PARAMS_IND, 
                     sizeof(cxm_config_params_ind_s) ); 

  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) ||
       COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_CONTROL) )
  {
    /* Initialize the UART */
    uart_params.baud = coex_params.baud;
    uart_params.mode = WCI2_CLIENT_MODE_WCI2;
    uart_params.type_mask = WCI2_REG_TYPE_1 | WCI2_REG_TYPE_6;
    uart_params.frame_mode = WCI2_FRAME_MODE_NONE;
    uart_params.basic_rx_cb = NULL;
    uart_params.dir_rx_cb = coex_algos_wci2_dir_read_cb;
    uart_params.t2_rx_cb = NULL;
    uart_retval = wci2_client_register( &uart_params, &coex_state.uart_handle );
    CXM_ASSERT( WCI2_E_SUCCESS == uart_retval );

    /*initialize manager for WLAN sticky bit*/
    cxm_wcn_tx_init();

    /* register for SLPC callbacks, used to tell when all techs are in/out 
     * of sleep. Used for powering UART on/off */
    slpc_set_notify_callback( &coex_slpc_notify_cb );
    slpc_set_notify_wakeup_callback( &coex_slpc_notify_wakeup_cb );

    coex_wci2_t7_state.wwan_tx_active = FALSE;
    coex_wci2_t7_state.wwan_tx_pwr_active = FALSE;
    coex_wci2_t7_state.tx_ant_sel = FALSE;
    #ifndef FEATURE_COEX_USE_NV
    coex_send_diag_policy( COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 );
    #endif /* FEATURE_COEX_USE_NV */
  }

  /* states for power limiting */
  coex_controller_pwr_lmt_state = COEX_PWR_LMT_OFF;
  coex_wci2_pwr_lmt_state = COEX_PWR_LMT_OFF;
  coex_wci2_pwr_lmt_src_req_status = 0x00;
  coex_check_power_limit_in_progress = FALSE;

  /* info to limit frequency of condition success/fail indications */
  memset( &coex_throttle_ind_info, 0, sizeof(coex_throttle_condition_ind_s) );

  coex_state_info.coex_initialized = TRUE;

  atomic_set( &coex_metrics_lte_bler_started, 0 );

  /* define timer needed for wdog in wci-2-based LTE power limiting */
  timer_def( &coex_pwr_lmt_wdog_timer, &cxm_nondeferrable_timer_group, 
             &cxm_tcb, CXM_COEX_PWR_LMT_WDOG_SIG, NULL, 0 );

  /* open a handle to DAL for use later getting the Qtimer 
   * timestamp for WCN<->WWAN page scan synchronization */
  DAL_retval = DalTimetick_Attach("SystemTimer", &coex_DAL_handle);
  CXM_ASSERT( (DAL_SUCCESS == DAL_retval) && (NULL != coex_DAL_handle) );
  coex_wcn_wake_sync_info.last_msg_ts = 0;
#ifdef FEATURE_COEX_USE_NV
  /*coerce victim table entries as needed*. This only happens once since
   victim table does not change*/
  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_VICTIM_TABLE) )
  {
    /*check all power limiting entries and coerce as needed*/
    for ( i = 0; i < coex_params.num_conflicts; i++ )
    {
      if ( coex_params.victim_tbl[i].mdm_policy & CXM_POLICY_ENFORCE_CTRLR_TX_PWR_LMT )
      {
        /*check upper and lower limits and coerce value as needed*/
        if( coex_params.victim_tbl[i].mdm_params.pwr_lmt.power > COEX_POLICY_TX_POWER_LIMIT_MAX )
        {
          coex_params.victim_tbl[i].mdm_params.pwr_lmt.power = COEX_POLICY_TX_POWER_LIMIT_MAX;
        }
        else if( coex_params.victim_tbl[i].mdm_params.pwr_lmt.power < COEX_POLICY_TX_POWER_LIMIT_MIN )
        {
          coex_params.victim_tbl[i].mdm_params.pwr_lmt.power = COEX_POLICY_TX_POWER_LIMIT_MIN;
        }
      }

      if ( coex_params.victim_tbl[i].mdm_policy & CXM_POLICY_REACT_TO_WCN_PRIORITY )
      {  
        /*convert tx power threshold from db10 to signed Q8*/
        temp_Q8_conv = coex_params.victim_tbl[i].mdm_params.wcn_prio.tx_thlds.tx_pwr_thld;
        temp_Q8_conv = ( temp_Q8_conv << 8 ) / 10;
        coex_params.victim_tbl[i].mdm_params.wcn_prio.tx_thlds.tx_pwr_thld = (int16)temp_Q8_conv;

        if( coex_params.victim_tbl[i].mdm_params.wcn_prio.frame_denials.max_cont_denials < COEX_POLICY_CONT_SF_DENIALS_THRESH_MIN )
        {
          coex_params.victim_tbl[i].mdm_params.wcn_prio.frame_denials.max_cont_denials = 
            COEX_POLICY_CONT_SF_DENIALS_THRESH_MIN;
        }
        else if( coex_params.victim_tbl[i].mdm_params.wcn_prio.frame_denials.max_cont_denials > COEX_POLICY_CONT_SF_DENIALS_THRESH_MAX )
        {
          coex_params.victim_tbl[i].mdm_params.wcn_prio.frame_denials.max_cont_denials = 
            COEX_POLICY_CONT_SF_DENIALS_THRESH_MAX;
        }
        if( coex_params.victim_tbl[i].mdm_params.wcn_prio.frame_denials.duty_cycle_params.frame_denial_window > COEX_POLICY_SF_DENIAL_WINDOW_MAX )
        {
          coex_params.victim_tbl[i].mdm_params.wcn_prio.frame_denials.duty_cycle_params.frame_denial_window = 
            COEX_POLICY_SF_DENIAL_WINDOW_MAX;
        }
        if( coex_params.victim_tbl[i].mdm_params.wcn_prio.frame_denials.duty_cycle_params.max_allowed_frame_denials > COEX_POLICY_MAX_SF_DENIALS_MAX )
        {
          coex_params.victim_tbl[i].mdm_params.wcn_prio.frame_denials.duty_cycle_params.max_allowed_frame_denials = 
            COEX_POLICY_MAX_SF_DENIALS_MAX;
        }
      }
    }

    /* check bounds on link path loss threshold */
    if(coex_params.link_path_loss_threshold < COEX_POLICY_LINK_PATH_LOSS_THRESH_MIN )
    {
      coex_params.link_path_loss_threshold = COEX_POLICY_LINK_PATH_LOSS_THRESH_MIN;
    }

    /* check bounds and coerce: [0 - 100] */
    if( coex_params.filtered_rb_threshold > COEX_POLICY_FILTERED_RB_THRESH_MAX)
    {
      coex_params.filtered_rb_threshold = COEX_POLICY_FILTERED_RB_THRESH_MAX;
    }
    else if( coex_params.filtered_rb_threshold < COEX_POLICY_FILTERED_RB_THRESH_MIN )
    {
      coex_params.filtered_rb_threshold = COEX_POLICY_FILTERED_RB_THRESH_MIN;
    }
  }
#endif
  /* register cb to receive err fatal indication in trace buffer */
  if ( err_crash_cb_register( coex_fatal_err_cb ) == FALSE )
  {
    CXM_MSG_0( ERROR, "failed to register err fatal callback" );
  }

  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_CONTROL) )
  {
    coex_algos_init_v2( &coex_params );
  }

  coex_state.active_tech_mask = 0;
  coex_state.conn_tech_mask = 0;
  coex_state.tx_pwr_tech_mask = 0;
#ifdef FEATURE_COEX_USE_NV
  coex_state.tx_ant_sel = CXM_TX_ANT_SEL_UNUSED;
#else
  coex_state.tx_ant_sel = CXM_TX_ANT_SEL_X;
#endif

  CXM_MSG_0( HIGH, "coex_algos initialized" );
  
  return;
}

/*=============================================================================

  FUNCTION:  cxm_slpc_notify_cb

=============================================================================*/
/*!
    @brief
    Callback called by slpc when a tech goes in to sleep.

    @return
    void
*/
/*===========================================================================*/
void coex_slpc_notify_cb( 
  slpc_id_type id, 
  uint64 wakeup_tick, 
  boolean update, 
  boolean extension 
)
{
  cxm_coex_tech_sleep_wakeup_duration_ind_s msg;
  DalTimetickTime64Type timestamp, delta, delta_ms;
  DALResult             DAL_retval = DAL_SUCCESS;
  cxm_tech_type         cxm_tech_id;
  errno_enum_type       retval;
  /*-----------------------------------------------------------------------*/
  /* translate slpc_id to cxm_id type */
  cxm_tech_id = coex_slpc_to_cxm_id_transl( id );
  if( cxm_tech_id == CXM_TECH_DFLT_INVLD )
  {
    CXM_MSG_1( ERROR, "SLPC cb: unsupported tech %d", id );
  }
  else if( (cxm_tech_id != CXM_TECH_LTE) &&
           (coex_wwan_state_info[cxm_tech_id].conn_state != 
            CXM_TECH_STATE_INACTIVE) )
  {
    /* LTE sends us their own sleep/wake msgs, so this is not needed 
     * for LTE */

    /* determine if this is a valid wakeup. If so, send ourselves a msg; 
     * if not, process now. */
    coex_wwan_state_info[cxm_tech_id].wakeup_tick = wakeup_tick;
    DAL_retval = DalTimetick_GetTimetick64( coex_DAL_handle, &timestamp );
    CXM_MSG_4( MED, "SLPC cb: id=%d, wake_tick=%u, ts=%u, update=%d", 
               id, wakeup_tick, timestamp, update );

    msg.tech_id = cxm_tech_id;

    if( DAL_SUCCESS != DAL_retval || timestamp > wakeup_tick )
    {
      /* wakeup in the past! Process now */
      msg.is_going_to_sleep = FALSE;
      msg.duration = 0;
    }
    else
    {
      /* Whether start of new wakeup or update to existing, send 
       * CXM Sleep notification */
      delta = wakeup_tick - timestamp;
      DalTimetick_CvtFromTimetick64( coex_DAL_handle, delta, T_MSEC, &delta_ms );
      CXM_ASSERT( DAL_SUCCESS == DAL_retval );
      msg.is_going_to_sleep = TRUE;
      msg.duration = delta_ms;
    } /* valid wakeup */

    retval = cxm_msgr_send_msg( &msg.msg_hdr, 
               MCS_CXM_COEX_TECH_SLEEP_WAKEUP_IND, 
               sizeof(cxm_coex_tech_sleep_wakeup_duration_ind_s) );
    CXM_ASSERT( E_SUCCESS == retval );

  } /* valid cxm_tech_id */

  return;
} /* coex_slpc_notify_cb */

/*=============================================================================

  FUNCTION:  coex_slpc_notify_wakeup_cb

=============================================================================*/
/*!
    @brief
    Callback triggered by slpc when tech is waking up. 

    @return
    void
*/
/*===========================================================================*/
void coex_slpc_notify_wakeup_cb( slpc_id_type id, uint64 wakeup_tick )
{
  cxm_coex_tech_sleep_wakeup_duration_ind_s msg;
  errno_enum_type                           retval;
  cxm_tech_type                             cxm_tech_id;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( LOW, "SLPC wakeup event cb: id=%d", id );

  /* translate slpc_id to cxm_id type */
  cxm_tech_id = coex_slpc_to_cxm_id_transl( id );

  if( (cxm_tech_id != CXM_TECH_DFLT_INVLD) &&
      (cxm_tech_id != CXM_TECH_LTE) &&
      (coex_wwan_state_info[cxm_tech_id].conn_state != 
       CXM_TECH_STATE_INACTIVE) )
  {
    /* fill out wakeup msg to send to ourselves, to process appropriately
     * in CXM context. */
    msg.tech_id = cxm_tech_id;
    msg.is_going_to_sleep = FALSE;
    msg.duration = 0;
    retval = cxm_msgr_send_msg( &msg.msg_hdr, 
               MCS_CXM_COEX_TECH_SLEEP_WAKEUP_IND, 
               sizeof(cxm_coex_tech_sleep_wakeup_duration_ind_s) );
    CXM_ASSERT( E_SUCCESS == retval );
  }

  return; 
} /* coex_slpc_notify_wakeup_cb */

/*=============================================================================

  FUNCTION:  coex_trigger_grfc

=============================================================================*/
/*!
    @brief
    Triggers TX Active grfc on or off at a specified ustmr time
 
    @return
    none
*/
/*===========================================================================*/
void coex_trigger_grfc (
  boolean level,
  uint32 ustmr_time
)
{
  /*-----------------------------------------------------------------------*/
  CXM_MSG_2( MED, "set grfc level: %d, at time: %d ", level, ustmr_time);
  if( 0 != cxm_fw_smem )
  {
    cxm_fw_smem->grfc_task.assert = level;
    cxm_fw_smem->grfc_task.trigger_time = ustmr_time;
  }
  fws_ipc_send(FWS_IPC_SW_FW__GRFC_CMD);

  return;
}

/*=============================================================================

  FUNCTION:  coex_process_late_tx_adv

=============================================================================*/
/*!
    @brief
    Handles late TX advanced notice according to NV specification. 
 
    @return
    none
*/
/*===========================================================================*/
void coex_process_late_tx_adv (
  void
)
{
  /*-----------------------------------------------------------------------*/
  cxm_counter_event(CXM_CNT_LATE_LTE_TX_MSG, 0);
  CXM_MSG_0( MED, "Processing late TX advanced, count");
  if (!coex_state_txadv_late)
  {
    coex_state_txadv_late = TRUE;
    switch (coex_params.tx_adv_late_toggle)
    {
      case COEX_STATE_OFF:
        coex_trigger_grfc(FALSE, COEX_ADD_USTMR(COEX_READ_USTMR_TIME(), COEX_ADV_TIMING_TOLERANCE) );
        coex_deassert_scheduled = FALSE;
        break;
      case COEX_STATE_ON:
        coex_trigger_grfc(TRUE, COEX_ADD_USTMR(COEX_READ_USTMR_TIME(), COEX_ADV_TIMING_TOLERANCE) );
        coex_deassert_scheduled = TRUE;
        break;
      case COEX_STATE_LAST:
      default:
        break;
    }
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_manage_uart_npa_vote

=============================================================================*/
/*!
    @brief
    Check policy and lte state to determine if uart should be powered
    down or up.

    @return none
*/
/*===========================================================================*/
static void coex_manage_uart_npa_vote ( void )
{
  boolean type7_active = FALSE;
  boolean lte_metrics_vote = FALSE;
  /*-----------------------------------------------------------------------*/
  /*if CXM is controlling the TX grfc, check if it needs to be turned OFF*/
  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_CXM_ASSERTS_TX_ADV) &&
       coex_deassert_scheduled && 
       ( coex_state.active_tech_mask == 0 ||
       !(coex_wwan_state_info[CXM_TECH_LTE].policy & CXM_POLICY_TOGGLE_TX_ACTIVE) ) )
  {
    coex_deassert_scheduled = FALSE;
    coex_trigger_grfc(FALSE, COEX_ADD_USTMR(COEX_READ_USTMR_TIME(), COEX_ADV_TIMING_TOLERANCE) );
  }

  /* check if UART needs to be on for the type 7 policy 
   * (tech both on and connected) */
  if( ( COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy ||
        COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy ) && 
      ( (coex_state.conn_tech_mask & coex_state.active_tech_mask) != 0 ) )
  {
    type7_active = TRUE;
  }

  /* don't want to force the UART on if doing control plane over WCI-2 */
  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_QMI_METRICS) && 
      ( atomic_read(&coex_metrics_lte_bler_started) || 
        atomic_read(&coex_wwan_state_info[CXM_TECH_LTE].metrics_started) ) )
  {
    lte_metrics_vote = TRUE;
  }

  if ( ((coex_state.active_tech_mask & (1<<CXM_TECH_LTE)) ||
        type7_active ) &&
       (coex_wwan_state_info[CXM_TECH_LTE].policy_state == COEX_POLICY_ACTIVE || 
        lte_metrics_vote ) &&
       (coex_state.uart_en_state == FALSE) )
  {
    coex_state.uart_en_state = TRUE;
    wci2_client_enable_uart( coex_state.uart_handle, coex_state.uart_en_state );
  }
  else if ( ( (((coex_state.active_tech_mask & (1<<CXM_TECH_LTE)) == 0) && 
               !type7_active) ||
              ( coex_wwan_state_info[CXM_TECH_LTE].policy_state == COEX_POLICY_INACTIVE && 
         !lte_metrics_vote ) ) &&
            (coex_state.uart_en_state == TRUE) )
  {
    coex_state.uart_en_state = FALSE;
    wci2_client_enable_uart( coex_state.uart_handle, coex_state.uart_en_state );
  }

  CXM_MSG_6( MED, "coex npa - vote %d, policy %d, bler %d, sinr %d, techs_active 0x%x, techs_conn 0x%x",
             coex_state.uart_en_state, 
             coex_wwan_state_info[CXM_TECH_LTE].policy_state,
             atomic_read(&coex_metrics_lte_bler_started),
             atomic_read(&coex_wwan_state_info[CXM_TECH_LTE].metrics_started),
             coex_state.active_tech_mask,
             coex_state.conn_tech_mask );
  CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_STATE);

  return;
}

/*=============================================================================

  FUNCTION:  coex_update_wlan_conn_state_info

=============================================================================*/
/*!
    @brief
    This function updates current connection list with new connection,
    or deletes an old connection, per incoming wlan message
	
    @detail
    Using the conn linked list, the function attempts to locate the
    incoming conn data in the existing list.  If it is not found, a new
    node is created with the new conn data.  Then the function determines
    if the incoming conn data is changing to DISABLED state, indicating
    that the conn is no longer active.  If this is the case, the conn
    data is removed from the conn linked list.
 
    @return
    none
*/
/*===========================================================================*/
static void coex_update_wlan_conn_state_info (
  coex_set_wlan_state_req_msg_v01 *request
)
{
  coex_wlan_conn_node_type *node = NULL;
  coex_wlan_conn_node_type *prev = NULL;
  /*-----------------------------------------------------------------------*/
  CXM_TRACE( CXM_TRC_QMI_UPDATE_WLAN_CONN,
                   request->conn_info.handle, request->conn_info.state,
                   request->conn_info.mode, CXM_TRC_EN );

  if ( coex_wlan_conn_head == NULL )
  {
    CXM_MSG_1( MED, "creating new wlan conn node, handle %d",
               request->conn_info.handle );

    /* list is empty, create single node list */
    node = CXM_MEM_CALLOC( 1, (size_t) sizeof(coex_wlan_conn_node_type) );
    CXM_ASSERT( NULL != node );
    coex_wlan_conn_head = node;
  }
  else
  {
    /* list is not empty, try to find an existing node */
    prev = coex_wlan_conn_head;
    node = coex_wlan_conn_head;
    while ( node != NULL )
    {
      /* if handles match, this is a known connection */
      if ( node->conn.handle == request->conn_info.handle )
      {
        break;
      }
      prev = node;
      node = node->next;
    }

    if ( node == NULL )
    {
      CXM_MSG_1( MED, "creating new wlan conn node, handle %d",
                 request->conn_info.handle );

      /* could not find existing node, create new one at tail */
      node = CXM_MEM_CALLOC( 1, (size_t) sizeof(coex_wlan_conn_node_type) );
      CXM_ASSERT( NULL != node );
      prev->next = node;
    }
  }

  /* at this point node points to the correct node for this data */

  if ( request->conn_info.state == COEX_WLAN_CONN_DISABLED )
  {
    CXM_MSG_1( MED, "deleting wlan conn node, handle %d",
               request->conn_info.handle );

    /* this conn is done, delete it */
    if ( node == coex_wlan_conn_head )
    {
      /* special case to delete first node */
      coex_wlan_conn_head = node->next;
    }
    else if ( prev != NULL )
    {
      /* multiple nodes, delete and rejoin the remaining list */
      prev->next = node->next;
    }

    CXM_MEM_FREE( node );
  }
  else
  {
    /* this connection is active, copy in the data */
    node->conn = request->conn_info;
  }

  return;  
}

/*=============================================================================

  FUNCTION:  coex_update_wlan_hp_info

=============================================================================*/
/*!
    @brief
    This function updates current high prio list with new high prio,
    or deletes an old high prio, per incoming wlan message
	
    @detail
    Using the high prio event linked list, the function attempts to locate the
    incoming high prio data in the existing list. If it is not found, a new
    node is created with the new high prio data. Then the function determines
    if the incoming high prio data is changing to STOP state, indicating
    that the high prio event is no longer active. If this is the case, the high prio
    event data is removed from the linked list.
 
    @return
    none
*/
/*===========================================================================*/
static void coex_update_wlan_hp_state_info (
  coex_set_wlan_state_req_msg_v01 *request
)
{
  coex_wlan_hp_node_type *node = NULL;
  coex_wlan_hp_node_type *prev = NULL;
  /*-----------------------------------------------------------------------*/
  CXM_TRACE( CXM_TRC_QMI_UPDATE_WLAN_HP,
                   request->high_prio_info.id, request->high_prio_info.state, 
                   request->high_prio_info.band.freq, CXM_TRC_EN );

  if ( coex_wlan_hp_head == NULL )
  {
    CXM_MSG_1( MED, "creating first wlan high prio node, id %d",
               request->high_prio_info.id );

    /* list is empty, create single node list */
    node = CXM_MEM_CALLOC( 1, (size_t) sizeof(coex_wlan_hp_node_type) );
    CXM_ASSERT( NULL != node );
    coex_wlan_hp_head = node;
  }
  else
  {
    /* list is not empty, try to find an existing node */
    prev = coex_wlan_hp_head;
    node = coex_wlan_hp_head;
    while ( node != NULL )
    {
      /* if id's match, this is a known connection */
      if ( node->high_prio.id == request->high_prio_info.id )
      {
        break;
      }
      prev = node;
      node = node->next;
    }

    if ( node == NULL )
    {
      CXM_MSG_1( MED, "creating new wlan high prio node, id %d",
                 request->high_prio_info.id );

      /* could not find existing node, create new one at tail */
      node = CXM_MEM_CALLOC( 1, (size_t) sizeof(coex_wlan_hp_node_type) );
      CXM_ASSERT( NULL != node );
      prev->next = node;
    }
  }

  if ( request->high_prio_info.state == COEX_WLAN_HIGH_PRIO_STOP )
  {
    CXM_MSG_1( MED, "deleting wlan high prio node, id %d",
               request->high_prio_info.id );

    /* this high prio is done, delete it */
    if ( node == coex_wlan_hp_head )
    {
      /* special case to delete first node */
      coex_wlan_hp_head = node->next;
    }
    else if ( prev != NULL )
    {
      /* multiple nodes, delete and rejoin the remaining list */
      prev->next = node->next;
    }

    CXM_MEM_FREE( node );
  }
  else
  {
    /* this high prio event is active, copy in the data */
    node->high_prio = request->high_prio_info;
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_boot_params_req

=============================================================================*/
/*!
    @brief
    Handles the boot parameters confirmation message
 
    @detail
    This function handles the parameters request message, sent by
    lte when it desires the params information.  This function
    verifies that the result returned in the message is successful.  If
    this is a successful request, this function sends the params
    indication to lte.
 
    @return
    err code from lte indicating success/failure in receiving params
*/
/*===========================================================================*/
errno_enum_type coex_handle_params_req (
  void *rcv_msgr_msg_ptr
)
{
  /* boot message received by CFW and tech L1s*/
  cxm_config_params_ind_s        cxm_config_ind;
  errno_enum_type ret_val;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( coex_state_info.coex_initialized == TRUE );

  /* send config params */
  cxm_config_ind.boot_config.static_sys_bhvr = coex_sys_behavior_mask;
  cxm_config_ind.boot_config.port_tbl = coex_params.port_table;

  ret_val = cxm_msgr_send_msg( &cxm_config_ind.msg_hdr, 
                     MCS_CXM_COEX_CONFIG_PARAMS_IND, 
                     sizeof(cxm_config_params_ind_s) ); 

  return ret_val;
}

/*===========================================================================

  FUNCTION:  cxm_req_boot_params

===========================================================================*/
/*!
  @brief
    Resend the boot params message. CFW will use this to ask for the boot
    params message. A function call is used here since CFW does not yet
    have a msgr message send queue.

  @return
    none
*/
/*=========================================================================*/
void cxm_req_boot_params (
  void
)
{
  cxm_config_params_ind_s        cxm_config_ind;
  /*-----------------------------------------------------------------------*/
  /* if algos init is not yet done, we will send the boot params when it is done*/
  if ( coex_state_info.coex_initialized )
  {
    /* send config params */
    cxm_config_ind.boot_config.static_sys_bhvr = coex_sys_behavior_mask;
    cxm_config_ind.boot_config.port_tbl = coex_params.port_table;

    cxm_msgr_send_msg( &cxm_config_ind.msg_hdr, 
                       MCS_CXM_COEX_CONFIG_PARAMS_IND, 
                       sizeof(cxm_config_params_ind_s) );
  }

  return;
}

/*===========================================================================

  FUNCTION:  cxm_set_tx_ant_sel

===========================================================================*/
/*!
  @brief
    Used to set transmit (Tx) antenna selected
	  
	@return
	  errno_enum_type
*/
/*=========================================================================*/
errno_enum_type cxm_set_tx_ant_sel (
  cxm_tx_ant_sel_e tx_ant_sel
)
{
  errno_enum_type retval = E_SUCCESS;
  cxm_coex_tx_ant_sel_ind_s msgr_msg;
  /*-----------------------------------------------------------------------*/

  CXM_MSG_2( HIGH, "Set Tx Ant selection[%d] supported[%d]",
             tx_ant_sel,
             COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) );
  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) )
  {
    msgr_msg.tx_ant_sel = tx_ant_sel;
    retval = cxm_msgr_send_msg( &msgr_msg.msg_hdr,
                                MCS_CXM_COEX_TX_ANT_SEL_IND,
                                sizeof(cxm_coex_tx_ant_sel_ind_s) );
    CXM_ASSERT( E_SUCCESS == retval );
  }
  else
  {
    retval = E_NOT_SUPPORTED;
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_check_and_set_power_limit

=============================================================================*/
/*!
    @brief
    Helper function to clear or set appropriately the LTE power limit

    @return void
*/
/*===========================================================================*/
void coex_check_and_set_power_limit (
  void
)
{
#ifdef FEATURE_LTE
  static float current_power_limit = COEX_DEFAULT_LTE_POWER_LIMIT;
  float new_power_limit;
  int16 int_power_limit;
  rfa_rf_lte_set_tx_plimit_ind_s msgr_msg;
  errno_enum_type                retval = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  /* Reset the memory available for the RF set Tx power limit message */
  memset( &msgr_msg, 0, sizeof( rfa_rf_lte_set_tx_plimit_ind_s ) );

  /* can only reset limit if LTE in system... */
  if( coex_wwan_state_info[CXM_TECH_LTE].state_data.num_link_info_sets != 0 )
  {
    /* set based on state of both controller and wci-2 power limit */
    if( (coex_controller_pwr_lmt_state == COEX_PWR_LMT_ACTIVE) &&
        (coex_wci2_pwr_lmt_state == COEX_PWR_LMT_ACTIVE) )
    {
      /* resolve - use lower of the two */
      if( coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_power_limit < 
          coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_power_limit )
      {
        new_power_limit = 
          coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_power_limit;
      }
      else
      {
        new_power_limit = coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_power_limit;
      }
    }
    else if( coex_controller_pwr_lmt_state == COEX_PWR_LMT_ACTIVE )
    {
      new_power_limit = 
        coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_power_limit;
    }
    else if( coex_wci2_pwr_lmt_state == COEX_PWR_LMT_ACTIVE )
    {
      new_power_limit = coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_power_limit;
    }
    else
    {
      /* neither policy active - clear limit */
      new_power_limit = COEX_DEFAULT_LTE_POWER_LIMIT;
    }

    if( new_power_limit != current_power_limit )
    {
      /* cast is safe - range is checked in SET POLICY REQ */
      int_power_limit = (int16) (new_power_limit * 10);

      CXM_MSG_1( HIGH, "Setting LTE power limit to %d(db10)", int_power_limit );

      msgr_msg.sg_txpl_params.plimit_type_mask = RFA_RF_LTE_WCN_COEX_PLIMIT_MASK;
      msgr_msg.sg_txpl_params.wcn_coex_plimit = int_power_limit;
      retval = cxm_msgr_send_msg( &msgr_msg.hdr,
                                  RFA_RF_LTE_SET_TX_PLIMIT_IND,
                                  sizeof(rfa_rf_lte_set_tx_plimit_ind_s) );
      CXM_ASSERT( E_SUCCESS == retval );

      CXM_TRACE( CXM_TRC_SET_LTE_TX_PWR_LMT, (uint32)new_power_limit, 
                       (uint32)current_power_limit, 0, CXM_TRC_AND_PKT_EN,
                       CXM_LOG_WCI2_PWR_LMT_STATE,
                       CXM_LOG_CONTROLLER_PWR_LMT_STATE, CXM_LOG_LTE_POLICY_INFO );
      current_power_limit = new_power_limit;
    }
  }
#endif /*FEATURE_LTE*/

  return;
}

/*=============================================================================

  FUNCTION:  coex_check_power_limiting_conditions

=============================================================================*/
/*!
    @brief
    Helper function to check with ML1 to see if conditions allow LTE
    power limiting

    @return void
*/
/*===========================================================================*/
void coex_check_power_limiting_conditions (
  void
)
{
  errno_enum_type                  retval = E_SUCCESS;
  cxm_coex_generic_req_s           msgr_msg;
  /*-----------------------------------------------------------------------*/
  if(coex_check_power_limit_in_progress)
  {
    /* interface only allows for one 'GET' call at a time */
    return;
  }
  else
  {
    /* only call if LTE in system. if LTE not in system, state remains
       pending, and check will repeat when it comes back */
    if( coex_wwan_state_info[CXM_TECH_LTE].state_data.num_link_info_sets != 0 )
    {
      coex_check_power_limit_in_progress = TRUE;
      retval = cxm_msgr_send_msg( &msgr_msg.msg_hdr,
                                  MCS_CXM_COEX_TX_PWR_LMT_LTE_CNDTNS_REQ,
                                  sizeof(cxm_coex_generic_req_s) );
      CXM_ASSERT( E_SUCCESS == retval );
    }
  }

  CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, 
            CXM_LOG_WCI2_PWR_LMT_STATE, CXM_LOG_CONTROLLER_PWR_LMT_STATE );

  return;
}

/*=============================================================================

  FUNCTION:  coex_prep_and_send_wci2_type7

=============================================================================*/
/*!
    @brief
    Send a type 7 (WWAN TX ACTIVE) message out over WCI2, if the corresponding
    policy bit is enabled.

    @return
    void
*/
/*===========================================================================*/
void coex_prep_and_send_wci2_type7(
  uint32 event_mask,
  cxm_tech_type tech
)
{
  wci2_msg_type_s t7_msg;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( HIGH, "Request to prep & send wci2 type7 for event mask %d",
             event_mask );
  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) &&
      (COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 & coex_wwan_state_info[tech].policy ||
       COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01  & coex_wwan_state_info[tech].policy ||
       COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL_V01 & coex_wwan_state_info[tech].policy) )
  {
    t7_msg.type = WCI2_TYPE7;

    /* copy over previously sent state */
    t7_msg.data.type7_wwan_state.wwan_tx_active     = coex_wci2_t7_state.wwan_tx_active;
    t7_msg.data.type7_wwan_state.wwan_tx_pwr_active = coex_wci2_t7_state.wwan_tx_pwr_active;
    t7_msg.data.type7_wwan_state.tx_ant_sel         = coex_wci2_t7_state.tx_ant_sel;

    /* handle wci2 t7[rrc_c] state */
    if( (event_mask & COEX_WCI2_T7_CONN_ST_EVNT_MSK) != 0 )
    {
      t7_msg.data.type7_wwan_state.wwan_tx_active = 
          (coex_state.conn_tech_mask != 0) ? TRUE : FALSE;
      coex_wci2_t7_state.wwan_tx_active = t7_msg.data.type7_wwan_state.wwan_tx_active;
    }

    /* handle wci2 t7[p_q] state */
    if ( (event_mask & COEX_WCI2_T7_PWR_ST_EVNT_MSK) != 0 )
    {
      t7_msg.data.type7_wwan_state.wwan_tx_pwr_active = 
        (coex_state.tx_pwr_tech_mask != 0) ? TRUE : FALSE;
      coex_wci2_t7_state.wwan_tx_pwr_active = t7_msg.data.type7_wwan_state.wwan_tx_pwr_active;
    }

    /* handle wci2 t7[ant_tx] state */
    if ( (event_mask & COEX_WCI2_T7_TX_ANT_SEL_EVNT_MSK) != 0 )
    {
      t7_msg.data.type7_wwan_state.tx_ant_sel =
        (coex_state.tx_ant_sel == CXM_TX_ANT_SEL_Y) ? TRUE : FALSE;
      coex_wci2_t7_state.tx_ant_sel = t7_msg.data.type7_wwan_state.tx_ant_sel;
    }

    /* since various state bits of type7 transitioning to off may be delayed 
     * by holdoff timer for filtering, we may have to power back on the UART
     * explicitly. This is because the coex npa voting code relies on the modem 
     * connection state, and a delayed type7 does not reflect the true 
     * Modem CONN state or Modem Tx power state. */
    if( coex_state.uart_en_state == FALSE )
    {
      CXM_MSG_0( MED, "Powering on/off UART for type 7" );
      wci2_client_enable_uart( coex_state.uart_handle, TRUE );
      wci2_send_msg( &t7_msg );
      wci2_client_enable_uart( coex_state.uart_handle, FALSE );
    }
    else
    {
      wci2_send_msg( &t7_msg );
    }
    CXM_MSG_3( HIGH, "Sent wci2 type7 data: wwan active %d, wwan tx active %d, tx ant %d",
               t7_msg.data.type7_wwan_state.wwan_tx_active,
               t7_msg.data.type7_wwan_state.wwan_tx_pwr_active,
               t7_msg.data.type7_wwan_state.tx_ant_sel );
    CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_MDM_STATE );
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_check_wci2_type7_state

=============================================================================*/
/*!
    @brief
    Helper function to check current type7 state & decide to send it

    @return void
*/
/*===========================================================================*/
void coex_check_wci2_type7_state (
  uint32           old_tech_tx_active_mask,
  uint32           old_tech_tx_power_mask,
  cxm_tx_ant_sel_e old_tx_ant_sel,
  uint32           event_mask
)
{
  uint32 wci2_t7_msk = 0x00;
  /*-----------------------------------------------------------------------*/
  /* if TX On, see if we need to power on UART to send type 7 */
  coex_manage_uart_npa_vote();

  CXM_MSG_7( HIGH, "Check type7-[event %d][conn old:%d new:%d][tx_pwr old:%d new:%d][tx_ant old:%d new:%d]",
             event_mask,
             old_tech_tx_active_mask,
             coex_state.conn_tech_mask,
             old_tech_tx_power_mask,
             coex_state.tx_pwr_tech_mask,
             old_tx_ant_sel,
             coex_state.tx_ant_sel );

  /* handle wci2 type7[rrc_c] processing */
  if( (COEX_WCI2_T7_CONN_ST_EVNT_MSK & event_mask) &&
      (COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 &
       coex_wwan_state_info[CXM_TECH_LTE].policy) )
  {
    if( (old_tech_tx_active_mask == 0 && coex_state.conn_tech_mask != 0) ||
        (old_tech_tx_active_mask != 0 && coex_state.conn_tech_mask == 0) )
    {
      wci2_t7_msk |= COEX_WCI2_T7_CONN_ST_EVNT_MSK;
    }
  }

  /* handle wci2 type7[p_q] processing */
  if( (COEX_WCI2_T7_PWR_ST_EVNT_MSK & event_mask) &&
      (COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01 &
       coex_wwan_state_info[CXM_TECH_LTE].policy) )
  {
    if( (old_tech_tx_power_mask == 0 && coex_state.tx_pwr_tech_mask != 0) ||
        (old_tech_tx_power_mask != 0 && coex_state.tx_pwr_tech_mask == 0) )
    {
      wci2_t7_msk |= COEX_WCI2_T7_PWR_ST_EVNT_MSK;
    }
  }

  /* handle wci2 type7[ant_tx] processing */
  if( (COEX_WCI2_T7_TX_ANT_SEL_EVNT_MSK & event_mask) &&
    (COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL_V01 &
     coex_wwan_state_info[CXM_TECH_LTE].policy) )
  {
    /* React only to transitions
       - Unused --> X or Y
       - X --> Y
       - Y --> X */
    if( (old_tx_ant_sel == CXM_TX_ANT_SEL_UNUSED && 
         coex_state.tx_ant_sel != CXM_TX_ANT_SEL_UNUSED) ||
        (old_tx_ant_sel == CXM_TX_ANT_SEL_X && 
         coex_state.tx_ant_sel == CXM_TX_ANT_SEL_Y) ||
        (old_tx_ant_sel == CXM_TX_ANT_SEL_Y && 
         coex_state.tx_ant_sel == CXM_TX_ANT_SEL_X) )
    {
      wci2_t7_msk |= COEX_WCI2_T7_TX_ANT_SEL_EVNT_MSK;
    }
  }

  if( wci2_t7_msk )
  {
    coex_prep_and_send_wci2_type7( wci2_t7_msk, CXM_TECH_LTE );
  }
  /* TX now off, power off UART if needed after sending the type 7 */
  coex_manage_uart_npa_vote();

  return;
}

/*=============================================================================

  FUNCTION:  coex_prep_and_send_policy_msg

=============================================================================*/
/*!
    @brief
    Function to prepare & send the coex policy currently in use

    @detail
    This function currently is can be called from either within the actual
    set policy REQ message or when sending the WWAN state IND msg

    @return
    qmi_csi_error
*/
/*===========================================================================*/
errno_enum_type coex_prep_and_send_policy_msg (
  cxm_tech_type tech
)
{
  errno_enum_type                   retval = E_SUCCESS;
  cxm_coex_active_policy_lte_ind_s  msgr_msg;
  cxm_coex_active_policy_ind_s      msgr_tech_msg; 
  int32                             tx_thresh;
  uint32                            rb_alpha;
  uint32                            tx_pwr_alpha;
  uint8                             active_conf_index;
  uint32                            log_code = 0;
  /* points to the active_config of the message payload we will send */
  cxm_active_config_v01_s           *active_config;
  msgr_umid_type                    tech_umid = 0;
  /*-----------------------------------------------------------------------*/
  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_CXM_SENDS_POLICY) )
  {
    if ( tech == CXM_TECH_LTE )
    {
      memset( &msgr_msg, 0, sizeof( cxm_coex_active_policy_lte_ind_s ) );
      active_config = &msgr_msg.active_config;
    }
    else
    {
      memset( &msgr_tech_msg, 0, sizeof( cxm_coex_active_policy_ind_s ) );
      active_config = &msgr_tech_msg.active_config;
    }
    
    /*copy info from policy info to msgr msg*/
    active_config->active_cxm_policy = (cxm_policy_t)coex_wwan_state_info[tech].policy;
    
    /*if CXM function to assert TX active in advance is enabled, do not send TX active policy to FW
     and send type 6 policy*/
    if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_CXM_ASSERTS_TX_ADV) )
    {
      if ( active_config->active_cxm_policy & CXM_POLICY_TOGGLE_TX_ACTIVE )
      {
        active_config->active_cxm_policy &= ~CXM_POLICY_TOGGLE_TX_ACTIVE;
        active_config->active_cxm_policy |= CXM_POLICY_WCI2_OUTGNG_TYPE6_TX_ADV_NOTICE;
      }
      else
      {
        active_config->active_cxm_policy &= ~CXM_POLICY_WCI2_OUTGNG_TYPE6_TX_ADV_NOTICE;
      }
      
    }
    
    if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_SMEM_DATA) )
    {
      if ( tech == CXM_TECH_LTE )
      {
        active_config->active_port_mapping = coex_smem_port_mapping;
      }
      else
      {
        active_config->active_port_mapping = coex_smem_v2_port_mapping;
      }
      
    }
    else
    {
      active_config->active_port_mapping = coex_wci2_port_mapping;
    }
    if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_QMI_POLICY_CONTROL) )
    {
      /* convert from float to Q8 */
      cxm_float_to_signed_Q8(  coex_wwan_state_info[tech].plcy_parms.power_threshold, &tx_thresh);
      cxm_float_to_unsigned_Q8( coex_plcy_parms.rb_filter_alpha, &rb_alpha );
      cxm_float_to_unsigned_Q16( coex_plcy_parms.t7_pwr_alpha, &tx_pwr_alpha );

      active_config->tech_tx_thlds.tx_pwr_thld = (int16) tx_thresh;
      active_config->tech_tx_thlds.tx_bw_thld = coex_wwan_state_info[tech].plcy_parms.rb_threshold;

      active_config->wcn_txfrmdnl_params.max_continuous_allowed_frame_denials = 
        (uint32) coex_wwan_state_info[tech].plcy_parms.tx_continuous_subframe_denials_threshold;
      active_config->wcn_txfrmdnl_params.frame_denial_params.max_allowed_frame_denials = 
        (uint32) coex_wwan_state_info[tech].plcy_parms.tx_subrame_denial_params.max_allowed_frame_denials;
      active_config->wcn_txfrmdnl_params.frame_denial_params.frame_denial_window =
        (uint32) coex_wwan_state_info[tech].plcy_parms.tx_subrame_denial_params.frame_denial_window;
        if ( tech == CXM_TECH_LTE )
        {
          msgr_msg.tx_bw_filter_alpha = rb_alpha;
        }
        else
        {
          msgr_tech_msg.tx_bw_filter_alpha = rb_alpha;
        }
      msgr_msg.tx_pwr_filter_params.filter_alpha = tx_pwr_alpha;
      msgr_msg.tx_pwr_filter_params.threshold = coex_plcy_parms.t7_pwr_thresh;
    }
    if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_VICTIM_TABLE) && coex_wwan_state_info[tech].policy != CXM_POLICY_NONE)
    {
      active_conf_index = coex_wwan_state_info[tech].active_conf_index;
      /* convert from float to Q8 */
      cxm_float_to_unsigned_Q8(coex_params.rb_filter_alpha, &rb_alpha );
      active_config->tech_tx_thlds.tx_pwr_thld = coex_params.victim_tbl[active_conf_index].mdm_params.wcn_prio.tx_thlds.tx_pwr_thld;
      active_config->tech_tx_thlds.tx_bw_thld = coex_params.victim_tbl[active_conf_index].mdm_params.wcn_prio.tx_thlds.tx_bw_thld;

      active_config->wcn_txfrmdnl_params.max_continuous_allowed_frame_denials = 
        coex_params.victim_tbl[active_conf_index].mdm_params.wcn_prio.frame_denials.max_cont_denials;
      active_config->wcn_txfrmdnl_params.frame_denial_params.frame_denial_window = 
        coex_params.victim_tbl[active_conf_index].mdm_params.wcn_prio.frame_denials.duty_cycle_params.frame_denial_window;
      active_config->wcn_txfrmdnl_params.frame_denial_params.max_allowed_frame_denials = 
        coex_params.victim_tbl[active_conf_index].mdm_params.wcn_prio.frame_denials.duty_cycle_params.max_allowed_frame_denials;
      /* Only LTE uses RBs */      
      msgr_msg.tx_bw_filter_alpha = rb_alpha;
      /* TODO: add tx pwr filtering parameters to NV based solution */
    }

    /* if sinr and bler are acttive we need lte subframe marker. */
    if ( ( atomic_read(&coex_metrics_lte_bler_started) || atomic_read(&coex_wwan_state_info[tech].metrics_started) ) &&
          COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) && tech == CXM_TECH_LTE )
    {
       msgr_msg.active_config.dynamic_sys_bhvr |= CXM_SYS_BHVR_TRIGGER_LTE_SUBFR_MARKER;
    }

    if ( tech == CXM_TECH_LTE )
    {
      retval = cxm_msgr_send_msg( &msgr_msg.msg_hdr, 
                              MCS_CXM_COEX_ACTIVE_POLICY_LTE_IND, 
                              sizeof(cxm_coex_active_policy_lte_ind_s) 
                              );
      log_code = CXM_LOG_LTE_POLICY_INFO;
    }
    else if ( tech == CXM_TECH_TDSCDMA || tech == CXM_TECH_GSM1 || tech == CXM_TECH_GSM2 ||
              tech == CXM_TECH_GSM3 )
    {
      switch ( tech )
      {
        case CXM_TECH_TDSCDMA:
          tech_umid = MCS_CXM_COEX_ACTIVE_POLICY_TDSCDMA_IND;
          log_code = CXM_LOG_TDS_POLICY_INFO;
          break;
        case CXM_TECH_GSM1:
          tech_umid = MCS_CXM_COEX_ACTIVE_POLICY_GSM1_IND;
          log_code = CXM_LOG_GSM1_POLICY_INFO;
          break;
        case CXM_TECH_GSM2:
          tech_umid = MCS_CXM_COEX_ACTIVE_POLICY_GSM2_IND;
          log_code = CXM_LOG_GSM2_POLICY_INFO;
          break;
        case CXM_TECH_GSM3:
          tech_umid = MCS_CXM_COEX_ACTIVE_POLICY_GSM3_IND;
          log_code = CXM_LOG_GSM3_POLICY_INFO;
          break;
        default:
          break;
      }

      retval = cxm_msgr_send_msg( &msgr_tech_msg.msg_hdr, 
                                  tech_umid, 
                                  sizeof(cxm_coex_active_policy_ind_s) 
                                );
    }
    
    CXM_MSG_2( HIGH, "Active (policy) msg sent, tech=%d, mask=%x", tech, 
               active_config->active_cxm_policy );
    CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN, log_code );
  }

  return retval;
}

#ifdef FEATURE_COEX_USE_NV
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
  cxm_tech_type                     wwan_tech,
  cxm_wcn_tech_type                 wcn_tech,
  cxm_tech_link_info_s *            wwan_band,
  coex_band_type_v01 *              wcn_band,
  uint32                            wcn_mode,
  coex_wwan_operating_dimension_v01 wwan_op_dim,
  uint64 *                          conf_prio,
  uint8 *                           conf_prio_id
)
{
  uint8  i;
  uint32 wwan_state_start;
  uint32 wwan_state_stop;
  uint32 wcn_state_start;
  uint32 wcn_state_stop;
  int64  curr_wwan_overlap = 0;
  uint64 curr_conf_prio = 0;

  /*--------------------------------------------------------------------------*/
  CXM_MSG_3( MED, "resolve_con, wlan freq is %d and bandwidth is %d, mode=%d" ,  
             wcn_band->freq, wcn_band->bandwidth, wcn_mode );
  /* initialize max overlap values */
  *conf_prio = *conf_prio_id = 0;

  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_CNTR_FRQ_CONFLICT) )
  {
    /*if a conflict is defined by only the center frequency of the current wwan and wcn,
      set start = stop = center_frequency. This preserves the range based algorithm although
      there is a loss in efficiency since the conflict check could be simpler*/
    wcn_state_start = wcn_state_stop = wcn_band->freq * MHZ_KHZ_CONVERSION;
    wwan_state_start = wwan_state_stop = wwan_band->frequency;
  }
  else
  {
    /*get the ranges from the wwan and wlan state infos in Hz*/
    wcn_state_start = (wcn_band->freq - wcn_band->bandwidth/2) * MHZ_KHZ_CONVERSION;
    wcn_state_stop  = (wcn_band->freq + wcn_band->bandwidth/2) * MHZ_KHZ_CONVERSION;
    wwan_state_start = (wwan_band->frequency) - wwan_band->bandwidth/(2 * KHZ_HZ_CONVERSION);
    wwan_state_stop  = (wwan_band->frequency) + wwan_band->bandwidth/(2 * KHZ_HZ_CONVERSION);
  }
  /* Use the GSM victim table entries for GSM2 and GSM3 */
  if ( wwan_tech == CXM_TECH_GSM2 || wwan_tech == CXM_TECH_GSM3 )
  {
    wwan_tech = CXM_TECH_GSM1;
  }

  for ( i = 0; i < coex_params.num_conflicts; i++ )
  {
    /* determine the overlap for WWAN */
    curr_wwan_overlap = (int64)COEX_MIN( coex_params.victim_tbl[i].wwan.freq_stop, wwan_state_stop ) - 
                        (int64)COEX_MAX( coex_params.victim_tbl[i].wwan.freq_start, wwan_state_start );
    /* check other conditions for WWAN conflict */
    if ( (coex_wwan_tech_v01)wwan_tech == coex_params.victim_tbl[i].wwan.tech && 
          wwan_band->direction & coex_params.victim_tbl[i].wwan.direction &&
         ( (wwan_tech !=  CXM_TECH_LTE) || 
           (wwan_tech ==  CXM_TECH_LTE && (wwan_op_dim & coex_params.victim_tbl[i].wwan.operating_dim)) ) &&
         curr_wwan_overlap > 0  )
    {
      curr_conf_prio = (uint64)(curr_wwan_overlap & 0x3FFFFFFFFFFFFFFF);
      /*check if center freq match on WWAN tech*/
      if( coex_params.victim_tbl[i].wwan.freq_start >= wwan_band->frequency &&
          coex_params.victim_tbl[i].wwan.freq_stop <= wwan_band->frequency )
      {
        curr_conf_prio = (curr_conf_prio | 0x4000000000000000);
      }
      /*check if UL/DL vs DL only on WWAN tech*/ 
      if( wwan_band->direction & CXM_LNK_DRCTN_UL )
      {
        curr_conf_prio = (curr_conf_prio | 0x8000000000000000);
      }
      /*we possibly have a conflict, now check if the wlan state is conflicting*/
      CXM_MSG_4( MED, "possible conflict, wwan_freq_start: %d, wwan_freq_stop: %d, table_index: %d,"
                      "conf_prio: %x", wwan_state_start, wwan_state_stop, i, curr_conf_prio );
      /* Always do Complete Overlap for WCN */
      if( (coex_wcn_tech_v01)wcn_tech == coex_params.victim_tbl[i].wcn.tech &&
          ( wcn_mode & coex_params.victim_tbl[i].wcn.mode ) &&
            wcn_state_start >= coex_params.victim_tbl[i].wcn.freq_start &&
            wcn_state_stop <= coex_params.victim_tbl[i].wcn.freq_stop )
      {
        CXM_MSG_3( MED, "found conflict, wlan_freq_start: %d, wlan_freq_stop: %d, table_index: %d, ",
                   wcn_state_start, wcn_state_stop, i );
        /* determine if this conflict has the max over lap */
        if ( curr_conf_prio > *conf_prio )
        {
          *conf_prio = curr_conf_prio;
          *conf_prio_id = i;
        }
      }
    }
  }

  return;
}

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
)
{
  coex_wlan_conn_node_type *conn_node = coex_wlan_conn_head;
  coex_wlan_hp_node_type *hp_node = coex_wlan_hp_head;
  boolean                  new_conflict = FALSE;
  coex_wwan_operating_dimension_v01 wwan_op_dim = 0;
  uint8                     i, j;
  uint8                     run_conf_prio_id = 0;
  uint8                     max_conf_prio_vctbl_idx = 0;
  uint64                    max_conf_prio = 0;
  uint64                    run_conf_prio = 0;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( HIGH, "Processing possible conflicts, tech=%d", wwan_tech );

  /* if control plane is done over WCI-2, need to use an alternate form
   * of process_conflicts(v2) that reads the new data structures */
  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_CONTROL) )
  {
    coex_process_conflicts_v2( wwan_tech, wcn_tech, &max_conf_prio, &max_conf_prio_vctbl_idx );
  }
  else /* control plane over QMI, use QMI v01 data structures */
  {
    /*Search through state data to find conflicts related to WIFI or WWAN */
    /* determine the operating dimension */
    if ( wcn_tech == CXM_TECH_WIFI )
    {
      if ( wwan_tech == CXM_TECH_LTE )
      {
        wwan_op_dim = coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.frame_structure == 
                    LTE_L1_FRAME_STRUCTURE_FS2 ? COEX_TECH_OPERATING_DIMENSION_TDD_V01 : 
                    COEX_TECH_OPERATING_DIMENSION_FDD_V01;
      }
      for ( i = 0; i <coex_wwan_state_info[wwan_tech].state_data.num_link_info_sets; i++) 
      {
        /* search through WIFI high prio nodes */
        while( hp_node != NULL )
        {
           coex_resolve_conflict(wwan_tech, CXM_TECH_WIFI,
             &coex_wwan_state_info[wwan_tech].state_data.link_list[i],
             &hp_node->high_prio.band, CXM_WLAN_HIGH_PRIO_TYPE, wwan_op_dim,
             &run_conf_prio, &run_conf_prio_id );
           if ( run_conf_prio > max_conf_prio )
           {
             max_conf_prio = run_conf_prio;
             max_conf_prio_vctbl_idx = run_conf_prio_id;
           }
           run_conf_prio = 0;
           hp_node = hp_node->next;
        }

        /* if WIFI high prio conflict found, go with it */
        if( max_conf_prio > 0 )
        {
          new_conflict = TRUE;
        }

        /* search through WIFI connection nodes */
        while( (conn_node != NULL) && (!new_conflict) )
        {
          for ( j = 0; j < conn_node->conn.band_len; j++ )
          {
             coex_resolve_conflict(wwan_tech, CXM_TECH_WIFI,
               &coex_wwan_state_info[wwan_tech].state_data.link_list[i],
               &conn_node->conn.band[j], CXM_WLAN_CONN_TYPE, wwan_op_dim, 
               &run_conf_prio, &run_conf_prio_id );
             if ( run_conf_prio > max_conf_prio )
             {
               max_conf_prio = run_conf_prio;
               max_conf_prio_vctbl_idx = run_conf_prio_id;
             }
             run_conf_prio = 0;
          }
          conn_node = conn_node->next;
        }

        /* return wlan node positions to the beginning for next wwan band */
        conn_node = coex_wlan_conn_head;
        hp_node = coex_wlan_hp_head;
      }
    }
  }

  /* now that the conflict has been determined (if any) send appropriate policy */
  if ( max_conf_prio > 0 )
  {
    /* update our return value to show that a conflict has been found */
    new_conflict = TRUE;
    /* Set global policy info */
    coex_wwan_state_info[wwan_tech].policy = 
      (cxm_policy_t)coex_params.victim_tbl[max_conf_prio_vctbl_idx].mdm_policy;
    /* keep track of the active conflict because we need to check RBs and Power thresholds
        before asserting TX active */
    coex_wwan_state_info[wwan_tech].active_conf_index = max_conf_prio_vctbl_idx;
    CXM_MSG_2( HIGH, "max conflict index = %d, with conflict priority = %x",
               max_conf_prio_vctbl_idx, max_conf_prio );
    CXM_TRACE(CXM_TRC_COEX_CONFLICT_START, 
               max_conf_prio_vctbl_idx, (uint16)((max_conf_prio&0xC000000000000000)>>48),
              (uint32)(max_conf_prio&0x00000000FFFFFFFF), CXM_TRC_EN);

    /* Set policy state */
    if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) ||
         COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_CONTROL) )
    {
      coex_manage_uart_npa_vote();
    }
    coex_prep_and_send_policy_msg( wwan_tech );

    /* If power limiting policy is ON, set the TX power limit */
    if ( (coex_params.victim_tbl[max_conf_prio_vctbl_idx].mdm_policy & CXM_POLICY_ENFORCE_CTRLR_TX_PWR_LMT) && 
         wwan_tech == CXM_TECH_LTE )
    {
      coex_wwan_state_info[wwan_tech].plcy_parms.params.lte.controller_tx_power_limit = 
        coex_params.victim_tbl[max_conf_prio_vctbl_idx].mdm_params.pwr_lmt.power;

      /* If power limiting has been OFF, check power limiting conditions */
      if ( coex_controller_pwr_lmt_state == COEX_PWR_LMT_OFF ) 
      {
        /* controller power limiting now enforced. First, set the power limiting
           conditions */
        coex_plcy_parms.link_path_loss_threshold = coex_params.link_path_loss_threshold;
        coex_plcy_parms.filtered_rb_threshold = coex_params.filtered_rb_threshold;

        coex_controller_pwr_lmt_state = COEX_PWR_LMT_PENDING;
        coex_check_power_limiting_conditions();
        cxm_counter_event( CXM_CNT_CONTR_PWR_LMT_REQ, 
                           coex_wwan_state_info[wwan_tech].plcy_parms.params.lte.controller_tx_power_limit );
      }
    }
    /* If power limiting policy is OFF, set the state */
    else
    {
      coex_controller_pwr_lmt_state = COEX_PWR_LMT_OFF;
    }

    /* Power limit states may have changed. Check if power limit needs to be set. 
       In this case, we don't need to check power limiting conditions */
    coex_check_and_set_power_limit();
  }

  if ( coex_wwan_state_info[wwan_tech].policy_state == COEX_POLICY_INACTIVE
       && new_conflict )
  {
    coex_wwan_state_info[wwan_tech].policy_state = COEX_POLICY_ACTIVE;
  }
  else if ( coex_wwan_state_info[wwan_tech].policy_state == COEX_POLICY_ACTIVE
            && !new_conflict )
  {
    /* Turn power limiting OFF if it is not already OFF */
    if( (coex_wwan_state_info[wwan_tech].policy & CXM_POLICY_ENFORCE_CTRLR_TX_PWR_LMT) && 
        wwan_tech == CXM_TECH_LTE )
    {
      coex_controller_pwr_lmt_state = COEX_PWR_LMT_OFF;
      coex_check_and_set_power_limit();
    }
    /* Set global policy info */
    coex_wwan_state_info[wwan_tech].policy = CXM_POLICY_NONE;
    coex_wwan_state_info[wwan_tech].policy_state = COEX_POLICY_INACTIVE;
    if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) ||
         COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_CONTROL) ) 
    {
      coex_manage_uart_npa_vote();
    }
    coex_prep_and_send_policy_msg( wwan_tech );

    CXM_MSG_0( HIGH, "conflict end" );
    CXM_TRACE( CXM_TRC_COEX_CONFLICT_END,
                   0, /* pointer to existing conflict? */
                   0, 0, CXM_TRC_EN );

  }

  return;
}
#endif

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
)
{
  /*-----------------------------------------------------------------------*/
  coex_wwan_state_info[CXM_TECH_LTE].policy = (coex_policy_config_mask_v01) policy;
  coex_prep_and_send_policy_msg( CXM_TECH_LTE );

  return;
}  /* coex_send_diag_policy */

/*=============================================================================

  FUNCTION:  coex_prep_and_send_wwan_state_ind

=============================================================================*/
/*!
    @brief
    Function to prepare & send the WWAN STATE info INDICATION message

    @detail
    This function currently is can be called from either within the actual
    send out IND message or as a follow on to the enable REQ for the WWAN
    STATE info IND by any client
 
    @return
    qmi_csi_error
*/
/*===========================================================================*/
static qmi_csi_error coex_prep_and_send_wwan_state_ind (
   boolean always_send
)
{
  qmi_csi_error               retval = QMI_CSI_NO_ERR;
  errno_enum_type             temp_retval = E_SUCCESS;
  /* static variables initialized to first time */
  /*used to determine if lte has just become active*/
  static boolean              prev_lte_state_inactive = TRUE;
  coex_wwan_state_ind_msg_v01 indication;
  /*store the old_indication to know if message has changed*/
  static coex_wwan_state_ind_msg_v01 old_indication;
  uint8                       i, band_index;
  /*-----------------------------------------------------------------------*/
  /* Reset the memory available for the IND message */
  memset( &indication, 0, sizeof( coex_wwan_state_ind_msg_v01 ) );

  /* Setting up the optional fields valid flag explicitly to FALSE
     so they are not included in QMI message unless they are needed.
     Fields that are needed will be set to TRUE before sending out the 
     QMI message. */
  indication.lte_band_info_valid         = FALSE;
  indication.lte_tdd_info_valid          = FALSE;
  indication.lte_off_period_valid        = FALSE;
  indication.lte_band_info_set_valid     = FALSE;
  indication.tdscdma_band_info_set_valid = FALSE;
  indication.gsm_band_info_set_valid     = FALSE;
  indication.hdr_band_info_set_valid     = FALSE;
  indication.wcdma_band_info_set_valid   = FALSE;
  indication.onex_band_info_set_valid    = FALSE;

  /*if band information is being sent for this tech, it is valid to add it to the QMI message*/
  if (coex_wwan_state_info[CXM_TECH_LTE].state_data.num_link_info_sets != 0) 
  {  
    /* fill in the old lte_band_info message to support backwards compatibility */
    for (i=0; i< coex_wwan_state_info[CXM_TECH_LTE].state_data.num_link_info_sets; i++) 
    {
      /*if an entry in our state array matches UL and we havent already 
        encountered one that does, fill in the ul lte info */
      if ( indication.lte_band_info.ul_band.freq==0 &&
           coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].direction == CXM_LNK_DRCTN_UL && 
           (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_LTE].tech_band_filter[i] == TRUE) ) 
      {
        /*old bandwidth is in Mhz*/
        indication.lte_band_info.ul_band.bandwidth = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].bandwidth / MHZ_HZ_CONVERSION;
        indication.lte_band_info.ul_band.freq = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].frequency / MHZ_KHZ_CONVERSION;
      }
      /*if an entry in our state array matches DL and we havent already 
        encountered one that does, fill in the dl lte info*/
      if ( indication.lte_band_info.dl_band.freq==0 &&
           coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].direction == CXM_LNK_DRCTN_DL &&
           (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_LTE].tech_band_filter[i] == TRUE)) 
      {
        indication.lte_band_info.dl_band.bandwidth = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].bandwidth / MHZ_HZ_CONVERSION;
        indication.lte_band_info.dl_band.freq = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].frequency / MHZ_KHZ_CONVERSION;
      }
    }

    /*Fill in the LTE band array*/
    band_index = 0;
    /*Fill in the LTE band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_LTE].state_data.num_link_info_sets && band_index < COEX_WWAN_MAX_BANDS_PER_TECH_V01; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_LTE].tech_band_filter[i] == TRUE)
      {
         indication.lte_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].bandwidth;
         indication.lte_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].frequency;
         indication.lte_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      indication.lte_band_info_valid = TRUE;
      indication.lte_band_info_set_valid = TRUE;
      indication.lte_band_info_set_len = band_index; 
    }

    /*Check if we are in LTE TDD and fill in the appropriate fields. If all band 
     * information has been filtered out (i.e. length is 0), dont send out other LTE params*/
    if ((coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.frame_structure == 
         LTE_L1_FRAME_STRUCTURE_FS2) &&
       (indication.lte_band_info_set_len > 0))
    {
        /* Setup the optional LTE TDD info valid flag */
        indication.lte_tdd_info_valid = TRUE;

        /* Update LTE TDD frame offset info (if provided) */
        indication.lte_tdd_info.frame_offset = 
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.frame_offset;

        /* Update LTE TDD config (if provided) */
        indication.lte_tdd_info.tdd_config = (coex_lte_tdd_config_enum_v01)
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.tdd_config;

        /* Update LTE TDD SSP (if provided) */
        indication.lte_tdd_info.subframe_config = (coex_lte_tdd_subframe_config_enum_v01)
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.ssp;

        /* Update LTE TDD DL CP config (if provided) */
        indication.lte_tdd_info.dl_config = (coex_lte_tdd_link_config_enum_v01)
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.dl_cp;

        /* Update LTE TDD UL CP config (if provided) */
        indication.lte_tdd_info.ul_config = (coex_lte_tdd_link_config_enum_v01)
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.ul_cp;
    }

    /* if lte comes up for first time: 
      - resend policy msg 
      - check if power limit changed */
    if( prev_lte_state_inactive )
    {
      prev_lte_state_inactive = FALSE;
      temp_retval = coex_prep_and_send_policy_msg( CXM_TECH_LTE );
      CXM_ASSERT( temp_retval == E_SUCCESS );

      /* Set current tx power limit (if any) */
      coex_check_and_set_power_limit();

      if( ( coex_controller_pwr_lmt_state == COEX_PWR_LMT_PENDING ) ||
          ( coex_wci2_pwr_lmt_state == COEX_PWR_LMT_PENDING ) )
      {
        coex_check_power_limiting_conditions();
      }
    }


  } /*end of 'if lte is valid'*/
  else
  {
    prev_lte_state_inactive = TRUE;
  }

  if (coex_wwan_state_info[CXM_TECH_TDSCDMA].state_data.num_link_info_sets != 0) 
  {  
    band_index = 0;
    /*Fill in the TDS band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_TDSCDMA].state_data.num_link_info_sets && band_index < COEX_WWAN_MAX_BANDS_PER_TECH_V01; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_TDSCDMA].tech_band_filter[i] == TRUE)
      {
         indication.tdscdma_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_TDSCDMA].state_data.link_list[i].bandwidth;
         indication.tdscdma_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_TDSCDMA].state_data.link_list[i].frequency;
         indication.tdscdma_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_TDSCDMA].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      indication.tdscdma_band_info_set_valid = TRUE;
      indication.tdscdma_band_info_set_len = band_index; 
    }

  } /*end of if TDS is valid*/

  if( coex_wwan_state_info[CXM_TECH_GSM1].state_data.num_link_info_sets != 0 )
  {  
    band_index = 0;
    /*Fill in the GSM1 band array*/
    for (i=0; i < coex_wwan_state_info[CXM_TECH_GSM1].state_data.num_link_info_sets && band_index < COEX_WWAN_MAX_BANDS_PER_TECH_V01; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_GSM1].tech_band_filter[i] == TRUE)
      {
        indication.gsm_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_GSM1].state_data.link_list[i].bandwidth;
        indication.gsm_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_GSM1].state_data.link_list[i].frequency;
        indication.gsm_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_GSM1].state_data.link_list[i].direction;
        band_index++;
      }
    }

    if (band_index > 0) 
    {
      indication.gsm_band_info_set_valid = TRUE;
      indication.gsm_band_info_set_len = band_index; 
    }
  } /*end of if GSM1 is valid*/

  if( coex_wwan_state_info[CXM_TECH_GSM2].state_data.num_link_info_sets != 0 )
  {  
    band_index = 0;
    /*Fill in the GSM2 band array*/
    for (i=0; i < coex_wwan_state_info[CXM_TECH_GSM2].state_data.num_link_info_sets && band_index < COEX_WWAN_MAX_BANDS_PER_TECH_V01; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_GSM2].tech_band_filter[i] == TRUE)
      {  
        indication.gsm2_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_GSM2].state_data.link_list[i].bandwidth;
        indication.gsm2_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_GSM2].state_data.link_list[i].frequency;
        indication.gsm2_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_GSM2].state_data.link_list[i].direction;
        band_index++;
      }
    }

    if (band_index > 0) 
    {
      indication.gsm2_band_info_set_valid = TRUE;
      indication.gsm2_band_info_set_len = band_index; 
    }
  } /*end of if GSM2 is valid*/

  if( coex_wwan_state_info[CXM_TECH_GSM3].state_data.num_link_info_sets != 0 )
  {  
    band_index = 0;
    /*Fill in the GSM3 band array*/
    for (i=0; i < coex_wwan_state_info[CXM_TECH_GSM3].state_data.num_link_info_sets && band_index < COEX_WWAN_MAX_BANDS_PER_TECH_V01; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_GSM3].tech_band_filter[i] == TRUE)
      {
        indication.gsm3_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_GSM3].state_data.link_list[i].bandwidth;
        indication.gsm3_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_GSM3].state_data.link_list[i].frequency;
        indication.gsm3_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_GSM3].state_data.link_list[i].direction;
        band_index++;
      }
    }

    if (band_index > 0) 
    {
      indication.gsm3_band_info_set_valid = TRUE;
      indication.gsm3_band_info_set_len = band_index; 
    }
  } /*end of if GSM3 is valid*/

  if (coex_wwan_state_info[CXM_TECH_ONEX].state_data.num_link_info_sets != 0) 
  {  
    band_index = 0;
    /*Fill in the ONEX band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_ONEX].state_data.num_link_info_sets && band_index < COEX_WWAN_MAX_BANDS_PER_TECH_V01; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_ONEX].tech_band_filter[i] == TRUE)
      {
         indication.onex_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_ONEX].state_data.link_list[i].bandwidth;
         indication.onex_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_ONEX].state_data.link_list[i].frequency;
         indication.onex_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_ONEX].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      indication.onex_band_info_set_valid = TRUE;
      indication.onex_band_info_set_len = band_index; 
    }

  } /*end of if ONEX is valid*/

  if (coex_wwan_state_info[CXM_TECH_HDR].state_data.num_link_info_sets != 0) 
  {  
    band_index = 0;
    /*Fill in the HDR band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_HDR].state_data.num_link_info_sets && band_index < COEX_WWAN_MAX_BANDS_PER_TECH_V01; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_HDR].tech_band_filter[i] == TRUE)
      {
         indication.hdr_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_HDR].state_data.link_list[i].bandwidth;
         indication.hdr_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_HDR].state_data.link_list[i].frequency;
         indication.hdr_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_HDR].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      indication.hdr_band_info_set_valid = TRUE;
      indication.hdr_band_info_set_len = band_index; 
    }

  } /*end of if HDR is valid*/

  if (coex_wwan_state_info[CXM_TECH_WCDMA].state_data.num_link_info_sets != 0) 
  {  
    band_index = 0;
    /*Fill in the WCDMA band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_WCDMA].state_data.num_link_info_sets && band_index < COEX_WWAN_MAX_BANDS_PER_TECH_V01; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_WCDMA].tech_band_filter[i] == TRUE)
      {
         indication.wcdma_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_WCDMA].state_data.link_list[i].bandwidth;
         indication.wcdma_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_WCDMA].state_data.link_list[i].frequency;
         indication.wcdma_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_WCDMA].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      indication.wcdma_band_info_set_valid = TRUE;
      indication.wcdma_band_info_set_len = band_index; 
    }

  } /*end of if WCDMA is valid*/

  /*check if the indication is different from the last one*/
  if( always_send || memcmp( &indication, &old_indication, sizeof(indication) ) )
  {
    old_indication = indication;
    /*Send the message via QMI*/
    retval = mqcsi_conn_mgr_send_ind (
               MQCSI_COEX_SERVICE_ID,
               QMI_COEX_WWAN_STATE_IND_V01,
               &indication,
               sizeof ( coex_wwan_state_ind_msg_v01 )
             );

    CXM_TRACE( CXM_TRC_QMI_SND_WWAN_STATE_IND, retval, 0, 0,
               CXM_TRC_EN );

    CXM_MSG_1( HIGH, "Send WWAN state IND return %d ", retval );
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_throttle_condition_ind_masks

=============================================================================*/
/*!
    @brief
    Helper function to check if a QMI condition mask (for condition fail 
    or condition success indications) needs to be throttled or should be sent.

    @detail
    Only send the IND if it hasn't been sent within 1 second (throttled to 
    prevent memory depletion from sending too often

    @return throttle-adjusted mask, drop_count
*/
/*===========================================================================*/
uint64 coex_throttle_condition_ind_masks (
  coex_throttle_count_s *conditions, /* array of conditions for this mask */
  int                    num_conditions,
  uint64                 mask,
  uint32                *drop_count /* output - total drops in last second */
)
{
  timetick_type curr_time, time_elapsed;
  uint64        mask_to_send = 0;
  int           i;
  /*-----------------------------------------------------------------------*/
  /* check if we have to throttle (if timestamp is older than throttling 
   * period */
  CXM_ASSERT( NULL != drop_count && NULL != conditions );
  *drop_count = 0;

  curr_time = timetick_get();

  /* check each bit in the mask in turn */
  for( i = 0; i < num_conditions; i++ )
  {
    /* check if this condition bit is set in the mask */
    if( 0x01 & (mask >> i) )
    {
      /* bit set. check timestamp for last time this condition was
       * sent out to see if we need to throttle */
      time_elapsed = timetick_diff( conditions[i].last_sent_ts,
                                    curr_time, T_MSEC );
      if( time_elapsed > COEX_THROTTLE_QMI_CONDITION_IND_PERIOD_MS )
      {
        /* don't throttle. Send this condition and reset info */
        conditions[i].last_sent_ts = curr_time;
        conditions[i].drop_count   = 0;
        mask_to_send |= (0x01 << i);
      }
      else
      {
        /* this condition has been sent out before in this throttling
         * period, so don't send this one */
        conditions[i].drop_count++;
        *drop_count += conditions[i].drop_count;
      }

      CXM_MSG_2( LOW, "Condition IND mask bit %d: drop_cnt %d", 
                 i, conditions[i].drop_count );
    }
  }

  return mask_to_send;
}

/*=============================================================================

  FUNCTION:  coex_prep_and_send_condition_fail_ind

=============================================================================*/
/*!
    @brief
    Check if appropriate to send a QMI condition fail IND, and send it if so.

    @detail
    Only send the IND if it hasn't been sent within 1 second (throttled to 
    prevent memory depletion from sending too often

    @return void
*/
/*===========================================================================*/
void coex_prep_and_send_condition_fail_ind (
  coex_txfrmdnl_condition_failure_mask_v01 tx_subframe_denials_cond,
  coex_pwrlmt_condition_fail_mask_v01      controller_tx_pwrlmt_fail_cond,
  coex_pwrlmt_condition_fail_mask_v01      wci2_tx_pwrlmt_fail_cond
)
{
  coex_condition_fail_ind_msg_v01 fail_indication;
  qmi_csi_error                   qmi_retval = QMI_CSI_NO_ERR;
  boolean                         send_ind = FALSE;
  uint32                          drop_count;
  uint64                          mask_to_send = 0;
  /*-----------------------------------------------------------------------*/
  /* Reset the memory available for the IND message */
  memset( &fail_indication, 0, sizeof(coex_condition_fail_ind_msg_v01) );

  /* for each of three masks: if mask is empty, we don't need to worry 
   * about filling out or sending it in this indication */
  if( tx_subframe_denials_cond )
  {
    /* check if this is a unique failure in this throttling period */
    mask_to_send = coex_throttle_condition_ind_masks(
                      coex_throttle_ind_info.wcn_prio_fail,
                      COEX_WCN_PRIO_COND_FAIL_MASK_LEN,
                      tx_subframe_denials_cond,
                      &drop_count );

    if( mask_to_send )
    {
      /* this failure bit has not been set yet in this throttling period;
       * fill out the field in the indication to send */
      send_ind = TRUE;
      fail_indication.tx_subframe_denials_status_valid = TRUE;
      fail_indication.tx_subframe_denials_status = mask_to_send;
    }

    CXM_MSG_3( HIGH, "WCN Prio condition fail: this_mask=%x sent_mask=%x drop_cnt=%d",
               tx_subframe_denials_cond, mask_to_send, drop_count );

    CXM_TRACE( CXM_TRC_QMI_TX_SUBFRAME_DENIAL,
               tx_subframe_denials_cond,
               fail_indication.tx_subframe_denials_status,
               drop_count, CXM_TRC_EN );
  }

  if( controller_tx_pwrlmt_fail_cond )
  {
    /* check if this is a unique failure in this throttling period */
    mask_to_send = coex_throttle_condition_ind_masks(
                      coex_throttle_ind_info.contr_pwrlmt_fail,
                      COEX_CONTR_PWRLMT_COND_FAIL_MASK_LEN,
                      controller_tx_pwrlmt_fail_cond,
                      &drop_count );

   if( mask_to_send )
    {
      /* this failure bit has not been set yet in this throttling period;
       * fill out the field in the indication to send */
      send_ind = TRUE;
      fail_indication.controller_tx_pwrlmt_fail_cond_valid = TRUE;
      fail_indication.controller_tx_pwrlmt_fail_cond = mask_to_send;
    }

    CXM_TRACE( CXM_TRC_QMI_LTE_TX_PWR_LMT_FAIL,
               controller_tx_pwrlmt_fail_cond, 0, drop_count,
               CXM_TRC_EN );

    cxm_counter_event( CXM_CNT_CONTR_PWR_LMT_COND_FAIL, 
                       (uint32) controller_tx_pwrlmt_fail_cond );

    CXM_MSG_3( HIGH, "Contr Pwrlmt condition fail: this_mask=%x sent_mask=%x drop_cnt=%d", 
               controller_tx_pwrlmt_fail_cond, mask_to_send, drop_count );
  }

  if( wci2_tx_pwrlmt_fail_cond )
  {
    /* check if this is a unique failure in this throttling period */
    mask_to_send = coex_throttle_condition_ind_masks(
                      coex_throttle_ind_info.wci2_pwrlmt_fail,
                      COEX_WCI2_PWRLMT_COND_FAIL_MASK_LEN,
                      wci2_tx_pwrlmt_fail_cond,
                      &drop_count );

    if( mask_to_send )
    {
      /* this failure bit has not been set yet in this throttling period;
       * fill out the field in the indication to send */
      send_ind = TRUE;
      fail_indication.wci2_tx_pwrlmt_fail_cond_valid = TRUE;
      fail_indication.wci2_tx_pwrlmt_fail_cond = mask_to_send;
    }

    CXM_TRACE( CXM_TRC_QMI_LTE_TX_PWR_LMT_FAIL,
               0, wci2_tx_pwrlmt_fail_cond, drop_count, CXM_TRC_EN );

    cxm_counter_event( CXM_CNT_WCI2_PWR_LMT_COND_FAIL, 
                       (uint32) wci2_tx_pwrlmt_fail_cond );

    CXM_MSG_3( HIGH, "Wci2 Pwrlmt condition fail: this_mask=%x sent_mask=%x drop_cnt=%d", 
               wci2_tx_pwrlmt_fail_cond, mask_to_send, drop_count );
  }

  /* if indication changed from last time or it's been more than 1 second since
   * the last time the indication was sent, send this one */
  if( send_ind )
  {
    qmi_retval = mqcsi_conn_mgr_send_ind (
                   MQCSI_COEX_SERVICE_ID,
                   QMI_COEX_CONDITION_FAIL_IND_V01,
                   &fail_indication,
                   sizeof ( coex_condition_fail_ind_msg_v01 )
                 );
    CXM_ASSERT( QMI_CSI_NO_ERR == qmi_retval );
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_prep_and_send_condition_success_ind

=============================================================================*/
/*!
    @brief
    Check if appropriate to send a QMI condition succ IND, and send it if so.

    @detail
    Only send the IND if it hasn't been sent within 1 second (throttled to 
    prevent memory depletion from sending too often

    @return void
*/
/*===========================================================================*/
void coex_prep_and_send_condition_success_ind (
  coex_pwrlmt_condition_success_mask_v01 tx_pwrlmt_success_case
)
{
  coex_condition_success_ind_msg_v01 succ_indication;
  qmi_csi_error                      qmi_retval = QMI_CSI_NO_ERR;
  uint32                             drop_count;
  uint64                             mask_to_send;
  /*-----------------------------------------------------------------------*/
  if( tx_pwrlmt_success_case )
  {
    /* filter mask of all 'throttled' bits */
    mask_to_send = coex_throttle_condition_ind_masks(
                      coex_throttle_ind_info.pwrlmt_success,
                      COEX_PWRLMT_COND_SUCCESS_MASK_LEN,
                      tx_pwrlmt_success_case,
                      &drop_count );

    if( mask_to_send )
    {
      /* message not restricted from throttling, send it out */
      succ_indication.tx_pwrlmt_success_case_valid = TRUE;
      succ_indication.tx_pwrlmt_success_case = mask_to_send;

      qmi_retval = mqcsi_conn_mgr_send_ind (
                     MQCSI_COEX_SERVICE_ID,
                     QMI_COEX_CONDITION_SUCCESS_IND_V01,
                     &succ_indication,
                     sizeof ( coex_condition_success_ind_msg_v01 )
                   );
      CXM_ASSERT( QMI_CSI_NO_ERR == qmi_retval );
    }

    CXM_TRACE( CXM_TRC_QMI_LTE_TX_PWR_LMT_SUCC,
               tx_pwrlmt_success_case,
               mask_to_send,
               drop_count,
               CXM_TRC_EN );

    CXM_MSG_3( HIGH, "Power limit cond succ: real_mask=%x sent_mask=%x drop_cnt=%d", 
               tx_pwrlmt_success_case, mask_to_send, drop_count );
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_apply_tech_band_filter

=============================================================================*/
/*!
    @brief
    Function to apply band filtering for a specified tech

    @detail
    This function is called when a new tech update comes in or band filtering
    has just been set.
 
    @return
    void
*/
/*===========================================================================*/
void coex_apply_tech_band_filter (
   cxm_tech_type tech_id
)
{
  uint8 i,j;
  /*-----------------------------------------------------------------------*/
  /*if band filtering is ON, run the filter*/
  if (coex_band_filter_info.filter_on) 
  {
    for (i=0; i < coex_wwan_state_info[tech_id].state_data.num_link_info_sets; i++) 
    {

      /*if it is an UL band perform filtering with the UL filter*/
      if (coex_wwan_state_info[tech_id].state_data.link_list[i].direction & CXM_LNK_DRCTN_UL )
      {
        /*if there is no band filter in this direction, set filter to true*/
        if ( coex_band_filter_info.ul_len == 0 ) 
        {
          coex_wwan_state_info[tech_id].tech_band_filter[i] = FALSE;
        }

        for (j=0; j<coex_band_filter_info.ul_len; j++) 
        {
          if (coex_wwan_state_info[tech_id].state_data.link_list[i].frequency >= coex_band_filter_info.ul_filter[j].freq_start
              && coex_wwan_state_info[tech_id].state_data.link_list[i].frequency <= coex_band_filter_info.ul_filter[j].freq_stop) 
          {
            coex_wwan_state_info[tech_id].tech_band_filter[i] = TRUE;
            break;
          }
          else
          {
            coex_wwan_state_info[tech_id].tech_band_filter[i] = FALSE;
          }
        }
      } /* end UL filtering */

      /*if it is a DL band perform filtering with the DL filter*/
      if ( coex_wwan_state_info[tech_id].state_data.link_list[i].direction & CXM_LNK_DRCTN_DL )
      {
        /*if there is no band filter in this direction, set filter to true*/
        if ( coex_band_filter_info.dl_len == 0 ) 
        {
          coex_wwan_state_info[tech_id].tech_band_filter[i] = FALSE;
        }

        for (j=0; j<coex_band_filter_info.dl_len; j++) 
        {
          if (coex_wwan_state_info[tech_id].state_data.link_list[i].frequency >= coex_band_filter_info.dl_filter[j].freq_start
              && coex_wwan_state_info[tech_id].state_data.link_list[i].frequency <= coex_band_filter_info.dl_filter[j].freq_stop) 
          {
            coex_wwan_state_info[tech_id].tech_band_filter[i] = TRUE;
            break;
          }
          else
          {
            coex_wwan_state_info[tech_id].tech_band_filter[i] = FALSE;
          }
        }
      } /* end DL filtering */

    }
  }

  return;
} /* coex_apply_tech_band_filter */

/*=============================================================================

  FUNCTION:  coex_prep_and_send_wcn_wake_sync_ind

=============================================================================*/
/*!
    @brief
    Function to prepare & send the WCN wake sync INDICATION message to 
    synchronize the WWAN and WCN page wakeups

    @detail
    This function first finds the common interval between the two page
    intervals, checks timing, and sends out the indication if the
    conditions are appropriate.
 
    @return
    qmi_csi_error
*/
/*===========================================================================*/
static qmi_csi_error coex_prep_and_send_wcn_wake_sync_ind (
  uint32 wwan_page_cycle
)
{
  qmi_csi_error                  retval = QMI_CSI_NO_ERR;
  DALResult                      DAL_retval = DAL_SUCCESS;
  coex_wcn_wake_sync_ind_msg_v01 indication;
  int                            WCN_i, WWAN_i;
  int32                          time_elapsed, interval_error;
  uint64                         timestamp;
  /*-----------------------------------------------------------------------*/
  /* find the common interval at which to send the indication from
   * the lookup table */

  /* find WCN index into table */
  for( WCN_i = 0; WCN_i < COEX_MAX_WCN_WAKE_INTERVALS; WCN_i++ )
  {
    if( coex_wcn_wake_intervals[WCN_i] == coex_wcn_wake_sync_info.scan_interval )
    {
      break;
    }
  }
  /* find WWAN index into table */
  for( WWAN_i = 0; WWAN_i < COEX_MAX_WWAN_WAKE_INTERVALS; WWAN_i++ )
  {
    if( coex_wwan_wake_intervals[WWAN_i] == wwan_page_cycle )
    {
      break;
    }
  }
  /* use both indices to find the common interval; else use 0 */
  if( WCN_i  < COEX_MAX_WCN_WAKE_INTERVALS &&
      WWAN_i < COEX_MAX_WWAN_WAKE_INTERVALS )
  {
    coex_wcn_wake_sync_info.common_interval = coex_wcn_wake_interval_LUT[WWAN_i][WCN_i];
  }
  else
  {
    coex_wcn_wake_sync_info.common_interval = 0;
  }

  CXM_MSG_3( MED, "wake_sync, intervals: lte=%d bt=%d common=%d", 
             wwan_page_cycle, coex_wcn_wake_sync_info.scan_interval,
             coex_wcn_wake_sync_info.common_interval );

  /* based on the common interval and the last time we sent an indication,
   * decide whether or not to send one now. We will send one if one of the
   * following cases occurs:
   *   1) It has been ~ <common_interval> since the last time we sent an
   *      indication (+/- a small interval error threshold)
   *   2) It has been more than MAX_TIME_THRESHOLD since we last sent one */

  /* query DAL to get the Qtimer timestamp */
  DAL_retval = DalTimetick_GetTimetick64( coex_DAL_handle, 
                                          (DalTimetickTime64Type *) &timestamp );
  CXM_ASSERT( DAL_SUCCESS == DAL_retval );

  /* calculate time_elapsed in Qclock timeticks (19.2MHz ticks) */
  time_elapsed = timestamp - coex_wcn_wake_sync_info.last_msg_ts;
  /* convert time_elapsed from ticks to mSec */
  time_elapsed = time_elapsed / COEX_TIMETICKS_PER_MSEC;
  interval_error = time_elapsed - coex_wcn_wake_sync_info.common_interval;
  interval_error = (interval_error < 0) ? -interval_error : interval_error;
  CXM_MSG_3( MED, "wake_sync, interval_error=%d timestamp=%d, elapsed_ms=%d",
             interval_error, timestamp, time_elapsed );
  if( ( interval_error < COEX_WCN_WAKE_MIN_TIME_THRESH ) ||
      ( time_elapsed > COEX_WCN_WAKE_MAX_TIME_THRESH ) )
  {
    /* fill in and send out indication to WCN */
    memset( &indication, 0, sizeof( coex_wcn_wake_sync_ind_msg_v01 ) );
    indication.page_interval = wwan_page_cycle;
    indication.timestamp_valid = TRUE;
    indication.timestamp = timestamp;

    retval = mqcsi_conn_mgr_send_ind (
                MQCSI_COEX_SERVICE_ID,
                QMI_COEX_WCN_WAKE_SYNC_IND_V01,
                &indication,
                sizeof ( coex_wcn_wake_sync_ind_msg_v01 )
             );

    /* reset timestamp for next cycle */
    coex_wcn_wake_sync_info.last_msg_ts = timestamp;
  }

  return retval;
}

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
)
{
  /*-----------------------------------------------------------------------*/
  /* stop power limiting */
  coex_wci2_pwr_lmt_state = COEX_PWR_LMT_OFF;
  coex_check_and_set_power_limit();

  /* send condition fail to AP */
  CXM_MSG_0( MED, "Condition fail for power limiting - wci2 timer expired" );

  coex_prep_and_send_condition_fail_ind( 0, 0, COEX_PLCFM_WCI2_TX_PWRLMT_TIMED_OUT_V01 );

  return;
}

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
)
{
  /*-----------------------------------------------------------------------*/
  CXM_MSG_1( HIGH, "WCI2 type7 timer handler for event mask %d",
             event_mask );
  /* send the type 7 */
  coex_prep_and_send_wci2_type7( event_mask, CXM_TECH_LTE );

  return;
}

/*=============================================================================

  FUNCTION:  coex_assert_lte_tx_adv

=============================================================================*/
/*!
    @brief
    CXM_SYS_BHVR_CXM_ASSERTS_TX_ADV

    @return
    void
*/
/*===========================================================================*/
void coex_assert_lte_tx_adv(
  void *rcv_msgr_msg_ptr
)
{
  cxm_coex_tech_tx_adv_ntc_ind_s *msg_ptr;
  uint32                          i;
  uint32                          tx_trigger_time;
  boolean                         rb_check_passed = FALSE;
  uint8                           active_conf_index;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_tech_tx_adv_ntc_ind_s *) rcv_msgr_msg_ptr;

  active_conf_index = coex_wwan_state_info[CXM_TECH_LTE].active_conf_index;

  CXM_MSG_5( LOW, "SFN received at time: %d, for subframe_ustmr: %d, sfn: %d, power: %d, rbs: %d", 
             COEX_READ_USTMR_TIME(), msg_ptr->params.lte.ustmr_time, msg_ptr->params.lte.sfn,
             msg_ptr->params.lte.tx_power, msg_ptr->params.lte.rbs);

  /*if this is an assert subframe, perform condition checks*/
  if (msg_ptr->params.lte.transmit)
  {
    /*find out the assert time for the tx assert*/
    tx_trigger_time = COEX_SUB_USTMR(msg_ptr->params.lte.ustmr_time, coex_adv_notifi_period);
    /*check if this notification is too late because we are too close to its assert time*/
    CXM_MSG_3( LOW, "assert trigger time for sf: %d space till transmit time: %d, required space: %d ", 
               tx_trigger_time,
               COEX_SUB_USTMR( msg_ptr->params.lte.ustmr_time, COEX_READ_USTMR_TIME() ), 
               (COEX_ADV_TIMING_TOLERANCE + coex_adv_notifi_period) );
    if( COEX_SUB_USTMR( msg_ptr->params.lte.ustmr_time, COEX_READ_USTMR_TIME() ) >= 
        (COEX_ADV_TIMING_TOLERANCE + coex_adv_notifi_period) )
    {
      coex_state_txadv_late = FALSE;
      /*perform RB checks */
      for ( i = 0; i < coex_params.victim_tbl[active_conf_index].mdm_params.tx_adv.rb_thresh_len; i++ )
      {
        if ( msg_ptr->params.lte.rbs >= coex_params.victim_tbl[active_conf_index].mdm_params.tx_adv.rb_thresh[i].start
              && msg_ptr->params.lte.rbs <= coex_params.victim_tbl[active_conf_index].mdm_params.tx_adv.rb_thresh[i].end )
        {
          rb_check_passed = TRUE;
          break;
        }
      }
      /*if both RB and power checks pass, assert the grfc and schedule the deassert*/
      CXM_MSG_3( LOW, "rb passed: %d, lte_tx_power: %d, conflict_tx_thres: %d ", 
                 rb_check_passed, msg_ptr->params.lte.tx_power, 
                 coex_params.victim_tbl[active_conf_index].mdm_params.tx_adv.tx_power_thresh );
      if ( rb_check_passed && ( msg_ptr->params.lte.tx_power >= 
           coex_params.victim_tbl[active_conf_index].mdm_params.tx_adv.tx_power_thresh ) )
      {
        /*send assert command to common FW if the line is not already high*/
        if (!coex_deassert_scheduled)
        {
          coex_trigger_grfc(TRUE, tx_trigger_time);
        }
        /*schedule the off time*/
        coex_deassert_scheduled = TRUE;
        coex_tx_deassert_time = COEX_ADD_USTMR(msg_ptr->params.lte.ustmr_time, COEX_GRFC_ASSERT_PERIOD_USTMR);
        cxm_counter_event(CXM_CNT_TX_ADV_TRANSMIT, 0);

      }
      else
      {
        coex_deassert_scheduled = FALSE;
        /*send deassert to FW*/
        coex_trigger_grfc(FALSE, tx_trigger_time);
      }
    }
    else
    {
      /*if late state is going to be assert, we make sure we know when the deassert should be*/
      coex_tx_deassert_time = COEX_ADD_USTMR(msg_ptr->params.lte.ustmr_time, COEX_GRFC_ASSERT_PERIOD_USTMR);

      /*handle late message*/
      coex_process_late_tx_adv();
    }
  }
  else if (!msg_ptr->params.lte.transmit && coex_deassert_scheduled)
  {
    /*check if you are too late to deassert on time*/
    tx_trigger_time = COEX_SUB_USTMR(coex_tx_deassert_time, coex_adv_notifi_period);
    CXM_MSG_1( LOW, "deassert trigger time check for sf: %d ", tx_trigger_time );
    if( COEX_SUB_USTMR( coex_tx_deassert_time, COEX_READ_USTMR_TIME() )
                               >= (COEX_ADV_TIMING_TOLERANCE + coex_adv_notifi_period) )
    {
      coex_state_txadv_late = FALSE;
      /*send deassert to FW if grfc is currently high*/
      if (coex_deassert_scheduled)
      {
        coex_trigger_grfc(FALSE, tx_trigger_time);
        coex_deassert_scheduled = FALSE;
      }
    }
    else
    {
      /*move out the scheduled deassert to the next subframe*/
      coex_tx_deassert_time+=COEX_GRFC_ASSERT_PERIOD_USTMR;

      /*handle late message*/
      coex_process_late_tx_adv();
    }
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_algos_wci2_dir_read_cb

=============================================================================*/
/*!
    @brief
    Called when direct read of type 0-7 is ready from UART

    @return
    void
*/
/*===========================================================================*/
void coex_algos_wci2_dir_read_cb( uint8* rx_types, uint8 status )
{
  uint8 i;
  /*-----------------------------------------------------------------------*/

  for( i = 0; i < 8; i++ )
  {
    coex_wci2_rx.bytes[i] = rx_types[i];
  }
  /* add new status bits to existing status bit mask */
  atomic_or( &coex_wci2_rx.status, (atomic_plain_word_t) status );
  CXM_TRACE( CXM_TRC_WCI2_RX_CB, status, 0, 0, CXM_TRC_EN );
  rex_set_sigs( &cxm_tcb, CXM_UART_RX_DIRECT_SIG );

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_recv_wci2_type1

=============================================================================*/
/*!
    @brief
    handle an incoming type 1 message. Type 0 is automatically sent by
    hardware, but we need to send a type 7 out by software.

    @return
    void
*/
/*===========================================================================*/
void coex_handle_recv_wci2_type1( 
  uint8 data
)
{
  /*-----------------------------------------------------------------------*/
  CXM_MSG_0( HIGH, "Handling incoming wci2 type1 message" );
  coex_prep_and_send_wci2_type7( COEX_WCI2_T7_NO_EVNT_MSK, CXM_TECH_LTE );
}

/*=============================================================================

  FUNCTION:  coex_handle_recv_wci2_type6

=============================================================================*/
/*!
    @brief
    Function to handle wci-2 type 6 messages received from 3rd party. Input
    data has already been stripped of type.

    @return void
*/
/*===========================================================================*/
void coex_handle_recv_wci2_type6 (
  uint8 data
)
{
  uint8 ored_pl_req_update = 0x00;

  /*-----------------------------------------------------------------------*/
  CXM_MSG_2( HIGH, "Handling wci2 type 6 msg: [incoming_data:%x][curr_req_status:%x]",
             data, coex_wci2_pwr_lmt_src_req_status );

  if( coex_wwan_state_info[CXM_TECH_LTE].policy & 
        COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT_V01 )
  {
    data &= COEX_WCI2_T6_PL_MASK;
    switch (data)
    {
      case COEX_WCI2_T6_PL_REQ_OFF_SRC_BT:
        ored_pl_req_update |= (COEX_WCI2_T6_PL_REQ_OFF << COEX_WCI2_T6_PL_SRC_BT);
        break;
      case COEX_WCI2_T6_PL_REQ_ON_SRC_BT:
        ored_pl_req_update |= (COEX_WCI2_T6_PL_REQ_ON << COEX_WCI2_T6_PL_SRC_BT);
        break;
      case COEX_WCI2_T6_PL_REQ_OFF_SRC_WIFI:
        ored_pl_req_update |= (COEX_WCI2_T6_PL_REQ_OFF << COEX_WCI2_T6_PL_SRC_WIFI);
        break;
      case COEX_WCI2_T6_PL_REQ_ON_SRC_WIFI:
        ored_pl_req_update |= (COEX_WCI2_T6_PL_REQ_ON << COEX_WCI2_T6_PL_SRC_WIFI);
        break;
      default:
        ored_pl_req_update = coex_wci2_pwr_lmt_src_req_status;
        CXM_MSG_0( ERROR, "Incorrect data for type 6 msg" );
        break;
    }

    CXM_MSG_2( HIGH, "Processing WCI2 power limiting: [curr_state:%x][new_req:%x]",
               coex_wci2_pwr_lmt_state, ored_pl_req_update );
    if( coex_wci2_pwr_lmt_state == COEX_PWR_LMT_OFF &&
        ored_pl_req_update != 0 )
    {
      /* wci2 power limiting now enforced */
      coex_wci2_pwr_lmt_state = COEX_PWR_LMT_PENDING;
      /* call to ML1 to check power limiting conditions */
      coex_check_power_limiting_conditions();
      cxm_counter_event( CXM_CNT_WCI2_PWR_LMT_REQ, 
                         (uint32) coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_power_limit );
      CXM_MSG_0( HIGH, "Requesting to enforce WCI-2 power limiting" );
    }
    else if( ( coex_wci2_pwr_lmt_state == COEX_PWR_LMT_ACTIVE ||
               coex_wci2_pwr_lmt_state == COEX_PWR_LMT_RRC_PROC_ONGOING ||
               coex_wci2_pwr_lmt_state == COEX_PWR_LMT_PENDING ) &&
               ored_pl_req_update == 0 )
    {
      /* clear the power limit */
      timer_clr( &coex_pwr_lmt_wdog_timer, T_MSEC );
      coex_wci2_pwr_lmt_state = COEX_PWR_LMT_OFF;
      coex_check_and_set_power_limit();
      CXM_MSG_0( HIGH, "Stopping WCI-2 power limiting" );
    }
    else
    {
      CXM_MSG_1( HIGH, "Previous WCI2 power limiting state continues %x", coex_wci2_pwr_lmt_state );
    }
  }
  coex_wci2_pwr_lmt_src_req_status = ored_pl_req_update;
  return;
}

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
void coex_algos_process_wci2_rx( void )
{
  uint8 i;
  uint8 data;
  uint8 status;
  /*-----------------------------------------------------------------------*/
  /* read the status mask, then clear it and process new data */
  status = (uint8) atomic_read( &coex_wci2_rx.status );
  for( i = 0; i < 8; i++ )
  {
    /* if this type mask is set in the status, this byte is new */
    if( status & (1 << i) )
    {
      data = WCI2_GET_PAYLOAD(coex_wci2_rx.bytes[i]);
      /* log received data */
      cxm_recved_wci2_data_log = data;
      CXM_TRACE( CXM_TRC_WCI2_PROC_RX, coex_wci2_rx.bytes[i], i, 0, 
                 CXM_TRC_AND_PKT_EN, CXM_LOG_WCI2_DATA_RECV );

      atomic_clear_bit( &coex_wci2_rx.status, i );
      switch( (wci2_data_type_e) i )
      {
        case WCI2_TYPE1: /* type 1 message - handled in hardware */
          coex_handle_recv_wci2_type1( data );
          break;

        case WCI2_TYPE6: /* type 6 message */
          coex_handle_recv_wci2_type6( data );
          break;

        default:
          CXM_MSG_2( ERROR, "dropped type %d data = 0x%x", i, data);
          break;
      }
    }
  }

  return;
}

/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  coex_handle_indication_register_req

=============================================================================*/
/*!
    @brief
    Function to handle & respond to CXM_INDICATION_REGISTER_REQ message
 
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
)
{
  qmi_csi_cb_error                      req_cb_retval = QMI_CSI_CB_NO_ERR;
  errno_enum_type                       estatus       = E_SUCCESS;
  qmi_csi_error                         ind_retval    = QMI_CSI_NO_ERR;
  coex_indication_register_req_msg_v01 *request;
  coex_indication_register_resp_msg_v01 response;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  request = ( coex_indication_register_req_msg_v01* ) req_c_struct;

  /* Init response data */
  memset( &response, 0, sizeof( coex_indication_register_resp_msg_v01 ) );

  /* Validate REQ message */
  if( sizeof( coex_indication_register_req_msg_v01 ) != req_c_struct_len )
  {
    response.resp.result = QMI_RESULT_FAILURE_V01;
    response.resp.error  = QMI_ERR_MALFORMED_MSG_V01;
    req_cb_retval        = QMI_CSI_CB_INTERNAL_ERR;
  }
  else
  {
    /* All looks okay, update the indication map for this client and
         send out the response */
    response.resp.result = QMI_RESULT_SUCCESS_V01;
    response.resp.error  = QMI_ERR_NONE_V01;

    if( TRUE == request->report_coex_wwan_state_valid )
    {
      /* call CXM CONN MGR layer to update the client's indication map */
      estatus = mqcsi_conn_mgr_update_client_ind_map(
                  MQCSI_COEX_SERVICE_ID,
                  connection_handle,
                  QMI_COEX_WWAN_STATE_IND_V01,
                  request->report_coex_wwan_state
                );
      if( E_SUCCESS != estatus )
      {
        req_cb_retval = QMI_CSI_CB_INTERNAL_ERR;
      }
      else
      {
        /* Send out the current WWAN state info IND */
        ind_retval = coex_prep_and_send_wwan_state_ind( TRUE );
        if( QMI_CSI_NO_ERR != ind_retval )
        {
          req_cb_retval = QMI_CSI_CB_INTERNAL_ERR;
        }
      }
      CXM_TRACE( CXM_TRC_QMI_HNDL_REG_IND_REQ,
                 req_cb_retval, QMI_COEX_WWAN_STATE_IND_V01,
                 (uint32)connection_handle, /* client ptr */
                 CXM_TRC_EN );
    }

    if( TRUE == request->report_coex_metrics_lte_bler_valid )
    {
      /* call CXM CONN MGR layer to update the client's indication map */
      estatus = mqcsi_conn_mgr_update_client_ind_map(
                  MQCSI_COEX_SERVICE_ID,
                  connection_handle,
                  QMI_COEX_METRICS_LTE_BLER_IND_V01,
                  request->report_coex_metrics_lte_bler
                );
      if( E_SUCCESS != estatus )
      {
        req_cb_retval = QMI_CSI_CB_INTERNAL_ERR;
      }
      CXM_TRACE( CXM_TRC_QMI_HNDL_REG_IND_REQ,
                 req_cb_retval, QMI_COEX_METRICS_LTE_BLER_IND_V01,
                 (uint32)connection_handle, /* client ptr */
                 CXM_TRC_EN );
    }

    if( TRUE == request->report_coex_fail_condition_valid )
    {
      /* call CXM CONN MGR layer to update the client's indication map */
      estatus = mqcsi_conn_mgr_update_client_ind_map(
                  MQCSI_COEX_SERVICE_ID,
                  connection_handle,
                  QMI_COEX_CONDITION_FAIL_IND_V01,
                  request->report_coex_fail_condition
                );
      if( E_SUCCESS != estatus )
      {
        req_cb_retval = QMI_CSI_CB_INTERNAL_ERR;
      }
      CXM_TRACE( CXM_TRC_QMI_HNDL_REG_IND_REQ,
                 req_cb_retval, QMI_COEX_CONDITION_FAIL_IND_V01,
                 (uint32)connection_handle, /* client ptr */
                 CXM_TRC_EN );
    }

    if( TRUE == request->report_coex_success_condition_valid )
    {
      /* call CXM CONN MGR layer to update the client's indication map */
      estatus = mqcsi_conn_mgr_update_client_ind_map(
                  MQCSI_COEX_SERVICE_ID,
                  connection_handle,
                  QMI_COEX_CONDITION_SUCCESS_IND_V01,
                  request->report_coex_success_condition
                );
      if( E_SUCCESS != estatus )
      {
        req_cb_retval = QMI_CSI_CB_INTERNAL_ERR;
      }
      CXM_TRACE( CXM_TRC_QMI_HNDL_REG_IND_REQ,
                 req_cb_retval, QMI_COEX_CONDITION_SUCCESS_IND_V01,
                 (uint32)connection_handle, /* client ptr */
                 CXM_TRC_EN );
    }

    if( TRUE == request->report_coex_sleep_valid )
    {
      /* call CXM CONN MGR layer to update the client's indication map */
      estatus = mqcsi_conn_mgr_update_client_ind_map(
                  MQCSI_COEX_SERVICE_ID,
                  connection_handle,
                  QMI_COEX_SLEEP_IND_V01,
                  request->report_coex_sleep
                );
      if( E_SUCCESS != estatus )
      {
        req_cb_retval = QMI_CSI_CB_INTERNAL_ERR;
      }
      CXM_TRACE( CXM_TRC_QMI_HNDL_REG_IND_REQ,
                 req_cb_retval, QMI_COEX_SLEEP_IND_V01,
                 (uint32)connection_handle, /* client ptr */
                 CXM_TRC_EN );
    }

    if( TRUE == request->report_coex_wakeup_valid )
    {
      /* call CXM CONN MGR layer to update the client's indication map */
      estatus = mqcsi_conn_mgr_update_client_ind_map(
                  MQCSI_COEX_SERVICE_ID,
                  connection_handle,
                  QMI_COEX_WAKEUP_IND_V01,
                  request->report_coex_wakeup
                );
      if( E_SUCCESS != estatus )
      {
        req_cb_retval = QMI_CSI_CB_INTERNAL_ERR;
      }
      CXM_TRACE( CXM_TRC_QMI_HNDL_REG_IND_REQ,
                 req_cb_retval, QMI_COEX_WAKEUP_IND_V01,
                 (uint32)connection_handle, /* client ptr */
                 CXM_TRC_EN );
    }

    if( TRUE == request->report_coex_page_sync_valid )
    {
      /* call CXM CONN MGR layer to update the client's indication map */
      estatus = mqcsi_conn_mgr_update_client_ind_map(
                  MQCSI_COEX_SERVICE_ID,
                  connection_handle,
                  QMI_COEX_WCN_WAKE_SYNC_IND_V01,
                  request->report_coex_page_sync
                );
      if( E_SUCCESS != estatus )
      {
        req_cb_retval = QMI_CSI_CB_INTERNAL_ERR;
      }
      CXM_TRACE( CXM_TRC_QMI_HNDL_REG_IND_REQ,
                 req_cb_retval, QMI_COEX_WCN_WAKE_SYNC_IND_V01,
                 (uint32)connection_handle, /* client ptr */
                 CXM_TRC_EN );
    }
  }

  CXM_TRACE( CXM_TRC_QMI_HNDL_REG_IND_REQ,
             req_cb_retval, !0x00, (uint32)connection_handle,
             CXM_TRC_EN );

  if( QMI_CSI_CB_NO_ERR != req_cb_retval )
  {
    /* Something went wrong while updating one of the indication map(s) for this client and
         send out the response */
    response.resp.result = QMI_RESULT_FAILURE_V01;
    response.resp.error  = QMI_ERR_INTERNAL_V01;
  }

  /* Send the response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_indication_register_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}


/*=============================================================================

  FUNCTION:  coex_handle_get_wwan_state_req

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
)
{
  qmi_csi_cb_error                 req_cb_retval = QMI_CSI_CB_NO_ERR;

  coex_get_wwan_state_resp_msg_v01 response;
  uint8                           i, band_index;
  /*-----------------------------------------------------------------------*/
  /* Reset the memory available for the RESP message */
  memset( &response, 0, sizeof( coex_get_wwan_state_resp_msg_v01 ) );

  /* Setting up the optional fields valid flag explicitly */
  response.lte_band_info_valid         = FALSE;
  response.lte_tdd_info_valid          = FALSE;
  response.lte_off_period_valid        = FALSE;
  response.lte_band_info_set_valid     = FALSE;
  response.tdscdma_band_info_set_valid = FALSE;
  response.gsm_band_info_set_valid     = FALSE;
  response.hdr_band_info_set_valid     = FALSE;
  response.wcdma_band_info_set_valid   = FALSE;
  response.onex_band_info_set_valid    = FALSE;

  if (coex_wwan_state_info[CXM_TECH_LTE].state_data.num_link_info_sets != 0) 
  {
    /*fill in the old lte_band_info message to support backwards compatibility*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_LTE].state_data.num_link_info_sets; i++) 
    {
      /*if an entry in our state array matches UL and we havent already 
        encountered one that does, fill in the ul lte info*/
      if ( response.lte_band_info.ul_band.freq==0 &&
           coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].direction == CXM_LNK_DRCTN_UL &&
           (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_LTE].tech_band_filter[i] == TRUE)) 
      {
         response.lte_band_info.ul_band.bandwidth = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].bandwidth / MHZ_HZ_CONVERSION;
         response.lte_band_info.ul_band.freq = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].frequency / MHZ_KHZ_CONVERSION;
      }
      /*if an entry in our state array matches DL and we havent already 
        encountered one that does, fill in the dl lte info*/
      if ( response.lte_band_info.dl_band.freq==0 &&
           coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].direction == CXM_LNK_DRCTN_DL &&
           (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_LTE].tech_band_filter[i] == TRUE)) 
      {
         response.lte_band_info.dl_band.bandwidth = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].bandwidth / MHZ_HZ_CONVERSION;
         response.lte_band_info.dl_band.freq = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].frequency / MHZ_KHZ_CONVERSION;
      }
    }

    /*Fill in the LTE band array*/
    band_index = 0;
    /*Fill in the LTE band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_LTE].state_data.num_link_info_sets; i++) 
    {
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_LTE].tech_band_filter[i] == TRUE)
      {
         response.lte_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].bandwidth;
         response.lte_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].frequency;
         response.lte_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_LTE].state_data.link_list[i].direction;
         band_index++;
      }
    }
    if (band_index > 0) 
    {
      response.lte_band_info_valid = TRUE;
      response.lte_band_info_set_valid = TRUE;
      response.lte_band_info_set_len = band_index; 
    }

    /*Check if we are in LTE TDD and fill in the appropriate fields. If all band information has been filtered out
      (i.e. length is 0), dont send out other LTE params*/
    if (coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.frame_structure == LTE_L1_FRAME_STRUCTURE_FS2 &&
	    response.lte_band_info_set_len > 0 ) 
    {
        /* Setup the optional LTE TDD info valid flag */
        response.lte_tdd_info_valid = TRUE;

        /* Update LTE TDD frame offset info (if provided) */
        response.lte_tdd_info.frame_offset = 
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.frame_offset;

        /* Update LTE TDD config (if provided) */
        response.lte_tdd_info.tdd_config = (coex_lte_tdd_config_enum_v01)
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.tdd_config;

        /* Update LTE TDD SSP (if provided) */
        response.lte_tdd_info.subframe_config = (coex_lte_tdd_subframe_config_enum_v01)
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.ssp;

        /* Update LTE TDD DL CP config (if provided) */
        response.lte_tdd_info.dl_config = (coex_lte_tdd_link_config_enum_v01)
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.dl_cp;

        /* Update LTE TDD UL CP config (if provided) */
        response.lte_tdd_info.ul_config = (coex_lte_tdd_link_config_enum_v01)
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.ul_cp;
    }

  } /*end of 'if lte is valid'*/

  if (coex_wwan_state_info[CXM_TECH_TDSCDMA].state_data.num_link_info_sets != 0) 
  {
    band_index = 0;
    /*Fill in the TDS band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_TDSCDMA].state_data.num_link_info_sets; i++) 
    {
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_TDSCDMA].tech_band_filter[i] == TRUE)
      {
         response.tdscdma_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_TDSCDMA].state_data.link_list[i].bandwidth;
         response.tdscdma_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_TDSCDMA].state_data.link_list[i].frequency;
         response.tdscdma_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_TDSCDMA].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      response.tdscdma_band_info_set_valid = TRUE;
      response.tdscdma_band_info_set_len = band_index; 
    }

  } /*end of if TDS is valid*/

  if (coex_wwan_state_info[CXM_TECH_GSM1].state_data.num_link_info_sets != 0) 
  {  
    band_index = 0;
    /*Fill in the GSM band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_GSM1].state_data.num_link_info_sets; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_GSM1].tech_band_filter[i] == TRUE)
      {
         response.gsm_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_GSM1].state_data.link_list[i].bandwidth;
         response.gsm_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_GSM1].state_data.link_list[i].frequency;
         response.gsm_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_GSM1].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      response.gsm_band_info_set_valid = TRUE;
      response.gsm_band_info_set_len = band_index; 
    }

  } /*end of if GSM 1 is valid*/

  if (coex_wwan_state_info[CXM_TECH_GSM2].state_data.num_link_info_sets != 0) 
  {  
    band_index = 0;
    /*Fill in the GSM band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_GSM2].state_data.num_link_info_sets; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_GSM2].tech_band_filter[i] == TRUE)
      {
         response.gsm2_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_GSM2].state_data.link_list[i].bandwidth;
         response.gsm2_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_GSM2].state_data.link_list[i].frequency;
         response.gsm2_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_GSM2].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      response.gsm2_band_info_set_valid = TRUE;
      response.gsm2_band_info_set_len = band_index; 
    }

  } /*end of if GSM 2 is valid*/

  if (coex_wwan_state_info[CXM_TECH_GSM3].state_data.num_link_info_sets != 0) 
  {  
    band_index = 0;
    /*Fill in the GSM band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_GSM3].state_data.num_link_info_sets; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_GSM3].tech_band_filter[i] == TRUE)
      {
         response.gsm3_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_GSM3].state_data.link_list[i].bandwidth;
         response.gsm3_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_GSM3].state_data.link_list[i].frequency;
         response.gsm3_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_GSM3].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      response.gsm3_band_info_set_valid = TRUE;
      response.gsm3_band_info_set_len = band_index; 
    }

  } /*end of if GSM 3 is valid*/

  if (coex_wwan_state_info[CXM_TECH_ONEX].state_data.num_link_info_sets != 0) 
  {  
    band_index = 0;
    /*Fill in the ONEX band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_ONEX].state_data.num_link_info_sets; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_ONEX].tech_band_filter[i] == TRUE)
      {
         response.onex_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_ONEX].state_data.link_list[i].bandwidth;
         response.onex_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_ONEX].state_data.link_list[i].frequency;
         response.onex_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_ONEX].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      response.onex_band_info_set_valid = TRUE;
      response.onex_band_info_set_len = band_index; 
    }

  } /*end of if ONEX is valid*/

  if (coex_wwan_state_info[CXM_TECH_HDR].state_data.num_link_info_sets != 0) 
  {  
    band_index = 0;
    /*Fill in the HDR band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_HDR].state_data.num_link_info_sets; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_HDR].tech_band_filter[i] == TRUE)
      {
         response.hdr_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_HDR].state_data.link_list[i].bandwidth;
         response.hdr_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_HDR].state_data.link_list[i].frequency;
         response.hdr_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_HDR].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      response.hdr_band_info_set_valid = TRUE;
      response.hdr_band_info_set_len = band_index; 
    }

  } /*end of if HDR is valid*/

  if (coex_wwan_state_info[CXM_TECH_WCDMA].state_data.num_link_info_sets != 0) 
  {  
    band_index = 0;
    /*Fill in the WCDMA band array*/
    for (i=0; i< coex_wwan_state_info[CXM_TECH_WCDMA].state_data.num_link_info_sets; i++) 
    {  
      if (!coex_band_filter_info.filter_on || coex_wwan_state_info[CXM_TECH_WCDMA].tech_band_filter[i] == TRUE)
      {
         response.wcdma_band_info_set[band_index].bandwidth = coex_wwan_state_info[CXM_TECH_WCDMA].state_data.link_list[i].bandwidth;
         response.wcdma_band_info_set[band_index].freq = coex_wwan_state_info[CXM_TECH_WCDMA].state_data.link_list[i].frequency;
         response.wcdma_band_info_set[band_index].direction = coex_wwan_state_info[CXM_TECH_WCDMA].state_data.link_list[i].direction;
         band_index++;
      }
    }

    if (band_index > 0) 
    {
      response.wcdma_band_info_set_valid = TRUE;
      response.wcdma_band_info_set_len = band_index; 
    }

  } /*end of if WCDMA is valid*/


  /* Send the response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_get_wwan_state_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}


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
)
{
  qmi_csi_cb_error req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_set_wlan_state_req_msg_v01 *request;
  coex_set_wlan_state_resp_msg_v01 response;

  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  CXM_MSG_0( HIGH, "handling QMI_COEX_SET_WLAN_STATE_REQ_V01" );

  request = (coex_set_wlan_state_req_msg_v01 *) req_c_struct;

  /* Init response data */
  memset( &response, 0, sizeof(coex_set_wlan_state_resp_msg_v01) );
  response.resp.result = QMI_RESULT_SUCCESS_V01;
  response.resp.error  = QMI_ERR_NONE_V01;

  if ( request->scan_info_valid )
  {
    response.resp.result = QMI_RESULT_FAILURE_V01;
    response.resp.error  = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
  }
  else
  {
    if ( request->conn_info_valid )
    {
      /* if the connection info is valid, need to compare to our conn list */
      coex_update_wlan_conn_state_info( request );
    }

    if ( request->high_prio_info_valid )
    {
      /* if the high prio info is valid, need to compare to our high prio list */
      coex_update_wlan_hp_state_info( request );
    }
  }

  /* Send the response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_set_wlan_state_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );
#ifdef FEATURE_COEX_USE_NV
  /* check for conflicts and mitigate if necessary.*/
  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_VICTIM_TABLE) && coex_params.num_conflicts != 0)
  {
    coex_process_conflicts(CXM_TECH_LTE, CXM_TECH_WIFI); 
    coex_process_conflicts(CXM_TECH_TDSCDMA, CXM_TECH_WIFI);
    coex_process_conflicts(CXM_TECH_GSM1, CXM_TECH_WIFI);
    coex_process_conflicts(CXM_TECH_GSM2, CXM_TECH_WIFI);
    coex_process_conflicts(CXM_TECH_GSM3, CXM_TECH_WIFI);
  }
#endif

  return req_cb_retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_get_wlan_hp_state_req

=============================================================================*/
/*!
    @brief
    Given a high prio id, the function locates the id in the coex wlan data (if
    it exists) and returns the data in that high prio entry.
 
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
)
{
  qmi_csi_cb_error req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_get_wlan_high_prio_state_req_msg_v01 *request;
  coex_get_wlan_high_prio_state_resp_msg_v01 response;
  coex_wlan_hp_node_type *node = coex_wlan_hp_head;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  request = (coex_get_wlan_high_prio_state_req_msg_v01 *) req_c_struct;

  /* intialize to default, in case no match is found */
  response.resp.result = QMI_RESULT_FAILURE_V01;
  response.resp.error = QMI_ERR_INVALID_HANDLE_V01;
  response.high_prio_info.id = 0;
  response.high_prio_info.band.freq = 0;
  response.high_prio_info.band.bandwidth = 0;
  response.high_prio_info.state = 0;
  response.high_prio_info_valid = FALSE;

  while ( node != NULL )
  {
    /* id is unique and indicates a match */
    if ( node->high_prio.id == request->id )
    {
      response.resp.result = QMI_RESULT_SUCCESS_V01;
      response.resp.error = QMI_ERR_NONE_V01;
      response.high_prio_info.id = node->high_prio.id;
      response.high_prio_info.band.freq = node->high_prio.band.freq;
      response.high_prio_info.band.bandwidth = node->high_prio.band.bandwidth;
      response.high_prio_info.state = node->high_prio.state;
      response.high_prio_info_valid = TRUE;
      break;
    }
    node = node->next;
  }

  /* send the qmi response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,  
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_get_wlan_high_prio_state_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_get_wlan_conn_state_req_msg_v01 *request;
  coex_get_wlan_conn_state_resp_msg_v01 response;
  coex_wlan_conn_node_type *node = coex_wlan_conn_head;
  uint16 i;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  request = (coex_get_wlan_conn_state_req_msg_v01 *) req_c_struct;

  /* intialize to default, in case no match is found */
  response.resp.result = QMI_RESULT_FAILURE_V01;
  response.resp.error = QMI_ERR_INVALID_HANDLE_V01;
  response.conn_info_valid = FALSE;
  response.conn_info.handle = 0;
  response.conn_info.band_len = 0;
  response.conn_info.band[0].bandwidth = 0;
  response.conn_info.band[0].freq = 0;
  response.conn_info.state = COEX_WLAN_CONN_DISABLED;
  response.conn_info.mode = COEX_WLAN_CONN_MODE_NONE;

  while ( node != NULL )
  {
    /* handle is unique and indicates a match */
    if ( node->conn.handle == request->conn_handle )
    {
      response.resp.result = QMI_RESULT_SUCCESS_V01;
      response.resp.error = QMI_ERR_NONE_V01;
      response.conn_info_valid = TRUE;
      response.conn_info.handle = node->conn.handle;
      response.conn_info.band_len = node->conn.band_len;
      for ( i = 0; i < response.conn_info.band_len; i++ )
      {
        response.conn_info.band[i].bandwidth = node->conn.band[i].bandwidth;
        response.conn_info.band[i].freq = node->conn.band[i].freq;
      }
      response.conn_info.state = node->conn.state;
      response.conn_info.mode = node->conn.mode;

      /* handle has been found, no need to keep searching */
      break;
    }
    node = node->next;
  }

  /* send the qmi response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_get_wlan_conn_state_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error             req_cb_retval = QMI_CSI_CB_NO_ERR;
  qmi_response_type_v01        response;
  coex_set_policy_req_msg_v01 *req_ptr;
  errno_enum_type              retval = E_SUCCESS;
  uint32                       wci2_t7_msk = 0x00;
  coex_policy_config_mask_v01  prev_policy_mask = 0x00;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( qmi_response_type_v01 ) );

  /* De-mystify req_c_struct */
  req_ptr = (coex_set_policy_req_msg_v01 *) req_c_struct;

  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_QMI_POLICY_CONTROL) )
  {
    CXM_MSG_2( HIGH, "Handling QMI_COEX_SET_POLICY_REQ policy_valid=%d mask=%d",
               req_ptr->policy_valid, req_ptr->policy );
    response.result = QMI_RESULT_SUCCESS_V01;
    response.error  = QMI_ERR_NONE_V01;

    /* Update global copy of policy info. Use prev value when a field is invalid */
    if(req_ptr->policy_valid)
    {
      prev_policy_mask = coex_wwan_state_info[CXM_TECH_LTE].policy;
      coex_wwan_state_info[CXM_TECH_LTE].policy = req_ptr->policy;

      if( COEX_SYS_ENABLED(CXM_SYS_BHVR_BASIC_WCI2) )
      {
        coex_wwan_state_info[CXM_TECH_LTE].policy &= COEX_BASIC_WCI2_MASK;
      }

      /* manage controller power limit */
      if( ( coex_controller_pwr_lmt_state == COEX_PWR_LMT_OFF ) &&
          ( coex_wwan_state_info[CXM_TECH_LTE].policy & 
            COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT_V01 ))
      {
        /* controller power limiting now enforced */
        coex_controller_pwr_lmt_state = COEX_PWR_LMT_PENDING;
        coex_check_power_limiting_conditions();
        cxm_counter_event( CXM_CNT_CONTR_PWR_LMT_REQ, 
                           coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_power_limit );
      }
      else if( (coex_wwan_state_info[CXM_TECH_LTE].policy & 
                COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT_V01) == 0 )
      {
        coex_controller_pwr_lmt_state = COEX_PWR_LMT_OFF;
      }

      /* manage wci-2 power limit */
      if( ((coex_wwan_state_info[CXM_TECH_LTE].policy & 
            COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT_V01) == 0 ) &&
          (coex_wci2_pwr_lmt_state != COEX_PWR_LMT_OFF) )
      {
        coex_wci2_pwr_lmt_state = COEX_PWR_LMT_OFF;
        /* stop wdog timer */
        timer_clr( &coex_pwr_lmt_wdog_timer, T_MSEC );
      }

      if ( req_ptr->policy != 0 )
      {
        /* policy is now enforced, check if uart should be powered on */
        coex_wwan_state_info[CXM_TECH_LTE].policy_state = COEX_POLICY_ACTIVE;

        if (COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA)) 
        {
          coex_manage_uart_npa_vote();

          /* Check for type7[connected] policy transition */
          if( !(COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 & prev_policy_mask) &&
              COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy )
          {
            wci2_t7_msk |= COEX_WCI2_T7_CONN_ST_EVNT_MSK;
          }
          if( !(COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy) )
          {
            coex_wci2_t7_state.wwan_tx_active = FALSE;
          }

          /* Check for type7[tx_pwr] policy transition */
          if( !(COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01 & prev_policy_mask) &&
              COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy )
          {
            wci2_t7_msk |= COEX_WCI2_T7_PWR_ST_EVNT_MSK;
          }
          if( !(COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy) )
          {
            coex_wci2_t7_state.wwan_tx_pwr_active = FALSE;
          }

          /* Check for type7[tx_ant] policy transition */
          if( !(COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL_V01 & prev_policy_mask) &&
              COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy )
          {
	   		    wci2_t7_msk |= COEX_WCI2_T7_TX_ANT_SEL_EVNT_MSK;
          }
          if( !(COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy) )
          {
            coex_wci2_t7_state.tx_ant_sel = FALSE;
          }

          /* Trigger WCI2 type7 message only if a valid event identified */
          if( COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy ||
              COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01  & coex_wwan_state_info[CXM_TECH_LTE].policy ||
              COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy )
          {
            coex_prep_and_send_wci2_type7( wci2_t7_msk, CXM_TECH_LTE );
          }
        }

        if( COEX_PCM_RESERVED_FOR_CONTROLLER_V01 & coex_wwan_state_info[CXM_TECH_LTE].policy )
        {
          CXM_MSG_0( HIGH, "Setting policy RESERVED FOR CONTROLLER");
          cxm_counter_event( CXM_CNT_PLCY_RSRVD_FOR_CTRLR, 0 );
        }
      }
      else
      {
        /* policy is now not enforced, check if uart should be powered off */
        coex_wwan_state_info[CXM_TECH_LTE].policy_state = COEX_POLICY_INACTIVE;
        if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) ) 
        {
          coex_manage_uart_npa_vote();
          coex_wci2_t7_state.wwan_tx_active = FALSE;
          coex_wci2_t7_state.wwan_tx_pwr_active = FALSE;
          coex_wci2_t7_state.tx_ant_sel = FALSE;
        }

        /* if the policy was set to 0 (reset) count it */
        cxm_counter_event( CXM_CNT_POLICY_RESET, 0 );
      }
    }

    COEX_BOUND_AND_SET( req_ptr->power_threshold_valid,
                        coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.power_threshold, 
                        req_ptr->power_threshold, COEX_POLICY_POWER_THRESH_MIN, 
                        COEX_POLICY_POWER_THRESH_MAX );

    COEX_BOUND_AND_SET( req_ptr->rb_threshold_valid,
                        coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.rb_threshold, 
                        req_ptr->rb_threshold, 0, COEX_POLICY_RB_THRESH_MAX );

    COEX_BOUND_AND_SET( req_ptr->lte_tx_continuous_subframe_denials_threshold_valid,
                        coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.tx_continuous_subframe_denials_threshold, 
                        req_ptr->lte_tx_continuous_subframe_denials_threshold, 
                        COEX_POLICY_CONT_SF_DENIALS_THRESH_MIN, COEX_POLICY_CONT_SF_DENIALS_THRESH_MAX );

    if(req_ptr->lte_tx_subrame_denial_params_valid)
    {
      coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.tx_subrame_denial_params = 
        req_ptr->lte_tx_subrame_denial_params;

      coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.tx_subrame_denial_params.max_allowed_frame_denials =
        ( req_ptr->lte_tx_subrame_denial_params.max_allowed_frame_denials > COEX_POLICY_MAX_SF_DENIALS_MAX ?
          COEX_POLICY_MAX_SF_DENIALS_MAX : req_ptr->lte_tx_subrame_denial_params.max_allowed_frame_denials );

      coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.tx_subrame_denial_params.frame_denial_window =
        ( req_ptr->lte_tx_subrame_denial_params.frame_denial_window > COEX_POLICY_SF_DENIAL_WINDOW_MAX ?
          COEX_POLICY_SF_DENIAL_WINDOW_MAX : req_ptr->lte_tx_subrame_denial_params.frame_denial_window );
    }

    if(req_ptr->apt_table_valid)
    {
      /* this value no longer supported but still copy it in */
      coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.apt_table = req_ptr->apt_table;
    }

    COEX_BOUND_AND_SET( req_ptr->controller_tx_power_limit_valid,
                        coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_power_limit, 
                        req_ptr->controller_tx_power_limit, 
                        COEX_POLICY_TX_POWER_LIMIT_MIN, COEX_POLICY_TX_POWER_LIMIT_MAX );

    COEX_BOUND_AND_SET( req_ptr->wci2_power_limit_valid,
                        coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_power_limit, 
                        req_ptr->wci2_power_limit, 
                        COEX_POLICY_TX_POWER_LIMIT_MIN, COEX_POLICY_TX_POWER_LIMIT_MAX );

    COEX_BOUND_AND_SET( req_ptr->link_path_loss_threshold_valid,
                        coex_plcy_parms.link_path_loss_threshold, 
                        req_ptr->link_path_loss_threshold, 
                        COEX_POLICY_LINK_PATH_LOSS_THRESH_MIN, COEX_POLICY_LINK_PATH_LOSS_THRESH_MAX );

    COEX_BOUND_AND_SET( req_ptr->rb_filter_alpha_valid,
                        coex_plcy_parms.rb_filter_alpha, 
                        req_ptr->rb_filter_alpha, 
                        COEX_POLICY_RB_FILTER_ALPHA_MIN, COEX_POLICY_RB_FILTER_ALPHA_MAX );

    COEX_BOUND_AND_SET( req_ptr->filtered_rb_threshold_valid,
                        coex_plcy_parms.filtered_rb_threshold, 
                        req_ptr->filtered_rb_threshold, 
                        COEX_POLICY_FILTERED_RB_THRESH_MIN, COEX_POLICY_FILTERED_RB_THRESH_MAX );

    if(req_ptr->wci2_tx_pwrlmt_timeout_valid)
    {
      coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_tx_pwrlmt_timeout = 
        req_ptr->wci2_tx_pwrlmt_timeout;
    }

    if(req_ptr->controller_tx_pwrlmt_timeout_valid)
    {
      coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_pwrlmt_timeout = 
        req_ptr->controller_tx_pwrlmt_timeout;
    }

    COEX_BOUND_AND_SET( req_ptr->tx_power_threshold_for_adv_tx_notice_valid,
                        coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.tx_power_threshold_for_adv_tx_notice, 
                        req_ptr->tx_power_threshold_for_adv_tx_notice, 
                        COEX_POLICY_POWER_THRESH_FOR_TX_ADV_NTC_MIN_DB10, 
                        COEX_POLICY_POWER_THRESH_FOR_TX_ADV_NTC_MAX_DB10 );

    COEX_BOUND_AND_SET( req_ptr->rb_threshold_for_adv_tx_notice_valid,
                        coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.rb_threshold_for_adv_tx_notice, 
                        req_ptr->rb_threshold_for_adv_tx_notice, 
                        0, COEX_POLICY_RB_THRESH_FOR_TX_ADV_NTC_MAX );

    if(req_ptr->holdoff_timer_for_rat_conn_state_valid)
    {
      /* to uint32, incoming value uint16, min=0 and max=MAX_INT(uint16) enforced by type */
      coex_plcy_parms.t7_con_holdoff = req_ptr->holdoff_timer_for_rat_conn_state;
    }

    COEX_BOUND_AND_SET( req_ptr->filter_alpha_for_rat_power_state_valid,
                        coex_plcy_parms.t7_pwr_alpha, 
                        req_ptr->filter_alpha_for_rat_power_state, 
                        COEX_POLICY_FILTER_ALPHA_RAT_PWR_STATE_MIN, 
                        COEX_POLICY_FILTER_ALPHA_RAT_PWR_STATE_MAX );

    COEX_BOUND_AND_SET( req_ptr->tx_power_threshold_for_rat_power_state_valid,
                        coex_plcy_parms.t7_pwr_thresh, 
                        req_ptr->tx_power_threshold_for_rat_power_state, 
                        COEX_POLICY_TX_PWR_THRESH_RAT_PWR_STATE_MIN, 
                        COEX_POLICY_TX_PWR_THRESH_RAT_PWR_STATE_MAX );

    if(req_ptr->holdoff_timer_for_rat_power_state_valid)
    {
      /* to uint32, incoming value uint16, min=0 and max=MAX_INT(uint16) enforced by type */
      coex_plcy_parms.t7_pwr_holdoff = req_ptr->holdoff_timer_for_rat_power_state;
    }

    /* Power limit states may have changed. Check if power limit needs to be set */
    coex_check_and_set_power_limit();

    retval = coex_prep_and_send_policy_msg( CXM_TECH_LTE );

    cxm_counter_event( CXM_CNT_POLICY_UPDATES, coex_wwan_state_info[CXM_TECH_LTE].policy );
    CXM_TRACE( CXM_TRC_QMI_SET_POLICY_REQ, req_ptr->policy, 0, 0,
                 CXM_TRC_AND_PKT_EN, CXM_LOG_GLOBAL_POLICY_PARAMS,
                 CXM_LOG_WCI2_PWR_LMT_STATE, CXM_LOG_CONTROLLER_PWR_LMT_STATE );
  } 
  else if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_OFF_STATE_ENABLED) && 
                  req_ptr->policy_valid && req_ptr->policy == CXM_POLICY_NONE)
  {
    /*send policy 0 then terminate CXM task after sending the QMI response*/
    coex_wwan_state_info[CXM_TECH_LTE].policy = CXM_POLICY_NONE;
    retval = coex_prep_and_send_policy_msg( CXM_TECH_LTE );

    response.result = QMI_RESULT_SUCCESS_V01;
    response.error  = QMI_ERR_NONE_V01;
  }
  else
  {
    /* currently not supported, send out the response */
    response.result = QMI_RESULT_FAILURE_V01;
    response.error  = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
  } /*CXM_SYS_BHVR_QMI_CONTROL, CXM_SYS_BHVR_OFF_STATE_ENABLED*/

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( qmi_response_type_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  if(retval != E_SUCCESS)
  {
    req_cb_retval = QMI_CSI_CB_INTERNAL_ERR;
  }

  /*terminate CXM task if OFF state is enabled and policy 0 is sent*/
  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_OFF_STATE_ENABLED) && 
                  req_ptr->policy_valid && req_ptr->policy == CXM_POLICY_NONE)
  {
    cxm_terminate_task();
  }

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error             req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_get_policy_resp_msg_v01 response;
  /*-----------------------------------------------------------------------*/
  /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( coex_get_policy_resp_msg_v01 ) );

  response.resp.result = QMI_RESULT_SUCCESS_V01;
  response.resp.error  = QMI_ERR_NONE_V01;

  /* copy fields from the stored info */
  response.policy_valid = TRUE;
  response.policy = coex_wwan_state_info[CXM_TECH_LTE].policy;
  response.power_threshold_valid = TRUE;
  response.power_threshold = coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.power_threshold;
  response.rb_threshold_valid = TRUE;
  response.rb_threshold = coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.rb_threshold;
  response.lte_tx_continuous_subframe_denials_threshold_valid = TRUE;
  response.lte_tx_continuous_subframe_denials_threshold = 
    coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.tx_continuous_subframe_denials_threshold;
  response.lte_tx_subrame_denial_params_valid = TRUE;
  response.lte_tx_subrame_denial_params = 
    coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.tx_subrame_denial_params;
  response.apt_table_valid = TRUE;
  response.apt_table = coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.apt_table;
  response.controller_tx_power_limit_valid = TRUE;
  response.controller_tx_power_limit = 
    coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_power_limit;
  response.wci2_power_limit_valid = TRUE;
  response.wci2_power_limit = coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_power_limit;
  response.link_path_loss_threshold_valid = TRUE;
  response.link_path_loss_threshold = 
    coex_plcy_parms.link_path_loss_threshold;
  response.rb_filter_alpha_valid = TRUE;
  response.rb_filter_alpha = coex_plcy_parms.rb_filter_alpha;
  response.filtered_rb_threshold_valid = TRUE;
  response.filtered_rb_threshold = coex_plcy_parms.filtered_rb_threshold;
  response.wci2_tx_pwrlmt_timeout_valid = TRUE;
  response.wci2_tx_pwrlmt_timeout = coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_tx_pwrlmt_timeout;
  response.controller_tx_pwrlmt_timeout_valid = TRUE;
  response.controller_tx_pwrlmt_timeout = 
    coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_pwrlmt_timeout;
  response.tx_power_threshold_for_adv_tx_notice_valid = TRUE;
  response.tx_power_threshold_for_adv_tx_notice = 
    coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.tx_power_threshold_for_adv_tx_notice;
  response.rb_threshold_for_adv_tx_notice_valid = TRUE;
  response.rb_threshold_for_adv_tx_notice = 
    coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.rb_threshold_for_adv_tx_notice;
  response.holdoff_timer_for_rat_conn_state_valid = TRUE;
  response.holdoff_timer_for_rat_conn_state = 
    (uint16_t) coex_plcy_parms.t7_con_holdoff;
  response.filter_alpha_for_rat_power_state_valid = TRUE;
  response.filter_alpha_for_rat_power_state =
    coex_plcy_parms.t7_pwr_alpha;
  response.tx_power_threshold_for_rat_power_state_valid = TRUE;
  response.tx_power_threshold_for_rat_power_state =
    coex_plcy_parms.t7_pwr_thresh;
  response.holdoff_timer_for_rat_power_state_valid = TRUE;
  response.holdoff_timer_for_rat_power_state =
    (uint16_t) coex_plcy_parms.t7_pwr_holdoff;

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_get_policy_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error                           req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_metrics_lte_bler_start_resp_msg_v01   response;
  errno_enum_type                            retval = E_SUCCESS;
  coex_metrics_lte_bler_start_req_msg_v01    *request;
  cxm_coex_metrics_lte_bler_req_s            bler_start_req;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( coex_metrics_lte_bler_start_resp_msg_v01 ) );
  request = (coex_metrics_lte_bler_start_req_msg_v01 *) req_c_struct;
  CXM_MSG_2( HIGH,
             "handling COEX_METRICS_BLER_START, tb_cnt=%d, err_thresh=%d",
             request->tb_cnt, request->threshold_err_tb_cnt );

  /* Implicity register for indications */
  retval = mqcsi_conn_mgr_update_client_ind_map(
              MQCSI_COEX_SERVICE_ID,
              connection_handle,
              QMI_COEX_METRICS_LTE_BLER_IND_V01,
              TRUE
           );

  if( E_SUCCESS != retval )
  {
    /* failed to register for indication */
    response.resp.result = QMI_RESULT_FAILURE_V01;
    response.resp.error  = QMI_ERR_INTERNAL_V01;
  }
  else if (coex_wwan_state_info[CXM_TECH_LTE].state_data.num_link_info_sets == 0)
  {
    /* LTE not currently active */
    response.resp.result = QMI_RESULT_FAILURE_V01;
    response.resp.error  = QMI_ERR_NO_RADIO_V01;
  }
  else
  {
    /* check bounds on request */
    if( request->tb_cnt == 0 || request->threshold_err_tb_cnt == 0 ||
        request->threshold_err_tb_cnt > request->tb_cnt )
    {
      response.resp.result = QMI_RESULT_FAILURE_V01;
      response.resp.error  = QMI_ERR_INVALID_ARG_V01;
    }
    else
    {
      /* check if bler already running */
      if( atomic_compare_and_set( &coex_metrics_lte_bler_started, 0, 1 ) )
      {
        CXM_TRACE( CXM_TRC_QMI_STRT_BLER_REQ,
                         request->tb_cnt, request->threshold_err_tb_cnt, 0,
                         CXM_TRC_AND_PKT_EN, CXM_LOG_METRICS_LTE_BLER_STARTED );

        /*Turn on the uart and send policy for lte subframe marker*/
        if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) )
        {
           coex_manage_uart_npa_vote();
        }
        coex_prep_and_send_policy_msg( CXM_TECH_LTE );

        /* send start msgr msg to LTE */
        bler_start_req.action = CXM_ACTION_START;
        bler_start_req.payload.start_params.tb_count = request->tb_cnt;
        bler_start_req.payload.start_params.tb_err_count_thresh = request->threshold_err_tb_cnt;
        retval = cxm_msgr_send_msg( &bler_start_req.msg_hdr,
                                     MCS_CXM_COEX_METRICS_LTE_BLER_REQ,
                                     sizeof(cxm_coex_metrics_lte_bler_req_s) );
        CXM_ASSERT( E_SUCCESS == retval );
        response.resp.result = QMI_RESULT_SUCCESS_V01;
        response.resp.error  = QMI_ERR_NONE_V01;

        CXM_MSG_0( HIGH, "BLER started" );
      }
      else
      {
        /* bler already started - can't start again */
        response.resp.result = QMI_RESULT_FAILURE_V01;
        response.resp.error  = QMI_ERR_INTERNAL_V01;
      }
    }
  }
  
  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_metrics_lte_bler_start_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error             req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_metrics_lte_bler_stop_resp_msg_v01 response;
  errno_enum_type              retval = E_SUCCESS;
  cxm_coex_metrics_lte_bler_req_s            msgr_stop_bler_req;
  /*-----------------------------------------------------------------------*/
  /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( coex_metrics_lte_bler_stop_resp_msg_v01 ) );
  
  /* check if bler has been started first */
  if( atomic_read(&coex_metrics_lte_bler_started) )
  {
    /* forward stop message to LTE */
    msgr_stop_bler_req.action = CXM_ACTION_STOP;
    retval = cxm_msgr_send_msg( &msgr_stop_bler_req.msg_hdr,
                                MCS_CXM_COEX_METRICS_LTE_BLER_REQ,
                                sizeof(cxm_coex_metrics_lte_bler_req_s) );
    CXM_ASSERT( E_SUCCESS == retval );

    atomic_set(&coex_metrics_lte_bler_started, 0);

    /*Turn off the uart and send policy for lte subframe marker*/
    if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) )
    {
       coex_manage_uart_npa_vote();
    }
    coex_prep_and_send_policy_msg( CXM_TECH_LTE );

    response.resp.result = QMI_RESULT_SUCCESS_V01;
    response.resp.error  = QMI_ERR_NONE_V01;

    CXM_MSG_0( HIGH, "BLER stopped" );
  }
  else
  {
    /* bler hasn't been started yet */
    response.resp.result = QMI_RESULT_FAILURE_V01;
    response.resp.error  = QMI_ERR_INVALID_OPERATION_V01;
  }

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_metrics_lte_bler_stop_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_start_metrics_req_over_qmi

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to start collecting/
    collating SINR metrics

    @return
    none
*/
/*===========================================================================*/
void coex_handle_start_metrics_req_over_qmi (
  coex_wwan_tech_v01    tech,
  qmi_response_type_v01 *resp,
  float                 alpha
)
{
  uint32                                   alpha_q8;
  errno_enum_type                          retval = E_SUCCESS;
  /*-----------------------------------------------------------------------*/ 
  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_QMI_METRICS) )
  {
    /* convert and check bounds on alpha */
    retval = cxm_float_to_unsigned_Q8( alpha, &alpha_q8 );

    if (coex_wwan_state_info[tech].state_data.num_link_info_sets == 0)
    {
      /* tech not currently active */
      resp->result = QMI_RESULT_FAILURE_V01;
      resp->error  = QMI_ERR_NO_RADIO_V01;
    }
    /*validate alpha. Only supported for LTE, TDS, GSM.*/
    else if( retval == E_SUCCESS && alpha_q8 <= COEX_LTE_METRICS_MAX_ALPHA 
            && (tech == COEX_LTE_TECH_V01 || tech == COEX_TDSCDMA_TECH_V01 ||
                tech == COEX_GSM_TECH_V01 || tech == COEX_GSM2_TECH_V01 ||
                tech == COEX_GSM3_TECH_V01 ) )
    {
      /* check if sinr already running */
      if( atomic_compare_and_set( &coex_wwan_state_info[tech].metrics_started, 0, 1 ) )
      {
        CXM_TRACE(CXM_TRC_QMI_STRT_SINR_REQ, alpha_q8, tech, 0, 
                 CXM_TRC_EN );

        /* Tell LTE to start sinr */
        coex_tech_metrics_initiate_req( tech, CXM_ACTION_START, alpha_q8, 0 );

        resp->result = QMI_RESULT_SUCCESS_V01;
        resp->error  = QMI_ERR_NONE_V01;

        CXM_MSG_1( HIGH, "SINR Started, tech=%d", tech );
      }
      else
      {
        /* sinr already started - can't start again */
        resp->result = QMI_RESULT_FAILURE_V01;
        resp->error  = QMI_ERR_INVALID_OPERATION_V01;
      }
    }
    else
    {
      /* alpha or tech from request not valid */
      resp->result = QMI_RESULT_FAILURE_V01;
      resp->error  = QMI_ERR_INVALID_ARG_V01;
    }
  }
  else
  {
    /* LTE metrics not supported over QMI interface */
    resp->result = QMI_RESULT_FAILURE_V01;
    resp->error  = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
  }
}

/*=============================================================================

  FUNCTION:  coex_handle_read_metrics_req_over_qmi

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to start collecting/
    collating SINR metrics. Returns true if the reponse is deferred.

    @return
    boolean
*/
/*===========================================================================*/
boolean coex_handle_read_metrics_req_over_qmi (
  coex_wwan_tech_v01    tech,
  qmi_response_type_v01 *resp,
  void          *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id
)
{
  boolean             msg_deferred = FALSE;
  qmi_csi_error       push_req_error;
  uint32              deferred_req_id;
  /*-----------------------------------------------------------------------*/
  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_QMI_METRICS) )
  {
    if ( tech != COEX_LTE_TECH_V01 && tech != COEX_TDSCDMA_TECH_V01 &&
         tech != COEX_GSM_TECH_V01 && tech != COEX_GSM2_TECH_V01 &&
         tech != COEX_GSM3_TECH_V01 )
    {
      /* tech from request not valid */
      resp->result = QMI_RESULT_FAILURE_V01;
      resp->error  = QMI_ERR_INVALID_ARG_V01;
    }
    /* check if currently doing SINR; if not, read_req is not appropriate */
    else if( atomic_read(&coex_wwan_state_info[tech].metrics_started) )
    {
      /* push qmi req info onto delayed response stack to recover when
         read cnf response from LTE received */
      push_req_error = mqcsi_conn_mgr_push_deferred_req( MQCSI_COEX_SERVICE_ID,
                         connection_handle,
                         req_handle,
                         msg_id,
                         &deferred_req_id );
      CXM_ASSERT( QMI_CSI_NO_ERR == push_req_error );

      /* ask tech to read the info for us */
      coex_tech_metrics_initiate_req( tech, CXM_ACTION_READ, 0, deferred_req_id );
      msg_deferred = TRUE;
    }
    else
    {
      /* SINR not started - send error response */
      resp->result = QMI_RESULT_FAILURE_V01;
      resp->error  = QMI_ERR_INVALID_OPERATION_V01;
    }
  }
  else
  {
    /* LTE metrics not supported over QMI interface */
    resp->result = QMI_RESULT_FAILURE_V01;
    resp->error  = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
  }

  return msg_deferred;
}

/*=============================================================================

  FUNCTION:  coex_handle_stop_metrics_req_over_qmi

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to stop collecting/
    collating SINR metrics

    @return
    none
*/
/*===========================================================================*/
void coex_handle_stop_metrics_req_over_qmi (
  coex_wwan_tech_v01    tech,
  qmi_response_type_v01 *resp
)
{
  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_QMI_METRICS) )
  {
    if ( tech != COEX_LTE_TECH_V01 && tech != COEX_TDSCDMA_TECH_V01 &&
         tech != COEX_GSM_TECH_V01 && tech != COEX_GSM2_TECH_V01 &&
         tech != COEX_GSM3_TECH_V01 )
    {
      /* tech from request not valid */
      resp->result = QMI_RESULT_FAILURE_V01;
      resp->error  = QMI_ERR_INVALID_ARG_V01;
    }
    /* REQ only appropriate if SINR has been started */
    else if( atomic_read(&coex_wwan_state_info[tech].metrics_started) )
    {
      /* tell LTE to stop SINR */
      coex_tech_metrics_initiate_req( tech, CXM_ACTION_STOP, 0, 0 );

      resp->result = QMI_RESULT_SUCCESS_V01;
      resp->error  = QMI_ERR_NONE_V01;

      CXM_MSG_1( HIGH, "SINR Stopped, tech=%d", tech );
    }
    else
    {
      /* SINR not started yet */
      resp->result = QMI_RESULT_FAILURE_V01;
      resp->error  = QMI_ERR_INVALID_OPERATION_V01;
    }
  }
  else
  {
    resp->result = QMI_RESULT_FAILURE_V01;
    resp->error  = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
  }
}

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
)
{
  qmi_csi_cb_error                         req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_metrics_lte_sinr_start_resp_msg_v01 response;
  coex_metrics_lte_sinr_start_req_msg_v01 *request;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( coex_metrics_lte_sinr_start_resp_msg_v01 ) );
  request = (coex_metrics_lte_sinr_start_req_msg_v01 *) req_c_struct;

  coex_handle_start_metrics_req_over_qmi ( COEX_LTE_TECH_V01, &response.resp, request->alpha );

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_metrics_lte_sinr_start_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_metrics_lte_sinr_read_resp_msg_v01 response;
  boolean        msg_deferred;
  /*-----------------------------------------------------------------------*/
  memset( &response, 0, sizeof( coex_metrics_lte_sinr_read_resp_msg_v01 ) );

  msg_deferred = coex_handle_read_metrics_req_over_qmi ( COEX_LTE_TECH_V01, &response.resp, 
                                                         connection_handle, req_handle, msg_id );
  if ( !msg_deferred  )
  {
    req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                      connection_handle,
                      req_handle,
                      msg_id,
                      &response,
                      sizeof( coex_metrics_lte_sinr_read_resp_msg_v01 )
                    );
    CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );
  }

  return req_cb_retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_lte_metric_sinr_stop_req

=============================================================================*/
/*!
    @brief
    Function to handle the client's query request to stop collecting/collating
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
)
{
  qmi_csi_cb_error                        req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_metrics_lte_sinr_stop_resp_msg_v01 response;
  /*-----------------------------------------------------------------------*/
    /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( coex_metrics_lte_sinr_stop_resp_msg_v01 ) );

  coex_handle_stop_metrics_req_over_qmi ( COEX_LTE_TECH_V01, &response.resp );

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_metrics_lte_sinr_stop_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error                req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_metrics_start_resp_msg_v01 response;
  coex_metrics_start_req_msg_v01  *request;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( coex_metrics_start_resp_msg_v01 ) );
  request = (coex_metrics_start_req_msg_v01 *) req_c_struct;

  coex_handle_start_metrics_req_over_qmi ( request->tech, &response.resp, request->alpha );

  /* set the tech the start request is for */
  response.tech_valid = TRUE;
  response.tech = request->tech;

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_metrics_start_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_metrics_read_resp_msg_v01 response;
  coex_metrics_read_req_msg_v01   *request;
  boolean        msg_deferred;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  memset( &response, 0, sizeof( coex_metrics_read_resp_msg_v01 ) );
  request = (coex_metrics_read_req_msg_v01 *) req_c_struct;

  msg_deferred = coex_handle_read_metrics_req_over_qmi ( request->tech, &response.resp, 
                                                         connection_handle, req_handle, msg_id );
  if ( !msg_deferred  )
  {
    response.tech_valid = TRUE;
    response.tech = request->tech;
    req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                      connection_handle,
                      req_handle,
                      msg_id,
                      &response,
                      sizeof( coex_metrics_read_resp_msg_v01 )
                    );
    CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );
  }

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error               req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_metrics_stop_resp_msg_v01 response;
  coex_metrics_stop_req_msg_v01  *request;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( coex_metrics_stop_resp_msg_v01 ) );
  request = (coex_metrics_stop_req_msg_v01 *) req_c_struct;

  coex_handle_stop_metrics_req_over_qmi ( request->tech, &response.resp );

  /* send QMI response */
  response.tech_valid = TRUE;
  response.tech = request->tech;
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_metrics_stop_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error                       req_cb_retval = QMI_CSI_CB_NO_ERR;
  qmi_csi_error                          qmi_retval = QMI_CSI_NO_ERR;
  coex_set_band_filter_info_resp_msg_v01 response;
  coex_set_band_filter_info_req_msg_v01 *request;
  uint8 i;
  uint8 ul_count = 0;
  uint8 dl_count = 0;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  /* De-mystify the received message pointer to the appropriate type */
  request = (coex_set_band_filter_info_req_msg_v01 *) req_c_struct;

  /* Init the memory available for the REQ and LTE ML1 message */
  memset( &response, 0, sizeof( coex_set_band_filter_info_resp_msg_v01 ) );

  /*clear existing band filter info*/  
  memset( &coex_band_filter_info, 0, sizeof( coex_band_filter_s ) );

  response.resp.result = QMI_RESULT_SUCCESS_V01;
  response.resp.error  = QMI_ERR_NONE_V01;

  /* forward band filter info to LTE ML1 */
  if(request->bands_valid)
  {
    if( request->bands_len > COEX_WWAN_MAX_BANDS_TO_MONITOR_V01 )
    {
      response.resp.result = QMI_RESULT_FAILURE_V01;
      response.resp.error  = QMI_ERR_INVALID_ARG_V01;
    }
    else
    {
      for( i = 0; i < request->bands_len; i++ )
      {
        if ( (request->bands[i].band_mask & COEX_ENABLE_UL_BAND_INFO_V01) &&  request->bands[i].band_info.ul_band.freq != 0 ) 
        {
          coex_band_filter_info.ul_filter[ul_count].freq_stop = 
            ( request->bands[i].band_info.ul_band.freq + (request->bands[i].band_info.ul_band.bandwidth/2) ) * MHZ_KHZ_CONVERSION;

          coex_band_filter_info.ul_filter[ul_count].freq_start = 
            ( request->bands[i].band_info.ul_band.freq - (request->bands[i].band_info.ul_band.bandwidth/2) ) * MHZ_KHZ_CONVERSION;

          ul_count++;

        }
        if ( (request->bands[i].band_mask & COEX_ENABLE_DL_BAND_INFO_V01) &&  request->bands[i].band_info.dl_band.freq != 0 ) 
        {
          coex_band_filter_info.dl_filter[dl_count].freq_stop = 
            ( request->bands[i].band_info.dl_band.freq + (request->bands[i].band_info.dl_band.bandwidth/2) ) * MHZ_KHZ_CONVERSION;

          coex_band_filter_info.dl_filter[dl_count].freq_start = 
            ( request->bands[i].band_info.dl_band.freq - (request->bands[i].band_info.dl_band.bandwidth/2) ) * MHZ_KHZ_CONVERSION;

          dl_count++;

        }
      }

      coex_band_filter_info.ul_len = ul_count;
      coex_band_filter_info.dl_len = dl_count;
      if (request->bands_len > 0) 
      {
        coex_band_filter_info.filter_on = TRUE;
      }
      else
      {
        coex_band_filter_info.filter_on = FALSE;
      }
    }
  }
  /*set the band filter for all techs*/
  for (i=0; i < CXM_TECH_MAX; i++)
  {
    coex_apply_tech_band_filter(i);
  }

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_set_band_filter_info_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  /* Prepare & send out the IND message*/
  qmi_retval = coex_prep_and_send_wwan_state_ind( TRUE );

  /* although we lose info here, it is recorded in an f3 inside
   * the above function */
  if( QMI_CSI_NO_ERR != qmi_retval )
  {
    req_cb_retval = QMI_CSI_CB_INTERNAL_ERR;
  }

  CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_BAND_FILTER_INFO);

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error                       req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_get_band_filter_info_resp_msg_v01 response;
  uint8 i;
  /*-----------------------------------------------------------------------*/
  /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( coex_get_band_filter_info_resp_msg_v01 ) );

  response.resp.result = QMI_RESULT_SUCCESS_V01;
  response.resp.error  = QMI_ERR_NONE_V01;

  response.bands_valid = TRUE;

  for (i=0; i < coex_band_filter_info.ul_len; i++) 
  {
    response.bands[i].band_info.ul_band.freq      = 
      ( (coex_band_filter_info.ul_filter[i].freq_stop + coex_band_filter_info.ul_filter[i].freq_start)/2 ) / MHZ_KHZ_CONVERSION;
    response.bands[i].band_info.ul_band.bandwidth = 
      ( coex_band_filter_info.ul_filter[i].freq_stop - coex_band_filter_info.ul_filter[i].freq_start ) / MHZ_KHZ_CONVERSION;
    response.bands[i].band_mask |= COEX_ENABLE_UL_BAND_INFO_V01;
  }

  for (i=0; i < coex_band_filter_info.dl_len; i++) 
  {
    response.bands[i].band_info.dl_band.freq      = 
      ( (coex_band_filter_info.dl_filter[i].freq_stop + coex_band_filter_info.dl_filter[i].freq_start)/2 ) / MHZ_KHZ_CONVERSION;
    response.bands[i].band_info.dl_band.bandwidth = 
      ( coex_band_filter_info.dl_filter[i].freq_stop - coex_band_filter_info.dl_filter[i].freq_start ) / MHZ_KHZ_CONVERSION;
    response.bands[i].band_mask |= COEX_ENABLE_DL_BAND_INFO_V01;
  }

  response.bands_len = coex_band_filter_info.ul_len > coex_band_filter_info.dl_len ? 
                          coex_band_filter_info.ul_len : coex_band_filter_info.dl_len;
  
  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_get_band_filter_info_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error                      req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_get_wci2_mws_params_resp_msg_v01 response;
  uint8                                 char_jitter;
  wci2_uart_baud_type_e                 baud_rate;
  /*-----------------------------------------------------------------------*/
  /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( coex_get_wci2_mws_params_resp_msg_v01 ) );

  response.resp.result = QMI_RESULT_SUCCESS_V01;
  response.resp.error  = QMI_ERR_NONE_V01;

  baud_rate = wci2_uart_get_baud();

  if(baud_rate == WCI2_UART_BAUD_115200)
  {
    char_jitter = COEX_UART_JITTER_115200;
  }
  else if(baud_rate == WCI2_UART_BAUD_2000000)
  {
    char_jitter = COEX_UART_JITTER_2000000;
  }
  else
  {
    /* 3Mbps */
    char_jitter = COEX_UART_JITTER_3000000;
  }

  /* fill in params. Note: all values in uSec */
  response.mws_frame_sync_assert_offset_valid = TRUE;
  response.mws_frame_sync_assert_offset.min = -60;
  response.mws_frame_sync_assert_offset.max = -60;
  response.mws_frame_sync_assert_jitter_valid = TRUE;
  response.mws_frame_sync_assert_jitter.min = -3;
  response.mws_frame_sync_assert_jitter.max =  3;
  response.mws_rx_assert_offset_valid = TRUE;
  response.mws_rx_assert_offset.min   = -40;
  response.mws_rx_assert_offset.max   = -40;
  response.mws_rx_assert_jitter_valid = TRUE;
  response.mws_rx_assert_jitter.min   = -char_jitter;
  response.mws_rx_assert_jitter.max   =  char_jitter;
  response.mws_rx_deassert_offset_valid = TRUE;
  response.mws_rx_deassert_offset.min = 40;
  response.mws_rx_deassert_offset.max = 40;
  response.mws_rx_deassert_jitter_valid = TRUE;
  response.mws_rx_deassert_jitter.min = -char_jitter;
  response.mws_rx_deassert_jitter.max =  char_jitter;
  response.mws_tx_assert_offset_valid = TRUE;
  response.mws_tx_assert_offset.min   = -50;
  response.mws_tx_assert_offset.max   = -50;
  response.mws_tx_assert_jitter_valid = TRUE;
  response.mws_tx_assert_jitter.min   = -char_jitter;
  response.mws_tx_assert_jitter.max   =  char_jitter;
  response.mws_tx_deassert_offset_valid = TRUE;
  response.mws_tx_deassert_offset.min = -50;
  response.mws_tx_deassert_offset.max = -50;
  response.mws_tx_deassert_jitter_valid = TRUE;
  response.mws_tx_deassert_jitter.min = -char_jitter;
  response.mws_tx_deassert_jitter.max =  char_jitter;

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_get_wci2_mws_params_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}


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
)
{
  qmi_csi_cb_error                           req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_set_sleep_notification_resp_msg_v01   response;
  coex_set_sleep_notification_req_msg_v01   *request;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );  

  /* De-mystify the received message pointer to the appropriate type */
  request = (coex_set_sleep_notification_req_msg_v01 *) req_c_struct;  

  /* Init the memory available for the resp message */
  memset( &response, 0, sizeof( coex_set_sleep_notification_resp_msg_v01 ) );
  
  if( request->tech > (coex_wwan_tech_v01)CXM_TECH_DFLT_INVLD && 
              request->tech < (coex_wwan_tech_v01)CXM_TECH_MAX )
  {  
    /*check the technolgy entry and store in the appropriate field*/
    coex_wwan_state_info[request->tech].sleep_thresh = request->off_period_threshold;
    /*internal support for GSM2 aswell*/
    if (request->tech == COEX_GSM_TECH_V01)
    { 
      coex_wwan_state_info[CXM_TECH_GSM2].sleep_thresh = request->off_period_threshold;
    }  
  
    response.resp.result = QMI_RESULT_SUCCESS_V01;
    response.resp.error  = QMI_ERR_NONE_V01;
  }
  else
  {
    response.resp.result = QMI_RESULT_FAILURE_V01;
    response.resp.error  = QMI_ERR_INVALID_ARG_V01;
  }
  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_set_sleep_notification_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  CXM_TRACE( CXM_TRC_QMI_SET_SLP_NOTFY_THRESH, request->tech, 
                   request->off_period_threshold, 0, CXM_TRC_EN );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error                           req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_get_sleep_notification_resp_msg_v01   response;
  coex_get_sleep_notification_req_msg_v01   *request;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  /* De-mystify the received message pointer to the appropriate type */
  request = (coex_get_sleep_notification_req_msg_v01 *) req_c_struct;  

  /* Init the memory available for the resp message */
  memset( &response, 0, sizeof( coex_get_sleep_notification_resp_msg_v01 ) );

  /*explicitly set optional fields to FALSE*/
  response.off_period_threshold_valid = FALSE;
  response.tech_valid = FALSE;
  
  if( request->tech > (coex_wwan_tech_v01)CXM_TECH_DFLT_INVLD && 
              request->tech < (coex_wwan_tech_v01)CXM_TECH_MAX )
  {   
    /*check the technolgy entry and store in the appropriate field*/
    response.off_period_threshold = coex_wwan_state_info[request->tech].sleep_thresh;
    response.off_period_threshold_valid = TRUE;
    response.tech =  request->tech;
    response.tech_valid = TRUE;

    response.resp.result = QMI_RESULT_SUCCESS_V01;
    response.resp.error  = QMI_ERR_NONE_V01;
  }
  else
  {
    response.resp.result = QMI_RESULT_FAILURE_V01;
    response.resp.error  = QMI_ERR_INVALID_ARG_V01;
  }

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_get_sleep_notification_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error                 req_cb_retval = QMI_CSI_CB_NO_ERR;
#ifdef T_WINNT
  #error code not present
#else
  coex_wcn_wake_sync_resp_msg_v01  response = {
    .resp.result = QMI_RESULT_SUCCESS_V01,
    .resp.error  = QMI_ERR_NONE_V01
  };
#endif
  coex_wcn_wake_sync_req_msg_v01 * request;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  /* de-mystify pointer */
  request = (coex_wcn_wake_sync_req_msg_v01 *) req_c_struct;

#ifdef T_WINNT
  #error code not present
#endif

  /* save the WCN info from the request */
  coex_wcn_wake_sync_info.scan_enabled = request->scan_enabled;

  /* only save the page scan interval if scan_enabled == TRUE */
  if (request->scan_interval_valid && request->scan_enabled)
  {
    coex_wcn_wake_sync_info.scan_interval = request->scan_interval;
    /* reset last timestamp, so first IND will be sent at next
     * WWAN wakeup */
    coex_wcn_wake_sync_info.last_msg_ts = 0;
  }

  CXM_MSG_2( HIGH, "Wake Sync REQ, enable=%d interval=%d", 
             request->scan_enabled, request->scan_interval );

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_wcn_wake_sync_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error                        req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_get_conflict_params_resp_msg_v01   response;
  uint8 i, j;
  coex_get_conflict_params_req_msg_v01    *request;
  uint32 num_remaining_entries;
  uint32 ending_index = 0;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  /* De-mystify the received message pointer to the appropriate type */
  request = (coex_get_conflict_params_req_msg_v01 *) req_c_struct;

  /* Init the memory available for the resp message */
  memset( &response, 0, sizeof( coex_get_conflict_params_resp_msg_v01 ) );

  response.wcn_behavior_valid = TRUE;
  response.resp.result = QMI_RESULT_SUCCESS_V01;
  response.resp.error  = QMI_ERR_NONE_V01;

  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_SMEM_DATA) )
  {
    response.wcn_behavior |= COEX_WCN_BHVR_SMEM_DATA_V01;
    if ( request->victim_tbl_offset >= coex_params.num_conflicts )
    {
      response.resp.result = QMI_RESULT_FAILURE_V01;
      response.resp.error  = QMI_ERR_INVALID_ARG_V01;
    }
    else
    {
      num_remaining_entries = coex_params.num_conflicts - request->victim_tbl_offset;
      response.victim_tbl_valid = TRUE;
      response.victim_tbl_len = COEX_MAX_VICTIM_TBL_ENTRIES_V01 > num_remaining_entries?
                                num_remaining_entries : COEX_MAX_VICTIM_TBL_ENTRIES_V01;
      response.victim_tbl_complete_size_valid = TRUE;
      response.victim_tbl_complete_size = coex_params.num_conflicts;
      response.victim_tbl_offset_valid = TRUE;
      response.victim_tbl_offset = request->victim_tbl_offset;
      
      ending_index = request->victim_tbl_offset + response.victim_tbl_len - 1;
      for ( i = request->victim_tbl_offset, j = 0; i <= ending_index; i++, j++ )
      {
        response.victim_tbl[j].wcn = coex_params.victim_tbl[i].wcn;
        response.victim_tbl[j].wwan = coex_params.victim_tbl[i].wwan;
        response.victim_tbl[j].wcn_params = coex_params.victim_tbl[i].wcn_params;
        response.victim_tbl[j].mdm_params = coex_params.victim_tbl[i].mdm_params;
        response.victim_tbl[j].mdm_policy = coex_params.victim_tbl[i].mdm_policy;
        response.victim_tbl[j].wcn_policy = coex_params.victim_tbl[i].wcn_policy;
      }
    }
  }
  else
  {
    response.wcn_behavior |= COEX_WCN_BHVR_NONE_V01;
  }

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_get_conflict_params_resp_msg_v01 )
                  );
  CXM_MSG_2( HIGH, "Sent config to WCN, behavior=%d return_val=%d", 
             response.wcn_behavior, req_cb_retval );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );
  /*if we are in SMEM system behavior and all conflict data has been transferred, init SMEM*/
  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_SMEM_DATA) && ( ( ending_index + 1 ) == coex_params.num_conflicts ) )
  {
    cxm_smem_init();
  }

  return req_cb_retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_sleep_wakeup_ind

=============================================================================*/
/*!
    @brief
    Function to send out the sleep or wakeup indication message and
    send through QMI
 
    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type coex_handle_sleep_wakeup_ind (
  void *rcv_msgr_msg_ptr
)
{
  cxm_coex_tech_sleep_wakeup_duration_ind_s *msg_ptr;
  errno_enum_type         retval = E_SUCCESS;
  qmi_csi_error           qmi_retval = QMI_CSI_NO_ERR;
  coex_sleep_ind_msg_v01  sleep_msg;
  coex_wakeup_ind_msg_v01 wakeup_msg;
  wci2_msg_type_s         wci2_msg;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_tech_sleep_wakeup_duration_ind_s *) rcv_msgr_msg_ptr;

  CXM_MSG_3( HIGH, "Sleep duration rcvd, tech: %d is_going_to_sleep: %d, duration: %d", 
             msg_ptr->tech_id, msg_ptr->is_going_to_sleep, msg_ptr->duration );

  cxm_counter_event( CXM_CNT_LTE_ML1_SLEEP_DURN_RECVD, msg_ptr->duration );

  /* update sleep/wake state for this tech and handle UART power on/off */
  if( msg_ptr->is_going_to_sleep )
  {
    coex_state.active_tech_mask &= ~(1<<msg_ptr->tech_id);
  }
  else
  {
    coex_state.active_tech_mask |= 1<<msg_ptr->tech_id;
  }

  if ( ( msg_ptr->is_going_to_sleep && (msg_ptr->duration >= coex_wwan_state_info[msg_ptr->tech_id].sleep_thresh) ) 
         || !(msg_ptr->is_going_to_sleep) )
  {
    if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) ||
       COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_CONTROL) )
    {
      /* if possibly waking from sleep, make sure to power up before sending */
      if ( !msg_ptr->is_going_to_sleep )
      {
        coex_manage_uart_npa_vote();
      }

      if ( coex_wwan_state_info[msg_ptr->tech_id].policy & COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN_V01 &&
           CXM_TECH_LTE == msg_ptr->tech_id )
      {
        /* if type3 policy set, then send the data out uart (LTE only) */
        wci2_msg.type = WCI2_TYPE3;
        wci2_msg.data.type3_inact_durn = (uint16) msg_ptr->duration;
        wci2_send_msg( &wci2_msg );
      }

      if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_CONTROL) )
      {
        /* send sync msg to WLAN on both sleep and wake */
        coex_process_wwan_sleep_wake_v2( msg_ptr );
      }

      /* if possibly entering sleep, make sure to power down after sending */
      if ( msg_ptr->is_going_to_sleep )
      {
        coex_manage_uart_npa_vote();
      }
    }
    else if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_SMEM_DATA) && 
              coex_wwan_state_info[msg_ptr->tech_id].policy & COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN_V01)
    {
      cxm_smem_handle_sleep_wakeup( msg_ptr );
    }
  }

  /*if it is a sleep duration, check the time against the threshold val*/
  if (msg_ptr->is_going_to_sleep) 
  {
    if (msg_ptr->duration >= coex_wwan_state_info[msg_ptr->tech_id].sleep_thresh ) 
    {
      /* send the sleep message to QMI */
      /* Reset the memory available for the IND message */
      memset( &sleep_msg, 0, sizeof( coex_sleep_ind_msg_v01 ) );

      /* populate the sleep msg */
      sleep_msg.off_period = msg_ptr->duration;
      if (msg_ptr->tech_id <= CXM_TECH_WCDMA)
      {
        sleep_msg.tech = (coex_wwan_tech_v01) msg_ptr->tech_id;
      }
      /* internal support for GSM2 */
      else if (msg_ptr->tech_id == CXM_TECH_GSM2)
      {
        sleep_msg.tech = COEX_GSM_TECH_V01; 
      }

      /* send the QMI message */
      qmi_retval = mqcsi_conn_mgr_send_ind (
                  MQCSI_COEX_SERVICE_ID,
                  QMI_COEX_SLEEP_IND_V01,
                  &sleep_msg,
                  sizeof ( coex_sleep_ind_msg_v01 )
              );
      CXM_ASSERT( QMI_CSI_NO_ERR == qmi_retval );
    }
  }

  /*else if it is a wakeup msg*/
  else if (!msg_ptr->is_going_to_sleep)
  {
    /* If an LTE wakeup, want to sync WCN and WWAN wakeups to save power.
     * We only do this if both LTE and BT are doing paging (LTE in idle DRX)
     * (FR 2288) */
    if ( CXM_TECH_LTE == msg_ptr->tech_id && msg_ptr->params.lte.idle_flag &&
         coex_wcn_wake_sync_info.scan_enabled )
    {
      qmi_retval = coex_prep_and_send_wcn_wake_sync_ind( msg_ptr->params.lte.page_cycle );
      CXM_ASSERT( QMI_CSI_NO_ERR == qmi_retval );
    }

     /*send the wakeup message to QMI*/
     /* Reset the memory available for the IND message */
     memset( &wakeup_msg, 0, sizeof( coex_wakeup_ind_msg_v01 ) );

     /*populate the wakeup msg*/
     wakeup_msg.time_to_wakeup = msg_ptr->duration;
     if (msg_ptr->tech_id <= CXM_TECH_WCDMA)
     {
       wakeup_msg.tech = (coex_wwan_tech_v01) msg_ptr->tech_id;
     }
     /*internal support for GSM2*/
     else if (msg_ptr->tech_id == CXM_TECH_GSM2)
     {
       wakeup_msg.tech = COEX_GSM_TECH_V01;
     }

     /* send the QMI message */
     qmi_retval = mqcsi_conn_mgr_send_ind (
               MQCSI_COEX_SERVICE_ID,
               QMI_COEX_WAKEUP_IND_V01,
               &wakeup_msg,
               sizeof ( coex_wakeup_ind_msg_v01 )
             );
     CXM_ASSERT( QMI_CSI_NO_ERR == qmi_retval );
  }

  CXM_TRACE( CXM_TRC_MSGR_WWAN_TECH_SLEEP_WAKEUP, msg_ptr->tech_id, 
             msg_ptr->is_going_to_sleep, msg_ptr->duration,
             CXM_TRC_AND_PKT_EN, CXM_LOG_STATE );

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_update_coex_wwan_state_info_ind

=============================================================================*/
/*!
    @brief
    Function to update the WWAN state info & if needed send out the 
    COEX_WWAN_STATE info IND message
 
    @return
    qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type coex_update_wwan_state_info_ind (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type            retval = E_SUCCESS;
  qmi_csi_error              qmi_retval = QMI_CSI_NO_ERR;
  boolean                    state_changed = FALSE;
  cxm_tech_data_s            filtered_tech_data;
  int32                      lte_frame_offset_delta = 0;
  unsigned int               i, j;
  cxm_wwan_tech_state_ind_s *msg_ptr;
  cxm_coex_tech_sleep_wakeup_duration_ind_s sleep_wake_ind;
  cxm_tech_conn_state_e      new_conn_state;
  uint32                     trc_code = 0;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_wwan_tech_state_ind_s *) rcv_msgr_msg_ptr;

  CXM_ASSERT( msg_ptr->tech_id > CXM_TECH_DFLT_INVLD && 
              msg_ptr->tech_id < CXM_TECH_MAX &&
              msg_ptr->tech_data.num_link_info_sets <= CXM_MAX_SUPPORTED_LINK_SETS );

  /* copy relevant tech data into temporary local data structure, filtering
   * out unused info. Use this filtered info to determine if anything we care
   * about has changed */
  memset( &filtered_tech_data, 0, sizeof(cxm_tech_data_s) );
  for( i = 0, j = 0; i < msg_ptr->tech_data.num_link_info_sets; i++ )
  {
    if( CXM_LNK_TYPE_UNUSED == msg_ptr->tech_data.link_list[i].type )
    {
      filtered_tech_data.link_list[j++] = msg_ptr->tech_data.link_list[i];
    }
  }
  filtered_tech_data.num_link_info_sets = j;
  filtered_tech_data.params = msg_ptr->tech_data.params;

  CXM_MSG_9( HIGH, "Received WWAN state update, tech: %d, len: %d, filter_len: %d, "
                   "Entry 1: freq: %d, bandwidth: %d, dir: %d "
                   "Entry 2: freq: %d, bandwidth: %d, dir: %d",
                   msg_ptr->tech_id,
                   msg_ptr->tech_data.num_link_info_sets,
                   filtered_tech_data.num_link_info_sets,
                   filtered_tech_data.link_list[0].frequency,
                   filtered_tech_data.link_list[0].bandwidth,
                   filtered_tech_data.link_list[0].direction,
                   filtered_tech_data.link_list[1].frequency,
                   filtered_tech_data.link_list[1].bandwidth,
                   filtered_tech_data.link_list[1].direction);

  /* For WWAN tech LTE check if change in frame_offset is greater than 2 units 
     This helps filtering minor updates or rounding off updates to other side */
  if( msg_ptr->tech_id == CXM_TECH_LTE )
  {
    lte_frame_offset_delta = filtered_tech_data.params.lte_params.frame_offset -
          coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.frame_offset;
    lte_frame_offset_delta = ((lte_frame_offset_delta >= 0)?lte_frame_offset_delta: (-(lte_frame_offset_delta)));
    if( lte_frame_offset_delta <= COEX_TECH_LTE_FRAME_OFFSET_DIFF_THRESH ) 
    {
      /* Update incoming "frame offset" with previously sent value */
      filtered_tech_data.params.lte_params.frame_offset = coex_wwan_state_info[CXM_TECH_LTE].state_data.params.lte_params.frame_offset;
    }
  }

  /* -Determine the tech and copy in current state info as it is in the correct 
      local state info object if it has changed.
     -Note that we need to know if state has changed to determine whether to run 
      the process_conflicts algorithm */
  if( memcmp( &filtered_tech_data, 
              &coex_wwan_state_info[msg_ptr->tech_id].state_data, 
              sizeof(msg_ptr->tech_data) ) )
  {
    coex_wwan_state_info[msg_ptr->tech_id].state_data = filtered_tech_data;

    /* parse link list for UL/DL to determine tech state (conn/idle/inactive) */
    new_conn_state = CXM_TECH_STATE_INACTIVE;
    for( i = 0; i < filtered_tech_data.num_link_info_sets; i++ )
    {
      if( filtered_tech_data.link_list[i].direction == CXM_LNK_DRCTN_UL || 
          filtered_tech_data.link_list[i].direction == CXM_LNK_DRCTN_UL_AND_DL )
      {
        new_conn_state = CXM_TECH_STATE_CONN;
        break;
      }

      if( filtered_tech_data.link_list[i].direction == CXM_LNK_DRCTN_DL )
      {
        new_conn_state = CXM_TECH_STATE_IDLE;
      }
    }

    /* we don't get slpc sleep notifications when tech leaves the system or wake 
     * notifications when tech enters the system for the first time, so trigger ourselves */
    /* LTE sends us their own SLEEP/WAKE notifications, so this doesn't apply for LTE */
    if( msg_ptr->tech_id != CXM_TECH_LTE )
    {
      if( coex_wwan_state_info[msg_ptr->tech_id].conn_state == CXM_TECH_STATE_INACTIVE && 
          new_conn_state != CXM_TECH_STATE_INACTIVE )
      {
        /* when entering the system, need to process wake BEFORE we finish processing this
         * state update, so call the wake handler directly */
        sleep_wake_ind.tech_id = msg_ptr->tech_id;
        sleep_wake_ind.is_going_to_sleep = FALSE;
        sleep_wake_ind.duration = 0;
        retval = coex_handle_sleep_wakeup_ind( &sleep_wake_ind );
      }
      else if( coex_wwan_state_info[msg_ptr->tech_id].conn_state != CXM_TECH_STATE_INACTIVE &&
               new_conn_state == CXM_TECH_STATE_INACTIVE )
      {
        /* process sleep after we finish processing the current state */
        sleep_wake_ind.tech_id = msg_ptr->tech_id;
        sleep_wake_ind.is_going_to_sleep = TRUE;
        sleep_wake_ind.duration = 0xffffffff; /* infinite duraton */
        retval = cxm_msgr_send_msg( &sleep_wake_ind.msg_hdr, 
                   MCS_CXM_COEX_TECH_SLEEP_WAKEUP_IND, 
                   sizeof(cxm_coex_tech_sleep_wakeup_duration_ind_s) );
      }
    }
    coex_wwan_state_info[msg_ptr->tech_id].conn_state = new_conn_state;

    /* if we're running control plane over WCI-2, process this state change
     * for that interface as well */
    if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_CONTROL) )
    {
      coex_process_wwan_state_info_msg_v2( msg_ptr->tech_id, &filtered_tech_data );
    }

#ifdef FEATURE_COEX_USE_NV
    /* check for conflicts and mitigate if necessary. Do this for 3-wire mode only */
    if( COEX_SYS_ENABLED(CXM_SYS_BHVR_VICTIM_TABLE) && coex_params.num_conflicts != 0 )
    {
      coex_process_conflicts(msg_ptr->tech_id, CXM_TECH_WIFI);
    }
#endif

    /* apply band filter and send QMI indication based on new tech state */
    coex_apply_tech_band_filter(msg_ptr->tech_id);
    qmi_retval = coex_prep_and_send_wwan_state_ind( FALSE );
    retval = ( QMI_CSI_NO_ERR != qmi_retval ? E_FAILURE : retval );

    state_changed = TRUE;

    /* log the new WWAN state info*/
    switch( msg_ptr->tech_id )
    {
      case CXM_TECH_LTE:
        trc_code = CXM_LOG_LTE_WWAN_STATE_INFO;
        break;
      case CXM_TECH_TDSCDMA:
        trc_code = CXM_LOG_TDS_WWAN_STATE_INFO;
        break;
      case CXM_TECH_GSM1:
        trc_code = CXM_LOG_GSM1_WWAN_STATE_INFO;
        break;
      case CXM_TECH_GSM2:
        trc_code = CXM_LOG_GSM2_WWAN_STATE_INFO;
        break;
      case CXM_TECH_GSM3:
        trc_code = CXM_LOG_GSM3_WWAN_STATE_INFO;
        break;
      default:
        trc_code = CXM_LOG_LTE_WWAN_STATE_INFO;
        break;
    }
    CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN,
             CXM_LOG_STATE, trc_code );
  }

  CXM_TRACE(CXM_TRC_MSGR_WWAN_TECH_STATE_INFO, msg_ptr->tech_id, state_changed,
             filtered_tech_data.num_link_info_sets, CXM_TRC_EN);

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_sfn_ind

=============================================================================*/
/*!
    @brief
    Function to send out wci2 type 6 msg when SFN IND received

    @return qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type coex_handle_tx_adv_ntc_ind (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                 retval = E_SUCCESS;
  cxm_coex_tech_tx_adv_ntc_ind_s *msg_ptr;
  boolean                         thresh_check_passed = FALSE;
  uint8                           i;
  uint8                           active_conf_index;
  wci2_msg_type_s                 wci2_msg;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_tech_tx_adv_ntc_ind_s *) rcv_msgr_msg_ptr;
  cxm_counter_event( CXM_CNT_LTE_ML1_ADVANCE_NOTICE_RECVD, msg_ptr->params.lte.sfn );
  CXM_MSG_3( HIGH, " Advance Tx notification for %d sfn, %d rb & %d tx_power",
             msg_ptr->params.lte.sfn, msg_ptr->params.lte.rbs, msg_ptr->params.lte.tx_power );

  /* log tx adv */
  cxm_tx_adv_log.sfn = msg_ptr->params.lte.sfn;
  cxm_tx_adv_log.transmit = msg_ptr->params.lte.transmit;
  cxm_tx_adv_log.ustmr_time = msg_ptr->params.lte.ustmr_time;
  cxm_tx_adv_log.rbs = msg_ptr->params.lte.rbs;
  cxm_tx_adv_log.tx_power = msg_ptr->params.lte.tx_power;
  CXM_TRACE( CXM_TRC_MSGR_LTE_SFN, msg_ptr->params.lte.sfn, 0, 0,
             CXM_TRC_AND_PKT_EN, CXM_LOG_LTE_TX_ADV );

  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_CXM_ASSERTS_TX_ADV) )
  {
    coex_assert_lte_tx_adv( rcv_msgr_msg_ptr );
  }
  else if( COEX_SYS_ENABLED(CXM_SYS_BHVR_TX_ADV_ALL_SUBFRAMES) )
  {
    /* send message for both TX and non-TX subframes in this mode. Use 5th
     * bit to specify which is the case */

    /* Only mark as a TX if all of the following are true:
     *  - Is a TX subframe
     *  - #RBs within range in victim table
     *  - LTE TX power >= threshold in NV */
    active_conf_index = coex_wwan_state_info[CXM_TECH_LTE].active_conf_index;
    if( msg_ptr->params.lte.transmit && (msg_ptr->params.lte.tx_power >= 
        coex_params.victim_tbl[active_conf_index].mdm_params.tx_adv.tx_power_thresh) )
    {
      for(i = 0; i < coex_params.victim_tbl[active_conf_index].mdm_params.tx_adv.rb_thresh_len; i++)
      {
        if ( msg_ptr->params.lte.rbs >= coex_params.victim_tbl[active_conf_index].mdm_params.tx_adv.rb_thresh[i].start && 
             msg_ptr->params.lte.rbs <= coex_params.victim_tbl[active_conf_index].mdm_params.tx_adv.rb_thresh[i].end )
        {
          /* all checks passed! */
          thresh_check_passed = TRUE;
          break;
        } /* rb check */
      } /* rb loop */
    } /* transmit + power limit check */

    CXM_MSG_6( MED, "Type 6 TX adv: sfn=%d, tx=%d, tx_pwr=%d, tx_pwr_thresh=%d, rbs=%d, t6_tx=%d",
               msg_ptr->params.lte.sfn, msg_ptr->params.lte.transmit, msg_ptr->params.lte.tx_power, 
               coex_params.victim_tbl[active_conf_index].mdm_params.tx_adv.tx_power_thresh,
               msg_ptr->params.lte.rbs, thresh_check_passed );

    if( COEX_SYS_ENABLED(CXM_SYS_BHVR_CXM_SENDS_TYPE6) )
    {
      /* send message for both TX and non-TX subframes in this mode. Use 5th
       * bit to specify which is the case */
      wci2_msg.type = WCI2_TYPE6;
      wci2_msg.data.type6_adv_tx_sfn.sfn = msg_ptr->params.lte.sfn;
      wci2_msg.data.type6_adv_tx_sfn.tx = thresh_check_passed;

      /* send the completed type 6 message */
      wci2_send_msg( &wci2_msg );
    }
    else if( COEX_SYS_ENABLED(CXM_SYS_BHVR_SMEM_DATA) )
    {
      cxm_smem_handle_lte_tx_adv( &(msg_ptr->params.lte), thresh_check_passed );
    }
  }
  else if( COEX_SYS_ENABLED(CXM_SYS_BHVR_CXM_SENDS_TYPE6) )
  {
    if ( msg_ptr->params.lte.rbs >= 
         coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.rb_threshold_for_adv_tx_notice &&
         msg_ptr->params.lte.tx_power >= 
         coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.tx_power_threshold_for_adv_tx_notice )
    {
      /* normal ML1 protocol mode - only send TX subframes. TX is 
       * implied, so do not fill in extra TX boolean */
      wci2_msg.type = WCI2_TYPE6;
      wci2_msg.data.type6_adv_tx_sfn.sfn = msg_ptr->params.lte.sfn;
      wci2_msg.data.type6_adv_tx_sfn.tx = FALSE;
      wci2_send_msg( &wci2_msg );

      CXM_MSG_1( HIGH, "Advance Tx notification sent out for %d LTE sfn", msg_ptr->params.lte.sfn );
    }
    else
    {
      CXM_MSG_1( HIGH, "Advance Tx notification filtered out for %d LTE sfn", msg_ptr->params.lte.sfn );
    }
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_activity_timeline_ind

=============================================================================*/
/*!
    @brief
    Handle activity timeline message 

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_activity_timeline_ind (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type     retval = E_SUCCESS;
  cxm_timing_info_s   *msg_ptr;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_timing_info_s *) rcv_msgr_msg_ptr;

  /* make sure policy is enabled because activity timeline message can come in when 
     policy is not enabled */
  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_SMEM_DATA) 
       && (coex_wwan_state_info[msg_ptr->tech_id].policy & 
           CXM_POLICY_WCI2_OUTGNG_TYPE6_TX_ADV_NOTICE) )
  {
    cxm_smem_handle_activity_timeline( msg_ptr );
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_bler_ind

=============================================================================*/
/*!
    @brief
    Forward BLER indication from LTE to AP over QMI as part of metrics

    @return qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type coex_handle_bler_ind (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                         retval = E_SUCCESS;
  cxm_coex_metrics_lte_bler_ind_s         *msg_ptr;
  qmi_csi_error                           qmi_retval = QMI_CSI_NO_ERR;
  coex_metrics_lte_bler_ind_msg_v01 indication;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_metrics_lte_bler_ind_s *) rcv_msgr_msg_ptr;

  CXM_TRACE( CXM_TRC_MSGR_LTE_BLER, 0, msg_ptr->tb_err_count, msg_ptr->tb_count,
             CXM_TRC_EN );

  /* Reset the memory available for the IND message */
  memset( &indication, 0, sizeof( coex_metrics_lte_bler_ind_msg_v01 ) );

  indication.tb_cnt_valid         = TRUE;
  indication.tb_cnt               = msg_ptr->tb_count;
  indication.errored_tb_cnt_valid = TRUE;
  indication.errored_tb_cnt       = msg_ptr->tb_err_count;

  if (msg_ptr->tb_count_bt)
  {
     indication.tb_cnt_bt_only_valid = TRUE;
     indication.tb_cnt_bt_only       = msg_ptr->tb_count_bt;
     indication.errored_tb_cnt_bt_only_valid = TRUE;
     indication.errored_tb_cnt_bt_only = msg_ptr->tb_err_count_bt;
  }
  if (msg_ptr->tb_count_wifi)
  {
     indication.tb_cnt_wifi_only_valid = TRUE;
     indication.tb_cnt_wifi_only       = msg_ptr->tb_count_wifi;
     indication.errored_tb_cnt_wifi_only_valid = TRUE;
     indication.errored_tb_cnt_wifi_only = msg_ptr->tb_err_count_wifi;
  }
  if (msg_ptr->tb_count_bt_wifi)
  {
     indication.tb_cnt_bt_wifi_valid = TRUE;
     indication.tb_cnt_bt_wifi      = msg_ptr->tb_count_bt_wifi;
     indication.errored_tb_cnt_bt_wifi_valid = TRUE;
     indication.errored_tb_cnt_bt_wifi = msg_ptr->tb_err_count_bt_wifi;
  }
  if (msg_ptr->tb_count_lte_only)
  {
     indication.tb_cnt_lte_only_valid = TRUE;
     indication.tb_cnt_lte_only      = msg_ptr->tb_count_lte_only;
     indication.errored_tb_cnt_lte_only_valid = TRUE;
     indication.errored_tb_cnt_lte_only = msg_ptr->tb_err_count_lte_only;
  }

  qmi_retval = mqcsi_conn_mgr_send_ind (
                 MQCSI_COEX_SERVICE_ID,
                 QMI_COEX_METRICS_LTE_BLER_IND_V01,
                 &indication,
                 sizeof ( coex_metrics_lte_bler_ind_msg_v01 )
               );
  CXM_ASSERT( QMI_CSI_NO_ERR == qmi_retval );

  if(qmi_retval != QMI_CSI_NO_ERR)
  {
    retval = E_FAILURE;
  }

  CXM_MSG_3( HIGH, "Send Metrics BLER stats; tb_cnt=%d, err_cnt=%d IND return %d", 
             msg_ptr->tb_count, msg_ptr->tb_err_count, qmi_retval );

  /* log BLER */
  cxm_lte_bler_log.tb_count = msg_ptr->tb_count;
  cxm_lte_bler_log.tb_err_count = msg_ptr->tb_err_count;
  cxm_lte_bler_log.tb_count_bt = msg_ptr->tb_count_bt;
  cxm_lte_bler_log.tb_err_count_bt = msg_ptr->tb_err_count_bt;
  cxm_lte_bler_log.tb_count_wifi = msg_ptr->tb_count_wifi;
  cxm_lte_bler_log.tb_err_count_wifi = msg_ptr->tb_err_count_wifi;
  cxm_lte_bler_log.tb_count_bt_wifi = msg_ptr->tb_count_bt_wifi;
  cxm_lte_bler_log.tb_err_count_bt_wifi = msg_ptr->tb_err_count_bt_wifi;
  cxm_lte_bler_log.tb_count_lte_only = msg_ptr->tb_count_lte_only;
  cxm_lte_bler_log.tb_err_count_lte_only = msg_ptr->tb_err_count_lte_only;
  CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_LTE_BLER_READ );

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_lte_sinr_read_cnf

=============================================================================*/
/*!
    @brief
    Function to send QMI message with SINR stats from LTE

    @return qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type coex_handle_lte_sinr_read_cnf (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                             retval = E_SUCCESS;
  cxm_coex_metrics_lte_sinr_rsp_s            *msg_ptr;

  qmi_csi_error                               qmi_retval = QMI_CSI_NO_ERR;
  coex_metrics_lte_sinr_read_resp_msg_v01     qmi_response;
  void                                       *connection_handle;
  qmi_req_handle                              req_handle;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_metrics_lte_sinr_rsp_s *) rcv_msgr_msg_ptr;

  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_QMI_METRICS) )
  {
    memset( &qmi_response, 0, sizeof( coex_metrics_lte_sinr_read_resp_msg_v01 ) );

    if(msg_ptr->status == E_SUCCESS)
    {
      /* all okay; send the qmi_response */
      qmi_response.sinr_valid          = TRUE;
      qmi_response.sinr                = msg_ptr->filtered_sinr_db;
      if (msg_ptr->sinr_count_bt)
      {
        qmi_response.sinr_bt_only_valid  = TRUE;
        qmi_response.sinr_bt_only        = msg_ptr->filtered_sinr_bt_db;
      }
      if (msg_ptr->sinr_count_wifi)
      {
        qmi_response.sinr_wifi_only_valid  = TRUE;
        qmi_response.sinr_wifi_only        = msg_ptr->filtered_sinr_wifi_db;
      }
      if (msg_ptr->sinr_count_bt_wifi)
      {
        qmi_response.sinr_bt_and_wifi_valid  = TRUE;
        qmi_response.sinr_bt_and_wifi        = msg_ptr->filtered_sinr_bt_wifi_db;
      }
      if (msg_ptr->sinr_count_lte_only)
      {
        qmi_response.sinr_lte_only_valid  = TRUE;
        qmi_response.sinr_lte_only        = msg_ptr->filtered_sinr_lte_only_db;
      }

      qmi_response.resp.result = QMI_RESULT_SUCCESS_V01;
      qmi_response.resp.error  = QMI_ERR_NONE_V01;
    }
    else if(msg_ptr->status == E_NOT_AVAILABLE)
    {
      /* likely LTE not in system */
      qmi_response.sinr_valid  = TRUE;
      qmi_response.sinr        = COEX_INVALID_SINR_VALUE;
      qmi_response.resp.result = QMI_RESULT_SUCCESS_V01;
      qmi_response.resp.error  = QMI_ERR_NONE_V01;
    }
    else
    {
      qmi_response.sinr_valid  = FALSE;
      qmi_response.resp.result = QMI_RESULT_FAILURE_V01;
      qmi_response.resp.error  = QMI_ERR_NONE_V01;
    }

    /* look up the QMI delayed qmi_response to the read req */
    qmi_retval = mqcsi_conn_mgr_pop_deferred_req(
                    MQCSI_COEX_SERVICE_ID,
                    msg_ptr->msg_id,
                    QMI_COEX_METRICS_LTE_SINR_READ_REQ_V01,
                    &connection_handle,
                    &req_handle);
    CXM_ASSERT( QMI_CSI_NO_ERR == qmi_retval );

    /* send the qmi delayed qmi_response */
    qmi_retval = mqcsi_conn_mgr_send_resp (
                    connection_handle,  
                    req_handle,
                    QMI_COEX_METRICS_LTE_SINR_READ_RESP_V01,
                    &qmi_response,
                    sizeof( coex_metrics_lte_sinr_read_resp_msg_v01 )
                  );
    CXM_ASSERT( QMI_CSI_NO_ERR == qmi_retval );

    if(QMI_CSI_NO_ERR != qmi_retval )
    {
      retval = E_FAILURE;
    }

    CXM_TRACE( CXM_TRC_MSGR_LTE_SINR,
             msg_ptr->status,
             qmi_response.sinr_valid,
             qmi_response.sinr,
             CXM_TRC_EN );
  }
  else
  {
    retval = coex_handle_lte_sinr_read_cnf_v2( rcv_msgr_msg_ptr );
  }

  CXM_MSG_1( HIGH, "Send Metrics SINR read resp, filtered_sinr_db=%d",
             msg_ptr->filtered_sinr_db );

  /* log metrics */
  cxm_tech_metrics_log.status = msg_ptr->status;
  cxm_tech_metrics_log.tech_id = CXM_TECH_LTE;
  cxm_tech_metrics_log.metric_cnt = msg_ptr->sinr_count;
  cxm_tech_metrics_log.metric_cnt_bt = msg_ptr->sinr_count_bt;
  cxm_tech_metrics_log.metric_cnt_wifi = msg_ptr->sinr_count_wifi;
  cxm_tech_metrics_log.metric_cnt_bt_wifi = msg_ptr->sinr_count_bt_wifi;
  cxm_tech_metrics_log.metric_cnt_mdm_only = msg_ptr->sinr_count_lte_only;
  cxm_tech_metrics_log.filt_metric = msg_ptr->filtered_sinr_db;
  cxm_tech_metrics_log.filt_metric_bt = msg_ptr->filtered_sinr_bt_db;
  cxm_tech_metrics_log.filt_metric_wifi = msg_ptr->filtered_sinr_wifi_db;
  cxm_tech_metrics_log.filt_metric_bt_wifi = msg_ptr->filtered_sinr_bt_wifi_db;
  cxm_tech_metrics_log.filt_metric_mdm_only = msg_ptr->filtered_sinr_lte_only_db;
  CXM_TRACE( 0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_TECH_METRICS_READ );

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_metrics_read_rsp

=============================================================================*/
/*!
    @brief
    Function to send QMI message with metrics stats

    @return qmi_csi_cb_error
*/
/*===========================================================================*/
errno_enum_type coex_handle_metrics_read_rsp (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                    retval = E_SUCCESS;
  cxm_coex_metrics_rsp_s             *msg_ptr;

  qmi_csi_error                      qmi_retval = QMI_CSI_NO_ERR;
  coex_metrics_read_resp_msg_v01     qmi_response;
  void                               *connection_handle;
  qmi_req_handle                     req_handle;

  /*-----------------------------------------------------------------------*/

  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_metrics_rsp_s *) rcv_msgr_msg_ptr;

  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_QMI_METRICS) )
  {
    memset( &qmi_response, 0, sizeof( coex_metrics_read_resp_msg_v01 ) );

    if(msg_ptr->status == E_SUCCESS)
    {
      /* all okay; send the qmi_response */
      if ( msg_ptr->tech_id == CXM_TECH_GSM1 || msg_ptr->tech_id == CXM_TECH_GSM2 
           || msg_ptr->tech_id == CXM_TECH_GSM3 )
      {
        qmi_response.sinr_valid          = TRUE;
        qmi_response.sinr                = msg_ptr->filt_metric;
      }
      else if ( msg_ptr->tech_id == CXM_TECH_TDSCDMA )
      {
        qmi_response.nb_noise_valid      = TRUE;
        qmi_response.nb_noise            = msg_ptr->filt_metric;
      }

      qmi_response.tech_valid = TRUE;
      qmi_response.tech = (coex_wwan_tech_v01)msg_ptr->tech_id;

      qmi_response.resp.result = QMI_RESULT_SUCCESS_V01;
      qmi_response.resp.error  = QMI_ERR_NONE_V01;
    }
    else if(msg_ptr->status == E_NOT_AVAILABLE)
    {
      /* likely tech not in system */
      if ( msg_ptr->tech_id == CXM_TECH_GSM1 || msg_ptr->tech_id == CXM_TECH_GSM2 
           || msg_ptr->tech_id == CXM_TECH_GSM3 )
      {
        qmi_response.sinr_valid          = TRUE;
        qmi_response.sinr                = COEX_INVALID_SINR_VALUE;
      }
      else if ( msg_ptr->tech_id == CXM_TECH_TDSCDMA )
      {
        qmi_response.nb_noise_valid      = TRUE;
        qmi_response.nb_noise            = COEX_INVALID_SINR_VALUE;
      }
      qmi_response.resp.result = QMI_RESULT_SUCCESS_V01;
      qmi_response.resp.error  = QMI_ERR_NONE_V01;
    }
    else
    {
      qmi_response.resp.result = QMI_RESULT_FAILURE_V01;
      qmi_response.resp.error  = QMI_ERR_NONE_V01;
    }

    /* look up the QMI delayed qmi_response to the read req */
    qmi_retval = mqcsi_conn_mgr_pop_deferred_req(
                    MQCSI_COEX_SERVICE_ID,
                    msg_ptr->msg_id,
                    QMI_COEX_METRICS_READ_REQ_V01,
                    &connection_handle,
                    &req_handle);
    CXM_ASSERT( QMI_CSI_NO_ERR == qmi_retval );

    /* send the qmi delayed qmi_response */
    qmi_retval = mqcsi_conn_mgr_send_resp (
                    connection_handle,  
                    req_handle,
                    QMI_COEX_METRICS_READ_RESP_V01,
                    &qmi_response,
                    sizeof( coex_metrics_read_resp_msg_v01 )
                  );
    CXM_ASSERT( QMI_CSI_NO_ERR == qmi_retval );

    if(QMI_CSI_NO_ERR != qmi_retval )
    {
      retval = E_FAILURE;
    }

    /* log metrics info */
    cxm_tech_metrics_log.status = msg_ptr->status;
    cxm_tech_metrics_log.tech_id = msg_ptr->tech_id;
    cxm_tech_metrics_log.metric_cnt = msg_ptr->metric_cnt;
    cxm_tech_metrics_log.filt_metric = msg_ptr->filt_metric;

    CXM_TRACE( CXM_TRC_MSGR_LTE_SINR,
             msg_ptr->status,
             qmi_response.sinr_valid,
             qmi_response.sinr, CXM_TRC_AND_PKT_EN,
             CXM_LOG_TECH_METRICS_READ );
  }

  CXM_MSG_2( HIGH, "Send Metrics read resp, tech=%d, filtered_metric=%d",
             msg_ptr->tech_id, msg_ptr->filt_metric );

  return retval;

}

/*=============================================================================

  FUNCTION:  coex_handle_lte_tx_power_limit_cnf

=============================================================================*/
/*!
    @brief
    Function to handle the response to the request to LTE ML1 for power
    limiting conditions. Evaluates conditions and starts power limiting if
    appropriate

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_lte_tx_power_limit_rsp (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                        retval = E_SUCCESS;
  cxm_coex_tx_pwr_lmt_lte_cndtns_rsp_s   *msg_ptr;
  coex_pwrlmt_condition_success_mask_v01 succ_ind_mask = 0;
  coex_pwrlmt_condition_fail_mask_v01    controller_tx_pwrlmt_fail_cond = 0;
  coex_pwrlmt_condition_fail_mask_v01    wci2_tx_pwrlmt_fail_cond = 0;
  float path_loss, filtered_rbs;
  /*-----------------------------------------------------------------------*/
  coex_check_power_limit_in_progress = FALSE;

  if( (coex_controller_pwr_lmt_state != COEX_PWR_LMT_PENDING) &&
      (coex_wci2_pwr_lmt_state       != COEX_PWR_LMT_PENDING) )
  {
    /* if policy has been disabled before power limit enacted do not
       need to do this check */
    return retval;
  }

  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_tx_pwr_lmt_lte_cndtns_rsp_s *) rcv_msgr_msg_ptr;

  /* check conditions, set power limit if appropriate */
  path_loss = (float) msg_ptr->dl_pathloss;
  retval = cxm_unsigned_Q8_to_float(msg_ptr->rb_filtered, &filtered_rbs);
  CXM_ASSERT( retval == E_SUCCESS );

  if( ( path_loss    < coex_plcy_parms.link_path_loss_threshold ) &&
      ( filtered_rbs > coex_plcy_parms.filtered_rb_threshold )    &&
      ( COEX_SYS_ENABLED(CXM_SYS_BHVR_DISREGARD_RRC_PROC) || msg_ptr->rrc_state == FALSE ) )
  {
    /* conditions passed: set power limit if LTE in system. If LTE not in
       system, state remains pending and the check will happen again when
       it re-enters */
    if( coex_wwan_state_info[CXM_TECH_LTE].state_data.num_link_info_sets != 0 )
    {
      if( coex_controller_pwr_lmt_state == COEX_PWR_LMT_PENDING )
      {
        coex_controller_pwr_lmt_state = COEX_PWR_LMT_ACTIVE;
        succ_ind_mask |= COEX_PLCSM_CONTROLLER_TX_PWR_LMT_ENFORCED_V01;
        cxm_counter_event( CXM_CNT_CONTR_PWR_LMT_HONORED, 
                           (uint32) coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.controller_tx_power_limit );
        CXM_MSG_0( HIGH, "Starting controller power limiting" );
      }

      if( coex_wci2_pwr_lmt_state == COEX_PWR_LMT_PENDING )
      {
        coex_wci2_pwr_lmt_state = COEX_PWR_LMT_ACTIVE;
        succ_ind_mask |= COEX_PLCSM_WCI2_TX_PWR_LMT_ENFORCED_V01;

        /* start watchdog timer */
        timer_set( &coex_pwr_lmt_wdog_timer,
                   (timetick_type) coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_tx_pwrlmt_timeout,
                   0, T_MSEC );
        cxm_counter_event( CXM_CNT_WCI2_PWR_LMT_HONORED, 
                           (uint32) coex_wwan_state_info[CXM_TECH_LTE].plcy_parms.params.lte.wci2_power_limit );
        CXM_MSG_0( HIGH, "Starting WCI-2 power limiting" );
      }

      /* one or more states may have changed, set power limit appropriately */
      coex_check_and_set_power_limit();

      /* send out the indication if necessary */
      coex_prep_and_send_condition_success_ind( succ_ind_mask );
    }
  }
  else
  {
    /* condition failed; send IND */
    if( coex_controller_pwr_lmt_state == COEX_PWR_LMT_PENDING )
    {
      /* controller power limiting was not enforced. */
      coex_controller_pwr_lmt_state = COEX_PWR_LMT_OFF;

      /* One or more conditions failed for controller, fill in appropriately */
      if( path_loss >= coex_plcy_parms.link_path_loss_threshold )
      {
        controller_tx_pwrlmt_fail_cond |= COEX_PLCFM_LINK_PATH_LOSS_THLD_CROSSED_V01;
      }
      if( filtered_rbs <= coex_plcy_parms.filtered_rb_threshold )
      {
        controller_tx_pwrlmt_fail_cond |= COEX_PLCFM_FILTERED_RB_THLD_CROSSED_V01;
      }
      /*We will not report fail on RRC procdure if we are disregarding it*/
      if( msg_ptr->rrc_state &&  !COEX_SYS_ENABLED(CXM_SYS_BHVR_DISREGARD_RRC_PROC) )
      {
        controller_tx_pwrlmt_fail_cond |= COEX_PLCFM_RRC_PROCEDURE_ACTIVE_V01;
      }
    }
    if( coex_wci2_pwr_lmt_state == COEX_PWR_LMT_PENDING )
    {
      coex_wci2_pwr_lmt_state = COEX_PWR_LMT_OFF;

      /* One or more conditions failed for wci2, fill in appropriately */
      if( path_loss >= coex_plcy_parms.link_path_loss_threshold )
      {
        wci2_tx_pwrlmt_fail_cond |= COEX_PLCFM_LINK_PATH_LOSS_THLD_CROSSED_V01;
      }
      if( filtered_rbs <= coex_plcy_parms.filtered_rb_threshold )
      {
        wci2_tx_pwrlmt_fail_cond |= COEX_PLCFM_FILTERED_RB_THLD_CROSSED_V01;
      }
      /*We will not report fail on RRC procdure if we are disregarding it*/
      if( msg_ptr->rrc_state &&  !COEX_SYS_ENABLED(CXM_SYS_BHVR_DISREGARD_RRC_PROC) )
      {
        wci2_tx_pwrlmt_fail_cond |= COEX_PLCFM_RRC_PROCEDURE_ACTIVE_V01;
      }
    }

    /* send the indication over QMI if necessary */
    coex_prep_and_send_condition_fail_ind( 0, controller_tx_pwrlmt_fail_cond, 
                                           wci2_tx_pwrlmt_fail_cond );
  }

  CXM_TRACE( CXM_TRC_MSGR_LTE_TX_PWR_LMT,
                   msg_ptr->dl_pathloss, msg_ptr->rb_filtered, msg_ptr->rrc_state,
                   CXM_TRC_AND_PKT_EN, 
                   CXM_LOG_WCI2_PWR_LMT_STATE, CXM_LOG_CONTROLLER_PWR_LMT_STATE );

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_lte_tx_power_limit_ind

=============================================================================*/
/*!
    @brief
    Function to handle indication that RRC procedure is in progress. If we
    are power limiting, stop and send condition fail

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_lte_tx_power_limit_ind (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                       retval = E_SUCCESS;
  cxm_coex_tx_pwr_lmt_lte_cndtns_ind_s  *msg_ptr;
  coex_pwrlmt_condition_fail_mask_v01   controller_tx_pwrlmt_fail_cond = 0;
  coex_pwrlmt_condition_fail_mask_v01   wci2_tx_pwrlmt_fail_cond = 0;
  boolean coex_reenforce_power_limit_reqd = FALSE;
  /*-----------------------------------------------------------------------*/
  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_DISREGARD_RRC_PROC) )
  {
    return retval;
  }

  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_tx_pwr_lmt_lte_cndtns_ind_s *) rcv_msgr_msg_ptr;

  /* check if RRC Procedure in progress AND currently power limiting */
  if( msg_ptr->rrc_state )
  {
    /* fill in indication and disable power limiting */
    if( coex_controller_pwr_lmt_state == COEX_PWR_LMT_ACTIVE )
    {
      controller_tx_pwrlmt_fail_cond |= COEX_PLCFM_RRC_PROCEDURE_ACTIVE_V01;
      coex_controller_pwr_lmt_state = COEX_PWR_LMT_RRC_PROC_ONGOING;
    }

    if( coex_wci2_pwr_lmt_state == COEX_PWR_LMT_ACTIVE )
    {
      wci2_tx_pwrlmt_fail_cond |= COEX_PLCFM_RRC_PROCEDURE_ACTIVE_V01;
      coex_wci2_pwr_lmt_state = COEX_PWR_LMT_RRC_PROC_ONGOING;
    }

    /* Power limit states have changed. Power limit needs to be cleared */
    coex_check_and_set_power_limit();

    coex_prep_and_send_condition_fail_ind( 0, 
      controller_tx_pwrlmt_fail_cond, 
      wci2_tx_pwrlmt_fail_cond 
    );
  }
  else
  {
    /* manage controller power limit */
    if( ( coex_controller_pwr_lmt_state == COEX_PWR_LMT_OFF ||
          coex_controller_pwr_lmt_state == COEX_PWR_LMT_RRC_PROC_ONGOING  ) &&
        ( coex_wwan_state_info[CXM_TECH_LTE].policy & 
          COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT_V01 ))
    {
      /* controller power limiting now enforced */
      coex_controller_pwr_lmt_state = COEX_PWR_LMT_PENDING;
      coex_reenforce_power_limit_reqd = TRUE;
    }

    /* manage wci2 power limit */
    if( ( coex_wci2_pwr_lmt_state == COEX_PWR_LMT_RRC_PROC_ONGOING ) &&
      ( coex_wwan_state_info[CXM_TECH_LTE].policy & 
        COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT_V01 ) )
    {
      /* wci2 power limiting now enforced */
      coex_wci2_pwr_lmt_state = COEX_PWR_LMT_PENDING;
      coex_reenforce_power_limit_reqd = TRUE;
    }

    if( coex_reenforce_power_limit_reqd )
    {
      /* call to ML1 to check power limiting conditions */
      coex_check_power_limiting_conditions();
    }
  }

  CXM_MSG_1( MED, "recvd RRC Proc status: %x", msg_ptr->rrc_state );

  CXM_TRACE( CXM_TRC_MSGR_LTE_RRC_STATE, msg_ptr->rrc_state, 
                   coex_controller_pwr_lmt_state, coex_wci2_pwr_lmt_state,
                   CXM_TRC_EN );
  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_lte_wcn_report_ind

=============================================================================*/
/*!
    @brief
    Handle message from ML1 to CXM reporting FW WCN threshold exceeded

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_wcn_txfrndnl_report_ind (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                          retval = E_SUCCESS;
  cxm_coex_wcn_txfrndnl_report_ind_s      *msg_ptr;
  coex_txfrmdnl_condition_failure_mask_v01 tx_subframe_denials_status = 0;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_wcn_txfrndnl_report_ind_s *) rcv_msgr_msg_ptr;

  CXM_TRACE( CXM_TRC_MSGR_WCN_RPT, msg_ptr->stats.result, 
                   msg_ptr->stats.num_wcn_reqs, 0,
                   CXM_TRC_EN );

  /*error number from msgr should line up with QMI IDL error number*/
  tx_subframe_denials_status = msg_ptr->stats.result;

  CXM_MSG_2( MED, "WCN Prio condition fail: %d blanking requests recvd, mask=%x",
             msg_ptr->stats.num_wcn_reqs,
             tx_subframe_denials_status );

  coex_prep_and_send_condition_fail_ind( tx_subframe_denials_status, 0, 0 );

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_tech_wcn_txfrndnl_report_ind

=============================================================================*/
/*!
    @brief
    Handle message from tech L1 to CXM reporting FW WCN threshold exceeded

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_tech_wcn_txfrndnl_report_ind (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                          retval = E_SUCCESS;
  cxm_coex_tech_tx_frm_dnl_report_ind_s    *msg_ptr;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_tech_tx_frm_dnl_report_ind_s *) rcv_msgr_msg_ptr;

  CXM_TRACE( CXM_TRC_MSGR_WCN_RPT, msg_ptr->stats.result, 
                   msg_ptr->stats.num_wcn_reqs, msg_ptr->tech_id, CXM_TRC_EN );

  CXM_MSG_3( MED, "WCN Prio condition fail: %d blanking requests recvd, result=%x, tech=%d",
             msg_ptr->stats.num_wcn_reqs,
             msg_ptr->stats.result, 
             msg_ptr->tech_id );

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_activity_adv_ntc

=============================================================================*/
/*!
    @brief
    Handle message from L1 to CXM reporting activity in advance

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_activity_adv_ntc (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                          retval = E_SUCCESS;
  cxm_coex_tech_activity_adv_ntc_ind_s     *msg_ptr;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_tech_activity_adv_ntc_ind_s *) rcv_msgr_msg_ptr;

  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_SMEM_DATA) )
  {
    cxm_smem_handle_activity_adv_ntc(msg_ptr);
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_tx_ant_sel_ind

=============================================================================*/
/*!
  @brief
    Handle set transmit (Tx) antenna selection message
 
  @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_tx_ant_sel_ind (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type  retval = E_SUCCESS;
  uint32           old_tech_tx_active_mask = coex_state.conn_tech_mask;
  uint32           old_tech_tx_power_mask  = coex_state.tx_pwr_tech_mask;
  cxm_tx_ant_sel_e old_tx_ant_sel          = coex_state.tx_ant_sel;
  cxm_coex_tx_ant_sel_ind_s *msg_ptr;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_tx_ant_sel_ind_s *) rcv_msgr_msg_ptr;
  coex_state.tx_ant_sel = msg_ptr->tx_ant_sel;
  CXM_MSG_2( HIGH, "Handling update for type7[ant_tx] - old:%d new:%d",
             old_tx_ant_sel,
             coex_state.tx_ant_sel );

  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) &&
      (COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL_V01 &
      coex_wwan_state_info[CXM_TECH_LTE].policy) )
  {
    coex_check_wci2_type7_state( old_tech_tx_active_mask,
                                 old_tech_tx_power_mask,
                                 old_tx_ant_sel,
                                 COEX_WCI2_T7_TX_ANT_SEL_EVNT_MSK );
  }
  return retval;
}

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
)
{
  /*-----------------------------------------------------------------------*/
  switch( (coex_wci2_t7_event_e)event )
  {
    case COEX_WCI2_T7_CONN_ST_EVNT:
      coex_plcy_parms.t7_con_holdoff = time_ms;
      break;

    case COEX_WCI2_T7_PWR_ST_EVNT:
      coex_plcy_parms.t7_pwr_holdoff = time_ms;
      break;

    case COEX_WCI2_T7_TX_ANT_SEL_EVNT:
    default:
      CXM_MSG_1( HIGH, "Unsupported/Incorrect update for WCI2 type7 event %d", event);
      break;
  }
  return;
}

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
)
{
  errno_enum_type                 retval = E_SUCCESS;
  cxm_coex_metrics_lte_sinr_req_s msgr_msg;
  cxm_coex_metrics_req_s          msgr_tech_msg;
  uint32                          trc_code = 0;
  /*-----------------------------------------------------------------------*/
  switch( action )
  {
    case CXM_ACTION_START:
      /* make sure alpha within range [0 - 1] */
      if( alpha_q8 > COEX_LTE_METRICS_MAX_ALPHA )
      {
        alpha_q8 = COEX_LTE_METRICS_MAX_ALPHA;
      }

      if ( tech == CXM_TECH_LTE )
      {
        msgr_msg.action = CXM_ACTION_START;
        msgr_msg.payload.start_params.alpha = alpha_q8;
      }
      else
      {
        msgr_tech_msg.action = CXM_ACTION_START;
        msgr_tech_msg.payload.start_params.alpha = alpha_q8;
      }
      
      atomic_set( &coex_wwan_state_info[tech].metrics_started, 1 );
      /* Turn on the uart and send policy for lte subframe marker */
      if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) )
      {
        coex_manage_uart_npa_vote();
      }
      coex_prep_and_send_policy_msg( (cxm_tech_type)tech );
      break;

    case CXM_ACTION_STOP:
      if ( tech == CXM_TECH_LTE )
      {
        msgr_msg.action = CXM_ACTION_STOP;
      }
      else
      {
        msgr_tech_msg.action = CXM_ACTION_STOP;
      }
      
      atomic_set( &coex_wwan_state_info[tech].metrics_started, 0 );
      /* Turn off the uart and send policy for lte subframe marker */
      if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) )
      {
         coex_manage_uart_npa_vote();
      }
      coex_prep_and_send_policy_msg( (cxm_tech_type)tech );
      break;

    case CXM_ACTION_READ:
      if ( tech == CXM_TECH_LTE )
      {
        msgr_msg.action = CXM_ACTION_READ;
        msgr_msg.payload.read_params.msg_id = msg_id;
      }
      else
      {
        msgr_tech_msg.action = CXM_ACTION_READ;
        msgr_tech_msg.payload.read_params.msg_id = msg_id;
      }
      break;

    default:
      return retval;
  }

  switch ( tech )
  {
    case CXM_TECH_LTE:
      /* send request to LTE ML1 */
      retval = cxm_msgr_send_msg( &msgr_msg.msg_hdr,
                                  MCS_CXM_COEX_METRICS_LTE_SINR_REQ,
                                  sizeof(cxm_coex_metrics_lte_sinr_req_s) );
      trc_code = CXM_LOG_METRICS_LTE_SINR_STARTED;
      break;
    case CXM_TECH_TDSCDMA:
      /* send request to TL1 */
      retval = cxm_msgr_send_msg( &msgr_tech_msg.msg_hdr,
                                  MCS_CXM_COEX_METRICS_TDSCDMA_REQ,
                                  sizeof(cxm_coex_metrics_req_s) );
      trc_code = CXM_LOG_METRICS_TDS_NRRWBND_NOISE_STARTED;
      break;
    /* send request to GL1 */
    case CXM_TECH_GSM1:
      retval = cxm_msgr_send_msg( &msgr_tech_msg.msg_hdr,
                                  MCS_CXM_COEX_METRICS_GSM1_REQ,
                                  sizeof(cxm_coex_metrics_req_s) );
      trc_code = CXM_LOG_METRICS_GSM1_SINR_STARTED;
      break;
    case CXM_TECH_GSM2:
      retval = cxm_msgr_send_msg( &msgr_tech_msg.msg_hdr,
                                  MCS_CXM_COEX_METRICS_GSM2_REQ,
                                  sizeof(cxm_coex_metrics_req_s) );
      trc_code = CXM_LOG_METRICS_GSM2_SINR_STARTED;
      break;
    case CXM_TECH_GSM3:
      retval = cxm_msgr_send_msg( &msgr_tech_msg.msg_hdr,
                                  MCS_CXM_COEX_METRICS_GSM3_REQ,
                                  sizeof(cxm_coex_metrics_req_s) );
      trc_code = CXM_LOG_METRICS_GSM3_SINR_STARTED;
      break;
    default:
      break;
  }
  
  CXM_ASSERT( E_SUCCESS == retval );
  CXM_MSG_2( HIGH, "Initiated metrics req, tech=%d, action=%x", tech, 
               action );
  if ( action == CXM_ACTION_START || action == CXM_ACTION_STOP )
  {
    CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN, trc_code);
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_tech_tx_pwr_report_ind

=============================================================================*/
/*!
    @brief
    Message from WWAN techs informing of the Tx power report.

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_tech_tx_pwr_report_ind(
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type            retval = E_SUCCESS;
  cxm_coex_tech_tx_pwr_rpt_ind_s *msg_ptr;
  uint32 old_tech_tx_active_mask = coex_state.conn_tech_mask;
  uint32 old_tech_tx_power_mask  = coex_state.tx_pwr_tech_mask;
  cxm_tx_ant_sel_e old_tx_ant_sel = coex_state.tx_ant_sel;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_tech_tx_pwr_rpt_ind_s *) rcv_msgr_msg_ptr;

  CXM_ASSERT( CXM_TECH_LTE == msg_ptr->tech_id );
  CXM_MSG_3( MED, "Tech %d Tx pwr state %d & Tx pwr (mW) %d",
             msg_ptr->tech_id, msg_ptr->report.lte.wci2_t7_tx_pwr_state,
             msg_ptr->report.lte.filtered_tx_pwr_mW );
  if( msg_ptr->report.lte.wci2_t7_tx_pwr_state )
  {
    coex_state.tx_pwr_tech_mask |= (1<<msg_ptr->tech_id);
  }
  else
  {
    coex_state.tx_pwr_tech_mask &= ~(1<<msg_ptr->tech_id);
  }

  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) && 
      (COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01 & 
      coex_wwan_state_info[CXM_TECH_LTE].policy) )
  {
    coex_check_wci2_type7_state( old_tech_tx_active_mask,
                                 old_tech_tx_power_mask,
                                 old_tx_ant_sel,
                                 COEX_WCI2_T7_PWR_ST_EVNT_MSK );
  }

  CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_STATE);

  return retval;
}

/*=============================================================================

  FUNCTION:  coex_apply_scan_freq_filter

=============================================================================*/
/*!
    @brief
    Helper API to apply the scan freq filter to WWAN tech's high prio event

    @return
    boolean
*/
/*===========================================================================*/
boolean coex_apply_scan_freq_filter(
  uint32 freq,
  uint8 *index
)
{
  boolean found = FALSE;
  uint8 i;
  /*-----------------------------------------------------------------------*/
  *index = 0; /* no match found to begin with */
  for( i=0 ; i < coex_scan_freq_filter_info.bands_len+1 ; i++ )
  {
    if( freq >= coex_scan_freq_filter_info.processed_filter[i].freq_start &&
        freq <= coex_scan_freq_filter_info.processed_filter[i].freq_stop )
    {
      found = TRUE;
      *index = i;
      break;
    }
  }

  return found;
}

/*=============================================================================

  FUNCTION:  coex_prep_and_send_wci2_type4

=============================================================================*/
/*!
    @brief
    Helper API to send out WCI2 type4 scan freq message

    @return
    void
*/
/*===========================================================================*/
void coex_prep_and_send_wci2_type4(
  cxm_tech_type tech_id,
  uint8 index
)
{
  wci2_msg_type_s t4_msg;
  /*-----------------------------------------------------------------------*/
  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) )
  {
    if( COEX_PCM_SEND_WCI2_TYPE4_SCAN_FREQ_V01 & coex_wwan_state_info[tech_id].policy )  
    {
      coex_scan_freq_filter_info.reported_filter_index = index;
      coex_scan_freq_filter_info.reported_scan_freq =
        coex_scan_freq_filter_info.bands[index].freq * MHZ_KHZ_CONVERSION;
      coex_scan_freq_filter_info.reported_tech_id = tech_id;

      t4_msg.type = WCI2_TYPE4;
      /* payload/index can only be a number between [0-31]
       * 0 - reserved to indicate stop event or no scanning
       * and 1 to 31 represent indices 0 to 30 in the SCAN filter set using QMI message
       */
      t4_msg.data.type4_freq_index = index; 
      wci2_send_msg( &t4_msg );
      CXM_MSG_3( MED, "Sending wci2 type4: payload/index %d, tech %d, freq %d",
                 index, tech_id, coex_scan_freq_filter_info.reported_scan_freq );
    }
  }

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_high_priority_ind

=============================================================================*/
/*!
    @brief
    Message from WWAN techs informing of high priority events.

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_high_priority_ind(
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type           retval = E_SUCCESS;
  cxm_coex_high_prio_ind_s *msg_ptr;
  uint8 i, filter_index = 0;
  boolean match = FALSE;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_high_prio_ind_s *) rcv_msgr_msg_ptr;
  CXM_ASSERT( msg_ptr->num_freqs <= COEX_MAX_HIGH_PRIO_FREQ );

  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_SMEM_DATA) || 
      COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) )
  {
    /* F3 flagged for excessive volume (>3x/ms) */
    CXM_MSG_5( MED, "WWAN tech %d high priority event %d; start %x end %x; #freq %d",
               msg_ptr->tech_id, msg_ptr->op_id, msg_ptr->start, msg_ptr->end,
               msg_ptr->num_freqs );

    /* send a log packet with high prio event */
    cxm_high_prio_log.tech_id = msg_ptr->tech_id;
    cxm_high_prio_log.op_id = msg_ptr->op_id;
    cxm_high_prio_log.start = msg_ptr->start;
    cxm_high_prio_log.end= msg_ptr->end;
    cxm_high_prio_log.num_freqs = msg_ptr->num_freqs;
    for (i=0; i<msg_ptr->num_freqs; i++)
    {
      cxm_high_prio_log.freq[i] = msg_ptr->freq[i];
    }
    CXM_TRACE(0, 0, 0, 0, CXM_LOG_PKT_EN, CXM_LOG_HIGH_PRIO_INFO);

    if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) && msg_ptr->tech_id == CXM_TECH_LTE )
    {
      switch( msg_ptr->op_id )
      {
        case CXM_HIGH_PRIO_MEAS:
          if( coex_scan_freq_filter_info.filter_on )
          {
            for( i=0; i<msg_ptr->num_freqs; i++ )
            {
              match = coex_apply_scan_freq_filter( msg_ptr->freq[i], &filter_index );
              CXM_MSG_7( HIGH, "SCAN filter:[match/tech/op/freq/indx/st/en][%d/%d/%d/%d/%d/%d/%d]",
                         match, msg_ptr->tech_id, msg_ptr->op_id, msg_ptr->freq[i],
                         filter_index, msg_ptr->start, msg_ptr->end );
              if( match )
              {
                /* Ensure that payload for type4 is between [1-31]
                 * 0 is reserved to indicate scan stop 
                 * for all other indices from the filter table add 1 to report
                 */
                /* check whether 'start' or 'end' event */
                if( msg_ptr->start == 0xFFFFFFFF && msg_ptr->end == 0xFFFFFFFF )
                {
                  /* 'start' of SCAN freq event
                      - send WCI2 type4 message with (filter_index) as payload */
                  coex_prep_and_send_wci2_type4( msg_ptr->tech_id, filter_index);
                }
                else if( msg_ptr->start == 0x000000000 && msg_ptr->end == 0x000000000 )
                {
                  /* 'end' of SCAN freq event
                      - send WCI2 type4 message with 0x00 payload */
                  coex_prep_and_send_wci2_type4( msg_ptr->tech_id, 0x00 );
                }
              }
            }
          }
          break;
        default:
          CXM_MSG_1( MED, "Unsupported COEX high priority event with id %d", msg_ptr->op_id );
      }
    }

    if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_SMEM_DATA) )
    {
      if ( msg_ptr->tech_id == CXM_TECH_LTE && COEX_SYS_ENABLED(CXM_SYS_BHVR_LTE_HIGH_PRIO_MSG) )
      {
        cxm_smem_handle_high_prio(msg_ptr);
      }
      else if ( (msg_ptr->tech_id == CXM_TECH_TDSCDMA || msg_ptr->tech_id == CXM_TECH_GSM1 || 
                msg_ptr->tech_id == CXM_TECH_GSM2 || msg_ptr->tech_id == CXM_TECH_GSM3) &&  
                COEX_SYS_ENABLED(CXM_SYS_BHVR_GSM_TDS_HIGH_PRIO_MSG) )
      {
        cxm_smem_handle_high_prio(msg_ptr);
      }
    }   
  }

  return retval;
}

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
)
{
  qmi_csi_cb_error req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_set_scan_freq_band_filter_resp_msg_v01 response;
  coex_set_scan_freq_band_filter_req_msg_v01 *request;
  uint32 i;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( req_c_struct != NULL );

  /* De-mystify the received message pointer to the appropriate type */
  request = (coex_set_scan_freq_band_filter_req_msg_v01 *) req_c_struct;

  CXM_MSG_1 ( HIGH, "Set SCAN freq band filter request",
             request->bands_len );
  /* Init the memory available for the REQ and LTE ML1 message */
  memset( &response, 0, sizeof( coex_set_scan_freq_band_filter_resp_msg_v01 ) );

  /*clear existing band filter info*/  
  memset( &coex_scan_freq_filter_info, 0, sizeof( coex_scan_freq_band_filter_s ) );
  coex_scan_freq_filter_info.bands_len = request->bands_len;

  response.resp.result = QMI_RESULT_SUCCESS_V01;
  response.resp.error  = QMI_ERR_NONE_V01;

  /* forward band filter info to LTE ML1 */
  if(request->bands_valid)
  {
    /* SCAN freq filter can support only 31 bands (even though the struct support 32)
     * Limitation is due to the way we report type 4 message
     * - 0th index is reserved to report SCAN stop
     * - 1 to 31 indices are reported as valid payloads */
    if( request->bands_len >= COEX_WWAN_MAX_BANDS_TO_MONITOR_V01 )
    {
      response.resp.result = QMI_RESULT_FAILURE_V01;
      response.resp.error  = QMI_ERR_INVALID_ARG_V01;
    }
    else
    {
      for( i = 0; i < request->bands_len; i++ )
      {
        /* NOTE: 0 to 30 indices of incoming scan filter are stored in 1 to 31 indices in the table */
        coex_scan_freq_filter_info.bands[i+1] = request->bands[i];
        coex_scan_freq_filter_info.processed_filter[i+1].freq_start = 
          ( request->bands[i].freq - (request->bands[i].bandwidth/2) ) * MHZ_KHZ_CONVERSION;

        coex_scan_freq_filter_info.processed_filter[i+1].freq_stop = 
          ( request->bands[i].freq + (request->bands[i].bandwidth/2) ) * MHZ_KHZ_CONVERSION;
      }

      if (request->bands_len > 0) 
      {
        coex_scan_freq_filter_info.filter_on = TRUE;
      }
      else
      {
        coex_scan_freq_filter_info.filter_on = FALSE;
      }
    }
  }

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_set_scan_freq_band_filter_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  qmi_csi_cb_error req_cb_retval = QMI_CSI_CB_NO_ERR;
  coex_get_scan_freq_band_filter_resp_msg_v01 response;
  uint32 i;
  /*-----------------------------------------------------------------------*/
  /* Init the memory available for the REQ message */
  memset( &response, 0, sizeof( coex_get_scan_freq_band_filter_resp_msg_v01 ) );

  response.resp.result = QMI_RESULT_SUCCESS_V01;
  response.resp.error  = QMI_ERR_NONE_V01;

  /* Copy over current filter info */
  response.bands_valid = TRUE;
  response.bands_len   = coex_scan_freq_filter_info.bands_len;
  /* NOTE: 0th element is reserved and empty */
  for( i = 0; i < coex_scan_freq_filter_info.bands_len; i++ )
  {
    response.bands[i] = coex_scan_freq_filter_info.bands[i+1];
  }

  /* send QMI response */
  req_cb_retval = mqcsi_conn_mgr_send_resp_from_cb (
                    connection_handle,
                    req_handle,
                    msg_id,
                    &response,
                    sizeof( coex_get_scan_freq_band_filter_resp_msg_v01 )
                  );
  CXM_ASSERT( QMI_CSI_CB_NO_ERR == req_cb_retval );

  return req_cb_retval;
}

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
)
{
  cxm_coex_metrics_lte_bler_req_s  msgr_lte_bler_msg;
  uint8                            i;
  /*-----------------------------------------------------------------------*/
  /* Disable COEX algorithms for the lower layers */

  /* 1. clear all timers (if any pending) */
  (void) timer_clr( &coex_pwr_lmt_wdog_timer, T_MSEC );
  CXM_MSG_0( HIGH, "Task deinit: Disabling coex timers" );

  /* 2. disable coex policy (if any) */
  for ( i = 0; i < CXM_TECH_MAX; i++ )
  {
    if( coex_wwan_state_info[i].policy_state == COEX_POLICY_ACTIVE )
    {
      coex_wwan_state_info[i].policy = CXM_POLICY_NONE;
      coex_prep_and_send_policy_msg( i );
      CXM_MSG_1( HIGH, "Task deinit: Inactive (policy) msg sent, tech=%d", i);
    }
  }

  /* 3. stop/disable all metrics (if any) collection/reporting */
  for ( i = 0; i < CXM_TECH_MAX; i++ )
  {
    if( atomic_read(&coex_wwan_state_info[i].metrics_started) )
    {
      coex_tech_metrics_initiate_req( i, CXM_ACTION_STOP, 0, 0);
      CXM_MSG_1( HIGH, "Task deinit: Stopping metrics, tech=%d", i );
    }
  }

  if( atomic_read(&coex_metrics_lte_bler_started) )
  {
    msgr_lte_bler_msg.action = CXM_ACTION_STOP;
    (void) cxm_msgr_send_msg( &msgr_lte_bler_msg.msg_hdr,
                                MCS_CXM_COEX_METRICS_LTE_BLER_REQ,
                                sizeof(cxm_coex_metrics_lte_bler_req_s) );
    atomic_set(&coex_metrics_lte_bler_started, 0);
    CXM_MSG_0( HIGH, "Task deinit: Stopping LTE BLER metrics" );
  }

  /* 4. if enabled disable QMB solution */
  if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_CONTROL) )
  {
    coex_algos_deinit_v2();
    CXM_MSG_0( HIGH, "Task deinit: Disabling COEX QMB control plane" );
  }

  /* 5. disable uart interface for WCI2 solution (if enabled) */
  if ( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) ||
       COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_CONTROL) )
  {
    /* deinitialize the UART */
    CXM_MSG_0( HIGH, "Task deinit: Powering down UART interface" );
    (void) wci2_client_deregister( coex_state.uart_handle );

    /* unregister from all slpc callbacks */
    slpc_deregister_notify_callback( &coex_slpc_notify_cb );
    slpc_deregister_notify_wakeup_callback( &coex_slpc_notify_wakeup_cb );
    CXM_MSG_0( HIGH, "Task deinit: Disabling all slpc callbacks" );
  }

  return;
}

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
)
{
  cxm_coex_tech_l3_state_ind_s msg;
  errno_enum_type retval = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  CXM_MSG_3( HIGH, "Tech[%d] called API to set L3 mode=%d & state=%d",
             tech, l3->mode, l3->state );
  msg.tech     = tech;
  msg.l3.mode  = l3->mode;
  msg.l3.state = l3->state;

  retval = cxm_msgr_send_msg( &msg.msg_hdr, 
                              MCS_CXM_COEX_TECH_L3_STATE_IND, 
                              sizeof(cxm_coex_tech_l3_state_ind_s) ); 
  return retval;
}

/*=============================================================================

  FUNCTION:  coex_handle_tech_l3_state_ind

=============================================================================*/
/*!
    @brief
    Message from WWAN techs informing of their layer 3 (RRC/CP/RR) state.

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_tech_l3_state_ind(
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type               retval = E_SUCCESS;
  cxm_coex_tech_l3_state_ind_s *msg_ptr;
  uint32                        old_tech_tx_active_mask = coex_state.conn_tech_mask;
  uint32                        old_tech_tx_power_mask  = coex_state.tx_pwr_tech_mask;
  cxm_tx_ant_sel_e              old_tx_ant_sel          = coex_state.tx_ant_sel;
  /*-----------------------------------------------------------------------*/
  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_tech_l3_state_ind_s *) rcv_msgr_msg_ptr;

  CXM_MSG_3( HIGH, "Processing L3 state message for tech[%d]:mode[%d],state[%d]",
             msg_ptr->tech, msg_ptr->l3.mode, msg_ptr->l3.state);

  if( msg_ptr->tech > CXM_TECH_DFLT_INVLD && msg_ptr->tech < CXM_TECH_MAX )
  {
    /* storing current RAT's layer 3 state */
    coex_wwan_state_info[msg_ptr->tech].l3 = msg_ptr->l3;

    switch ( msg_ptr->l3.mode )
    {
      case CXM_TECH_INACTIVE:
        // RAT is either going out of service or is the source RAT for an IRAT scenario
        // Update info for 'tech' reported & ignore any 'l3.state' reported
        coex_state.conn_tech_mask &= ~(1 << msg_ptr->tech);
        coex_state.tx_pwr_tech_mask &= ~(1 << msg_ptr->tech);
        break;

      case CXM_TECH_ACTIVE:
        // RAT is actively updating it's layer 3 (RRC/CP/RR) state
        // Update info for 'tech' & accordingly report it as connected or
        //   not connected over WCI2 type7 data message
        switch ( msg_ptr->l3.state )
        {
          // Capture all not-connected (no uplink activity) states here
          case CXM_TECH_L3_INACTIVE:
          case CXM_TECH_L3_DISCONNECT:
          case CXM_TECH_L3_ACQUISITION:
          case CXM_TECH_L3_IDLE:
          case CXM_TECH_L3_CSFB_IDLE:
          case CXM_TECH_L3_IDLE_NOT_CAMPED:
          case CXM_TECH_L3_IDLE_CAMPED:
          case CXM_TECH_L3_CELL_PCH:
          case CXM_TECH_L3_URA_PCH:
            coex_state.conn_tech_mask &= ~(1 << msg_ptr->tech);
            if( msg_ptr->tech != CXM_TECH_LTE )
            {
              coex_state.tx_pwr_tech_mask &= ~(1 << msg_ptr->tech);
            }
            break;

          // Capture all connected (uplink activity) states here
          case CXM_TECH_L3_CONNECTING:
          case CXM_TECH_L3_CONNECTION_ESTB:
          case CXM_TECH_L3_ACCESS:
          case CXM_TECH_L3_CSFB_ACCESS:
          case CXM_TECH_L3_CONNECTED:
          case CXM_TECH_L3_CELL_DCH:
          case CXM_TECH_L3_CELL_FACH:
          case CXM_TECH_L3_TRAFFIC:
          case CXM_TECH_L3_RACH:
          case CXM_TECH_L3_CONNECTED_CS:
          case CXM_TECH_L3_CONNECTED_PS:
            coex_state.conn_tech_mask |= (1 << msg_ptr->tech);
            if( msg_ptr->tech != CXM_TECH_LTE )
            {
              coex_state.tx_pwr_tech_mask |= (1 << msg_ptr->tech);
            }
            break;

          case CXM_TECH_L3_SUSPEND:
          default:
            CXM_MSG_2( HIGH, "Tech[%d] L3 state[%d] not supported",
                       msg_ptr->tech, msg_ptr->l3.state );
        }
        break; 

      default:
        CXM_MSG_2( HIGH, "Tech[%d] L3 mode[%d] not supported",
                   msg_ptr->tech, msg_ptr->l3.mode );
    }

    /* update tech's conn & pwr state needed for wci2 type7 data */
    if( COEX_SYS_ENABLED(CXM_SYS_BHVR_WCI2_DATA) && 
        ((COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01 &
         coex_wwan_state_info[CXM_TECH_LTE].policy) ||
         (COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01 &
          coex_wwan_state_info[CXM_TECH_LTE].policy)) )
    {
      coex_check_wci2_type7_state( old_tech_tx_active_mask,
                                   old_tech_tx_power_mask,
                                   old_tx_ant_sel,
                                   COEX_WCI2_T7_ALL_EVNT_MSK );
    }
  }
  else
  {
    CXM_MSG_1( ERROR, "Invalid tech[%d] reporting L3 state", msg_ptr->tech );
  }
  return retval; 
}
