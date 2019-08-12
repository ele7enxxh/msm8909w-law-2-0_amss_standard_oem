/*===========================================================================

                         D S _ Q M I _ A U T H . C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

EXTERNALIZED FUNCTIONS

  qmi_auth_init()
    Register the AUTH service with QMUX for all applicable QMI links
    

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_auth.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/12/13    vrk    Corrected variable intialization in bind subscription
12/07/13    vrk    Added EAP Notificaton code Indication support
07/20/13    gk     Corrected method_mask TLV validation
07/08/13    gk     Modified the subscription validation properly
06/11/13    gk     Fixed KW errors 
06/06/13    gk     Added support for subscription binding and multisim support
08/27/12    gk     Added support for EAP-AKA-PRIME method in start_eap_session_message
08/06/12    gk     Added support for QMI service to log what messages/TLVs  
                   are supported on this branch.
07/26/12    sb     Fixed KW Errors.
06/04/12    wc     Fix futex leaking in client deallocation
04/25/11    sa     Fix to avoid crash because of incompatible auth command id type.
12/17/10    kk     Removing DCC command buffer dependency from QMI services.
11/08/10    mct    Support for AKAv2 QMI messages.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
09/12/08    ks     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DATA_QMI_EAP
#include <stdio.h>
#include <string.h>

#include "dsm.h"
#include "nv.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_task.h"
#include "ds_qmux_ext.h"
#include "ds_qmux.h"
#include "ds_qmi_svc_ext.h"
#include "ps_eap.h"
#include "dcc_task_defs.h"
#include "dcc_task_svc.h"

#include "ps_eap_sim.h"
#include "ps_eap_aka.h"
#include "ps_eap_aka_prime.h"
#include "ps_eap_sim_aka.h"
#include "ps_utils_aka.h"
#include "ps_eap_client_config.h"

#ifdef FEATURE_MMGSDI_SESSION_LIB
  #include "mmgsdilib_common.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"
#include "qmi_framework.h"
#include "ds_qmi_auth.h"
#include "ds_qmi_fw_common.h"

#include "ps_system_heap.h"
#include "ds_Utils_DebugMsg.h"

#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "authentication_service_v01.h"
#include "authentication_service_impl_v01.h"
#include "ps_utils.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define AUTHI_MAX_CLIDS    (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)
#define AUTHI_INVALID_INSTANCE    -1

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for AUTH
---------------------------------------------------------------------------*/
#define AUTHI_BASE_VER_MAJOR    (1)
#define AUTHI_BASE_VER_MINOR    (3)

#define AUTHI_ADDENDUM_VER_MAJOR  (0)
#define AUTHI_ADDENDUM_VER_MINOR  (0)

/*---------------------------------------------------------------------------
  QMI_AUTH_START_EAP_SESSION TLV Message IDs
---------------------------------------------------------------------------*/
#define AUTHI_PRM_TYPE_EAP_METHOD_MASK   (0x10)
#define AUTHI_PRM_TYPE_EAP_USER_ID       (0x11)
#define AUTHI_PRM_TYPE_EAP_META_IDENTITY (0x12)
#define AUTHI_PRM_TYPE_EAP_SIM_AKA_ALGO  (0x13)


/*---------------------------------------------------------------------------
  AKA Run Algorithm Parameters
-----------------------------------------------------------------------------*/
#define AUTHI_PRM_TYPE_AKA_PARAMS        (0x10)

/*---------------------------------------------------------------------------
  AKA Algorithm Result Data
-----------------------------------------------------------------------------*/
#define AUTHI_PRM_TYPE_AKA_RESULT_DATA        (0x10)

/*---------------------------------------------------------------------------
  bind_subscription TLV definitions
---------------------------------------------------------------------------*/
#define AUTHI_PRM_TYPE_BIND_SUBSCRIPTION (0x10)

/*---------------------------------------------------------------------------
  Indication_register TLV definitions
---------------------------------------------------------------------------*/
#define AUTHI_PRM_TYPE_EAP_NOTIFICATION_CODE (0x10)

/*---------------------------------------------------------------------------
  Control Cap BIT MASK
---------------------------------------------------------------------------*/
typedef enum
{
  AUTHI_EAP_METHOD_NONE = 0x0,
  AUTHI_EAP_METHOD_SIM  = 0x1,
  AUTHI_EAP_METHOD_AKA       = 0x2,
  AUTHI_EAP_METHOD_AKA_PRIME = 0x4
} authi_control_cap_e_type;


/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }




/*===========================================================================

                                DATA TYPES

===========================================================================*/


/*---------------------------------------------------------------------------
  AUTH Command enum type - not equal to the actual command values!
  mapping is in qmi_auth_cmd_callbacks table

  DO NOT REORDER THIS ENUM!
---------------------------------------------------------------------------*/
typedef enum
{
  AUTHI_CMD_MIN                          = 0,
  AUTHI_CMD_RESET                        = AUTHI_CMD_MIN,
  AUTHI_CMD_START_EAP_SESSION            ,
  AUTHI_CMD_SEND_EAP_PACKET              ,
  AUTHI_CMD_GET_EAP_SESSION_KEYS         ,
  AUTHI_CMD_END_EAP_SESSION              ,
  AUTHI_CMD_RUN_AKA_ALGO                 ,
  AUTHI_CMD_SET_SUBSCRIPTION_BINDING     ,
  AUTHI_CMD_GET_BIND_SUBSCRIPTION        ,
  AUTHI_CMD_INDICATION_REGISTER          ,
  AUTHI_CMD_MAX, 
  AUTHI_CMD_WIDTH                    = 0xFFFF                        
} qmi_authi_cmd_e_type;

typedef enum
{
  AUTHI_CMD_VAL_RESET                         = 0x0000,
  AUTHI_CMD_VAL_INDICATION_REGISTER           = 0x0003,
  AUTHI_CMD_VAL_START_EAP_SESSION             = 0x0020,
  AUTHI_CMD_VAL_SEND_EAP_PACKET               = 0x0021,
  AUTHI_CMD_VAL_EAP_SESSION_RESULT_IND        = 0x0022,
  AUTHI_CMD_VAL_GET_EAP_SESSION_KEYS          = 0x0023,
  AUTHI_CMD_VAL_END_EAP_SESSION               = 0x0024,
  AUTHI_CMD_VAL_RUN_AKA_ALGO                  = 0x0025,
  AUTHI_CMD_VAL_AKA_ALGO_RESULT_IND           = 0x0026,
  AUTHI_CMD_VAL_SET_SUBSCRIPTION_BINDING      = 0x0027,
  AUTHI_CMD_VAL_GET_BIND_SUBSCRIPTION         = 0x0028,
  AUTHI_CMD_VAL_EAP_NOTIFICATION_CODE_IND     = 0x0029,
/* QC EXTERNAL QMI COMMAND RANGE IS 0x0000 - 0x5555. 
   Add the next external QMI Command here */

/* VENDOR SPECIFIC QMI COMMAND RANGE IS 0x5556 - 0xAAAA.
   IMPORTANT!
   Add the vendor specific QMI Commands within this range only to avoid 
   conflicts with QC QMI commands that would get released in future */

/* RESERVED QC QMI COMMAND RANGE IS 0xAAAB - 0xFFFE */
  AUTHI_CMD_VAL_WIDTH                      = 0xFFFF
} qmi_authi_cmd_val_e_type;

/*---------------------------------------------------------------------------
  AUTH client sunscription preference enum
---------------------------------------------------------------------------*/
typedef enum
{
  AUTHI_BIND_SUBS_PRIMARY   = 0x01,
  AUTHI_BIND_SUBS_SECONDARY = 0x02,
  AUTHI_BIND_SUBS_TERTIARY  = 0x03,
  AUTHI_BIND_SUBS_MAX
} qmi_authi_client_subs_e_type;
/*---------------------------------------------------------------------------
  QMI AUTH instance state definition &
  AUTH client state definition
---------------------------------------------------------------------------*/
#define QMI_AUTH_MAX_AKA_RAND_LEN_SIZE   255
#define QMI_AUTH_MAX_AKA_AUTN_LEN_SIZE   255
#define QMI_AUTH_MAX_AKA_DIGEST_LEN_SIZE 255
#define QMI_AUTH_MAX_AKA_DATA_LEN_SIZE   255
#define QMI_EAP_MAX_PRE_MASTER_KEY_SIZE  256

typedef struct 
{
  qmi_common_client_state_type common; // must be first since we alias
  int16     instance;  
  int16     service_id;  
  struct
  {
    eap_handle_type              eap_handle;
    qmi_cmd_buf_type           * pending_req_cmd_buf;
    eap_result_enum_type         result;
    uint8                        msk[QMI_EAP_MAX_PRE_MASTER_KEY_SIZE];
    uint16                       msk_len; 
    boolean                      eap_result_ready;
  }eap_info;

  qmi_authi_client_subs_e_type subscription_id;
  struct 
  {
    boolean  report_eap_notification_code;
  } report_status;
} qmi_authi_client_state_type;

/*---------------------------------------------------------------------------
  Watermark queue to hold deferred responses
---------------------------------------------------------------------------*/
typedef qmi_authi_client_state_type* qmi_authi_client_state_ptr_type; 

typedef struct
{
  uint16                           num_qmi_instances;  
  qmi_authi_client_state_ptr_type  client[AUTHI_MAX_CLIDS];
} qmi_authi_state_type;

typedef struct
{
  uint16 cmd_id;
  union
  {
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
      eap_sim_aka_task_srv_req_cb_type req_cb;
      void *                           user_data;
    } eap_sim_aka;

    struct
    {
      void           * user_data; 
      dsm_item_type  * pkt;
    } eap_resp;
    struct
    {
      uint16             notification_code;
      eap_handle_type    eap_handle; 
    } eap_notification_cb;
  } data;
} qmi_authi_cmd_buf_type;

typedef struct
{
  uint16                cmd_id;
  uint8                 pmk[QMI_EAP_MAX_PRE_MASTER_KEY_SIZE];
  uint16                pmk_len;
  eap_handle_type       handle;
  eap_result_enum_type  result;
  void *                user_data;
} qmi_authi_eap_result_cmd_buf_type;

typedef struct
{
  uint16                         cmd_id;
  ps_utils_aka_handle_type       handle;
  ps_utils_aka_status_enum_type  status;
  uint8                          digest[QMI_AUTH_MAX_AKA_DIGEST_LEN_SIZE];
  uint8                          digest_len;
  uint8                          aka_data[QMI_AUTH_MAX_AKA_DATA_LEN_SIZE];
  uint8                          aka_data_len;
  void                          *user_data;
} qmi_authi_aka_result_cmd_buf_type;

/*---------------------------------------------------------------------------
  QMI AUTH Service configuration 
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_framework_svc_config_type    fw_cfg;
  qmi_svc_cmd_hdlr_type *          cmd_hdlr_array;   
  uint16                           cmd_num_entries;  
  void *                           sp;               
  boolean                          registered;   /* Service registration statu   */ 
} qmi_auth_svc_config_type;


/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

static dsm_item_type* qmi_authi_reset(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_authi_start_eap_session(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_authi_send_eap_packet(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_authi_get_eap_session_keys(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_authi_end_eap_session(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_authi_run_aka_algo(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_authi_set_subscription_binding(void *,void *,void *,dsm_item_type ** );
static dsm_item_type* qmi_authi_get_bind_subscription(void *,void *,void *,dsm_item_type ** );
static dsm_item_type* qmi_authi_indication_register(void*, void*, void*, dsm_item_type **);

#define AUTH_HDLR(a,b)  QMI_SVC_HDLR( a, (qmi_svc_hdlr_ftype)b )

static qmi_svc_cmd_hdlr_type  qmi_authi_cmd_callbacks[AUTHI_CMD_MAX] =
{   
  AUTH_HDLR( AUTHI_CMD_VAL_RESET,
                qmi_authi_reset),
  AUTH_HDLR( AUTHI_CMD_VAL_START_EAP_SESSION,
                qmi_authi_start_eap_session),
  AUTH_HDLR( AUTHI_CMD_VAL_SEND_EAP_PACKET,
                qmi_authi_send_eap_packet),
  AUTH_HDLR( AUTHI_CMD_VAL_GET_EAP_SESSION_KEYS,
                qmi_authi_get_eap_session_keys),
  AUTH_HDLR( AUTHI_CMD_VAL_END_EAP_SESSION,
                qmi_authi_end_eap_session),
  AUTH_HDLR( AUTHI_CMD_VAL_RUN_AKA_ALGO,
                qmi_authi_run_aka_algo),
  AUTH_HDLR( AUTHI_CMD_VAL_SET_SUBSCRIPTION_BINDING,
                qmi_authi_set_subscription_binding),
  AUTH_HDLR( AUTHI_CMD_VAL_GET_BIND_SUBSCRIPTION,
                qmi_authi_get_bind_subscription),
  AUTH_HDLR( AUTHI_CMD_VAL_INDICATION_REGISTER,
		qmi_authi_indication_register)
};

static qmi_authi_state_type  qmi_auth_state;

/*---------------------------------------------------------------------------
  QMI callback definition
---------------------------------------------------------------------------*/

static boolean qmi_authi_alloc_clid_cb
(
  qmi_framework_common_msg_hdr_type * svc_common_hdr  
);

static void qmi_authi_dealloc_clid_cb
(
  qmi_framework_common_msg_hdr_type * svc_common_hdr 
); 

static void qmi_authi_init_cb
(
  uint16 num_instances
);

static void qmi_authi_cmd_hdlr_cb
( 
  qmi_framework_msg_hdr_type* msg_hdr,
  dsm_item_type ** sdu 
);

static qmi_authi_client_state_type * qmi_authi_alloc_cl_sp
(
  uint8 clid
);

static boolean qmi_authi_dealloc_cl_sp
(
  uint8 clid
);

static void    qmi_authi_reset_client(void *);
static void    qmi_authi_initialize_client(qmi_authi_client_state_type *);
static boolean qmi_auth_eap_task_srv_fct
(
  eap_sim_aka_task_srv_req_cb_type req_cb,
  void * user_data
);

/*---------------------------------------------------------------------------
  QMI Service (QMI_AUTH) configuration definition
---------------------------------------------------------------------------*/
static qmi_auth_svc_config_type  qmi_authi_cfg;

static struct
{ 
  boolean inited;
  eap_sim_aka_meta_info_type    sim_aka_meta_info;
} qmi_authi_global = {FALSE,};

/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/

static void qmi_authi_process_eap_sim_aka_task_switch
(
  qmi_authi_cmd_buf_type*
);

static void qmi_authi_send_eap_resp
(
  qmi_authi_cmd_buf_type*
);

static void qmi_authi_send_eap_result_ind
(
  qmi_authi_eap_result_cmd_buf_type*
);

static void qmi_authi_send_aka_result_ind
(
  qmi_authi_aka_result_cmd_buf_type*
);

static void qmi_authi_process_cmd(void *);

static void qmi_authi_process_svc_init
(
  uint16 num_instances
);

static void qmi_authi_process_alloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
);

static void qmi_authi_process_dealloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
);

static void qmi_authi_process_cmd_hdlr
(
  qmi_framework_msg_hdr_type* msg_hdr,
  dsm_item_type * sdu 
);

static void qmi_authi_dispatch_transaction
(
  qmi_transaction_type *  x_p
);

static boolean qmi_authi_input
(
  qmi_cmd_buf_type *             cmd_buf_p,
  dsm_item_type **               sdu_in
);

static boolean qmi_authi_send_response
(
  qmi_authi_client_state_type * cl_sp, 
  qmi_cmd_buf_type *cmd_buf_p, 
  dsm_item_type *  msg_ptr
);

static boolean qmi_authi_queue_response
( 
  qmi_cmd_buf_type **  cmd_buf_p,
  dsm_item_type **     msg
);

static boolean qmi_authi_send_indication
(
  void * msg_hdr_p, 
  uint16 cmd_type, 
  dsm_item_type * ind 
);

static qmi_cmd_buf_type *  qmi_authi_get_transaction_cmd_buf
(
  qmi_transaction_type *  x_p
);

static void qmi_authi_free_transaction_cmd_buf
(
  qmi_cmd_buf_type **  cmd_buf_handle
);

static qmi_transaction_type * qmi_authi_get_transaction
(
  qmi_authi_state_type        *   svc_sp,
  qmi_authi_client_state_type *   cl_sp
);

static void qmi_authi_free_transaction
(
  qmi_transaction_type **  x_p
);

static void *qmi_authi_get_cmd_buf(qmi_cmd_id_e_type,uint16);

static void qmi_authi_generate_eap_notification_code_ind
(
  qmi_authi_cmd_buf_type* cmd_ptr
);

static void qmi_authi_eap_notification_code_ind
(
   uint8 clid,
   uint16 notification_code
);

#define qmi_authi_free_cmd_buf(buf_ptr) PS_SYSTEM_HEAP_MEM_FREE(buf_ptr)

/*===========================================================================
  FUNCTION QMI_AUTH_INIT()

  DESCRIPTION
    Register the AUTH service with QMI Framework 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_auth_init
(
  void
)
{
  qmi_framework_err_e_type  reg_result = QMI_FRAMEWORK_ERR_NONE;
  qmi_idl_service_object_type   svc_obj;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_0("QMI AUTH service init");

  /*-------------------------------------------------------------------------
    Setting QMI AUTH service command handler with dcc task process
  -------------------------------------------------------------------------*/
  qmi_task_set_svc_cmd_handler(QMUX_SERVICE_AUTH,qmi_authi_process_cmd);

  /*-------------------------------------------------------------------------
    QMI Auth service configuration setup
  -------------------------------------------------------------------------*/
  qmi_authi_cfg.fw_cfg.base_version.major     = AUTHI_BASE_VER_MAJOR;
  qmi_authi_cfg.fw_cfg.base_version.minor     = AUTHI_BASE_VER_MINOR;

  qmi_authi_cfg.fw_cfg.addendum_version.major = AUTHI_ADDENDUM_VER_MAJOR;
  qmi_authi_cfg.fw_cfg.addendum_version.minor = AUTHI_ADDENDUM_VER_MINOR;

  qmi_authi_cfg.fw_cfg.cbs.alloc_clid         = qmi_authi_alloc_clid_cb;
  qmi_authi_cfg.fw_cfg.cbs.dealloc_clid       = qmi_authi_dealloc_clid_cb;
  qmi_authi_cfg.fw_cfg.cbs.init_cback         = qmi_authi_init_cb;
  qmi_authi_cfg.fw_cfg.cbs.cmd_hdlr           = qmi_authi_cmd_hdlr_cb;

  qmi_authi_cfg.cmd_hdlr_array               = qmi_authi_cmd_callbacks;
  qmi_authi_cfg.cmd_num_entries               = AUTHI_CMD_MAX;
  qmi_authi_cfg.sp                            = &qmi_auth_state;

  /*-------------------------------------------------------------------------
    Calling QMI Framework API to register the service with Framework
  -------------------------------------------------------------------------*/
  reg_result = qmi_framework_reg_service( QMUX_SERVICE_AUTH,
                                          &qmi_authi_cfg.fw_cfg );

  if (reg_result != QMI_FRAMEWORK_ERR_NONE)
  {
    return;
  }

  svc_obj = auth_get_service_object_v01();
  (void) qmi_si_register_object ( svc_obj,
                                  0, /* Service Instance */
                                  auth_get_service_impl_v01() );
  
  qmi_authi_cfg.registered  = TRUE;
} /* qmi_auth_init */

/*===========================================================================
  FUNCTION QMI_AUTHI_PROCESS_EAP_SIM_AKA_TASK_SWITCH()

  DESCRIPTION
    Process the task switch req

  PARAMETERS
    cmd_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_process_eap_sim_aka_task_switch
(
  qmi_authi_cmd_buf_type *cmd_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr->data.eap_sim_aka.req_cb(cmd_ptr->data.eap_sim_aka.user_data);
} /* qmi_authi_process_eap_sim_aka_task_switch */


/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_AUTH_EAP_TASK_SRV_FCT()

  DESCRIPTION
    Cback function registered with EAP SIM/AKA for task switch

  PARAMETERS
    req_cb - cback to be called after switching tasks
    user_data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_auth_eap_task_srv_fct
(
  eap_sim_aka_task_srv_req_cb_type req_cb,
  void * user_data
)
{
  qmi_authi_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr = (qmi_authi_cmd_buf_type *)qmi_authi_get_cmd_buf(
                      QMI_CMD_EAP_SIM_AKA_TASK_SWITCH_EV,__LINE__);
  if( cmd_ptr == NULL)
  {
    return FALSE;
  }

  memset(cmd_ptr, 0, sizeof(qmi_authi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_EAP_SIM_AKA_TASK_SWITCH_EV;
  cmd_ptr->data.eap_sim_aka.req_cb = req_cb;
  cmd_ptr->data.eap_sim_aka.user_data = user_data;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);

  return TRUE;
} /* qmi_auth_eap_task_srv_fct() */


/*===========================================================================
  FUNCTION QMI_EAP_TRP_TX_CBACK()

  DESCRIPTION
    Tx cback registered with EAP to get EAP response packets

  PARAMETERS
    user_data
    eap_resp_pkt

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_eap_trp_tx_cback
(
  void           * user_data, 
  dsm_item_type ** eap_resp_pkt
)
{
  qmi_authi_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(user_data);

  cmd_ptr = (qmi_authi_cmd_buf_type *)qmi_authi_get_cmd_buf(QMI_CMD_SEND_EAP_RESP,__LINE__);
  if( cmd_ptr == NULL)
  {
    return;
  }

  memset(cmd_ptr, 0, sizeof(qmi_authi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_SEND_EAP_RESP;
  cmd_ptr->data.eap_resp.pkt = *eap_resp_pkt;
  cmd_ptr->data.eap_resp.user_data = user_data;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
}


/*===========================================================================
  FUNCTION QMI_AUTH_SENDI_EAP_RESP()

  DESCRIPTION
    Process the eap resp cback and send EAP response

  PARAMETERS
    cmd_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_send_eap_resp
(
  qmi_authi_cmd_buf_type * cmd_ptr
)
{
  qmi_authi_client_state_type * auth_cl_sp;
  qmi_cmd_buf_type            * cmd_buf_p;
  dsm_item_type *              eap_resp_pkt;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  uint16   tlv_len;
  uint8  tlv_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  eap_resp_pkt = cmd_ptr->data.eap_resp.pkt;
  auth_cl_sp = (qmi_authi_client_state_type *) cmd_ptr->data.eap_resp.user_data;

  ASSERT(auth_cl_sp);
  cmd_buf_p = auth_cl_sp->eap_info.pending_req_cmd_buf;

  /* clear the cmd_buf since now sending response */
  auth_cl_sp->eap_info.pending_req_cmd_buf = NULL;

  if(cmd_buf_p == NULL)
  {
    LOG_MSG_ERROR_0("got eap_trp_tx_cback but no REQ pending!");
    return;
  }

  errval = QMI_ERR_NONE;
 
  tlv_len = (uint16) dsm_length_packet(eap_resp_pkt);
  tlv_type = QMI_TYPE_REQUIRED_PARAMETERS;

  if ( (QMI_SVC_PKT_PUSH(&eap_resp_pkt, &tlv_len, sizeof(uint16))) &&
       (QMI_SVC_PKT_PUSH(&eap_resp_pkt, &tlv_type, sizeof(uint8))) 
       == FALSE )
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&eap_resp_pkt);
  }

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  if(FALSE == qmi_svc_put_result_tlv(&eap_resp_pkt, result, errval))
  {
    dsm_free_packet(&eap_resp_pkt);
    qmi_authi_free_transaction_cmd_buf( &cmd_buf_p );
    return;
  }

  if(FALSE == qmi_authi_send_response(auth_cl_sp, cmd_buf_p, eap_resp_pkt) )
  {
    LOG_MSG_ERROR_0 ("Unable to send response for QMI EAP SEND PKT");
    dsm_free_packet(&eap_resp_pkt);
    qmi_authi_free_transaction_cmd_buf(&cmd_buf_p);
  }
} /* qmi_authi_send_eap_resp() */


/*===========================================================================
  FUNCTION QMI_EAP_RESULT_IND_CBACK()

  DESCRIPTION
    Process the task switch req

  PARAMETERS
    handle - eap handle
    user data
    result - result of EAP 
    pre_master_key - MSK
    pre_master_key_len - MSK len

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_eap_result_ind_cback
(
  eap_handle_type       handle, 
  void                 *result_ind_user_data,
  eap_result_enum_type  result,
  uint8                *pre_master_key,
  uint16                pre_master_key_len
)
{
  qmi_authi_eap_result_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(result_ind_user_data && (pre_master_key_len <= QMI_EAP_MAX_PRE_MASTER_KEY_SIZE));

  /* check that result_ind_user_data is valid in case ASSERT is removed */
  if(result_ind_user_data == NULL)
  {
    LOG_MSG_ERROR_0("Result IND user data NULL");
    return;
  }
  
  /* check that pre_master_key_len is valid in case ASSERT is removed */
  if(pre_master_key_len > QMI_EAP_MAX_PRE_MASTER_KEY_SIZE)
  {
    LOG_MSG_ERROR_1("Pre master key len invalid (%d)", pre_master_key_len);
    return;
  }

  cmd_ptr = (qmi_authi_eap_result_cmd_buf_type *)qmi_authi_get_cmd_buf(
  	            QMI_CMD_SEND_EAP_RESULT,__LINE__);
  if( cmd_ptr == NULL)
  {
    return;
  }

  memset(cmd_ptr, 0, sizeof(qmi_authi_eap_result_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_SEND_EAP_RESULT;
  cmd_ptr->handle = handle;
  cmd_ptr->result = result;
  cmd_ptr->pmk_len = pre_master_key_len;
  cmd_ptr->user_data = result_ind_user_data;

  memscpy(cmd_ptr->pmk, pre_master_key_len, pre_master_key, pre_master_key_len);

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
}


/*===========================================================================
  FUNCTION QMI_AUTH_EAP_NOTIFICATION_CBACK()

  DESCRIPTION
     Notification code callback function from EAP module

  PARAMETERS
     User data - pointer of notification code

  RETURN VALUE
     None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_auth_eap_notification_cback
(
  void* user_data
)
{
  qmi_authi_cmd_buf_type *cmd_ptr;
  eap_sim_aka_notification_s_type * eap_notify_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(user_data);

  eap_notify_data = (eap_sim_aka_notification_s_type*)user_data;
  cmd_ptr = (qmi_authi_cmd_buf_type *)qmi_authi_get_cmd_buf(
  	                QMI_CMD_EAP_NOTIFICATION_CODE,__LINE__);
  if( cmd_ptr == NULL)
  {
    return;
  }

  memset(cmd_ptr, 0, sizeof(qmi_authi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_EAP_NOTIFICATION_CODE;
  cmd_ptr->data.eap_notification_cb.notification_code = (uint16)eap_notify_data->notification_code;
  cmd_ptr->data.eap_notification_cb.eap_handle        = eap_notify_data->eap_handle;
  /*Post command to DCC task*/
  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
}

/*===========================================================================
  FUNCTION QMI_AUTHI_SEND_EAP_RESULT_IND()

  DESCRIPTION
    Process the eap resp cback and send EAP response

  PARAMETERS
    cmd_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_send_eap_result_ind
(
  qmi_authi_eap_result_cmd_buf_type *  cmd_ptr
)
{
  dsm_item_type               *  ind;
  qmi_authi_client_state_type * auth_cl_sp;
  qmi_framework_msg_hdr_type    framework_msg_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(cmd_ptr != NULL);

  auth_cl_sp = (qmi_authi_client_state_type *) cmd_ptr->user_data;

  if(auth_cl_sp->eap_info.eap_handle != cmd_ptr->handle)
  {
    LOG_MSG_ERROR_2("Eap Handle in result_ind_cb(%d) does not match client's handle(%d)",
                    cmd_ptr->handle, auth_cl_sp->eap_info.eap_handle);
    return;
  } 

  ind = NULL;
  auth_cl_sp->eap_info.result = cmd_ptr->result;

  ASSERT(cmd_ptr->pmk_len <= QMI_EAP_MAX_PRE_MASTER_KEY_SIZE);

  /* check that pre_master_key_len is valid in case ASSERT is removed */
  if(cmd_ptr->pmk_len > QMI_EAP_MAX_PRE_MASTER_KEY_SIZE)
  {
    LOG_MSG_ERROR_1("PMK len invalid (%d)", cmd_ptr->pmk_len);
    return;
  }
  
  auth_cl_sp->eap_info.msk_len = cmd_ptr->pmk_len;

  memscpy(auth_cl_sp->eap_info.msk, 
          cmd_ptr->pmk_len,
         cmd_ptr->pmk, 
         cmd_ptr->pmk_len);

  auth_cl_sp->eap_info.eap_result_ready = TRUE;

  if( FALSE == qmi_svc_put_param_tlv(&ind, 
                                     QMI_TYPE_REQUIRED_PARAMETERS,
                                     sizeof (uint8),
                                     &(auth_cl_sp->eap_info.result)) )
  {
    LOG_MSG_INFO2_0("Unable to generate EAP Result indication!");
    dsm_free_packet(&ind);
    return;
  }

  /*-----------------------------------------------------------------------
    Fill the QMI Framework message header before sending Indication to
    Framework.
  -----------------------------------------------------------------------*/
  framework_msg_hdr.common_hdr.client_id = auth_cl_sp->common.clid;
  /* if unicast indication, qmi framework map the instance and no need to fill*/
  framework_msg_hdr.common_hdr.qmi_instance = AUTHI_INVALID_INSTANCE;
  framework_msg_hdr.common_hdr.service = QMUX_SERVICE_AUTH;
  /* for indication the transaction ID can be ignored */
  framework_msg_hdr.common_hdr.transaction_id = 0;
  framework_msg_hdr.msg_ctl_flag = QMI_FLAG_MSGTYPE_IND;
  framework_msg_hdr.msg_len = (uint16) dsm_length_packet(ind);

  if( FALSE == qmi_authi_send_indication( &framework_msg_hdr, 
                                          AUTHI_CMD_VAL_EAP_SESSION_RESULT_IND, 
                                          ind))
  {
    LOG_MSG_ERROR_0("Unable to send qmi_authi_send_eap_result_ind!");
    dsm_free_packet(&ind);
  }
} /* qmi_authi_send_eap_result_ind */


/*===========================================================================
  FUNCTION QMI_AUTHI_GENERATE_EAP_NOTIFICATION_IND()

  DESCRIPTION
    Process the notification call back and send indication

  PARAMETERS
    cmd_ptr : data passed from command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_generate_eap_notification_code_ind
(
   qmi_authi_cmd_buf_type *cmd_ptr
)
{
  qmi_authi_client_state_type * cl_sp = NULL;
  uint16 notification_code = 0;
  eap_handle_type eap_handle;
  int index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   ASSERT(cmd_ptr);

   notification_code = cmd_ptr->data.eap_notification_cb.notification_code;
   eap_handle        = (uint16)cmd_ptr->data.eap_notification_cb.eap_handle;
   
   LOG_MSG_INFO2_1("Send the eap_notification_code: %d to registered clients",notification_code);
   
    /*loop through all clients and send ind to clients registered*/
    for( index = 0; index < AUTHI_MAX_CLIDS; index++ )
    {
      cl_sp = (qmi_authi_client_state_type *) qmi_auth_state.client[index];
      if( cl_sp && (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) && 
         (cl_sp->eap_info.eap_handle == eap_handle) &&
          cl_sp->report_status.report_eap_notification_code )
      {
        qmi_authi_eap_notification_code_ind(cl_sp->common.clid,
                                            notification_code);
        break;
      }
    }
}

/*===========================================================================
  FUNCTION QMI_AUTHI_EAP_NOTIFICATION_CODE_IND()

  DESCRIPTION
    Send eap notification code indication

  PARAMETERS
     clid - ID of client subscribed for indication
     notification_code from eap server

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_eap_notification_code_ind
(
   uint8 clid,
   uint16 notification_code
)
{
  dsm_item_type *  ind = NULL;
  qmi_framework_common_msg_hdr_type    common_hdr;
  
  PACKED struct PACKED_POST
  {
    uint16 eap_notification_code;
  } v_out1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    memset(&v_out1,0,sizeof(v_out1));
    v_out1.eap_notification_code  = (uint16)notification_code;
    if( FALSE == qmi_svc_put_param_tlv(&ind, 
                                       QMI_TYPE_REQUIRED_PARAMETERS,
                                       sizeof(v_out1),
                                       &v_out1) )
    {
      LOG_MSG_INFO2_0("Unable to generate eap_notification_code indication!");
      dsm_free_packet(&ind);
      return;
    }

  /*-----------------------------------------------------------------------
  Fill the QMI message header before sending Indication to Framework.
  -----------------------------------------------------------------------*/
  common_hdr.client_id = clid;
  /* if unicast indication, qmi framework map the instance and no need to fill*/
  common_hdr.qmi_instance = AUTHI_INVALID_INSTANCE;
  common_hdr.service = QMUX_SERVICE_AUTH;
  /* for indication the transaction ID can be ignored */
  common_hdr.transaction_id = 0;

 if( FALSE == ds_qmi_fw_send_ind( &common_hdr,
                                  AUTHI_CMD_VAL_EAP_NOTIFICATION_CODE_IND,
                                  ind ) )
 {
   LOG_MSG_ERROR_0("Unable to send eap_notification_code indication!");
   dsm_free_packet(&ind);
 }
}

/*===========================================================================
  FUNCTION QMI_AUTHI_RESET()

  DESCRIPTION
    Reset the issuing AUTH client's state
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_authi_reset
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
  qmi_authi_reset_client(cl_sp);

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

} /* qmi_authi_reset() */

/*===========================================================================
  FUNCTION QMI_AUTHI_START_EAP_SESSION()

  DESCRIPTION
    Start the EAP session
  
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_authi_start_eap_session
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *               response;
  qmi_authi_client_state_type * auth_cl_sp;
  eap_info_type                eap_req_params;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean                      retval;
  boolean  algo_tlv_set = FALSE;
  uint32   eap_sim_aka_algo = EAP_AKA_ALGO_NONE;
  uint32                       eap_method_mask;
  uint8    type;
  uint16   len;
  void *   value;
  uint16   expected_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  memset( &eap_req_params, 0, sizeof(eap_req_params) );
  /* Initialise the global variables */
  memset( &(qmi_authi_global.sim_aka_meta_info.eap_meta_identity), 0,
            sizeof(qmi_authi_global.sim_aka_meta_info.eap_meta_identity));
  memset( &(qmi_authi_global.sim_aka_meta_info.id), 0,
            sizeof(qmi_authi_global.sim_aka_meta_info.id));
  memset( &(qmi_authi_global.sim_aka_meta_info.sim_aka_algo_type), 0,
            sizeof(qmi_authi_global.sim_aka_meta_info.sim_aka_algo_type));
  qmi_authi_global.sim_aka_meta_info.new_client = FALSE;

  auth_cl_sp = (qmi_authi_client_state_type *) cl_sp;
  /* default allow SIM or AKA or AKA_PRIME(AKA') */
  eap_method_mask = AUTHI_EAP_METHOD_SIM | AUTHI_EAP_METHOD_AKA | AUTHI_EAP_METHOD_AKA_PRIME;

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
      case AUTHI_PRM_TYPE_EAP_METHOD_MASK:
        expected_len = sizeof(eap_method_mask);
        value = (void *) &eap_method_mask;
        break;

      /* expected_len is still 0 so that we will go past the check */
      case AUTHI_PRM_TYPE_EAP_USER_ID:
        if(len <= (sizeof(eap_req_params.user_id.name)+sizeof(uint8)))
        {
          value = (void *) &eap_req_params.user_id.len;

          /* copy first byte to length field.The structure here is as below
             =========================================
              type | Length | user_id length| user_id
             ========================================= */
          if(PS_UTILS_DSM_PULLUP( sdu_in, value, sizeof(uint8)) != sizeof(uint8))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          if((eap_req_params.user_id.len != (len -= sizeof(uint8))) || 
            (eap_req_params.user_id.len == 0))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            /* TODO need to failover here */
            goto send_result;
          }  
          value = (void *) &eap_req_params.user_id.name;
        }
        else
        {
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        break;

      case AUTHI_PRM_TYPE_EAP_META_IDENTITY:
        if(len <= (sizeof(qmi_authi_global.sim_aka_meta_info.eap_meta_identity.name) + sizeof(uint8)))
        {
          value = (void *) &qmi_authi_global.sim_aka_meta_info.eap_meta_identity.len;
          /* copy first byte to length field.The structure here is as below
                 ========================================
                 type | Length | meta_id length| meta_id
                 ======================================== */
          if(PS_UTILS_DSM_PULLUP( sdu_in, value, sizeof(uint8)) != sizeof(uint8))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          if((qmi_authi_global.sim_aka_meta_info.eap_meta_identity.len != (len -= sizeof(uint8))) ||
            (qmi_authi_global.sim_aka_meta_info.eap_meta_identity.len == 0))
          {
            errval = QMI_ERR_MALFORMED_MSG;
            /* TODO need to failover here */
            goto send_result;
          }  
          value = (void *) &qmi_authi_global.sim_aka_meta_info.eap_meta_identity.name;
        }
        else
        {
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        break;

      case AUTHI_PRM_TYPE_EAP_SIM_AKA_ALGO:
        expected_len = sizeof(eap_sim_aka_algo);
        value = (void *) &eap_sim_aka_algo;
        algo_tlv_set = TRUE;
        break;

     default:
        LOG_MSG_INFO2_1 ("Unrecognized TLV type (%d)", type);
        break;
    }
    if(!qmi_svc_validate_type_length( type,
                                          len,
                                          expected_len,
                                          FILE_ID_DS_QMI_AUTH,__LINE__))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and dsm_pullup will 
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }


  if(!((eap_method_mask > AUTHI_EAP_METHOD_NONE)
      && (eap_method_mask <= (AUTHI_EAP_METHOD_SIM | AUTHI_EAP_METHOD_AKA | AUTHI_EAP_METHOD_AKA_PRIME))))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
  if((algo_tlv_set == TRUE) && !(eap_sim_aka_algo < EAP_SIM_AKA_ALGO_MAX))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }    

  if(auth_cl_sp->eap_info.eap_handle != EAP_INVALID_HANDLE)
  {
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  /* populate the eap_info_type struct used to create eap instance */
  if(algo_tlv_set == TRUE)
  {
    /* if eap_method_mask is set  to AUTHI_EAP_METHOD_SIM
       then acceptable eap_sim_aka_algo types are EAP_SIM_ALGO_GSM or EAP_SIM_ALGO_USIM_GSM
       or EAP_SIM_ALGO_NONE */
    if((eap_method_mask & AUTHI_EAP_METHOD_SIM) && !((eap_method_mask & AUTHI_EAP_METHOD_AKA) ||
                                                     (eap_method_mask & AUTHI_EAP_METHOD_AKA_PRIME)))
    {
      if((eap_sim_aka_algo == EAP_SIM_ALGO_GSM) ||
         (eap_sim_aka_algo == EAP_SIM_ALGO_USIM_GSM) ||
         (eap_sim_aka_algo == EAP_AKA_ALGO_NONE))
      {
        eap_req_params.eap_auth_prot_mask |= (0x1 << EAP_SIM_METHOD);
      }
      else
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }
    }

    /* if eap_method_mask is set  to AUTHI_EAP_METHOD_AKA
       then acceptable eap_sim_aka_algo types are EAP_AKA_ALGO_SHA1 or EAP_AKA_ALGO_MILENAGE
       or EAP_AKA_ALGO_CAVE or EAP_SIM_ALGO_NONE */
    else if((eap_method_mask & AUTHI_EAP_METHOD_AKA) && !((eap_method_mask & AUTHI_EAP_METHOD_SIM) ||
                                                         (eap_method_mask & AUTHI_EAP_METHOD_AKA_PRIME)))
    {
      if((eap_sim_aka_algo == EAP_AKA_ALGO_SHA1) ||
        (eap_sim_aka_algo == EAP_AKA_ALGO_MILENAGE) ||
        (eap_sim_aka_algo == EAP_AKA_ALGO_CAVE)  ||
        (eap_sim_aka_algo == EAP_AKA_ALGO_NONE))
      {
        eap_req_params.eap_auth_prot_mask |= (0x1 << EAP_AKA_METHOD);
      }
      else
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }
    }
    else if((eap_method_mask & AUTHI_EAP_METHOD_AKA_PRIME) && 
          !((eap_method_mask & AUTHI_EAP_METHOD_SIM) || (eap_method_mask & AUTHI_EAP_METHOD_AKA)))
    {
      if((eap_sim_aka_algo == EAP_AKA_ALGO_MILENAGE) ||
         (eap_sim_aka_algo == EAP_AKA_ALGO_NONE))
      {
        eap_req_params.eap_auth_prot_mask |= (0x1 << EAP_AKA_PRIME_METHOD);
      }
      else
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }
    }
    /* if eap_method_mask is not set then eap_auth_prot_mask is set as per eap_sim_aka_algo type */
    else if(eap_method_mask & (AUTHI_EAP_METHOD_SIM | AUTHI_EAP_METHOD_AKA | 
                                                 AUTHI_EAP_METHOD_AKA_PRIME))
    {
      if((eap_sim_aka_algo == EAP_SIM_ALGO_GSM) ||
        (eap_sim_aka_algo == EAP_SIM_ALGO_USIM_GSM))
      {
        eap_req_params.eap_auth_prot_mask |= (0x1 << EAP_SIM_METHOD);
      }
      else if((eap_sim_aka_algo == EAP_AKA_ALGO_SHA1) ||
              (eap_sim_aka_algo == EAP_AKA_ALGO_CAVE))
      {
        eap_req_params.eap_auth_prot_mask |= (0x1 << EAP_AKA_METHOD);
      }
      else if(eap_sim_aka_algo == EAP_AKA_ALGO_MILENAGE)
      {
        eap_req_params.eap_auth_prot_mask |= ((0x1 << EAP_AKA_METHOD) | 
                                              (0x1 << EAP_AKA_PRIME_METHOD));
      }
      /* since we have already checked the validity of eap_sim_aka_algo this will obviously fall 
         into EAP_AKA_ALGO_NONE case */
      else
      {
        eap_req_params.eap_auth_prot_mask |= (0x1 << EAP_SIM_METHOD | 0x1 << EAP_AKA_METHOD |
                                              0x1 << EAP_AKA_PRIME_METHOD);
      }
    }
  }
  else
  {
    if(eap_method_mask & AUTHI_EAP_METHOD_SIM)
      eap_req_params.eap_auth_prot_mask |= (0x1 << EAP_SIM_METHOD);
    if(eap_method_mask & AUTHI_EAP_METHOD_AKA) 
      eap_req_params.eap_auth_prot_mask |= (0x1 << EAP_AKA_METHOD);
    if(eap_method_mask & AUTHI_EAP_METHOD_AKA_PRIME)
      eap_req_params.eap_auth_prot_mask |= (0x1 << EAP_AKA_PRIME_METHOD);
  }

  eap_req_params.authenticator_rsp_required = FALSE;
  eap_req_params.raw_mode = FALSE;
  eap_req_params.support_eap_ext = FALSE;
  eap_req_params.result_ind_f_ptr = qmi_eap_result_ind_cback;
  eap_req_params.result_ind_user_data = cl_sp;
  eap_req_params.trp_tx_f_ptr = qmi_eap_trp_tx_cback;
  eap_req_params.trp_tx_user_data = cl_sp;

  /* init SIM and AKA methods. */
  eap_sim_init();
  eap_aka_init();
  /* init AKA PRIME method */
  if( eap_req_params.eap_auth_prot_mask & (0x1 << EAP_AKA_PRIME_METHOD) )
  {
    eap_aka_prime_init();
  }

  auth_cl_sp->eap_info.eap_handle = eap_create_instance(&eap_req_params);

  if(auth_cl_sp->eap_info.eap_handle == EAP_INVALID_HANDLE)
  {
    LOG_MSG_ERROR_1("EAP create instance failed for client %d",
                    auth_cl_sp->common.clid);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  /* populate the eap_sim_aka_meta_info_type struct used to set meta_info 
     for SIM AKA and meta_info */
  if(eap_req_params.user_id.len > 0)
  {
    qmi_authi_global.sim_aka_meta_info.id.provided_id_len = (uint8)eap_req_params.user_id.len;
    memscpy(qmi_authi_global.sim_aka_meta_info.id.provided_id, 
            eap_req_params.user_id.len,
            eap_req_params.user_id.name,
           eap_req_params.user_id.len);

    /* Since user_id TLV is optional we need this to set this to identify if it is 
       being dealt as a client which is supposed to be using user_id  */
    qmi_authi_global.sim_aka_meta_info.new_client = TRUE;
  }

  /*Save the Algo received in global*/
  /*EAP_AKA_ALGO_NONE will be saved/passed if no user preference*/
  if(algo_tlv_set == TRUE)
  {
    qmi_authi_global.sim_aka_meta_info.sim_aka_algo_type = 
                          (eap_sim_aka_algo_enum_type)eap_sim_aka_algo;
  }

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
/*Device is in multsim mode, get the correct session type mapped to subs_id*/
  if( auth_cl_sp->subscription_id >= AUTHI_BIND_SUBS_MAX)
  {
     errval = QMI_ERR_INTERNAL;
     goto send_result;
  }

  qmi_authi_global.sim_aka_meta_info.eap_session_is_set = TRUE;
  qmi_authi_global.sim_aka_meta_info.eap_session_type = 
           eap_client_config_get_session_type
            ( (uint8)auth_cl_sp->subscription_id,
              eap_sim_aka_algo ); 

  LOG_MSG_INFO1_3("subscription type %d algo_type %d & session set to %d", 
                  auth_cl_sp->subscription_id,
                  eap_sim_aka_algo,
                  qmi_authi_global.sim_aka_meta_info.eap_session_type );
#endif

  qmi_authi_global.sim_aka_meta_info.notification_rcv_cb_fct = qmi_auth_eap_notification_cback;
  /* set the meta_info for SIM AKA */
  eap_set_meta_info(auth_cl_sp->eap_info.eap_handle, (void *) &(qmi_authi_global.sim_aka_meta_info));
send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response,
                                  result,
                                  errval); 
  CHECK_RETVAL();

  return response;

} /* qmi_authi_start_eap_session() */



/*===========================================================================
  FUNCTION QMI_AUTHI_SEND_EAP_PACKET()

  DESCRIPTION
    Gives the name of the device manufacturer.
    
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
static dsm_item_type * qmi_authi_send_eap_packet
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *    response;
  qmi_authi_client_state_type  * auth_cl_sp;
  qmi_transaction_type *       x_p;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  uint8    type;
  uint16   len;
  boolean  got_v_in_reqd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);
  
  response = NULL;
  errval = QMI_ERR_NONE;
  auth_cl_sp = (qmi_authi_client_state_type *) cl_sp;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        got_v_in_reqd = TRUE;
        break;

     default:
        LOG_MSG_INFO2_1 ("Unrecognized TLV type (%d)", type);

        /* No other TLVs expected in this req */
        errval = QMI_ERR_MALFORMED_MSG;
        goto send_result;
    }

    if(got_v_in_reqd == TRUE)
    {
      /* since don't want to parse more or else the value protion is pulled out */
      break;
    }

    /* don't pull up, since need to send dsm_item to eap */
  }

  if(auth_cl_sp->eap_info.eap_handle == EAP_INVALID_HANDLE)
  {
    dsm_free_packet(sdu_in);
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  eap_input(auth_cl_sp->eap_info.eap_handle, sdu_in);

  if(auth_cl_sp->eap_info.pending_req_cmd_buf != NULL)
  {
    LOG_MSG_ERROR_0("Got next EAP req packet, before response to the previous one was sent");
    x_p = auth_cl_sp->eap_info.pending_req_cmd_buf->x_p;
    qmi_authi_free_transaction(&x_p);
    
  }
  auth_cl_sp->eap_info.pending_req_cmd_buf = cmd_buf_p;

  return QMI_SVC_RESPONSE_PENDING;

send_result: 
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  
  return response;
  
} /* qmi_authi_send_eap_packet() */


/*===========================================================================
  FUNCTION QMI_AUTHI_GET_EAP_SESSION_KEYS()

  DESCRIPTION
    To get the session keys.
        
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
static dsm_item_type * qmi_authi_get_eap_session_keys
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *    response;
  qmi_authi_client_state_type * auth_cl_sp;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  auth_cl_sp = (qmi_authi_client_state_type *) cl_sp;

  if(auth_cl_sp->eap_info.result != EAP_RESULT_SUCCESS)
  {
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  if( FALSE == qmi_svc_put_param_tlv(&response, 
                                     QMI_TYPE_REQUIRED_PARAMETERS,
                                     auth_cl_sp->eap_info.msk_len,
                                     auth_cl_sp->eap_info.msk) )
  {
    LOG_MSG_INFO2_0("Unable to generate EAP Result indication!");
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
  }
 
send_result:  
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  
  return response;
} /* qmi_authi_get_eap_session_keys() */



/*===========================================================================
  FUNCTION QMI_AUTHI_END_EAP_SESSION

  DESCRIPTION
    To end the EAP session.
        
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
static dsm_item_type * qmi_authi_end_eap_session
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *    response;
  qmi_authi_client_state_type  *  auth_cl_sp;
  qmi_transaction_type *  x_p;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  auth_cl_sp = (qmi_authi_client_state_type *) cl_sp;

  if(auth_cl_sp->eap_info.eap_handle == EAP_INVALID_HANDLE)
  {
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  eap_delete_instance(auth_cl_sp->eap_info.eap_handle);

  /* Free pending req if any */
  if(auth_cl_sp->eap_info.pending_req_cmd_buf != NULL)
  {
    x_p = auth_cl_sp->eap_info.pending_req_cmd_buf->x_p;
    qmi_authi_free_transaction(&x_p);
  }

  /* Clear Eap info */
  memset(&(auth_cl_sp->eap_info), 0, sizeof(auth_cl_sp->eap_info));
  auth_cl_sp->eap_info.eap_handle = EAP_INVALID_HANDLE;
  auth_cl_sp->eap_info.result = EAP_RESULT_FAILURE;


send_result:  
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  
  return response;
} /* qmi_authi_end_eap_session() */

/*===========================================================================
  FUNCTION QMI_AUTH_AKA_RESULT_IND_CBACK()

  DESCRIPTION
    Process the AKA callback and send the command to QMI taks to send the
    indication to the client.

  PARAMETERS
    aka_handle   - aka handle
    status       - result of the aka algorithm
    digest       - Digest data
    digest_len   - the length of the digest data
    aka_data     - the AKA response data
    aka_data_len - the length of the AKA response data
    user_data    - the user_data passed into the start_aka_algo fcn

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_auth_aka_result_ind_cback
(
  ps_utils_aka_handle_type       aka_handle,
  ps_utils_aka_status_enum_type  status,
  uint8                         *digest,
  uint8                          digest_len,
  uint8                         *aka_data,
  uint8                          aka_data_len,
  void                          *user_data
)
{
  qmi_authi_aka_result_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(user_data);

  cmd_ptr = (qmi_authi_aka_result_cmd_buf_type *)qmi_authi_get_cmd_buf(
  	          QMI_CMD_SEND_AKA_ALGO_RESULT,__LINE__);
  if( cmd_ptr == NULL)
  {
    return;
  }

  memset(cmd_ptr, 0, sizeof(qmi_authi_aka_result_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_SEND_AKA_ALGO_RESULT;
  cmd_ptr->handle        = aka_handle;
  cmd_ptr->status        = status;
  cmd_ptr->digest_len    = digest_len;
  cmd_ptr->aka_data_len  = aka_data_len;
  cmd_ptr->user_data     = user_data;  

  memscpy(cmd_ptr->digest, digest_len, digest, digest_len);
  memscpy(cmd_ptr->aka_data, aka_data_len, aka_data, aka_data_len);

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
} /* qmi_auth_aka_result_ind_cback() */

/*===========================================================================
  FUNCTION QMI_AUTHI_RUN_AKA_ALGO()

  DESCRIPTION
    Gives the name of the device manufacturer.
    
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
static dsm_item_type * qmi_authi_run_aka_algo
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  void *             value;
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  ps_utils_aka_ver_enum_type    aka_ver = 0;
  uint8    aka_params[2*sizeof(uint8)+
                      QMI_AUTH_MAX_AKA_RAND_LEN_SIZE +
	              QMI_AUTH_MAX_AKA_AUTN_LEN_SIZE] = {0};
  uint8    autn_len_offset;
  uint8    autn_offset;
  ps_utils_aka_handle_type aka_handle;
  uint8    type;
  uint16   len = 0, aka_params_len = 0;
  boolean  got_v_in_reqd = FALSE;
  boolean  got_v_in_aka_params = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);
  
  errval = QMI_ERR_NONE;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
	value = (void *) &aka_ver;
        if(sizeof(aka_ver) != len)
        {
          LOG_MSG_INFO2_2("Invalid TLV len (%d) for type (%d)", len, type);
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        got_v_in_reqd = TRUE;
        break;

      case AUTHI_PRM_TYPE_AKA_PARAMS:
        if(len > sizeof(aka_params))
        {
          LOG_MSG_INFO2_2("TLV len (%d) too long for type (%d)", len, type);
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        value = (void *) aka_params;
        aka_params_len = len;
        got_v_in_aka_params = TRUE;
        break;

     default:
        LOG_MSG_INFO2_1 ("Unrecognized TLV type (%d)", type);

        /* No other TLVs expected in this req */
        errval = QMI_ERR_MALFORMED_MSG;
        goto send_result;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and dsm_pullup will 
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  } /* end while */


  if(got_v_in_reqd != TRUE)
  {
    /* Missing the required TLV, fail */
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if(aka_ver >= PS_UTILS_AKA_MAX_VER)
  {
    LOG_MSG_INFO2_1 ("Invalid AKA version: %d in TLV", aka_ver);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result; 
  }

  if(aka_ver == PS_UTILS_AKA_V1 || aka_ver == PS_UTILS_AKA_V2)
  {
    if (got_v_in_aka_params != TRUE)
    {
      /* This optional TLV must be present for AKA_V1 and AKA_V2 */
      LOG_MSG_INFO2_0 ("Missing AKA Params TLV");
      errval = QMI_ERR_MISSING_ARG;
      goto send_result;
    }
    /* Determine offsets for parameters in TLV */
    /*------------------------------------------------
     * | rand_len |   rand   | autn_len |   autn   |
     * |  uint8   | variable |  uint8   | variable |
     *-----------------------------------------------*/
    autn_len_offset =   sizeof(uint8) + aka_params[0];
    autn_offset     = 2*sizeof(uint8) + aka_params[0];    
 
    /* Validate some length values */
    if (aka_params_len != (2 * sizeof(uint8) + aka_params[0] + aka_params[autn_len_offset]))
    {
      LOG_MSG_INFO2_3 ("TLV Length %d does not match rand_len %d and autn_len %d",
                       aka_params_len,
                       aka_params[0],
                       aka_params[autn_len_offset]);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
 
    /* Call AKA processing function */
    if(-1 == (aka_handle = ps_utils_run_aka_algo
                           (
                             aka_ver,
                             &aka_params[1], /* rand     */
                             aka_params[0],  /* rand_len */
                             &aka_params[autn_offset],
                             aka_params[autn_len_offset],
                             (ps_utils_aka_callback_type) qmi_auth_aka_result_ind_cback,
                             cl_sp
                           )))
    {
      LOG_MSG_INFO2_0 ("Unknown QMI AKA processing error!");
      errval = QMI_ERR_INTERNAL;
      goto send_result;
    }

    /* Return AKA_V1/V2 handle to client */
    if( FALSE == qmi_svc_put_param_tlv(&response, 
                                       QMI_TYPE_REQUIRED_PARAMETERS,
                                       (uint16) sizeof(aka_handle),
                                       (void*)  &aka_handle))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

send_result: 
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  
  return response;
  
} /* qmi_authi_run_aka_algo() */

/*===========================================================================
  FUNCTION QMI_AUTHI_SEND_AKA_RESULT_IND()

  DESCRIPTION
    Send the AKA result to the client.

  PARAMETERS
    cmd_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_send_aka_result_ind
(
  qmi_authi_aka_result_cmd_buf_type *  cmd_ptr
)
{
  dsm_item_type  *  ind = NULL;
  uint8             tlv_type;
  uint16            tlv_len;
  uint16            opt_tlv_len;
  qmi_authi_client_state_type * auth_cl_sp = NULL;
  qmi_framework_msg_hdr_type    framework_msg_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cmd_ptr);
  auth_cl_sp = (qmi_authi_client_state_type *)cmd_ptr->user_data;

  /* Only populate the optional TLV if the status is success */
  if(cmd_ptr->status == PS_UTILS_AKA_SUCCESS)
  {
    /* Push down the optional TLV for AKAv1/v2 */
    opt_tlv_len = sizeof(cmd_ptr->digest_len) +
                  sizeof(cmd_ptr->aka_data_len) +
                  cmd_ptr->aka_data_len +
                  cmd_ptr->digest_len;

    tlv_type = AUTHI_PRM_TYPE_AKA_RESULT_DATA;

    if ( (QMI_SVC_PKT_PUSH(&ind, cmd_ptr->aka_data, 
                           cmd_ptr->aka_data_len)) &&
         (QMI_SVC_PKT_PUSH(&ind, &cmd_ptr->aka_data_len, sizeof(uint8))) &&
         (QMI_SVC_PKT_PUSH(&ind, cmd_ptr->digest, 
                           cmd_ptr->digest_len)) &&
         (QMI_SVC_PKT_PUSH(&ind, &cmd_ptr->digest_len, sizeof(uint8))) &&
         (QMI_SVC_PKT_PUSH(&ind, &opt_tlv_len, sizeof(uint16))) &&
         (QMI_SVC_PKT_PUSH(&ind, &tlv_type, sizeof(uint8))) 
         == FALSE )
    {
      LOG_MSG_INFO2_0("Unable to generate AKA Result indication data!");
      dsm_free_packet(&ind);
      return;
    }
  } /* opt TLV */

  /* Push down the two required params: aka_handle (4 bytes) and status (1 byte) */
  tlv_len = (uint16) (sizeof(uint8)+sizeof(uint32));
  tlv_type = QMI_TYPE_REQUIRED_PARAMETERS;

  if ( (QMI_SVC_PKT_PUSH(&ind, &cmd_ptr->status, sizeof(uint8))) &&
       (QMI_SVC_PKT_PUSH(&ind, &cmd_ptr->handle, sizeof(uint32))) &&
       (QMI_SVC_PKT_PUSH(&ind, &tlv_len, sizeof(uint16))) &&
       (QMI_SVC_PKT_PUSH(&ind, &tlv_type, sizeof(uint8))) 
       == FALSE )
  {
    LOG_MSG_INFO2_0("Unable to generate AKA Result indication req params!");
    dsm_free_packet(&ind);
    return;
  }

  /*-----------------------------------------------------------------------
    Fill the QMI Framework message header before sending Indication to
    Framework.
  -----------------------------------------------------------------------*/
  framework_msg_hdr.common_hdr.client_id = auth_cl_sp->common.clid;
  /* if unicast indication, qmi framework map the instance and no need to fill*/
  framework_msg_hdr.common_hdr.qmi_instance = AUTHI_INVALID_INSTANCE;
  framework_msg_hdr.common_hdr.service = QMUX_SERVICE_AUTH;
  /* for indication the transaction ID can be ignored */
  framework_msg_hdr.common_hdr.transaction_id = 0;
  framework_msg_hdr.msg_ctl_flag = QMI_FLAG_MSGTYPE_IND;
  framework_msg_hdr.msg_len = (uint16) dsm_length_packet(ind);
  if(FALSE == qmi_authi_send_indication( &framework_msg_hdr, 
                                         AUTHI_CMD_VAL_AKA_ALGO_RESULT_IND, 
                                         ind))
  {
    LOG_MSG_ERROR_0("Unable to send qmi_authi_send_aka_result_ind!");
    dsm_free_packet(&ind);
  }
} /* qmi_authi_send_aka_result_ind */

/*===========================================================================
  FUNCTION QMI_AUTHI_SET_SUBSCRIPTION_BINDING()

  DESCRIPTION
    Binds user given subscription to this client

  PARAMETERS
    sp           : service state pointer
    cl_sp        : client state pointer
    cmd_buf_p    : the current command buffer
    sdu_in       : dsm_item containing incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_authi_set_subscription_binding
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  uint8    type;
  uint16   len;
  uint16   expected_len;
  uint32   subs_id;
  void *   value;
  qmi_authi_client_state_type *  auth_client_sp;
  boolean  got_v_in_reqd = FALSE;
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);
  
  response = NULL;
  errval = QMI_ERR_NONE;

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  auth_client_sp = (qmi_authi_client_state_type *)cl_sp;

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
        expected_len = sizeof(subs_id);
        value = &subs_id;
        got_v_in_reqd = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1( "Unrecognized TLV type (%d)",type);
        break;
    }
    if(!qmi_svc_validate_type_length( type,
                                          len,
                                          expected_len,
                                          FILE_ID_DS_QMI_AUTH,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and dsm_pullup will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
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

  if(!(subs_id < AUTHI_BIND_SUBS_MAX))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
  
  #ifndef FEATURE_TRIPLE_SIM
    if(subs_id == AUTHI_BIND_SUBS_TERTIARY)
    {
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
      goto send_result;
    }
  #endif
  auth_client_sp->subscription_id = (qmi_authi_client_subs_e_type)subs_id ;

#else
  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
  goto send_result;
#endif  
    
  send_result:
    result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                     : QMI_RESULT_FAILURE);

    retval = qmi_svc_put_result_tlv(&response, result, errval);
    CHECK_RETVAL();

    return response;

} /* qmi_authi_bind_subscription */

/*===========================================================================
  FUNCTION QMI_AUTHI_GET_BIND_SUBSCRIPTION()

  DESCRIPTION
    Returns already bound  subscription to this client

  PARAMETERS
    sp           : service state pointer
    cl_sp        : client state pointer
    cmd_buf_p    : the current command buffer
    sdu_in       : dsm_item containing incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_authi_get_bind_subscription
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval; 

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  qmi_authi_client_state_type *  auth_client_sp;
  uint32 subscription_id;
#endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  auth_client_sp = (qmi_authi_client_state_type *)cl_sp;

  if(auth_client_sp == NULL)
  {
    MSG_ERROR("client state pointer is NULL",0,0,0);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  if( auth_client_sp->subscription_id >= AUTHI_BIND_SUBS_MAX )
  {
    MSG_ERROR("Invalid subscription id stored for client %d",auth_client_sp->common.clid,0,0);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  subscription_id = auth_client_sp->subscription_id;
  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  if(FALSE == qmi_svc_put_param_tlv(&response, 
                                    AUTHI_PRM_TYPE_BIND_SUBSCRIPTION,
                                    sizeof(subscription_id),
                                    &subscription_id))
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
  }
#else
  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
  goto send_result;
#endif
  
  send_result:
   result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                    : QMI_RESULT_FAILURE);
   retval = qmi_svc_put_result_tlv(&response, result, errval);
   CHECK_RETVAL();
       
   return response;

}/* qmi_authi_get_bind_subscription */


/*===========================================================================
  FUNCTION QMI_AUTHI_INDICATION_REGISTER()

  DESCRIPTION
    To set state for indicaiton registeration

  PARAMETERS
    sp           : service state pointer
    cmd_buf_p    : the current command buffer
    cl_sp        : client state pointer
    sdu_in       : dsm_item containing incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_authi_indication_register
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
    dsm_item_type *    response = NULL;
    qmi_error_e_type   errval   = QMI_ERR_NONE;
    qmi_result_e_type  result;
    boolean 	       retval; 
    qmi_authi_client_state_type * auth_client_sp = NULL;
    
    PACKED struct PACKED_POST
    {
      boolean report_eap_notification_code;
    } v_in1;
    uint8    type;
    uint16   len;
    uint16   expected_len;
    void *   value;
    boolean  got_v_in1 = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

    auth_client_sp = (qmi_authi_client_state_type *)cl_sp;
    while (*sdu_in)
    {
       if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
       {
         continue;
       }

       expected_len = 0;
       value = NULL;
       switch (type)
       {
         case AUTHI_PRM_TYPE_EAP_NOTIFICATION_CODE:
            expected_len = sizeof(v_in1);
            value = &v_in1;
            got_v_in1 = TRUE;
          break;

          default:
             LOG_MSG_INFO2_1( "Unrecognized TLV type (%d)",type);
           break;
       }
	
       if(!qmi_svc_validate_type_length( type,
                                         len,
                                         expected_len,
                                         FILE_ID_DS_QMI_AUTH,__LINE__) )

       {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
       }

      /*-----------------------------------------------------------------------
        If type is unrecognized, value will be NULL, and dsm_pullup will
        free the unrecognized value, i.e. we skip past unrecognized TLVs
        -----------------------------------------------------------------------*/
       if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
       {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
       }
    }

    /*-------------------------------------------------------------------------
     If ALL optional TLVs are absent return error or else carry on and
      process the request.
    -------------------------------------------------------------------------*/	 
    if(!got_v_in1)
    {
      errval = QMI_ERR_MISSING_ARG;
      goto send_result;
    }

    /*Process the report eap notification code value*/
    if(v_in1.report_eap_notification_code == TRUE)
    {
      auth_client_sp->report_status.report_eap_notification_code = TRUE;
    }
    else if(v_in1.report_eap_notification_code == FALSE)
    {
      auth_client_sp->report_status.report_eap_notification_code = FALSE;
    }
    else
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;		  
    }

send_result:
    result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
	                                 : QMI_RESULT_FAILURE);

    retval = qmi_svc_put_result_tlv(&response, result, errval);
    CHECK_RETVAL();

    return response;
}

/*===========================================================================
FUNCTION QMI_AUTHI_INIT_CB()

  DESCRIPTION
  Function gets called from framework whenever the QMI AUTH service registers with
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
static void qmi_authi_init_cb
(
  uint16 num_instances
)
{
  qmi_authi_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr = (qmi_authi_cmd_buf_type *)qmi_authi_get_cmd_buf(QMI_CMD_AUTH_INIT_CB,__LINE__);
  if( cmd_ptr == NULL)
  {
    return;
  }

  memset(cmd_ptr, 0, sizeof(qmi_authi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_AUTH_INIT_CB;
  cmd_ptr->data.init_cb.num_instances = num_instances;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);

} /* qmi_authi_init_cb() */

/*===========================================================================
FUNCTION QMI_AUTHI_ALLOC_CLID_CB()

DESCRIPTION
  Function gets called from framework whenever a clid is allocated by the
  framework for QMI AUTH service. This function posts a DCC command for the
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
static boolean qmi_authi_alloc_clid_cb
(  
  qmi_framework_common_msg_hdr_type*   common_msg_hdr  
)
  {
  qmi_authi_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(common_msg_hdr);

  LOG_MSG_INFO1_2("QMI AUTH alloc clid cb: clid:%d, QMI Instances:%d",
                  common_msg_hdr->client_id, common_msg_hdr->qmi_instance);

  cmd_ptr = (qmi_authi_cmd_buf_type *)qmi_authi_get_cmd_buf(QMI_CMD_AUTH_ALLOC_CLID_CB,__LINE__);
  if( cmd_ptr == NULL)
    {
    return FALSE;
    }

  /*-------------------------------------------------------------------------
    copy the framework common message header to local struct
  -------------------------------------------------------------------------*/
  memset(cmd_ptr, 0, sizeof(qmi_authi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_AUTH_ALLOC_CLID_CB;
  cmd_ptr->data.clid_cb.common_msg_hdr.service = common_msg_hdr->service;
  cmd_ptr->data.clid_cb.common_msg_hdr.client_id = common_msg_hdr->client_id;
  cmd_ptr->data.clid_cb.common_msg_hdr.transaction_id = common_msg_hdr->transaction_id;
  cmd_ptr->data.clid_cb.common_msg_hdr.qmi_instance = common_msg_hdr->qmi_instance;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
  return TRUE;
} /* qmi_authi_alloc_clid_cb */

/*===========================================================================
FUNCTION QMI_AUTHI_DEALLOC_CLID_CB()

DESCRIPTION 
  Function gets called from framework whenever a clid is deallocated by the
  framework for QMI AUTH service. This function posts a DCC command for the
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
static void qmi_authi_dealloc_clid_cb
(  
  qmi_framework_common_msg_hdr_type*   common_msg_hdr  
)
{
  qmi_authi_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(common_msg_hdr);
  LOG_MSG_INFO1_2("QMI AUTH dealloc clid cb: clid:%d, QMI Instances:%d",
                  common_msg_hdr->client_id, common_msg_hdr->qmi_instance);

  cmd_ptr = (qmi_authi_cmd_buf_type *)qmi_authi_get_cmd_buf(QMI_CMD_AUTH_DEALLOC_CLID_CB,__LINE__);
  if( cmd_ptr == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    copy the framework common message header to local struct
  -------------------------------------------------------------------------*/
  memset(cmd_ptr, 0, sizeof(qmi_authi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_AUTH_DEALLOC_CLID_CB;
  cmd_ptr->data.clid_cb.common_msg_hdr.service = common_msg_hdr->service;
  cmd_ptr->data.clid_cb.common_msg_hdr.client_id = common_msg_hdr->client_id;
  cmd_ptr->data.clid_cb.common_msg_hdr.transaction_id = common_msg_hdr->transaction_id;
  cmd_ptr->data.clid_cb.common_msg_hdr.qmi_instance = common_msg_hdr->qmi_instance;

  /*-------------------------------------------------------------------------
    Positing command to QMI Service task to handle the clid callback
  -------------------------------------------------------------------------*/
  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
} /* qmi_authi_dealloc_clid_cb */

/*===========================================================================
  FUNCTION FRAMEWORK_AUTHI_CMD_HDLR_CB()

  DESCRIPTION
    Function gets called from framework whenever a new QMI AUTH request
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
static void  qmi_authi_cmd_hdlr_cb
( 
  qmi_framework_msg_hdr_type * msg_hdr,
  dsm_item_type             ** sdu 
)
{
  qmi_authi_cmd_buf_type *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(msg_hdr);
  LOG_MSG_INFO1_3("qmi_authi_cmd_hdlr_cb: clid:%d, QMI Instances:%d, tx_id: %d",
                  msg_hdr->common_hdr.client_id,
                  msg_hdr->common_hdr.qmi_instance,
                  msg_hdr->common_hdr.transaction_id);
  
  cmd_ptr = (qmi_authi_cmd_buf_type *)qmi_authi_get_cmd_buf(QMI_CMD_AUTH_CMD_HDLR_CB,__LINE__);
  if( cmd_ptr == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    copy the framework message header to local struct
  -------------------------------------------------------------------------*/
  memset(cmd_ptr, 0, sizeof(qmi_authi_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_AUTH_CMD_HDLR_CB;
  cmd_ptr->data.cmd_hdlr_cb.msg_hdr.common_hdr.service = 
                                        msg_hdr->common_hdr.service;
  cmd_ptr->data.cmd_hdlr_cb.msg_hdr.common_hdr.client_id = 
                                        msg_hdr->common_hdr.client_id;
  cmd_ptr->data.cmd_hdlr_cb.msg_hdr.common_hdr.transaction_id = 
                                        msg_hdr->common_hdr.transaction_id;
  cmd_ptr->data.cmd_hdlr_cb.msg_hdr.common_hdr.qmi_instance = 
                                        msg_hdr->common_hdr.qmi_instance;
  cmd_ptr->data.cmd_hdlr_cb.msg_hdr.msg_ctl_flag = msg_hdr->msg_ctl_flag; 
  cmd_ptr->data.cmd_hdlr_cb.msg_hdr.msg_len = msg_hdr->msg_len; 
  cmd_ptr->data.cmd_hdlr_cb.sdu_in = *sdu;

  /*-------------------------------------------------------------------------
    Positing command to QMI Service task to handle the service request
  -------------------------------------------------------------------------*/
  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
} /* qmi_authi_cmd_hdlr_cb */

/*===========================================================================
FUNCTION QMI_AUTHI_ALLOC_CL_SP()

  DESCRIPTION
  This function gets called whenever a client state need to be allocated.
  The client state has allocated and deallocated dynamically at runtime.
    
  PARAMETERS
  clid: client ID

  RETURN VALUE
  qmi_authi_client_state_type * : ptr to QMI AUTH client state.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_authi_client_state_type * qmi_authi_alloc_cl_sp
(
  uint8 clid
)
{
  qmi_authi_client_state_type *  cl_sp = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1( "qmi_authi_alloc_cl_sp clid :%d", clid );

  if ((clid == QMI_SVC_CLID_UNUSED ) ||
      (clid >= QMI_FRAMEWORK_SVC_MAX_CLIENTS ))
  {
    LOG_MSG_INFO2_1( "Invalid client ID clid :%d", clid );
    return cl_sp;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(cl_sp, sizeof(qmi_authi_client_state_type),
                                  qmi_authi_client_state_type*);

  qmi_auth_state.client[clid - 1] = cl_sp;
  return cl_sp;
} /* qmi_authi_alloc_cl_sp */

/*===========================================================================
FUNCTION QMI_AUTHI_DEALLOC_CL_SP()

DESCRIPTION
  This function gets called whenever a client state need to be deallocated.
  The client state is allocated and deallocated dynamically at runtime.

PARAMETERS
    clid: client ID

RETURN VALUE
  TRUE: client Id deallocated otherwise error.
 
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean qmi_authi_dealloc_cl_sp
(
  uint8 clid
)
  {
  qmi_authi_client_state_type *  cl_sp = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1( "qmi_authi_dealloc_cl_sp clid :%d", clid );

  if ((clid == QMI_SVC_CLID_UNUSED ) ||
      (clid >= QMI_FRAMEWORK_SVC_MAX_CLIENTS ))
  {
    LOG_MSG_INFO2_1( "Invalid client ID clid :%d", clid );
    return FALSE;
  }

  cl_sp = (qmi_authi_client_state_type *) qmi_auth_state.client[clid - 1];
  if(cl_sp)
  {
    q_destroy(&cl_sp->common.x_free_q);
    q_destroy(&cl_sp->common.cmd_free_q);

    PS_SYSTEM_HEAP_MEM_FREE(cl_sp);
    qmi_auth_state.client[clid - 1] = NULL;
  }
  else
  {
    LOG_MSG_ERROR_0("Client state pointer is already NULL");
    return FALSE;
  }
  return TRUE;
} /* qmi_authi_dealloc_cl_sp */


/*===========================================================================
  FUNCTION QMI_AUTHI_INITIALIZE_CLIENT()

  DESCRIPTION
    Initialize the new client state for the given client state ptr.
    
  PARAMETERS
    cl_sp :  client state pointer
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_initialize_client
(
  qmi_authi_client_state_type * cl_sp
)
{
  qmi_authi_state_type *  auth_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cl_sp);
  auth_sp = &qmi_auth_state;
  memset( cl_sp , 0, sizeof(qmi_authi_client_state_type) );

  LOG_MSG_INFO2_0( "qmi_authi_initialize_client " );
  cl_sp->instance     = AUTHI_INVALID_INSTANCE;
  cl_sp->common.clid  = QMI_SVC_CLID_UNUSED;
  cl_sp->service_id   = QMUX_SERVICE_AUTH;

  ds_qmi_fw_common_cl_init(auth_sp, &cl_sp->common);

  cl_sp->eap_info.eap_handle = EAP_INVALID_HANDLE;
  cl_sp->eap_info.result = EAP_RESULT_FAILURE;

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  cl_sp->subscription_id = AUTHI_BIND_SUBS_PRIMARY;
#endif

} /* qmi_authi_initialize_client() */


/*===========================================================================
  FUNCTION QMI_AUTHI_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client
    
  PARAMETERS
    cl_sp :  client state pointer
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_reset_client
(
  void * cl_sp
)
{
  qmi_authi_client_state_type * auth_cl_sp;
  int i;
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  auth_cl_sp = (qmi_authi_client_state_type *) cl_sp;

  LOG_MSG_INFO2_0( "qmi_authi_reset_client " );

  if(auth_cl_sp->eap_info.eap_handle != EAP_INVALID_HANDLE)
  {
    LOG_MSG_INFO1_1("Delete EAP instance since client (%d) reset",
                    auth_cl_sp->common.clid);
    eap_delete_instance(auth_cl_sp->eap_info.eap_handle);
  }

  if(auth_cl_sp->eap_info.pending_req_cmd_buf != NULL)
  {
    x_p = auth_cl_sp->eap_info.pending_req_cmd_buf->x_p;
    qmi_authi_free_transaction(&x_p);
  }

  /* Clear Eap info */
  memset(&(auth_cl_sp->eap_info), 0, sizeof(auth_cl_sp->eap_info));
  auth_cl_sp->eap_info.eap_handle = EAP_INVALID_HANDLE;
  auth_cl_sp->eap_info.result = EAP_RESULT_FAILURE;

  /*-------------------------------------------------------------------------
    Free resources for client
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    q_init() is used here as a way to clear the queue and not as a way to 
    initialize the queue.
  -------------------------------------------------------------------------*/ 
  (void) q_init( &auth_cl_sp->common.x_free_q );
  for( i = 0; i < MAX_PENDING_TRANSACTIONS; i++ )
  {
    x_p = &auth_cl_sp->common.transactions[i];
    qmi_authi_free_transaction( &x_p );
  }
   
  /*-------------------------------------------------------------------------
    clear event reporting
  -------------------------------------------------------------------------*/
} /* qmi_authi_reset_client() */

/*===========================================================================
  FUNCTION QMI_ATI_PROCESS_CMD()

  DESCRIPTION
    This function processes a QMI AT command or event.

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI AT command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI AT must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_process_cmd
(
  void *cmd_ptr
)
{
  qmi_authi_cmd_buf_type *cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);
  cmd_buf_ptr = (qmi_authi_cmd_buf_type *)cmd_ptr;

  switch(cmd_buf_ptr->cmd_id)
  {
    case QMI_CMD_EAP_SIM_AKA_TASK_SWITCH_EV:
    {
      qmi_authi_process_eap_sim_aka_task_switch(cmd_buf_ptr);
      break;
    }

    case QMI_CMD_SEND_EAP_RESP:
    {
      qmi_authi_send_eap_resp(cmd_buf_ptr);
      break;
    }

    case QMI_CMD_AUTH_INIT_CB:
    {
      qmi_authi_process_svc_init(cmd_buf_ptr->data.init_cb.num_instances);
      break;
    }

    case QMI_CMD_AUTH_ALLOC_CLID_CB:
    {
      qmi_authi_process_alloc_clid(&cmd_buf_ptr->data.clid_cb.common_msg_hdr);
      break;
    }

    case QMI_CMD_AUTH_DEALLOC_CLID_CB:
    {
      qmi_authi_process_dealloc_clid(&cmd_buf_ptr->data.clid_cb.common_msg_hdr);
      break;
    }

    case QMI_CMD_AUTH_CMD_HDLR_CB:
    {
      qmi_authi_process_cmd_hdlr(&cmd_buf_ptr->data.cmd_hdlr_cb.msg_hdr,
                                    cmd_buf_ptr->data.cmd_hdlr_cb.sdu_in );
      break;
    }

    case QMI_CMD_SEND_EAP_RESULT:
    {
      qmi_authi_eap_result_cmd_buf_type *eap_cmd_buf_ptr =
        (qmi_authi_eap_result_cmd_buf_type *)cmd_ptr;
      qmi_authi_send_eap_result_ind(eap_cmd_buf_ptr);
      memset(eap_cmd_buf_ptr, 0, sizeof(qmi_authi_eap_result_cmd_buf_type));
      break;
    }
      
    case QMI_CMD_SEND_AKA_ALGO_RESULT:
    {
      qmi_authi_aka_result_cmd_buf_type *aka_cmd_buf_ptr =
        (qmi_authi_aka_result_cmd_buf_type *)cmd_ptr;
      qmi_authi_send_aka_result_ind(aka_cmd_buf_ptr);
      break;
    }
    case QMI_CMD_EAP_NOTIFICATION_CODE:
    { 
      qmi_authi_generate_eap_notification_code_ind(cmd_buf_ptr);
      break;
    }

    default:
      LOG_MSG_ERROR_1("Unexpected cmd(%d) recd in QMI AT cmd handler",
                      cmd_buf_ptr->cmd_id);
      ASSERT(0);
  }
  qmi_authi_free_cmd_buf(cmd_ptr);
  return;
} /* qmi_authi_process_cmd() */

/*===========================================================================
  FUNCTION QMI_AUTHI_GET_CMD_BUF()

  DESCRIPTION
    Allocate and assign a QMI AT command buffer from the PS MEM heap
    based on the QMI CMD type
    
  PARAMETERS
    cmd - QMI command type
    line_num - to indicate the line num of caller.
   
  RETURN VALUE
    cmd_buf_ptr - Pointer to the allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void *qmi_authi_get_cmd_buf
(
  qmi_cmd_id_e_type cmd_id,
  uint16 line_num
)
{
  LOG_MSG_INFO1_2("cmd_id = %d line_num = %d",cmd_id,line_num);
  switch (cmd_id)
  {
    case QMI_CMD_EAP_SIM_AKA_TASK_SWITCH_EV:
    case QMI_CMD_SEND_EAP_RESP:
    case QMI_CMD_AUTH_INIT_CB:
    case QMI_CMD_AUTH_ALLOC_CLID_CB:
    case QMI_CMD_AUTH_DEALLOC_CLID_CB:
    case QMI_CMD_AUTH_CMD_HDLR_CB:
    case QMI_CMD_EAP_NOTIFICATION_CODE:
    {
      qmi_authi_cmd_buf_type *cmd_buf_ptr;
      PS_SYSTEM_HEAP_MEM_ALLOC(cmd_buf_ptr, sizeof(qmi_authi_cmd_buf_type),
                               qmi_authi_cmd_buf_type*);
      return ((void *)cmd_buf_ptr);
    }

    case QMI_CMD_SEND_EAP_RESULT:
    {
      qmi_authi_eap_result_cmd_buf_type *eap_cmd_buf_ptr;
      PS_SYSTEM_HEAP_MEM_ALLOC(eap_cmd_buf_ptr, sizeof(qmi_authi_eap_result_cmd_buf_type),
                               qmi_authi_eap_result_cmd_buf_type*);
      return ((void *)eap_cmd_buf_ptr);
    }

    case QMI_CMD_SEND_AKA_ALGO_RESULT:
    {
      qmi_authi_aka_result_cmd_buf_type *aka_cmd_buf_ptr;
      PS_SYSTEM_HEAP_MEM_ALLOC(aka_cmd_buf_ptr, sizeof(qmi_authi_aka_result_cmd_buf_type),
                               qmi_authi_aka_result_cmd_buf_type*);
      return ((void *)aka_cmd_buf_ptr);
    }

    default:
      break;
  }
  return NULL;
} /* qmi_authi_get_cmd_buf */

/*===========================================================================
  FUNCTION QMI_AUTHI_PROCESS_SVC_INIT()

  DESCRIPTION
    This function processes a init callback request and intialize the service
    related infos.

  PARAMETERS
    num_instances:  nnumber of QMI Instances

  RETURN VALUE
    None

  DEPENDENCIES
    QMI AUTH must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_process_svc_init
(
  uint16 num_instances
)
{
  qmi_authi_state_type *         auth_sp;
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO2_0( "qmi_authi init callback " );
  auth_sp = &qmi_auth_state;
  memset( auth_sp, 0, sizeof(qmi_authi_state_type) );


  /*-------------------------------------------------------------------------
    Max QMI Instances
  -------------------------------------------------------------------------*/
  auth_sp->num_qmi_instances = num_instances;
  /*-------------------------------------------------------------------------
    initialize client state non-zero fields
  -------------------------------------------------------------------------*/
  if(qmi_authi_global.inited == FALSE)
  {
    memset(&(qmi_authi_global.sim_aka_meta_info), 0, sizeof(eap_sim_aka_meta_info_type));
    qmi_authi_global.sim_aka_meta_info.id.provided_id_len = 0;
    qmi_authi_global.sim_aka_meta_info.task_srv_fct = qmi_auth_eap_task_srv_fct;

    qmi_authi_global.inited = TRUE;
  }
} /* qmi_authi_process_svc_init */

/*===========================================================================
  FUNCTION QMI_AUTHI_PROCESS_ALLOC_CLID()

  DESCRIPTION
    This function process the client alloc callback request. This function
    allocates and initialize the new client state for the allocated client ID.

  PARAMETERS
    common_msg_hdr:  private data buffer containing the clid alloc request information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI AUTH must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_process_alloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_authi_client_state_type   *  cl_sp;  
  qmi_result_e_type                result = QMI_RESULT_FAILURE; 
  qmi_error_e_type                 errval = QMI_ERR_NONE;
/*-------------------------------------------------------------------------*/

  ASSERT(common_msg_hdr);
  cl_sp = NULL;

  LOG_MSG_INFO2_0( "qmi_authi process clid allocation" );
  cl_sp = qmi_authi_alloc_cl_sp(common_msg_hdr->client_id);

  if(cl_sp)
  {
    qmi_authi_initialize_client(cl_sp);
    cl_sp->common.clid = common_msg_hdr->client_id;
    cl_sp->instance    = (uint16)common_msg_hdr->qmi_instance;
    cl_sp->service_id  = common_msg_hdr->service ;

    LOG_MSG_INFO2_1( "qmi_authi clid set to local client state ptr: clid%d",
                     cl_sp->common.clid );
    result = QMI_RESULT_SUCCESS;
  }
  else
  {
    LOG_MSG_INFO1_0 ("No available service clids!" );
    errval = QMI_ERR_INTERNAL;
  }

  /* Notifying the clid allocation back to client via QMI Framework*/
  qmi_framework_svc_send_alloc_clid_result_ex(result, common_msg_hdr, errval);

} /* qmi_authi_process_alloc_clid */

/*===========================================================================
  FUNCTION QMI_AUTHI_PROCESS_DEALLOC_CLID()

  DESCRIPTION
    This function process the client dealloc callback request. This function
    reset and deallocates the new client state for the allocated client ID.

  PARAMETERS
    common_msg_hdr:  private data buffer containing the clid dealloc
    request information.
 
  RETURN VALUE
    None

  DEPENDENCIES
    QMI AUTH must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_process_dealloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_authi_client_state_type   *  cl_sp;   
  qmi_result_e_type                result = QMI_RESULT_FAILURE; 
  qmi_error_e_type                 errval = QMI_ERR_NONE;
/*-------------------------------------------------------------------------*/

  ASSERT(common_msg_hdr);

  cl_sp = NULL;
  LOG_MSG_INFO2_0( "qmi_authi process clid deallocation" );

  cl_sp = (qmi_authi_client_state_type *) 
          qmi_auth_state.client[common_msg_hdr->client_id  - 1];

  if (cl_sp != NULL && cl_sp->common.clid == common_msg_hdr->client_id)
  {
    qmi_authi_reset_client(cl_sp);
    qmi_authi_dealloc_cl_sp(common_msg_hdr->client_id);
    LOG_MSG_INFO2_1( "qmi_authi clid free to local client state ptr: clid%d",
                     common_msg_hdr->client_id );
    result = QMI_RESULT_SUCCESS;
  }
  else
  {
    errval = QMI_ERR_INVALID_CLIENT_ID;
    LOG_MSG_ERROR_1( "Can't find clid %d!", common_msg_hdr->client_id);
  }

  /* Notifying the clid deallocation back to client via QMI Framework*/
  qmi_framework_svc_send_dealloc_clid_result_ex(result, common_msg_hdr, errval);
} /* qmi_authi_process_dealloc_clid */

/*===========================================================================
  FUNCTION QMI_AUTHI_PROCESS_CMD_HDLR()

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
    QMI AUTH must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_process_cmd_hdlr
(
  qmi_framework_msg_hdr_type* msg_hdr,
  dsm_item_type * sdu_in 
)
{
  qmi_authi_state_type    * svc_sp;
  qmi_authi_client_state_type * cl_sp;
  int32                         temp;
  uint16                        cmd_type;
  uint16                        sdu_len;
  uint16                        remaining_bytes;
  dsm_item_type *               next_sdu;
  qmi_transaction_type *  x_p;
  qmi_cmd_buf_type *   cmd_buf_p;
  
/*-------------------------------------------------------------------------*/

  ASSERT(msg_hdr && sdu_in);
  LOG_MSG_INFO2_3( "Process QMI AUTH svc commoan handlr callback function: clid%d, tx_id:%d, ctl_flag:%d ",
                   msg_hdr->common_hdr.client_id,
                   msg_hdr->common_hdr.transaction_id,
                   msg_hdr->msg_ctl_flag );

  /*-------------------------------------------------------------------------
    Get a service state and client state pointer
  -------------------------------------------------------------------------*/
  svc_sp = (qmi_authi_state_type *)&qmi_auth_state;
  cl_sp = (qmi_authi_client_state_type *) 
             qmi_auth_state.client[msg_hdr->common_hdr.client_id - 1];
  
  ASSERT(cl_sp);

  /*-------------------------------------------------------------------------
    Get a transaction buffer to track all contained commands in the bundle
  -------------------------------------------------------------------------*/
  x_p = qmi_authi_get_transaction(svc_sp, cl_sp);
  if (x_p == NULL)
  {
    LOG_MSG_INFO1_1 ("No transaction container!  Discarding service %x message",
                     svc_sp);
    dsm_free_packet(&sdu_in);
    return; 
  }

  x_p->ctl  = msg_hdr->msg_ctl_flag;
  x_p->x_id = msg_hdr->common_hdr.transaction_id;

  /*-------------------------------------------------------------------------                  
    Parse the incoming service message
  -------------------------------------------------------------------------*/
  next_sdu = NULL;
  remaining_bytes = (uint16) dsm_length_packet(sdu_in);
  while (sdu_in)
  {
    /*-----------------------------------------------------------------------
      Extract service message header
    -----------------------------------------------------------------------*/
    temp = dsm_pull16( &sdu_in ); 
    if (temp == -1)
    {
      LOG_MSG_INFO1_1 ("Short QMI AUTH svc header!  Discarding transaction %d",
                       x_p->x_id);
      dsm_free_packet( &sdu_in );
      qmi_authi_free_transaction( &x_p );
      return;
    }
    cmd_type = ps_ntohs(temp);

    temp = dsm_pull16( &sdu_in );
    if (temp == -1)
    {
      LOG_MSG_INFO1_1 ("Short QMI AUTH svc header!  Discarding transaction %d",
                       x_p->x_id);
      dsm_free_packet( &sdu_in );
      qmi_authi_free_transaction( &x_p );
      return;
    }
    sdu_len = ps_ntohs(temp);
    remaining_bytes -= 4;

    if (sdu_len > remaining_bytes)
    {
      LOG_MSG_INFO1_1 ("Short QMI AUTH svc payload!  Discarding service %x message",
                       svc_sp);
      dsm_free_packet( &sdu_in );
      qmi_authi_free_transaction( &x_p );
      return;
    }
    else if (sdu_len < remaining_bytes)
    {
      /*---------------------------------------------------------------------
        save subsequent SDUs and isolate the current by trimming,
        unbundling individual commands into separate dsm chains
      ---------------------------------------------------------------------*/
      ASSERT (next_sdu == NULL);
      temp = PS_UTILS_DSM_DUP_PACKET( &next_sdu, sdu_in, sdu_len, 65535);
      if (0 == temp)
      {
        dsm_free_packet( &next_sdu ); // is this needed?
        dsm_free_packet( &sdu_in );
        qmi_authi_free_transaction( &x_p );
        return;
      }
      dsm_trim_packet( &sdu_in, sdu_len );
    }

    /*-----------------------------------------------------------------------
      sdu_in now contains a single command.
      Get a QMI command buffer to hold the command
    -----------------------------------------------------------------------*/
    ASSERT (x_p);
    cmd_buf_p = qmi_authi_get_transaction_cmd_buf( x_p );
    if (cmd_buf_p == NULL)
    {
      LOG_MSG_INFO1_1 ("No QMI command buffer!  Discarding service %x message",
                       svc_sp);
      dsm_free_packet(&sdu_in);
      qmi_authi_free_transaction( &x_p );
      return;
    }

    cmd_buf_p->cmd_type = cmd_type;
    x_p->req_list[x_p->n_cmds-1] = sdu_in;

    /*-----------------------------------------------------------------------
      move to next sdu in bundle, if present
    -----------------------------------------------------------------------*/
    sdu_in = next_sdu;
    next_sdu = NULL;
    remaining_bytes -= sdu_len;
  }
  LOG_MSG_INFO2_3 ("QMI Auth Dispatch the transaction(%x) with (%d cmds) ctl_flag= %x",
                   x_p->x_id, x_p->n_cmds, x_p->ctl);
  qmi_authi_dispatch_transaction(x_p);
} /* qmi_authi_process_cmd_hdlr */

/*===========================================================================
  FUNCTION QMI_AUTHI_DISPATCH_TRANSACTION()

  DESCRIPTION
    Dispatches all QMI AUTH commands in a passed transaction.
    This should be called after the comman handlr function has unbundled the request.

  PARAMETERS
    x_p : pointer to transaction containing commands to dispatch

  RETURN VALUE
    None

  DEPENDENCIES
    QMI Auth service must be initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_authi_dispatch_transaction
(
  qmi_transaction_type *  x_p
)
{
  qmi_cmd_buf_type *  cmd_buf_p;
  dsm_item_type *     msg;
  int                 i;
  boolean             free_t = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Dispatch each of the commands in the transaction
  -------------------------------------------------------------------------*/
  for( i = 0; i < x_p->n_cmds; i++ )
  {
    cmd_buf_p = x_p->cmd_list[i];
    msg = x_p->req_list[i];
    x_p->req_list[i] = NULL;

    /*-----------------------------------------------------------------------
      Dispatch the current SDU/msg transaction
    -----------------------------------------------------------------------*/
    if( FALSE == qmi_authi_input( cmd_buf_p, &msg ) )
    {
      LOG_MSG_INFO2_3 ("Transaction %x Command #%d (%x) processing failed",
                       x_p, i, cmd_buf_p);
      free_t = TRUE;
     //  we should ensure the transaction doesn't stall here.
    }

    /*-----------------------------------------------------------------------
      Ensure the input SDU is freed.  Command handlers don't need to worry
      about freeing the input message.
    -----------------------------------------------------------------------*/
    dsm_free_packet ( &msg );
  }
  if(TRUE == free_t)
  {
    // this indicates that qmi_authi_input() failed for one of the bundled 
    // commands in the transaction. This failure could be either problem
    // with assembling response or invalid command type was sent to the
    // service. In either case right thing to do is to cleanup transaction
    // at this point, otherwise it will never be cleaned up.
    qmi_authi_free_transaction( &x_p );
  }
} /* qmi_authi_dispatch_transaction() */

/*===========================================================================
  FUNCTION QMI_AUTHI_INPUT()

  DESCRIPTION
    Process the incoming QMI message and send the response back.
    This function is registered with QMUX.

  PARAMETERS
    cmd_buf_p_in : command buffer pointer
    sdu_in    : message to be processed

  RETURN VALUE
    TRUE:   message processed successfully
    FALSE:  invalid message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_authi_input
(
  qmi_cmd_buf_type *             cmd_buf_p,
  dsm_item_type **               sdu_in
)
{
  qmi_authi_state_type *    svc_sp;
  qmi_authi_client_state_type * cl_sp;
  dsm_item_type *          response_ptr = NULL;
  qmi_svc_cmd_hdlr_type *  cmd_hdlr;
  uint16                   cmd;
  boolean                  retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cmd_buf_p && sdu_in);

  LOG_MSG_INFO3_2( "Got QMI Service cmd type %d (%d bytes)",
                   cmd_buf_p->cmd_type, dsm_length_packet(*sdu_in));

  svc_sp = (qmi_authi_state_type *)&qmi_auth_state;
  cl_sp =  (qmi_authi_client_state_type *)cmd_buf_p->x_p->cl_sp;

  ASSERT (svc_sp && cl_sp);

  /*-------------------------------------------------------------------------
    Checking to see if a cmd hndler is registerd for the input command
  -------------------------------------------------------------------------*/
  cmd_hdlr = qmi_authi_cfg.cmd_hdlr_array;
  for (cmd = 0; cmd < qmi_authi_cfg.cmd_num_entries; cmd++, cmd_hdlr++)
  {
    if (cmd_buf_p->cmd_type == cmd_hdlr->cmd_type)
    {
      break; 
    }
  }

  if( cmd == qmi_authi_cfg.cmd_num_entries )
  {
    LOG_MSG_INFO2_2( "Unrecognized type (=%d) for service (=%d)! Return error",
                     cmd_buf_p->cmd_type, cl_sp->service_id );

    retval = qmi_svc_put_result_tlv( &response_ptr,
                                     QMI_RESULT_FAILURE,
                                     QMI_ERR_INVALID_QMI_CMD );
    if (FALSE == retval)
    {
      dsm_free_packet(&response_ptr);
      response_ptr = NULL;
    }
  }
  else
  {
    cmd_buf_p->in_progress = TRUE;
    if((cmd_buf_p->x_p->ctl & QMI_FLAG_MASK_MSGTYPE) == QMI_FLAG_MSGTYPE_CMD)
    {
      MSG_SPRINTF_1( MSG_SSID_DS, MSG_LEGACY_MED, "Handling %s", cmd_hdlr->msg_desc);
      response_ptr = cmd_hdlr->request_hdlr( svc_sp, cmd_buf_p, cl_sp, sdu_in );
    }
    else
    {
      LOG_MSG_INFO2_2 ("Unhandled (%d cmd) msg type (%d)",
                       cmd_buf_p->x_p->ctl&QMI_FLAG_MASK_MSGTYPE,
                       cmd_buf_p->cmd_type);
      response_ptr = NULL;
    }
  }

  cmd_buf_p->in_progress = FALSE;
  /*-------------------------------------------------------------------------
    send response if ready.  Check for need to queue this command in 
    pending queue.
  -------------------------------------------------------------------------*/
  if (response_ptr == NULL)
  {
    LOG_MSG_INFO2_0 ("(Service generated no response)");
    qmi_authi_free_transaction_cmd_buf(&cmd_buf_p);
    return FALSE;
  }
  else if (response_ptr == QMI_SVC_RESPONSE_PENDING)
  {
    LOG_MSG_INFO2_1 ("Response pending for cmd_buf_p %p", cmd_buf_p);
    // command buffer will be freed later when response is completed
    return TRUE;
  }
  return qmi_authi_send_response( cl_sp, cmd_buf_p, response_ptr );
} /* qmi_authi_input() */

/*===========================================================================
FUNCTION QMIAUTHI_SEND_RESPONSE()

DESCRIPTION
  This function calls QMI Framework API to send a QMI AUTH Service response to a
  clients.

PARAMETERS 
  cl_sp    : client state pointer
  cmd_buf_p: command buffer
  msg_ptr : dsm item

RETURN VALUE
  TRUE: If responses send to Framework
  FALSE: otherwise

DEPENDENCIES
  QMI Auth service must be initialized and registered with Framework

SIDE EFFECTS
  None
===========================================================================*/
static boolean qmi_authi_send_response
(
  qmi_authi_client_state_type *    cl_sp,
  qmi_cmd_buf_type *               cmd_buf_p,
  dsm_item_type *                  msg_ptr
)
{
  qmi_transaction_type * x_p;
  int                          i;
  qmi_framework_msg_hdr_type   msg_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT (cl_sp && cmd_buf_p && msg_ptr);

  if(FALSE == qmi_svc_prepend_msg_hdr(&msg_ptr, cmd_buf_p->cmd_type)) 
  {
    LOG_MSG_INFO2_1 ("Unable to send Response for cmd %d",
                     cmd_buf_p->cmd_type);
    /*-----------------------------------------------------------------------
      Note - no need to free msg_ptr, taken care of in 
             qmi_authi_prepend_msg_hdr()
    -----------------------------------------------------------------------*/
    qmi_authi_free_transaction_cmd_buf(&cmd_buf_p);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    find this outstanding transaction and save the response.
    If all responses are present, generate the response bundle and send
  -------------------------------------------------------------------------*/
  x_p = cmd_buf_p->x_p;
  ASSERT (x_p);
  
  /*-------------------------------------------------------------------------
    Queue the response (ok if freed above - need to queue regardless for 
    accounting purposes)
  -------------------------------------------------------------------------*/
  if( FALSE == qmi_authi_queue_response( &cmd_buf_p, &msg_ptr ) )
  {
    DATA_ERR_FATAL("Unable to push pending response to send queue");
    ASSERT (msg_ptr == NULL); // msg_ptr is freed in queue_response
    return FALSE;
  }

  ASSERT ((cmd_buf_p == NULL) && (msg_ptr == NULL));

  /*-------------------------------------------------------------------------
    If other responses are pending, return now.  Transmission of the bundled
    response will be defered until all responses are "sent" by command hdlrs
  -------------------------------------------------------------------------*/
  for( i = 0; i < x_p->n_cmds; i++ )
  {
    if( QMI_SVC_RESPONSE_PENDING == x_p->resp_list[i] )
    {
      LOG_MSG_INFO2_0 ("qmi_authi_response pending");
      return TRUE;
    }
  }

  /*-------------------------------------------------------------------------
    If all responses are present, generate the bundle by appending all 
    responses and send the whole out
    dequeue off bundle_resp_q and verify that cmd buffers are correct
  -------------------------------------------------------------------------*/
  for( i = 0; i < x_p->n_cmds; i++ )
  {
    ASSERT( x_p->resp_list[i] != QMI_SVC_RESPONSE_PENDING );
    if( x_p->resp_list[i] != QMI_SVC_RESPONSE_NONE )
    {
        dsm_append( &msg_ptr, &(x_p->resp_list[i]) );
    }
  }

  if (msg_ptr)
  {

    msg_hdr.common_hdr.client_id      = cl_sp->common.clid; 
    msg_hdr.common_hdr.qmi_instance   = cl_sp->instance;  
    msg_hdr.common_hdr.service        = (qmux_service_e_type)cl_sp->service_id;  
    msg_hdr.common_hdr.transaction_id = x_p->x_id;

    msg_hdr.msg_ctl_flag = QMI_FLAG_MSGTYPE_RESP;
    if( x_p->n_cmds > 1 )
    {
      msg_hdr.msg_ctl_flag |= QMI_FLAG_MASK_COMPOUND;
    }
    msg_hdr.msg_len  = (uint16) dsm_length_packet(msg_ptr);
    qmi_framework_svc_send_response( &msg_hdr, msg_ptr );
  }

  /*-------------------------------------------------------------------------
    free the transaction buffer
  -------------------------------------------------------------------------*/
  qmi_authi_free_transaction( &x_p );  
  return TRUE;

} /* qmi_authi_send_response */

/*===========================================================================
  FUNCTION QMI_AUTHIQUEUE_RESPONSE()

  DESCRIPTION
    Schedule the response to be sent back to the client The bundled response
    will be sent when all responses have been queued.

  PARAMETERS
    cmd_buf_p :  The command buffer for the request for which the response
                 is being queued.
    msg       :  A DSM chain containing the response

  RETURN VALUE
    TRUE  -  Success
    FALSE -  Failure

  DEPENDENCIES
    none

  SIDE EFFECTS
    The command buffer pointed to by cmd_buf_p will be freed.
    The dsm chain referenced by *msg is handled herein.  Caller should 
    never reference the pointer again after this call.
===========================================================================*/
static boolean qmi_authi_queue_response
( 
  qmi_cmd_buf_type **  cmd_buf_p,
  dsm_item_type **     msg
)
{
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cmd_buf_p && *cmd_buf_p && ((*cmd_buf_p)->x_p) && msg);

  /*-------------------------------------------------------------------------
    find this outstanding transaction and save the response.
    If all responses are present, generate the response bundle and send
  -------------------------------------------------------------------------*/
  x_p = (*cmd_buf_p)->x_p;
  
  /*-------------------------------------------------------------------------
    check if cmd buffer was one of the cmds in the transaction.
  -------------------------------------------------------------------------*/
  if( (*cmd_buf_p) != x_p->cmd_list[(*cmd_buf_p)->cmd_no] )
  {
    /*-----------------------------------------------------------------------
      If cmd was not part of the associated transaction, bad news!
    -----------------------------------------------------------------------*/
    DATA_ERR_FATAL("Command is linked to transaction it doesn't belong to! discarding");
    dsm_free_packet( msg );
    qmi_authi_free_transaction_cmd_buf( cmd_buf_p );
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If so, save the response dsm pointer in the transaction
    If response dsm pointer is null, the free command buffer call below
    will set the pointer to 1 to indicate that processing is complete but
    no response was generated.
  -------------------------------------------------------------------------*/
  x_p->cmd_list[(*cmd_buf_p)->cmd_no]  = NULL;
  x_p->resp_list[(*cmd_buf_p)->cmd_no] = *msg;
  *msg = NULL;

  /*-------------------------------------------------------------------------
    The command buffer is no longer needed so free it up for reuse
  -------------------------------------------------------------------------*/
  qmi_authi_free_transaction_cmd_buf( cmd_buf_p );
  return TRUE;

} /* qmi_authi_queue_response() */

/*===========================================================================
  FUNCTION QMI_AUTHI_SEND_INDICATION()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the AUTH service
    indication to client.

  PARAMETERS
    msg_hdr_p : QMI Framework message header
    cmd_type : type of indication
    ind : dsm item
 
  RETURN VALUE
    TRUE if ind sent to Framework sucessfully
    FALSE otherwise
 
  DEPENDENCIES
    QMI Auth service must be initialized and registered with Framework
 
  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_authi_send_indication
(
  void *           msg_hdr_p,
  uint16           cmd_type,
  dsm_item_type *  ind
)
{
  qmi_framework_msg_hdr_type   * framework_msg_hdr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(msg_hdr_p && ind);  
      
  framework_msg_hdr = (qmi_framework_msg_hdr_type*) msg_hdr_p;

  LOG_MSG_INFO2_3( "Sending QMI AUTH Indication to QMI Framework: clid:%d, tx_id:%d, ind_id:%d",
                   framework_msg_hdr->common_hdr.client_id,
                   framework_msg_hdr->common_hdr.transaction_id,
                   cmd_type );

  if(FALSE == qmi_svc_prepend_msg_hdr(&ind, cmd_type)) 
  {
    return FALSE;
  }

  if(TRUE != qmi_framework_svc_send_ind(framework_msg_hdr, ind))  
  {
    LOG_MSG_ERROR_0( "Unable to send QMI AUTH indication to QMI Framework" );
    return FALSE;
  }
  return TRUE;
} /* qmi_authi_send_indication */

/*===========================================================================
  FUNCTION QMI_AUTHI_GET_TRANSACTION_CMD_BUF()

  DESCRIPTION
    Allocate a QMI AUTH service command buffer

  PARAMETERS
    x_p : QMI Transaction that the command buf will be added to

  RETURN VALUE
    pointer to allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cmd_buf_type *  qmi_authi_get_transaction_cmd_buf
(
  qmi_transaction_type *  x_p
)
{
  qmi_cmd_buf_type *     cmd_buf_p;
  q_type *               q_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (x_p);
  if (x_p->n_cmds >= MAX_MSGS_IN_BUNDLE)
  {
    LOG_MSG_INFO1_0 ("Too many commands in transaction");
    return NULL;
  }

  if (x_p->cl_sp)  // cl_sp != NULL means this transaction is client specific
  {
    q_ptr = &x_p->cl_sp->cmd_free_q;
    cmd_buf_p = q_get(q_ptr);
    if (NULL == cmd_buf_p)
    {
      LOG_MSG_INFO1_0 ("No command buffers available!");
      return NULL;
    }
  }
  else
  {
      LOG_MSG_INFO1_0 ("Invalid state pointer !");
      return NULL;
  }

  cmd_buf_p->x_p    = x_p;
  cmd_buf_p->cmd_no = x_p->n_cmds;
  cmd_buf_p->in_progress = FALSE;
  x_p->cmd_list[x_p->n_cmds] = cmd_buf_p;
  x_p->resp_list[x_p->n_cmds] = QMI_SVC_RESPONSE_PENDING;
  x_p->n_cmds++;
  return cmd_buf_p;

} /* qmi_authi_get_transaction_cmd_buf() */

/*===========================================================================
  FUNCTION QMI_AUTHI_FREE_TRANSACTION_CMD_BUF()

  DESCRIPTION
    Frees a previously allocated QMI AUTH transaction command buffer

  PARAMETERS
    cmd_buf_handle : Handle to command buf pointer which points to the cmd
                     buffer to be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI Auth service must be initialized and registered with Framework
 
  SIDE EFFECTS
    Removes reference to this command buf from the associated transaction
    structure.  
===========================================================================*/
static void qmi_authi_free_transaction_cmd_buf
(
  qmi_cmd_buf_type **  cmd_buf_handle
)
{
  qmi_authi_client_state_type *  cl_sp;
  qmi_cmd_buf_type *            freeing_cmd_buf_p;
  q_type *                      q_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cmd_buf_handle);
  /* get the pointer locally so after we modify the x_p below we don't 
  dereference the null x_p->cmd_list[0] */
  freeing_cmd_buf_p = *cmd_buf_handle; 
  *cmd_buf_handle = NULL; // free the callers reference immediately

  LOG_MSG_INFO3_1 ("Freeing QMI command buffer %x", freeing_cmd_buf_p);

  if (freeing_cmd_buf_p)
  {
    ASSERT (freeing_cmd_buf_p->x_p);
    cl_sp = (qmi_authi_client_state_type *)freeing_cmd_buf_p->x_p->cl_sp;

    /* remove reference to this command buffer from transaction */
    ASSERT(freeing_cmd_buf_p->cmd_no < MAX_MSGS_IN_BUNDLE);
    LOG_MSG_INFO3_3 ("About to clear reference to cmd_buf_p (%x) from x_p(%x:%d)",
                     freeing_cmd_buf_p,
                     freeing_cmd_buf_p->x_p,
                     freeing_cmd_buf_p->cmd_no );

    /* clear the reference to this cmd buffer in transaction to handle
       the case where the response hasn't yet been queued (this would
       also clear the reference) */
    freeing_cmd_buf_p->x_p->cmd_list[freeing_cmd_buf_p->cmd_no] = NULL;

    /* if cmd buffer is freed before response is generated, need to update
       the containing transaction accounting to ensure response is not 
       withheld on account of this freed response */
       
    LOG_MSG_INFO3_3 ("Check if need to clear pending response (%x) from x_p(%x:%d)",
                     freeing_cmd_buf_p,
                     freeing_cmd_buf_p->x_p,
                     freeing_cmd_buf_p->cmd_no );

    if( freeing_cmd_buf_p->x_p->resp_list[freeing_cmd_buf_p->cmd_no] 
        == QMI_SVC_RESPONSE_PENDING )
    {
      /* indicate that no response is forthcoming for this command */
      freeing_cmd_buf_p->x_p->resp_list[freeing_cmd_buf_p->cmd_no] = 
        (dsm_item_type *) QMI_SVC_RESPONSE_NONE;
    }

    memset( freeing_cmd_buf_p, 0, sizeof (qmi_cmd_buf_type) );
    (void) q_link( freeing_cmd_buf_p, &(freeing_cmd_buf_p->link) );
    if (cl_sp)
    {
      q_ptr = &cl_sp->common.cmd_free_q;
      q_put( q_ptr, &(freeing_cmd_buf_p->link) );
    }
    else
    {
      LOG_MSG_INFO2_0("Freeing cmd_buf for global transaction");
    }
  }
  else
  {
    LOG_MSG_INFO2_0 ("Attempt to free NULL command buffer!");
  }
} /* qmi_authi_free_transaction_cmd_buf() */

/*===========================================================================
  FUNCTION QMI_AUTHI_GET_TRANSACTION()

  DESCRIPTION
    Get a transaction buffer for the QMI Auth service 
    by the client state pointer provided

  PARAMETERS
    svc_sp :  pointer to service state from which the transaction is allocated
    cl_sp :  client state pointer for client to get transaction buffer for
             or NULL if sending a broadcast indication

  RETURN VALUE
    NULL if no transaction buffer is available
    otherwise, pointer to transaction buffer

  DEPENDENCIES
    QMI Auth service must be initialized and registered with Framework
 
  SIDE EFFECTS
    service global transaction might be allocated.
===========================================================================*/
static qmi_transaction_type *  qmi_authi_get_transaction
(
  qmi_authi_state_type *  svc_sp,
  qmi_authi_client_state_type *   cl_sp
)
{
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(svc_sp);
  x_p = NULL;

  if (cl_sp)
  {
    x_p = q_get( &cl_sp->common.x_free_q );
  }
  else /* if no client sp, must be a broadcast indication */
  {
      LOG_MSG_ERROR_0("Invalid client state pointer!");
      return NULL;
    }

  if (x_p)
  {
    x_p->cl_sp = (qmi_common_client_state_type *)cl_sp;
    x_p->svc_sp =  svc_sp;
  }
  return x_p;
} /* qmi_authi_get_transaction() */

/*===========================================================================
  FUNCTION QMI_AUTHI_FREE_TRANSACTION()

  DESCRIPTION
    Frees the passed transaction buffer

  PARAMETERS
    x_p :  handle to transaction pointer pointing to transaction buffer to
           be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI Auth service must be initialized and registered with Framework

  SIDE EFFECTS
    Nulls out the caller's transaction buffer pointer
===========================================================================*/
static void qmi_authi_free_transaction
(
  qmi_transaction_type **  x_p
)
{
  qmi_authi_state_type *         svc_sp;
  qmi_authi_client_state_type *  cl_sp;
  int i;
  boolean free_x;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (x_p);
  if (*x_p == NULL)
  {
    LOG_MSG_INFO1_0 ("Attempt to free a NULL transaction pointer!");
    return;
  }
  svc_sp = (qmi_authi_state_type *) (*x_p)->svc_sp; 
  cl_sp = (qmi_authi_client_state_type * )(*x_p)->cl_sp;

  free_x = TRUE;
  for( i = 0; i < MAX_MSGS_IN_BUNDLE; i++ )
  {
    if( (*x_p)->resp_list[i] != (dsm_item_type *) QMI_SVC_RESPONSE_NONE &&
        (*x_p)->resp_list[i] != (dsm_item_type *) QMI_SVC_RESPONSE_PENDING )
    {
      dsm_free_packet( &((*x_p)->resp_list[i]));
    }
    if( (*x_p)->req_list[i] )
    {
      dsm_free_packet( &((*x_p)->req_list[i]) );
    }
    if ((*x_p)->cmd_list[i])
    {
      if( FALSE == (*x_p)->cmd_list[i]->in_progress )
      {
        qmi_authi_free_transaction_cmd_buf( &((*x_p)->cmd_list[i]) );
      }
      else
      {
        /*-------------------------------------------------------------------
          don't free cmd buf if it's in process (i.e. currently executing,
          called free_transaction as part of processing, e.g. reset, abort)
        -------------------------------------------------------------------*/
        free_x = FALSE;
        LOG_MSG_INFO2_0 ("Freeing transaction while command in progress - deferred.");
        /* ensure partially freed transaction doesn't get dispatched in this
           case - queued response for pending command should be freed after
           processing is complete. */
      }
    }
  }

  if (free_x)
  {
    memset( *x_p, 0, sizeof(qmi_transaction_type) );

    (void) q_link( *x_p, &((*x_p)->link) );
    (*x_p)->cl_sp = (qmi_common_client_state_type *)cl_sp;
    (*x_p)->svc_sp = svc_sp;
    if (cl_sp)
    {
      q_put( &cl_sp->common.x_free_q, &((*x_p)->link) );
    }
    *x_p = NULL;
  }
  else
  {
    LOG_MSG_INFO2_0 ("Transaction in process, "
                     "will be freed once current cmd is completed");
  }
} /* qmi_authi_free_transaction() */
#endif /* FEATURE_DATA_QMI_EAP */
