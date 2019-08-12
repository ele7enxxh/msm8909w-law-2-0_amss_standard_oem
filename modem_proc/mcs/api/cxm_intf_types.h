#ifndef __CXM_INTF_TYPES_H__
#define __CXM_INTF_TYPES_H__
/*!
  @file cxm_types.h

  @brief
   CXM data types exposed to tech LL for WCN-WWAN COEX

*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/cxm_intf_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/20/15   btl     Add interface support for LTE carrier aggregation
07/15/14   tak     Added interface to support GSM/TDS FW
06/14/14   ckk     Added WCI2 type4 & type7[pwr_bit] support
01/21/13   ckk     Updates to add additional CXM types & renames
01/18/13   hj      Initial proposal

=============================================================================*/

/*=============================================================================

                   COMMENTS/NOTES FOR USAGE/UPDATES

   In order to backward compatibility as well as being able to expose
   essential data types to lower layers here are some ground rules,
   - Only include generic headers and not any tech-specific headers
   - ENUMs & DEFINEs for mask-bits are not version controlled and new values
       can be added as future design needs come up
   - Structures must be versioned to enable support for multiple techs
 
=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
/* To expose compiler supported data types */
#include <comdef.h>

/*=============================================================================

                       CONSTANTS, DEFINES & ENUMS

=============================================================================*/
/*! @brief CXM's Q format used for Tx power threshold passed in active policy message */
#define CXM_TXPWRTHLD_Q_FORMAT 8

/*! @brief CXM's policy bit masks to control enabling/disabling behaviors
      NOTE: these should be the same as in coexistence_service_v01.h 
            so that the QMI port definitions match */
#define CXM_POLICY_NONE                            0x00000000
#define CXM_POLICY_TOGGLE_FRAME_SYNC               0x00000001 /* COEX_PCM_TOGGLE_FRAME_SYNC_V01                  - 0x0000000000000001 */
#define CXM_POLICY_TOGGLE_TX_ACTIVE                0x00000002 /* COEX_PCM_TOGGLE_TX_ACTIVE_V01                   - 0x0000000000000002 */
#define CXM_POLICY_TOGGLE_RX_PRIORITY              0x00000004 /* COEX_PCM_TOGGLE_RX_PRIORITY_V01                 - 0x0000000000000004 */
#define CXM_POLICY_REACT_TO_WCN_PRIORITY           0x00000008 /* COEX_PCM_REACT_TO_WCN_PRIORITY_V01              - 0x0000000000000008 */
#define CXM_POLICY_WCI2_OUTGNG_TYPE3_INACTVTY_DURN 0x00000010 /* COEX_PCM_SEND_WCI2_TYPE3_INACT_DURN_V01         - 0x0000000000000010 */
#define CXM_POLICY_WCI2_OUTGNG_TYPE6_TX_ADV_NOTICE 0x00000020 /* COEX_PCM_SEND_WCI2_TYPE6_TX_ADV_NOTICE_V01      - 0x0000000000000020 */
#define CXM_POLICY_ENFORCE_CTRLR_TX_PWR_LMT        0x00000040 /* COEX_PCM_ENFORCE_CONTROLLER_TX_POWER_LIMIT_V01  - 0x0000000000000040 */
#define CXM_POLICY_WCI2_REACT_TO_INCMNG_TX_PWR_LMT 0x00000080 /* COEX_PCM_REACT_TO_WCI2_TYPE6_TX_POWER_LIMIT_V01 - 0x0000000000000080 */
#define CXM_POLICY_WCI2_SEND_TYPE7_MDM_CONN_STATE  0x00000100 /* COEX_PCM_SEND_WCI2_TYPE7_MDM_CONN_STATE_V01     - 0x0000000000000100 */
#define CXM_POLICY_WCI2_SEND_TYPE7_MDM_PWR_STATE   0x00000200 /* COEX_PCM_SEND_WCI2_TYPE7_MDM_PWR_STATE_V01      - 0x0000000000000200 */
#define CXM_POLICY_WCI2_SEND_TYPE4_SCAN_FREQ       0x00000400 /* COEX_PCM_SEND_WCI2_TYPE4_SCAN_FREQ_V01          - 0x0000000000000400 */
#define CXM_POLICY_WCI2_SEND_TYPE7_MDM_TX_ANT_SEL  0x00000800 /* COEX_PCM_SEND_WCI2_TYPE7_MDM_TX_ANT_SEL_V01     - 0x0000000000000800 */
#define CXM_POLICY_RESERVED_FOR_CONTROLLER         0x00001000 /* COEX_PCM_RESERVED_FOR_CONTROLLER                - 0x0000000000001000 */
#define CXM_POLICY_UNSUPPORTED                     0xFFFFE000

/*! @brief CXM's system-wide behavior control bit masks for enabling 
             various algorithms to be enabled/disabled */
#define CXM_SYS_BHVR_NONE                      0x00000000
#define CXM_SYS_BHVR_WCNTXFRMDNL_WDW_CONS_FRMS 0x00000001
#define CXM_SYS_BHVR_WCNTXFRMDNL_WDW_AVG_FRMS  0x00000002
#define CXM_SYS_BHVR_DISREGARD_RRC_PROC        0x00000004 /*Disregard RRC*/
#define CXM_SYS_BHVR_TRIGGER_LTE_SUBFR_MARKER  0x00000008 /*Dynamically control subframe marker*/
#define CXM_SYS_BHVR_WCI2_DATA                 0x00000010 /*Data plane messages through WCI-2*/
#define CXM_SYS_BHVR_GPIO_DATA                 0x00000020 /*Data plane messages through GPIO*/
#define CXM_SYS_BHVR_VICTIM_TABLE              0x00000040 /*Victim table is processed*/
#define CXM_SYS_BHVR_QMI_POLICY_CONTROL        0x00000080 /*Policy can be controlled through QMI message*/
#define CXM_SYS_BHVR_CXM_SENDS_POLICY          0x00000100 /*Controls whether policy message is sent down or not*/
#define CXM_SYS_BHVR_CXM_SENDS_TYPE6           0x00000200 /*CXM sends type 6 message through the UART*/
#define CXM_SYS_BHVR_CXM_ASSERTS_TX_ADV        0x00000400 /*CXM sends type 0 message for TX active in advance*/
#define CXM_SYS_BHVR_OFF_STATE_ENABLED         0x00000800 /*CXM task shuts down when policy 0 is received*/
#define CXM_SYS_BHVR_CNTR_FRQ_CONFLICT         0x00001000 /*Conflict processing only takes into account the tech center freq*/
#define CXM_SYS_BHVR_WCI2_CONTROL              0x00002000 /*Control plane messages sent through UART using WCI2 type2*/
#define CXM_SYS_BHVR_WLAN_ANT_SHARE            0x00004000 /*Request diversity antenna on behalf of WLAN*/
#define CXM_SYS_BHVR_SMEM_DATA                 0x00008000 /*Data plane messages through SMEM*/
#define CXM_SYS_BHVR_TX_ADV_ALL_SUBFRAMES      0x00010000 /*Send TX advanced message on every subframe*/
#define CXM_SYS_BHVR_QMIL                      0x00020000 /*QMIL messaging enabled - this behavior is no longer supported*/
#define CXM_SYS_BHVR_QMI_METRICS               0x00040000 /*QMI_COEX service supports metrics*/
#define CXM_SYS_BHVR_BASIC_WCI2                0x00080000 /*Configure for basic WCI-2*/
#define CXM_SYS_BHVR_GSM_TDS_HIGH_PRIO_MSG     0x00100000 /* Modem will send high priority events for GSM and TDS */
#define CXM_SYS_BHVR_LTE_HIGH_PRIO_MSG         0x00200000 /* Modem will send high priority events for LTE */
#define CXM_SYS_BHVR_UNSUPPORTED               0xFFC00000 /*Mask indicating unsupported control bits*/

/*!< @brief Calls out names as CXM's port IDs */
typedef enum
{
  CXM_PORT_UNDEF = -1, /*!< indicates undef/unintialized port */
  CXM_PORT_A     =  0,
  CXM_PORT_B,
  CXM_PORT_C,
  CXM_PORT_D,
  CXM_PORT_E,
  CXM_PORT_MAX         /*!< indicates max supported ports */
}cxm_port_id_e;

/*! @brief Defines all the possible port types supported by CXM */
typedef enum
{
  CXM_PORT_TYPE_UNDEF,         /*!< indicates invalid/unintialized port type */
  CXM_PORT_TYPE_GRFC_BASED,    /*!< output only port, routed via a GRFC reg */
  CXM_PORT_TYPE_L2VIC_BASED,   /*!< input only port, routed via a L2VIC reg */
  CXM_PORT_TYPE_GPIO_BASED,    /*!< input only port, routed via a TLMM_reg->GPIO */
  CXM_PORT_TYPE_SMEM_BASED,    /*!< i/o port, mapped via SMEM (shared memory) */
  CXM_PORT_TYPE_SMEM_V2_BASED, /*!< i/o port, mapped via SMEM (shared memory) */
  CXM_PORT_TYPE_MAX            /*!< indicates max supported port types */
}cxm_port_type_e;

/*! @brief Defines the purpose for or the signal driven by the defined CXM port */
typedef enum
{
  CXM_PORT_PURPOSE_UNDEF,        /*!< indicates invalid/uninitialized port */
  CXM_PORT_PURPOSE_FRAME_SYNC,   /*!< output signal to indicate tech's frame start */
  CXM_PORT_PURPOSE_TX_ACTIVE,    /*!< output signal to indicate tech's active Tx */
  CXM_PORT_PURPOSE_RX_ACTIVE,    /*!< output singal to indicate tech's active Rx */
  CXM_PORT_PURPOSE_WCN_PRIORITY, /*!< input signal for tech to react to WCN tech */
  CXM_PORT_PURPOSE_LTE_SUBFR_MARKER, /*!< signal to indicate LTE tech's subframe (every 1mSec) */
  CXM_PORT_PURPOSE_MAX           /*!< indicates max supported purpose for the port */
}cxm_port_purpose_e;

/*! @brief Errors reported by tech's for WCN_PRORITY based Tx/frame blanking */
typedef enum
{
  CXM_WCNTXFRMDNL_ERR_NONE,               /*!< indicates all is okay (unused) */
  CXM_WCNTXFRMDNL_ERR_MAX_CONT_CROSSED,   /*!< COEX_TFDCFM_CONT_TX_FRAME_DENIAL_THLD_CROSSED_V01  - 0x0000000000000001 */
  CXM_WCNTXFRMDNL_ERR_DUTY_CYCLE_CROSSED, /*!< COEX_TFDCFM_TX_FRAME_DENIAL_DUTY_CYCLE_CROSSED_V01 - 0x0000000000000002 */
  CXM_WCNTXFRMDNL_ERR_MAX                 /*!< indicates max supported error enum */
}cxm_wcn_txfrmdnl_err_e;

/*! @brief Events that FW counts. Use this to index into cxm_tech_fw_counters and update the appropriate fields*/
typedef enum
{
  CXM_CNT_FRAME_SYNC,                
  CXM_CNT_TX_ACTIVE,
  CXM_CNT_RX_PRIO,
  CXM_CNT_WCN_PRIO_BLANK_REQ,
  CXM_CNT_WCN_PRIO_BLANK_DENIED,
  CXM_CNT_RESERVED,                /*!< count slot reserved for future use */
  CXM_CNT_MAX_FW_COUNTERS          /*!< indicates max number of FW counters */
} cxm_fw_counters_e;

/*! @brief CXM WWAN Tech Identifier Type. This structure lines up with QMI tech enum */
typedef enum
{
  CXM_TECH_DFLT_INVLD = -1, /*!< Default/Invalid tech ID */
  CXM_TECH_LTE        =  0, /*!< LTE */
  CXM_TECH_TDSCDMA,         /*!< TD-SCDMA */
  CXM_TECH_GSM1,            /*!< GSM on Subscription_1 */
  CXM_TECH_ONEX,            /*!< 1X */
  CXM_TECH_HDR,             /*!< HDR */
  CXM_TECH_WCDMA,           /*!< WCDMA on Subscription_1 */
  CXM_TECH_GSM2,            /*!< GSM on Subscription_2 */
  CXM_TECH_GSM3,            /*!< GSM on Subscription_3 */
  CXM_TECH_WCDMA2,          /*!< WCDMA on Subscription_2 */
  CXM_TECH_MAX              /*!< Max tech ID supported */
}cxm_tech_type;

/*! @brief CXM component carrier IDs for LTE Carrier Aggregation. This structure 
 *         lines up with lte_LL1_carrier_type_enum */
typedef enum
{
  CXM_CARRIER_PCC   = 0,
  CXM_CARRIER_SCC_0 = 1,
  CXM_CARRIER_SCC_1 = 2,
  CXM_CARRIER_MAX
}cxm_carrier_e;

/*=============================================================================

                             DATA TYPES

=============================================================================*/
/*! @brief Indicates the CXM policy in the system (lines up with the QMI intf) */
typedef uint32 cxm_policy_t;

/*! @brief Indicates the CXM system behavior for various algos supported */
typedef uint32 cxm_sys_bhvr_t;

/*! @brief Used to hold all the information for a single CXM port */
typedef struct
{
  cxm_port_id_e   id;                 /*!< used to identify ports internally */
  cxm_port_type_e type;               /*!< what kind of interface does it use/talk to */
  uint32          addr;               /*!< GRFC/L2VIC/TLMM reg # or SMEM address */
  int16           assert_offset_us;   /*!< time before the event to raise the signal
                                           valid range is +/- 200 uSecs (to follow-up)
                                           negative val means before subframe boundary */
  int16           deassert_offset_us; /*!< time after the event to pulldown the signal
                                           valid range is +/- 200 uSecs (to follow-up)
                                           negative val means before subframe boundary */
}cxm_port_config_v01_s;

/*! @brief CXM port interface definition - table of configs for all ports */
typedef struct
{
  uint8                 num_ports;                    /*!< current number of ports in the interface */
  cxm_port_config_v01_s port_info[CXM_PORT_MAX]; /*!< interface details */
}cxm_port_table_v01_s;

/*! @brief Used to indicate the purpose of each port defined in the interface
           NOTE: the port ID must match the one in port_configuration */
typedef struct
{
  cxm_port_id_e      id;      /*!< used to identify ports internally */
  cxm_port_purpose_e purpose; /*!< defines the purpose of the port */
}cxm_port_purpose_mapping_v01_s;

/*! @brief table of ports from the CXM interface and their purpose */
typedef struct
{
  uint8                          num_ports;                       /*!< current number of ports in the interface */
  cxm_port_purpose_mapping_v01_s port_purpose[CXM_PORT_MAX]; /*!< enables tech to determine 
                                                                       how to use the defined ports */
}cxm_port_purpose_mapping_table_v01_s;

/*! @brief defines the various tech's Tx related thresholds */
typedef struct
{
  int16  tx_pwr_thld; /*!< current tech's Tx power threshold to monitor (signed Q8 format) */
  uint16 tx_bw_thld;  /*!< current tech's Tx bandwidth usage to monitor */
}cxm_tech_tx_thlds_v01_s;

/*! @brief define tech's Tx frame blanking duty cycle to ensure proper functionality */
typedef struct
{
  uint32 max_allowed_frame_denials; /*!< Number of maximum allowed frame denials over the window */
  uint32 frame_denial_window;       /*!< Window of frames over which frame denial applies. */
}cxm_wcn_txfrmdnl_duty_cycle_params_v01_s;

/*! @brief tech's tx frame denial/blanking parameters for reacting to incoming WCN priority request */
typedef struct
{
  uint32                                   max_continuous_allowed_frame_denials; /*!< max cont denials */
  cxm_wcn_txfrmdnl_duty_cycle_params_v01_s frame_denial_params;                  /*!< max duty cycle */
}cxm_wcn_txfrmdnl_params_v01_s;

/* CXM counter instance structure */
typedef struct
{
  uint32 count;        /*!< Count for an event */
  uint64 last_tstamp;  /*!< Timestamp of last update in QTIMER (56 bit)*/
}cxm_fw_counter_type_s;

/*! @brief  SMEM entry for each tech */
typedef struct
{
   cxm_fw_counter_type_s counter_event[CXM_CNT_MAX_FW_COUNTERS]; /*!< Counter unit for a FW tech. */
   uint32 reserved;                                              /*!< Reserved */
}cxm_tech_smem_type_s;

/*! @brief Used to communicate a GRFC task from CXM to FW*/
typedef struct
{
  uint32 grfc_num;     /*!< GRFC number to assert/deassert */
  /*lint -save -e123 */
  boolean assert;      /*!< True to assert this GRFC, false to deassert. Lint in place to prevent
                            false errors due to the use of the word 'assert'  */
  /*lint -restore -e123 */
  uint32 trigger_time; /*!< Time in USTMR to assert or deassert this grfc */
}cxm_grfc_task_type_s;

/*! @brief SMEM structure between MCS and FW */
typedef struct
{
   cxm_tech_smem_type_s tech_data[CXM_TECH_MAX];   /*!< FW counter structure for all techs */
   cxm_grfc_task_type_s grfc_task;                 /*!< Control parameters for a grfc. CXM tells FW 
                                                        to read this via IPC */
   uint32 reserved;                                /*!< Reserved */
}cxm_fw_smem_s;

/*! @brief Activity info from CFW. Passed on WCN */
typedef union
{
  struct
  {
    uint32 ustmr_start : 27;     /*!< bit[0:26] - USTMR Start */
    uint32             : 2;      /*!< Reserved */
    uint32 rx	       : 1;      /*!< bit[29] - RX slot if bit value is 1 else no RX */
    uint32 tx          : 1;      /*!< bit[30] - TX slot if bit value is 1 else no TX */
    uint32 check1      : 1;      /*!< bit[31] - Check Bit 1. This value must be thesame as Check Bit 2. 
                                      If they are different, abort read operation and retry immediately */

    uint32 ustmr_stop  : 27;     /*!< bit[32:58] - USTMR Stop */
    uint32 	       : 4;      /*!< Reserved */
    uint32 check2      : 1;      /*!< bit[63] - Check Bit 2. This value must be thesame as Check Bit 1. 
                                      If they are different, abort read operation and retry immediately */
  };
  struct
  {
    uint32 word0;                     
    uint32 word1;                     
  };
  uint64 data;
}cxm_rx_tx_activity_info_u;


/*=============================================================================

                              MESSAGE PAYLOADS

	The idea here is to define payloads which can be used by any underlying
    layers into the MSGR message definitions

=============================================================================*/

/*! @brief Message payload to be sent at boot-up or whenever requested 
            to inform lower layers of current CxM ports & their configs */
typedef struct
{
  cxm_sys_bhvr_t       static_sys_bhvr; /*!< Mask to enable various static CxM sys bhvr */
  cxm_port_table_v01_s port_tbl;        /*!< List of all valid port definitions */
}cxm_boot_config_v01_s;

/*! @brief Message payload to be sent whenever CxM algos are being imposed 
            to inform lower of current policies, thresholds & parameters */
typedef struct
{
  cxm_policy_t                         active_cxm_policy;   /*!< Overall policy to follow */
  cxm_port_purpose_mapping_table_v01_s active_port_mapping; /*!< Current port purpose mapping */
  cxm_sys_bhvr_t                       dynamic_sys_bhvr;    /*!< Mask to enable dynamic CxM sys bhvr */
  cxm_tech_tx_thlds_v01_s              tech_tx_thlds;       /*!< Tech's Tx thresholds over which to limit/blank */
  cxm_wcn_txfrmdnl_params_v01_s        wcn_txfrmdnl_params; /*!< Tx frame denial params when reacting to WCN prio */
}cxm_active_config_v01_s;

/*! @brief Sent to inform LTE lower of current policies, thresholds & parameters 
 *          that apply on a per-component carrier basis */
typedef struct
{
  cxm_policy_t            active_cxm_policy;   /*!< Overall policy to follow */
  cxm_tech_tx_thlds_v01_s tech_tx_thlds;       /*!< Tech's Tx thresholds over which to limit/blank */
}cxm_carrier_config_v01_s;

/*! @brief Message payload to be sent whenever CxM algos are being imposed 
            to inform LTE lower of current policies, thresholds & parameters */
typedef struct
{
  cxm_port_purpose_mapping_table_v01_s active_port_mapping;          /*!< Current port purpose mapping */
  cxm_sys_bhvr_t                       dynamic_sys_bhvr;             /*!< Mask to enable dynamic CxM sys bhvr */
  cxm_wcn_txfrmdnl_params_v01_s        wcn_txfrmdnl_params;          /*!< Tx frame denial params when reacting to WCN prio */
  cxm_carrier_config_v01_s             carrier_cfg[CXM_CARRIER_MAX]; /*!< Per-component carrier config 
      each index corresponds to cxm_carrier_e */
}cxm_active_config_v02_s;

/*! @brief Message payload sent by tech to inform coex manager of system's 
            state as it reacts (honors/denies) to WCN Prio signal */
typedef struct
{
  uint32 num_wcn_reqs;           /*!< Number of requests made by WCN component */
  cxm_wcn_txfrmdnl_err_e result; /*!< Error encountered (if any) */
}cxm_wcn_txfrmdnl_stats_v01_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES
             These are CXM functions that can be called from FW

===========================================================================*/
/*===========================================================================

  FUNCTION:  cxm_smem_handle_fw_msg_intf

===========================================================================*/
/*!
  @brief
    This is the interface function for cxm_smem_handle_fw_msg.
    Send event indication from FW to WCN via smp2p. CFW calls this
    when port ID is CXM_PORT_TYPE_SMEM_BASED.

  @return
    none
*/
/*=========================================================================*/
/*lint -save -e123 */
void cxm_smem_handle_fw_msg_intf (
  cxm_tech_type tech,
  cxm_port_id_e id,
  boolean assert
);
/*lint -restore -e123 */
/*===========================================================================

  FUNCTION:  cxm_smem_handle_fw_activity_intf

===========================================================================*/
/*!
  @brief
    This is the interface function for cxm_smem_handle_fw_activity.
    Send activity indication to WCN via SMEM. CFW calls this
    when port ID is CXM_PORT_TYPE_SMEM_V2_BASED.

  @return
    none
*/
/*=========================================================================*/
void cxm_smem_handle_fw_activity_intf (
  cxm_tech_type tech,
  cxm_rx_tx_activity_info_u activity
);

/*===========================================================================

  FUNCTION:  cxm_req_boot_params_intf

===========================================================================*/
/*!
  @brief
    This is the interface function for cxm_req_boot_params.
    Resend the boot params message. CFW will use this to ask for the boot
    params message. A function call is used here since CFW does not yet
    have a msgr message Q.

  @return
    none
*/
/*=========================================================================*/
void cxm_req_boot_params_intf (
  void
);

#endif /* __CXM_INTF_TYPES_H__ */
