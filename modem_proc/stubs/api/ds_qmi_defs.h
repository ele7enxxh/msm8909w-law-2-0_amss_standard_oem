#ifndef _DS_QMI_DEFS_H
#define _DS_QMI_DEFS_H
/*===========================================================================

                         D S _ Q M I _ D E F S . H

DESCRIPTION

 The Data Services QMI Definitions header file. 
 Contains definitions common to all QMI services.


Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/ds_qmi_defs.h#1 $ $DateTime: 2016/12/13 07:57:47 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/28/11    sj     Added support for EVDO page monitor period change related
                   messages.
02/14/2011  ua     Added support for DUAL IP Autoconnect. 
06/22/11    wc     Send PRL init indication when PRL is loaded
06/21/11    kk     Added support for DUN entitlement and silent redial control.
06/10/11    sy     Added support for call throttle handler.
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
#include "ds_qmi_svc_ext.h"
/*===========================================================================

                               DEFINITIONS

===========================================================================*/
                  
                        
#define QMI_TYPE_REQUIRED_PARAMETERS  (0x01)
#define QMI_TYPE_RESULT_CODE          (0x02)
#define QMI_TYPE_EXT_ERROR_INFO       (0xE0)
#define QMI_TYPE_PARTIAL_OPER_FAILURE (0xE1)
                                                   
#define QMI_FLAG_MASK_COMPOUND        (0x01)
#define QMI_FLAG_MASK_MSGTYPE         (0x06)

#define QMI_FLAG_MSGTYPE_CMD          (0x00)
#define QMI_FLAG_MSGTYPE_RESP         (0x02)
#define QMI_FLAG_MSGTYPE_IND          (0x04)


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
  QMI_CMD_WMS_CMD_STATUS            = 29,
  QMI_CMD_WMS_MSG_EVT               = 30,
  QMI_CMD_WMS_CFG_EVT               = 31,
  QMI_CMD_WMS_DC_EVT                = 32,
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
  QMI_CMD_WMS_BC_MM_EVT             = 58,
  QMI_CMD_WMS_DC_DISCONNECT         = 59,
  QMI_CMD_CAT_DECODED_PROACTIVE_CMD = 60,
  QMI_CMD_WMS_MMGSDI_CMD_STATUS     = 61,
  QMI_CMD_WMS_MMGSDI_CARD_EVENT     = 62,
  QMI_CMD_NAS_MMGSDI_CMD_CB         = 63,
  QMI_CMD_NAS_MMGSDI_EVT_CB         = 64,
  QMI_CMD_PROCESS_WDS_PREF_NETWORK_EVT = 65,
  QMI_CMD_PROCESS_DATA_CALL_STATUS_EVT = 66,
  QMI_CMD_WDS_DATA_CAP_EVT           = 67,
  QMI_CMD_NAS_CMSTATS_EVT_CB        = 68,
  QMI_CMD_SEND_AKA_ALGO_RESULT      = 69,
  QMI_CMD_NAS_CMSUBS_EVT_CB         = 70,
  QMI_CMD_SEND_DUN_PKT_STATS        = 71,
  QMI_CMD_WMS_TRANSPORT_STATUS_CB   = 72,
  QMI_CMD_PROCESS_DUN_PHYS_LINK_EVT = 73,
  QMI_CMD_PROCESS_CALL_THROTTLE_CB  = 74,
  QMI_CMD_NAS                       = 75,
  QMI_CMD_PROCESS_DUN_RMSM_EVENT    = 76,
  QMI_CMD_CM_PRL_INIT               = 77,
  QMI_CMD_PROCESS_EVDO_PMP_CHANGE_EVT = 78,
  QMI_CMD_PROCESS_SET_EVDO_PMP_CB     = 79,
  QMI_CMD_WDS_DATA_SYSTEM_STATUS_CHANGE_EVT = 80,
  QMI_CMD_MAX
} qmi_cmd_id_e_type;

/*---------------------------------------------------------------------------
  Enum for result code
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_RESULT_SUCCESS  = 0,
  QMI_RESULT_FAILURE  = 1,
  QMI_RESULT_MAX,
  QMI_RESULT_WIDTH    = 0xFFFF
} qmi_result_e_type;


/*---------------------------------------------------------------------------
  Enum for Error code
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_ERR_NONE                  = 0,
  QMI_ERR_MALFORMED_MSG         = 1,
  QMI_ERR_NO_MEMORY             = 2,
  QMI_ERR_INTERNAL              = 3,
  QMI_ERR_ABORTED               = 4,
  QMI_ERR_CLIENT_IDS_EXHAUSTED  = 5,
  QMI_ERR_UNABORTABLE_TRANSACTION  = 6,
  QMI_ERR_INVALID_CLIENT_ID     = 7,
  QMI_ERR_NO_THRESHOLDS         = 8,
  QMI_ERR_INVALID_HANDLE        = 9,
  QMI_ERR_INVALID_PROFILE       = 10,
  QMI_ERR_INVALID_PINID         = 11,
  QMI_ERR_INCORRECT_PIN         = 12,
  QMI_ERR_NO_NETWORK_FOUND       = 13,
  QMI_ERR_CALL_FAILED           = 14,
  QMI_ERR_OUT_OF_CALL           = 15,
  QMI_ERR_NOT_PROVISIONED       = 16,  
  QMI_ERR_MISSING_ARG           = 17,
  QMI_ERR_ARG_TOO_LONG          = 19,
  QMI_ERR_INVALID_TX_ID         = 22,
  QMI_ERR_DEVICE_IN_USE         = 23,
  QMI_ERR_OP_NETWORK_UNSUPPORTED = 24,
  QMI_ERR_OP_DEVICE_UNSUPPORTED  = 25,
  QMI_ERR_NO_EFFECT              = 26,
  QMI_ERR_NO_FREE_PROFILE        = 27,
  QMI_ERR_INVALID_PDP_TYPE       = 28,
  QMI_ERR_INVALID_TECH_PREF      = 29,
  QMI_ERR_INVALID_PROFILE_TYPE   = 30,
  QMI_ERR_INVALID_SERVICE_TYPE   = 31,
  QMI_ERR_INVALID_REGISTER_ACTION  = 32,
  QMI_ERR_INVALID_PS_ATTACH_ACTION = 33,
  QMI_ERR_AUTHENTICATION_FAILED    = 34,
  QMI_ERR_PIN_BLOCKED              = 35,
  QMI_ERR_PIN_PERM_BLOCKED         = 36,
  QMI_ERR_SIM_NOT_INITIALIZED      = 37,
  QMI_ERR_MAX_QOS_REQUESTS_IN_USE  = 38,
  QMI_ERR_INCORRECT_FLOW_FILTER    = 39,
  QMI_ERR_NETWORK_QOS_UNAWARE      = 40,
  QMI_ERR_INVALID_ID               = 41, 
  QMI_ERR_INVALID_QOS_ID           = QMI_ERR_INVALID_ID,
  QMI_ERR_REQUESTED_NUM_UNSUPPORTED = 42,
  QMI_ERR_INTERFACE_NOT_FOUND       = 43,
  QMI_ERR_FLOW_SUSPENDED           = 44,
  QMI_ERR_INVALID_DATA_FORMAT      = 45,
  QMI_ERR_GENERAL                = 46,
  QMI_ERR_UNKNOWN                = 47,
  QMI_ERR_INVALID_ARG            = 48,
  QMI_ERR_INVALID_INDEX          = 49,
  QMI_ERR_NO_ENTRY               = 50,
  QMI_ERR_DEVICE_STORAGE_FULL    = 51,
  QMI_ERR_DEVICE_NOT_READY       = 52,
  QMI_ERR_NETWORK_NOT_READY      = 53,
  QMI_ERR_CAUSE_CODE             = 54,
  QMI_ERR_MESSAGE_NOT_SENT       = 55,
  QMI_ERR_MESSAGE_DELIVERY_FAILURE = 56,
  QMI_ERR_INVALID_MESSAGE_ID     = 57,
  QMI_ERR_ENCODING               = 58,
  QMI_ERR_AUTHENTICATION_LOCK    = 59,
  QMI_ERR_INVALID_TRANSITION     = 60,
  QMI_ERR_NOT_A_MCAST_IFACE        = 61,
  QMI_ERR_MAX_MCAST_REQUESTS_IN_USE= 62,
  QMI_ERR_INVALID_MCAST_HANDLE     = 63, 
  QMI_ERR_INVALID_IP_FAMILY_PREF   = 64,
  QMI_ERR_SESSION_INACTIVE       = 65,
  QMI_ERR_SESSION_INVALID        = 66,
  QMI_ERR_SESSION_OWNERSHIP      = 67,
  QMI_ERR_INSUFFICIENT_RESOURCES = 68,
  QMI_ERR_DISABLED               = 69,
  QMI_ERR_INVALID_OPERATION      = 70,
  QMI_ERR_INVALID_QMI_CMD        = 71,
  QMI_ERR_TPDU_TYPE              = 72,
  QMI_ERR_SMSC_ADDR              = 73,
  QMI_ERR_INFO_UNAVAILABLE       = 74,
  QMI_ERR_SEGMENT_TOO_LONG       = 75,
  QMI_ERR_SEGMENT_ORDER          = 76,
  QMI_ERR_BUNDLING_NOT_SUPPORTED = 77,
  QMI_ERR_OP_PARTIAL_FAILURE     = 78,
  QMI_ERR_POLICY_MISMATCH        = 79,
  QMI_ERR_SIM_FILE_NOT_FOUND     = 80,
  QMI_ERR_EXTENDED_INTERNAL      = 81,
  QMI_ERR_ACCESS_DENIED          = 82,
  QMI_ERR_HARDWARE_RESTRICTED    = 83,
  QMI_ERR_ACK_NOT_SENT           = 84,
  QMI_ERR_INJECT_TIMEOUT         = 85,
  // Sups Errors
  QMI_ERR_INCOMPATIBLE_STATE              = 90,
  QMI_ERR_FDN_RESTRICT                           = 91,
  QMI_ERR_SUPS_FAILURE_CAUSE=92,
  QMI_ERR_NO_RADIO=93,
  QMI_ERR_NOT_SUPPORTED=94,
  QMI_ERR_NO_SUBSCRIPTION          = 95,
  QMI_ERR_CARD_CALL_CONTROL_FAILED = 96,
  QMI_ERR_NETWORK_ABORTED          = 97,
  QMI_ERR_MSG_BLOCKED              = 98,
  // Error 99 is open
  /* QMI PBM Errors */
  QMI_ERR_INVALID_SESSION_TYPE   = 100,
  QMI_ERR_INVALID_PB_TYPE   = 101,
  QMI_ERR_NO_SIM            = 102,
  QMI_ERR_PB_NOT_READY      = 103,
  QMI_ERR_PIN_RESTRICTION       = 104,
  QMI_ERR_PIN2_RESTRICTION      = 105,
  QMI_ERR_PUK_RESTRICTION       = 106,
  QMI_ERR_PUK2_RESTRICTION      = 107,
  QMI_ERR_PB_ACCESS_RESTRICTED  = 108,
  QMI_ERR_PB_DELETE_IN_PROG     = 109,
  QMI_ERR_MESSAGE_DELIVERY_FAILURE_TEMP = 110,
  QMI_ERR_MAX,
  QMI_ERR_WIDTH            = 0xFFFF
} qmi_error_e_type;


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
  QMI_RADIO_IF_LTE           = 0x08
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
  QMI_SPC_LOCKED    = 0x02 
} qmi_if_spc_result_e_type;

#endif /* _DS_QMI_DEFS_H */
