/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         D S _ Q M I _ C F L O G . C

GENERAL DESCRIPTION

  The Data Services Qualcomm MSM Interface Call Flow Logging service.
  This module logs a QXDM log packet 0x14CF (LOG_QMI_CALL_FLOW_C) to the 
  .isf file.  APEX can render a call flow diagram from the .isf file.

EXTERNALIZED FUNCTIONS


Copyright(c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_cflog.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/21/14    sk     QMI-DMS Service QCSI migration.
03/10/14    vrk    CM SS events handling for SGLTE+G CM redesign
10/04/11    sa     Q6 free floating changes for QMI.
01/28/10    dna    Created file.
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include <stdio.h>
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#include "msg.h"
#include "log.h"
#include "err.h"
#include "amssassert.h"
#include "log_codes.h"
#include "ds_qmi_svc_ext.h"
#include "ds_qmi_cflog.h"
#include "ds_qmux.h"
#include "ds_qmi_if.h"

#include "dcc_task_defs.h"
#include "wms.h"


/*===========================================================================
							
              LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Gain access to the cached CM information for detailed logging */
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
//static qmi_if_info_type *qmi_if_info_ptr = &qmi_if_info;

static struct
{
  qmi_result_e_type  result;
  qmi_error_e_type   error;
  boolean initialized;
} qmi_cflog_result;
#endif

/* This macro needs to work for index enum values that can be -1 */
#define QMI_CFLOG_SET_MAX(type, index, arr_size) \
  if ((index >= arr_size - 1) || (index < 0)) { index = (type) (arr_size-1); }

/* But for other enums LINT complains they can't be negative... */
#define QMI_CFLOG_SET_MAX2(type, index, arr_size) \
  if (index >= arr_size - 1) { index = (type) (arr_size-1); }

/* Subsystem type of the call flow */
typedef enum
{
  QMI_CFLOG_SUBSYSTEM_NONE, /* Used for banners e.g. state change that go across all subsystems */
  QMI_CFLOG_SUBSYSTEM_AMSS,
  QMI_CFLOG_SUBSYSTEM_QMI,
  QMI_CFLOG_SUBSYSTEM_CONTROL_POINT
} qmi_cflog_call_flow_subsystem_e_type;

/* Arrow type of the call flow */
typedef enum
{
  /* Bit 2 - Line type [0=Solid line, 1=Dashed line], 
     Bit 1 - Arrow direction [0=One direction, 1=Bi-directional],
     Bit 0 - Arrow head type [0=Solid arrow head, 1=Open arrow head]
  */
  QMI_CFLOG_S_LINE_S_HEAD_S_DIR = 0x00, /* Solid line with solid arrow head pointing at destination */
  QMI_CFLOG_S_LINE_O_HEAD_S_DIR = 0x01, /* Solid line with open arrow head pointing at destination */
  QMI_CFLOG_S_LINE_S_HEAD_B_DIR = 0x02, /* Solid line with solid arrow head pointing at both source and destination */
  QMI_CFLOG_S_LINE_O_HEAD_B_DIR = 0x03, /* Solid line with open arrow head pointing at both source and destination */
  QMI_CFLOG_D_LINE_S_HEAD_S_DIR = 0x04, /* Dashed line with solid arrow head pointing at destination */
  QMI_CFLOG_D_LINE_O_HEAD_S_DIR = 0x05, /* Dashed line with open arrow head pointing at destination */
  QMI_CFLOG_D_LINE_S_HEAD_B_DIR = 0x06, /* Dashed line with solid arrow head pointing at both source and destination */
  QMI_CFLOG_D_LINE_O_HEAD_B_DIR = 0x07  /* Dashed line with open arrow head pointing at both source and destination */
} qmi_cflog_call_flow_arrow_e_type;

/* Call flow log packet */
typedef struct
{
  log_hdr_type hdr;     /* Log header (length, code, timestamp) */
  uint8 src_subsystem;  /* Subsystem generating this call flow event */
  uint8 dest_subsystem; /* Subsystem this call flow event is being sent to */
  uint8 arrow;          /* Bit mask describing the line and arrow type to be drawn on the call flow */
  uint8 label[ 1 ];     /* Used to locate the first character of the Text to be displayed for the banner or arrow */
} qmi_cflog_call_flow_log_packet_type;

/* Log AMSS event/callback */
#define QMI_CFLOG_PKT_AMSS_EVT( label )                                                                                  \
  {                                                                                                                          \
    qmi_cflog_call_flow_packet( QMI_CFLOG_SUBSYSTEM_AMSS, QMI_CFLOG_SUBSYSTEM_QMI, QMI_CFLOG_S_LINE_S_HEAD_S_DIR, label ); \
  }
 
/* Log call into an AMSS function */
#define QMI_CFLOG_PKT_AMSS_CALL( label )                                                                                  \
  {                                                                                                                          \
    qmi_cflog_call_flow_packet( QMI_CFLOG_SUBSYSTEM_QMI, QMI_CFLOG_SUBSYSTEM_AMSS, QMI_CFLOG_S_LINE_S_HEAD_S_DIR, label ); \
  }

/* Log QMI request */
#define QMI_CFLOG_PKT_QMI_REQ( label )                                                                                       \
  {                                                                                                                             \
    qmi_cflog_call_flow_packet( QMI_CFLOG_SUBSYSTEM_CONTROL_POINT, QMI_CFLOG_SUBSYSTEM_QMI, QMI_CFLOG_S_LINE_S_HEAD_S_DIR, label ); \
  }

/* Log QMI request with a bold arrow for important requests */
#define QMI_CFLOG_PKT_QMI_REQ2( label )                                                                                      \
  {                                                                                                                             \
    qmi_cflog_call_flow_packet( QMI_CFLOG_SUBSYSTEM_CONTROL_POINT, QMI_CFLOG_SUBSYSTEM_QMI, QMI_CFLOG_S_LINE_S_HEAD_S_DIR, label ); \
  }

/* Log QMI response */
#define QMI_CFLOG_PKT_QMI_RES( label )                                                                                       \
  {                                                                                                                             \
    qmi_cflog_call_flow_packet( QMI_CFLOG_SUBSYSTEM_QMI, QMI_CFLOG_SUBSYSTEM_CONTROL_POINT, QMI_CFLOG_S_LINE_S_HEAD_S_DIR, label ); \
  }

/* Log QMI indication */
#define QMI_CFLOG_PKT_QMI_IND( label )                                                                                 \
  {                                                                                                                             \
    qmi_cflog_call_flow_packet( QMI_CFLOG_SUBSYSTEM_QMI, QMI_CFLOG_SUBSYSTEM_CONTROL_POINT, QMI_CFLOG_S_LINE_O_HEAD_S_DIR, label ); \
  }

/* Log internal QMI event e.g. between QMI services */
#define QMI_CFLOG_PKT_QMI_EVT( label )                                                                                     \
  {                                                                                                                           \
    qmi_cflog_call_flow_packet( QMI_CFLOG_SUBSYSTEM_QMI, QMI_CFLOG_SUBSYSTEM_QMI, QMI_CFLOG_S_LINE_S_HEAD_S_DIR, label ); \
  }

/* Log state change of QMI (creates a banner line across the diagram) */
#define QMI_CFLOG_PKT_QMI_ST_CHG( label )                                                                                \
  {                                                                                                                         \
    qmi_cflog_call_flow_packet( QMI_CFLOG_SUBSYSTEM_NONE, QMI_CFLOG_SUBSYSTEM_NONE, QMI_CFLOG_D_LINE_S_HEAD_S_DIR, label ); \
  }

/* Service names, for logging */
#define QMI_CFLOG_NUM_KNOWN_SERVICES 11
const char *qmi_cflog_service_name[]={"CTL","WDS","DMS","NAS","QOS","WMS",
                            "PDS","AUTH","AT","VOICE","CAT","Unknown"};

typedef struct
{
  const char *name;
  const uint32 val;
} qmi_cflog_cmd_name_type;

#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
static const qmi_cflog_cmd_name_type qmi_cflog_command_name_ctl[] = 
{
  /* qmi_cflog_command_name[0] - CTL service */
  {"QMI_CTL_SET_INSTANCE_ID",0x0020},
  {"QMI_CTL_GET_VERSION_INFO",0x0021},
  {"QMI_CTL_GET_CLIENT_ID",0x0022},
  {"QMI_CTL_RELEASE_CLIENT_ID",0x0023},
  {"QMI_CTL_SET_DATA_FORMAT",0x0026},
  {"QMI_CTL_SYNC",0x0027},
  {"QMI_CTL_SET_EVENT_REPORT",0x0028},
  {"QMI_CTL_CONFIG_PWR_SAVE_SETTINGS",0x0029},
  {"QMI_CTL_SET_PWR_SAVE_MODE",0x002A},
  {"QMI_CTL_GET_PWR_SAVE_MODE",0x002B},
  {"Unknown CTL service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_wds[] = 
{
  /* qmi_cflog_command_name[1] - WDS service */
  {"QMI_WDS_RESET",0x0000},
  {"QMI_WDS_SET_EVENT_REPORT",0x0001},
  {"QMI_WDS_ABORT",0x0002},
  {"QMI_WDS_START_NETWORK_INTERFACE",0x0020},
  {"QMI_WDS_STOP_NETWORK_INTERFACE",0x0021},
  {"QMI_WDS_GET_PKT_SRVC_STATUS",0x0022},
  {"QMI_WDS_GET_CURRENT_CHANNEL_RATE",0x0023},
  {"QMI_WDS_GET_PKT_STATISTICS",0x0024},
  {"QMI_WDS_GO_DORMANT",0x0025},
  {"QMI_WDS_CREATE_PROFILE",0x0027},
  {"QMI_WDS_MODIFY_PROFILE_SETTINGS",0x0028},
  {"QMI_WDS_DELETE_PROFILE",0x0029},
  {"QMI_WDS_GET_PROFILE_LIST",0x002A},
  {"QMI_WDS_GET_PROFILE_SETTINGS",0x002B},
  {"QMI_WDS_GET_DEFAULT_SETTINGS",0x002C},
  {"QMI_WDS_GET_RUNTIME_SETTINGS",0x002D},
  {"QMI_WDS_SET_MIP_MODE",0x002E},
  {"QMI_WDS_GET_MIP_MODE",0x002F},
  {"QMI_WDS_GET_DORMANCY_STATUS",0x0030},
  {"QMI_WDS_GET_CALL_DURATION",0x0035},
  {"QMI_WDS_GET_CURRENT_DATA_BEARER_TECHNOLOGY",0x0044},
  {"Unknown WDS service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_dms[] = 
{
  /* qmi_cflog_command_name[2] - DMS service */
  {"QMI_DMS_RESET",0x0000},
  {"QMI_DMS_SET_EVENT_REPORT",0x0001},
  {"QMI_DMS_GET_DEVICE_CAP",0x0020},
  {"QMI_DMS_GET_DEVICE_MFR",0x0021},
  {"QMI_DMS_GET_DEVICE_MODEL_ID",0x0022},
  {"QMI_DMS_GET_DEVICE_REV_ID",0x0023},
  {"QMI_DMS_GET_MSISDN",0x0024},
  {"QMI_DMS_GET_DEVICE_SERIAL_NUMBERS",0x0025},
  {"QMI_DMS_GET_POWER_STATE",0x0026},
  {"QMI_DMS_UIM_SET_PIN_PROTECTION",0x0027},
  {"QMI_DMS_UIM_VERIFY_PIN",0x0028},
  {"QMI_DMS_UIM_UNBLOCK_PIN",0x0029},
  {"QMI_DMS_UIM_CHANGE_PIN",0x002A},
  {"QMI_DMS_UIM_GET_PIN_STATUS",0x002B},
  {"QMI_DMS_GET_DEVICE_HARDWARE_REV",0x002C},
  {"QMI_DMS_GET_OPERATING_MODE",0x002D},
  {"QMI_DMS_SET_OPERTAING_MODE",0x002E},
  {"QMI_DMS_GET_TIME",0x002F},
  {"QMI_DMS_UIM_GET_ICCID",0x003C},
  {"QMI_DMS_UIM_GET_CK_STATUS",0x0040},
  {"QMI_DMS_UIM_SET_CK_PROTECTION",0x0041},
  {"QMI_DMS_UIM_UNBLOCK_CK",0x0042},
  {"QMI_DMS_UIM_GET_STATE",0x0044},
  {"QMI_DMS_GET_BAND_CAPABILITY",0x0045},
  {"Unknown DMS service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_nas[] = 
{
  /* qmi_cflog_command_name[3] - NAS service */
  {"QMI_NAS_RESET",0x0000},
  {"QMI_NAS_ABORT",0x0001},
  {"QMI_NAS_SET_EVENT_REPORT",0x0002},
  {"QMI_NAS_INDICATION_REGISTER",0x0003},
  {"QMI_NAS_GET_SIGNAL_STRENGTH",0x0020},
  {"QMI_NAS_PERFORM_NETWORK_SCAN",0x0021},
  {"QMI_NAS_INITIATE_NETWORK_REGISTER",0x0022},
  {"QMI_NAS_INITIATE_ATTACH",0x0023},
  {"QMI_NAS_GET_SERVING_SYSTEM",0x0024},
  {"QMI_NAS_GET_HOME_NETWORK",0x0025},
  {"QMI_NAS_GET_PREFERRED_NETWORKS",0x0026},
  {"QMI_NAS_SET_PREFERRED_NETWORKS",0x0027},
  {"QMI_NAS_GET_FORBIDDEN_NETWORKS",0x0028},
  {"QMI_NAS_SET_FORBIDDEN_NETWORKS",0x0029},
  {"QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE",0x0033},
  {"QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE",0x0034},
  {"QMI_NAS_SET_NAM",0x0035},
  {"QMI_NAS_GET_NAM",0x0036},
  {"QMI_NAS_SET_DDTM_PREFERENCE",0x0037},
  {"Unknown NAS service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_qos[] = 
{
  /* qmi_cflog_command_name[4]- QOS service */
  {"QMI_QOS_RESET",0x0000},
  {"QMI_QOS_SET_EVENT_REPORT",0x0001},
  {"QMI_QOS_REQUEST_QOS",0x0020},
  {"QMI_QOS_RELEASE_QOS",0x0021},
  {"QMI_QOS_SUSPEND_QOS",0x0022},
  {"QMI_QOS_RESUME_QOS",0x0023},
  {"QMI_QOS_MODIFY_QOS",0x0024},
  {"QMI_QOS_GET_GRANTED_QOS_REQ",0x0025},
  {"QMI_QOS_GET_QOS_STATUS_REQ",0x0026},
  {"QMI_QOS_GET_QOS_NETWORK_STATUS",0x0027},
  {"QMI_QOS_GET_NW_SUPPORTED_QOS_PROFILES",0x0028},
  {"Unknown QOS service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_wms[] = 
{
  /* cmd_name[5][x] - WMS service */
  {"QMI_WMS_RESET",0x0000},
  {"QMI_WMS_SET_EVENT_REPORT",0x0001},
  {"QMI_WMS_RAW_SEND",0x0020},
  {"QMI_WMS_RAW_WRITE",0x0021},
  {"QMI_WMS_RAW_READ",0x0022},
  {"QMI_WMS_MODIFY_TAG",0x0023},
  {"QMI_WMS_DELETE",0x0024},
  {"QMI_WMS_GET_MESSAGE_PROTOCOL",0x0030},
  {"QMI_WMS_LIST_MESSAGES",0x0031},
  {"QMI_WMS_SET_ROUTES",0x0032},
  {"QMI_WMS_GET_ROUTES",0x0033},
  {"QMI_WMS_GET_SMSC_ADDRESS",0x0034},
  {"QMI_WMS_SET_SMSC_ADDRESS",0x0035},
  {"QMI_WMS_GET_STORE_MAX_SIZE",0x0036},
  {"QMI_WMS_SEND_ACK",0x0037},
  {"QMI_WMS_SET_RETRY_PERIOD",0x0038},
  {"QMI_WMS_SET_RETRY_INTERVAL",0x0039},
  {"QMI_WMS_SET_DC_DISCONNECT_TIMER",0x003A},
  {"Unknown WMS service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_pds[] = 
{
  /* qmi_cflog_command_name[6] - PDS service */
  {"QMI_PDS_RESET",0x0000},
  {"QMI_PDS_SET_EVENT_REPORT",0x0001},
  {"QMI_PDS_GET_GPS_SERVICE_STATE",0x0020},
  {"QMI_PDS_SET_GPS_SERVICE_STATE",0x0021},
  {"QMI_PDS_GET_NMEA_CONFIG",0x0026},
  {"QMI_PDS_SET_NMEA_CONFIG",0x0027},
  {"QMI_PDS_INJECT_RELATIVE_TIME_REF",0x0028},
  {"QMI_PDS_GET_DEFAULT_TRACKING_SESSION",0x0029},
  {"QMI_PDS_SET_DEFAULT_TRACKING_SESSION",0x002A},
  {"QMI_PDS_GET_XTRA_PARAMETERS",0x002B},
  {"QMI_PDS_SET_XTRA_PARAMETERS",0x002C},
  {"QMI_PDS_FORCE_XTRA_DOWNLOAD",0x002D},
  {"QMI_PDS_GET_AGPS_CONFIG",0x002E},
  {"QMI_PDS_SET_AGPS_CONFIG",0x002F},
  {"QMI_PDS_GET_AUTO_TRACKING_STATE",0x0030},
  {"QMI_PDS_SET_AUTO_TRACKNIG_STATE",0x0031},
  {"QMI_PDS_GET_COM_PORT_CONFIG",0x0032},
  {"QMI_PDS_SET_COM_PORT_CONFIG",0x0033},
  {"QMI_PDS_RESET_GPS_SERVICE_DATA",0x0034},
  {"QMI_PDS_DETERMINE_SINGLE_POSITION",0x0035},
  {"QMI_PDS_GET_GPS_SERVICE_VERSION",0x0036},
  {"QMI_PDS_INJECT_XTRA_DATA",0x0037},
  {"QMI_PDS_INJECT_POSITION_DATA",0x0038},
  {"QMI_PDS_INJECT_WIFI_POSITION_DATA",0x0039},
  {"QMI_PDS_GET_SBAS_CONFIG",0x003A},
  {"QMI_PDS_SET_SBAS_CONFIG",0x003B},
  {"QMI_PDS_SEND_NI_RESPONSE",0x003C},
  {"QMI_PDS_INJECT_ABSOLUTE_TIME_REF",0x003D},
  {"QMI_PDS_INJECT_EFS_DATA",0x003E},
  {"QMI_PDS_GET_DPO_CONFIG",0x003F},
  {"QMI_PDS_SET_DPO_CONFIG",0x0040},
  {"QMI_PDS_GET_ODP_CONFIG",0x0041},
  {"QMI_PDS_SET_ODP_CONFIG",0x0042},
  {"QMI_PDS_CANCEL_SINGLE_POSITION",0x0043},
  {"QMI_PDS_GET_GPS_STATE_INFO",0x0044},
  {"Unknown PDS service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_auth[] = 
{
  /* qmi_cflog_command_name[7] - AUTH service */
  {"QMI_AUTH_START_EAP_SESSION",0x0020},
  {"QMI_AUTH_SEND_EAP_PACKET",0x0021},
  {"QMI_AUTH_GET_EAP_SESSION_KEYS",0x0023},
  {"QMI_AUTH_END_EAP_SESSION",0x0024},
  {"Unknown AUTH service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_at[] = 
{
  /* qmi_cflog_command_name[8] - AT service */
  {"QMI_AT_REG_AT_CMD_FWD",0x0020},
  {"Unknown AT service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_voice[] = 
{
  /* qmi_cflog_command_name[9] - VOICE service */
  {"QMI_VOICE_INDICATION_REGISTER",0x0003},
  {"QMI_VOICE_DIAL_CALL",0x0020},
  {"QMI_VOICE_END_CALL",0x0021},
  {"QMI_VOICE_ANSWER_CALL",0x0022},
  {"QMI_VOICE_GET_CALL_INFO",0x0024},
  {"QMI_VOICE_SEND_FLASH",0x0027},
  {"QMI_VOICE_BURST_DTMF",0x0028},
  {"QMI_VOICE_START_CONT_DTMF",0x0029},
  {"QMI_VOICE_STOP_CONT_DTMF",0x002A},
  {"QMI_VOICE_SET_PREFERRED_PRIVACY",0x002C},
  {"Unknown VOICE service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_cat[] = 
{
  /* qmi_cflog_command_name[10] - CAT service */
  {"QMI_CAT_RESET",0x0000},
  {"QMI_CAT_SET_EVENT_REPORT",0x0001},
  {"QMI_CAT_GET_SERVICE_STATE",0x0020},
  {"QMI_CAT_SEND_TR",0x0021},
  {"QMI_CAT_SEND_ENVELOPE_CMD",0x0022},
  {"Unknown CAT service cmd",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type qmi_cflog_command_name_unknown[] = 
{
  /* qmi_cflog_command_name[11] - unknown service */
  {"CMD for unknown service",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type *qmi_cflog_command_name[] = 
{
  qmi_cflog_command_name_ctl,
  qmi_cflog_command_name_wds,
  qmi_cflog_command_name_dms,
  qmi_cflog_command_name_nas,
  qmi_cflog_command_name_qos,
  qmi_cflog_command_name_wms,
  qmi_cflog_command_name_pds,
  qmi_cflog_command_name_auth,
  qmi_cflog_command_name_at,
  qmi_cflog_command_name_voice,
  qmi_cflog_command_name_cat,
  qmi_cflog_command_name_unknown
};

static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_ctl[] = 
{
  /* qmi_cflog_indication_name[0] - CTL service */
  {"QMI_CTL_REVOKE_CLIENT_ID_IND",0x0024},
  {"QMI_CTL_INVALID_CLIENT_ID_IND",0X0025},
  {"QMI_CTL_SYNC",0x0027},
  {"QMI_CTL_EVENT_REPORT_IND",0x0028},
  {"Unknown CTL service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_wds[] = 
{
  /* qmi_cflog_indication_name[1] - WDS service */
  {"QMI_WDS_EVENT_REPORT_IND",0x0001},
  {"QMI_WDS_GET_PKT_SRVC_STATUS_IND",0x0022},
  {"Unknown WDS service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_dms[] = 
{
  /* qmi_cflog_indication_name[2] - DMS service */
  {"QMI_DMS_SET_EVENT_REPORT_IND",0x0001},
  {"Unknown DMS service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_nas[] = 
{
  /* qmi_cflog_indication_name[3] - NAS service */
  {"QMI_NAS_EVENT_IND",0x0002},
  {"QMI_NAS_SERVING_SYSTEM_IND",0x0024},
  {"QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND",0x0034},
  {"QMI_NAS_CURRENT_NAM_IND",0x0036},
  {"QMI_NAS_DDTM_IND",0x0038},
  {"Unknown NAS service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_qos[] = 
{
  /* qmi_cflog_indication_name[4] - QOS service */
  {"QMI_QOS_SET_EVENT_REPORT_IND",0x0001},
  {"QMI_QOS_STATUS_IND",0x0026},
  {"QMI_QOS_NETWORK_STATUS_IND",0x0027},
  {"QMI_QOS_PRIMARY_QOS_EVENT_IND",0x0029},
  {"Unknown QOS service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_wms[] = 
{
  /* qmi_cflog_indication_name[5] - WMS service */
  {"QMI_WMS_EVENT_REPORT_IND",0x0001},
  {"Unknown WMS service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_pds[] = 
{
  /* qmi_cflog_indication_name[6] - PDS service */
  {"QMI_PDS_EVENT_REPORT_IND",0x0001},
  {"QMI_PDS_GPS_SERVICE_STATE_IND",0x0020},
  {"Unknown PDS service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_auth[] = 
{
  /* qmi_cflog_indication_name[7] - AUTH service */
  {"QMI_AUTH_EAP_SESSION_RESULT_IND",0x0022},
  {"Unknown AUTH service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_at[] = 
{
  /* qmi_cflog_indication_name[8] - AT service */
  {"QMI_AT_FWD_AT_CMD_IND",0x0021},
  {"QMI_AT_ABORT_AT_CMD_IND",0x0023},
  {"Unknown AT service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_voice[] = 
{
  /* qmi_cflog_indication_name[9] - VOICE service */
  {"QMI_VOICE_CALL_STATUS_IND",0x0023},
  {"QMI_VOICE_OTASP_STATUS_IND",0x0025},
  {"QMI_VOICE_INFO_REC_IND",0x0026},
  {"QMI_VOICE_DTMF_IND",0x002B},
  {"QMI_VOICE_PRIVACY_IND",0x002D},
  {"Unknown VOICE service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_cat[] = 
{
  /* qmi_cflog_indication_name[10] - CAT service */
  {"QMI_CAT_EVENT_REPORT_IND",0x0001},
  {"Unknown CAT service ind",0xFFFFFFFF}
};
  
static const qmi_cflog_cmd_name_type qmi_cflog_indication_name_unknown[] = 
{
  /* qmi_cflog_indication_name[11] - unknown service */
  {"IND for unknown service",0xFFFFFFFF}
};

static const qmi_cflog_cmd_name_type *qmi_cflog_indication_name[] = 
{
  qmi_cflog_indication_name_ctl,
  qmi_cflog_indication_name_wds,
  qmi_cflog_indication_name_dms,
  qmi_cflog_indication_name_nas,
  qmi_cflog_indication_name_qos,
  qmi_cflog_indication_name_wms,
  qmi_cflog_indication_name_pds,
  qmi_cflog_indication_name_auth,
  qmi_cflog_indication_name_at,
  qmi_cflog_indication_name_voice,
  qmi_cflog_indication_name_cat,
  qmi_cflog_indication_name_unknown
};
#endif 

const char *qmi_cflog_qmi_err_name[]=
{
  "QMI_ERR_NONE",
  "QMI_ERR_MALFORMED_MSG",
  "QMI_ERR_NO_MEMORY",
  "QMI_ERR_INTERNAL",
  "QMI_ERR_ABORTED",
  "QMI_ERR_CLIENT_IDS_EXHAUSTED",
  "QMI_ERR_UNABORTABLE_TRANSACTION",
  "QMI_ERR_INVALID_CLIENT_ID",
  "QMI_ERR_NO_THRESHOLDS",
  "QMI_ERR_INVALID_HANDLE",
  "QMI_ERR_INVALID_PROFILE",
  "QMI_ERR_INVALID_PINID",
  "QMI_ERR_INCORRECT_PIN",
  "QMI_ERR_NO_NETWORK_FOUND",
  "QMI_ERR_CALL_FAILED",
  "QMI_ERR_OUT_OF_CALL",
  "QMI_ERR_NOT_PROVISIONED",
  "QMI_ERR_MISSING_ARG",
  "unused",
  "QMI_ERR_ARG_TOO_LONG",
  "unused",
  "unused",
  "QMI_ERR_INVALID_TX_ID",
  "QMI_ERR_DEVICE_IN_USE",
  "QMI_ERR_OP_NETWORK_UNSUPPORTED",
  "QMI_ERR_OP_DEVICE_UNSUPPORTED",
  "QMI_ERR_NO_EFFECT",
  "QMI_ERR_NO_FREE_PROFILE",
  "QMI_ERR_INVALID_PDP_TYPE",
  "QMI_ERR_INVALID_TECH_PREF",
  "QMI_ERR_INVALID_PROFILE_TYPE",
  "QMI_ERR_INVALID_SERVICE_TYPE",
  "QMI_ERR_INVALID_REGISTER_ACTION",
  "QMI_ERR_INVALID_PS_ATTACH_ACTION",
  "QMI_ERR_AUTHENTICATION_FAILED",
  "QMI_ERR_PIN_BLOCKED",
  "QMI_ERR_PIN_PERM_BLOCKED",
  "QMI_ERR_SIM_NOT_INITIALIZED",
  "QMI_ERR_MAX_QOS_REQUESTS_IN_USE",
  "QMI_ERR_INCORRECT_FLOW_FILTER",
  "QMI_ERR_NETWORK_QOS_UNAWARE",
  "QMI_ERR_INVALID_ID",
  "QMI_ERR_REQUESTED_NUM_UNSUPPORTED",
  "QMI_ERR_INTERFACE_NOT_FOUND",
  "QMI_ERR_FLOW_SUSPENDED",
  "QMI_ERR_INVALID_DATA_FORMAT",
  "QMI_ERR_GENERAL",
  "QMI_ERR_UNKNOWN",
  "QMI_ERR_INVALID_ARG",
  "QMI_ERR_INVALID_INDEX",
  "QMI_ERR_NO_ENTRY",
  "QMI_ERR_DEVICE_STORAGE_FULL",
  "QMI_ERR_DEVICE_NOT_READY",
  "QMI_ERR_NETWORK_NOT_READY",
  "QMI_ERR_CAUSE_CODE",
  "QMI_ERR_MESSAGE_NOT_SENT",
  "QMI_ERR_MESSAGE_DELIVERY_FAILURE",
  "QMI_ERR_INVALID_MESSAGE_ID",
  "QMI_ERR_ENCODING",
  "QMI_ERR_AUTHENTICATION_LOCK",
  "QMI_ERR_INVALID_TRANSITION",
  "QMI_ERR_NOT_A_MCAST_IFACE",
  "QMI_ERR_MAX_MCAST_REQUESTS_IN_USE",
  "QMI_ERR_INVALID_MCAST_HANDLE",
  "QMI_ERR_INVALID_IP_FAMILY_PREF",
  "QMI_ERR_SESSION_INACTIVE",
  "QMI_ERR_SESSION_INVALID",
  "QMI_ERR_SESSION_OWNERSHIP",
  "QMI_ERR_INSUFFICIENT_RESOURCES",
  "QMI_ERR_DISABLED",
  "QMI_ERR_INVALID_OPERATION",
  "QMI_ERR_INVALID_QMI_CMD",
  "QMI_ERR_TPDU_TYPE",
  "QMI_ERR_SMSC_ADDR",
  "QMI_ERR_INFO_UNAVAILABLE",
  "QMI_ERR_SEGMENT_TOO_LONG",
  "QMI_ERR_SEGMENT_ORDER",
  "QMI_ERR_BUNDLING_NOT_SUPPORTED",
  "QMI_ERR_OP_PARTIAL_FAILURE",
  "QMI_ERR_POLICY_MISMATCH",
  "QMI_ERR_SIM_FILE_NOT_FOUND",
  "QMI_ERR_EXTENDED_INTERNAL",
  "QMI_ERR_ACCESS_DENIED",
  "QMI_ERR_HARDWARE_RESTRICTED",
  "QMI_ERR_ACK_NOT_SENT",
  "Unknown"
};
#define QMI_CFLOG_QMI_ERR_SIZE (sizeof(qmi_cflog_qmi_err_name)/4)

const char *qmi_cflog_amss_cmd_name[]=
{
  "QMI_CMD_SEND_PKT_STATS",
  "QMI_CMD_MMGSDI_READ_COMPLETE",
  "QMI_CMD_MMGSDI_WRITE_COMPLETE",
  "QMI_CMD_NETWORK_SCAN_COMPLETE",
  "", // removed command
  "QMI_CMD_NEW_NETWORK_SCAN_INFO",
  "", // removed command
  "QMI_CMD_NEW_CM_SS_INFO",
  "QMI_CMD_NEW_CM_PH_INFO",
  "QMI_CMD_MMGSDI_OPERATION_COMPLETE",
  "QMI_CMD_PROCESS_PIN_EVT",
  "QMI_CMD_PROCESS_FLOW_EVT",
  "QMI_CMD_CM_CLIENT_ACTIVATE",
  "QMI_CMD_PROCESS_PHYS_LINK_EVT",
  "QMI_CMD_NEW_CM_OPRT_MODE_INFO",
  "QMI_CMD_SET_OPERATING_MODE_RESP",
  "QMI_CMD_PROCESS_WDS_UM_IFACE_EVT",
  "QMI_CMD_PROCESS_QOS_PHYS_LINK_EVT",
  "QMI_CMD_EAP_SIM_AKA_TASK_SWITCH_EV",
  "QMI_CMD_SEND_EAP_RESP",
  "QMI_CMD_SEND_EAP_RESULT",
  "QMI_CMD_PROCESS_QOS_IFACE_EVT",
  "QMI_CMD_AT_CLIENT_REG_STATUS",
  "QMI_CMD_AT_CMD_REG_STATUS",
  "QMI_CMD_FWD_AT_CMD",
  "QMI_CMD_AT_ABORT_CMD",
  "QMI_CMD_PROCESS_UIM_GET_STATE",
  "QMI_CMD_PDS_LBS_MSG",
  "QMI_CMD_WMS_CMD_STATUS",
  "QMI_CMD_WMS_MSG_EVT",
  "QMI_CMD_WMS_CFG_EVT",
  "QMI_CMD_WMS_DC_EVT",
  "QMI_CMD_CAT_RAW_PROACTIVE_CMD",
  "QMI_CMD_CAT_RAW_ENV_RSP",
  "QMI_CMD_CAT_SETUP_EVT_LIST_CMD",
  "QMI_CMD_CAT_END_PROACTIVE_CMD",
  "QMI_CMD_CAT_REFRESH_CMD",
  "QMI_CMD_CAT_REFRESH_REG_OP",
  "QMI_CMD_QMUX_PORT_OPEN",
  "QMI_CMD_QMUX_PORT_CLOSE",
  "QMI_CMD_QMUX_PORT_ASSIGN",
  "QMI_CMD_QMUX_PORT_ASSIGN_STATUS",
  "QMI_CMD_PROCESS_POWER_EVT",
  "QMI_CMD_GSDI_PERSO_FEATURE_EVT",
  "QMI_CMD_CTL_SEND_SYNC_IND",
  "QMI_CMD_DMS_CM_CALL_CMD_STATUS",
  "QMI_CMD_PROCESS_CALL_EVT",
  "QMI_CMD_DMS_OTASP_EVT",
  "QMI_CMD_DMS_SEND_ACTIVATION_IND",
  "QMI_CMD_MMGSDI_SUBSCRIPTION_READY",
  "QMI_CMD_PROCESS_DUN_CALL_INFO_EVT",
  "QMI_CMD_UIM_OPERATION_COMPLETE",
  "QMI_CMD_UIM_EVENT",
  "QMI_CMD_NAS_CMPH_EVT_CB",
  "QMI_CMD_NAS_CMPH_CMD_CB",
  "QMI_CMD_NAS_CMSS_EVT_CB",
  "QMI_CMD_NAS_CMSS_CMD_CB",
  "QMI_CMD_WMS_BC_MM_EVT",
  "QMI_CMD_WMS_DC_DISCONNECT",
  "Unknown"
};
#define QMI_CFLOG_AMSS_CMD_SIZE (sizeof(qmi_cflog_amss_cmd_name)/4)

#define QMI_CFLOG_NUM_KNOWN_META_SM_STATES 6
const char *qmi_cflog_meta_sm_state_name[]=
{
  "NULL",
  "UM_CONFIGURING",
  "RM_CONFIGURING",
  "NET_IF_UP",
  "RM_RECONFIGURING",
  "WAITING_FOR_UM_DOWN",
  "Unknown"
};

#define QMI_CFLOG_NUM_KNOWN_V4_SM_STATES 4
const char *qmi_cflog_v4_sm_state_name[]=
{
  "NULL",
  "DHCP_WAITING",
  "DHCP_IN_PROGRESS",
  "ADDR_CFG_COMPLETE",
  "Unknown"
};

#define QMI_CFLOG_NUM_KNOWN_V6_SM_STATES 3
const char *qmi_cflog_v6_sm_state_name[]=
{
  "NULL",
  "AUTOCFG_IN_PROGRESS",
  "WAIT_FOR_LL_ADDR_RESOL",
  "ADDR_CFG_COMPLETE",
  "Unknown"
};

const char *qmi_cflog_mmgsdi_cnf_name[]=
{
  "MMGSDI_CLIENT_ID_REG_CNF",
  "MMGSDI_CLIENT_EVT_REG_CNF",
  "MMGSDI_CLIENT_ID_DEREG_CNF",
  "MMGSDI_CLIENT_EVT_DEREG_CNF",
  "MMGSDI_READ_CNF",
  "MMGSDI_WRITE_CNF",
  "MMGSDI_GET_FILE_ATTR_CNF",
  "MMGSDI_SESSION_OPEN_CNF",
  "MMGSDI_SESSION_CLOSE_CNF",
  "MMGSDI_ISIM_AUTH_CNF",

  "MMGSDI_SAP_CONNECT_CNF",
  "MMGSDI_SAP_DISCONNECT_CNF",
  "MMGSDI_SAP_GET_ATR_CNF",
  "MMGSDI_SAP_SEND_APDU_CNF",
  "MMGSDI_SEND_APDU_CNF",
  "MMGSDI_SAP_POWER_OFF_CNF",
  "MMGSDI_SAP_CARD_READER_STATUS_CNF",
  "MMGSDI_SAP_POWER_ON_CNF",
  "MMGSDI_SAP_RESET_CNF",
  "MMGSDI_CARD_PDOWN_CNF",

  "MMGSDI_CARD_PUP_CNF",
  "MMGSDI_SAP_INIT_CNF",
  "MMGSDI_SAP_VERIFY_PIN_CNF",
  "MMGSDI_SAP_DEREG_CNF",
  "MMGSDI_CARD_STATUS_CNF",
  "MMGSDI_REFRESH_CNF",
  "MMGSDI_ACTIVATE_ONCHIP_SIM_CNF",
  "MMGSDI_DISABLE_BDN_CNF",
  "MMGSDI_ENABLE_BDN_CNF",
  "MMGSDI_REHABILITATE_CNF",

  "MMGSDI_INVALIDATE_CNF",
  "MMGSDI_PIN_OPERATION_CNF",
  "MMGSDI_SELECT_AID_CNF",
  "MMGSDI_GET_ALL_PIN_STATUS_CNF",
  "MMGSDI_ACL_OPERATION_CNF",
  "MMGSDI_SESSION_DEACTIVATE_CNF",
  "MMGSDI_SESSION_SELECT_AID_CNF",
  "MMGSDI_SRV_AVAILABLE_CNF",
  "MMGSDI_USIM_AUTH_CNF",
  "MMGSDI_COMPUTE_IP_AUTH_CNF",

  "MMGSDI_RUN_CAVE_CNF",
  "MMGSDI_GENERATE_KEY_VPM_CNF",
  "MMGSDI_JCDMA_GET_CARD_INFO_CNF",
  "MMGSDI_CARD_RESET_CNF",
  "MMGSDI_CARD_READER_STATUS_CNF",
  "MMGSDI_GET_ATR_CNF",
  "MMGSDI_MFLO_INIT_ACT_PARAMS_CNF",
  "MMGSDI_MFLO_GET_SUBSCRIBER_ID_CNF",
  "MMGSDI_MFLO_GET_PUBLIC_KEY_CNF",
  "MMGSDI_MFLO_SIGN_UCAST_CNF",

  "MMGSDI_MFLO_VERIFY_UCAST_CNF",
  "MMGSDI_CLIENT_INIT_COMP_CNF",
  "MMGSDI_BCAST_CNF",
  "MMGSDI_SEARCH_CNF",
  "MMGSDI_CREATE_PKCS15_TABLE_CNF",
  "MMGSDI_PERSO_CNF",
  "Unknown"
};
#define QMI_CFLOG_MMGSDI_CNF_SIZE (sizeof(qmi_cflog_mmgsdi_cnf_name)/4)


const char *qmi_cflog_mmgsdi_pin_status_name[]=
{
  "MMGSDI_PIN_STATUS_NOT_INITIALIZED",
  "MMGSDI_PIN_ENABLED_NOT_VERIFIED",
  "MMGSDI_PIN_ENABLED_VERIFIED",
  "MMGSDI_PIN_DISABLED",
  "MMGSDI_PIN_BLOCKED",
  "MMGSDI_PIN_PERM_BLOCKED",
  "MMGSDI_PIN_UNBLOCKED",
  "MMGSDI_PIN_CHANGED",
  "Unknown"
};
#define QMI_CFLOG_MMGSDI_PIN_STATUS_SIZE (sizeof(qmi_cflog_mmgsdi_pin_status_name)/4)

const char *qmi_cflog_mmgsdi_card_event_name[]=
{
  "MMGSDI_CARD_INSERTED_EVT",
  "MMGSDI_CARD_ERROR_EVT",
  "MMGSDI_CARD_REMOVED_EVT",
  "MMGSDI_CARD_INIT_COMPLETED_EVT",
  "MMGSDI_PIN1_EVT",
  "MMGSDI_PIN2_EVT",
  "MMGSDI_UNIVERSAL_PIN_EVT",
  "MMGSDI_TERMINAL_PROFILE_DL_EVT",
  "MMGSDI_REFRESH_EVT",
  "MMGSDI_FDN_EVT",
  "MMGSDI_SWITCH_SLOT_EVT",
  "MMGSDI_ILLEGAL_CARD_EVT",
  "MMGSDI_SAP_CONNECT_EVT",
  "MMGSDI_SAP_DISCONNECT_EVT",
  "MMGSDI_SAP_PIN_EVT",
  "MMGSDI_SESSION_CLOSE_EVT",
  "MMGSDI_SELECT_AID_EVT",
  "Unknown"
};
#define QMI_CFLOG_MMGSDI_CARD_EVENT_SIZE (sizeof(qmi_cflog_mmgsdi_card_event_name)/4)

const char *qmi_cflog_mmgsdi_perso_event_name[]=
{
  "GSDI_PERSO_NO_EVENT",
  "GSDI_PERSO_NW_FAILURE",
  "GSDI_PERSO_NS_FAILURE",
  "GSDI_PERSO_SP_FAILURE",
  "GSDI_PERSO_CP_FAILURE",
  "GSDI_PERSO_SIM_FAILURE",
  "GSDI_PERSO_NW_DEACTIVATED",
  "GSDI_PERSO_NS_DEACTIVATED",
  "GSDI_PERSO_SP_DEACTIVATED",
  "GSDI_PERSO_CP_DEACTIVATED",
  "GSDI_PERSO_SIM_DEACTIVATED",
  "GSDI_PERSO_NCK_BLOCKED",
  "GSDI_PERSO_NSK_BLOCKED",
  "GSDI_PERSO_SPK_BLOCKED",
  "GSDI_PERSO_CCK_BLOCKED",
  "GSDI_PERSO_PPK_BLOCKED",
  "GSDI_PERSO_NCK_UNBLOCKED",
  "GSDI_PERSO_NSK_UNBLOCKED",
  "GSDI_PERSO_SPK_UNBLOCKED",
  "GSDI_PERSO_CCK_UNBLOCKED",
  "GSDI_PERSO_PPK_UNBLOCKED",

  "GSDI_PERSO_RUIM_NW1_FAILURE",
  "GSDI_PERSO_RUIM_NW2_FAILURE",
  "GSDI_PERSO_RUIM_HRPD_FAILURE",
  "GSDI_PERSO_RUIM_SP_FAILURE",
  "GSDI_PERSO_RUIM_CP_FAILURE",
  "GSDI_PERSO_RUIM_RUIM_FAILURE",

  "GSDI_PERSO_RUIM_NW1_DEACTIVATED",
  "GSDI_PERSO_RUIM_NW2_DEACTIVATED",
  "GSDI_PERSO_RUIM_HRPD_DEACTIVATED",
  "GSDI_PERSO_RUIM_SP_DEACTIVATED",
  "GSDI_PERSO_RUIM_CP_DEACTIVATED",
  "GSDI_PERSO_RUIM_RUIM_DEACTIVATED",

  "GSDI_PERSO_RUIM_NCK1_BLOCKED",
  "GSDI_PERSO_RUIM_NCK2_BLOCKED",
  "GSDI_PERSO_RUIM_HNCK_BLOCKED",
  "GSDI_PERSO_RUIM_SPCK_BLOCKED",
  "GSDI_PERSO_RUIM_CCK_BLOCKED",
  "GSDI_PERSO_RUIM_PCK_BLOCKED",

  "GSDI_PERSO_RUIM_NCK1_UNBLOCKED",
  "GSDI_PERSO_RUIM_NCK2_UNBLOCKED",
  "GSDI_PERSO_RUIM_HNCK_UNBLOCKED",
  "GSDI_PERSO_RUIM_SPCK_UNBLOCKED",
  "GSDI_PERSO_RUIM_CCK_UNBLOCKED",
  "GSDI_PERSO_RUIM_PCK_UNBLOCKED",

  "GSDI_PERSO_PROP1_FAILURE",
  "GSDI_PERSO_PROP1_DEACTIVATED",
  "GSDI_PERSO_PROP1_BLOCKED",
  "GSDI_PERSO_PROP1_UNBLOCKED",
  "GSDI_PERSO_SANITY_ERROR",
  "GSDI_PERSO_GEN_PROP1",
  "GSDI_PERSO_GEN_PROP2",
  "GSDI_PERSO_EVT_INIT_COMPLETED",
  "Unknown"
};
#define QMI_CFLOG_MMGSDI_PERSO_EVENT_SIZE (sizeof(qmi_cflog_mmgsdi_perso_event_name)/4)

const char *qmi_cflog_cm_ss_event_name[]=
{
  "CM_SS_EVENT_SRV_CHANGED",
  "CM_SS_EVENT_RSSI",
  "CM_SS_EVENT_INFO",
  "CM_SS_EVENT_REG_SUCCESS",
  "CM_SS_EVENT_REG_FAILURE",
  "CM_SS_EVENT_HDR_RSSI",
  "CM_SS_EVENT_WLAN_RSSI",
  "CM_SS_EVENT_SRV_NEW",
  "CM_SS_EVENT_SECONDARY_MSM",
  "CM_SS_EVENT_PS_DATA_AVAIL",
  "CM_SS_EVENT_PS_DATA_FAIL",
  "CM_SS_EVENT_PS_DATA_SUCCESS",
  "CM_SS_EVENT_WLAN_STATS",
  "CM_SS_EVENT_ORIG_THR_TBL_UPDATE",
  "CM_SS_EVENT_EMERG_NUM_LIST",
  "CM_SS_EVENT_IPAPP_REG_STATUS",
  "CM_SS_EVENT_GW_RSSI",
  "CM_SS_EVENT_HDR_LN_STATUS",
  "CM_SS_EVENT_SELECT_COUNTRY",
  "CM_SS_EVENT_COUNTRY_SELECTED",
  "CM_SS_EVENT_REG_REJECT",
  "Unknown"
};
#define QMI_CFLOG_CM_SS_EVENT_SIZE (sizeof(qmi_cflog_cm_ss_event_name)/4)

const char *qmi_cflog_cm_ss_cmd_name[]=
{
  "CM_SS_CMD_INFO_GET",
  "CM_SS_CMD_PS_DATA_AVAIL",
  "CM_SS_CMD_ORIG_THR_UPDATE_TBL",
  "CM_SS_CMD_COUNTRY_SELECTED",
  "Unknown"
};
#define QMI_CFLOG_CM_SS_CMD_SIZE (sizeof(qmi_cflog_cm_ss_cmd_name)/4)

const char *qmi_cflog_cm_ph_event_name[]=
{
  "CM_PH_EVENT_OPRT_MODE",
  "CM_PH_EVENT_TEST_CONTROL_TYPE",
  "CM_PH_EVENT_SYS_SEL_PREF",
  "CM_PH_EVENT_ANSWER_VOICE",
  "CM_PH_EVENT_NAM_SEL",
  "CM_PH_EVENT_CURR_NAM",
  "CM_PH_EVENT_IN_USE_STATE",
  "CM_PH_EVENT_CDMA_LOCK_MODE",
  "CM_PH_EVENT_UZ_CHANGED",
  "CM_PH_EVENT_MAINTREQ",
  "CM_PH_EVENT_STANDBY_SLEEP",
  "CM_PH_EVENT_STANDBY_WAKE",
  "CM_PH_EVENT_INFO",
  "CM_PH_EVENT_PACKET_STATE",
  "CM_PH_EVENT_INFO_AVAIL",
  "CM_PH_EVENT_SUBSCRIPTION_AVAILABLE",
  "CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE",
  "CM_PH_EVENT_SUBSCRIPTION_CHANGED",
  "CM_PH_EVENT_AVAILABLE_NETWORKS_CONF",
  "CM_PH_EVENT_PREFERRED_NETWORKS_CONF",
  "CM_PH_EVENT_FUNDS_LOW",
  "CM_PH_EVENT_WAKEUP_FROM_STANDBY",
  "CM_PH_EVENT_NVRUIM_CONFIG_CHANGED",
  "CM_PH_EVENT_PREFERRED_NETWORKS_SET",
  "CM_PH_EVENT_DDTM_PREF",
  "CM_PH_EVENT_PS_ATTACH_FAILED",
  "CM_PH_EVENT_RESET_ACM_COMPLETED",
  "CM_PH_EVENT_SET_ACMMAX_COMPLETED",
  "CM_PH_EVENT_CDMA_CAPABILITY_UPDATED",
  "CM_PH_EVENT_LINE_SWITCHING",
  "CM_PH_EVENT_SELECTED_LINE",
  "CM_PH_EVENT_SECONDARY_MSM",
  "CM_PH_EVENT_TERMINATE_GET_NETWORKS",
  "CM_PH_EVENT_DDTM_STATUS",
  "CM_PH_EVENT_CCBS_STORE_INFO_CHANGED",
  "CM_PH_EVENT_WPS_COMPLETED",
  "CM_PH_EVENT_WPS_ABORTED",
  "CM_PH_EVENT_WPS_START",
  "Unknown"
};
#define QMI_CFLOG_CM_PH_EVENT_SIZE (sizeof(qmi_cflog_cm_ph_event_name)/4)

const char *qmi_cflog_cm_ph_cmd_name[] =
{
  "CM_PH_CMD_OPRT_MODE",
  "CM_PH_CMD_SYS_SEL_PREF",
  "CM_PH_CMD_ANSWER_VOICE",
  "CM_PH_CMD_NAM_SEL",
  "CM_PH_CMD_CLIENT_ACT",
  "CM_PH_CMD_INFO_GET",
  "CM_PH_CMD_RSSI_DELTA",
  "CM_PH_CMD_SUBSCRIPTION_AVAILABLE",
  "CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE",
  "CM_PH_CMD_SUBSCRIPTION_CHANGED",
  "CM_PH_CMD_GET_NETWORKS",
  "CM_PH_CMD_TERMINATE_GET_NETWORKS",
  "CM_PH_CMD_SET_PREFERRED_NETWORKS",
  "CM_PH_CMD_PACKET_STATE",
  "CM_PH_CMD_VOC_ACTIVITY",
  "CM_PH_CMD_AVOID_SYS",
  "CM_PH_CMD_WAKEUP_FROM_STANDBY",
  "CM_PH_CMD_CHANGE_RTRE_CONFIG",
  "CM_PH_CMD_DDTM_PREF",
  "CM_PH_CMD_RESET_ACM",
  "CM_PH_CMD_SET_ACMMAX",
  "CM_PH_CMD_LINE_SWITCHING",
  "CM_PH_CMD_SELECTED_LINE",
  "CM_PH_CMD_ALS_SIM_INFO_AVAIL",
  "CM_PH_CMD_LINE_SWITCHING_SAVED",
  "CM_PH_CMD_SELECTED_LINE_SAVED",
  "CM_PH_CMD_UPDATE_CDMA_CAPABILITY",
  "CM_PH_CMD_CLIENT_RELEASE",
  "CM_PH_CMD_SIGNAL_STRENGTH_DELTA",
  "CM_PH_CMD_WPS_START",
  "CM_PH_CMD_WPS_ABORT",
  "Unknown"
};
#define QMI_CFLOG_CM_PH_CMD_SIZE (sizeof(qmi_cflog_cm_ph_cmd_name)/4)

const char *qmi_cflog_cm_call_event_name[]=
{
  "CM_CALL_EVENT_ORIG",
  "CM_CALL_EVENT_ANSWER",
  "CM_CALL_EVENT_END_REQ",
  "CM_CALL_EVENT_END",
  "CM_CALL_EVENT_SUPS",
  "CM_CALL_EVENT_INCOM",
  "CM_CALL_EVENT_CONNECT",
  "CM_CALL_EVENT_SRV_OPT",
  "CM_CALL_EVENT_PRIVACY",
  "CM_CALL_EVENT_PRIVACY_PREF",
  "CM_CALL_EVENT_CALLER_ID",
  "CM_CALL_EVENT_ABRV_ALERT",
  "CM_CALL_EVENT_ABRV_REORDER",
  "CM_CALL_EVENT_ABRV_INTERCEPT",
  "CM_CALL_EVENT_SIGNAL",
  "CM_CALL_EVENT_DISPLAY",
  "CM_CALL_EVENT_CALLED_PARTY",
  "CM_CALL_EVENT_CONNECTED_NUM",
  "CM_CALL_EVENT_INFO",
  "CM_CALL_EVENT_EXT_DISP",
  "CM_CALL_EVENT_NDSS_START",
  "CM_CALL_EVENT_NDSS_CONNECT",
  "CM_CALL_EVENT_EXT_BRST_INTL",
  "CM_CALL_EVENT_NSS_CLIR_REC",
  "CM_CALL_EVENT_NSS_REL_REC",
  "CM_CALL_EVENT_NSS_AUD_CTRL",
  "CM_CALL_EVENT_L2ACK_CALL_HOLD",
  "CM_CALL_EVENT_SETUP_IND",
  "CM_CALL_EVENT_SETUP_RES",
  "CM_CALL_EVENT_CALL_CONF",
  "CM_CALL_EVENT_PDP_ACTIVATE_IND",
  "CM_CALL_EVENT_PDP_ACTIVATE_RES",
  "CM_CALL_EVENT_PDP_MODIFY_REQ",
  "CM_CALL_EVENT_PDP_MODIFY_IND",
  "CM_CALL_EVENT_PDP_MODIFY_REJ",
  "CM_CALL_EVENT_PDP_MODIFY_CONF",
  "CM_CALL_EVENT_RAB_REL_IND",
  "CM_CALL_EVENT_RAB_REESTAB_IND",
  "CM_CALL_EVENT_RAB_REESTAB_REQ",
  "CM_CALL_EVENT_RAB_REESTAB_CONF",
  "CM_CALL_EVENT_RAB_REESTAB_REJ",
  "CM_CALL_EVENT_RAB_REESTAB_FAIL",
  "CM_CALL_EVENT_PS_DATA_AVAILABLE",
  "CM_CALL_EVENT_MNG_CALLS_CONF",
  "CM_CALL_EVENT_CALL_BARRED",
  "CM_CALL_EVENT_CALL_IS_WAITING",
  "CM_CALL_EVENT_CALL_ON_HOLD",
  "CM_CALL_EVENT_CALL_RETRIEVED",
  "CM_CALL_EVENT_ORIG_FWD_STATUS",
  "CM_CALL_EVENT_CALL_FORWARDED",
  "CM_CALL_EVENT_CALL_BEING_FORWARDED",
  "CM_CALL_EVENT_INCOM_FWD_CALL",
  "CM_CALL_EVENT_CALL_RESTRICTED",
  "CM_CALL_EVENT_CUG_INFO_RECEIVED",
  "CM_CALL_EVENT_CNAP_INFO_RECEIVED",
  "CM_CALL_EVENT_EMERGENCY_FLASHED",
  "CM_CALL_EVENT_PROGRESS_INFO_IND",
  "CM_CALL_EVENT_CALL_DEFLECTION",
  "CM_CALL_EVENT_TRANSFERRED_CALL",
  "CM_CALL_EVENT_EXIT_TC",
  "CM_CALL_EVENT_REDIRECTING_NUMBER",
  "CM_CALL_EVENT_PDP_PROMOTE_IND",
  "CM_CALL_EVENT_UMTS_CDMA_HANDOVER_START",
  "CM_CALL_EVENT_UMTS_CDMA_HANDOVER_END",
  "CM_CALL_EVENT_SECONDARY_MSM",
  "CM_CALL_EVENT_ORIG_MOD_TO_SS",
  "CM_CALL_EVENT_USER_DATA_IND",
  "CM_CALL_EVENT_USER_DATA_CONG_IND",
  "CM_CALL_EVENT_MODIFY_IND",
  "CM_CALL_EVENT_MODIFY_REQ",
  "CM_CALL_EVENT_LINE_CTRL",
  "CM_CALL_EVENT_CCBS_ALLOWED",
  "CM_CALL_EVENT_ACT_CCBS_CNF",
  "CM_CALL_EVENT_CCBS_RECALL_IND",
  "CM_CALL_EVENT_CCBS_RECALL_RSP",
  "CM_CALL_EVENT_CALL_ORIG_THR",
  "CM_CALL_EVENT_VS_AVAIL",
  "CM_CALL_EVENT_VS_NOT_AVAIL",
  "CM_CALL_EVENT_MODIFY_COMPLETE_CONF",
  "CM_CALL_EVENT_MODIFY_RES",
  "CM_CALL_EVENT_CONNECT_ORDER_ACK",
  "CM_CALL_EVENT_TUNNEL_MSG",
  "CM_CALL_EVENT_END_VOIP_CALL",
  "CM_CALL_EVENT_VOIP_CALL_END_CNF",
  "CM_CALL_EVENT_PS_SIG_REL_REQ",
  "CM_CALL_EVENT_PS_SIG_REL_CNF",
  "CM_CALL_EVENT_HANDOFF_CALL_ORIG",
  "CM_CALL_EVENT_HANDOFF_CALL_CONNECT",
  "CM_CALL_EVENT_HANDOFF_CALL_END",
  "CM_CALL_EVENT_HANDOFF_MANUAL_CALL_ORIG",
  "CM_CALL_EVENT_MPTY_CALL_IND",
  "CM_CALL_EVENT_OTASP_STATUS",
  "CM_CALL_EVENT_PDP_NW_MODIFY_REQ",
  "CM_CALL_EVENT_PDP_NW_MODIFY_RSP",
  "CM_CALL_EVENT_ACT_BEARER_IND",
  "CM_CALL_EVENT_GET_PDN_CONN_IND",
  "CM_CALL_EVENT_MOD_BEARER_IND",
  "CM_CALL_EVENT_PDN_CONN_REJ_IND",
  "CM_CALL_EVENT_PDN_CONN_FAIL_IND",
  "CM_CALL_EVENT_PDN_DISCONNECT_REJECT_IND",
  "CM_CALL_EVENT_RES_ALLOC_REJ_IND",
  "CM_CALL_EVENT_RES_ALLOC_FAIL_IND",
  "CM_CALL_EVENT_BEARER_MOD_REJ_IND",
  "Unknown"
};
#define QMI_CFLOG_CM_CALL_EVENT_SIZE (sizeof(qmi_cflog_cm_call_event_name)/4)

const char *qmi_cflog_cm_network_sel_mode_pref_name[]=
{
  "AUTOMATIC",
  "MANUAL",
  "LIMITED_SRV",
  "NO_CHANGE",
  "PERSISTENT",
  "HPLMN_SRCH",
  "Unknown"
};
#define QMI_CFLOG_CM_NETWORK_SEL_MODE_PREF_SIZE (sizeof(qmi_cflog_cm_network_sel_mode_pref_name)/4)

const char *qmi_cflog_cm_mode_pref_name[]=
{
  "AMPS_ONLY",
  "DIGITAL_ONLY",
  "AUTOMATIC",
  "EMERGENCY",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "CDMA_ONLY",
  "HDR_ONLY",
  "CDMA_AMPS_ONLY",
  "GPS_ONLY",
  "GSM_ONLY",
  "WCDMA_ONLY",
  "PERSISTENT",
  "NO_CHANGE",
  "ANY_BUT_HDR",
  "CURRENT_LESS_HDR",
  "GSM_WCDMA_ONLY",
  "DIGITAL_LESS_HDR_ONLY",
  "CURRENT_LESS_HDR_AND_AMPS",
  "CDMA_HDR_ONLY",
  "CDMA_AMPS_HDR_ONLY",
  "CURRENT_LESS_AMPS",
  "WLAN_ONLY",
  "CDMA_WLAN",
  "HDR_WLAN",
  "CDMA_HDR_WLAN",
  "GSM_WLAN",
  "WCDMA_WLAN",
  "GW_WLAN",
  "CURRENT_PLUS_WLAN",
  "CURRENT_LESS_WLAN",
  "CDMA_AMPS_HDR_WLAN_ONLY",
  "CDMA_AMPS_WLAN_ONLY",
  "INTERSECT_OR_FORCE",
  "ANY_BUT_HDR_WLAN",
  "LTE_ONLY",
  "GWL",
  "HDR_LTE_ONLY",
  "CDMA_HDR_LTE_ONLY",
  "CDMA_HDR_GW",
  "CDMA_GW",
  "ANY_BUT_WLAN",
  "GWL_WLAN",
  "CDMA_LTE_ONLY",
  "Unknown"
};
#define QMI_CFLOG_CM_MODE_PREF_SIZE (sizeof(qmi_cflog_cm_mode_pref_name)/4)

const char *qmi_cflog_cm_srv_domain_pref_name[]=
{
  "CS_ONLY",
  "PS_ONLY",
  "CS_PS",
  "ANY",
  "NO_CHANGE",
  "PS_ATTACH",
  "PS_DETACH",
  "PERSISTENT",
  "Unknown"
};
#define QMI_CFLOG_CM_SRV_DOMAIN_PREF_SIZE (sizeof(qmi_cflog_cm_srv_domain_pref_name)/4)

const char *qmi_cflog_cm_pref_term_name[]=
{
  "PERMANENT",
  "PWR_CYCLE",
  "1_CALL",
  "1_CALL_OR_TIME",
  "CM_1_CALL",
  "CM_1_CALL_PLUS",
  "CM_MO_SMS",
  "Unknown"
};
#define QMI_CFLOG_CM_PREF_TERM_SIZE (sizeof(qmi_cflog_cm_pref_term_name)/4)

const char *qmi_cflog_cm_hybr_pref_name[]=
{
  "OFF",
  "ON",
  "NO_CHANGE",
  "PERSISTENT",
  "CDMA__HDR_WCDMA",
  "CDMA__WCDMA",
  "Unknown"
};
#define QMI_CFLOG_CM_HBYR_PREF_SIZE (sizeof(qmi_cflog_cm_hybr_pref_name)/4)

const char *qmi_cflog_cm_srv_type_name[]=
{
  "AMPS",
  "HDR",
  "CDMA_SPECIFIC",
  "CDMA_AUTOMATIC",
  "AUTOMATIC",
  "CDMA_HDR",
  "GSM_WCDMA",
  "WCDMA",
  "GSM",
  "WLAN",
  "NON_AUTOMATIC",
  "LTE",
  "GSM_WCDMA_LTE",
  "HDR_MORE_PREF",
  "Unknown"
};
#define QMI_CFLOG_CM_SRV_TYPE_SIZE (sizeof(qmi_cflog_cm_srv_type_name)/4)

const char *qmi_cflog_cm_sys_mode_name[]=
{
  "NO_SRV",
  "AMPS",
  "CDMA",
  "GSM",
  "HDR",
  "WCDMA",
  "GPS",
  "GW",
  "WLAN",
  "LTE",
  "GWL",
  "Unknown"
};
#define QMI_CFLOG_CM_SYS_MODE_SIZE (sizeof(qmi_cflog_cm_sys_mode_name)/4)

const char *qmi_cflog_cm_call_type_name[]=
{
  "VOICE",
  "CS_DATA",
  "PS_DATA",
  "SMS",
  "PD",
  "TEST",
  "OTAPA",
  "STD_OTASP",
  "NON_STD_OTASP",
  "EMERGENCY",
  "SUPS",
  "VT",
  "VT_LOOPBACK",
  "VS",
  "PS_DATA_IS707B",
  "Unknown"
};
#define QMI_CFLOG_CM_CALL_TYPE_SIZE (sizeof(qmi_cflog_cm_call_type_name)/4)

const char *qmi_cflog_cm_call_mode_info_name[]=
{
  "NONE",
  "CDMA",
  "GW_CS",
  "GW_PS",
  "IP",
  "LTE",
  "Unknown"
};
#define QMI_CFLOG_CM_CALL_MODE_INFO_SIZE (sizeof(qmi_cflog_cm_call_mode_info_name)/4)

const char *qmi_cflog_cm_oprt_mode_name[]=
{
  "PWROFF",
  "FTM",
  "OFFLINE",
  "OFFLINE_AMPS",
  "OFFLINE_CDMA",
  "ONLINE",
  "LPM",
  "RESET",
  "NET_TEST_GW",
  "OFFLINE_IF_NOT_FTM",
  "PSEUDO_ONLINE",
  "RESET_MODEM",
  "Unknown"
};
#define QMI_CFLOG_CM_OPRT_MODE_SIZE (sizeof(qmi_cflog_cm_oprt_mode_name)/4)

const char *qmi_cflog_cm_srv_status_name[]=
{
  "NO_SRV",
  "LIMITED",
  "SRV",
  "LIMITED_REGIONAL",
  "PWR_SAVE",
  "Unknown"
};
#define QMI_CFLOG_CM_SRV_STATUS_SIZE (sizeof(qmi_cflog_cm_srv_status_name)/4)

static const qmi_cflog_cmd_name_type qmi_cflog_sups_name[] = 
{
  {"all", 0x00},
  {"allLineIdentificationSS", 0x10},
  {"clip", 0x11},
  {"clir", 0x12},
  {"colp", 0x13},
  {"colr", 0x14},
  {"cnap", 0x19},
  {"mci", 0x1a},
  {"allForwardingSS", 0x20},
  {"cfu", 0x21},
  {"cd", 0x24},
  {"allCondForwardingSS", 0x28},
  {"cfb", 0x29},
  {"cfnry", 0x2a},
  {"cfnrc", 0x2b},
  {"allCallOfferingSS", 0x30},
  {"ect", 0x31},
  {"mah", 0x32},
  {"allCallCompletionSS", 0x40},
  {"cw", 0x41},
  {"hold", 0x42},
  {"ccbs", 0x43},
  {"allMultiPartySS", 0x50},
  {"multiPTY", 0x51},
  {"allCommunityOfInterest_SS", 0x60},
  {"cug", 0x61},
  {"allChargingSS", 0x70},
  {"aoci", 0x71},
  {"aocc", 0x72},
  {"allAdditionalInfoTransferSS", 0x80},
  {"uus", 0x81},
  {"allCallRestrictionSS", 0x90},
  {"barringOfOutgoingCalls", 0x91},
  {"baoc", 0x92},
  {"boic", 0x93},
  {"boicExHC", 0x94},
  {"barringOfIncomingCalls", 0x99},
  {"baic", 0x9a},
  {"bicRoam", 0x9b},
  {"allPLMN_specificSS", 0xf0},
  {"chargingInfoId", 0xa1},
  {"Unknown Supplementary Service ID",0xFFFFFFFF}
};

const char *qmi_cflog_wms_memory_store_name[]=
{
  "NONE",
  "RAM_GW",
  "SIM",
  "NV_GW",
  "RAM_CDMA",
  "RUIM",
  "NV_CDMA",
  "Unknown"
};
#define QMI_CFLOG_WMS_MEMORY_STORE_SIZE (sizeof(qmi_cflog_wms_memory_store_name)/4)

const char *qmi_cflog_ps_event_name[]=
{
  "PHYS_LINK_DOWN",
  "PHYS_LINK_COMING_UP",
  "PHYS_LINK_UP",
  "PHYS_LINK_GOING_DOWN",
  "PHYS_LINK_GONE",
  "PHYS_LINK_FLOW_ENABLED",
  "PHYS_LINK_FLOW_DISABLED",
  "PHYS_LINK_RESUMING",
  "PHYS_LINK_GOING_NULL",
  "PHYS_LINK_707_DOS_ACK",

  "PHYS_LINK_MAX",
  "IFACE_UP",
  "IFACE_GOING_DOWN",
  "IFACE_ENABLED",
  "IFACE_DISABLED",
  "IFACE_DOWN",
  "IFACE_COMING_UP",
  "IFACE_CONFIGURING",
  "IFACE_FLOW_ENABLED",
  "IFACE_FLOW_DISABLED",
  "IFACE_ROUTEABLE",
  "IFACE_ADDR_CHANGED",
  "IFACE_MTU_CHANGED",
  "IFACE_DELETE",
  "IFACE_IPFLTR_UPDATED",
  "IFACE_PRI_PHYS_LINK_CHANGED",
  "IFACE_PREFIX_UPDATE",
  "IFACE_VALID_RA",
  "IFACE_ADDR_FAMILY_CHANGED",
  "IFACE_MT_REQUEST",
  "IFACE_MCAST_REGISTER_SUCCESS",
  "IFACE_MCAST_REGISTER_FAILURE",
  "IFACE_MCAST_DEREGISTERED",
  "IFACE_BEARER_TECH_CHANGED",
  "IFACE_QOS_AWARE_SYSTEM",
  "IFACE_QOS_UNAWARE_SYSTEM",
  "IFACE_FLOW_ADDED",
  "IFACE_FLOW_DELETED",

  "IFACE_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS",
  "IFACE_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE",

  "IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS",
  "IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE",
  "IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED",

  "IFACE_IPV6_PRIV_ADDR_GENERATED",
  "IFACE_IPV6_PRIV_ADDR_DEPRECATED",
  "IFACE_IPV6_PRIV_ADDR_EXPIRED",
  "IFACE_IPV6_PRIV_ADDR_DELETED",
  "IFACE_OUTAGE_NOTIFICATION",
  "IFACE_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED",
  "IFACE_RF_CONDITIONS_CHANGED",
  "IFACE_EXTENDED_IP_CONFIG",
  "IFACE_LINGERING",

  "IFACE_MBMS_CONTEXT_ACT_SUCCESS",
  "IFACE_MBMS_CONTEXT_ACT_FAILURE",
  "IFACE_MBMS_CONTEXT_DEACT_SUCCESS",
  "IFACE_MBMS_CONTEXT_DEACT_FAILURE",

  "IFACE_AUTHENTICATING",
  "IFACE_APP_PREEMPTED",
  "IFACE_MCAST_STATUS",
  "IFACE_ALL",
  "FLOW_NULL",
  "FLOW_ACTIVATING",
  "FLOW_ACTIVATED",
  "FLOW_SUSPENDING",
  "FLOW_SUSPENDED",
  "FLOW_RESUMING",
  "FLOW_GOING_NULL",
  "FLOW_CONFIGURING",
  "FLOW_TX_ENABLED",
  "FLOW_TX_DISABLED",
  "FLOW_RX_FLTR_UPDATED",
  "FLOW_MODIFY_ACCEPTED",
  "FLOW_MODIFY_REJECTED",
  "FLOW_PRIMARY_MODIFY_RESULT",
  "FLOW_INFO_CODE_UPDATED",
  "Unknown"
};
#define QMI_CFLOG_PS_EVENT_SIZE (sizeof(qmi_cflog_ps_event_name)/4)

const char *qmi_cflog_conn_status_name[]=
{
  "Undefined",
  "Disconnected",
  "Connected",
  "Suspended",
  "Authenticating",
  "Unknown"
};
#define QMI_CFLOG_CONN_STATUS_SIZE (sizeof(qmi_cflog_conn_status_name)/4)

/*WMS event numbers have big gaps so use a hash */
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
static const qmi_cflog_cmd_name_type qmi_cflog_wms_cmd_name[] =
{
  {"WMS_CMD_MSG_SEND",WMS_CMD_MSG_SEND},
  {"WMS_CMD_MSG_WRITE",WMS_CMD_MSG_WRITE},
  {"WMS_CMD_MSG_READ",WMS_CMD_MSG_READ},
  {"WMS_CMD_MSG_READ_TEMPLATE",WMS_CMD_MSG_READ_TEMPLATE},
  {"WMS_CMD_MSG_WRITE_TEMPLATE",WMS_CMD_MSG_WRITE_TEMPLATE},
  {"WMS_CMD_CFG_SET_ROUTES",WMS_CMD_CFG_SET_ROUTES},
  {"WMS_CMD_CFG_GET_ROUTES",WMS_CMD_CFG_GET_ROUTES},
  {"WMS_CMD_CFG_GET_MESSAGE_LIST",WMS_CMD_CFG_GET_MESSAGE_LIST},
  {"WMS_CMD_CFG_GET_MEMORY_STATUS",WMS_CMD_CFG_GET_MEMORY_STATUS},
  {"WMS_CMD_MSG_DELETE",WMS_CMD_MSG_DELETE},
  {"WMS_CMD_MSG_DELETE_ALL",WMS_CMD_MSG_DELETE_ALL},
  {"WMS_CMD_MSG_MODIFY_TAG",WMS_CMD_MSG_MODIFY_TAG},
  {"WMS_CMD_MSG_ACK",WMS_CMD_MSG_ACK},
  {"WMS_CMD_DBG_SET_RETRY_INTERVAL",WMS_CMD_DBG_SET_RETRY_INTERVAL},
  {"WMS_CMD_DC_ENABLE_AUTO_DISCONNECT",WMS_CMD_DC_ENABLE_AUTO_DISCONNECT},
  {"WMS_CMD_DC_DISABLE_AUTO_DISCONNECT",WMS_CMD_DC_DISABLE_AUTO_DISCONNECT},
  {"WMS_CMD_DC_CONNECT",WMS_CMD_DC_CONNECT},
  {"Unknown",0xFFFFFFFF}
};
#endif

const char *qmi_cflog_wms_event_name[]=
{
  "WMS_MSG_EVENT_SEND",
  "WMS_MSG_EVENT_ACK",
  "WMS_MSG_EVENT_READ",
  "WMS_MSG_EVENT_WRITE",
  "WMS_MSG_EVENT_DELETE",
  "WMS_MSG_EVENT_DELETE_ALL",
  "WMS_MSG_EVENT_MODIFY_TAG",
  "WMS_MSG_EVENT_READ_TEMPLATE",
  "WMS_MSG_EVENT_WRITE_TEMPLATE",
  "WMS_MSG_EVENT_DELETE_TEMPLATE",
  "WMS_MSG_EVENT_DELETE_TEMPLATE_ALL",
  "WMS_MSG_EVENT_READ_STS_REPORT",
  "WMS_MSG_EVENT_WRITE_STS_REPORT",
  "WMS_MSG_EVENT_DELETE_STS_REPORT",
  "WMS_MSG_EVENT_DELETE_STS_REPORT_ALL",
  "WMS_MSG_EVENT_RECEIVED_MESSAGE",
  "WMS_MSG_EVENT_SUBMIT_REPORT",
  "WMS_MSG_EVENT_STATUS_REPORT",
  "WMS_MSG_EVENT_MT_MESSAGE_ERROR",
  "WMS_MSG_EVENT_ACK_REPORT",
  "WMS_MSG_EVENT_DUPLICATE_CB_PAGE",
  "Unknown"
};
#define QMI_CFLOG_WMS_EVENT_SIZE (sizeof(qmi_cflog_wms_event_name)/4)

const char *qmi_cflog_wms_cfg_event_name[]=
{
  "WMS_CFG_EVENT_GW_READY",
  "WMS_CFG_EVENT_CDMA_READY",
  "WMS_CFG_EVENT_ROUTES",
  "WMS_CFG_EVENT_MEMORY_STATUS",
  "WMS_CFG_EVENT_MESSAGE_LIST",
  "WMS_CFG_EVENT_MEMORY_FULL",
  "WMS_CFG_EVENT_GW_DOMAIN_PREF",
  "WMS_CFG_EVENT_CELL_CHANGE",
  "WMS_CFG_EVENT_PRIMARY_CLIENT_SET",
  "WMS_CFG_EVENT_MEMORY_STATUS_SET",
  "WMS_CFG_EVENT_LINK_CONTROL",
  "Unknown"
};
#define QMI_CFLOG_WMS_CFG_EVENT_SIZE (sizeof(qmi_cflog_wms_cfg_event_name)/4)


const char *qmi_cflog_wms_dc_event_name[]=
{
  "WMS_DC_EVENT_INCOMING",
  "WMS_DC_EVENT_CONNECTED",
  "WMS_DC_EVENT_ABORTED",
  "WMS_DC_EVENT_DISCONNECTED",
  "WMS_DC_EVENT_CONNECTING",
  "WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT",
  "WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT",
  "Unknown"
};
#define QMI_CFLOG_WMS_DC_EVENT_SIZE (sizeof(qmi_cflog_wms_dc_event_name)/4)


/*===========================================================================

                           FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
/*===========================================================================
  FUNCTION QMI_CFLOG_GET_SVC_NAME

  DESCRIPTION
    This function returns a string describing the QMI command/indication.

  PARAMETERS
    src_id - QMI service the command belongs to

  DEPENDENCIES
    None

  RETURN VALUE
    String which is the service name

  SIDE EFFECTS
    None
===========================================================================*/
static const char * qmi_cflog_get_svc_name
(
  qmux_service_e_type svc_id
)
{
  QMI_CFLOG_SET_MAX2 (qmux_service_e_type, svc_id, QMI_CFLOG_NUM_KNOWN_SERVICES);

  return qmi_cflog_service_name[svc_id];

} /* qmi_cflog_get_svc_name */
#endif /*FEATURE_QMI_CALL_FLOW_LOGGING*/

/*===========================================================================
  FUNCTION QMI_CFLOG_FIND_NAME

  DESCRIPTION
    This function searches a hash of string/value pairs and returns the 
    string matching the given value.  It assumes the hash is terminated 
    with an entry with value 0xFFFFFFFF.

  PARAMETERS
    names - Hash of string/value pairs

  DEPENDENCIES
    None

  RETURN VALUE
    String from the hash matching the given value

  SIDE EFFECTS
    None
===========================================================================*/
static const char* qmi_cflog_find_name
(
  const qmi_cflog_cmd_name_type *names,
  uint32 val
)
{
  int i=0; /* loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i = 0; names[i].val != 0xFFFFFFFF; i++)
  {
    if (names[i].val == val) { break; }
  }

  return names[i].name;
} /* qmi_cflog_find_name */

#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
/*===========================================================================
  FUNCTION QMI_CFLOG_GET_QMI_CMD_NAME

  DESCRIPTION
    This function returns a string describing the QMI command/indication.

  PARAMETERS
    src_id - QMI service the command belongs to
    cmd_id - The command number
    type - Command, Response, or Indication

  DEPENDENCIES
    None

  RETURN VALUE
    String describing the command/indication

  SIDE EFFECTS
    None
===========================================================================*/
static const char* qmi_cflog_get_qmi_cmd_name
(
  qmux_service_e_type svc_id,
  uint16 cmd_id,
  uint8 type
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX2 (qmux_service_e_type, svc_id, QMI_CFLOG_NUM_KNOWN_SERVICES);

  if (type == QMI_FLAG_MSGTYPE_IND)
  {
    return (qmi_cflog_find_name(qmi_cflog_indication_name[svc_id], cmd_id));
  }

  return (qmi_cflog_find_name(qmi_cflog_command_name[svc_id], cmd_id));

} /* qmi_cflog_get_qmi_cmd_name */
#endif /* FEATURE_QMI_CALL_FLOW_LOGGING*/

/*===========================================================================
  FUNCTION QMI_CFLOG_CALL_FLOW_PACKET

  DESCRIPTION
    Function to log the call flow log packet.  A .isf file containing these
    packets can be parsed by APEX "view->call flow analysis" option to 
    produce an HTML rendered call flow of interactions between AMSS
    tech areas (CM, WMS, UIM, etc.), QMI (this module) and the QMI control
    points.  For example:

    AMSS             QMI        Control Point
      |               |<--- QMI_CMD ---|
      |<--- func() ---|                |
      |--- cback() -->|                |
      |               |--- QMI_RSP --->|

  PARAMETERS
    src_subsystem - AMSS, QMI, or Control Point
    dest_subsystem - AMSS, QMI, or Control Point
    arrow - The direction and type of arrow
    label - Text to be displayed on the arrow

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cflog_call_flow_packet
(
  qmi_cflog_call_flow_subsystem_e_type src_subsystem,
  qmi_cflog_call_flow_subsystem_e_type dest_subsystem,
  qmi_cflog_call_flow_arrow_e_type arrow,
  char *label
)
{
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
  qmi_cflog_call_flow_log_packet_type *log_buf;                                                                
  uint16 label_len, log_packet_size = 0;                                                                   
                                                                                                           
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Calculate the size of the log packet */                                                              
  label_len = strlen( label );                                                                             
  log_packet_size = sizeof( qmi_cflog_call_flow_log_packet_type ) + label_len;                                

  /* Allocate log buffer, reuse the QCRIL call flow packet for now */
  log_buf = ( qmi_cflog_call_flow_log_packet_type * ) log_alloc(LOG_QMI_CALL_FLOW_C, log_packet_size ); 
                                                                                                           
  if ( log_buf != NULL )
  {                                                                         
    /* Fill in the log buffer */                                                                            
    log_buf->src_subsystem = (uint8) src_subsystem;                                                          
    log_buf->dest_subsystem = (uint8) dest_subsystem;                                                       
    log_buf->arrow = (uint8) arrow;                                                                        
    log_buf->label[ 0 ] = '\0';                                                                             
    if ( label_len > 0 )                                                                                    
    {                                                                                                       
      memscpy( (void *) log_buf->label, 
                label_len + 1, 
                label, 
                label_len + 1 );
    }                                                                                                      

    /* Commit log buffer */                                                                                  
    log_commit( log_buf ); 
  }
#else
  return;
#endif
} /* qmi_cflog_call_flow_packet */

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_CLIENT_REQUEST

  DESCRIPTION
    This function logs a QMI request received from a control point.

  PARAMETERS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_client_request
( 
  qmux_svc_info_type *svc_s,
  qmi_cmd_buf_type *cmd_buf_p
)
{
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
  char label[80]={0};
  qmi_instance_e_type qmi_instance;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (svc_s);
  ASSERT (cmd_buf_p);

  qmi_instance = qmi_instance_by_qmux_handle (svc_s);

  (void) snprintf (label, 80, "%s  %s  [%d RMNET%d]",
          qmi_cflog_get_svc_name( svc_s->service_id ),
          qmi_cflog_get_qmi_cmd_name( svc_s->service_id, 
                              cmd_buf_p->cmd_type, 
                              QMI_FLAG_MSGTYPE_CMD ),
          (cmd_buf_p->x_p && cmd_buf_p->x_p->cl_sp)?
          cmd_buf_p->x_p->cl_sp->clid:0, qmi_instance+1);

  QMI_CFLOG_PKT_QMI_REQ( label );
#else
  return;
#endif

} /* ds_qmi_cflog_client_request */

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_CLIENT_RESP_RESULT

  DESCRIPTION
    This function stores the result code of the next QMI client response.
    There should always be a call to this function immediately before 
    ds_qmi_cflog_client_resp().  This relies on the code convention used
    in QMI.  Otherwise we would need to parse the result TLV back out of 
    the response or else put log messages all over the place...

  PARAMETERS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_client_resp_result
( 
  qmi_result_e_type  result,
  qmi_error_e_type   error
)
{
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qmi_cflog_result.initialized = TRUE;
  qmi_cflog_result.error = error;
  qmi_cflog_result.result = result;
#else
  return;
#endif
} /* ds_qmi_cflog_client_resp_result */

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_CLIENT_RESP

  DESCRIPTION
    This function logs a QMI response or indication for a control point.

  PARAMETERS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_client_resp
( 
  qmux_svc_info_type *svc_s,
  qmi_cmd_buf_type *cmd_buf_p,
  uint8 msg_type, 
  boolean filter_active
)
{
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
  char label[80] = {0};
  qmi_instance_e_type qmi_instance;
  uint32 temp_index;
  const char *error = "";

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (svc_s);
  ASSERT (cmd_buf_p);

  qmi_instance = qmi_instance_by_qmux_handle (svc_s);

  /* Add error/result to responses */
  if (msg_type == QMI_FLAG_MSGTYPE_RESP)
  {
    if (!qmi_cflog_result.initialized)
    {
      error = "Result Uninitialized!";
    }
    else if (qmi_cflog_result.result == QMI_RESULT_SUCCESS)
    {
      error = "Success";
    }
    else
    {
      temp_index = (uint32) qmi_cflog_result.error;
      QMI_CFLOG_SET_MAX2 (uint32, temp_index, QMI_CFLOG_QMI_ERR_SIZE);
      error = qmi_cflog_qmi_err_name[temp_index];
    }
    qmi_cflog_result.initialized = FALSE;
  }

  (void) snprintf (label, 80, "%s %s %s [%d RMNET%d] %s",
           qmi_cflog_get_svc_name( svc_s->service_id ),
           qmi_cflog_get_qmi_cmd_name( svc_s->service_id,                               
                                       cmd_buf_p->cmd_type, 
                                       msg_type ),
           (filter_active)?"FILTERED":"",
           (cmd_buf_p->x_p && cmd_buf_p->x_p->cl_sp)?
           cmd_buf_p->x_p->cl_sp->clid:0, qmi_instance+1, error);
     

  if (msg_type == QMI_FLAG_MSGTYPE_IND)
  {
    QMI_CFLOG_PKT_QMI_IND( label );
  }
  else
  {
    QMI_CFLOG_PKT_QMI_RES( label );
  }
#else
  return;
#endif
} /* ds_qmi_cflog_client_resp */

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_AMSS_CALL

  DESCRIPTION
    This function logs calls into AMSS that result in a response i.e. a
    command queued back to QMI that evantually completes a QMI request.

  PARAMETERS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_amss_call
( 
  char *amss_func_name
)
{
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_PKT_AMSS_CALL( amss_func_name );

#else
  return;
#endif
} /* ds_qmi_cflog_amss_call */

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_AMSS_RSP

  DESCRIPTION
    This function logs an AMSS response or indication for QMI.

  PARAMETERS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_amss_rsp
( 
  dcc_cmd_data_buf_type  *cmd_ptr
)
{
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
  char label[80];
  const char *cmd_name;
  uint8 sub_id;
  int temp_index, temp_index2;
  dcc_qmi_cmd_type *qptr;
  qmi_if_cm_ss_info_type *ss_info_ptr;
  qmi_if_cm_ph_info_type *ph_info_ptr;
  char *evdo_rev[]={ "","","Rel 0","Rel A","Rel B", "Unknown" };
  qmi_if_info_type  *qmi_if_info_ptr;
  qmi_if_cmd_buf_type *qmi_if_cmd_buf_ptr;
  qmi_dmsi_cmd_buf_type *qmi_dmsi_cmd_buf_ptr;
#define QMI_CFLOG_EVDO_REV_SIZE (sizeof(evdo_rev)/4)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cmd_ptr );
  qptr = &cmd_ptr->cmd.qmi; /* shorthand */

  temp_index = qptr->id;
  QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_AMSS_CMD_SIZE);
  cmd_name = qmi_cflog_amss_cmd_name[temp_index];

  qmi_if_info_ptr = qmi_if_info_get_ptr();
  /* Fill label with text to be logged */
  switch (qptr->id)
  {
    case QMI_CMD_SEND_PKT_STATS:
    case QMI_CMD_NEW_NETWORK_SCAN_INFO:
    case QMI_CMD_CM_CLIENT_ACTIVATE:
    case QMI_CMD_SET_OPERATING_MODE_RESP:
    case QMI_CMD_DMS_CM_CALL_CMD_STATUS:
    case QMI_CMD_DMS_SEND_ACTIVATION_IND:
    case QMI_CMD_PDS_LBS_MSG:
      (void) snprintf (label, 80, "%s", cmd_name);
      break;
  
    case QMI_CMD_MMGSDI_OPERATION_COMPLETE:
      qmi_if_cmd_buf_ptr = (qmi_if_cmd_buf_type *)cmd_ptr;
      temp_index = (int) qmi_if_cmd_buf_ptr->data.mmgsdi.mmgsdi_cnf_info.cnf_type;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_MMGSDI_CNF_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name,
                             qmi_cflog_mmgsdi_cnf_name[temp_index]);
      break;

    case QMI_CMD_PROCESS_PIN_EVT:
      qmi_if_cmd_buf_ptr = (qmi_if_cmd_buf_type *)cmd_ptr;
      temp_index = (int) qmi_if_cmd_buf_ptr->data.mmgsdi.mmgsdi_evt_info.pin.status;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_MMGSDI_PIN_STATUS_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_mmgsdi_pin_status_name[temp_index]);
      break;
 
  case QMI_CMD_PROCESS_UIM_GET_STATE:
      qmi_dmsi_cmd_buf_ptr = (qmi_dmsi_cmd_buf_ptr *)cmd_ptr;
      temp_index = (int) qmi_dmsi_cmd_buf_ptr->data.mmgsdi_evt_info.event;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_MMGSDI_CARD_EVENT_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_mmgsdi_card_event_name[temp_index]);
      break;

    case QMI_CMD_GSDI_PERSO_FEATURE_EVT:
      temp_index = (int) qptr->data.mmgsdi.gsdi_perso.event;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_MMGSDI_PERSO_EVENT_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_mmgsdi_perso_event_name[temp_index]);
      break;

    case QMI_CMD_NEW_CM_SS_INFO:
      qmi_if_cmd_buf_ptr =  (qmi_if_cmd_buf_type *) cmd_ptr;
      temp_index = (int) qmi_if_cmd_buf_ptr->data.cm_ss.event;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_CM_SS_EVENT_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_cm_ss_event_name[temp_index]);
      break;

    case QMI_CMD_NEW_CM_PH_INFO:
      qmi_if_cmd_buf_ptr = (qmi_if_cmd_buf_type *)cmd_ptr;
      temp_index = (int) qmi_if_cmd_buf_ptr->data.cm_ph.event;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_CM_PH_EVENT_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_cm_ph_event_name[temp_index]);
      break;

    case QMI_CMD_PROCESS_FLOW_EVT:
    case QMI_CMD_PROCESS_QOS_PHYS_LINK_EVT:
    case QMI_CMD_PROCESS_QOS_IFACE_EVT:
      (void) snprintf (label, 80, "%s", cmd_name);
      break;

    case QMI_CMD_PROCESS_WDS_UM_IFACE_EVT:
      temp_index = (int) qptr->data.wds.iface_info.event;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_PS_EVENT_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_ps_event_name[temp_index]);
      break;

    case QMI_CMD_PROCESS_PHYS_LINK_EVT:
      temp_index = (int) qptr->data.wds.phys_link_status.event;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_PS_EVENT_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_ps_event_name[temp_index]);
      break;

    case QMI_CMD_PROCESS_DUN_CALL_INFO_EVT:
      temp_index = (int) qptr->data.wds.dun_call_info.conn_status;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_CONN_STATUS_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_conn_status_name[temp_index]);
      break;

    case QMI_CMD_NEW_CM_OPRT_MODE_INFO:
      qmi_if_cmd_buf_ptr = (qmi_if_cmd_buf_type *) cmd_ptr;
      temp_index = qmi_if_cmd_buf_ptr->data.cm_ph.event;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_CM_PH_EVENT_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_cm_ph_event_name[temp_index]);
      break;

    case QMI_CMD_DMS_OTASP_EVT:
      // temp_index = (int) qptr->data.dms.otasp_evt.otasp_state;
      // QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_OTASP_EVENT_SIZE);
      // (void) snprintf (label, 80, "%s %s", cmd_name, 
      //                       qmi_cflog_otasp_event_name[temp_index]);

      /* otasp state doesn't seem to exist? */
      (void) snprintf (label, 80, "%s", cmd_name);
      break;

    case QMI_CMD_EAP_SIM_AKA_TASK_SWITCH_EV:
    case QMI_CMD_SEND_EAP_RESP:
    case QMI_CMD_SEND_EAP_RESULT:
      (void) snprintf (label, 80, "%s", cmd_name);
      break;

    case QMI_CMD_WMS_CMD_STATUS:
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_find_name (qmi_cflog_wms_cmd_name, 
                                                  qptr->data.wms.cmd_status.cmd));
      break;

    case QMI_CMD_WMS_MSG_EVT:
      temp_index = (int) qptr->data.wms.msg_event.event;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_WMS_EVENT_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_wms_event_name[temp_index]);
      break;

    case QMI_CMD_WMS_CFG_EVT:
      temp_index = (int) qptr->data.wms.cfg_event.event;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_WMS_CFG_EVENT_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_wms_cfg_event_name[temp_index]);
      break;

    case QMI_CMD_WMS_DC_EVT:
      temp_index = (int) qptr->data.wms.dc_event.event;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_WMS_DC_EVENT_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qmi_cflog_wms_dc_event_name[temp_index]);
      break;

#if 0 /* FEATURE_DATA_QMI_AT */
    case QMI_CMD_AT_CLIENT_REG_STATUS:
      (void) snprintf (label, 80, "%s %s [client ID %d]", cmd_name, 
                             (qptr->data.at.client_reg.status)?"Success":"Failure",
                             qptr->data.at.client_reg.client_id);
      break;

    case QMI_CMD_AT_CMD_REG_STATUS:
      (void) snprintf (label, 80, "%s %s [%d]", cmd_name, 
                             (qptr->data.at.cmd_reg.status)?"Success":"Failure");
      break;

    case QMI_CMD_FWD_AT_CMD:
      /* Just guessing that fwd_cmd.name is null terminated, 
         otherwise there is a bug here */
      (void) snprintf (label, 80, "%s %s", cmd_name, 
                             qptr->data.at.fwd_cmd.name);
    break;

    case QMI_CMD_AT_ABORT_CMD:
      (void) snprintf (label, 80, "%s [handle %d]", cmd_name, 
                             qptr->data.at.cmd_abort.handle);
      break;
#endif

#if 0 /* FEATURE_DATA_QMI_CAT */
    case QMI_CMD_CAT_RAW_PROACTIVE_CMD :
      (void) snprintf (label, 80, "%s [cmd_ref_id %d]", cmd_name, 
                             qptr->data.cat.cat_raw_proactive_cmd.cmd_ref_id);
        break;

    case QMI_CMD_CAT_SETUP_EVT_LIST_CMD :
      (void) snprintf (label, 80, "%s [pc_setup_evt_list %d]", cmd_name, 
                             qptr->data.cat.cat_setup_evt_list_cmd.pc_setup_evt_list);
        break;

    case QMI_CMD_CAT_END_PROACTIVE_CMD :
      (void) snprintf (label, 80, "%s [end type %d]", cmd_name, 
                             qptr->data.cat.cat_end_proactive_cmd.proactive_session_end_type);
        break;

    case QMI_CMD_CAT_RAW_ENV_RSP:
      (void) snprintf (label, 80, "%s", cmd_name);
      break;

    case QMI_CMD_CAT_REFRESH_CMD :
      (void) snprintf (label, 80, "%s (mode %d stage %d]", cmd_name,
                             qptr->data.cat.cat_refresh_cmd.refresh_mode,
                             qptr->data.cat.cat_refresh_cmd.refresh_stage); 
      break;

    case QMI_CMD_CAT_REFRESH_REG_OP:
      temp_index = (int) qptr->data.cat.cat_refresh_reg_info.cnf_type;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_MMGSDI_CNF_SIZE);
      (void) snprintf (label, 80, "%s %s", cmd_name,
                             qmi_cflog_mmgsdi_cnf_name[temp_index]);
      break;
#endif

    case QMI_CMD_QMUX_PORT_OPEN:
    case QMI_CMD_QMUX_PORT_CLOSE:
    case QMI_CMD_QMUX_PORT_ASSIGN:
      (void) snprintf (label, 80, "%s [port %d]", cmd_name, 
                             qptr->data.qmux_port.sio_port);
      break;

    case QMI_CMD_PROCESS_POWER_EVT:
      (void) snprintf (label, 80, "%s", cmd_name);
      break;

    case QMI_CMD_CTL_SEND_SYNC_IND:
      (void) snprintf (label, 80, "%s", cmd_name);
      break;

    default:
      (void) snprintf (label, 80, "%s", cmd_name);
      break;
  }

  if (qptr->id == QMI_CMD_CM_CLIENT_ACTIVATE)
  {
    /* Internal QMI events */
    QMI_CFLOG_PKT_QMI_EVT( label );
  }
  else
  {
    QMI_CFLOG_PKT_AMSS_EVT( label );
  }

  /* Look deep into the CM events and log system state changes */
  /* phone operating mode change (online, LPM, etc.)
     change to acquired radio network
   */

  if (qptr->id == QMI_CMD_NEW_CM_SS_INFO)
  {
    qmi_if_cmd_buf_ptr =  (qmi_if_cmd_buf_type *) cmd_ptr;
    ss_info_ptr = &qmi_if_cmd_buf_ptr->data.cm_ss.info;
    sub_id = qmi_if_cmd_buf_ptr->data.cm_ss.asubs_id;

    if (ss_info_ptr->srv_status != qmi_if_info_ptr->ss_info[sub_id].srv_status )
    {
      temp_index = qmi_if_info_ptr->ss_info[sub_id].srv_status;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_CM_SRV_STATUS_SIZE);
      temp_index2 = ss_info_ptr->srv_status;
      QMI_CFLOG_SET_MAX (int, temp_index2, QMI_CFLOG_CM_SRV_STATUS_SIZE);

      (void) snprintf (label, 80, "Service State Change %s->%s on sub:%d",
                             qmi_cflog_cm_srv_status_name[temp_index],
                             qmi_cflog_cm_srv_status_name[temp_index2],
                             sub_id);
      QMI_CFLOG_PKT_QMI_ST_CHG( label );

      if ((ss_info_ptr->srv_status > SYS_SRV_STATUS_NO_SRV) && 
          (ss_info_ptr->srv_status < SYS_SRV_STATUS_PWR_SAVE))
      {
        temp_index = ss_info_ptr->sys_mode;
        QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_CM_SYS_MODE_SIZE);
        temp_index2 = ss_info_ptr->hdr_active_prot;
        QMI_CFLOG_SET_MAX (int, temp_index2, QMI_CFLOG_EVDO_REV_SIZE);

        (void) snprintf (label, 80, "Service on %s %s on sub:%d",
                               qmi_cflog_cm_sys_mode_name[temp_index],
                               (ss_info_ptr->sys_mode == SYS_SYS_MODE_HDR)?
                               evdo_rev[temp_index2]:"",sub_id);
        QMI_CFLOG_PKT_QMI_ST_CHG( label );
      }
    }

    if (ss_info_ptr->sys_mode != qmi_if_info_ptr->ss_info[sub_id].sys_mode )
    {
      temp_index = qmi_if_info_ptr->ss_info[sub_id].sys_mode;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_CM_SYS_MODE_SIZE);
      temp_index2 = ss_info_ptr->sys_mode;
      QMI_CFLOG_SET_MAX (int, temp_index2, QMI_CFLOG_CM_SYS_MODE_SIZE);

      (void) snprintf (label, 80, "System Change %s->%s %s on sub:%d",
                             qmi_cflog_cm_sys_mode_name[temp_index],
                             qmi_cflog_cm_sys_mode_name[temp_index2],
                             (ss_info_ptr->hdr_hybrid)?"Hybrid":"",sub_id);
      QMI_CFLOG_PKT_QMI_ST_CHG( label );
    }
  }
  else if (qptr->id == QMI_CMD_NEW_CM_PH_INFO)
  {
    qmi_if_cmd_buf_ptr =  (qmi_if_cmd_buf_type *) cmd_ptr;
    ph_info_ptr = &qmi_if_cmd_buf_ptr->data.cm_ph.info;
    if (ph_info_ptr->oprt_mode != qmi_if_info_ptr->ph_info.oprt_mode )
    {
      temp_index = qmi_if_info_ptr->ph_info.oprt_mode;
      QMI_CFLOG_SET_MAX (int, temp_index, QMI_CFLOG_CM_OPRT_MODE_SIZE);
      temp_index2 = ph_info_ptr->oprt_mode;
      QMI_CFLOG_SET_MAX (int, temp_index2, QMI_CFLOG_CM_OPRT_MODE_SIZE);

      (void) snprintf (label, 80, "Operating Mode Change %s->%s",
                             qmi_cflog_cm_oprt_mode_name[temp_index],
                             qmi_cflog_cm_oprt_mode_name[temp_index2]);
      QMI_CFLOG_PKT_QMI_ST_CHG( label );

      if (ph_info_ptr->oprt_mode == SYS_OPRT_MODE_ONLINE)
      {
        temp_index = ph_info_ptr->mode_pref;
        QMI_CFLOG_SET_MAX (int, temp_index2, QMI_CFLOG_CM_MODE_PREF_SIZE);

        (void) snprintf (label, 80, "Acquisition Mode %s",
                               qmi_cflog_cm_mode_pref_name[temp_index]);

        QMI_CFLOG_PKT_QMI_ST_CHG( label );
      }
    }
  }

#else
  return;
#endif
} /* ds_qmi_cflog_amss_rsp */

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_META_SM_STATE_CHANGE

  DESCRIPTION
    This function logs a state change banner comment.

  PARAMETERS
    old_state - The current state
    new_state - The state being changed to
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_meta_sm_state_change
( 
  uint32 old_state,
  uint32 new_state
)
{
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
  char label[80];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX2 (uint32, old_state, QMI_CFLOG_NUM_KNOWN_META_SM_STATES);
  QMI_CFLOG_SET_MAX2 (uint32, new_state, QMI_CFLOG_NUM_KNOWN_META_SM_STATES);

  (void) snprintf ( label, 80, "Data Call State %s -> %s",
                          qmi_cflog_meta_sm_state_name[old_state],
                          qmi_cflog_meta_sm_state_name[new_state] );
  QMI_CFLOG_PKT_QMI_ST_CHG( label );
#else
  return;
#endif
} /* ds_qmi_cflog_meta_sm_state_change */

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_V4_SM_STATE_CHANGE

  DESCRIPTION
    This function logs a state change banner comment.

  PARAMETERS
    old_state - The current state
    new_state - The state being changed to
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_v4_sm_state_change
( 
  uint32 old_state,
  uint32 new_state
)
{
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
  char label[80];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX2 (uint32, old_state, QMI_CFLOG_NUM_KNOWN_V4_SM_STATES);
  QMI_CFLOG_SET_MAX2 (uint32, new_state, QMI_CFLOG_NUM_KNOWN_V4_SM_STATES);

  (void) snprintf ( label, 80, "IPV4 State %s -> %s",
                          qmi_cflog_v4_sm_state_name[old_state],
                          qmi_cflog_v4_sm_state_name[new_state] );
  QMI_CFLOG_PKT_QMI_ST_CHG( label );
#else
  return;
#endif
} /* ds_qmi_cflog_v4_sm_state_change */

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_V6_SM_STATE_CHANGE

  DESCRIPTION
    This function logs a state change banner comment.

  PARAMETERS
    old_state - The current state
    new_state - The state being changed to
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_v6_sm_state_change
( 
  uint32 old_state,
  uint32 new_state
)
{
#ifdef FEATURE_QMI_CALL_FLOW_LOGGING
  char label[80];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX2 (uint32, old_state, QMI_CFLOG_NUM_KNOWN_V6_SM_STATES);
  QMI_CFLOG_SET_MAX2 (uint32, new_state, QMI_CFLOG_NUM_KNOWN_V6_SM_STATES);

  (void) snprintf ( label, 80, "IPV6 State %s -> %s",
                          qmi_cflog_v6_sm_state_name[old_state],
                          qmi_cflog_v6_sm_state_name[new_state] );
  QMI_CFLOG_PKT_QMI_ST_CHG( label );
#else
  return;
#endif
} /* ds_qmi_cflog_v6_sm_state_change */

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_CALL_STATE_CHANGE

  DESCRIPTION
    This function logs a state change banner comment for circuit switched
    (voice) calls.

  PARAMETERS
    label - QMI_VOICE already has the logic to build the label so no
            need to move it for now.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_call_state_change
( 
  char *label
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_PKT_QMI_ST_CHG( label );
} /* ds_qmi_cflog_call_state_change */


/*===========================================================================
  FUNCTION This comment is for the rest of the functions in this file.

  DESCRIPTION
    To enable QMI_CFLOG to have a minimum footprint in the main QMI files
    but still log specific details of important calls into AMSS, it is 
    necessary to have a separate function for each log message.  Including
    a standard banner comment for each function would significantly detract
    from the readability of this file so instead this one comment is used
    to apply to all remaining functions in the file.

  PARAMETERS
    Various parameters related to a call QMI is making to an AMSS function.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/

void ds_qmi_cflog_cm_sys_sel_pref
(
  cm_mode_pref_e_type              mode_pref_cur,
  cm_mode_pref_e_type              mode_pref_new,
  cm_band_pref_e_type              band_pref_new,
  cm_prl_pref_e_type               prl_pref_new,
  cm_roam_pref_e_type              roam_pref_new
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX (cm_mode_pref_e_type, mode_pref_cur, QMI_CFLOG_CM_MODE_PREF_SIZE);
  QMI_CFLOG_SET_MAX (cm_mode_pref_e_type, mode_pref_new, QMI_CFLOG_CM_MODE_PREF_SIZE);

  (void) snprintf (label, 120, "cm_ph_cmd_sys_sel_pref_with_lte(S %s->%s, Bands 0x%llx, PRL Pref 0x%x, Roam 0x%x )",
    qmi_cflog_cm_mode_pref_name[mode_pref_cur],
    qmi_cflog_cm_mode_pref_name[mode_pref_new],
    band_pref_new, prl_pref_new, roam_pref_new );
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_sys_sel_pref_net_reg
(
  uint8 register_action,
  cm_network_sel_mode_pref_e_type  network_sel_mode_pref_cur,
  cm_network_sel_mode_pref_e_type  network_sel_mode_pref_new,
  cm_mode_pref_e_type              mode_pref_new,
  sys_plmn_id_s_type *             plmn_ptr
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX (cm_network_sel_mode_pref_e_type, network_sel_mode_pref_cur, 
                     QMI_CFLOG_CM_NETWORK_SEL_MODE_PREF_SIZE);
  QMI_CFLOG_SET_MAX (cm_network_sel_mode_pref_e_type, network_sel_mode_pref_new, 
                     QMI_CFLOG_CM_NETWORK_SEL_MODE_PREF_SIZE);
  QMI_CFLOG_SET_MAX (cm_mode_pref_e_type, mode_pref_new, QMI_CFLOG_CM_MODE_PREF_SIZE);

  if (register_action == 1) /* Automatic */
  {
    (void) snprintf (label, 120, "cm_ph_cmd_sys_sel_pref_with_lte(A %s->%s, %s)",
      qmi_cflog_cm_network_sel_mode_pref_name[network_sel_mode_pref_cur],
      qmi_cflog_cm_network_sel_mode_pref_name[network_sel_mode_pref_new],
      qmi_cflog_cm_mode_pref_name[mode_pref_new]);
  }
  else if (register_action == 2) /* Manual */
  {
    (void) snprintf (label, 120, "cm_ph_cmd_sys_sel_pref_with_lte(M %s->%s, %s, plmn[0] %u, plmn[1] %u, plmn[2] %u)",
      qmi_cflog_cm_network_sel_mode_pref_name[network_sel_mode_pref_cur],
      qmi_cflog_cm_network_sel_mode_pref_name[network_sel_mode_pref_new],
      qmi_cflog_cm_mode_pref_name[mode_pref_new],
      (plmn_ptr != NULL)?plmn_ptr->identity[0]:0,
      (plmn_ptr != NULL)?plmn_ptr->identity[1]:0,
      (plmn_ptr != NULL)?plmn_ptr->identity[2]:0 );
  }
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_sys_sel_pref_net_attach
(
  cm_srv_domain_pref_e_type srv_domain_pref_new
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX (cm_srv_domain_pref_e_type, srv_domain_pref_new, 
                     QMI_CFLOG_CM_SRV_DOMAIN_PREF_SIZE);

  (void) snprintf (label, 120, "cm_ph_cmd_sys_sel_pref_with_lte(N %s)",
    qmi_cflog_cm_srv_domain_pref_name[srv_domain_pref_new]);
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_sys_sel_pref_tech_pref
(
  cm_mode_pref_e_type              mode_pref_cur,
  cm_mode_pref_e_type              mode_pref_new,
  cm_pref_term_e_type              pref_term_new,
  cm_hybr_pref_e_type              hybr_pref_new
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX (cm_mode_pref_e_type, mode_pref_cur, QMI_CFLOG_CM_MODE_PREF_SIZE);
  QMI_CFLOG_SET_MAX (cm_mode_pref_e_type, mode_pref_new, QMI_CFLOG_CM_MODE_PREF_SIZE);
  QMI_CFLOG_SET_MAX (cm_pref_term_e_type, pref_term_new, QMI_CFLOG_CM_PREF_TERM_SIZE);
  QMI_CFLOG_SET_MAX (cm_hybr_pref_e_type, hybr_pref_new, QMI_CFLOG_CM_HBYR_PREF_SIZE);
      
  (void) snprintf (label, 120, "cm_ph_cmd_sys_sel_pref_with_lte(T %s->%s pref_term %s hybr_pref %s)",
    qmi_cflog_cm_mode_pref_name[mode_pref_cur],
    qmi_cflog_cm_mode_pref_name[mode_pref_new],
    qmi_cflog_cm_pref_term_name[pref_term_new],
    qmi_cflog_cm_hybr_pref_name[hybr_pref_new]);
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_call_cmd_orig_otasp
(
  cm_num_s_type            *num_ptr
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "cm_mm_call_cmd_orig(OTASP, %s)",
                         (num_ptr != NULL)? (char *) num_ptr->buf: "no number");
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_call_cmd_orig
(
  cm_srv_type_e_type       srv_type,
  cm_call_type_e_type      call_type,
  cm_num_s_type            *num_ptr
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX (cm_srv_type_e_type, srv_type, QMI_CFLOG_CM_SRV_TYPE_SIZE);
  QMI_CFLOG_SET_MAX (cm_call_type_e_type, call_type, QMI_CFLOG_CM_CALL_TYPE_SIZE);

  (void) snprintf (label, 120, "cm_mm_call_cmd_orig(%s, %s, %s)",
    (num_ptr != NULL)? (char *) num_ptr->buf: "no number",
    qmi_cflog_cm_call_type_name[call_type],
    qmi_cflog_cm_srv_type_name[srv_type]);
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_call_cmd_answer
(
  uint8                           call_id,
  cm_call_mode_info_e_type        info_type,
  cm_call_type_e_type             call_type
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX2 (cm_call_mode_info_e_type, info_type, 
                     QMI_CFLOG_CM_CALL_MODE_INFO_SIZE);
  QMI_CFLOG_SET_MAX (cm_call_type_e_type, call_type, 
                     QMI_CFLOG_CM_CALL_TYPE_SIZE);

  (void) snprintf (label, 120, "cm_mm_call_cmd_answer(%d %s, %s)",
    call_id,
    qmi_cflog_cm_call_type_name[call_type],
    qmi_cflog_cm_call_mode_info_name[info_type]);
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_call_cmd_end
(
  uint8                           call_id,
  cm_call_mode_info_e_type        info_type,
  char                            *end_type
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX2 (cm_call_mode_info_e_type, info_type, 
                     QMI_CFLOG_CM_CALL_MODE_INFO_SIZE);

  (void) snprintf (label, 120, "cm_mm_call_cmd_end(%d %s) %s",
    call_id,
    qmi_cflog_cm_call_mode_info_name[info_type],
    end_type);
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_call_cmd_privacy_pref
(
  uint8                           call_id,
  char                            *privacy_mode_name
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "cm_mm_call_cmd_privacy_pref(%d) %s",
    call_id, privacy_mode_name);
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_call_cmd_sups
(
  uint8                           call_id,
  char                            *sups_type
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "cm_mm_call_cmd_sups(%d) %s",
    call_id, sups_type);
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_call_cmd_sups_flash
(
  uint8                           call_id,
  cm_num_s_type                   *num_ptr
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "cm_mm_call_cmd_sups(%d) Flash %s",
    call_id, (num_ptr != NULL)? (char *) num_ptr->buf: "no number");
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_call_cmd_setup_res
(
  uint8                           call_id
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "cm_mm_call_cmd_setup_res(%d)", call_id);
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_inband_cmd_start_cont_dtmf
(
  uint8               call_id,
  uint8               digit
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "cm_mm_inband_cmd_start_cont_dtmf(%d, %d)", 
                         call_id, digit );
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_inband_cmd_stop_cont_dtmf
(
  uint8               call_id
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "cm_mm_inband_cmd_stop_cont_dtmf(%d)", 
                         call_id );
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_inband_cmd_burst_dtmf
(
  uint8               call_id,
  char                *digit_buffer,
  uint8               on_length,
  uint8               off_length

)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "cm_mm_inband_cmd_burst_dtmf(%d, %s, on %d off %d)", 
                         call_id, digit_buffer, on_length, off_length );
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_ph_cmd_oprt_mode
(
  sys_oprt_mode_e_type   new_oprt_mode
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX (sys_oprt_mode_e_type, new_oprt_mode, QMI_CFLOG_CM_OPRT_MODE_SIZE);

  (void) snprintf (label, 120, "cm_ph_cmd_oprt_mode(%s)", 
                         qmi_cflog_cm_oprt_mode_name[new_oprt_mode] );
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_sups_cmd
(
  char *name,
  int  ss_code,
  int  ss_ref,
  char *ss_buf
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "cm_sups_cmd_%s(%d, %s) %s", name, ss_ref,
                         qmi_cflog_find_name( qmi_cflog_sups_name, ss_code ),
                         ss_buf );
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_cm_sups_cmd_cf
(
  char *name,
  int  ss_code,
  int  ss_ref,
  char *ss_buf,
  char *cf_reason
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "cm_sups_cmd_%s(%d, %s, %s) %s", name, ss_ref,
                         qmi_cflog_find_name( qmi_cflog_sups_name, ss_code ),
                         cf_reason, ss_buf );
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_wms_bc_mm_set_pref
(
  uint8  mode,
  uint8  bc_pref
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "wms_bc_mm_set_pref(%s %s)",
                         (bc_pref == 0)?"Disable":"Enable",
                         (mode == 0)?"CDMA":"GW" );
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_wms_cfg_set_memory_full
( 
  uint8 memory_available 
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) snprintf (label, 120, "wms_cfg_set_memory_full(%s)",
                         (memory_available == 0)?"Full":"Memory Available");
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_wms_msg_delete
(
  wms_memory_store_e_type storage_type, 
  uint32 storage_index
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX (wms_memory_store_e_type, storage_type, 
                     QMI_CFLOG_WMS_MEMORY_STORE_SIZE);

  (void) snprintf (label, 120, "wms_msg_delete(%s, %d)",
                         qmi_cflog_wms_memory_store_name[storage_type],
                         (int)storage_index );
                         
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}

void ds_qmi_cflog_wms_msg_read
(
  wms_memory_store_e_type storage_type, 
  uint32 storage_index
)
{
  char label[120];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CFLOG_SET_MAX (wms_memory_store_e_type, storage_type, 
                     QMI_CFLOG_WMS_MEMORY_STORE_SIZE);

  (void) snprintf (label, 120, "wms_msg_read(%s, %d)",
                         qmi_cflog_wms_memory_store_name[storage_type],
                         (int)storage_index );
                         
      
  QMI_CFLOG_PKT_AMSS_CALL( label );
}


