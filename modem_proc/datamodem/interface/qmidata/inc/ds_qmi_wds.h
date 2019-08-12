#ifndef _DS_QMI_WDS_H
#define _DS_QMI_WDS_H
/*===========================================================================

                         D S _ Q M I _ W D S . H

DESCRIPTION

 The Data Services Qualcomm Wireless Data Services MSM Interface header file.

EXTERNALIZED FUNCTIONS

  qmi_wds_init()
    Register the Wireless Data Service with QMUX for all applicable QMI links.

  qmi_wds_pkt_srvc_status_ind()
    Indicates the state of the network connection

  qmi_wds_gen_serving_system_ind_cb()
    Call back function called when the servign system changes.

  qmi_wds_rssi_change_cb()
    Callback to be called when signal strength changes.

  qmi_wds_gen_channel_rate_ind()
    Call back function called when the channel rate changes.


Copyright (c) 2004-2006, 2010 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_wds.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
11/21/12    svj    Added support for Operator Reserved Protocol Information.  
08/08/12    pgm    Support for different APNS in Dual-IP.
12/17/10    kk     Removing DCC command buffer dependency from QMI services.
06/19/10    vs     Changes for Dual IP support over single QMI instance
08/23/06    ks     Cleaned up media connect/disconnect, pkt_srvc_ind to be
                   sent from wds.
07/06/06    ks     Clean up
12/02/05    ks     Added new commnads for profiles. Added support for using
                   profiles in Start Network Interface
06/27/05    jd     Added qmi_wds_send_pkt_stats QMI command handler
05/31/05   jd/ks   Code review updates
05/11/05    ks     Code review comments and clean up.
03/14/05    ks     Removed featurization
11/21/04    jd/ks  Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "ds_qmi_defs.h"
#include "ds_qmi_if.h"
#include "ps_iface_defs.h"
#include "ds_profile_3gpp.h"
#include "ds_rmnet_defs.h"
#include "qmi_si.h"


/*---------------------------------------------------------------------------
  WDS connection status enum type
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_WDS_CONN_DISCONNECTED   = 1,
  QMI_WDS_CONN_CONNECTED      = 2,
  QMI_WDS_CONN_SUSPENDED      = 3,
  QMI_WDS_CONN_AUTHENTICATING = 4
} qmi_wds_conn_status_e_type;

/*---------------------------------------------------------------------------
  Traffic channel dormancy status enum type
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_WDS_TRAFFIC_CH_MIN = -1,
  QMI_WDS_TRAFFIC_CH_UNKNOWN = 0, /* not published */
  QMI_WDS_TRAFFIC_CH_DORMANT = 1,
  QMI_WDS_TRAFFIC_CH_ACTIVE  = 2,
  QMI_WDS_TRAFFIC_CH_MAX
} qmi_wds_dormancy_status_e_type;

#define WDS_PROFILE_TYPE_3GPP (0x00)
#define WDS_PROFILE_TYPE_3GPP2 (0x01)
#define WDS_PROFILE_TYPE_EPC   (0x02)

#define WDS_TECH_PREF_ANY  (0x03)
#define WDS_TECH_PREF_UMTS (0x01)
#define WDS_TECH_PREF_CDMA (0x02)

#define WDSI_APN_MAX_LEN   (256)

/* Currently not being used */
/* #define WDS_PROFILE_SOCKETS_FAMILY  1 */
/* #define WDS_PROFILE_ATCOP_FAMILY    2 */
#define WDS_PROFILE_RMNET_FAMILY    DS_PROFILE_3GPP_RMNET_PROFILE_FAMILY

/*---------------------------------------------------------------------------
  REQUESTED RUNTIME SETTINGS bitmask
---------------------------------------------------------------------------*/
#define QMI_WDS_MASK_PROFILE_ID   (0x01)
#define QMI_WDS_MASK_PROFILE_NAME (0x02)
#define QMI_WDS_MASK_PDP_TYPE     (0x04)
#define QMI_WDS_MASK_APN          (0x08)
#define QMI_WDS_MASK_DNS_ADDR     (0x10)
#define QMI_WDS_MASK_GRANTED_QOS  (0x20)
#define QMI_WDS_MASK_USERNAME     (0x40)
#define QMI_WDS_MASK_AUTH_PROTO   (0x80)
#define QMI_WDS_MASK_IP_ADDR      (0x0100)
#define QMI_WDS_MASK_GATEWAY      (0x0200)
#define QMI_WDS_MASK_PCSCF_PCO    (0x0400)
#define QMI_WDS_MASK_PCSCF_ADDR   (0x0800)
#define QMI_WDS_MASK_PCSCF_NAME   (0x1000)
#define QMI_WDS_MASK_RUNTIME_DEFAULT  (0x1FFF) /* Only covers bits 0 to 12 */
#define QMI_WDS_MASK_MTU              (0x2000)
#define QMI_WDS_MASK_DOMAIN_NAME_LIST (0x4000)
#define QMI_WDS_MASK_IP_FAMILY        (0x8000)
#define QMI_WDS_MASK_IM_CN_FLAG       (0x10000)
#define QMI_WDS_MASK_TECH_NAME        (0x20000)
#define QMI_WDS_MASK_OP_RES_PCO       (0x40000)

/*---------------------------------------------------------------------------
Type for additional Optional TLV in GET PROFILE LIST REQUEST
Also accessed from DS Profile file.
---------------------------------------------------------------------------*/
#define WDSI_PRM_TYPE_PROFILE_TECH_TYPE        (0x10)

/*---------------------------------------------------------------------------
 PROFILE CREATE request optional TLVs
---------------------------------------------------------------------------*/
#define WDSI_PRM_TYPE_PROFILE_PERSISTENT       (0x8F)


/*---------------------------------------------------------------------------
  callback function to be invoked when call get connected
---------------------------------------------------------------------------*/
typedef void (* qmi_wds_call_status_cb_f_type)
(
  uint32  ep_id,
  uint8   mux_id,
  boolean is_v6,
  boolean call_up
);

typedef void (* qmi_wds_swap_instances_cb_f_type)
(
  uint32  ep_id,
  uint8   mux_id
);

/*-------------------------------------------------------------------------
  Macros 
---------------------------------------------------------------------------*/
#define QMI_WDSI_ENCODE_RESPONSE_MSG( message_id, resp_msg, size, err_code, response) \
         qmi_wdsi_encode_msg( message_id, resp_msg, size, err_code, response)

#define QMI_WDSI_DECODE_REQUEST_MSG( message_id, sdu_in, req_msg, req_size)\
        qmi_wdsi_decode_msg( message_id, sdu_in, req_msg, req_size);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  FUNCTION QMI_WDS_INIT()

  DESCRIPTION
    Register the Wireless Data Service with QMUX

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_wds_init
(
  void 
);

/*===========================================================================
  FUNCTION QMI_WDS_GENERATE_CHANNEL_RATE_IND()

  DESCRIPTION
    Called when the channel rate changes.
    Causes wds_event_report_ind to be sent

  PARAMETERS
    event  : Event that causes channel rate ind to be sent out

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_wds_generate_channel_rate_ind
(
  qmi_if_ch_rate_trigger_e_type event,
  boolean is_v4,
  uint32 subs_id
);

/*===========================================================================
FUNCTION  QMI_WDS_CALLHIST_SAVE_PHONE_NUM

DESCRIPTION
  Saves the phone number from CM into a local QMI variable for call history
  purposes.  The data in phone_num_buf should be in 8-bit ASCII format
  (not 4-bit DTMF) with a trailing null byte.  This function is called from
  qmi_if_process_cm_call_event() in the file ds_qmi_if.c

PARAMETERS
  phone_num_len - length of phone_num_buf (max we will accept is governed
                  by QMI_PBM_PHONENUM_MAX_LEN)
  phone_num_buf - buffer containing phone number

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qmi_wds_callhist_save_phone_num
(
  uint8         phone_num_len,
  const uint8  *phone_num_buf
);

/*===========================================================================
  FUNCTION QMI_WDS_SET_CALL_STATUS_HANDLER()

  DESCRIPTION
    Specify the callback function that should be invoked when the call comes
    UP or is torn DOWN

  PARAMETERS
    qmi_instance_e_type : QMI instance

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_wds_init() must have been called previously

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_wds_set_call_status_handler
(
  qmi_wds_call_status_cb_f_type  call_status_cb
);

/*===========================================================================
  FUNCTION QMI_WDS_SET_SWAP_INSTANCE_HANDLER()

  DESCRIPTION
   Specify the callback function for QMI QoS so that WDS can invoke it
   when default instance is changed.

  PARAMETERS
    qmi_wds_swap_instances_cb_f_type - callback function

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_wds_init() must have been called previously

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_wds_set_swap_instance_handler
(
  qmi_wds_swap_instances_cb_f_type  swap_instance_cb
);
/*===========================================================================
  FUNCTION WDS_RMNET_IFACE_BINDING_COMPLETE_CB()

  DESCRIPTION
    Callback when rmnet binding is complete

  PARAMETERS
    rmnet_iface_ptr: rmnet iface pointer
    ep_id:           Endpoint ID
    mux_id:          MUX id
    is_v6:           is V6 call
    subs_id:         Subscription identifier

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void wds_rmnet_iface_binding_complete_cb
(
  ps_iface_type *       rmnet_iface_ptr,
  uint32                ep_id,
  uint8                 mux_id,
  uint8                 is_v6,
  uint32                subs_id
);

/*===========================================================================
FUNCTION   WDS_RMNET_FLOW_CTRL_CB()

DESCRIPTION
  This callback function is called when flow control events occur on
  RmNet interface. An indication will be sent to registered clients.

PARAMETERS
    rmnet_inst:       RMNET instance
    uplink_flow_ctrl: Uplink flow control status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wds_rmnet_flow_ctrl_cb
(
  uint32                          ep_id,
  uint8                           mux_id,
  uint8                           is_v6,
  boolean                         uplink_flow_ctrl
);

/*===========================================================================
  FUNCTION QMI_WDS_PROCESS_OPRT_MODE()

  DESCRIPTION
    Process needed actions for operation mode change event from CM
    Called when Operating Mode Change in CM_PH event is received.

  PARAMETERS
    oprt_mode :  Operating mode

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_wds_process_oprt_mode
(
  sys_oprt_mode_e_type             oprt_mode
);

/*===========================================================================
  FUNCTION QMI_WDS_BRINGUP_ON_REV_IP_TRANSPORT_INSTANCE()

  DESCRIPTION
    Performs initialization needed for WDS instance on which Reverse IP transport is
    being set up

  PARAMETERS
    iface_ptr :  iface ptr thats being brought up by MH for which reverse data path
                 needs to be setup
    qmi_inst  : WDS Instance on which Reverse IP Transport is to be established

  RETURN VALUE
    0 on Failure
    1 on Succes

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_wds_bringup_on_rev_ip_transport_instance
(
  ps_iface_type *             iface_ptr,
  uint32                      ep_id,
  uint8                       mux_id
);

/*===========================================================================
  FUNCTION QMI_WDS_GET_FC_STATS()

  DESCRIPTION
    This function retrieves WDS flow control stats

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
void qmi_wds_get_fc_stats
(
  uint32  * num_flow_enable,
  uint32  * num_flow_disable
);

/*===========================================================================
  FUNCTION QMI_WDSI_RESOLVE_CLIENT_SUBS()

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
uint32 qmi_wdsi_resolve_client_subs
(
  void         * cl_sp
);

/*===========================================================================
  FUNCTION QMI_WDSI_ENCODE_MSG()

  DESCRIPTION
    This function is a wrapper functions for qmi_svc_idl_message_encode
    assgins error and result codes to response and invlokes idl encode function.
 
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
boolean qmi_wdsi_encode_msg
(
  uint16_t           message_id,
  void *             resp_msg,
  uint32_t           size,
  qmi_error_type_v01 err_code,
  dsm_item_type **   response
);

/*===========================================================================
  FUNCTION QMI_WDSI_DECODE_MSG()

  DESCRIPTION
    This function is a wrapper functions for qmi_svc_idl_message_decode
    assgins error and result codes to response and invlokes idl encode function.
 
  PARAMETERS
    message_id : unique message ID.
    msg_ptr    : input buffer with all the TLV's.
    req_msg    : c struct to be filled
    req_size   : sizeof c structure

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_wdsi_decode_msg
(
  uint16_t          message_id,
  dsm_item_type **  msg_ptr,
  void *            req_msg,
  uint32_t          req_size
);

#endif /* _DS_QMI_WDS_H */
