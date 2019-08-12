/*===========================================================================

                         D S _ Q M I _ DFS . C

DESCRIPTION

  The Data Services QMI Data Filter service source file.

EXTERNALIZED FUNCTIONS

  qmi_dfs_init()
    Initialize the QMI DFS service

Copyright (c) 2013-16 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_dfs.c#1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/25/13    bh     Created module
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

#include "ds_qmi_dfs.h"
#include "ps_system_heap.h"
#include "ps_sys.h"
#include "ds_Utils_DebugMsg.h"
#include "qmi_idl_lib.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "mqcsi_log.h"
#include "common_v01.h"
#include "data_filter_service_v01.h"
#include "qmi_sap.h"
#include "queue.h"
#include "qmi_framework.h"
#include "ds_rmnet_defs.h"
#include "ds_qmi_if.h"
#include "ds_qmux.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_rmnet_xport.h"
#include "PS_Audio_Offload_Client.h"
#include "ds_rev_ip_transport_hdlr.h"
#include <stringl/stringl.h>
#include "ps_utils.h"
#include "ps_sys_event.h"
#include "ps_sys_conf.h"
#include "ps_sys_ioctl.h"
#include "ps_ifacei.h"

/*---------------------------------------------------------------------------
  Service configuration
  - DPM service version is 1.0
---------------------------------------------------------------------------*/
#define DFSI_BASE_VER_MAJOR  (1)
#define DFSI_BASE_VER_MINOR  (4)

#define DFSI_ADDENDUM_VER_MAJOR  (0)
#define DFSI_ADDENDUM_VER_MINOR  (0)

#define DFSI_MAX_CLIDS    (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)
#define DFSI_QCSI_OS_SIGNAL ((rex_sigs_type)(1 << DCC_DFS_RDR_SIGNAL))

#define DFSI_MAX_REV_IP_FLTRS   8

/*To Verify whether the call is up or not on passed rmnet_instance*/
#define QMI_DFSI_IS_CALL_UP(rmnet_instance) \
                        if(rmnet_meta_sm_in_call(rmnet_instance) == FALSE)\
                         {\
                           resp_msg.resp.error = QMI_ERR_OUT_OF_CALL_V01;\
                           resp_msg.resp.result = QMI_RESULT_FAILURE_V01;\
                           goto bail;\
                         }

/*To Get rm_iface_ptr and verify the ptr*/
#define QMI_DFSI_GET_RM_IFACE_PTR(rmnet_instance)\
                         rm_iface_ptr = rmnet_meta_sm_iface(rmnet_instance);\
                         if(rm_iface_ptr == NULL)\
                         {\
                           resp_msg.resp.error = QMI_ERR_OUT_OF_CALL_V01;\
                           resp_msg.resp.result = QMI_RESULT_FAILURE_V01;\
                           goto bail;\
                         }

#define QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(bound_ip_type,ep_id,mux_id,rmnet_instance)\
                         if (bound_ip_type == DFS_IP_FAMILY_IPV6_V01)\
                         {\
                           rmnet_instance = rmnet_inst_from_ep_and_mux_id(ep_id, mux_id, 0);\
                         }\
                         else\
                         {\
                           rmnet_instance = rmnet_inst_from_ep_and_mux_id(ep_id, mux_id, 1);\
                         }\
                         if (rmnet_instance >= RMNET_INSTANCE_MAX)\
                         {\
                           LOG_MSG_ERROR_2("Invalid EP id %d and mux_id %d", ep_id, mux_id);\
                           resp_msg.resp.error  = QMI_ERR_OUT_OF_CALL_V01;\
                           resp_msg.resp.result = QMI_RESULT_FAILURE_V01;\
                           goto bail;\
                         }

/*---------------------------------------------------------------------------
  QMI DFS Client state info
---------------------------------------------------------------------------*/
typedef struct {
  q_link_type              link;           /*< Data Structure required by the queue 
                                              library to add this as part of a 
                                              queue */
  qmi_client_handle        clnt;
  dfs_ip_family_enum_v01   bound_ip_type;
  uint32                   ep_id;          // Current binding physical end point
  uint8                    mux_id;         // Current (binding) Mux Id
  dfs_bind_subscription_enum_v01 subscription;
  boolean                  is_already_bound;
  boolean                  report_powersave_filter_ind; //indication registration value
  boolean                  report_rev_ip_transport_filters;
  boolean                  report_low_latency_traffic_status; //indication registration value
  boolean                  remote_socket_handling;
}qmi_dfsi_client_info_type;

/*---------------------------------------------------------------------------
  QMI DFS Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  q_type                 client_q; /*< Information about all the clients that
                                       have connected to the service. This is queue 
                                       of type qmi_dfsi_client_info_type */
  qmi_csi_service_handle service_handle;
  qmi_sap_client_handle  sap_client_handle;
  uint16                 num_of_clients;
  void*                  powersave_filter_change_buf_ptr;
  void*                  rev_ip_transport_filter_change_buf_ptr;
} qmi_dfsi_state_info_type;


/*---------------------------------------------------------------------------
  dfs commands
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_DFSI_CMD_MIN_VAL = -1,	 
  QMI_DFSI_POWERSAVE_FILTER_STATE_CHANGE_EVT = 0,
  QMI_DFSI_CMD_REVERSE_IP_TRANSPORT_IPFLTR_CHANGED,
  QMI_DFSI_LOW_LATENCY_TRAFFIC_STATUS_EVT,
  QMI_DFSI_REMOTE_SOCKET_REQUEST,
  QMI_DFSI_REMOTE_SOCKET_RELEASE,
  QMI_DFSI_REMOTE_SOCKET_SET_OPTION,
  QMI_DFSI_CMD_MAX_VAL = 0x7FFFFFFF
} qmi_dfsi_cmd_type;

/* This is the default QMI_DFS command buffer */
typedef struct
{
  uint16  cmd_id; /* qmi_cmd_id_e_type */
  union
  {
    struct
    {
      ps_iface_type *iface_ptr; 
      dfs_powersave_filter_mode_ind_msg_v01 powersave_filter_state;
    }pws_state_data;
    struct
    {
      ps_iface_type *iface_ptr; 
      ps_iface_ipfltr_change_type fltr_info;
    }rev_ip_fltr_change;
    
    struct
    {
      ps_sys_low_latency_traffic_info_type low_latency_traffic_status;
    }low_latency_traffic_info;

    ps_sys_remote_socket_request_type socket_request;
     
    ps_sys_remote_socket_release_type socket_release;

    ps_sys_remote_socket_set_option_type socket_option;
  } data;
} qmi_dfsi_cmd_buf_type;

static qmi_dfsi_state_info_type      qmi_dfs_state;
static qmi_csi_os_params             os_params;
static boolean                       qmi_dfsi_inited = FALSE;

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

static qmi_csi_cb_error  qmi_dfsi_get_filter_capability(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dfsi_get_media_offload_statistics(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error  qmi_dfsi_bind_client(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error  qmi_dfsi_get_client_binding(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dfsi_remove_media_offload_filter(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error  qmi_dfsi_add_media_offload_filter(
   qmi_dfsi_client_info_type        *clnt_info,
   qmi_req_handle           req_handle,
   int                      msg_id,
   void                    *req_c_struct,
   int                      req_c_struct_len,
   void                     *service_handle
   ); 


static boolean qmi_dfsi_msg_hdlr_cb
(
  dcc_sig_enum_type  sig,
  void              *svc_ptr
);

static qmi_csi_cb_error qmi_dfsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmi_dfsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_dfsi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);

static void qmi_dfs_low_latency_traffic_status_ev_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

static void qmi_dfs_remote_socket_ev_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

static qmi_csi_cb_error qmi_dfsi_add_powersave_filters
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);
static qmi_csi_cb_error qmi_dfsi_set_powersave_filter_mode
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);
static qmi_csi_cb_error qmi_dfsi_get_powersave_filter_mode
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);
static qmi_csi_cb_error qmi_dfsi_set_autoexit_powersave_filter_mode
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_dfsi_remove_filters
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_dfsi_remove_all_powersave_filters
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_dfsi_get_reverse_ip_transport_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_dfsi_remote_socket_allocated
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error qmi_dfsi_indication_register
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);

static void qmi_dfsi_generate_low_latency_traffic_status_indication
(
  qmi_dfsi_cmd_buf_type     *cmd_ptr
);

static void qmi_dfsi_generate_filter_mode_indication
(
  qmi_dfsi_cmd_buf_type     *cmd_ptr
);

static void qmi_dfsi_generate_rev_ip_fltr_changed_ind
(
  qmi_dfsi_cmd_buf_type          *cmd_ptr
);

static void qmi_dfsi_generate_remote_socket_request_indication
(
  qmi_dfsi_cmd_buf_type          *cmd_ptr
);

static void qmi_dfsi_generate_remote_socket_release_indication
(
  qmi_dfsi_cmd_buf_type          *cmd_ptr
);

static void qmi_dfsi_generate_remote_socket_set_option_indication
(
  qmi_dfsi_cmd_buf_type          *cmd_ptr
);

static void qmi_dfsi_iface_ev_cb
(
  ps_iface_type              *this_iface_ptr,
  ps_iface_event_enum_type    event,
  ps_iface_event_info_u_type  event_info,
  void                       *user_data_ptr
);

static void qmi_dfsi_process_cmd
(
  dcc_cmd_enum_type  cmd,
  void              *cmd_ptr
);

static void* qmi_dfsi_get_cmd_buf
(
  qmi_dfsi_cmd_type cmd_id
);

static void qmi_dfsi_convert_ps_ipfltr_to_dfs_type
(
  ip_filter_type           * ps_fltr_rule,
  dfs_filter_rule_type_v01 * dfs_fltr_rule
);

static void qmi_dfsi_convert_ps_to_dfs_v4_info
(
  ip_filter_type           * ps_fltr_rule,
  dfs_filter_rule_type_v01 * dfs_fltr_rule
);

static void qmi_dfsi_convert_ps_to_dfs_xport_info
(
  ip_filter_type           * ps_fltr_rule,
  dfs_filter_rule_type_v01 * dfs_fltr_rule
);

static void qmi_dfsi_convert_ps_to_dfs_v6_info
(
  ip_filter_type           * ps_fltr_rule,
  dfs_filter_rule_type_v01 * dfs_fltr_rule
);

static void qmi_dfsi_fill_v4_filter_spec
(
  dfs_filter_rule_type_v01*   filter_rules,
  ip_filter_type*             fltr_spec
);

static void qmi_dfsi_fill_v6_filter_spec
(
  dfs_filter_rule_type_v01*   filter_rules,
  ip_filter_type*             fltr_spec
);

static void qmi_dfsi_fill_fltr_xport_info
(
  dfs_xport_header_type_v01*  filter_rules,
  ip_filter_type*             fltr_spec
);

static void qmi_dfsi_fill_filter_error_mask
(
  ip_filter_type*                  filter_spec,
  dfs_filter_param_error_mask_v01* error_mask
);

#ifdef FEATURE_DATA_AUDIO_OFFLOAD
static int dfsi_dss_error_to_qmi_error 
(
  int16 dss_err
);
#endif

static data_ep_id_type_v01 qmi_dfs_internal_ep_id_to_qmi_ep_id
(
  uint32 ep_id
);

boolean qmi_dfsi_verify_client_subs
(
  qmi_dfsi_client_info_type            *clnt_info,
  uint32                               event_subs_id,
  boolean                              call_specific
);
/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/


/* Define service handle table for DFS messages */
static qmi_csi_cb_error (* const req_handle_table[])
(
 qmi_dfsi_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
) = 
{
  NULL,NULL,NULL,                             /* Request handler for message ID 0x00 - 0x02*/
  qmi_dfsi_indication_register,               /* Request handler for message ID 0x03 */
  NULL,NULL,NULL,NULL,                        /* Request handler for message ID 0x04 - 0x07*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x08 - 0x0F*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x10 - 0x17*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x18 - 0x1F*/
  qmi_dfsi_get_filter_capability,          /* Request handler for message ID 0x20 */
  qmi_dfsi_bind_client,                    /* Request handler for message ID 0x21 */
  qmi_dfsi_get_client_binding,             /* Request handler for message ID 0x22 */
  qmi_dfsi_add_media_offload_filter,       /* Request handler for message ID 0x23 */
  qmi_dfsi_remove_media_offload_filter,    /* Request handler for message ID 0x24 */
  qmi_dfsi_get_media_offload_statistics,      /* Request handler for message ID 0x25 */
  NULL,                                       /* Request handler for message ID 0x26 */
  qmi_dfsi_remove_filters,                    /* Request handler for message ID 0x27 */
  qmi_dfsi_add_powersave_filters,             /* Request handler for message ID 0x28*/
  qmi_dfsi_set_powersave_filter_mode,         /* Request handler for message ID 0x29 */
  qmi_dfsi_get_powersave_filter_mode,         /* Request handler for message ID 0x2A */
  qmi_dfsi_set_autoexit_powersave_filter_mode,/* Request handler for message ID 0x2B */
  NULL,                                       /* Request handler for message ID 0x2C */
  qmi_dfsi_remove_all_powersave_filters,      /* Request handler for message ID 0x2D */
  NULL, 
  NULL,
  qmi_dfsi_get_reverse_ip_transport_filters,  /* Request handler for message ID 0x30 */ 
  NULL,
  NULL,
  qmi_dfsi_remote_socket_allocated,           /* Request handler for message ID 0x33 */          
};

extern rex_tcb_type dcc_tcb;

static int dfsi_compare_qmi_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_dfsi_client_info_type *clnt_info = (qmi_dfsi_client_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
}

static int dfsi_compare_connection_info_handle
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
  FUNCTION QMI_DFS_INIT()

  DESCRIPTION
    Register the DFS service with QCSI 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dfs_init
(
  void
)
{
  qmi_csi_error         rc;
  qmi_sap_error         src;
  int16                 ps_errno;
  ps_sys_event_enum_type    sys_event = PS_SYS_EVENT_MAX;
  qmi_csi_options       options;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1("QMI DFS service init %d", qmi_dfsi_inited);

  if (!qmi_dfsi_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)dcc_set_sig_handler(DCC_DFS_RDR_SIGNAL, 
                              qmi_dfsi_msg_hdlr_cb, &qmi_dfs_state);
    dcc_enable_sig(DCC_DFS_RDR_SIGNAL);

    /* Set the signal to init again */
    DCC_SET_SIGNAL(DCC_DFS_RDR_SIGNAL);
    return;
  }

  os_params.tcb = &dcc_tcb;
  os_params.sig = DFSI_QCSI_OS_SIGNAL;
  
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);
  
  /*-------------------------------------------------------------------------
    Register QMI dfs service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register_with_options(dfs_get_service_object_v01(),
                        qmi_dfsi_svc_connect_cb,
                        qmi_dfsi_svc_disconnect_cb, 
                        qmi_dfsi_handle_client_req_cb, 
                        &qmi_dfs_state, &os_params, 
                        &options,
                        &qmi_dfs_state.service_handle);
  
  if(rc != QMI_CSI_NO_ERR)
  {
    ERR_FATAL("Unable to register dfs service! Error %d", rc, 0, 0);
    ASSERT(0);
    return;
  }

  //register with SAP
  src = qmi_sap_register(dfs_get_service_object_v01(), NULL, &qmi_dfs_state.sap_client_handle);
  if (src != QMI_SAP_NO_ERR)
  {
    ERR_FATAL("Unable to register dfs service with QSAP! Error %d", src, 0, 0);
    ASSERT(0);
    return;
  }


  /* Initialize the queue which will hold all handles of clients */
  (void) q_init(&qmi_dfs_state.client_q);
  qmi_dfs_state.num_of_clients = 0;

  do
  {
    if( 0 != ps_sys_event_reg( PS_SYS_TECH_ALL,
                    PS_SYS_EVENT_LOW_LATENCY_TRAFFIC_STATUS,
                    qmi_dfs_low_latency_traffic_status_ev_cb,
                    NULL,
                    &ps_errno ) )
    {
      sys_event = PS_SYS_EVENT_LOW_LATENCY_TRAFFIC_STATUS;
      break;
    }
    if (0 != ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                                 PS_SYS_EVENT_REMOTE_SOCKET_REQUEST,
                                 PS_SYS_PRIMARY_SUBS,
                                 qmi_dfs_remote_socket_ev_cb,
                                 NULL,
                                 &ps_errno))
    {
      sys_event = PS_SYS_EVENT_REMOTE_SOCKET_REQUEST;
      break;
    }
    if (0 != ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                                 PS_SYS_EVENT_REMOTE_SOCKET_RELEASE,
                                 PS_SYS_PRIMARY_SUBS,
                                 qmi_dfs_remote_socket_ev_cb,
                                 NULL,
                                 &ps_errno))
    {
      sys_event = PS_SYS_EVENT_REMOTE_SOCKET_RELEASE;
      break;
    }
    if (0 != ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                                 PS_SYS_EVENT_REMOTE_SOCKET_SET_OPTION,
                                 PS_SYS_PRIMARY_SUBS,
                                 qmi_dfs_remote_socket_ev_cb,
                                 NULL,
                                 &ps_errno))
    {
      sys_event = PS_SYS_EVENT_REMOTE_SOCKET_SET_OPTION;
      break;
    }
  }
  while (0); 
  if (sys_event != PS_SYS_EVENT_MAX)
  {
    LOG_MSG_ERROR_1("PS_SYS event reg failed for %d",sys_event);
    ASSERT(0);
    return;
  }

  /* Register powersave filter mode change event */
    qmi_dfs_state.powersave_filter_change_buf_ptr = 
             ps_iface_alloc_event_cback_buf( qmi_dfsi_iface_ev_cb, NULL );
    if(qmi_dfs_state.powersave_filter_change_buf_ptr != NULL)
    {
      if(0 != ps_iface_event_cback_reg( NULL,
                          IFACE_POWERSAVE_FILTERING_MODE_CHANGE_EV,
                          qmi_dfs_state.powersave_filter_change_buf_ptr))
      {
        LOG_MSG_ERROR_0("IFACE_POWERSAVE_FILTERING_MODE_CHANGE_EV reg failed");
        ASSERT(0);
        return;
      }
    }
    else
    {
      ASSERT(0);
      return;
    }

  qmi_dfs_state.rev_ip_transport_filter_change_buf_ptr = ps_iface_alloc_event_cback_buf( 
                                       qmi_dfsi_iface_ev_cb, NULL );
  if(qmi_dfs_state.rev_ip_transport_filter_change_buf_ptr != NULL)
  {
    if(0 != ps_iface_event_cback_reg( NULL,
                                    IFACE_REVERSE_IP_TRANSPORT_IPFLTR_CHANGED_EV,
                                    qmi_dfs_state.rev_ip_transport_filter_change_buf_ptr))
    {
      ASSERT(0);
      return;
    }
  }
  else
  {
    ASSERT(0);
    return;
  }
///*-------------------------------------------------------------------------
//  Setting QMI dfs service command handler with DCC task process
//-------------------------------------------------------------------------*/
    dcc_set_cmd_handler(DCC_DFS_SVC_CMD, qmi_dfsi_process_cmd);
} /* qmi_dfs_init */

static boolean qmi_dfsi_msg_hdlr_cb
(
  dcc_sig_enum_type  sig,
  void              *svc_ptr
)
{
  qmi_dfsi_state_info_type *dfs_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT((sig == DCC_DFS_RDR_SIGNAL) && svc_ptr);

  if (!qmi_dfsi_inited)
  {
    qmi_dfsi_inited = TRUE;
    /* Call init again to register with QCSI now
     * Wait for all tasks to be inited */
    qmi_dfs_init();
    return TRUE;
  }

  dfs_sp = (qmi_dfsi_state_info_type*) svc_ptr;

  qmi_csi_handle_event(dfs_sp->service_handle, &os_params);
  return TRUE;
} /* qmi_dfsi_msg_hdlr_cb */

static qmi_csi_cb_error qmi_dfsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_dfsi_client_info_type    *clnt_info_ptr;
  qmi_dfsi_state_info_type     *dfs_svc_ptr;
  qmi_csi_cb_error             rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT(connection_handle);
  ASSERT(service_handle);

  dfs_svc_ptr = (qmi_dfsi_state_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &dfs_svc_ptr->client_q,
                                dfsi_compare_qmi_client_handle,
                                client_handle ) )
  {
    LOG_MSG_ERROR_1("dfsi_svc_connect_cb: dfs client 0x%p present",
                  client_handle);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  if (DFSI_MAX_CLIDS == dfs_svc_ptr->num_of_clients)
  {
    LOG_MSG_ERROR_1("dfsi_svc_connect_cb: Maximum number of clients already allocated %d",
                  DFSI_MAX_CLIDS);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(clnt_info_ptr, sizeof(qmi_dfsi_client_info_type),
                           qmi_dfsi_client_info_type*);
  if(!clnt_info_ptr)
  {
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  memset(clnt_info_ptr, 0, sizeof(qmi_dfsi_client_info_type));
  clnt_info_ptr->bound_ip_type = DFS_IP_FAMILY_IPV4_V01;
  clnt_info_ptr->subscription =  DFS_DONT_CARE_SUBS_V01; 
  clnt_info_ptr->is_already_bound = FALSE;

  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;
  /* Add it to the queue of clients */
  q_put( &(dfs_svc_ptr->client_q), &(clnt_info_ptr->link) );
  dfs_svc_ptr->num_of_clients++;

  *connection_handle = clnt_info_ptr;
  LOG_MSG_INFO1_1("dfsi_svc_connect_cb: Alloc client 0x%p", clnt_info_ptr);
  rc = QMI_CSI_CB_NO_ERR;
bail:
  return rc;
} /* qmi_dfsi_svc_connect_cb() */

static void qmi_dfsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_dfsi_state_info_type    *dfs_svc_ptr;
  qmi_dfsi_client_info_type    *clnt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle);
  ASSERT(service_handle);
  dfs_svc_ptr = (qmi_dfsi_state_info_type *)service_handle;

  LOG_MSG_INFO1_1("dfsi_svc_disconnect_cb: Releasing client 0x%p", 
                connection_handle);

  clnt_info_ptr = q_linear_search( &dfs_svc_ptr->client_q,
                                   dfsi_compare_connection_info_handle,
                                   connection_handle );
  if(NULL == clnt_info_ptr)
  {
    LOG_MSG_ERROR_1("dfsi_svc_connect_cb: dfs client 0x%p not found",
                  connection_handle);
    return;

  }

  if(TRUE != q_delete_ext( &dfs_svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
  {
    LOG_MSG_ERROR_1("dfsi_svc_connect_cb: dfs client 0x%p could not be deleted from queue",
                    clnt_info_ptr);
    return;
  }
  PS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  dfs_svc_ptr->num_of_clients--;
} /* qmi_dfsi_svc_disconnect_cb() */


static qmi_csi_cb_error qmi_dfsi_handle_client_req_cb
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
  qmi_dfsi_client_info_type     *clnt_info_ptr;
  qmi_dfsi_state_info_type      *dfs_svc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle);
  ASSERT(service_handle);

  rc = QMI_CSI_CB_INTERNAL_ERR;
  dfs_svc_ptr = (qmi_dfsi_state_info_type*) service_handle;

  clnt_info_ptr = q_linear_search( &dfs_svc_ptr->client_q,
                                   dfsi_compare_connection_info_handle,
                                   connection_handle );
  if(NULL == clnt_info_ptr)
  {
    LOG_MSG_ERROR_1("dfsi_handle_req_cb: Invalid clnt 0x%p",
                  connection_handle);
    return rc;
  }

  if(msg_id < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
  {
    if(req_handle_table[msg_id])
    {
      LOG_MSG_INFO1_2("Handling msg id %d for client 0x%p", 
                msg_id, clnt_info_ptr);
      rc = req_handle_table[msg_id] (clnt_info_ptr, req_handle, msg_id,
                                     req_c_struct, req_c_struct_len,
                                     service_handle);
    }
    else
    {
      LOG_MSG_ERROR_1("dfsi_handle_req_cb: NULL message ID handler: %d",
                    msg_id);
    }
  }
  else
  {
    LOG_MSG_ERROR_1("dfsi_handle_req_cb: Invalid message ID: %d", msg_id);
  }

  return rc;
} /* qmi_dfsi_handle_client_req_cb() */

/*===========================================================================
  FUNCTION QMI_DFS_GET_FILTER_CAPABILITY()

  DESCRIPTION
    Get the filter capability for the client.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_dfsi_get_filter_capability
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                   req_handle,
  int                              msg_id,
  void                             *req_c_struct,
  int                              req_c_struct_len,
  void                             *service_handle
)
{
  dfs_get_filter_capability_resp_msg_v01   resp_msg;
#ifdef FEATURE_DATA_AUDIO_OFFLOAD
  uint32 max_filters = 0;
#endif
/*-----------------------------------------------------------------------------*/
  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_get_filter_capability_resp_msg_v01));

#ifdef FEATURE_DATA_AUDIO_OFFLOAD
  max_filters = ps_audio_offload_client_get_max_fltr();
  resp_msg.max_media_offload_filters_valid = TRUE;
  resp_msg.max_media_offload_filters = max_filters;
  LOG_MSG_INFO1_1("Sending Filter Capability %d", max_filters);
#endif
    resp_msg.max_powersave_filters_valid = TRUE;
    resp_msg.max_powersave_filters.max_filters_supported = 
                                           PS_IFACE_PWRSAVE_IPFLTR_MAX;
    resp_msg.max_powersave_filters.max_filters_supported_per_add = 
                                           PS_IFACE_PWRSAVE_IPFLTR_MAX;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(dfs_get_filter_capability_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_DFSI_GET_MEDIA_OFFLOAD_STATISTICS()

  DESCRIPTION
    Get the filter statistics for given filter id.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_dfsi_get_media_offload_statistics
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                   req_handle,
  int                              msg_id,
  void                             *req_c_struct,
  int                              req_c_struct_len,
  void                             *service_handle
)
{
  dfs_get_media_offload_statistics_resp_msg_v01   resp_msg;
#ifdef FEATURE_DATA_AUDIO_OFFLOAD

  dfs_get_media_offload_statistics_req_msg_v01    *req_ptr;
  int16 ps_error = 0;
  ps_audio_offload_client_stats_type              filter_stats;
#endif
//------------------------------------------------------------------------------
  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_get_media_offload_statistics_resp_msg_v01));

#ifndef FEATURE_DATA_AUDIO_OFFLOAD
  resp_msg.resp.error  = QMI_ERR_NOT_SUPPORTED_V01;
  resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  goto bail;
#else
  req_ptr = (dfs_get_media_offload_statistics_req_msg_v01*)req_c_struct;
  if (clnt_info->is_already_bound == FALSE)
  {
    resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
  if (ps_audio_offload_client_get_stats(req_ptr->filter_handle, &filter_stats, &ps_error) != DSS_SUCCESS) 
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error  = dfsi_dss_error_to_qmi_error(ps_error);      
    goto bail;
  }
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error  = QMI_ERR_NONE_V01;
  resp_msg.bytes_sent_valid = TRUE;
  resp_msg.bytes_sent = filter_stats.bytes_tx;
  resp_msg.bytes_received_valid = TRUE;
  resp_msg.bytes_received = filter_stats.bytes_rx;
  resp_msg.packets_sent_valid = TRUE;
  resp_msg.packets_sent = filter_stats.pkts_tx;
  resp_msg.packets_received_valid = TRUE;
  resp_msg.packets_received = filter_stats.pkts_rx;

  LOG_MSG_INFO1_4("Offload Statistics: bytes sent: %d, bytes recv : %d, pkts_sent : %d, pkts_rx : %d",
                  filter_stats.bytes_tx,
                  filter_stats.bytes_rx,
                  filter_stats.pkts_tx,
                  filter_stats.pkts_rx);
#endif
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(dfs_get_media_offload_statistics_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_DFS_BIND_CLIENT

  DESCRIPTION
    Bind the client to a given data port.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_dfsi_bind_client
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle            req_handle,
  int                       msg_id,
  void                      *req_c_struct,
  int                       req_c_struct_len,
  void                      *service_handle
)
{
  dfs_bind_client_req_msg_v01   *req_msg;
  dfs_bind_client_resp_msg_v01  resp_msg;
  uint32                        ep_id;
  uint8                         mux_id;
  dfs_ip_family_enum_v01        ip_preference;
  int                           errval = QMI_ERR_NONE_V01;
  boolean                       is_bound;
//------------------------------------------------------------------------------
  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);

  ep_id  = clnt_info->ep_id;
  mux_id = clnt_info->mux_id;
  ip_preference = clnt_info->bound_ip_type;
  is_bound = clnt_info->is_already_bound;

  req_msg = (dfs_bind_client_req_msg_v01 *)req_c_struct;

  if (req_msg->ep_id_valid == TRUE)
  {
    if (clnt_info->is_already_bound == TRUE)
    {
      errval = QMI_ERR_NO_EFFECT_V01;
      goto bail;
    }
	if (QMI_EP_ID_VALID(req_msg->ep_id))
	{
      ep_id = QMI_EP_ID_TO_INTERNAL_EP_ID(req_msg->ep_id);
	}
	else
	{
	  LOG_MSG_ERROR_2("Invalid EP id 0x%x:0x%x", req_msg->ep_id.ep_type, req_msg->ep_id.iface_id);
	  errval = QMI_ERR_INVALID_ARG;
	  goto bail;
	}
    if (rmnet_phys_xport_from_ep_id(ep_id) == NULL)
    {
      LOG_MSG_ERROR_2("Invalid EP id 0x%x:0x%x", req_msg->ep_id.ep_type, req_msg->ep_id.iface_id);
	  errval = QMI_ERR_INVALID_ARG;
	  goto bail;
    }
    if (req_msg->mux_id_valid == TRUE)
    {
      mux_id = (uint8)req_msg->mux_id;
    }
    else
    {
      mux_id = 0;
    }
    is_bound = TRUE;
  }
  if (req_msg->mux_id_valid == TRUE)
  {
    if((rmnet_phys_xport_from_ep_id(ep_id) == NULL))
    {
      LOG_MSG_ERROR_3("Invalid EP id 0x%x:0x%x for mux id %d", req_msg->ep_id.ep_type, req_msg->ep_id.iface_id, req_msg->mux_id);
	  errval = QMI_ERR_INVALID_ARG;
	  goto bail;
    }
    mux_id = (uint8)req_msg->mux_id;
  }
  if (req_msg->ip_preference_valid == TRUE)
  {
    if ((req_msg->ip_preference == DFS_IP_FAMILY_IPV4_V01)
       || (req_msg->ip_preference == DFS_IP_FAMILY_IPV6_V01))
    {
      ip_preference = req_msg->ip_preference;
    }
    else
    {
      LOG_MSG_INFO1_1("Unknown IP Preference received %d",
                      req_msg->ip_preference);
      errval = QMI_ERR_INVALID_ARG;
      goto bail;
    }
  }

  if (req_msg->bind_subs_valid == TRUE)
  {
    if ( (req_msg->bind_subs < DFS_DEFAULT_SUBS_V01 || 
          req_msg->bind_subs > DFS_TERTIARY_SUBS_V01) &&
          req_msg->bind_subs != DFS_DONT_CARE_SUBS_V01 ) 
      {
        LOG_MSG_INFO1_1("Unknown Subscription received %d",
                        req_msg->bind_subs);
        errval = QMI_ERR_INVALID_ARG;
        goto bail;
    }
  }
bail:
  if (errval == QMI_ERR_NONE_V01)
  {
    clnt_info->bound_ip_type = ip_preference;
    clnt_info->ep_id = ep_id;
    clnt_info->mux_id = mux_id;
    clnt_info->is_already_bound = is_bound;
    clnt_info->subscription =req_msg->bind_subs;
    LOG_MSG_INFO1_3("The bound ep_id %d, mux_id %d, Ip_pref %d", ep_id, mux_id, ip_preference);
  }
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01) ? QMI_RESULT_SUCCESS_V01 : QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(dfs_bind_client_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DFS_GET_CLIENT_BINDING

  DESCRIPTION
    Get the Client Binding data (Client Ip_pref and Client SIO Data port)

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_dfsi_get_client_binding
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle            req_handle,
  int                       msg_id,
  void                      *req_c_struct,
  int                       req_c_struct_len,
  void                      *service_handle
)
{
  dfs_get_client_binding_resp_msg_v01  resp_msg;
//------------------------------------------------------------------------------
  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_get_client_binding_resp_msg_v01));
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  if (clnt_info->is_already_bound)
  {
    resp_msg.bound_ep_id_valid = TRUE;
    resp_msg.bound_mux_id_valid = TRUE;
    resp_msg.bound_ep_id = qmi_dfs_internal_ep_id_to_qmi_ep_id(clnt_info->ep_id);
    resp_msg.bound_mux_id = clnt_info->mux_id;
  }
  resp_msg.ip_preference_valid = TRUE;
  resp_msg.ip_preference = clnt_info->bound_ip_type;
  resp_msg.bound_subs_valid = TRUE;
  resp_msg.bound_subs = ( dfs_bind_subscription_enum_v01 )clnt_info->subscription;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(dfs_get_client_binding_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}


/*=========================================================================== 

  FUNCTION QMI_DFS_ADD_MEDIA_OFFLOAD_FILTER

  DESCRIPTION
    Add and apply a filter with parameters provided.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_dfsi_add_media_offload_filter
( 
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                   req_handle,
  int                              msg_id,
  void                             *req_c_struct,
  int                              req_c_struct_len,
  void                             *service_handle
)
{
  
  dfs_add_media_offload_filter_resp_msg_v01   resp_msg;
  ip_filter_spec_type                         filter_data;
#ifdef FEATURE_DATA_AUDIO_OFFLOAD
  dfs_add_media_offload_filter_req_msg_v01    *req_msg;
  int16 ps_error = 0;
  ps_iface_type                               *iface_ptr;
  int32                                       retval = 0;
  ps_iface_id_type                            iface_id;
  rmnet_instance_e_type                       rmnet_instance = RMNET_INSTANCE_MAX;
  uint32                                      subs_id = 0;
#endif
//====================================================================
  ASSERT(req_c_struct &&(req_c_struct_len >0)&& clnt_info && service_handle);

  memset(&filter_data, 0, sizeof(ip_filter_spec_type));
  memset(&resp_msg, 0, sizeof(dfs_add_media_offload_filter_resp_msg_v01));

#ifndef FEATURE_DATA_AUDIO_OFFLOAD
  resp_msg.resp.error  = QMI_ERR_NOT_SUPPORTED_V01;
  resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  goto bail;
#else
  req_msg = (dfs_add_media_offload_filter_req_msg_v01 *)req_c_struct;
  if (clnt_info->is_already_bound == FALSE)
  {
    resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    LOG_MSG_INFO1_0("The Client is not yet bound to any Data port");
    goto bail;
  }
  if (req_msg->filter_id_valid) 
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(filter_data.list_ptr, sizeof(ip_filter_type),
                             ip_filter_type*);
    if (filter_data.list_ptr == NULL) 
    {
      resp_msg.resp.error  = QMI_ERR_NO_MEMORY_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }
    if (clnt_info->bound_ip_type == DFS_IP_FAMILY_IPV4_V01)
    {
      rmnet_instance = rmnet_inst_from_ep_and_mux_id(clnt_info->ep_id, clnt_info->mux_id, 1);
    }
    else if (clnt_info->bound_ip_type == DFS_IP_FAMILY_IPV6_V01)
    {
      rmnet_instance = rmnet_inst_from_ep_and_mux_id(clnt_info->ep_id, clnt_info->mux_id, 0);
    }
    if (rmnet_instance == RMNET_INSTANCE_MAX)
    {
      LOG_MSG_ERROR_2("Invalid EP id %d and mux_id %d", clnt_info->ep_id, clnt_info->mux_id);
      resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }

    if (!rmnet_meta_sm_in_call(rmnet_instance)) 
    {
      LOG_MSG_ERROR_2("Call not up on the ep_id %d and mux_id %d", clnt_info->ep_id, clnt_info->mux_id);
      resp_msg.resp.error  = QMI_ERR_OUT_OF_CALL_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }
    filter_data.num_filters = 1;
    memset(filter_data.list_ptr, 0, sizeof(ip_filter_type));
    if (req_msg->ipv4_dest_address_valid == TRUE)
    {
      if (clnt_info->bound_ip_type != DFS_IP_FAMILY_IPV4_V01)
      {
        resp_msg.resp.error  = QMI_ERR_INVALID_ARG_V01;
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        LOG_MSG_INFO1_0("The Client is Bound to a V4 type, but got request for V6");
        goto bail;
      }
      filter_data.list_ptr[0].ip_vsn = IP_V4;
      filter_data.list_ptr[0].ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_DST_ADDR;
      filter_data.list_ptr[0].ip_hdr.v4.dst.addr.ps_s_addr   =  ps_htonl(req_msg->ipv4_dest_address);
      filter_data.list_ptr[0].ip_hdr.v4.dst.subnet_mask.ps_s_addr = 0xFFFFFFFF;
    }
    if (req_msg->ipv6_dest_address_valid == TRUE)
    {
      if (clnt_info->bound_ip_type != DFS_IP_FAMILY_IPV6_V01)
      {
        resp_msg.resp.error  = QMI_ERR_INVALID_ARG_V01;
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        LOG_MSG_INFO1_0("The Client is Bound to a V6 type, but got request for V4");
        goto bail;
      }
      filter_data.list_ptr[0].ip_vsn = IP_V6;
      filter_data.list_ptr[0].ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_DST_ADDR;
      memscpy( filter_data.list_ptr[0].ip_hdr.v6.dst.addr.in6_u.u6_addr8,
               QMI_DFS_IPV6_ADDR_LEN_V01,
               req_msg->ipv6_dest_address.ipv6_address,
               QMI_DFS_IPV6_ADDR_LEN_V01);
      filter_data.list_ptr[0].ip_hdr.v6.dst.prefix_len = req_msg->ipv6_dest_address.prefix_len;
      
    }
    if (req_msg->udp_dest_port_valid == TRUE)
    {
      filter_data.list_ptr[0].next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;
      filter_data.list_ptr[0].next_prot_hdr.udp.dst.port    = ps_htons(req_msg->udp_dest_port);
    }
    if (req_msg->xport_protocol_valid == TRUE)
    {
      if (req_msg->xport_protocol != DFS_PROTO_UDP_V01)
      {
        resp_msg.resp.error  = QMI_ERR_INVALID_ARG_V01;
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        goto bail;
      }
      if (filter_data.list_ptr[0].ip_vsn == (ip_version_enum_type)DFS_IP_FAMILY_IPV4_V01)
      {
        filter_data.list_ptr[0].ip_hdr.v4.next_hdr_prot = PS_IPPROTO_UDP;
        filter_data.list_ptr[0].ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
      }
      else if (filter_data.list_ptr[0].ip_vsn == (ip_version_enum_type)DFS_IP_FAMILY_IPV6_V01)
      {
        filter_data.list_ptr[0].ip_hdr.v6.next_hdr_prot = PS_IPPROTO_UDP;
        filter_data.list_ptr[0].ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
      }
    }

    iface_ptr = rmnet_meta_sm_get_um_iface(rmnet_instance);
    iface_id  = PS_IFACE_GET_ID(iface_ptr);
    subs_id   = PS_IFACE_GET_SUBS_ID(iface_ptr);
    if (!qmi_dfsi_verify_client_subs(clnt_info, subs_id, TRUE))
    {
      LOG_MSG_ERROR_2( "Error: Client subs doesn't match current data call subs"
                       " Client subs %d Call subs %d", clnt_info->subscription, subs_id);
      resp_msg.resp.error  = QMI_ERR_INCOMPATIBLE_STATE_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }

    if ( (retval = ps_audio_offload_client_add_fltr(req_msg->filter_id, iface_id, &filter_data, &ps_error)) < 0) 
    {
      resp_msg.resp.error = dfsi_dss_error_to_qmi_error(ps_error);
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    } 
    else 
    {
      resp_msg.filter_handle_valid = TRUE;
      resp_msg.filter_handle       = (uint32)retval;
      LOG_MSG_INFO1_1("Successfully added filter and filter handle is %d",
                      retval);
      resp_msg.resp.error = QMI_ERR_NONE_V01;
      resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    }
  }
#endif
bail:
  if (filter_data.list_ptr != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(filter_data.list_ptr);
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(dfs_add_media_offload_filter_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR; 
}/*qmi_dfsi_add_media_offload_filter*/



/*===========================================================================
  FUNCTION QMI_DFSI_REMOVE_MEDIA_OFFLOAD_FILTER

  DESCRIPTION
   Remove the filter got from the filter id

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_dfsi_remove_media_offload_filter
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                   req_handle,
  int                              msg_id,
  void                             *req_c_struct,
  int                              req_c_struct_len,
  void                             *service_handle
)
{
#ifdef FEATURE_DATA_AUDIO_OFFLOAD
  dfs_remove_media_offload_filter_req_msg_v01 *req_msg;
  int16 ps_error = 0;
#endif
  dfs_remove_media_offload_filter_resp_msg_v01 resp_msg;
//--------------------------------------------------------------------------
  ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_remove_media_offload_filter_resp_msg_v01));
#ifndef FEATURE_DATA_AUDIO_OFFLOAD
  resp_msg.resp.error  = QMI_ERR_NOT_SUPPORTED_V01;
  resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  goto bail;
#else
  req_msg = (dfs_remove_media_offload_filter_req_msg_v01*)req_c_struct;
  if (clnt_info->is_already_bound == FALSE)
  {
    resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    LOG_MSG_INFO1_0("The Client is not bound to any ep_id and mux_id to remove filter");
    goto bail;
  }
  LOG_MSG_INFO1_1("Received Remove Filter Request with handle %d",
                  req_msg->filter_handle);

  if (ps_audio_offload_client_delete_fltr(req_msg->filter_handle, &ps_error) != QMI_NO_ERR)
  {
    if ((ps_error == DS_EINVAL) || (ps_error == DS_EFAULT))
    {
      resp_msg.resp.error =  QMI_ERR_INVALID_HANDLE_V01;
    }
    else
    {
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    }
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
#endif
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(dfs_remove_media_offload_filter_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR; 
}/*qmi_dfsi_remove_media_offload_filter*/


#ifdef FEATURE_DATA_AUDIO_OFFLOAD
/*===========================================================================
  FUNCTION QMI_DFS_DSS_ERROR_TO_QMI_ERROR

  DESCRIPTION
    Map DSS error to qmi error

  PARAMETERS
    dss_error : DSS Socket error
    
  RETURN VALUE
    QMI_ERR_Types
 
  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
static int dfsi_dss_error_to_qmi_error 
(
  int16 dss_err
)
{
  int qmi_err = QMI_ERR_NONE_V01;
  switch (dss_err) 
  {
    case DS_ENOMEM:
      qmi_err = QMI_ERR_NO_MEMORY_V01;
      break;
    case DS_EINVAL:
      qmi_err = QMI_ERR_INVALID_ARG_V01;
      break;
    case DS_EADDRINUSE:
      qmi_err = QMI_ERR_INVALID_ARG_V01;
      break;
    case DS_ENETNONET:
      qmi_err = QMI_ERR_OUT_OF_CALL_V01;
      break;
    case DS_EFAULT:
      qmi_err = QMI_ERR_INVALID_ARG_V01;
      break;
    default:
      qmi_err = QMI_ERR_INTERNAL_V01;
      break;
  }
  return qmi_err;
}
#endif

/*===========================================================================
  FUNCTION QMI_DFS_INTERNAL_EP_ID_TO_QMI_EP_ID

  DESCRIPTION
    Convert internal EP ID to QMI EP ID

  PARAMETERS
    ep_id: internal end point id
    
  RETURN VALUE
    qmi_ep_id_type
 
  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
data_ep_id_type_v01 qmi_dfs_internal_ep_id_to_qmi_ep_id
(
  uint32 ep_id
)
{
  data_ep_id_type_v01 qmi_ep_id = {0};
  qmi_ep_id.ep_type = ((ep_id & 0xFFFFFFFF)>>24);
  qmi_ep_id.iface_id = (ep_id & 0x00FFFFFF);
  
  return qmi_ep_id;
}


/*===========================================================================
  FUNCTION QMI_DFSI_ADD_POWERSAVE_FILTERS()

  DESCRIPTION
    This function adds powersave filters and returns handles for each filter added.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle
    msg_id      : Message ID
    req_c_struct_len : length of request message
    service_handle :DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_add_powersave_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations -------------------------------*/
  dfs_add_powersave_filters_req_msg_v01	*req_msg;
  dfs_add_powersave_filters_resp_msg_v01 resp_msg;
  ps_iface_type*                         rm_iface_ptr = NULL;
  int                                    retval = -1;
  boolean                                filter_mode;
  ip_filter_type                        *fltr_spec = NULL;
  ps_iface_ipfltr_add_param_type         filter_param;
  ps_iface_ipfltr_handle_type           *ipfltr_handle_arr = NULL;
  uint8                                  cnt = 0;
  sint15                                 ps_errno = 0;
  uint32                                 max_allowed = 0;
  uint32                                 used        = 0;
  rmnet_instance_e_type                  rmnet_instance = RMNET_INSTANCE_MAX;
  /*-------------------------------------------------------*/

  ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_add_powersave_filters_resp_msg_v01));
  req_msg = (dfs_add_powersave_filters_req_msg_v01*)req_c_struct;

  /*return if arguments are not passed*/
  if(req_msg->filter_rules_valid != TRUE)
  {
    resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
  QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
  	                                      clnt_info->bound_ip_type,
  	                                      clnt_info->ep_id,
                                              clnt_info->mux_id,
                                              rmnet_instance);
  QMI_DFSI_IS_CALL_UP(rmnet_instance);
  QMI_DFSI_GET_RM_IFACE_PTR(rmnet_instance);
  if(ps_iface_ipfltr_stats(rm_iface_ptr,
                           IP_FLTR_CLIENT_POWERSAVE,
                           &max_allowed,&used,&ps_errno) != 0)
  {
    LOG_MSG_ERROR_2("Getting Stats Failed max_allowed = (%d) used = (%d)",
                     max_allowed,used);
    resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
  ps_errno = 0;
  if(req_msg->filter_rules_len > 0 && 
       req_msg->filter_rules_len  <= (max_allowed - used))
  {
    /* Get the current filter mode before adding new filters 
       which needs to be passed to ps_iface_ipfltr_add_ex*/
    retval = ps_iface_get_powersave_filtering_mode(rm_iface_ptr, 
                           &filter_mode,
                           &ps_errno);
    if(retval != 0)
    {
      LOG_MSG_ERROR_1("ps_iface_get_powersave_filtering_mode failed %d)",retval);
      resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }
    else
    {
      LOG_MSG_INFO1_1("Current Powersave Filter_mode = %d",filter_mode);
      memset(&filter_param, 0, sizeof(ps_iface_ipfltr_add_param_type));  
      PS_SYSTEM_HEAP_MEM_ALLOC(fltr_spec, 
                   sizeof(ip_filter_type)*(req_msg->filter_rules_len),
                   ip_filter_type*);

      PS_SYSTEM_HEAP_MEM_ALLOC(ipfltr_handle_arr,
            sizeof(ps_iface_ipfltr_handle_type)*(req_msg->filter_rules_len),
                              ps_iface_ipfltr_handle_type*);

      if(fltr_spec == NULL || ipfltr_handle_arr == NULL)
      {
        resp_msg.resp.error  = QMI_ERR_NO_MEMORY_V01;
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        goto bail;
      }
      memset(fltr_spec, 0, 
             sizeof(ip_filter_type)*(req_msg->filter_rules_len));
      memset(ipfltr_handle_arr, 0, 
             sizeof(ps_iface_ipfltr_handle_type)*(req_msg->filter_rules_len));
      /*fill filter spec with values from request struct*/
      while(cnt < req_msg->filter_rules_len)
      {
        fltr_spec[cnt].ip_vsn = req_msg->filter_rules[cnt].ip_info.ip_version;
        if(req_msg->filter_rules[cnt].ip_info.ip_version == 
                                       DFS_IP_FAMILY_IPV6_V01)
        {
          qmi_dfsi_fill_v6_filter_spec(&(req_msg->filter_rules[cnt]),
                                       &fltr_spec[cnt]); 
        }
        else if(req_msg->filter_rules[cnt].ip_info.ip_version == 
                                       DFS_IP_FAMILY_IPV4_V01)
        {
          qmi_dfsi_fill_v4_filter_spec(&(req_msg->filter_rules[cnt]),
                                       &fltr_spec[cnt]); 
        }
        cnt++;
      }
      filter_param.fi_ptr_arr         = fltr_spec;
      filter_param.num_filters        = (uint8)req_msg->filter_rules_len;
      filter_param.enable             = filter_mode;
      filter_param.filter_type        = IPFLTR_DEFAULT_TYPE;
      filter_param.is_validated       = FALSE;
      filter_param.fltr_priority      = PS_IFACE_IPFLTR_PRIORITY_DEFAULT;
      filter_param.fltr_compare_f_ptr = NULL;
      filter_param.fi_result          = PS_IFACE_IPFLTR_DELETE_ALL_HANDLE;
      ps_errno = 0;
      /* call PS_IFACE API to add filters - 
	     ipfltr_handle_arr contains handles for all filters added*/
      if(!ps_iface_ipfltr_add_ex(rm_iface_ptr,IP_FLTR_CLIENT_POWERSAVE,
                &filter_param,ipfltr_handle_arr,&ps_errno))
      {
        LOG_MSG_INFO1_0("ps_iface_ipfltr_add_ex success");
        resp_msg.filter_handles_valid = TRUE;
        resp_msg.filter_handles_len = req_msg->filter_rules_len;
        memscpy(&(resp_msg.filter_handles),
			    sizeof(uint32)*(req_msg->filter_rules_len),
			    ipfltr_handle_arr,
			    sizeof(uint32)*(req_msg->filter_rules_len));
        resp_msg.resp.error = QMI_ERR_NONE_V01;
        resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
      }
      else
      {
        cnt = 0;
        LOG_MSG_ERROR_1("ps_iface_ipfltr_add_ex failed with ps_errno (%d)",
                                                                ps_errno);
        /*if ps_errno is EINVAL,we need to fill error mask and report in response*/
        if(ps_errno == DS_EINVAL)
        {
          resp_msg.filter_rule_error_valid = TRUE;
          resp_msg.filter_rule_error_len = req_msg->filter_rules_len;
          while(cnt < req_msg->filter_rules_len)
          {
            qmi_dfsi_fill_filter_error_mask(&(fltr_spec[cnt]),
                                            &(resp_msg.filter_rule_error[cnt]));
            cnt++;
          }
          resp_msg.resp.error  = QMI_ERR_INVALID_ARG_V01;
          resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        }
        else
        {
          resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
          resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        }
      }		      
    }
  }
  else
  {
    resp_msg.resp.error = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
  
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_add_powersave_filters_resp_msg_v01));
  if(fltr_spec != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(fltr_spec);
  }
  if(ipfltr_handle_arr != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(ipfltr_handle_arr);
  }  
  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_DFSI_SET_POWERSAVE_FILTER_MODE()

  DESCRIPTION
    To Set powersave filter mode.When powersave filter mode is on DL packets 
    are filtered based filters applied in qmi_dfsi_add_powersave_filters.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle
    msg_id      : Message ID
    req_c_struct_len : length of request message
    service_handle :DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_set_powersave_filter_mode
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations ----------------------------*/
  dfs_set_powersave_filter_mode_req_msg_v01* req_msg;  
  dfs_set_powersave_filter_mode_resp_msg_v01 resp_msg;
  ps_iface_type* rm_iface_ptr = NULL;
  int retval = -1;
  sint15 ps_errno;
  rmnet_instance_e_type     rmnet_instance = RMNET_INSTANCE_MAX;
  /*-----------------------------------------------------*/
  ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_set_powersave_filter_mode_resp_msg_v01));
  req_msg = (dfs_set_powersave_filter_mode_req_msg_v01*)req_c_struct;
  QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
  	                                      clnt_info->bound_ip_type,
  	                                      clnt_info->ep_id,
                                              clnt_info->mux_id,
                                              rmnet_instance);
  QMI_DFSI_IS_CALL_UP(rmnet_instance);   
  QMI_DFSI_GET_RM_IFACE_PTR(rmnet_instance);
  /* if passed value is not enable or disable filter then return error */
  if(req_msg->powersave_filter_mode != TRUE && 
  	          req_msg->powersave_filter_mode != FALSE)
  {
    resp_msg.resp.error  = QMI_ERR_INVALID_ARG_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }

  retval = ps_iface_set_powersave_filtering_mode(rm_iface_ptr, 
                                                 req_msg->powersave_filter_mode,
                          &ps_errno);
  if(retval != 0)
  {
    LOG_MSG_ERROR_1("ps_iface_set_powersave_filtering_mode failed (%d)",retval);
    resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  }
  else
  {
    resp_msg.resp.error = QMI_ERR_NONE_V01;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;;
  }	
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_set_powersave_filter_mode_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DFSI_GET_POWERSAVE_FILTER_MODE()

  DESCRIPTION
    This function gets current powersave filter mode.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle
    msg_id	  : Message ID
    req_c_struct_len : length of request message
    service_handle :DFS service handle

  RETURN VALUE
    qmi_error_e_type types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_get_powersave_filter_mode
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations --------------------*/
  dfs_get_powersave_filter_mode_resp_msg_v01 resp_msg;
  ps_iface_type* rm_iface_ptr = NULL;
  int retval = -1;
  sint15 ps_errno;
  boolean filter_mode;
  rmnet_instance_e_type rmnet_instance = RMNET_INSTANCE_MAX;	
  /*--------------------------------------------*/
  ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_get_powersave_filter_mode_resp_msg_v01));
  QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
  	                                      clnt_info->bound_ip_type,
  	                                      clnt_info->ep_id,
  	                                      clnt_info->mux_id,
  	                                      rmnet_instance);
  QMI_DFSI_IS_CALL_UP(rmnet_instance);   
  QMI_DFSI_GET_RM_IFACE_PTR(rmnet_instance);
  retval = ps_iface_get_powersave_filtering_mode(rm_iface_ptr, 
                            &filter_mode,
                            &ps_errno);
  if(retval != 0)
  {
    LOG_MSG_ERROR_1("ps_iface_get_powersave_filtering_mode (%d)",retval);
    resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  }
  else
  {
    resp_msg.powersave_filter_mode_valid = TRUE;
    resp_msg.powersave_filter_mode = filter_mode;
    resp_msg.resp.error = QMI_ERR_NONE_V01;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;;
  }	
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_get_powersave_filter_mode_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_DFSI_SET_AUTOEXIT_POWERSAVE_FILTER_MODE()

  DESCRIPTION
    To Set autoexit powersave filter mode.When autoexit mode is on device 
    comes out of powersave filter mode after first packet matches with
    any of the filter.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    qmi_error_e_type types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_set_autoexit_powersave_filter_mode
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations -------------------------------------*/
  dfs_set_autoexit_powersave_filter_mode_req_msg_v01	*req_msg;  
  dfs_set_autoexit_powersave_filter_mode_resp_msg_v01	 resp_msg;
  ps_iface_type* rm_iface_ptr = NULL;
  int retval = -1;
  sint15 ps_errno;
  rmnet_instance_e_type rmnet_instance = RMNET_INSTANCE_MAX;
  /*-------------------------------------------------------------*/
  ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_set_autoexit_powersave_filter_mode_resp_msg_v01));
  req_msg = (dfs_set_autoexit_powersave_filter_mode_req_msg_v01*)req_c_struct;
  QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
  	                                      clnt_info->bound_ip_type,
  	                                      clnt_info->ep_id,
  	                                      clnt_info->mux_id,
  	                                      rmnet_instance);
  QMI_DFSI_IS_CALL_UP(rmnet_instance);
  QMI_DFSI_GET_RM_IFACE_PTR(rmnet_instance);
  /* if passed value is not enable or disable autoexit mode then return error */
  if(req_msg->autoexit_powersave_filter_mode != TRUE && 
     req_msg->autoexit_powersave_filter_mode != FALSE)
  {
    resp_msg.resp.error  = QMI_ERR_INVALID_ARG_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }

  retval = ps_iface_set_powersave_filtering_auto_exit_mode(rm_iface_ptr, 
                                req_msg->autoexit_powersave_filter_mode,
                          &ps_errno);
  if(retval != 0)
  {
    LOG_MSG_ERROR_1("ps_iface_set_powersave_filtering_mode failed (%d)",retval);
    resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  }
  else
  {
    resp_msg.resp.error = QMI_ERR_NONE_V01;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;;
  }	
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_set_autoexit_powersave_filter_mode_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DFSI_REMOVE_FILTERS()

  DESCRIPTION
    To remove powersave filters for passed handles .

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_remove_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations ----------------------------------------*/
  dfs_remove_filters_req_msg_v01     *req_msg;  
  dfs_remove_filters_resp_msg_v01     resp_msg;
  uint32 filter_count = 0;
  ps_iface_type* rm_iface_ptr = NULL;
  int16 ps_errno;
  rmnet_instance_e_type rmnet_instance = RMNET_INSTANCE_MAX;
  /*-----------------------------------------------------------------*/
  
  ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_remove_filters_resp_msg_v01));
  req_msg = (dfs_remove_filters_req_msg_v01*)req_c_struct;
  QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
  	                                      clnt_info->bound_ip_type,
  	                                      clnt_info->ep_id,
  	                                      clnt_info->mux_id,
  	                                      rmnet_instance);
  QMI_DFSI_IS_CALL_UP(rmnet_instance);
  QMI_DFSI_GET_RM_IFACE_PTR(rmnet_instance);
  
  if(req_msg->filter_handles_len >= QMI_DFS_MAX_FILTERS_V01)
  {
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }

  /* Validate all filter handles before passing to ps_iface*/
  while(filter_count < req_msg->filter_handles_len)
  {
    if(req_msg->filter_handles[filter_count] == PS_IFACE_IPFLTR_INVALID_HANDLE)
    {
      resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }
    filter_count++;
  }
  if(ps_iface_ipfltr_delete_ex(rm_iface_ptr,
                               IP_FLTR_CLIENT_POWERSAVE,
		               (ps_iface_ipfltr_handle_type*)
                               (&(req_msg->filter_handles)),
                               (uint16)req_msg->filter_handles_len,
                               &ps_errno) != 0)
  {
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_remove_filters_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DFSI_REMOVE_ALL_POWERSAVE_FILTERS()

  DESCRIPTION
    To remove all powersave filters.

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_remove_all_powersave_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations ----------------------------*/
  dfs_remove_all_powersave_filters_resp_msg_v01	 resp_msg;
  ps_iface_type* rm_iface_ptr = NULL;
  int16 ps_errno = 0;
  rmnet_instance_e_type rmnet_instance = RMNET_INSTANCE_MAX;
  /*----------------------------------------------------*/
  
  ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_remove_all_powersave_filters_resp_msg_v01));
  QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
  	                                      clnt_info->bound_ip_type,
  	                                      clnt_info->ep_id,
  	                                      clnt_info->mux_id,
  	                                      rmnet_instance);
  QMI_DFSI_IS_CALL_UP(rmnet_instance);
  QMI_DFSI_GET_RM_IFACE_PTR(rmnet_instance);
  /* To remove all powersave filters we need to pass 
     PS_IFACE_IPFLTR_GENERIC_HANDLE to ps_iface_ipfltr_delete*/
  if(!ps_iface_ipfltr_delete(rm_iface_ptr,IP_FLTR_CLIENT_POWERSAVE,
  	                      PS_IFACE_IPFLTR_DELETE_ALL_HANDLE,&ps_errno))
  {
    resp_msg.resp.error = QMI_ERR_NONE_V01;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  }
  else
  {
    LOG_MSG_INFO1_0("qmi_dfsi_remove_all_powersave_filters failed");
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  }
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_remove_all_powersave_filters_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DFSI_GET_REVERSE_IP_TRANSPORT_FILTERS()

  DESCRIPTION
    Query existing filters on reverse IP transport connection.

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_get_reverse_ip_transport_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations ----------------------------*/
  dfs_get_reverse_ip_transport_filters_resp_msg_v01	 * resp_msg;
  ps_iface_type* rev_ip_iface_ptr = NULL; /* Could be WLAN_LB or IWLAN_S2B */
  sint15 ps_errno = 0;
  ps_iface_ipfltr_list_type  ipfltr_list;
  int itr = 0;
  /*----------------------------------------------------*/
  
  ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);

  memset(&ipfltr_list, 0, sizeof(ps_iface_ipfltr_list_type));
  PS_SYSTEM_HEAP_MEM_ALLOC(resp_msg,
           sizeof(dfs_get_reverse_ip_transport_filters_resp_msg_v01),
           dfs_get_reverse_ip_transport_filters_resp_msg_v01*);
  if(resp_msg == NULL)
  {
    return QMI_CSI_CB_NO_MEM;
  }
  memset(resp_msg, 0, sizeof(dfs_get_reverse_ip_transport_filters_resp_msg_v01));

  if (rev_ip_transport_get_iface(clnt_info->ep_id, clnt_info->mux_id,
          clnt_info->bound_ip_type==DFS_IP_FAMILY_IPV6_V01 ) == NULL)
  {
    LOG_MSG_INFO1_0("out of call");
    resp_msg->resp.error = QMI_ERR_OUT_OF_CALL_V01;
    resp_msg->resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(ipfltr_list.filters.list_ptr,
                           sizeof(ip_filter_type) * DFSI_MAX_REV_IP_FLTRS,
                           ip_filter_type*);
  if(ipfltr_list.filters.list_ptr == NULL)
  {
    resp_msg->resp.error = QMI_ERR_NO_MEMORY_V01;
    resp_msg->resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }

  memset(ipfltr_list.filters.list_ptr, 0, sizeof(ip_filter_type) * DFSI_MAX_REV_IP_FLTRS);

  rev_ip_iface_ptr = rev_ip_transport_get_iface(clnt_info->ep_id, clnt_info->mux_id,
                          clnt_info->bound_ip_type==DFS_IP_FAMILY_IPV6_V01 );

  LOG_MSG_INFO1_1("rev_iface_ptr = 0x%x", rev_ip_iface_ptr);

  /* Pass in number of filters for which memory has been allocated */
  ipfltr_list.list_ptr_size = DFSI_MAX_REV_IP_FLTRS;

  /* Retrieve the filters on reverse iface */
  if (ps_iface_ioctl(rev_ip_iface_ptr,
                     PS_IFACE_IOCTL_REVERSE_IP_TRANSPORT_GET_IPFLTRS,
                     &ipfltr_list,
                     &ps_errno) != 0)
  {
    LOG_MSG_ERROR_1("Couldn't get IPfltrs, IOCTL gave errno %d", ps_errno);
    resp_msg->resp.error = QMI_ERR_INTERNAL_V01;
    resp_msg->resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }

  for(itr=0;itr<MIN(ipfltr_list.filters.num_filters,DFSI_MAX_REV_IP_FLTRS);itr++)
  {
    qmi_dfsi_convert_ps_ipfltr_to_dfs_type(&ipfltr_list.filters.list_ptr[itr],
                                           &resp_msg->filter_rules[itr]);
  }
  resp_msg->filter_rules_valid = TRUE;
  resp_msg->filter_rules_len = MIN(ipfltr_list.filters.num_filters,DFSI_MAX_REV_IP_FLTRS);
  resp_msg->resp.error = QMI_ERR_NONE_V01;
  resp_msg->resp.result = QMI_RESULT_SUCCESS_V01;

bail:
  qmi_csi_send_resp(req_handle, msg_id, resp_msg, sizeof(dfs_get_reverse_ip_transport_filters_resp_msg_v01));
  if(resp_msg != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(resp_msg);
  }
  if(ipfltr_list.filters.list_ptr != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(ipfltr_list.filters.list_ptr);
  }
  return QMI_CSI_CB_NO_ERR;
} /* qmi_dfsi_get_reverse_ip_transport_filters */

/*===========================================================================
  FUNCTION QMI_DFSI_REMOTE_SOCKET_ALLOCATED()

  DESCRIPTION
    Allocated socket information sent as per previous
    QMI_DFS_REMOTE_SOCKET_REQUEST_IND.

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_remote_socket_allocated
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations ----------------------------*/
  dfs_remote_socket_allocated_req_msg_v01     * req_msg;
  dfs_remote_socket_allocated_resp_msg_v01	  resp_msg;
  ps_sys_remote_socket_allocated_type  * allocated_sockets;
  uint8 itr = 0;
  int16    ps_result = 0;
  int16    ps_errno = 0;
  qmi_csi_error rc;
  /*----------------------------------------------------*/
  
  ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);

  req_msg = (dfs_remote_socket_allocated_req_msg_v01*)req_c_struct;
  memset(&resp_msg, 0, sizeof(dfs_remote_socket_allocated_resp_msg_v01));

  PS_SYSTEM_HEAP_MEM_ALLOC(allocated_sockets,
                           sizeof(ps_sys_remote_socket_allocated_type),
                           ps_sys_remote_socket_allocated_type*);
  if(allocated_sockets == NULL)
  {
    return QMI_CSI_CB_NO_MEM;
  }
  memset(allocated_sockets, 0, sizeof(ps_sys_remote_socket_allocated_type));
   
  LOG_MSG_INFO1_1("num sockets = %d", req_msg->socket_list_len);
  allocated_sockets->num_sockets = req_msg->socket_list_len;
  for (itr=0;itr<req_msg->socket_list_len;itr++)
  {
    allocated_sockets->socket_list[itr].status = req_msg->socket_list[itr].status;
    allocated_sockets->socket_list[itr].remote_socket_handle = 
      req_msg->socket_list[itr].socket_handle;
    allocated_sockets->socket_list[itr].is_ephemeral_port = 
      req_msg->socket_list[itr].is_ephemeral;
    allocated_sockets->socket_list[itr].socket_info.ip_family = 
      req_msg->socket_list[itr].socket_info.ip_family;
    allocated_sockets->socket_list[itr].socket_info.xport_proto = 
      req_msg->socket_list[itr].socket_info.xport_prot;
    allocated_sockets->socket_list[itr].socket_info.port_no = 
      ps_htons(req_msg->socket_list[itr].socket_info.port_no);
  }


  /* Pass down the info */
  ps_result = ps_sys_ioctl_ex(PS_SYS_TECH_ALL,
                              PS_SYS_IOCTL_REMOTE_SOCKET_ALLOCATED,
                              PS_SYS_PRIMARY_SUBS, //Subscription agnostic, use primary
                              allocated_sockets,
                              &ps_errno);
  if ( ps_result != 0)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
  }
  else
  {
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    resp_msg.resp.error = QMI_ERR_NONE_V01;
  }

  rc = qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(dfs_remote_socket_allocated_resp_msg_v01));
  LOG_MSG_INFO1_1("rc = %d", rc);
  PS_SYSTEM_HEAP_MEM_FREE(allocated_sockets);
  return QMI_CSI_CB_NO_ERR;
} /* qmi_dfsi_remote_socket_allocated */


/*===========================================================================
  FUNCTION QMI_DFSI_INDICATION_REGISTER()

  DESCRIPTION
    To Register for dfsi indications.
    report_powersave_filter_ind flag will be updated 
    in the client info based on input.

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_indication_register
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations --------------------*/
  dfs_indication_register_req_msg_v01   *req_msg;  
  dfs_indication_register_resp_msg_v01   resp_msg;
  boolean                                is_empty_req = TRUE;
  ps_sys_remote_socket_hdlr_avail_type   remote_socket_hdlr_avail;
  int16                                  ps_errno = 0;
  /*--------------------------------------------------------*/
  ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_indication_register_resp_msg_v01));
  memset(&remote_socket_hdlr_avail,0,sizeof(ps_sys_remote_socket_hdlr_avail_type));

  req_msg = (dfs_indication_register_req_msg_v01*)req_c_struct;
  if(req_msg->report_powersave_filter_mode_change_valid)
  {
    is_empty_req = FALSE;
    if(req_msg->report_powersave_filter_mode_change > 0)
    {
      clnt_info->report_powersave_filter_ind = TRUE;
    }
    else
    {
      clnt_info->report_powersave_filter_ind = FALSE;
    }
  }
  if(req_msg->report_reverse_ip_transport_filters_update_valid)
  {
    is_empty_req = FALSE;
    if(req_msg->report_reverse_ip_transport_filters_update > 0)
    {
      clnt_info->report_rev_ip_transport_filters = TRUE;
    }
    else
    {
      clnt_info->report_rev_ip_transport_filters = FALSE;
    }
  }

  if(req_msg->report_low_latency_traffic_valid)
  {
    is_empty_req = FALSE;
    
    if(req_msg->report_low_latency_traffic > 0)
    {
      clnt_info->report_low_latency_traffic_status = TRUE;
    }
    else
    {
      clnt_info->report_low_latency_traffic_status = FALSE;
    }
  }

  if(req_msg->remote_socket_handling_valid)
  {
    is_empty_req = FALSE;
    
    if(req_msg->remote_socket_handling > 0)
    {
      clnt_info->remote_socket_handling = TRUE;
      remote_socket_hdlr_avail.remote_socket_hdlr_avail = TRUE;
    }
    else
    {
      clnt_info->remote_socket_handling = FALSE;
      remote_socket_hdlr_avail.remote_socket_hdlr_avail = FALSE;
    }

    if (0 > ps_sys_conf_set_ex(PS_SYS_TECH_ALL,
                       PS_SYS_CONF_REMOTE_SOCKET_HDLR_AVAIL,
                       PS_SYS_PRIMARY_SUBS, //Subscription agnostic, but use primary for consistency
                       (void *)&remote_socket_hdlr_avail,
                       &ps_errno))
    {
      LOG_MSG_ERROR_1("ps_sys_conf_set error %d", ps_errno);
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
      goto bail;    
    }
  }

  if( !is_empty_req )
  {
    resp_msg.resp.error = QMI_ERR_NONE_V01;
  }
  else
  {
    resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
  }

bail:
  if (resp_msg.resp.error == QMI_ERR_NONE_V01)
  {
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  }
  else
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_indication_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DFSI_GENERATE_LOW_LATENCY_TRAFFIC_STATUS_INDICATION()

  DESCRIPTION
    To generte low latency traffic status indication when its status changes.

  PARAMETERS
    filter_mode  : filter mode indications
    clnt_info    : Client Info Handle

  RETURN VALUE
    void.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_generate_low_latency_traffic_status_indication
(
  qmi_dfsi_cmd_buf_type*             cmd_ptr

)
{
  /*--------------declarations ---------------------------------*/
  qmi_dfsi_client_info_type              * clnt_info = NULL;
  dfs_low_latency_traffic_ind_msg_v01    * indication;
  qmi_csi_error                            rc;
  ps_sys_low_latency_traffic_info_type   * low_latency_traffic_info_ptr = NULL;
  /*--------------------------------------------------------------*/
  ASSERT( cmd_ptr );

  /* Convert PS ipfltr structure to DFS indication type */
  PS_SYSTEM_HEAP_MEM_ALLOC(indication, 
         sizeof(dfs_low_latency_traffic_ind_msg_v01),
         dfs_low_latency_traffic_ind_msg_v01*);
  if( NULL == indication )
  {
    goto bail;
  }

  low_latency_traffic_info_ptr = 
    &(cmd_ptr->data.low_latency_traffic_info.low_latency_traffic_status);
  
  indication->traffic_start = 
    (uint8) low_latency_traffic_info_ptr->traffic_state;
  indication->filter_handle = low_latency_traffic_info_ptr->filter_handle;

  clnt_info = 
    (qmi_dfsi_client_info_type*)q_check(&qmi_dfs_state.client_q);

  while( clnt_info != NULL )
  {
    if( clnt_info->report_low_latency_traffic_status)
    {
      rc = qmi_csi_send_ind( clnt_info->clnt, 
                             QMI_DFS_LOW_LATENCY_TRAFFIC_IND_V01,
                             (void*)indication, 
                             sizeof(dfs_low_latency_traffic_ind_msg_v01));
      
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR_1("qmi_dfsi_generate_filter_mode_indication error %d",
                          rc);
        goto bail;
      }
    }
    clnt_info = q_next( &qmi_dfs_state.client_q,
                        &(clnt_info->link) );
  }

bail:

  if(indication != NULL)
  { 
    PS_SYSTEM_HEAP_MEM_FREE(indication);
  }
}

/*===========================================================================
  FUNCTION QMI_DFSI_GENERATE_REV_IP_FLTR_CHANGED_IND()

  DESCRIPTION
    Generate rev IP filter changed indication for all clients that are
    registered for it

  PARAMETERS
    cmd_ptr    : command pointer from event callback

  RETURN VALUE
    void.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_generate_rev_ip_fltr_changed_ind
(
  qmi_dfsi_cmd_buf_type*             cmd_ptr
)
{
  /*--------------declarations ---------------------------------*/
  dfs_reverse_ip_transport_filters_updated_ind_msg_v01 * ind = NULL;
  qmi_csi_error                         rc;
  ps_iface_type*                        rev_ip_iface_ptr = NULL;
  qmi_dfsi_client_info_type*            client_info_ptr = NULL;
  uint32                                itr=0;
  /*----------------------------------------------------------*/
  ASSERT(cmd_ptr && cmd_ptr->data.rev_ip_fltr_change.iface_ptr);

  /* Convert PS ipfltr structure to DFS indication type */
  PS_SYSTEM_HEAP_MEM_ALLOC(ind, 
         sizeof(dfs_reverse_ip_transport_filters_updated_ind_msg_v01),
         dfs_reverse_ip_transport_filters_updated_ind_msg_v01*);
  if (ind == NULL) 
  {
    goto bail;
  }

  memset(ind, 0, sizeof(dfs_reverse_ip_transport_filters_updated_ind_msg_v01));

  ind->filter_action = (dfs_reverse_ip_transport_filters_action_enum_v01)
                        cmd_ptr->data.rev_ip_fltr_change.fltr_info.status;
  ind->filter_rules_len = cmd_ptr->data.rev_ip_fltr_change.fltr_info.filters.num_filters;
  ind->filter_rules_valid = TRUE;
  for(itr=0;itr<ind->filter_rules_len;itr++)
  {
    qmi_dfsi_convert_ps_ipfltr_to_dfs_type(
       &cmd_ptr->data.rev_ip_fltr_change.fltr_info.filters.list_ptr[itr],
       &ind->filter_rules[itr]);
  }

  /* Traverse entire Queue to get the client info ptr matching with iface_ptr 
     in the event callback  Report to all clients who are using the same iface_ptr*/

  client_info_ptr = (qmi_dfsi_client_info_type*)q_check(&qmi_dfs_state.client_q);
  while(client_info_ptr != NULL)
  {
    if (client_info_ptr->report_rev_ip_transport_filters == TRUE)
    {
      rev_ip_iface_ptr = rev_ip_transport_get_iface(client_info_ptr->ep_id,
                                                    client_info_ptr->mux_id,
                            client_info_ptr->bound_ip_type == DFS_IP_FAMILY_IPV6_V01);
      if(cmd_ptr->data.rev_ip_fltr_change.iface_ptr == rev_ip_iface_ptr)
      {

        rc = qmi_csi_send_ind(client_info_ptr->clnt, 
                          QMI_DFS_REVERSE_IP_TRANSPORT_FILTERS_UPDATED_IND_V01,
                          (void*)ind, 
                          sizeof(dfs_reverse_ip_transport_filters_updated_ind_msg_v01));
        LOG_MSG_INFO1_1("sent indication client_info_ptr = %0x",client_info_ptr);
        if ( rc != QMI_CSI_NO_ERR )
        {
          LOG_MSG_ERROR_1("qmi_dfsi_generate_rev_ip_fltr_changed_ind error %d", rc);
          break;		
        }
      }
    }
    client_info_ptr = q_next(&qmi_dfs_state.client_q,
                           &(client_info_ptr->link));
  }
bail:
  /* Free memory allocated for the ipfltr list */
  if(cmd_ptr->data.rev_ip_fltr_change.fltr_info.filters.list_ptr != NULL)
  { 
    PS_SYSTEM_HEAP_MEM_FREE(cmd_ptr->data.rev_ip_fltr_change.fltr_info.filters.list_ptr);
  }
  if(ind != NULL)
  { 
    PS_SYSTEM_HEAP_MEM_FREE(ind);
  }
} /* qmi_dfsi_generate_rev_ip_fltr_changed_ind */

/*===========================================================================
  FUNCTION QMI_DFSI_GENERATE_REMOTE_SOCKET_REQUEST_IND()

  DESCRIPTION
    Generate remote socket request indication for all clients that are
    registered for remote socket handling

  PARAMETERS
    cmd_ptr    : command pointer from event callback

  RETURN VALUE
    void.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_generate_remote_socket_request_indication
(
  qmi_dfsi_cmd_buf_type*             cmd_ptr
)
{
  /*--------------declarations ---------------------------------*/
  dfs_remote_socket_request_ind_msg_v01 * ind = NULL;
  qmi_csi_error                         rc;
  qmi_dfsi_client_info_type*            client_info_ptr = NULL;
  uint8                                 itr=0;
  ps_sys_remote_socket_request_type     * socket_request = (ps_sys_remote_socket_request_type*)&cmd_ptr->data.socket_request;
  /*----------------------------------------------------------*/
  ASSERT(cmd_ptr);

  PS_SYSTEM_HEAP_MEM_ALLOC(ind, 
         sizeof(dfs_remote_socket_request_ind_msg_v01),
         dfs_remote_socket_request_ind_msg_v01*);
  if (ind == NULL) 
  {
    goto bail;
  }

  memset(ind, 0, sizeof(dfs_remote_socket_request_ind_msg_v01));

  ind->request_socket_list_len = socket_request->num_sockets;

  for (itr=0;itr<socket_request->num_sockets;itr++)
  {
    ind->request_socket_list[itr].ip_family = 
      (dfs_ip_family_enum_v01)socket_request->socket_list[itr].ip_family;
    ind->request_socket_list[itr].xport_prot = 
      (dfs_xport_protocol_enum_v01)socket_request->socket_list[itr].xport_proto;
    ind->request_socket_list[itr].port_no =
      ps_ntohs(socket_request->socket_list[itr].port_no);
  }

  /* Traverse queue of clients to report to the registered ones */

  client_info_ptr = (qmi_dfsi_client_info_type*)q_check(&qmi_dfs_state.client_q);
  while(client_info_ptr != NULL)
  {
    if (client_info_ptr->remote_socket_handling == TRUE)
    {
      rc = qmi_csi_send_ind(client_info_ptr->clnt, 
                          QMI_DFS_REMOTE_SOCKET_REQUEST_IND_V01,
                          (void*)ind, 
                          sizeof(dfs_remote_socket_request_ind_msg_v01));
      LOG_MSG_INFO1_1("sent indication client_info_ptr = %0x",client_info_ptr);
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR_1("qmi_dfsi_generate_remote_socket_request_indication error %d", rc);
        break;		
      }
    }
    client_info_ptr = q_next(&qmi_dfs_state.client_q,
                           &(client_info_ptr->link));
  }
bail:
  if(ind != NULL)
  { 
    PS_SYSTEM_HEAP_MEM_FREE(ind);
  }
} /* qmi_dfsi_generate_remote_socket_request_indication */

/*===========================================================================
  FUNCTION QMI_DFSI_GENERATE_REMOTE_SOCKET_RELEASE_IND()

  DESCRIPTION
    Generate remote socket release indication for all clients that are
    registered for remote socket handling

  PARAMETERS
    cmd_ptr    : command pointer from event callback

  RETURN VALUE
    void.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_generate_remote_socket_release_indication
(
  qmi_dfsi_cmd_buf_type*             cmd_ptr
)
{
  /*--------------declarations ---------------------------------*/
  dfs_remote_socket_release_ind_msg_v01 * ind = NULL;
  qmi_csi_error                         rc;
  qmi_dfsi_client_info_type*            client_info_ptr = NULL;
  ps_sys_remote_socket_release_type     * socket_release = (ps_sys_remote_socket_release_type*)&cmd_ptr->data.socket_release;
  /*----------------------------------------------------------*/
  ASSERT(cmd_ptr);

  PS_SYSTEM_HEAP_MEM_ALLOC(ind, 
         sizeof(dfs_remote_socket_release_ind_msg_v01),
         dfs_remote_socket_release_ind_msg_v01*);
  if (ind == NULL) 
  {
    goto bail;
  }

  memset(ind, 0, sizeof(dfs_remote_socket_release_ind_msg_v01));

  ind->socket_handles_valid = TRUE;
  ind->socket_handles_len = socket_release->num_sockets;

  /* Copy array of uint32s over */
  memscpy(ind->socket_handles, sizeof(ind->socket_handles),
          socket_release->socket_list, socket_release->num_sockets * sizeof(uint32));

  /* Traverse queue of clients to report to the registered ones */

  client_info_ptr = (qmi_dfsi_client_info_type*)q_check(&qmi_dfs_state.client_q);
  while(client_info_ptr != NULL)
  {
    if (client_info_ptr->remote_socket_handling == TRUE)
    {
      rc = qmi_csi_send_ind(client_info_ptr->clnt, 
                          QMI_DFS_REMOTE_SOCKET_RELEASE_IND_V01,
                          (void*)ind, 
                          sizeof(dfs_remote_socket_release_ind_msg_v01));
      LOG_MSG_INFO1_1("sent indication client_info_ptr = %0x",client_info_ptr);
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR_1("qmi_dfsi_generate_remote_socket_release_indication error %d", rc);
        break;		
      }
    }
    client_info_ptr = q_next(&qmi_dfs_state.client_q,
                           &(client_info_ptr->link));
  }
bail:
  if(ind != NULL)
  { 
    PS_SYSTEM_HEAP_MEM_FREE(ind);
  }
} /* qmi_dfsi_generate_remote_socket_release_indication */

/*===========================================================================
  FUNCTION QMI_DFSI_GENERATE_REMOTE_SOCKET_SET_OPTION_IND()

  DESCRIPTION
    Generate remote socket set option indication for all clients that are
    registered for remote socket handling

  PARAMETERS
    cmd_ptr    : command pointer from event callback

  RETURN VALUE
    void.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_generate_remote_socket_set_option_indication
(
  qmi_dfsi_cmd_buf_type*             cmd_ptr
)
{
  /*--------------declarations ---------------------------------*/
  dfs_remote_socket_set_option_ind_msg_v01 * ind = NULL;
  qmi_csi_error                         rc;
  qmi_dfsi_client_info_type*            client_info_ptr = NULL;
  ps_sys_remote_socket_set_option_type  * socket_option = (ps_sys_remote_socket_set_option_type*)&cmd_ptr->data.socket_option;
  /*----------------------------------------------------------*/
  ASSERT(cmd_ptr);

  PS_SYSTEM_HEAP_MEM_ALLOC(ind, 
         sizeof(dfs_remote_socket_set_option_ind_msg_v01),
         dfs_remote_socket_set_option_ind_msg_v01*);
  if (ind == NULL) 
  {
    goto bail;
  }

  memset(ind, 0, sizeof(dfs_remote_socket_set_option_ind_msg_v01));

  ind->socket_handle = socket_option->socket_handle;
  ind->is_udp_encaps_valid = TRUE; //Always there for now
  ind->is_udp_encaps = socket_option->is_udp_encaps;

  /* Traverse queue of clients to report to the registered ones */

  client_info_ptr = (qmi_dfsi_client_info_type*)q_check(&qmi_dfs_state.client_q);
  while(client_info_ptr != NULL)
  {
    if (client_info_ptr->remote_socket_handling == TRUE)
    {
      rc = qmi_csi_send_ind(client_info_ptr->clnt, 
                          QMI_DFS_REMOTE_SOCKET_SET_OPTION_IND_V01,
                          (void*)ind, 
                          sizeof(dfs_remote_socket_set_option_ind_msg_v01));
      LOG_MSG_INFO1_1("sent indication client_info_ptr = %0x",client_info_ptr);
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR_1("qmi_dfsi_generate_remote_socket_set_option_indication error %d", rc);
        break;		
      }
    }
    client_info_ptr = q_next(&qmi_dfs_state.client_q,
                           &(client_info_ptr->link));
  }
bail:
  if(ind != NULL)
  { 
    PS_SYSTEM_HEAP_MEM_FREE(ind);
  }
} /* qmi_dfsi_generate_remote_socket_set_option_indication */

/*===========================================================================
  FUNCTION QMI_DFSI_GENERATE_FILTER_MODE_INDICATION()

  DESCRIPTION
    To generte filter mode indication when powersave filter mode changes.

  PARAMETERS
    cmd_ptr    : command pointer from event callback

  RETURN VALUE
    void.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_generate_filter_mode_indication
(
  qmi_dfsi_cmd_buf_type*             cmd_ptr
)
{
  /*--------------declarations ---------------------------------*/
  dfs_powersave_filter_mode_ind_msg_v01 indication;
  qmi_csi_error                         rc;
  ps_iface_type*                        rm_iface_ptr = NULL;
  qmi_dfsi_client_info_type*            client_info_ptr = NULL;
  rmnet_instance_e_type                 rmnet_instance = RMNET_INSTANCE_MAX;
  /*----------------------------------------------------------*/
  ASSERT(cmd_ptr && cmd_ptr->data.pws_state_data.iface_ptr);

  /* Traverse entire Queue to get the client info ptr matching with iface_ptr 
       in the event callback  Report to all clients who is using the same iface_ptr*/
  client_info_ptr = (qmi_dfsi_client_info_type*)q_check(&qmi_dfs_state.client_q);
  while(client_info_ptr != NULL)
  {
    if(client_info_ptr->report_powersave_filter_ind != TRUE)
    {
      client_info_ptr = q_next(&qmi_dfs_state.client_q,
                           &(client_info_ptr->link));
      continue;
    }
    if (client_info_ptr->bound_ip_type == DFS_IP_FAMILY_IPV6_V01)
    {
      rmnet_instance = rmnet_inst_from_ep_and_mux_id(client_info_ptr->ep_id,
	  	                                     client_info_ptr->mux_id, 0);
    }
    else
    {
      rmnet_instance = rmnet_inst_from_ep_and_mux_id(client_info_ptr->ep_id, 
                                                     client_info_ptr->mux_id, 1);
    }
    if(rmnet_instance >= RMNET_INSTANCE_MAX)
    {
      LOG_MSG_ERROR_0("Invalid instance - Ignored indication");
      client_info_ptr = q_next(&qmi_dfs_state.client_q,
                               &(client_info_ptr->link));
      continue;
    }
    rm_iface_ptr = rmnet_meta_sm_iface(rmnet_instance);
    if((rm_iface_ptr != NULL) && 
	   (cmd_ptr->data.pws_state_data.iface_ptr == rm_iface_ptr))
    {
      indication.powersave_filter_mode = 
           cmd_ptr->data.pws_state_data.powersave_filter_state.powersave_filter_mode;
      rc = qmi_csi_send_ind(client_info_ptr->clnt, 
                        QMI_DFS_POWERSAVE_FILTER_MODE_IND_V01,
                        (void*)&indication, 
                        sizeof(dfs_powersave_filter_mode_ind_msg_v01));
      LOG_MSG_INFO1_1("sent indication client_info_ptr = %0x",client_info_ptr);
      if ( rc != QMI_CSI_NO_ERR )
      {
        LOG_MSG_ERROR_1("qmi_dfsi_generate_filter_mode_indication error %d", rc);
        break;		
      }
    }
    client_info_ptr = q_next(&qmi_dfs_state.client_q,
                           &(client_info_ptr->link));
  }
}

/*===========================================================================
  FUNCTION DFSI_COPY_IPFLTR_LIST_FROM_EVENT()

  DESCRIPTION
    Does a deep copy of IPFLTR list information from event to cmd buf.

  PARAMETERS
    event_info : Event information payload
    cmd_ptr    : cmd buf to which info is to be copied

  RETURN VALUE
    TRUE - SUCCESS
    FALSE - FAILURE

  DEPENDENCIES
    None.

  SIDE EFFECTS
    None
===========================================================================*/

static boolean dfsi_copy_ipfltr_list_from_event
(
  ps_iface_event_info_u_type * event_info,
  qmi_dfsi_cmd_buf_type      * cmd_ptr
)
{
  uint8           num_filters;
  ip_filter_type * filter_list = NULL;
  uint8            itr;

  num_filters = event_info->fltr_change_info.filters.num_filters;
  if(num_filters > 0)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(filter_list, 
                    sizeof(ip_filter_type) * num_filters,
                    ip_filter_type*);
    if(filter_list == NULL)
    {
      return FALSE;
    }

    for(itr=0;itr<num_filters;itr++)
    {
      filter_list[itr] = event_info->fltr_change_info.filters.list_ptr[itr];
    }
  }
  cmd_ptr->data.rev_ip_fltr_change.fltr_info.filters.num_filters = num_filters;
  cmd_ptr->data.rev_ip_fltr_change.fltr_info.filters.list_ptr = filter_list;
  cmd_ptr->data.rev_ip_fltr_change.fltr_info.status = 
    event_info->fltr_change_info.status;

  LOG_MSG_INFO2_2("num_filters = %d, status = %d", num_filters, event_info->fltr_change_info.status);

  return TRUE;
}

/*===========================================================================
  FUNCTION QMI_DFSI_IFACE_EV_CB()

  DESCRIPTION
	This callback function is called when one of the registered ps_iface events
	occur on RmNet interface.
  
  DEPENDENCIES
	None
  
  RETURN VALUE
	None
  
  SIDE EFFECTS
	None
  ===========================================================================*/

static void qmi_dfsi_iface_ev_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{
  qmi_dfsi_cmd_buf_type* cmd_ptr = NULL;
  ASSERT( this_iface_ptr );
  LOG_MSG_INFO1_1("qmi_dfsi_iface_ev_cb event = %d",event);
  switch( event )
  {
    case IFACE_POWERSAVE_FILTERING_MODE_CHANGE_EV:
    {
      cmd_ptr = (qmi_dfsi_cmd_buf_type*)qmi_dfsi_get_cmd_buf(QMI_DFSI_POWERSAVE_FILTER_STATE_CHANGE_EVT);
      if(cmd_ptr != NULL)
      {
        cmd_ptr->data.pws_state_data.iface_ptr = this_iface_ptr;
        cmd_ptr->cmd_id = QMI_DFSI_POWERSAVE_FILTER_STATE_CHANGE_EVT;
        cmd_ptr->data.pws_state_data.powersave_filter_state.powersave_filter_mode = 
          	               event_info.powersave_filtering_info.powersave_mode;
      }
      break;
    }
    case IFACE_REVERSE_IP_TRANSPORT_IPFLTR_CHANGED_EV:
      {
      cmd_ptr = (qmi_dfsi_cmd_buf_type*)qmi_dfsi_get_cmd_buf
        (QMI_DFSI_CMD_REVERSE_IP_TRANSPORT_IPFLTR_CHANGED);
      if(cmd_ptr != NULL)
      {
        cmd_ptr->data.rev_ip_fltr_change.iface_ptr = this_iface_ptr;
        cmd_ptr->cmd_id = QMI_DFSI_CMD_REVERSE_IP_TRANSPORT_IPFLTR_CHANGED;
        if ( dfsi_copy_ipfltr_list_from_event(&event_info, cmd_ptr) != TRUE )
        {
          LOG_MSG_ERROR_0("Unable to copy IPFLTR list");
          PS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
          return;
      }
    }
      break;
  }
    default:
      LOG_MSG_ERROR_1("Unhandled IFACE ev %d", event);
      return;
  }
  dcc_send_cmd_ex (DCC_DFS_SVC_CMD, cmd_ptr);
}

/*===========================================================================
  FUNCTION QMI_DFS_LOW_LATENCY_TRAFFIC_STATUS_EV_CB()

  DESCRIPTION
    This callback function is called when low latency traffic status event 
    occur.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
  ===========================================================================*/

static void qmi_dfs_low_latency_traffic_status_ev_cb
(
  ps_sys_tech_enum_type                  tech_type,
  ps_sys_event_enum_type                 event_name,
  void                                   * event_info_ptr,
  void                                   * user_data_ptr
)
{
  qmi_dfsi_cmd_buf_type                  * cmd_ptr = NULL;
  ps_sys_low_latency_traffic_info_type   * low_latency_traffic_info = NULL;
  ps_sys_low_latency_traffic_info_type   * low_latency_traffic_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT( event_info_ptr );
 
  low_latency_traffic_info = 
    (ps_sys_low_latency_traffic_info_type *)event_info_ptr;

  if( PS_SYS_LOW_LATENCY_TRAFFIC_STATE_CONTINUE == 
        low_latency_traffic_info->traffic_state )
  {
    return;
  }
  
  cmd_ptr = (qmi_dfsi_cmd_buf_type *)
              qmi_dfsi_get_cmd_buf(QMI_DFSI_LOW_LATENCY_TRAFFIC_STATUS_EVT);

  if( cmd_ptr == NULL)
  {
    return;
  }

  memset( cmd_ptr, 0x0, sizeof(qmi_dfsi_cmd_buf_type));
  

  low_latency_traffic_info_ptr = 
    &(cmd_ptr->data.low_latency_traffic_info.low_latency_traffic_status);

  low_latency_traffic_info_ptr->traffic_state = 
    low_latency_traffic_info->traffic_state;
  low_latency_traffic_info_ptr->filter_handle    =
    low_latency_traffic_info->filter_handle;

  cmd_ptr->cmd_id = QMI_DFSI_LOW_LATENCY_TRAFFIC_STATUS_EVT;

  dcc_send_cmd_ex (DCC_DFS_SVC_CMD, cmd_ptr);
}

/*===========================================================================
  FUNCTION QMI_DFS_REMOTE_SOCKET_EV_CB()

  DESCRIPTION
    This callback function is called when remote socket events 
    occur.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
  ===========================================================================*/

static void qmi_dfs_remote_socket_ev_cb
(
  ps_sys_tech_enum_type                  tech_type,
  ps_sys_event_enum_type                 event_name,
  ps_sys_subscription_enum_type          subscription_id,
  void                                   * event_info_ptr,
  void                                   * user_data_ptr
)
{
  qmi_dfsi_cmd_buf_type                  * cmd_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT( event_info_ptr );

  switch (event_name)
  {
    case PS_SYS_EVENT_REMOTE_SOCKET_REQUEST:
    {
      ps_sys_remote_socket_request_type *temp_ptr = (ps_sys_remote_socket_request_type*)event_info_ptr;

      LOG_MSG_INFO1_1("num_sockets: %d",temp_ptr->num_sockets);
      cmd_ptr = (qmi_dfsi_cmd_buf_type *) qmi_dfsi_get_cmd_buf(QMI_DFSI_REMOTE_SOCKET_REQUEST);
      if (cmd_ptr != NULL)
      {
        cmd_ptr->cmd_id = QMI_DFSI_REMOTE_SOCKET_REQUEST;
        memscpy(&cmd_ptr->data.socket_request, sizeof(ps_sys_remote_socket_request_type),
                event_info_ptr, sizeof(ps_sys_remote_socket_request_type));
      }
      break;
    }
    case PS_SYS_EVENT_REMOTE_SOCKET_RELEASE:
    {
      ps_sys_remote_socket_release_type *temp_ptr = (ps_sys_remote_socket_release_type*)event_info_ptr;
      uint16 i;
      LOG_MSG_INFO1_1("num_sockets: %d",temp_ptr->num_sockets);
      for (i=0;i<temp_ptr->num_sockets;i++)
      {
        LOG_MSG_INFO1_1("Socket handle 0x%x ",
          temp_ptr->socket_list[i]);
      }
      cmd_ptr = (qmi_dfsi_cmd_buf_type *) qmi_dfsi_get_cmd_buf(QMI_DFSI_REMOTE_SOCKET_RELEASE);
      if (cmd_ptr != NULL)
      {
        cmd_ptr->cmd_id = QMI_DFSI_REMOTE_SOCKET_RELEASE;
        memscpy(&cmd_ptr->data.socket_release, sizeof(ps_sys_remote_socket_release_type),
                event_info_ptr, sizeof(ps_sys_remote_socket_release_type));
      }
      break;
    }
    case PS_SYS_EVENT_REMOTE_SOCKET_SET_OPTION:
    {
      ps_sys_remote_socket_set_option_type *temp_ptr = (ps_sys_remote_socket_set_option_type*)event_info_ptr;
      LOG_MSG_INFO1_2("Socket handle 0x%x is_udp_encaps %d ", temp_ptr->socket_handle, temp_ptr->is_udp_encaps);
      cmd_ptr = (qmi_dfsi_cmd_buf_type *) qmi_dfsi_get_cmd_buf(QMI_DFSI_REMOTE_SOCKET_SET_OPTION);
      if (cmd_ptr != NULL)
      {
        cmd_ptr->cmd_id = QMI_DFSI_REMOTE_SOCKET_SET_OPTION;
        memscpy(&cmd_ptr->data.socket_option, sizeof(ps_sys_remote_socket_set_option_type),
                event_info_ptr, sizeof(ps_sys_remote_socket_set_option_type));
      }
      break;
    }
     default:
       LOG_MSG_ERROR_1("Unhandled ps_sys ev %d", event_name);
       return;
  }
  if (cmd_ptr == NULL)
  {
    return;
  }
 
  dcc_send_cmd_ex (DCC_DFS_SVC_CMD, cmd_ptr);
}

/*===========================================================================
  FUNCTION qmi_dfsi_process_cmd()

  DESCRIPTION
    This function processes a QMI DFS command

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI DFS command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI DFS must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_process_cmd
(
  dcc_cmd_enum_type  cmd,
  void              *cmd_ptr
)
{
  /*------------------------------------------------------------------------*/
    qmi_dfsi_cmd_buf_type *cmd_buf_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);
  cmd_buf_ptr = (qmi_dfsi_cmd_buf_type *)cmd_ptr;

  LOG_MSG_INFO1_1("qmi_dfsi_process_cmd: cmdbuf's cmd_id:%d",cmd_buf_ptr->cmd_id);

  switch( cmd_buf_ptr->cmd_id )
  {
    case QMI_DFSI_POWERSAVE_FILTER_STATE_CHANGE_EVT:
    {
      qmi_dfsi_generate_filter_mode_indication(cmd_buf_ptr);
      break;
    }
    case QMI_DFSI_CMD_REVERSE_IP_TRANSPORT_IPFLTR_CHANGED:
    {
      qmi_dfsi_generate_rev_ip_fltr_changed_ind(cmd_buf_ptr);
      break;
    }

    case QMI_DFSI_LOW_LATENCY_TRAFFIC_STATUS_EVT:
    {
      qmi_dfsi_generate_low_latency_traffic_status_indication( cmd_buf_ptr );
      break;
    }
    
    case QMI_DFSI_REMOTE_SOCKET_REQUEST:
    {
      qmi_dfsi_generate_remote_socket_request_indication( cmd_buf_ptr );
      break;
    }

    case QMI_DFSI_REMOTE_SOCKET_RELEASE:
    {
      qmi_dfsi_generate_remote_socket_release_indication( cmd_buf_ptr );
      break;
    }

    case QMI_DFSI_REMOTE_SOCKET_SET_OPTION:
    {
      qmi_dfsi_generate_remote_socket_set_option_indication( cmd_buf_ptr );
      break;
    }
    
    default:
      break;
  }
  if(cmd_ptr != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
  }
} /* qmi_dfsi_process_cmd() */
/*===========================================================================
  FUNCTION QMI_DFSI_GET_CMD_BUF()

  DESCRIPTION
    Allocate and assign a QMI DFS command buffer from the PS MEM heap
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
static void* qmi_dfsi_get_cmd_buf
(
  qmi_dfsi_cmd_type cmd_id
)
{
  qmi_dfsi_cmd_buf_type *cmd_buf_ptr = NULL;
  switch (cmd_id)
  {
    case QMI_DFSI_POWERSAVE_FILTER_STATE_CHANGE_EVT:
    case QMI_DFSI_CMD_REVERSE_IP_TRANSPORT_IPFLTR_CHANGED:
    case QMI_DFSI_LOW_LATENCY_TRAFFIC_STATUS_EVT:
    case QMI_DFSI_REMOTE_SOCKET_REQUEST:
    case QMI_DFSI_REMOTE_SOCKET_RELEASE:
    case QMI_DFSI_REMOTE_SOCKET_SET_OPTION:
    {
      PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(cmd_buf_ptr, sizeof(qmi_dfsi_cmd_buf_type),
                               qmi_dfsi_cmd_buf_type*);
      return ((void*)cmd_buf_ptr);
    }
     
    default:
      LOG_MSG_ERROR_1("Unknown QMI DFS command %d", (int32)cmd_id);
      break;
  }
  return NULL;
} /* qmi_dfsi_get_cmd_buf */

/*===========================================================================
  FUNCTION QMI_DFSI_CONVERT_PS_IPFLTR_TO_DFS_TYPE

  DESCRIPTION
   fills dfs filter_rule from ps_ipfltr type.
    
  PARAMETERS
    ps_fltr_rule  - ps filter rule to be converted
    dfs_fltr_fule - dfs filter rule to be filled out

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qmi_dfsi_convert_ps_ipfltr_to_dfs_type
(
  ip_filter_type           * ps_fltr_rule,
  dfs_filter_rule_type_v01 * dfs_fltr_rule
)
{
  dfs_fltr_rule->ip_info.ip_version = (dfs_ip_family_enum_v01)ps_fltr_rule->ip_vsn;
  if(dfs_fltr_rule->ip_info.ip_version == DFS_IP_FAMILY_IPV4_V01)
  {
    qmi_dfsi_convert_ps_to_dfs_v4_info(ps_fltr_rule, dfs_fltr_rule);
  }
  else
  {
    qmi_dfsi_convert_ps_to_dfs_v6_info(ps_fltr_rule, dfs_fltr_rule);
  }

} /* qmi_dfsi_convert_ps_ipfltr_to_dfs_type */

/*===========================================================================
  FUNCTION QMI_DFSI_CONVERT_PS_TO_DFS_V4_INFO

  DESCRIPTION
   fills dfs ipv4 info from ps_ipfltr type.
    
  PARAMETERS
    ps_fltr_rule  - ps filter rule to be converted
    dfs_fltr_fule - dfs filter rule to be filled out
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qmi_dfsi_convert_ps_to_dfs_v4_info
(
  ip_filter_type           * ps_fltr_rule,
  dfs_filter_rule_type_v01 * dfs_fltr_rule
)
{
  if (ps_fltr_rule->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR)
  {
    dfs_fltr_rule->ip_info.v4_info.valid_params |= QMI_DFS_IPV4_FILTER_MASK_SRC_ADDR_V01;
    dfs_fltr_rule->ip_info.v4_info.src_addr.ipv4_addr = 
      ps_ntohl(ps_fltr_rule->ip_hdr.v4.src.addr.ps_s_addr);
    dfs_fltr_rule->ip_info.v4_info.src_addr.subnet_mask = 
      ps_ntohl(ps_fltr_rule->ip_hdr.v4.src.subnet_mask.ps_s_addr);
  }
  if (ps_fltr_rule->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR)
  {
    dfs_fltr_rule->ip_info.v4_info.valid_params |= QMI_DFS_IPV4_FILTER_MASK_DEST_ADDR_V01;
    dfs_fltr_rule->ip_info.v4_info.dest_addr.ipv4_addr = 
      ps_ntohl(ps_fltr_rule->ip_hdr.v4.dst.addr.ps_s_addr);
    dfs_fltr_rule->ip_info.v4_info.dest_addr.subnet_mask = 
      ps_ntohl(ps_fltr_rule->ip_hdr.v4.dst.subnet_mask.ps_s_addr);
  }
  if (ps_fltr_rule->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS)
  {
    dfs_fltr_rule->ip_info.v4_info.valid_params |= QMI_DFS_IPV4_FILTER_MASK_TOS_V01;
    dfs_fltr_rule->ip_info.v4_info.tos.mask = ps_fltr_rule->ip_hdr.v4.tos.mask;
    dfs_fltr_rule->ip_info.v4_info.tos.val = ps_fltr_rule->ip_hdr.v4.tos.val;
  }
  if (ps_fltr_rule->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
  {
    dfs_fltr_rule->xport_info.xport_protocol = 
      (dfs_xport_protocol_enum_v01)ps_fltr_rule->ip_hdr.v4.next_hdr_prot;
    qmi_dfsi_convert_ps_to_dfs_xport_info(ps_fltr_rule,dfs_fltr_rule);
  }
} /* qmi_dfsi_convert_ps_to_dfs_v4_info */

/*===========================================================================
  FUNCTION QMI_DFSI_CONVERT_PS_TO_DFS_V6_INFO

  DESCRIPTION
   fills dfs ipv6 info from ps_ipfltr type.
    
  PARAMETERS
    ps_fltr_rule  - ps filter rule to be converted
    dfs_fltr_fule - dfs filter rule to be filled out
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qmi_dfsi_convert_ps_to_dfs_v6_info
(
  ip_filter_type           * ps_fltr_rule,
  dfs_filter_rule_type_v01 * dfs_fltr_rule
)
{
  if (ps_fltr_rule->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
  {
    dfs_fltr_rule->ip_info.v6_info.valid_params |= QMI_DFS_IPV6_FILTER_MASK_SRC_ADDR_V01;
    memscpy(dfs_fltr_rule->ip_info.v6_info.src_addr.ipv6_address, 
            sizeof(dfs_fltr_rule->ip_info.v6_info.src_addr.ipv6_address),
            ps_fltr_rule->ip_hdr.v6.src.addr.in6_u.u6_addr8,
            sizeof(ps_fltr_rule->ip_hdr.v6.src.addr.in6_u.u6_addr8));
    dfs_fltr_rule->ip_info.v6_info.src_addr.prefix_len = 
      ps_fltr_rule->ip_hdr.v6.src.prefix_len;
  }
  if (ps_fltr_rule->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR)
  {
    dfs_fltr_rule->ip_info.v6_info.valid_params |= QMI_DFS_IPV6_FILTER_MASK_DEST_ADDR_V01;
    memscpy(dfs_fltr_rule->ip_info.v6_info.dest_addr.ipv6_address, 
            sizeof(dfs_fltr_rule->ip_info.v6_info.dest_addr.ipv6_address),
            ps_fltr_rule->ip_hdr.v6.dst.addr.in6_u.u6_addr8,
            sizeof(ps_fltr_rule->ip_hdr.v6.dst.addr.in6_u.u6_addr8));
    dfs_fltr_rule->ip_info.v6_info.dest_addr.prefix_len = 
      ps_fltr_rule->ip_hdr.v6.dst.prefix_len;
  }
  if (ps_fltr_rule->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
  {
    dfs_fltr_rule->ip_info.v6_info.valid_params |= QMI_DFS_IPV6_FILTER_MASK_TRAFFIC_CLASS_V01;
    dfs_fltr_rule->ip_info.v6_info.trf_cls.mask = ps_fltr_rule->ip_hdr.v6.trf_cls.mask;
    dfs_fltr_rule->ip_info.v6_info.trf_cls.val = ps_fltr_rule->ip_hdr.v6.trf_cls.val;
  }
  if (ps_fltr_rule->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
  {
    dfs_fltr_rule->ip_info.v6_info.valid_params |= QMI_DFS_IPV6_FILTER_MASK_FLOW_LABEL_V01;
    dfs_fltr_rule->ip_info.v6_info.flow_label = ps_ntohl(ps_fltr_rule->ip_hdr.v6.flow_label);
  }
  if (ps_fltr_rule->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
  {
    dfs_fltr_rule->xport_info.xport_protocol = 
      (dfs_xport_protocol_enum_v01)ps_fltr_rule->ip_hdr.v6.next_hdr_prot;
    qmi_dfsi_convert_ps_to_dfs_xport_info(ps_fltr_rule,dfs_fltr_rule);
  }
} /* qmi_dfsi_convert_ps_to_dfs_v6_info */

/*===========================================================================
  FUNCTION QMI_DFSI_CONVERT_PS_TO_DFS_XPORT_INFO

  DESCRIPTION
   fills dfs xport info from ps_ipfltr type.
    
  PARAMETERS
    ps_fltr_rule  - ps filter rule to be converted
    dfs_fltr_fule - dfs filter rule to be filled out
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qmi_dfsi_convert_ps_to_dfs_xport_info
(
  ip_filter_type           * ps_fltr_rule,
  dfs_filter_rule_type_v01 * dfs_fltr_rule
)
{
  switch(dfs_fltr_rule->xport_info.xport_protocol)
  {
    case DFS_PROTO_ICMP_V01:
    case DFS_PROTO_ICMP6_V01:
    {
      if(ps_fltr_rule->next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_TYPE)
      {
        dfs_fltr_rule->xport_info.icmp_info.valid_params |= 
          QMI_DFS_ICMP_FILTER_MASK_MSG_TYPE_V01;
        dfs_fltr_rule->xport_info.icmp_info.type = 
          ps_fltr_rule->next_prot_hdr.icmp.type;
      }
      if(ps_fltr_rule->next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE)
      {
        dfs_fltr_rule->xport_info.icmp_info.valid_params |= 
          QMI_DFS_ICMP_FILTER_MASK_MSG_CODE_V01;
        dfs_fltr_rule->xport_info.icmp_info.code = 
          ps_fltr_rule->next_prot_hdr.icmp.code;
      }
      break;
    }
    case DFS_PROTO_TCP_V01:
    {
      if(ps_fltr_rule->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
      {
        dfs_fltr_rule->xport_info.tcp_info.valid_params |= 
          QMI_DFS_PORT_INFO_FILTER_MASK_SRC_PORT_V01;
        dfs_fltr_rule->xport_info.tcp_info.src_port_info.port = 
          ps_ntohs(ps_fltr_rule->next_prot_hdr.tcp.src.port);
        dfs_fltr_rule->xport_info.tcp_info.src_port_info.range = 
          ps_fltr_rule->next_prot_hdr.tcp.src.range;
      }
      if(ps_fltr_rule->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
      {
        dfs_fltr_rule->xport_info.tcp_info.valid_params |= 
          QMI_DFS_PORT_INFO_FILTER_MASK_DEST_PORT_V01;
        dfs_fltr_rule->xport_info.tcp_info.dest_port_info.port = 
          ps_ntohs(ps_fltr_rule->next_prot_hdr.tcp.dst.port);
        dfs_fltr_rule->xport_info.tcp_info.dest_port_info.range = 
          ps_fltr_rule->next_prot_hdr.tcp.dst.range;
      }
      break;
    }
    case DFS_PROTO_UDP_V01:
    {
      if(ps_fltr_rule->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT)
      {
        dfs_fltr_rule->xport_info.udp_info.valid_params |= 
          QMI_DFS_PORT_INFO_FILTER_MASK_SRC_PORT_V01;
        dfs_fltr_rule->xport_info.udp_info.src_port_info.port = 
          ps_ntohs(ps_fltr_rule->next_prot_hdr.udp.src.port);
        dfs_fltr_rule->xport_info.udp_info.src_port_info.range = 
          ps_fltr_rule->next_prot_hdr.udp.src.range;
      }
      if(ps_fltr_rule->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT)
      {
        dfs_fltr_rule->xport_info.udp_info.valid_params |= 
          QMI_DFS_PORT_INFO_FILTER_MASK_DEST_PORT_V01;
        dfs_fltr_rule->xport_info.udp_info.dest_port_info.port = 
          ps_ntohs(ps_fltr_rule->next_prot_hdr.udp.dst.port);
        dfs_fltr_rule->xport_info.udp_info.dest_port_info.range = 
          ps_fltr_rule->next_prot_hdr.udp.dst.range;
      }
      break;
    }
    case DFS_PROTO_ESP_V01:
    {
      if(ps_fltr_rule->next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI)
      {
        dfs_fltr_rule->xport_info.esp_info.valid_params |= 
          QMI_DFS_IPSEC_FILTER_MASK_SPI_V01;
        dfs_fltr_rule->xport_info.esp_info.spi = 
          ps_fltr_rule->next_prot_hdr.esp.spi;
      }
      break;
    }
    case DFS_PROTO_AH_V01:
    {
      if(ps_fltr_rule->next_prot_hdr.ah.field_mask & IPFLTR_MASK_AH_SPI)
      {
        dfs_fltr_rule->xport_info.ah_info.valid_params |= 
          QMI_DFS_IPSEC_FILTER_MASK_SPI_V01;
        dfs_fltr_rule->xport_info.ah_info.spi = 
          ps_fltr_rule->next_prot_hdr.ah.spi;
      }
      break;
    }
    default:
      break;
  }
} /* qmi_dfsi_convert_ps_to_dfs_xport_info */

/*===========================================================================
  FUNCTION QMI_DFSI_FILL_V4_FILTER_SPEC()

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

static void qmi_dfsi_fill_v4_filter_spec
(
  dfs_filter_rule_type_v01* filter_rules,
  ip_filter_type*           fltr_spec
)
{
  ASSERT(filter_rules && fltr_spec);
  
  if(filter_rules->ip_info.v4_info.valid_params == QMI_DFS_IPV4_FILTER_MASK_NONE_V01)
  {
    fltr_spec->ip_hdr.v4.field_mask = IPFLTR_MASK_IP4_NONE;	
  }
  else
  {
    if(filter_rules->ip_info.v4_info.valid_params & 
                      QMI_DFS_IPV4_FILTER_MASK_SRC_ADDR_V01)
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_SRC_ADDR;
      fltr_spec->ip_hdr.v4.src.addr.ps_s_addr = 
              ps_htonl(filter_rules->ip_info.v4_info.src_addr.ipv4_addr);
      fltr_spec->ip_hdr.v4.src.subnet_mask.ps_s_addr =
              ps_htonl(filter_rules->ip_info.v4_info.src_addr.subnet_mask);
    }
    if(filter_rules->ip_info.v4_info.valid_params & 
                      QMI_DFS_IPV4_FILTER_MASK_DEST_ADDR_V01)
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_DST_ADDR;
      fltr_spec->ip_hdr.v4.dst.addr.ps_s_addr = 
              ps_htonl(filter_rules->ip_info.v4_info.dest_addr.ipv4_addr);
      fltr_spec->ip_hdr.v4.dst.subnet_mask.ps_s_addr = 
              ps_htonl(filter_rules->ip_info.v4_info.dest_addr.subnet_mask);			  
    }
    if(filter_rules->ip_info.v4_info.valid_params & 
                      QMI_DFS_IPV4_FILTER_MASK_TOS_V01)
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
      fltr_spec->ip_hdr.v4.tos.val     = filter_rules->ip_info.v4_info.tos.val;
      fltr_spec->ip_hdr.v4.tos.mask    = filter_rules->ip_info.v4_info.tos.mask;
    }
  }
    if(filter_rules->xport_info.xport_protocol != DFS_PROTO_NONE_V01) 
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
      fltr_spec->ip_hdr.v4.next_hdr_prot = 
                 filter_rules->xport_info.xport_protocol;
      qmi_dfsi_fill_fltr_xport_info(&(filter_rules->xport_info),fltr_spec);
    }
  }

/*===========================================================================
  FUNCTION QMI_DFSI_FILL_V6_FILTER_SPEC()

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
static void qmi_dfsi_fill_v6_filter_spec
(
  dfs_filter_rule_type_v01* filter_rules,
  ip_filter_type*           fltr_spec
)
{
  ASSERT(filter_rules && fltr_spec);

  if(filter_rules->ip_info.v6_info.valid_params == 
                          QMI_DFS_IPV6_FILTER_MASK_NONE_V01)
  {
    fltr_spec->ip_hdr.v6.field_mask = IPFLTR_MASK_IP6_NONE;		    
  }
  else
  {
    if(filter_rules->ip_info.v6_info.valid_params & 
                           QMI_DFS_IPV6_FILTER_MASK_SRC_ADDR_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
      memscpy(&fltr_spec->ip_hdr.v6.src.addr,sizeof(fltr_spec->ip_hdr.v6.src.addr),
    	&filter_rules->ip_info.v6_info.src_addr.ipv6_address,
    	sizeof(filter_rules->ip_info.v6_info.src_addr.ipv6_address));
      fltr_spec->ip_hdr.v6.src.prefix_len = 
         filter_rules->ip_info.v6_info.src_addr.prefix_len;
    }
    if(filter_rules->ip_info.v6_info.valid_params & 
                            QMI_DFS_IPV6_FILTER_MASK_DEST_ADDR_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_DST_ADDR;
      memscpy(&fltr_spec->ip_hdr.v6.dst.addr,sizeof(fltr_spec->ip_hdr.v6.dst.addr),
         &filter_rules->ip_info.v6_info.dest_addr.ipv6_address,
    	 sizeof(filter_rules->ip_info.v6_info.dest_addr.ipv6_address));
      fltr_spec->ip_hdr.v6.dst.prefix_len = 
         filter_rules->ip_info.v6_info.dest_addr.prefix_len;			  
    }
    if(filter_rules->ip_info.v6_info.valid_params & 
                            QMI_DFS_IPV6_FILTER_MASK_TRAFFIC_CLASS_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
      fltr_spec->ip_hdr.v6.trf_cls.val = 
         filter_rules->ip_info.v6_info.trf_cls.val;
      fltr_spec->ip_hdr.v6.trf_cls.mask = 
         filter_rules->ip_info.v6_info.trf_cls.mask;
    }
    if(filter_rules->ip_info.v6_info.valid_params & 
                             QMI_DFS_IPV6_FILTER_MASK_FLOW_LABEL_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_FLOW_LABEL;
      fltr_spec->ip_hdr.v6.flow_label = 
          ps_htonl(filter_rules->ip_info.v6_info.flow_label);
    }
  }
  if(filter_rules->xport_info.xport_protocol != DFS_PROTO_NONE_V01) 
  {
    fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
    fltr_spec->ip_hdr.v6.next_hdr_prot = 
       filter_rules->xport_info.xport_protocol;
    qmi_dfsi_fill_fltr_xport_info(&(filter_rules->xport_info),fltr_spec);
  }    			
}

/*===========================================================================
  FUNCTION QMI_DFSI_FILL_FLTR_XPORT_INFO()

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
static void qmi_dfsi_fill_fltr_xport_info
(
  dfs_xport_header_type_v01* xport_info,
  ip_filter_type*            fltr_spec
)
{
  ASSERT(xport_info && fltr_spec);

  LOG_MSG_INFO1_1("qmi_dfsi_fill_fltr_xport_info: protocol = %d",
  	               xport_info->xport_protocol);
  
  switch( xport_info->xport_protocol )
  {
    case DFS_PROTO_ICMP_V01:
    case DFS_PROTO_ICMP6_V01:
        {
          if(xport_info->icmp_info.valid_params != 
                        QMI_DFS_ICMP_FILTER_MASK_NONE_V01)
          {
            if(xport_info->icmp_info.valid_params & 
                        QMI_DFS_ICMP_FILTER_MASK_MSG_TYPE_V01)
            {
              fltr_spec->next_prot_hdr.icmp.field_mask |= IPFLTR_MASK_ICMP_MSG_TYPE;
              fltr_spec->next_prot_hdr.icmp.type = xport_info->icmp_info.type;
            }
            if(xport_info->icmp_info.valid_params & 
                        QMI_DFS_ICMP_FILTER_MASK_MSG_CODE_V01)
            {
              fltr_spec->next_prot_hdr.icmp.field_mask |= IPFLTR_MASK_ICMP_MSG_CODE;
              fltr_spec->next_prot_hdr.icmp.code = xport_info->icmp_info.code;
            }
          }
    	}
        break;
    case DFS_PROTO_TCP_V01:
        {
          if(xport_info->tcp_info.valid_params != 
                         QMI_DFS_PORT_INFO_FILTER_MASK_NONE_V01)
          {
            if(xport_info->tcp_info.valid_params & 
                         QMI_DFS_PORT_INFO_FILTER_MASK_SRC_PORT_V01)
            {
              fltr_spec->next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_SRC_PORT;
              fltr_spec->next_prot_hdr.tcp.src.port  = 
         			  ps_htons(xport_info->tcp_info.src_port_info.port);
              fltr_spec->next_prot_hdr.tcp.src.range = 
         			  xport_info->tcp_info.src_port_info.range;
            }
            if(xport_info->tcp_info.valid_params & 
                         QMI_DFS_PORT_INFO_FILTER_MASK_DEST_PORT_V01)
            {
              fltr_spec->next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_DST_PORT;
              fltr_spec->next_prot_hdr.tcp.dst.port  = 
                                  ps_htons(xport_info->tcp_info.dest_port_info.port);
              fltr_spec->next_prot_hdr.tcp.dst.range = 
                                  xport_info->tcp_info.dest_port_info.range;
            }
          }	
        }
        break;
    case DFS_PROTO_UDP_V01:
        {
          if(xport_info->udp_info.valid_params != 
                                QMI_DFS_PORT_INFO_FILTER_MASK_NONE_V01)
          {
            if(xport_info->udp_info.valid_params & 
                             QMI_DFS_PORT_INFO_FILTER_MASK_SRC_PORT_V01)
            {
              fltr_spec->next_prot_hdr.udp.field_mask |= 
                                   IPFLTR_MASK_UDP_SRC_PORT;
              fltr_spec->next_prot_hdr.udp.src.port  = 
            			   ps_htons(xport_info->udp_info.src_port_info.port);
              fltr_spec->next_prot_hdr.udp.src.range = 
            		       xport_info->udp_info.src_port_info.range;
            }
            if(xport_info->udp_info.valid_params &
                               QMI_DFS_PORT_INFO_FILTER_MASK_DEST_PORT_V01)
            {
              fltr_spec->next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;
              fltr_spec->next_prot_hdr.udp.dst.port  = 
            				   ps_htons(xport_info->udp_info.dest_port_info.port);
              fltr_spec->next_prot_hdr.udp.dst.range = 
            			       xport_info->udp_info.dest_port_info.range;
            }  
          }	
        }
    	break;
    case DFS_PROTO_ESP_V01:
        {
          if(xport_info->esp_info.valid_params != QMI_DFS_IPSEC_FILTER_MASK_NONE_V01)
          {
            if(xport_info->esp_info.valid_params & QMI_DFS_IPSEC_FILTER_MASK_SPI_V01)
            {
              fltr_spec->next_prot_hdr.esp.field_mask |= IPFLTR_MASK_ESP_SPI;
              fltr_spec->next_prot_hdr.esp.spi  = xport_info->esp_info.spi;
            }
          }	
        }
        break;
    case DFS_PROTO_AH_V01:
        {
          if(xport_info->ah_info.valid_params != QMI_DFS_IPSEC_FILTER_MASK_NONE_V01)
          {
            if(xport_info->ah_info.valid_params & QMI_DFS_IPSEC_FILTER_MASK_SPI_V01)
            {
              fltr_spec->next_prot_hdr.ah.field_mask |= IPFLTR_MASK_ESP_SPI;
              fltr_spec->next_prot_hdr.ah.spi  = xport_info->ah_info.spi;
            }
          }	
        }
        break;
    default:
        break;
  }
}

/*===========================================================================
  FUNCTION QMI_DFSI_FILL_FILTER_ERROR_MASK()

  DESCRIPTION
  This fills Filter mask to be reported in error case of add_powersave_filters_req.
  Error mask will be filled based on the error mask in filterspec which is updated by ps_iface in case of validation failes.  

  PARAMETERS
   ip_filter_type - Filter rule 
   dfs_filter_param_error_mask_v01 - error mask to be filled for passed filter rule.
   
  RETURN VALUE
    boolean(true or false)

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_fill_filter_error_mask
(
  ip_filter_type* filter_spec,
  dfs_filter_param_error_mask_v01* error_mask
)
{
  uint8    next_hdr_prot = DFS_PROTO_NONE_V01;

  ASSERT(filter_spec && error_mask);
  if(filter_spec->ip_vsn == IP_V4)
  {
    if(filter_spec->ip_hdr.v4.err_mask != IPFLTR_MASK_IP4_NONE)
    {
      if(filter_spec->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_SRC_ADDR)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV4_SRC_ADDR_V01;
      }
      if(filter_spec->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_DST_ADDR)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV4_DEST_ADDR_V01;
      }
      if(filter_spec->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_TOS)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV4_TOS_V01;
      }  
      if(filter_spec->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_XPORT_PROT_V01;
      }
    }
    next_hdr_prot = filter_spec->ip_hdr.v4.next_hdr_prot;
  }
  else if(filter_spec->ip_vsn == IP_V6)
  {
    if(filter_spec->ip_hdr.v6.err_mask != IPFLTR_MASK_IP6_NONE)
    {
      if(filter_spec->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_SRC_ADDR)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV6_SRC_ADDR_V01;
      }
      if(filter_spec->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_DST_ADDR)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV6_DEST_ADDR_V01;
      }
      if(filter_spec->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV6_TRF_CLS_V01;
      }	  
      if(filter_spec->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV6_FLOW_LABEL_V01;
      }
    }
    next_hdr_prot = filter_spec->ip_hdr.v6.next_hdr_prot;;
  }
  else
  {
    *error_mask |= QMI_DFS_FILTER_PARAM_IP_VERSION_V01;
  }
  switch(next_hdr_prot)
  {
    case DFS_PROTO_ICMP_V01:
    case DFS_PROTO_ICMP6_V01:
    {
      if(filter_spec->next_prot_hdr.icmp.err_mask != IPFLTR_MASK_ICMP_NONE)
      {
        if(filter_spec->next_prot_hdr.icmp.err_mask & IPFLTR_MASK_ICMP_MSG_TYPE)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_ICMP_TYPE_V01;
        }
        if(filter_spec->next_prot_hdr.icmp.err_mask & IPFLTR_MASK_ICMP_MSG_CODE)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_ICMP_CODE_V01;
        }			  
      }
    }
    break;
    case DFS_PROTO_TCP_V01:
    {
      if(filter_spec->next_prot_hdr.tcp.err_mask != IPFLTR_MASK_TCP_NONE)
      {
        if(filter_spec->next_prot_hdr.tcp.err_mask & IPFLTR_MASK_TCP_SRC_PORT)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_TCP_SRC_PORT_V01;
        }
        if(filter_spec->next_prot_hdr.tcp.err_mask & IPFLTR_MASK_TCP_DST_PORT)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_TCP_DEST_PORT_V01;
        }			  
      }
    }
    break;
    case DFS_PROTO_UDP_V01:
    {
      if(filter_spec->next_prot_hdr.udp.err_mask != IPFLTR_MASK_UDP_NONE)
      {
        if(filter_spec->next_prot_hdr.udp.err_mask & IPFLTR_MASK_UDP_SRC_PORT)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_UDP_SRC_PORT_V01;
        }
        if(filter_spec->next_prot_hdr.udp.err_mask & IPFLTR_MASK_UDP_DST_PORT)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_UDP_DEST_PORT_V01;
        }			  
      }			
    }
    break;
    case DFS_PROTO_ESP_V01:
    {
      if(filter_spec->next_prot_hdr.esp.err_mask != IPFLTR_MASK_ESP_NONE)
      {
        if(filter_spec->next_prot_hdr.esp.err_mask & IPFLTR_MASK_ESP_SPI)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_ESP_SPI_V01;
        }
      }			
    }
    break;
    case DFS_PROTO_AH_V01:
    {
      if(filter_spec->next_prot_hdr.ah.err_mask != IPFLTR_MASK_AH_NONE)
      {
        if(filter_spec->next_prot_hdr.ah.err_mask & IPFLTR_MASK_AH_SPI)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_AH_SPI_V01;
        }
      }			
    }
    break;
    default:
      break;
  }
  LOG_MSG_INFO1_1("error mask info (%d)",*error_mask);
}

/*===========================================================================
  FUNCTION QMI_DFSI_VERIFY_CLIENT_SUBS()

  DESCRIPTION
    This function verifies if the client subscription is applicable to
    the event subscription.
 
  PARAMETERS
    clnt_info         : client state pointer
    event_subs_id     : Subscription of the event
    call_specific     : Identifies if the request is related to a data call

  RETURN VALUE
   TRUE : if the event is applicable to client
   FALSE: otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_dfsi_verify_client_subs
(
  qmi_dfsi_client_info_type            *clnt_info,
  uint32                               event_subs_id,
  boolean                              call_specific
)
{
  boolean                             send_ind = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(clnt_info);

  if (event_subs_id >= PS_SYS_SUBS_MAX|| 
      event_subs_id <= PS_SYS_DEFAULT_SUBS)
  {
    //event subs should be valid for resolving
    LOG_MSG_ERROR_1( "Cannot resolve: Invalid subs passed %d", event_subs_id );
    return FALSE;
  }

  if (clnt_info->subscription == event_subs_id)
  {
    //Client subs and event subs match
    send_ind = TRUE;
    goto send_result;
  }

  if (event_subs_id == ps_sys_get_default_data_subscription())
  {
    if (clnt_info->subscription == DFS_DEFAULT_SUBS_V01 || 
        clnt_info->subscription == DFS_DONT_CARE_SUBS_V01)
    {
      //Send to DDS and Don't care clients
      send_ind = TRUE;
      goto send_result;
    }
  }
  else
  {
    if (clnt_info->subscription == DFS_DONT_CARE_SUBS_V01 && 
        call_specific == TRUE) 
    {
      //Send all call specific events to don't care clients
      send_ind = TRUE;
      goto send_result;
    }
  }

send_result:
  LOG_MSG_INFO1_3( "Cl subs %d, event_subs %d send_ind %d", 
                   clnt_info->subscription, event_subs_id, send_ind );
  return send_ind;
}/* qmi_dfsi_verify_client_subs */



