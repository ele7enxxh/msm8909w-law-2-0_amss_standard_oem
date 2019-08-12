/*===========================================================================

                         D S _ Q M I _ A T P . C

DESCRIPTION

  The Data Services QMI Application Traffic Pairing service source file.
QMI_ATP provides commands that deal with the ATP Proxy on the modem. 

ATP service provides tools to operators to differentiate subscription offers
and introduce new ways to monetize the increased data demand.

EXTERNALIZED FUNCTIONS

  qmi_atp_init()
    Initialize the QMI ATP service

Copyright (c) 2013-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_atp.c#1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/13/13    svj    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"
#include "dcc_task_defs.h"
#include "dcc_task_svc.h"

#include "ds_qmi_atp.h"
#include "ds_atp_api.h"
#ifdef FEATURE_DATA_ATP
  #include "application_traffic_pairing_v01.h"
#endif /* FEATURE_DATA_ATP */
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"

#include "qmi_idl_lib.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "mqcsi_log.h"
#include "common_v01.h"
#include "qmi_sap.h"
#include "queue.h"
#include "qmi_framework.h"
#include "stringl/stringl.h"


/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define ATP_MAX_CLIDS    (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)
#define ATP_INVALID_INSTANCE    -1

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for ATP
---------------------------------------------------------------------------*/
#define ATP_BASE_VER_MAJOR    (1)
#define ATP_BASE_VER_MINOR    (1)

#define ATP_ADDENDUM_VER_MAJOR  (0)
#define ATP_ADDENDUM_VER_MINOR  (0)

#define ATP_MAX_IPV6_PREFIX_LENGTH           128
#define ATP_RESERVED_CLIENT_ID               0xFFFF
#define ATP_MAX_POLICY_INFO_PER_IND          1
/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

#define ATPI_QCSI_OS_SIGNAL ((rex_sigs_type)(1 << DCC_ATP_RDR_SIGNAL))

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/* Filter report status report id list */
typedef struct qmi_atpi_filter_report_status_s
{
  uint32 report_id;
  struct qmi_atpi_filter_report_status_s * next;
} qmi_atpi_filter_report_status_list_type;

/*---------------------------------------------------------------------------
  QMI ATP Client state info
---------------------------------------------------------------------------*/
typedef struct {
  q_link_type                  link; /*< Data Structure required by the queue 
                                         library to add this as part of a 
                                         queue */
  qmi_client_handle clnt;
   
  struct
  {
    boolean     report_activation_status;
    boolean     report_policy_info;
  } report_status;

  /*Unique id for every policy info indication sent */
  uint32 policy_info_list_id;

  /* Filter report list and count  */
  qmi_atpi_filter_report_status_list_type *  filter_list;
  uint8                                      num_filter_list;
}qmi_atpi_client_info_type;

/*---------------------------------------------------------------------------
  QMI ATP Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  q_type                 client_q; /*< Information about all the clients that
                                       have connected to the service. This is 
                                       queue of qmi_atpi_client_info_type */
  qmi_csi_service_handle service_handle;
  qmi_sap_client_handle  sap_client_handle;
  uint16                 num_of_clients;
} qmi_atpi_state_info_type;

typedef enum
{
  QMI_ATPI_CMD_MIN_VAL = -1,	
  QMI_ATPI_ACT_STATUS_CHANGE_EVT = 0,
  QMI_ATPI_POLICY_INFO_CHANGE_EVT,
  QMI_ATPI_FILTER_REPORT_STATUS_EVT,
  QMI_ATPI_CMD_MAX_VAL = 0x7FFFFFFF
} qmi_atpi_cmd_type;

#ifdef FEATURE_DATA_ATP
static qmi_atpi_state_info_type      qmi_atp_state;
static qmi_csi_os_params             os_params;
static boolean                       qmi_atpi_inited = FALSE;


/* This is the default QMI_ATP cmd buffer. Make sure that it does not grow
 * beyond 512 bytes. In case you need a command buffer larger than 512 bytes,
 * declare a separate structure. */
typedef struct
{
  uint16  cmd_id; /* qmi_cmd_id_e_type */
  union
  {
    atp_activation_status_ind_msg_v01      act_status;
    struct
    { 
      uint32                                  cl_id;
      ds_atp_policy_info_type               * policy_info_ptr;
    }policy_tuple;
    atp_send_filter_report_ind_msg_v01     filter_report_status;
  } data; 
} qmi_atpi_cmd_buf_type;



/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

static boolean qmi_atpi_msg_hdlr_cb
(
  dcc_sig_enum_type  sig,
  void              *svc_ptr
);

static qmi_csi_cb_error qmi_atpi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmi_atpi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_atpi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);

static void qmi_atpi_process_cmd
(
  dcc_cmd_enum_type  cmd,
  void              *cmd_ptr
);

static void *qmi_atpi_get_cmd_buf
(
  qmi_atpi_cmd_type cmd_id
);

static qmi_csi_cb_error  qmi_atpi_indication_register(
  qmi_atpi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_atpi_activation_status_query(
  qmi_atpi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_atpi_policy_info_query(
  qmi_atpi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_atpi_send_filter_report(
  qmi_atpi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static void atpi_act_status_change_cb
(
  ds_atp_proxy_event_enum_type  event,
  void                          *event_info_ptr,
  void                          *user_data_ptr
);

static void atpi_policy_info_change_cb
(
  ds_atp_proxy_event_enum_type  event,
  void                          *event_info_ptr,
  void                          *user_data_ptr
);

static void atpi_filter_report_status_cb
(
  ds_atp_proxy_event_enum_type  event,
  void                          *event_info_ptr,
  void                          *user_data_ptr
);

static void qmi_atpi_generate_act_status_ind
(
  atp_activation_status_ind_msg_v01  act_status
);

static void qmi_atpi_act_status_ind
(
  void                                  * client,
  atp_activation_status_ind_msg_v01     * act_status
);

static void qmi_atpi_generate_filter_report_ind
(
  atp_send_filter_report_ind_msg_v01  filter_report
);

static void qmi_atpi_filter_report_status_ind
(
  void                                  * client,
  atp_send_filter_report_ind_msg_v01    * filter_report
);

static void qmi_atpi_generate_policy_info_ind
(
  uint32      cl_id,
  ds_atp_policy_info_type  *policy_info
);

static void qmi_atpi_policy_info_ind
(
  void                                  * client,
  ds_atp_policy_info_type               * policy_info
);
/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/


/* Define service handle table for ATP messages */
static qmi_csi_cb_error (* const req_handle_table[])
(
 qmi_atpi_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
) =
{
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x00 - 0x07*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x08 - 0x0F*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x10 - 0x17*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x18 - 0x1F*/
  qmi_atpi_indication_register,            /* Request handler for message ID 0x20 */
  qmi_atpi_activation_status_query,        /* Request handler for message ID 0x21 */
  NULL,                                    /* Request handler for message ID 0x22 */
  qmi_atpi_policy_info_query,              /* Request handler for message ID 0x23 */
  NULL,                                    /* Request handler for message ID 0x24 */
  qmi_atpi_send_filter_report              /* Request handler for message ID 0x25 */ 
};

extern rex_tcb_type dcc_tcb;

// Comparison function for Client handle
static int atpi_compare_qmi_client_handle
( 
  void *item_ptr,            // Queue Item
  void *compare_val          // Comparison value
)
{
  qmi_atpi_client_info_type *clnt_info = (qmi_atpi_client_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
}

// Comparison function for connection info
static int atpi_compare_connection_info_handle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ( (p_ItemPtr == p_CompareVal) ? 1 : 0 );
}

/*===========================================================================

                       EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_ATP_INIT()

  DESCRIPTION
    Registers the ATP service with QCSI 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_atp_init
(
   void
)
{
  qmi_csi_error                      rc = QMI_CSI_NO_ERR;
  qmi_sap_error                      src = QMI_SAP_NO_ERR;
  ds_atp_proxy_err_enum_type         result = DS_ATP_PROXY_ERR_NONE;
  qmi_csi_options                    options;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_1("QMI ATP service init %d", qmi_atpi_inited);

  if (!qmi_atpi_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)dcc_set_sig_handler(DCC_ATP_RDR_SIGNAL, 
                              qmi_atpi_msg_hdlr_cb, &qmi_atp_state);
    dcc_enable_sig(DCC_ATP_RDR_SIGNAL);

    /* Set the signal to init again */
    DCC_SET_SIGNAL(DCC_ATP_RDR_SIGNAL);
    return;
  }

  os_params.tcb = &dcc_tcb;
  os_params.sig = ATPI_QCSI_OS_SIGNAL;
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);

  /*-------------------------------------------------------------------------
    Register QMI ATP service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register_with_options(atp_get_service_object_v01(),
                        qmi_atpi_svc_connect_cb,
                        qmi_atpi_svc_disconnect_cb, 
                        qmi_atpi_handle_client_req_cb, 
                        &qmi_atp_state, &os_params, 
                        &options,
                        &qmi_atp_state.service_handle); 
 
  if(rc != QMI_CSI_NO_ERR)
  {
    ERR_FATAL("Unable to register ATP service! Error %d", rc, 0, 0);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO1_1("QMI ATP service registered with QMI Framework, rc = %d ",
                rc);

  //register with SAP
  src = qmi_sap_register(atp_get_service_object_v01(), NULL, 
                         &qmi_atp_state.sap_client_handle);
  if (src != QMI_SAP_NO_ERR)
  {
    ERR_FATAL("Unable to register ATP service with QSAP! Error %d", src, 0, 0);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO1_0("Registering ATP events");
  /*-------------------------------------------------------------------------
    Register at power up for interested Events.
  -------------------------------------------------------------------------*/
  result = ds_atp_proxy_event_reg(DS_ATP_PROXY_EVENT_ACTIVE_STATUS_IND,
                                  DS_ATP_PROXY_EVENT_ACTIVE_STATUS_IND,
                                  atpi_act_status_change_cb,
                                  NULL); 

  if(result < 0)
  {
    LOG_MSG_ERROR_1("Couldn't reg for ATP activation status Err : (%d)",result);
    ASSERT(0);
    return;
  }

  result = ds_atp_proxy_event_reg(DS_ATP_PROXY_EVENT_POLICY_UPDATED_IND,
                                  DS_ATP_PROXY_EVENT_POLICY_UPDATED_IND,
                                  atpi_policy_info_change_cb,
                                  NULL); 

  if(result < 0)
  {
    LOG_MSG_ERROR_1("Couldn't reg for ATP policy info Err: (%d)",result);
    ASSERT(0);
    return;
  }

  result = ds_atp_proxy_event_reg(DS_ATP_PROXY_EVENT_FILTER_REPORT_ACK_IND,
                                  DS_ATP_PROXY_EVENT_FILTER_REPORT_ACK_IND,
                                  atpi_filter_report_status_cb,
                                  NULL); 

  if(result < 0)
  {
    LOG_MSG_ERROR_1("Couldn't reg for filter report status Err:(%d)",result);
    ASSERT(0);
    return;
  }

  /* Initialize the queue which will hold all handles of clients */
  (void) q_init(&qmi_atp_state.client_q);
  qmi_atp_state.num_of_clients = 0;

  /*-------------------------------------------------------------------------
    Setting QMI ATP service command handler with DCC task process
  -------------------------------------------------------------------------*/
  dcc_set_cmd_handler(DCC_ATP_SVC_CMD, qmi_atpi_process_cmd);
}

static boolean qmi_atpi_msg_hdlr_cb
(
  dcc_sig_enum_type  sig,
  void              *svc_ptr
)
{
  qmi_atpi_state_info_type *atp_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(sig == DCC_ATP_RDR_SIGNAL);
  ASSERT(svc_ptr);

  if (!qmi_atpi_inited)
  {
    qmi_atpi_inited = TRUE;
    /* Call init again to register with QCSI now
     * Wait for all tasks to be inited */
    qmi_atp_init();
    return TRUE;
  }

  atp_sp = (qmi_atpi_state_info_type*) svc_ptr;

  qmi_csi_handle_event(atp_sp->service_handle, &os_params);
  return TRUE;
} /* qmi_atpi_msg_hdlr_cb */

static qmi_csi_cb_error qmi_atpi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_atpi_client_info_type    *clnt_info_ptr;
  qmi_atpi_state_info_type     *atp_svc_ptr;
  qmi_csi_cb_error              rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT(connection_handle);
  ASSERT(service_handle);

  atp_svc_ptr = (qmi_atpi_state_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &atp_svc_ptr->client_q,
                                atpi_compare_qmi_client_handle,
                                client_handle ) )
  {
    LOG_MSG_ERROR_1("atpi_svc_connect_cb: ATP client 0x%p present",
                  client_handle);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  if (ATP_MAX_CLIDS == atp_svc_ptr->num_of_clients)
  {
    LOG_MSG_ERROR_1("atpi_svc_connect_cb: Maximum number of clients already "
                  "allocated %d", ATP_MAX_CLIDS);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  clnt_info_ptr = ps_system_heap_mem_alloc(
                    sizeof(qmi_atpi_client_info_type));
  if(!clnt_info_ptr)
  {
    LOG_MSG_ERROR_0("atpi_svc_connect_cb: Out of mem");
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  memset(clnt_info_ptr, 0, sizeof(qmi_atpi_client_info_type));
  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;
  /* Add it to the queue of clients */
  q_put( &(atp_svc_ptr->client_q), &(clnt_info_ptr->link) );
  atp_svc_ptr->num_of_clients++;

  *connection_handle = clnt_info_ptr;
  LOG_MSG_INFO1_1("atpi_svc_connect_cb: Alloc client 0x%p", 
                clnt_info_ptr);
  rc = QMI_CSI_CB_NO_ERR;
bail:
  return rc;
} /* qmi_atpi_svc_connect_cb() */

static void qmi_atpi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_atpi_state_info_type     *atp_svc_ptr;
  qmi_atpi_client_info_type    *clnt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle);
  ASSERT(service_handle);
  atp_svc_ptr = (qmi_atpi_state_info_type *)service_handle;

  LOG_MSG_INFO1_1("atpi_svc_disconnect_cb: Releasing client 0x%p", 
                   connection_handle);

  clnt_info_ptr = q_linear_search( &atp_svc_ptr->client_q,
                                   atpi_compare_connection_info_handle,
                                   connection_handle );
  if (NULL == clnt_info_ptr)
  {
    LOG_MSG_ERROR_1("atpi_svc_disconnect_cb: ATP client 0x%p not found",
                  connection_handle);
    return;

  }

  if ( TRUE != q_delete_ext( &atp_svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
  {
    LOG_MSG_ERROR_1("ATP client 0x%p could not be deleted from queue",
                  clnt_info_ptr);
    return;
  }
  PS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  atp_svc_ptr->num_of_clients--;
} /* qmi_atpi_svc_disconnect_cb() */


static qmi_csi_cb_error qmi_atpi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
)
{
  qmi_csi_cb_error               rc;
  qmi_atpi_client_info_type     *clnt_info_ptr;
  qmi_atpi_state_info_type      *atp_svc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle);
  ASSERT(service_handle);

  rc = QMI_CSI_CB_INTERNAL_ERR;
  atp_svc_ptr = (qmi_atpi_state_info_type*) service_handle;

  clnt_info_ptr = q_linear_search( &atp_svc_ptr->client_q,
                                   atpi_compare_connection_info_handle,
                                   connection_handle );
  if (NULL == clnt_info_ptr)
  {
    LOG_MSG_ERROR_1("atpi_handle_req_cb: Invalid clnt 0x%p",
                  connection_handle);
    return rc;
  }

  if(msg_id < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
  {
    if(req_handle_table[msg_id])
    {
      rc = req_handle_table[msg_id] (clnt_info_ptr, req_handle, msg_id,
                                     req_c_struct, req_c_struct_len,
                                     service_handle);
    }
    else
    {
      LOG_MSG_ERROR_1("atpi_handle_req_cb: NULL message ID handler: %d",
                    msg_id);
    }
  }
  else
  {
    LOG_MSG_ERROR_1("atpi_handle_req_cb: Invalid message ID: %d",
                   msg_id);
  }

  return rc;
} /* qmi_atpi_handle_client_req_cb() */


/*===========================================================================
  FUNCTION QMI_ATPI_GET_CMD_BUF()

  DESCRIPTION
    Allocate and assign a QMI ATP command buffer from the PS MEM heap
    based on the QMI CMD type

  PARAMETERS
    cmd_id - QMI command type

  RETURN VALUE
    cmd_buf_ptr - Pointer to the allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void *qmi_atpi_get_cmd_buf
(
  qmi_atpi_cmd_type cmd_id
)
{
  switch (cmd_id)
  {
     case QMI_ATPI_ACT_STATUS_CHANGE_EVT:
       {
         qmi_atpi_cmd_buf_type *cmd_buf_ptr = NULL;
         cmd_buf_ptr = ps_system_heap_mem_alloc(sizeof(qmi_atpi_cmd_buf_type));
         return ((void*)cmd_buf_ptr);
       }
   
     case QMI_ATPI_POLICY_INFO_CHANGE_EVT:
       {
         qmi_atpi_cmd_buf_type *cmd_buf_ptr = NULL;
         cmd_buf_ptr = ps_system_heap_mem_alloc(sizeof(qmi_atpi_cmd_buf_type));
         return ((void*)cmd_buf_ptr);
       }

     case QMI_ATPI_FILTER_REPORT_STATUS_EVT:
       {
         qmi_atpi_cmd_buf_type *cmd_buf_ptr = NULL;
         cmd_buf_ptr = ps_system_heap_mem_alloc(sizeof(qmi_atpi_cmd_buf_type));
         return ((void*)cmd_buf_ptr);
       }
     
    default:
      LOG_MSG_ERROR_1("Unknown QMI ATP command %d",(int32)cmd_id);
  }
  return NULL;
} /* qmi_atpi_get_cmd_buf */


/*===========================================================================
  FUNCTION qmi_atpi_process_cmd()

  DESCRIPTION
    This function processes a QMI ATP command

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd:      DCC cmd to be processed
    cmd_ptr:  private data buffer containing the QMI ATP command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI ATP must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_atpi_process_cmd
(
  dcc_cmd_enum_type  cmd,
  void              *cmd_ptr
)
{
  qmi_atpi_cmd_buf_type *cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);
  cmd_buf_ptr = (qmi_atpi_cmd_buf_type *)cmd_ptr;

  LOG_MSG_INFO1_2("qmi_atpi_process_cmd: cmdbuf's cmd_id:%d  cmd %d", 
                 cmd_buf_ptr->cmd_id, cmd);

  switch(cmd_buf_ptr->cmd_id)
  {
    case QMI_ATPI_ACT_STATUS_CHANGE_EVT:
      qmi_atpi_generate_act_status_ind
      (
        cmd_buf_ptr->data.act_status
      );
      break;

     case QMI_ATPI_POLICY_INFO_CHANGE_EVT:
      qmi_atpi_generate_policy_info_ind
      (
        cmd_buf_ptr->data.policy_tuple.cl_id,
        cmd_buf_ptr->data.policy_tuple.policy_info_ptr
      );
      break;

    case QMI_ATPI_FILTER_REPORT_STATUS_EVT:
      qmi_atpi_generate_filter_report_ind
      (
        cmd_buf_ptr->data.filter_report_status
      );
      break;

    default:
      LOG_MSG_ERROR_1("Unexpected cmd(%d) recd in QMI ATP cmd handler",
                    cmd_buf_ptr->cmd_id);
      ASSERT(0);
      
  }

  PS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
  return;
} /* qmi_atpi_process_cmd() */

/*===========================================================================
  FUNCTION QMI_ATPI_ACTIVATION_STATUS_QUERY()

  DESCRIPTION
    Queries for the activation status

  PARAMETERS
    clnt_info          : Client info pointer
    req_handle         : Request handle
    msg_id             : Message Id
    req_c_struct       : Incoming request 
    req_c_struct_len   : incoming request length
    service_handle     : Service handle

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_atp_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_atpi_activation_status_query
(
  qmi_atpi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_atpi_state_info_type                *atp_sp;
  qmi_csi_cb_error                         rc;
  atp_activation_status_query_req_msg_v01 *req_ptr;
  atp_activation_status_query_resp_msg_v01 resp_msg;
  ds_atp_active_status_type                act_status;
  ds_atp_proxy_err_enum_type               atp_errno = DS_ATP_PROXY_ERR_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  atp_sp = (qmi_atpi_state_info_type *)service_handle;
  req_ptr = (atp_activation_status_query_req_msg_v01 *)req_c_struct;
  rc = QMI_CSI_CB_INTERNAL_ERR;

  LOG_MSG_INFO1_1("Client 0x%p querying ATP activation status",
                  clnt_info);

  memset(&resp_msg, 0, sizeof(atp_activation_status_query_resp_msg_v01));
  memset(&act_status, 0, sizeof(ds_atp_active_status_type));

  atp_errno = ds_atp_proxy_service_req(
     DS_ATP_PROXY_SERVICE_ACTIVE_STATUS_QUERY_REQ,(void *)&act_status); 

  if (DS_ATP_PROXY_ERR_NONE != atp_errno)
  {
    LOG_MSG_ERROR_1("qmi_atpi_activation_status_query: Error %d",
                   atp_errno);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(atp_activation_status_query_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  LOG_MSG_INFO1_1("ATP act status %d ",act_status.active_status);

  resp_msg.is_activated_valid        = TRUE;
  resp_msg.is_activated              = act_status.active_status;
  resp_msg.resp.result               = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error                = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(atp_activation_status_query_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_atpi_activation_status_query() */

/*===========================================================================
  FUNCTION QMI_ATPI_POLICY_INFO_QUERY()

  DESCRIPTION
    Queries for the ATP policy information

  PARAMETERS
    clnt_info          : Client info pointer
    req_handle         : Request handle
    msg_id             : Message Id
    req_c_struct       : Incoming request 
    req_c_struct_len   : incoming request length
    service_handle     : Service handle

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_atp_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_atpi_policy_info_query
(
  qmi_atpi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_atpi_state_info_type                *atp_sp;
  qmi_csi_cb_error                         rc;
  atp_policy_info_req_msg_v01             *req_ptr;
  atp_policy_info_resp_msg_v01             resp_msg;
  ds_atp_proxy_err_enum_type               atp_errno = DS_ATP_PROXY_ERR_NONE;
  ds_atp_policy_info_type                 * atp_policy;
  qmi_atpi_cmd_buf_type                   * cmd_ptr = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  atp_sp = (qmi_atpi_state_info_type *)service_handle;
  req_ptr = (atp_policy_info_req_msg_v01 *)req_c_struct;
  rc = QMI_CSI_CB_INTERNAL_ERR;

  LOG_MSG_INFO1_1("Client 0x%p querying ATP policy info", clnt_info);

  memset(&resp_msg, 0, sizeof(atp_policy_info_resp_msg_v01));

  atp_policy = ps_system_heap_mem_alloc(sizeof(ds_atp_policy_info_type));

  if( atp_policy == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI ATP policy buf");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error  = QMI_ERR_NO_MEMORY_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(atp_policy_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(atp_policy, 0, sizeof(ds_atp_policy_info_type));

  atp_errno = ds_atp_proxy_service_req(DS_ATP_PROXY_SERVICE_POLICY_QUERY_REQ,
                                       (void *)atp_policy); 

  if (DS_ATP_PROXY_ERR_NONE != atp_errno)
  {
    LOG_MSG_ERROR_1("qmi_atpi_policy_info_query: Error %d",atp_errno);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    if (DS_ATP_PROXY_ERR_EVENT_NOT_SUPP == atp_errno)
    {
      resp_msg.resp.error = QMI_ERR_INFO_UNAVAILABLE_V01;
    }
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(atp_policy_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

//Copy info and send DCC cmd for policy info ind
  
  cmd_ptr = (qmi_atpi_cmd_buf_type *)
              qmi_atpi_get_cmd_buf(QMI_ATPI_POLICY_INFO_CHANGE_EVT);

  if( cmd_ptr == NULL )
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI ATP cmd buf");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NO_MEMORY_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(atp_policy_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  memset(cmd_ptr, 0, sizeof(qmi_atpi_cmd_buf_type));

  cmd_ptr->data.policy_tuple.cl_id = (uint32) clnt_info;

  if (atp_policy == NULL)
  {
    LOG_MSG_ERROR_0("NULL ATP policy returned ");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(atp_policy_info_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  cmd_ptr->data.policy_tuple.policy_info_ptr = atp_policy;
  cmd_ptr->cmd_id = QMI_ATPI_POLICY_INFO_CHANGE_EVT;
  dcc_send_cmd_ex (DCC_ATP_SVC_CMD, cmd_ptr);

  LOG_MSG_INFO1_1("ATP policy info ptr 0x%p ", atp_policy);

  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(atp_policy_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_atpi_policy_info_query() */

/*===========================================================================
  FUNCTION QMI_ATPI_SEND_FILTER_REPORT()

  DESCRIPTION
    Send the filter report request generated by the AP to network

  PARAMETERS
    clnt_info          : Client info pointer
    req_handle         : Request handle
    msg_id             : Message Id
    req_c_struct       : Incoming request 
    req_c_struct_len   : incoming request length
    service_handle     : Service handle

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_atp_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_atpi_send_filter_report
(
  qmi_atpi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  qmi_atpi_state_info_type                *atp_sp;
  qmi_csi_cb_error                         rc;
  atp_send_filter_report_req_msg_v01      *req_ptr;
  atp_send_filter_report_resp_msg_v01      resp_msg;
  ds_atp_proxy_err_enum_type               atp_errno = DS_ATP_PROXY_ERR_NONE;
  ds_atp_filter_report_type                atp_filter_report;
  qmi_atpi_filter_report_status_list_type  *new_filter;
  uint32 name_len = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct);
  ASSERT(req_c_struct_len > 0);
  ASSERT(clnt_info);
  ASSERT(service_handle);

  atp_sp = (qmi_atpi_state_info_type *)service_handle;
  req_ptr = (atp_send_filter_report_req_msg_v01 *)req_c_struct;
  rc = QMI_CSI_CB_INTERNAL_ERR;

  LOG_MSG_INFO1_1("Client 0x%p requesting filter report ", clnt_info);

  memset(&resp_msg, 0, sizeof(atp_send_filter_report_resp_msg_v01));
  memset(&atp_filter_report, 0, sizeof(ds_atp_filter_report_type));
  
  /*---------------------------------------------------------------------
    Validate some input TLVs
  ---------------------------------------------------------------------*/

  if (!req_ptr->report_id)
  {
    LOG_MSG_ERROR_0("Report ID not present in filter request ");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(atp_send_filter_report_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  else
  {
    atp_filter_report.report_id = req_ptr->report_id;
  }

  if (req_ptr->action_valid)
  {
    atp_filter_report.filter_report.action = req_ptr->action;
    if (atp_filter_report.filter_report.action < DS_ATP_FILTER_ACTION_MIN &&
        atp_filter_report.filter_report.action > DS_ATP_FILTER_ACTION_MAX)
    {
      LOG_MSG_ERROR_1("Invalid filter report action %d ", req_ptr->action);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                        sizeof(atp_send_filter_report_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
  }

  if (req_ptr->service_id_valid)
  {
    atp_filter_report.filter_report.service_id = req_ptr->service_id;
  }

  if (req_ptr->package_name_valid)
  {
    name_len = (uint32)strlen(req_ptr->package_name);
    if (name_len > DS_ATP_MAX_PACKAGE_NAME_LEN)
    {
      LOG_MSG_ERROR_1("Pkg name too long. Len %d ",name_len);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                        sizeof(atp_send_filter_report_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
    atp_filter_report.filter_report.package_name_len = (uint8)name_len;
    strlcpy(atp_filter_report.filter_report.package_name, req_ptr->package_name, 
            strlen(req_ptr->package_name) + 1);
    
  }

  if (req_ptr->hash_values_valid)
  {
    if (req_ptr->hash_values_len > DS_ATP_MAX_NUM_HASH_VALUE)
    {
      LOG_MSG_ERROR_1("Too many hashes %d ", 
                    req_ptr->hash_values_len);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                        sizeof(atp_send_filter_report_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }

    memscpy(atp_filter_report.filter_report.hash_values, 
            sizeof(atp_filter_report.filter_report.hash_values),
            req_ptr->hash_values, sizeof(req_ptr->hash_values));
		atp_filter_report.filter_report.hash_num = req_ptr->hash_values_len;

  }

  if (req_ptr->ip_type_valid)
  {
    if ((req_ptr->ip_type != ATP_FILTER_ADDR_V4_V01) &&
        (req_ptr->ip_type != ATP_FILTER_ADDR_V6_V01))
    {
      LOG_MSG_ERROR_1("Invalid IP type %d ", 
                    req_ptr->ip_type);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                        sizeof(atp_send_filter_report_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }

    if (req_ptr->ip_type == ATP_FILTER_ADDR_V4_V01 ) 
    {
      if(req_ptr->dest_ipv6_addr_valid || req_ptr->src_ipv6_addr_valid )
      {
        LOG_MSG_ERROR_0(" Mismatch in IP parameters ");
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                          sizeof(atp_send_filter_report_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      }
      else
      {
        // Copy IPV4 paramters
        atp_filter_report.filter_report.tuple_info.ip_type = req_ptr->ip_type;
        if (req_ptr->dest_ipv4_addr_valid)
        {
          memscpy((void *)&atp_filter_report.filter_report.tuple_info.dest_ip.dest_ipv4_addr,
                  sizeof(atp_filter_report.filter_report.tuple_info.dest_ip.dest_ipv4_addr),
                  req_ptr->dest_ipv4_addr, sizeof(req_ptr->dest_ipv4_addr));
          
        }
        else
        {
	       	LOG_MSG_INFO1_0("No IPv4 dest! Ignoring!!! ");  
        }
        if (req_ptr->src_ipv4_addr_valid)
        {
          memscpy((void *)&atp_filter_report.filter_report.tuple_info.src_ip.src_ipv4_addr,
                  sizeof(atp_filter_report.filter_report.tuple_info.src_ip.src_ipv4_addr),
                  req_ptr->src_ipv4_addr, sizeof(req_ptr->src_ipv4_addr));
       
        }
        else
        {
	       	LOG_MSG_INFO1_0("No IPv4 src! Ignoring!!! ");  
        }
      }
    } //IPV4 filter

    if (req_ptr->ip_type == ATP_FILTER_ADDR_V6_V01)
    {  
      if(( req_ptr->dest_ipv4_addr_valid || req_ptr->src_ipv4_addr_valid)) 
      {
        LOG_MSG_ERROR_0(" Mismatch in IP parameters ");
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
        qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                          sizeof(atp_send_filter_report_resp_msg_v01));
        return QMI_CSI_CB_NO_ERR;
      }
      else
      {
        // Copy IPV6 paramters
        atp_filter_report.filter_report.tuple_info.ip_type = req_ptr->ip_type;
        if (req_ptr->dest_ipv6_addr_valid)
        {
					if ( req_ptr->dest_ipv6_addr.dest_ipv6_prefix_length > ATP_MAX_IPV6_PREFIX_LENGTH)
					{
						LOG_MSG_ERROR_1("Invalid IPV6 prefix length Dest %d", 
													req_ptr->dest_ipv6_addr.dest_ipv6_prefix_length);
						resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
						resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
						qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
														sizeof(atp_send_filter_report_resp_msg_v01));
						return QMI_CSI_CB_NO_ERR;
					}
          memscpy(atp_filter_report.filter_report.tuple_info.dest_ip.dest_ipv6_addr,
                  sizeof(atp_filter_report.filter_report.tuple_info.dest_ip.dest_ipv6_addr),
                  req_ptr->dest_ipv6_addr.dest_ipv6_addr, sizeof(req_ptr->dest_ipv6_addr.dest_ipv6_addr));
          
        }
        else
        {
           LOG_MSG_INFO1_0("No IPv6 dest! Ignoring!!! ");  
        }
        if (req_ptr->src_ipv6_addr_valid)
        {
					if ( req_ptr->src_ipv6_addr.src_ipv6_prefix_length > ATP_MAX_IPV6_PREFIX_LENGTH)
					{
						LOG_MSG_ERROR_1("Invalid IPV6 prefix length Src %d", 
													req_ptr->src_ipv6_addr.src_ipv6_prefix_length);
						resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
						resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
						qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
														sizeof(atp_send_filter_report_resp_msg_v01));
						return QMI_CSI_CB_NO_ERR;
					}
          memscpy(atp_filter_report.filter_report.tuple_info.src_ip.src_ipv6_addr,
                  sizeof(atp_filter_report.filter_report.tuple_info.src_ip.src_ipv6_addr),
                  req_ptr->src_ipv6_addr.src_ipv6_addr, sizeof(req_ptr->src_ipv6_addr.src_ipv6_addr));
          
        }
        else
        {
           LOG_MSG_INFO1_0("No IPv6 src! Ignoring!!! ");  
        }
      }
    } //IPv6 filter
  } //ip_type_valid

  if (req_ptr->dest_port_valid)
  {
    atp_filter_report.filter_report.tuple_info.dest_port = req_ptr->dest_port;
  }
  if (req_ptr->src_port_valid)
  {
    atp_filter_report.filter_report.tuple_info.src_port = req_ptr->src_port;
  }
  if (req_ptr->protocol_valid)
  {
    atp_filter_report.filter_report.tuple_info.protocol = req_ptr->protocol;
  }

  atp_errno = ds_atp_proxy_service_req(DS_ATP_PROXY_SERVICE_FILTER_REPORT_REQ,
                                       &atp_filter_report); 

  if (DS_ATP_PROXY_ERR_NONE != atp_errno)
  {
    LOG_MSG_ERROR_1("qmi_atpi_send_filter_report: Error %d",atp_errno);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    if (DS_ATP_PROXY_ERR_EVENT_NOT_SUPP == atp_errno)
    {
      resp_msg.resp.error = QMI_ERR_INVALID_OPERATION_V01;
    }
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(atp_send_filter_report_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  /*---------------------------------------------------------------------
    Add the report id to the client's list: Response to the filter report
    request is sent by the network later. This is notified to the client
    via fiter report indication. The report_id uniquely identfies the
    client to which the indication is sent. Report_id is expected to be
    unique across all clients/requests.
  ---------------------------------------------------------------------*/

  new_filter = ps_system_heap_mem_alloc(
     sizeof(qmi_atpi_filter_report_status_list_type));

  if (new_filter == NULL)
  {
    LOG_MSG_INFO2_0("No memory for allocating filter report!");
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_NO_MEMORY_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(atp_send_filter_report_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  new_filter->report_id = req_ptr->report_id;
  new_filter->next =  clnt_info->filter_list;

  clnt_info->filter_list = new_filter;
  clnt_info->num_filter_list++;
  
  LOG_MSG_INFO1_2("Added report id %d to client 0x%p", new_filter->report_id, 
                   clnt_info);

  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(atp_send_filter_report_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_atpi_send_filter_report() */

/*===========================================================================
  FUNCTION QMI_ATPI_INDICATION_REGISTER()

  DESCRIPTION
    Sets the conditions for indication reporting

  PARAMETERS
    clnt_info          : Client info pointer
    req_handle         : Request handle
    msg_id             : Message Id
    req_c_struct       : Incoming request 
    req_c_struct_len   : incoming request length
    service_handle     : Service handle

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_atp_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_atpi_indication_register
(
  qmi_atpi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  qmi_atpi_state_info_type                * atp_sp;
  atp_indication_register_req_msg_v01     * req_ptr;
  atp_indication_register_resp_msg_v01      resp_msg;
   
  qmi_error_type_v01   errval;
  qmi_result_type_v01  result;
  qmi_atpi_client_info_type  * clnt_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1("qmi_atpi_indication_register: msg_id %d ",msg_id);

  ASSERT(clnt_info);
  ASSERT(service_handle);
  
  errval = QMI_ERR_NONE_V01;
  memset(&resp_msg, 0, sizeof(atp_indication_register_resp_msg_v01));  
  
  req_ptr = (atp_indication_register_req_msg_v01 *)req_c_struct;
  atp_sp = (qmi_atpi_state_info_type *) service_handle;
  clnt_info_ptr = q_linear_search( &atp_sp->client_q,
                                   atpi_compare_connection_info_handle,
                                   clnt_info );
                                   
  if(clnt_info_ptr == NULL)
  {
    LOG_MSG_ERROR_0("qmi_atpi_indication_register: No client found ");
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }             

  //Send the registered values in response
  if (req_ptr->report_activation_status_valid)
  {
    if (req_ptr->report_activation_status != TRUE &&
        req_ptr->report_activation_status != FALSE)
    {
      LOG_MSG_ERROR_1("Invalid input for act status %d ",
                    req_ptr->report_activation_status);
      errval =  QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }
    clnt_info_ptr->report_status.report_activation_status = 
      req_ptr->report_activation_status ? TRUE : FALSE;
    resp_msg.activation_status_valid = TRUE;
    resp_msg.activation_status = req_ptr->report_activation_status;
  }

  if (req_ptr->report_policy_info_valid)
  {
    if (req_ptr->report_policy_info != TRUE &&
        req_ptr->report_policy_info != FALSE)
    {
      LOG_MSG_ERROR_1("Invalid input for policy info %d ",
                    req_ptr->report_policy_info);
      errval =  QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }
    clnt_info_ptr->report_status.report_policy_info = 
      req_ptr->report_policy_info ? TRUE : FALSE;   
    resp_msg.policy_info_valid = TRUE;
    resp_msg.policy_info = req_ptr->report_policy_info;
  }

send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.result = result;
  resp_msg.resp.error = errval;  

  LOG_MSG_INFO2_2("qmi_atpi_indication_register result %d errval %d",
                 result, errval);     

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(atp_indication_register_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_atpi_indication_register */

/*===========================================================================
  FUNCTION ATPI_ACT_STATUS_CHANGE_CB()

  DESCRIPTION
    This the callback that we have registered for activation status change

  PARAMETERS
    event_name     : Event name
    event_info_ptr : event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void atpi_act_status_change_cb
(
  ds_atp_proxy_event_enum_type  event,
  void                          *event_info_ptr,
  void                          *user_data_ptr
)
{
  qmi_atpi_cmd_buf_type                   * cmd_ptr = NULL;
  atp_activation_status_ind_msg_v01       * atp_act_status = NULL;
  ds_atp_active_status_type               * act_status_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("atpi_act_status_change_cb event_name %d"
                " event_info_ptr 0x%x", event, event_info_ptr);   

  ASSERT(event_info_ptr);

  act_status_info = (ds_atp_active_status_type *)event_info_ptr;

  cmd_ptr = (qmi_atpi_cmd_buf_type *)
              qmi_atpi_get_cmd_buf(QMI_ATPI_ACT_STATUS_CHANGE_EVT);

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI ATP cmd buf");
    return;
  }
  
  memset(cmd_ptr, 0, sizeof(qmi_atpi_cmd_buf_type));
  
  atp_act_status = &cmd_ptr->data.act_status;
  atp_act_status->is_activated = act_status_info->active_status;

  cmd_ptr->cmd_id = QMI_ATPI_ACT_STATUS_CHANGE_EVT ;
  dcc_send_cmd_ex (DCC_ATP_SVC_CMD, cmd_ptr);

} /* atpi_act_status_change_cb */

/*===========================================================================
  FUNCTION ATPI_POLICY_INFO_CHANGE_CB()

  DESCRIPTION
    This the callback that we have registered for policy info change

  PARAMETERS
    event_name     : Event name
    event_info_ptr : event info
    user_data_ptr  : Optional user data to be sent

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void atpi_policy_info_change_cb
(
  ds_atp_proxy_event_enum_type  event,
  void                          *event_info_ptr,
  void                          *user_data_ptr
)
{
  qmi_atpi_cmd_buf_type                   * cmd_ptr = NULL;
  ds_atp_policy_info_type                  *policy_info_ptr = NULL;
  ds_atp_policy_info_type                  *atp_policy = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  ASSERT(event_info_ptr);
  LOG_MSG_INFO2_2("atpi_policy_info_change_cb() %d , event_info_ptr 0x%p ",
                event, event_info_ptr);

  atp_policy = (ds_atp_policy_info_type *)event_info_ptr;
  
  cmd_ptr = (qmi_atpi_cmd_buf_type *)
              qmi_atpi_get_cmd_buf(QMI_ATPI_POLICY_INFO_CHANGE_EVT);

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI ATP cmd buf");
    return;
  }
  memset(cmd_ptr, 0, sizeof(qmi_atpi_cmd_buf_type));

  /*-------------------------------------------------------------------------
    Copy policy info from event info 
  -------------------------------------------------------------------------*/
  policy_info_ptr = (ds_atp_policy_info_type *)ps_system_heap_mem_alloc(
                      sizeof(ds_atp_policy_info_type));

  if( policy_info_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI ATP policy buf");
    PS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
    return;
  }

  memset(policy_info_ptr, 0, sizeof(ds_atp_policy_info_type));

  memscpy(policy_info_ptr, sizeof(ds_atp_policy_info_type),
          atp_policy, sizeof(ds_atp_policy_info_type)); 

  cmd_ptr->data.policy_tuple.cl_id = ATP_RESERVED_CLIENT_ID;
  cmd_ptr->data.policy_tuple.policy_info_ptr = policy_info_ptr; 
  cmd_ptr->cmd_id = QMI_ATPI_POLICY_INFO_CHANGE_EVT;
  dcc_send_cmd_ex (DCC_ATP_SVC_CMD, cmd_ptr);
  
} /* atpi_policy_info_change_cb */

/*===========================================================================
  FUNCTION ATPI_FILTER_REPORT_STATUS_CB()

  DESCRIPTION
    This the callback that we have registered for filter report status

  PARAMETERS
    event_name     : Event name
    event_info_ptr : event info
    user_data_ptr  : Optional user data to be sent

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void atpi_filter_report_status_cb
(
  ds_atp_proxy_event_enum_type  event,
  void                          *event_info_ptr,
  void                          *user_data_ptr
)
{
  qmi_atpi_cmd_buf_type                   * cmd_ptr = NULL;
  atp_send_filter_report_ind_msg_v01      * atp_filter_report_status = NULL;
  ds_atp_filter_report_resp_type              * filter_report_status_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("atpi_filter_report_status_cb event_name %d"
                " event_info_ptr 0x%x", event, event_info_ptr);   

  ASSERT(event_info_ptr);

  filter_report_status_info = (ds_atp_filter_report_resp_type *)event_info_ptr;

  cmd_ptr = (qmi_atpi_cmd_buf_type *)
              qmi_atpi_get_cmd_buf(QMI_ATPI_FILTER_REPORT_STATUS_EVT);

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI ATP cmd buf");
    return;
  }
  
  memset(cmd_ptr, 0, sizeof(qmi_atpi_cmd_buf_type));
  
  atp_filter_report_status = &cmd_ptr->data.filter_report_status;

  atp_filter_report_status->report_id = filter_report_status_info->report_id;
  atp_filter_report_status->filter_report_resp_valid = TRUE;
  atp_filter_report_status->filter_report_resp =
    (atp_filter_report_resp_enum_type_v01)filter_report_status_info->filter_ack;
  
  cmd_ptr->cmd_id = QMI_ATPI_FILTER_REPORT_STATUS_EVT;
  dcc_send_cmd_ex (DCC_ATP_SVC_CMD, cmd_ptr);
} /* atpi_filter_report_status_cb */

/*===========================================================================
FUNCTION  QMI_ATPI_GENERATE_ACT_STATUS_IND

DESCRIPTION
  This function generated act status indication to registered clients

PARAMETERS
  act_status - Indicates activation status

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void qmi_atpi_generate_act_status_ind
(
  atp_activation_status_ind_msg_v01  act_status 
)
{
  qmi_atpi_client_info_type  * cl_sp;
  q_type                      * client_q_ptr = NULL;
  void                        * client_buf_ptr;
  void                        * next_client_buf_ptr;
  int                           errval;
  qmi_atpi_state_info_type    * atp_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1("qmi_atpi_generate_act_status_ind(): act status %d ",
                 act_status.is_activated);
    
  errval         = QMI_ERR_NONE_V01;
  atp_sp         = &qmi_atp_state;
  client_q_ptr   = &atp_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);
  
  while(client_buf_ptr != NULL)
  {
    cl_sp = (qmi_atpi_client_info_type *) client_buf_ptr;
    
    if (TRUE == cl_sp->report_status.report_activation_status)
    {    
      qmi_atpi_act_status_ind(cl_sp,
                              &act_status);
    }
    
    next_client_buf_ptr = 
      q_next(client_q_ptr, &((q_generic_item_type *) client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
} /* qmi_atpi_generate_act_status_ind */

/*===========================================================================
  FUNCTION QMI_ATPI_ACT_STATUS_IND()

  DESCRIPTION
    This function sends activation status indication to registered
    control point.

  PARAMETERS
    clid             : clientID
    act_status       : act status

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_atpi_act_status_ind
(
  void                                  * client,
  atp_activation_status_ind_msg_v01     * act_status
)
{
  qmi_atpi_client_info_type               * client_info;
  qmi_csi_error                             rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rc = QMI_CSI_NO_ERR;
  client_info = (qmi_atpi_client_info_type *) client;
  if (client_info == NULL)
  {
    DATA_ERR_FATAL("qmi_atpi_act_status_ind : Null client_info");
    return;
  }
            
  rc = qmi_csi_send_ind(client_info->clnt, 
                        QMI_ATP_ACTIVATION_STATUS_IND_V01 ,
                        act_status, 
                        sizeof(atp_activation_status_ind_msg_v01));
  
  if ( rc != QMI_CSI_NO_ERR )
  {
    LOG_MSG_ERROR_1("qmi_atpi_act_status_ind error %d", rc);
  }

} /* qmi_atpi_act_status_ind() */

/*===========================================================================
FUNCTION  QMI_ATPI_GENERATE_FILTER_REPORT_STATUS_IND 

DESCRIPTION
  Generates filter report status indication to the requested control point

PARAMETERS
  filter_report - Filter report status sent by the network

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void qmi_atpi_generate_filter_report_ind
(
  atp_send_filter_report_ind_msg_v01  filter_report
)
{
  qmi_atpi_client_info_type  * cl_sp;
  q_type                      * client_q_ptr = NULL;
  void                        * client_buf_ptr;
  void                        * next_client_buf_ptr;
  int                           errval;
  qmi_atpi_state_info_type    * atp_sp;
  qmi_atpi_filter_report_status_list_type    * fltr_prev;
  qmi_atpi_filter_report_status_list_type    * fltr;
  boolean                       filter_found;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1("qmi_atpi_generate_act_status_ind(): report id %d ",
                 filter_report.report_id);
    
  errval         = QMI_ERR_NONE_V01;
  atp_sp         = &qmi_atp_state;
  client_q_ptr   = &atp_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);
  filter_found   = FALSE;
  
  while(client_buf_ptr != NULL)
  {
    cl_sp = (qmi_atpi_client_info_type *) client_buf_ptr;
    fltr_prev = NULL;
    fltr = cl_sp->filter_list;
  /*-------------------------------------------------------------------------
    Find the client with matching report_id.
    Report_id assumed to be unique across all clients/all filter requests
  -------------------------------------------------------------------------*/
    while (fltr != NULL)
    {
      if (fltr->report_id == filter_report.report_id)
      {
        filter_found = TRUE;
        break;
      }
      fltr_prev = fltr;
      fltr = fltr->next;
    }

    if (fltr == NULL)
    {
      LOG_MSG_INFO2_0("No filters with current client");
    }
    
    if (filter_found)
    {
      qmi_atpi_filter_report_status_ind(cl_sp, &filter_report);

      //Clean up the filter from the ATP Client's list
      if (fltr_prev == NULL)
      {
        cl_sp->filter_list = fltr->next;
      }
      else
      {
        fltr_prev->next = fltr->next;
      }

      PS_SYSTEM_HEAP_MEM_FREE(fltr);
      cl_sp->num_filter_list--;
      
      LOG_MSG_INFO1_1("Filter report status indication sent! Report_id : %d ", 
                    filter_report.report_id);
      
      break;
    }
    
    next_client_buf_ptr = 
      q_next(client_q_ptr, &((q_generic_item_type *) client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }

} /* qmi_atpi_generate_filter_report_ind */

/*===========================================================================
  FUNCTION QMI_FILTER_REPORT_STATUS_IND()

  DESCRIPTION
    This function sends filter report status indication to the
    requested control point.

  PARAMETERS
    client             : client to which indication is sent
    filter_report      : filter report status

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_atpi_filter_report_status_ind
(
  void                                  * client,
  atp_send_filter_report_ind_msg_v01    * filter_report
)
{
  qmi_atpi_client_info_type               * client_info;
  qmi_csi_error                             rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rc = QMI_CSI_NO_ERR;
  client_info = (qmi_atpi_client_info_type *) client;
  if (client_info == NULL)
  {
    DATA_ERR_FATAL("qmi_atpi_filter_report_status_ind: Null client_info");
    return;
  }
            
  rc = qmi_csi_send_ind(client_info->clnt, 
                        QMI_ATP_SEND_FILTER_REPORT_IND_V01,
                        filter_report, 
                        sizeof(atp_send_filter_report_ind_msg_v01));
  
  if ( rc != QMI_CSI_NO_ERR )
  {
    LOG_MSG_ERROR_1("qmi_atpi_filter_report_status_ind error %d", rc);
  }

} /* qmi_atpi_filter_report_status_ind() */

/*===========================================================================
FUNCTION  QMI_ATPI_GENERATE_POLICY_INFO_IND

DESCRIPTION
  Generates policy info indication to the control point

PARAMETERS 
  cl_id       - Client ID(Client/s to which indication is sent)
  policy_info - Policy information

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void qmi_atpi_generate_policy_info_ind
(
  uint32                   cl_id,
  ds_atp_policy_info_type  *policy_info
)
{
  qmi_atpi_client_info_type  * cl_sp;
  q_type                      * client_q_ptr = NULL;
  void                        * client_buf_ptr;
  void                        * next_client_buf_ptr;
  int                           errval;
  qmi_atpi_state_info_type    * atp_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(policy_info);

  errval         = QMI_ERR_NONE_V01;
  atp_sp         = &qmi_atp_state;
  client_q_ptr   = &atp_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);
 
  LOG_MSG_INFO2_1("qmi_atpi_generate_policy_info_ind: cl id %d ",cl_id);

  /*-------------------------------------------------------------------------
    if cl_sp == ATP_RESERVED_CLIENT_ID, its a nw inited policy change.
    Send to all registered clients.
  -------------------------------------------------------------------------*/
  if ( cl_id == ATP_RESERVED_CLIENT_ID )
  {
    while(client_buf_ptr != NULL)
    {
      cl_sp = (qmi_atpi_client_info_type *) client_buf_ptr;
    
      if (TRUE == cl_sp->report_status.report_policy_info)
      {    
        qmi_atpi_policy_info_ind(cl_sp,
                                 policy_info);
      }
    
      next_client_buf_ptr = 
        q_next(client_q_ptr, &((q_generic_item_type *) client_buf_ptr)->link);
      client_buf_ptr = next_client_buf_ptr;
    }
  }
  else
  {
    /*-------------------------------------------------------------------------
      if cl_sp != ATP_RESERVED_CLIENT_ID , this policy info indication is a
      response to previous query. Generate ind only to that client 
    -------------------------------------------------------------------------*/
    cl_sp = (qmi_atpi_client_info_type *) cl_id;
    if (TRUE == cl_sp->report_status.report_policy_info)
    {
      qmi_atpi_policy_info_ind(cl_sp,
                               policy_info);
    }
  }

  PS_SYSTEM_HEAP_MEM_FREE(policy_info); 
} /* qmi_atpi_generate_policy_info_ind */

/*===========================================================================
  FUNCTION QMI_ATPI_POLICY_INFO_IND()

  DESCRIPTION
    This function sends policy info via multiple indications to registered
    control point. Policy info indication is fragmented into multiple
    indications due to large size of the structure.

  PARAMETERS
    client                 : client ptr
    atp_policy_info        : policy information

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_atpi_policy_info_ind
(
  void                                  * client,
  ds_atp_policy_info_type               * atp_policy_info
)
{
  qmi_atpi_client_info_type               * client_info;
  qmi_csi_error                             rc;
  atp_policy_info_change_ind_msg_v01      * policy_info_ind;
  int                                       num_ind = 0;
  int                                       list_size = 0;
  int                                       cnt_ind  = 0;
  int                                       cnt_policy = 0;
  int                                       cnt_uid = 0;
  int                                       cnt_hash = 0;
  int                                       package_arr_offset = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(client);
  ASSERT(atp_policy_info);

  rc = QMI_CSI_NO_ERR;
  client_info = (qmi_atpi_client_info_type *) client;
 
  /*-------------------------------------------------------------------------
    Policy info indication is fragmented into multiple indications due to
    large size of the structure. Each ind containd utmost
    ATP_MAX_POLICY_INFO_PER_IND.
  -------------------------------------------------------------------------*/
  if (atp_policy_info->num_package > DS_ATP_MAX_NUM_PACKAGE)
  {
    LOG_MSG_ERROR_1("qmi_atpi_policy_info_ind: Max pkgs reached %d", 
                   atp_policy_info->num_package);
    return;
  }

  LOG_MSG_INFO2_1("qmi_atpi_policy_info_ind(): Num pkgs %d  ",
                 atp_policy_info->num_package);

  num_ind = (atp_policy_info->num_package)/ ATP_MAX_POLICY_INFO_PER_IND + 1;
  
  /*-------------------------------------------------------------------------
    Increment list id for this policy info ; List id unique per client/per
    policy info. This list id will be same across all fragmented indications
    for this policy info
  -------------------------------------------------------------------------*/
  client_info->policy_info_list_id++;
  LOG_MSG_INFO2_2("num pkgs %d list id: %d  ",atp_policy_info->num_package, 
                client_info->policy_info_list_id);

  LOG_MSG_INFO2_1("Sending list %d  ",client_info->policy_info_list_id);
  for (cnt_ind = 0; cnt_ind < num_ind; cnt_ind ++)
  {
    if (cnt_ind == num_ind - 1) 
    {
      //Calculate num policy sent in last iteration.
      list_size = 
        (atp_policy_info->num_package) % ATP_MAX_POLICY_INFO_PER_IND;

      if ( (atp_policy_info->num_package != 0) && (list_size == 0) )
      {
        break;
      }
    }
    else
    {
      list_size = ATP_MAX_POLICY_INFO_PER_IND;
    }
    
    policy_info_ind = 
      (atp_policy_info_change_ind_msg_v01 *)ps_system_heap_mem_alloc(
         sizeof(atp_policy_info_change_ind_msg_v01));

    if (policy_info_ind == NULL)
    {
       LOG_MSG_ERROR_0("Couldn't alloc mem for policy info ind");
       return;
    }
    memset(policy_info_ind, 0, sizeof(atp_policy_info_change_ind_msg_v01));

    policy_info_ind->atp_policy_list_valid = TRUE;
    policy_info_ind->atp_policy_list.list_id =
       client_info->policy_info_list_id;
    policy_info_ind->atp_policy_list.total_list_entries = 
      atp_policy_info->num_package;
    policy_info_ind->atp_policy_list.policy_info_len = list_size;

    //Each ind contains list_size policy entries
    for (cnt_policy = 0; cnt_policy < list_size; cnt_policy++)
    {
      if (cnt_policy + package_arr_offset < DS_ATP_MAX_NUM_PACKAGE)
      {
        if ((strlen(atp_policy_info->package_arr[cnt_policy + package_arr_offset].apn) > DS_ATP_MAX_APN_NAME_LEN) ||
           (strlen(atp_policy_info->package_arr[cnt_policy + package_arr_offset].package_name) > DS_ATP_MAX_PACKAGE_NAME_LEN))
        {
          LOG_MSG_ERROR_2("pkg or apn name too large %d", 
                        strlen(atp_policy_info->package_arr[cnt_policy + package_arr_offset].apn),
                        strlen(atp_policy_info->package_arr[cnt_policy + package_arr_offset].package_name));
          PS_SYSTEM_HEAP_MEM_FREE(policy_info_ind);
          return;
        }
      strlcpy( policy_info_ind->atp_policy_list.policy_info[cnt_policy].apn,
               atp_policy_info->package_arr[cnt_policy + package_arr_offset].apn,
               strlen(atp_policy_info->package_arr[cnt_policy + package_arr_offset].apn) + 1 );

      strlcpy( policy_info_ind->atp_policy_list.policy_info[cnt_policy].package_name,
               atp_policy_info->package_arr[cnt_policy + package_arr_offset].package_name,
               strlen(atp_policy_info->package_arr[cnt_policy + package_arr_offset].package_name) +1 );  

      policy_info_ind->atp_policy_list.policy_info[cnt_policy].service_id =
        atp_policy_info->package_arr[cnt_policy + package_arr_offset].service_id;

      policy_info_ind->atp_policy_list.policy_info[cnt_policy].max_ack_timeout =
        atp_policy_info->package_arr[cnt_policy + package_arr_offset].max_ack_timeout;


      policy_info_ind->atp_policy_list.policy_info[cnt_policy].framework_uid_len = 
        atp_policy_info->package_arr[cnt_policy + package_arr_offset].framework_uid_num;
    
      for (cnt_uid = 0; cnt_uid < policy_info_ind->atp_policy_list. \
            policy_info[cnt_policy].framework_uid_len; cnt_uid++)
      {
        policy_info_ind->atp_policy_list.policy_info[cnt_policy].framework_uid[cnt_uid] = 
          atp_policy_info->package_arr[cnt_policy + package_arr_offset].framework_uid[cnt_uid];
      }
     
      policy_info_ind->atp_policy_list.policy_info[cnt_policy].hash_values_len = 
        atp_policy_info->package_arr[cnt_policy + package_arr_offset].hash_num;
    
      for (cnt_hash = 0; cnt_hash < policy_info_ind->atp_policy_list. \
            policy_info[cnt_policy].hash_values_len; cnt_hash++)
      {
        policy_info_ind->atp_policy_list.policy_info[cnt_policy].hash_values[cnt_hash] = 
          atp_policy_info->package_arr[cnt_policy + package_arr_offset].hash_values[cnt_hash];
      }
      }
      else
      {
        LOG_MSG_ERROR_1("Invalid offset in accessing pkg array %d", 
                       cnt_policy + package_arr_offset);
        PS_SYSTEM_HEAP_MEM_FREE(policy_info_ind);
        return; 
      }
    } /* for each policy */ 

    package_arr_offset = package_arr_offset + list_size;
    rc = qmi_csi_send_ind(client_info->clnt, 
                          QMI_ATP_POLICY_INFO_CHANGE_IND_V01 ,
                          policy_info_ind, 
                          sizeof(atp_policy_info_change_ind_msg_v01)); 

    if ( rc != QMI_CSI_NO_ERR )
    {
      LOG_MSG_ERROR_1("qmi_atpi_policy_info_ind error %d", rc);
      PS_SYSTEM_HEAP_MEM_FREE(policy_info_ind);
      return; 
    }

    PS_SYSTEM_HEAP_MEM_FREE(policy_info_ind); 
  } /* for each ind */
} /* qmi_atpi_policy_info_ind() */

#endif /* FEATURE_DATA_ATP */

        
