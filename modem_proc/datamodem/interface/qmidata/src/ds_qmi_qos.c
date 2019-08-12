/*===========================================================================

                         D S _ Q M I _ Q O S . C

DESCRIPTION

 The Qualcomm Quality Of Service MSM Interface source file.

EXTERNALIZED FUNCTIONS

  qmi_qos_init()
    Register the QoS Service with QMUX for all applicable QMI links.

Copyright (c) 2004-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_qos.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/16/14    sk     Fixed instance validation issue in qmi_qosi_reset_client.
01/16/14    sah    Fix for QOS indication in case of NW initiated flow events.
11/07/13    sj     Use new QMUX framework and support BIND_DATA_PORT
10/25/13    wc     Fix incorrect update of nw-inited flow cache
09/27/13    wc     Support dynamic port configuration
04/29/13    svj    Fix for querying network initiated Flows
08/28/13    bh     Handling condition to not send next header prot if
                   port based filter is applied.
09/10/12    sb     Len checks for QMI TLVs
04/04/12    wc     Added IP family TLV in broadcast indications
10/27/11    sy     Added QoS flow type TLV
10/04/11    sa     Q6 free floating changes for QMI.
08/01/11    wc     Fix qos flow clean in qosi_dereg_ps_flow_event
05/03/11    sa     Fix for QMI Request QoS TLV parsing error.
12/17/10    kk     Removing DCC command buffer dependency from QMI services.
10/07/10    kk     Added support for FLOW_MODIFY events on default flow.
07/09/10    asn    Support for Arbitration
06/19/10    vs     Changes for Dual IP support over single QMI instance
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
04/13/09    rt     Posting a command to DCC task to process QOS events.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
05/01/08    mct    Added IPv6 QOS support.
08/29/07    ks     Removed limit of 10 QoS Reqs per client. Now have a global
                   pool for all clients.
08/15/07    ks     Fixed edge case of releasing/suspending/resuming MAX flows
                   in a bundle.
08/08/07    ks     Fixed mapping of ps info codes to qmi info codes.
07/30/07    ks     Send Activate ind before flow_enabled ind, so that filter
                   spec is sent for new flows.
07/30/07    ks     keep state of registration for flow events.
06/05/07    ks     Register cbacks with wds and qmi_task modules.
05/01/07    ks     Updates to use common svc routines
04/30/07    ks     Changes for flow separation
04/17/07    ks     Converting IP address and Port no to Network order.
12/12/06    ks     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stdio.h>
#include <string.h>

#include "amssassert.h"
#include "dsm.h"

#include "timer.h"
#include "ds_qmap.h"
#include "ps_byte.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "ps_flow_event.h"
#include "ps_ipfltr_defs.h"
#include "ps_flowi.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_rmnet_xport.h"

#include "qmi_framework.h"
#include "ds_qmux.h"
#include "ds_qmux_ext.h"
#include "ds_qmi_if.h"
#include "ds_qmi_fw_common.h"
#include "ds_qmi_qos.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_wds.h"
#include "ds_qmi_wds_profile.h"
#include "ds_qmi_task.h"
#include "dcc_task_defs.h"
#include "dcc_task_svc.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_sys.h"
#include "ps_sys_conf.h"
#include "ps_sys_event.h"
#include "ps_sys_ioctl.h"

#include "ps_system_heap.h"
#include "ps_handle_mgr.h"

#include <stringl/stringl.h>
#include "ps_utils.h"
#include "ps_ifacei_event.h"
#include "ps_flowi_event.h"
#include "ps_phys_linki_event.h"
#include "quality_of_service_v01.h"
/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Service configuration
  - maximum number of clients per QOS instance
---------------------------------------------------------------------------*/

#ifndef MAX_UINT16
#define MAX_UINT16 65535
#endif

#define QOSI_MAX_BIT_RATE_PRE_REL_11 256000000

#define QOSI_BASE_VER_MAJOR  (1)
#define QOSI_BASE_VER_MINOR  (12)

#define QOSI_ADDENDUM_VER_MAJOR  (0)
#define QOSI_ADDENDUM_VER_MINOR  (0)

#define QOSI_MAX_QOS_REQUESTS (30)
#define QOSI_MAX_QOS_SPECS_PER_REQUEST (10)
#define QOSI_MAX_NETWORK_INIT_FLOW_NUM (30)
#define QOSI_MAX_QOS_FLTRS_PER_REQUEST (MAX_FLTR_PER_REQ) // 8
#define QOSI_MAX_QOS_FLOWS_PER_REQUEST (MAX_ADDITIONAL_FLOWS_PER_REQ + 2) //6+2

/*---------------------------------------------------------------------------
  Message-internal TLV type field values
---------------------------------------------------------------------------*/
#define QMI_PRM_TYPE_FAILED_TLV_LIST (0x11)

#define QMI_PRM_TYPE_QOS_IDENTIFIER (0x01)

#define QMI_PRM_TYPE_QOS_SPEC (0x10)
#define QMI_PRM_TYPE_QOS_REQ_OPCODE  (0x11)
#define QMI_PRM_TYPE_TX_QOS_FLOW_REQ (0x11)
#define QMI_PRM_TYPE_RX_QOS_FLOW_REQ (0x12)
#define QMI_PRM_TYPE_TX_QOS_FILTER_REQ (0x13)
#define QMI_PRM_TYPE_RX_QOS_FILTER_REQ (0x14)
#define QMI_PRM_TYPE_QOS_FLOW_TYPE     (0x15)
#define QMI_PRM_TYPE_QOS_BEARER_ID     (0x16)
#define QMI_PRM_TYPE_UPLINK_FC_SEQ_NUM (0x17)
#define QMI_PRM_TYPE_QOS_FLOW_SPEC (0x10)
#define QMI_PRM_TYPE_QOS_FILTER_SPEC (0x10)
#define QMI_PRM_TYPE_QOS_SPEC_INDEX (0x10)
#define QMI_PRM_TYPE_PRIM_MODIFY_QOS_REQ  (0x11)

#define QMI_PRM_TYPE_TX_QOS_FILTER_PARAMS (0x10)
#define QMI_PRM_TYPE_RX_QOS_FILTER_PARAMS (0x11)

#define QMI_PRM_TYPE_TX_QOS_GRANTED_FLOW (0x11)
#define QMI_PRM_TYPE_RX_QOS_GRANTED_FLOW (0x12)

#define QMI_PRM_TYPE_QOS_SPEC_ERR (0x10)
#define QMI_PRM_TYPE_TX_QOS_FLOW_REQ_ERR (0x11)
#define QMI_PRM_TYPE_RX_QOS_FLOW_REQ_ERR (0x12)
#define QMI_PRM_TYPE_TX_QOS_FILTER_REQ_ERR (0x13)
#define QMI_PRM_TYPE_RX_QOS_FILTER_REQ_ERR (0x14)
#define QMI_PRM_TYPE_QOS_FLOW_SPEC_ERR (0x10)
#define QMI_PRM_TYPE_QOS_FILTER_SPEC_ERR (0x10)

#define QMI_PRM_TYPE_GLOBAL_FLOW_INDICATOR (0x10)
#define QMI_PRM_TYPE_NW_SUPP_QOS_PROFILE_CH_INDICATOR (0x11)
#define QMI_PRM_TYPE_FLOW_INDEX (0x10)
#define QMI_PRM_TYPE_FLOW_TRF_CLASS (0x11)
#define QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX (0x12)
#define QMI_PRM_TYPE_FLOW_DATA_R_TOKEN_BUK (0x13)
#define QMI_PRM_TYPE_FLOW_LATENCY (0x14)
#define QMI_PRM_TYPE_FLOW_JITTER (0x15)
#define QMI_PRM_TYPE_FLOW_PKT_ERR_RATE (0x16)
#define QMI_PRM_TYPE_FLOW_MIN_POLICED_PKT_SIZE (0x17)
#define QMI_PRM_TYPE_FLOW_MAX_ALLOWED_PKT_SIZE (0x18)
#define QMI_PRM_TYPE_FLOW_RES_BER (0x19)
#define QMI_PRM_TYPE_FLOW_TRF_PRI (0x1A)
#define QMI_PRM_TYPE_FLOW_PROFILE_ID (0x1B)
#define QMI_PRM_TYPE_FLOW_PRIORITY (0x1C)
#define QMI_PRM_TYPE_FLOW_IM_CN_FLAG (0x1D)
#define QMI_PRM_TYPE_FLOW_SIG_IND (0x1E)
#define QMI_PRM_TYPE_FLOW_LTE_QCI (0x1F)
#define QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_EX (0x20)

#define QMI_PRM_TYPE_FILTER_INDEX (0x10)
#define QMI_PRM_TYPE_IP_VSN (0x11)
#define QMI_PRM_TYPE_IPV4_SRC_ADDR (0x12)
#define QMI_PRM_TYPE_IPV4_DST_ADDR (0x13)
#define QMI_PRM_TYPE_IP_NEXT_HDR_PROT (0x14)
#define QMI_PRM_TYPE_IPV4_TOS (0x15)
#define QMI_PRM_TYPE_IPV6_SRC_ADDR (0x16)
#define QMI_PRM_TYPE_IPV6_DST_ADDR (0x17)
#define QMI_PRM_TYPE_IPV6_TRAFFIC_CLASS (0x19)
#define QMI_PRM_TYPE_IPV6_FLOW_LABEL (0x1A)
#define QMI_PRM_TYPE_TCP_SRC_PORT (0x1B)
#define QMI_PRM_TYPE_TCP_DST_PORT (0x1C)
#define QMI_PRM_TYPE_UDP_SRC_PORT (0x1D)
#define QMI_PRM_TYPE_UDP_DST_PORT (0x1E)
#define QMI_PRM_TYPE_ICMP_MSG_TYPE (0x1F)
#define QMI_PRM_TYPE_ICMP_MSG_CODE (0x20)
#define QMI_PRM_TYPE_ESP_SPI (0x21)
#define QMI_PRM_TYPE_FILTER_PRECEDENCE (0x22)
#define QMI_PRM_TYPE_FILTER_ID (0x23)
#define QMI_PRM_TYPE_TCP_UDP_SRC_PORT (0x24)
#define QMI_PRM_TYPE_TCP_UDP_DST_PORT (0x25)

#define QMI_PRM_TYPE_REASON_CODE  (0x10)
#define QMI_PRM_TYPE_QOS_IP_FAMILY (0x11)

#define QMI_PRM_TYPE_PRI_QOS_IP_FAMILY (0x10)

/* set event report */
#define QOSI_PRM_TYPE_REPORT_FLOW_CHANGE (0x10)
#define QOSI_PRM_TYPE_GLOBAL_FLOW_CHANGE_REPORT  (0x10)
#define QOSI_PRM_TYPE_REPORT_NW_SUPP_QOS_PROFILE (0x11)

/* Mode specific flow operations  -- input */
#define QMI_PRM_TYPE_REQUESTED_OPERATION   (0x10)
#define QMI_PRM_TYPE_SET_INACTIVITY_TIMER  (0x11)
#define QMI_PRM_TYPE_PRIMARY_FLOW_OPER_REQ (0x12)
/* Mode specific flow operations  -- output */
#define QMI_PRM_TYPE_TX_QUEUE_LEVEL       (0x11)
#define QMI_PRM_TYPE_RMAC3_INFO           (0x12)
#define QMI_PRM_TYPE_TX_STATUS            (0x13)
#define QMI_PRM_TYPE_INACTIVITY_TIMER     (0x14)

#define QMI_PRM_TYPE_PRIM_GRANTED_QOS_REQ (0x10)

#define QMI_PRM_TYPE_EXT_TECH_PREF        (0x10)

#define QOSI_PRM_TYPE_IFACE_NAME          (0x10)

/* Set mux data port TLVs */
#define QOSI_PRM_TYPE_EP_ID                      (0x10)
#define QOSI_PRM_TYPE_MUX_ID                     (0x11)

#define QMI_IPV6_FLTR_ADDR_SIZE (0x11) // 17=16+1
#define QMI_IPV6_FLTR_TRF_CLS_SIZE (0x02) //1+1

typedef enum
{
  QOSI_REPORT_STATUS_FLOW_ACTIVATED = 0x01,
  QOSI_REPORT_STATUS_FLOW_MODIFIED  = 0x02,
  QOSI_REPORT_STATUS_FLOW_DELETED   = 0x03,
  QOSI_REPORT_STATUS_FLOW_SUSPENDED = 0x04,
  QOSI_REPORT_STATUS_FLOW_ENABLED   = 0x05,
  QOSI_REPORT_STATUS_FLOW_DISABLED  = 0x06
} qosi_event_report_flow_type;

/* network status */
#define QOSI_AWARE_SYSTEM   (0x01)
#define QOSI_UNAWARE_SYSTEM (0x00)
#define QMI_FLOW_ENABLED (0x01)
#define QMI_FLOW_DISABLED (0x00)

/*---------------------------------------------------------------------------
  Message-internal TLV length field values
---------------------------------------------------------------------------*/
#define QMI_PRM_TYPE_GLOBAL_FLOW_INDICATOR_LEN                       (0x1)
#define QMI_PRM_TYPE_NW_SUPP_QOS_PROFILE_CH_INDICATOR_LEN            (0x3)
#define QMI_PRM_TYPE_FLOW_TRF_CLASS_LEN                              (0x1)
#define QMI_PRM_TYPE_QOS_REQ_OPCODE_LEN                              (0x1)
#define QOSI_PRM_TYPE_IFACE_NAME_LEN                                 (0x2)
#define QMI_PRM_TYPE_EXT_TECH_PREF_LEN                               (0x2)
#define QMI_PRM_TYPE_TX_QUEUE_LEVEL_LEN                              (0xC)
#define QMI_PRM_TYPE_TX_STATUS_LEN                                   (0x1)
#define QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_LEN                         (0x8)
#define QMI_PRM_TYPE_FLOW_DATA_R_TOKEN_BUK_LEN                       (0xC)
#define QMI_PRM_TYPE_FLOW_PKT_ERR_RATE_LEN                           (0x4)
#define QMI_PRM_TYPE_FLOW_RES_BER_LEN                                (0x1)
#define QMI_PRM_TYPE_FLOW_TRF_PRI_LEN                                (0x1)
#define QMI_PRM_TYPE_FLOW_LTE_QCI_LEN                                (0x1)
#define QMI_PRM_TYPE_IPV4_SRC_ADDR_LEN                               (0x8)
#define QMI_PRM_TYPE_IPV4_DST_ADDR_LEN                               (0x8)
#define QMI_PRM_TYPE_IPV4_TOS_LEN                                    (0x2)
#define QMI_PRM_TYPE_TCP_SRC_PORT_LEN                                (0x4)
#define QMI_PRM_TYPE_TCP_DST_PORT_LEN                                (0x4)
#define QMI_PRM_TYPE_UDP_SRC_PORT_LEN                                (0x4)
#define QMI_PRM_TYPE_UDP_DST_PORT_LEN                                (0x4)
#define QMI_PRM_TYPE_TCP_UDP_SRC_PORT_LEN                            (0x4)
#define QMI_PRM_TYPE_TCP_UDP_DST_PORT_LEN                            (0x4)
#define QMI_PRM_TYPE_IP_VSN_LEN                                      (0x1)
#define QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_EX_LEN                      (0x10)

/* Bind subs TLVs */ 
#define WDSI_PRM_TYPE_SUBS_ID_REQ                                    (0x01)
#define WDSI_PRM_TYPE_SUBS_ID_RESP                                   (0x10)
/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

/*---------------------------------------------------------------------------
  MACRO to check temp return var during message processing
---------------------------------------------------------------------------*/
#define CHECK_TEMP_FOR_FAILURE()  if (temp == -1)               \
                                  { dsm_free_packet( &sdu_in ); \
                                    return; }

#define HANDLE_FAILURE()  dsm_free_packet(&response); \
                          errval = QMI_ERR_NO_MEMORY; \
                          goto send_result

#define QMI_QOSI_DECODE_REQUEST_MSG( message_id, sdu_in, req_msg, req_size)\
        qmi_qosi_decode_msg( message_id, sdu_in, req_msg, req_size);

#define QMI_QOSI_ENCODE_RESPONSE_MSG( message_id, resp_msg, size, err_code, response) \
         qmi_qosi_encode_msg( message_id, resp_msg, size, err_code, response)

qmi_idl_service_object_type 	qos_svc_obj;


/*===========================================================================

                                DATA TYPES

===========================================================================*/


/*---------------------------------------------------------------------------
  QOS Command enum type - not equal to the actual command values!
  mapping is in qmi_qos_cmd_callbacks table

  DO NOT REORDER THIS ENUM!  (make sure the command value here = index into
  qmi_qosi_cmd_callbacks table for corresponding command)
---------------------------------------------------------------------------*/
typedef enum
{
  QOSI_CMD_MIN                           = 0,
  QOSI_CMD_RESET                         = QOSI_CMD_MIN,
  QOSI_CMD_SET_EVENT_REPORT              ,
  QOSI_CMD_REQUEST_QOS                   ,
  QOSI_CMD_RELEASE_QOS                   ,
  QOSI_CMD_SUSPEND_QOS                   ,
  QOSI_CMD_RESUME_QOS                    ,
  QOSI_CMD_MODIFY_QOS                    ,
  QOSI_CMD_GET_GRANTED_QOS               ,
  QOSI_CMD_GET_QOS_STATUS                ,
  QOSI_CMD_GET_QOS_NETWORK_STATUS        ,
  QOSI_CMD_GET_NW_SUPPORTED_QOS_PROFILES ,
  QOSI_CMD_SET_CLIENT_IP_PREF            ,
  QOSI_CMD_BIND_DATA_PORT                ,
  QOSI_CMD_GET_FILTER_PARAMS             ,
  QOSI_CMD_BIND_SUBSCRIPTION             ,
  QOSI_CMD_GET_BIND_SUBSCRIPTION         ,
  QOSI_CMD_INDICATION_REGISTER           ,
  QOSI_CMD_REQUEST_QOS_EX                ,
  QOSI_CMD_MODIFY_QOS_EX                 ,
  QOSI_CMD_GET_QOS_INFO                  ,
/* ADD EXTERNAL QMI QOS COMMANDS ABOVE and also equate QOSI_CMD_EXTERNAL_MAX
   to the last service value added above */
  QOSI_CMD_EXTERNAL_MAX                 = QOSI_CMD_GET_QOS_INFO,

  QOSI_CMD_VS_MIN                       = QOSI_CMD_EXTERNAL_MAX,
/* ADD VENDOR-SPECIFIC QMI COMMANDS ABOVE and also equate QOSI_CMD_VS_MAX
   to the last VS service value added above */
  QOSI_CMD_VS_MAX  = QOSI_CMD_VS_MIN,

  QOSI_CMD_PERFORM_FLOW_OPERATION ,
/* ADD INTERNAL QMI QOS COMMANDS HERE */
  QOSI_CMD_MAX,
  QOSI_CMD_WIDTH                    = 0xFFFF
} qmi_qosi_cmd_e_type;

typedef enum
{
  QOSI_CMD_VAL_RESET                         = 0x0000,
  QOSI_CMD_VAL_SET_EVENT_REPORT              = 0x0001,
  QOSI_CMD_VAL_REQUEST_QOS                   = 0x0020,
  QOSI_CMD_VAL_RELEASE_QOS                   = 0x0021,
  QOSI_CMD_VAL_SUSPEND_QOS                   = 0x0022,
  QOSI_CMD_VAL_RESUME_QOS                    = 0x0023,
  QOSI_CMD_VAL_MODIFY_QOS                    = 0x0024,
  QOSI_CMD_VAL_GET_GRANTED_QOS               = 0x0025,
  QOSI_CMD_VAL_GET_QOS_STATUS                = 0x0026,
  QOSI_CMD_VAL_GET_QOS_NETWORK_STATUS        = 0x0027,
  QOSI_CMD_VAL_GET_NW_SUPPORTED_QOS_PROFILES = 0x0028,
  QOSI_CMD_VAL_PRIMARY_QOS_EVENT             = 0x0029,
  QOSI_CMD_VAL_SET_CLIENT_IP_PREF            = 0x002A,
  QOSI_CMD_VAL_BIND_DATA_PORT                = 0x002B,
  QOSI_CMD_VAL_GET_FILTER_PARAMS             = 0x002C,
  QOSI_CMD_VAL_BIND_SUBSCRIPTION             = 0x002D,
  QOSI_CMD_VAL_GET_BIND_SUBSCRIPTION         = 0x002E,
  QOSI_CMD_VAL_INDICATION_REGISTER           = 0x002F,
  QOSI_CMD_VAL_REQUEST_QOS_EX                = 0x0030,
  QOSI_CMD_VAL_GLOBAL_QOS_FLOW_IND           = 0x0031,
  QOSI_CMD_VAL_MODIFY_QOS_EX                 = 0x0032,
  QOSI_CMD_VAL_GET_QOS_INFO                  = 0x0033,
   
/* QC EXTERNAL QMI COMMAND RANGE IS 0x0000 - 0x5555.
   Add the next external QMI Command here */

/* VENDOR SPECIFIC QMI COMMAND RANGE IS 0x5556 - 0xAAAA.
   IMPORTANT!
   Add the vendor specific QMI Commands within this range only to avoid
   conflicts with QC QMI commands that would get released in future */

/* RESERVED QC QMI COMMAND RANGE IS 0xAAAB - 0xFFFE
   Internal QMI Commands must be added in DECREASING ORDER from Below */
  QOSI_CMD_VAL_PERFORM_FLOW_OPERATION        = 0xFFFE,
  QOSI_CMD_VAL_WIDTH                         = 0xFFFF
} qmi_qosi_cmd_val_e_type;

typedef enum
{
  QOSI_STATUS_DEFAULT    = 0x00,
  QOSI_STATUS_ACTIVATED  = 0x01,
  QOSI_STATUS_SUSPENDED  = 0x02,
  QOSI_STATUS_GONE       = 0x03,
  QOSI_STATUS_MAX
} qmi_qosi_status_e_type;

typedef enum
{
  QOSI_MIN_EV               = 0x00,
  QOSI_ACTIVATED_EV         = 0x01,
  QOSI_SUSPENDED_EV         = 0x02,
  QOSI_GONE_EV              = 0x03,
  QOSI_MODIFY_ACCEPTED_EV   = 0x04,
  QOSI_MODIFY_REJECTED_EV   = 0x05,
  QOSI_INFO_CODE_UPDATED_EV = 0x06,
  QOSI_EVENT_MAX
} qmi_qosi_event_e_type;

typedef enum
{
  QOSI_PRIMARY_EV_MIN               = 0x0000,
  QOSI_PRIMARY_MODIFY_SUCCESS_EV    = 0x0001,
  QOSI_PRIMARY_MODIFY_FAILURE_EV    = 0x0002,
  QOSI_PRIMARY_EV_MAX,
  QOSI_PRIMARY_EV_WIDTH             = 0xFFFF
} qmi_qosi_primary_qos_event_e_type;

typedef enum
{
  QOSI_INVALID_PARAMS                  = 0x01,
  QOSI_INTERNAL_CALL_ENDED             = 0x02,
  QOSI_INTERNAL_ERROR                  = 0x03,
  QOSI_INSUFFICIENT_LOCAL_RESOURCES    = 0x04,
  QOSI_TIMED_OUT_OPERATION             = 0x05,
  QOSI_INTERNAL_UNKNOWN_CAUSE_CODE     = 0x06,
  QOSI_INTERNAL_MODIFY_IN_PROGRESS     = 0x07,
  QOSI_NOT_SUPPORTED                   = 0x08,
  QOSI_NOT_AVAILABLE                   = 0x09,
  QOSI_NOT_GUARANTEED                  = 0x0A,
  QOSI_INSUFFICIENT_NETWORK_RESOURCES  = 0x0B,
  QOSI_QOS_AWARE_SYSTEM                = 0x0C,
  QOSI_QOS_UNAWARE_SYSTEM              = 0x0D,
  QOSI_REJECTED_OPERATION              = 0x0E,
  QOSI_WILL_GRANT_WHEN_QOS_RESUMED     = 0x0F,
  QOSI_NETWORK_CALL_ENDED              = 0x10,
  QOSI_NETWORK_SERVICE_NOT_AVAILABLE   = 0x11,
  QOSI_NETWORK_L2_LINK_RELEASED        = 0x12,
  QOSI_NETWORK_L2_LINK_REESTAB_REJ     = 0x13,
  QOSI_NETWORK_L2_LINK_REESTAB_IND     = 0x14,
  QOSI_NETWORK_UNKNOWN_CAUSE_CODE      = 0x15,
  QOSI_NETWORK_BUSY		       = 0x16,
  QOSI_NETWORK_DISJOINT_PROFILE_SET_SUGGESTED
                                       = 0X17,
  QOSI_NETWORK_NULL_PROFILE_SUGGESTED  = 0x18,
  QOSI_NETWORK_UE_NOT_AUTHORIZED       = 0x19,
  QOSI_INFO_CODE_MAX
} qmi_qosi_info_code_e_type;

/*---------------------------------------------------------------------------
  QMI QoS Service instance state definition &
  QoS Service client state definition
---------------------------------------------------------------------------*/

typedef enum
{
  QOSI_IPV4_STATE  = 0x00,
  QOSI_IPV6_STATE  = 0x01,
  QOSI_MAX_STATE
} qmi_qosi_state_e_type;


typedef enum 
{
  QMI_QOSI_SUBS_DEFAULT     = 0x0000,  /* Default Data Subscription */
  QMI_QOSI_SUBS_PRIMARY     = 0x0001,  /* Primary */
  QMI_QOSI_SUBS_SECONDARY   = 0x0002,  /* Secondary */
  QMI_QOSI_SUBS_TERTIARY    = 0x0003,  /* Tertiary */
  QMI_QOSI_SUBS_MAX,                   /* Max supp subs */
  QMI_QOSI_SUBS_DONT_CARE   = 0x00FF   /* Don't care subs */
} qmi_qosi_subs_enum_type;

typedef struct qmi_qosi_client_state_s
{
  qmi_common_client_state_type   common; // must be first since we alias
  struct qmi_qosi_client_state_s * next;    // Link to next client
  qmi_ip_family_e_type           bound_ip_type;
  qmi_qosi_state_e_type          current_qos_state;
  uint8                          qos_inst;   // Current (binding) QOS instance
  uint8                          orig_qmi_inst; //Original QMI instance
  struct
  {
    boolean report_global_flow; //needed for driver
    struct
    {
      boolean                  report_nw_supp_qos_profile_ch;
      ps_iface_name_enum_type  tech_pref;
    }nw_supp_qos_prof;
    boolean report_global_flow_ex;
  } report_status;
  qmi_qosi_subs_enum_type        subscription_id; //Subs id of the client
} qmi_qosi_client_state_type;

/*---------------------------------------------------------------------------
  Structure to maintain QOS state
---------------------------------------------------------------------------*/
typedef struct qmi_qosi_state_s
{
  qmi_qosi_client_state_type * client_list;
  qmi_ip_family_e_type         cur_ip_call_type;
  boolean                      is_primary_state;
  ps_iface_type            *   ps_iface_ptr;
  /*-------------------------------------------------------------------------
    Iface and Flow event callback buffers
  -------------------------------------------------------------------------*/
  void *  flow_activated_buf_ptr;
  void *  flow_suspended_buf_ptr;
  void *  flow_null_buf_ptr;
  void *  flow_modify_accepted_buf_ptr;
  void *  flow_modify_rejected_buf_ptr;
  void *  flow_prim_modify_result_buf_ptr;
  void *  flow_enabled_buf_ptr;
  void *  flow_disabled_buf_ptr;
  void *  flow_info_code_updated_buf_ptr;

  void *  iface_flow_enabled_buf_ptr;
  void *  iface_flow_disabled_buf_ptr;

  uint8   num_qos_reqs;
  boolean network_qos_aware_status;
  boolean flow_ev_reg;
  uint16  uplink_fc_seq_num; // Default flow Seq No

  struct
  {
    uint32                    ep_id;
    uint8                     mux_id;
    rmnet_instance_e_type     rmnet_inst;
    uint32                    subs_id;
  } binding;

  struct qos_constants_s
  {
    uint8                  qos_inst;
    boolean                is_default;
  } constants;
} qmi_qosi_state_type;

static struct
{
  boolean inited;
  boolean phys_link_ev_reg;
  void *  iface_nw_supported_qos_profile_ch_buf_ptr;
  void *  phys_link_flow_enabled_buf_ptr;
  void *  phys_link_flow_disabled_buf_ptr;
  ps_iface_ioctl_get_network_supported_qos_profiles_type nw_supp_qos_profiles;
  uint32  num_flow_enable;
  uint32  num_flow_disable;
} qmi_qosi_global = {0,};

/*---------------------------------------------------------------------------
  Structure to store QoS flow related info
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_qosi_client_state_type * cl_sp;
  ps_flow_type *          flow_ptr;
  uint32                  qos_handle;
  qmi_qosi_status_e_type  qos_status;
  boolean                 last_rep;
  boolean                 fwd_on_default;
  uint8                   flow_ptr_cookie;
  uint16                  uplink_fc_seq_num;
} qmi_flow_info_type;

/*-------------------------------------------------------------------------
  Array to store flow info of all QMI QoS flows
-------------------------------------------------------------------------*/

static qmi_flow_info_type qosi_flow_info[QOSI_MAX_QOS_REQUESTS] = {{0,}};

/*-------------------------------------------------------------------------
  Array to store flow info of all nw inited QMI QoS flows
-------------------------------------------------------------------------*/

static qmi_flow_info_type nw_init_qosi_flow_info[QOSI_MAX_NETWORK_INIT_FLOW_NUM] = {{0,}};

/*---------------------------------------------------------------------------
  Causes for event report indication to be triggered
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_QOSI_EV_REPORT_CAUSE_MIN                    = 0x00,
  QMI_QOSI_EV_REPORT_CAUSE_NW_SUPP_QOS_PROFILE_CH = 0x01,
  QMI_QOSI_EV_REPORT_CAUSE_MAX
} qmi_qosi_ev_report_cause_e_type;

/*---------------------------------------------------------------------------
  REQUESTED OPERATION BIT enum type
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_QOSI_REQ_OPER_BIT0_GET_TX_Q_LVL           = 0x00,
  QMI_QOSI_REQ_OPER_BIT1_GET_RMAC3_INFO         = 0x01,
  QMI_QOSI_REQ_OPER_BIT2_GET_TX_STATUS          = 0x02,
  QMI_QOSI_REQ_OPER_BIT3_GET_INACTIVITY_TIMER   = 0x03,
  QMI_QOSI_REQ_OPER_BIT4_SET_INACTIVITY_TIMER   = 0x04,
  QMI_QOSI_REQ_OPER_MAX
} qmi_qosi_req_oper_bit_e_type;

#define QOSI_REQ_OPER_ALL_BITS   (0xFFFFFFFF)

#define MAX_TECH_PREFS (2) /* allocate memory for CDMA & UMTS */
typedef PACKED struct PACKED_POST
{
  ps_iface_name_enum_type tech_name;
  uint8                   profile_count;
  uint16                  profile_value[MAX_NETWORK_SUPPORTED_PROFILE_COUNT];
} nw_supp_qos_profile_type;

typedef PACKED struct PACKED_POST
{
  uint8 num_instances;
  nw_supp_qos_profile_type nw_supp_qos_prof[MAX_TECH_PREFS];
} qosi_nw_supp_qos_profile_type;

/*---------------------------------------------------------------------------
  Structure to store event related info for reporting
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  qmi_qosi_ev_report_cause_e_type ind_cause;
  PACKED union PACKED_POST
  {
    nw_supp_qos_profile_type nw_supp_qos_prof;
  }event_info;
} qmi_qosi_event_info_type;

typedef struct
{
  uint16 cmd_id; /* qmi_cmd_id_e_type */
  uint32 subscription_id;
  union
  {
    struct
    {
      ps_flow_type                * flow_ptr;
      uint8                         flow_ptr_cookie;
      ps_iface_type               * iface_ptr;
      ps_iface_event_enum_type      event;
      ps_iface_event_info_u_type    event_info;
      void                        * qos_sp;
    } flow_info;
    struct
    {
      ps_phys_link_type           * phys_link_ptr;
      ps_iface_event_enum_type      event;
      ps_iface_event_info_u_type    event_info;
    } phys_link_info;
    struct
    {
      ps_iface_type               * iface_ptr;
      ps_iface_event_enum_type      event;
      ps_iface_event_info_u_type    event_info;
      void                        * qos_sp;
    } iface_info;

    struct
    {
      uint16    num_instances;
    } init_cb;

    struct
    {
      qmi_framework_common_msg_hdr_type    common_msg_hdr;
    } clid_cb;

    struct
    {
      qmi_framework_msg_hdr_type    msg_hdr;
      dsm_item_type             *   sdu_in;
    } cmd_hdlr_cb;

    struct
    {
      boolean network_aware_status;
    }nw_aware_info;
   
  } data;
} qmi_qosi_cmd_buf_type;

typedef struct
{
  uint32	  max_rate; 		 
  uint32	  guaranteed_rate;	
} qmi_qosi_min_max_type;


/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_QOSI_SWAP_INSTANCES()

  DESCRIPTION

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES

  SIDE EFFECTS
===========================================================================*/
void qmi_qos_swap_instances(uint32  ep_id, uint8   mux_id);

/*---------------------------------------------------------------------------
  QMI_QOS service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/
static dsm_item_type*  qmi_qosi_reset(void*, void*, void*, dsm_item_type **);
static dsm_item_type*  qmi_qosi_set_event_report(
                                             void*,
                                             void*,
                                             void*,
                                             dsm_item_type**);
static dsm_item_type*  qmi_qosi_request_qos(void*,
                                            void*,
                                            void*,
                                            dsm_item_type**);
static dsm_item_type*  qmi_qosi_release_qos(void*,
                                            void*,
                                            void*,
                                            dsm_item_type**);
static dsm_item_type*  qmi_qosi_suspend_qos(void*,
                                            void*,
                                            void*,
                                            dsm_item_type**);
static dsm_item_type*  qmi_qosi_resume_qos(void*,
                                           void*,
                                           void*,
                                           dsm_item_type**);
static dsm_item_type*  qmi_qosi_modify_qos(void*,
                                           void*,
                                           void*,
                                           dsm_item_type**);
static dsm_item_type*  qmi_qosi_get_granted_qos(
                                             void*,
                                             void*,
                                             void*,
                                             dsm_item_type**);
static dsm_item_type*  qmi_qosi_get_qos_status(
                                             void*,
                                             void*,
                                             void*,
                                             dsm_item_type**);
static dsm_item_type*  qmi_qosi_get_qos_network_status(
                                             void*,
                                             void*,
                                             void*,
                                             dsm_item_type**);

static dsm_item_type*  qmi_qosi_get_nw_supported_qos_profiles(
                                                            void*,
                                                            void*,
                                                            void*,
                                                            dsm_item_type**);
static dsm_item_type*  qmi_qosi_perform_flow_operation(void*,
                                                       void*,
                                                       void*,
                                                       dsm_item_type**);
static dsm_item_type * qmi_qosi_set_client_ip_pref(
                                                  void *            sp,
                                                  void *            cmd_buf_p,
                                                  void *            cl_sp,
                                                  dsm_item_type **  sdu_in );

static dsm_item_type*  qmi_qosi_bind_data_port(
                                       void*,
                                       void*,
                                       void*,
                                       dsm_item_type**);
static dsm_item_type * qmi_qosi_get_filter_params(
                                                  void *            sp,
                                                  void *            cmd_buf_p,
                                                  void *            cl_sp,
                                                  dsm_item_type **  sdu_in );

static dsm_item_type*  qmi_qosi_bind_subscription(
                                       void*,
                                       void*,
                                       void*,
                                       dsm_item_type**); 

static dsm_item_type*  qmi_qosi_get_bind_subscription(
                                       void*,
                                       void*,
                                       void*,
                                       dsm_item_type**); 

static dsm_item_type*  qmi_qosi_indication_register(void*,
                                            void*,
                                            void*,
                                            dsm_item_type**);

static dsm_item_type*  qmi_qosi_request_qos_ex(void*,
                                            void*,
                                            void*,
                                            dsm_item_type**);

static dsm_item_type*  qmi_qosi_modify_qos_ex(void*,
                                            void*,
                                            void*,
                                            dsm_item_type**);

static dsm_item_type*  qmi_qosi_get_qos_info(void*,
                                            void*,
                                            void*,
                                            dsm_item_type**);

qmi_error_type_v01 qmi_qosi_decode_msg
(
  uint16_t          message_id,
  dsm_item_type **  msg_ptr,
  void *            req_msg,
  uint32_t          req_size
);

boolean qmi_qosi_encode_msg
(
  uint16_t             message_id,
  void *               resp_msg,
  uint32_t             size,
  qmi_error_type_v01     err_code,
  dsm_item_type **     response
);

#define QOSI_PARSER(f)   #f,f

#define QOS_HDLR(a,b)  QMI_SVC_HDLR( a, (qmi_svc_hdlr_ftype) b )

static qmi_svc_cmd_hdlr_type  qmi_qosi_cmd_callbacks[QOSI_CMD_MAX] =
{
  QOS_HDLR( QOSI_CMD_VAL_RESET,
            qmi_qosi_reset),
  QOS_HDLR( QOSI_CMD_VAL_SET_EVENT_REPORT,
            qmi_qosi_set_event_report),
  QOS_HDLR( QOSI_CMD_VAL_REQUEST_QOS,
            qmi_qosi_request_qos),
  QOS_HDLR( QOSI_CMD_VAL_RELEASE_QOS,
            qmi_qosi_release_qos),
  QOS_HDLR( QOSI_CMD_VAL_SUSPEND_QOS,
            qmi_qosi_suspend_qos),
  QOS_HDLR( QOSI_CMD_VAL_RESUME_QOS,
            qmi_qosi_resume_qos),
  QOS_HDLR( QOSI_CMD_VAL_MODIFY_QOS,
            qmi_qosi_modify_qos),
  QOS_HDLR( QOSI_CMD_VAL_GET_GRANTED_QOS,
            qmi_qosi_get_granted_qos),
  QOS_HDLR( QOSI_CMD_VAL_GET_QOS_STATUS,
            qmi_qosi_get_qos_status),
  QOS_HDLR( QOSI_CMD_VAL_GET_QOS_NETWORK_STATUS,
            qmi_qosi_get_qos_network_status),
  QOS_HDLR( QOSI_CMD_VAL_GET_NW_SUPPORTED_QOS_PROFILES,
            qmi_qosi_get_nw_supported_qos_profiles),
  QOS_HDLR( QOSI_CMD_VAL_PERFORM_FLOW_OPERATION,
            qmi_qosi_perform_flow_operation),
  QOS_HDLR( QOSI_CMD_VAL_SET_CLIENT_IP_PREF,
            qmi_qosi_set_client_ip_pref),
  QOS_HDLR( QOSI_CMD_VAL_BIND_DATA_PORT,
            qmi_qosi_bind_data_port),
  QOS_HDLR( QOSI_CMD_VAL_GET_FILTER_PARAMS,
            qmi_qosi_get_filter_params),
  QOS_HDLR( QOSI_CMD_VAL_BIND_SUBSCRIPTION,
            qmi_qosi_bind_subscription),
  QOS_HDLR( QOSI_CMD_VAL_GET_BIND_SUBSCRIPTION,
            qmi_qosi_get_bind_subscription),
  QOS_HDLR( QOSI_CMD_VAL_INDICATION_REGISTER,
            qmi_qosi_indication_register),
  QOS_HDLR( QOSI_CMD_VAL_REQUEST_QOS_EX,
            qmi_qosi_request_qos_ex),
  QOS_HDLR( QOSI_CMD_VAL_MODIFY_QOS_EX,
            qmi_qosi_modify_qos_ex),
  QOS_HDLR( QOSI_CMD_VAL_GET_QOS_INFO,
            qmi_qosi_get_qos_info)
};

/*---------------------------------------------------------------------------
  Each QMI link (usb, BT etc) has its own QOS state. The first
  QMI_INSTANCE_MAX instances are for legacy non-binding clients. Others
  are reserved for binding MUX clients. 
---------------------------------------------------------------------------*/
#define QOS_INSTANCE_MAX  (QMI_INSTANCE_MAX + 11)
static qmi_qosi_state_type  * qmi_qos_state[QOSI_MAX_STATE][QOS_INSTANCE_MAX];

/*---------------------------------------------------------------------------
  QMI framework callback definition
---------------------------------------------------------------------------*/
static boolean qmi_qosi_alloc_clid_cb
(
  qmi_framework_common_msg_hdr_type * svc_common_hdr  
);

static void qmi_qosi_dealloc_clid_cb
(
  qmi_framework_common_msg_hdr_type * svc_common_hdr 
); 

static void qmi_qosi_init_cb
(
  uint16 num_instances
);

static void qmi_qosi_cmd_hdlr_cb
( 
  qmi_framework_msg_hdr_type* msg_hdr,
  dsm_item_type ** sdu 
);

/*---------------------------------------------------------------------------
  QMI framework command handler
---------------------------------------------------------------------------*/
static void qmi_qosi_process_svc_init
(
  uint16 num_instances
);

static void qmi_qosi_process_alloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
);

static void qmi_qosi_process_dealloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
);

static void qmi_qosi_process_cmd_hdlr
(
  qmi_framework_msg_hdr_type * msg_hdr,
  dsm_item_type * sdu 
);

static void qmi_qosi_initialize_client
(
  qmi_qosi_client_state_type  * cl_sp,
  uint16                        qos_inst,
  uint8                         clid
);

static boolean qmi_qosi_send_ind
(
  qmi_qosi_state_type        * qos_sp,
  qmi_qosi_client_state_type * cl_sp,
  uint16                       cmd_type,
  dsm_item_type              * ind
);

static boolean qmi_qosi_qos_inst_bind
(
  uint8    qos_inst,
  uint32   ep_id,
  uint8    mux_id
);

static boolean qmi_qosi_qos_inst_unbind
(
  uint8    qos_inst
);

static boolean qmi_qosi_verify_client_subs
(
  qmi_qosi_client_state_type         * cl_sp,
  uint32                               event_subs_id,
  boolean                              call_specific
);

static void qmi_qosi_reg_ps_sys_events
(  
  void
);
static uint32 qmi_qosi_resolve_client_subs
(
  void         * cl_sp
);
/*---------------------------------------------------------------------------
  QMI_WS instance configurations
---------------------------------------------------------------------------*/
static void     qmi_qosi_reset_client(void *);

static void qmi_qosi_update_client(void *);
void  qmi_qosi_sync_clients(uint8 inst);
static  ds_qmi_fw_svc_cfg_type  qmi_qosi_cfg;

typedef struct
{
  qos_spec_type   qos_spec[QOSI_MAX_QOS_SPECS_PER_REQUEST];
  ip_flow_type    tx_flows[QOSI_MAX_QOS_SPECS_PER_REQUEST][QOSI_MAX_QOS_FLOWS_PER_REQUEST];
  ip_flow_type    rx_flows[QOSI_MAX_QOS_SPECS_PER_REQUEST][QOSI_MAX_QOS_FLOWS_PER_REQUEST];
  ip_filter_type  tx_filters[QOSI_MAX_QOS_SPECS_PER_REQUEST][QOSI_MAX_QOS_FLTRS_PER_REQUEST];
  ip_filter_type  rx_filters[QOSI_MAX_QOS_SPECS_PER_REQUEST][QOSI_MAX_QOS_FLTRS_PER_REQUEST];
  ps_flow_type *  flow_ptr[QOSI_MAX_QOS_SPECS_PER_REQUEST];
  uint8           tx_flow_indxs[QOSI_MAX_QOS_SPECS_PER_REQUEST][QOSI_MAX_QOS_FLOWS_PER_REQUEST];
  uint8           rx_flow_indxs[QOSI_MAX_QOS_SPECS_PER_REQUEST][QOSI_MAX_QOS_FLOWS_PER_REQUEST];
  uint8           tx_fltr_indxs[QOSI_MAX_QOS_SPECS_PER_REQUEST][QOSI_MAX_QOS_FLTRS_PER_REQUEST];
  uint8           rx_fltr_indxs[QOSI_MAX_QOS_SPECS_PER_REQUEST][QOSI_MAX_QOS_FLTRS_PER_REQUEST];
  uint8           qos_spec_indxs[QOSI_MAX_QOS_SPECS_PER_REQUEST];
} qos_req_info_type;

typedef struct
{
  qos_spec_type   qos_spec;
  ip_flow_type    tx_flows[QOSI_MAX_QOS_FLOWS_PER_REQUEST];
  ip_flow_type    rx_flows[QOSI_MAX_QOS_FLOWS_PER_REQUEST];
  ip_filter_type  tx_filters[QOSI_MAX_QOS_FLTRS_PER_REQUEST];
  ip_filter_type  rx_filters[QOSI_MAX_QOS_FLTRS_PER_REQUEST];
} qos_req_mod_info_ex_type;

typedef struct
{
  qos_req_info_type qos_req;
  uint32 qos_id[QOSI_MAX_QOS_SPECS_PER_REQUEST];
} qos_modify_info_type;

/*===========================================================================

                   INTERNAL FUNCTION FORWARD DECLARATIONS

===========================================================================*/
static qmi_qosi_state_type *  qmi_qosi_get_qos_sp_by_cl_sp(
                                qmi_qosi_client_state_type *);
static qmi_qosi_client_state_type * qmi_qosi_get_cl_sp_by_clid(
                                qmi_qosi_state_type *, uint8);

static void qosi_flow_ev_cb
(
  ps_flow_type                * flow_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
);
static void qosi_iface_ev_cb
(
  ps_iface_type             * this_iface_ptr,
  ps_iface_event_enum_type    event,
  ps_iface_event_info_u_type  event_info,
  void                      * user_data_ptr
);

static void qosi_phys_link_ev_cb
(
  ps_phys_link_type           * phys_link_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
);

static qmi_error_e_type qosi_parse_flow_specs
(
  dsm_item_type **  sdu_in,
  uint16            len_lvl1,
  ip_flow_type *    ip_flows_arr, //array of size MAX_FLOWs
  uint8 *           flow_indxs_arr,//array of size max flows
  uint8 *           flow_count
);
static qmi_error_e_type qosi_parse_filter_specs
(
  dsm_item_type **  sdu_in,
  uint16            len_lvl1,
  ip_filter_type *  ip_fltrs_ptr,
  uint8 *           fltr_indxs_arr,//array of size max flows
  uint8 *           fltr_count
);
static qmi_error_e_type qosi_generate_flow_error_tlvs
(
  ip_flow_spec_type *  flow_spec,
  dsm_item_type **     response,
  uint8 *              flow_indxs,
  uint16 *             num_bytes_pushed
);
static qmi_error_e_type qosi_generate_filter_error_tlvs
(
  ip_filter_spec_type *  fltr_spec,
  dsm_item_type **       response,
  uint8 *                fltr_indxs,
  uint16 *             num_bytes_pushed
);
static qmi_error_e_type qosi_append_granted_flow_spec
(
  qmi_qosi_state_type * qos_sp,
  dsm_item_type ** response,
  ps_flow_type *   flow_ptr,
  uint16       *   total_bytes_pushed
);
static qmi_error_e_type qosi_append_filter_spec
(
  dsm_item_type **      response,
  ps_iface_type *       iface_ptr,
  ps_flow_type *        flow_ptr,
  uint16             *  total_bytes_pushed,
  uint8                 tx_filter_type,
  uint8                 rx_filter_type
);
static qmi_error_type_v01 qosi_append_granted_flow_spec_ex
(
  qmi_qosi_state_type             * qos_sp,
  boolean                         * tx_qos_flow_granted_valid,
  qos_flow_type_v01               * tx_qos_flow_granted,
  boolean                         * rx_qos_flow_granted_valid,
  qos_flow_type_v01               * rx_qos_flow_granted,
  uint16                          * ext_err,
  ps_flow_type                    * flow_ptr
);

static qmi_error_type_v01 qosi_append_filter_spec_ex
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * flow_ptr,
  boolean                          * tx_qos_filters_valid,
  uint32                           * tx_qos_filters_len,
  qos_filter_rule_resp_ind_type_v01 * tx_qos_filters,
  boolean                          * rx_qos_filters_valid,
  uint32                           * rx_qos_filters_len,
  qos_filter_rule_resp_ind_type_v01 * rx_qos_filters
);

static qmi_error_type_v01 qosi_convert_flow_spec_to_ps
(
  uint8                ps_flow_max_size,
  ip_flow_type *       ps_flows,
  uint32               num_flows,
  qos_flow_type_v01 *  qmi_flows
);

static qmi_error_type_v01 qosi_convert_filter_spec_to_ps
(
  uint8                       ps_filter_max_size,
  ip_filter_type *            ps_filters,
  uint32                      num_filters,
  qos_filter_rule_type_v01 *  qmi_filters
);

static void qosi_fill_v4_filter_spec
(
  qos_filter_rule_type_v01*   filter_rules,
  ip_filter_type*             fltr_spec
);

static void qosi_fill_v6_filter_spec
(
  qos_filter_rule_type_v01*   filter_rules,
  ip_filter_type*             fltr_spec
);

static void qosi_fill_fltr_xport_info
(
  qos_xport_header_type_v01* xport_info,
  ip_filter_type*            fltr_spec
);

static qmi_error_type_v01 qosi_fill_flow_error_info
(
  ip_flow_spec_type *            flow_spec,
  uint8 *                        flow_error_valid,
  uint32 *                       flow_error_len,
  qos_ip_flow_params_mask_v01 *  qos_flow_error
);

static qmi_error_type_v01 qosi_fill_filter_error_info
(
  ip_filter_spec_type *              filter_spec,
  uint8 *                            filter_error_valid,
  uint32 *                           filter_error_len,
  qos_filter_param_error_mask_v01 *  qos_filter_error
);

static void qosi_send_nw_init_event_ind
(
  qmi_qosi_state_type * qos_sp
);

static void qosi_convert_filter_spec_ntoh
(
  ip_filter_spec_type *filter_spec_ptr
);

static uint8  qosi_get_instance_from_qos_sp
(
  void *  qos_sp
);
static uint8 qosi_get_inst_from_ep_and_mux_id (uint32, uint8);

static void qosi_call_status_cb
(
  uint32               ep_id,
  uint8                mux_id,
  boolean              is_v6,
  boolean              call_up
);

static void qosi_qmi_cmd_hdlr
(
  void *  cmd_ptr
);

static void qosi_reg_ps_flow_event
(
  uint8                qos_inst,
  boolean              is_v6
);

static void qosi_dereg_ps_flow_event
(
  uint8                qos_inst,
  boolean              is_v6
);

static void qosi_reg_phys_link_event
(
  void
);

static void qosi_dereg_phys_link_event
(
  void
);

static void qosi_reg_iface_event
(
  void * sp
);

static void qosi_dereg_iface_event
(
  void * sp
);

static void qosi_process_flow_event
(
  ps_flow_type                * flow_ptr,
  uint8                         flow_ptr_cookie,
  ps_iface_type               * iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * sp
);

static void qosi_process_phys_link_event
(
  ps_phys_link_type           * phys_link_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * sp
);

static void qosi_process_iface_event
(
  ps_iface_type             * iface_ptr,
  ps_iface_event_enum_type    event,
  ps_iface_event_info_u_type  event_info,
  void                      * sp
);

static void qosi_process_iface_flow_event
(
  ps_iface_type             * iface_ptr,
  ps_iface_event_enum_type    event,
  ps_iface_event_info_u_type  event_info,
  void                      * sp
);

static void qosi_populate_ext_error_tlv
(
  qmi_error_e_type *  errval,
  dsm_item_type    ** response,
  int16              err_no
);

static void *qmi_qosi_get_cmd_buf(qmi_cmd_id_e_type);

#define qmi_qosi_free_cmd_buf(buf_ptr) PS_SYSTEM_HEAP_MEM_FREE(buf_ptr)

static void qosi_generate_event_report_ind
(
  qmi_qosi_state_type *  qos_sp,
  qmi_flow_info_type  **  qmi_flow_info_ptr_ptr,
  uint16                  num_qos_ids,
  uint8                   flow_change_ev,
  boolean                 new_flow
);

static boolean qmi_qosi_process_bind_data_port
(
  qmi_qosi_client_state_type  * cl_sp,
  uint8                         bind_inst
);

static void qmi_qosi_network_status_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_QOSI_INIT_INT()

  DESCRIPTION
    Initialize QOS Service for one QMI instance

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qosi_init_int
(
  uint8                 qos_inst,
  qmi_qosi_state_type * qos_sp,
  uint8                 is_default
)
{
  boolean                       is_err = FALSE;
  ps_iface_event_enum_type  event = IFACE_MIN_EV;
  memset( qos_sp, 0, sizeof(qmi_qosi_state_type) );

  qos_sp->constants.qos_inst = qos_inst;
  qos_sp->constants.is_default = is_default;
  qos_sp->binding.rmnet_inst = RMNET_INSTANCE_MAX;

  /*-------------------------------------------------------------------------
    Initialize the ps_iface cback buf_ptrs for Um iface event handling
  -------------------------------------------------------------------------*/

  /* FLOW, IFACE events */
  do
  {
    qos_sp->flow_activated_buf_ptr =
      ps_flow_alloc_event_cback_buf(qosi_flow_ev_cb,
                                  (void *) qos_sp);
    if(qos_sp->flow_activated_buf_ptr == NULL)
    {
      is_err = TRUE;
	  event = FLOW_ACTIVATED_EV;
      break;
    }

    qos_sp->flow_suspended_buf_ptr =
      ps_flow_alloc_event_cback_buf(qosi_flow_ev_cb,
                                  (void *) qos_sp);
    if(qos_sp->flow_suspended_buf_ptr == NULL)
    {
      is_err = TRUE;
      event = FLOW_SUSPENDED_EV;
      break;
    }

    qos_sp->flow_null_buf_ptr =
      ps_flow_alloc_event_cback_buf(qosi_flow_ev_cb,
                                  (void *) qos_sp);
    if(qos_sp->flow_null_buf_ptr == NULL)
    {
      is_err = TRUE;
  	  event = FLOW_NULL_EV;
      break;
    }

    qos_sp->flow_modify_accepted_buf_ptr =
      ps_flow_alloc_event_cback_buf(qosi_flow_ev_cb,
                                  (void *) qos_sp);
    if(qos_sp->flow_modify_accepted_buf_ptr == NULL)
    {
      is_err = TRUE;
      event = FLOW_MODIFY_ACCEPTED_EV;
      break;
    }

    qos_sp->flow_modify_rejected_buf_ptr =
      ps_flow_alloc_event_cback_buf(qosi_flow_ev_cb,
                                  (void *) qos_sp);
    if(qos_sp->flow_modify_rejected_buf_ptr == NULL)
    {
      is_err = TRUE;
   	  event = FLOW_MODIFY_REJECTED_EV;
      break;
    }

    qos_sp->flow_prim_modify_result_buf_ptr =
      ps_flow_alloc_event_cback_buf(qosi_flow_ev_cb,
                                  (void *) qos_sp);
    if(qos_sp->flow_prim_modify_result_buf_ptr == NULL)
    {
      is_err = TRUE;
      event = FLOW_PRIMARY_MODIFY_RESULT_EV;
      break;
    }

    qos_sp->flow_enabled_buf_ptr =
      ps_flow_alloc_event_cback_buf(qosi_flow_ev_cb,
                                  (void *) qos_sp);
    if(qos_sp->flow_enabled_buf_ptr == NULL)
    {
      is_err = TRUE;
      event = FLOW_TX_ENABLED_EV;
      break;
    }

    qos_sp->flow_disabled_buf_ptr =
      ps_flow_alloc_event_cback_buf(qosi_flow_ev_cb,
                                  (void *) qos_sp);
    if(qos_sp->flow_disabled_buf_ptr == NULL)
    {
      is_err = TRUE;
      event = FLOW_TX_DISABLED_EV;
      break;
    }

    qos_sp->flow_info_code_updated_buf_ptr =
      ps_flow_alloc_event_cback_buf(qosi_flow_ev_cb,
                                  (void *) qos_sp);
    if(qos_sp->flow_info_code_updated_buf_ptr == NULL)
    {
      is_err = TRUE;
      event = FLOW_INFO_CODE_UPDATED_EV;
      break;
    }

    qos_sp->iface_flow_enabled_buf_ptr   = ps_iface_alloc_event_cback_buf(
                                         qosi_iface_ev_cb,
                                         (void *) qos_sp);
    if(qos_sp->iface_flow_enabled_buf_ptr == NULL)
    {
      is_err = TRUE;
      event = IFACE_FLOW_ENABLED_EV;
      break;
    }

    qos_sp->iface_flow_disabled_buf_ptr   = ps_iface_alloc_event_cback_buf(
                                          qosi_iface_ev_cb,
                                          (void *) qos_sp);
    if(qos_sp->iface_flow_disabled_buf_ptr == NULL)
    {
      is_err = TRUE;
      event = IFACE_FLOW_DISABLED_EV;
      break;
    }
  }while(0);
  if(is_err == TRUE)
  {
    LOG_MSG_ERROR_1("registration failed at event = %d!", event);
    ASSERT(0);
    return;
  }
  if(is_default)
  {
    qos_sp->is_primary_state = TRUE;
    qos_sp->cur_ip_call_type = QMI_AF_INET;
  }else
  {
    qos_sp->is_primary_state = FALSE;
    qos_sp->cur_ip_call_type = QMI_AF_INET6;
  }
}  /* qmi_qosi_init_int() */


/*===========================================================================
  FUNCTION QMI_QOS_INIT()

  DESCRIPTION
    Register the QoS service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qos_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qmi_framework_err_e_type        reg_result;
  //qmi_idl_service_object_type     svc_obj;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_0("QMI QOS service init");

  /*-------------------------------------------------------------------------
    Setting QMI QOS service command handler with dcc task process
  -------------------------------------------------------------------------*/
  qmi_task_set_svc_cmd_handler(QMUX_SERVICE_QOS, qosi_qmi_cmd_hdlr);

  /*-------------------------------------------------------------------------
    QMI QOS service configuration setup
  -------------------------------------------------------------------------*/
  memset(&qmi_qosi_cfg, 0, sizeof(qmi_qosi_cfg));

  qmi_qosi_cfg.fw_cfg.base_version.major     = QOSI_BASE_VER_MAJOR;
  qmi_qosi_cfg.fw_cfg.base_version.minor     = QOSI_BASE_VER_MINOR;
  qmi_qosi_cfg.fw_cfg.addendum_version.major = QOSI_ADDENDUM_VER_MAJOR;
  qmi_qosi_cfg.fw_cfg.addendum_version.minor = QOSI_ADDENDUM_VER_MINOR;

  qmi_qosi_cfg.fw_cfg.cbs.alloc_clid         = qmi_qosi_alloc_clid_cb;
  qmi_qosi_cfg.fw_cfg.cbs.dealloc_clid       = qmi_qosi_dealloc_clid_cb;
  qmi_qosi_cfg.fw_cfg.cbs.init_cback         = qmi_qosi_init_cb;
  qmi_qosi_cfg.fw_cfg.cbs.cmd_hdlr           = qmi_qosi_cmd_hdlr_cb;
  
  qmi_qosi_cfg.cmd_hdlr_array    = qmi_qosi_cmd_callbacks;
  qmi_qosi_cfg.cmd_num_entries   = QOSI_CMD_MAX;

  /*-------------------------------------------------------------------------
    Calling QMI Framework API to register the service with Framework
  -------------------------------------------------------------------------*/
  reg_result = qmi_framework_reg_service( QMUX_SERVICE_QOS, &qmi_qosi_cfg.fw_cfg );

  if (reg_result != QMI_FRAMEWORK_ERR_NONE)
  {
    return;
  }

  qmi_qosi_cfg.registered = TRUE;

  qos_svc_obj = qos_get_service_object_v01();

} /* qmi_qos_init() */

/*===========================================================================
  FUNCTION QMI_QOS_GET_FLOW_PTR_FROM_HANDLE()

  DESCRIPTION
    Given a qos handle it returns the corresponding flow pointer

  PARAMETERS
    qos_handle : QOS handle for which flow pointer is to be determined
    flow_ptr   : Flow pointer corresponding to QOS handle

  RETURN VALUE
    0 on Failure
    1 on Succes

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_qos_get_flow_ptr_from_handle
(
  uint32  qos_handle,
  ps_flow_type **flow_ptr
)
{
  int16 index = 0;

  if (qos_handle == 0)
  {
    if (NULL != flow_ptr)
    {
      *flow_ptr = NULL;
    }
    return TRUE;
  }


  index = ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_QOS,
                                  (int32)qos_handle);

  if (PS_HANDLE_MGR_INVALID_INDEX == index ||
      0 >= index ||
      QOSI_MAX_QOS_REQUESTS + QOSI_MAX_NETWORK_INIT_FLOW_NUM + 1 <= index)
  {
    return FALSE;
  }
  if (NULL != flow_ptr)
  {
    if (index <= QOSI_MAX_QOS_REQUESTS)
    {
      *flow_ptr = qosi_flow_info[index-1].flow_ptr;
    }
    else
    {
      *flow_ptr = nw_init_qosi_flow_info[index-QOSI_MAX_QOS_REQUESTS-1].flow_ptr;
    }
  }
  return TRUE;
}

/*===========================================================================
FUNCTION QMI_QOSI_INIT_CB()

  DESCRIPTION
  Function gets called from framework whenever the QMI QOS service registers with
  framework. This function posts a DCC command for the service to handle this
  callback.
    
  PARAMETERS
  num_instances : getting Num of QMI instances at run time.

  RETURN VALUE
  None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_init_cb
(
  uint16 num_instances
)
{
  qmi_qosi_cmd_buf_type * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = (qmi_qosi_cmd_buf_type *)
                 qmi_qosi_get_cmd_buf(QMI_CMD_QOS_INIT_CB);
  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI QOS cmd buf");
    return;
  }

  memset(cmd_ptr, 0, sizeof(qmi_qosi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_QOS_INIT_CB;
  cmd_ptr->data.init_cb.num_instances = num_instances;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);

} /* qmi_qosi_init_cb() */

/*===========================================================================
FUNCTION QMI_QOSI_ALLOC_CLID_CB()

DESCRIPTION
  Function gets called from framework whenever a clid is allocated by the
  framework for QMI QOS service. This function posts a DCC command for the
  service to handle this callback.

PARAMETERS
  common_msg_hdr  : QMI Framework common msg hdr

RETURN VALUE
  Boolean : TRUE if the clid set properly
          : FALSE, otherwise
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean qmi_qosi_alloc_clid_cb
(  
  qmi_framework_common_msg_hdr_type * common_msg_hdr  
)
{
  qmi_qosi_cmd_buf_type * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(common_msg_hdr);

  LOG_MSG_INFO1_2("qmi_qosi_alloc_clid_cb: clid: %d, QMI inst: %d",
                 common_msg_hdr->client_id, common_msg_hdr->qmi_instance);

  cmd_ptr = (qmi_qosi_cmd_buf_type *)
                qmi_qosi_get_cmd_buf(QMI_CMD_QOS_ALLOC_CLID_CB);
  if(cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI QOS cmd buf");
    return FALSE;
  }

  memset(cmd_ptr, 0, sizeof(qmi_qosi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_QOS_ALLOC_CLID_CB;
  cmd_ptr->data.clid_cb.common_msg_hdr = *common_msg_hdr;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
  return TRUE;

} /* qmi_qosi_alloc_clid_cb() */

/*===========================================================================
FUNCTION QMI_QOSI_DEALLOC_CLID_CB()

DESCRIPTION 
  Function gets called from framework whenever a clid is deallocated by the
  framework for QMI QOS service. This function posts a DCC command for the
  service to handle this callback.

PARAMETERS
  common_msg_hdr  : QMI Framework common msg hdr

RETURN VALUE
  None
 
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void qmi_qosi_dealloc_clid_cb
(  
  qmi_framework_common_msg_hdr_type * common_msg_hdr  
)
  {
  qmi_qosi_cmd_buf_type * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(common_msg_hdr);
  LOG_MSG_INFO1_2("QMI QOS dealloc clid cb: clid: %d, QMI inst: %d",
                 common_msg_hdr->client_id, common_msg_hdr->qmi_instance);

  cmd_ptr = (qmi_qosi_cmd_buf_type *)
                qmi_qosi_get_cmd_buf(QMI_CMD_QOS_DEALLOC_CLID_CB);
  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI QOS cmd buf");
    return;
}

  memset(cmd_ptr, 0, sizeof(qmi_qosi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_QOS_DEALLOC_CLID_CB;
  cmd_ptr->data.clid_cb.common_msg_hdr = *common_msg_hdr;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);

} /* qmi_qosi_dealloc_clid_cb */

/*===========================================================================
  FUNCTION QMI_QOSI_CMD_HDLR_CB()

  DESCRIPTION
    Function gets called from framework whenever a new QMI QOS request
    message is received. This function posts a DCC command for the
    service to handle this callback.

  PARAMETERS
    msg_hdr  :  msg hdr
    sdu : dsm item

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void  qmi_qosi_cmd_hdlr_cb
(
  qmi_framework_msg_hdr_type * msg_hdr,
  dsm_item_type             ** sdu 
)
{
  qmi_qosi_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(msg_hdr);
 
  cmd_ptr = (qmi_qosi_cmd_buf_type *)
                 qmi_qosi_get_cmd_buf(QMI_CMD_QOS_CMD_HDLR_CB);
  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI QOS cmd buf");
    return;
  }

  memset(cmd_ptr, 0, sizeof(qmi_qosi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_QOS_CMD_HDLR_CB;
  cmd_ptr->data.cmd_hdlr_cb.msg_hdr = *msg_hdr;
  cmd_ptr->data.cmd_hdlr_cb.sdu_in  = *sdu;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);

} /* qmi_qosi_cmd_hdlr_cb() */

/*===========================================================================
  FUNCTION QMI_QOSI_PROCESS_SVC_INIT()

  DESCRIPTION
    This function processes a init callback request and intialize the service
    related infos.

  PARAMETERS
    num_instances:  number of QMI Instances

  RETURN VALUE
    None

  DEPENDENCIES
    QMI QOS must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_process_svc_init
(
  uint16 num_instances
)
{  
  uint8           reg_cnt = 0;
  boolean         is_err  = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* check that num_instances is within range in case ASSERT is removed */
  if (num_instances > QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI num inst out of range (%d)!", num_instances);
    ASSERT(0);
    return;
  }

  if(qmi_qosi_global.inited == FALSE)
  {
    memset(&qmi_qosi_global, 0, sizeof(qmi_qosi_global));
//TODO
    /* IFACE, physlink events */
    do
    {
      reg_cnt++;
      qmi_qosi_global.iface_nw_supported_qos_profile_ch_buf_ptr =
        ps_iface_alloc_event_cback_buf(qosi_iface_ev_cb,
                                     NULL );
      if(qmi_qosi_global.iface_nw_supported_qos_profile_ch_buf_ptr == NULL)
      {
        is_err = TRUE;
        break;
      }

      reg_cnt++;
      qmi_qosi_global.phys_link_flow_disabled_buf_ptr =
        ps_phys_link_alloc_event_cback_buf(qosi_phys_link_ev_cb,
                                       NULL);
      if(qmi_qosi_global.phys_link_flow_disabled_buf_ptr == NULL)
      {
        is_err = TRUE;
        break;
      }

      reg_cnt++;
      qmi_qosi_global.phys_link_flow_enabled_buf_ptr =
        ps_phys_link_alloc_event_cback_buf(qosi_phys_link_ev_cb,
                                       NULL);
      if(qmi_qosi_global.phys_link_flow_enabled_buf_ptr == NULL)
      {
        is_err = TRUE;
        break;
      }
    }while(0);
    if(is_err == TRUE)
    {
      LOG_MSG_ERROR_1("registration failed at reg_cnt value = %d!", reg_cnt);
      ASSERT(0);
    }

    /* Register for AWARE/UNAWARE IFACE events */
    if(0 != ps_iface_event_cback_reg(NULL,
                           IFACE_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV,
                           qmi_qosi_global.iface_nw_supported_qos_profile_ch_buf_ptr))
    {
      DATA_ERR_FATAL("Subscribe event list failed!");
       return;
     }

    /*-------------------------------------------------------------------------
      Initialize system level events at power up
    -------------------------------------------------------------------------*/
    qmi_qosi_reg_ps_sys_events();

    /* register call status cback with WDS to inform qos service
     when call is up or torn down */
    qmi_wds_set_call_status_handler(qosi_call_status_cb);

    /* Create a pool of QOSI_MAX_QOS_REQUESTS + QOSI_MAX_NETWORK_INIT_FLOW_NUM + 1 items
       Index 0 will not be used otherwise there is no way to distinguish default flow
       from a valid flow at index 0.
       Index 1 to QOSI_MAX_QOS_REQUESTS will be used for UE init QOS flows
       Index QOSI_MAX_QOS_REQUESTS + 1 to QOSI_MAX_QOS_REQUESTS + QOSI_MAX_NETWORK_INIT_FLOW_NUM
       will be used for NW init QOS flows */
    ps_handle_mgr_init_client(PS_HANDLE_MGR_CLIENT_QOS,
                              QOSI_MAX_QOS_REQUESTS + QOSI_MAX_NETWORK_INIT_FLOW_NUM + 1,
                              0,
                              0);

    qmi_qosi_global.inited = TRUE;
  }
  /* QMI instance state init is postponed until a client is registered  */
  qmi_wds_set_swap_instance_handler(qmi_qos_swap_instances);

} /* qmi_qosi_process_svc_init() */

/*===========================================================================
  FUNCTION QMI_QOSI_CHECK_AND_ALLOC_QOS_SP()

  DESCRIPTION
    This function checks and allocates qos state for both v4 and v6 instance.

  PARAMETERS
    qos_inst: QOS instance

  RETURN VALUE
    TRUE: Success. FALSE: Failure  

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_qosi_check_and_alloc_qos_sp
(
  uint8   qos_inst
)
{
  qmi_qosi_state_type * v4_sp;
  qmi_qosi_state_type * v6_sp;
/*-------------------------------------------------------------------------*/

  if (qos_inst >= QOS_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("Invalid qos inst %d", qos_inst);
    ASSERT(0);
    return FALSE;
  }

  // Check if QOS states are already allocated
  if ( (qmi_qos_state[QOSI_IPV4_STATE][qos_inst] != NULL) &&
       (qmi_qos_state[QOSI_IPV6_STATE][qos_inst] != NULL) )
  {
    return TRUE;
  }

  // Allocate both V4 and V6 QOS states
  PS_SYSTEM_HEAP_MEM_ALLOC(v4_sp, sizeof(qmi_qosi_state_type),
                           qmi_qosi_state_type*);
  if (v4_sp == NULL)
  {
    return FALSE;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(v6_sp , sizeof(qmi_qosi_state_type),
                           qmi_qosi_state_type*);
  if (v6_sp == NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(v4_sp);
    return FALSE;
  }

  qmi_qosi_init_int(qos_inst, v4_sp, 1);
  qmi_qosi_init_int(qos_inst, v6_sp, 0);

  qmi_qos_state[QOSI_IPV4_STATE][qos_inst] = v4_sp;
  qmi_qos_state[QOSI_IPV6_STATE][qos_inst] = v6_sp;

  return TRUE;

} /* qmi_qosi_check_and_alloc_qos_sp() */

/*===========================================================================
  FUNCTION QMI_QOSI_PROCESS_ALLOC_CLID()

  DESCRIPTION
    This function process the client alloc callback request. This function
    allocates and initialize the new client state for the allocated client ID.

  PARAMETERS
    common_msg_hdr:  private data buffer containing the clid alloc request information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI QOS must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_process_alloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_qosi_state_type          * qos_sp;
  qmi_qosi_client_state_type   * cl_sp;  
  qmi_result_e_type              result = QMI_RESULT_SUCCESS; 
  qmi_error_e_type               errval = QMI_ERR_NONE;
  uint32                         ep_id;
  uint8                          qos_inst, bind_inst;
/*-------------------------------------------------------------------------*/

  if ( (common_msg_hdr == NULL) ||
       (common_msg_hdr->service != QMUX_SERVICE_QOS) ||
       (common_msg_hdr->qmi_instance >= QMI_INSTANCE_MAX) ||
       (common_msg_hdr->client_id == QMI_SVC_CLID_UNUSED) ||
       (common_msg_hdr->client_id >= QMI_FRAMEWORK_SVC_MAX_CLIENTS) )
  {
    DATA_ERR_FATAL("Invalid common_msg_hdr");
    return;
  }

  // Default binding
  ep_id = qmux_get_default_ep_id_from_inst(common_msg_hdr->qmi_instance);
  qos_inst = (uint8)common_msg_hdr->qmi_instance;
  if (qmi_qosi_qos_inst_bind(qos_inst, ep_id, 0))
  {
    qos_sp = qmi_qos_state[QOSI_IPV4_STATE][common_msg_hdr->qmi_instance];

    // Check if client already exists
    cl_sp = qmi_qosi_get_cl_sp_by_clid(NULL, common_msg_hdr->client_id);
    if (cl_sp != NULL)
    {
      LOG_MSG_ERROR_1("clid %d exists", common_msg_hdr->client_id);
      errval = QMI_ERR_INTERNAL;
      goto send_result;
    }

    PS_SYSTEM_HEAP_MEM_ALLOC(cl_sp, sizeof(qmi_qosi_client_state_type),
                             qmi_qosi_client_state_type*);
    if (cl_sp)
    {
      qmi_qosi_initialize_client(cl_sp,
          (uint16)common_msg_hdr->qmi_instance, common_msg_hdr->client_id);

      // Link the client to the head of the client list
      cl_sp->next = qos_sp->client_list;
      qos_sp->client_list = cl_sp;

      // V4 and V6 QOS states share same clients
      qos_sp = qmi_qos_state[QOSI_IPV6_STATE][common_msg_hdr->qmi_instance];
      qos_sp->client_list = cl_sp;

      if (ep_id != 0)
      {
        bind_inst = qmux_get_inst_from_ep_id(ep_id);
        if (bind_inst != (uint8)common_msg_hdr->qmi_instance)
        {
          if (!qmi_qosi_qos_inst_bind(bind_inst, ep_id, 0))
          {
            LOG_MSG_ERROR_1("Unable to bind to %d inst", bind_inst);
            errval = QMI_ERR_INTERNAL;
            goto send_result;
          }
          // In case two control channels have the same default ep_id, 
          // bind the client to the first channel
          cl_sp = qmi_qosi_get_cl_sp_by_clid(NULL, common_msg_hdr->client_id);
          qmi_qosi_process_bind_data_port(cl_sp, bind_inst);
        }
      }
      if(cl_sp != NULL)
      {
        LOG_MSG_INFO2("Allocated QOS clid %d QMI inst %d cl_sp %p", 
                       cl_sp->common.clid, cl_sp->qos_inst, cl_sp );
      }
    }
    else
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }
  }
  else
  {
    errval = QMI_ERR_INTERNAL;
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;
  /* Notifying the clid allocation back to client via QMI Framework */
  qmi_framework_svc_send_alloc_clid_result_ex(result, common_msg_hdr, errval);

} /* qmi_qosi_process_alloc_clid() */

/*===========================================================================
  FUNCTION QMI_QOSI_PROCESS_DEALLOC_CLID()

  DESCRIPTION
    This function process the client dealloc callback request. This function
    reset and deallocates the new client state for the allocated client ID.

  PARAMETERS
    common_msg_hdr:  private data buffer containing the clid dealloc
    request information.
 
  RETURN VALUE
    None

  DEPENDENCIES
    QMI QOS must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_process_dealloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_qosi_state_type         * qos_sp_v4;
  qmi_qosi_state_type         * qos_sp_v6;
  qmi_qosi_client_state_type  * cl_sp;   
  qmi_qosi_client_state_type  * cl_sp_prev;
  qmi_result_e_type             result = QMI_RESULT_FAILURE; 
  qmi_error_e_type              errval = QMI_ERR_INTERNAL;
  uint8                         cl_sp_qmi_inst;
/*-------------------------------------------------------------------------*/

  if ( (common_msg_hdr == NULL) ||
       (common_msg_hdr->service != QMUX_SERVICE_QOS) ||
       (common_msg_hdr->qmi_instance >= QMI_INSTANCE_MAX) ||
       (common_msg_hdr->client_id == QMI_SVC_CLID_UNUSED) ||
       (common_msg_hdr->client_id >= QMI_FRAMEWORK_SVC_MAX_CLIENTS) )
  {
    DATA_ERR_FATAL("Invalid common_msg_hdr");
    return;
  }

  cl_sp = qmi_qosi_get_cl_sp_by_clid(NULL, common_msg_hdr->client_id);
  if (cl_sp == NULL)
  {
    LOG_MSG_ERROR_1("Can't find client %d", common_msg_hdr->client_id);
    errval = QMI_ERR_INVALID_CLIENT_ID;
    goto send_result;
  }

  if (cl_sp->orig_qmi_inst != common_msg_hdr->qmi_instance)
  {
    LOG_MSG_ERROR("QMI inst mismatch. clid %d inst %d orig_qmi_inst %d",
                      common_msg_hdr->client_id,
                      common_msg_hdr->qmi_instance,
                      cl_sp->orig_qmi_inst);
    goto send_result;
  }

  cl_sp_qmi_inst = cl_sp->qos_inst;
  qos_sp_v4 = qmi_qos_state[QOSI_IPV4_STATE][cl_sp_qmi_inst];
  qos_sp_v6 = qmi_qos_state[QOSI_IPV6_STATE][cl_sp_qmi_inst];
  if ((qos_sp_v4 == NULL) || (qos_sp_v6 == NULL))
  {
    LOG_MSG_ERROR_1("QOS state %d is not initialized",
                     cl_sp_qmi_inst);
    goto send_result;
  }

  cl_sp_prev = NULL;
  cl_sp = qos_sp_v4->client_list;
  while (cl_sp != NULL)
  {
    if (cl_sp->common.clid == common_msg_hdr->client_id)
    {
      qmi_qosi_reset_client(cl_sp);

      q_destroy(&cl_sp->common.x_free_q);
      q_destroy(&cl_sp->common.cmd_free_q);

      // Unlink the client from client list
      if (cl_sp_prev == NULL)
      {
        qos_sp_v4->client_list = cl_sp->next;
      }
      else
      {
        cl_sp_prev->next = cl_sp->next;
      }

      // QOS V4 and V6 states share same clients
      qos_sp_v6->client_list = qos_sp_v4->client_list;

      PS_SYSTEM_HEAP_MEM_FREE(cl_sp);

      LOG_MSG_INFO2_2( "Released QOS clid %d QMI inst %d", 
               common_msg_hdr->client_id, cl_sp_qmi_inst );

      // Unbind QOS if all clients and calls are released
      if ( (qos_sp_v4->client_list == NULL) &&
           (qos_sp_v4->ps_iface_ptr == NULL) &&
           (qos_sp_v6->ps_iface_ptr == NULL) )
      {
        qmi_qosi_qos_inst_unbind(cl_sp_qmi_inst);
      }
      errval = QMI_ERR_NONE;
      result = QMI_RESULT_SUCCESS;
      break;
    }

    cl_sp_prev = cl_sp;
    cl_sp = cl_sp->next;
  }

  if (result != QMI_RESULT_SUCCESS)
  {
    LOG_MSG_ERROR_1("Can't find clid %d!", common_msg_hdr->client_id);
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;
  /* Notifying the clid deallocation back to client via QMI Framework */
  qmi_framework_svc_send_dealloc_clid_result_ex(result, common_msg_hdr, errval);

} /* qmi_qosi_process_dealloc_clid() */

/*===========================================================================
  FUNCTION QMI_QOSI_INITIALIZE_CLIENT()

  DESCRIPTION
    Initialize the new client state for the given client state ptr.
    
  PARAMETERS
    cl_sp :   client state pointer
    qos_inst: QOS instance
    clid:     client id
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_initialize_client
(
  qmi_qosi_client_state_type  * cl_sp,
  uint16                        qos_inst,
  uint8                         clid
)
{
  int i;
  qmi_transaction_type *  x_p;
  qmi_qosi_state_type *   qos_sp;
  qmi_cmd_buf_type *      cmd_buf_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (cl_sp == NULL) || (qos_inst >= QOS_INSTANCE_MAX) ||
       (qmi_qos_state[QOSI_IPV4_STATE][qos_inst] == NULL) ||
       (qmi_qos_state[QOSI_IPV6_STATE][qos_inst] == NULL) )
  {
    DATA_ERR_FATAL("Invalid parameters");
    return;
  }

  memset(cl_sp, 0, sizeof(qmi_qosi_client_state_type));

  cl_sp->qos_inst      = (uint8) qos_inst;
  cl_sp->orig_qmi_inst = (uint8) qos_inst;
  cl_sp->common.clid  = clid;

  if (qmi_qos_state[QOSI_IPV4_STATE][qos_inst]->is_primary_state)
  {
    qos_sp = qmi_qos_state[QOSI_IPV4_STATE][qos_inst];
    cl_sp->current_qos_state = QOSI_IPV4_STATE;
  }
  else
  {
    qos_sp = qmi_qos_state[QOSI_IPV6_STATE][qos_inst];
    cl_sp->current_qos_state = QOSI_IPV6_STATE;
  }

  /*---------------------------------------------------------------------
    initialize the transaction buffers and place in free q.
    client state pointer must always point to the client state pointer
    to ensure even freeing a freed transaction (as in qosi_reset_client)
    queues the transaction in the correct client state info
  ---------------------------------------------------------------------*/
  memset (&cl_sp->common.x_free_q, 0, sizeof(q_type));
  (void) q_init( &cl_sp->common.x_free_q );
  x_p = &cl_sp->common.transactions[0];
  for (i=MAX_PENDING_TRANSACTIONS; i; i--, x_p++ )
  {
    x_p->svc_sp = qos_sp;
    x_p->cl_sp = (qmi_common_client_state_type *)cl_sp;
    (void) q_link( x_p, &x_p->link );
    q_put( &cl_sp->common.x_free_q, &x_p->link );
  }

  /* initialize the command buffers and and place in free q */
  memset (&cl_sp->common.cmd_free_q, 0, sizeof(q_type));
  (void) q_init( &cl_sp->common.cmd_free_q );
  cmd_buf_p = &cl_sp->common.cmd_buf[0];
  for (i=MAX_PENDING_COMMANDS; i; i--, cmd_buf_p++ )
  {
    (void) q_link( cmd_buf_p, &cmd_buf_p->link );
    q_put( &cl_sp->common.cmd_free_q, &cmd_buf_p->link );
  }
} /* qmi_qosi_initialize_client() */

/*===========================================================================
  FUNCTION QMI_QOSI_PROCESS_CMD_HDLR()

  DESCRIPTION
    This function process the service command handler request callback.
    This function creates a transaction and dispatches to the appropriate
    message handler
    
  PARAMETERS
    msg_hdr:  Framework message header for the request.
    sdu_in : request

  RETURN VALUE
    None

  DEPENDENCIES
    QMI QOS must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_process_cmd_hdlr
(
  qmi_framework_msg_hdr_type * msg_hdr,
  dsm_item_type              * sdu 
)
{
  qmi_qosi_state_type         * qos_sp;
  qmi_qosi_client_state_type  * cl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (msg_hdr == NULL) || (sdu == NULL) ||
       (msg_hdr->common_hdr.qmi_instance >= QMI_INSTANCE_MAX) )
  {
    DATA_ERR_FATAL("Invalid parameters");
    dsm_free_packet(&sdu);
    return;
  }

  cl_sp = qmi_qosi_get_cl_sp_by_clid(NULL, msg_hdr->common_hdr.client_id);
  if (cl_sp == NULL)
  {
    dsm_free_packet(&sdu);
    LOG_MSG_ERROR_1("Can't find client %d",
                      msg_hdr->common_hdr.client_id);
    return;
  }

  if (cl_sp->orig_qmi_inst != msg_hdr->common_hdr.qmi_instance)
  {
    LOG_MSG_ERROR("QMI inst mismatch. clid %d inst %d orig_qmi_inst %d",
                      msg_hdr->common_hdr.client_id,
                      msg_hdr->common_hdr.qmi_instance,
                      cl_sp->orig_qmi_inst);
    dsm_free_packet(&sdu);
    return;
  }

  LOG_MSG_INFO2_2("Client %d is bound to QOS inst %d",
                     cl_sp->common.clid, cl_sp->qos_inst);
  // This qos_sp is the one that the client actually binds to
  qos_sp = qmi_qosi_get_qos_sp_by_cl_sp(cl_sp); 

  ds_qmi_fw_recv(&qmi_qosi_cfg, qos_sp, msg_hdr, &cl_sp->common, sdu);

} /* qmi_qosi_process_cmd_hdlr() */

/*===========================================================================
  FUNCTION QMI_QOSI_PROCESS_BIND_DATA_PORT()

  DESCRIPTION
    This function process the QOS client bind command. The client on the
    original instance will be released and a new client with the same clid
    will be allocated on the binding instance.
    
  PARAMETERS
    clid:         client ID
    orig_inst:    orignal qmi instance
    bind_inst:    binding qmi instance

  RETURN VALUE
    None

  DEPENDENCIES
    QMI QOS must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_qosi_process_bind_data_port
(
  qmi_qosi_client_state_type  * cl_sp,
  uint8                         bind_inst
)
{
  qmi_qosi_state_type         * curr_qos_sp_v4;
  qmi_qosi_state_type         * curr_qos_sp_v6;
  qmi_qosi_state_type         * bind_qos_sp_v4;
  qmi_qosi_state_type         * bind_qos_sp_v6;
  qmi_qosi_client_state_type  * cl_sp_prev;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cl_sp == NULL || bind_inst >= QOS_INSTANCE_MAX ||
      cl_sp->qos_inst >= QOS_INSTANCE_MAX)
  {
    ASSERT(0);
    return FALSE;
  }

  curr_qos_sp_v4 = qmi_qos_state[QOSI_IPV4_STATE][cl_sp->qos_inst];
  curr_qos_sp_v6 = qmi_qos_state[QOSI_IPV6_STATE][cl_sp->qos_inst];

  bind_qos_sp_v4 = qmi_qos_state[QOSI_IPV4_STATE][bind_inst];
  bind_qos_sp_v6 = qmi_qos_state[QOSI_IPV6_STATE][bind_inst];

  if (curr_qos_sp_v4 == NULL || curr_qos_sp_v6 == NULL ||
      bind_qos_sp_v4 == NULL || bind_qos_sp_v6 == NULL)
  {
    ASSERT(0);
    return FALSE;
  }

  LOG_MSG_INFO1("Binding clid %d from inst %d to inst %d",
                   cl_sp->common.clid, cl_sp->qos_inst, bind_inst);

  // Reset the client
  qmi_qosi_reset_client(cl_sp);

  // Remove the client from the current inst
  if (curr_qos_sp_v4->client_list == cl_sp)
  {
    curr_qos_sp_v4->client_list = cl_sp->next;
    curr_qos_sp_v6->client_list = cl_sp->next;

    // Unbind QOS if all clients and calls are released
    if ( (curr_qos_sp_v4->client_list == NULL) &&
         (curr_qos_sp_v4->ps_iface_ptr == NULL) &&
         (curr_qos_sp_v6->ps_iface_ptr == NULL) )
    {
      qmi_qosi_qos_inst_unbind(cl_sp->qos_inst);
    }
  }
  else
  {
    cl_sp_prev = curr_qos_sp_v4->client_list;
    while (cl_sp_prev != NULL)
    {
      if (cl_sp_prev->next == cl_sp)
      {
        break;
      }
      cl_sp_prev = cl_sp_prev->next;
    }
    if (cl_sp_prev == NULL)
    {
      LOG_MSG_ERROR_2("Client %d not found in QOS inst %d",
            cl_sp->common.clid, cl_sp->qos_inst);
      return FALSE;
    }
    cl_sp_prev->next = cl_sp->next;
  }

  // Update the client
  cl_sp->qos_inst = bind_inst;
  if (bind_qos_sp_v4->is_primary_state)
  {
    cl_sp->current_qos_state = QOSI_IPV4_STATE;
  }
  else
  {
    cl_sp->current_qos_state = QOSI_IPV6_STATE;
  }
  qmi_qosi_update_client(cl_sp);

  // Move the client to the binding inst
  cl_sp->next = bind_qos_sp_v4->client_list;
  bind_qos_sp_v4->client_list = cl_sp;
  bind_qos_sp_v6->client_list = cl_sp;

  return TRUE;

} /* qmi_qosi_process_bind_data_port() */

/*===========================================================================
  FUNCTION QMI_QOSI_SEND_IND()

  DESCRIPTION
    This function sends a QOS indication message.
    
  PARAMETERS
    qos_sp: qos state pointer
    cl_sp: qos client state pointer
    cmd_type: indication cmd type 
    ind: indication data pointer

  RETURN VALUE
    TRUE: success. FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_qosi_send_ind
(
  qmi_qosi_state_type        * qos_sp,
  qmi_qosi_client_state_type * cl_sp,
  uint16                       cmd_type,
  dsm_item_type              * ind
)
{
  qmi_framework_common_msg_hdr_type  common_hdr;

  dsm_item_type                    * dup_ind = NULL;
  uint16                             len = 0;
  boolean                            ret = TRUE;
  qmi_qosi_client_state_type       * loop_cl_sp = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( qos_sp == NULL )
  {
    DATA_ERR_FATAL("Invalid parameters");
    dsm_free_packet(&ind);
    return FALSE;
  }

  common_hdr.service        = QMUX_SERVICE_QOS;
  common_hdr.transaction_id = 0; // Ignored in indication

  /* For broadcast ind, convert it to unicast */
  if (cl_sp == NULL)
  {
    len = (uint16)dsm_length_packet(ind);

    loop_cl_sp = qos_sp->client_list;
    while ( (loop_cl_sp != NULL) && (ret == TRUE) )
    {
      /* For legacy clients always send indication.
       * Otherwise send the ind only when IP type matches
       */
      if ( (loop_cl_sp->bound_ip_type == QMI_AF_ANY ||
            qos_sp == qmi_qosi_get_qos_sp_by_cl_sp(loop_cl_sp)) )
      {
        common_hdr.client_id    = loop_cl_sp->common.clid;
        common_hdr.qmi_instance = loop_cl_sp->orig_qmi_inst;

        dup_ind = NULL;
        if (PS_UTILS_DSM_DUP_PACKET(&dup_ind, ind, 0, len) == len)
        {
          ret = ds_qmi_fw_send_ind(&common_hdr, cmd_type, dup_ind);
        }
        else
        {
          ret = FALSE;
        }
      } /* if (loop_cl_sp ...) */
      loop_cl_sp = loop_cl_sp->next;
    } /* while */

    dsm_free_packet(&ind);
  }
  else
  {
    common_hdr.client_id    = cl_sp->common.clid;
    common_hdr.qmi_instance = cl_sp->orig_qmi_inst; 
    ret = ds_qmi_fw_send_ind(&common_hdr, cmd_type, ind);
  }

  return ret;

} /* qmi_qosi_send_ind() */

/*===========================================================================
  FUNCTION QMI_QOSI_QOS_INST_BIND()

  DESCRIPTION
    This function binds a QOS instance.
    
  PARAMETERS
    qos_inst:     QOS instance to bind
    ep_id:        EP id
    mux_id:       MUX id

  RETURN VALUE
    TRUE: success. FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_qosi_qos_inst_bind
(
  uint8    qos_inst,
  uint32   ep_id,
  uint8    mux_id
)
{
  qmi_qosi_state_type  * qos_sp_v4;
  qmi_qosi_state_type  * qos_sp_v6;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qos_inst >= QOS_INSTANCE_MAX)
  {
    ASSERT(0);
    return FALSE;
  }

  if (!qmi_qosi_check_and_alloc_qos_sp(qos_inst))
  {
    LOG_MSG_ERROR_1("Failed to alloc qos sp %d", qos_inst);
    return FALSE;
  }

  qos_sp_v4 = qmi_qos_state[QOSI_IPV4_STATE][qos_inst];
  qos_sp_v6 = qmi_qos_state[QOSI_IPV6_STATE][qos_inst];

  if ( (qos_sp_v4->binding.ep_id != ep_id) ||
       (qos_sp_v4->binding.mux_id != mux_id) )
  {
    ASSERT(qos_sp_v4->binding.ep_id == 0);
    // Bind qos instance
    qos_sp_v4->binding.ep_id       = ep_id;
    qos_sp_v4->binding.mux_id      = mux_id;
    qos_sp_v4->binding.subs_id     = QMI_QOSI_SUBS_DEFAULT;

    qos_sp_v6->binding.ep_id       = ep_id;
    qos_sp_v6->binding.mux_id      = mux_id;
    qos_sp_v6->binding.subs_id     = QMI_QOSI_SUBS_DEFAULT;


    LOG_MSG_INFO1("Binding QOS inst %d to ep_id 0x%x mux_id %d",
                     qos_inst, ep_id, mux_id);
    return TRUE;
  }

  return TRUE;

} /* qmi_qosi_qos_inst_bind() */

/*===========================================================================
  FUNCTION QMI_QOSI_QOS_INST_UNBIND()

  DESCRIPTION
    This function unbinds a QOS instance.
    
  PARAMETERS
    qos_inst:  QOS instance to unbind

  RETURN VALUE
    TRUE: success. FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_qosi_qos_inst_unbind
(
  uint8    qos_inst
)
{
  qmi_qosi_state_type  * qos_sp_v4;
  qmi_qosi_state_type  * qos_sp_v6;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qos_inst >= QOS_INSTANCE_MAX)
  {
    ASSERT(0);
    return FALSE;
  }

  qos_sp_v4 = qmi_qos_state[QOSI_IPV4_STATE][qos_inst];
  qos_sp_v6 = qmi_qos_state[QOSI_IPV6_STATE][qos_inst];
  if (qos_sp_v4 == NULL || qos_sp_v6 == NULL)
  {
    ASSERT(0);
    return FALSE;
  }

  // Unbind qos instance
  qos_sp_v4->binding.ep_id       = 0;
  qos_sp_v4->binding.mux_id      = 0;

  qos_sp_v6->binding.ep_id       = 0;
  qos_sp_v6->binding.mux_id      = 0;

  LOG_MSG_INFO1_1("Unbinding QOS inst %d", qos_inst);

  return TRUE;

} /* qmi_qosi_qos_inst_unbind() */

/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_QOSI_RESET()

  DESCRIPTION
    Reset the issuing client's QOS state

  PARAMETERS
    sp      : service provided state pointer (user data)
    clid    : client ID
    sdu_in  : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_qosi_reset
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *              response;
  boolean                      retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  /*-------------------------------------------------------------------------
    Reset the client's state
  -------------------------------------------------------------------------*/
  qmi_qosi_reset_client(cl_sp);

  /*-------------------------------------------------------------------------
    Build the response from back to front
    No error possible in reset, so result is success
  -------------------------------------------------------------------------*/
  response = NULL;

  retval = qmi_svc_put_result_tlv(&response,
                                  QMI_RESULT_SUCCESS,
                                  QMI_ERR_NONE);
  CHECK_RETVAL();

  return response;

} /* qmi_qosi_reset() */



/*===========================================================================
  FUNCTION QMI_QOSI_SET_EVENT_REPORT()

  DESCRIPTION
    Set the conditions for event reporting

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_qosi_set_event_report
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *  response;
  qmi_qosi_client_state_type * qos_client_sp;
  struct
  {
    boolean report_global_flow;
  } v_in1;

  PACKED struct PACKED_POST
  {
    boolean                  report_nw_supp_qos_profile_ch;
    ps_iface_name_enum_type  tech_pref;
  } v_in2 = {0,};

  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;
  boolean            got_global_flow_tlv;
  boolean            got_nw_supp_qos_profile_ch_tlv;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  memset(&v_in1, 0 , sizeof(v_in1));
  memset((void *)&v_in2, 0 , sizeof(v_in2));

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  response = NULL;
  errval = QMI_ERR_NONE;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;

  got_global_flow_tlv = FALSE;
  got_nw_supp_qos_profile_ch_tlv = FALSE;
  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_PRM_TYPE_GLOBAL_FLOW_INDICATOR:
        expected_len = QMI_PRM_TYPE_GLOBAL_FLOW_INDICATOR_LEN;
        got_global_flow_tlv = TRUE;
        value = (void *) &v_in1;
        break;

      case QMI_PRM_TYPE_NW_SUPP_QOS_PROFILE_CH_INDICATOR:
        expected_len = QMI_PRM_TYPE_NW_SUPP_QOS_PROFILE_CH_INDICATOR_LEN;
        got_nw_supp_qos_profile_ch_tlv = TRUE;
        value = (void *) &v_in2;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                       len,
                                       expected_len,
                                       FILE_ID_DS_QMI_QOS,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  /*-------------------------------------------------------------------------
    If ALL optional TLVs are absent return error or else carry on and
    process the request. (Needed since NO mandatory TLVs)
  -------------------------------------------------------------------------*/
  if(!(got_global_flow_tlv || got_nw_supp_qos_profile_ch_tlv))
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if(v_in1.report_global_flow)
  {
    qos_client_sp->report_status.report_global_flow = TRUE;
  }
  else
  {
    qos_client_sp->report_status.report_global_flow = FALSE;
  }

  if(v_in2.report_nw_supp_qos_profile_ch)
  {
    qos_client_sp->report_status.nw_supp_qos_prof.report_nw_supp_qos_profile_ch = TRUE;
    qos_client_sp->report_status.nw_supp_qos_prof.tech_pref = v_in2.tech_pref;
  }
  else
  {
    qos_client_sp->report_status.nw_supp_qos_prof.report_nw_supp_qos_profile_ch = FALSE;
    qos_client_sp->report_status.nw_supp_qos_prof.tech_pref =
                                                  (ps_iface_name_enum_type)0;
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv( &response, result, errval );
  CHECK_RETVAL();
  return response;
} /* qmi_qosi_set_event_report() */


/*===========================================================================
  FUNCTION QMI_QOS_REQUEST_QOS()

  DESCRIPTION
    Request a new QoS Flow

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_request_qos
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_client_state_type *     qos_client_sp;
  qmi_qosi_state_type *            qos_sp;
  dsm_item_type *                  response;
  ps_iface_type *                  ps_iface_ptr;
  ps_iface_ioctl_qos_request_type  qos_request;
  ps_iface_ioctl_qos_request_ex_type  qos_req_ex;
  qos_req_info_type *  req_ptr;
  uint32 qos_id_list[QOSI_MAX_QOS_SPECS_PER_REQUEST];
  uint8                  qos_spec_cnt;
  uint8                  qos_id_bytes;
  uint8                  type;
  uint8                  i;
  uint8                  j;
  uint16                 len_lvl1 = 0;
  uint16                 len_lvl2 = 0;
  uint16                 num_bytes_lvl2, expected_len;
  void *                 value;
  uint16                 cnt;
  uint16                 num_bytes_pushed;
  uint8                  flow_count;
  uint8                  fltr_count;
  int16                  ps_errno;
  qmi_error_e_type       errval;
  qmi_result_e_type      result;
  boolean                retval;
  boolean                bundling_supported;
  int                    return_val;
  boolean                got_qos_req_opcode_tlv = FALSE;
  qmi_ip_family_e_type   ip_type, ip_type_prev;
  uint8                  inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  num_bytes_lvl2 = 0;
  qos_spec_cnt = 0;
  cnt = 0;
  ps_errno = 0;
  ip_type_prev = QMI_AF_ANY;

  qos_sp = (qmi_qosi_state_type *) sp;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;
  ip_type = qos_client_sp->bound_ip_type;
  inst = (uint8) qosi_get_instance_from_qos_sp(qos_sp);

  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(req_ptr, sizeof(qos_req_info_type),
                                  qos_req_info_type*);
  if (req_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  memset(req_ptr, 0, sizeof(qos_req_info_type));
  memset(&qos_request, 0, sizeof(qos_request));
  memset(&qos_req_ex, 0, sizeof(qos_req_ex));

  LOG_MSG_INFO2_1("Received QoS Request from client %d",
                  qos_client_sp->common.clid);

  response = NULL;
  errval   = QMI_ERR_NONE;
  bundling_supported = TRUE;

  while(*sdu_in)
  {
    if(qos_spec_cnt == QOSI_MAX_QOS_SPECS_PER_REQUEST)
    {
      break;
    }

    if( !qmi_svc_get_tl(sdu_in, &type, &len_lvl1) )
    {
      continue;
    }

    value = NULL;
    switch(type)
    {
      case QMI_PRM_TYPE_QOS_SPEC:
      {
        while (*sdu_in)
        {
          if(num_bytes_lvl2 == len_lvl1)
          {
            num_bytes_lvl2 = 0;
            break;
          }

          if( !qmi_svc_get_tl(sdu_in, &type, &len_lvl2) )
          {
            continue;
          }
          if(!(len_lvl2 < len_lvl1))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          /* Check for overflow */
          if((MAX_UINT16 - len_lvl2 - 3) < num_bytes_lvl2)
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          num_bytes_lvl2 += len_lvl2 + 3;

          if(num_bytes_lvl2 > len_lvl1)
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          expected_len = 0;
          value = NULL;
          switch (type)
          {
            case QMI_PRM_TYPE_QOS_SPEC_INDEX:
            {
              expected_len = sizeof(uint8);
              value = (void*) &(req_ptr->qos_spec_indxs[qos_spec_cnt]);
              if(!qmi_svc_validate_type_length( type,
                                                len_lvl2,
                                                expected_len,
                                                FILE_ID_DS_QMI_QOS,__LINE__) )
              {
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
              if(len_lvl2 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl2 ))
              {
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
              break;
            }

            case QMI_PRM_TYPE_TX_QOS_FLOW_REQ:
            {
              /*-------------------------------------------------------------------
                Parse the Nested Flow spec TLVs
              -------------------------------------------------------------------*/
              errval = qosi_parse_flow_specs(sdu_in,
                                             len_lvl2,
                                             &(req_ptr->tx_flows[qos_spec_cnt][0]),
                                             &(req_ptr->tx_flow_indxs[qos_spec_cnt][0]),
                                             &flow_count);
              if(errval != QMI_ERR_NONE)
              {
                goto send_result;
              }
              /*-------------------------------------------------------------------
                Skip, If no Flow specs were present
              -------------------------------------------------------------------*/
              if(flow_count == 0)
              {
                break;
              }

              ASSERT(flow_count <= QOSI_MAX_QOS_FLOWS_PER_REQUEST);

              /*-------------------------------------------------------------------
                Copy the parsed TX flows specs into the qos_spec struct to be sent
                to the ioctl
                - The 1st flow spec becomes the REQ flow
                - the last flow spec becomes the MIN flow
                - Other flows in between become AUX flows
              -------------------------------------------------------------------*/
              memscpy(&(req_ptr->qos_spec[qos_spec_cnt].tx.flow_template.req_flow),
                      sizeof(ip_flow_type),
                     &(req_ptr->tx_flows[qos_spec_cnt][0]),
                     sizeof(ip_flow_type));
              req_ptr->qos_spec[qos_spec_cnt].field_mask |= QOS_MASK_TX_FLOW;

              if(flow_count > 1)
              {
                memscpy(&(req_ptr->qos_spec[qos_spec_cnt].tx.flow_template.min_req_flow),
                        sizeof(ip_flow_type),
                       &(req_ptr->tx_flows[qos_spec_cnt][flow_count-1]),
                       sizeof(ip_flow_type));
                req_ptr->qos_spec[qos_spec_cnt].field_mask |= QOS_MASK_TX_MIN_FLOW;

                if(flow_count > 2)
                {
                  req_ptr->qos_spec[qos_spec_cnt].tx.flow_template.aux_flow_list_ptr =
                                                 &(req_ptr->tx_flows[qos_spec_cnt][1]);
                  req_ptr->qos_spec[qos_spec_cnt].tx.flow_template.num_aux_flows =
                                                                        flow_count - 2;
                  req_ptr->qos_spec[qos_spec_cnt].field_mask |= QOS_MASK_TX_AUXILIARY_FLOWS;
                }
              }
              break;
            } //case QMI_PRM_TYPE_TX_QOS_FLOW_REQ

            case QMI_PRM_TYPE_RX_QOS_FLOW_REQ:
            {
              /*-------------------------------------------------------------------
                Parse the Nested Flow spec TLVs
              -------------------------------------------------------------------*/
              errval = qosi_parse_flow_specs(sdu_in,
                                             len_lvl2,
                                             &(req_ptr->rx_flows[qos_spec_cnt][0]),
                                             &(req_ptr->rx_flow_indxs[qos_spec_cnt][0]),
                                             &flow_count);
              if(errval != QMI_ERR_NONE)
              {
                goto send_result;
              }
              /*-------------------------------------------------------------------
                Skip, If no Flow specs were present
              -------------------------------------------------------------------*/
              if(flow_count == 0)
              {
                break;
              }

              ASSERT(flow_count <= QOSI_MAX_QOS_FLOWS_PER_REQUEST);

              /*-------------------------------------------------------------------
                Copy the parsed RX flows specs into the qos_spec struct to be sent
                to the ioctl
                - The 1st flow spec becomes the REQ flow
                - the last flow spec becomes the MIN flow
                - Other flows in between become AUX flows
              -------------------------------------------------------------------*/
              memscpy(&(req_ptr->qos_spec[qos_spec_cnt].rx.flow_template.req_flow),
                      sizeof(ip_flow_type),
                     &(req_ptr->rx_flows[qos_spec_cnt][0]),
                     sizeof(ip_flow_type));
              req_ptr->qos_spec[qos_spec_cnt].field_mask |= QOS_MASK_RX_FLOW;

              if(flow_count > 1)
              {
                memscpy(&(req_ptr->qos_spec[qos_spec_cnt].rx.flow_template.min_req_flow),
                        sizeof(ip_flow_type),
                       &(req_ptr->rx_flows[qos_spec_cnt][flow_count-1]),
                       sizeof(ip_flow_type));
                req_ptr->qos_spec[qos_spec_cnt].field_mask |= QOS_MASK_RX_MIN_FLOW;

                if(flow_count > 2)
                {
                  req_ptr->qos_spec[qos_spec_cnt].rx.flow_template.aux_flow_list_ptr =
                                                &(req_ptr->rx_flows[qos_spec_cnt][1]);
                  req_ptr->qos_spec[qos_spec_cnt].rx.flow_template.num_aux_flows =
                                                                       flow_count - 2;
                  req_ptr->qos_spec[qos_spec_cnt].field_mask |= QOS_MASK_RX_AUXILIARY_FLOWS;
                }
              }
              break;
            } //case QMI_PRM_TYPE_RX_QOS_FLOW_REQ

            case QMI_PRM_TYPE_TX_QOS_FILTER_REQ:
            {
              /*-------------------------------------------------------------------
                Parse the Nested Filter spec TLVs
              -------------------------------------------------------------------*/
              errval = qosi_parse_filter_specs(sdu_in,
                                               len_lvl2,
                                               &(req_ptr->tx_filters[qos_spec_cnt][0]),
                                               &(req_ptr->tx_fltr_indxs[qos_spec_cnt][0]),
                                               &fltr_count);
              if(errval != QMI_ERR_NONE)
              {
                goto send_result;
              }
              if(fltr_count != 0)
              {
                if( IP_V4 == req_ptr->tx_filters[qos_spec_cnt][0].ip_vsn )
                {
                  ip_type = QMI_AF_INET;
                }else
                {
                  ip_type = QMI_AF_INET6;
                }
                /*-----------------------------------------------------------------
                  Populate the QoS spec with TX filter info
                -----------------------------------------------------------------*/
                req_ptr->qos_spec[qos_spec_cnt].tx.fltr_template.num_filters =
                                                                      fltr_count;
                req_ptr->qos_spec[qos_spec_cnt].tx.fltr_template.list_ptr =
                                              req_ptr->tx_filters[qos_spec_cnt];
              }
              break;
            }

            case QMI_PRM_TYPE_RX_QOS_FILTER_REQ:
            {
              /*-------------------------------------------------------------------
                Parse the Nested Filter spec TLVs
              -------------------------------------------------------------------*/
              errval = qosi_parse_filter_specs(sdu_in,
                                               len_lvl2,
                                               &(req_ptr->rx_filters[qos_spec_cnt][0]),
                                               &(req_ptr->rx_fltr_indxs[qos_spec_cnt][0]),
                                               &fltr_count);
              if(errval != QMI_ERR_NONE)
              {
                goto send_result;
              }
              if(fltr_count != 0)
              {
                if( IP_V4 == req_ptr->rx_filters[qos_spec_cnt][0].ip_vsn )
                {
                  ip_type = QMI_AF_INET;
                }else
                {
                  ip_type = QMI_AF_INET6;
                }
                /*-----------------------------------------------------------------
                  Populate the QoS spec with TX filter info
                -----------------------------------------------------------------*/
                req_ptr->qos_spec[qos_spec_cnt].rx.fltr_template.num_filters =
                                                                      fltr_count;
                req_ptr->qos_spec[qos_spec_cnt].rx.fltr_template.list_ptr =
                                               req_ptr->rx_filters[qos_spec_cnt];
              }
              break;
            }

            default:
            {
              LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);

              /* remove the payload for this unrecognized tlv */
              if(len_lvl2 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl2 ))
              {
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
              break;
            } // case default
          } //inner switch
        } // inner while

        qos_spec_cnt++;
        break;
      } //case: QMI_PRM_TYPE_QOS_SPEC

      case QMI_PRM_TYPE_QOS_REQ_OPCODE:
      {
        value = &qos_req_ex.opcode;
        if ( (len_lvl1 != QMI_PRM_TYPE_QOS_REQ_OPCODE_LEN) ||
             (len_lvl1 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl1)) )
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_qos_req_opcode_tlv = TRUE;
        break;
      }

      default:
      {
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);

        /* remove the payload for this unrecognized tlv */
        if(len_lvl1 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl1))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;
      } // case default
    } // outer switch
  } //outer while

  if((qos_sp->num_qos_reqs + qos_spec_cnt) > QOSI_MAX_QOS_REQUESTS)
  {
    errval = QMI_ERR_MAX_QOS_REQUESTS_IN_USE;
    goto send_result;
  }

  ip_type_prev = qos_client_sp->bound_ip_type;
  if( QMI_AF_ANY == qos_client_sp->bound_ip_type )
  {
    qos_client_sp->bound_ip_type = ip_type;
    LOG_MSG_INFO2_2("previous IP type = %d, new IP type = %d",
                    ip_type_prev, ip_type);
    qmi_qosi_sync_clients(inst);
  }
  else if ( ip_type != qos_client_sp->bound_ip_type )
  {
    LOG_MSG_ERROR_2("IP type %d in request, but client already bound to IP type %d",
                    ip_type, qos_client_sp->bound_ip_type);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  qos_sp = qmi_qosi_get_qos_sp_by_cl_sp(qos_client_sp);

  /*-------------------------------------------------------------------------
    Check whether the call is up
    iface_ptr is stored in qos_sp when call comes up
  -------------------------------------------------------------------------*/
  if( NULL != qos_sp)
  {
    ps_iface_ptr = qos_sp->ps_iface_ptr;
  }else
  {
    LOG_MSG_ERROR_0("QMI QoS SP is NULL!");
    ps_iface_ptr = NULL;
  }
  if(ps_iface_ptr == NULL)
  {
    //if the call is not up then return client to previous state
    qos_client_sp->bound_ip_type = ip_type_prev;
    qmi_qosi_sync_clients(inst);

    errval = QMI_ERR_OUT_OF_CALL;
    goto send_result;
  }

  if(!qmi_qosi_verify_client_subs(qos_client_sp, qos_sp->binding.subs_id, TRUE))
  {
    errval = QMI_ERR_INCOMPATIBLE_STATE;
    goto send_result;
  }
  /*-------------------------------------------------------------------------
    set filter id and filter preced to default values
  -------------------------------------------------------------------------*/
  for (i=0;i<qos_spec_cnt;i++)
  {
    for (j=0; j<req_ptr->qos_spec[i].rx.fltr_template.num_filters;j++)
    {
      req_ptr->qos_spec[i].rx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_id
                                                = PS_IFACE_IPFLTR_DEFAULT_ID;
      req_ptr->qos_spec[i].rx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_precedence
                                                = PS_IFACE_IPFLTR_DEFAULT_PRCD;
    }
    for (j=0; j<req_ptr->qos_spec[i].tx.fltr_template.num_filters;j++)
    {
      req_ptr->qos_spec[i].tx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_id
                                                = PS_IFACE_IPFLTR_DEFAULT_ID;
      req_ptr->qos_spec[i].tx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_precedence
                                                = PS_IFACE_IPFLTR_DEFAULT_PRCD;
    }
  }
  qos_req_ex.num_qos_specs = qos_spec_cnt;
  if (!got_qos_req_opcode_tlv)
  {
    qos_req_ex.opcode = PS_IFACE_IOCTL_QOS_REQUEST_OP;
  }
  qos_req_ex.qos_control_flags = 0;
  qos_req_ex.subset_id = 0;
  qos_req_ex.qos_specs_ptr = req_ptr->qos_spec;
  qos_req_ex.flows_ptr = req_ptr->flow_ptr;

  return_val = ps_iface_ioctl(ps_iface_ptr,
                              PS_IFACE_IOCTL_QOS_REQUEST_EX,
                              &qos_req_ex,
                              &ps_errno);

  if(ps_errno == DS_EINVAL)
  {
    if(qos_spec_cnt > 1)
    {
      errval = QMI_ERR_BUNDLING_NOT_SUPPORTED;
      goto send_result;
    }

    bundling_supported = FALSE;
    qos_request.qos_ptr = &(req_ptr->qos_spec[0]);
    qos_request.subset_id = 0;

    return_val = ps_iface_ioctl(ps_iface_ptr,
                                PS_IFACE_IOCTL_QOS_REQUEST,
                                &qos_request,
                                &ps_errno);
  }

  if(return_val == 0) //IOCTL succeeded
  {
    /* go through each qos_spec and save the info in the client */
    for(j = 0; j < qos_req_ex.num_qos_specs; j++)
    {
      /* find empty slot to store flow info */
      for(i=0; i<QOSI_MAX_QOS_REQUESTS; i++)
      {
        if(qosi_flow_info[i].qos_handle == 0)
        {
          break;
        }
      }

      if(i == QOSI_MAX_QOS_REQUESTS)
      {
        DATA_ERR_FATAL("QOS handle slots exhausted");
        errval = QMI_ERR_MAX_QOS_REQUESTS_IN_USE;
        goto send_result;
      }

      /*-----------------------------------------------------------------------
        Save all the needed flow info in the cl_sp
        - flow_ptr
        - qos handle
        - qos_status
        - tx, rx filters  (needed for evt_report_ind later)
      -----------------------------------------------------------------------*/
      qosi_flow_info[i].cl_sp = qos_client_sp;
      if(bundling_supported == TRUE)
      {
        qosi_flow_info[i].flow_ptr = qos_req_ex.flows_ptr[j];
        /* When available, use PS api to get the cookie */
        qosi_flow_info[i].flow_ptr_cookie =
                                qos_req_ex.flows_ptr[j]->flow_private.cookie;
      }
      else
      {
        qosi_flow_info[i].flow_ptr = qos_request.flow_ptr;
        /* When available, use PS api to get the cookie */
        qosi_flow_info[i].flow_ptr_cookie =
                                   qos_request.flow_ptr->flow_private.cookie;
      }
      qosi_flow_info[i].qos_handle = (uint32)ps_handle_mgr_get_handle(PS_HANDLE_MGR_CLIENT_QOS,
                                                              (uint16)i+1);
      if (PS_HANDLE_MGR_INVALID_HANDLE == qosi_flow_info[i].qos_handle)
      {
        LOG_MSG_ERROR_0("Invalid handle returned by handle mgr");
        qosi_flow_info[i].qos_handle = 0;
        errval = QMI_ERR_INTERNAL;
        goto send_result;
      }
      else
      {
        LOG_MSG_INFO2_2("Mapping flow_ptr 0x%x to handle 0x%x", 
                        qosi_flow_info[i].flow_ptr, qosi_flow_info[i].qos_handle)
      }

      qosi_flow_info[i].qos_status = QOSI_STATUS_DEFAULT;

      /* by default flow is enabled and fwd on default is TRUE */
      qosi_flow_info[i].last_rep = QMI_FLOW_ENABLED;
      qosi_flow_info[i].fwd_on_default = TRUE;

      qos_sp->num_qos_reqs++; //num of QoS reqs per qmi instance

      qos_id_list[j] = qosi_flow_info[i].qos_handle;
    }

    qos_id_bytes = sizeof(uint32) * qos_req_ex.num_qos_specs;
    num_bytes_pushed = qos_id_bytes + sizeof(uint8);
    type = QMI_PRM_TYPE_QOS_IDENTIFIER;
    /*-----------------------------------------------------------------------
      Send the QoS Identifier list in the qmi response
    -----------------------------------------------------------------------*/
    if(!((QMI_SVC_PKT_PUSH(&response, qos_id_list, qos_id_bytes)) &&
        (QMI_SVC_PKT_PUSH(&response, &(qos_req_ex.num_qos_specs), sizeof(uint8))) &&
        (QMI_SVC_PKT_PUSH(&response, &num_bytes_pushed, sizeof(uint16))) &&
        (QMI_SVC_PKT_PUSH(&response, &type, sizeof(uint8)))))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
      goto send_result;
    }

    goto send_result;
  }
  else //report errors to client
  {
    if(ps_errno == DS_EQOSUNAWARE)
    {
      /*---------------------------------------------------------------------
        The request failed because the network is QoS unaware
      ---------------------------------------------------------------------*/
      errval = QMI_ERR_NETWORK_QOS_UNAWARE;
      goto send_result;
    }
    else
    {
      errval = QMI_ERR_INCORRECT_FLOW_FILTER;
      /*---------------------------------------------------------------------
        loop through all Qos specs to look through flow/filter err_masks
        to see if any error indicated
        If so, send appropriate error tlvs in response
      ---------------------------------------------------------------------*/
      for(j=0; j < qos_req_ex.num_qos_specs; j++)
      {
        num_bytes_pushed = 0;

        /* TX flow errors */
        errval = qosi_generate_flow_error_tlvs(&(qos_req_ex.qos_specs_ptr[j].tx.flow_template),
                                               &response,
                                               req_ptr->tx_flow_indxs[j],
                                               &cnt);
        if(errval != QMI_ERR_NONE)
        {
          dsm_free_packet(&response);
          goto send_result;
        }

        if(cnt > 0)
        {
          errval = QMI_ERR_INCORRECT_FLOW_FILTER;
          type = QMI_PRM_TYPE_TX_QOS_FLOW_REQ_ERR;

          if(!((QMI_SVC_PKT_PUSH(&response, &cnt, sizeof(uint16))) &&
               (QMI_SVC_PKT_PUSH(&response, &type, sizeof(uint8)))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          num_bytes_pushed += cnt + 3;
        }

        /* RX flow errors */
        errval = qosi_generate_flow_error_tlvs(&(qos_req_ex.qos_specs_ptr[j].rx.flow_template),
                                               &response,
                                               req_ptr->rx_flow_indxs[j],
                                               &cnt);

        if(errval != QMI_ERR_NONE)
        {
          dsm_free_packet(&response);
          goto send_result;
        }

        if(cnt > 0)
        {
          errval = QMI_ERR_INCORRECT_FLOW_FILTER;
          type = QMI_PRM_TYPE_RX_QOS_FLOW_REQ_ERR;

          if(!((QMI_SVC_PKT_PUSH(&response, &cnt, sizeof(uint16))) &&
               (QMI_SVC_PKT_PUSH(&response, &type, sizeof(uint8)))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          num_bytes_pushed += cnt + 3;
        }

        /* TX filter errors */
        errval = qosi_generate_filter_error_tlvs(&(qos_req_ex.qos_specs_ptr[j].tx.fltr_template),
                                                 &response,
                                                 req_ptr->tx_fltr_indxs[j],
                                                 &cnt);
        if(errval != QMI_ERR_NONE)
        {
          dsm_free_packet(&response);
          goto send_result;
        }

        if(cnt > 0)
        {
          errval = QMI_ERR_INCORRECT_FLOW_FILTER;
          type = QMI_PRM_TYPE_TX_QOS_FILTER_REQ_ERR;

          if(!((QMI_SVC_PKT_PUSH(&response, &cnt, sizeof(uint16))) &&
               (QMI_SVC_PKT_PUSH(&response, &type, sizeof(uint8)))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          num_bytes_pushed += cnt + 3;
        }

        /* RX filter errors */
        errval = qosi_generate_filter_error_tlvs(&(qos_req_ex.qos_specs_ptr[j].rx.fltr_template),
                                                 &response,
                                                 req_ptr->rx_fltr_indxs[j],
                                                 &cnt);
        if(errval != QMI_ERR_NONE)
        {
          dsm_free_packet(&response);
          goto send_result;
        }

        if(cnt > 0)
        {
          errval = QMI_ERR_INCORRECT_FLOW_FILTER;
          type = QMI_PRM_TYPE_RX_QOS_FILTER_REQ_ERR;

          if(!((QMI_SVC_PKT_PUSH(&response, &cnt, sizeof(uint16))) &&
               (QMI_SVC_PKT_PUSH(&response, &type, sizeof(uint8)))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          num_bytes_pushed += cnt + 3;
        }

        if(num_bytes_pushed > 0)
        {
          /* append QoS spec index tlv */
          if(FALSE == qmi_svc_put_param_tlv(&response,
                                            QMI_PRM_TYPE_QOS_SPEC_INDEX,
                                            sizeof(uint8),
                                            &(req_ptr->qos_spec_indxs[j])))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
          num_bytes_pushed +=  sizeof(uint8) + 3;

          /* add the qos spec outer TL */
          type = QMI_PRM_TYPE_QOS_SPEC_ERR;
          if(!((QMI_SVC_PKT_PUSH(&response, &num_bytes_pushed, sizeof(uint16))) &&
               (QMI_SVC_PKT_PUSH(&response, &type, sizeof(uint8)))))
          {
            errval = QMI_ERR_NO_MEMORY;
            dsm_free_packet(&response);
            goto send_result;
          }
        }
      } //end of for loop

      /* If no error found till now, return INTERNAL error as ioctl failed
         for some other reason */
      if(errval == QMI_ERR_NONE)
      {
        errval = QMI_ERR_INTERNAL;
      }
    }
  }
send_result:
  PS_SYSTEM_HEAP_MEM_FREE(req_ptr);
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv( &response, result, errval );
  CHECK_RETVAL();
  return response;
} /* qmi_qosi_request_qos() */



/*===========================================================================
  FUNCTION QMI_QOS_RELEASE_QOS()

  DESCRIPTION
    Release a particular QoS flow

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_release_qos
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{ qmi_qosi_client_state_type * qos_client_sp;
  dsm_item_type *        response;
  PACKED struct PACKED_POST
  {
    uint8   num_qos_ids;
    uint32  qos_id_list[QOSI_MAX_QOS_SPECS_PER_REQUEST];
  } v_in_reqd = {0,};

  ps_flow_ioctl_qos_release_ex_type qos_release;
  uint8              type, i, spec_cnt;
  uint16             len = 0;
  uint16             expected_len;
  void *             value;
  int16             ps_errno;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  boolean            got_v_in_reqd;
  int                return_val;
  ps_flow_type       ** flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  got_v_in_reqd = FALSE;

  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(flow_ptr,
                      (sizeof(ps_flow_type *) * QOSI_MAX_QOS_SPECS_PER_REQUEST),
                       ps_flow_type**);
  if (flow_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }
  memset(flow_ptr, 0, sizeof(flow_ptr[0]) * QOSI_MAX_QOS_SPECS_PER_REQUEST);
  memset(&v_in_reqd, 0, sizeof(v_in_reqd));

  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;

  response = NULL;
  errval   = QMI_ERR_NONE;
  ps_errno = 0;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        if(len <= sizeof(v_in_reqd))
        {
          value = (void *) &v_in_reqd;
          got_v_in_reqd = TRUE;
        }
        else
        {
          errval = QMI_ERR_REQUESTED_NUM_UNSUPPORTED;
          goto send_result;
        }
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

     if(!qmi_svc_validate_type_length( type,
                                       len,
                                       expected_len,
                                       FILE_ID_DS_QMI_QOS,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
    /*-----------------------------------------------------------------------
      Speed up the parsing
    -----------------------------------------------------------------------*/
    if(got_v_in_reqd == TRUE)
    {
      break;
    }
  }

  if(!got_v_in_reqd)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }
  if((v_in_reqd.num_qos_ids == 0) ||
     (v_in_reqd.num_qos_ids > QOSI_MAX_QOS_SPECS_PER_REQUEST))
  {
    errval = QMI_ERR_REQUESTED_NUM_UNSUPPORTED;
    goto send_result;
  }

  /* Check that that the qos_id_list length matches up to num_qos_ids */
  if (len != (sizeof(v_in_reqd.num_qos_ids) + sizeof(uint32) * v_in_reqd.num_qos_ids))
  {
    errval = QMI_ERR_MALFORMED_MSG;
    LOG_MSG_INFO2_2("Mismatch between TLV Len %d and num_qos_ids %d",
                    len, v_in_reqd.num_qos_ids);
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Check if the given qos_handle belongs to the client
  -------------------------------------------------------------------------*/
  for(spec_cnt = 0; spec_cnt < v_in_reqd.num_qos_ids; spec_cnt++)
  {
    for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
    {
      if( (qosi_flow_info[i].qos_handle == v_in_reqd.qos_id_list[spec_cnt]) &&
          (qosi_flow_info[i].cl_sp == qos_client_sp)  )
      {
        break;
      }
    }

    if(i == QOSI_MAX_QOS_REQUESTS)
    {
      errval = QMI_ERR_INVALID_QOS_ID;
      goto send_result;
    }

    flow_ptr[spec_cnt] = qosi_flow_info[i].flow_ptr;
  }

  qos_release.num_flows = v_in_reqd.num_qos_ids;
  qos_release.flows_ptr = flow_ptr;

  return_val = ps_flow_ioctl(qos_release.flows_ptr[0],
                             PS_FLOW_IOCTL_QOS_RELEASE_EX,
                             &qos_release,
                             &ps_errno);

  if(ps_errno == DS_EINVAL)
  {
    if(v_in_reqd.num_qos_ids > 1)
    {
      errval = QMI_ERR_BUNDLING_NOT_SUPPORTED;
      goto send_result;
    }

    return_val = ps_flow_ioctl(qos_release.flows_ptr[0],
                               PS_FLOW_IOCTL_QOS_RELEASE,
                               NULL,
                               &ps_errno);
  }

  if(return_val == 0)
  {
    /* CANT reset the qos_handle, flow_ptr at this point, because we need to
       still send the ind when we get FLOW_NULL_EV
       So in case we get another qmi qos req on this flow before we get the
       flow_null_ev then we'll send it to lower layers. */
    goto send_result;
  }
  else
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

send_result:
  PS_SYSTEM_HEAP_MEM_FREE(flow_ptr);
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv( &response, result, errval );
  CHECK_RETVAL();
  return response;
} /* qmi_qosi_release_qos */



/*===========================================================================
  FUNCTION QMI_QOS_SUSPEND_QOS()

  DESCRIPTION
    Suspend a QoS on a particular flow

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_suspend_qos
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_client_state_type * qos_client_sp;
  dsm_item_type *        response;
  PACKED struct PACKED_POST
  {
    uint8   num_qos_ids;
    uint32  qos_id_list[QOSI_MAX_QOS_SPECS_PER_REQUEST];
  } v_in_reqd = {0,};
  ps_flow_ioctl_qos_suspend_ex_type qos_suspend;
  uint8              type;
  uint8              i;
  uint8              spec_cnt;
  uint16             len = 0;
  uint16             expected_len;
  void *             value;
  int16             ps_errno;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  boolean            got_v_in_reqd;
  int                return_val;
  ps_flow_type       ** flow_ptr;
  qmi_qosi_state_type * qos_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in );

  response = NULL;
  got_v_in_reqd = FALSE;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;
  qos_sp = (qmi_qosi_state_type *) sp;
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(flow_ptr,
                     (sizeof(ps_flow_type *) * QOSI_MAX_QOS_SPECS_PER_REQUEST),
                     ps_flow_type**);
  if (flow_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }
  memset(flow_ptr, 0, sizeof(flow_ptr[0]) * QOSI_MAX_QOS_SPECS_PER_REQUEST);
  memset(&v_in_reqd, 0, sizeof(v_in_reqd));
  errval = QMI_ERR_NONE;
  ps_errno = 0;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }
    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        if(len <= sizeof(v_in_reqd))
        {
          value = (void *) &v_in_reqd;
          got_v_in_reqd = TRUE;
        }
        else
        {
          errval = QMI_ERR_REQUESTED_NUM_UNSUPPORTED;
          goto send_result;
        }
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                       len,
                                       expected_len,
                                       FILE_ID_DS_QMI_QOS,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
    /*-----------------------------------------------------------------------
      Speed up the parsing
    -----------------------------------------------------------------------*/
    if(got_v_in_reqd == TRUE)
    {
      break;
    }
  }

  if(!got_v_in_reqd)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if((v_in_reqd.num_qos_ids == 0) ||
     (v_in_reqd.num_qos_ids > QOSI_MAX_QOS_SPECS_PER_REQUEST))
  {
    errval = QMI_ERR_REQUESTED_NUM_UNSUPPORTED;
    goto send_result;
  }

  /* Check that that the qos_id_list length matches up to num_qos_ids */
  if (len != (sizeof(v_in_reqd.num_qos_ids) + sizeof(uint32) * v_in_reqd.num_qos_ids))
  {
    errval = QMI_ERR_MALFORMED_MSG;
    LOG_MSG_INFO2_2("Mismatch between TLV Len %d and num_qos_ids %d",
                    len, v_in_reqd.num_qos_ids);
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Check if the given qos_handle belongs to the client
  -------------------------------------------------------------------------*/
  for(spec_cnt = 0; spec_cnt < v_in_reqd.num_qos_ids; spec_cnt++)
  {
    for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
    {
      if( (qosi_flow_info[i].qos_handle == v_in_reqd.qos_id_list[spec_cnt]) &&
          (qosi_flow_info[i].cl_sp == qos_client_sp) )
      {
        break;
      }
    }

    if(i == QOSI_MAX_QOS_REQUESTS)
    {
      /* Check if the flow is network initiated if not found in UE init cache */
      for(i=0; i < QOSI_MAX_NETWORK_INIT_FLOW_NUM; i++)
      {
        if(nw_init_qosi_flow_info[i].qos_handle == v_in_reqd.qos_id_list[spec_cnt]) 
        {
          break;
        }
      }
      /* Check that flow is associated with PDN/port on which request is sent */
      if ((i == QOSI_MAX_NETWORK_INIT_FLOW_NUM) || (qos_sp->ps_iface_ptr == NULL) || 
          (PS_FLOW_GET_IFACE(nw_init_qosi_flow_info[i].flow_ptr) != qos_sp->ps_iface_ptr))
      {
        errval = QMI_ERR_INVALID_QOS_ID;
        goto send_result;
      }
      flow_ptr[spec_cnt] = nw_init_qosi_flow_info[i].flow_ptr;    
    }
    else
    {
      flow_ptr[spec_cnt] = qosi_flow_info[i].flow_ptr;
    }
  }

  qos_suspend.num_flows = v_in_reqd.num_qos_ids;
  qos_suspend.flows_ptr = flow_ptr;

  return_val =  ps_flow_ioctl(qos_suspend.flows_ptr[0],
                              PS_FLOW_IOCTL_QOS_SUSPEND_EX,
                              &qos_suspend,
                              &ps_errno);

  if(ps_errno == DS_EINVAL)
  {
    if(v_in_reqd.num_qos_ids > 1)
    {
      errval = QMI_ERR_BUNDLING_NOT_SUPPORTED;
      goto send_result;
    }

    return_val = ps_flow_ioctl(qos_suspend.flows_ptr[0],
                               PS_FLOW_IOCTL_QOS_SUSPEND,
                               NULL,
                               &ps_errno);
  }

  if(return_val == 0)
  {
    goto send_result;
  }
  else
  {
    if(ps_errno == DS_EINVAL)
    {
      errval = QMI_ERR_OP_NETWORK_UNSUPPORTED;
    }
    else
    {
      errval = QMI_ERR_INTERNAL;
    }
    goto send_result;
  }

send_result:
  PS_SYSTEM_HEAP_MEM_FREE(flow_ptr);
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv( &response, result, errval );
  CHECK_RETVAL();
  return response;
} /* qmi_qosi_suspend_qos() */


/*===========================================================================
  FUNCTION QMI_QOSI_RESUME_QOS()

  DESCRIPTION
    Resume QoS on a particular flow

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_resume_qos
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_client_state_type * qos_client_sp;
  dsm_item_type *        response;
  PACKED struct PACKED_POST
  {
    uint8   num_qos_ids;
    uint32  qos_id_list[QOSI_MAX_QOS_SPECS_PER_REQUEST];
  } v_in_reqd = {0,};
  ps_flow_ioctl_qos_resume_ex_type qos_resume;
  uint8                  type;
  uint8                  i;
  uint8                  spec_cnt;
  uint16                 len = 0;
  uint16                 expected_len;
  void *                 value;
  int16                 ps_errno;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  boolean            got_v_in_reqd;
  int                return_val;
  ps_flow_type       ** flow_ptr;
  qmi_qosi_state_type* qos_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  got_v_in_reqd = FALSE;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;
  qos_sp = (qmi_qosi_state_type *) sp;
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(flow_ptr,
                        (sizeof(ps_flow_type *) * QOSI_MAX_QOS_SPECS_PER_REQUEST),
                         ps_flow_type**);
  if (flow_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }
  memset(flow_ptr, 0, sizeof(flow_ptr[0]) * QOSI_MAX_QOS_SPECS_PER_REQUEST);
  memset(&v_in_reqd, 0, sizeof(v_in_reqd));
  errval = QMI_ERR_NONE;
  ps_errno = 0;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }
    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        if(len <= sizeof(v_in_reqd))
        {
          value = (void *) &v_in_reqd;
          got_v_in_reqd = TRUE;
        }
        else
        {
          errval = QMI_ERR_REQUESTED_NUM_UNSUPPORTED;
          goto send_result;
        }
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_QOS,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
    /*-----------------------------------------------------------------------
      Speed up the parsing
    -----------------------------------------------------------------------*/
    if(got_v_in_reqd == TRUE)
    {
      break;
    }
  }

  if(!got_v_in_reqd)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }
  if((v_in_reqd.num_qos_ids == 0) ||
     (v_in_reqd.num_qos_ids > QOSI_MAX_QOS_SPECS_PER_REQUEST))
  {
    errval = QMI_ERR_REQUESTED_NUM_UNSUPPORTED;
    goto send_result;
  }

  /* Check that that the qos_id_list length matches up to num_qos_ids */
  if (len != (sizeof(v_in_reqd.num_qos_ids) + sizeof(uint32) * v_in_reqd.num_qos_ids))
  {
    errval = QMI_ERR_MALFORMED_MSG;
    LOG_MSG_INFO2_2("Mismatch between TLV Len %d and num_qos_ids %d",
                    len, v_in_reqd.num_qos_ids);
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Check if the given qos_handle belongs to the client
  -------------------------------------------------------------------------*/
  for(spec_cnt = 0; spec_cnt < v_in_reqd.num_qos_ids; spec_cnt++)
  {
    for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
    {
      if( (qosi_flow_info[i].qos_handle == v_in_reqd.qos_id_list[spec_cnt]) &&
          (qosi_flow_info[i].cl_sp == qos_client_sp) )
      {
        break;
      }
    }

    if(i == QOSI_MAX_QOS_REQUESTS)
    {
      /* Check if the flow is network initiated if not found in UE init cache */
      for(i=0; i < QOSI_MAX_NETWORK_INIT_FLOW_NUM; i++)
      {
        if(nw_init_qosi_flow_info[i].qos_handle == v_in_reqd.qos_id_list[spec_cnt]) 
        {
          break;
        }
      }
      /* Check that flow is associated with PDN/port on which request is sent */
      if ((i == QOSI_MAX_NETWORK_INIT_FLOW_NUM) || (qos_sp->ps_iface_ptr == NULL) || 
          (PS_FLOW_GET_IFACE(nw_init_qosi_flow_info[i].flow_ptr) != qos_sp->ps_iface_ptr))
      {
        errval = QMI_ERR_INVALID_QOS_ID;
        goto send_result;
      }
      flow_ptr[spec_cnt] = nw_init_qosi_flow_info[i].flow_ptr;
    }
    else
    {
      flow_ptr[spec_cnt] = qosi_flow_info[i].flow_ptr;
    }
  }

  qos_resume.num_flows = v_in_reqd.num_qos_ids;
  qos_resume.flows_ptr = flow_ptr;

  return_val = ps_flow_ioctl(qos_resume.flows_ptr[0],
                             PS_FLOW_IOCTL_QOS_RESUME_EX,
                             &qos_resume,
                             &ps_errno);

  if(ps_errno == DS_EINVAL)
  {
    if(v_in_reqd.num_qos_ids > 1)
    {
      errval = QMI_ERR_BUNDLING_NOT_SUPPORTED;
      goto send_result;
    }

    return_val =  ps_flow_ioctl(qos_resume.flows_ptr[0],
                                PS_FLOW_IOCTL_QOS_RESUME,
                                NULL,
                                &ps_errno);
  }


  if(return_val == 0)
  {
    goto send_result;
  }
  else
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

send_result:
  PS_SYSTEM_HEAP_MEM_FREE(flow_ptr);
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv( &response, result, errval );
  CHECK_RETVAL();
  return response;
} /* qmi_qosi_resume_qos() */

/*===========================================================================
  FUNCTION QMI_QOS_MODIFY_QOS()

  DESCRIPTION
    Modify QoS on a particular flow

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_modify_qos
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_client_state_type *     qos_client_sp;
  qmi_qosi_state_type *            qos_sp;
  dsm_item_type *                  response;
  ps_flow_ioctl_qos_modify_type    qos_modify;
  ps_flow_ioctl_primary_qos_modify_type prim_qos_modify;
  qos_modify_info_type *    modify_ptr;
  uint8                  qos_spec_cnt;
  uint8                  type;
  uint8                  i;
  uint8                  j;
  uint16                 len_lvl1 = 0;
  uint16                 len_lvl2 = 0;
  uint16                 num_bytes_lvl2, expected_len;
  void *                 value;
  uint16                 cnt;
  uint8                  flow_count;
  uint8                  fltr_count;
  int16                 ps_errno;
  qmi_error_e_type       errval;
  qmi_result_e_type      result;
  boolean                retval;
  int                    return_val;
  boolean                got_v_in_reqd_qos_id;
  qos_spec_field_mask_type  qos_spec_field_mask;
  boolean                v_opt_req_primary;
  boolean                got_v_opt_req_prim;
  primary_qos_spec_type  primary_qos_spec;
  boolean                primary;
  ip_flow_spec_type      temp_flow_spec;
  ps_flow_type *         def_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in );

  response = NULL;
  num_bytes_lvl2 = 0;
  qos_spec_cnt = 0;
  cnt = 0;
  ps_errno = 0;
  flow_count = 0;
  fltr_count = 0;

  qos_sp = (qmi_qosi_state_type *) sp;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;

  PS_SYSTEM_HEAP_MEM_ALLOC(modify_ptr,
                           sizeof(qos_modify_info_type),
                           qos_modify_info_type*);
  if(modify_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  memset(modify_ptr, 0, sizeof(qos_modify_info_type));
  memset(&primary_qos_spec, 0, sizeof(primary_qos_spec));
  memset(&qos_modify, 0, sizeof(qos_modify));
  memset(&prim_qos_modify, 0, sizeof(prim_qos_modify));

  LOG_MSG_INFO2_1("Received QoS Modify Request from client %d",
                  qos_client_sp->common.clid);

  response = NULL;
  errval   = QMI_ERR_NONE;
  got_v_in_reqd_qos_id = FALSE;
  v_opt_req_primary = FALSE;
  got_v_opt_req_prim = FALSE;
  primary = FALSE;

  while(*sdu_in)
  {
    if(qos_spec_cnt == QOSI_MAX_QOS_SPECS_PER_REQUEST)
    {
      break;
    }

    if( !qmi_svc_get_tl(sdu_in, &type, &len_lvl1) )
    {
      continue;
    }

    value = NULL;
    switch(type)
    {
      case QMI_PRM_TYPE_QOS_SPEC:
      {
        while (*sdu_in)
        {
          if(num_bytes_lvl2 == len_lvl1)
          {
            num_bytes_lvl2 = 0;
            break;
          }

          if( !qmi_svc_get_tl(sdu_in, &type, &len_lvl2) )
          {
            continue;
          }
          if(!(len_lvl2 < len_lvl1))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          /* Check for overflow */
          if((MAX_UINT16 - len_lvl2 - 3) < num_bytes_lvl2)
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          num_bytes_lvl2 += len_lvl2 + 3;

          if(num_bytes_lvl2 > len_lvl1)
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          expected_len = 0;
          value = NULL;
          switch (type)
          {
            case QMI_PRM_TYPE_QOS_IDENTIFIER:
            {
              expected_len = sizeof(uint32);
              value = (void*) &(modify_ptr->qos_id[qos_spec_cnt]);
              if(!qmi_svc_validate_type_length( type,
                                                len_lvl2,
                                                expected_len,
                                                FILE_ID_DS_QMI_QOS,__LINE__) )
              {
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
              if(len_lvl2 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl2))
              {
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
              got_v_in_reqd_qos_id = TRUE;
              break;
            }

            case QMI_PRM_TYPE_QOS_SPEC_INDEX:
            {
              expected_len = sizeof(uint8);
              value =
                 (void*) &(modify_ptr->qos_req.qos_spec_indxs[qos_spec_cnt]);
              if(!qmi_svc_validate_type_length( type,
								   len_lvl2,
								   expected_len,
								   FILE_ID_DS_QMI_QOS,__LINE__))			  
              {
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
              if(len_lvl2 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl2 ))
              {
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
              break;
            }

            case QMI_PRM_TYPE_TX_QOS_FLOW_REQ:
            {
              /*-------------------------------------------------------------
                Parse the Nested Flow spec TLVs
              -------------------------------------------------------------*/
              errval =
                  qosi_parse_flow_specs(
                       sdu_in,
                       len_lvl2,
                       &(modify_ptr->qos_req.tx_flows[qos_spec_cnt][0]),
                       &(modify_ptr->qos_req.tx_flow_indxs[qos_spec_cnt][0]),
                       &flow_count);
              if(errval != QMI_ERR_NONE)
              {
                goto send_result;
              }
              /*-------------------------------------------------------------
                Skip, If no Flow specs were present
              -------------------------------------------------------------*/
              if(flow_count == 0)
              {
                break;
              }

              ASSERT(flow_count <= QOSI_MAX_QOS_FLOWS_PER_REQUEST);

              /*-------------------------------------------------------------
                Copy the parsed TX flows specs into the qos_spec struct to be
                sent to the ioctl
                - The 1st flow spec becomes the REQ flow
                - the last flow spec becomes the MIN flow
                - Other flows in between become AUX flows
              -------------------------------------------------------------*/
              memscpy(&(modify_ptr->qos_req.qos_spec[qos_spec_cnt].tx.flow_template.req_flow),
                      sizeof(ip_flow_type),
                     &(modify_ptr->qos_req.tx_flows[qos_spec_cnt][0]),
                     sizeof(ip_flow_type));
              modify_ptr->qos_req.qos_spec[qos_spec_cnt].field_mask |=
                                                            QOS_MASK_TX_FLOW;

              if(flow_count > 1)
              {
                memscpy(&(modify_ptr->qos_req.qos_spec[qos_spec_cnt].tx.flow_template.min_req_flow),
                        sizeof(ip_flow_type),
                       &(modify_ptr->qos_req.tx_flows[qos_spec_cnt][flow_count-1]),
                       sizeof(ip_flow_type));
                modify_ptr->qos_req.qos_spec[qos_spec_cnt].field_mask |=
                                                        QOS_MASK_TX_MIN_FLOW;

                if(flow_count > 2)
                {
                  modify_ptr->qos_req.qos_spec[qos_spec_cnt].tx.flow_template.aux_flow_list_ptr =
                            &(modify_ptr->qos_req.tx_flows[qos_spec_cnt][1]);
                  modify_ptr->qos_req.qos_spec[qos_spec_cnt].tx.flow_template.num_aux_flows =
                                                              flow_count - 2;
                  modify_ptr->qos_req.qos_spec[qos_spec_cnt].field_mask |=
                                                 QOS_MASK_TX_AUXILIARY_FLOWS;
                }
              }
              break;
            } //case QMI_PRM_TYPE_TX_QOS_FLOW_REQ

            case QMI_PRM_TYPE_RX_QOS_FLOW_REQ:
            {
              /*-------------------------------------------------------------
                Parse the Nested Flow spec TLVs
              -------------------------------------------------------------*/
              errval =
                  qosi_parse_flow_specs(
                       sdu_in,
                       len_lvl2,
                       &(modify_ptr->qos_req.rx_flows[qos_spec_cnt][0]),
                       &(modify_ptr->qos_req.rx_flow_indxs[qos_spec_cnt][0]),
                       &flow_count);
              if(errval != QMI_ERR_NONE)
              {
                goto send_result;
              }
              /*-------------------------------------------------------------
                Skip, If no Flow specs were present
              -------------------------------------------------------------*/
              if(flow_count == 0)
              {
                break;
              }

              ASSERT(flow_count <= QOSI_MAX_QOS_FLOWS_PER_REQUEST);

              /*-------------------------------------------------------------
                Copy the parsed RX flows specs into the qos_spec struct to be
                sent to the ioctl
                - The 1st flow spec becomes the REQ flow
                - the last flow spec becomes the MIN flow
                - Other flows in between become AUX flows
              -------------------------------------------------------------*/
              memscpy(&(modify_ptr->qos_req.qos_spec[qos_spec_cnt].rx.flow_template.req_flow),
                     sizeof(ip_flow_type),
                     &(modify_ptr->qos_req.rx_flows[qos_spec_cnt][0]),
                     sizeof(ip_flow_type));
              modify_ptr->qos_req.qos_spec[qos_spec_cnt].field_mask |=
                                                            QOS_MASK_RX_FLOW;

              if(flow_count > 1)
              {
                memscpy(&(modify_ptr->qos_req.qos_spec[qos_spec_cnt].rx.flow_template.min_req_flow),
                        sizeof(ip_flow_type),
                       &(modify_ptr->qos_req.rx_flows[qos_spec_cnt][flow_count-1]),
                       sizeof(ip_flow_type));
                modify_ptr->qos_req.qos_spec[qos_spec_cnt].field_mask |=
                                                        QOS_MASK_RX_MIN_FLOW;

                if(flow_count > 2)
                {
                  modify_ptr->qos_req.qos_spec[qos_spec_cnt].rx.flow_template.aux_flow_list_ptr =
                            &(modify_ptr->qos_req.rx_flows[qos_spec_cnt][1]);
                  modify_ptr->qos_req.qos_spec[qos_spec_cnt].rx.flow_template.num_aux_flows =
                                                              flow_count - 2;
                  modify_ptr->qos_req.qos_spec[qos_spec_cnt].field_mask |=
                                                 QOS_MASK_RX_AUXILIARY_FLOWS;
                }
              }
              break;
            } //case QMI_PRM_TYPE_RX_QOS_FLOW_REQ

            case QMI_PRM_TYPE_TX_QOS_FILTER_REQ:
            {
              /*-------------------------------------------------------------
                Parse the Nested Filter spec TLVs
              -------------------------------------------------------------*/
              errval =
                  qosi_parse_filter_specs(
                       sdu_in,
                       len_lvl2,
                       &(modify_ptr->qos_req.tx_filters[qos_spec_cnt][0]),
                       &(modify_ptr->qos_req.tx_fltr_indxs[qos_spec_cnt][0]),
                       &fltr_count);
              if(errval != QMI_ERR_NONE)
              {
                goto send_result;
              }
              if(fltr_count != 0)
              {
                /*-----------------------------------------------------------
                  Populate the QoS spec with TX filter info
                -----------------------------------------------------------*/
                modify_ptr->qos_req.qos_spec[qos_spec_cnt].tx.fltr_template.num_filters =
                                                                  fltr_count;
                modify_ptr->qos_req.qos_spec[qos_spec_cnt].tx.fltr_template.list_ptr =
                                modify_ptr->qos_req.tx_filters[qos_spec_cnt];
              }
              break;
            }

            case QMI_PRM_TYPE_RX_QOS_FILTER_REQ:
            {
              /*-------------------------------------------------------------
                Parse the Nested Filter spec TLVs
              -------------------------------------------------------------*/
              errval =
                  qosi_parse_filter_specs(
                       sdu_in,
                       len_lvl2,
                       &(modify_ptr->qos_req.rx_filters[qos_spec_cnt][0]),
                       &(modify_ptr->qos_req.rx_fltr_indxs[qos_spec_cnt][0]),
                       &fltr_count);
              if(errval != QMI_ERR_NONE)
              {
                goto send_result;
              }
              if(fltr_count != 0)
              {
                /*-----------------------------------------------------------
                  Populate the QoS spec with TX filter info
                -----------------------------------------------------------*/
                modify_ptr->qos_req.qos_spec[qos_spec_cnt].rx.fltr_template.num_filters =
                                                                  fltr_count;
                modify_ptr->qos_req.qos_spec[qos_spec_cnt].rx.fltr_template.list_ptr =
                                modify_ptr->qos_req.rx_filters[qos_spec_cnt];
              }
              break;
            }

            default:
            {
              LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);

              /* remove the payload for this unrecognized tlv */
              if(len_lvl2 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl2 ))
              {
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
              break;
            } // case default
          } //inner switch
        } // inner while

        if (!got_v_in_reqd_qos_id)
        {
          errval = QMI_ERR_MISSING_ARG;
          goto send_result;
        }

        qos_spec_cnt++;
        break;
      } //case: QMI_PRM_TYPE_QOS_SPEC

      case QMI_PRM_TYPE_PRIM_MODIFY_QOS_REQ:
      {
        expected_len = sizeof(v_opt_req_primary);
        value = (void *) &v_opt_req_primary;
        got_v_opt_req_prim = TRUE;

        if(expected_len != len_lvl1)
        {
          LOG_MSG_INFO2_2("Invalid TLV len (%d) for type (%d)",
                          len_lvl1, type);
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }

        /*-------------------------------------------------------------------
          if value is NULL, skip past the current TLV
        -------------------------------------------------------------------*/
        if(len_lvl1 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl1))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;
      }

      default:
      {
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);

        /* remove the payload for this unrecognized tlv */
        if(len_lvl1 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl1))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;
      } // case default
    } // outer switch
  } //outer while

  if((qos_sp->num_qos_reqs + qos_spec_cnt) > QOSI_MAX_QOS_REQUESTS)
  {
    errval = QMI_ERR_MAX_QOS_REQUESTS_IN_USE;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Check whether the call is up
    iface_ptr is stored in qos_sp when call comes up
  -------------------------------------------------------------------------*/
  if(qos_sp->ps_iface_ptr == NULL)
  {
    errval = QMI_ERR_OUT_OF_CALL;
    goto send_result;
  }

  if(!qmi_qosi_verify_client_subs(qos_client_sp, qos_sp->binding.subs_id, TRUE))
  {
    errval = QMI_ERR_INCOMPATIBLE_STATE;
    goto send_result;
  }

  if(qos_spec_cnt > 1)
  {
    errval = QMI_ERR_BUNDLING_NOT_SUPPORTED;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    set filter id and filter preced to default values
  -------------------------------------------------------------------------*/
  for (i=0;i<qos_spec_cnt;i++)
  {
    for (j=0; j<modify_ptr->qos_req.qos_spec[i].rx.fltr_template.num_filters;j++)
    {
      modify_ptr->qos_req.qos_spec[i].rx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_id
                                                = PS_IFACE_IPFLTR_DEFAULT_ID;
      modify_ptr->qos_req.qos_spec[i].rx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_precedence
                                                = PS_IFACE_IPFLTR_DEFAULT_PRCD;
    }
    for (j=0; j<modify_ptr->qos_req.qos_spec[i].tx.fltr_template.num_filters;j++)
    {
      modify_ptr->qos_req.qos_spec[i].tx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_id
                                                = PS_IFACE_IPFLTR_DEFAULT_ID;
      modify_ptr->qos_req.qos_spec[i].tx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_precedence
                                                = PS_IFACE_IPFLTR_DEFAULT_PRCD;
    }
  }

  /*-------------------------------------------------------------------------
    Check if the request is primary modify and call the ioctl accordingly
  -------------------------------------------------------------------------*/
  if ( (got_v_opt_req_prim == TRUE) && (v_opt_req_primary == TRUE) )
  {
    if (modify_ptr->qos_id[0] != 0)
    {
      errval = QMI_ERR_INVALID_QOS_ID;
      goto send_result;
    }

    def_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(qos_sp->ps_iface_ptr);
    ASSERT (def_flow_ptr);

    primary_qos_spec.tx_flow_template =
                            modify_ptr->qos_req.qos_spec[0].tx.flow_template;
    primary_qos_spec.rx_flow_template =
                            modify_ptr->qos_req.qos_spec[0].rx.flow_template;
    primary_qos_spec.field_mask = modify_ptr->qos_req.qos_spec[0].field_mask;

    prim_qos_modify.primary_qos_spec_ptr = &primary_qos_spec;
    primary = TRUE;
    return_val = ps_flow_ioctl( def_flow_ptr,
                                PS_FLOW_IOCTL_PRIMARY_QOS_MODIFY,
                                &prim_qos_modify,
                                &ps_errno );
  }
  else
  {
    /*-------------------------------------------------------------------------
      Check if the given qos_handle belongs to the client
    -------------------------------------------------------------------------*/
    for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
    {
      if( (qosi_flow_info[i].qos_handle == modify_ptr->qos_id[0]) &&
          (qosi_flow_info[i].cl_sp == qos_client_sp) )
      {
        break;
      }
    }
    if(i == QOSI_MAX_QOS_REQUESTS)
    {
      errval = QMI_ERR_INVALID_QOS_ID;
      goto send_result;
    }

    /*-------------------------------------------------------------------------
      If Any TX filters present, check to see if the flow's current qos spec
      filed mask has any TX flows ( => it has TX filters) hence the filters are
      getting modified and so set the TX_FLTR_MODIFY_MASK
      If the qos filed mask has no TX flows, then filter is added 1st time,
      so don't set MODIFY MASK
    -------------------------------------------------------------------------*/
    qos_spec_field_mask =
      PS_FLOW_GET_QOS_FIELD_MASK(qosi_flow_info[i].flow_ptr);

    if(modify_ptr->qos_req.qos_spec[0].tx.fltr_template.num_filters > 0)
    {
      if(qos_spec_field_mask & QOS_MASK_TX_FLOW)
      {
        modify_ptr->qos_req.qos_spec[0].field_mask |= QOS_MODIFY_MASK_TX_FLTR_MODIFY;
      }
    }

    if(modify_ptr->qos_req.qos_spec[0].rx.fltr_template.num_filters > 0)
    {
      if(qos_spec_field_mask & QOS_MASK_RX_FLOW)
      {
        modify_ptr->qos_req.qos_spec[0].field_mask |= QOS_MODIFY_MASK_RX_FLTR_MODIFY;
      }
    }

    qos_modify.qos_ptr = &(modify_ptr->qos_req.qos_spec[0]);
    qos_modify.subset_id = 0;
    return_val = ps_flow_ioctl(qosi_flow_info[i].flow_ptr,
                               PS_FLOW_IOCTL_QOS_MODIFY,
                               &qos_modify,
                               &ps_errno);
  }

  if(return_val != -1) //IOCTL succeeded
  {
    goto send_result;
  }

  /* report errors */
  if(ps_errno == DS_EQOSUNAWARE)
  {
    errval = QMI_ERR_NETWORK_QOS_UNAWARE;
    goto send_result;
  }

  if(ps_errno == DS_EOPNOTSUPP)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
    goto send_result;
  }

  errval = QMI_ERR_INCORRECT_FLOW_FILTER;

  /*-------------------------------------------------------------------------
    loop through all Qos specs to look through flow/filter err_masks to see
    if any error indicated
    If so, send appropriate error tlvs in response
  -------------------------------------------------------------------------*/
  j = 0; // bundling not supported
//  for(j=0; j < qos_modify_ex.num_qos_specs; j++)
//  {
//    num_bytes_pushed = 0;

    /* TX flow errors */
    if (primary == TRUE)
    {
      temp_flow_spec = prim_qos_modify.primary_qos_spec_ptr->tx_flow_template;
    }
    else
    {
      temp_flow_spec = qos_modify.qos_ptr->tx.flow_template;
    }
    errval = qosi_generate_flow_error_tlvs(
                                        &temp_flow_spec,
                                        &response,
                                        modify_ptr->qos_req.tx_flow_indxs[j],
                                        &cnt);
    if(errval != QMI_ERR_NONE)
    {
      dsm_free_packet(&response);
      goto send_result;
    }

    if(cnt > 0)
    {
      errval = QMI_ERR_INCORRECT_FLOW_FILTER;
      type = QMI_PRM_TYPE_TX_QOS_FLOW_REQ_ERR;

      if(!((QMI_SVC_PKT_PUSH(&response, &cnt, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(&response, &type, sizeof(uint8)))))
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
        goto send_result;
      }
//      num_bytes_pushed += cnt + 3;
    }

    /* RX flow errors */
    if (primary == TRUE)
    {
      temp_flow_spec = prim_qos_modify.primary_qos_spec_ptr->rx_flow_template;
    }
    else
    {
      temp_flow_spec = qos_modify.qos_ptr->rx.flow_template;
    }
    errval = qosi_generate_flow_error_tlvs(
                                        &temp_flow_spec,
                                        &response,
                                        modify_ptr->qos_req.rx_flow_indxs[j],
                                        &cnt);

    if(errval != QMI_ERR_NONE)
    {
      dsm_free_packet(&response);
      goto send_result;
    }

    if(cnt > 0)
    {
      errval = QMI_ERR_INCORRECT_FLOW_FILTER;
      type = QMI_PRM_TYPE_RX_QOS_FLOW_REQ_ERR;

      if(!((QMI_SVC_PKT_PUSH(&response, &cnt, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(&response, &type, sizeof(uint8)))))
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
        goto send_result;
      }
//      num_bytes_pushed += cnt + 3;
    }

    if (primary != TRUE)
    {

      /* TX filter errors */
      errval = qosi_generate_filter_error_tlvs(
                                     &(qos_modify.qos_ptr->tx.fltr_template),
                                     &response,
                                     modify_ptr->qos_req.tx_fltr_indxs[j],
                                     &cnt);
      if(errval != QMI_ERR_NONE)
      {
        dsm_free_packet(&response);
        goto send_result;
      }

      if(cnt > 0)
      {
        errval = QMI_ERR_INCORRECT_FLOW_FILTER;
        type = QMI_PRM_TYPE_TX_QOS_FILTER_REQ_ERR;

        if(!((QMI_SVC_PKT_PUSH(&response, &cnt, sizeof(uint16))) &&
             (QMI_SVC_PKT_PUSH(&response, &type, sizeof(uint8)))))
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
          goto send_result;
        }
//      num_bytes_pushed += cnt + 3;
      }

      /* RX filter errors */
      errval = qosi_generate_filter_error_tlvs(
                                     &(qos_modify.qos_ptr->rx.fltr_template),
                                     &response,
                                     modify_ptr->qos_req.rx_fltr_indxs[j],
                                     &cnt);
      if(errval != QMI_ERR_NONE)
      {
        dsm_free_packet(&response);
        goto send_result;
      }

      if(cnt > 0)
      {
        errval = QMI_ERR_INCORRECT_FLOW_FILTER;
        type = QMI_PRM_TYPE_RX_QOS_FILTER_REQ_ERR;

        if(!((QMI_SVC_PKT_PUSH(&response, &cnt, sizeof(uint16))) &&
             (QMI_SVC_PKT_PUSH(&response, &type, sizeof(uint8)))))
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
          goto send_result;
        }
//      num_bytes_pushed += cnt + 3;
      }

//    if(num_bytes_pushed > 0)
//    {
//      do similar to qos_request
//    }

    }
//  } //end of for loop. Commenting as bundling is not supported for QoS Modify

  /* If no error found till now, return INTERNAL error as ioctl failed
     for some other reason */
  if(errval == QMI_ERR_NONE)
  {
    errval = QMI_ERR_INTERNAL;
  }

send_result:
  PS_SYSTEM_HEAP_MEM_FREE(modify_ptr);
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv( &response, result, errval );
  CHECK_RETVAL();
  return response;
} /* qmi_qosi_modify_qos() */

/*===========================================================================
  FUNCTION QMI_QOS_GET_GRANTED_QOS()

  DESCRIPTION
    Get the granted QoS flow spec for a particular QoS identifier

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_get_granted_qos
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_state_type        *  qos_sp;
  qmi_qosi_client_state_type *  qos_client_sp;
  dsm_item_type *               response;
  PACKED struct PACKED_POST
  {
    uint32 qos_handle;
  } v_in_reqd;
  boolean            v_opt_req_primary;
  uint16             len = 0;
  uint16             expected_len;
  uint8              type;
  uint8              i;
  void *             value;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  boolean            got_v_in_reqd;
  boolean            got_v_opt_req_prim;
  uint16             num_bytes_pushed;
  ps_flow_type *     flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  got_v_in_reqd = FALSE;
  got_v_opt_req_prim = FALSE;
  errval   = QMI_ERR_NONE;
  memset(&v_in_reqd, 0, sizeof(v_in_reqd));
  v_opt_req_primary = FALSE;
  qos_sp = (qmi_qosi_state_type *) sp;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(v_in_reqd);
        value = (void *) &v_in_reqd;
        got_v_in_reqd = TRUE;
        break;

      case QMI_PRM_TYPE_PRIM_GRANTED_QOS_REQ:
        expected_len = sizeof(v_opt_req_primary);
        value = (void *) &v_opt_req_primary;
        got_v_opt_req_prim = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_QOS,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if(!got_v_in_reqd)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if ( (got_v_opt_req_prim) && (v_opt_req_primary == TRUE) )
  {
    if (v_in_reqd.qos_handle != 0)
    {
      errval = QMI_ERR_INVALID_QOS_ID;
      goto send_result;
    }

    if (qos_sp->ps_iface_ptr == NULL)
    {
      errval = QMI_ERR_OUT_OF_CALL;
      goto send_result;
    }

    if(!qmi_qosi_verify_client_subs(qos_client_sp, qos_sp->binding.subs_id, TRUE))
    {
      errval = QMI_ERR_INCOMPATIBLE_STATE;
      goto send_result;
    }

    /* Primary granted qos */
    flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(qos_sp->ps_iface_ptr);
    ASSERT (flow_ptr);
  }
  else
  {
    /* Secondary granted qos */

    if (qos_sp->ps_iface_ptr == NULL)
    {
      errval = QMI_ERR_OUT_OF_CALL;
      goto send_result;
    }

    if(!qmi_qosi_verify_client_subs(qos_client_sp, qos_sp->binding.subs_id, TRUE))
    {
      errval = QMI_ERR_INCOMPATIBLE_STATE;
      goto send_result;
    }

  /*-------------------------------------------------------------------------
    Check if the given qos_handle belongs to the client
  -------------------------------------------------------------------------*/
    for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
    {
      if( (qosi_flow_info[i].qos_handle == v_in_reqd.qos_handle) &&
          (qosi_flow_info[i].cl_sp == qos_client_sp) )
      {
        break;
      }
    }

    if(i == QOSI_MAX_QOS_REQUESTS)
    {
      /* Check if the flow is network initiated if
         not found in UE init cache */
      for(i=0; i < QOSI_MAX_NETWORK_INIT_FLOW_NUM; i++)
      {
        if(nw_init_qosi_flow_info[i].qos_handle == v_in_reqd.qos_handle) 
        {
          break;
        }
      }
      if ((i == QOSI_MAX_NETWORK_INIT_FLOW_NUM) || 
          (PS_FLOW_GET_IFACE(nw_init_qosi_flow_info[i].flow_ptr) != qos_sp->ps_iface_ptr))
      {
        errval = QMI_ERR_INVALID_QOS_ID;
        goto send_result;
      }
      flow_ptr = nw_init_qosi_flow_info[i].flow_ptr;
    }
    else
    {
      /* UE initiated flows */
      flow_ptr = qosi_flow_info[i].flow_ptr;
    }
  }

  /*-------------------------------------------------------------------------
    Populate the response with the GRANTED Flow Spec
  -------------------------------------------------------------------------*/
  errval = qosi_append_granted_flow_spec(qos_sp,
                                         &response,
                                         flow_ptr,
                                         &num_bytes_pushed);


send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv( &response, result, errval );
  CHECK_RETVAL();
  return response;
} /* qmi_qosi_get_granted_qos */


/*===========================================================================
  FUNCTION QMI_QOS_GET_QOS_STATUS()

  DESCRIPTION
    This command is a request to get the status of a QoS instance.

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_get_qos_status
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_client_state_type *    qos_client_sp;
  dsm_item_type *                 response;
  struct
  {
    uint32  qos_handle;
  } v_in_reqd;
  PACKED struct PACKED_POST
  {
    uint8   qos_status;
  } v_out_reqd;
  uint16               len = 0;
  uint16               expected_len;
  uint8                type;
  uint8                i;
  void *               value;
  qmi_error_e_type     errval;
  qmi_result_e_type    result;
  boolean              retval;
  boolean              got_v_in_reqd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  got_v_in_reqd = FALSE;
  errval = QMI_ERR_NONE;
  memset(&v_in_reqd, 0, sizeof(v_in_reqd));
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;

  while (*sdu_in)
  {
    if(!qmi_svc_get_tl( sdu_in, &type, &len ))
    {
      continue;
    }
    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(v_in_reqd);
        value = (void*)&v_in_reqd;
        got_v_in_reqd = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_QOS,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP(sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
    /*-----------------------------------------------------------------------
      Speed up the parsing
    -----------------------------------------------------------------------*/
    if(got_v_in_reqd == TRUE)
    {
      break;
    }
  }

  if(!got_v_in_reqd)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }
  /*-------------------------------------------------------------------------
    Check if the given qos_handle belongs to the client
  -------------------------------------------------------------------------*/
  for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
  {
    if((qosi_flow_info[i].qos_handle == v_in_reqd.qos_handle) &&
       (qosi_flow_info[i].cl_sp == qos_client_sp))
    {
      v_out_reqd.qos_status = qosi_flow_info[i].qos_status;
      break;
    }
  }
  if(i == QOSI_MAX_QOS_REQUESTS)
  {
    errval = QMI_ERR_INVALID_QOS_ID;
    goto send_result;
  }

  if(FALSE == qmi_svc_put_param_tlv(&response,
                                    QMI_TYPE_REQUIRED_PARAMETERS,
                                    sizeof(v_out_reqd),
                                    &v_out_reqd))
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
    goto send_result;
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_qosi_get_qos_status() */



/*===========================================================================
  FUNCTION QMI_QOSI_GET_QOS_NETWORK_STATUS()

  DESCRIPTION
    Give the current QoS Network Status

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_qosi_get_qos_network_status
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_state_type *  qos_sp;
  dsm_item_type *        response;
  struct
  {
    uint8  network_qos_aware_status;
  } v_out_reqd;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  uint32             subs_id;
  ps_sys_3gpp2_network_qos_aware_status  network_qos_aware_status = FALSE;
  int16              ps_errno;
  int16              ps_result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  qos_sp = (qmi_qosi_state_type *) sp;

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  response = NULL;
  errval = QMI_ERR_NONE;

  subs_id = qmi_qosi_resolve_client_subs(cl_sp);
  ps_result = ps_sys_conf_get_ex (PS_SYS_TECH_3GPP2, 
                                  PS_SYS_CONF_3GPP2_QOS_NETWORK_STATUS,
                                  (ps_sys_subscription_enum_type)subs_id, 
                                  &network_qos_aware_status,
                                  &ps_errno);
                     
  if(ps_result < 0)
  {
    {
      LOG_MSG_ERROR_1("ps_sys_conf_get_ex %d operation failed:(%d)",ps_errno);
      switch (ps_errno)
      {                                   
        case DS_NOMEMORY:
          errval = QMI_ERR_NO_MEMORY;
          break;

        default:
          errval = QMI_ERR_INTERNAL;
          break;
      } 
      goto send_result;
    }
  }

  v_out_reqd.network_qos_aware_status = network_qos_aware_status;

  if( FALSE == qmi_svc_put_param_tlv(&response,
                                     QMI_TYPE_REQUIRED_PARAMETERS,
                                     sizeof (v_out_reqd),
                                     &v_out_reqd))
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_qosi_get_network_status() */



/*===========================================================================
  FUNCTION QMI_QOSI_NETWORK_STATUS_IND()

  DESCRIPTION
    Indicates the QoS aware state of the network connection

  PARAMETERS
    sp            : service provided state pointer (user data)
    cl_sp          : client state pointer
    network_qos_aware_status : qos aware state of network

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_network_status_ind
(
  qmi_qosi_state_type *   qos_sp,
  qmi_qosi_client_state_type * cl_sp,
  boolean                        network_qos_aware_status
)
{
  dsm_item_type  *       ind;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(qos_sp && cl_sp);

  /*-------------------------------------------------------------------------
    build indication from the back to the front
  -------------------------------------------------------------------------*/
  ind = NULL;

  if( FALSE == qmi_svc_put_param_tlv2( &ind,
                                      QMI_TYPE_REQUIRED_PARAMETERS,
                                      sizeof (network_qos_aware_status),
                                      &network_qos_aware_status,
                                      FILE_ID_DS_QMI_QOS,__LINE__ ) )
  {
    dsm_free_packet( &ind );
    return;
  }

  (void)qmi_qosi_send_ind(qos_sp, cl_sp, QOSI_CMD_VAL_GET_QOS_NETWORK_STATUS, ind);
} /* qmi_qosi_network_status_ind() */

/*===========================================================================
  FUNCTION QMI_QOSI_GENERATE_NETWORK_STATUS_IND()

  DESCRIPTION
    Checks for valid clients and generates QoS Network Status Indication

  PARAMETERS
    network_qos_aware_status : qos aware state of network
    subscription_id          : Subscription identifier

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_generate_network_status_ind
(
  boolean                 network_qos_aware_status,
  uint32                  subscription_id
)
{
  qmi_qosi_state_type *         qos_sp;
  qmi_qosi_client_state_type *  cl_sp;
  uint8                         state;
  uint8                         inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (state = 0; state < QOSI_MAX_STATE; state++)
  {
    for (inst = 0; inst < QOS_INSTANCE_MAX; inst++)
    {
      qos_sp = qmi_qos_state[state][inst];
  if(qos_sp == NULL)
  {
         continue;
  }
  /*-------------------------------------------------------------------------
    Go through all clients and send the event rpt ind to any
    client registered for it
  -------------------------------------------------------------------------*/
      cl_sp = qos_sp->client_list; 
      while(cl_sp != NULL)
      {
        if (qos_sp == qmi_qosi_get_qos_sp_by_cl_sp(cl_sp) &&
            qmi_qosi_verify_client_subs(cl_sp, subscription_id, FALSE))
  {
    qmi_qosi_network_status_ind(qos_sp,
                                       cl_sp,
                                       network_qos_aware_status);
  }
        cl_sp = cl_sp->next;
      } /* while */
    }/* inner for*/
  } /* outer for */
} /* qmi_qosi_generate_network_status_ind() */

/*===========================================================================
  FUNCTION QMI_QOSI_GET_NW_SUPPORTED_QOS_PROFILES()

  DESCRIPTION
    Give the list of network supported QoS Profile Values

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_qosi_get_nw_supported_qos_profiles
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *       response;
  qosi_nw_supp_qos_profile_type *v_out1 = NULL;
  uint8                type;
  uint16               len = 0;
  uint16               expected_len;
  void *               value;
  uint8                i, j;
  qmi_error_e_type     errval;
  qmi_result_e_type    result;
  boolean              retval;
  ps_iface_type *      ps_iface_ptr;
  int16               ps_errno;
  int                  return_val;
  ps_iface_ioctl_get_network_supported_qos_profiles_type nw_supp_qos_profiles;
  rmnet_meta_sm_network_info_type  *rtlkup_in_params;
  uint16               umts_default_profile_num = 0;
  uint16               cdma_default_profile_num = 0;
  uint8                profile_family = 0;
  rmnet_meta_sm_arb_info_type arb_info;
  qmi_qosi_client_state_type *client_sp;
  uint32               subs_id = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  memset( &arb_info, 0, sizeof(rmnet_meta_sm_arb_info_type) );
  response = NULL;
  errval = QMI_ERR_NONE;
  client_sp = (qmi_qosi_client_state_type *)cl_sp;
  subs_id   = qmi_qosi_resolve_client_subs(cl_sp);

  /*-------------------------------------------------------------------------
    Initialize rtlkup_in_params
    set the profile num to default profile, so that default profile is used
    if the config profile TLV is absent.
    tech_pref is ANY (WWAN_GROUP as of now)
  -------------------------------------------------------------------------*/
   PS_SYSTEM_HEAP_MEM_ALLOC(rtlkup_in_params,
                      sizeof(rmnet_meta_sm_network_info_type),
                            rmnet_meta_sm_network_info_type*);

   if (NULL == rtlkup_in_params)
   {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
   }

  memset(rtlkup_in_params, 0, sizeof(rmnet_meta_sm_network_info_type));
  /* This qmi msg is only applicable for CDMA as of now. So, umts profile
     number/profile family are not relavent. Hence ok to default to any
     value */

  profile_family = WDS_PROFILE_RMNET_FAMILY;
  (void)qmi_wds_profile_retrieve_default_profile_num(WDS_PROFILE_TYPE_3GPP,
                                                   profile_family,
                                                   &umts_default_profile_num,
                                                     subs_id);
  rtlkup_in_params->umts_profile_num = umts_default_profile_num;

  profile_family = 0;
  (void)qmi_wds_profile_retrieve_default_profile_num(WDS_PROFILE_TYPE_3GPP2,
                                                   profile_family,
                                                   &cdma_default_profile_num,
                                                     subs_id);
  rtlkup_in_params->cdma_profile_num = (uint8)cdma_default_profile_num;

  rtlkup_in_params->tech_pref        = WWAN_GROUP;
  rtlkup_in_params->subscription_id  = qmi_qosi_resolve_client_subs(cl_sp);
   
  PS_SYSTEM_HEAP_MEM_ALLOC(v_out1,
            sizeof(qosi_nw_supp_qos_profile_type),
            qosi_nw_supp_qos_profile_type*);

  if (NULL == v_out1)
  {
     errval = QMI_ERR_NO_MEMORY;
     goto send_result;
  }

  memset((void *)v_out1, 0, sizeof(qosi_nw_supp_qos_profile_type));

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      /*---------------------------------------------------------------------
        If get_tl() fails it continues and reaches the end of pkt
        (contents will be null) and terminates
      ---------------------------------------------------------------------*/
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_PRM_TYPE_EXT_TECH_PREF:
        expected_len = QMI_PRM_TYPE_EXT_TECH_PREF_LEN;
        value = &rtlkup_in_params->tech_pref;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_QOS,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and PS_UTILS_DSM_PULLUP will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  /*-------------------------------------------------------------------------
    Do route lookup to get the iface_ptr irrespective of whether the call is
    up or not.
  -------------------------------------------------------------------------*/
  ps_iface_ptr = rmnet_meta_smi_get_um_iface_ptr(rtlkup_in_params,
                                                 RMNET_RT_LK_UP_TYPE_IFACE,
                                                 NULL,
                                                 &arb_info);

  if(ps_iface_ptr == NULL)
  {
    errval = QMI_ERR_INTERFACE_NOT_FOUND;
    goto send_result;
  }

  /* This ioctl is only supported on CDMA currently. Hence no need to call it
     multiple times on different technologies. Populate the num_instances = 1
     and tech pref as CDMA in the output tlv for now. */
  len = 0;
  v_out1->num_instances = 1;
  len += sizeof(v_out1->num_instances);
  for (i=0; i<v_out1->num_instances; i++)
  {
    memset(&nw_supp_qos_profiles, 0, sizeof(nw_supp_qos_profiles));
    return_val = ps_iface_ioctl(
                           ps_iface_ptr,
                           PS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES,
                           &nw_supp_qos_profiles,
                           &ps_errno);
    if (return_val == -1)
    {
      qosi_populate_ext_error_tlv(&errval, &response, ps_errno);
      goto send_result;
    }

    v_out1->nw_supp_qos_prof[i].tech_name =
           (ps_iface_name_enum_type)
                    PS_IFACE_GET_NAME_FROM_ID(PS_IFACE_GET_ID(ps_iface_ptr));

    v_out1->nw_supp_qos_prof[i].profile_count =
                                          nw_supp_qos_profiles.profile_count;

    for (j=0; j<nw_supp_qos_profiles.profile_count; j++)
    {
      v_out1->nw_supp_qos_prof[i].profile_value[j] =
                                       nw_supp_qos_profiles.profile_value[j];
    }
    len += ( sizeof(v_out1->nw_supp_qos_prof[i].tech_name) +
               sizeof(v_out1->nw_supp_qos_prof[i].profile_count) +
               (v_out1->nw_supp_qos_prof[i].profile_count *
                      sizeof(v_out1->nw_supp_qos_prof[i].profile_value[0])) );
  }

  if(FALSE == qmi_svc_put_param_tlv(&response,
                                    QMI_TYPE_REQUIRED_PARAMETERS,
                                    len,
                                    (void *)v_out1))
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
    goto send_result;
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  if(rtlkup_in_params != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(rtlkup_in_params);
  }
  if(v_out1 != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(v_out1);
  }

  return response;
} /* qmi_qosi_get_nw_supported_qos_profiles() */

/*===========================================================================
  FUNCTION QMI_QOSI_PERFORM_FLOW_OPERATION()

  DESCRIPTION
    Perform requested operations on a flow

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_perform_flow_operation
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_state_type        * qos_sp;
  qmi_qosi_client_state_type * qos_client_sp;
  dsm_item_type *        response;
  uint32 v_in_reqd_qos_id = 0;
  uint32 v_in_opt_req_oper;
  uint32 v_in_opt_inact_tmr;
  boolean v_in_primary = FALSE;

  uint8                  type;
  uint8                  i;
  uint16                 len = 0;
  uint16                 expected_len;
  void *                 value;
  int16                 ps_errno;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  boolean            got_v_in_reqd;
  boolean            got_v_in_opt_inact_tmr;
  boolean            got_v_in_primary;
  int                return_val;
  ps_flow_type *     flow_ptr;
  ps_flow_ioctl_tx_queue_level_type tx_q_lvl;
  ps_flow_ioctl_707_hdr_rmac3_info_type rmac3_info;
  PACKED struct PACKED_POST
  {
    uint16  ps_headroom_payload_size;
    uint16  bucket_level_payload_size;
    uint16  t2p_inflow_payload_size;
  } v_out_rmac3_info = {0,};

  ps_flow_ioctl_707_tx_status_type tx_status;
  ps_flow_ioctl_707_inactivity_timer_type inact_timer;

  PACKED struct PACKED_POST
  {
    uint8    num_failures;
    PACKED struct PACKED_POST
    {
      uint8   bit_number;
      int16  ps_error;
    }error_list[QMI_QOSI_REQ_OPER_MAX];
  } v_out_oper_failure = {0,};
  uint8 error_list_len = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define FILL_OPER_FAIL_TLV(BIT, ERROR) \
  v_out_oper_failure.error_list[v_out_oper_failure.num_failures].bit_number = BIT; \
  v_out_oper_failure.error_list[v_out_oper_failure.num_failures].ps_error = ERROR; \
  v_out_oper_failure.num_failures++

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  got_v_in_reqd = FALSE;
  got_v_in_opt_inact_tmr = FALSE;
  got_v_in_primary = FALSE;

  qos_sp = (qmi_qosi_state_type *) sp;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;
  errval = QMI_ERR_NONE;
  ps_errno = 0;

  v_in_opt_req_oper = QOSI_REQ_OPER_ALL_BITS;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }
    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(v_in_reqd_qos_id);
        value = &v_in_reqd_qos_id;
        got_v_in_reqd = TRUE;
        break;

      case QMI_PRM_TYPE_REQUESTED_OPERATION:
        expected_len = sizeof(v_in_opt_req_oper);
        value = &v_in_opt_req_oper;
        break;

      case QMI_PRM_TYPE_SET_INACTIVITY_TIMER:
        expected_len = sizeof(v_in_opt_inact_tmr);
        value = &v_in_opt_inact_tmr;
        got_v_in_opt_inact_tmr = TRUE;
        break;

      case QMI_PRM_TYPE_PRIMARY_FLOW_OPER_REQ:
        expected_len = sizeof(v_in_primary);
        value = &v_in_primary;
        got_v_in_primary = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_QOS,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if(!got_v_in_reqd)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Iface_ptr is stored in qos_sp when call comes up
    Check whether the call is up and fail if not
  -------------------------------------------------------------------------*/
  if (qos_sp->ps_iface_ptr == NULL)
  {
    errval = QMI_ERR_OUT_OF_CALL;
    goto send_result;
  }

  if(!qmi_qosi_verify_client_subs(qos_client_sp, qos_sp->binding.subs_id, TRUE))
  {
    errval = QMI_ERR_INCOMPATIBLE_STATE;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Check if the request is for default flow and use the appropriate flow ptr
  -------------------------------------------------------------------------*/
  if ( (got_v_in_primary == TRUE) && (v_in_primary == TRUE) )
  {
    /* Default flow */
    LOG_MSG_INFO2_0("Operation requested on default flow");
    if (v_in_reqd_qos_id != 0)
    {
      LOG_MSG_ERROR_1("QoS ID(%d) must be sent as 0 for default flow",
                      v_in_reqd_qos_id);
      errval = QMI_ERR_INVALID_QOS_ID;
      goto send_result;
    }

    flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(qos_sp->ps_iface_ptr);
  }
  else
  {
     /* Secondary flow */
    LOG_MSG_INFO2_0("Operation requested on secondary flow");
    /*-----------------------------------------------------------------------
      Check if the given qos_handle belongs to the client
    -----------------------------------------------------------------------*/
    for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
    {
      if( (qosi_flow_info[i].qos_handle == v_in_reqd_qos_id) &&
          (qosi_flow_info[i].cl_sp == qos_client_sp) )
      {
        break;
      }
    }

    if(i == QOSI_MAX_QOS_REQUESTS)
    {
      errval = QMI_ERR_INVALID_QOS_ID;
      goto send_result;
    }

    flow_ptr = qosi_flow_info[i].flow_ptr;
  }

  ASSERT (flow_ptr);
  /*-------------------------------------------------------------------------
    Call relevant IOCTLs and populate response
  -------------------------------------------------------------------------*/
  if(v_in_opt_req_oper & ( 1 << QMI_QOSI_REQ_OPER_BIT0_GET_TX_Q_LVL))
  {
    return_val = ps_flow_ioctl( flow_ptr,
                                PS_FLOW_IOCTL_GET_TX_QUEUE_LEVEL,
                                &tx_q_lvl,
                                &ps_errno );
    if (return_val == -1)
    {
      FILL_OPER_FAIL_TLV(QMI_QOSI_REQ_OPER_BIT0_GET_TX_Q_LVL, ps_errno);
    }
    else
    {
      if(FALSE == qmi_svc_put_param_tlv( &response,
                                         QMI_PRM_TYPE_TX_QUEUE_LEVEL,
                                         QMI_PRM_TYPE_TX_QUEUE_LEVEL_LEN,
                                         &tx_q_lvl))
      {
        HANDLE_FAILURE();
      }
    }
  }

  if(v_in_opt_req_oper & ( 1 << QMI_QOSI_REQ_OPER_BIT1_GET_RMAC3_INFO))
  {
    return_val = ps_flow_ioctl( flow_ptr,
                                PS_FLOW_IOCTL_HDR_GET_RMAC3_INFO,
                                &rmac3_info,
                                &ps_errno );
    if (return_val == -1)
    {
      FILL_OPER_FAIL_TLV(QMI_QOSI_REQ_OPER_BIT1_GET_RMAC3_INFO, ps_errno);
    }
    else
    {
      v_out_rmac3_info.ps_headroom_payload_size =
                                         rmac3_info.ps_headroom_payload_size;
      v_out_rmac3_info.bucket_level_payload_size =
                                        rmac3_info.bucket_level_payload_size;
      v_out_rmac3_info.t2p_inflow_payload_size =
                                          rmac3_info.t2p_inflow_payload_size;
      if(FALSE == qmi_svc_put_param_tlv( &response,
                                         QMI_PRM_TYPE_RMAC3_INFO,
                                         sizeof(v_out_rmac3_info),
                                         (void *)&v_out_rmac3_info) )
      {
        HANDLE_FAILURE();
      }
    }
  }

  if(v_in_opt_req_oper & ( 1 << QMI_QOSI_REQ_OPER_BIT2_GET_TX_STATUS))
  {
    return_val = ps_flow_ioctl( flow_ptr,
                                PS_FLOW_IOCTL_707_GET_TX_STATUS,
                                &tx_status,
                                &ps_errno );
    if (return_val == -1)
    {
      FILL_OPER_FAIL_TLV(QMI_QOSI_REQ_OPER_BIT2_GET_TX_STATUS, ps_errno);
    }
    else
    {
      if(FALSE == qmi_svc_put_param_tlv( &response,
                                         QMI_PRM_TYPE_TX_STATUS,
                                         QMI_PRM_TYPE_TX_STATUS_LEN,
                                         &tx_status) )
      {
        HANDLE_FAILURE();
      }
    }
  }

  if(v_in_opt_req_oper & ( 1 << QMI_QOSI_REQ_OPER_BIT3_GET_INACTIVITY_TIMER))
  {
    return_val = ps_flow_ioctl( flow_ptr,
                                PS_FLOW_IOCTL_707_GET_INACTIVITY_TIMER,
                                &inact_timer,
                                &ps_errno );
    if (return_val == -1)
    {
      FILL_OPER_FAIL_TLV(QMI_QOSI_REQ_OPER_BIT3_GET_INACTIVITY_TIMER,
                         ps_errno);
    }
    else
    {
      if(FALSE == qmi_svc_put_param_tlv( &response,
                                         QMI_PRM_TYPE_INACTIVITY_TIMER,
                                         sizeof(inact_timer),
                                         &inact_timer) )
      {
        HANDLE_FAILURE();
      }
    }
  }

  if(v_in_opt_req_oper & ( 1 << QMI_QOSI_REQ_OPER_BIT4_SET_INACTIVITY_TIMER))
  {
    if (!got_v_in_opt_inact_tmr)
    {
      FILL_OPER_FAIL_TLV(QMI_QOSI_REQ_OPER_BIT4_SET_INACTIVITY_TIMER,
                         DS_EINVAL);
    }
    else
    {
      return_val = ps_flow_ioctl( flow_ptr,
                                  PS_FLOW_IOCTL_707_SET_INACTIVITY_TIMER,
                                  &v_in_opt_inact_tmr,
                                  &ps_errno );
      if (return_val == -1)
      {
        FILL_OPER_FAIL_TLV(QMI_QOSI_REQ_OPER_BIT4_SET_INACTIVITY_TIMER,
                           ps_errno);
      }
    }
  }

  if (v_out_oper_failure.num_failures > 0)
  {
    errval = QMI_ERR_OP_PARTIAL_FAILURE;
    ASSERT (v_out_oper_failure.num_failures <= QMI_QOSI_REQ_OPER_MAX);
    error_list_len = ( v_out_oper_failure.num_failures *
                       sizeof(v_out_oper_failure.error_list[0]) ) +
                     sizeof(uint8);
    if(FALSE == qmi_svc_put_param_tlv2(
                        &response,
                        QMI_TYPE_PARTIAL_OPER_FAILURE,
                        error_list_len,
                        (void *)&v_out_oper_failure,
                        FILE_ID_DS_QMI_QOS,__LINE__))
    {
      HANDLE_FAILURE();
    }
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv( &response, result, errval );
  CHECK_RETVAL();
  return response;
} /* qmi_qosi_perform_flow_operation() */

/*===========================================================================
  FUNCTION QOSI_MAP_PS_INFO_CODE()

  DESCRIPTION
    Maps the ps extended info codes to qmi info code values

  PARAMETERS
    info_code : qos status change reason code

  RETURN VALUE
    qmi_reason_code : corresponding qmi reason code

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_qosi_info_code_e_type qosi_map_ps_info_code
(
  ps_extended_info_code_enum_type  info_code
)
{
  qmi_qosi_info_code_e_type  qmi_reason_code;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(info_code)
  {
    case PS_EIC_QOS_INVALID_PARAMS:
    //case PS_EIC_QOS_INTERNAL_INVALID_PARAMS:
      qmi_reason_code = QOSI_INVALID_PARAMS;
      break;

    case PS_EIC_QOS_INTERNAL_CALL_ENDED:
      qmi_reason_code = QOSI_INTERNAL_CALL_ENDED;
      break;

    case PS_EIC_QOS_INTERNAL_ERROR:
      qmi_reason_code = QOSI_INTERNAL_ERROR;
      break;

    case PS_EIC_QOS_INSUFFICIENT_LOCAL_RESOURCES:
      qmi_reason_code = QOSI_INSUFFICIENT_LOCAL_RESOURCES;
      break;

    case PS_EIC_QOS_TIMED_OUT_OPERATION:
      qmi_reason_code = QOSI_TIMED_OUT_OPERATION;
      break;

    case PS_EIC_QOS_INTERNAL_UNKNOWN_CAUSE_CODE:
      qmi_reason_code = QOSI_INTERNAL_UNKNOWN_CAUSE_CODE;
      break;

    case PS_EIC_QOS_INTERNAL_MODIFY_IN_PROGRESS:
      qmi_reason_code = QOSI_INTERNAL_MODIFY_IN_PROGRESS;
      break;

    case PS_EIC_QOS_NOT_SUPPORTED:
      qmi_reason_code = QOSI_NOT_SUPPORTED;
      break;

    case PS_EIC_QOS_NOT_AVAILABLE:
      qmi_reason_code = QOSI_NOT_AVAILABLE;
      break;

    case PS_EIC_QOS_NOT_GUARANTEED:
      qmi_reason_code = QOSI_NOT_GUARANTEED;
      break;

    case PS_EIC_QOS_INSUFFICIENT_NET_RESOURCES:
      qmi_reason_code = QOSI_INSUFFICIENT_NETWORK_RESOURCES;
      break;

    case PS_EIC_QOS_AWARE_SYSTEM:
      qmi_reason_code = QOSI_QOS_AWARE_SYSTEM;
      break;

    case PS_EIC_QOS_UNAWARE_SYSTEM:
      qmi_reason_code = QOSI_QOS_UNAWARE_SYSTEM;
      break;

    case PS_EIC_QOS_REJECTED_OPERATION:
      qmi_reason_code = QOSI_REJECTED_OPERATION;
      break;

    case PS_EIC_QOS_WILL_GRANT_WHEN_QOS_RESUMED:
      qmi_reason_code = QOSI_WILL_GRANT_WHEN_QOS_RESUMED;
      break;

    case PS_EIC_QOS_NETWORK_CALL_ENDED:
      qmi_reason_code = QOSI_NETWORK_CALL_ENDED;
      break;

    case PS_EIC_QOS_NETWORK_SVC_NOT_AVAILABLE:
      qmi_reason_code = QOSI_NETWORK_SERVICE_NOT_AVAILABLE;
      break;

    case PS_EIC_QOS_NETWORK_L2_LINK_RELEASED:
      qmi_reason_code = QOSI_NETWORK_L2_LINK_RELEASED;
      break;

    case PS_EIC_QOS_NETWORK_L2_LINK_REESTAB_REJ:
      qmi_reason_code = QOSI_NETWORK_L2_LINK_REESTAB_REJ;
      break;

    case PS_EIC_QOS_NETWORK_L2_LINK_REESTAB_IND:
      qmi_reason_code = QOSI_NETWORK_L2_LINK_REESTAB_IND;
      break;

    case PS_EIC_QOS_NETWORK_UNKNOWN_CAUSE_CODE:
      qmi_reason_code = QOSI_NETWORK_UNKNOWN_CAUSE_CODE;
      break;

    case PS_EIC_NETWORK_BUSY:
      qmi_reason_code = QOSI_NETWORK_BUSY;
      break;
	  
    case PS_EIC_QOS_NETWORK_DISJOINT_PROFILE_SET_SUGGESTED:
      qmi_reason_code = QOSI_NETWORK_DISJOINT_PROFILE_SET_SUGGESTED;
      break;

    case PS_EIC_QOS_NETWORK_NULL_PROFILE_SUGGESTED:
      qmi_reason_code = QOSI_NETWORK_NULL_PROFILE_SUGGESTED;
      break;

    case PS_EIC_QOS_NETWORK_UE_NOT_AUTHORIZED:
      qmi_reason_code = QOSI_NETWORK_UE_NOT_AUTHORIZED;
      break;

    default:
      LOG_MSG_ERROR_1("Unrecognized extended info code (%d)", info_code);
      qmi_reason_code = (qmi_qosi_info_code_e_type) (PS_EIC_NOT_SPECIFIED);
      break;
  }

  return qmi_reason_code;
}


/*===========================================================================
  FUNCTION QMI_QOSI_QOS_STATUS_IND()

  DESCRIPTION
    Is called when any of the conditions set in event_report_req becomes
    true.  Sends an indication to the client.

  PARAMETERS
    qos_sp      : service provided state pointer
    cl_sp       : client provided state pointer
    qos_handle  : QoS Handle
    qos_status  : QoS Status
    qos_event   : QoS Event
    info_code   : Info Code

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_qos_status_ind
(
  qmi_qosi_state_type  *           qos_sp,
  qmi_qosi_client_state_type *     cl_sp,
  uint32                           qos_handle,
  qmi_qosi_status_e_type           qos_status,
  qmi_qosi_event_e_type            qos_event,
  ps_extended_info_code_enum_type  info_code
)
{
  dsm_item_type *               ind;
  struct
  {
    uint32 qos_handle;
    uint8  qos_status;
    uint8  qos_event;
  } v_out1;
  qmi_qosi_info_code_e_type  qmi_reason_code;
  uint8                      ip_family;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Build indication from the back to the front
  -------------------------------------------------------------------------*/
  ind = NULL;
  LOG_MSG_INFO2_2("Sending qos_status (%d) ind for qos_handle %d",
                  qos_status, qos_handle);

  if (qos_sp == NULL)
  {
    DATA_ERR_FATAL("qos_sp is NULL !");
    return;
  }

  ip_family = (uint8)ps_iface_get_addr_family(qos_sp->ps_iface_ptr);
  if (FALSE == qmi_svc_put_param_tlv2(&ind,
                                 QMI_PRM_TYPE_QOS_IP_FAMILY,
                                 sizeof(uint8),
                                 &ip_family,
                                 FILE_ID_DS_QMI_QOS,__LINE__))
  {
    dsm_free_packet(&ind);
    return;
  }

  v_out1.qos_handle = qos_handle;
  v_out1.qos_status = qos_status;
  v_out1.qos_event = qos_event;

  if(info_code != PS_EIC_NOT_SPECIFIED)
  {
    qmi_reason_code = qosi_map_ps_info_code(info_code);
    if(FALSE == qmi_svc_put_param_tlv(&ind,
                                      QMI_PRM_TYPE_REASON_CODE,
                                      sizeof(uint8),
                                      &qmi_reason_code))
    {
      dsm_free_packet(&ind);
    }
  }

  if(FALSE == qmi_svc_put_param_tlv(&ind,
                                    QMI_TYPE_REQUIRED_PARAMETERS,
                                    6,
                                    &v_out1))
  {
    dsm_free_packet(&ind);
    return;
  }

  /*-------------------------------------------------------------------------
    Go through all clients and send the event rpt ind to any
    client registered for it
  -------------------------------------------------------------------------*/
  if(cl_sp == NULL)
  {
    cl_sp = qos_sp->client_list; 
    while(cl_sp != NULL)
    {
      if (qos_sp == qmi_qosi_get_qos_sp_by_cl_sp(cl_sp) &&
          qmi_qosi_verify_client_subs(cl_sp,qos_sp->binding.subs_id, TRUE))
      {
        (void)qmi_qosi_send_ind( qos_sp, cl_sp, QOSI_CMD_VAL_GET_QOS_STATUS,
                                  ind );
      }
      cl_sp = cl_sp->next;
    }
  }
  else
  {
  (void)qmi_qosi_send_ind( qos_sp, cl_sp, QOSI_CMD_VAL_GET_QOS_STATUS, ind );
  }
} /* qmi_qosi_qos_status_ind() */



/*===========================================================================
  FUNCTION QMI_QOSI_EVENT_REPORT_IND()

  DESCRIPTION
    Is called when any of the conditions set in event_report_req becomes
    true.  Sends an indication to the client.

  PARAMETERS
    qos_sp      : service provided state pointer
    cl_sp       : client provided state pointer
    qos_flow_info_ptr  : QoS Flow Pointer
    num_qos_ids  : Number of QoS IDs
    flow_change_ev   : Flow Change Event
    new_flow   : Whether it is a new flow

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_event_report_ind
(
  qmi_qosi_state_type *        qos_sp,
  qmi_qosi_client_state_type * cl_sp,
  qmi_flow_info_type **         qmi_flow_info_ptr,
  uint16                        num_qos_ids,
  uint8                         flow_change_ev,
  boolean                       new_flow
)
{
  dsm_item_type *               ind;
  struct
  {
    uint32  qos_handle;
    uint8   new_flow;
    uint8   flow_change;
  } v_out1;
  uint16   i;
  uint8    type;
  uint16  total_bytes_pushed;
  uint16  num_bytes_pushed;
  boolean is_nw_initiated = FALSE;
  uint8   bearer_id = 0;
  uint16 uplink_fc_seq_num;
  ps_phys_link_type *  phys_link_ptr;
  qmi_error_e_type ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Build indication from the back to the front
  -------------------------------------------------------------------------*/
  ind = NULL;
  v_out1.flow_change = flow_change_ev;
  v_out1.new_flow = new_flow;

  for(i = 0; i < num_qos_ids; i++)
  {
    total_bytes_pushed = 0;
    num_bytes_pushed = 0;
    uplink_fc_seq_num = 0;

    if(qmi_flow_info_ptr[i] != NULL)
    {
      v_out1.qos_handle = qmi_flow_info_ptr[i]->qos_handle;
    }
    else
    {
      v_out1.qos_handle = 0; //default flow
    }

    LOG_MSG_INFO2_2("sending flow event %d for handle 0x%x",flow_change_ev,v_out1.qos_handle);
    switch(flow_change_ev)
    {
      case QOSI_REPORT_STATUS_FLOW_ACTIVATED:
      {
        if(NULL == qmi_flow_info_ptr[i]->flow_ptr)
        {
          LOG_MSG_ERROR_0("Flow ptr NULL, unable to send QoS event report indication");
          return;
        }

        /*-------------------------------------------------------------------
          Add bearer ID TLV
        -------------------------------------------------------------------*/
        phys_link_ptr = PS_FLOW_GET_PHYS_LINK(qmi_flow_info_ptr[i]->flow_ptr);
        bearer_id = PS_PHYS_LINK_GET_BEARER_ID(phys_link_ptr);

        if(FALSE == qmi_svc_put_param_tlv2(&ind,
                                      QMI_PRM_TYPE_QOS_BEARER_ID,
                                      sizeof(uint8),
                                      &bearer_id,
                                      FILE_ID_DS_QMI_QOS,__LINE__))
        {
          dsm_free_packet(&ind);
          return;
        }

        total_bytes_pushed += 4;

        /*-------------------------------------------------------------------
          Add flow type TLV
        -------------------------------------------------------------------*/
        is_nw_initiated = PS_FLOW_IS_NW_INITIATED(qmi_flow_info_ptr[i]->flow_ptr);

        if(FALSE == qmi_svc_put_param_tlv2(&ind,
                                      QMI_PRM_TYPE_QOS_FLOW_TYPE,
                                      1,
                                      &is_nw_initiated,
                                      FILE_ID_DS_QMI_QOS,__LINE__))
        {
          dsm_free_packet(&ind);
          return;
        }

        total_bytes_pushed += 4;

        /*-------------------------------------------------------------------
          Send both granted flow spec and filter spec only if its a new flow
        -------------------------------------------------------------------*/
        if(qosi_append_granted_flow_spec(qos_sp,
                                         &ind,
                                         qmi_flow_info_ptr[i]->flow_ptr,
                                         &num_bytes_pushed)
         != QMI_ERR_NONE)
        {
          dsm_free_packet(&ind);
          return;
        }
        total_bytes_pushed += num_bytes_pushed;

        if(qosi_append_filter_spec(&ind,
                                   qos_sp->ps_iface_ptr,
                                   qmi_flow_info_ptr[i]->flow_ptr,
                                   &num_bytes_pushed,
                                   QMI_PRM_TYPE_TX_QOS_FILTER_REQ,
                                   QMI_PRM_TYPE_RX_QOS_FILTER_REQ) != QMI_ERR_NONE)
        {
          dsm_free_packet(&ind);
          return;
        }
        total_bytes_pushed += num_bytes_pushed;
      }
      break;

      case QOSI_REPORT_STATUS_FLOW_MODIFIED:
      {
        /*-------------------------------------------------------------------
        Send both granted flow spec and filter spec in case of FLOW MODIFIED
        -------------------------------------------------------------------*/
        ret = qosi_append_granted_flow_spec(qos_sp,
                                         &ind,
                                         qmi_flow_info_ptr[i]->flow_ptr,
                                         &num_bytes_pushed);
        /* Flow might be SUSPENDED for NW init QOS in eHRPD in which case
           there will be no granted flow info to add */
        if (ret != QMI_ERR_FLOW_SUSPENDED && ret != QMI_ERR_NONE)
        {
          dsm_free_packet(&ind);
          return;
        }
        total_bytes_pushed += num_bytes_pushed;

        if(qosi_append_filter_spec(&ind,
                                   qos_sp->ps_iface_ptr,
                                   qmi_flow_info_ptr[i]->flow_ptr,
                                   &num_bytes_pushed,
                                   QMI_PRM_TYPE_TX_QOS_FILTER_REQ,
                                   QMI_PRM_TYPE_RX_QOS_FILTER_REQ) != QMI_ERR_NONE)
        {
          dsm_free_packet(&ind);
          return;
        }
        total_bytes_pushed += num_bytes_pushed;
      }
      break;

      case QOSI_REPORT_STATUS_FLOW_DELETED:
      break;

      case QOSI_REPORT_STATUS_FLOW_SUSPENDED:
      {
        /* Flow type and filter TLVs are needed for eHRPD NW init QOS */

        /*-------------------------------------------------------------------
          Add flow type TLV
        -------------------------------------------------------------------*/
        is_nw_initiated = PS_FLOW_IS_NW_INITIATED(qmi_flow_info_ptr[i]->flow_ptr);

        if(FALSE == qmi_svc_put_param_tlv2(&ind,
                                      QMI_PRM_TYPE_QOS_FLOW_TYPE,
                                      1,
                                      &is_nw_initiated,
                                      FILE_ID_DS_QMI_QOS,__LINE__))
        {
          dsm_free_packet(&ind);
          return;
        }
        total_bytes_pushed += 4;

        /*-------------------------------------------------------------------
        Send only filter spec in case of FLOW SUSPENDED
        -------------------------------------------------------------------*/

        if(qosi_append_filter_spec(&ind,
                                   qos_sp->ps_iface_ptr,
                                   qmi_flow_info_ptr[i]->flow_ptr,
                                   &num_bytes_pushed,
                                   QMI_PRM_TYPE_TX_QOS_FILTER_REQ,
                                   QMI_PRM_TYPE_RX_QOS_FILTER_REQ) != QMI_ERR_NONE)
        {
          LOG_MSG_ERROR_0("Error returned from qosi_append_filter_spec");
          dsm_free_packet(&ind);
          return;
        }
        total_bytes_pushed += num_bytes_pushed;
      }
      break;

      case QOSI_REPORT_STATUS_FLOW_ENABLED:
      {
        if (qmi_flow_info_ptr[i] != NULL)
        {
          uplink_fc_seq_num = qmi_flow_info_ptr[i]->uplink_fc_seq_num;
        }
        else
        {
          // Default Flow; Use seq no from qos_sp
          uplink_fc_seq_num = qos_sp->uplink_fc_seq_num;
        }

        if (FALSE == qmi_svc_put_param_tlv2(&ind,
                                          QMI_PRM_TYPE_UPLINK_FC_SEQ_NUM,
                                          sizeof(uint16),
                                          &uplink_fc_seq_num,
                                          FILE_ID_DS_QMI_QOS,__LINE__))
        {
          dsm_free_packet(&ind);
          return;
        }

        total_bytes_pushed += 5;
      }
      break;

      case QOSI_REPORT_STATUS_FLOW_DISABLED:
      {
        if (qmi_flow_info_ptr[i] != NULL)
        {
          uplink_fc_seq_num = qmi_flow_info_ptr[i]->uplink_fc_seq_num;
        }
        else
        {
          // Default Flow; Use seq no from qos_sp
          uplink_fc_seq_num = qos_sp->uplink_fc_seq_num;
        }

        if (FALSE == qmi_svc_put_param_tlv2(&ind,
                                          QMI_PRM_TYPE_UPLINK_FC_SEQ_NUM,
                                          sizeof(uint16),
                                          &uplink_fc_seq_num,
                                          FILE_ID_DS_QMI_QOS,__LINE__))
        {
          dsm_free_packet(&ind);
          return;
        }

        total_bytes_pushed += 5;
      }
      break;

      default:
        LOG_MSG_ERROR_1("Unhandled flow change ev %d received in event_rpt_ind",
                        flow_change_ev);
        ASSERT(0);
        return;
    }

    if(FALSE == qmi_svc_put_param_tlv(&ind,
                                      QOSI_PRM_TYPE_REPORT_FLOW_CHANGE,
                                      6,
                                      &v_out1))
    {
      dsm_free_packet(&ind);
      return;
    }

    total_bytes_pushed += 6 + 3;

    if(total_bytes_pushed > 0)
    {
      type = QOSI_PRM_TYPE_GLOBAL_FLOW_CHANGE_REPORT;
      if(!((QMI_SVC_PKT_PUSH(&ind, &total_bytes_pushed, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(&ind, &type, sizeof(uint8)))))
      {
        dsm_free_packet(&ind);
        return;
      }
    }
  }

  (void)qmi_qosi_send_ind( qos_sp, cl_sp, QOSI_CMD_VAL_SET_EVENT_REPORT, ind );
} /* qmi_qosi_event_report_ind() */

/*===========================================================================
  FUNCTION QMI_QOSI_EVENT_REPORT_IND_EX()

  DESCRIPTION
    Is called when any of the conditions set in event_report_req becomes
    true.  Sends an indication to the client.

  PARAMETERS
    qos_sp      : service provided state pointer
    cl_sp       : client provided state pointer
    qos_flow_info_ptr  : QoS Flow Pointer
    num_qos_ids  : Number of QoS IDs
    flow_change_ev   : Flow Change Event
    new_flow   : Whether it is a new flow

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_event_report_ind_ex
(
  qmi_qosi_state_type *        qos_sp,
  qmi_qosi_client_state_type * cl_sp,
  qmi_flow_info_type **         qmi_flow_info_ptr,
  uint16                        num_qos_ids,
  uint8                         flow_change_ev,
  boolean                       new_flow
)
{
  dsm_item_type *               ind = NULL;
  qos_global_qos_flow_ind_msg_v01 * ind_msg = NULL;
  uint16   i;
  ps_phys_link_type *  phys_link_ptr;
  qmi_error_type_v01 ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Build indication from the back to the front
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(ind_msg, sizeof(qos_global_qos_flow_ind_msg_v01),
                           qos_global_qos_flow_ind_msg_v01*);
  if (ind_msg == NULL)
  {
    return; 
  }


  for(i = 0; i < num_qos_ids; i++)
  {
    memset(ind_msg, 0, sizeof(qos_global_qos_flow_ind_msg_v01));
    ind_msg->qos_flow_state.state_change = (qos_flow_state_change_enum_v01)(flow_change_ev - 1); //for now
    ind_msg->qos_flow_state.new_flow = new_flow;


    if(qmi_flow_info_ptr[i] != NULL)
    {
      ind_msg->qos_flow_state.qos_id = qmi_flow_info_ptr[i]->qos_handle;
    }
    else
    {
      ind_msg->qos_flow_state.qos_id = 0; //default flow
    }

    LOG_MSG_INFO2_2("sending flow event %d for handle 0x%x",flow_change_ev,ind_msg->qos_flow_state.qos_id);
    switch(ind_msg->qos_flow_state.state_change)
    {
      case QOS_FLOW_ACTIVATED_V01:
      {
        if(NULL == qmi_flow_info_ptr[i]->flow_ptr)
        {
          LOG_MSG_ERROR_0("Flow ptr NULL, unable to send QoS event report indication");
          PS_SYSTEM_HEAP_MEM_FREE(ind_msg);
          return;
        }

        /*-------------------------------------------------------------------
          Add bearer ID TLV
        -------------------------------------------------------------------*/
        phys_link_ptr = PS_FLOW_GET_PHYS_LINK(qmi_flow_info_ptr[i]->flow_ptr);
        ind_msg->bearer_id_valid = TRUE;
        ind_msg->bearer_id = PS_PHYS_LINK_GET_BEARER_ID(phys_link_ptr);

        /*-------------------------------------------------------------------
          Add flow type TLV
        -------------------------------------------------------------------*/
        ind_msg->flow_type_valid = TRUE;
        ind_msg->flow_type = PS_FLOW_IS_NW_INITIATED(qmi_flow_info_ptr[i]->flow_ptr);

        /*-------------------------------------------------------------------
          Send both granted flow spec and filter spec only if its a new flow
        -------------------------------------------------------------------*/
        if(qosi_append_granted_flow_spec_ex(qos_sp,
                                            &ind_msg->tx_qos_flow_granted_valid,
											&ind_msg->tx_qos_flow_granted,
											&ind_msg->rx_qos_flow_granted_valid,
											&ind_msg->rx_qos_flow_granted,
                                            NULL,
                                            qmi_flow_info_ptr[i]->flow_ptr)
         != QMI_ERR_NONE_V01)
        {
          PS_SYSTEM_HEAP_MEM_FREE(ind_msg);
          return;
        }

        if(qosi_append_filter_spec_ex(qos_sp->ps_iface_ptr,
                                      qmi_flow_info_ptr[i]->flow_ptr,
                                      &ind_msg->tx_qos_filters_valid,
                                      &ind_msg->tx_qos_filters_len,
                                      ind_msg->tx_qos_filters,
                                      &ind_msg->rx_qos_filters_valid,
                                      &ind_msg->rx_qos_filters_len,
                                      ind_msg->rx_qos_filters) != QMI_ERR_NONE_V01)
        {
          PS_SYSTEM_HEAP_MEM_FREE(ind_msg);
          return;
        }
      }
      break;

      case QOS_FLOW_MODIFIED_V01:
      {
        /*-------------------------------------------------------------------
        Send both granted flow spec and filter spec in case of FLOW MODIFIED
        -------------------------------------------------------------------*/
        ret = qosi_append_granted_flow_spec_ex(qos_sp,
                                            &ind_msg->tx_qos_flow_granted_valid,
											&ind_msg->tx_qos_flow_granted,
											&ind_msg->rx_qos_flow_granted_valid,
											&ind_msg->rx_qos_flow_granted,
                                            NULL,
                                            qmi_flow_info_ptr[i]->flow_ptr);
        /* Flow might be SUSPENDED for NW init QOS in eHRPD in which case
           there will be no granted flow info to add */
        if (ret != QMI_ERR_FLOW_SUSPENDED_V01 && ret != QMI_ERR_NONE_V01)
        {
          PS_SYSTEM_HEAP_MEM_FREE(ind_msg);
          return;
        }

        if(qosi_append_filter_spec_ex(qos_sp->ps_iface_ptr,
                                      qmi_flow_info_ptr[i]->flow_ptr,
                                      &ind_msg->tx_qos_filters_valid,
                                      &ind_msg->tx_qos_filters_len,
                                      ind_msg->tx_qos_filters,
                                      &ind_msg->rx_qos_filters_valid,
                                      &ind_msg->rx_qos_filters_len,
                                      ind_msg->rx_qos_filters) != QMI_ERR_NONE_V01)
        {
          PS_SYSTEM_HEAP_MEM_FREE(ind_msg);
          return;
        }
      }
      break;

      case QOS_FLOW_DELETED_V01:
      break;

      case QOS_FLOW_SUSPENDED_V01:
      {
        /* Flow type and filter TLVs are needed for eHRPD NW init QOS */

        /*-------------------------------------------------------------------
          Add flow type TLV
        -------------------------------------------------------------------*/
        ind_msg->flow_type_valid = TRUE;
        ind_msg->flow_type = PS_FLOW_IS_NW_INITIATED(qmi_flow_info_ptr[i]->flow_ptr);

        /*-------------------------------------------------------------------
        Send only filter spec in case of FLOW SUSPENDED
        -------------------------------------------------------------------*/

        if(qosi_append_filter_spec_ex(qos_sp->ps_iface_ptr,
                                      qmi_flow_info_ptr[i]->flow_ptr,
                                      &ind_msg->tx_qos_filters_valid,
                                      &ind_msg->tx_qos_filters_len,
                                      ind_msg->tx_qos_filters,
                                      &ind_msg->rx_qos_filters_valid,
                                      &ind_msg->rx_qos_filters_len,
                                      ind_msg->rx_qos_filters) != QMI_ERR_NONE_V01)
        {
          LOG_MSG_ERROR_0("Error returned from qosi_append_filter_spec_ex");
          PS_SYSTEM_HEAP_MEM_FREE(ind_msg);
          return;
        }
      }
      break;

      case QOS_FLOW_ENABLED_V01:
      {
        if (qmi_flow_info_ptr[i] != NULL)
        {
          ind_msg->fc_seq_num = qmi_flow_info_ptr[i]->uplink_fc_seq_num;
        }
        else
        {
          // Default Flow; Use seq no from qos_sp
          ind_msg->fc_seq_num = qos_sp->uplink_fc_seq_num;
        }
        ind_msg->fc_seq_num_valid = TRUE;
      }
      break;

      case QOS_FLOW_DISABLED_V01:
      {
        if (qmi_flow_info_ptr[i] != NULL)
        {
          ind_msg->fc_seq_num = qmi_flow_info_ptr[i]->uplink_fc_seq_num;
        }
        else
        {
          // Default Flow; Use seq no from qos_sp
          ind_msg->fc_seq_num = qos_sp->uplink_fc_seq_num;
        }
        ind_msg->fc_seq_num_valid = TRUE;
      }
      break;

      default:
        LOG_MSG_ERROR_1("Unhandled flow change ev %d received in event_rpt_ind",
                        flow_change_ev);
        ASSERT(0);
        return;
    }
    //Encode and send indication
    if(!qmi_svc_idl_message_encode(qos_svc_obj,
                                   QMI_IDL_INDICATION,
                                   QMI_QOS_GLOBAL_QOS_FLOW_IND_V01,
                                   ind_msg,
                                   sizeof(qos_global_qos_flow_ind_msg_v01),
                                   &ind))
    {
      LOG_MSG_ERROR_0("encoding failed for global flow ind ");
    }
    else
    {
      (void)qmi_qosi_send_ind( qos_sp, cl_sp, QOSI_CMD_VAL_GLOBAL_QOS_FLOW_IND, ind );
    }
  }
  PS_SYSTEM_HEAP_MEM_FREE(ind_msg);
} /* qmi_qosi_event_report_ind_ex() */

/*===========================================================================
  FUNCTION QMI_QOSI_EVENT_REPORT_IND2()

  DESCRIPTION
    Is called when any of the conditions set in event_report_req becomes
    true.  Sends an indication to the client.

  PARAMETERS
    qos_sp      : service provided state pointer
    clsp        : client state pointer
    ev_rep_info : mask to indicate what caused the indication to be called

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_event_report_ind2
(
  qmi_qosi_state_type *        qos_sp,
  qmi_qosi_client_state_type * cl_sp,
  qmi_qosi_event_info_type   * ev_rep_info
)
{
  dsm_item_type *               ind;
  uint16    len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Build indication from the back to the front
  -------------------------------------------------------------------------*/
  ind = NULL;

  switch (ev_rep_info->ind_cause)
  {
    case QMI_QOSI_EV_REPORT_CAUSE_NW_SUPP_QOS_PROFILE_CH:
    {
    /*-----------------------------------------------------------------------
       Event causing indication is Change in NW Supported QOS profiles
    -----------------------------------------------------------------------*/
      len = sizeof(ev_rep_info->event_info.nw_supp_qos_prof.tech_name) +
            sizeof(ev_rep_info->event_info.nw_supp_qos_prof.profile_count) +
         (ev_rep_info->event_info.nw_supp_qos_prof.profile_count *
          sizeof(ev_rep_info->event_info.nw_supp_qos_prof.profile_value[0]));

      if(FALSE == qmi_svc_put_param_tlv(&ind,
                        QOSI_PRM_TYPE_REPORT_NW_SUPP_QOS_PROFILE,
                        len,
                        (void *)&(ev_rep_info->event_info.nw_supp_qos_prof)))
      {
        dsm_free_packet(&ind);
        return;
      }
      break;
    }

    default:
      LOG_MSG_ERROR_1("Unexpected ev cause %d received in event_rpt_ind",
                      ev_rep_info->ind_cause);
      ASSERT(0);
      return;
  }

  (void)qmi_qosi_send_ind(qos_sp,
                          cl_sp,
                          QOSI_CMD_VAL_SET_EVENT_REPORT,
                          ind );

} /* qmi_qosi_event_report_ind2() */

/*===========================================================================
  FUNCTION QMI_QOSI_PRIMARY_QOS_EVENT_IND()

  DESCRIPTION
    Indicates various events specific to the primary flow

  PARAMETERS
    qos_sp     : service provided state pointer
    cl_sp      : client sp
    event      : event that occured on the primary flow
    event_info : event information associated with the event

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_primary_qos_event_ind
(
  qmi_qosi_state_type *             qos_sp,
  qmi_qosi_client_state_type *      cl_sp,
  qmi_qosi_primary_qos_event_e_type event,
  void                *             event_info
)
{
  dsm_item_type  *       ind;
  uint8                  ip_family;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(qos_sp);

  /* Currently only primary modify result event is supported. No event_info
     is associated with this event. When more events are added, add logic to
     read event_info and send out in optional tlvs. */

  /*-------------------------------------------------------------------------
    build indication from the back to the front
  -------------------------------------------------------------------------*/
  ind = NULL;

  ip_family = (uint8)ps_iface_get_addr_family(qos_sp->ps_iface_ptr);
  if ( FALSE == qmi_svc_put_param_tlv2( &ind,
                                 QMI_PRM_TYPE_PRI_QOS_IP_FAMILY,
                                 sizeof(uint8),
                                 &ip_family,
                                 FILE_ID_DS_QMI_QOS,__LINE__ ) )
  {
    dsm_free_packet( &ind );
    return;
  }

  if( FALSE == qmi_svc_put_param_tlv2( &ind,
                                      QMI_TYPE_REQUIRED_PARAMETERS,
                                      sizeof (event),
                                      &event,
                                      FILE_ID_DS_QMI_QOS,__LINE__ ) )
  {
    dsm_free_packet( &ind );
    return;
  }

  if(cl_sp == NULL)
  {
    cl_sp = qos_sp->client_list; 
    while(cl_sp != NULL)
    {
      if (qos_sp == qmi_qosi_get_qos_sp_by_cl_sp(cl_sp) &&
          qmi_qosi_verify_client_subs(cl_sp,qos_sp->binding.subs_id, TRUE))
      {
          (void)qmi_qosi_send_ind(qos_sp, cl_sp,
                                  QOSI_CMD_VAL_PRIMARY_QOS_EVENT, ind );
      }
      cl_sp = cl_sp->next;
    }
  }
  else
  {
  (void)qmi_qosi_send_ind(qos_sp,
                          cl_sp,
                          QOSI_CMD_VAL_PRIMARY_QOS_EVENT,
                          ind );
  }

} /* qmi_qosi_primary_qos_event_ind() */

/*===========================================================================
  FUNCTION QOSI_GENERATE_FLOW_FAILED_TLV_LIST()

  DESCRIPTION
    Generate a list of error tlvs for flows provided in QOS REQUEST/MODIFY

  PARAMETERS
    flow      : ptr to flow parameters
    response  : response ptr to append the error tlvs to
    flow_indx : index of the given flow

  RETURN VALUE
    num_bytes_pushed : number of bytes appended to the response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qosi_generate_flow_failed_tlv_list
(
  ip_flow_type *    flow,
  dsm_item_type **  response,
  uint8             flow_indx,
  uint16        *   num_bytes_pushed
)
{
  uint8  failed_tlv_list[20];
  uint8  f_tlv_cnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *num_bytes_pushed = 0;
  f_tlv_cnt = 0;
  memset(failed_tlv_list, 0, sizeof(failed_tlv_list));

  if(flow->err_mask & IPFLOW_MASK_TRF_CLASS)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_TRF_CLASS;
  }
  if(flow->err_mask & IPFLOW_MASK_DATA_RATE)
  {
    if(flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
    {
      failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX;
    }
    else if(flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE)
    {
      failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_EX;
    }
    else
    {
      failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_DATA_R_TOKEN_BUK;
    }
  }
  if(flow->err_mask & IPFLOW_MASK_LATENCY)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_LATENCY;
  }
  if(flow->err_mask & IPFLOW_MASK_LATENCY_VAR)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_JITTER;
  }
  if(flow->err_mask & IPFLOW_MASK_PKT_ERR_RATE)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_PKT_ERR_RATE;
  }
  if(flow->err_mask & IPFLOW_MASK_MIN_POLICED_PKT_SIZE)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_MIN_POLICED_PKT_SIZE;
  }
  if(flow->err_mask & IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_MAX_ALLOWED_PKT_SIZE;
  }
  if(flow->err_mask & IPFLOW_MASK_UMTS_RES_BER)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_RES_BER;
  }
  if(flow->err_mask & IPFLOW_MASK_UMTS_TRF_PRI)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_TRF_PRI;
  }
  if(flow->err_mask & IPFLOW_MASK_CDMA_PROFILE_ID)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_PROFILE_ID;
  }
  if(flow->err_mask & IPFLOW_MASK_CDMA_FLOW_PRIORITY)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_PRIORITY;
  }
  if(flow->err_mask & IPFLOW_MASK_UMTS_IM_CN_FLAG)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_IM_CN_FLAG;
  }
  if(flow->err_mask & IPFLOW_MASK_UMTS_SIG_IND)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_SIG_IND;
  }
  if(flow->err_mask & IPFLOW_MASK_LTE_QCI)
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_FLOW_LTE_QCI;
  }

  if(f_tlv_cnt == 0)
  {
    return QMI_ERR_NONE; //no tlv to send
  }
  ASSERT(f_tlv_cnt <= 20);
  if(FALSE == qmi_svc_put_param_tlv(response,
                                    QMI_PRM_TYPE_FAILED_TLV_LIST,
                                    f_tlv_cnt,
                                    failed_tlv_list))
  {
    dsm_free_packet(response);
    return QMI_ERR_NO_MEMORY;
  }

  *num_bytes_pushed += f_tlv_cnt + 3;

  if(FALSE == qmi_svc_put_param_tlv(response,
                                    QMI_PRM_TYPE_FLOW_INDEX,
                                    sizeof(uint8),
                                    &flow_indx))
  {
    dsm_free_packet(response);
    return QMI_ERR_NO_MEMORY;
  }

  *num_bytes_pushed += sizeof(uint8) + 3;
  return QMI_ERR_NONE;

} /* qosi_generate_flow_failed_tlv_list() */



/*===========================================================================
  FUNCTION QOSI_GENERATE_FLOW_ERROR_TLVS()

  DESCRIPTION
    Generate a list of error tlvs for flows provided in QOS REQUEST/MODIFY

  PARAMETERS
    flow_spec  : ptr to a flow_spec
    response   : ptr to the response for the error tlvs to be appended to
    flow_indxs : ptr to flow index arrary

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qosi_generate_flow_error_tlvs
(
  ip_flow_spec_type *  flow_spec,
  dsm_item_type **     response,
  uint8 *              flow_indxs,
  uint16 *             num_bytes_pushed
)
{
  uint16 cnt;
  uint8 i;
  uint8 type;
  qmi_error_e_type errval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *num_bytes_pushed = 0;
  type = QMI_PRM_TYPE_QOS_FLOW_SPEC_ERR;

  if(flow_spec->req_flow.err_mask != 0)
  {
    /*-------------------------------------------------------------------------
      Build the response from back to front
    -------------------------------------------------------------------------*/
    errval = qosi_generate_flow_failed_tlv_list(&flow_spec->req_flow,
                                                response,
                                                flow_indxs[0],
                                                &cnt);
    if(errval != QMI_ERR_NONE)
    {
      return errval;
    }

    if(cnt > 0)
    {
      if(!((QMI_SVC_PKT_PUSH(response, &cnt, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      *num_bytes_pushed += cnt + 3;
    }
  }

  for(i=0; i<flow_spec->num_aux_flows; i++)
  {
    if(flow_spec->aux_flow_list_ptr[i-1].err_mask != 0)
    {
      errval = qosi_generate_flow_failed_tlv_list((flow_spec->aux_flow_list_ptr + i),
                                                  response,
                                                  flow_indxs[i+1],
                                                  &cnt);
      if(errval != QMI_ERR_NONE)
      {
        return errval;
      }

      if(cnt > 0)
      {
        if(!((QMI_SVC_PKT_PUSH(response, &cnt, sizeof(uint16))) &&
             (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        *num_bytes_pushed += cnt + 3;
      }
    }
  }

  if(flow_spec->min_req_flow.err_mask != 0)
  {
    errval = qosi_generate_flow_failed_tlv_list(&flow_spec->min_req_flow,
                                                response,
                                                flow_indxs[flow_spec->num_aux_flows+1],
                                                &cnt);
    if(errval != QMI_ERR_NONE)
    {
      return errval;
    }

    if(cnt > 0)
    {
      if(!((QMI_SVC_PKT_PUSH(response, &cnt, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      *num_bytes_pushed += cnt + 3;
    }
  }
  return QMI_ERR_NONE;
} /* qosi_generate_flow_error_tlvs() */



/*===========================================================================
  FUNCTION QOSI_GENERATE_FILTER_FAILED_TLV_LIST()

  DESCRIPTION
    Generate a list of error tlvs for filters provided in QOS REQUEST/MODIFY

  PARAMETERS
    filter    : ptr to filter parameters
    response  : response ptr to append the error tlvs to
    fltr_indx : index of the given flow

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qosi_generate_filter_failed_tlv_list
(
  ip_filter_type *    filter,
  dsm_item_type **  response,
  uint8             fltr_indx,
  uint16         *   num_bytes_pushed
)
{
  uint8  failed_tlv_list[20];
  uint8  f_tlv_cnt;
  uint8  tmp_next_hdr_prot = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *num_bytes_pushed = 0;
  f_tlv_cnt = 0;
  memset(failed_tlv_list, 0, sizeof(failed_tlv_list));

  /*-------------------------------------------------------------------------
    Report any error in IP Version here, because it is not reported by
    PS through error masks
  -------------------------------------------------------------------------*/
  if((filter->ip_vsn != IP_V4) && (filter->ip_vsn != IP_V6))
  {
    failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_IP_VSN;
  }

  if(filter->ip_vsn == IP_V4)
  {
    if(filter->ip_hdr.v4.err_mask != 0)
    {
      if(filter->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_SRC_ADDR)
      {
        failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_IPV4_SRC_ADDR;
      }
      if(filter->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_DST_ADDR)
      {
        failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_IPV4_DST_ADDR;
      }
      if(filter->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
      {
        failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_IP_NEXT_HDR_PROT;
      }
      if(filter->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_TOS)
      {
        failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_IPV4_TOS;
      }
    }
    tmp_next_hdr_prot = filter->ip_hdr.v4.next_hdr_prot;
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if(filter->ip_vsn == IP_V6)
  {
    if(filter->ip_hdr.v6.err_mask != 0)
    {
      if(filter->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_SRC_ADDR)
      {
        failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_IPV6_SRC_ADDR;
      }
      if(filter->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_DST_ADDR)
      {
        failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_IPV6_DST_ADDR;
      }
      if(filter->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
      {
        failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_IP_NEXT_HDR_PROT;
      }
      if(filter->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
      {
        failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_IPV6_FLOW_LABEL;
      }
      if(filter->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
      {
        failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_IPV6_TRAFFIC_CLASS;
      }
    }
    tmp_next_hdr_prot = filter->ip_hdr.v6.next_hdr_prot;
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  switch(tmp_next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
      if(filter->next_prot_hdr.tcp.err_mask != 0)
      {
        if(filter->next_prot_hdr.tcp.err_mask & IPFLTR_MASK_TCP_SRC_PORT)
        {
          failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_TCP_SRC_PORT;
        }
        if(filter->next_prot_hdr.tcp.err_mask & IPFLTR_MASK_TCP_DST_PORT)
        {
          failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_TCP_DST_PORT;
        }
      }
      break;

    case PS_IPPROTO_UDP:
      if(filter->next_prot_hdr.udp.err_mask != 0)
      {
        if(filter->next_prot_hdr.udp.err_mask & IPFLTR_MASK_UDP_SRC_PORT)
        {
          failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_UDP_SRC_PORT;
        }
        if(filter->next_prot_hdr.udp.err_mask & IPFLTR_MASK_UDP_DST_PORT)
        {
          failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_UDP_DST_PORT;
        }
      }
      break;
     case PS_IPPROTO_TCP_UDP:
      if(filter->next_prot_hdr.tcp_udp_port_range.err_mask != 0)
      {
        if(filter->next_prot_hdr.tcp_udp_port_range.err_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT)
        {
          failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_TCP_UDP_SRC_PORT;
        }
        if(filter->next_prot_hdr.tcp_udp_port_range.err_mask & IPFLTR_MASK_TCP_UDP_DST_PORT)
        {
          failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_TCP_UDP_DST_PORT;
        }
      }
      break;

    case PS_IPPROTO_ICMP:
      if(filter->next_prot_hdr.icmp.err_mask != 0)
      {
        if(filter->next_prot_hdr.icmp.err_mask & IPFLTR_MASK_ICMP_MSG_TYPE)
        {
          failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_ICMP_MSG_TYPE;
        }
        if(filter->next_prot_hdr.icmp.err_mask & IPFLTR_MASK_ICMP_MSG_CODE)
        {
          failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_ICMP_MSG_CODE;
        }
      }
      break;

    case PS_IPPROTO_ESP:
      if(filter->next_prot_hdr.esp.err_mask != 0)
      {
        if(filter->next_prot_hdr.esp.err_mask & IPFLTR_MASK_ESP_SPI)
        {
          failed_tlv_list[f_tlv_cnt++] = QMI_PRM_TYPE_ESP_SPI;
        }
      }
      break;

  }

  if(f_tlv_cnt == 0)
  {
    return QMI_ERR_NONE; //no tlv to send
  }

  ASSERT(f_tlv_cnt <= 20);

  if(FALSE == qmi_svc_put_param_tlv(response,
                                    QMI_PRM_TYPE_FAILED_TLV_LIST,
                                    f_tlv_cnt,
                                    failed_tlv_list))
  {
    dsm_free_packet(response);
    return QMI_ERR_NO_MEMORY;
  }

  *num_bytes_pushed += f_tlv_cnt + 3;

  if(FALSE == qmi_svc_put_param_tlv(response,
                                    QMI_PRM_TYPE_FILTER_INDEX,
                                    sizeof(uint8),
                                    &fltr_indx))
  {
    dsm_free_packet(response);
    return QMI_ERR_NO_MEMORY;
  }

  *num_bytes_pushed += sizeof(uint8) + 3;

  return QMI_ERR_NONE;
} /* qosi_generate_filter_failed_tlv_list() */



/*===========================================================================
  FUNCTION QOSI_GENERATE_FILTER_ERROR_TLVS()

  DESCRIPTION
    Generate a list of error tlvs for filters provided in QOS REQUEST/MODIFY

  PARAMETERS
    fltr_spec  : ptr to filter spec
    response   : ptr to response
    fltr_indxs : ptr to filter index array

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qosi_generate_filter_error_tlvs
(
  ip_filter_spec_type *  fltr_spec,
  dsm_item_type **       response,
  uint8 *                fltr_indxs,
  uint16 *               num_bytes_pushed
)
{
  uint16 cnt;
  uint8  i;
  uint8  type;
  qmi_error_e_type errval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  *num_bytes_pushed = 0;
  type = QMI_PRM_TYPE_QOS_FILTER_SPEC_ERR;

  if(fltr_spec->num_filters > QOSI_MAX_QOS_FLTRS_PER_REQUEST)
  {
    return QMI_ERR_MALFORMED_MSG;
  }

  for(i=0; i<fltr_spec->num_filters; i++)
  {
    errval = qosi_generate_filter_failed_tlv_list((fltr_spec->list_ptr + i),
                                                  response,
                                                  fltr_indxs[i],
                                                  &cnt);

    if(errval != QMI_ERR_NONE)
    {
      return errval;
    }

    if(cnt > 0)
    {
      if(!((QMI_SVC_PKT_PUSH(response, &cnt, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      *num_bytes_pushed += cnt + 3;
    }
  }
  return QMI_ERR_NONE;
} /* qosi_generate_filter_error_tlvs() */

/*===========================================================================
  FUNCTION QOSI_CONVERT_FLOW_SPEC_TO_PS()

  DESCRIPTION
    Convert the flow spec from QMI format to PS format

  PARAMETERS
    ps_flow_max_size : Size of ps_flows array
    ps_flows  : ptr to array of PS flows
    num_flow  : number of valid qmi_flows
    qmi_flows : ptr to array of flows in QMI format

  RETURN VALUE
    QMI_ERR_NONE_V01 for SUCCESS
    Appropriate QMI error code otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qosi_convert_flow_spec_to_ps
(
  uint8                ps_flow_max_size,
  ip_flow_type *       ps_flows,
  uint32               num_flows,
  qos_flow_type_v01 *  qmi_flows
)
{
  uint8  itr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(itr=0; itr<num_flows && itr<ps_flow_max_size; itr++)
  {
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_TRF_CLASS_V01)
    {
      ps_flows[itr].trf_class = 
        (ip_traffic_class_enum_type)qmi_flows[itr].ip_flow_trf_cls;
      ps_flows[itr].field_mask |= IPFLOW_MASK_TRF_CLASS;
    }
    /* Only 64 bit date rate*/
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_DATA_RATE_MIN_MAX_V01)
    {
      ps_flows[itr].data_rate.format.min_max_ex.guaranteed_rate = 
        qmi_flows[itr].data_rate_min_max.guaranteed_rate;
      ps_flows[itr].data_rate.format.min_max_ex.max_rate =
        qmi_flows[itr].data_rate_min_max.data_rate_max;
      ps_flows[itr].data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_EX_TYPE;
      ps_flows[itr].field_mask |= IPFLOW_MASK_DATA_RATE;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_DATA_RATE_TOK_BUCKET_V01)
    {
      ps_flows[itr].data_rate.format_type = DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE;
      ps_flows[itr].data_rate.format.token_bucket.peak_rate = 
        qmi_flows[itr].data_rate_token_bucket.peak_rate;
      ps_flows[itr].data_rate.format.token_bucket.token_rate = 
        qmi_flows[itr].data_rate_token_bucket.token_rate;
      ps_flows[itr].data_rate.format.token_bucket.size = 
        qmi_flows[itr].data_rate_token_bucket.bucket_size;
      ps_flows[itr].field_mask |= IPFLOW_MASK_DATA_RATE;
    }   
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_LATENCY_V01)
    {
      ps_flows[itr].latency = qmi_flows[itr].ip_flow_latency;
      ps_flows[itr].field_mask |= IPFLOW_MASK_LATENCY;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_JITTER_V01)
    {
      ps_flows[itr].latency_var = qmi_flows[itr].ip_flow_jitter;
      ps_flows[itr].field_mask |= IPFLOW_MASK_LATENCY_VAR;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_PKT_ERR_RATE_V01)
    {
      ps_flows[itr].pkt_err_rate.multiplier = 
        qmi_flows[itr].ip_flow_pkt_error_rate.ip_flow_pkt_error_rate_multiplier;
      ps_flows[itr].pkt_err_rate.exponent = 
        qmi_flows[itr].ip_flow_pkt_error_rate.ip_flow_pkt_error_rate_exponent;
      ps_flows[itr].field_mask |= IPFLOW_MASK_PKT_ERR_RATE;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_MIN_POLICED_PKT_SIZE_V01)
    {
      ps_flows[itr].min_policed_pkt_size = qmi_flows[itr].ip_flow_min_policed_packet_size;
      ps_flows[itr].field_mask |= IPFLOW_MASK_MIN_POLICED_PKT_SIZE;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_MAX_ALLOWED_PKT_SIZE_V01)
    {
      ps_flows[itr].max_allowed_pkt_size = qmi_flows[itr].ip_flow_max_allowed_packet_size;
      ps_flows[itr].field_mask |= IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_3GPP_RES_BER_V01)
    {
      ps_flows[itr].umts_params.res_ber = 
        (umts_residual_ber_enum_type)qmi_flows[itr].ip_flow_3gpp_residual_bit_error_rate;
      ps_flows[itr].field_mask |= IPFLOW_MASK_UMTS_RES_BER;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_3GPP_TRF_PRI_V01)
    {
      ps_flows[itr].umts_params.trf_pri = 
        (umts_trf_handling_pri_enum_type)qmi_flows[itr].ip_flow_3gpp_traffic_handling_priority;
      ps_flows[itr].field_mask |= IPFLOW_MASK_UMTS_TRF_PRI;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_3GPP2_PROFILE_ID_V01)
    {
      ps_flows[itr].cdma_params.profile_id = 
        qmi_flows[itr].ip_flow_3gpp2_profile_id;
      ps_flows[itr].field_mask |= IPFLOW_MASK_CDMA_PROFILE_ID;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_3GPP2_FLOW_PRIORITY_V01)
    {
      ps_flows[itr].cdma_params.flow_priority = 
        qmi_flows[itr].ip_flow_3gpp2_flow_priority;
      ps_flows[itr].field_mask |= IPFLOW_MASK_CDMA_FLOW_PRIORITY;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_3GPP_IM_CN_FLAG_V01)
    {
      ps_flows[itr].umts_params.im_cn_flag = 
        qmi_flows[itr].ip_flow_3gpp_im_cn_flag;
      ps_flows[itr].field_mask |= IPFLOW_MASK_UMTS_IM_CN_FLAG;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_3GPP_SIG_IND_V01)
    {
      ps_flows[itr].umts_params.sig_ind = 
        qmi_flows[itr].ip_flow_3gpp_sig_ind;
      ps_flows[itr].field_mask |= IPFLOW_MASK_UMTS_SIG_IND;
    }
    if (qmi_flows[itr].flow_valid_params & QMI_QOS_IP_FLOW_MASK_LTE_QCI_V01)
    {
      ps_flows[itr].lte_params.lte_qci = 
        (lte_qci_enum_type)qmi_flows[itr].ip_flow_lte_qci;
      ps_flows[itr].field_mask |= IPFLOW_MASK_LTE_QCI;
    }
  }
  return QMI_ERR_NONE_V01;
} /* qosi_convert_flow_spec_to_ps() */

/*===========================================================================
  FUNCTION QOSI_CONVERT_FILTER_SPEC_TO_PS()

  DESCRIPTION
    Convert the filter spec from QMI format to PS format

  PARAMETERS
    ps_filter_max_size : Size of ps_filters array
    ps_filters  : ptr to array of PS filters
    num_filters  : number of valid qmi_filters
    qmi_filters : ptr to array of filters in QMI format

  RETURN VALUE
    QMI_ERR_NONE_V01 for SUCCESS
    Appropriate QMI error code otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qosi_convert_filter_spec_to_ps
(
  uint8                       ps_filter_max_size,
  ip_filter_type *            ps_filters,
  uint32                      num_filters,
  qos_filter_rule_type_v01 *  qmi_filters
)
{
  uint8  itr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(itr=0; itr<num_filters && itr<ps_filter_max_size; itr++)
  {
    ps_filters[itr].ip_vsn = qmi_filters[itr].ip_info.ip_version;
    if(qmi_filters[itr].ip_info.ip_version == 
                                   QOS_IP_FAMILY_IPV6_V01)
    {
      qosi_fill_v6_filter_spec(&(qmi_filters[itr]),
                               &ps_filters[itr]); 
    }
    else if(qmi_filters[itr].ip_info.ip_version == 
                                   QOS_IP_FAMILY_IPV4_V01)
    {
      qosi_fill_v4_filter_spec(&(qmi_filters[itr]),
                               &ps_filters[itr]); 
    }
  }
  return QMI_ERR_NONE_V01;
} /* qosi_convert_filter_spec_to_ps() */

/*===========================================================================
  FUNCTION QOSI_FILL_V4_FILTER_SPEC()

  DESCRIPTION
   fills filter_spec based on the masks set in filter rules.
    Values are filled based on valid parameters value.
    
  PARAMETERS
    filter_rules - filter rules sent from client
    fltr_spec - filter rules which will be passed to ps_iface

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qosi_fill_v4_filter_spec
(
  qos_filter_rule_type_v01* filter_rules,
  ip_filter_type*           fltr_spec
)
{
  ASSERT(filter_rules && fltr_spec);
  
  if(filter_rules->ip_info.v4_info.valid_params == QMI_QOS_IPV4_FILTER_MASK_NONE_V01)
  {
    fltr_spec->ip_hdr.v4.field_mask = IPFLTR_MASK_IP4_NONE;	
  }
  else
  {
    if(filter_rules->ip_info.v4_info.valid_params & 
                      QMI_QOS_IPV4_FILTER_MASK_SRC_ADDR_V01)
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_SRC_ADDR;
      fltr_spec->ip_hdr.v4.src.addr.ps_s_addr = 
              ps_htonl(filter_rules->ip_info.v4_info.src_addr.ipv4_addr);
      fltr_spec->ip_hdr.v4.src.subnet_mask.ps_s_addr =
              ps_htonl(filter_rules->ip_info.v4_info.src_addr.subnet_mask);
      IPV4_ADDR_MSG(fltr_spec->ip_hdr.v4.src.addr.ps_s_addr);
      IPV4_ADDR_MSG(fltr_spec->ip_hdr.v4.src.subnet_mask.ps_s_addr);
    }
    if(filter_rules->ip_info.v4_info.valid_params & 
                      QMI_QOS_IPV4_FILTER_MASK_DEST_ADDR_V01)
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_DST_ADDR;
      fltr_spec->ip_hdr.v4.dst.addr.ps_s_addr = 
              ps_htonl(filter_rules->ip_info.v4_info.dest_addr.ipv4_addr);
      fltr_spec->ip_hdr.v4.dst.subnet_mask.ps_s_addr = 
              ps_htonl(filter_rules->ip_info.v4_info.dest_addr.subnet_mask);			  
      IPV4_ADDR_MSG(fltr_spec->ip_hdr.v4.dst.addr.ps_s_addr);
      IPV4_ADDR_MSG(fltr_spec->ip_hdr.v4.dst.subnet_mask.ps_s_addr);
    }
    if(filter_rules->ip_info.v4_info.valid_params & 
                      QMI_QOS_IPV4_FILTER_MASK_TOS_V01)
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
      fltr_spec->ip_hdr.v4.tos.val     = filter_rules->ip_info.v4_info.tos.val;
      fltr_spec->ip_hdr.v4.tos.mask    = filter_rules->ip_info.v4_info.tos.mask;
      LOG_MSG_INFO1_2("IP4  TOS value %d mask %d ",
                       fltr_spec->ip_hdr.v4.tos.val,
                       fltr_spec->ip_hdr.v4.tos.mask);
    }
  }
  if(filter_rules->xport_info.xport_protocol != QOS_PROTO_NONE_V01) 
  {
    fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
    fltr_spec->ip_hdr.v4.next_hdr_prot = 
               filter_rules->xport_info.xport_protocol;
    qosi_fill_fltr_xport_info(&(filter_rules->xport_info),fltr_spec);
  }
} /* qosi_fill_v4_filter_spec */

/*===========================================================================
  FUNCTION QOSI_FILL_V6_FILTER_SPEC()

  DESCRIPTION
   fills V6 filter_spec based on the masks set in filter rules.
   Values are filled based on valid parameters value.
    
  PARAMETERS
    filter_rules - filter rules sent from client
    fltr_spec - filter rules which will be passed to ps_iface

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_fill_v6_filter_spec
(
  qos_filter_rule_type_v01* filter_rules,
  ip_filter_type*           fltr_spec
)
{
  ASSERT(filter_rules && fltr_spec);

  if(filter_rules->ip_info.v6_info.valid_params == 
                          QMI_QOS_IPV6_FILTER_MASK_NONE_V01)
  {
    fltr_spec->ip_hdr.v6.field_mask = IPFLTR_MASK_IP6_NONE;		    
  }
  else
  {
    if(filter_rules->ip_info.v6_info.valid_params & 
                           QMI_QOS_IPV6_FILTER_MASK_SRC_ADDR_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
      memscpy(&fltr_spec->ip_hdr.v6.src.addr,sizeof(fltr_spec->ip_hdr.v6.src.addr),
    	&filter_rules->ip_info.v6_info.src_addr.ipv6_address,
    	sizeof(filter_rules->ip_info.v6_info.src_addr.ipv6_address));
      fltr_spec->ip_hdr.v6.src.prefix_len = 
         filter_rules->ip_info.v6_info.src_addr.prefix_len;
      IPV6_ADDR_MSG(fltr_spec->ip_hdr.v6.src.addr.in6_u.u6_addr64);
      LOG_MSG_INFO1_1("IPV6 prefix len %d",fltr_spec->ip_hdr.v6.src.prefix_len );
    }
    if(filter_rules->ip_info.v6_info.valid_params & 
                            QMI_QOS_IPV6_FILTER_MASK_DEST_ADDR_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_DST_ADDR;
      memscpy(&fltr_spec->ip_hdr.v6.dst.addr,sizeof(fltr_spec->ip_hdr.v6.dst.addr),
         &filter_rules->ip_info.v6_info.dest_addr.ipv6_address,
    	 sizeof(filter_rules->ip_info.v6_info.dest_addr.ipv6_address));
      fltr_spec->ip_hdr.v6.dst.prefix_len = 
         filter_rules->ip_info.v6_info.dest_addr.prefix_len;			  
      IPV6_ADDR_MSG(fltr_spec->ip_hdr.v6.dst.addr.in6_u.u6_addr64);
      LOG_MSG_INFO1_1("IPV6 prefix len %d",fltr_spec->ip_hdr.v6.dst.prefix_len );
    }
    if(filter_rules->ip_info.v6_info.valid_params & 
                            QMI_QOS_IPV6_FILTER_MASK_TRAFFIC_CLASS_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
      fltr_spec->ip_hdr.v6.trf_cls.val = 
         filter_rules->ip_info.v6_info.trf_cls.val;
      fltr_spec->ip_hdr.v6.trf_cls.mask = 
         filter_rules->ip_info.v6_info.trf_cls.mask;
      LOG_MSG_INFO1_2("IPV6 trf class value %d mask %d",
                        fltr_spec->ip_hdr.v6.trf_cls.val,
                        fltr_spec->ip_hdr.v6.trf_cls.mask);
    }
    if(filter_rules->ip_info.v6_info.valid_params & 
                             QMI_QOS_IPV6_FILTER_MASK_FLOW_LABEL_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_FLOW_LABEL;
      fltr_spec->ip_hdr.v6.flow_label = 
          ps_htonl(filter_rules->ip_info.v6_info.flow_label);
    }
  }
  if(filter_rules->xport_info.xport_protocol != QOS_PROTO_NONE_V01) 
  {
    fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
    fltr_spec->ip_hdr.v6.next_hdr_prot = 
       filter_rules->xport_info.xport_protocol;
    qosi_fill_fltr_xport_info(&(filter_rules->xport_info),fltr_spec);
  }    			
} /* qosi_fill_v6_filter_spec */

/*===========================================================================
  FUNCTION QOSI_FILL_FLTR_XPORT_INFO()

  DESCRIPTION
   fills protocols xport info based on masks set in filter rules.
   Filterspec will be filled based on valid parameters in the xport_info.
    
  PARAMETERS
    xport_info - protocol inforamtion
    fltr_spec - filter rules which will be passed to ps_iface

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_fill_fltr_xport_info
(
  qos_xport_header_type_v01* xport_info,
  ip_filter_type*            fltr_spec
)
{
  ASSERT(xport_info && fltr_spec);
  
  LOG_MSG_INFO1_1("protocol = %d",xport_info->xport_protocol);
  switch( xport_info->xport_protocol )
  {
    case QOS_PROTO_ICMP_V01:
    case QOS_PROTO_ICMPV6_V01:
    {
      if(xport_info->icmp_info.valid_params != 
                    QMI_QOS_ICMP_FILTER_MASK_NONE_V01)
      {
        if(xport_info->icmp_info.valid_params & 
                    QMI_QOS_ICMP_FILTER_MASK_MSG_TYPE_V01)
        {
          fltr_spec->next_prot_hdr.icmp.field_mask |= IPFLTR_MASK_ICMP_MSG_TYPE;
          fltr_spec->next_prot_hdr.icmp.type = xport_info->icmp_info.type;
        }
        if(xport_info->icmp_info.valid_params & 
                    QMI_QOS_ICMP_FILTER_MASK_MSG_CODE_V01)
        {
          fltr_spec->next_prot_hdr.icmp.field_mask |= IPFLTR_MASK_ICMP_MSG_CODE;
          fltr_spec->next_prot_hdr.icmp.code = xport_info->icmp_info.code;
        }
        LOG_MSG_INFO1_3("ICMP valid params (%d) type (%d) code (%d)",
				      xport_info->icmp_info.valid_params,
				      xport_info->icmp_info.type,
				      xport_info->icmp_info.code);
      }
    }
    break;
    case QOS_PROTO_TCP_V01:
    {
      if(xport_info->tcp_info.valid_params != 
                     QMI_QOS_PORT_INFO_FILTER_MASK_NONE_V01)
      {
        LOG_MSG_INFO1_1("tcp valid params (%d)",xport_info->tcp_info.valid_params);
        if(xport_info->tcp_info.valid_params & 
                     QMI_QOS_PORT_INFO_FILTER_MASK_SRC_PORT_V01)
        {
          fltr_spec->next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_SRC_PORT;
          fltr_spec->next_prot_hdr.tcp.src.port  = 
         	ps_htons(xport_info->tcp_info.src_port_info.port);
          fltr_spec->next_prot_hdr.tcp.src.range = 
         	xport_info->tcp_info.src_port_info.range;
          LOG_MSG_INFO1_2("tcp src port (%d) range (%d)",
                           fltr_spec->next_prot_hdr.tcp.src.port,
                           fltr_spec->next_prot_hdr.tcp.src.range);
        }
        if(xport_info->tcp_info.valid_params & 
                QMI_QOS_PORT_INFO_FILTER_MASK_DEST_PORT_V01)
        {
          fltr_spec->next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_DST_PORT;
          fltr_spec->next_prot_hdr.tcp.dst.port  = 
                           ps_htons(xport_info->tcp_info.dest_port_info.port);
          fltr_spec->next_prot_hdr.tcp.dst.range = 
                           xport_info->tcp_info.dest_port_info.range;
          LOG_MSG_INFO1_2("tcp dst port (%d) range (%d)",
                          fltr_spec->next_prot_hdr.tcp.dst.port,
                          fltr_spec->next_prot_hdr.tcp.dst.range);
        }
      }	
    }
    break;
    case QOS_PROTO_UDP_V01:
    {
      if(xport_info->udp_info.valid_params != 
                    QMI_QOS_PORT_INFO_FILTER_MASK_NONE_V01)
      {
        LOG_MSG_INFO1_1("udp valid params (%d)",
                        xport_info->udp_info.valid_params);
        if(xport_info->udp_info.valid_params & 
                     QMI_QOS_PORT_INFO_FILTER_MASK_SRC_PORT_V01)
        {
          fltr_spec->next_prot_hdr.udp.field_mask |= 
                              IPFLTR_MASK_UDP_SRC_PORT;
          fltr_spec->next_prot_hdr.udp.src.port  = 
        			   ps_htons(xport_info->udp_info.src_port_info.port);
          fltr_spec->next_prot_hdr.udp.src.range = 
         		       xport_info->udp_info.src_port_info.range;
          LOG_MSG_INFO1_2("udp src port (%d) range (%d)",
                           fltr_spec->next_prot_hdr.udp.src.port,
                           fltr_spec->next_prot_hdr.udp.src.range);
        }
        if(xport_info->udp_info.valid_params &
                      QMI_QOS_PORT_INFO_FILTER_MASK_DEST_PORT_V01)
        {
          fltr_spec->next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;
          fltr_spec->next_prot_hdr.udp.dst.port  = 
          		   ps_htons(xport_info->udp_info.dest_port_info.port);
          fltr_spec->next_prot_hdr.udp.dst.range = 
          	       xport_info->udp_info.dest_port_info.range;
          LOG_MSG_INFO1_2("udp dst port (%d) range (%d)",
                          fltr_spec->next_prot_hdr.udp.dst.port,
                          fltr_spec->next_prot_hdr.udp.dst.range);
        }  
      }	
    }
    break;
    case QOS_PROTO_ESP_V01:
    {
      if(xport_info->esp_info.valid_params != QMI_QOS_IPSEC_FILTER_MASK_NONE_V01)
      {
        if(xport_info->esp_info.valid_params & QMI_QOS_IPSEC_FILTER_MASK_SPI_V01)
        {
          fltr_spec->next_prot_hdr.esp.field_mask |= IPFLTR_MASK_ESP_SPI;
          fltr_spec->next_prot_hdr.esp.spi  = xport_info->esp_info.spi;
          LOG_MSG_INFO1_1("esp spi (%d)",fltr_spec->next_prot_hdr.esp.spi);			  
        }
      }	
    }
    break;
    case QOS_PROTO_AH_V01:
    {
      if(xport_info->ah_info.valid_params != QMI_QOS_IPSEC_FILTER_MASK_NONE_V01)
      {
        if(xport_info->ah_info.valid_params & QMI_QOS_IPSEC_FILTER_MASK_SPI_V01)
        {
          fltr_spec->next_prot_hdr.ah.field_mask |= IPFLTR_MASK_ESP_SPI;
          fltr_spec->next_prot_hdr.ah.spi  = xport_info->ah_info.spi;
          LOG_MSG_INFO1_1("ah spi (%d)",fltr_spec->next_prot_hdr.ah.spi);			  
        }
      }	
    }
    break;
    default:
      break;
  }
} /* qosi_fill_fltr_xport_info */

/*===========================================================================
  FUNCTION QOSI_CONVERT_FLOW_ERROR_MASK()

  DESCRIPTION
    Converts the flow error mask from PS format to QMI format

  PARAMETERS
    flow  : ptr to a flow, this contains error mask aslo
    flow_error_mask : QMI flow error mask

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qosi_convert_flow_error_mask
(
  ip_flow_type *            flow,
  qos_ip_flow_params_mask_v01 *  flow_error_mask
)
{
  memset(flow_error_mask, 0, sizeof(qos_ip_flow_params_mask_v01));

  if(flow->err_mask & IPFLOW_MASK_TRF_CLASS)
  {
    *flow_error_mask = QMI_QOS_IP_FLOW_MASK_TRF_CLASS_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_DATA_RATE)
  {
    if((flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE) ||
       (flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE))
    {
      *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_DATA_RATE_MIN_MAX_V01;
    }
    else
    {
      *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_DATA_RATE_TOK_BUCKET_V01;
    }
  }
  if(flow->err_mask & IPFLOW_MASK_LATENCY)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_LATENCY_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_LATENCY_VAR)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_JITTER_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_PKT_ERR_RATE)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_PKT_ERR_RATE_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_MIN_POLICED_PKT_SIZE)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_MIN_POLICED_PKT_SIZE_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_MAX_ALLOWED_PKT_SIZE_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_UMTS_RES_BER)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_3GPP_RES_BER_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_UMTS_TRF_PRI)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_3GPP_TRF_PRI_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_CDMA_PROFILE_ID)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_3GPP2_PROFILE_ID_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_CDMA_FLOW_PRIORITY)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_3GPP2_FLOW_PRIORITY_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_UMTS_IM_CN_FLAG)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_3GPP_IM_CN_FLAG_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_UMTS_SIG_IND)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_3GPP_SIG_IND_V01;
  }
  if(flow->err_mask & IPFLOW_MASK_LTE_QCI)
  {
    *flow_error_mask |= QMI_QOS_IP_FLOW_MASK_LTE_QCI_V01;
  }
} /* qosi_convert_flow_error_mask */

/*===========================================================================
  FUNCTION QOSI_FILL_FLOW_ERROR_INFO()

  DESCRIPTION
    Generate a list of error tlvs for flows provided in QOS REQUEST/MODIFY

  PARAMETERS
    flow_spec  : ptr to a flow_spec
    flow_error_valid : TRUE if any invalid flow is detected
    flow_error_len   : Number of valid flow error entries
    qos_flow_error : Array of masks containing the flow errors

  RETURN VALUE
    qmi error code

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qosi_fill_flow_error_info
(
  ip_flow_spec_type *            flow_spec,
  uint8 *                        flow_error_valid,
  uint32 *                       flow_error_len,
  qos_ip_flow_params_mask_v01 *  qos_flow_error
)
{
  uint8 itr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(flow_spec->req_flow.err_mask != 0)
  {
    /*-------------------------------------------------------------------------
      Build the response from back to front
    -------------------------------------------------------------------------*/
    qosi_convert_flow_error_mask(&flow_spec->req_flow,
                                 &qos_flow_error[0]);

    *flow_error_valid = TRUE;
  }

  for(itr=0; itr<flow_spec->num_aux_flows; itr++)
  {
    if(flow_spec->aux_flow_list_ptr[itr-1].err_mask != 0) //why itr-1?
    {
      qosi_convert_flow_error_mask((flow_spec->aux_flow_list_ptr + itr),
                                   &qos_flow_error[itr+1]);
      *flow_error_valid = TRUE;
    }
  }

  if(flow_spec->min_req_flow.err_mask != 0)
  {
    qosi_convert_flow_error_mask(&flow_spec->min_req_flow,
                                 &qos_flow_error[flow_spec->num_aux_flows+1]);
    *flow_error_valid = TRUE;
  }
  if (*flow_error_valid == TRUE)
  {
    *flow_error_len = 1 + flow_spec->num_aux_flows;
  }
  return QMI_ERR_NONE_V01;
} /* qosi_fill_flow_error_info() */

/*===========================================================================
  FUNCTION QOSI_FILL_FLOW_ERROR_INFO()

  DESCRIPTION
    Generate a list of error tlvs for filters provided in QOS REQUEST/MODIFY

  PARAMETERS
    filter_spec  : ptr to a filter_spec
    filter_error_valid : TRUE if any invalid filter is detected
    filter_error_len   : Number of valid filter error entries
    qos_filter_error : Array of masks containing the filter errors

  RETURN VALUE
    qmi error code

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qosi_fill_filter_error_info
(
  ip_filter_spec_type *              filter_spec,
  uint8 *                            filter_error_valid,
  uint32 *                           filter_error_len,
  qos_filter_param_error_mask_v01 *  qos_filter_error
)
{
  uint8 itr;
  uint8    next_hdr_prot = QOS_PROTO_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(itr=0; (itr<filter_spec->num_filters) && (itr<QMI_QOS_MAX_QOS_FILTERS_V01); itr++)
  {
    if(filter_spec->list_ptr[itr].ip_vsn == IP_V4)
    {
      if(filter_spec->list_ptr[itr].ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_SRC_ADDR)
      {
        qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_IPV4_SRC_ADDR_V01;
      }
      if(filter_spec->list_ptr[itr].ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_DST_ADDR)
      {
        qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_IPV4_DEST_ADDR_V01;
      }
      if(filter_spec->list_ptr[itr].ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_TOS)
      {
        *qos_filter_error |= QMI_QOS_FILTER_PARAM_IPV4_TOS_V01;
      }  
      if(filter_spec->list_ptr[itr].ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
      {
        qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_XPORT_PROT_V01;
      }
      next_hdr_prot = filter_spec->list_ptr[itr].ip_hdr.v4.next_hdr_prot;
    }
    else if(filter_spec->list_ptr[itr].ip_vsn == IP_V6)
    {
      if(filter_spec->list_ptr[itr].ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_SRC_ADDR)
      {
        qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_IPV6_SRC_ADDR_V01;
      } 
      if(filter_spec->list_ptr[itr].ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_DST_ADDR)
      {
        qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_IPV6_DEST_ADDR_V01;
      }
      if(filter_spec->list_ptr[itr].ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
      {
        qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_IPV6_TRF_CLS_V01;
      } 	  
      if(filter_spec->list_ptr[itr].ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
      {
        qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_IPV6_FLOW_LABEL_V01;
      }
      next_hdr_prot = filter_spec->list_ptr[itr].ip_hdr.v6.next_hdr_prot;;
    }
    else
    {
      qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_IP_VERSION_V01;
    }
    switch(next_hdr_prot)
    {
      case QOS_PROTO_ICMP_V01:
      case QOS_PROTO_ICMPV6_V01:
      {
        if(filter_spec->list_ptr[itr].next_prot_hdr.icmp.err_mask & IPFLTR_MASK_ICMP_MSG_TYPE)
        {
          qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_ICMP_TYPE_V01;
        }
        if(filter_spec->list_ptr[itr].next_prot_hdr.icmp.err_mask & IPFLTR_MASK_ICMP_MSG_CODE)
        {
          qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_ICMP_CODE_V01;
        }			  
      }
      break;
      case QOS_PROTO_TCP_V01:
      {
        if(filter_spec->list_ptr[itr].next_prot_hdr.tcp.err_mask & IPFLTR_MASK_TCP_SRC_PORT)
        {
          qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_TCP_SRC_PORT_V01;
        }
        if(filter_spec->list_ptr[itr].next_prot_hdr.tcp.err_mask & IPFLTR_MASK_TCP_DST_PORT)
        {
          qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_TCP_DEST_PORT_V01;
        }			  
      }
      break;
      case QOS_PROTO_UDP_V01:
      {
        if(filter_spec->list_ptr[itr].next_prot_hdr.udp.err_mask & IPFLTR_MASK_UDP_SRC_PORT)
        {
          qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_UDP_SRC_PORT_V01;
        }
        if(filter_spec->list_ptr[itr].next_prot_hdr.udp.err_mask & IPFLTR_MASK_UDP_DST_PORT)
        {
          qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_UDP_DEST_PORT_V01;
        }			  
      }
      break;
      case QOS_PROTO_ESP_V01:
      {
        if(filter_spec->list_ptr[itr].next_prot_hdr.esp.err_mask & IPFLTR_MASK_ESP_SPI)
        {
          qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_ESP_SPI_V01;
        }
      }
      break;
      case QOS_PROTO_AH_V01:
      {
        if(filter_spec->list_ptr[itr].next_prot_hdr.ah.err_mask & IPFLTR_MASK_AH_SPI)
        {
          qos_filter_error[itr] |= QMI_QOS_FILTER_PARAM_AH_SPI_V01;
        }
      }
        break;
      default:
        break;
    } /* switch */
    if (qos_filter_error[itr] != QMI_QOS_FILTER_PARAM_NONE_V01)
    {
      *filter_error_valid = TRUE;
    }
  } /* for */
  if (*filter_error_valid == TRUE)
  {
    *filter_error_len = filter_spec->num_filters;
  }
  return QMI_ERR_NONE_V01;
} /* qosi_fill_filter_error_info() */


/*===========================================================================
  FUNCTION QOSI_PARSE_FLOW_SPECS()

  DESCRIPTION
    Function to parse the flow spec tlvs received in QoS Request/Modify
    and populate the internal flow structs

  PARAMETERS
    sdu_in         : incoming request
    len_lvl1       : length of the outermost tlv
    ip_flows_arr   : ptr to array of ip_flow_type (output)
    flow_indxs_arr : ptr to array of flow indexes (output)
    flow_count     : number of flows

  RETURN VALUE
    errval :  any errors while parsing

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qosi_parse_flow_specs
(
  dsm_item_type **  sdu_in,
  uint16            len_lvl1,
  ip_flow_type *    ip_flows_arr, //array of size MAX_FLOWs
  uint8 *           flow_indxs_arr,//array of size max flows
  uint8 *           flow_count
)
{
  void *     value;
  uint16     num_bytes_lvl2;
  uint16     num_bytes_lvl3;
  uint16     len_lvl2;
  uint16     len_lvl3;
  uint16     expected_len;
  uint8      cnt;
  uint8      type;
  boolean got_flow_index;
  boolean got_min_max_tlv;
  boolean got_min_max_ex_tlv;
  qmi_error_e_type  errval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  errval = QMI_ERR_NONE;
  num_bytes_lvl2 = 0;
  num_bytes_lvl3 = 0;
  cnt = 0;
  got_flow_index = FALSE;
  got_min_max_tlv = FALSE;
  got_min_max_ex_tlv = FALSE;

  while (*sdu_in)
  {
    if(cnt == QOSI_MAX_QOS_FLOWS_PER_REQUEST)
    {
      break;
    }
    if(num_bytes_lvl2 == len_lvl1)
    {
      break;
    }

    if( !qmi_svc_get_tl(sdu_in, &type, &len_lvl2) )
    {
      continue;
    }
    if(!(len_lvl2 < len_lvl1))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /* Check for overflow */
    if((MAX_UINT16 - len_lvl2 - 3) < num_bytes_lvl2)
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
    num_bytes_lvl2 += len_lvl2 + 3;


    /* Check if total number of bytes processed for nested TLV will exceed the 
      length value specified */
    if(num_bytes_lvl2 > len_lvl1)
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    value = NULL;
    switch(type)
    {
      case QMI_PRM_TYPE_QOS_FLOW_SPEC:
      {
        while (*sdu_in)
        {
          if(num_bytes_lvl3 == len_lvl2)
          {
            num_bytes_lvl3 = 0;
            break;
          }

          if( !qmi_svc_get_tl(sdu_in, &type, &len_lvl3) )
          {
            continue;
          }
          if(!(len_lvl3 < len_lvl2))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          /* Check for overflow */
          if((MAX_UINT16 - len_lvl3 - 3) < num_bytes_lvl3)
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          num_bytes_lvl3 += len_lvl3 + 3;

          if(num_bytes_lvl3 > len_lvl2)
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          expected_len = 0;
          value = NULL;
          switch(type)
          {
            case QMI_PRM_TYPE_FLOW_INDEX:
              expected_len = sizeof(uint8);
              value = (void*) &flow_indxs_arr[cnt];
              got_flow_index = TRUE;
            break;

            case QMI_PRM_TYPE_FLOW_TRF_CLASS:
              expected_len = QMI_PRM_TYPE_FLOW_TRF_CLASS_LEN;
              value = (void *)&ip_flows_arr[cnt].trf_class;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_TRF_CLASS;
              break;

            case QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX:
              expected_len = QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_LEN;
              value = (void*)&ip_flows_arr[cnt].data_rate.format.min_max;
              ip_flows_arr[cnt].data_rate.format_type =
                                               DATA_RATE_FORMAT_MIN_MAX_TYPE;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_DATA_RATE;
              got_min_max_tlv = TRUE;
              break;

            case QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_EX:
              expected_len = QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_EX_LEN;
              value = (void*)&ip_flows_arr[cnt].data_rate.format.min_max_ex;
              ip_flows_arr[cnt].data_rate.format_type =
                                               DATA_RATE_FORMAT_MIN_MAX_EX_TYPE;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_DATA_RATE;
              got_min_max_ex_tlv = TRUE;
              break;

            case QMI_PRM_TYPE_FLOW_DATA_R_TOKEN_BUK:
              expected_len = QMI_PRM_TYPE_FLOW_DATA_R_TOKEN_BUK_LEN;
              value = (void*)&ip_flows_arr[cnt].data_rate.format.token_bucket;
              ip_flows_arr[cnt].data_rate.format_type =
                                          DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_DATA_RATE;
              break;

            case QMI_PRM_TYPE_FLOW_LATENCY:
              expected_len = sizeof(ip_flows_arr[cnt].latency);
              value = (void*)&ip_flows_arr[cnt].latency;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_LATENCY;
              break;

            case QMI_PRM_TYPE_FLOW_JITTER:
              expected_len = sizeof(ip_flows_arr[cnt].latency_var);
              value = (void*)&ip_flows_arr[cnt].latency_var;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_LATENCY_VAR;
              break;

            case QMI_PRM_TYPE_FLOW_PKT_ERR_RATE:
              expected_len = QMI_PRM_TYPE_FLOW_PKT_ERR_RATE_LEN;
              value = (void*)&ip_flows_arr[cnt].pkt_err_rate;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_PKT_ERR_RATE;
              break;

            case QMI_PRM_TYPE_FLOW_MIN_POLICED_PKT_SIZE:
              expected_len = sizeof(ip_flows_arr[cnt].min_policed_pkt_size);
              value = (void*)&ip_flows_arr[cnt].min_policed_pkt_size;
              ip_flows_arr[cnt].field_mask |=
                                    IPFLOW_MASK_MIN_POLICED_PKT_SIZE;
              break;

            case QMI_PRM_TYPE_FLOW_MAX_ALLOWED_PKT_SIZE:
              expected_len = sizeof(ip_flows_arr[cnt].max_allowed_pkt_size);
              value = (void*)&ip_flows_arr[cnt].max_allowed_pkt_size;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE;
              break;

            case QMI_PRM_TYPE_FLOW_RES_BER:
              expected_len = QMI_PRM_TYPE_FLOW_RES_BER_LEN;
              value = (void*)&ip_flows_arr[cnt].umts_params.res_ber;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_UMTS_RES_BER;
              break;

            case QMI_PRM_TYPE_FLOW_TRF_PRI:
              expected_len = QMI_PRM_TYPE_FLOW_TRF_PRI_LEN;
              value = (void*)&ip_flows_arr[cnt].umts_params.trf_pri;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_UMTS_TRF_PRI;
              break;

            case QMI_PRM_TYPE_FLOW_PROFILE_ID:
              expected_len = sizeof(ip_flows_arr[cnt].cdma_params.profile_id);
              value = (void*)&ip_flows_arr[cnt].cdma_params.profile_id;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_CDMA_PROFILE_ID;
              break;

            case QMI_PRM_TYPE_FLOW_PRIORITY:
              expected_len = sizeof(ip_flows_arr[cnt].cdma_params.flow_priority);
              value = (void*)&ip_flows_arr[cnt].cdma_params.flow_priority;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_CDMA_FLOW_PRIORITY;
              break;

            case QMI_PRM_TYPE_FLOW_IM_CN_FLAG:
              expected_len = sizeof(ip_flows_arr[cnt].umts_params.im_cn_flag);
              value = (void*)&ip_flows_arr[cnt].umts_params.im_cn_flag;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_UMTS_IM_CN_FLAG;
              break;

            case QMI_PRM_TYPE_FLOW_SIG_IND:
              expected_len = sizeof(ip_flows_arr[cnt].umts_params.sig_ind);
              value = (void*)&ip_flows_arr[cnt].umts_params.sig_ind;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_UMTS_SIG_IND;
              break;

            case QMI_PRM_TYPE_FLOW_LTE_QCI:
              expected_len = QMI_PRM_TYPE_FLOW_LTE_QCI_LEN;
              value = (void*)&ip_flows_arr[cnt].lte_params.lte_qci;
              ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_LTE_QCI;
              break;

              /* future add WLAN user pri field */

            default:
              LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
              break;
          }

         if(!qmi_svc_validate_type_length( type,
                                           len_lvl3,
                                           expected_len,
                                           FILE_ID_DS_QMI_QOS,__LINE__) )
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          if(len_lvl3 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl3 ))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
        }

        /*If both min_max(32bit) and min_max_ex(64 bit) TLVs are present, 
        then the 64bit TLV is given precedence.*/
        if(got_min_max_ex_tlv == TRUE)
        {
          ip_flows_arr[cnt].data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_EX_TYPE;
          ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_DATA_RATE;
          /*Reset the min_max TLV*/
          ip_flows_arr[cnt].data_rate.format.min_max.max_rate = 0;
          ip_flows_arr[cnt].data_rate.format.min_max.guaranteed_rate = 0;
        }
        else
        {
          if(got_min_max_tlv == TRUE)
          {
            ip_flows_arr[cnt].data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
            ip_flows_arr[cnt].field_mask |= IPFLOW_MASK_DATA_RATE;
          }
        }
        got_min_max_ex_tlv = FALSE;
        got_min_max_tlv    = FALSE;

        cnt++; // increment the flow count
      } //case QMI_PRM_TYPE_QOS_FLOW_SPEC
      break;

      default:
      {
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);

        /* remove the payload for this unrecognized tlv */
        if(len_lvl2 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl2 ))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;
      } // case default
    }
    if(got_flow_index == FALSE)
    {
      errval = QMI_ERR_MISSING_ARG;
      goto send_result;
    }
    got_flow_index = FALSE;
  }
  /* Something wrong, len_lvl1 passed in does not match up, either because too
     many flows present or we ran out of bytes to process */
  if(num_bytes_lvl2 != len_lvl1)
  {
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }

  *flow_count = cnt;
send_result:
  return errval;
} /* qosi_parse_flow_specs() */



/*===========================================================================
  FUNCTION QOSI_PARSE_FILTER_SPECS()

  DESCRIPTION
    Function to parse the flow spec tlvs received in QoS Request/Modify
    and populate the internal flow structs

  PARAMETERS
    sdu_in         : incoming request
    len_lvl1       : length of the outermost tlv
    ip_fltrs_arr   : ptr to array of ip_filter_type (output)
    fltr_indxs_arr : ptr to array of filter indexes (output)
    fltr_count     : number of filters

  RETURN VALUE
    errval : error encountered during parsing

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qosi_parse_filter_specs
(
  dsm_item_type **  sdu_in,
  uint16            len_lvl1,
  ip_filter_type *  ip_fltrs_arr,
  uint8 *           fltr_indxs_arr,
  uint8 *           fltr_count
)
{
  void *     value;
  uint16     num_bytes_lvl2;
  uint16     num_bytes_lvl3;
  uint16     len_lvl2;
  uint16     len_lvl3;
  uint16     expected_len;
  qmi_error_e_type  errval;
  uint8 cnt;
  uint8 type;
  boolean got_filter_index, got_ip_vsn;
  uint8 tmp_next_hdr_prot = 0;
  boolean got_next_hdr_prot = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  errval = QMI_ERR_NONE;
  num_bytes_lvl2 = 0;
  num_bytes_lvl3 = 0;
  cnt = 0;
  got_filter_index = FALSE;
  got_ip_vsn = FALSE;

  while (*sdu_in)
  {
    if(cnt == QOSI_MAX_QOS_FLTRS_PER_REQUEST)
    {
      break;
    }
    if(num_bytes_lvl2 == len_lvl1)
    {
      break;
    }

    if( !qmi_svc_get_tl(sdu_in, &type, &len_lvl2) )
    {
      continue;
    }
    if(!(len_lvl2 < len_lvl1))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /* Check for overflow */
    if((MAX_UINT16 - len_lvl2 - 3) < num_bytes_lvl2)
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
    num_bytes_lvl2 += len_lvl2 + 3;

    /* Check if total number of bytes processed for nested TLV will exceed the 
       length value specified */
    if(num_bytes_lvl2 > len_lvl1)
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    value = NULL;
    switch(type)
    {
      case QMI_PRM_TYPE_QOS_FILTER_SPEC:
      {
        while (*sdu_in)
        {
          if(num_bytes_lvl3 == len_lvl2)
          {
            num_bytes_lvl3 = 0;
            break;
          }

          if( !qmi_svc_get_tl(sdu_in, &type, &len_lvl3) )
          {
            continue;
          }
          if(!(len_lvl3 < len_lvl2))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          /* Check for overflow */
          if((MAX_UINT16 - len_lvl3 - 3) < num_bytes_lvl3)
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          num_bytes_lvl3 += len_lvl3 + 3;

          if(num_bytes_lvl3 > len_lvl2)
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          expected_len = 0;
          value = NULL;
          switch(type)
          {
            case QMI_PRM_TYPE_FILTER_INDEX:
              expected_len = sizeof(uint8);
              value = (void*) &fltr_indxs_arr[cnt];
              got_filter_index = TRUE;
            break;

            case QMI_PRM_TYPE_IP_VSN:
              expected_len = sizeof(uint8);
              value = (void *)&ip_fltrs_arr[cnt].ip_vsn;
              got_ip_vsn = TRUE;
              break;

            case QMI_PRM_TYPE_IPV4_SRC_ADDR:
              expected_len = QMI_PRM_TYPE_IPV4_SRC_ADDR_LEN;
              value = (void*)&ip_fltrs_arr[cnt].ip_hdr.v4.src;
              ip_fltrs_arr[cnt].ip_hdr.v4.field_mask |=
                                                IPFLTR_MASK_IP4_SRC_ADDR;
              break;

            case QMI_PRM_TYPE_IPV4_DST_ADDR:
              expected_len = QMI_PRM_TYPE_IPV4_DST_ADDR_LEN;
              value = (void*)&ip_fltrs_arr[cnt].ip_hdr.v4.dst;
              ip_fltrs_arr[cnt].ip_hdr.v4.field_mask |=
                                                IPFLTR_MASK_IP4_DST_ADDR;
              break;

            case QMI_PRM_TYPE_IP_NEXT_HDR_PROT:
              expected_len = sizeof(uint8);
              value = (void*)&tmp_next_hdr_prot;
              got_next_hdr_prot = TRUE;
              break;

            case QMI_PRM_TYPE_IPV4_TOS:
              expected_len = QMI_PRM_TYPE_IPV4_TOS_LEN;
              value = (void*)&ip_fltrs_arr[cnt].ip_hdr.v4.tos;
              ip_fltrs_arr[cnt].ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
              break;

#ifdef FEATURE_DATA_PS_IPV6
            case QMI_PRM_TYPE_IPV6_SRC_ADDR:
              expected_len = QMI_IPV6_FLTR_ADDR_SIZE;
              value = (void*)&ip_fltrs_arr[cnt].ip_hdr.v6.src;
              ip_fltrs_arr[cnt].ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
              break;

            case QMI_PRM_TYPE_IPV6_DST_ADDR:
              expected_len = QMI_IPV6_FLTR_ADDR_SIZE;
              value = (void*)&ip_fltrs_arr[cnt].ip_hdr.v6.dst;
              ip_fltrs_arr[cnt].ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_DST_ADDR;
              break;

            case QMI_PRM_TYPE_IPV6_TRAFFIC_CLASS:
              expected_len = QMI_IPV6_FLTR_TRF_CLS_SIZE;
              value = (void*)&ip_fltrs_arr[cnt].ip_hdr.v6.trf_cls;
              ip_fltrs_arr[cnt].ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
              break;

            case QMI_PRM_TYPE_IPV6_FLOW_LABEL:
              expected_len = sizeof(ip_fltrs_arr[cnt].ip_hdr.v6.flow_label);
              value = (void*)&ip_fltrs_arr[cnt].ip_hdr.v6.flow_label;
              ip_fltrs_arr[cnt].ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_FLOW_LABEL;
              break;
#endif /* FEATURE_DATA_PS_IPV6 */

            case QMI_PRM_TYPE_TCP_SRC_PORT:
              expected_len = QMI_PRM_TYPE_TCP_SRC_PORT_LEN;
              value = (void*)&ip_fltrs_arr[cnt].next_prot_hdr.tcp.src;
              ip_fltrs_arr[cnt].next_prot_hdr.tcp.field_mask |=
                                                    IPFLTR_MASK_TCP_SRC_PORT;
              break;

            case QMI_PRM_TYPE_TCP_DST_PORT:
              expected_len = QMI_PRM_TYPE_TCP_DST_PORT_LEN;
              value = (void*)&ip_fltrs_arr[cnt].next_prot_hdr.tcp.dst;
              ip_fltrs_arr[cnt].next_prot_hdr.tcp.field_mask |=
                                                    IPFLTR_MASK_TCP_DST_PORT;
              break;

            case QMI_PRM_TYPE_UDP_SRC_PORT:
              expected_len = QMI_PRM_TYPE_UDP_SRC_PORT_LEN;
              value = (void*)&ip_fltrs_arr[cnt].next_prot_hdr.udp.src;
              ip_fltrs_arr[cnt].next_prot_hdr.udp.field_mask |=
                                                    IPFLTR_MASK_UDP_SRC_PORT;
              break;

            case QMI_PRM_TYPE_UDP_DST_PORT:
              expected_len = QMI_PRM_TYPE_UDP_DST_PORT_LEN;
              value = (void*)&ip_fltrs_arr[cnt].next_prot_hdr.udp.dst;
              ip_fltrs_arr[cnt].next_prot_hdr.udp.field_mask |=
                                                    IPFLTR_MASK_UDP_DST_PORT;
              break;

	     case QMI_PRM_TYPE_TCP_UDP_SRC_PORT:
              expected_len = QMI_PRM_TYPE_TCP_UDP_SRC_PORT_LEN;
              value = (void*)&ip_fltrs_arr[cnt].next_prot_hdr.tcp_udp_port_range.src;
              ip_fltrs_arr[cnt].next_prot_hdr.tcp_udp_port_range.field_mask |=
                                                    IPFLTR_MASK_TCP_UDP_SRC_PORT;
              /* Whenver we get protocol independent filter request, we make sure to set the next header protocol
                 as PS_IPPROTO_TCP_UDP for the lower layers (CR 531477). */
              got_next_hdr_prot = TRUE;
              tmp_next_hdr_prot = PS_IPPROTO_TCP_UDP;
              break;

            case QMI_PRM_TYPE_TCP_UDP_DST_PORT:
              expected_len = QMI_PRM_TYPE_TCP_UDP_DST_PORT_LEN;
              value = (void*)&ip_fltrs_arr[cnt].next_prot_hdr.tcp_udp_port_range.dst;
              ip_fltrs_arr[cnt].next_prot_hdr.tcp_udp_port_range.field_mask |=
                                                    IPFLTR_MASK_TCP_UDP_DST_PORT;
              /* Whenver we get protocol independent filter request, we make sure to set the next header protocol
                 as PS_IPPROTO_TCP_UDP for the lower layers (CR 531477). */             
              got_next_hdr_prot = TRUE;
              tmp_next_hdr_prot = PS_IPPROTO_TCP_UDP;
              break;

            case QMI_PRM_TYPE_ICMP_MSG_TYPE:
              expected_len = sizeof(ip_fltrs_arr[cnt].next_prot_hdr.icmp.type);
              value = (void*)&ip_fltrs_arr[cnt].next_prot_hdr.icmp.type;
              ip_fltrs_arr[cnt].next_prot_hdr.icmp.field_mask |=
                                                     IPFLTR_MASK_ICMP_MSG_TYPE;
              break;

            case QMI_PRM_TYPE_ICMP_MSG_CODE:
              expected_len = sizeof(ip_fltrs_arr[cnt].next_prot_hdr.icmp.code);
              value = (void*)&ip_fltrs_arr[cnt].next_prot_hdr.icmp.code;
              ip_fltrs_arr[cnt].next_prot_hdr.icmp.field_mask |=
                                                     IPFLTR_MASK_ICMP_MSG_CODE;
              break;

            case QMI_PRM_TYPE_ESP_SPI:
              expected_len = sizeof(ip_fltrs_arr[cnt].next_prot_hdr.esp.spi);
              value = (void*)&ip_fltrs_arr[cnt].next_prot_hdr.esp.spi;
              ip_fltrs_arr[cnt].next_prot_hdr.esp.field_mask |=
                                                         IPFLTR_MASK_ESP_SPI;
              break;

            default:
              LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
              break;
          }

          if(!qmi_svc_validate_type_length( type,
                                            len_lvl3,
                                            expected_len,
                                            FILE_ID_DS_QMI_QOS,__LINE__))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          if(len_lvl3 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl3))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
        }

        if(ip_fltrs_arr[cnt].ip_vsn == IP_V4)
        {
          if (TRUE == got_next_hdr_prot)
          {
            ip_fltrs_arr[cnt].ip_hdr.v4.next_hdr_prot = tmp_next_hdr_prot;
            ip_fltrs_arr[cnt].ip_hdr.v4.field_mask |=
                                               IPFLTR_MASK_IP4_NEXT_HDR_PROT;
          }

          /* convert to network order since PS api needs it */
          if(ip_fltrs_arr[cnt].ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR)
          {
            ip_fltrs_arr[cnt].ip_hdr.v4.src.addr.ps_s_addr =
                ps_htonl(ip_fltrs_arr[cnt].ip_hdr.v4.src.addr.ps_s_addr);
            ip_fltrs_arr[cnt].ip_hdr.v4.src.subnet_mask.ps_s_addr =
                ps_htonl(ip_fltrs_arr[cnt].ip_hdr.v4.src.subnet_mask.ps_s_addr);
          }
          if(ip_fltrs_arr[cnt].ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR)
          {
            ip_fltrs_arr[cnt].ip_hdr.v4.dst.addr.ps_s_addr =
                ps_htonl(ip_fltrs_arr[cnt].ip_hdr.v4.dst.addr.ps_s_addr);
            ip_fltrs_arr[cnt].ip_hdr.v4.dst.subnet_mask.ps_s_addr =
                ps_htonl(ip_fltrs_arr[cnt].ip_hdr.v4.dst.subnet_mask.ps_s_addr);
          }
        }

#ifdef FEATURE_DATA_PS_IPV6
        if(ip_fltrs_arr[cnt].ip_vsn == IP_V6)
        {
          if (TRUE == got_next_hdr_prot)
          {
            ip_fltrs_arr[cnt].ip_hdr.v6.next_hdr_prot = tmp_next_hdr_prot;
            ip_fltrs_arr[cnt].ip_hdr.v6.field_mask |=
                                               IPFLTR_MASK_IP6_NEXT_HDR_PROT;
          }

          if(ip_fltrs_arr[cnt].ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
          {
            ip_fltrs_arr[cnt].ip_hdr.v6.flow_label =
                ps_htonl(ip_fltrs_arr[cnt].ip_hdr.v6.flow_label);
          }
        }
#endif /* FEATURE_DATA_PS_IPV6 */

        switch(tmp_next_hdr_prot)
        {
          case 0x11:
          {
            if(ip_fltrs_arr[cnt].next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT)
            {
              ip_fltrs_arr[cnt].next_prot_hdr.udp.src.port =
                  ps_htons(ip_fltrs_arr[cnt].next_prot_hdr.udp.src.port);
            }
            if(ip_fltrs_arr[cnt].next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT)
            {
              ip_fltrs_arr[cnt].next_prot_hdr.udp.dst.port =
                  ps_htons(ip_fltrs_arr[cnt].next_prot_hdr.udp.dst.port);
            }
            break;
          }

          case 0x06:
          {
            if(ip_fltrs_arr[cnt].next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
            {
              ip_fltrs_arr[cnt].next_prot_hdr.tcp.src.port =
                  ps_htons(ip_fltrs_arr[cnt].next_prot_hdr.tcp.src.port);
            }
            if(ip_fltrs_arr[cnt].next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
            {
              ip_fltrs_arr[cnt].next_prot_hdr.tcp.dst.port =
                  ps_htons(ip_fltrs_arr[cnt].next_prot_hdr.tcp.dst.port);
            }
            break;
          }

	  case PS_IPPROTO_TCP_UDP:
          {
            if(ip_fltrs_arr[cnt].next_prot_hdr.tcp_udp_port_range.field_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT)
            {
              ip_fltrs_arr[cnt].next_prot_hdr.tcp_udp_port_range.src.port =
                  ps_htons(ip_fltrs_arr[cnt].next_prot_hdr.tcp_udp_port_range.src.port);
            }
            if(ip_fltrs_arr[cnt].next_prot_hdr.tcp_udp_port_range.field_mask & IPFLTR_MASK_TCP_UDP_DST_PORT)
            {
              ip_fltrs_arr[cnt].next_prot_hdr.tcp_udp_port_range.dst.port =
                  ps_htons(ip_fltrs_arr[cnt].next_prot_hdr.tcp_udp_port_range.dst.port);
            }
            break;
          }

          default:
             break;
        }

        cnt++; //increment filter count
      } //case QMI_PRM_TYPE_QOS_FILTER_SPEC
      break;

      default:
      {
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);

        /* remove the payload for this unrecognized tlv */
        if(len_lvl2 != PS_UTILS_DSM_PULLUP( sdu_in, value, len_lvl2 ))
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        break;
      } // case default
    }
    if(got_filter_index == FALSE ||
       got_ip_vsn == FALSE)
    {
      errval = QMI_ERR_MISSING_ARG;
      goto send_result;
    }
    got_filter_index = FALSE;
    got_ip_vsn = FALSE;
  }

  /* Something wrong, len_lvl1 passed in does not match up, either because too
     many filters present or we ran out of bytes to process*/
  if(num_bytes_lvl2 != len_lvl1)
  {
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }

  *fltr_count = cnt;
send_result:
  return errval;
} /* qosi_parse_filter_specs() */


/*===========================================================================
FUNCTION   QOSI_PUT_GRANTED_FLOW_SPEC_TLV()

DESCRIPTION
  Generate Flow TLVs for granted flow from ip_flow_type

PARAMETERS
  response      : ptr to response to append the flow TLVs
  granted_flow  : ptr to granted flow
  num_bytes     : number of bytes appended to response

RETURN VALUE
  errval

SIDE EFFECTS
  None
===========================================================================*/
static qmi_error_e_type qosi_put_granted_flow_spec_tlv
(
  dsm_item_type **  response,
  ip_flow_type *    granted_flow,
  uint16       *    num_bytes
)
{
  uint16 cnt;
  qmi_qosi_min_max_type min_max;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cnt = 0;
  min_max.max_rate = 0;
  min_max.guaranteed_rate = 0;

  if(granted_flow->field_mask != 0)
  {
    if(granted_flow->field_mask & IPFLOW_MASK_LTE_QCI)
    {
      if (FALSE == qmi_svc_put_param_tlv(
                     response,
                     QMI_PRM_TYPE_FLOW_LTE_QCI,
                     QMI_PRM_TYPE_FLOW_LTE_QCI_LEN,
                     &granted_flow->lte_params.lte_qci))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->lte_params.lte_qci);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_UMTS_SIG_IND)
    {
      if (FALSE == qmi_svc_put_param_tlv(
                     response,
                     QMI_PRM_TYPE_FLOW_SIG_IND,
                     sizeof(granted_flow->umts_params.sig_ind),
                     &granted_flow->umts_params.sig_ind))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->umts_params.sig_ind);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_UMTS_IM_CN_FLAG)
    {
      if (FALSE == qmi_svc_put_param_tlv(
                     response,
                     QMI_PRM_TYPE_FLOW_IM_CN_FLAG,
                     sizeof(granted_flow->umts_params.im_cn_flag),
                     &granted_flow->umts_params.im_cn_flag))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->umts_params.im_cn_flag);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_CDMA_FLOW_PRIORITY)
    {
      if (FALSE == qmi_svc_put_param_tlv(
                     response,
                     QMI_PRM_TYPE_FLOW_PRIORITY,
                     sizeof(granted_flow->cdma_params.flow_priority),
                     &granted_flow->cdma_params.flow_priority))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->cdma_params.flow_priority);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_CDMA_PROFILE_ID)
    {
      if(FALSE == qmi_svc_put_param_tlv(
                    response,
                    QMI_PRM_TYPE_FLOW_PROFILE_ID,
                    sizeof(granted_flow->cdma_params.profile_id),
                    &granted_flow->cdma_params.profile_id))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->cdma_params.profile_id);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_UMTS_TRF_PRI)
    {
      if(FALSE == qmi_svc_put_param_tlv(
                    response,
                    QMI_PRM_TYPE_FLOW_TRF_PRI,
                    QMI_PRM_TYPE_FLOW_TRF_PRI_LEN,
                    &granted_flow->umts_params.trf_pri))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->umts_params.trf_pri);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_UMTS_RES_BER)
    {
      if(FALSE == qmi_svc_put_param_tlv(
                    response,
                    QMI_PRM_TYPE_FLOW_RES_BER,
                    QMI_PRM_TYPE_FLOW_RES_BER_LEN,
                    &granted_flow->umts_params.res_ber))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->umts_params.res_ber);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE)
    {
      if(FALSE == qmi_svc_put_param_tlv(
                    response,
                    QMI_PRM_TYPE_FLOW_MAX_ALLOWED_PKT_SIZE,
                    sizeof(granted_flow->max_allowed_pkt_size),
                    &granted_flow->max_allowed_pkt_size))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->max_allowed_pkt_size);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_MIN_POLICED_PKT_SIZE)
    {
      if(FALSE == qmi_svc_put_param_tlv(
                    response,
                    QMI_PRM_TYPE_FLOW_MIN_POLICED_PKT_SIZE,
                    sizeof(granted_flow->min_policed_pkt_size),
                    &granted_flow->min_policed_pkt_size))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->min_policed_pkt_size);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_PKT_ERR_RATE)
    {
      if(FALSE == qmi_svc_put_param_tlv(response,
                                        QMI_PRM_TYPE_FLOW_PKT_ERR_RATE,
                                        QMI_PRM_TYPE_FLOW_PKT_ERR_RATE_LEN,
                                        &granted_flow->pkt_err_rate))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->pkt_err_rate);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_LATENCY_VAR)
    {
      if(FALSE == qmi_svc_put_param_tlv(response,
                                        QMI_PRM_TYPE_FLOW_JITTER,
                                        sizeof(granted_flow->latency_var),
                                        &granted_flow->latency_var))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->latency_var);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_LATENCY)
    {
      if(FALSE == qmi_svc_put_param_tlv(response,
                                        QMI_PRM_TYPE_FLOW_LATENCY,
                                        sizeof(granted_flow->latency),
                                        &granted_flow->latency))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + sizeof(granted_flow->latency);
    }
    if(granted_flow->field_mask & IPFLOW_MASK_DATA_RATE)
    {
      if(granted_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
      {
        if(FALSE == qmi_svc_put_param_tlv(
                      response,
                      QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX,
                      QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_LEN,
                      &granted_flow->data_rate.format.min_max))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + sizeof(granted_flow->data_rate.format.min_max);
      }
      else if(granted_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE)
      {
        /*Send the response for both the clients who are Rel11 capable and who are not rel11 capable*/
        if(FALSE == qmi_svc_put_param_tlv(
                      response,
                      QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_EX,
                      QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_EX_LEN,
                      &granted_flow->data_rate.format.min_max_ex))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + sizeof(granted_flow->data_rate.format.min_max_ex);

        /* Send indication to the old clients as well.
           If the data rates are greater than the max supported (256Mpbs),
           restrict the rates to the max possible value for old clients */
        if(granted_flow->data_rate.format.min_max_ex.max_rate > QOSI_MAX_BIT_RATE_PRE_REL_11)
        {
          min_max.max_rate = QOSI_MAX_BIT_RATE_PRE_REL_11;
        }
        else
        {
          min_max.max_rate = (uint32)granted_flow->data_rate.format.min_max_ex.max_rate;
        }
        
        if(granted_flow->data_rate.format.min_max_ex.guaranteed_rate > QOSI_MAX_BIT_RATE_PRE_REL_11)
        {
          min_max.guaranteed_rate = QOSI_MAX_BIT_RATE_PRE_REL_11;
        }
        else
        {
          min_max.guaranteed_rate = (uint32)granted_flow->data_rate.format.min_max_ex.guaranteed_rate;
        }

        if(FALSE == qmi_svc_put_param_tlv(
			        response,
			        QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX,
			        QMI_PRM_TYPE_FLOW_DATA_R_MIN_MAX_LEN,
			        &min_max))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + sizeof(granted_flow->data_rate.format.min_max);
      }
      else if(granted_flow->data_rate.format_type ==
                                        DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE)
      {
        if(FALSE == qmi_svc_put_param_tlv(
                      response,
                      QMI_PRM_TYPE_FLOW_DATA_R_TOKEN_BUK,
                      QMI_PRM_TYPE_FLOW_DATA_R_TOKEN_BUK_LEN,
                      &granted_flow->data_rate.format.token_bucket))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + sizeof(granted_flow->data_rate.format.token_bucket);
      }
    }
    if(granted_flow->field_mask & IPFLOW_MASK_TRF_CLASS)
    {
      if(FALSE == qmi_svc_put_param_tlv(response,
                                        QMI_PRM_TYPE_FLOW_TRF_CLASS,
                                        QMI_PRM_TYPE_FLOW_TRF_CLASS_LEN,
                                        &granted_flow->trf_class))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      cnt += 3 + QMI_PRM_TYPE_FLOW_TRF_CLASS_LEN;
    }
  }
  *num_bytes = cnt;
  return QMI_ERR_NONE;
} /* qosi_put_granted_flow_spec_tlv */

/*===========================================================================
FUNCTION   QOSI_CONVERT_FLOW_SPEC_TO_QMI()

DESCRIPTION
  Generate Flow TLVs for granted flow from ip_flow_type

PARAMETERS
  flow_valid      : TRUE if any flow parameters are being added
  qos_flow_granted  : flow in QMI format
  granted_flow     : ptr to granted flow

RETURN VALUE
  errval

SIDE EFFECTS
  None
===========================================================================*/
static qmi_error_type_v01 qosi_convert_flow_spec_to_qmi
(
  boolean           * flow_valid,
  qos_flow_type_v01 * qos_flow_granted,
  ip_flow_type      * granted_flow
)
{
  memset(qos_flow_granted, 0, sizeof(qos_flow_type_v01));
  if(granted_flow->field_mask != 0)
  {
    *flow_valid = TRUE;
    if(granted_flow->field_mask & IPFLOW_MASK_LTE_QCI)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_LTE_QCI_V01;
      qos_flow_granted->ip_flow_lte_qci = (qos_flow_lte_qci_enum_v01)granted_flow->lte_params.lte_qci;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_UMTS_SIG_IND)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_3GPP_SIG_IND_V01;
      qos_flow_granted->ip_flow_3gpp_sig_ind = granted_flow->umts_params.sig_ind;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_UMTS_IM_CN_FLAG)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_3GPP_IM_CN_FLAG_V01;
      qos_flow_granted->ip_flow_3gpp_im_cn_flag = granted_flow->umts_params.im_cn_flag;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_CDMA_FLOW_PRIORITY)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_3GPP2_FLOW_PRIORITY_V01;
      qos_flow_granted->ip_flow_3gpp2_flow_priority = granted_flow->cdma_params.flow_priority;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_CDMA_PROFILE_ID)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_3GPP2_PROFILE_ID_V01;
      qos_flow_granted->ip_flow_3gpp2_profile_id = granted_flow->cdma_params.profile_id;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_UMTS_TRF_PRI)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_3GPP_TRF_PRI_V01;
      qos_flow_granted->ip_flow_3gpp_traffic_handling_priority = 
        (qos_flow_3gpp_traffic_handling_pri_enum_v01)granted_flow->umts_params.trf_pri;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_UMTS_RES_BER)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_3GPP_RES_BER_V01;
	  qos_flow_granted->ip_flow_3gpp_residual_bit_error_rate = 
        (qos_flow_3gpp_residual_ber_enum_v01)granted_flow->umts_params.res_ber;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_MAX_ALLOWED_PKT_SIZE_V01;
	  qos_flow_granted->ip_flow_max_allowed_packet_size = granted_flow->max_allowed_pkt_size;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_MIN_POLICED_PKT_SIZE)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_MIN_POLICED_PKT_SIZE_V01;
	  qos_flow_granted->ip_flow_min_policed_packet_size = granted_flow->min_policed_pkt_size;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_PKT_ERR_RATE)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_PKT_ERR_RATE_V01;
	  qos_flow_granted->ip_flow_pkt_error_rate.ip_flow_pkt_error_rate_exponent = 
        granted_flow->pkt_err_rate.exponent;
      qos_flow_granted->ip_flow_pkt_error_rate.ip_flow_pkt_error_rate_multiplier = 
        granted_flow->pkt_err_rate.multiplier;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_LATENCY_VAR)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_JITTER_V01;
	  qos_flow_granted->ip_flow_jitter = granted_flow->latency_var;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_LATENCY)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_LATENCY_V01;
	  qos_flow_granted->ip_flow_latency = granted_flow->latency;
    }
    if(granted_flow->field_mask & IPFLOW_MASK_DATA_RATE)
    {
      if(granted_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
      {
        qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_DATA_RATE_MIN_MAX_V01;
        qos_flow_granted->data_rate_min_max.data_rate_max = granted_flow->data_rate.format.min_max.max_rate;
		qos_flow_granted->data_rate_min_max.guaranteed_rate = granted_flow->data_rate.format.min_max.guaranteed_rate;
      }
      else if(granted_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE)
      {
        qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_DATA_RATE_MIN_MAX_V01;
        qos_flow_granted->data_rate_min_max.data_rate_max = granted_flow->data_rate.format.min_max_ex.max_rate;
		qos_flow_granted->data_rate_min_max.guaranteed_rate = granted_flow->data_rate.format.min_max_ex.guaranteed_rate;
      }
      else if(granted_flow->data_rate.format_type ==
                                        DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE)
      {
        qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_DATA_RATE_TOK_BUCKET_V01;
		qos_flow_granted->data_rate_token_bucket.peak_rate = granted_flow->data_rate.format.token_bucket.peak_rate;
		qos_flow_granted->data_rate_token_bucket.token_rate = granted_flow->data_rate.format.token_bucket.token_rate;
		qos_flow_granted->data_rate_token_bucket.bucket_size = granted_flow->data_rate.format.token_bucket.size;
      }
    }
    if(granted_flow->field_mask & IPFLOW_MASK_TRF_CLASS)
    {
      qos_flow_granted->flow_valid_params |= QMI_QOS_IP_FLOW_MASK_TRF_CLASS_V01;
	  qos_flow_granted->ip_flow_trf_cls = 
        (qos_ip_flow_trf_cls_enum_v01)granted_flow->trf_class;
    }
  }
  return QMI_ERR_NONE_V01;
} /* qosi_convert_flow_spec_to_qmi */



/*===========================================================================
FUNCTION   QOSI_PUT_FILTER_SPEC_TLV()

DESCRIPTION
 Generate Filter TLVs for filter spec from ip_filter_type

PARAMETERS
  response   : ptr to response to append the filter TLVs
  filter     : ptr to granted flow
  num_bytes  : number of bytes appended to response

DEPENDENCIES
  None

RETURN VALUE
  errval

SIDE EFFECTS
  None
===========================================================================*/
static qmi_error_e_type qosi_put_filter_spec_tlv
(
  dsm_item_type **  response,
  ip_filter_type *    filter,
  uint16       *    num_bytes
)
{
  uint16 cnt;
  uint8  tmp_next_hdr_prot = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cnt = 0;

  if(FALSE == qmi_svc_put_param_tlv(response,
                                    QMI_PRM_TYPE_IP_VSN,
                                    QMI_PRM_TYPE_IP_VSN_LEN,
                                    &(filter->ip_vsn)))
  {
    dsm_free_packet(response);
    return QMI_ERR_NO_MEMORY;
  }
  cnt += 3 + sizeof(filter->ip_vsn);

  if(filter->ip_vsn == IP_V4)
  {
    tmp_next_hdr_prot = filter->ip_hdr.v4.next_hdr_prot;

    if(filter->ip_hdr.v4.field_mask != 0)
    {
      if(filter->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR)
      {
        if(FALSE == qmi_svc_put_param_tlv(response,
                                          QMI_PRM_TYPE_IPV4_SRC_ADDR,
                                          QMI_PRM_TYPE_IPV4_SRC_ADDR_LEN,
                                          &(filter->ip_hdr.v4.src)))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + sizeof(filter->ip_hdr.v4.src);
      }
      if(filter->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR)
      {
        if(FALSE == qmi_svc_put_param_tlv(response,
                                          QMI_PRM_TYPE_IPV4_DST_ADDR,
                                          QMI_PRM_TYPE_IPV4_DST_ADDR_LEN,
                                          &(filter->ip_hdr.v4.dst)))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + sizeof(filter->ip_hdr.v4.dst);
      }
      if(filter->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
      {
      /* Adding check to, not send out next header protocol TLV if its 
         PS_IPPROTO_TCP_UDP, as the HLOS does not understand this value (CR 531477).*/
        if(filter->ip_hdr.v4.next_hdr_prot != PS_IPPROTO_TCP_UDP)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_IP_NEXT_HDR_PROT,
                                            sizeof(filter->ip_hdr.v4.next_hdr_prot),
                                            &(filter->ip_hdr.v4.next_hdr_prot)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->ip_hdr.v4.next_hdr_prot);
        }
      }

      if(filter->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS)
      {
        if(FALSE == qmi_svc_put_param_tlv(response,
                                          QMI_PRM_TYPE_IPV4_TOS,
                                           QMI_PRM_TYPE_IPV4_TOS_LEN,
                                          &(filter->ip_hdr.v4.tos)))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + sizeof(filter->ip_hdr.v4.tos);
      }
    }
  }

#ifdef FEATURE_DATA_PS_IPV6
  if(filter->ip_vsn == IP_V6)
  {
    tmp_next_hdr_prot = filter->ip_hdr.v6.next_hdr_prot;

    if(filter->ip_hdr.v6.field_mask != 0)
    {
      if(filter->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
      {
        if(FALSE == qmi_svc_put_param_tlv(response,
                                          QMI_PRM_TYPE_IPV6_SRC_ADDR,
                                          QMI_IPV6_FLTR_ADDR_SIZE,
                                          &(filter->ip_hdr.v6.src)))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + QMI_IPV6_FLTR_ADDR_SIZE;
      }

      if(filter->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR)
      {
        if(FALSE == qmi_svc_put_param_tlv(response,
                                          QMI_PRM_TYPE_IPV6_DST_ADDR,
                                          QMI_IPV6_FLTR_ADDR_SIZE,
                                          &(filter->ip_hdr.v6.dst)))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + QMI_IPV6_FLTR_ADDR_SIZE;
      }

      if(filter->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
      {
      /* Adding check to, not send out next header protocol TLV if its 
         PS_IPPROTO_TCP_UDP, as the HLOS does not understand this value (CR 531477).*/
        if(filter->ip_hdr.v6.next_hdr_prot != PS_IPPROTO_TCP_UDP)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_IP_NEXT_HDR_PROT,
                                            sizeof(filter->ip_hdr.v6.next_hdr_prot),
                                            &(filter->ip_hdr.v6.next_hdr_prot)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->ip_hdr.v6.next_hdr_prot);
        }
      }

      if(filter->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
      {
        if(FALSE == qmi_svc_put_param_tlv(response,
                                          QMI_PRM_TYPE_IPV6_TRAFFIC_CLASS,
                                          QMI_IPV6_FLTR_TRF_CLS_SIZE,
                                          &(filter->ip_hdr.v6.trf_cls)))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + QMI_IPV6_FLTR_TRF_CLS_SIZE;
      }

      if(filter->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
      {
        if(FALSE == qmi_svc_put_param_tlv(response,
                                          QMI_PRM_TYPE_IPV6_FLOW_LABEL,
                                          sizeof(filter->ip_hdr.v6.flow_label),
                                          &(filter->ip_hdr.v6.flow_label)))
        {
          dsm_free_packet(response);
          return QMI_ERR_NO_MEMORY;
        }
        cnt += 3 + sizeof(filter->ip_hdr.v6.flow_label);
      }
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  switch(tmp_next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
      if(filter->next_prot_hdr.tcp.field_mask != 0)
      {
        if(filter->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_TCP_SRC_PORT,
                                            QMI_PRM_TYPE_TCP_SRC_PORT_LEN,
                                            &(filter->next_prot_hdr.tcp.src)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->next_prot_hdr.tcp.src);
        }
        if(filter->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_TCP_DST_PORT,
                                            QMI_PRM_TYPE_TCP_DST_PORT_LEN,
                                            &(filter->next_prot_hdr.tcp.dst)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->next_prot_hdr.tcp.dst);
        }
      }
      break;

    case PS_IPPROTO_UDP:
      if(filter->next_prot_hdr.udp.field_mask != 0)
      {
        if(filter->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_UDP_SRC_PORT,
                                            QMI_PRM_TYPE_UDP_SRC_PORT_LEN,
                                            &(filter->next_prot_hdr.udp.src)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->next_prot_hdr.udp.src);
        }
        if(filter->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_UDP_DST_PORT,
                                            QMI_PRM_TYPE_UDP_DST_PORT_LEN,
                                            &(filter->next_prot_hdr.udp.dst)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->next_prot_hdr.udp.dst);
        }
      }
      break;
    case PS_IPPROTO_TCP_UDP:
      if(filter->next_prot_hdr.tcp_udp_port_range.field_mask != 0)
      {
        if(filter->next_prot_hdr.tcp_udp_port_range.field_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_TCP_UDP_SRC_PORT,
                                            QMI_PRM_TYPE_TCP_UDP_SRC_PORT_LEN,
                                            &(filter->next_prot_hdr.tcp_udp_port_range.src)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->next_prot_hdr.tcp_udp_port_range.src);
        }
        if(filter->next_prot_hdr.tcp_udp_port_range.field_mask & IPFLTR_MASK_TCP_UDP_DST_PORT)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_TCP_UDP_DST_PORT,
                                            QMI_PRM_TYPE_TCP_UDP_DST_PORT_LEN,
                                            &(filter->next_prot_hdr.tcp_udp_port_range.dst)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->next_prot_hdr.tcp_udp_port_range.dst);
        }
      }
      break;

    case PS_IPPROTO_ICMP:
      if(filter->next_prot_hdr.icmp.field_mask != 0)
      {
        if(filter->next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_TYPE)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_ICMP_MSG_TYPE,
                                            sizeof(filter->next_prot_hdr.icmp.type),
                                            &(filter->next_prot_hdr.icmp.type)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->next_prot_hdr.icmp.type);
        }
        if(filter->next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_ICMP_MSG_CODE,
                                            sizeof(filter->next_prot_hdr.icmp.code),
                                            &(filter->next_prot_hdr.icmp.code)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->next_prot_hdr.icmp.code);
        }
      }
      break;

    case PS_IPPROTO_ESP:
      if(filter->next_prot_hdr.esp.field_mask != 0)
      {
        if(filter->next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI)
        {
          if(FALSE == qmi_svc_put_param_tlv(response,
                                            QMI_PRM_TYPE_ESP_SPI,
                                            sizeof(filter->next_prot_hdr.esp.spi),
                                            &(filter->next_prot_hdr.esp.spi)))
          {
            dsm_free_packet(response);
            return QMI_ERR_NO_MEMORY;
          }
          cnt += 3 + sizeof(filter->next_prot_hdr.esp.spi);
        }
      }
      break;
  }

  // fill in filter ID and preced info
  if(FALSE == qmi_svc_put_param_tlv(response,
                                    QMI_PRM_TYPE_FILTER_PRECEDENCE,
                                    sizeof(filter->ipfltr_aux_info.fi_precedence),
                                    &(filter->ipfltr_aux_info.fi_precedence)))
  {
    dsm_free_packet(response);
    return QMI_ERR_NO_MEMORY;
  }
  cnt += 3 + sizeof(filter->ipfltr_aux_info.fi_precedence);

  if(FALSE == qmi_svc_put_param_tlv(response,
                                    QMI_PRM_TYPE_FILTER_ID,
                                    sizeof(filter->ipfltr_aux_info.fi_id),
                                    &(filter->ipfltr_aux_info.fi_id)))
  {
    dsm_free_packet(response);
    return QMI_ERR_NO_MEMORY;
  }
  cnt += 3 + sizeof(filter->ipfltr_aux_info.fi_id);

  *num_bytes = cnt;
  return QMI_ERR_NONE;
} /* qosi_put_filter_spec_tlv */

/*===========================================================================
  FUNCTION QOSI_CONVERT_TO_QMI_XPORT_INFO

  DESCRIPTION
   fills qos xport info from ps_ipfltr type.
    
  PARAMETERS
    ps_fltr_rule  - ps filter rule to be converted
    qos_fltr_fule - qos filter rule to be filled out
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qosi_convert_to_qmi_xport_info
(
  ip_filter_type           * ps_fltr_rule,
  qos_filter_rule_type_v01 * qos_fltr_rule
)
{
  switch(qos_fltr_rule->xport_info.xport_protocol)
  {
    case QOS_PROTO_ICMP_V01:
    case QOS_PROTO_ICMPV6_V01:
    {
      if(ps_fltr_rule->next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_TYPE)
      {
        qos_fltr_rule->xport_info.icmp_info.valid_params |= 
          QMI_QOS_ICMP_FILTER_MASK_MSG_TYPE_V01;
        qos_fltr_rule->xport_info.icmp_info.type = 
          ps_fltr_rule->next_prot_hdr.icmp.type;
      }
      if(ps_fltr_rule->next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE)
      {
        qos_fltr_rule->xport_info.icmp_info.valid_params |= 
          QMI_QOS_ICMP_FILTER_MASK_MSG_CODE_V01;
        qos_fltr_rule->xport_info.icmp_info.code = 
          ps_fltr_rule->next_prot_hdr.icmp.code;
      }
      break;
    }
    case QOS_PROTO_TCP_V01:
    {
      if(ps_fltr_rule->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
      {
        qos_fltr_rule->xport_info.tcp_info.valid_params |= 
          QMI_QOS_PORT_INFO_FILTER_MASK_SRC_PORT_V01;
        qos_fltr_rule->xport_info.tcp_info.src_port_info.port = 
          ps_ntohs(ps_fltr_rule->next_prot_hdr.tcp.src.port);
        qos_fltr_rule->xport_info.tcp_info.src_port_info.range = 
          ps_fltr_rule->next_prot_hdr.tcp.src.range;
      }
      if(ps_fltr_rule->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
      {
        qos_fltr_rule->xport_info.tcp_info.valid_params |= 
          QMI_QOS_PORT_INFO_FILTER_MASK_DEST_PORT_V01;
        qos_fltr_rule->xport_info.tcp_info.dest_port_info.port = 
          ps_ntohs(ps_fltr_rule->next_prot_hdr.tcp.dst.port);
        qos_fltr_rule->xport_info.tcp_info.dest_port_info.range = 
          ps_fltr_rule->next_prot_hdr.tcp.dst.range;
      }
      break;
    }
    case QOS_PROTO_UDP_V01:
    {
      if(ps_fltr_rule->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT)
      {
        qos_fltr_rule->xport_info.udp_info.valid_params |= 
          QMI_QOS_PORT_INFO_FILTER_MASK_SRC_PORT_V01;
        qos_fltr_rule->xport_info.udp_info.src_port_info.port = 
          ps_ntohs(ps_fltr_rule->next_prot_hdr.udp.src.port);
        qos_fltr_rule->xport_info.udp_info.src_port_info.range = 
          ps_fltr_rule->next_prot_hdr.udp.src.range;
      }
      if(ps_fltr_rule->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT)
      {
        qos_fltr_rule->xport_info.udp_info.valid_params |= 
          QMI_QOS_PORT_INFO_FILTER_MASK_DEST_PORT_V01;
        qos_fltr_rule->xport_info.udp_info.dest_port_info.port = 
          ps_ntohs(ps_fltr_rule->next_prot_hdr.udp.dst.port);
        qos_fltr_rule->xport_info.udp_info.dest_port_info.range = 
          ps_fltr_rule->next_prot_hdr.udp.dst.range;
      }
      break;
    }
    case QOS_PROTO_ESP_V01:
    {
      if(ps_fltr_rule->next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI)
      {
        qos_fltr_rule->xport_info.esp_info.valid_params |= 
          QMI_QOS_IPSEC_FILTER_MASK_SPI_V01;
        qos_fltr_rule->xport_info.esp_info.spi = 
          ps_fltr_rule->next_prot_hdr.esp.spi;
      }
      break;
    }
    case QOS_PROTO_AH_V01:
    {
      if(ps_fltr_rule->next_prot_hdr.ah.field_mask & IPFLTR_MASK_AH_SPI)
      {
        qos_fltr_rule->xport_info.ah_info.valid_params |= 
          QMI_QOS_IPSEC_FILTER_MASK_SPI_V01;
        qos_fltr_rule->xport_info.ah_info.spi = 
          ps_fltr_rule->next_prot_hdr.ah.spi;
      }
      break;
    }
    default:
      break;
  }
} /* qosi_convert_to_qmi_xport_info */

/*===========================================================================
  FUNCTION QOSI_CONVERT_TO_QMI_V4_INFO

  DESCRIPTION
   fills qos ipv4 info from ps_ipfltr type.
    
  PARAMETERS
    ps_fltr_rule  - ps filter rule to be converted
    qos_fltr_fule - qos filter rule to be filled out
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qosi_convert_to_qmi_v4_info
(
  ip_filter_type           * ps_fltr_rule,
  qos_filter_rule_type_v01 * qos_fltr_rule
)
{
  if (ps_fltr_rule->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR)
  {
    qos_fltr_rule->ip_info.v4_info.valid_params |= QMI_QOS_IPV4_FILTER_MASK_SRC_ADDR_V01;
    qos_fltr_rule->ip_info.v4_info.src_addr.ipv4_addr = 
      ps_ntohl(ps_fltr_rule->ip_hdr.v4.src.addr.ps_s_addr);
    qos_fltr_rule->ip_info.v4_info.src_addr.subnet_mask = 
      ps_ntohl(ps_fltr_rule->ip_hdr.v4.src.subnet_mask.ps_s_addr);
  }
  if (ps_fltr_rule->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR)
  {
    qos_fltr_rule->ip_info.v4_info.valid_params |= QMI_QOS_IPV4_FILTER_MASK_DEST_ADDR_V01;
    qos_fltr_rule->ip_info.v4_info.dest_addr.ipv4_addr = 
      ps_ntohl(ps_fltr_rule->ip_hdr.v4.dst.addr.ps_s_addr);
    qos_fltr_rule->ip_info.v4_info.dest_addr.subnet_mask = 
      ps_ntohl(ps_fltr_rule->ip_hdr.v4.dst.subnet_mask.ps_s_addr);
  }
  if (ps_fltr_rule->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS)
  {
    qos_fltr_rule->ip_info.v4_info.valid_params |= QMI_QOS_IPV4_FILTER_MASK_TOS_V01;
    qos_fltr_rule->ip_info.v4_info.tos.mask = ps_fltr_rule->ip_hdr.v4.tos.mask;
    qos_fltr_rule->ip_info.v4_info.tos.val = ps_fltr_rule->ip_hdr.v4.tos.val;
  }
  if (ps_fltr_rule->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
  {
    qos_fltr_rule->xport_info.xport_protocol = 
      (qos_xport_protocol_enum_v01)ps_fltr_rule->ip_hdr.v4.next_hdr_prot;
    qosi_convert_to_qmi_xport_info(ps_fltr_rule,qos_fltr_rule);
  }
} /* qosi_convert_to_qmi_v4_info */

/*===========================================================================
  FUNCTION QOSI_CONVERT_TO_QMI_V6_INFO

  DESCRIPTION
   fills qos ipv6 info from ps_ipfltr type.
    
  PARAMETERS
    ps_fltr_rule  - ps filter rule to be converted
    qos_fltr_fule - qos filter rule to be filled out
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_convert_to_qmi_v6_info
(
  ip_filter_type           * ps_fltr_rule,
  qos_filter_rule_type_v01 * qos_fltr_rule
)
{
  if (ps_fltr_rule->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
  {
    qos_fltr_rule->ip_info.v6_info.valid_params |= QMI_QOS_IPV6_FILTER_MASK_SRC_ADDR_V01;
    memscpy(qos_fltr_rule->ip_info.v6_info.src_addr.ipv6_address, 
            sizeof(qos_fltr_rule->ip_info.v6_info.src_addr.ipv6_address),
            ps_fltr_rule->ip_hdr.v6.src.addr.in6_u.u6_addr8,
            sizeof(ps_fltr_rule->ip_hdr.v6.src.addr.in6_u.u6_addr8));
    qos_fltr_rule->ip_info.v6_info.src_addr.prefix_len = 
      ps_fltr_rule->ip_hdr.v6.src.prefix_len;
  }
  if (ps_fltr_rule->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR)
  {
    qos_fltr_rule->ip_info.v6_info.valid_params |= QMI_QOS_IPV6_FILTER_MASK_DEST_ADDR_V01;
    memscpy(qos_fltr_rule->ip_info.v6_info.dest_addr.ipv6_address, 
            sizeof(qos_fltr_rule->ip_info.v6_info.dest_addr.ipv6_address),
            ps_fltr_rule->ip_hdr.v6.dst.addr.in6_u.u6_addr8,
            sizeof(ps_fltr_rule->ip_hdr.v6.dst.addr.in6_u.u6_addr8));
    qos_fltr_rule->ip_info.v6_info.dest_addr.prefix_len = 
      ps_fltr_rule->ip_hdr.v6.dst.prefix_len;
  }
  if (ps_fltr_rule->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
  {
    qos_fltr_rule->ip_info.v6_info.valid_params |= QMI_QOS_IPV6_FILTER_MASK_TRAFFIC_CLASS_V01;
    qos_fltr_rule->ip_info.v6_info.trf_cls.mask = ps_fltr_rule->ip_hdr.v6.trf_cls.mask;
    qos_fltr_rule->ip_info.v6_info.trf_cls.val = ps_fltr_rule->ip_hdr.v6.trf_cls.val;
  }
  if (ps_fltr_rule->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
  {
    qos_fltr_rule->ip_info.v6_info.valid_params |= QMI_QOS_IPV6_FILTER_MASK_FLOW_LABEL_V01;
    qos_fltr_rule->ip_info.v6_info.flow_label = ps_ntohl(ps_fltr_rule->ip_hdr.v6.flow_label);
  }
  if (ps_fltr_rule->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
  {
    qos_fltr_rule->xport_info.xport_protocol = 
      (qos_xport_protocol_enum_v01)ps_fltr_rule->ip_hdr.v6.next_hdr_prot;
    qosi_convert_to_qmi_xport_info(ps_fltr_rule,qos_fltr_rule);
  }
} /* qosi_convert_to_qmi_v6_info */

/*===========================================================================
  FUNCTION QOSI_CONVERT_FILTER_SPEC_TO_QMI

  DESCRIPTION
   fills qmi_qos filter_rule from ps_ipfltr type.
    
  PARAMETERS
    ps_fltr_rule  - ps filter rule to be converted
    qos_fltr_fule - qos filter rule to be filled out

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_convert_filter_spec_to_qmi
(
  ip_filter_type                   * ps_fltr_rule,
  qos_filter_rule_resp_ind_type_v01 * qos_fltr_rule
)
{
  qos_fltr_rule->fltr_info.ip_info.ip_version = (qos_ip_family_enum_v01)ps_fltr_rule->ip_vsn;
  qos_fltr_rule->filter_id = ps_fltr_rule->ipfltr_aux_info.fi_id;
  qos_fltr_rule->filter_precedence = ps_fltr_rule->ipfltr_aux_info.fi_precedence;
  if(qos_fltr_rule->fltr_info.ip_info.ip_version == QOS_IP_FAMILY_IPV4_V01)
  {
    qosi_convert_to_qmi_v4_info(ps_fltr_rule, &qos_fltr_rule->fltr_info);
  }
  else
  {
    qosi_convert_to_qmi_v6_info(ps_fltr_rule, &qos_fltr_rule->fltr_info);
  }
} /* qosi_convert_filter_spec_to_qmi */

/*===========================================================================
  FUNCTION QOSI_APPEND_GRANTED_FLOW_SPEC()

  DESCRIPTION
    Get the Granted flow spec and append the flow tlvs to the response

  PARAMETERS
    response  : ptr to response
    flow_ptr  : ptr to the QoS flow

  RETURN VALUE
    errval

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qosi_append_granted_flow_spec
(
  qmi_qosi_state_type * qos_sp,
  dsm_item_type **   response,
  ps_flow_type *     flow_ptr,
  uint16       *     total_bytes_pushed
)
{
  ip_flow_type *                granted_flow_ptr;
  qmi_error_e_type  errval;
  uint16 num_bytes;
  uint8  type;
  ps_flow_ioctl_primary_qos_get_granted_flow_spec_type prim_granted_flow_spec;
  int                return_val;
  int16             ps_errno;
  boolean            primary = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (qos_sp->ps_iface_ptr);
  *total_bytes_pushed = 0;
  num_bytes = 0;
  memset(&prim_granted_flow_spec, 0, sizeof(prim_granted_flow_spec));

  if (TRUE == PS_IFACE_IS_FLOW_DEFAULT(qos_sp->ps_iface_ptr, flow_ptr))
  {
    LOG_MSG_INFO2_0("Primary QoS get granted flow IOCTL");
    return_val = ps_flow_ioctl(flow_ptr,
                               PS_FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC,
                               &prim_granted_flow_spec,
                               &ps_errno);
    if (return_val == -1)
    {
      qosi_populate_ext_error_tlv(&errval, response, ps_errno);
      return errval;
    }
    primary = TRUE;
  }

  if (primary == TRUE)
  {
    granted_flow_ptr = &(prim_granted_flow_spec.tx_ip_flow);
  }
  else
  {
    granted_flow_ptr = PS_FLOW_GET_TX_GRANTED_FLOW(flow_ptr);
  }
  if(granted_flow_ptr != NULL)
  {
    errval = QMI_ERR_NONE;
    errval = qosi_put_granted_flow_spec_tlv(response,
                                            granted_flow_ptr,
                                            &num_bytes);

    if(errval != QMI_ERR_NONE)
    {
      dsm_free_packet(response);
      return errval;
    }

    if(num_bytes > 0)
    {
      type = QMI_PRM_TYPE_QOS_FLOW_SPEC;
      if(!((QMI_SVC_PKT_PUSH(response, &num_bytes, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      type = QMI_PRM_TYPE_TX_QOS_GRANTED_FLOW;
      num_bytes += 3;
      if(!((QMI_SVC_PKT_PUSH(response, &num_bytes, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      num_bytes += 3;
    }
    (*total_bytes_pushed) += num_bytes;
  }
  else
  {
    errval = QMI_ERR_FLOW_SUSPENDED;
  }

  if (primary == TRUE)
  {
    granted_flow_ptr = &(prim_granted_flow_spec.rx_ip_flow);
  }
  else
  {
    granted_flow_ptr = PS_FLOW_GET_RX_GRANTED_FLOW(flow_ptr);
  }
  if(granted_flow_ptr != NULL)
  {
    errval = QMI_ERR_NONE;
    errval = qosi_put_granted_flow_spec_tlv(response,
                                            granted_flow_ptr,
                                            &num_bytes);
    if(errval != QMI_ERR_NONE)
    {
      dsm_free_packet(response);
      return errval;
    }

    if(num_bytes > 0)
    {
      type = QMI_PRM_TYPE_QOS_FLOW_SPEC;
      if(!((QMI_SVC_PKT_PUSH(response, &num_bytes, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      type = QMI_PRM_TYPE_RX_QOS_GRANTED_FLOW;
      num_bytes += 3;
      if(!((QMI_SVC_PKT_PUSH(response, &num_bytes, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
      {
        dsm_free_packet(response);
        return QMI_ERR_NO_MEMORY;
      }
      num_bytes += 3;
    }
    (*total_bytes_pushed) += num_bytes;
  }
  else
  {
    errval = QMI_ERR_FLOW_SUSPENDED;
  }
  return errval;
} /* qosi_append_granted_flow_spec */

/*===========================================================================
  FUNCTION QOSI_APPEND_GRANTED_FLOW_SPEC_EX()

  DESCRIPTION
    Get the Granted flow spec and append the flow tlvs to the response

  PARAMETERS
    qos_sp  : ptr to qos state
    tx_qos_flow_granted_valid  : output value which is TRUE if any tx flows are being added
    tx_qos_flow_granted : array of tx flows
    rx_qos_flow_granted_valid :output value which is TRUE if any rx flows are being added
    rx_qos_flow_granted : array of rx flows
    ext_err : error code returned by ps
    flow_ptr : ptr to the QoS flow

  RETURN VALUE
    qmi error code

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qosi_append_granted_flow_spec_ex
(
  qmi_qosi_state_type             * qos_sp,
  boolean                         * tx_qos_flow_granted_valid,
  qos_flow_type_v01               * tx_qos_flow_granted,
  boolean                         * rx_qos_flow_granted_valid,
  qos_flow_type_v01               * rx_qos_flow_granted,
  uint16                          * ext_err,
  ps_flow_type                    * flow_ptr
)
{
  ip_flow_type *                granted_flow_ptr;
  qmi_error_type_v01  errval;
  ps_flow_ioctl_primary_qos_get_granted_flow_spec_type prim_granted_flow_spec;
  int               return_val;
  int16             ps_errno;
  boolean           primary = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (qos_sp->ps_iface_ptr);
  memset(&prim_granted_flow_spec, 0, sizeof(prim_granted_flow_spec));

  if (TRUE == PS_IFACE_IS_FLOW_DEFAULT(qos_sp->ps_iface_ptr, flow_ptr))
  {
    LOG_MSG_INFO2_0("Primary QoS get granted flow IOCTL");
    return_val = ps_flow_ioctl(flow_ptr,
                               PS_FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC,
                               &prim_granted_flow_spec,
                               &ps_errno);
    if (return_val == -1)
    {
      if (NULL != ext_err)
      {
        *ext_err = ps_errno;
      }
      return QMI_ERR_EXTENDED_INTERNAL_V01;
    }
    primary = TRUE;
  }

  if (primary == TRUE)
  {
    granted_flow_ptr = &(prim_granted_flow_spec.tx_ip_flow);
  }
  else
  {
    granted_flow_ptr = PS_FLOW_GET_TX_GRANTED_FLOW(flow_ptr);
  }
  if(granted_flow_ptr != NULL)
  {
    errval = QMI_ERR_NONE_V01;
    errval = qosi_convert_flow_spec_to_qmi(tx_qos_flow_granted_valid,
                                           tx_qos_flow_granted,
                                           granted_flow_ptr);

    if(errval != QMI_ERR_NONE_V01)
    {
      return errval;
    }

  }
  else
  {
    errval = QMI_ERR_FLOW_SUSPENDED_V01;
  }

  if (primary == TRUE)
  {
    granted_flow_ptr = &(prim_granted_flow_spec.rx_ip_flow);
  }
  else
  {
    granted_flow_ptr = PS_FLOW_GET_RX_GRANTED_FLOW(flow_ptr);
  }
  if(granted_flow_ptr != NULL)
  {
    errval = QMI_ERR_NONE_V01;
    errval = qosi_convert_flow_spec_to_qmi(rx_qos_flow_granted_valid,
                                           rx_qos_flow_granted,
                                           granted_flow_ptr);
    if(errval != QMI_ERR_NONE_V01)
    {
      return errval;
    }
  }
  else
  {
    errval = QMI_ERR_FLOW_SUSPENDED_V01;
  }
  return errval;
} /* qosi_append_granted_flow_spec_ex */



/*===========================================================================
  FUNCTION QOSI_APPEND_FILTER_SPEC()

  DESCRIPTION
    Append the filter tlvs registered by the client to the response

  PARAMETERS
    response       : ptr to response
    cls_sp         : client state ptr
    qmi_flow_indx  : indx of the qos flow for which filters are to be sent

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qosi_append_filter_spec
(
  dsm_item_type **      response,
  ps_iface_type *       iface_ptr,
  ps_flow_type *        flow_ptr,
  uint16       *     total_bytes_pushed,
  uint8                 tx_filter_type,
  uint8                 rx_filter_type
)
{
  qmi_error_e_type  errval;
  uint16 cnt;
  uint16 num_bytes;
  uint8  type;
  uint8  fltr_cnt;
  ip_filter_spec_type ip_fltr_spec;
  int16  ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (iface_ptr && flow_ptr);
  memset(&ip_fltr_spec, 0, sizeof(ip_filter_spec_type));

  PS_SYSTEM_HEAP_MEM_ALLOC( ip_fltr_spec.list_ptr,
                          sizeof(ip_filter_type)*QOSI_MAX_QOS_FLTRS_PER_REQUEST,
                          ip_filter_type*);

  if (NULL == ip_fltr_spec.list_ptr)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  memset(ip_fltr_spec.list_ptr, 0, (sizeof(ip_filter_type) * QOSI_MAX_QOS_FLTRS_PER_REQUEST));

  *total_bytes_pushed = 0;
  num_bytes = 0;
  errval = QMI_ERR_NONE;

  /*-------------------------------------------------------------------------
    Retrieve tx filter list for this flow ptr
  -------------------------------------------------------------------------*/
  if (FALSE == ps_iface_flow_get_tx_fltr_spec(iface_ptr,
                                              flow_ptr,
                                              &ip_fltr_spec,
                                              FALSE,
                                              QOSI_MAX_QOS_FLTRS_PER_REQUEST,
                                              &ps_errno))
  {
    LOG_MSG_ERROR_1("Failed to retrieve tx filters for flow ptr %x", flow_ptr);
    errval =  QMI_ERR_INTERNAL;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Do ntoh on appropriate fields in the filter spec
  -------------------------------------------------------------------------*/
  qosi_convert_filter_spec_ntoh(&ip_fltr_spec);
  /*-------------------------------------------------------------------------
    Go through all the filters for the client and append them to the
    response
  -------------------------------------------------------------------------*/
  for(fltr_cnt = 0; fltr_cnt < ip_fltr_spec.num_filters; fltr_cnt++)
  {
    errval = qosi_put_filter_spec_tlv(response,
                                      &(ip_fltr_spec.list_ptr[fltr_cnt]),
                                      &cnt);
    if(errval != QMI_ERR_NONE)
    {
      goto send_result;
    }

    if(cnt > 0)
    {
      type = QMI_PRM_TYPE_QOS_FILTER_SPEC;
      if(!((QMI_SVC_PKT_PUSH(response, &cnt, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
      {
        errval = QMI_ERR_NO_MEMORY;
        goto send_result;
      }
      num_bytes += (cnt + 3);
    }
  }

  if(num_bytes > 0) //if any filter spec tlvs were added
  {
    type = tx_filter_type;
    if(!((QMI_SVC_PKT_PUSH(response, &num_bytes, sizeof(uint16))) &&
         (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
    {
      errval =  QMI_ERR_NO_MEMORY;
      goto send_result;
    }
    num_bytes += 3;
    (*total_bytes_pushed) += num_bytes;
  }

  num_bytes = 0;

  /*-------------------------------------------------------------------------
    Retrieve rx filter list for this flow ptr
  -------------------------------------------------------------------------*/
  ip_fltr_spec.num_filters = 0;
  memset(ip_fltr_spec.list_ptr, 0, (sizeof(ip_filter_type) * QOSI_MAX_QOS_FLTRS_PER_REQUEST));
  if (FALSE == ps_flow_get_rx_fltr_spec(iface_ptr,
                                        flow_ptr,
                                        &ip_fltr_spec,
                                        FALSE,
                                        QOSI_MAX_QOS_FLTRS_PER_REQUEST,
                                        &ps_errno))
  {
    LOG_MSG_ERROR_1("Failed to retrieve rx filters for flow ptr %x", flow_ptr);
    errval =  QMI_ERR_INTERNAL;
    goto send_result;
  }

  qosi_convert_filter_spec_ntoh(&ip_fltr_spec);
  /*-------------------------------------------------------------------------
    Go through all the rx filters for the client and append them to the
    response
  -------------------------------------------------------------------------*/
  for(fltr_cnt = 0; fltr_cnt < ip_fltr_spec.num_filters; fltr_cnt++)
  {
    errval = qosi_put_filter_spec_tlv(response,
                                      &(ip_fltr_spec.list_ptr[fltr_cnt]),
                                      &cnt);
    if(errval != QMI_ERR_NONE)
    {
      goto send_result;
    }

    if(cnt > 0)
    {
      type = QMI_PRM_TYPE_QOS_FILTER_SPEC;
      if(!((QMI_SVC_PKT_PUSH(response, &cnt, sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
      {
        errval =  QMI_ERR_NO_MEMORY;
        goto send_result;
      }
      num_bytes += (cnt + 3);
    }
  }
  if(num_bytes > 0)
  {
    type = rx_filter_type;
    if(!((QMI_SVC_PKT_PUSH(response, &num_bytes, sizeof(uint16))) &&
         (QMI_SVC_PKT_PUSH(response, &type, sizeof(uint8)))))
    {
      errval =  QMI_ERR_NO_MEMORY;
      goto send_result;
    }
    num_bytes += 3;
    (*total_bytes_pushed) += num_bytes;
  }

send_result:
  if(errval != QMI_ERR_NONE)
  {
    dsm_free_packet(response);
  }
  if (ip_fltr_spec.list_ptr != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(ip_fltr_spec.list_ptr);
  }
  return errval;
} /* qosi_append_filter_spec */

/*===========================================================================
  FUNCTION QOSI_APPEND_FILTER_SPEC_EX()

  DESCRIPTION
    Append the filter tlvs registered by the client to the response

  PARAMETERS
    iface_ptr       : ptr to iface
    flow_ptr         : ptr to QOS flow
    tx_qos_filters_valid  : set to TRUE if tx filters are added
    tx_qos_filters_len : no of tx filters
    tx_qos_filters : array of tx filters
    rx_qos_filters_valid : set to TRUE if rx filters are added
    rx_qos_filters_len : no of rx filters
    rx_qos_filters : array of rx filters

  RETURN VALUE
    qmi error code

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qosi_append_filter_spec_ex
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * flow_ptr,
  boolean                          * tx_qos_filters_valid,
  uint32                           * tx_qos_filters_len,
  qos_filter_rule_resp_ind_type_v01 * tx_qos_filters,
  boolean                          * rx_qos_filters_valid,
  uint32                           * rx_qos_filters_len,
  qos_filter_rule_resp_ind_type_v01 * rx_qos_filters
)
{
  qmi_error_type_v01  errval;
  uint8  fltr_cnt;
  ip_filter_spec_type ip_fltr_spec;
  int16  ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (iface_ptr && flow_ptr);
  memset(&ip_fltr_spec, 0, sizeof(ip_filter_spec_type));

  PS_SYSTEM_HEAP_MEM_ALLOC( ip_fltr_spec.list_ptr,
                          sizeof(ip_filter_type)*QMI_QOS_MAX_QOS_FILTERS_V01,
                          ip_filter_type*);

  if (NULL == ip_fltr_spec.list_ptr)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  memset(ip_fltr_spec.list_ptr, 0, (sizeof(ip_filter_type) * QMI_QOS_MAX_QOS_FILTERS_V01));

  errval = QMI_ERR_NONE_V01;

  /*-------------------------------------------------------------------------
    Retrieve tx filter list for this flow ptr
  -------------------------------------------------------------------------*/
  if (FALSE == ps_iface_flow_get_tx_fltr_spec(iface_ptr,
                                              flow_ptr,
                                              &ip_fltr_spec,
                                              FALSE,
                                              QMI_QOS_MAX_QOS_FILTERS_V01,
                                              &ps_errno))
  {
    LOG_MSG_ERROR_1("Failed to retrieve tx filters for flow ptr %x", flow_ptr);
    errval =  QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Go through all the filters for the client and append them to the
    response
  -------------------------------------------------------------------------*/
  for(fltr_cnt = 0; fltr_cnt < ip_fltr_spec.num_filters; fltr_cnt++)
  {
    qosi_convert_filter_spec_to_qmi(&(ip_fltr_spec.list_ptr[fltr_cnt]),                                      
                                    &tx_qos_filters[fltr_cnt]);
    
  }

  if(ip_fltr_spec.num_filters > 0) //if any filter spec tlvs were added
  {
    *tx_qos_filters_valid = TRUE;
    *tx_qos_filters_len = ip_fltr_spec.num_filters;
  }

  /*-------------------------------------------------------------------------
    Retrieve rx filter list for this flow ptr
  -------------------------------------------------------------------------*/
  ip_fltr_spec.num_filters = 0;
  memset(ip_fltr_spec.list_ptr, 0, (sizeof(ip_filter_type) * QMI_QOS_MAX_QOS_FILTERS_V01));
  if (FALSE == ps_flow_get_rx_fltr_spec(iface_ptr,
                                        flow_ptr,
                                        &ip_fltr_spec,
                                        FALSE,
                                        QMI_QOS_MAX_QOS_FILTERS_V01,
                                        &ps_errno))
  {
    LOG_MSG_ERROR_1("Failed to retrieve rx filters for flow ptr %x", flow_ptr);
    errval =  QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Go through all the rx filters for the client and append them to the
    response
  -------------------------------------------------------------------------*/
  for(fltr_cnt = 0; fltr_cnt < ip_fltr_spec.num_filters; fltr_cnt++)
  {
    qosi_convert_filter_spec_to_qmi(&(ip_fltr_spec.list_ptr[fltr_cnt]),                                      
                                    &rx_qos_filters[fltr_cnt]);
  }
  if(ip_fltr_spec.num_filters > 0) //if any filter spec tlvs were added
  {
    *rx_qos_filters_valid = TRUE;
    *rx_qos_filters_len = ip_fltr_spec.num_filters;
  }

send_result:
  if(errval != QMI_ERR_NONE_V01)
  {
    *tx_qos_filters_valid = FALSE;
    *rx_qos_filters_valid = FALSE;
  }
  if (ip_fltr_spec.list_ptr != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(ip_fltr_spec.list_ptr);
  }
  return errval;
} /* qosi_append_filter_spec_ex */

/*===========================================================================
  FUNCTION QOSI_REG_PS_FLOW_EVENT

  DESCRIPTION
     Register for PS Flow Events

  PARAMETERS
    qos_inst  : QMI QoS instance for which to register for
    is_v6     : TRUE for v6 instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_reg_ps_flow_event
(
  uint8                qos_inst,
  boolean              is_v6
)
{
  qmi_qosi_state_type *  qos_sp;
  ps_iface_type       *  ps_iface_ptr;
  boolean                is_err = FALSE;
  ps_iface_event_enum_type  event = IFACE_MIN_EV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check that qos_inst is within range */
  if (qos_inst >= QOS_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!", qos_inst);
    return;
  }

  if(is_v6)
    qos_sp = qmi_qos_state[QOSI_IPV6_STATE][qos_inst];
  else
    qos_sp = qmi_qos_state[QOSI_IPV4_STATE][qos_inst];

  ps_iface_ptr = rmnet_meta_sm_get_um_iface(qos_sp->binding.rmnet_inst);
  if(ps_iface_ptr == NULL)
  {
    DATA_ERR_FATAL("Rmnet call not up, while trying to reg for def flow ev");
    return;
  }

  if(qos_sp->flow_ev_reg == TRUE)
  {
    return;
  }
  qos_sp->ps_iface_ptr = ps_iface_ptr;

  do
  {
    if(0 != ps_flow_event_cback_reg(NULL,
                                    FLOW_ACTIVATED_EV,
                                    qos_sp->flow_activated_buf_ptr))
    {
        is_err = TRUE;
        event = FLOW_ACTIVATED_EV;
        break;
    }
  
    if(0 != ps_flow_event_cback_reg(NULL,
                                    FLOW_SUSPENDED_EV,
                                    qos_sp->flow_suspended_buf_ptr))
    {
        is_err = TRUE;
        event = FLOW_SUSPENDED_EV;
        break;
    }
  
    if(0 != ps_flow_event_cback_reg(NULL,
                                    FLOW_NULL_EV,
                                    qos_sp->flow_null_buf_ptr))
    {
        is_err = TRUE;
        event = FLOW_NULL_EV;
        break;
    }
  
    if(0 != ps_flow_event_cback_reg(NULL,
                                    FLOW_MODIFY_ACCEPTED_EV,
                                    qos_sp->flow_modify_accepted_buf_ptr))
    {
        is_err = TRUE;
        event = FLOW_MODIFY_ACCEPTED_EV;
        break;
    }
  
    if(0 != ps_flow_event_cback_reg(NULL,
                                   FLOW_MODIFY_REJECTED_EV,
                                   qos_sp->flow_modify_rejected_buf_ptr))
    {
        is_err = TRUE;
        event = FLOW_MODIFY_REJECTED_EV;
        break;
    }
  
    if(0 != ps_flow_event_cback_reg(NULL,
                                    FLOW_PRIMARY_MODIFY_RESULT_EV,
                                    qos_sp->flow_prim_modify_result_buf_ptr))
    {
        is_err = TRUE;
        event = FLOW_PRIMARY_MODIFY_RESULT_EV;
        break;
    }
  
    if(0 != ps_flow_event_cback_reg(NULL,
                                    FLOW_TX_ENABLED_EV,
                                    qos_sp->flow_enabled_buf_ptr))
    {
        is_err = TRUE;
        event = FLOW_TX_ENABLED_EV;
        break;
    }
  
    if(0 != ps_flow_event_cback_reg(NULL,
                                    FLOW_TX_DISABLED_EV,
                                    qos_sp->flow_disabled_buf_ptr))
    {
        is_err = TRUE;
        event = FLOW_TX_DISABLED_EV;
        break;
    }
  
    if(0 != ps_flow_event_cback_reg(NULL,
                                    FLOW_INFO_CODE_UPDATED_EV,
                                    qos_sp->flow_info_code_updated_buf_ptr))
    {
        is_err = TRUE;
        event = FLOW_INFO_CODE_UPDATED_EV;
        break;
    }
  }while(0);
  if(is_err == TRUE)
  {
    LOG_MSG_ERROR_1("Couldn't register for event (%d)!",event);
    ASSERT( 0 );
    return;
  }

  /* reg phys link events */
  qosi_reg_phys_link_event();

  /* reg iface events */
  qosi_reg_iface_event(qos_sp);

  /* set the flow_ev_reg state to TRUE */
  qos_sp->flow_ev_reg = TRUE;

  /* get all NW initiated flow and send ind */
  qosi_send_nw_init_event_ind(qos_sp);
}



/*===========================================================================
  FUNCTION QOSI_DEREG_PS_FLOW_EVENT

  DESCRIPTION
    Deregister for flow events

  PARAMETERS
    qos_sp : QMI QoS instance to deregister flow events for

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_dereg_ps_flow_event
(
  uint8                qos_inst,
  boolean              is_v6
)
{
  qmi_qosi_state_type *  qos_sp;
  int                    index;
  qmi_flow_info_type  *  qmi_flow_info_ptr_arr[QOSI_MAX_QOS_REQUESTS];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check that qos_inst is within range */
  if (qos_inst >= QOS_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!", qos_inst);
    return;
  }

  if(is_v6)
    qos_sp = qmi_qos_state[QOSI_IPV6_STATE][qos_inst];
  else
    qos_sp = qmi_qos_state[QOSI_IPV4_STATE][qos_inst];

  if (qos_sp == NULL)
  {
    LOG_MSG_ERROR_1("QOS state %d is not initialized", qos_inst);
    return;
  }

  if((qos_sp->ps_iface_ptr == NULL) || (qos_sp->flow_ev_reg == FALSE))
  {
    return; //nothing to dereg
  }

  // Clear all the non-default flow info for this qos_sp
  for (index = 0; index < QOSI_MAX_QOS_REQUESTS; index++)
  {
    if (qosi_flow_info[index].cl_sp != NULL)
    {
      if (qmi_qosi_get_qos_sp_by_cl_sp(qosi_flow_info[index].cl_sp) == qos_sp)
      {
        LOG_MSG_INFO2_2("Clear flow info %d, num_qos_reqs = %d",
                        index, qos_sp->num_qos_reqs);

        // Send ind to client
        qmi_qosi_qos_status_ind(qos_sp,
                                qosi_flow_info[index].cl_sp,
                                qosi_flow_info[index].qos_handle,
                                QOSI_STATUS_GONE,
                                QOSI_GONE_EV,
                                PS_EIC_NOT_SPECIFIED);

        // Send ind to info driver
        if (qosi_flow_info[index].qos_status != QOSI_STATUS_DEFAULT)
        {
          qmi_flow_info_ptr_arr[0] = &qosi_flow_info[index];
          qosi_generate_event_report_ind(qos_sp,
                                         qmi_flow_info_ptr_arr,
                                         1,
                                         QOSI_REPORT_STATUS_FLOW_DELETED,
                                         FALSE);
        }

        memset( &qosi_flow_info[index], 0, sizeof(qosi_flow_info[index]) );
        qos_sp->num_qos_reqs--;
      }
    }
  }

  qmi_qosi_sync_clients((uint8)qos_inst);

  /* dereg iface events */
  qosi_dereg_iface_event(qos_sp);

  qos_sp->ps_iface_ptr = NULL; //call torn down

  ps_flow_event_cback_dereg(NULL,
                            FLOW_ACTIVATED_EV,
                            qos_sp->flow_activated_buf_ptr);
  ps_flow_event_cback_dereg(NULL,
                            FLOW_SUSPENDED_EV,
                            qos_sp->flow_suspended_buf_ptr);
  ps_flow_event_cback_dereg(NULL,
                            FLOW_NULL_EV,
                            qos_sp->flow_null_buf_ptr);
  ps_flow_event_cback_dereg(NULL,
                            FLOW_MODIFY_ACCEPTED_EV,
                            qos_sp->flow_modify_accepted_buf_ptr);
  ps_flow_event_cback_dereg(NULL,
                            FLOW_MODIFY_REJECTED_EV,
                            qos_sp->flow_modify_rejected_buf_ptr);
  ps_flow_event_cback_dereg(NULL,
                            FLOW_PRIMARY_MODIFY_RESULT_EV,
                            qos_sp->flow_prim_modify_result_buf_ptr);
  ps_flow_event_cback_dereg(NULL,
                            FLOW_TX_ENABLED_EV,
                            qos_sp->flow_enabled_buf_ptr);
  ps_flow_event_cback_dereg(NULL,
                            FLOW_TX_DISABLED_EV,
                            qos_sp->flow_disabled_buf_ptr);
  ps_flow_event_cback_dereg(NULL,
                            FLOW_INFO_CODE_UPDATED_EV,
                            qos_sp->flow_info_code_updated_buf_ptr);

  /* dereg phys link events */
  qosi_dereg_phys_link_event();

  /* reset the flow_ev_reg state to FALSE */
  qos_sp->flow_ev_reg = FALSE;
}

/*===========================================================================
  FUNCTION QOSI_REG_PHYS_LINK_EVENT

  DESCRIPTION
     Register for phys link events

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_reg_phys_link_event
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_iface_event_enum_type event = IFACE_MIN_EV;
  boolean                  is_err = FALSE;

  if (qmi_qosi_global.phys_link_ev_reg)
  {
    LOG_MSG_INFO2_0("Phys link events already registered");
    return;
  }

  LOG_MSG_INFO2_0("Reg phys link events");

  do
  {  
    if (0 != ps_phys_link_event_cback_reg(NULL,
                           PHYS_LINK_FLOW_ENABLED_EV,
                           qmi_qosi_global.phys_link_flow_enabled_buf_ptr))
    {
        is_err = TRUE;
        event = PHYS_LINK_FLOW_ENABLED_EV;
        break;
    }
  
    if (0 != ps_phys_link_event_cback_reg(NULL,
                           PHYS_LINK_FLOW_DISABLED_EV,
                           qmi_qosi_global.phys_link_flow_disabled_buf_ptr))
    {
        is_err = TRUE;
        event = PHYS_LINK_FLOW_DISABLED_EV;
        break;
      }
  }while(0);
  if(is_err == TRUE)
  {
    LOG_MSG_ERROR_1("Couldn't register for Phys link event (%d)!",event);
    ASSERT( 0 );
    return;
  }

  qmi_qosi_global.phys_link_ev_reg = TRUE;

} /* qosi_reg_phys_link_event() */

/*===========================================================================
  FUNCTION QOSI_DEREG_PHYS_LINK_EVENT

  DESCRIPTION
     De-register for phys link events

  PARAMETERS
    None

  RETURN VALUE
    TRUE: Success
    FALSE: Otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_dereg_phys_link_event
(
  void
)
{
  uint8  state;
  uint8  inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qmi_qosi_global.phys_link_ev_reg == FALSE)
  {
    LOG_MSG_ERROR_0("Phys link events not registered");
    return;
  }

  // If any QOS state has call up, don't dereg
  for (state = 0; state < QOSI_MAX_STATE; state++)
  {
    for (inst = 0; inst < QOS_INSTANCE_MAX; inst++)
    {
      if (qmi_qos_state[state][inst] == NULL)
    {
        continue;
      }
      if (qmi_qos_state[state][inst]->ps_iface_ptr != NULL)
      {
        LOG_MSG_INFO2_1("Call exists on inst %d, not dereg phys link events",
                        inst);
        return;
      }
    }
  }

  LOG_MSG_INFO2_0("Dereg phys link events");

  ps_phys_link_event_cback_dereg(NULL,
                           PHYS_LINK_FLOW_ENABLED_EV,
                           qmi_qosi_global.phys_link_flow_enabled_buf_ptr);

  ps_phys_link_event_cback_dereg(NULL,
                           PHYS_LINK_FLOW_DISABLED_EV,
                           qmi_qosi_global.phys_link_flow_disabled_buf_ptr);

  qmi_qosi_global.phys_link_ev_reg = FALSE;

} /* qosi_dereg_phys_link_event() */

/*===========================================================================
  FUNCTION QOSI_REG_IFACE_EVENT

  DESCRIPTION
     Register for iface events

  PARAMETERS
    qos_sp

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_reg_iface_event
(
  void * sp
)
{
  qmi_qosi_state_type *  qos_sp;
  ps_iface_event_enum_type event = IFACE_MIN_EV;
  boolean                  is_err = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  qos_sp = (qmi_qosi_state_type*)sp;
  ASSERT(qos_sp->ps_iface_ptr);
  do
  {
    if(ps_iface_event_cback_reg( qos_sp->ps_iface_ptr,
                                 IFACE_FLOW_ENABLED_EV,
                                 qos_sp->iface_flow_enabled_buf_ptr
                               ) < 0 )
    {
      is_err = TRUE;
      event = IFACE_FLOW_ENABLED_EV;
      break;
    }
  
    if(ps_iface_event_cback_reg( qos_sp->ps_iface_ptr,
                                 IFACE_FLOW_DISABLED_EV,
                                 qos_sp->iface_flow_disabled_buf_ptr
                               ) < 0 )
    {
      is_err = TRUE;
      event = IFACE_FLOW_DISABLED_EV;
      break;
    }
  }while(0);
  if(is_err == TRUE)
  {
    LOG_MSG_ERROR_1("IFACE event (%d) reg failed",event );
    ASSERT(0);
    return;
  }

} /* qosi_reg_iface_event() */

/*===========================================================================
  FUNCTION QOSI_DEREG_IFACE_EVENT

  DESCRIPTION
     De-register for iface events

  PARAMETERS
    qos_sp

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_dereg_iface_event
(
  void * sp
)
{
  qmi_qosi_state_type *  qos_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);

  qos_sp = (qmi_qosi_state_type*)sp;

  ASSERT(qos_sp->ps_iface_ptr);
 /*FIX ME check iface_flow_enabled_buf_ptr for null*/
  ps_iface_event_cback_dereg( qos_sp->ps_iface_ptr,
                              IFACE_FLOW_ENABLED_EV,
                              qos_sp->iface_flow_enabled_buf_ptr );


  ps_iface_event_cback_dereg( qos_sp->ps_iface_ptr,
                              IFACE_FLOW_DISABLED_EV,
                              qos_sp->iface_flow_disabled_buf_ptr );

} /* qosi_dereg_iface_event() */


/*===========================================================================
  FUNCTION QOSI_QMI_CMD_HDLR()

  DESCRIPTION
    This function processes QMI QoS Cmds.

  PARAMETERS
    cmd_ptr : ptr to QMI QOS cmd buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_qmi_cmd_hdlr
(
  void *  cmd_ptr
)
{
  qmi_qosi_cmd_buf_type *cmd_buf_ptr = NULL;
  uint8 state, inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cmd_ptr);

  /* check that cmd_ptr is not NULL in case ASSERT is removed */
  if (cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("cmd_ptr is NULL!");
    return;
  }
  
  cmd_buf_ptr = (qmi_qosi_cmd_buf_type *)cmd_ptr;

  switch(cmd_buf_ptr->cmd_id)
  {
    case QMI_CMD_QOS_INIT_CB:
      qmi_qosi_process_svc_init(cmd_buf_ptr->data.init_cb.num_instances);
      break;

    case QMI_CMD_QOS_ALLOC_CLID_CB:
      qmi_qosi_process_alloc_clid(&cmd_buf_ptr->data.clid_cb.common_msg_hdr);
      break;

    case QMI_CMD_QOS_DEALLOC_CLID_CB:
      qmi_qosi_process_dealloc_clid(&cmd_buf_ptr->data.clid_cb.common_msg_hdr);
      break;

    case QMI_CMD_QOS_CMD_HDLR_CB:
      qmi_qosi_process_cmd_hdlr(&cmd_buf_ptr->data.cmd_hdlr_cb.msg_hdr,
                                    cmd_buf_ptr->data.cmd_hdlr_cb.sdu_in);
      break;
    case QMI_CMD_PROCESS_FLOW_EVT:
      qosi_process_flow_event(cmd_buf_ptr->data.flow_info.flow_ptr,
                              cmd_buf_ptr->data.flow_info.flow_ptr_cookie,
                              cmd_buf_ptr->data.flow_info.iface_ptr,
                              cmd_buf_ptr->data.flow_info.event,
                              cmd_buf_ptr->data.flow_info.event_info,
                              cmd_buf_ptr->data.flow_info.qos_sp);
      break;

    case QMI_CMD_PROCESS_QOS_PHYS_LINK_EVT:

      LOG_MSG_INFO2_2("Received QoS phys link Ev %d on iface 0x%x",
                      cmd_buf_ptr->data.phys_link_info.event,
                      cmd_buf_ptr->data.phys_link_info.phys_link_ptr);

      for (state = 0; state < QOSI_MAX_STATE; state++)
      {
        for (inst = 0; inst < QOS_INSTANCE_MAX; inst++)
        {
          if ((NULL == qmi_qos_state[state][inst]) || 
              (NULL == qmi_qos_state[state][inst]->ps_iface_ptr))
          {
            continue;
          }
          qosi_process_phys_link_event(cmd_buf_ptr->data.phys_link_info.phys_link_ptr,
                                   cmd_buf_ptr->data.phys_link_info.event,
                                   cmd_buf_ptr->data.phys_link_info.event_info,
                                   qmi_qos_state[state][inst]);
        }
      }
      break;

    case QMI_CMD_PROCESS_QOS_IFACE_EVT:

      LOG_MSG_INFO2_2("Received QoS iface Ev %d on iface 0x%x",
                      cmd_buf_ptr->data.iface_info.event,
                      cmd_buf_ptr->data.iface_info.iface_ptr);

      if ((IFACE_FLOW_ENABLED_EV == cmd_buf_ptr->data.iface_info.event) ||
          (IFACE_FLOW_DISABLED_EV == cmd_buf_ptr->data.iface_info.event))
      {
        qosi_process_iface_flow_event(cmd_buf_ptr->data.iface_info.iface_ptr,
                                   cmd_buf_ptr->data.iface_info.event,
                                   cmd_buf_ptr->data.iface_info.event_info,
                                   cmd_buf_ptr->data.iface_info.qos_sp);
      }
      else
      {
        for( state = 0; state < QOSI_MAX_STATE; state++ )
        {
          for( inst = 0; inst < QOS_INSTANCE_MAX; inst++ )
          {
            if((NULL == qmi_qos_state[state][inst]) || 
               (NULL == qmi_qos_state[state][inst]->client_list ))
            {
              continue;
            }
            qosi_process_iface_event(cmd_buf_ptr->data.iface_info.iface_ptr,
                                     cmd_buf_ptr->data.iface_info.event,
                                     cmd_buf_ptr->data.iface_info.event_info,
                                     qmi_qos_state[state][inst]);
          }
        }
      }
      break;

    case QMI_CMD_QOS_NW_STATUS_CHANGE_EVT:
      qmi_qosi_generate_network_status_ind(
        cmd_buf_ptr->data.nw_aware_info.network_aware_status,
        cmd_buf_ptr->subscription_id);
      break;

    default:
      LOG_MSG_ERROR_1("Invalid QMI cmd with type %d recd",
                      cmd_buf_ptr->cmd_id);
      ASSERT(0);
  }
  qmi_qosi_free_cmd_buf(cmd_ptr);
  return;
}

/*===========================================================================
  FUNCTION QOSI_CALL_STATUS_CB()

  DESCRIPTION
    This function processes the change in call status.

  PARAMETERS
    call_up: flag which indicates current ca;; status -
                 TRUE: UP, FALSE: DOWN

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_call_status_cb
(
  uint32               ep_id,
  uint8                mux_id,
  boolean              is_v6,
  boolean              call_up
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  uint8                        bind_inst;
  qmi_qosi_state_type *        qos_sp_v4;
  qmi_qosi_state_type *        qos_sp_v6;
  uint32                       subs_id;

  // Find the binding inst from ep and mux id
  bind_inst = qosi_get_inst_from_ep_and_mux_id(ep_id, mux_id);

  if(call_up == TRUE)
  {
    if (bind_inst >= QOS_INSTANCE_MAX)
    {
      // Check if we need to put the client into non-mux section
      bind_inst = QMI_INSTANCE_MAX;
      if (mux_id == 0)
      {
        bind_inst = qmux_get_inst_from_ep_id(ep_id); // validate ep_id?
      }

      // Find a free qos state in the mux section
      if (bind_inst >= QMI_INSTANCE_MAX)
      {
        for (bind_inst = QMI_INSTANCE_MAX; bind_inst < QOS_INSTANCE_MAX; bind_inst++)
        {
          if (qmi_qos_state[QOSI_IPV4_STATE][bind_inst] == NULL ||
              qmi_qos_state[QOSI_IPV4_STATE][bind_inst]->binding.ep_id == 0)
          {
            break;
          }
        }
      }

      if ((bind_inst >= QOS_INSTANCE_MAX) || (!qmi_qosi_qos_inst_bind(bind_inst, ep_id, mux_id)))
      {
        LOG_MSG_ERROR_2("Unable to bind to ep 0x%x mux_id %d", ep_id, mux_id);
        return;
      }
    }
    /* Figure out RMNET instance */
    qos_sp_v4 = qmi_qos_state[QOSI_IPV4_STATE][bind_inst];
    qos_sp_v6 = qmi_qos_state[QOSI_IPV6_STATE][bind_inst];
    qos_sp_v4->binding.rmnet_inst = rmnet_inst_from_ep_and_mux_id(
              ep_id, mux_id, qos_sp_v4->constants.is_default);
    qos_sp_v6->binding.rmnet_inst = rmnet_inst_from_ep_and_mux_id(
              ep_id, mux_id, qos_sp_v6->constants.is_default);
     
    if(rmnet_meta_sm_get_subs_id(qos_sp_v4->binding.rmnet_inst,&subs_id))
    {
      qos_sp_v4->binding.subs_id = subs_id;
      LOG_MSG_INFO2_2("Setting QOS subs %d rmnet inst %d call_up %d", qos_sp_v4->binding.subs_id,
                       qos_sp_v4->binding.rmnet_inst);
    }

    if(rmnet_meta_sm_get_subs_id(qos_sp_v6->binding.rmnet_inst,&subs_id))
    {
      qos_sp_v6->binding.subs_id = subs_id;
      LOG_MSG_INFO2_2("Setting QOS subs %d rmnet inst %d call_up %d", qos_sp_v6->binding.subs_id,
                       qos_sp_v6->binding.rmnet_inst);
    }

    qosi_reg_ps_flow_event(bind_inst, is_v6);

  }
  else
  {
    if (bind_inst >= QOS_INSTANCE_MAX)
    {
      LOG_MSG_ERROR_2("Unable to find QOS bind instance from ep 0x%x mux_id %d", ep_id, mux_id);
      return;
    }
    qosi_dereg_ps_flow_event(bind_inst, is_v6);

    qos_sp_v4 = qmi_qos_state[QOSI_IPV4_STATE][bind_inst];
    qos_sp_v6 = qmi_qos_state[QOSI_IPV6_STATE][bind_inst];

    // Unbind QOS if all clients and calls are released
    if ( (qos_sp_v4->client_list == NULL) &&
         (qos_sp_v4->ps_iface_ptr == NULL) &&
         (qos_sp_v6->ps_iface_ptr == NULL) )
    {
      LOG_MSG_INFO2_1("Unbinding QOS since no clients and all calls down %d", bind_inst);
      qmi_qosi_qos_inst_unbind(bind_inst);
      /* Clear RMNET instance*/
      qos_sp_v4->binding.rmnet_inst = RMNET_INSTANCE_MAX;
      qos_sp_v6->binding.rmnet_inst = RMNET_INSTANCE_MAX;
    }
  }
}

/*===========================================================================
  FUNCTION QOSI_GENERATE_EVENT_REPORT_IND

  DESCRIPTION
    Generate the event report ind for registered clients

  PARAMETERS
    qos_sp : QMI QoS instance to deregister flow events for

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_generate_event_report_ind
(
  qmi_qosi_state_type *  qos_sp,
  qmi_flow_info_type  **  qmi_flow_info_ptr_ptr,
  uint16                  num_qos_ids,
  uint8                   flow_change_ev,
  boolean                 new_flow
)
{
  qmi_qosi_client_state_type *  cl_sp;
  uint32                        qos_id;
  uint16                        i;
  ps_iface_addr_family_type     ip_family;
  ds_qmap_cmd_flow_ctrl_type    qmap_fc_cmd;
  uint16                      * uplink_fc_seq_num;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // Send QMAP flow ctrl cmds only if QOS is enabled
  if ( (flow_change_ev == QOSI_REPORT_STATUS_FLOW_ENABLED ||
        flow_change_ev == QOSI_REPORT_STATUS_FLOW_DISABLED ) &&
        rmnet_meta_sm_qos_or_te_fc_enabled(qos_sp->binding.rmnet_inst) )
  {
    ip_family = ps_iface_get_addr_family(qos_sp->ps_iface_ptr);
    if ( (ip_family != IPV4_ADDR) && (ip_family != IPV6_ADDR) )
    {
       LOG_MSG_ERROR_2("Invalid IP family %d for um iface 0x%p",
                       ip_family, qos_sp->ps_iface_ptr);
       ip_family = IFACE_UNSPEC_ADDR_FAMILY;
    }

    for (i = 0; i < num_qos_ids; i++)
    {
      if (qmi_flow_info_ptr_ptr[i] != NULL)
      {
        uplink_fc_seq_num = &qmi_flow_info_ptr_ptr[i]->uplink_fc_seq_num;
        qos_id = qmi_flow_info_ptr_ptr[i]->qos_handle;
      }
      else
      {
        // Default Flow; Use seq no from qos_sp
        uplink_fc_seq_num = &qos_sp->uplink_fc_seq_num;
        qos_id = 0;
      }

      if (flow_change_ev == QOSI_REPORT_STATUS_FLOW_ENABLED)
      {
        qmi_qosi_global.num_flow_enable++;
        ds_qmap_build_flow_enable_cmd(
                       &qmap_fc_cmd,
                       (uint8)ip_family,
                       *uplink_fc_seq_num,
                       qos_id);
      }
      else
      {
        qmi_qosi_global.num_flow_disable++;
        *uplink_fc_seq_num = (*uplink_fc_seq_num) + 1;
        ds_qmap_build_flow_disable_cmd(
                       &qmap_fc_cmd,
                       (uint8)ip_family,
                       *uplink_fc_seq_num,
                       qos_id);
      }

      // Send QMAP command
      rmnet_send_qmap_cmd(qos_sp->binding.rmnet_inst, 
                               sizeof(qmap_fc_cmd), &qmap_fc_cmd);
    }
  }

  /*-------------------------------------------------------------------------
    Go through all clients and send the event rpt ind to any
    client registered for it
  -------------------------------------------------------------------------*/
  cl_sp = qos_sp->client_list;
  while(cl_sp != NULL)
    {
    if ((qos_sp == qmi_qosi_get_qos_sp_by_cl_sp(cl_sp)) &&
        (cl_sp->report_status.report_global_flow == TRUE) &&
         qmi_qosi_verify_client_subs(cl_sp, qos_sp->binding.subs_id, TRUE))
      {
        qmi_qosi_event_report_ind(qos_sp,
                                  cl_sp,
                                  qmi_flow_info_ptr_ptr,
                                  num_qos_ids,
                                  flow_change_ev,
                                  new_flow);
      }
    if ((qos_sp == qmi_qosi_get_qos_sp_by_cl_sp(cl_sp)) &&
        (cl_sp->report_status.report_global_flow_ex == TRUE) &&
         qmi_qosi_verify_client_subs(cl_sp, qos_sp->binding.subs_id, TRUE))
      {
        qmi_qosi_event_report_ind_ex(qos_sp,
                                  cl_sp,
                                  qmi_flow_info_ptr_ptr,
                                  num_qos_ids,
                                  flow_change_ev,
                                  new_flow);
      }
    cl_sp = cl_sp->next;
  }
}

/*===========================================================================
  FUNCTION QOSI_GENERATE_EVENT_REPORT_IND2

  DESCRIPTION
    Generate the event report ind for registered clients

  PARAMETERS
    qos_sp : QMI QoS instance to deregister flow events for
    ev_rep_info : Event report info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_generate_event_report_ind2
(
  qmi_qosi_state_type *  qos_sp,
  qmi_qosi_event_info_type * ev_rep_info
)
{
  qmi_qosi_client_state_type *  cl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Go through all clients and send the event rpt ind to any
    client registered for it
  -------------------------------------------------------------------------*/
  cl_sp = qos_sp->client_list;
  while(cl_sp != NULL)
  {
    if (qos_sp == qmi_qosi_get_qos_sp_by_cl_sp(cl_sp))
    {
      if ( (ev_rep_info->ind_cause ==
                          QMI_QOSI_EV_REPORT_CAUSE_NW_SUPP_QOS_PROFILE_CH) &&
           (cl_sp->report_status.nw_supp_qos_prof.report_nw_supp_qos_profile_ch
                                                                  == TRUE) &&
           ((cl_sp->report_status.nw_supp_qos_prof.tech_pref ==
                       ev_rep_info->event_info.nw_supp_qos_prof.tech_name) ||
            (cl_sp->report_status.nw_supp_qos_prof.tech_pref == WWAN_GROUP)) &&
            qmi_qosi_verify_client_subs(cl_sp, QMI_QOSI_SUBS_PRIMARY, FALSE) )
      {
  /*-------------------------------------------------------------------------
    This 3gpp2 event is currently supported only on primary subs. If at all
    CDMA is ever supported on non-primary subscriptions, client validation
    should rely on the event subs id. 
  -------------------------------------------------------------------------*/
        qmi_qosi_event_report_ind2( qos_sp,
                                    cl_sp,
                                    ev_rep_info );
      }
    }
    cl_sp = cl_sp->next;
  }
} /* qosi_generate_event_report_ind2() */

/*===========================================================================
FUNCTION   QOSI_FLOW_EV_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_flow events
  occurs. Flow events were registered on ALL flows

PARAMETERS:
  flow_ptr      : ptr to flow for which event occured
  event         : type of event
  event_info    : info for the event
  user_data_ptr : user data (qos_sp)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void qosi_flow_ev_cb
(
  ps_flow_type                * flow_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
)
{
  qmi_qosi_cmd_buf_type *cmd_ptr = NULL;
  qmi_qosi_state_type * qos_sp;
  ps_iface_type * iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (user_data_ptr);
#if 0
  LOG_MSG_INFO2_2( "Recvd flow ev %d for flow_ptr 0x%x", event, flow_ptr );
#endif

  iface_ptr = PS_FLOW_GET_IFACE(flow_ptr);
  qos_sp = (qmi_qosi_state_type *) user_data_ptr;
  if (iface_ptr != qos_sp->ps_iface_ptr)
  {
#if 0
    LOG_MSG_INFO1_3("Flow ev on iface(0x%x). For this QMI inst, call is up on iface"
                    "(0x%x). Ignore ev=%d",
                    iface_ptr, qos_sp->ps_iface_ptr, event);
#endif
    return;
  }

  cmd_ptr = (qmi_qosi_cmd_buf_type *)qmi_qosi_get_cmd_buf(QMI_CMD_PROCESS_FLOW_EVT);
  if( cmd_ptr == NULL)
  {
    return;
  }

  cmd_ptr->cmd_id = QMI_CMD_PROCESS_FLOW_EVT;
  cmd_ptr->data.flow_info.flow_ptr = flow_ptr;
  cmd_ptr->data.flow_info.flow_ptr_cookie = flow_ptr->flow_private.cookie;
  cmd_ptr->data.flow_info.iface_ptr = iface_ptr;
  cmd_ptr->data.flow_info.event = event;
  cmd_ptr->data.flow_info.event_info = event_info;
  cmd_ptr->data.flow_info.qos_sp = user_data_ptr;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
} /* qosi_flow_ev_cb() */


/*===========================================================================
FUNCTION   QOSI_PHYS_LINK_EV_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_flow events
  occurs. Flow events were registered on ALL flows

PARAMETERS:
  flow_ptr      : ptr to flow for which event occured
  event         : type of event
  event_info    : info for the event
  user_data_ptr : user data (qos_sp)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void qosi_phys_link_ev_cb
(
  ps_phys_link_type           * phys_link_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
)
{
  qmi_qosi_cmd_buf_type *cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2( "Recvd phys link ev %d for phys_link_ptr 0x%x",
                   event, phys_link_ptr );

  cmd_ptr = (qmi_qosi_cmd_buf_type *)qmi_qosi_get_cmd_buf(QMI_CMD_PROCESS_QOS_PHYS_LINK_EVT);
  if( cmd_ptr == NULL)
  {
    return;
  }

  cmd_ptr->cmd_id = QMI_CMD_PROCESS_QOS_PHYS_LINK_EVT;
  cmd_ptr->data.phys_link_info.phys_link_ptr = phys_link_ptr;
  cmd_ptr->data.phys_link_info.event = event;
  cmd_ptr->data.phys_link_info.event_info = event_info;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
} /* qosi_phys_link_ev_cb() */



/*===========================================================================
FUNCTION   QOSI_PROCESS_FLOW_EVENT()

DESCRIPTION
  This callback function is called when one of the registered ps_flow events
  occurs. Flow events were registered on ALL flows

PARAMETERS:
  flow_ptr      : ptr to flow for which event occured
  flow_ptr_cookie: cookie from the flow ptr
  iface_ptr     : iface ptr on which the event occured
  event         : type of event
  event_info    : info for the event
  user_data_ptr : user data (qos_sp)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void qosi_process_flow_event
(
  ps_flow_type                * flow_ptr,
  uint8                         flow_ptr_cookie,
  ps_iface_type               * iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * sp
)
{
  uint8 i=0;
  qmi_qosi_state_type *  qos_sp;
  qmi_flow_info_type *  qmi_flow_info_ptr_arr[QOSI_MAX_QOS_REQUESTS + 1]; /*+1 for default flow*/
  uint16  qos_id_cnt;
  ps_flow_type * def_flow_ptr;
  boolean        new_flow;
  ps_phys_link_type * phys_link_ptr;
  uint8 j = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  qos_sp = (qmi_qosi_state_type *) sp;
  def_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(qos_sp->ps_iface_ptr);
  qos_id_cnt = 0;

  LOG_MSG_INFO2_3("Received Flow event %d on flow ptr=0x%x on iface ptr=0x%x",
                  event, flow_ptr, iface_ptr);

  /* event on defalut flow */
  if(def_flow_ptr == flow_ptr)
  {
    switch(event)
    {
      case FLOW_TX_ENABLED_EV:
      {
        phys_link_ptr = PS_FLOW_GET_PHYS_LINK(def_flow_ptr);
        /* send flow_enable for qmi qos flows which can be fwded on default */
        if(PS_PHYS_LINK_FLOW_ENABLED(phys_link_ptr))
        {
          /* go through all qmi qos flows */

          for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
          {
            if(qosi_flow_info[i].qos_handle != 0)
            {
              if((qosi_flow_info[i].last_rep == QMI_FLOW_DISABLED) &&
                 (qosi_flow_info[i].fwd_on_default == TRUE) &&
                 (PS_FLOW_GET_STATE(qosi_flow_info[i].flow_ptr) != FLOW_ACTIVATED) &&
                 (PS_FLOW_IS_TX_ENABLED(qosi_flow_info[i].flow_ptr)))
              {
                qmi_flow_info_ptr_arr[qos_id_cnt++] = &(qosi_flow_info[i]);
                qosi_flow_info[i].last_rep = QMI_FLOW_ENABLED;
              }
            }
          }
          /* indicate flow_enabled for default flow */
          qmi_flow_info_ptr_arr[qos_id_cnt++] = NULL;

          /* send event rpt ind including all affected sec flows + default */
          qosi_generate_event_report_ind(qos_sp,
                                         qmi_flow_info_ptr_arr,
                                         qos_id_cnt,
                                         QOSI_REPORT_STATUS_FLOW_ENABLED,
                                         FALSE);
        }
        break;
      }

      case FLOW_TX_DISABLED_EV:
      {
        /* indicate flow_disabled for default flow */

        /* send flow_disable for qmi qos flows which are going on default */
        for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
        {
          if(qosi_flow_info[i].qos_handle != 0)
          {
            if(qosi_flow_info[i].last_rep == QMI_FLOW_ENABLED)
            {
              /* check if flow going on default */
              if(PS_FLOW_GET_STATE(qosi_flow_info[i].flow_ptr) != FLOW_ACTIVATED)
              {
                qmi_flow_info_ptr_arr[qos_id_cnt++] = &(qosi_flow_info[i]);
                qosi_flow_info[i].last_rep = QMI_FLOW_DISABLED;
              }
            }
          }
        }

        /* indicate flow_disabled for default flow */
        qmi_flow_info_ptr_arr[qos_id_cnt++] = NULL;

        /* send event rpt ind including all affected sec flows + default */
        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_DISABLED,
                                       FALSE);
        break;
      }

      case FLOW_PRIMARY_MODIFY_RESULT_EV:
      {
        if (event_info.primary_qos_modify_result_info.is_modify_succeeded)
        {
          qmi_qosi_primary_qos_event_ind( qos_sp,
                                          NULL,
                                          QOSI_PRIMARY_MODIFY_SUCCESS_EV,
                                          NULL );
        }
        else
        {
          qmi_qosi_primary_qos_event_ind( qos_sp,
                                          NULL,
                                          QOSI_PRIMARY_MODIFY_FAILURE_EV,
                                          NULL );
        }
        break;
      }

      case FLOW_MODIFY_ACCEPTED_EV:
      {
        qmi_qosi_qos_status_ind(qos_sp,
                                NULL,
                                0, /* default flow */
                                QOSI_STATUS_DEFAULT,
                                QOSI_MODIFY_ACCEPTED_EV,
                                event_info.flow_event_info.info_code);
        break;
      }

      case FLOW_MODIFY_REJECTED_EV:
      {
        qmi_qosi_qos_status_ind(qos_sp,
                                NULL,
                                0, /* default flow */
                                QOSI_STATUS_DEFAULT,
                                QOSI_MODIFY_REJECTED_EV,
                                event_info.flow_event_info.info_code);
        break;
      }

      default:
      {
        LOG_MSG_ERROR_1("Unregistered flow event %d recvd, ignoring", event);
        break;
      }
    }
    return;
  }

  /* IF event for non-default flow */
  for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
  {
    if(qosi_flow_info[i].flow_ptr == flow_ptr)
    {
        break;
    }
  }

  /* if this flow is not in qos flow info table, it is a NW init flow */
  if(i == QOSI_MAX_QOS_REQUESTS)
  {
    /*-----------------------------------------------------------------------
      For network initiated flows, check if the flow iface is the same as QMI
      iface to filter out flow events from other ifaces. Check is not needed
      in UE inited flows as the flowptr is checked against cached flow ptrs.
    -----------------------------------------------------------------------*/
    if (iface_ptr != qos_sp->ps_iface_ptr)
    {
      LOG_MSG_INFO2_0("Received Flow event for non-QMI qos client, ignoring");
      return;
    }

    /* find it in the nw init qos flow info table */
    for (j = 0; j < QOSI_MAX_NETWORK_INIT_FLOW_NUM; j++)
    {
      if (nw_init_qosi_flow_info[j].flow_ptr == flow_ptr)
      {
          break;
      }
    }    

    if (j < QOSI_MAX_NETWORK_INIT_FLOW_NUM)
    {
      LOG_MSG_INFO2_0("Found nw inited flow in cache");

    }
    else 
    {
      if (!PS_FLOW_IS_NW_INITIATED(flow_ptr))
      {
        LOG_MSG_INFO2_0("Flow cannot be found in UE init cache and is not NW init either, ignoring");
        return;
      }

      /* find empty slot to store nw inited flow info */
      for (j = 0; j < QOSI_MAX_NETWORK_INIT_FLOW_NUM; j++)
      {
        if (nw_init_qosi_flow_info[j].qos_handle == 0)
        {
          break;
        }
      }

      if (j >= QOSI_MAX_NETWORK_INIT_FLOW_NUM)
      {
        LOG_MSG_ERROR_0("QOS nw inited handle slots exhausted");
        return;
      }
      else
      {
        nw_init_qosi_flow_info[j].qos_handle = (uint32) ps_handle_mgr_get_handle(PS_HANDLE_MGR_CLIENT_QOS,
                                               (uint16)(j+ QOSI_MAX_QOS_REQUESTS + 1));
        if (PS_HANDLE_MGR_INVALID_HANDLE == nw_init_qosi_flow_info[j].qos_handle)
        {
          LOG_MSG_ERROR_0("Invalid handle returned by handle mgr");
          nw_init_qosi_flow_info[j].qos_handle = 0;
          return;
        }
        else
        {
          LOG_MSG_INFO2_2("Mapping flow_ptr 0x%x to handle 0x%x", 
                           flow_ptr, nw_init_qosi_flow_info[j].qos_handle);

          nw_init_qosi_flow_info[j].flow_ptr   = flow_ptr;
          nw_init_qosi_flow_info[j].uplink_fc_seq_num = 0;
        }
      }
    }

    qos_id_cnt = 0;
    qmi_flow_info_ptr_arr[qos_id_cnt++] = &(nw_init_qosi_flow_info[j]);

    LOG_MSG_INFO2_2("Sending event %d for NI Qos Flow %x", event, flow_ptr);
  
    /* Determine if event is for new flow or exisiting flow based on
       event info which contains previous flow state. Can do the same for UE
       inited flows too instead of relying on cached flow status in qmi */
    if (event_info.flow_event_info.state == FLOW_NULL
        || event_info.flow_event_info.state == FLOW_ACTIVATING )
    {
      new_flow = TRUE;
    }
    else
    {
      new_flow = FALSE;
    }
    /*-------------------------------------------------------------------------
      For NW init flow, no book keeping is needed, we will just generate
      event report ind
    -------------------------------------------------------------------------*/
    switch( event )
    {
      case FLOW_ACTIVATED_EV:
      {
        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_ACTIVATED,
                                       new_flow);
        break;
      }
      case FLOW_SUSPENDED_EV:
      {
        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_SUSPENDED,
                                       new_flow);
        break;
      }
      case FLOW_TX_DISABLED_EV:
      {
        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_DISABLED,
                                       new_flow);
        break;
      }
      case FLOW_NULL_EV:
      {
        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_DELETED,
                                       new_flow);
        if (j < QOSI_MAX_NETWORK_INIT_FLOW_NUM)
        {
          memset(&(nw_init_qosi_flow_info[j]), 0, sizeof(nw_init_qosi_flow_info[j]));
        }
        break;
      }
      case FLOW_TX_ENABLED_EV:
      {
        phys_link_ptr = PS_FLOW_GET_PHYS_LINK(flow_ptr);
        if(PS_PHYS_LINK_FLOW_ENABLED(phys_link_ptr))
        {
          qosi_generate_event_report_ind(qos_sp,
                                         qmi_flow_info_ptr_arr,
                                         qos_id_cnt,
                                         QOSI_REPORT_STATUS_FLOW_ENABLED,
                                         new_flow);
        }
        break;
      }
      case FLOW_MODIFY_ACCEPTED_EV:
      {
        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_MODIFIED,
                                       new_flow);
        break;
      }
      default:
      {
        LOG_MSG_ERROR_1("Unregistered flow event %d recvd, ignoring", event);
        break;
      }
    }
  }
  /* if the flow is found, copy its info in the flow ptr array */
  else
  {
    /* Ignore the event on non-default flow (UE initiated) if the cookie of
       this flow ptr does not match with the one in qmi cache */
    if ( qosi_flow_info[i].flow_ptr_cookie != flow_ptr_cookie )
    {
      LOG_MSG_INFO1_3("Ignoring flow event %d due to cookie mismatch (%d != %d)",
                      event,
                      qosi_flow_info[i].flow_ptr_cookie,
                      flow_ptr_cookie);
      return;
    }

    qos_id_cnt = 0;
    qmi_flow_info_ptr_arr[qos_id_cnt++] = &(qosi_flow_info[i]);
    if(qosi_flow_info[i].qos_status == QOSI_STATUS_DEFAULT)
    {
      new_flow = TRUE;
    }
    else
    {
      new_flow = FALSE;
    }
    /*-------------------------------------------------------------------------
      Act on Flow events
      Generate QoS Status ind and Event_Report ind for the client
      as appropriate
    -------------------------------------------------------------------------*/

    switch( event )
    {
      case FLOW_ACTIVATED_EV:
      {
        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_ACTIVATED,
                                       new_flow);
        new_flow = FALSE;

        if(qosi_flow_info[i].last_rep == QMI_FLOW_DISABLED)
        {
          phys_link_ptr = PS_FLOW_GET_PHYS_LINK(qosi_flow_info[i].flow_ptr);
          if((PS_FLOW_IS_TX_ENABLED(qosi_flow_info[i].flow_ptr)) &&
             (PS_PHYS_LINK_FLOW_ENABLED(phys_link_ptr)))
          {
            qosi_generate_event_report_ind(qos_sp,
                                           qmi_flow_info_ptr_arr,
                                           qos_id_cnt,
                                           QOSI_REPORT_STATUS_FLOW_ENABLED,
                                           new_flow);
            qosi_flow_info[i].last_rep = QMI_FLOW_ENABLED;
          }
        }

        qmi_qosi_qos_status_ind(qos_sp,
                                qosi_flow_info[i].cl_sp,
                                qosi_flow_info[i].qos_handle,
                                QOSI_STATUS_ACTIVATED,
                                QOSI_ACTIVATED_EV,
                                event_info.flow_event_info.info_code);

        qosi_flow_info[i].qos_status = QOSI_STATUS_ACTIVATED;
        break;
      }

      case FLOW_SUSPENDED_EV:
      {
        if(qosi_flow_info[i].last_rep == QMI_FLOW_ENABLED)
        {
          if((qosi_flow_info[i].fwd_on_default == FALSE) ||
             ((PS_IFACE_COMMON_DEFAULT_IS_FLOW_ENABLED((qos_sp->ps_iface_ptr))) == FALSE))
          {
            qosi_generate_event_report_ind(qos_sp,
                                           qmi_flow_info_ptr_arr,
                                           qos_id_cnt,
                                           QOSI_REPORT_STATUS_FLOW_DISABLED,
                                           new_flow);
             new_flow = FALSE;
             qosi_flow_info[i].last_rep = QMI_FLOW_DISABLED;
          }
        }

        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_SUSPENDED,
                                       new_flow);

        qmi_qosi_qos_status_ind(qos_sp,
                                qosi_flow_info[i].cl_sp,
                                qosi_flow_info[i].qos_handle,
                                QOSI_STATUS_SUSPENDED,
                                QOSI_SUSPENDED_EV,
                                event_info.flow_event_info.info_code);

        qosi_flow_info[i].qos_status = QOSI_STATUS_SUSPENDED;
        break;
      }

      case FLOW_NULL_EV:
      {
        qmi_qosi_qos_status_ind(qos_sp,
                                qosi_flow_info[i].cl_sp,
                                qosi_flow_info[i].qos_handle,
                                QOSI_STATUS_GONE,
                                QOSI_GONE_EV,
                                event_info.flow_event_info.info_code);

        if(qosi_flow_info[i].qos_status != QOSI_STATUS_DEFAULT)
        {
        qosi_generate_event_report_ind(qos_sp,
                                         qmi_flow_info_ptr_arr,
                                         qos_id_cnt,
                                         QOSI_REPORT_STATUS_FLOW_DELETED,
                                         new_flow);
        }

        qosi_flow_info[i].qos_status = QOSI_STATUS_GONE;
        /*---------------------------------------------------------------------
          Since the QoS is released for this flow, clear all the info for
          this flow in client state.
          Decrement the num_qos_reqs for the client and overall for the
          QMI instance
        ---------------------------------------------------------------------*/
        memset(&(qosi_flow_info[i]), 0, sizeof(qosi_flow_info[i]));
        qos_sp->num_qos_reqs--;

        break;
      }

      case FLOW_TX_ENABLED_EV:
      {
        if(qosi_flow_info[i].qos_status != QOSI_STATUS_DEFAULT)
        {
          phys_link_ptr = PS_FLOW_GET_PHYS_LINK(flow_ptr);
          if(PS_PHYS_LINK_FLOW_ENABLED(phys_link_ptr))
          {
            if(qosi_flow_info[i].last_rep == QMI_FLOW_DISABLED)
            {
              if((event_info.flow_event_info.state == FLOW_ACTIVATED) ||
                 ((qosi_flow_info[i].fwd_on_default == TRUE) &&
                  ((PS_IFACE_COMMON_DEFAULT_IS_FLOW_ENABLED((qos_sp->ps_iface_ptr))) == TRUE) ))
              {
                qosi_generate_event_report_ind(qos_sp,
                                               qmi_flow_info_ptr_arr,
                                               qos_id_cnt,
                                               QOSI_REPORT_STATUS_FLOW_ENABLED,
                                               new_flow);
                qosi_flow_info[i].last_rep = QMI_FLOW_ENABLED;
              }
            }
          }
        }
        break;
      }

      case FLOW_TX_DISABLED_EV:
      {
        if(qosi_flow_info[i].qos_status != QOSI_STATUS_DEFAULT)
        {
          if(qosi_flow_info[i].last_rep == QMI_FLOW_ENABLED)
          {
             qosi_generate_event_report_ind(qos_sp,
                                           qmi_flow_info_ptr_arr,
                                           qos_id_cnt,
                                           QOSI_REPORT_STATUS_FLOW_DISABLED,
                                           new_flow);
            qosi_flow_info[i].last_rep = QMI_FLOW_DISABLED;
          }
        }
        break;
      }

      case FLOW_MODIFY_ACCEPTED_EV:
      {
        qmi_qosi_qos_status_ind(qos_sp,
                                qosi_flow_info[i].cl_sp,
                                qosi_flow_info[i].qos_handle,
                                qosi_flow_info[i].qos_status,
                                QOSI_MODIFY_ACCEPTED_EV,
                                event_info.flow_event_info.info_code);

        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_MODIFIED,
                                       new_flow);
        break;
      }

      case FLOW_MODIFY_REJECTED_EV:
      {
        qmi_qosi_qos_status_ind(qos_sp,
                                qosi_flow_info[i].cl_sp,
                                qosi_flow_info[i].qos_handle,
                                qosi_flow_info[i].qos_status,
                                QOSI_MODIFY_REJECTED_EV,
                                event_info.flow_event_info.info_code);
        break;
      }

      case FLOW_INFO_CODE_UPDATED_EV:
      {
        qmi_qosi_qos_status_ind(qos_sp,
                                qosi_flow_info[i].cl_sp,
                                qosi_flow_info[i].qos_handle,
                                qosi_flow_info[i].qos_status,
                                QOSI_INFO_CODE_UPDATED_EV,
                                event_info.flow_event_info.info_code);
        break;
      }

      default:
      {
        LOG_MSG_ERROR_1("Unregistered flow event %d recvd, ignoring", event);
        break;
      }
    } /* switch( event ) */
  }
} /* qmi_qos_process_flow_event() */



/*===========================================================================
FUNCTION   QOSI_PROCESS_PHYS_LINK_EVENT()

DESCRIPTION
  This callback function is called when one of the registered ps_flow events
  occurs. Flow events were registered on ALL flows

PARAMETERS:
  flow_ptr      : ptr to flow for which event occured
  event         : type of event
  event_info    : info for the event
  user_data_ptr : user data (qos_sp)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void qosi_process_phys_link_event
(
  ps_phys_link_type           * phys_link_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * sp
)
{
  uint8 i=0;
  qmi_qosi_state_type *  qos_sp;
  qmi_flow_info_type *  qmi_flow_info_ptr_arr[QOSI_MAX_QOS_REQUESTS+1]; /*+1 for default flow*/
  uint16  qos_id_cnt;
  ps_phys_link_type * def_phys_link_ptr;
  ps_flow_type * def_flow_ptr;
  ps_phys_link_type* phys_link_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  qos_sp = (qmi_qosi_state_type *) sp;
  def_phys_link_ptr = PS_IFACE_GET_PHYS_LINK(qos_sp->ps_iface_ptr);
  qos_id_cnt = 0;

  /* event on defalut phys link */
  if(def_phys_link_ptr == phys_link_ptr)
  {
    switch(event)
    {
      case PHYS_LINK_FLOW_ENABLED_EV:
      {
        def_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(qos_sp->ps_iface_ptr);
        /* send flow_enable for qmi qos flows which can be fwded on default */
        if(PS_FLOW_IS_TX_ENABLED(def_flow_ptr))
        {
          /* go through all qmi qos flows */

          for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
          {
            if(qosi_flow_info[i].qos_handle != 0)
            {
              if((qosi_flow_info[i].last_rep == QMI_FLOW_DISABLED) &&
                 (qosi_flow_info[i].fwd_on_default == TRUE) &&
                 (PS_FLOW_GET_STATE(qosi_flow_info[i].flow_ptr) != FLOW_ACTIVATED) &&
                 (PS_FLOW_IS_TX_ENABLED(qosi_flow_info[i].flow_ptr)))
              {
                  qmi_flow_info_ptr_arr[qos_id_cnt++] = &(qosi_flow_info[i]);
                  qosi_flow_info[i].last_rep = QMI_FLOW_ENABLED;
              }
            }
          }

          /* Process all the NW init flows as well */
          for (i = 0; (i < QOSI_MAX_NETWORK_INIT_FLOW_NUM && qos_id_cnt < QOSI_MAX_QOS_REQUESTS); i++)
          {
            /* Check if the flow is associated with the QOS iface */
            if ((nw_init_qosi_flow_info[i].qos_handle != 0) &&
                (PS_FLOW_GET_IFACE(nw_init_qosi_flow_info[i].flow_ptr) == qos_sp->ps_iface_ptr) &&
                (PS_FLOW_IS_TX_ENABLED(nw_init_qosi_flow_info[i].flow_ptr)))
            {
              qmi_flow_info_ptr_arr[qos_id_cnt++] = &(nw_init_qosi_flow_info[i]);
            }
          }    

          /* indicate flow_enabled for default flow */
          qmi_flow_info_ptr_arr[qos_id_cnt++] = NULL;

          /* send event rpt ind including all affected sec flows + default */
          qosi_generate_event_report_ind(qos_sp,
                                         qmi_flow_info_ptr_arr,
                                         qos_id_cnt,
                                         QOSI_REPORT_STATUS_FLOW_ENABLED,
                                         FALSE);
        }
        /* need to handle case when default flow is not enabled, but some other
           flow bound to same phys link is */
        break;
      }

      case PHYS_LINK_FLOW_DISABLED_EV:
      {
        /* indicate flow_disabled for default flow */

        /* send flow_disable for qmi qos flows which are going on default */
        for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
        {
          if(qosi_flow_info[i].qos_handle != 0)
          {
            if(qosi_flow_info[i].last_rep == QMI_FLOW_ENABLED)
            {
               /* check if flow going on default */
              if(PS_FLOW_GET_STATE(qosi_flow_info[i].flow_ptr) != FLOW_ACTIVATED)
              {
                qmi_flow_info_ptr_arr[qos_id_cnt++] = &(qosi_flow_info[i]);
                qosi_flow_info[i].last_rep = QMI_FLOW_DISABLED;
              }
            }
          }
        }

        /* Process all the NW init flows as well */
        for (i = 0; i < QOSI_MAX_NETWORK_INIT_FLOW_NUM && qos_id_cnt < QOSI_MAX_QOS_REQUESTS ; i++)
        {
           phys_link_flow_ptr = PS_FLOW_GET_PHYS_LINK(nw_init_qosi_flow_info[i].flow_ptr);
          /* Check if the flow is associated with the QOS iface and also if associated phys link 
             flow is disabled */
          if ((nw_init_qosi_flow_info[i].qos_handle != 0) &&
              (PS_FLOW_GET_IFACE(nw_init_qosi_flow_info[i].flow_ptr) == qos_sp->ps_iface_ptr) &&
              (!PS_PHYS_LINK_FLOW_ENABLED(phys_link_flow_ptr)))
          {
            qmi_flow_info_ptr_arr[qos_id_cnt++] = &(nw_init_qosi_flow_info[i]);
          }
        }    

        /* indicate flow_disabled for default flow */
        qmi_flow_info_ptr_arr[qos_id_cnt++] = NULL;

        /* send event rpt ind including all affected sec flows + default */
        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_DISABLED,
                                       FALSE);
        break;
      }

      default:
      {
        LOG_MSG_ERROR_1("Unregistered default flow event %d recvd, ignoring",
                        event);
        break;
      }
    }
    return;
  }

  /* IF event for non-default phys link */
  /*-------------------------------------------------------------------------
    Act on Flow events
    Generate QoS Status ind and Event_Report ind for the client
    as appropriate
  -------------------------------------------------------------------------*/
  qos_id_cnt = 0;

  switch( event )
  {
    case PHYS_LINK_FLOW_ENABLED_EV:
    {
      /*---------------------------------------------------------------------
        Check if the phys link corresponds to any QMI QoS flows
        If not then ignore the phys link event
      ---------------------------------------------------------------------*/
      for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
      {
        if(PS_FLOW_GET_PHYS_LINK(qosi_flow_info[i].flow_ptr) != phys_link_ptr)
        {
          continue;
        }

        if(qosi_flow_info[i].qos_status != QOSI_STATUS_DEFAULT)
        {
          if(PS_FLOW_IS_TX_ENABLED(qosi_flow_info[i].flow_ptr))
          {
            if(qosi_flow_info[i].last_rep == QMI_FLOW_DISABLED)
            {
              if((event_info.flow_event_info.state == FLOW_ACTIVATED) ||
                 ((qosi_flow_info[i].fwd_on_default == TRUE) &&
                  ((PS_IFACE_COMMON_DEFAULT_IS_FLOW_ENABLED((qos_sp->ps_iface_ptr))) == TRUE) ))
              {
                qmi_flow_info_ptr_arr[qos_id_cnt++] = &(qosi_flow_info[i]);
                qosi_flow_info[i].last_rep = QMI_FLOW_ENABLED;
              }
            }
          }
        }
      }
      
      /* Process all the NW init flows as well */
      for (i = 0; i < QOSI_MAX_NETWORK_INIT_FLOW_NUM && qos_id_cnt < QOSI_MAX_QOS_REQUESTS ; i++)
      {
        /* Check if the flow is associated with the phys link on which event occurred
           and also if it is associated with the QOS iface */
        if ((nw_init_qosi_flow_info[i].qos_handle != 0) &&
            (PS_FLOW_GET_PHYS_LINK(nw_init_qosi_flow_info[i].flow_ptr) == phys_link_ptr) &&
            (PS_FLOW_GET_IFACE(nw_init_qosi_flow_info[i].flow_ptr) == qos_sp->ps_iface_ptr) &&
            (PS_FLOW_IS_TX_ENABLED(nw_init_qosi_flow_info[i].flow_ptr)))
        {
          qmi_flow_info_ptr_arr[qos_id_cnt++] = &(nw_init_qosi_flow_info[i]);
        }
      }    

      if( 0 < qos_id_cnt )
      {
        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_ENABLED,
                                       FALSE);
      }
      break;
    }

    case PHYS_LINK_FLOW_DISABLED_EV:
    {
      for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
      {
        if(PS_FLOW_GET_PHYS_LINK(qosi_flow_info[i].flow_ptr) != phys_link_ptr)
        {
          continue;
        }

        if(qosi_flow_info[i].qos_status != QOSI_STATUS_DEFAULT)
        {
          if(qosi_flow_info[i].last_rep == QMI_FLOW_ENABLED)
          {
            qmi_flow_info_ptr_arr[qos_id_cnt++] = &(qosi_flow_info[i]);
            qosi_flow_info[i].last_rep = QMI_FLOW_DISABLED;
          }
        }
      }

      /* Process all the NW init flows as well */
      for (i = 0; i < QOSI_MAX_NETWORK_INIT_FLOW_NUM && qos_id_cnt < QOSI_MAX_QOS_REQUESTS ; i++)
      {
        /* Check if the flow is associated with the phys link on which event occurred
           and also if it is associated with the QOS iface. Send event if flow is also
           disabled on the default phys link/ flow */
        if ((nw_init_qosi_flow_info[i].qos_handle != 0) &&
            (PS_FLOW_GET_PHYS_LINK(nw_init_qosi_flow_info[i].flow_ptr) == phys_link_ptr) &&
            (PS_FLOW_GET_IFACE(nw_init_qosi_flow_info[i].flow_ptr) == qos_sp->ps_iface_ptr) &&
            (!PS_IFACE_COMMON_DEFAULT_IS_FLOW_ENABLED(qos_sp->ps_iface_ptr )))
        {
          qmi_flow_info_ptr_arr[qos_id_cnt++] = &(nw_init_qosi_flow_info[i]);
        }
      }  
        
      if(0 < qos_id_cnt)
      {
        qosi_generate_event_report_ind(qos_sp,
                                       qmi_flow_info_ptr_arr,
                                       qos_id_cnt,
                                       QOSI_REPORT_STATUS_FLOW_DISABLED,
                                       FALSE);
      }
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Unregistered flow event %d recvd, ignoring", event);
      break;
    }
  } /* switch( event ) */
} /* qmi_qos_process_phys_link_event() */


/*===========================================================================
FUNCTION   QOSI_IFACE_EV_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on RmNet interface.

PARAMETERS:
  iface_ptr      : ptr to iface for which event occured
  event          : type of event
  event_info     : info for the event
  user_data_ptr  : user data (qos_sp)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void qosi_iface_ev_cb
(
  ps_iface_type             * iface_ptr,
  ps_iface_event_enum_type    event,
  ps_iface_event_info_u_type  event_info,
  void                      * user_data_ptr
)
{
  qmi_qosi_cmd_buf_type *cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2( "Recvd iface ev %d for iface_ptr 0x%x", event, iface_ptr );

  cmd_ptr = (qmi_qosi_cmd_buf_type *)qmi_qosi_get_cmd_buf(QMI_CMD_PROCESS_QOS_IFACE_EVT);
  if( cmd_ptr == NULL)
  {
    return;
  }

  cmd_ptr->cmd_id = QMI_CMD_PROCESS_QOS_IFACE_EVT;
  cmd_ptr->data.iface_info.iface_ptr   = iface_ptr;
  cmd_ptr->data.iface_info.event       = event;
  cmd_ptr->data.iface_info.event_info  = event_info;
  cmd_ptr->data.iface_info.qos_sp      = user_data_ptr;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);

} /* qosi_iface_ev_cb() */


/*===========================================================================
FUNCTION   QOSI_PROCESS_IFACE_EVENT()

DESCRIPTION
  This function is called when one of the registered ps_iface events
  occur on RmNet interface.

PARAMETERS:
  iface_ptr      : ptr to iface for which event occured
  event          : type of event
  event_info     : info for the event
  sp             : qos_sp

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void qosi_process_iface_event
(
  ps_iface_type             * iface_ptr,
  ps_iface_event_enum_type    event,
  ps_iface_event_info_u_type  event_info,
  void                      * sp
)
{
  qmi_qosi_state_type *  qos_sp;
  int16                 ps_errno;
  int                    return_val;
  qmi_qosi_event_info_type *ev_rep_info = NULL;
  ps_iface_ioctl_get_network_supported_qos_profiles_type *nw_supp_qos_profiles = NULL;
  uint8                  i;
  ps_iface_name_enum_type iface_name;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == iface_ptr)
  {
    LOG_MSG_INFO1_0("Received NULL iface pointer");
    return;
  }

  ASSERT (sp);
  qos_sp = (qmi_qosi_state_type *) sp;

  iface_name = (ps_iface_name_enum_type)
                       PS_IFACE_GET_NAME_FROM_ID(PS_IFACE_GET_ID(iface_ptr));
  /*-------------------------------------------------------------------------
    Act on Iface events. QoS Aware, UNAware
  -------------------------------------------------------------------------*/
  switch( event )
  {
    case IFACE_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV:
        PS_SYSTEM_HEAP_MEM_ALLOC(ev_rep_info,
                     sizeof(qmi_qosi_event_info_type),
                     qmi_qosi_event_info_type*);
      if (NULL == ev_rep_info)
      {
        return;
      }

      memset(ev_rep_info, 0,sizeof(qmi_qosi_event_info_type));
      ev_rep_info->event_info.nw_supp_qos_prof.tech_name = iface_name;

      PS_SYSTEM_HEAP_MEM_ALLOC (nw_supp_qos_profiles,
                              sizeof(ps_iface_ioctl_get_network_supported_qos_profiles_type),
                              ps_iface_ioctl_get_network_supported_qos_profiles_type*);

      if (NULL == nw_supp_qos_profiles)
      {
        PS_SYSTEM_HEAP_MEM_FREE(ev_rep_info);
        return;
      }

      memset(nw_supp_qos_profiles, 0,sizeof(ps_iface_ioctl_get_network_supported_qos_profiles_type));
      if ( (ev_rep_info->event_info.nw_supp_qos_prof.tech_name != CDMA_SN_IFACE) &&
           (ev_rep_info->event_info.nw_supp_qos_prof.tech_name != EPC_IFACE) )
      {
        LOG_MSG_ERROR_1("Don't report nw supported qos profile for iface_name=%d",
                        iface_ptr->name);
        ASSERT(0);
        break;
      }
      return_val = ps_iface_ioctl(iface_ptr,
                           PS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES,
                           nw_supp_qos_profiles,
                           &ps_errno);
      if (return_val == -1)
      {
        break;
      }

      if((qmi_qosi_global.nw_supp_qos_profiles.profile_count == 
           nw_supp_qos_profiles->profile_count) &&
         (memcmp((void*)qmi_qosi_global.nw_supp_qos_profiles.profile_value,
                 (void*)nw_supp_qos_profiles->profile_value,
                 nw_supp_qos_profiles->profile_count*sizeof(uint16)) == 0))
      {
        break;
      }

      ev_rep_info->ind_cause = QMI_QOSI_EV_REPORT_CAUSE_NW_SUPP_QOS_PROFILE_CH;
      ev_rep_info->event_info.nw_supp_qos_prof.profile_count =
                                          nw_supp_qos_profiles->profile_count;
      qmi_qosi_global.nw_supp_qos_profiles.profile_count = 
                                          nw_supp_qos_profiles->profile_count;
      for (i=0; i<nw_supp_qos_profiles->profile_count; i++)
      {
        ev_rep_info->event_info.nw_supp_qos_prof.profile_value[i] =
                                       nw_supp_qos_profiles->profile_value[i];
        qmi_qosi_global.nw_supp_qos_profiles.profile_value[i] =
                                       nw_supp_qos_profiles->profile_value[i];
      }
      qosi_generate_event_report_ind2(qos_sp, ev_rep_info);
      break;

    default:
      LOG_MSG_ERROR_1("Unregistered event %d recvd, ignoring", event);
      break;
  } /* switch( event ) */

  if(ev_rep_info != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(ev_rep_info);
  }

  if(nw_supp_qos_profiles != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(nw_supp_qos_profiles);
  }

} /* qosi_process_iface_event() */

/*===========================================================================
FUNCTION   QOSI_PROCESS_IFACE_FLOW_EVENT()

DESCRIPTION
  This function is called when one of the registered ps_iface events
  occurs .

PARAMETERS:
  iface_ptr      : ptr to iface for which event occured
  event          : type of event
  event_info     : info for the event
  sp             : qos_sp

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void qosi_process_iface_flow_event
(
  ps_iface_type             * iface_ptr,
  ps_iface_event_enum_type    event,
  ps_iface_event_info_u_type  event_info,
  void                      * sp
)
{
  qmi_flow_info_type  * qmi_flow_info_ptr_arr[QOSI_MAX_QOS_REQUESTS + QOSI_MAX_NETWORK_INIT_FLOW_NUM + 1]; /*+1 for default flow*/
  qmi_qosi_state_type * qos_sp;
  ps_flow_type        * ps_flow_ptr;
  ps_flow_type        * def_flow_ptr;
  void                * ps_flow_handle;
  void                * new_ps_flow_handle;
  boolean               result;
  uint16                num_qos_ids = 0;
  uint8                 itr;
  boolean               send_info = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == iface_ptr)
  {
    LOG_MSG_INFO1_0("Received NULL iface pointer");
    return;
  }

  ASSERT (sp);
  qos_sp = (qmi_qosi_state_type *) sp;


  if (qos_sp->ps_iface_ptr == NULL)
  {
    LOG_MSG_INFO1_0("Received NULL PS-Iface pointer, Dropping  Event!");
    return;
  }

  def_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(qos_sp->ps_iface_ptr);
  ASSERT (def_flow_ptr);

  memset(qmi_flow_info_ptr_arr, 0, sizeof(qmi_flow_info_type*) * 
         (QOSI_MAX_QOS_REQUESTS + QOSI_MAX_NETWORK_INIT_FLOW_NUM + 1));

  /* get all flows from the existing iface */
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  ps_flow_handle = ps_iface_get_sec_flow_handle(qos_sp->ps_iface_ptr);

  while (num_qos_ids < (QOSI_MAX_QOS_REQUESTS + QOSI_MAX_NETWORK_INIT_FLOW_NUM) 
         && ps_flow_handle != NULL)
  {
    result = ps_iface_get_sec_flow_by_handle(qos_sp->ps_iface_ptr,
                                             ps_flow_handle,
                                             &ps_flow_ptr,
                                             &new_ps_flow_handle);
    if (!result)
    {
      LOG_MSG_ERROR_0("Returning incomplete list of flows");
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      goto bail;
    }

    LOG_MSG_INFO1_2("Found flow 0x%x on iface 0x%x",
                    ps_flow_ptr, qos_sp->ps_iface_ptr);

    /* Check if it is a NW initiated flow */
    if (PS_FLOW_IS_NW_INITIATED(ps_flow_ptr))
    {
      /* Check that for IFACE_FLOW_ENABLED_EV, flow is enabled on the NW
         initiated flow and corresponding physlink also. No checks needed
         for IFACE_FLOW_DISABLED_EV */
      if ((IFACE_FLOW_DISABLED_EV == event) ||
          (PS_IFACE_COMMON_IS_FLOW_ENABLED(iface_ptr, ps_flow_ptr)))
      {
        send_info = TRUE;
      }
    }
    else /* UE initiated flow */
    {
      for(itr=0; itr < QOSI_MAX_QOS_REQUESTS; itr++)
      {
        if(qosi_flow_info[itr].qos_handle == (uint32) ps_flow_ptr)
        {
          switch (event)
          {
            case IFACE_FLOW_ENABLED_EV:
              if(qosi_flow_info[itr].last_rep == QMI_FLOW_DISABLED)
              {
                ps_flow_type        * check_ps_flow_ptr;
                /* if fwd_on_default and flow is not activated, we need to
                   check that flow is enabled on the default flow ptr */
                if((qosi_flow_info[itr].fwd_on_default == TRUE) &&
                 (PS_FLOW_GET_STATE(qosi_flow_info[itr].flow_ptr) != FLOW_ACTIVATED))
                {
                  check_ps_flow_ptr = def_flow_ptr;
                }
                else
                {
                  check_ps_flow_ptr = ps_flow_ptr;
                }
                if (PS_IFACE_COMMON_IS_FLOW_ENABLED(iface_ptr, check_ps_flow_ptr))
                {
                  send_info = TRUE;
                  qosi_flow_info[itr].last_rep = QMI_FLOW_ENABLED;
                }
              }
              break;
            case IFACE_FLOW_DISABLED_EV:
              if(qosi_flow_info[itr].last_rep == QMI_FLOW_ENABLED) 
              {
                send_info = TRUE;
                qosi_flow_info[itr].last_rep = QMI_FLOW_DISABLED;
              }
              break;
            default:
              ASSERT(0);
              break;
          } /* switch */
        } 
      } /* for itr < QOSI_MAX_QOS_REQUESTS */
    } /* UE initiated flow */

    if (send_info)
    {
      PS_SYSTEM_HEAP_MEM_ALLOC(qmi_flow_info_ptr_arr[num_qos_ids], 
                               sizeof(qmi_flow_info_type),
                               qmi_flow_info_type*);
      if (NULL == qmi_flow_info_ptr_arr[num_qos_ids])
      {
        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        goto bail;
      }
      memset(qmi_flow_info_ptr_arr[num_qos_ids], 0, sizeof(qmi_flow_info_type));
      qmi_flow_info_ptr_arr[num_qos_ids]->flow_ptr = ps_flow_ptr;
      qmi_flow_info_ptr_arr[num_qos_ids]->qos_handle = (uint32) ps_flow_ptr;
      num_qos_ids++;
    }
    ps_flow_handle = new_ps_flow_handle;
    send_info = FALSE;
  } /* while (ps_flow_handle != NULL) */
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /* indicate flow control change for default flow */
  qmi_flow_info_ptr_arr[num_qos_ids++] = NULL;

  switch (event)
  {
    case IFACE_FLOW_ENABLED_EV:
      /* send event rpt ind including all affected sec flows + default */
      qosi_generate_event_report_ind(qos_sp,
                                     qmi_flow_info_ptr_arr,
                                     num_qos_ids,
                                     QOSI_REPORT_STATUS_FLOW_ENABLED,
                                     FALSE);
      break;
     case IFACE_FLOW_DISABLED_EV:
       /* send event rpt ind including all affected sec flows + default */
       qosi_generate_event_report_ind(qos_sp,
                                      qmi_flow_info_ptr_arr,
                                      num_qos_ids,
                                      QOSI_REPORT_STATUS_FLOW_DISABLED,
                                      FALSE);
       break;
     default:
       ASSERT(0);
       break;
  }
bail:
  for (itr=0; itr<num_qos_ids; itr++)
  {
    if (qmi_flow_info_ptr_arr[itr])
    {
      PS_SYSTEM_HEAP_MEM_FREE(qmi_flow_info_ptr_arr[itr]);
    }
  }
} /* qosi_process_iface_flow_event() */


/*===========================================================================
  FUNCTION QMI_QOSI_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client

  PARAMETERS
    cl_sp :  client state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_reset_client
(
  void * cl_sp_in
)
{
  int i, inst;
  qmi_transaction_type *  x_p;
  qmi_qosi_state_type *   qos_sp;
  qmi_qosi_client_state_type * loop_cl_sp;
  qmi_flow_info_type * qmi_flow_info_ptr_arr[QOSI_MAX_QOS_REQUESTS];
  uint16               qos_id_cnt;
  int16                  ps_errno;
  qmi_qosi_client_state_type * cl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cl_sp_in);

  cl_sp = (qmi_qosi_client_state_type *)cl_sp_in;

  qos_id_cnt = 0;
  qos_sp = qmi_qosi_get_qos_sp_by_cl_sp(cl_sp);
  inst = qosi_get_instance_from_qos_sp(qos_sp);

  ASSERT((inst < QOS_INSTANCE_MAX) && qos_sp);

/*-------------------------------------------------------------------------
    q_init() is used here as a way to clear the queue and not as a way to
    initialize the queue.
  -------------------------------------------------------------------------*/
  (void) q_init( &(cl_sp->common.x_free_q) );
  for( i = 0; i < MAX_PENDING_TRANSACTIONS; i++ )
  {
    x_p = &(cl_sp->common.transactions[i]);
    ds_qmi_fw_free_transaction(&x_p );
  }

  /*-------------------------------------------------------------------------
    release all QoS flows for the client
  -------------------------------------------------------------------------*/
  /* go through all qmi qos flows and clear entries for this client */

  for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
  {
    if(qosi_flow_info[i].cl_sp == cl_sp)
    {
      qmi_flow_info_ptr_arr[qos_id_cnt++] = &(qosi_flow_info[i]);
    }
  }

  if(0 < qos_id_cnt)
  {
    loop_cl_sp = qos_sp->client_list;
    while(loop_cl_sp != NULL)
    {
      if ((qos_sp == qmi_qosi_get_qos_sp_by_cl_sp(loop_cl_sp)) &&
          (loop_cl_sp->report_status.report_global_flow == TRUE))
        {
          /* send delete flow event report ind */
          (void) qmi_qosi_event_report_ind(qos_sp,
                                           loop_cl_sp,
                                           qmi_flow_info_ptr_arr,
                                           qos_id_cnt,
                                           QOSI_REPORT_STATUS_FLOW_DELETED,
                                           FALSE);
        }
      if ((qos_sp == qmi_qosi_get_qos_sp_by_cl_sp(loop_cl_sp)) &&
          (loop_cl_sp->report_status.report_global_flow_ex == TRUE))
        {
          /* send delete flow event report ind */
          (void) qmi_qosi_event_report_ind_ex(qos_sp,
                                           loop_cl_sp,
                                           qmi_flow_info_ptr_arr,
                                           qos_id_cnt,
                                           QOSI_REPORT_STATUS_FLOW_DELETED,
                                           FALSE);
        }
      loop_cl_sp = loop_cl_sp->next;
    }
  }
  /* release any qos flows */
  for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
  {
    if(qosi_flow_info[i].cl_sp == cl_sp)
    {
      (void) ps_flow_ioctl(qosi_flow_info[i].flow_ptr,
                           PS_FLOW_IOCTL_QOS_RELEASE,
                           NULL,
                           &ps_errno);
      memset(&(qosi_flow_info[i]), 0, sizeof(qosi_flow_info[i]));

      /*---------------------------------------------------------------------
        Remove ALL the current client's qos reqs from the total reqs as this
        client is going away
      ---------------------------------------------------------------------*/
      qos_sp->num_qos_reqs--;
    }
  }

  cl_sp->report_status.report_global_flow = 0;
  cl_sp->report_status.report_global_flow_ex = 0;
  memset( &cl_sp->report_status.nw_supp_qos_prof,
          0,
          sizeof(cl_sp->report_status.nw_supp_qos_prof));

  cl_sp->bound_ip_type = QMI_AF_ANY;
  qmi_qosi_sync_clients(inst);
} /* qmi_qosi_reset_client() */

/*===========================================================================
  FUNCTION QMI_QOSI_GET_QMI_QOS_SP_BY_CL_SP()

  DESCRIPTION
    to get the QoS service state ptr from the client state ptr

  PARAMETERS
    cl_sp : client state ptr

  RETURN VALUE
    QoS service state ptr

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_qosi_state_type *  qmi_qosi_get_qos_sp_by_cl_sp
(
  qmi_qosi_client_state_type *  cl_sp
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (cl_sp == NULL) ||
       (cl_sp->current_qos_state >= QOSI_MAX_STATE) ||
       (cl_sp->qos_inst >= QOS_INSTANCE_MAX) )
  {
    LOG_MSG_ERROR_1("Invalid cl_sp %p", cl_sp);
    return NULL;
  }

  return qmi_qos_state[cl_sp->current_qos_state][cl_sp->qos_inst];

} /* qmi_qosi_get_qos_sp_by_cl_sp() */

/*===========================================================================
  FUNCTION QMI_QOSI_GET_CL_SP_BY_CL_ID()

  DESCRIPTION
    to get the QOS service client state ptr from the client id

  PARAMETERS
    qos_sp : qos state ptr (either v4 or v6 is ok)
    clid:    client id

  RETURN VALUE
    client state ptr

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_qosi_client_state_type * qmi_qosi_get_cl_sp_by_clid
(
  qmi_qosi_state_type * qos_sp,
  uint8                 clid
)
{
  qmi_qosi_client_state_type * cl_sp;
  int                          i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qos_sp == NULL)
  {
    // Search all QOS states
    for (i = 0; i < QOS_INSTANCE_MAX; i++)
  {
      qos_sp = qmi_qos_state[QOSI_IPV4_STATE][i];
      if (qos_sp == NULL)
    {
        continue;
      }

      cl_sp = qos_sp->client_list;
      while (cl_sp != NULL)
      {
        if (cl_sp->common.clid == clid)
        {
          return cl_sp;
        }
        cl_sp = cl_sp->next;
      }
    }
  }
  else
  {
    cl_sp = qos_sp->client_list;
    while (cl_sp != NULL)
    {
      if (cl_sp->common.clid == clid)
    {
        return cl_sp;
      }
      cl_sp = cl_sp->next;
    }
  }

  return NULL;

} /* qmi_qosi_get_cl_sp_by_clid() */


/*===========================================================================
  FUNCTION QOSI_GET_INSTANCE_FROM_QOS_SP()

  DESCRIPTION
    To obtain QMI QOS instance from qos_sp

  PARAMETERS
    qos_sp : service state ptr

  RETURN VALUE
    QOS Instance if qos_sp is valid
    QOS_INSTANCE_MAX if qos_sp is invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8  qosi_get_instance_from_qos_sp
(
  void *  sp
)
{
  qmi_qosi_state_type * qos_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qos_sp = (qmi_qosi_state_type*)sp;

  if ( (qos_sp == NULL) ||
       (qos_sp->constants.qos_inst >= QOS_INSTANCE_MAX) )
  {
    LOG_MSG_ERROR_1 ("Invalid qos_sp! (0x%x)", qos_sp);
    return QOS_INSTANCE_MAX;
  }

  return qos_sp->constants.qos_inst;
} /* qosi_get_instance_from_qos_sp() */

/*===========================================================================
  FUNCTION QOSI_GET_INST_FROM_EP_AND_MUX_ID()

  DESCRIPTION
    To obtain QMI QOS instance from ep_id and mux_id

  PARAMETERS

  RETURN VALUE
    QOS instance if qos_sp is valid
    QOS_INSTANCE_MAX if qos_sp is invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 qosi_get_inst_from_ep_and_mux_id
(
  uint32    ep_id,
  uint8     mux_id
)
{
  qmi_qosi_state_type  * qos_sp_v4;
  int                    i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0; i < QOS_INSTANCE_MAX; i++)
  {
    qos_sp_v4 = qmi_qos_state[QOSI_IPV4_STATE][i];
    if ( qos_sp_v4 != NULL &&
         qos_sp_v4->binding.ep_id == ep_id &&
         qos_sp_v4->binding.mux_id == mux_id )
    {
      return i;
    }
  }

  return QOS_INSTANCE_MAX;

} /* qosi_get_inst_from_ep_and_mux_id() */


/*===========================================================================
  FUNCTION QOSI_POPULATE_EXT_ERROR_TLV()

  DESCRIPTION
    Populate the Extended Error Info TLV with error from lower layers

  PARAMETERS
    errval   - QMI Error Value (output)
    response - Response Pointer (output)
    errno    - Extended Error value from lower layers to populate in response

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_populate_ext_error_tlv
(
  qmi_error_e_type *  errval,
  dsm_item_type    ** response,
  int16              err_no
)
{
  *errval = QMI_ERR_EXTENDED_INTERNAL;
  if(FALSE == qmi_svc_put_param_tlv(response,
                                    QMI_TYPE_EXT_ERROR_INFO,
                                    sizeof(err_no),
                                    &err_no))
  {
    *errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(response);
  }
} /* qosi_populate_ext_error_tlv() */

/*===========================================================================
  FUNCTION QOSI_SEND_NW_INIT_EVENT_IND

  DESCRIPTION
    Query the iface for NW initiated flow and send to the host side

  PARAMETERS
    qos_sp   - qos state pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_send_nw_init_event_ind
(
  qmi_qosi_state_type * qos_sp
)
{
  ps_flow_type     * ps_flow_ptr;
  void             * ps_flow_handle;
  void             * new_ps_flow_handle;
  boolean            result;
  qmi_flow_info_type *  qmi_flow_info_ptr_arr[QOSI_MAX_NETWORK_INIT_FLOW_NUM];
  uint16             num_qos_ids = 0;
  qmi_flow_info_type qos_flow_info_array[QOSI_MAX_NETWORK_INIT_FLOW_NUM];
  int                nw_init_idx = 0;

  memset(qos_flow_info_array, 0, sizeof(qmi_flow_info_type) * QOSI_MAX_NETWORK_INIT_FLOW_NUM);
  /* get all flows from the existing iface */
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  ps_flow_handle = ps_iface_get_sec_flow_handle(qos_sp->ps_iface_ptr);

  while (num_qos_ids < QOSI_MAX_NETWORK_INIT_FLOW_NUM && ps_flow_handle != NULL)
  {
    result = ps_iface_get_sec_flow_by_handle(qos_sp->ps_iface_ptr,
                                             ps_flow_handle,
                                             &ps_flow_ptr,
                                             &new_ps_flow_handle);
    if (!result)
    {
      LOG_MSG_ERROR_0("Returning incomplete list of flows");
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return;
    }

    /* If the flow is activated, fill up the flow pointer */
    if (PS_FLOW_GET_STATE(ps_flow_ptr) == FLOW_ACTIVATED)
    {

      LOG_MSG_INFO1_2("Found flow 0x%x on QMI iface 0x%x",
                      ps_flow_ptr, qos_sp->ps_iface_ptr);
      /* find it in the nw init qos flow info table and add there if not present */
      for (nw_init_idx = 0; nw_init_idx < QOSI_MAX_NETWORK_INIT_FLOW_NUM; nw_init_idx++)
      {
        if (nw_init_qosi_flow_info[nw_init_idx].flow_ptr == ps_flow_ptr)
        {
          break;
        }
      }  
      if (nw_init_idx >= QOSI_MAX_NETWORK_INIT_FLOW_NUM)
      {
        /* find empty slot to store nw inited flow info */
        for (nw_init_idx = 0; nw_init_idx < QOSI_MAX_NETWORK_INIT_FLOW_NUM; nw_init_idx++)
        {
          if (nw_init_qosi_flow_info[nw_init_idx].qos_handle == 0)
          {
            break;
          }
        }

        if (nw_init_idx >= QOSI_MAX_NETWORK_INIT_FLOW_NUM)
        {
          LOG_MSG_ERROR_0("QOS nw inited handle slots exhausted");
          PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          return;
        }
        else
        {
          nw_init_qosi_flow_info[nw_init_idx].qos_handle = (uint32) ps_handle_mgr_get_handle(PS_HANDLE_MGR_CLIENT_QOS,
                                               (uint16)(nw_init_idx+ QOSI_MAX_QOS_REQUESTS + 1));
          if (PS_HANDLE_MGR_INVALID_HANDLE == nw_init_qosi_flow_info[nw_init_idx].qos_handle)
          {
            LOG_MSG_ERROR_0("Invalid handle returned by handle mgr");
            nw_init_qosi_flow_info[nw_init_idx].qos_handle = 0;
            PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
            return;
          }
          else
          {
            LOG_MSG_INFO2_2("Mapping flow_ptr 0x%x to handle 0x%x", 
                            ps_flow_ptr, nw_init_qosi_flow_info[nw_init_idx].qos_handle);

            nw_init_qosi_flow_info[nw_init_idx].flow_ptr   = ps_flow_ptr;
            nw_init_qosi_flow_info[nw_init_idx].uplink_fc_seq_num = 0;
          }
        }
      }

      qos_flow_info_array[num_qos_ids].flow_ptr = ps_flow_ptr;
      qos_flow_info_array[num_qos_ids].qos_handle = nw_init_qosi_flow_info[nw_init_idx].qos_handle;
      /* add the flow info pointer to the list */
      qmi_flow_info_ptr_arr[num_qos_ids] = &qos_flow_info_array[num_qos_ids];

      num_qos_ids++;
    }

    ps_flow_handle = new_ps_flow_handle;
  } /* while (ps_flow_handle != NULL) */
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  // call event report indication
  if (num_qos_ids > 0)
  {
    qosi_generate_event_report_ind(qos_sp,
                                 qmi_flow_info_ptr_arr,
                                 num_qos_ids,
                                 QOSI_REPORT_STATUS_FLOW_ACTIVATED,
                                 TRUE);
    LOG_MSG_INFO1_1("QMI generating event report for %d network initiated flows",
                    num_qos_ids);
  }
}

/*===========================================================================
  FUNCTION QOSI_convert_filter_spec_ntoh

  DESCRIPTION
    Convert the filter spec to host order so the TLVs can be correctly parsed
    and sent to clients

  PARAMETERS
    filter_spec_ptr   - filter spec pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qosi_convert_filter_spec_ntoh
(
  ip_filter_spec_type *filter_spec_ptr
)
{
  uint8 i;

  for (i=0; i<filter_spec_ptr->num_filters; i++)
  {
    if ( IP_V4 == filter_spec_ptr->list_ptr[i].ip_vsn )
    {
      if(filter_spec_ptr->list_ptr[i].ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR)
      {
        filter_spec_ptr->list_ptr[i].ip_hdr.v4.src.addr.ps_s_addr =
            ps_ntohl(filter_spec_ptr->list_ptr[i].ip_hdr.v4.src.addr.ps_s_addr);
        filter_spec_ptr->list_ptr[i].ip_hdr.v4.src.subnet_mask.ps_s_addr =
            ps_ntohl(filter_spec_ptr->list_ptr[i].ip_hdr.v4.src.subnet_mask.ps_s_addr);
      }
      if(filter_spec_ptr->list_ptr[i].ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR)
      {
        filter_spec_ptr->list_ptr[i].ip_hdr.v4.dst.addr.ps_s_addr =
            ps_ntohl(filter_spec_ptr->list_ptr[i].ip_hdr.v4.dst.addr.ps_s_addr);
        filter_spec_ptr->list_ptr[i].ip_hdr.v4.dst.subnet_mask.ps_s_addr =
            ps_ntohl(filter_spec_ptr->list_ptr[i].ip_hdr.v4.dst.subnet_mask.ps_s_addr);
      }
    }

    if ( ( (filter_spec_ptr->list_ptr[i].ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT) &&
           (filter_spec_ptr->list_ptr[i].ip_hdr.v4.next_hdr_prot == 0x11) ) ||
         ( (filter_spec_ptr->list_ptr[i].ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT) &&
           (filter_spec_ptr->list_ptr[i].ip_hdr.v6.next_hdr_prot == 0x11) ) )
    {
      if(filter_spec_ptr->list_ptr[i].next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT)
      {
        filter_spec_ptr->list_ptr[i].next_prot_hdr.udp.src.port =
            ps_ntohs(filter_spec_ptr->list_ptr[i].next_prot_hdr.udp.src.port);
      }
      if(filter_spec_ptr->list_ptr[i].next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT)
      {
        filter_spec_ptr->list_ptr[i].next_prot_hdr.udp.dst.port =
            ps_ntohs(filter_spec_ptr->list_ptr[i].next_prot_hdr.udp.dst.port);
      }
    }

    if ( ( (filter_spec_ptr->list_ptr[i].ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT) &&
           (filter_spec_ptr->list_ptr[i].ip_hdr.v4.next_hdr_prot == 0x06) ) ||
         ( (filter_spec_ptr->list_ptr[i].ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT) &&
           (filter_spec_ptr->list_ptr[i].ip_hdr.v6.next_hdr_prot == 0x06) ) )
    {
      if(filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
      {
        filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp.src.port =
            ps_ntohs(filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp.src.port);
      }
      if(filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
      {
        filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp.dst.port =
            ps_ntohs(filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp.dst.port);
      }
    }
   if ( ( (filter_spec_ptr->list_ptr[i].ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT) &&
           (filter_spec_ptr->list_ptr[i].ip_hdr.v4.next_hdr_prot == PS_IPPROTO_TCP_UDP) ) ||
         ( (filter_spec_ptr->list_ptr[i].ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT) &&
           (filter_spec_ptr->list_ptr[i].ip_hdr.v6.next_hdr_prot == PS_IPPROTO_TCP_UDP) ) )
    {
      if(filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp_udp_port_range.field_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT)
      {
        filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp_udp_port_range.src.port =
            ps_ntohs(filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp_udp_port_range.src.port);
      }
      if(filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp_udp_port_range.field_mask & IPFLTR_MASK_TCP_DST_PORT)
      {
        filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp_udp_port_range.dst.port =
            ps_ntohs(filter_spec_ptr->list_ptr[i].next_prot_hdr.tcp_udp_port_range.dst.port);
      }
    }
  }
}

/*===========================================================================
  FUNCTION QMI_QOSI_UPDATE_CLIENT()

  DESCRIPTION
    Updates svc_sp on all transactions for a client 

  PARAMETERS
    cl_sp - client 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_qosi_update_client
(
  void *  cl_sp
)
{
  qmi_transaction_type *        x_p;
  qmi_qosi_state_type *         qos_sp;
  int i;

  ASSERT(cl_sp);

  x_p = &((qmi_qosi_client_state_type *)cl_sp)->common.transactions[0];
  qos_sp = qmi_qosi_get_qos_sp_by_cl_sp(cl_sp);
  for (i=MAX_PENDING_TRANSACTIONS; i; i--, x_p++ )
  {
    x_p->svc_sp = (void *)qos_sp;
  }

} /* qmi_qosi_update_client */

/*===========================================================================
  FUNCTION QMI_QOSI_MOVE_CLIENT_TO_EXT_STATE()

  DESCRIPTION
    Move client from one state to another.

  PARAMETERS
    cl_sp

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_qosi_move_client_to_ext_state
(
  qmi_qosi_client_state_type *  cl_sp
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cl_sp != NULL);


  if(QOSI_IPV4_STATE == cl_sp->current_qos_state)
    cl_sp->current_qos_state = QOSI_IPV6_STATE;
  else
    cl_sp->current_qos_state = QOSI_IPV4_STATE;

  qmi_qosi_update_client(cl_sp);
} /* qmi_qosi_move_client_to_ext_state() */

/*===========================================================================
  FUNCTION QMI_QOSI_SYNC_CLIENTS()

  DESCRIPTION
     Synchronize clients' QOS state

  PARAMETERS
    inst:  QMI instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_qosi_sync_clients(uint8 inst)
{
  qmi_qosi_state_type        * qos_sp;
  qmi_qosi_state_type *current;
  qmi_qosi_client_state_type * cl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(inst < QOS_INSTANCE_MAX);

  /* check that qmi_inst is within range in case ASSERT is removed */
  if (inst >= QOS_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QOS instance out of range (%d)!", inst);
    return;
  }
  
  qos_sp = qmi_qos_state[QOSI_IPV4_STATE][inst];
  if (qos_sp == NULL)
  {
    LOG_MSG_ERROR("qos_sp for inst %d is NULL", inst, 0, 0);
    return;
  }

  cl_sp = qos_sp->client_list;
  while (cl_sp != NULL)
  {
    current = qmi_qos_state[cl_sp->current_qos_state][inst];

    //if legacy
    if (QMI_AF_ANY == cl_sp->bound_ip_type)
    {
      if(!current->is_primary_state)
      {
        qmi_qosi_move_client_to_ext_state(cl_sp);
      }
      }
    else
    {
      if (cl_sp->bound_ip_type != current->cur_ip_call_type)
      {
          qmi_qosi_move_client_to_ext_state(cl_sp);
      }
    }
    cl_sp = cl_sp->next;
  }
} /* qmi_qosi_sync_clients() */

/*===========================================================================
  FUNCTION QMI_QOSI_SET_CLIENT_IP_PREF()

  DESCRIPTION
    Get the packet statistics

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_qosi_set_client_ip_pref
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_client_state_type *  qos_cl_sp;
  dsm_item_type *        response;
  uint8                  type;
  uint16                 len;
  uint16                 expected_len;
  void *                 value;
  uint8                  inst;

  PACKED struct PACKED_POST
  {
    uint8  ip_pref;
  } v_in_required;
  boolean                   got_v_in_required;
  qmi_error_e_type          errval;
  qmi_result_e_type         result;
  boolean                   retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  memset(&v_in_required, 0, sizeof(v_in_required));

  response          = NULL;
  errval            = QMI_ERR_NONE;
  value             = NULL;
  got_v_in_required = FALSE;
  qos_cl_sp = (qmi_qosi_client_state_type *)cl_sp;
  inst = qosi_get_instance_from_qos_sp(sp);

  ASSERT(inst < QOS_INSTANCE_MAX);

  /* check that qos_inst is within range in case ASSERT is removed */
  if (inst >= QOS_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QOS instance out of range (%d)!", inst);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(v_in_required);
        value = &v_in_required;
        got_v_in_required = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      /*---------------------------------------------------------------------
        Mandatory TLV is invalid so in result send error
      ---------------------------------------------------------------------*/
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if(!got_v_in_required)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  LOG_MSG_INFO2_1("Got IP pref setting: %d", v_in_required.ip_pref);

  if( v_in_required.ip_pref != QMI_AF_ANY &&
      v_in_required.ip_pref != QMI_AF_INET &&
      v_in_required.ip_pref != QMI_AF_INET6 )
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  qos_cl_sp->bound_ip_type = (qmi_ip_family_e_type) v_in_required.ip_pref;
  qmi_qosi_sync_clients(inst);

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;

} /* qmi_qosi_set_client_ip_pref() */

/*===========================================================================
FUNCTION QMI_QOSI_BIND_DATA_PORT()

  DESCRIPTION
    Bind the QOS client to a data port

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp_in  : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_qosi_bind_data_port
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp_in,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type              * response;
  qmi_qosi_state_type        * qos_sp;
  qmi_qosi_client_state_type * cl_sp;
  uint32                       ep_id;
  uint8                        mux_id;
  uint8                        bind_inst;
  qmi_ep_id_type               qmi_ep_id = {0};
  boolean                      got_ep_id_tlv = FALSE;

  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;

  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp_in && sdu_in);

  qos_sp = (qmi_qosi_state_type *) sp;
  cl_sp = (qmi_qosi_client_state_type *)cl_sp_in;

  response = NULL;
  errval = QMI_ERR_NONE;

  ep_id       = qos_sp->binding.ep_id;
  mux_id      = 0;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QOSI_PRM_TYPE_EP_ID:
        expected_len = sizeof(qmi_ep_id);
        value = (void *) &qmi_ep_id;
        got_ep_id_tlv = TRUE;
        break;

      case QOSI_PRM_TYPE_MUX_ID:
        expected_len = sizeof(mux_id);
        value = (void *) &mux_id;
        break;

      default:
        LOG_MSG_INFO2 ("Unrecognized TLV type (%d)",type,0,0);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_2 ("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if (len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if (got_ep_id_tlv)
  {
    // Converting to uint32
    if (QMI_EP_ID_VALID(qmi_ep_id))
    {
      ep_id = QMI_EP_ID_TO_INTERNAL_EP_ID(qmi_ep_id);
    }
    else
    {
      LOG_MSG_ERROR_2("Invalid ep_id 0x%x:0x%x",
                 qmi_ep_id.ep_type, qmi_ep_id.iface_id);
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  if ((ep_id == 0) || (NULL == rmnet_phys_xport_from_ep_id(ep_id)))
  {
    LOG_MSG_INFO1_1("Invalid EP id 0x%x", ep_id);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  // Find the binding inst from ep and mux id
  bind_inst = qosi_get_inst_from_ep_and_mux_id(ep_id, mux_id);
  if (bind_inst >= QOS_INSTANCE_MAX)
  {
    // Check if we need to put the client into non-mux section
    bind_inst = QMI_INSTANCE_MAX;
    if (mux_id == 0)
    {
      bind_inst = qmux_get_inst_from_ep_id(ep_id);
    }

    // Find a free qos state in the mux section
    if (bind_inst >= QMI_INSTANCE_MAX)
    {
      for (bind_inst = QMI_INSTANCE_MAX; bind_inst < QOS_INSTANCE_MAX; bind_inst++)
      {
        if (qmi_qos_state[QOSI_IPV4_STATE][bind_inst] == NULL ||
            qmi_qos_state[QOSI_IPV4_STATE][bind_inst]->binding.ep_id == 0)
        {
          break;
        }
      }
    }

    if (bind_inst >= QOS_INSTANCE_MAX)
    {
      LOG_MSG_ERROR_2("Unable to bind to ep 0x%x mux_id %d", ep_id, mux_id);
      errval = QMI_ERR_INTERNAL;
      goto send_result;
    }
  }

  // If bind to the current binding inst, no action is needed
  if (cl_sp->qos_inst == bind_inst)
  {
    LOG_MSG_INFO1_2("Client %d already bound to inst %d", 
                       cl_sp->common.clid, cl_sp->qos_inst);
    goto send_result;
  }

  // Bind the QOS instance
  if (!qmi_qosi_qos_inst_bind(bind_inst, ep_id, mux_id))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  // Bind the client
  if (!qmi_qosi_process_bind_data_port(cl_sp, bind_inst))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;

} /* qmi_qosi_bind_data_port() */

/*===========================================================================
  FUNCTION QMI_QOS_GET_FILTER_PARAMS()

  DESCRIPTION
    Get the filter parameters for a particular QoS identifier

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_get_filter_params
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_state_type        *  qos_sp;
  qmi_qosi_client_state_type *  qos_client_sp;
  dsm_item_type *               response;
  PACKED struct PACKED_POST
  {
    uint32 qos_handle;
  } v_in_reqd;
  uint16             len = 0;
  uint16             expected_len;
  uint8              type;
  uint8              i;
  void *             value;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  boolean            got_v_in_reqd;
  uint16             num_bytes_pushed;
  ps_flow_type *     flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  response = NULL;
  got_v_in_reqd = FALSE;
  errval   = QMI_ERR_NONE;
  memset(&v_in_reqd, 0, sizeof(v_in_reqd));
  qos_sp = (qmi_qosi_state_type *) sp;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(v_in_reqd);
        value = (void *) &v_in_reqd;
        got_v_in_reqd = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if(!got_v_in_reqd)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /* Cannot query filter params for default flow */
  if (v_in_reqd.qos_handle == 0)
  {
    errval = QMI_ERR_INVALID_QOS_ID;
    goto send_result;
  }

  if (qos_sp->ps_iface_ptr == NULL)
  {
    errval = QMI_ERR_OUT_OF_CALL;
    goto send_result;
  }

  if(!qmi_qosi_verify_client_subs(qos_client_sp, qos_sp->binding.subs_id, TRUE))
  {
    errval = QMI_ERR_INCOMPATIBLE_STATE;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Check if the given qos_handle belongs to the client
  -------------------------------------------------------------------------*/
  for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
  {
    if( (qosi_flow_info[i].qos_handle == v_in_reqd.qos_handle) &&
        (qosi_flow_info[i].cl_sp == qos_client_sp) )
    {
      break;
    }
  }

  if(i == QOSI_MAX_QOS_REQUESTS)
  {
    /* Check if the flow is network initiated if
       not found in UE init cache */
    for(i=0; i < QOSI_MAX_NETWORK_INIT_FLOW_NUM; i++)
    {
      if(nw_init_qosi_flow_info[i].qos_handle == v_in_reqd.qos_handle) 
      {
        break;
      }
    }
    if ((i == QOSI_MAX_NETWORK_INIT_FLOW_NUM) || 
        (PS_FLOW_GET_IFACE(nw_init_qosi_flow_info[i].flow_ptr) != qos_sp->ps_iface_ptr))
    {
      errval = QMI_ERR_INVALID_QOS_ID;
      goto send_result;
    }
    flow_ptr = nw_init_qosi_flow_info[i].flow_ptr;
  }
  else
  {
    /* UE initiated flows */
    flow_ptr = qosi_flow_info[i].flow_ptr;
  }

  /*-------------------------------------------------------------------------
    Populate the response with the Filter Params
  -------------------------------------------------------------------------*/
  errval = qosi_append_filter_spec(&response,
                                   qos_sp->ps_iface_ptr,
                                   flow_ptr,
                                   &num_bytes_pushed,
                                   QMI_PRM_TYPE_TX_QOS_FILTER_PARAMS,
                                   QMI_PRM_TYPE_RX_QOS_FILTER_PARAMS);


send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv( &response, result, errval );
  CHECK_RETVAL();
  return response;
} /* qmi_qosi_get_filter_params */

/*===========================================================================
  FUNCTION QMI_QOSI_SWAP_INSTANCES()

  DESCRIPTION


  PARAMETERS

  RETURN VALUE

  DEPENDENCIES

  SIDE EFFECTS
===========================================================================*/
void qmi_qos_swap_instances(uint32  ep_id, uint8   mux_id)
{
  uint8 temp, bind_inst;

  // Find the binding inst from ep and mux id
  bind_inst = qosi_get_inst_from_ep_and_mux_id(ep_id, mux_id);
  if (bind_inst < QOS_INSTANCE_MAX)
  {
    if ((NULL == qmi_qos_state[QOSI_IPV4_STATE][bind_inst]) ||
        (NULL == qmi_qos_state[QOSI_IPV6_STATE][bind_inst]))
  {
      LOG_MSG_ERROR_1("QOS sp not initialized (%d)!", bind_inst);
    return;
  }
    temp = qmi_qos_state[QOSI_IPV4_STATE][bind_inst]->is_primary_state;
    qmi_qos_state[QOSI_IPV4_STATE][bind_inst]->is_primary_state = qmi_qos_state[QOSI_IPV6_STATE][bind_inst]->is_primary_state;
    qmi_qos_state[QOSI_IPV6_STATE][bind_inst]->is_primary_state = temp;
    qmi_qosi_sync_clients(bind_inst);
  }
}

/*===========================================================================
  FUNCTION QMI_QOSI_GET_CMD_BUF()

  DESCRIPTION
    Allocate and assign a QMI QOS command buffer from the PS MEM heap
    based on the QMI CMD type

  PARAMETERS
    cmd - QMI command type

  RETURN VALUE
    cmd_buf_ptr - Pointer to the allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void *qmi_qosi_get_cmd_buf
(
  qmi_cmd_id_e_type cmd_id
)
{
      qmi_qosi_cmd_buf_type *cmd_buf_ptr = NULL;
      PS_SYSTEM_HEAP_MEM_ALLOC(cmd_buf_ptr, sizeof(qmi_qosi_cmd_buf_type),
                               qmi_qosi_cmd_buf_type*);
      return cmd_buf_ptr;
} /* qmi_qosi_get_cmd_buf */

/*===========================================================================
  FUNCTION QMI_QOS_GET_FC_STATS()

  DESCRIPTION
    This function retrieves QoS flow control stats

  PARAMETERS
    num_flow_enable: address of flow enable cnt
    num_flow_disable: address of flow disable cnt

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_qos_get_fc_stats
(
  uint32  * num_flow_enable,
  uint32  * num_flow_disable
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (num_flow_enable == NULL || num_flow_disable == NULL)
  {
    return;
  }

  *num_flow_enable = qmi_qosi_global.num_flow_enable;
  *num_flow_disable = qmi_qosi_global.num_flow_disable;

} /* qmi_qos_get_fc_stats() */

/*===========================================================================
  FUNCTION QMI_QOSI_BIND_SUBSCRIPTION()

  DESCRIPTION
    Bind the QOS client to a data subscription.
 
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp_in  : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_qosi_bind_subscription
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp_in,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type              * response;
  qmi_qosi_client_state_type * cl_sp;
  uint32                       subscription_id;

  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;

  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  boolean            got_subs_tlv = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp_in && sdu_in);

  cl_sp = (qmi_qosi_client_state_type *)cl_sp_in;

  response = NULL;
  errval = QMI_ERR_NONE;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case WDSI_PRM_TYPE_SUBS_ID_REQ:
        expected_len = sizeof(subscription_id);
        value = (void *) &subscription_id;
        got_subs_tlv = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1 ("Unrecognized TLV type (%d)", type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                       len,
                                       expected_len,
                                       FILE_ID_DS_QMI_WDS,__LINE__)  )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if (len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  } /* while */

  if (!(got_subs_tlv))
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if ((subscription_id >= QMI_QOSI_SUBS_MAX || 
       subscription_id < QMI_QOSI_SUBS_DEFAULT) &&
       subscription_id != QMI_QOSI_SUBS_DONT_CARE)
  {
      LOG_MSG_ERROR_1("Invalid subs id %d",subscription_id);
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
  }

  cl_sp->subscription_id = (qmi_qosi_subs_enum_type)subscription_id;

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;

} /* qmi_qosi_bind_subscription() */

/*===========================================================================
FUNCTION QMI_QOSI_GET_BIND_SUBSCRIPTION()

  DESCRIPTION
    Queries for the data subscription of the QOS client
 
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp_in  : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_qosi_get_bind_subscription
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp_in,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type              * response;
  qmi_qosi_client_state_type * cl_sp;
  uint32                       subscription_id;

  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp_in && sdu_in);

  cl_sp = (qmi_qosi_client_state_type *)cl_sp_in;

  response = NULL;
  errval = QMI_ERR_NONE;

  subscription_id = cl_sp->subscription_id;
  
  if ( FALSE == qmi_svc_put_param_tlv2(&response,
                                       WDSI_PRM_TYPE_SUBS_ID_RESP,
                                       sizeof(subscription_id),
                                       &subscription_id,
                                       FILE_ID_DS_QMI_QOS,__LINE__))
  {      
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
    goto send_result;
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;

} /* qmi_qosi_get_bind_subscription() */
/*===========================================================================
  FUNCTION QMI_QOS_INDICATION_REGISTER()

  DESCRIPTION
    Allows control point to register for indications

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static dsm_item_type*  qmi_qosi_indication_register
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_client_state_type *     qos_client_sp;
  qmi_qosi_state_type *            qos_sp;
  dsm_item_type *                  response;
  qos_indication_register_req_msg_v01  req_msg;
  qos_indication_register_resp_msg_v01 resp_msg;
  qmi_error_type_v01     errval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  qos_sp = (qmi_qosi_state_type *) sp;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;

  memset(&req_msg, 0, sizeof(qos_indication_register_req_msg_v01) );
  memset(&resp_msg, 0, sizeof(qos_indication_register_resp_msg_v01));

  errval = QMI_QOSI_DECODE_REQUEST_MSG(QMI_QOS_INDICATION_REGISTER_REQ_V01,
  	                                   sdu_in,
  	                                   &req_msg,
  	                                   sizeof(req_msg));

  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* At least one TLV should be present */
  if (req_msg.report_global_qos_flows_valid == TRUE)
  {
    qos_client_sp->report_status.report_global_flow_ex = req_msg.report_global_qos_flows ? TRUE : FALSE;
  }
  else
  {
    errval = QMI_ERR_MISSING_ARG_V01;
  }

send_result:
  QMI_QOSI_ENCODE_RESPONSE_MSG(QMI_QOS_INDICATION_REGISTER_RESP_V01,
                               &resp_msg,
                               sizeof(qos_indication_register_resp_msg_v01),
                               errval,
                               &response);
  return response;
} /*qmi_qosi_indication_register */

/*===========================================================================
  FUNCTION QMI_QOS_REQUEST_QOS_EX()

  DESCRIPTION
    Request a new QoS Flow

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_request_qos_ex
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_client_state_type *     qos_client_sp;
  qmi_qosi_state_type *            qos_sp;
  dsm_item_type *                  response;
  ps_iface_type *                  ps_iface_ptr;
  ps_iface_ioctl_qos_request_type  qos_request;
  qos_req_mod_info_ex_type *       req_ptr = NULL;
  int16                  ps_errno;
  int                    j, i;
  qmi_error_type_v01     errval;
  int                    return_val;
  qmi_ip_family_e_type   ip_type;
  qos_request_qos_ex_req_msg_v01  *req_msg = NULL;
  qos_request_qos_ex_resp_msg_v01 *resp_msg = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  ps_errno = 0;

  qos_sp = (qmi_qosi_state_type *) sp;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;
  ip_type = qos_client_sp->bound_ip_type;

  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(req_ptr, sizeof(qos_req_mod_info_ex_type),
                                  qos_req_mod_info_ex_type*);
  if (req_ptr == NULL)
  {
    return NULL;
  }
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(req_msg, sizeof(qos_request_qos_ex_req_msg_v01),
                                  qos_request_qos_ex_req_msg_v01*);
  if (req_msg == NULL)
  {
    return NULL;
  }
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(resp_msg, sizeof(qos_request_qos_ex_resp_msg_v01),
                                  qos_request_qos_ex_resp_msg_v01*);
  if (resp_msg == NULL)
  {
    return NULL;
  }

  memset(&qos_request, 0, sizeof(qos_request));
  memset(req_ptr, 0, sizeof(qos_req_mod_info_ex_type));
  memset(req_msg, 0, sizeof(qos_request_qos_ex_req_msg_v01) );
  memset(resp_msg, 0, sizeof(qos_request_qos_ex_resp_msg_v01));

  LOG_MSG_INFO2_1("Received QoS Request from client %d",
                  qos_client_sp->common.clid);

  response = NULL;
  errval   = QMI_ERR_NONE_V01;

  errval = QMI_QOSI_DECODE_REQUEST_MSG(QMI_QOS_REQUEST_QOS_EX_REQ_V01,
  	                                   sdu_in,
  	                                   req_msg,
  	                                   sizeof(qos_request_qos_ex_req_msg_v01));

  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  if (req_msg->tx_qos_flow_valid)
  {
    errval = qosi_convert_flow_spec_to_ps(QOSI_MAX_QOS_FLOWS_PER_REQUEST,
                                          req_ptr->tx_flows,
                                          req_msg->tx_qos_flow_len,
                                          req_msg->tx_qos_flow);
    if (errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
    if (req_msg->tx_qos_flow_len > 0)
    {
      /*-------------------------------------------------------------------
        Copy the parsed TX flows specs into the qos_spec struct to be sent
        to the ioctl
        - The 1st flow spec becomes the REQ flow
        - the last flow spec becomes the MIN flow
        - Other flows in between become AUX flows
      -------------------------------------------------------------------*/
      memscpy(&(req_ptr->qos_spec.tx.flow_template.req_flow),
              sizeof(ip_flow_type),
              &(req_ptr->tx_flows[0]),
              sizeof(ip_flow_type));
      req_ptr->qos_spec.field_mask |= QOS_MASK_TX_FLOW;

      if (req_msg->tx_qos_flow_len > 1)
      {
        memscpy(&(req_ptr->qos_spec.tx.flow_template.min_req_flow),
                sizeof(ip_flow_type),
                &(req_ptr->tx_flows[req_msg->tx_qos_flow_len - 1]),
                sizeof(ip_flow_type));
        req_ptr->qos_spec.field_mask |= QOS_MASK_TX_MIN_FLOW;

        if (req_msg->tx_qos_flow_len > 2)
        {
          req_ptr->qos_spec.tx.flow_template.aux_flow_list_ptr =
             &(req_ptr->tx_flows[1]);
          req_ptr->qos_spec.tx.flow_template.num_aux_flows = req_msg->tx_qos_flow_len - 2;
          req_ptr->qos_spec.field_mask |= QOS_MASK_TX_AUXILIARY_FLOWS;
        }
      }
    }
  } /* req_msg->tx_qos_flow_valid */

  if (req_msg->rx_qos_flow_valid)
  {
    errval = qosi_convert_flow_spec_to_ps(QOSI_MAX_QOS_FLOWS_PER_REQUEST,
                                          req_ptr->rx_flows,
                                          req_msg->rx_qos_flow_len,
                                          req_msg->rx_qos_flow);
    if (errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
    if (req_msg->rx_qos_flow_len > 0)
    {
      /*-------------------------------------------------------------------
        Copy the parsed RX flows specs into the qos_spec struct to be sent
        to the ioctl
        - The 1st flow spec becomes the REQ flow
        - the last flow spec becomes the MIN flow
        - Other flows in between become AUX flows
      -------------------------------------------------------------------*/
      memscpy(&(req_ptr->qos_spec.rx.flow_template.req_flow),
              sizeof(ip_flow_type),
              &(req_ptr->rx_flows[0]),
              sizeof(ip_flow_type));
      req_ptr->qos_spec.field_mask |= QOS_MASK_RX_FLOW;

      if (req_msg->rx_qos_flow_len > 1)
      {
        memscpy(&(req_ptr->qos_spec.rx.flow_template.min_req_flow),
                sizeof(ip_flow_type),
                &(req_ptr->rx_flows[req_msg->rx_qos_flow_len - 1]),
                sizeof(ip_flow_type));
        req_ptr->qos_spec.field_mask |= QOS_MASK_RX_MIN_FLOW;

        if (req_msg->rx_qos_flow_len > 2)
        {
          req_ptr->qos_spec.rx.flow_template.aux_flow_list_ptr =
             &(req_ptr->rx_flows[1]);
          req_ptr->qos_spec.rx.flow_template.num_aux_flows = req_msg->rx_qos_flow_len - 2;
          req_ptr->qos_spec.field_mask |= QOS_MASK_RX_AUXILIARY_FLOWS;
        }
      }
    }
  } /* req_msg->rx_qos_flow_valid */

  if (req_msg->tx_qos_filter_valid)
  {
    errval = qosi_convert_filter_spec_to_ps(QOSI_MAX_QOS_FLTRS_PER_REQUEST,
                                            req_ptr->tx_filters,
                                            req_msg->tx_qos_filter_len,
                                            req_msg->tx_qos_filter);
    if (errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
    if(req_msg->tx_qos_filter_len > 0)
    {
      if( IP_V4 == req_ptr->tx_filters[0].ip_vsn )
      {
        ip_type = QMI_AF_INET;
      }else
      {
        ip_type = QMI_AF_INET6;
      }
      /*-----------------------------------------------------------------
                  Populate the QoS spec with TX filter info
        -----------------------------------------------------------------*/
      req_ptr->qos_spec.tx.fltr_template.num_filters = req_msg->tx_qos_filter_len;
      req_ptr->qos_spec.tx.fltr_template.list_ptr = req_ptr->tx_filters;
    }
  }

  if (req_msg->rx_qos_filter_valid)
  {
    errval = qosi_convert_filter_spec_to_ps(QOSI_MAX_QOS_FLTRS_PER_REQUEST,
                                            req_ptr->rx_filters,
                                            req_msg->rx_qos_filter_len,
                                            req_msg->rx_qos_filter);
    if (errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
    if(req_msg->rx_qos_filter_len > 0)
    {
      if( IP_V4 == req_ptr->rx_filters[0].ip_vsn )
      {
        ip_type = QMI_AF_INET;
      }else
      {
        ip_type = QMI_AF_INET6;
      }
      req_ptr->qos_spec.rx.fltr_template.num_filters = req_msg->rx_qos_filter_len;
      req_ptr->qos_spec.rx.fltr_template.list_ptr = req_ptr->rx_filters;
    }
  }

  if((qos_sp->num_qos_reqs + 1) > QOSI_MAX_QOS_REQUESTS)
  {
    errval = QMI_ERR_MAX_QOS_REQUESTS_IN_USE_V01;
    goto send_result;
  }

  if(( QMI_AF_ANY == qos_client_sp->bound_ip_type ) || 
     ( ip_type != qos_client_sp->bound_ip_type ))
  {
    LOG_MSG_ERROR_2("IP type %d in request, client bound to IP type %d",
                    ip_type, qos_client_sp->bound_ip_type);
    errval = QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Check whether the call is up
    iface_ptr is stored in qos_sp when call comes up
  -------------------------------------------------------------------------*/
  if( NULL != qos_sp)
  {
    ps_iface_ptr = qos_sp->ps_iface_ptr;
  }else
  {
    LOG_MSG_ERROR_0("QMI QoS SP is NULL!");
    ps_iface_ptr = NULL;
  }
  if(ps_iface_ptr == NULL)
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }

  if(!qmi_qosi_verify_client_subs(qos_client_sp, qos_sp->binding.subs_id, TRUE))
  {
    errval = QMI_ERR_INCOMPATIBLE_STATE_V01;
    goto send_result;
  }
  /*-------------------------------------------------------------------------
    set filter id and filter preced to default values
  -------------------------------------------------------------------------*/
  if (req_msg->rx_qos_filter_valid && (req_msg->rx_qos_filter_len > 0)) 
  {
    for (j=0; j<req_ptr->qos_spec.rx.fltr_template.num_filters;j++)
    {
      req_ptr->qos_spec.rx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_id
                                                = PS_IFACE_IPFLTR_DEFAULT_ID;
      req_ptr->qos_spec.rx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_precedence
                                                = PS_IFACE_IPFLTR_DEFAULT_PRCD;
    }
  }
  if (req_msg->tx_qos_filter_valid && (req_msg->tx_qos_filter_len > 0))
  {
    for (j=0; j<req_ptr->qos_spec.tx.fltr_template.num_filters;j++)
    {
      req_ptr->qos_spec.tx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_id
                                                = PS_IFACE_IPFLTR_DEFAULT_ID;
      req_ptr->qos_spec.tx.fltr_template.list_ptr[j].ipfltr_aux_info.fi_precedence
                                                = PS_IFACE_IPFLTR_DEFAULT_PRCD;
    }
  }
  qos_request.qos_ptr = &req_ptr->qos_spec;
  qos_request.subset_id = 0;

  return_val = ps_iface_ioctl(ps_iface_ptr,
                              PS_IFACE_IOCTL_QOS_REQUEST,
                              &qos_request,
                              &ps_errno);

  if(return_val == 0) //IOCTL succeeded
  {
    /* find empty slot to store flow info */
    for(i=0; i<QOSI_MAX_QOS_REQUESTS; i++)
    {
      if(qosi_flow_info[i].qos_handle == 0)
      {
        break;
      }
    }

    if(i == QOSI_MAX_QOS_REQUESTS)
    {
      DATA_ERR_FATAL("QOS handle slots exhausted");
      errval = QMI_ERR_MAX_QOS_REQUESTS_IN_USE_V01;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      Save all the needed flow info in the cl_sp
      - flow_ptr
      - qos handle
      - qos_status
      - tx, rx filters  (needed for evt_report_ind later)
    -----------------------------------------------------------------------*/
    qosi_flow_info[i].cl_sp = qos_client_sp;
    qosi_flow_info[i].flow_ptr = qos_request.flow_ptr;
    /* When available, use PS api to get the cookie */
    qosi_flow_info[i].flow_ptr_cookie =
                                   qos_request.flow_ptr->flow_private.cookie;
    qosi_flow_info[i].qos_handle = (uint32)ps_handle_mgr_get_handle(PS_HANDLE_MGR_CLIENT_QOS,
                                                              (uint16)i+1);
    if (PS_HANDLE_MGR_INVALID_HANDLE == qosi_flow_info[i].qos_handle)
    {
      LOG_MSG_ERROR_0("Invalid handle returned by handle mgr");
      qosi_flow_info[i].qos_handle = 0;
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
    else
    {
      LOG_MSG_INFO2_2("Mapping flow_ptr 0x%x to handle 0x%x", 
                      qosi_flow_info[i].flow_ptr, qosi_flow_info[i].qos_handle)
    }

    qosi_flow_info[i].qos_status = QOSI_STATUS_DEFAULT;

    /* by default flow is enabled and fwd on default is TRUE */
    qosi_flow_info[i].last_rep = QMI_FLOW_ENABLED;
    qosi_flow_info[i].fwd_on_default = TRUE;

    qos_sp->num_qos_reqs++; //num of QoS reqs per qmi instance
    resp_msg->qos_id_valid = TRUE;
    resp_msg->qos_id = qosi_flow_info[i].qos_handle;

  }
  else //report errors to client
  {
    if(ps_errno == DS_EQOSUNAWARE)
    {
      /*---------------------------------------------------------------------
        The request failed because the network is QoS unaware
      ---------------------------------------------------------------------*/
      errval = QMI_ERR_NETWORK_QOS_UNAWARE_V01;
      goto send_result;
    }
    else
    {
      errval = QMI_ERR_INCORRECT_FLOW_FILTER_V01;
      /*---------------------------------------------------------------------
        loop through all Qos specs to look through flow/filter err_masks
        to see if any error indicated
        If so, send appropriate error tlvs in response
      ---------------------------------------------------------------------*/

      /* TX flow errors */
      errval = qosi_fill_flow_error_info(&(qos_request.qos_ptr->tx.flow_template),
                                         &resp_msg->tx_qos_flow_error_valid,
                                         &resp_msg->tx_qos_flow_error_len,
                                         resp_msg->tx_qos_flow_error);
      if(errval != QMI_ERR_NONE_V01)
      {
        goto send_result;
      }


      /* RX flow errors */
      errval = qosi_fill_flow_error_info(&(qos_request.qos_ptr->rx.flow_template),
                                         &resp_msg->rx_qos_flow_error_valid,
                                         &resp_msg->rx_qos_flow_error_len,
                                         resp_msg->rx_qos_flow_error);

      if(errval != QMI_ERR_NONE_V01)
      {
        goto send_result;
      }

      /* TX filter errors */
      errval = qosi_fill_filter_error_info(&(qos_request.qos_ptr->tx.fltr_template),
                                           &resp_msg->tx_qos_filter_error_valid,
                                           &resp_msg->tx_qos_filter_error_len,
                                           resp_msg->tx_qos_filter_error);
      if(errval != QMI_ERR_NONE_V01)
      {
        goto send_result;
      }

      /* RX filter errors */
      errval = qosi_fill_filter_error_info(&(qos_request.qos_ptr->rx.fltr_template),
                                           &resp_msg->rx_qos_filter_error_valid,
                                           &resp_msg->rx_qos_filter_error_len,
                                           resp_msg->rx_qos_filter_error);
      if(errval != QMI_ERR_NONE_V01)
      {
        goto send_result;
      }  

      /* If no error found till now, return INTERNAL error as ioctl failed
         for some other reason */
      if (!(resp_msg->tx_qos_flow_error_valid || resp_msg->rx_qos_flow_error_valid ||
           resp_msg->tx_qos_filter_error_valid || resp_msg->rx_qos_filter_error_valid))
      {
        errval = QMI_ERR_INTERNAL_V01;
      }
    }
  }
send_result:
  PS_SYSTEM_HEAP_MEM_FREE(req_ptr);
  PS_SYSTEM_HEAP_MEM_FREE(req_msg);
  QMI_QOSI_ENCODE_RESPONSE_MSG(QMI_QOS_REQUEST_QOS_EX_RESP_V01,
                               resp_msg,
                               sizeof(qos_request_qos_ex_resp_msg_v01),
                               errval,
                               &response);
  PS_SYSTEM_HEAP_MEM_FREE(resp_msg);
  return response;
} /* qmi_qosi_request_qos_ex() */

/*===========================================================================
  FUNCTION QMI_QOS_MODIFY_QOS_EX()

  DESCRIPTION
    Modify QoS on a particular flow

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_modify_qos_ex
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_client_state_type *     qos_client_sp;
  qmi_qosi_state_type *            qos_sp;
  dsm_item_type *                  response;
  ps_flow_ioctl_qos_modify_type    qos_modify;
  ps_flow_ioctl_primary_qos_modify_type prim_qos_modify;
  qos_req_mod_info_ex_type *       modify_ptr = NULL;
  qos_modify_qos_ex_req_msg_v01 * req_msg = NULL;
  qos_modify_qos_ex_resp_msg_v01* resp_msg = NULL;
  uint8                  itr;
  int16                  ps_errno;
  qmi_error_type_v01     errval;
  int                    return_val;
  qos_spec_field_mask_type  qos_spec_field_mask;
  primary_qos_spec_type  primary_qos_spec;
  boolean                primary = FALSE;
  ip_flow_spec_type      temp_flow_spec;
  ps_flow_type *         def_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in );

  response = NULL;
  ps_errno = 0;

  qos_sp = (qmi_qosi_state_type *) sp;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;

  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(modify_ptr, sizeof(qos_req_mod_info_ex_type),
                                  qos_req_mod_info_ex_type*);
  if (modify_ptr == NULL)
  {
    return NULL;
  }
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(req_msg, sizeof(qos_modify_qos_ex_req_msg_v01),
                                  qos_modify_qos_ex_req_msg_v01*);
  if (req_msg == NULL)
  {
    return NULL;
  }
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(resp_msg, sizeof(qos_modify_qos_ex_resp_msg_v01),
                                  qos_modify_qos_ex_resp_msg_v01*);
  if (resp_msg == NULL)
  {
    return NULL;
  }

  memset(modify_ptr, 0, sizeof(qos_req_mod_info_ex_type));
  memset(req_msg, 0, sizeof(qos_modify_qos_ex_req_msg_v01) );
  memset(resp_msg, 0, sizeof(qos_modify_qos_ex_resp_msg_v01));
  memset(&primary_qos_spec, 0, sizeof(primary_qos_spec));
  memset(&qos_modify, 0, sizeof(qos_modify));
  memset(&prim_qos_modify, 0, sizeof(prim_qos_modify));

  LOG_MSG_INFO2_1("Received QoS Modify from client %d",
                  qos_client_sp->common.clid);

  response = NULL;
  errval   = QMI_ERR_NONE_V01;

  errval = QMI_QOSI_DECODE_REQUEST_MSG(QMI_QOS_MODIFY_QOS_EX_REQ_V01,
  	                                   sdu_in,
  	                                   req_msg,
  	                                   sizeof(qos_modify_qos_ex_req_msg_v01));

  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /*-------------------------------------------------------------
    Convert the Flow spec TLV
  -------------------------------------------------------------*/
  if (req_msg->tx_qos_flow_valid)
  {
    errval = qosi_convert_flow_spec_to_ps(QOSI_MAX_QOS_FLOWS_PER_REQUEST,
                                          modify_ptr->tx_flows,
                                          req_msg->tx_qos_flow_len,
                                          req_msg->tx_qos_flow);
    if(errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }

    /*-------------------------------------------------------------
      Copy the parsed TX flows specs into the qos_spec struct to be
      sent to the ioctl
      - The 1st flow spec becomes the REQ flow
      - the last flow spec becomes the MIN flow
      - Other flows in between become AUX flows
    -------------------------------------------------------------*/
    memscpy(&(modify_ptr->qos_spec.tx.flow_template.req_flow),
            sizeof(ip_flow_type),
            &(modify_ptr->tx_flows[0]),
            sizeof(ip_flow_type));
    modify_ptr->qos_spec.field_mask |= QOS_MASK_TX_FLOW;

    if(req_msg->tx_qos_flow_len > 1)
    {
      memscpy(&(modify_ptr->qos_spec.tx.flow_template.min_req_flow),
              sizeof(ip_flow_type),
              &(modify_ptr->tx_flows[req_msg->tx_qos_flow_len-1]),
              sizeof(ip_flow_type));
      modify_ptr->qos_spec.field_mask |= QOS_MASK_TX_MIN_FLOW;
      if(req_msg->tx_qos_flow_len > 2)
      {
        modify_ptr->qos_spec.tx.flow_template.aux_flow_list_ptr =
                           &(modify_ptr->tx_flows[1]);
        modify_ptr->qos_spec.tx.flow_template.num_aux_flows = 
                           req_msg->tx_qos_flow_len - 2;
        modify_ptr->qos_spec.field_mask |= QOS_MASK_TX_AUXILIARY_FLOWS;
      }
    }
  }

  if (req_msg->rx_qos_flow_valid)
  {
    errval = qosi_convert_flow_spec_to_ps(QOSI_MAX_QOS_FLOWS_PER_REQUEST,
                                          modify_ptr->rx_flows,
                                          req_msg->rx_qos_flow_len,
                                          req_msg->rx_qos_flow);
    if(errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }

    /*-------------------------------------------------------------
      Copy the parsed RX flows specs into the qos_spec struct to be
      sent to the ioctl
      - The 1st flow spec becomes the REQ flow
      - the last flow spec becomes the MIN flow
      - Other flows in between become AUX flows
    -------------------------------------------------------------*/
    memscpy(&(modify_ptr->qos_spec.rx.flow_template.req_flow),
            sizeof(ip_flow_type),
            &(modify_ptr->rx_flows[0]),
            sizeof(ip_flow_type));
    modify_ptr->qos_spec.field_mask |= QOS_MASK_RX_FLOW;

    if(req_msg->rx_qos_flow_len > 1)
    {
      memscpy(&(modify_ptr->qos_spec.rx.flow_template.min_req_flow),
              sizeof(ip_flow_type),
              &(modify_ptr->rx_flows[req_msg->rx_qos_flow_len-1]),
              sizeof(ip_flow_type));
      modify_ptr->qos_spec.field_mask |= QOS_MASK_RX_MIN_FLOW;
      if(req_msg->rx_qos_flow_len > 2)
      {
        modify_ptr->qos_spec.rx.flow_template.aux_flow_list_ptr =
                           &(modify_ptr->rx_flows[1]);
        modify_ptr->qos_spec.rx.flow_template.num_aux_flows = 
                           req_msg->rx_qos_flow_len - 2;
        modify_ptr->qos_spec.field_mask |= QOS_MASK_RX_AUXILIARY_FLOWS;
      }
    }
  }


  /*-------------------------------------------------------------
    Convert the Filter spec TLVs
  -------------------------------------------------------------*/
  if (req_msg->tx_qos_filter_valid)
  {
    errval = qosi_convert_filter_spec_to_ps(QOSI_MAX_QOS_FLTRS_PER_REQUEST,
                                            modify_ptr->tx_filters,
                                            req_msg->tx_qos_filter_len,
                                            req_msg->tx_qos_filter);
    if(errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
    if(req_msg->tx_qos_filter_len > 0)
    {
      /*-----------------------------------------------------------
        Populate the QoS spec with TX filter info
      -----------------------------------------------------------*/
      modify_ptr->qos_spec.tx.fltr_template.num_filters = req_msg->tx_qos_filter_len;
      modify_ptr->qos_spec.tx.fltr_template.list_ptr = modify_ptr->tx_filters;
    }
  }

  if (req_msg->rx_qos_filter_valid)
  {
    errval = qosi_convert_filter_spec_to_ps(QOSI_MAX_QOS_FLTRS_PER_REQUEST,
                                            modify_ptr->rx_filters,
                                            req_msg->rx_qos_filter_len,
                                            req_msg->rx_qos_filter);
    if(errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
    if(req_msg->rx_qos_filter_len > 0)
    {
      /*-----------------------------------------------------------
        Populate the QoS spec with TX filter info
      -----------------------------------------------------------*/
      modify_ptr->qos_spec.rx.fltr_template.num_filters = req_msg->rx_qos_filter_len;
      modify_ptr->qos_spec.rx.fltr_template.list_ptr = modify_ptr->rx_filters;
    }
  }

  if((qos_sp->num_qos_reqs + 1) > QOSI_MAX_QOS_REQUESTS)
  {
    errval = QMI_ERR_MAX_QOS_REQUESTS_IN_USE_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Check whether the call is up
    iface_ptr is stored in qos_sp when call comes up
  -------------------------------------------------------------------------*/
  if(qos_sp->ps_iface_ptr == NULL)
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }

  if(!qmi_qosi_verify_client_subs(qos_client_sp, qos_sp->binding.subs_id, TRUE))
  {
    errval = QMI_ERR_INCOMPATIBLE_STATE_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    set filter id and filter preced to default values
  -------------------------------------------------------------------------*/
  if (req_msg->rx_qos_filter_valid && (req_msg->rx_qos_filter_len > 0))
  {
    for (itr=0; itr<modify_ptr->qos_spec.rx.fltr_template.num_filters;itr++)
    {
      modify_ptr->qos_spec.rx.fltr_template.list_ptr[itr].ipfltr_aux_info.fi_id
                                                = PS_IFACE_IPFLTR_DEFAULT_ID;
      modify_ptr->qos_spec.rx.fltr_template.list_ptr[itr].ipfltr_aux_info.fi_precedence
                                                = PS_IFACE_IPFLTR_DEFAULT_PRCD;
    }
  }
  if (req_msg->tx_qos_filter_valid && (req_msg->tx_qos_filter_len > 0))
  {
    for (itr=0; itr<modify_ptr->qos_spec.tx.fltr_template.num_filters;itr++)
    {
      modify_ptr->qos_spec.tx.fltr_template.list_ptr[itr].ipfltr_aux_info.fi_id
                                                = PS_IFACE_IPFLTR_DEFAULT_ID;
      modify_ptr->qos_spec.tx.fltr_template.list_ptr[itr].ipfltr_aux_info.fi_precedence
                                                = PS_IFACE_IPFLTR_DEFAULT_PRCD;
    }
  }

  /*-------------------------------------------------------------------------
    Check if the request is primary modify and call the ioctl accordingly
  -------------------------------------------------------------------------*/
  if ( req_msg->qos_id == 0 )
  {

    def_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(qos_sp->ps_iface_ptr);
    ASSERT (def_flow_ptr);

    primary_qos_spec.tx_flow_template =
                            modify_ptr->qos_spec.tx.flow_template;
    primary_qos_spec.rx_flow_template =
                            modify_ptr->qos_spec.rx.flow_template;
    primary_qos_spec.field_mask = modify_ptr->qos_spec.field_mask;

    prim_qos_modify.primary_qos_spec_ptr = &primary_qos_spec;
    primary = TRUE;
    return_val = ps_flow_ioctl( def_flow_ptr,
                                PS_FLOW_IOCTL_PRIMARY_QOS_MODIFY,
                                &prim_qos_modify,
                                &ps_errno );
  }
  else
  {
    /*-------------------------------------------------------------------------
      Check if the given qos_handle belongs to the client
    -------------------------------------------------------------------------*/
    for(itr=0; itr < QOSI_MAX_QOS_REQUESTS; itr++)
    {
      if( (qosi_flow_info[itr].qos_handle == req_msg->qos_id) &&
          (qosi_flow_info[itr].cl_sp == qos_client_sp) )
      {
        break;
      }
    }
    if(itr == QOSI_MAX_QOS_REQUESTS)
    {
      errval = QMI_ERR_INVALID_QOS_ID_V01;
      goto send_result;
    }

    /*-------------------------------------------------------------------------
      If Any TX filters present, check to see if the flow's current qos spec
      field mask has any TX flows ( => it has TX filters) hence the filters are
      getting modified and so set the TX_FLTR_MODIFY_MASK
      If the qos field mask has no TX flows, then filter is added 1st time,
      so don't set MODIFY MASK
    -------------------------------------------------------------------------*/
    qos_spec_field_mask =
      PS_FLOW_GET_QOS_FIELD_MASK(qosi_flow_info[itr].flow_ptr);

    if(modify_ptr->qos_spec.tx.fltr_template.num_filters > 0)
    {
      if(qos_spec_field_mask & QOS_MASK_TX_FLOW)
      {
        modify_ptr->qos_spec.field_mask |= QOS_MODIFY_MASK_TX_FLTR_MODIFY;
      }
    }

    if(modify_ptr->qos_spec.rx.fltr_template.num_filters > 0)
    {
      if(qos_spec_field_mask & QOS_MASK_RX_FLOW)
      {
        modify_ptr->qos_spec.field_mask |= QOS_MODIFY_MASK_RX_FLTR_MODIFY;
      }
    }

    qos_modify.qos_ptr = &(modify_ptr->qos_spec);
    qos_modify.subset_id = 0;
    return_val = ps_flow_ioctl(qosi_flow_info[itr].flow_ptr,
                               PS_FLOW_IOCTL_QOS_MODIFY,
                               &qos_modify,
                               &ps_errno);
  }

  if(return_val != -1) //IOCTL succeeded
  {
    goto send_result;
  }

  /* report errors */
  if(ps_errno == DS_EQOSUNAWARE)
  {
    errval = QMI_ERR_NETWORK_QOS_UNAWARE_V01;
    goto send_result;
  }

  if(ps_errno == DS_EOPNOTSUPP)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    goto send_result;
  }

  errval = QMI_ERR_INCORRECT_FLOW_FILTER_V01;

  /*-------------------------------------------------------------------------
    loop through all Qos specs to look through flow/filter err_masks to see
    if any error indicated
    If so, send appropriate error tlvs in response
  -------------------------------------------------------------------------*/
  /* TX flow errors */
  if (primary == TRUE)
  {
    temp_flow_spec = prim_qos_modify.primary_qos_spec_ptr->tx_flow_template;
  }
  else
  {
    temp_flow_spec = qos_modify.qos_ptr->tx.flow_template;
  }
  errval = qosi_fill_flow_error_info(&temp_flow_spec,
                                     &resp_msg->tx_qos_flow_error_valid,
                                     &resp_msg->tx_qos_flow_error_len,
                                     resp_msg->tx_qos_flow_error);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* RX flow errors */
  if (primary == TRUE)
  {
    temp_flow_spec = prim_qos_modify.primary_qos_spec_ptr->rx_flow_template;
  }
  else
  {
    temp_flow_spec = qos_modify.qos_ptr->rx.flow_template;
  }
  errval = qosi_fill_flow_error_info(&temp_flow_spec,
                                     &resp_msg->rx_qos_flow_error_valid,
                                     &resp_msg->rx_qos_flow_error_len,
                                     resp_msg->rx_qos_flow_error);

  if(errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if (primary != TRUE)
  {
    /* TX filter errors */
    errval = qosi_fill_filter_error_info(&(qos_modify.qos_ptr->tx.fltr_template),
                                         &resp_msg->tx_qos_filter_error_valid,
                                         &resp_msg->tx_qos_filter_error_len,
                                         resp_msg->tx_qos_filter_error);
    if(errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }

    /* RX filter errors */
    errval = qosi_fill_filter_error_info(&(qos_modify.qos_ptr->rx.fltr_template),
                                         &resp_msg->rx_qos_filter_error_valid,
                                         &resp_msg->rx_qos_filter_error_len,
                                         resp_msg->rx_qos_filter_error);
    if(errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
  }

  /* If no error found till now, return INTERNAL error as ioctl failed
     for some other reason */
  if (!(resp_msg->tx_qos_flow_error_valid || resp_msg->rx_qos_flow_error_valid ||
       resp_msg->tx_qos_filter_error_valid || resp_msg->rx_qos_filter_error_valid))
  {
    errval = QMI_ERR_INTERNAL_V01;
  }

send_result:
  PS_SYSTEM_HEAP_MEM_FREE(modify_ptr);
  PS_SYSTEM_HEAP_MEM_FREE(req_msg);
  QMI_QOSI_ENCODE_RESPONSE_MSG(QMI_QOS_MODIFY_QOS_EX_RESP_V01,
                               resp_msg,
                               sizeof(qos_modify_qos_ex_resp_msg_v01),
                               errval,
                               &response);
  PS_SYSTEM_HEAP_MEM_FREE(resp_msg);
  return response;
} /* qmi_qosi_modify_qos_ex() */

/*===========================================================================
  FUNCTION QMI_QOS_GET_QOS_INFO()

  DESCRIPTION
    Get the granted QoS flow spec and filters for a particular QoS identifier

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_qosi_get_qos_info
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_qosi_state_type        *  qos_sp;
  qmi_qosi_client_state_type *  qos_client_sp;
  dsm_item_type *               response;
  uint8              i;
  qmi_error_type_v01 errval;
  ps_flow_type *     flow_ptr;
  qos_get_qos_info_req_msg_v01    req_msg;
  qos_get_qos_info_resp_msg_v01 * resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval   = QMI_ERR_NONE_V01;
  qos_sp = (qmi_qosi_state_type *) sp;
  qos_client_sp = (qmi_qosi_client_state_type *) cl_sp;

  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(resp_msg, sizeof(qos_get_qos_info_resp_msg_v01),
                                  qos_get_qos_info_resp_msg_v01*);
  if (resp_msg == NULL)
  {
    return NULL;
  }

  memset(&req_msg, 0, sizeof(qos_get_qos_info_req_msg_v01));
  memset(resp_msg, 0, sizeof(qos_get_qos_info_resp_msg_v01));

  errval = QMI_QOSI_DECODE_REQUEST_MSG(QMI_QOS_GET_QOS_INFO_REQ_V01,
  	                                   sdu_in,
  	                                   &req_msg,
  	                                   sizeof(req_msg));

  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  if (qos_sp->ps_iface_ptr == NULL)
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }

  if ( req_msg.qos_id == 0 ) // Default flow
  {
    if(!qmi_qosi_verify_client_subs(qos_client_sp, qos_sp->binding.subs_id, TRUE))
    {
      errval = QMI_ERR_INCOMPATIBLE_STATE_V01;
      goto send_result;
    }

    /* Primary granted qos */
    flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(qos_sp->ps_iface_ptr);
    ASSERT (flow_ptr);
  }
  else
  {
    /* Secondary granted qos */
    if(!qmi_qosi_verify_client_subs(qos_client_sp, qos_sp->binding.subs_id, TRUE))
    {
      errval = QMI_ERR_INCOMPATIBLE_STATE_V01;
      goto send_result;
    }

  /*-------------------------------------------------------------------------
    Check if the given qos_handle is UE init
  -------------------------------------------------------------------------*/
    for(i=0; i < QOSI_MAX_QOS_REQUESTS; i++)
    {
      if (qosi_flow_info[i].qos_handle == req_msg.qos_id)
      {
        break;
      }
    }

    if(i == QOSI_MAX_QOS_REQUESTS)
    {
      /* Check if the flow is network initiated if
         not found in UE init cache */
      for(i=0; i < QOSI_MAX_NETWORK_INIT_FLOW_NUM; i++)
      {
        if(nw_init_qosi_flow_info[i].qos_handle == req_msg.qos_id) 
        {
          break;
        }
      }
      if ((i == QOSI_MAX_NETWORK_INIT_FLOW_NUM) || 
          (PS_FLOW_GET_IFACE(nw_init_qosi_flow_info[i].flow_ptr) != qos_sp->ps_iface_ptr))
      {
        errval = QMI_ERR_INVALID_QOS_ID_V01;
        goto send_result;
      }
      flow_ptr = nw_init_qosi_flow_info[i].flow_ptr;
    }
    else
    {
      /* UE initiated flows */
      /* Check that request is sent on correct instance */
      if (PS_FLOW_GET_IFACE(qosi_flow_info[i].flow_ptr) != qos_sp->ps_iface_ptr)
      {
        errval = QMI_ERR_INVALID_QOS_ID_V01;
        goto send_result;
      }
      flow_ptr = qosi_flow_info[i].flow_ptr;
    }
  }

  /*-------------------------------------------------------------------------
    Populate the response with the Filter Spec if applicable
  -------------------------------------------------------------------------*/

  if (req_msg.qos_id != 0)
  {
    errval = qosi_append_filter_spec_ex(qos_sp->ps_iface_ptr,
                                        flow_ptr,
                                        &resp_msg->tx_qos_filter_valid,
                                        &resp_msg->tx_qos_filter_len,
                                        resp_msg->tx_qos_filter,
                                        &resp_msg->rx_qos_filter_valid,
                                        &resp_msg->rx_qos_filter_len,
                                        resp_msg->rx_qos_filter);
    if (errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
  }

  /*-------------------------------------------------------------------------
    Populate the response with the GRANTED Flow Spec
  -------------------------------------------------------------------------*/
  errval = qosi_append_granted_flow_spec_ex(qos_sp,
                                            &resp_msg->tx_qos_flow_valid,
                                            &resp_msg->tx_qos_flow,
                                            &resp_msg->rx_qos_flow_valid,
                                            &resp_msg->rx_qos_flow,
                                            &resp_msg->ext_error_info,
                                            flow_ptr);
  if (errval == QMI_ERR_EXTENDED_INTERNAL_V01)
  {
    resp_msg->ext_error_info_valid = TRUE;
  }


send_result:
  QMI_QOSI_ENCODE_RESPONSE_MSG(QMI_QOS_GET_QOS_INFO_RESP_V01,
                               resp_msg,
                               sizeof(qos_get_qos_info_resp_msg_v01),
                               errval,
                               &response);
  PS_SYSTEM_HEAP_MEM_FREE(resp_msg);
  return response;
} /* qmi_qosi_get_qos_info */

/*===========================================================================
  FUNCTION QMI_QOSI_VERIFY_CLIENT_SUBS()

  DESCRIPTION
    This function verifies if the client subscription is applicable to
    the event subscription.
 
  PARAMETERS
    cl_sp         : client state pointer
    event_subs_id : Subscription of the event
    call_specific : Identifies if the request is related to a data call

  RETURN VALUE
   TRUE : if the event is applicable to client
   FALSE: otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_qosi_verify_client_subs
(
  qmi_qosi_client_state_type         * cl_sp,
  uint32                               event_subs_id,
  boolean                              call_specific
)
{
  boolean                             send_ind = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(cl_sp);

  if (event_subs_id >= QMI_QOSI_SUBS_MAX || 
      event_subs_id <= QMI_QOSI_SUBS_DEFAULT)
  {
    //event subs should be valid for resolving
    // ASSERT(0);
    send_ind = FALSE;
    goto send_result;
  }

  if (cl_sp->subscription_id == event_subs_id)
  {
    //Client subs and event subs match
    send_ind = TRUE;
    goto send_result;
  }

  if (event_subs_id == ps_sys_get_default_data_subscription())
  {
    if (cl_sp->subscription_id == QMI_QOSI_SUBS_DEFAULT ||
        cl_sp->subscription_id == QMI_QOSI_SUBS_DONT_CARE)
    {
      //Send to DDS and Don't care clients
      send_ind = TRUE;
      goto send_result;
    }
  }
  else
  {
    if (cl_sp->subscription_id == QMI_QOSI_SUBS_DONT_CARE &&
        call_specific == TRUE) 
    {
      //Send all call specific events to don't care clients
      send_ind = TRUE;
      goto send_result;
    }
  }

send_result:
  if(send_ind == FALSE)
  {
    LOG_MSG_INFO1_2("Client subs %d, event subs %d!",cl_sp->subscription_id,
                    event_subs_id);
  }
  return send_ind;
}/* qmi_qosi_verify_client_subs */

/*===========================================================================
  FUNCTION QMI_QOSI_REG_PS_SYS_EVENTS()

  DESCRIPTION
    This function registers for PS system level events
 
  PARAMETERS
    None

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_reg_ps_sys_events
(  
  void
)
{
  int                      sub_ind = 0;
  ps_sys_event_enum_type   event_id   = PS_SYS_EVENT_MIN;
  int16                    result;
  int16                    dss_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (sub_ind = PS_SYS_PRIMARY_SUBS; sub_ind < PS_SYS_SUBS_MAX; sub_ind++)
  {
    result = ps_sys_event_reg_ex(PS_SYS_TECH_3GPP2,
                                 PS_SYS_EVENT_3GPP2_QOS_NETWORK_STATUS_CHANGE,
                                 (ps_sys_subscription_enum_type)sub_ind,
                                 qmi_qosi_network_status_change_cb,
                                 NULL,
                                 &dss_errno);
    if (result < 0)
    {
      event_id = PS_SYS_EVENT_3GPP2_QOS_NETWORK_STATUS_CHANGE;
      break;
    }
  } /* end for */

  if(result < 0)
  {
    LOG_MSG_ERROR_2("Couldn't reg for ps sys event_id = %d Err : (%d)",
                    event_id, dss_errno);
    ASSERT(0);
    return;
  }

}/* qmi_qosi_reg_ps_sys_events */

/*===========================================================================
  FUNCTION QMI_QOSI_RESOLVE_CLIENT_SUBS()

  DESCRIPTION
    This function resolves the client subscription.
 
  PARAMETERS
    cl_sp         : client state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint32 qmi_qosi_resolve_client_subs
(
  void         * cl_sp
)
{
  qmi_qosi_client_state_type *       client_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(cl_sp);

  client_sp = (qmi_qosi_client_state_type *)cl_sp;

  if (client_sp->subscription_id == QMI_QOSI_SUBS_DONT_CARE || 
      client_sp->subscription_id == QMI_QOSI_SUBS_DEFAULT)
  {
    LOG_MSG_INFO1_1("DEBUG:Resolved cl subs to DDS subs_id = %d",
                     ps_sys_get_default_data_subscription());
    return ps_sys_get_default_data_subscription();
  }
  else
  {
    LOG_MSG_INFO1_1("DEBUG:Resolved cl subs as subs_id = %d",
                     client_sp->subscription_id);
    return(client_sp->subscription_id);
  }
}/* qmi_qosi_resolve_client_subs */

/*===========================================================================
  FUNCTION QMI_QOSI_NETWORK_STATUS_CHANGE_CB()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    qos network status change event

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    event_info_ptr : Aware/Unaware status
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qosi_network_status_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
)
{
  qmi_qosi_cmd_buf_type *cmd_ptr = NULL;
  ps_sys_3gpp2_network_qos_aware_status  * ps_nw_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(event_info_ptr);
  ps_nw_status = (ps_sys_3gpp2_network_qos_aware_status *)event_info_ptr;

  cmd_ptr = (qmi_qosi_cmd_buf_type *)
              qmi_qosi_get_cmd_buf(QMI_CMD_QOS_NW_STATUS_CHANGE_EVT);

  if( cmd_ptr == NULL)
  {
    return;
  }
  
  cmd_ptr->data.nw_aware_info.network_aware_status = *ps_nw_status;
  cmd_ptr->cmd_id = QMI_CMD_QOS_NW_STATUS_CHANGE_EVT;
  cmd_ptr->subscription_id = subscription_id;

  dcc_send_cmd_ex (DCC_QMI_CMD, cmd_ptr);
} /* qmi_qosi_network_status_change_cb() */

/*===========================================================================
  FUNCTION QMI_QOSI_DECODE_MSG()

  DESCRIPTION
    This function is a wrapper functions for qmi_svc_idl_message_decode
 
  PARAMETERS
    message_id : unique message ID.
    msg_ptr      : input buffer with all the TLV's.
    req_msg     : c struct to be filled
    req_size     : sizeof c structure

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_qosi_decode_msg
(
  uint16_t          message_id,
  dsm_item_type **  msg_ptr,
  void *            req_msg,
  uint32_t          req_size
)
{
  if(req_msg != NULL)
  {
    return (qmi_error_type_v01)qmi_svc_idl_message_decode(qos_svc_obj,
               message_id,msg_ptr,req_msg,req_size);
  }
  else
  {
    return QMI_ERR_INTERNAL_V01;
  }
} /* qmi_qosi_decode_msg */

/*===========================================================================
  FUNCTION QMI_QOSI_ENCODE_MSG()

  DESCRIPTION
    This function is a wrapper functions for qmi_svc_idl_message_encode
    It assigns error and result codes to response and invokes idl encode function.
 
  PARAMETERS
    message_id : unique message ID.
    resp_msg   : input response buffer.
    size       : size of resp_msg
    err_code   : error code to be filled in the response
    response   : output dsm item pointer which has all the result TLV's

  RETURN VALUE
    TRUE is successful othewise FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_qosi_encode_msg
(
  uint16_t             message_id,
  void *               resp_msg,
  uint32_t             size,
  qmi_error_type_v01     err_code,
  dsm_item_type **     response
)
{
  qmi_result_type_v01 result;
  qmi_response_type_v01* res_ptr = NULL;
  /*---------------------------------------------------------------------------*/
  result = (err_code == QMI_ERR_NONE_V01? QMI_RESULT_SUCCESS_V01
                                     : QMI_RESULT_FAILURE_V01);
  res_ptr = (qmi_response_type_v01*)resp_msg;
  res_ptr->result = (qmi_result_type_v01)result;
  res_ptr->error = (qmi_error_type_v01)err_code;

  if( !qmi_svc_idl_message_encode(qos_svc_obj,
      QMI_IDL_RESPONSE,message_id,resp_msg,size,response))
  {
    dsm_free_packet(response);
    *response = NULL;
    LOG_MSG_ERROR_1("Encoding failed for message id %d",message_id);
    if(!qmi_svc_put_result_tlv(response,(qmi_result_e_type)QMI_RESULT_FAILURE_V01,
                              (qmi_error_e_type)QMI_ERR_INTERNAL_V01))
    {
      dsm_free_packet(response);
      return FALSE;
    }
  }
  return TRUE;
} /* qmi_qosi_encode_msg */

