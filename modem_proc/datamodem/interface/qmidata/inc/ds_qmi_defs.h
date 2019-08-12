#ifndef _DS_QMI_DEFS_H
#define _DS_QMI_DEFS_H
/*===========================================================================

                         D S _ Q M I _ D E F S . H

DESCRIPTION

 The Data Services QMI Definitions header file. 
 Contains definitions common to all QMI services.


Copyright (c) 2005-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/19/14    vrk    Added support for current device config through policyman
12/07/13    vrk    Added EAP Notificaton code Indication support
08/08/12    pgm    Support for different APNS in Dual-IP.
03/29/12    sa     New QMI utils api for spc checks.
03/01/12    kk     Fix to clean up RmNet and WDS states coherently.
01/05/12    sa     Changes for mew QMI CSVT service.
10/04/11    sa     Q6 free floating changes for QMI.
08/18/11    sa     GSDI cleanup changes.
06/22/11    wc     Send PRL init indication when PRL is loaded
06/21/11    kk     Added support for DUN entitlement and silent redial control.
02/14/2011  ua     Added support for DUAL IP Autoconnect. 
11/08/10    mct    Support for AKAv2 QMI messages.
06/21/10    kk     Fix to check UIM status before posting commands to MMGSDI.
06/11/10    mib    Added QMI CAT decoded proactive command
06/07/10    kk     Fix to update app_type and uim_state from all possible
                   events.
10/23/09    ar     Add supprot for power save indication filtering and 
                   port configuration from NVRAM.
10/08/09    kk     Windows7 compliance - DMS changes.
04/30/07    ks     Added new qmi cmd to process flow events. 
12/12/06    ks     Added error codes for QoS. Moved qmi_instance enum to
                   external file.
09/05/06    ks     Added new cmds and error codes for PIN support using QMI.
06/07/06    ks     Added new error code QMI_ERR_NO_THRESHOLDS.
04/18/06    ks     Added error codes for Abort. removed unused error codes.
03/22/06    ks     Changed QMI_ERR_IN_USE to QMI_ERR_DEVICE_IN_USE.
03/13/06    ks     Changed QMI_ERR_INVALID_ARG to QMI_ERR_MALFORMED_MSG. 
                   Added some new error codes.
12/02/05    ks     Add new error codes for profiles.
09/08/05    ks     Added new error codes. Updated Radio Interfaces enum.
05/31/05   jd/ks   Code review updates
05/11/05    ks     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "ds_qmi_svc_ext.h"
#include "qmi_svc_defs.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/
                  


/*---------------------------------------------------------------------------
  Max Channel Rates for various radio technologies
---------------------------------------------------------------------------*/
#define QMI_MAX_CH_RATE_CDMA_ONE    (14400)
#define QMI_MAX_CH_RATE_CDMA_1X     (307000)
#define QMI_MAX_CH_RATE_1XEVDO_RX   (2400000)
#define QMI_MAX_CH_RATE_1XEVDO_TX   (300000)
#define QMI_MAX_CH_RATE_GSM         (9600)
#define QMI_MAX_CH_RATE_WCDMA       (3100000) 

/*---------------------------------------------------------------------------
  Max defined data classes (not NV config dependent)
---------------------------------------------------------------------------*/
#define QMI_DEFAULT_GPRS_EDGE_MULTISLOT_CLASS 18
#define QMI_DEFAULT_HSDPA_CAT 10
#define QMI_DEFAULT_HSUPA_CAT 6

/*---------------------------------------------------------------------------
  number of radio interfaces
---------------------------------------------------------------------------*/
#define QMI_N_RADIO_IFS (10)    


/*===========================================================================

                               DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Enum for QMI cmds
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_CMD_MIN                       = 0,
  QMI_CMD_SEND_PKT_STATS            = 0,
  QMI_CMD_MMGSDI_READ_COMPLETE      = 1,
  QMI_CMD_MMGSDI_WRITE_COMPLETE     = 2,
  QMI_CMD_NETWORK_SCAN_COMPLETE     = 3,
  QMI_CMD_NEW_NETWORK_SCAN_INFO     = 5,
  QMI_CMD_NEW_CM_SS_INFO            = 7,
  QMI_CMD_NEW_CM_PH_INFO            = 8,
  QMI_CMD_MMGSDI_OPERATION_COMPLETE = 9, 
  QMI_CMD_PROCESS_PIN_EVT           = 10,
  QMI_CMD_PROCESS_FLOW_EVT          = 11,
  QMI_CMD_CM_CLIENT_ACTIVATE        = 12,
  QMI_CMD_PROCESS_PHYS_LINK_EVT     = 13,
  QMI_CMD_NEW_CM_OPRT_MODE_INFO     = 15,
  QMI_CMD_SET_OPERATING_MODE_RESP   = 16,
  QMI_CMD_PROCESS_WDS_UM_IFACE_EVT  = 17,
  QMI_CMD_PROCESS_QOS_PHYS_LINK_EVT = 18,
  QMI_CMD_EAP_SIM_AKA_TASK_SWITCH_EV = 19,
  QMI_CMD_SEND_EAP_RESP              = 20,
  QMI_CMD_SEND_EAP_RESULT            = 21,
  QMI_CMD_PROCESS_QOS_IFACE_EVT      = 22,
  QMI_CMD_AT_CLIENT_REG_STATUS      = 23,
  QMI_CMD_AT_CMD_REG_STATUS         = 24,
  QMI_CMD_FWD_AT_CMD                = 25,
  QMI_CMD_AT_ABORT_CMD              = 26,
  QMI_CMD_PROCESS_UIM_GET_STATE     = 27,
  QMI_CMD_PDS_LBS_MSG               = 28,
  /* 29 was used for QMI_WMS and can be re-used */
  /* 30 was used for QMI_WMS and can be re-used */
  /* 31 was used for QMI_WMS and can be re-used */
  /* 32 was used for QMI_WMS and can be re-used */
  QMI_CMD_CAT_RAW_PROACTIVE_CMD     = 33,
  QMI_CMD_CAT_RAW_ENV_RSP           = 34,
  QMI_CMD_CAT_SETUP_EVT_LIST_CMD    = 35,
  QMI_CMD_CAT_END_PROACTIVE_CMD     = 36,
  QMI_CMD_CAT_REFRESH_CMD           = 37,
  QMI_CMD_CAT_REFRESH_REG_OP        = 38,
  QMI_CMD_QMUX_PORT_OPEN            = 39,
  QMI_CMD_QMUX_PORT_CLOSE           = 40,
  QMI_CMD_QMUX_PORT_ASSIGN          = 41,
  QMI_CMD_QMUX_PORT_ASSIGN_STATUS   = 42,
  QMI_CMD_PROCESS_POWER_EVT         = 43,
  QMI_CMD_GSDI_PERSO_FEATURE_EVT    = 44,
  QMI_CMD_CTL_SEND_SYNC_IND         = 45,
  QMI_CMD_DMS_CM_CALL_CMD_STATUS    = 46,
  QMI_CMD_PROCESS_CALL_EVT          = 47,
  QMI_CMD_DMS_OTASP_EVT             = 48,
  QMI_CMD_DMS_SEND_ACTIVATION_IND   = 49,
  QMI_CMD_MMGSDI_SESSION_READY      = 50,
  QMI_CMD_PROCESS_DUN_CALL_INFO_EVT = 51,
  QMI_CMD_UIM_OPERATION_COMPLETE    = 52,
  QMI_CMD_UIM_EVENT                 = 53,
  QMI_CMD_NAS_CMPH_EVT_CB           = 54,
  QMI_CMD_NAS_CMPH_CMD_CB           = 55,
  QMI_CMD_NAS_CMSS_EVT_CB           = 56,
  QMI_CMD_NAS_CMSS_CMD_CB           = 57,
  /* 58 was used for QMI_WMS and can be re-used */
  /* 59 was used for QMI_WMS and can be re-used */
  QMI_CMD_CAT_DECODED_PROACTIVE_CMD = 60,
  /* 61 was used for QMI_WMS and can be re-used */
  /* 62 was used for QMI_WMS and can be re-used */
  QMI_CMD_NAS_MMGSDI_CMD_CB         = 63,
  QMI_CMD_NAS_MMGSDI_EVT_CB         = 64,
  QMI_CMD_PROCESS_WDS_PREF_NETWORK_EVT = 65,
  QMI_CMD_PROCESS_DATA_CALL_STATUS_EVT = 66,
  /* 67 was used for data caps and can be re-used */
  QMI_CMD_NAS_CMSTATS_EVT_CB        = 68,
  QMI_CMD_SEND_AKA_ALGO_RESULT      = 69,
  QMI_CMD_NAS_CMSUBS_EVT_CB         = 70,
  QMI_CMD_SEND_DUN_PKT_STATS        = 71,
  /* 72 was used for QMI_WMS and can be re-used */
  QMI_CMD_PROCESS_DUN_PHYS_LINK_EVT = 73,
  QMI_CMD_GET_CLIENT_ID_RESP                = 74,
  QMI_CMD_RELEASE_CLIENT_ID_RESP            = 75,
  QMI_CMD_FRAMEWORK_SEND_RESPONSE           = 76,
  QMI_CMD_FRAMEWORK_SEND_IND                = 77,
  QMI_CMD_FRAMEWORK_LEGACY_SVC_RESPONSE     = 78,
  QMI_CMD_RMNET_OPEN_INSTANCE               = 79,
  QMI_CMD_RMNET_CLOSE_INSTANCE              = 80,
  QMI_CMD_NAS                               = 81,
  QMI_CMD_PROCESS_DUN_RMSM_EVENT    = 82,
  QMI_CMD_CM_PRL_INIT               = 83,
  QMI_CMD_MMGSDI_PERSO_FEATURE_EVT  = 84,
  QMI_CMD_AUTH_INIT_CB             = 85,
  QMI_CMD_AUTH_ALLOC_CLID_CB       = 86,
  QMI_CMD_AUTH_DEALLOC_CLID_CB     = 87,
  QMI_CMD_AUTH_CMD_HDLR_CB         = 88,
  QMI_CMD_WDA_INIT_CB              = 89,
  QMI_CMD_WDA_ALLOC_CLID_CB        = 90,
  QMI_CMD_WDA_DEALLOC_CLID_CB      = 91,
  QMI_CMD_WDA_CMD_HDLR_CB          = 92,
  QMI_CMD_WDS_DATA_SYSTEM_STATUS_CHANGE_EVT = 93,
  QMI_CMD_PROCESS_CALL_THROTTLE_CB  = 94,
  QMI_CMD_PROCESS_EVDO_PMP_CHANGE_EVT = 95,
  QMI_CMD_PROCESS_SET_EVDO_PMP_CB     = 96,
  QMI_CMD_PROCESS_CSVT_CALL_CONNECT_EVT =  97,
  QMI_CMD_PROCESS_CSVT_PROGRESS_INFO_EVT = 98,
  QMI_CMD_PROCESS_CSVT_CONFIRM_INFO_EVT  = 99,
  QMI_CMD_PROCESS_CSVT_INCOMING_INFO_EVT = 100,
  QMI_CMD_PROCESS_CSVT_SETUP_INFO_EVT    = 101,
  QMI_CMD_PROCESS_CSVT_END_INFO_EVT      = 102,
  QMI_CMD_PROCESS_CSVT_MODIFY_INFO_EVT   = 103,
  QMI_CMD_PROCESS_OTKSL_FLAG_UPDATE      = 104,
  QMI_CMD_LEGACY_SVC_ALLOC_CLID       = 105,
  QMI_CMD_LEGACY_SVC_DEALLOC_CLID     = 106,
  QMI_CMD_LEGACY_SVC_QMI_LINK_CLOSED  = 107,
  QMI_CMD_DTR_STATUS                  = 108,
  QMI_CMD_SEND_FLOW_CTRL              = 109,
  QMI_CMD_WDS_INIT_CB                 = 110,
  QMI_CMD_WDS_ALLOC_CLID_CB           = 111,
  QMI_CMD_WDS_DEALLOC_CLID_CB         = 112,
  QMI_CMD_WDS_CMD_HDLR_CB             = 113,
  QMI_CMD_WDS_BIND_DATA_PORT          = 114,
  QMI_CMD_DMS_INIT_CB                 = 115,
  QMI_CMD_DMS_ALLOC_CLID_CB           = 116,
  QMI_CMD_DMS_DEALLOC_CLID_CB         = 117,
  QMI_CMD_DMS_CMD_HDLR_CB             = 118,
  QMI_CMD_PROCESS_WLAN_IFACE_EVT      = 119,
  QMI_CMD_AT_INIT_CB                  = 120,
  QMI_CMD_AT_ALLOC_CLID_CB            = 121,
  QMI_CMD_AT_DEALLOC_CLID_CB          = 122,
  QMI_CMD_AT_CMD_HDLR_CB              = 123,
  QMI_CMD_CSVT_INIT_CB                = 124,
  QMI_CMD_CSVT_ALLOC_CLID_CB          = 125,
  QMI_CMD_CSVT_DEALLOC_CLID_CB        = 126,
  QMI_CMD_CSVT_CMD_HDLR_CB            = 127,
  QMI_CMD_WDS_LTE_ATTACH_PDN_LIST_CHANGE_EVT = 128,
  
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
  QMI_CMD_CTL_SVC_AVAILABLE           = 129,
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
  QMI_CMD_CDMA_LOCK_MODE              = 130,
  QMI_CMD_DMS_EVT_REPORT_REQ_IND      = 131,
  QMI_CMD_DMS_RESET_SHUTDOWN          = 132,
  QMI_CMD_QMAP_CMD_RESP_CB            = 133,
  QMI_CMD_QMAP_CMD_TIMER_EXP_CB       = 134,
  QMI_CMD_TDSCDMA_SET_CONFIG_RES      = 135,
  QMI_CMD_TDSCDMA_GET_CONFIG_RES      = 136,
  QMI_CMD_QOS_INIT_CB                 = 137,
  QMI_CMD_QOS_ALLOC_CLID_CB           = 138,
  QMI_CMD_QOS_DEALLOC_CLID_CB         = 139,
  QMI_CMD_QOS_CMD_HDLR_CB             = 140,
  QMI_CMD_EAP_NOTIFICATION_CODE       = 141,
  QMI_CMD_WDS_PROFILE_CHANGED_EVT     = 142,
  QMI_CMD_WDS_THRPT_INFO_EVT          = 143,
  QMI_CMD_WDS_ROAMING_INFO_EVT        = 144,
  QMI_CMD_DMS_DEV_CURRENT_CAP_CB      = 145,
  QMI_CMD_WDS_OUTAGE_NOTIFICATION_EVT = 146,
  QMI_CMD_QOS_NW_STATUS_CHANGE_EVT    = 147,
  QMI_CMD_WDS_DL_THRPT_INFO_EVT       = 148,
  QMI_CMD_WDS_GEN_REV_IP_CONNECTED_IND = 149,
  QMI_CMD_DMS_POWER_STATISTICS_EVT    = 154,
  QMI_CMD_DMS_PSM_GET_CFG_PARAMS_RESP = 155,
  QMI_CMD_DMS_PSM_STATUS_IND          = 156,
  QMI_CMD_MAX
} qmi_cmd_id_e_type;

/*---------------------------------------------------------------------------
  Enum for radio interfaces
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_RADIO_IF_NO_SVC        = 0x00,  /* Out of service */
  QMI_RADIO_IF_CDMA          = 0x01,  /* CMDA one, 1x */
  QMI_RADIO_IF_CDMA_1XEVDO   = 0x02,  /* CDMA HRPD */
  QMI_RADIO_IF_AMPS          = 0x03,
  QMI_RADIO_IF_GSM           = 0x04,
  QMI_RADIO_IF_UMTS          = 0x05,
  QMI_RADIO_IF_WLAN          = 0x06,
  QMI_RADIO_IF_GPS           = 0x07,
  QMI_RADIO_IF_LTE           = 0x08,
  QMI_RADIO_IF_TDS           = 0x09
} qmi_radio_if_e_type;

/*---------------------------------------------------------------------------
  Enum for the IP family preference
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_AF_MIN        = 0,
  QMI_AF_ANY        = QMI_AF_MIN,
  QMI_AF_INET       = 4,
  QMI_AF_INET6      = 6,
  QMI_AF_UNSPEC     = 8,
  QMI_AF_INET_4_6   = 10,
  QMI_AF_MAX
} qmi_ip_family_e_type;

/*---------------------------------------------------------------------------
  Enum for QMI SPC State 
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_SPC_STATE_UNLOCKED  = 0x00,   
  QMI_SPC_STATE_LOCKED    = 0x01 
} qmi_if_spc_state_e_type;

/*---------------------------------------------------------------------------
  Enum for SPC Check Result 
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_SPC_OK        = 0x00,   
  QMI_SPC_FAIL      = 0x01,  
  QMI_SPC_LOCKED       = 0x02,
  QMI_SPC_ERR_INTERNAL = 0xFF
} qmi_if_spc_result_e_type;

#endif /* _DS_QMI_DEFS_H */
