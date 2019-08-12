/*==============================================================================

    Q M I   P E R S I S T E N T   D E V I C E   C O N F I G   S E R V I C E

GENERAL DESCRIPTION
  Persistent Device Configuration (PDC) QMI service implementation

Copyright (c) 2012-2015 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_qmi/src/qmi_pdc_svc.c#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
01/28/13      Added support to query commands/TLVs supported by service 
04/30/10 mj   Initial creation

==============================================================================*/

#include "comdef.h"

#include "msg.h"

#include "qmi_csi.h"
#include "qmi_sap.h"

#include "persistent_device_configuration_v01.h"
#include "qmi_pdc_msg.h"
#include "persistent_device_configuration_impl_v01.h"

#include "mcfg_osal.h"
#include "mcfg_int.h"
#include "mcfg_utils.h"
#include "mcfg_common.h"
#include "mcfg_trl.h"
#include "mcfg_validate.h"
#include "mcfg_refresh_i.h"
#include "mcfg_uim.h"
#include "mcfg_setting.h"

/* Added for dummy XML*/
#include "fs_public.h"                          /* For handling EFS files */
#include "fs_sys_types.h"
/* -------------------*/
#include <string.h>
#include "mqcsi_log.h"

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
#define PDC_CLIENTS_MAX 8

/* Macro to return result based on a given qmi_error_type_v01 */
#define QMI_RESULT_FROM_ERROR(error) (error == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 \
                                                                : QMI_RESULT_FAILURE_V01)

/* Macro to validate config type */
#define IS_VALID_PDC_CONFIG_TYPE(type) ((type) == PDC_CONFIG_TYPE_MODEM_PLATFORM_V01 || \
                                        (type) == PDC_CONFIG_TYPE_MODEM_SW_V01)

/* Macro to validate config id len */
#define IS_VALID_PDC_CONFIG_ID_LEN(len) ((len) != 0 && (len) <= PDC_CONFIG_ID_SIZE_MAX_V01)

/* Macro to validate config storage type */
#define IS_VALID_PDC_STORAGE_TYPE(fs) ((fs) == PDC_STORAGE_LOCAL_V01 || \
                                       (fs) == PDC_STORAGE_REMOTE_V01)          

/* Macro to validate subscription ID */
#define IS_VALID_MCFG_SUBSCRIPTION_ID(sub) (MCFG_SUB_ID_FIRST <= (sub) && (sub) < MCFG_NUM_OF_SUB_ID)

/* Macro to validate Slot ID */
#define IS_VALID_MCFG_SLOT_ID(slot) (MCFG_SLOT_INDEX_FIRST <= (slot) && (slot) < MCFG_NUM_OF_UIM_SLOT)

#ifdef _WIN32
#define UNUSED_ATTRIBUTE 
#else
#define UNUSED_ATTRIBUTE __attribute__((unused))
#endif

/*==============================================================================
  Typedefs
==============================================================================*/
/* PDC responses */
typedef union
{
  qmi_response_type_v01                resp;
  pdc_reset_resp_msg_v01               reset;
  pdc_indication_register_resp_msg_v01 indication_register;
  pdc_get_selected_config_resp_msg_v01 get_selected_config;
  pdc_set_selected_config_resp_msg_v01 set_selected_config;
  pdc_list_configs_resp_msg_v01        list_configs;
  pdc_delete_config_resp_msg_v01       delete_config;
  pdc_load_config_resp_msg_v01         load_config;
  pdc_activate_config_resp_msg_v01     activate_config;
  pdc_get_config_info_resp_msg_v01     get_config_info;
  pdc_get_config_limits_resp_msg_v01   get_config_limits;
} pdc_general_resp_v01_type;

/* PDC load config cached information */
typedef struct 
{
  qmi_client_handle     qmi_client_handle;

  mcfg_storage_mode_e_type storage;
  mcfg_config_type_e_type type;
  mcfg_config_id_s_type id_s;

  uint32                offset;
  mcfg_config_s_type    config_s;
} pdc_load_config_cache_type;

/* PDC activate cached information */
typedef struct 
{
  qmi_client_handle     qmi_client_handle;
  uint8_t               ind_token_valid;
  uint32_t              ind_token;
} pdc_activate_cache_type;

/* PDC client info - event registration information */
typedef struct 
{
  boolean            selected_config;
} pdc_event_reg_type;

/* PDC client info */
typedef struct 
{
  qmi_client_handle  qmi_client_handle;
  boolean            connected;
  pdc_event_reg_type event_reg;
} pdc_client_type;

/* PDC cache of last-sent indications - used to remove duplicate IND's from
   being sent */
typedef struct
{
  boolean                              config_change_valid;
  pdc_config_change_ind_msg_v01        config_change;
  uint32                               config_change_len;
} pdc_last_ind_type;

/* PDC service info */
typedef struct 
{
  /* Individual clients */
  pdc_client_type pdc_clients[PDC_CLIENTS_MAX];
  
  /* Service handler from QCSI */
  qmi_csi_service_handle pdc_service_handler;
  
  /* Allocate response an indication message bodies outside of stack space */
  pdc_general_resp_v01_type     pdc_resp;
  pdc_last_ind_type             pdc_ind;

  pdc_load_config_cache_type    load_config_cache;

  pdc_activate_cache_type       activate_cache[MCFG_NUM_OF_UIM_SLOT];
} pdc_service_type;

/* request handler fucntion prototype */
typedef qmi_csi_cb_error (* const pdc_service_req_handler)
                         (void *connection_handle, qmi_req_handle req_handle, unsigned int msg_id, void *req_c_struct, unsigned int req_c_struct_len, void *service_cookie);

/*==============================================================================
  Variables
==============================================================================*/
/* Global QMI_PDC service data */
static pdc_service_type global_pdc_svc;


/*============================================================================== 
 
                 PRIVATE FUNCTION FORWARD DECLARATIONS
 
==============================================================================*/
/* message handler functions */
static qmi_csi_cb_error pdc_handle_reset_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_indication_register_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_get_selected_config_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_set_selected_config_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_list_configs_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_delete_config_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_load_config_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_activate_config_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_deactivate_config_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_get_config_info_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_get_config_limits_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_get_default_config_info_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_validate_config_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_get_feature_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static qmi_csi_cb_error pdc_handle_set_feature_req(void *, qmi_req_handle, unsigned int, void *, unsigned int, void *)UNUSED_ATTRIBUTE;
static void pdc_send_get_selected_config_ind(pdc_client_type *, pdc_get_selected_config_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_send_set_selected_config_ind(pdc_client_type *, pdc_set_selected_config_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_send_list_configs_ind(pdc_client_type *, pdc_list_configs_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_send_delete_config_ind(pdc_client_type *, pdc_delete_config_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_send_load_config_ind(pdc_client_type *, pdc_load_config_req_msg_v01 *, pdc_load_config_cache_type  *)UNUSED_ATTRIBUTE;
static void pdc_send_activate_config_ind(pdc_client_type *, pdc_activate_config_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_send_deactivate_config_ind(pdc_client_type *, pdc_deactivate_config_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_send_get_config_info_ind(pdc_client_type *, pdc_get_config_info_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_send_get_config_limits_ind(pdc_client_type *, pdc_get_config_limits_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_default_load_config_cache(pdc_load_config_cache_type *)UNUSED_ATTRIBUTE;
static void pdc_send_get_default_config_info_ind(pdc_client_type *, pdc_get_default_config_info_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_send_validate_config_ind(pdc_client_type *, pdc_validate_config_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_send_get_feature_ind(pdc_client_type *, pdc_get_feature_req_msg_v01 *)UNUSED_ATTRIBUTE;
static void pdc_send_set_feature_ind(pdc_client_type *, pdc_set_feature_req_msg_v01 *)UNUSED_ATTRIBUTE;

/* utility functions for the service */
static pdc_service_type * pdc_get_service_state(void);

/* Table to handle the dispatch of request handler functions. */
static pdc_service_req_handler pdc_service_req_handle_table[] =
{
#ifndef REMOVE_QMI_PDC_RESET_V01
  pdc_handle_reset_req,                    /* Request handler for message ID 0x0000 */
  pdc_handle_reset_req,                    /* Request handler for message ID 0x0001 */  // TODO: remove!
  //NULL,                                  /* Request handler for message ID 0x0001 */  // TODO: put back!
#else
  NULL,
  NULL,
#endif
  NULL,                                    /* Request handler for message ID 0x0002 */
  NULL,                                    /* Request handler for message ID 0x0003 */
  NULL,                                    /* Request handler for message ID 0x0004 */
  NULL,                                    /* Request handler for message ID 0x0005 */
  NULL,                                    /* Request handler for message ID 0x0006 */
  NULL,                                    /* Request handler for message ID 0x0007 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x0008 - 0x000F*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x0010 - 0x0017*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x0018 - 0x001F*/
#ifndef REMOVE_QMI_PDC_INDICATION_REGISTER_V01
  pdc_handle_indication_register_req,      /* Request handler for message ID 0x0020 */
#else
  NULL,                                    /* Request handler for message ID 0x0020 */
#endif
  NULL,                                    /* Request handler for message ID 0x0021 */
#ifndef REMOVE_QMI_PDC_GET_SELECTED_CONFIG_V01
  pdc_handle_get_selected_config_req,      /* Request handler for message ID 0x0022 */
#else
  NULL,                                    /* Request handler for message ID 0x0022 */
#endif
#ifndef REMOVE_QMI_PDC_SET_SELECTED_CONFIG_V01
  pdc_handle_set_selected_config_req,      /* Request handler for message ID 0x0023 */
#else
  NULL,                                    /* Request handler for message ID 0x0023 */
#endif
#ifndef REMOVE_QMI_PDC_LIST_CONFIGS_V01
  pdc_handle_list_configs_req,             /* Request handler for message ID 0x0024 */
#else
  NULL,                                    /* Request handler for message ID 0x0024 */
#endif
#ifndef REMOVE_QMI_PDC_DELETE_CONFIG_V01
  pdc_handle_delete_config_req,            /* Request handler for message ID 0x0025 */
#else
  NULL,                                    /* Request handler for message ID 0x0025 */
#endif
#ifndef REMOVE_QMI_PDC_LOAD_CONFIG_V01
  pdc_handle_load_config_req,              /* Request handler for message ID 0x0026 */
#else
  NULL,                                    /* Request handler for message ID 0x0026 */
#endif
#ifndef REMOVE_QMI_PDC_ACTIVATE_CONFIG_V01
  pdc_handle_activate_config_req,          /* Request handler for message ID 0x0027 */
#else
  NULL,                                    /* Request handler for message ID 0x0027 */
#endif
#ifndef REMOVE_QMI_PDC_GET_CONFIG_INFO_V01
  pdc_handle_get_config_info_req,          /* Request handler for message ID 0x0028 */
#else
  NULL,                                    /* Request handler for message ID 0x0028 */
#endif
#ifndef REMOVE_QMI_PDC_GET_CONFIG_LIMITS_V01
  pdc_handle_get_config_limits_req,        /* Request handler for message ID 0x0029 */
#else
  NULL,                                    /* Request handler for message ID 0x0029 */
#endif
#ifndef REMOVE_QMI_PDC_GET_DEFAULT_CONFIG_INFO_V01
  pdc_handle_get_default_config_info_req,  /* Request handler for message ID 0x002A */
#else
  NULL,                                    /* Request handler for message ID 0x002A */
#endif
#ifndef REMOVE_QMI_PDC_DEACTIVATE_CONFIG_V01
  pdc_handle_deactivate_config_req,        /* Request handler for message ID 0x002B */
#else
  NULL,                                    /* Request handler for message ID 0x002B */
#endif
#ifndef REMOVE_QMI_PDC_VALIDATE_CONFIG_V01
  pdc_handle_validate_config_req,          /* Request handler for message ID 0x002C */
#else
  NULL,                                    /* Request handler for message ID 0x002C */
#endif
#ifndef REMOVE_QMI_PDC_GET_FEATURE_V01
  pdc_handle_get_feature_req,              /* Request handler for message ID 0x002D */
#else
  NULL,                                    /* Request handler for message ID 0x002D */
#endif
#ifndef REMOVE_QMI_PDC_SET_FEATURE_V01
  pdc_handle_set_feature_req,              /* Request handler for message ID 0x002E */
#else
  NULL,                                    /* Request handler for message ID 0x002E */
#endif
};


/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

  FUNCTION pdc_connect_cb

  DESCRIPTION
    Callback when a client connects to the server

  DEPENDENCIES
    None

  PARAMETERS
    client_handle     [in]  - handle used by QCSI to idenfity services
    service_cookie    [in]  - cookie registered with QCSI during qmi_csi_register
    connection_handle [out] - handle for QMI service connection
 
  RETURN VALUE
    Status of connect

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_connect_cb
(
  qmi_client_handle  client_handle,
  void              *service_cookie,
  void             **connection_handle
)
{
  uint8 i;
  pdc_service_type *pdc_svc;
  pdc_client_type  *pdc_client;

  /*-----------------------------------------------------------------------*/

  pdc_client = NULL;
  pdc_svc = (pdc_service_type *) service_cookie;
  
  if ( !pdc_svc )
  {
    MSG_ERROR( "pdc_connect_cb(): pdc_svc NULL", 0, 0, 0 );
    return QMI_CSI_CB_INTERNAL_ERR;
  }
  
 if ( !connection_handle )
  {
    MSG_ERROR( "pdc_connect_cb(): connection_handle NULL", 0, 0, 0 );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Find empty client slot */
  for ( i = 0; i < PDC_CLIENTS_MAX; i++ )
  {
    if (pdc_svc->pdc_clients[i].connected == FALSE)
    {
      /* Found empty slot */
      pdc_client = &pdc_svc->pdc_clients[i];
      break;
    }
  }

  if ( !pdc_client )
  {
    MSG_ERROR( "pdc_connect_cb(): No empty client slots found", 0, 0, 0 );
    return QMI_CSI_CB_NO_MEM;
  }

  /* Clear out client struct memory */
  memset( pdc_client, 0, sizeof(pdc_client_type) );

  /* Fill out client slot data with default values */
  pdc_client->connected                 = TRUE;
  pdc_client->qmi_client_handle         = client_handle;
  pdc_client->event_reg.selected_config = FALSE;

  /* Return client handle */
  *connection_handle = pdc_client;
  
  return QMI_CSI_CB_NO_ERR;

} /* pdc_connect_cb() */


/*==============================================================================

  FUNCTION pdc_disconnect_cb

  DESCRIPTION
    Callback when a client disconnects from the server

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI client connection
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    None

  SIDE EFFECTS
    None

==============================================================================*/
static void pdc_disconnect_cb
(
  void  *connection_handle,
  void  *service_cookie
)
{
  pdc_client_type  *pdc_client;
  pdc_service_type *service;

  /*-----------------------------------------------------------------------*/

  pdc_client = (pdc_client_type *) connection_handle;
  
  if ( pdc_client )
  {
    pdc_client = (pdc_client_type *) connection_handle;

    /* Free client slot from being connected */
    pdc_client->connected = FALSE;

    /* reset config cache if client handle matches */
    service  = (pdc_service_type *) service_cookie;
    if (pdc_client->qmi_client_handle == service->load_config_cache.qmi_client_handle)
    {
      pdc_default_load_config_cache(&service->load_config_cache);
    }

  } else {
    MSG_ERROR( "pdc_connect_cb(): pdc_client NULL", 0, 0, 0 );
  }

} /* pdc_disconnect_cb() */


/*==============================================================================

  FUNCTION pdc_process_req_cb

  DESCRIPTION
    Handles PDC client requests

  DEPENDENCIES
    Service must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
qmi_csi_cb_error pdc_process_req_cb
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
  pdc_service_type *service;
  qmi_response_type_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  if(!connection_handle || !service_cookie)
  {
    MSG_ERROR( "pdc_process_req_cb(): NULL pointer", 0, 0, 0 );
    return QMI_CSI_CB_INTERNAL_ERR;
  }
  
  service  = (pdc_service_type *)       service_cookie;
  response = (qmi_response_type_v01 *) &service->pdc_resp;

  MSG_MED( "pdc_process_req_cb(): processing MSG ID %d", msg_id, 0, 0 );
  if ((msg_id < ((int) sizeof(pdc_service_req_handle_table) / sizeof(*pdc_service_req_handle_table)))
      &&
      (pdc_service_req_handle_table[msg_id]))
  {
    /* If the jump table has a valid entry at this message ID, call the handler function */
    retval = pdc_service_req_handle_table[msg_id] (connection_handle,req_handle,msg_id,req_c_struct,req_c_struct_len,service_cookie);
  } else {
    /* if the message ID doesn't exist format an error response */
    MSG_ERROR( "pdc_process_req_cb(): msg_id unknown (%d)", msg_id, 0, 0 );

    /* Clear the response memory */
    memset( response, 0, sizeof(qmi_response_type_v01) );

    /* Send the response now (there is no message handler to send the resp) */
    response->result = QMI_RESULT_FAILURE_V01;
    response->error  = QMI_ERR_INVALID_MESSAGE_ID_V01;

    qmi_csi_send_resp( req_handle,
                       msg_id,
                       response,
                       sizeof(qmi_response_type_v01) );
  }

  MSG_MED( "pdc_process_req_cb(): returning %d", retval, 0, 0 );
  
  return retval;

} /* pdc_process_req_cb() */


/*==============================================================================

  FUNCTION pdc_handle_reset_req

  DESCRIPTION
    Handles the request to reset PDC client

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_reset_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  /* there is no request TLV's */
  pdc_reset_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  (void) req_c_struct;
  response = (pdc_reset_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Request is not used in this function so no need to check it */
  pdc_default_load_config_cache(&service->load_config_cache);

  /* Init response data */
  memset( response, 0, sizeof(pdc_reset_resp_msg_v01) );

  /* Validate request */
  
  /* Perform requested action */
  client->event_reg.selected_config = FALSE;
  
  response->resp.result = QMI_RESULT_SUCCESS_V01;
  response->resp.error = QMI_ERR_NONE_V01;

  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_reset_resp_msg_v01) );

  return QMI_CSI_CB_NO_ERR;

} /* pdc_handle_reset_req() */


/*==============================================================================

  FUNCTION pdc_handle_indication_register_req

  DESCRIPTION
    Handles the request to register for QMI_PDC indications

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    Registers a client indication reporting and causes an indication to be sent
    ever time it is enabled.

==============================================================================*/
static qmi_csi_cb_error pdc_handle_indication_register_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_indication_register_req_msg_v01  *request;
  pdc_indication_register_resp_msg_v01 *response;
  
  qmi_csi_error qmi_error;
  boolean send_config_change_ind;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_indication_register_req_msg_v01 *)   req_c_struct;
  response = (pdc_indication_register_resp_msg_v01 *) &service->pdc_resp;
  
  qmi_error = QMI_CSI_NO_ERR;
  send_config_change_ind = FALSE;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MSG_ERROR( "pdc_handle_indication_register_req(): request pointer NULL", 0, 0, 0 );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_indication_register_resp_msg_v01) );
  response->resp.error = QMI_ERR_NONE_V01;

  /* Validate request */
  
  /* Perform requested action */
  if (request->reg_config_change_valid)
  {
    MSG_MED( "pdc_handle_indication_register_req(): handling reg_config_change TLV", 0, 0, 0 );
    client->event_reg.selected_config = request->reg_config_change;
    if (client->event_reg.selected_config)
    {
      send_config_change_ind = TRUE;
    }
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_indication_register_resp_msg_v01) );

  if (send_config_change_ind)
  {
    /* Send the IND to the client when the reporting is enabled using the cached
       IND that is stored in the service info */
    if (service->pdc_ind.config_change_valid)
    {
      MSG_MED( "pdc_handle_indication_register_req(): sending config_change IND", 0, 0, 0 );
      qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                                   QMI_PDC_CONFIG_CHANGE_IND_V01,
                                   &service->pdc_ind.config_change,
                                   service->pdc_ind.config_change_len);
    
      if ( qmi_error != QMI_CSI_NO_ERR )
      {
        MSG_ERROR( "pdc_handle_indication_register_req(): send_ind failed (%d)", qmi_error, 0, 0 );
        client->event_reg.selected_config = FALSE;
        response->resp.error = QMI_ERR_INTERNAL_V01;
      }
    } else {
      MSG_MED( "pdc_handle_indication_register_req(): no cached IND to send", 0, 0, 0 );
    }
  }
  
  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_indication_register_req() */


/*==============================================================================

  FUNCTION pdc_handle_get_selected_config_req

  DESCRIPTION
    Handles the request to query the selected configuration

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_get_selected_config_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_get_selected_config_req_msg_v01  *request;
  pdc_get_selected_config_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_get_selected_config_req_msg_v01 *)   req_c_struct;
  response = (pdc_get_selected_config_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_get_selected_config_req(): request pointer NULL" );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_get_selected_config_resp_msg_v01) );

  /* Validate request */
  if (!IS_VALID_PDC_CONFIG_TYPE(request->config_type))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_get_selected_config_req(): invalid type %d", 
                      request->config_type );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else if (request->subscription_id_valid == TRUE && 
      !IS_VALID_MCFG_SUBSCRIPTION_ID(request->subscription_id))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_get_selected_config_req(): invalid subscription %lu", 
                      request->subscription_id );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else if (request->slot_id_valid == TRUE && 
      !IS_VALID_MCFG_SLOT_ID(request->slot_id))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_get_selected_config_req(): invalid slot %lu", 
                      request->slot_id );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_get_selected_config_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_get_selected_config_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_get_selected_config_req() */


/*==============================================================================

  FUNCTION pdc_handle_set_selected_config_req

  DESCRIPTION
    Handles the request to set the selected device configuration

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_set_selected_config_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_set_selected_config_req_msg_v01  *request;
  pdc_set_selected_config_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_set_selected_config_req_msg_v01 *)   req_c_struct;
  response = (pdc_set_selected_config_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_set_selected_config_req(): request pointer NULL" );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_set_selected_config_resp_msg_v01) );

  /* Validate request */
  if (!IS_VALID_PDC_CONFIG_TYPE(request->new_config_info.config_type))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_set_selected_config_req(): invalid type %d", 
                      request->new_config_info.config_type );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else if (!IS_VALID_PDC_CONFIG_ID_LEN(request->new_config_info.config_id_len))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_set_selected_config_req(): invalid id len %lu", 
                    request->new_config_info.config_id_len );
    response->resp.error = QMI_ERR_INVALID_ID_V01;
  }
  else if (request->subscription_id_valid == TRUE && 
      !IS_VALID_MCFG_SUBSCRIPTION_ID(request->subscription_id))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_set_selected_config_req(): invalid subscription %lu", 
                      request->subscription_id );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else if (request->slot_id_valid == TRUE && 
      !IS_VALID_MCFG_SLOT_ID(request->slot_id))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_get_selected_config_req(): invalid slot %lu", 
                      request->slot_id );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_set_selected_config_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_set_selected_config_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_set_selected_config_req() */


/*==============================================================================

  FUNCTION pdc_handle_list_configs_req

  DESCRIPTION
    Handles the request to list available configurations on the device

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_list_configs_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_list_configs_req_msg_v01  *request;
  pdc_list_configs_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_list_configs_req_msg_v01 *)   req_c_struct;
  response = (pdc_list_configs_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_list_configs_req(): request pointer NULL" );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_list_configs_resp_msg_v01) );

  /* Validate request */
  if (request->config_type_valid == TRUE && 
      !IS_VALID_PDC_CONFIG_TYPE(request->config_type))
  {
    MCFG_MSG_ERROR_1("pdc_handle_list_configs_req(): invalid type %d", 
                     request->config_type);
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_list_configs_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_list_configs_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_list_configs_req() */


/*==============================================================================

  FUNCTION pdc_handle_delete_config_req

  DESCRIPTION
    Handles the request to delete a device configuration

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_delete_config_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_delete_config_req_msg_v01  *request;
  pdc_delete_config_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_delete_config_req_msg_v01 *)   req_c_struct;
  response = (pdc_delete_config_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_delete_config_req(): request pointer NULL" );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_delete_config_resp_msg_v01) );

  /* Validate request */
  if (!IS_VALID_PDC_CONFIG_TYPE(request->config_type))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_delete_config_req(): invalid type %d", 
                      request->config_type );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else if (request->config_id_valid && 
           !IS_VALID_PDC_CONFIG_ID_LEN(request->config_id_len))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_delete_config_req(): invalid id len %lu", 
                    request->config_id_len );
    response->resp.error = QMI_ERR_INVALID_ID_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }

  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_delete_config_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_delete_config_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_delete_config_req() */


/*==============================================================================

  FUNCTION pdc_handle_load_config_req

  DESCRIPTION
    Handles the request to load a new configuration to the device

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_load_config_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_load_config_req_msg_v01  *request;
  pdc_load_config_resp_msg_v01 *response;
  pdc_load_config_cache_type   *cache;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_load_config_req_msg_v01 *)   req_c_struct;
  response = (pdc_load_config_resp_msg_v01 *) &service->pdc_resp;
  cache    =  &service->load_config_cache;
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_load_config_req(): request pointer NULL");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_load_config_resp_msg_v01) );

  /* Validate request */
  response->resp.error = QMI_ERR_NONE_V01;
  do {
  if (!IS_VALID_PDC_CONFIG_TYPE(request->load_config_info.config_type))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_load_config_req(): invalid config type %d", 
                      request->load_config_info.config_type );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else if (!IS_VALID_PDC_CONFIG_ID_LEN(request->load_config_info.config_id_len))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_load_config_req(): invalid id len %lu", 
                      request->load_config_info.config_id_len );
    response->resp.error = QMI_ERR_INVALID_ID_V01;
  }
  else if (request->load_config_info.total_config_size == 0 ||
    request->load_config_info.total_config_size > MCFG_CONFIG_SIZE_MAX || 
    request->load_config_info.config_frame_len == 0 ||
    request->load_config_info.config_frame_len > MCFG_CONFIG_FRAME_SIZE_MAX )
  {
    MCFG_MSG_ERROR_2( "pdc_handle_load_config_req(): invalid total size %lu frame len %lu", 
                      request->load_config_info.total_config_size,
                      request->load_config_info.config_frame_len);
    response->resp.error = QMI_ERR_INVALID_ARG_V01;
  }
  else if (request->storage_valid == TRUE && 
           !IS_VALID_PDC_STORAGE_TYPE(request->storage))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_load_config_req(): invalid storage type %d", 
                      request->storage );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }

    if (cache->config_s.config_len != 0)
  {
    /* this is subsequential request, validate with cached information */
    if (cache->qmi_client_handle != client->qmi_client_handle)
    {
      MCFG_MSG_HIGH_2( "pdc_handle_load_config_req(): qmi handle %p mismatch %p", 
                        client->qmi_client_handle, cache->qmi_client_handle );
      response->resp.error = QMI_ERR_DEVICE_IN_USE_V01;

        /* Skip resetting the cache */
        break;
    }
    else if (request->storage_valid == TRUE && 
             request->storage != (pdc_storage_enum_v01) cache->storage)
    {
      MCFG_MSG_ERROR_1( "pdc_handle_load_config_req(): invalid storage type %d", 
                        request->storage );
      response->resp.error = QMI_ERR_INVALID_ARG_V01;
    } 
    else if (request->load_config_info.config_type != (pdc_config_type_enum_v01) cache->type)
    {
      MCFG_MSG_ERROR_1( "pdc_handle_load_config_req(): invalid config type %d", 
                        request->load_config_info.config_type );
      response->resp.error = QMI_ERR_INVALID_ARG_V01;
    }  
    else if (request->load_config_info.total_config_size != cache->config_s.config_len || 
                !MCFG_SIZE_OFFSET_WITHIN_LIMIT(request->load_config_info.config_frame_len, 
                  cache->offset,request->load_config_info.total_config_size))
    {
      MCFG_MSG_ERROR_1( "pdc_handle_load_config_req(): invalid total size %lu", 
                        request->load_config_info.total_config_size );
      response->resp.error = QMI_ERR_INVALID_ARG_V01;
    }  
    else if (request->load_config_info.config_id_len != cache->id_s.id_len ||
             memcmp(request->load_config_info.config_id, 
                    cache->id_s.id, cache->id_s.id_len) !=0)
    {
      MCFG_MSG_ERROR_1( "pdc_handle_load_config_req(): invalid ID %lu", 
                        request->load_config_info.config_id_len );
      response->resp.error = QMI_ERR_INVALID_ID_V01;
    }
  }
    /* this is first request, double check cached load config info*/
    else if (cache->qmi_client_handle != NULL || 
        cache->offset != 0 || 
        cache->id_s.id_len != 0)
    {
      MCFG_MSG_ERROR_1( "pdc_handle_load_config_req(): invalid qmi handle %p", 
                        client->qmi_client_handle );
        response->resp.error = QMI_ERR_INTERNAL_V01;
    }

    /* Reset the cache for all errors except a QMI handle mismatch */
    if (response->resp.error != QMI_ERR_NONE_V01)
    {
      pdc_default_load_config_cache(cache);
    }
  } while(0);

  /* A 'frame data reset' tlv is sent for all errors */
  if (response->resp.error != QMI_ERR_NONE_V01)
  {
    response->frame_data_reset = TRUE;
    response->frame_data_reset_valid = TRUE;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_load_config_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_load_config_ind(client, request, cache);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_load_config_req() */


/*==============================================================================

  FUNCTION pdc_handle_activate_config_req

  DESCRIPTION
    Handles the request to activate the currently selected device configuration

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    Resets the device

==============================================================================*/
static qmi_csi_cb_error pdc_handle_activate_config_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_activate_config_req_msg_v01  *request;
  pdc_activate_config_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_activate_config_req_msg_v01 *)   req_c_struct;
  response = (pdc_activate_config_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_activate_config_req(): request pointer NULL");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_activate_config_resp_msg_v01) );

  /* Validate request */
  if (!IS_VALID_PDC_CONFIG_TYPE(request->config_type))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_activate_config_req(): invalid type %d", 
                      request->config_type );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_activate_config_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_activate_config_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_activate_config_req() */

/*==============================================================================

  FUNCTION pdc_handle_validate_config_req

  DESCRIPTION
    Handles request to validate device configuration settings 

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_validate_config_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_validate_config_req_msg_v01  *request;
  pdc_validate_config_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_validate_config_req_msg_v01 *)   req_c_struct;
  response = (pdc_validate_config_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  MCFG_MSG_MED( "pdc_handle_validate_config_req(): successful entrance" );
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_validate_config_req(): request pointer NULL" );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_validate_config_resp_msg_v01) );

  /* Validate request */
  if (!IS_VALID_PDC_CONFIG_TYPE(request->config_type))
  {
    MCFG_MSG_ERROR_1("pdc_handle_validate_config_req(): invalid type (%d)", 
                     request->config_type);
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else if (request->config_id_valid == TRUE && 
      !IS_VALID_PDC_CONFIG_ID_LEN(request->config_id_len))
  {
    MCFG_MSG_ERROR_1("pdc_handle_validate_config_req(): invalid id len (%lu)", 
                     request->config_id_len);
    response->resp.error = QMI_ERR_INVALID_ID_V01;
  }
  else if (request->subscription_id_valid == TRUE && 
      !IS_VALID_MCFG_SUBSCRIPTION_ID(request->subscription_id))
  {
    MCFG_MSG_ERROR_1("pdc_handle_validate_config_req(): invalid subscription %lu", 
                     request->subscription_id);
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_validate_config_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_validate_config_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_get_config_info_req() */

/*==============================================================================

  FUNCTION pdc_handle_deactivate_config_req

  DESCRIPTION
    Handles the request to deactivate the currently active device configuration

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    Resets the device

==============================================================================*/
static qmi_csi_cb_error pdc_handle_deactivate_config_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_deactivate_config_req_msg_v01  *request;
  pdc_deactivate_config_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_deactivate_config_req_msg_v01 *)   req_c_struct;
  response = (pdc_deactivate_config_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_deactivate_config_req(): request pointer NULL");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_deactivate_config_resp_msg_v01) );

  /* Validate request */
  if (!IS_VALID_PDC_CONFIG_TYPE(request->config_type))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_deactivate_config_req(): invalid type %d", 
                      request->config_type );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else if (request->subscription_id_valid == TRUE && 
      !IS_VALID_MCFG_SUBSCRIPTION_ID(request->subscription_id))
  {
    MCFG_MSG_ERROR_1("pdc_handle_deactivate_config_req(): invalid subscription %lu", 
                     request->subscription_id);
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_deactivate_config_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_deactivate_config_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_deactivate_config_req() */

/*==============================================================================

  FUNCTION pdc_handle_get_config_info_req

  DESCRIPTION
    Handles the request to get more info for a device configuration

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_get_config_info_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_get_config_info_req_msg_v01  *request;
  pdc_get_config_info_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_get_config_info_req_msg_v01 *)   req_c_struct;
  response = (pdc_get_config_info_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_get_config_info_req(): request pointer NULL" );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_get_config_info_resp_msg_v01) );

  /* Validate request */
  if (!IS_VALID_PDC_CONFIG_TYPE(request->new_config_info.config_type))
  {
    MCFG_MSG_ERROR_1("pdc_handle_get_config_info_req(): invalid type (%d)", 
                     request->new_config_info.config_type);
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else if (!IS_VALID_PDC_CONFIG_ID_LEN(request->new_config_info.config_id_len))
  {
    MCFG_MSG_ERROR_1("pdc_handle_get_config_info_req(): invalid id len (%lu)", 
                     request->new_config_info.config_id_len);
    response->resp.error = QMI_ERR_INVALID_ID_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_get_config_info_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_get_config_info_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_get_config_info_req() */


/*==============================================================================

  FUNCTION pdc_handle_get_config_limits_req

  DESCRIPTION
    Handles the request to query limits for storing configurations on the device

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_get_config_limits_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_get_config_limits_req_msg_v01  *request;
  pdc_get_config_limits_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_get_config_limits_req_msg_v01 *)   req_c_struct;
  response = (pdc_get_config_limits_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_get_config_limits_req(): request pointer NULL" );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_get_config_limits_resp_msg_v01) );

  /* Validate request */
  if (!IS_VALID_PDC_CONFIG_TYPE(request->config_type))
  {
    MCFG_MSG_ERROR_1( "pdc_handle_get_config_limits_req(): invalid type %d", 
                      request->config_type );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_get_config_limits_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_get_config_limits_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_get_config_limits_req() */


/*==============================================================================

  FUNCTION pdc_handle_get_default_config_info_req

  DESCRIPTION
    Handles the request to get more info for a default device configuration

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_get_default_config_info_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_get_default_config_info_req_msg_v01  *request;
  pdc_get_default_config_info_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_get_default_config_info_req_msg_v01 *)   req_c_struct;
  response = (pdc_get_default_config_info_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by
     pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "get_default_config_info_req(): request pointer NULL" );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_get_default_config_info_resp_msg_v01) );

  /* Validate request */
  if (!IS_VALID_PDC_CONFIG_TYPE(request->config_type))
  {
    MCFG_MSG_ERROR_1("get_default_config_info_req(): invalid type (%d)", 
                     request->config_type);
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_get_default_config_info_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_get_default_config_info_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_get_default_config_info_req() */

/*==============================================================================

  FUNCTION pdc_handle_get_feature_req

  DESCRIPTION
    Handles the request to query the feature configuration

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_get_feature_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_get_feature_req_msg_v01  *request;
  pdc_get_feature_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_get_feature_req_msg_v01 *)   req_c_struct;
  response = (pdc_get_feature_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_get_feature_req(): request pointer NULL" );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_get_feature_resp_msg_v01) );

  /* Validate request */
  if (request->slot_id >= MCFG_NUM_OF_UIM_SLOT)
  {
    MCFG_MSG_ERROR_1( "pdc_handle_get_feature_req(): invalid slot %lu", 
                      request->slot_id );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_get_feature_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_get_feature_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_get_feature_req() */


/*==============================================================================

  FUNCTION pdc_handle_set_feature_req

  DESCRIPTION
    Handles the request to set the feature device configuration

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    connection_handle [in] - handle for QMI service connection
    req_handle        [in] - handle used to identify the transaction and client
    msg_id            [in] - message ID for this message
    req_c_struct      [in] - C data structure for this response
    req_c_struct_len  [in] - C data structure length
    service_cookie    [in] - cookie registered with QCSI during qmi_csi_register

  RETURN VALUE
    Status of QMI request

  SIDE EFFECTS
    None

==============================================================================*/
static qmi_csi_cb_error pdc_handle_set_feature_req
(
  void           *connection_handle,
  qmi_req_handle  req_handle,
  unsigned int    msg_id,
  void           *req_c_struct,
  unsigned int    req_c_struct_len,
  void           *service_cookie
)
{
  pdc_service_type *service;
  pdc_client_type  *client;
  pdc_set_feature_req_msg_v01  *request;
  pdc_set_feature_resp_msg_v01 *response;

  /*-----------------------------------------------------------------------*/
  
  service  = (pdc_service_type *) service_cookie;
  client   = (pdc_client_type *)  connection_handle;
  request  = (pdc_set_feature_req_msg_v01 *)   req_c_struct;
  response = (pdc_set_feature_resp_msg_v01 *) &service->pdc_resp;
  
  /* connection_handle and service_cookie have already been checked by pdc_process_req_cb() */
  
  /* Check that the request data is present */
  if (!request)
  {
    MCFG_MSG_ERROR( "pdc_handle_set_feature_req(): request pointer NULL" );
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Init response data */
  memset( response, 0, sizeof(pdc_set_feature_resp_msg_v01) );

  /* Validate request */
  if (request->slot_id >= MCFG_NUM_OF_UIM_SLOT && request->slot_id != 0xFFFFFFFF)
  {
    MCFG_MSG_ERROR_1( "pdc_handle_set_feature_req(): invalid slot %lu", 
                      request->slot_id );
    response->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
  }
  else
  {
    response->resp.error = QMI_ERR_NONE_V01;
  }
  
  /* Set the response result based on the response error */  
  response->resp.result = QMI_RESULT_FROM_ERROR(response->resp.error);
  
  /* Send the response */
  qmi_csi_send_resp( req_handle,
                     msg_id,
                     response,
                     sizeof(pdc_set_feature_resp_msg_v01) );

  /* Perform requested action and send indication for valid request */
  if (response->resp.error == QMI_ERR_NONE_V01)
  {
    pdc_send_set_feature_ind(client, request);
  }

  return QMI_CSI_CB_NO_ERR;
} /* pdc_handle_set_feature_req() */


/*==============================================================================

  FUNCTION pdc_get_service_state

  DESCRIPTION
    Returns a pointer to the global service data for QMI_PDC

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    Pointer to the service data stored globally

  SIDE EFFECTS
    None

==============================================================================*/
static pdc_service_type * pdc_get_service_state
(
  void
)
{
  return &global_pdc_svc;
} /* pdc_get_service_state() */

static qmi_error_type_v01 pdc_err_map_mcfg_to_qmi
(
  mcfg_error_e_type error
)
{
  if (error < MCFG_ERR_NON_QMI)
  {
    return (qmi_error_type_v01)error;
  }
  return QMI_ERR_INTERNAL_V01;
}

/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

/*===========================================================================

  FUNCTION pdc_generate_config_change_ind

  DESCRIPTION
    This function generates and sends QMI_PDC indications to all clients that
    have registered for the config file change event.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    config_type [in] type of config file
    config_id   [in] ID of new config file

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void pdc_generate_config_change_ind
( 
  mcfg_config_type_e_type config_type,
  mcfg_config_id_s_type  *config_id
)
{
  pdc_service_type *service;
  pdc_config_change_ind_msg_v01 *config_change_ind;
  
  qmi_csi_error qmi_error;
  uint32        ind_size;
  int           i;
  
  /*-----------------------------------------------------------------------*/
  
  service = NULL;
  config_change_ind = NULL;
  
  /* Check pass pointers */
  if (!config_id)
  {
    MCFG_MSG_ERROR( "pdc_generate_config_change_ind(): ID pointer NULL");
    return;
  }
  
  /* Get memory to store the IND data */
  config_change_ind = mcfg_malloc(sizeof(pdc_config_change_ind_msg_v01)); 
  if (!config_change_ind)
  {
    MCFG_MSG_ERROR( "pdc_generate_config_change_ind(): malloc failed");
    return;
  }
  memset(config_change_ind, 0, sizeof(pdc_config_change_ind_msg_v01));
  
  service = pdc_get_service_state();
  if (!service)
  {
    MCFG_MSG_ERROR( "pdc_generate_config_change_ind(): service pointer NULL");
    mcfg_free(config_change_ind);
    return;
  }
  
  /* Compose the IND payload */
  qmi_error = pdc_qmi_compose_config_change_ind(config_change_ind,
                                                &ind_size,
                                                config_type,
                                                config_id);
  if ( qmi_error != QMI_CSI_NO_ERR )
  {
    MCFG_MSG_ERROR_1( "pdc_generate_config_change_ind(): generating IND returned error (%d)", qmi_error);
    mcfg_free(config_change_ind);
    return;
  }
  
  /* Check that the IND has changed since the last IND of this type that was
     sent out */
  if ((service->pdc_ind.config_change_len == ind_size) && 
      (0 == memcmp(&service->pdc_ind.config_change, config_change_ind, ind_size)))
  {
    MCFG_MSG_HIGH( "pdc_generate_config_change_ind(): IND matches last, ignoring");
    mcfg_free(config_change_ind);
    return;
  }
  
  /* Update the last IND stored by the service with the new IND payload */
  memscpy(&service->pdc_ind.config_change, 
          sizeof(pdc_config_change_ind_msg_v01), config_change_ind, ind_size);
  
  /* Send the config indication to all clients that are both connected and
     registered for the indication */
  for ( i = 0; i < PDC_CLIENTS_MAX; i++ )
  {
    if ( service->pdc_clients[i].connected == TRUE )
    {
      if ( service->pdc_clients[i].event_reg.selected_config == TRUE )
      {
        qmi_error = qmi_csi_send_ind(service->pdc_clients[i].qmi_client_handle,
                                     QMI_PDC_CONFIG_CHANGE_IND_V01,
                                     config_change_ind,
                                     ind_size);

        if (qmi_error != QMI_CSI_NO_ERR)
        {
          MCFG_MSG_ERROR_1( "Failure to send indication: %d", qmi_error );
        }
      } /* if ( service->pdc_clients[i].event_reg.selected_config == TRUE ) */
    } /* if ( service->pdc_clients[i].connected == TRUE ) */
  } /* for ( i = 0; i < PDC_CLIENTS_MAX; i++ ) */
  
  /* Free memory */
  mcfg_free(config_change_ind);
} /* pdc_generate_config_change_ind() */


/*===========================================================================

  FUNCTION pdc_send_get_selected_config_ind

  DESCRIPTION
    This function sends QMI_PDC indications after the get selected config
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] get selected config request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_get_selected_config_ind
( 
  pdc_client_type                      *client,
  pdc_get_selected_config_req_msg_v01  *request
)
{
  pdc_get_selected_config_ind_msg_v01 selected_config_ind;
  mcfg_config_id_s_type   config_id;
  qmi_csi_error qmi_error;
  mcfg_sub_id_type_e_type sub;
  
  /*-----------------------------------------------------------------------*/
  
  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_get_selected_config_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_get_selected_config_ind(): request pointer NULL");
    return;
  }

  /* req handler suppose already verified input parameter: config type */
  
  /* init indication message */
  memset(&selected_config_ind, 0, sizeof(pdc_get_selected_config_ind_msg_v01));
  selected_config_ind.ind_token_valid = request->ind_token_valid;  
  selected_config_ind.ind_token = request->ind_token;

  if (request->subscription_id_valid == FALSE)
  {
    if (request->slot_id_valid == FALSE)
    {
      sub = MCFG_SUB_ID_FIRST;
    }
    else
    {
      sub = mcfg_uim_map_slot_index_to_sub_id(request->slot_id);
    }
  }
  else
  {
    sub = request->subscription_id;
  }

  /* generate indication message */
  if (mcfg_utils_get_active_config(request->config_type, &config_id, sub))
  {
    MCFG_MSG_MED_1("get_selected_config_ind active len %d", config_id.id_len);
    if (config_id.id_len > 0 && 
        config_id.id_len <= PDC_CONFIG_ID_SIZE_MAX_V01)
    {
      selected_config_ind.active_config_id_len = config_id.id_len;
      memscpy(selected_config_ind.active_config_id, PDC_CONFIG_ID_SIZE_MAX_V01,
              config_id.id, config_id.id_len);
      selected_config_ind.active_config_id_valid = TRUE;
    }
  }

  if (mcfg_utils_get_selected_config(request->config_type, &config_id, sub))
  {
    MCFG_MSG_MED_1("get_selected_config_ind pending len %d", config_id.id_len);
    if (config_id.id_len > 0 && 
        config_id.id_len <= PDC_CONFIG_ID_SIZE_MAX_V01)
    {
      if (!selected_config_ind.active_config_id_valid 
          || config_id.id_len != selected_config_ind.active_config_id_len 
          || (memcmp(config_id.id, selected_config_ind.active_config_id, 
                     config_id.id_len) != 0))
      {
        selected_config_ind.pending_config_id_len = config_id.id_len;
        memscpy(selected_config_ind.pending_config_id, 
                PDC_CONFIG_ID_SIZE_MAX_V01, config_id.id, config_id.id_len);
        selected_config_ind.pending_config_id_valid = TRUE;
      }
    }
  }
  
  if (!selected_config_ind.active_config_id_valid 
      && !selected_config_ind.pending_config_id_valid)
  {
    selected_config_ind.error = QMI_ERR_NOT_PROVISIONED_V01;
  }
  else
  {
    selected_config_ind.error = QMI_ERR_NONE_V01;
  }

  /* send indication message */
  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_GET_SELECTED_CONFIG_IND_V01,
                               &selected_config_ind,
                               sizeof(pdc_get_selected_config_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to send indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_GET_SELECTED_CONFIG_IND_V01 is sent");
  }
} /* pdc_send_get_selected_config_ind() */


/*===========================================================================

  FUNCTION pdc_send_set_selected_config_ind

  DESCRIPTION
    This function sends QMI_PDC indications after the set selected config
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] set selected config request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_set_selected_config_ind
( 
  pdc_client_type                      *client,
  pdc_set_selected_config_req_msg_v01  *request
)
{
  pdc_set_selected_config_ind_msg_v01 selected_config_ind;
  mcfg_config_id_s_type   config_id;
  qmi_csi_error qmi_error;
  mcfg_sub_id_type_e_type sub = MCFG_SUB_ID_FIRST;
  boolean valid = TRUE;
  mcfg_config_id_s_type active_id;
  
  /*-----------------------------------------------------------------------*/
  
  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_set_selected_config_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_set_selected_config_ind(): request pointer NULL");
    return;
  }
  
  if (request->subscription_id_valid == FALSE)
  {
    if (request->slot_id_valid == FALSE)
    {
      sub = MCFG_SUB_ID_FIRST;
    }
    else
    {
      sub = mcfg_uim_map_slot_index_to_sub_id(request->slot_id);
    }
  }
  else
  {
    if (request->slot_id_valid == FALSE)
    {
      sub = request->subscription_id;
    }
    else if (request->subscription_id == mcfg_uim_map_slot_index_to_sub_id(request->slot_id))
    {
      sub = request->subscription_id;
    }
    else if (request->new_config_info.config_type == MCFG_TYPE_HW)
    {
      MCFG_MSG_ERROR("Cross mapping for HW MBN is now allowed");
      valid = FALSE;
    }
    else if (mcfg_utils_get_active_config(request->new_config_info.config_type, &active_id, request->subscription_id) || 
             mcfg_utils_get_active_config(request->new_config_info.config_type, &active_id, mcfg_uim_map_slot_index_to_sub_id(request->slot_id)))
    {
      MCFG_MSG_ERROR_2("Deactivate existing mbn before changing mapping slot%lu sub%lu ", request->slot_id, request->subscription_id);
      valid = FALSE;
    }
    else
    {
      MCFG_MSG_HIGH_2("set_selected_config changed mapping to slot%lu sub%lu ", request->slot_id, request->subscription_id);
      sub = request->subscription_id;
      mcfg_uim_process_mapping(request->slot_id, request->subscription_id, FALSE);
    }
  }

  /* req handler suppose already verified input parameter: config type */
  
  /* init indication message */
  memset(&selected_config_ind, 0, sizeof(pdc_set_selected_config_ind_msg_v01));
  selected_config_ind.ind_token_valid = request->ind_token_valid;  
  selected_config_ind.ind_token = request->ind_token;

  /* generate indication message */
  if (!valid)
  {
    selected_config_ind.error = QMI_ERR_INVALID_ARG_V01;
  }
  else
  {
    /* generate indication message */
    if (request->new_config_info.config_id_len == 0 ||
        request->new_config_info.config_id_len > PDC_CONFIG_ID_SIZE_MAX_V01)
    {
      selected_config_ind.error = QMI_ERR_INVALID_ID_V01;
    }
    else
    {
      config_id.id_len = request->new_config_info.config_id_len;
      memscpy(config_id.id, PDC_CONFIG_ID_SIZE_MAX_V01, 
              request->new_config_info.config_id, config_id.id_len);
     
      if (mcfg_utils_is_config_id_unique(request->new_config_info.config_type, &config_id))
      {
        selected_config_ind.error = QMI_ERR_INVALID_ID_V01;
      }
      else if (mcfg_utils_set_selected_config(request->new_config_info.config_type, &config_id, sub))
      {
        selected_config_ind.error = QMI_ERR_NONE_V01;
      }
      else
      {
        selected_config_ind.error = QMI_ERR_INTERNAL_V01;
      }
    }
  }

  /* send indication message */
  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_SET_SELECTED_CONFIG_IND_V01,
                               &selected_config_ind,
                               sizeof(pdc_set_selected_config_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to send indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_SET_SELECTED_CONFIG_IND_V01 is sent");
  }

  if (selected_config_ind.error == QMI_ERR_NONE_V01)
  {
    pdc_generate_config_change_ind(request->new_config_info.config_type,
                                   &config_id);
  }
  
} /* pdc_send_set_selected_config_ind() */


/*===========================================================================

  FUNCTION pdc_send_list_configs_ind

  DESCRIPTION
    This function sends QMI_PDC indications after the list configs
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] list configs request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_list_configs_ind
( 
  pdc_client_type               *client,
  pdc_list_configs_req_msg_v01  *request
)
{
  pdc_list_configs_ind_msg_v01 *p_list_configs_ind;
  mcfg_config_id_s_type *p_id_list;
  mcfg_config_id_s_type *p_id_list_i;
  qmi_csi_error qmi_error;
  uint8_t i;
  pdc_config_type_enum_v01 type;
  uint32_t list_ret;
  
  /*-----------------------------------------------------------------------*/
  
  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_list_configs_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_list_configs_ind(): request pointer NULL");
    return;
  }
  
  p_list_configs_ind = mcfg_malloc(sizeof(pdc_list_configs_ind_msg_v01));
  if (!p_list_configs_ind)
  {
    MCFG_MSG_ERROR("pdc_send_list_configs_ind(): failed to malloc p_list_configs_ind");
    return;
  }

  p_id_list = mcfg_malloc(PDC_CONFIG_LIST_SIZE_MAX_V01 * sizeof(mcfg_config_id_s_type));
  if (!p_id_list)
  {
    MCFG_MSG_ERROR("pdc_send_list_configs_ind(): failed to malloc p_id_list");
    mcfg_free(p_list_configs_ind);
    return;
  }

  memset(p_list_configs_ind, 0, sizeof(pdc_list_configs_ind_msg_v01));
  p_list_configs_ind->ind_token_valid = request->ind_token_valid;  
  p_list_configs_ind->ind_token = request->ind_token;

  p_list_configs_ind->config_list_len = 0;
  for (type =  PDC_CONFIG_TYPE_MODEM_PLATFORM_V01; 
       type <= PDC_CONFIG_TYPE_MODEM_SW_V01; type++)
  {
    if (request->config_type_valid == FALSE || request->config_type == type)
    {
      list_ret = mcfg_utils_list_configs(
        type, 
        PDC_CONFIG_LIST_SIZE_MAX_V01 - p_list_configs_ind->config_list_len, 
        p_id_list);
      if ((p_list_configs_ind->config_list_len + list_ret) 
            > PDC_CONFIG_LIST_SIZE_MAX_V01)
      {
        MCFG_MSG_ERROR_2("pdc_send_list_configs_ind(): invalid number of list"\
                         " %lu for type %d", list_ret, type);
        p_list_configs_ind->config_list_len = 0;
        break;
      }
      else
      {
        for(i=0; i<list_ret; i++)
        {
          p_id_list_i = p_id_list + i;
          p_list_configs_ind->config_list[p_list_configs_ind->config_list_len+i].\
            config_type = type;
          p_list_configs_ind->config_list[p_list_configs_ind->config_list_len+i].\
            config_id_len = p_id_list_i->id_len;
          memscpy(p_list_configs_ind->config_list[p_list_configs_ind->config_list_len+i].\
                  config_id, PDC_CONFIG_ID_SIZE_MAX_V01, 
                  p_id_list_i->id, p_id_list_i->id_len);
        }

        p_list_configs_ind->config_list_len += list_ret;
      }
    }
  }

  p_list_configs_ind->error = QMI_ERR_NONE_V01;
  p_list_configs_ind->config_list_valid = TRUE;
  MCFG_MSG_LOW_1("pdc_send_list_configs_ind(): sending %d ids", 
                 p_list_configs_ind->config_list_len);

  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_LIST_CONFIGS_IND_V01,
                               p_list_configs_ind,
                               sizeof(pdc_list_configs_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to send list configs indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_LIST_CONFIGS_IND_V01 is sent");
  }

  mcfg_free(p_list_configs_ind);
  mcfg_free(p_id_list);
} /* pdc_send_list_configs_ind() */


/*===========================================================================

  FUNCTION pdc_send_delete_config_ind

  DESCRIPTION
    This function sends QMI_PDC indications after delete config
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] list configs request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_delete_config_ind
( 
  pdc_client_type               *client,
  pdc_delete_config_req_msg_v01 *request
)
{
  pdc_delete_config_ind_msg_v01 delete_config_ind;
  mcfg_config_id_s_type  id;
  qmi_csi_error qmi_error;
 
  /*-----------------------------------------------------------------------*/
  
  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_list_configs_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_list_configs_ind(): request pointer NULL");
    return;
  }
  
  /* init indication message */
  memset(&delete_config_ind, 0, sizeof(pdc_delete_config_ind_msg_v01));
  delete_config_ind.ind_token_valid = request->ind_token_valid;  
  delete_config_ind.ind_token = request->ind_token;

  if (request->config_id_valid == FALSE)
  {
    delete_config_ind.error = pdc_err_map_mcfg_to_qmi(
      mcfg_utils_delete_all(request->config_type));
  }
  else
  {
    id.id_len = request->config_id_len;
    memscpy(id.id, MCFG_CONFIG_ID_SIZE_MAX, request->config_id, id.id_len);

    if (mcfg_utils_is_config_id_unique(request->config_type, &id))
  {
    delete_config_ind.error = QMI_ERR_INVALID_ID_V01;
  }
  else
  {
    delete_config_ind.error = pdc_err_map_mcfg_to_qmi(
        mcfg_utils_delete_config(request->config_type, &id));
    }
  }

  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_DELETE_CONFIG_IND_V01,
                               &delete_config_ind,
                               sizeof(pdc_delete_config_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to delete config indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_DELETE_CONFIG_IND_V01 is sent");
  }
} /* pdc_send_delete_config_ind() */

/*===========================================================================

  FUNCTION pdc_send_load_config_ind

  DESCRIPTION
    This function sends QMI_PDC indications after load config
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] list configs request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_load_config_ind
( 
  pdc_client_type             *client,
  pdc_load_config_req_msg_v01 *request,
  pdc_load_config_cache_type  *cache
)
{
  pdc_load_config_ind_msg_v01 load_config_ind;
  qmi_csi_error qmi_error;
  uint32 mcfg_error;

  /*-----------------------------------------------------------------------*/
  
  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_load_config_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_load_config_ind(): request pointer NULL");
    return;
  }
  
  if (!cache)
  {
    MCFG_MSG_ERROR("pdc_send_load_config_ind(): cache pointer NULL");
    return;
  }
  
  /* init indication message */
  memset(&load_config_ind, 0, sizeof(pdc_load_config_ind_msg_v01));
  load_config_ind.ind_token_valid = request->ind_token_valid;  
  load_config_ind.ind_token = request->ind_token;
  load_config_ind.error = QMI_ERR_NONE_V01;
  
  if (!cache->config_s.config_addr)
  {
    /* this is first request, allocate memory buffer */
    
    MCFG_MSG_LOW("pdc_send_load_config_ind(): config_buf NULL");
    cache->qmi_client_handle = client->qmi_client_handle;
    cache->config_s.config_len = request->load_config_info.total_config_size;

    if (request->storage_valid == FALSE)
    {
       cache->storage = (mcfg_storage_mode_e_type) PDC_STORAGE_LOCAL_V01;
    }
    else 
    {
       cache->storage = request->storage;
    }

    cache->type = request->load_config_info.config_type;

    cache->id_s.id_len = request->load_config_info.config_id_len;
    memscpy(cache->id_s.id, MCFG_CONFIG_ID_SIZE_MAX, 
            request->load_config_info.config_id, 
            request->load_config_info.config_id_len);

    /* check ID and alloc memory buffer */
    if (!mcfg_utils_is_config_id_unique(request->load_config_info.config_type,
                                        &cache->id_s))
    {
      MCFG_MSG_ERROR_1( "pdc_send_load_config_ind(): id exists len %lu", 
                        request->load_config_info.config_id_len );
      load_config_ind.error = QMI_ERR_INVALID_ID_V01;
    }
    else if (!mcfg_utils_alloc_config_buffer(request->load_config_info.config_type,
                                   &cache->config_s))
    {

      MCFG_MSG_ERROR_1( "pdc_send_load_config_ind(): mem alloc failed with size %d", 
                        cache->config_s.config_len );
      load_config_ind.error = QMI_ERR_NO_MEMORY_V01;
    }
  }

  if (!MCFG_SIZE_OFFSET_WITHIN_LIMIT(request->load_config_info.config_frame_len, 
         cache->offset, cache->config_s.config_len) ||
       MCFG_INTERGER_OVERFLOWED(cache->config_s.config_addr, cache->offset) ||
       (request->load_config_info.config_frame_len > MCFG_CONFIG_FRAME_SIZE_MAX))
  {
    /* wrong sum, send err indication and reset cache */
    MCFG_MSG_ERROR_2( "pdc_send_load_config_ind(): offset %lu overflow config len %d", 
                      cache->offset, cache->config_s.config_len);
    load_config_ind.error = QMI_ERR_INVALID_ARG_V01;
  }

  /* Add frame len check again to avoid klocwork warnings */
  if (load_config_ind.error != QMI_ERR_NONE_V01 || 
      request->load_config_info.config_frame_len > MCFG_CONFIG_FRAME_SIZE_MAX)
  {
    load_config_ind.frame_data_reset = TRUE;
    load_config_ind.frame_data_reset_valid = TRUE;
    pdc_default_load_config_cache(cache);
  }
  else
  {
    memscpy((uint8 *)cache->config_s.config_addr + cache->offset,
            MCFG_CONFIG_SIZE_MAX,
            request->load_config_info.config_frame,
            request->load_config_info.config_frame_len );

    cache->offset += request->load_config_info.config_frame_len;

    load_config_ind.received_config_size = request->load_config_info.config_frame_len;
    load_config_ind.received_config_size_valid = TRUE;

    load_config_ind.remaining_config_size = cache->config_s.config_len - cache->offset;
    load_config_ind.remaining_config_size_valid = TRUE;

    if (cache->config_s.config_len > cache->offset)
    {
      /* send indication and wait for next frame */
      MCFG_MSG_LOW_1( "pdc_send_load_config_ind(): remaining %d and wait for next frame", 
                        load_config_ind.remaining_config_size );
    }
    else
    {
      /* no more frame, save config to efs and clear cache */
      MCFG_MSG_LOW("pdc_send_load_config_ind(): last frame, save and clear cache");
                       
      mcfg_error = mcfg_utils_add_config(request->load_config_info.config_type, 
                                         &cache->id_s, &cache->config_s, cache->storage);

      load_config_ind.error = pdc_err_map_mcfg_to_qmi(mcfg_error);
      if(load_config_ind.error != QMI_ERR_NONE_V01)
      {
        MCFG_MSG_ERROR_1( "pdc_send_load_config_ind(): failed to save config, reset frame (0x%lx)", mcfg_error);
        load_config_ind.frame_data_reset = TRUE;
        load_config_ind.frame_data_reset_valid = TRUE;
      }

      pdc_default_load_config_cache(cache);
    }
  }
  
  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_LOAD_CONFIG_IND_V01,
                               &load_config_ind,
                               sizeof(pdc_load_config_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to load config indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_LOAD_CONFIG_IND_V01 is sent");
  }
} /* pdc_send_load_config_ind() */

/*===========================================================================

  FUNCTION pdc_send_activate_config_ind

  DESCRIPTION
    This function sends QMI_PDC indications after activate config
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] list configs request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_activate_config_ind
( 
  pdc_client_type               *client,
  pdc_activate_config_req_msg_v01 *request
)
{
  pdc_activate_config_ind_msg_v01 activate_config_ind;
  qmi_csi_error qmi_error;
  mcfg_error_e_type mcfg_error;
#ifdef FEATURE_MCFG_REFRESH
  pdc_activate_cache_type *p_cache;
  uint8 slot;
#endif
  /*-----------------------------------------------------------------------*/
  
  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_activate_config_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_activate_config_ind(): request pointer NULL");
    return;
  }
  
  /* init indication message */
  memset(&activate_config_ind, 0, sizeof(pdc_activate_config_ind_msg_v01));
  activate_config_ind.ind_token_valid = request->ind_token_valid;  
  activate_config_ind.ind_token = request->ind_token;

#ifdef FEATURE_MCFG_REFRESH
  if (request->activation_type_valid && 
      request->activation_type == PDC_ACTIVATION_REFRESH_ONLY_V01)
  {
    MCFG_MSG_LOW("QMI_PDC_ACTIVATE refresh only");
    if (request->slot_id_valid 
        && request->slot_id <MCFG_NUM_OF_UIM_SLOT
        && request->subscription_id_valid 
        && request->subscription_id_valid <MCFG_NUM_OF_SUB_ID
        && mcfg_refresh_enabled(request->slot_id))
    {
      activate_config_ind.error = pdc_err_map_mcfg_to_qmi(
        mcfg_utils_activate_refreshonly(request->slot_id, request->subscription_id));
      if(activate_config_ind.error == QMI_ERR_NONE_V01)
      {
        p_cache = &global_pdc_svc.activate_cache[request->slot_id];
        /* indication will be sent out later */
        p_cache->qmi_client_handle = client->qmi_client_handle;
        p_cache->ind_token_valid = request->ind_token_valid;
        p_cache->ind_token = request->ind_token;
        return;
      }
    }
    else
    {
      activate_config_ind.error = QMI_ERR_NOT_SUPPORTED_V01;
    }
  }
  else
#endif
  {
    mcfg_error = mcfg_utils_activate_config(request->config_type, MCFG_ACTIVATION_BY_PDC); 
    
    if(mcfg_error != MCFG_ERR_NONE)
    {
      activate_config_ind.error = pdc_err_map_mcfg_to_qmi(mcfg_error);
    }
    else
    {
#ifdef FEATURE_MCFG_REFRESH
      activate_config_ind.error = QMI_ERR_NONE_V01;

      for (slot = MCFG_SUB_ID_FIRST; slot < MCFG_NUM_OF_UIM_SLOT; slot++)
      {
        if (mcfg_refresh_get_orig(slot) == MCFG_REFRESH_ORIG_PDC)
        {
          p_cache = &global_pdc_svc.activate_cache[slot];

          /* indication will be sent out later */
          p_cache->qmi_client_handle = client->qmi_client_handle;
          p_cache->ind_token_valid = request->ind_token_valid;
          p_cache->ind_token = request->ind_token;

          return;
        }
      }
#endif
    }
  }

  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_ACTIVATE_CONFIG_IND_V01,
                               &activate_config_ind,
                               sizeof(pdc_activate_config_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to activate config indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_ACTIVATE_CONFIG_IND_V01 is sent");
  }

  
} /* pdc_send_activate_config_ind() */

/*===========================================================================

  FUNCTION pdc_send_cached_activate_config_ind

  DESCRIPTION
    This function sends QMI_PDC indications after activate refresh only
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    slot    [in] slot index for refresh

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void pdc_send_cached_activate_config_ind
( 
  uint8             slot_index,
  mcfg_error_e_type mcfg_err
)
{
  pdc_activate_config_ind_msg_v01 activate_config_ind;
  qmi_csi_error qmi_error;
  pdc_activate_cache_type *p_cache;
  /*-----------------------------------------------------------------------*/
 
  MCFG_ASSERT_COND_RET(slot_index < MCFG_NUM_OF_UIM_SLOT);
   
  /* init indication message */
  p_cache = &global_pdc_svc.activate_cache[slot_index];

  memset(&activate_config_ind, 0, sizeof(pdc_activate_config_ind_msg_v01));
  activate_config_ind.ind_token_valid = p_cache->ind_token_valid;  
  activate_config_ind.ind_token = p_cache->ind_token ;
  activate_config_ind.error = pdc_err_map_mcfg_to_qmi(mcfg_err);

  qmi_error = qmi_csi_send_ind(p_cache->qmi_client_handle,
                               QMI_PDC_ACTIVATE_CONFIG_IND_V01,
                               &activate_config_ind,
                               sizeof(pdc_activate_config_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_MED_1("failed to activate config indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_ACTIVATE_CONFIG_IND_V01 is sent");
  }

  memset(p_cache, 0, sizeof(pdc_activate_cache_type));
} /* pdc_send_cached_activate_config_ind() */

/*===========================================================================

  FUNCTION pdc_send_deactivate_config_ind

  DESCRIPTION
    This function sends QMI_PDC indications after deactivate config
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] list configs request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_deactivate_config_ind
( 
  pdc_client_type               *client,
  pdc_deactivate_config_req_msg_v01 *request
)
{
  pdc_deactivate_config_ind_msg_v01 deactivate_config_ind;
  qmi_csi_error qmi_error;
  mcfg_sub_id_type_e_type sub;
#ifdef FEATURE_MCFG_HW_CONFIG_REQUIRED
  mcfg_config_id_s_type config_id;
#endif
 
  /*-----------------------------------------------------------------------*/
  
  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_deactivate_config_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_deactivate_config_ind(): request pointer NULL");
    return;
  }
  
  if (request->subscription_id_valid == FALSE)
  {
    sub = MCFG_SUB_ID_FIRST;
  }
  else
  {
    sub = request->subscription_id;
  }

  /* init indication message */
  memset(&deactivate_config_ind, 0, sizeof(pdc_deactivate_config_ind_msg_v01));
  deactivate_config_ind.ind_token_valid = request->ind_token_valid;  
  deactivate_config_ind.ind_token = request->ind_token;

#ifdef FEATURE_MCFG_HW_CONFIG_REQUIRED
  if (request->config_type == MCFG_TYPE_HW &&
      (mcfg_utils_get_active_config(MCFG_TYPE_SW, &config_id, MCFG_SUB_ID_FIRST) || 
       mcfg_utils_get_selected_config(MCFG_TYPE_SW, &config_id, MCFG_SUB_ID_FIRST)))
  {
    MCFG_MSG_ERROR("SW/Carrier MBN has to be deactivated first");
    deactivate_config_ind.error = QMI_ERR_INVALID_OPERATION_V01;
  }
  else 
#endif
  if(!mcfg_utils_deactivate_config(request->config_type, sub, FALSE))
  {
    deactivate_config_ind.error = QMI_ERR_INTERNAL_V01;
  }
  else
  {
    deactivate_config_ind.error = QMI_ERR_NONE_V01;
  }

  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_DEACTIVATE_CONFIG_IND_V01,
                               &deactivate_config_ind,
                               sizeof(pdc_deactivate_config_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to deactivate config indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_DEACTIVATE_CONFIG_IND_V01 is sent");
  }
} /* pdc_send_deactivate_config_ind() */

/*===========================================================================

  FUNCTION pdc_send_get_config_info_ind

  DESCRIPTION
    This function sends a QMI_PDC_GET_CONFIG_INFO_IND indication with a binary
    blob containing the config info trailer of the given configuration ID.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] get selected config request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_get_config_info_ind
( 
  pdc_client_type                 *client,
  pdc_get_config_info_req_msg_v01 *request
)
{
  pdc_get_config_info_ind_msg_v01 config_info_ind;
  mcfg_config_id_s_type config_id;
  mcfg_config_info_s_type config_info;
  qmi_csi_error qmi_error;

  /*-----------------------------------------------------------------------*/
  
  /* Check the input pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_get_config_info_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_get_config_info_ind(): request pointer NULL");
    return;
  }

  /* The request handler should have already validated the input parameters
     (the config type and ID) */
  
  /* Initialize the indication message */
  memset(&config_info_ind, 0, sizeof(pdc_get_config_info_ind_msg_v01));
  config_info_ind.ind_token_valid = request->ind_token_valid;  
  config_info_ind.ind_token = request->ind_token;

  config_id.id_len = request->new_config_info.config_id_len;
  memscpy(config_id.id, MCFG_CONFIG_ID_SIZE_MAX,
          request->new_config_info.config_id, config_id.id_len);

  /* Verify the config ID exists then build the indication */
  if (mcfg_utils_is_config_id_unique(request->new_config_info.config_type,
                                     &config_id))
  {
    MCFG_MSG_ERROR("pdc_send_get_config_info_ind(): the config ID does "
                   "not exist");
    config_info_ind.error = QMI_ERR_INVALID_ID_V01;
  }
  else
  {
    if (mcfg_utils_get_config_info(request->new_config_info.config_type, 
                                   &config_id, &config_info))
    {
      config_info_ind.config_size_valid = TRUE;
      config_info_ind.config_size = config_info.config_len;

      if (mcfg_trl_decode_tlv(
            &config_info, MCFG_TRL_TLV_ID_CARRIER_NAME, 
            config_info_ind.config_desc, MCFG_TRL_CARRIER_NAME_MAX_LEN+1))
      {
        config_info_ind.config_desc_len = 
          strlen((char *)config_info_ind.config_desc);
        if (config_info_ind.config_desc_len > MCFG_TRL_CARRIER_NAME_MAX_LEN)
        {
          config_info_ind.config_desc_len = MCFG_TRL_CARRIER_NAME_MAX_LEN;
        }
        config_info_ind.config_desc_valid = TRUE;
      }

      if (mcfg_trl_decode_tlv(&config_info, MCFG_TRL_TLV_ID_CONFIG_VERSION, 
                              &config_info_ind.config_version, 
                              sizeof(config_info_ind.config_version)))
      {
        config_info_ind.config_version_valid = TRUE;
      }

      if (mcfg_trl_decode_tlv(&config_info, MCFG_TRL_TLV_ID_BASE_VERSION, 
                              &config_info_ind.base_version, 
                              sizeof(config_info_ind.base_version)))
      {
        config_info_ind.base_version_valid = TRUE;
      }

      config_info_ind.storage = (pdc_storage_enum_v01) config_info.storage;
      config_info_ind.storage_valid = TRUE;
      if (config_info_ind.storage == PDC_STORAGE_REMOTE_V01)
      {
         memscpy(config_info_ind.path, MCFG_CONFIG_PATH_SIZE_MAX,
                 config_info.path, MCFG_CONFIG_PATH_SIZE_MAX);
         config_info_ind.path_valid = TRUE;
         config_info_ind.error = QMI_ERR_NONE_V01;
      }
      else if (config_info_ind.storage == PDC_STORAGE_LOCAL_V01)
      {
         config_info_ind.path_valid = FALSE;
         config_info_ind.error = QMI_ERR_NONE_V01;
      }
      else 
      {
         config_info_ind.storage_valid = FALSE;
         config_info_ind.error = QMI_ERR_INTERNAL_V01;
      }
    }
    else
    {
      config_info_ind.error = QMI_ERR_INTERNAL_V01;
    }
  }

  /* Send the indication message */
  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_GET_CONFIG_INFO_IND_V01,
                               &config_info_ind,
                               sizeof(pdc_get_config_info_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to send indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_GET_CONFIG_INFO_IND_V01 is sent");
  }
} /* pdc_send_get_config_info_ind() */

/*===========================================================================

  FUNCTION pdc_send_get_config_limits_ind

  DESCRIPTION
    This function sends QMI_PDC indications after get config limits
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] get selected config request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_get_config_limits_ind
( 
  pdc_client_type                   *client,
  pdc_get_config_limits_req_msg_v01 *request
)
{
  pdc_get_config_limits_ind_msg_v01 config_limits_ind;
  qmi_csi_error qmi_error;
  
  /*-----------------------------------------------------------------------*/
  
  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_get_config_limits_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_get_config_limits_ind(): request pointer NULL");
    return;
  }

  /* req handler suppose already verified input parameter: config type */
  
  /* init indication message */
  memset(&config_limits_ind, 0, sizeof(pdc_get_config_limits_ind_msg_v01));
  config_limits_ind.ind_token_valid = request->ind_token_valid;  
  config_limits_ind.ind_token = request->ind_token;

  if (mcfg_utils_get_config_limits(request->config_type, 
                                   &config_limits_ind.max_config_size,
                                   &config_limits_ind.curr_config_size))
  {
    config_limits_ind.max_config_size_valid = TRUE;
    config_limits_ind.curr_config_size_valid = TRUE;
    config_limits_ind.error = QMI_ERR_NONE_V01;
  }
  else
  {
    config_limits_ind.error = QMI_ERR_INTERNAL_V01;
  }

  /* send indication message */
  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_GET_CONFIG_LIMITS_IND_V01,
                               &config_limits_ind,
                               sizeof(pdc_get_config_limits_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to send indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_GET_CONFIG_LIMITS_IND_V01 is sent");
  }
} /* pdc_send_get_config_limits_ind() */

/*===========================================================================

  FUNCTION pdc_send_get_default_config_info_ind

  DESCRIPTION
    This function sends a QMI_PDC_GET_DEFAULT_CONFIG_INFO_IND indication
    with a binary blob containing the config info trailer of default config

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] get selected config request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_get_default_config_info_ind
( 
  pdc_client_type                         *client,
  pdc_get_default_config_info_req_msg_v01 *request
)
{
  pdc_get_default_config_info_ind_msg_v01 config_info_ind;
  mcfg_config_info_s_type config_info;
  qmi_csi_error qmi_error;
  
  /*-----------------------------------------------------------------------*/
  
  /* Check the input pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("get_default_config_info_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("get_default_config_info_ind(): request pointer NULL");
    return;
  }

  /* The request handler should have already validated the input parameters
     (the config type and ID) */
  
  /* Initialize the indication message */
  memset(&config_info_ind, 0, sizeof(pdc_get_default_config_info_ind_msg_v01));
  config_info_ind.ind_token_valid = request->ind_token_valid;  
  config_info_ind.ind_token = request->ind_token;

  /* Verify the config ID exists then build the indication */
  if (mcfg_utils_get_default_config_info(request->config_type, &config_info))
  {
    config_info_ind.config_size_valid = TRUE;
    config_info_ind.config_size = config_info.config_len;

    if (mcfg_trl_decode_tlv(
          &config_info, MCFG_TRL_TLV_ID_CARRIER_NAME, 
          config_info_ind.config_desc, MCFG_TRL_CARRIER_NAME_MAX_LEN+1))
    {
      config_info_ind.config_desc_len = 
        strlen((char *)config_info_ind.config_desc);
      if (config_info_ind.config_desc_len > MCFG_TRL_CARRIER_NAME_MAX_LEN)
      {
        config_info_ind.config_desc_len = MCFG_TRL_CARRIER_NAME_MAX_LEN;
      }
      config_info_ind.config_desc_valid = TRUE;
    }

    if (mcfg_trl_decode_tlv(&config_info, MCFG_TRL_TLV_ID_CONFIG_VERSION, 
                            &config_info_ind.config_version, 
                            sizeof(config_info_ind.config_version)))
    {
      config_info_ind.config_version_valid = TRUE;
    }

    config_info_ind.error = QMI_ERR_NONE_V01;
  }
  else
  {
    config_info_ind.error = QMI_ERR_INTERNAL_V01;
  }

  /* Send the indication message */
  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_GET_DEFAULT_CONFIG_INFO_IND_V01,
                               &config_info_ind,
                               sizeof(pdc_get_default_config_info_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to send indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_GET_DEFAULT_CONFIG_INFO_IND_V01 is sent");
  }
} /* pdc_send_get_default_config_info_ind() */

/*===========================================================================

  FUNCTION pdc_send_get_feature_ind

  DESCRIPTION
    This function sends QMI_PDC indications after the get feature config
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] get feature config request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_get_feature_ind
( 
  pdc_client_type              *client,
  pdc_get_feature_req_msg_v01  *request
)
{
  pdc_get_feature_ind_msg_v01 feature_ind;
  qmi_csi_error qmi_error;
  
  /*-----------------------------------------------------------------------*/
  
  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_get_feature_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_get_feature_ind(): request pointer NULL");
    return;
  }

  /* init indication message */
  memset(&feature_ind, 0, sizeof(pdc_get_feature_ind_msg_v01));
  feature_ind.ind_token_valid = request->ind_token_valid;  
  feature_ind.ind_token = request->ind_token;

  /* generate indication message */
  feature_ind.error = pdc_err_map_mcfg_to_qmi(
    mcfg_get_feature(request->slot_id, &feature_ind));

  /* send indication message */
  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_GET_FEATURE_IND_V01,
                               &feature_ind,
                               sizeof(pdc_get_feature_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to send indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_GET_FEATURE_IND_V01 is sent");
  }
} /* pdc_send_get_feature_ind() */


/*===========================================================================

  FUNCTION pdc_send_set_feature_ind

  DESCRIPTION
    This function sends QMI_PDC indications after the set feature config
    req/response are processed.

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] set feature config request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_set_feature_ind
( 
  pdc_client_type                      *client,
  pdc_set_feature_req_msg_v01  *request
)
{
  pdc_set_feature_ind_msg_v01 feature_ind;
  qmi_csi_error qmi_error;
  mcfg_slot_index_type_e_type sloti;
  
  /*-----------------------------------------------------------------------*/
  
  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_get_feature_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_get_feature_ind(): request pointer NULL");
    return;
  }
  
  /* req handler suppose already verified input parameter: config type */
  
  /* init indication message */
  memset(&feature_ind, 0, sizeof(pdc_set_feature_ind_msg_v01));
  feature_ind.ind_token_valid = request->ind_token_valid;  
  feature_ind.ind_token = request->ind_token;

  if (request->slot_id != 0xFFFFFFFF)
  {
  /* generate indication message */
  feature_ind.error = pdc_err_map_mcfg_to_qmi(mcfg_set_feature(request));
  }
  else
  {
    for (sloti = 0; sloti < MCFG_NUM_OF_UIM_SLOT; sloti++)
    {
      /* generate indication message */
      request->slot_id = sloti;
      feature_ind.error = pdc_err_map_mcfg_to_qmi(mcfg_set_feature(request));
      if (feature_ind.error != QMI_ERR_NONE_V01 &&
          feature_ind.error != QMI_ERR_NO_EFFECT_V01)
      {
        break;
      }
    }
  }
  
  /* send indication message */
  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_PDC_SET_FEATURE_IND_V01,
                               &feature_ind,
                               sizeof(pdc_set_feature_ind_msg_v01));

  if (qmi_error != QMI_CSI_NO_ERR)
  {
    MCFG_MSG_ERROR_1("Failure to send indication: %d", qmi_error);
  }
  else
  {
    MCFG_MSG_LOW("QMI_PDC_SET_FEATURE_IND_V01 is sent");
  }
} /* pdc_send_set_feature_ind() */

/*==============================================================================

  FUNCTION pdc_default_load_config_cache

  DESCRIPTION
    This function initialize / reinitialize the cached load config
    information.

  DEPENDENCIES
    QCSI/QSAP must be initialized

  PARAMETERS
    cache [in] - load_config_cache

  RETURN VALUE
    None

  SIDE EFFECTS
    
==============================================================================*/
static void pdc_default_load_config_cache
( 
  pdc_load_config_cache_type *cache
)
{
  
  /*-----------------------------------------------------------------------*/

  if (NULL == cache)
  {
    MCFG_MSG_HIGH( "pdc_default_load_config_cache(): cache NULL");
    return;
  }
  
  if (cache->config_s.config_addr)
  {
    mcfg_utils_free_config_buffer(&cache->config_s);
  }

  memset(cache, 0, sizeof(pdc_load_config_cache_type));

  MCFG_MSG_HIGH( "pdc_default_load_config_cache(): cache is now reset");
  return;
} /* pdc_default_load_config_cache() */


/*===========================================================================

  FUNCTION pdc_send_validate_config_ind

  DESCRIPTION

  DEPENDENCIES
    QMI_PDC must be initialized

  PARAMETERS
    client    [in] pdc client
    request   [in] validate config request

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void pdc_send_validate_config_ind
( 
  pdc_client_type                 *client,
  pdc_validate_config_req_msg_v01 *request
)
{
  pdc_validate_config_ind_msg_v01 *validate_config_ind;
  mcfg_config_id_s_type   config_id;
  mcfg_config_type_e_type type;
  mcfg_sub_id_type_e_type sub;
  qmi_csi_error qmi_error;
  /*-----------------------------------------------------------------------*/
  // For XML reading
  /*-----------------------------------------------------------------------*/
  struct fs_stat     temp_buf;
  int                fd=-1;
  char               f_name[128]; /* Increase size from 120 to 128 for context*/
  fs_size_t          nbytes;
  fs_size_t          bytes_read;
  fs_size_t          read_bytes;
  fs_size_t          bytes_left;
  boolean            read_success;
  fs_ssize_t         result = 0;


  MCFG_MSG_MED( "pdc_send_validate_config_ind(): successful entrance" );

  /* Check pass pointers */
  if (!client)
  {
    MCFG_MSG_ERROR("pdc_send_validate_config_ind(): client pointer NULL");
    return;
  }
  
  if (!request)
  {
    MCFG_MSG_ERROR("pdc_send_validate_config_ind(): request pointer NULL");
    return;
  }
  
  /* Get memory to store the IND data */
  validate_config_ind = mcfg_malloc(sizeof(pdc_validate_config_ind_msg_v01)); 
  if (!validate_config_ind)
  {
    MCFG_MSG_ERROR( "pdc_send_validate_config_ind(): malloc failed");
    return;
  }

  /* req handler suppose already verified input parameter: config type */
  type = request->config_type;

  if (request->subscription_id_valid == FALSE)
  {
    sub = MCFG_SUB_ID_FIRST;
  }
  else
  {
    sub = request->subscription_id;
  }

  /* init indication message */
  memset(validate_config_ind, 0, sizeof(pdc_validate_config_ind_msg_v01));
  validate_config_ind->ind_token_valid = request->ind_token_valid;  
  validate_config_ind->ind_token = request->ind_token;

  validate_config_ind->result_format_valid = TRUE;
  validate_config_ind->frame_index_valid   = TRUE;
  validate_config_ind->result_frame_valid  = TRUE;
  // not needed since memset but listed here for clarity
  //validate_config_ind->result_format = 0;
  //validate_config_ind->frame_index   = 0;
  //validate_config_ind->result_frame  = {0};

  validate_config_ind->error = QMI_ERR_NONE_V01;

 /* generate indication message */
  if (request->config_id_valid == TRUE)
  {
     config_id.id_len = request->config_id_len;
     memscpy(config_id.id, PDC_CONFIG_ID_SIZE_MAX_V01, 
             request->config_id, config_id.id_len);

     /* as long as the config is on the target the config id is valid */
     if (mcfg_utils_is_config_id_unique(type, &config_id))
     {
       MCFG_MSG_LOW("pdc_send_validate_config_ind(): Config ID not present on target");
       validate_config_ind->error = QMI_ERR_INVALID_ID_V01;
     }
     else 
     {
        // Config exists on the device
     }
  }
  else
  {
     if (!mcfg_utils_get_active_config(type, &config_id, sub))
     {
        MCFG_MSG_ERROR_2("pdc_send_validate_config_ind(): No config id found for cfg_type %d"\
                         " on subscription %d", type, sub);
        validate_config_ind->error = QMI_ERR_INVALID_ID_V01;
     }
  }

  if (validate_config_ind->error == QMI_ERR_NONE_V01)
  {
    validate_config_ind->error = pdc_err_map_mcfg_to_qmi( mcfg_validate_config(type, &config_id, sub) );
  }

  /* If qmi error at this point then either no config id was passed and there 
     is no active config or a config id was passed but not present on the target */
  if (validate_config_ind->error != QMI_ERR_NONE_V01)
  {
    MCFG_MSG_LOW("Error validating config");
     validate_config_ind->frame_index_valid = FALSE;
     validate_config_ind->result_frame_valid = FALSE;

     qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                                  QMI_PDC_VALIDATE_CONFIG_IND_V01,
                                  validate_config_ind,
                                  sizeof(pdc_validate_config_ind_msg_v01));

     if (qmi_error != QMI_CSI_NO_ERR)
     {
       MCFG_MSG_ERROR_1("failed to send validate config indication: %d", qmi_error);
     }
     else
     {
       MCFG_MSG_LOW("QMI_PDC_VALIDATE_CONFIG_IND_V01 is sent");
     }

     return;
  }

  (void)snprintf(f_name, sizeof(f_name), "/mcfg_test.xml");

  /* Check if the file is present or not. */
  if(efs_stat(f_name,&temp_buf) == -1) 
  {
    MCFG_MSG_ERROR("MCFG: /mcfg_test.xml not found");
    validate_config_ind->error = QMI_ERR_INTERNAL_V01;
  } 
  else if(temp_buf.st_size <= 1)
  {
     MCFG_MSG_ERROR("pdc_send_validate_config_ind(): XML file is 0B");
     validate_config_ind->error = QMI_ERR_INTERNAL_V01;
  }
  else 
  {
     fd = efs_open(f_name, O_RDWR, 0777 );
     if (fd < 0)
     {
        MCFG_MSG_ERROR("pdc_send_validate_config_ind(): error opening file");
        validate_config_ind->error = QMI_ERR_INTERNAL_V01;
     }
  }

  /* XML file successfully opened if this point is reached*/
  nbytes = temp_buf.st_size;
  read_success = TRUE;
  bytes_read = 0;
  while ((bytes_read < nbytes) && (TRUE == read_success))
  {
    bytes_left = nbytes - bytes_read;
    if (bytes_left < PDC_CONFIG_DESC_SIZE_MAX_V01)
    {
       read_bytes = bytes_left;
       validate_config_ind->frame_index = 0xFFFFFFFF;
    }
    else 
    {
       read_bytes = PDC_CONFIG_DESC_SIZE_MAX_V01;
    }
    result = efs_read (fd,
                       (void *) validate_config_ind->result_frame,
                       read_bytes);

    MCFG_MSG_MED_5("fsize: %d, bRead: %d, bytes_left: %d, readB: %d, result: %d", 
                    nbytes, bytes_read, bytes_left, read_bytes, result);

    if (result <= 0)
    {
       MCFG_MSG_ERROR_1("pdc_send_validate_config_ind(): read failure from file after %lu bytes read", bytes_read); 
       validate_config_ind->error = QMI_ERR_INTERNAL_V01;
       validate_config_ind->frame_index_valid = FALSE;
       validate_config_ind->result_frame_valid = FALSE;
       read_success = FALSE;
    }

    validate_config_ind->result_frame_len = result;
    qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                                 QMI_PDC_VALIDATE_CONFIG_IND_V01,
                                 validate_config_ind,
                                 sizeof(pdc_validate_config_ind_msg_v01));

    if (qmi_error != QMI_CSI_NO_ERR)
    {
      MCFG_MSG_ERROR_1("failed to send validate config indication: %d", qmi_error);
    }
    else
    {
      MCFG_MSG_LOW("QMI_PDC_VALIDATE_CONFIG_IND_V01 is sent");
    }

    /* This will overflow during last frame, but shouldn't matter
       since loop will not be entered again */
    validate_config_ind->frame_index++;
    bytes_read += (fs_size_t) result;
  }
  if (fd >= 0) 
  {
     efs_close(fd);
  }
  mcfg_free(validate_config_ind);
} /* pdc_send_validate_config_ind() */

/*==============================================================================

  FUNCTION pdc_svc_init

  DESCRIPTION
    This function registers the PDC service with the QCSI framework.  The service
    is also registered with QSAP so that it is available via QMUX framework.

  DEPENDENCIES
    QCSI/QSAP must be initialized

  PARAMETERS
    os_params [in] - task parameters for the service

  RETURN VALUE
    Service handle from the QCSI framework

  SIDE EFFECTS
    Registers with the QCSI framework and QSAP

==============================================================================*/
qmi_csi_service_handle pdc_svc_init
( 
  qmi_csi_os_params *os_params
)
{
  qmi_idl_service_object_type  pdc_service_object = NULL;
  qmi_csi_error                qmi_csi_error;
  qmi_sap_client_handle        pdc_sap_handle;
  qmi_sap_error                qmi_sap_error;
  
  pdc_service_type            *pdc_svc_state;
  qmi_csi_options              options;
  
  /*-----------------------------------------------------------------------*/

  if (NULL == os_params)
  {
    MCFG_MSG_ERROR( "pdc_svc_init(): os_params NULL");
    return NULL;
  }

  pdc_svc_state = pdc_get_service_state();
  memset(pdc_svc_state, 0, sizeof(*pdc_svc_state));
  if (!pdc_svc_state)
  {
    MCFG_MSG_ERROR( "pdc_svc_init(): service state NULL");
    return NULL;
  }

  pdc_default_load_config_cache(&pdc_svc_state->load_config_cache);
  
  pdc_service_object = pdc_get_service_object_v01();

  /*-------------------------------------------------------------------------
    Register QMI PDC service with QMI SI for logging of supported interfaces
  -------------------------------------------------------------------------*/
  (void) qmi_si_register_object( pdc_service_object,
                                 0, /* Service Instance */
                                 pdc_get_service_impl_v01() );

  /* Register with QCSI so clients can connect using IPCRouter */
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);
  qmi_csi_error = qmi_csi_register_with_options( pdc_service_object,
                                    pdc_connect_cb, 
                                    pdc_disconnect_cb,
                                    pdc_process_req_cb,
                                    pdc_svc_state,
                                    os_params,
                                    &options,
                                    &pdc_svc_state->pdc_service_handler );

  if ( qmi_csi_error == QMI_CSI_NO_ERR)
  {
    /* Register with QSAP so clients can connect using QMUX */
    qmi_sap_error = qmi_sap_register( pdc_service_object,
                                      NULL,
                                      &pdc_sap_handle );
    
    if ( qmi_sap_error != QMI_SAP_NO_ERR)
    {
      MCFG_MSG_ERROR_1( "pdc_svc_init(): QSAP error: %d", qmi_sap_error);
    }
  }
  else
  {
    MCFG_MSG_ERROR_1( "pdc_svc_init(): QCSI error: %d", qmi_csi_error);
  }

  return pdc_svc_state->pdc_service_handler;

} /* pdc_svc_init() */


