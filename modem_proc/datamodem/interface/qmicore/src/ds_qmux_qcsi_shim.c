/*===========================================================================

                        D S _ Q M U X _ Q C S I _ S H I M . C

DESCRIPTION

  The QMUX QCSI Shim interface file.
 
EXTERNALIZED FUNCTIONS
 
  qmux_qcsi_shim_init()
    Initialize QMUX QCSI Shim layer.
 
  qmux_qcsi_shim_reg()
    Register a QMUX service to the QCSI Framework.

  qmux_qcsi_shim_dereg()
    Deregister a QMUX service from the QCSI Framework.
 
  qmux_qcsi_shim_process_cl_alloc_result()
    Handle client allocation result from QMUX service.
 
  qmux_qcsi_shim_send_resp_ind()
    Send a response or indication message from QMUX to QCSI framework. 
 
Copyright (c) 2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/src/ds_qmux_qcsi_shim.c#1 $ 
   $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/15/15    sj      Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "queue.h"
#include "qmi_modem_task_svc.h"
#include "qmi_modem_task_cmd.h"
#include "qmi_svc_defs.h"
#include "ds_qmi_svc_ext.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"
#include <stringl/stringl.h>
#include "rcinit_rex.h"
#include "qmi_csi.h"
#include "qmi_framework.h"
#include "qmi_frameworki.h"
#include "ps_utils.h"
#include "qmi_csi_deprecated.h"
#include "mqcsi_log.h"

#include "wireless_data_service_v01.h"
#include "device_management_service_v01.h"
#include "network_access_service_v01.h"
#include "authentication_service_v01.h"
#include "voice_service_v02.h"
#include "card_application_toolkit_v02.h"
#include "user_identity_module_v01.h"
#include "phonebook_manager_service_v01.h"
#include "wireless_data_administrative_service_v01.h"
#include "circuit_switched_video_telephony_v01.h"
#include "wireless_messaging_service_v01.h"
#include "quality_of_service_v01.h"
#include "access_terminal_service_v01.h"

/*===========================================================================

                               DATA TYPES 

===========================================================================*/

extern rex_tcb_type qmi_modem_tcb;

/*===========================================================================
TYPEDEF QMI_FRAMEWORK_ERR_E_TYPE

DESCRIPTION
  Enumeration for qmi Framework Error code.
===========================================================================*/
typedef enum
{
  QMUX_QCSI_SHIM_ERR_NONE                     = 0,
  QMUX_QCSI_SHIM_ERR_SHIM_NOT_READY           = 1,  /* shim not init */
  QMUX_QCSI_SHIM_ERR_SVC_ALREADY_REGISTERED   = 2,
  QMUX_QCSI_SHIM_ERR_NO_MEMORY                = 3,
  QMUX_QCSI_SHIM_ERR_NO_SIGS                  = 4,
  QMUX_QCSI_SHIM_ERR_CL_NOT_FOUND             = 5,
  QMUX_QCSI_SHIM_ERR_FRAMEWORK_ERR            = 6,
  QMUX_QCSI_SHIM_ERR_MSG_ID_MISS              = 7,
  QMUX_QCSI_SHIM_ERR_MSG_LEN_MISS             = 8,
  QMUX_QCSI_SHIM_ERR_MALFORMED_MSG            = 9,
  QMUX_QCSI_SHIM_ERR_REQ_HANDLE_NOT_FOUND     = 10,
  QMUX_QCSI_SHIM_ERR_QCSI_SEND                = 11,
  QMUX_QCSI_SHIM_ERR_QCSI_REG                 = 12,
  QMUX_QCSI_SHIM_ERR_MAX,
  QMUX_QCSI_SHIM_ERR_WIDTH                   = 0xFFFF
} qmux_qcsi_shim_err_e_type;

typedef enum
{
  QMUX_QCSI_SHIM_CMD_SVC_REG         = 0,    /* Register QMUX service with QCSI */
  QMUX_QCSI_SHIM_CMD_MAX
} qmux_qcsi_shim_cmd_e_type;

typedef struct
{
  qmux_qcsi_shim_cmd_e_type     cmd_type;
  union
  {
    struct
    {
      qmux_service_e_type     service;
    } reg_service;
  } cmd_data;
} qmux_qcsi_shim_cmd_type;

/*===========================================================================
TYPEDEF QMUX_QCSI_SHIM_SVC_INFO_TYPE

DESCRIPTION 
  Shim layer Service Info type 
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type             link; /*< Data Structure required by the queue 
                                    library to add this as part of a queue */
  q_type                  client_q; /* Q of all service clients */
  q_type                  pending_requests; /* Q of all pending requests */
  qmi_csi_service_handle  service_handle; /* QCSI handle */
  qmux_service_e_type     service_id; /* Service type    */
  qmi_csi_os_params       os_params;
  qmi_sig_enum_type       svc_sig;  /* Specific signal being used for the service */
} qmux_qcsi_shim_svc_info_type;

/*===========================================================================
TYPEDEF QMUX_QCSI_SHIM_CLNT_INFO_TYPE

DESCRIPTION 
 Shim layer client info type.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type       link; /*< Data Structure required by the queue 
                              library to add this as part of a queue */
  qmi_client_handle clnt; /* QCSI handle */
  uint8             qmux_client_id;
  uint16            tx_id; 
} qmux_qcsi_shim_clnt_info_type;

/*===========================================================================
TYPEDEF QMUX_QCSI_SHIM_PENDING_REQ_TYPE

DESCRIPTION 
 Contains information on requests that have been sent to the QMUX service
 and are awaiting response.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type       link; /*< Data Structure required by the queue 
                              library to add this as part of a queue */
  qmi_req_handle    req_handle; /*< Req handle pertaining to the pending req */
  uint16            msg_id; 
  uint8             qmux_client_id;
  uint16            tx_id; 
} qmux_qcsi_shim_pending_req_type;

/*===========================================================================
TYPEDEF QMUX_SDU_HDR_TYPE

DESCRIPTION 
 Structure of SDU header.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8   ctl_flags;
  uint16  tx_id;
  uint16  msg_id;
  uint16  len;
} qmux_sdu_hdr_type;

/*---------------------------------------------------------------------------
  Queue used to keep track of shim service state for all services
---------------------------------------------------------------------------*/
static q_type  qmux_qcsi_shim_svc_q; 
static boolean qmux_qcsi_shim_powerup_inited = FALSE;

/*---------------------------------------------------------------------------
  Table holds in use flags for all SVC signals
---------------------------------------------------------------------------*/
#define QMUX_QCSI_SHIM_MAX_SVCS  13
static uint32 qmux_qcsi_shim_svc_sig_in_use_mask = 0;

/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/
static int qmux_qcsi_shim_compare_qmux_service_id
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmux_qcsi_shim_svc_info_type *svc_info = (qmux_qcsi_shim_svc_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (svc_info->service_id == 
                          (qmux_service_e_type)compare_val) ? 1 : 0 );
} /* qmux_qcsi_shim_compare_qmux_service_id */

static int qmux_qcsi_shim_compare_qmi_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmux_qcsi_shim_clnt_info_type *clnt_info = (qmux_qcsi_shim_clnt_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
} /* qmux_qcsi_shim_compare_qmi_client_handle */

static int qmux_qcsi_shim_compare_qmi_client_id
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmux_qcsi_shim_clnt_info_type *clnt_info = (qmux_qcsi_shim_clnt_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->qmux_client_id == 
                          (uint8)compare_val) ? 1 : 0 );
} /* qmux_qcsi_shim_compare_qmi_client_id */

static int qmux_qcsi_shim_compare_connection_info_handle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ( (p_ItemPtr == p_CompareVal) ? 1 : 0 );
} /* qmux_qcsi_shim_compare_connection_info_handle */

typedef struct
{
  uint8             qmux_client_id;
  uint16            tx_id; 
} qmux_qcsi_shim_pending_req_key_type;

static int qmux_qcsi_shim_compare_pending_req
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmux_qcsi_shim_pending_req_type *req_node = (qmux_qcsi_shim_pending_req_type*)item_ptr;
  qmux_qcsi_shim_pending_req_key_type *key_info = (qmux_qcsi_shim_pending_req_key_type*)compare_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( ((req_node->qmux_client_id == key_info->qmux_client_id) &&
            (req_node->tx_id == key_info->tx_id))? 1 : 0 );
} /* qmux_qcsi_shim_compare_pending_req */

static int qmux_qcsi_shim_compare_pending_req_for_client
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmux_qcsi_shim_pending_req_type *req_node;
  uint8 *clid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((item_ptr == NULL) || (compare_val == NULL))
  {
    return FALSE;
  }
  req_node = (qmux_qcsi_shim_pending_req_type*)item_ptr;
  clid = (uint8 *)compare_val;

  return ( (req_node->qmux_client_id == (*clid))? 1 : 0 );
} /* qmux_qcsi_shim_compare_pending_req_for_client */

static boolean qmux_qcsi_shim_msg_hdlr_cb
(
  qmi_sig_enum_type  sig,
  void              *svc_ptr
);

static qmi_csi_cb_error qmux_qcsi_shim_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmux_qcsi_shim_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmux_qcsi_shim_svc_handle_raw_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);

static void qmux_qcsi_shim_process_reg
( 
  qmux_service_e_type     service
);

static qmux_qcsi_shim_err_e_type qmux_qcsi_shim_send_raw_msg
(
  qmux_qcsi_shim_svc_info_type   *svc_info_ptr,
  qmux_qcsi_shim_clnt_info_type  *clnt_info_ptr,
  qmi_req_handle                  req_handle,
  unsigned int                    msg_id,
  void                           *raw_msg,
  unsigned int                    raw_msg_len
);
/*===========================================================================

                               DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION QMUX_QCSI_SHIM_INIT()

DESCRIPTION
  This function initializes the QMUX QCSI shim layer. 
  
PARAMETERS
  None
  
RETURN VALUE
  None

DEPENDENCIES
  QMUX is initialized.  

SIDE EFFECTS
  None.
===========================================================================*/
void qmux_qcsi_shim_init
( 
  void
)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize the shim state structure.
  -------------------------------------------------------------------------*/
  (void)q_init(&qmux_qcsi_shim_svc_q); 

  qmux_qcsi_shim_powerup_inited = TRUE;
  
} /* qmux_qcsi_shim_init() */

/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_PROCESS_CMD()

  DESCRIPTION
    This function processes a shim layer cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd          :  QMI command to be processed
    user_data_ptr:  private data buffer containing the shim layer command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    Shim layer module must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmux_qcsi_shim_process_cmd
(
  qmi_cmd_enum_type        cmd,
  void                   * user_data_ptr
)
{

  qmux_qcsi_shim_cmd_type  * cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = (qmux_qcsi_shim_cmd_type *)user_data_ptr;
  ASSERT (cmd_ptr && (cmd == QMI_QMUX_QCSI_SHIM_CMD) && (cmd_ptr->cmd_type < QMUX_QCSI_SHIM_CMD_MAX));

  switch (cmd_ptr->cmd_type)
  {
     case QMUX_QCSI_SHIM_CMD_SVC_REG:
       qmux_qcsi_shim_process_reg(cmd_ptr->cmd_data.reg_service.service);
       break;
     default:
       break;
  }
  /* user_data_ptr is freed within qmii_process_cmd  */
}
/*===========================================================================
FUNCTION QMUX_QCSI_SHIM_REG()

DESCRIPTION
  This function registers the QMUX service with QCSI framework. 
  
PARAMETERS
  service       : QMUX service ID.
  
RETURN VALUE
  None

DEPENDENCIES
  Shim layer has been initialized. 

SIDE EFFECTS
  None.
===========================================================================*/
void qmux_qcsi_shim_reg
( 
  qmux_service_e_type     service
)
{
  qmux_qcsi_shim_cmd_type *  cmd_ptr;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(cmd_ptr, sizeof(qmux_qcsi_shim_cmd_type),
                                   qmux_qcsi_shim_cmd_type*);
  if( cmd_ptr == NULL)
  {
    return;
  }
  cmd_ptr->cmd_type = QMUX_QCSI_SHIM_CMD_SVC_REG;
  cmd_ptr->cmd_data.reg_service.service = service;
  qmi_send_cmd(QMI_QMUX_QCSI_SHIM_CMD, cmd_ptr);
} /* qmux_qcsi_shim_reg() */

/*===========================================================================
FUNCTION QMUX_QCSI_SHIM_GET_SVC_OBJ()

DESCRIPTION
  This function returns the service object for the passed in service id. 
  
PARAMETERS
  service       : QMUX service ID.
  
RETURN VALUE
  Service object

DEPENDENCIES
  Shim layer has been initialized. 

SIDE EFFECTS
  None.
===========================================================================*/
qmi_idl_service_object_type qmux_qcsi_shim_get_svc_obj
(
  qmux_service_e_type service
)
{

  switch (service)
  {
    case QMUX_SERVICE_WDS:
      return wds_get_service_object_v01();
    case QMUX_SERVICE_DMS: 
      return dms_get_service_object_v01();
    case QMUX_SERVICE_NAS: 
      return nas_get_service_object_v01();
    case QMUX_SERVICE_QOS: 
      return qos_get_service_object_v01();
    case QMUX_SERVICE_WMS: 
      return wms_get_service_object_v01();
    case QMUX_SERVICE_AUTH: 
      return auth_get_service_object_v01();
    case QMUX_SERVICE_AT: 
      return at_get_service_object_v01();
    case QMUX_SERVICE_VOICE: 
      return voice_get_service_object_v02();
    case QMUX_SERVICE_CAT: 
      return cat_get_service_object_v02();
    case QMUX_SERVICE_UIM: 
      return uim_get_service_object_v01();
    case QMUX_SERVICE_PBM: 
      return pbm_get_service_object_v01();
    case QMUX_SERVICE_WDA: 
      return wda_get_service_object_v01();
    case QMUX_SERVICE_CSVT: 
      return csvt_get_service_object_v01();
    default:     
      ASSERT(0);
	  return NULL;
  }
}
/*===========================================================================
FUNCTION QMUX_QCSI_SHIM_PROCESS_REG()

DESCRIPTION
  This function processes the QMUX service registration with QCSI in
  QMI modem task. 
  
PARAMETERS
  service       : QMUX service ID.
  
RETURN VALUE
  None

DEPENDENCIES
  Shim layer has been initialized. 

SIDE EFFECTS
  None.
===========================================================================*/
static void qmux_qcsi_shim_process_reg
( 
  qmux_service_e_type     service
)
{
  qmux_qcsi_shim_err_e_type err = QMUX_QCSI_SHIM_ERR_NONE;
  int                       svc_idx = 0;
  qmi_csi_options           options;
  qmi_csi_error             rc;
  qmux_qcsi_shim_svc_info_type * svc_info = NULL;

  ASSERT (service < QMUX_SERVICE_MAX);
  
  do
  {
    if (!qmux_qcsi_shim_powerup_inited)
    {
      err = QMUX_QCSI_SHIM_ERR_SHIM_NOT_READY;
      break;
    }
    svc_info = (qmux_qcsi_shim_svc_info_type*)q_linear_search( &qmux_qcsi_shim_svc_q,
                                 qmux_qcsi_shim_compare_qmux_service_id,
                                 (void*)service );
    if (svc_info != NULL)
    {
      err = QMUX_QCSI_SHIM_ERR_SVC_ALREADY_REGISTERED;
      break;
    }
  
    PS_SYSTEM_HEAP_MEM_ALLOC(svc_info, 
                             sizeof(qmux_qcsi_shim_svc_info_type),
                             qmux_qcsi_shim_svc_info_type*);

    if (svc_info == NULL)
    {
      err = QMUX_QCSI_SHIM_ERR_NO_MEMORY;
      break;
    }

    memset(svc_info, 0, sizeof(qmux_qcsi_shim_svc_info_type));

    /* Identify a free signal */
    for (svc_idx = 0; svc_idx < QMUX_QCSI_SHIM_MAX_SVCS; svc_idx++)
    {
      if ((qmux_qcsi_shim_svc_sig_in_use_mask & (1 << svc_idx)) == 0)
      {
        svc_info->svc_sig = QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_1 + svc_idx;
        qmux_qcsi_shim_svc_sig_in_use_mask |= (1 << svc_idx);
        break;
      }
    }

    if ((svc_idx == QMUX_QCSI_SHIM_MAX_SVCS) || 
        (svc_info->svc_sig >= QMI_MAX_SIGNALS))
    {
      err = QMUX_QCSI_SHIM_ERR_NO_SIGS;
      break;
    }

    svc_info->os_params.tcb = &qmi_modem_tcb;
    svc_info->os_params.sig = 
      ((rex_sigs_type)(1 << svc_info->svc_sig));
    /*-------------------------------------------------------------------------
      Setting QMI MODEM TASK signal handler for handling QCSI messages
    -------------------------------------------------------------------------*/
    (void)qmi_set_sig_handler(svc_info->svc_sig, 
                              qmux_qcsi_shim_msg_hdlr_cb, 
                              svc_info);

    qmi_enable_sig(svc_info->svc_sig);

    /*-------------------------------------------------------------------------
    Register QMUX service with QCSI with option to handle message raw and 
    asynchronously inform result of client connect 
    -------------------------------------------------------------------------*/
    QMI_CSI_OPTIONS_INIT(options);
    QMI_CSI_OPTIONS_SET_PRE_REQUEST_CB(options, qmux_qcsi_shim_svc_handle_raw_req_cb);
    QMI_CSI_OPTIONS_SET_ASYNC_CONNECT_CB(options);
    (void)mqcsi_register_log_options(&options);
    rc = qmi_csi_register_with_options(qmux_qcsi_shim_get_svc_obj(service),
                          qmux_qcsi_shim_svc_connect_cb,
                          qmux_qcsi_shim_svc_disconnect_cb, 
                          qmux_qcsi_shim_svc_handle_raw_req_cb, 
                          svc_info, 
                          &svc_info->os_params,
                          &options, 
                          &svc_info->service_handle);   
    if(rc != QMI_CSI_NO_ERR)
    {
      ERR_FATAL("Unable to register QMUX service %d with QCSI! Error %d", service, rc, 0);
      err = QMUX_QCSI_SHIM_ERR_QCSI_REG;
      break;
    }
    
    svc_info->service_id = service; 
    /* Initialize the queue which will hold all handles of clients */
    (void)q_init(&svc_info->client_q);
    /* Initialize the queue which will hold requests for which the resp is pending */
    (void)q_init(&svc_info->pending_requests);

    /* Add this service information block to queue of services */
    (void) q_link ( svc_info, &svc_info->link );
    q_put( &(qmux_qcsi_shim_svc_q), &(svc_info->link) );
  }while(0);

  if(err != QMUX_QCSI_SHIM_ERR_NONE)
  {
    LOG_MSG_ERROR_2("QMUX QCSI shim service reg failed for service = %d err = %d",service,err);

    /* Free memory created for the service */
    PS_SYSTEM_HEAP_MEM_FREE(svc_info);
    ASSERT(0);
  }
} /* qmux_qcsi_shim_process_reg */

/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_MSG_HDLR_CB()

  DESCRIPTION
    CallBack function invoked when signal is set by QCSI

  PARAMETERS
    sig : Signal set
    svc_ptr : Service pointer (user data ptr) thats associated with it

  RETURN VALUE
    TRUE - SUCCESS

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmux_qcsi_shim_msg_hdlr_cb
(
  qmi_sig_enum_type  sig,
  void               *svc_ptr
)
{
  qmux_qcsi_shim_svc_info_type *svc_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT((sig >= QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_1) && 
         (sig <= QMI_QMUX_QCSI_SHIM_SVC_SIGNAL_13) &&
         svc_ptr);

  svc_sp = (qmux_qcsi_shim_svc_info_type *) svc_ptr;

  qmi_csi_handle_event(svc_sp->service_handle, &svc_sp->os_params);
  return TRUE;
} /* qmux_qcsi_shim_msg_hdlr_cb */


/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_SVC_CONNECT_CB()

  DESCRIPTION
    CallBack function to be registered with QCSI for connecting a client

  PARAMETERS
    Client Handle
    Service Handle
    Connection Handle

  RETURN VALUE
    QMI_ERR_NONE - Success
    QMI_CSI_CB_CONN_REFUSED - Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmux_qcsi_shim_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmux_qcsi_shim_clnt_info_type     *clnt_info_ptr;
  qmux_qcsi_shim_svc_info_type      *svc_ptr;
  qmi_csi_cb_error                  rc;
  qmi_error_e_type                  retval = QMI_ERR_NONE;
  qmi_framework_common_msg_hdr_type common_msg_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT(connection_handle && service_handle);

  svc_ptr = (qmux_qcsi_shim_svc_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &svc_ptr->client_q,
                                qmux_qcsi_shim_compare_qmi_client_handle,
                                client_handle ) )
  {
    LOG_MSG_ERROR_1("connect_cb: client 0x%p present",
                    client_handle);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(clnt_info_ptr, sizeof(qmux_qcsi_shim_clnt_info_type),
                           qmux_qcsi_shim_clnt_info_type*);
  if(!clnt_info_ptr)
  {
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }
  memset(clnt_info_ptr, 0, sizeof(qmux_qcsi_shim_clnt_info_type));
  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;

  /* Call QMUX framework to allocate QMUX client id and inform service */
  memset(&common_msg_hdr, 0, sizeof(common_msg_hdr));

  common_msg_hdr.service = svc_ptr->service_id;
  common_msg_hdr.qmi_instance = QMI_INSTANCE_SHIM;
  common_msg_hdr.transaction_id = ++clnt_info_ptr->tx_id; /* start with non zero txn */

  retval = qmi_framework_svc_alloc_clid(&common_msg_hdr);
  if(retval != QMI_ERR_NONE)
  {
    PS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }
  /* Extract client id picked by QMUX framework */
  clnt_info_ptr->qmux_client_id = common_msg_hdr.client_id;
  /* Add client node to the queue of clients */
  q_put( &(svc_ptr->client_q), &(clnt_info_ptr->link) );

  *connection_handle = clnt_info_ptr;
  LOG_MSG_INFO1_2("connect_cb: Alloc client 0x%p QMUX cl_id %d", 
                  clnt_info_ptr, clnt_info_ptr->qmux_client_id);
  rc = QMI_CSI_CB_NO_ERR;
bail:
  return rc;
} /* qmux_qcsi_shim_svc_connect_cb() */

/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_PROCESS_CL_ALLOC_RESULT()

  DESCRIPTION
    Handle success or failure for client creation with QMUX service in QMI
    modem task

  PARAMETERS
    service : QMUX service ID
    qmux_client_id : Client ID that was previously identified by QMUX service
    reg_result : SUCCESS or FAILURE

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmux_qcsi_shim_process_cl_alloc_result
(
  qmux_service_e_type   service,
  uint8                 qmux_client_id, 
  qmi_result_e_type     reg_result
)
{
  qmux_qcsi_shim_clnt_info_type     *clnt_info_ptr;
  qmux_qcsi_shim_svc_info_type      *svc_ptr;
  uint32                            clid = qmux_client_id;
  uint8                             accepted;

  ASSERT(service < QMUX_SERVICE_MAX); 
  svc_ptr = (qmux_qcsi_shim_svc_info_type *) q_linear_search( &qmux_qcsi_shim_svc_q,
                                 qmux_qcsi_shim_compare_qmux_service_id,
                                 (void*)service );
  if (NULL == svc_ptr)
  {
    ASSERT(0);
    return;
  }
 
  clnt_info_ptr = q_linear_search( &svc_ptr->client_q,
                                   qmux_qcsi_shim_compare_qmi_client_id,
                                   (void*)clid );
  if (NULL == clnt_info_ptr)
  {
    LOG_MSG_ERROR_2("process_cl_alloc_result: client %d not found on service %d",
                    qmux_client_id, service);
    ASSERT(0);
    return;
  }

  accepted = (reg_result == QMI_RESULT_SUCCESS) ? 1 : 0;
  /*TODO: Check if result is failure do we need to cleanup*/

  /* Inform QCSI asynchronously of SUCCESS/FAILURE */
  qmi_csi_conn_complete(clnt_info_ptr->clnt, accepted);

} /* qmux_qcsi_shim_process_cl_alloc_result() */

/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_SVC_DISCONNECT_CB()

  DESCRIPTION
    CallBack function to be registered with QCSI for disconnecting a client

  PARAMETERS
    connection_handle : QCSI client connection handle
    service_handle : QCSI service handle

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmux_qcsi_shim_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmux_qcsi_shim_clnt_info_type     *clnt_info_ptr;
  qmux_qcsi_shim_svc_info_type      *svc_ptr;
  qmi_framework_common_msg_hdr_type common_msg_hdr;
  qmi_error_e_type                  retval = QMI_ERR_NONE;
  qmux_qcsi_shim_err_e_type         err = QMUX_QCSI_SHIM_ERR_NONE;
  qmux_qcsi_shim_pending_req_type * pending_req = NULL;
  qmux_qcsi_shim_pending_req_type * temp_ptr = NULL;
  int num_deleted_pending_txs = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle && service_handle);
  svc_ptr = (qmux_qcsi_shim_svc_info_type *)service_handle;
  clnt_info_ptr = (qmux_qcsi_shim_clnt_info_type *)connection_handle;

  do
  {
    if ( TRUE != q_delete_ext( &svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
    {
      err = QMUX_QCSI_SHIM_ERR_CL_NOT_FOUND;
      break;
    }

   /* Delete any pending transactions for this client */
   if(q_cnt(&svc_ptr->pending_requests) > 0)
   {
      do
      {
        pending_req = q_linear_delete_new( &svc_ptr->pending_requests,
                                          qmux_qcsi_shim_compare_pending_req_for_client,
                                          (void*)&clnt_info_ptr->qmux_client_id,
                                           NULL,
                                           NULL );
         if (pending_req != NULL)
         {
           temp_ptr = pending_req;
           PS_SYSTEM_HEAP_MEM_FREE(temp_ptr);
           num_deleted_pending_txs++;
         }
       }while(pending_req != NULL);

       if(num_deleted_pending_txs > 0)
       {
          LOG_MSG_INFO1_3("disconnect_cb: Deleted %d pending txs of clid %d & svc: %d ",
                         num_deleted_pending_txs, 
                         clnt_info_ptr->qmux_client_id,
                         svc_ptr->service_id);
       }
    }

    /* Inform QMUX framework and service also of client deletion */
    common_msg_hdr.service = svc_ptr->service_id;
    common_msg_hdr.qmi_instance = QMI_INSTANCE_SHIM;
    common_msg_hdr.client_id = clnt_info_ptr->qmux_client_id;
    /* Let tx_id be 0 since dealloc result wont be sent to QMI_CTL and we
       dont care*/
    common_msg_hdr.transaction_id = 0; 

    retval = qmi_framework_svc_dealloc_clid(&common_msg_hdr);
    if (retval != QMI_ERR_NONE)
    {
      err = QMUX_QCSI_SHIM_ERR_FRAMEWORK_ERR;
      /* Free the memory anyway even if framework returned an error */
    }
    PS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  } while(0);

  LOG_MSG_INFO1_4("disconnect_cb: Releasing client 0x%p from service %d err %d retval %d",
                  connection_handle, svc_ptr->service_id, err, retval);
} /* qmux_qcsi_shim_svc_disconnect_cb() */

/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_SVC_HANDLE_RAW_REQ_CB()

  DESCRIPTION
    CallBack function to be registered with QCSI for handling raw client request

  PARAMETERS
    connection_handle : Connection handle for client
    req_handle : Handle identifying request
    msg_id : Message ID
    raw_msg : Pointer to raw message
    raw_msg_len : Length of raw message
    service_handle : Service handle

  RETURN VALUE
    QMI_CSI_CB_REQ_HANDLED on SUCCESS, appropriate failure code otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmux_qcsi_shim_svc_handle_raw_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *raw_msg,
  unsigned int             raw_msg_len,
  void                     *service_handle
)
{
  qmi_csi_cb_error                 rc = QMI_CSI_CB_REQ_HANDLED;
  qmux_qcsi_shim_clnt_info_type   *clnt_info_ptr;
  qmux_qcsi_shim_svc_info_type    *svc_ptr;
  qmux_qcsi_shim_err_e_type        err = QMUX_QCSI_SHIM_ERR_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle && service_handle);

  svc_ptr = (qmux_qcsi_shim_svc_info_type*) service_handle;
  do
  {
    clnt_info_ptr = q_linear_search( &svc_ptr->client_q,
                                     qmux_qcsi_shim_compare_connection_info_handle,
                                     connection_handle );
    if (NULL == clnt_info_ptr)
    {
      rc = QMI_CSI_CB_CONN_REFUSED;
      err = QMUX_QCSI_SHIM_ERR_CL_NOT_FOUND;
      break;
    }

    /*----------------------------------------------------------------------
       Client alloc has already been successfully processed by QMUX service,
       send the request message onwards to the service
       ---------------------------------------------------------------------*/
    err = qmux_qcsi_shim_send_raw_msg(svc_ptr,
                                      clnt_info_ptr,
                                      req_handle,
                                      msg_id,
                                      raw_msg,
                                      raw_msg_len);
    if (err != QMUX_QCSI_SHIM_ERR_NONE)
    {
      if (err == QMUX_QCSI_SHIM_ERR_NO_MEMORY)
      {
        rc = QMI_CSI_CB_NO_MEM;
      }
      else
      {
        rc = QMI_CSI_CB_INTERNAL_ERR;
      }
      break;
    }
  } while(0);

  if (err != QMUX_QCSI_SHIM_ERR_NONE)
  {  
    LOG_MSG_ERROR_5("handle_raw_req_cb: Error processing req"
                    "err = %d connection handle %p svc handle %p service id %d msg id %d",
                    err, connection_handle, service_handle, svc_ptr->service_id, msg_id);
  }
  else
  {
    LOG_MSG_INFO2_5("handle_raw_req_cb: Handled req "
                    "connection handle %p req_handle %p cl id %d service id %d msg id %d ",
                    connection_handle, req_handle, clnt_info_ptr->qmux_client_id, svc_ptr->service_id, msg_id);
  }

  return rc;
} /* qmux_qcsi_shim_svc_handle_raw_req_cb() */

/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_SEND_RAW_MSG()

  DESCRIPTION
    Constructs QMUX SDU from raw message and sends it to QMUX framework

  PARAMETERS
    svc_info_ptr : Service info
    clnt_info_ptr : Client info
    req_handle : Handle identifying request
    msg_id : Message ID
    raw_msg : Pointer to raw message
    raw_msg_len : Length of raw message

  RETURN VALUE
    QMUX_QCSI_SHIM_ERR_NONE on SUCCESS, appropriate failure code otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static qmux_qcsi_shim_err_e_type qmux_qcsi_shim_send_raw_msg
(
  qmux_qcsi_shim_svc_info_type   *svc_info_ptr,
  qmux_qcsi_shim_clnt_info_type  *clnt_info_ptr,
  qmi_req_handle                  req_handle,
  unsigned int                    msg_id,
  void                           *raw_msg,
  unsigned int                    raw_msg_len
)
{
  qmux_qcsi_shim_err_e_type        err = QMUX_QCSI_SHIM_ERR_NONE;
  qmux_sdu_hdr_type                sdu_hdr;
  dsm_item_type                   *qmux_req_msg = NULL;
  qmux_qcsi_shim_pending_req_type *pending_req = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&sdu_hdr, 0, sizeof(qmux_sdu_hdr_type));
  /* ctl_flags dont need to be set since "Command" = 0 */
  sdu_hdr.len = raw_msg_len;
  sdu_hdr.tx_id = ++clnt_info_ptr->tx_id;
  /* tx_id is uint16 which overflow to 0 if a client sends 65535 messages.
     As tx_id should be non-zero. So restart sequence from 1 */
  if(sdu_hdr.tx_id == 0)
  {
    sdu_hdr.tx_id = ++clnt_info_ptr->tx_id;
  }
  sdu_hdr.msg_id = msg_id;
  do
  {
    /* Push raw message onto dsm item */
    if ( raw_msg_len != PS_UTILS_DSM_PUSHDOWN_PACKED( &qmux_req_msg,
                                     raw_msg,
                                     raw_msg_len,
                                     DSM_DS_SMALL_ITEM_POOL) )
    {
      err = QMUX_QCSI_SHIM_ERR_NO_MEMORY;
      break;
    }
    /* Push SDU header onto dsm item */
    if ( sizeof(qmux_sdu_hdr_type) != PS_UTILS_DSM_PUSHDOWN_PACKED( &qmux_req_msg,
                                                  &sdu_hdr,
                                                  sizeof(qmux_sdu_hdr_type),
                                                  DSM_DS_SMALL_ITEM_POOL) )
    {
      err = QMUX_QCSI_SHIM_ERR_NO_MEMORY;
      break;
    }
    /* Create node for pending request queue */
    PS_SYSTEM_HEAP_MEM_ALLOC(pending_req, sizeof(qmux_qcsi_shim_pending_req_type),
                             qmux_qcsi_shim_pending_req_type*);
    if(!pending_req)
    {
      err = QMUX_QCSI_SHIM_ERR_NO_MEMORY;
      break;
    }
    memset(pending_req, 0, sizeof(qmux_qcsi_shim_pending_req_type));
    (void) q_link ( pending_req, &pending_req->link );
    pending_req->req_handle = req_handle;
    pending_req->msg_id = msg_id;
    pending_req->qmux_client_id = clnt_info_ptr->qmux_client_id;
    pending_req->tx_id = clnt_info_ptr->tx_id;
    q_put( &(svc_info_ptr->pending_requests), &(pending_req->link) );
    if (TRUE != qmi_framework_svc_recv(QMI_INSTANCE_SHIM,
                                       svc_info_ptr->service_id,
                                       clnt_info_ptr->qmux_client_id,
                                       qmux_req_msg) )
    {
      err = QMUX_QCSI_SHIM_ERR_FRAMEWORK_ERR;
      q_delete(&(svc_info_ptr->pending_requests), &(pending_req->link));
      break;
    }
  }while(0);

  if (err != QMUX_QCSI_SHIM_ERR_NONE)
  {
    dsm_free_packet (&qmux_req_msg);
    PS_SYSTEM_HEAP_MEM_FREE(pending_req);
  }
  return err;
}


/*===========================================================================
  FUNCTION QMUX_QCSI_SHIM_SEND_RESP_IND()

  DESCRIPTION
    Sends the response or indication from QMUX framework to QCSI

  PARAMETERS
    common_hdr : Contains QMUX header parameters
    dsm_item : Ptr to response/indication contents
    msg_type : Either QMI_FLAG_MSGTYPE_RESP or QMI_FLAG_MSGTYPE_IND

  RETURN VALUE
    TRUE on SUCCESS, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean qmux_qcsi_shim_send_resp_ind
(
  qmi_framework_common_msg_hdr_type common_hdr, 
  dsm_item_type                     *dsm_item, 
  uint8                              msg_type
)
{
  int32                              temp;
  uint16                             msg_id = 0, msg_len = 0;
  qmux_qcsi_shim_err_e_type          err = QMUX_QCSI_SHIM_ERR_NONE;
  qmux_qcsi_shim_svc_info_type       *svc_info_ptr = NULL;
  qmux_qcsi_shim_clnt_info_type      *clnt_info_ptr = NULL;
  void                               *raw_msg = NULL;
  qmi_csi_error                      qcsi_err = QMI_CSI_NO_ERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(dsm_item && common_hdr.service < QMUX_SERVICE_MAX);

  svc_info_ptr = (qmux_qcsi_shim_svc_info_type *) q_linear_search( &qmux_qcsi_shim_svc_q,
                                 qmux_qcsi_shim_compare_qmux_service_id,
                                 (void*)common_hdr.service );
  if (NULL == svc_info_ptr)
  {
    ASSERT(0);
    return FALSE;
  }
  do
  {
    /* DSM item contains msg id and length, in addition to raw payload */
    /* Extract msg id first */
    temp = dsm_pull16(&dsm_item); 

    if (temp == -1) 
    {
      err = QMUX_QCSI_SHIM_ERR_MSG_ID_MISS;
      break;
    }
    msg_id = ps_ntohs(temp);

    /* Extract msg len */
    temp = dsm_pull16(&dsm_item);
    if (temp == -1) 
    {
      err = QMUX_QCSI_SHIM_ERR_MSG_LEN_MISS;
      break;
    }
    msg_len = ps_ntohs(temp);

    if (common_hdr.client_id != QMI_SVC_CLID_BROADCAST)
    {
      uint32 clid = common_hdr.client_id;
      clnt_info_ptr = q_linear_search( &svc_info_ptr->client_q,
                                      qmux_qcsi_shim_compare_qmi_client_id,
                                      (void*)clid );
      if (NULL == clnt_info_ptr)
      {
        err = QMUX_QCSI_SHIM_ERR_CL_NOT_FOUND;
        break;
      }
    }

    if (msg_len > 0)
    {
      PS_SYSTEM_HEAP_MEM_ALLOC(raw_msg, msg_len, void*);
      if(!raw_msg)
      {
        err = QMUX_QCSI_SHIM_ERR_NO_MEMORY;
        break;
      }
      memset(raw_msg, 0, msg_len);
      if (msg_len != PS_UTILS_DSM_PULLUP(&dsm_item, raw_msg, msg_len))
      {
        err = QMUX_QCSI_SHIM_ERR_MALFORMED_MSG;
        break;
      }
    }
    
 
    if (msg_type == QMI_FLAG_MSGTYPE_IND)
    {
      if (common_hdr.client_id == QMI_SVC_CLID_BROADCAST)
      {
        qcsi_err = qmi_csi_send_broadcast_ind_raw(svc_info_ptr->service_handle,
                                                  msg_id,
                                                  raw_msg,
                                                  msg_len);
      }
      else
      {
        qcsi_err = qmi_csi_send_ind_raw(clnt_info_ptr->clnt,
                                        msg_id,
                                        raw_msg,
                                        msg_len);
      }
    }
    else
    {
      qmux_qcsi_shim_pending_req_type * pending_req = NULL;
      qmux_qcsi_shim_pending_req_key_type key;

      key.qmux_client_id = common_hdr.client_id;
      key.tx_id = common_hdr.transaction_id;

      /* Identify handle from queue of pending requests and delete from queue */
      pending_req = q_linear_delete_new( &svc_info_ptr->pending_requests,
                                     qmux_qcsi_shim_compare_pending_req,
                                     &key,
                                     NULL,
                                     NULL );
      if (NULL == pending_req)
      {
        err = QMUX_QCSI_SHIM_ERR_REQ_HANDLE_NOT_FOUND;
        break;
      }
      qcsi_err = qmi_csi_send_resp_raw(pending_req->req_handle,
                                       msg_id,
                                       raw_msg,
                                       msg_len);

      /* Free memory */
      PS_SYSTEM_HEAP_MEM_FREE(pending_req);
    }
    if (qcsi_err != QMI_CSI_NO_ERR)
    {
      err = QMUX_QCSI_SHIM_ERR_QCSI_SEND;
      LOG_MSG_ERROR_1("QCSI error in send %d", qcsi_err)
      break;
    }
  } while(0);
  dsm_free_packet(&dsm_item); //Is this needed??
  PS_SYSTEM_HEAP_MEM_FREE(raw_msg);

  if (err == QMUX_QCSI_SHIM_ERR_NONE)
  {
    LOG_MSG_INFO2_5("Sent %d to QMUX client %d"
                      " Service %d Msg ID %d tx_id %d ",
                      msg_type, common_hdr.client_id, common_hdr.service,
                      msg_id, common_hdr.transaction_id);
    return TRUE;
  }
  else
  {
    LOG_MSG_ERROR_6("Error %d encountered sending %d"
                    "to QMUX client %d Service %d Msg ID %d tx_id %d ",
                      err, msg_type, common_hdr.client_id, common_hdr.service,
                      msg_id, common_hdr.transaction_id);
    return FALSE;
  }
} /* qmux_qcsi_shim_send_resp_ind*/
