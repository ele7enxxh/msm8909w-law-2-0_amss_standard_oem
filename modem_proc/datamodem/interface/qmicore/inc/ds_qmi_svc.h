#ifndef _DS_QMI_SVC_H
#define _DS_QMI_SVC_H
/*===========================================================================

                         D S _ Q M I _ S V C  . H

DESCRIPTION

 The Data Services QMI Services Header file. Contains routines common to
 all QMI services.

EXTERNALIZED FUNCTIONS

   qmi_svc_put_param_tlv()
     formulate a message option TLV from given type, length, and value in 
     the provided DSM item

   qmi_svc_put_result_tlv()
     formulate a result option TLV from given result and error code in 
     the provided DSM item

   qmi_svc_prepend_msg_hdr()
     Append the message header type and length to the provided QMI service
     message payload

   qmi_svc_get_tl()
     Extract the type and length fields from a message parameter TLV  

Copyright (c) 2005-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/ds_qmi_svc.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/19/10    vs     Changes for Dual IP support over single QMI instance
01/14/10    ar     Add predefined power state handles
10/08/09    kk     Windows7 compliance - DMS changes.
07/11/06    ks     Moved QMI service header struct to this file.
07/06/06    ks     Added generic qmi_svc_get_tl() to get type and length.
09/08/05    ks     Moved QMI_SVC_PKT_PUSH macro from .c file.
06/27/05    jd     Moved QMI_SVC_PKT_PUSH to .c
05/31/05   jd/ks   Code review updates
05/11/05    ks     Created module
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_svc_ext.h"
#include "dcc_task_defs.h"
#include "qmi_svc_utils.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Power save mode filtering values.
---------------------------------------------------------------------------*/
#define QMI_SVC_PWRSAVE_STATE_INT_MIN      (0x00)
#define QMI_SVC_PWRSAVE_STATE_INT_MAX      (0xFF)

#define QMI_SVC_PWRSAVE_INDS_PER_SVC_MAX   (10)
#define QMI_SVC_PWRSAVE_EXT_INDS_PER_SVC_MAX   (255)

#define QMI_SVC_PWRSAVE_SPECS_MAX          (4)

typedef void*  qmi_svc_pwr_state_handle_type;
#define QMI_SVC_PWRSAVE_STATE_INVALID    ((qmi_svc_pwr_state_handle_type)0xFF)
#define QMI_SVC_PWRSAVE_STATE_NORMAL     ((qmi_svc_pwr_state_handle_type)0x00)
#define QMI_SVC_PWRSAVE_STATE_SUSPEND    ((qmi_svc_pwr_state_handle_type)0x01)
#define QMI_SVC_PWRSAVE_STATE_PWRDOWN    ((qmi_svc_pwr_state_handle_type)0x02)

/* Macro to validate power state handles specified by clients */
#define IS_PWRSAVE_STATE_VALID(state)                     \
       ((QMI_SVC_PWRSAVE_STATE_NORMAL    == state) ||     \
        (QMI_SVC_PWRSAVE_STATE_SUSPEND   == state) ||     \
        (QMI_SVC_PWRSAVE_STATE_PWRDOWN   == state) ||     \
        (QMI_SVC_PWRSAVE_STATE_INT_MAX < (uint32)state))



/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service header type
---------------------------------------------------------------------------*/
typedef  struct
{
  uint8 ctl_flags;
  uint8 transaction_id[2];
} qmi_svc_hdr_type;

/*---------------------------------------------------------------------------
  QMUX power save indication filter structures
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint16 num_inds;
  uint16 ind_list[0];
} qmi_svc_ind_spec_s_type;

typedef struct qmi_svc_power_spec_s
{
  qmi_svc_pwr_state_handle_type  state_handle;
  qmi_svc_ind_spec_s_type *      svc_inds[QMUX_SERVICE_MAX];
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
  boolean                        response_reporting;
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/
} qmi_svc_power_spec_s_type;

typedef struct qmi_svc_pwrsave_info_s
{
  qmi_svc_power_spec_s_type  *specs[QMI_SVC_PWRSAVE_SPECS_MAX];
  qmi_svc_power_spec_s_type  *cur_spec_ptr;
} qmi_svc_pwrsave_info_s_type;

/*===========================================================================
  TYPEDEF QMI_LEGACY_SVC_COMMON_MSG_HDR_TYPE

  DESCRIPTION 
    Type for common legacy service msg header. 
===========================================================================*/
typedef struct
{
  int32                 qmi_instance;       /* QMI Instance               */
  qmux_service_e_type   service;            /* QMI Service type           */
  uint16                transaction_id;     /* Transaction ID             */ 
  uint8                 client_id;          /* Service client ID          */   
}qmi_legacy_svc_common_msg_hdr_type;

/*===========================================================================
  TYPEDEF FILE_ID_ENUM_TYPE

  DESCRIPTION 
    To Indicate File ID's in QMI and RMNET,mainly used for logging purppose 
    instead of using file name which saves around 10 bytes for each message.
===========================================================================*/
enum
{
  FILE_ID_DS_QMI_SVC_EXT = 0,
  FILE_ID_DS_QMI_SVC = 1,
  FILE_ID_DS_QMI_TASK = 2,
  FILE_ID_DS_QMUX = 3,
  FILE_ID_DS_QMUX_LOGGING = 4,
  FILE_ID_QMI_FRAMEWORK = 5,
  FILE_ID_QMI_CMD_PROCESS = 6, 
  FILE_ID_QMI_SAP_XPORT_QMUX = 7,
  FILE_ID_QMI_SVC_IMPL = 8,
  FILE_ID_QMI_CHARGER = 9,
  FILE_ID_DS_QMI_CTL = 10,
  FILE_ID_DS_QMI_CTL_REMOTE = 11,
  FILE_ID_DS_QMI_DMS = 12,
  FILE_ID_DS_QMI_IF = 13,
  FILE_ID_DS_QMI_WDS = 14,
  FILE_ID_DS_QMI_AUTH = 15,
  FILE_ID_DS_QMI_AT = 16,
  FILE_ID_DS_QMI_WDS_PROFILE = 17,
  FILE_ID_DS_QMI_CFLOG = 18,
  FILE_ID_DS_QMI_PWRMGMT_SHIM = 19,
  FILE_ID_DS_QMI_WDS_PBM = 20,
  FILE_ID_DS_QMI_NV_UTIL = 21,
  FILE_ID_DS_QMI_WDA = 22,
  FILE_ID_DS_QMI_CSVT = 23,
  FILE_ID_DS_QMI_DSD = 24,
  FILE_ID_DS_QMI_DFS = 25,
  FILE_ID_DS_QMI_FW_COMMON = 26,
  FILE_ID_DS_QMI_QOS = 27,
  FILE_ID_DS_QMI_WDS_UMTS = 28,
  FILE_ID_DS_QMI_WDS_LTE = 29,
  FILE_ID_DS_QMI_QCMAP = 30,
  FILE_ID_DS_QMI_OTDT = 31,
  FILE_ID_DS_QMI_VS = 32,
  FILE_ID_DS_RMNET_SM_EXT = 33,
  FILE_ID_DS_RMNET_META_SM = 34,
  FILE_ID_DS_RMNET_SIO = 35,
  FILE_ID_DS_RMNET_SIO_RT_ACL = 36,
  FILE_ID_DS_RMNET_TASK = 37,
  FILE_ID_DS_RMNET_V4_SM = 38,
  FILE_ID_DS_RMNET_V6_SM = 39,
  FILE_ID_DS_RMNET_UTILS = 40,
  FILE_ID_PS_UL_OPTIMIZED_HDLR = 41,
  FILE_ID_PS_DL_OPTIMIZED_HDLR = 42,
  FILE_ID_PS_NAT_OPTIMIZED_DATA_PATH = 43,
  FILE_ID_DS_QMAP = 44,
  FILE_ID_DS_RMNET_QMAP = 45,
  FILE_ID_PS_UICC = 46,
  FILE_ID_PS_UICC_IFACE4_HDLR = 47,
  FILE_ID_PS_UICC_META_SM = 48,
  FILE_ID_PS_UICC_V4_SM = 49,
  FILE_ID_PS_UICC_RT_ACL = 50,
  FILE_ID_PS_SLIP_CORE = 51,
  FILE_ID_PS_SLIP_CONFIG = 52,
  FILE_ID_PS_SLIP_IFACE_HDLR = 53,
  FILE_ID_PS_SLIP_RT_ACL = 54,
  FILE_ID_DS_REV_IP_TRANSPORT_HDLR = 55,
  FILE_ID_PS_DPM = 56,
  FILE_ID_PS_DPM_UTILS = 57,
  FILE_ID_PS_DPM_MBIM_HDLR = 58,
  FILE_ID_DS_QMI_DPM = 59,
  FILE_ID_DS_RMNET_XPORT = 60,
  FILE_ID_PS_DPM_UL_LEGACY = 61,
  FILE_ID_PS_DPM_UL_OPT = 62
}file_id_enum_type;

/*===========================================================================

                            EXTERNALIZED FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_SVC_GET_PARAM_TL()

  DESCRIPTION
    Extract the type and length fields from a message parameter TLV.

  PARAMETERS
    pkt:   pointer to dsm containing tlv
    type:  output - extracted TLV type value
    len:   output - extracted TLV length value

  RETURN VALUE
    TRUE:   Type and length extracted successfully
    FALSE:  Couldn't extract type and length

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean  qmi_svc_get_param_tl
(
  dsm_item_type **  pkt,
  uint8 *           type,
  uint16 *          len
);

/*===========================================================================
  FUNCTION QMI_SVC_NEW_CMD_BUF()

  DESCRIPTION
    Allocate a QMI command buffer

  PARAMETERS
    x_p : Transaction that the new command is part of

  RETURN VALUE
    pointer to allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern qmi_cmd_buf_type *  qmi_svc_new_cmd_buf
(
  qmi_transaction_type *  x_p
);

/*===========================================================================
  FUNCTION QMI_SVC_FREE_CMD_BUF()

  DESCRIPTION
    Frees a previously allocated QMI command buffer

  PARAMETERS
    cmd_buf_handle : Handle to command buf pointer which points to the cmd
                     buffer to be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_xxx_init() must have been called before.
    *Must be called in the xxx serving task context*
    (xxx depends on service type)

  SIDE EFFECTS
    Removes reference to this command buf from the associated transaction
    structure.  
===========================================================================*/
extern void qmi_svc_free_cmd_buf
(
  qmi_cmd_buf_type **  cmd_buf_handle
);


/*===========================================================================
  FUNCTION QMI_SVC_GET_TRANSACTION()

  DESCRIPTION
    Get a transaction buffer for the specified qmi control point specified
    by the client state pointer provided

  PARAMETERS
    svc_sp :  state pointer for Service that the transaction was sent to
    cl_sp :  client state pointer for client to get transaction buffer for
             or NULL if sending a broadcast indication

  RETURN VALUE
    NULL if no transaction buffer is available
    otherwise, pointer to transaction buffer

  DEPENDENCIES
    If obtaining the global transaction buffer (for broadcast indications),
    the transaction must be freed soon after being allocated, before any
    subsequent call to this function.

  SIDE EFFECTS
    service global transaction might be allocated.
===========================================================================*/
extern qmi_transaction_type *  qmi_svc_get_transaction
(
  qmi_common_svc_state_type *      svc_sp,
  qmi_common_client_state_type *   cl_sp
);

/*===========================================================================
  FUNCTION QMI_SVC_FREE_TRANSACTION()

  DESCRIPTION
    Frees the passed transaction buffer, returning it to the client state
    info block if not the global transaction.

  PARAMETERS
    x_p :  handle to transaction pointer pointing to transaction buffer to
           be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_xxx_init() must have been called prior to this Api
    (xxx depends on service)

  SIDE EFFECTS
    Nulls out the caller's transaction buffer pointer
===========================================================================*/
extern void qmi_svc_free_transaction
(
  qmi_transaction_type **  x_p
);

/*===========================================================================
  FUNCTION QMI_SVC_RECV()

  DESCRIPTION
    Receives the QMUX SDU and passes each individual SVC message to 
    qmi_xxxi_input. This function is registered with QMUX.
    (xxx depends on service)
    
  PARAMETERS
    sp     : service provided state pointer (user data)
    clid   : Client ID corresponding the message 
    sdu_in : DSM item containing incoming msg
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_svc_recv
( 
  qmi_common_svc_state_type *  sp,
  uint8             clid, 
  dsm_item_type *   sdu_in
);

/*===========================================================================
  FUNCTION QMI_SVC_SEND()

  DESCRIPTION
    Send the message to the QMUX layer

  PARAMETERS
    svc_sp    : service provided state pointer (user data)
    cmd_buf_p : cmd_buf pointer for the command that the response is for
    msg_type : message type, whether it is a cmd, response or indication
    msg_ptr   : response to send out

  RETURN VALUE
    TRUE if command was sent (or is queued in transaction for later send)
    FALSE if something was bad

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_svc_send
(
  qmi_common_svc_state_type *  sp,
  qmi_cmd_buf_type * cmd_buf_p,
  uint8              msg_type,
  dsm_item_type *     msg_ptr
);

/*===========================================================================
  FUNCTION QMI_SVC_GET_CLIENT_SP_BY_CLID()

  DESCRIPTION
    To obtain cl_sp from svc_sp and clientID
    
  PARAMETERS
    state ptr
    clientID

  RETURN VALUE
    client state ptr

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void *  qmi_svc_get_client_sp_by_clid
(
  qmi_common_svc_state_type *  svc_sp,
  uint8   clid
);

/*===========================================================================
  FUNCTION QMI_SVC_ISVALID_CLID()

  DESCRIPTION
    Checks validity of the clientID. This function is registered with QMUX
                       
  PARAMETERS
    svc_sp:  service provided state pointer (user data)
    clid:  ID allocated to the client, whose validity is to be checked

  RETURN VALUE
    TRUE  :  Client ID is allocated
    FALSE :  Client ID is not allocated
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean  qmi_svc_is_valid_clid
(
  qmi_common_svc_state_type *   svc_sp,       
  uint8    clid
);

/*===========================================================================
  FUNCTION QMI_SVC_ALLOC_CLID()

  DESCRIPTION
    Allocate a client ID.  This function is called whenever a QMI_CTL alloc 
    client id message with service type xxx is received.
    (xxx depends on service type)

  PARAMETERS
    svc_sp:  service provided state pointer (user data)

  RETURN VALUE
    Client ID on Sucesss
    Zero      on Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern uint8  qmi_svc_alloc_clid
(
  qmi_common_svc_state_type *  svc_sp
);

/*===========================================================================
  FUNCTION QMI_SVC_QMI_LINK_CLOSED()

  DESCRIPTION
    Frees all existing service clients, as QMI link is closed.

  PARAMETERS
    svc_sp : holds service instance state pointer 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_svc_qmi_link_closed
(
  qmi_common_svc_state_type *  svc_sp
);

/*===========================================================================
  FUNCTION QMI_SVC_DEALLOC_CLID_CB()

  DESCRIPTION
    Deallocate ClientID
                       
  PARAMETERS
    state ptr: service provided state pointer (user data)
    clientID : ID of the client to be deallocated

  RETURN VALUE
    TRUE  :on Success
    FALSE :on Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean  qmi_svc_dealloc_clid
(
  qmi_common_svc_state_type *  svc_sp,
  uint8   clid
);

/*===========================================================================
FUNCTION QMI_SVC_CMD_BUF_BELONGS_TO_CLIENT_TRANSACTION

DESCRIPTION
  Queue Compare Function
    Used by the queue searching functions to determine if a command buf
    belongs to client transaction. 

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if the element should be operated upon, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/

extern int qmi_svc_cmd_buf_belongs_to_client_transaction
(
  qmi_cmd_buf_type  *  cmd_buf_p,       
  qmi_cmd_buf_type  *  comp_param_ptr   
);

/*===========================================================================
FUNCTION QMI_SVC_CMD_BUF_BELONGS_TO_CLIENT

DESCRIPTION
  Queue Compare Function
    Used by the queue searching functions to determine if an item is in
       the queue.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if the element should be operated upon, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern int qmi_svc_cmd_buf_belongs_to_client
(
  qmi_cmd_buf_type *              cmd_buf_p,
  qmi_common_client_state_type *  cl_sp
);

/*===========================================================================
  FUNCTION QMI_SVC_GET_SVC_TYPE_FROM_CMD_BUF

  DESCRIPTION
    To obtain the service type from a given command buffer
    
  PARAMETERS
    cmd_buf   : pointer to the command buffer

  RETURN VALUE
    svc_type  : QMI service type

  DEPENDENCIES
    None

  SIDE EFFECTS
    Returns QMUX_SERVICE_MIN in case an error is encountered
===========================================================================*/
qmux_service_e_type qmi_svc_get_svc_type_from_cmd_buf
(
  qmi_cmd_buf_type * cmd_buf
);


/*===========================================================================
  FUNCTION QMI_SVC_PWRSAVE_STORE_SPEC

  DESCRIPTION
    Function to store the power save spec parameters in the QMUX instance.
  
  PARAMETERS
    service_handle  - qmux service handle returned by qmi_svc_reg_service()
    state_handle    - power save state handle
    svc_id          - the QMI service
    num_inds        - number of indications in list
    ind_list        - arrary of indications
    
  RETURN VALUE
    TRUE if power save spec successfully stored 
    FALSE otherwise
    
  DEPENDENCIES
    qmi_svc_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_svc_pwrsave_store_spec
(
  void                         * service_handle,
  qmi_svc_pwr_state_handle_type  state_handle,
  uint8                          svc_id,
  uint8                          num_inds,
  uint16                       * ind_list
);

/*===========================================================================
  FUNCTION QMI_SVC_SET_PWRSAVE_STATE
 
  DESCRIPTION
    Notify QMUX to update the power save state.  This may result in
    installation of new service indication filter.  If new state is
    normal/full power, filtering will be disabled.

  PARAMETERS  
    service_handle    -  qmux service handle returned by qmi_svc_reg_service()
    new_state_handle  -  New power save state
    old_state_handle  -  Pointer to previous power save state; output
  
  DEPENDENCIES
    qmi_svc_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    TRUE on successful operation
    FALSE on error condition

  SIDE EFFECTS
    Indication filtering may be modified
===========================================================================*/
extern boolean qmi_svc_set_pwrsave_state
(
  void                          * service_handle,
  qmi_svc_pwr_state_handle_type   new_state_handle,
  qmi_svc_pwr_state_handle_type * old_state_handle
);

/*===========================================================================
  FUNCTION QMI_SVC_GET_PWRSAVE_STATE

  DESCRIPTION
    Query QMUX for the current power save state.

  PARAMETERS  
    service_handle    -  qmux service handle returned by qmi_svc_reg_service()
    state_handle_ptr  -  Pointer to current power save state; output
  
  DEPENDENCIES
    qmi_svc_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    TRUE on successful operation
    FALSE on error condition

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_svc_get_pwrsave_state
(
  void                          * service_handle,
  qmi_svc_pwr_state_handle_type * state_handle_ptr
);


/*===========================================================================
  FUNCTION QMI_SVC_PWRSAVE_EVT_HDLR

  DESCRIPTION
    QMUX power save event handler (runs in DCC task)

  PARAMETERS
    state_handle   - Power save state handle
    data           - Context user data

  RETURN VALUE
    void

  SIDE EFFECTS
    none
===========================================================================*/
extern void  qmi_svc_pwrsave_evt_hdlr
(
  void*     state_handle,
  void*     data
);

/*=========================================================================
  FUNCTION QMI_SVC_PWR_SAVE_CBACK

  DESCRIPTION
    Notify QMI of power management state change.  
    Used to install Modem event indication filter previously registered 
    by control point.

  PARAMETERS
    qmi_instance   :  QMI instance
    new_state      :  New power save state
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    May change indication filter installed for QMI service(s). 
=========================================================================*/
extern void qmi_svc_pwr_save_cback
(
  uint8                          qmi_instance,
  qmi_svc_pwr_state_handle_type  new_state
);

/*===========================================================================
  FUNCTION QMI_SVC_FILTER_MESSAGE

  DESCRIPTION
    Applies current power save mode indication filter to specified
    message.  Returns boolean result on whether message should be
    dropped.

    When in power save mode, all responses are filtered to avoid
    waking up client processor.  Clients are expected to ensure there
    are no pending transactions before entering power save mode.
    
  PARAMETERS
    qmi_instance    - the QMI instance being used
    svd_id          - QMI service identifier
    msg_type        - whether msg is a cmd, response or indication
    ind_id          - indication identifier
    
  RETURN VALUE
    TRUE if message should be filtered
    FALSE if message should be sent
    
  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_svc_filter_message
(
  qmi_instance_e_type  qmi_instance,
  qmux_service_e_type  svc_id,
  uint8                msg_type,
  uint16               ind_id
);

/*===========================================================================
  FUNCTION QMI_SVC_DISPATCH_TRANSACTION()

  DESCRIPTION
    Dispatches all commands in a passed transaction.  This should be called
    after the recv function has unbundled the request.

  PARAMETERS
    sp  : service provided state pointer (user data)
    x_p : pointer to transaction containing commands to dispatch

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_xxx_init() must have been called previously
    (xxx depends on service type)

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_svc_dispatch_transaction
(
  qmi_common_svc_state_type *  svc_sp,
  qmi_transaction_type *  x_p
);

/*===========================================================================
  FUNCTION QMI_SVC_LEGACY_SVC_SEND()

  DESCRIPTION
    Send the message from Legacy service to QMUX layer

  PARAMETERS
    svci      : service info for the incoming request
    clid      : clinet ID where the request come from.
    sdu_out   : Raw QMUX SDU

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_legacy_svc_send
( 
  qmux_svc_info_type * svci,
  uint8                clid, 
  dsm_item_type *      sdu_in
);

/*===========================================================================
  FUNCTION QMI_SVC_PROCESS_LEGACY_SVC_REQ_CMD()

  DESCRIPTION
    Process the qmi request and route to service handling function in DCC task

  PARAMETERS
    cmd           : DCC cmd ID.
    user_data_ptr : payload to QMI Services running in DCC task.

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_process_legacy_svc_req_cmd
(
  dcc_cmd_enum_type cmd,        /* Actual command to be processed         */
  void *user_data_ptr 
);

/*===========================================================================
  FUNCTION QMI_LEGACY_SVC_ALLOC_CLID()

  DESCRIPTION
    Allocate a new Client ID to control point. This function is called by QMI_CTL
    service to allocate a client id and client ID allocation is asynchronous.
    On successfull allocation, the service sends a response to ctl by calling
    qmi_legacy_svc_send_alloc_clid_result.

  PARAMETERS
    common_hdr : legacy service common msg hdr type.

  RETURN VALUE
    None.

  DEPENDENCIES
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
void  qmi_legacy_svc_alloc_clid
( 
  void * qmux_svc_handle,
  uint8  qmux_type,
  uint8  clid,
  uint8  tx_id
);

/*===========================================================================
  FUNCTION QMI_LEGACY_SVC_DEALLOC_CLIID()

  DESCRIPTION
    Handle the release client ID message for QMI Legacy Service.

    Dispatches a request to the specified service to deallocate a client ID.
    Returns the result of the operation to the issuer.

  PARAMETERS
    qmux_svc_handle: Service Handle
    qmux_type: Service 
    clid:    corresponding client id
    tx_id: Transaction ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_legacy_svc_dealloc_clid
(
  void * qmux_svc_handle,
  uint8  qmux_type,
  uint8  clid,
  uint8  tx_id
);

/*===========================================================================
  FUNCTION QMI_LEGACY_SVC_QMI_LINK_CLOSED()

  DESCRIPTION
    Free all existing Legacy service clients, as QMI link is closed.

  PARAMETERS
    instance - the QMI instance whose link was disconnected

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    Closes all QMI application state for Legacy Services
===========================================================================*/
void  qmi_legacy_svc_qmi_link_closed
(
  qmi_instance_e_type  instance
);
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
/*===========================================================================
  FUNCTION QMI_SVC_PWRSAVE_STORE_RESPONSE_REPORTING

  DESCRIPTION
    Function to store the power save spec response reporting
    parameters in the QMUX instance.
  
  PARAMETERS
    service_handle      - qmux service handle returned by qmux_reg_service()
    state_handle        - power save state handle
    response_reporting  - flag which checks if responses will be reported
                         by the modem during Power Save Mode
 
  RETURN VALUE
    TRUE if power save spec successfully stored response reporting,
    FALSE otherwise
    
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_pwrsave_store_response_reporting
(
  void                         * service_handle,
  qmi_svc_pwr_state_handle_type  state_handle,
  boolean                        response_reporting
);

/*===========================================================================
  FUNCTION QMI_SVC_GET_RESPONSE_REPORTING_IN_CONFIGURED_PWRSAVE_MODES

  DESCRIPTION
    Function to query for the list of all configured power save mode spec's
    with their corresponsding response reporting parameters in the
    QMUX instance.
  
  PARAMETERS
    service_handle  - qmux service handle returned by qmux_reg_service()
    reporting_opt   - pointer to the structure to store the power save spec
                      and response_reporting list
    num_pwr_states -  number of valid configured power save states stored
                      in the QMUX instance
 
  RETURN VALUE
    TRUE if power save spec successfully stored response reporting,
    FALSE otherwise
    
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_get_response_reporting_in_configured_pwrsave_modes
(
  void                         * service_handle,
  void                         * reporting_opt,
  uint8                        * num_pwr_states
);

/*===========================================================================
  FUNCTION QMI_SVC_GET_RESPONSE_REPORTING_IN_CURRENT_PWRSAVE_MODE

  DESCRIPTION
    Function to get the power save spec response reporting parameter of the
    current power save state in the QMUX instance.
  
  PARAMETERS
    service_handle  - qmux service handle returned by qmux_reg_service()
    response_reporting - flag stored which checks if responses will be reported
                         by the modem during Power Save Mode
 
  RETURN VALUE
    TRUE if power save spec response reporting is successfully returned,
    FALSE otherwise
    
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_get_response_reporting_in_current_pwrsave_mode
(
  void                         * service_handle,
  boolean                      * reponse_reporting
);
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/

/*===========================================================================
  FUNCTION QMI_SVC_PUT_PARAM_TLV2()

  DESCRIPTION
    Construct a TLV with the given input type, length and value

  PARAMETERS
    pkt   : message
    type  : value to be put in type field
    len   : value to be put in length field
    value : contents of the value field of TLV
    file_id : indicated file_id of the caller function.
    line  :indicated line number of file.
  RETURN VALUE
    TRUE
    FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_svc_put_param_tlv2
(
  dsm_item_type **  pkt,
  uint8             type,
  uint16            len,
  void *            value,
  uint8             file_id,
  unsigned int      line
);

/*==========================================================================
FUNCTION QMI_SVC_VALIDATE_TYPE_LEGTH()

DESCRIPTION
  This function validates the length based on passed expected and length params.

PARAMETERS
  type - param type
  length - recieved length of specific value.
  expected_len - expected length of value.
  file_id - specifies file id of the caller.
  line - line number of the caller.
  
RETURN VALUE
  returns true if both lengths are equal.
  
DEPENDENCIES
  none

SIDE EFFECTS
  None

===========================================================================*/
boolean qmi_svc_validate_type_length
(
  uint8 type,
  uint32 length,
  uint32 expected_len,
  uint8 file_id,
  uint16 line
);

/*===========================================================================
  FUNCTION QMI_SVC_IDL_MESSAGE_DECODE()

  DESCRIPTION
    Decode QMI SVC message into c structure from incoming dsm item

  PARAMETERS
    svc         The QMI IDL service object.
    message_id  QMI message id
    msg_ptr     Pointer to dsm item that contains TLVs 
    p_req       Pointer to C structure to contain decoded message
    p_req_size  Size of C structure to contain decoded data

  RETURN VALUE
    QMI_ERR_NONE_V01 is returned if decode is successful, otherwise relevant error is returned

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01  qmi_svc_idl_message_decode
 ( 
   qmi_idl_service_object_type svc, 
   uint16_t                    message_id, 
   dsm_item_type**             msg_ptr, 
   void *                      p_req, 
   uint32_t                    p_req_size
 );
 
/*===========================================================================
  FUNCTION QMI_SVC_IDL_MESSAGE_ENCODE()

  DESCRIPTION
    Encode QMI SVC message into dsm item from incoming c structure

  PARAMETERS
    svc          The QMI IDL service object.
    type         QMI IDL type ( QMI_IDL_RESPONSE or QMI_IDL_INDICATION )
    message_id   QMI message id
    p_src        Pointer to C structure containing message data
    p_src_size   Size of C structure that contains message data
    msg_ptr      Address of pointer to dsm item for storing encoded message data

  RETURN VALUE
    TRUE if success, FALSE if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_idl_message_encode
( 
  qmi_idl_service_object_type  svc, 
  qmi_idl_type_of_message_type type, 
  uint16_t                     message_id, 
  void*                        p_src, 
  uint32_t                     p_src_size, 
  dsm_item_type**              p_response 
);

#endif /* _DS_QMI_SVC_H */
