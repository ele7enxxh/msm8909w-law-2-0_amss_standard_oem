/*===========================================================================

                         D S _ Q M I _ D S D . C

DESCRIPTION

  The Data Services QMI Data System Determination service source file.

EXTERNALIZED FUNCTIONS

  qmi_dsd_init()
    Initialize the QMI DSD service

Copyright (c) 2013-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_dsd.c#1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/14/14    sah    Added support for limit_so_mask_change_ind in system status
                   change request.
11/07/12    sj     Created module
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

#include "ds_qmi_dsd.h"
#include "ps_system_heap.h"
#include "ps_sys.h"
#include "ps_sys_conf.h"
#include "ps_sys_event.h"
#include "ps_sys_ioctl.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_mppm_pdn_db_mgr.h"
#include "qmi_idl_lib.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "mqcsi_log.h"
#include "common_v01.h"
#include "data_system_determination_v01.h"
#include "qmi_sap.h"
#include "queue.h"
#include "qmi_framework.h"
#include <stringl/stringl.h>

#include "event_defs.h"
#include "event.h"           /* For logging DIAG event and event codes      */

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define DSDI_MAX_CLIDS    (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)
#define DSDI_INVALID_INSTANCE    -1

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for DSD
---------------------------------------------------------------------------*/
#define DSD_BASE_VER_MAJOR    (1)
#define DSD_BASE_VER_MINOR    (7)

#define DSD_ADDENDUM_VER_MAJOR  (0)
#define DSD_ADDENDUM_VER_MINOR  (0)

/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

#define DSDI_QCSI_OS_SIGNAL ((rex_sigs_type)(1 << DCC_DSD_RDR_SIGNAL))

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI DSD Client state info
---------------------------------------------------------------------------*/
typedef struct {
  q_link_type                  link; /*< Data Structure required by the queue 
                                         library to add this as part of a 
                                         queue */
  qmi_client_handle clnt;
   
  struct
  {
    boolean     report_system_status;
    boolean     limit_so_mask_change_report_ind;
    boolean     report_thermal_info;
    boolean     start_wifi_meas;
    boolean     report_wqe_profile_type_changes;
    boolean     report_wqe_profile_met_changes;
    boolean     report_nat_keep_alive;
    boolean     report_roaming_status_change;
  } report_status;
  
  dsd_bind_subscription_enum_v01   subscription_id;

}qmi_dsdi_client_info_type;

/* Global variable required to track last avail sys pref network */
ps_sys_system_status_info_type  qmi_dsdi_last_avail_sys_pref_network;

/*---------------------------------------------------------------------------
  QMI DSD Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  q_type                 client_q; /*< Information about all the clients that
                                       have connected to the service. This is queue 
                                       of type qmi_dsdi_client_info_type */
  qmi_csi_service_handle service_handle;
  qmi_sap_client_handle  sap_client_handle;
  uint16                 num_of_clients;
} qmi_dsdi_state_info_type;

/*---------------------------------------------------------------------------
  DSD Event report indication bitmask values
---------------------------------------------------------------------------*/
typedef enum
{
  DSDI_REPORT_STATUS_SYSTEM_STATUS       = 0x00000001,
  DSDI_REPORT_WIDTH         = 0x7FFFFFFF  /* forces enum values to 32 bits */
} qmi_dsdi_report_status_ev_e_type;

/*---------------------------------------------------------------------------
  DSD commands
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_DSDI_CMD_MIN_VAL = -1,	
  QMI_CMD_DSD_SYSTEM_STATUS_CHANGE_EVT = 0,
  QMI_CMD_DSD_THERMAL_INFO_CHANGE_EVT  = 1,
  QMI_CMD_DSD_START_WIFI_MEAS_EVT      = 2,
  QMI_CMD_DSD_STOP_WIFI_MEAS_EVT       = 3,
  QMI_CMD_DSD_WQE_PROFILE_EVT          = 4,
  QMI_CMD_DSD_WQE_PROFILE_MET_EVT      = 5,
  QMI_CMD_DSD_NAT_KEEP_ALIVE_EVT       = 6,
  QMI_CMD_DSD_WQE_PROFILE_INIT_EVT     = 7,
  QMI_CMD_DSD_ROAMING_STATUS_CHANGE_EVT = 8,
  QMI_DSDI_CMD_MAX_VAL = 0x7FFFFFFF
} qmi_dsdi_cmd_type;

static qmi_dsdi_state_info_type      qmi_dsd_state;
static qmi_csi_os_params             os_params;
static boolean                       qmi_dsdi_inited = FALSE;


/* This is the default QMI_DSD command buffer. Make sure that it does not grow
 * beyond 512 bytes. In case you need a command buffer larger than 512 bytes,
 * declare a separate structure. */
typedef struct
{
  uint16  cmd_id; /* qmi_cmd_id_e_type */
  ps_sys_subscription_enum_type   subscription_id;
  union
  {
    dsd_system_status_ind_msg_v01 system_status;
    dsd_thermal_info_change_ind_msg_v01 thermal_info;
    dsd_start_wifi_meas_ind_msg_v01 start_wifi_meas_info;
    dsd_stop_wifi_meas_ind_msg_v01  stop_wifi_meas_info;
    dsd_wqe_profile_type_ind_msg_v01 wqe_profile_info;
    dsd_wqe_profile_type_met_ind_msg_v01 wqe_profile_met_info;
    dsd_nat_keep_alive_info_ind_msg_v01 nat_keep_alive_info;
    dsd_wqe_profile_init_ind_msg_v01 wqe_profile_init_info; 
    dsd_roaming_status_change_ind_msg_v01 roaming_status_info; 
  } data;
} qmi_dsdi_cmd_buf_type;



/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_WLAN_MAPCON
static qmi_csi_cb_error  qmi_dsdi_wlan_available(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_wlan_not_available(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_set_wlan_preference(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_get_wlan_preference(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_set_apn_preference_system(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
#endif /* FEATURE_DATA_WLAN_MAPCON */

static qmi_csi_cb_error  qmi_dsdi_get_modem_power_cost(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static boolean qmi_dsdi_msg_hdlr_cb
(
  dcc_sig_enum_type  sig,
  void              *svc_ptr
);

static qmi_csi_cb_error qmi_dsdi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmi_dsdi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_dsdi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);

#ifdef FEATURE_DATA_WLAN_MAPCON
static qmi_csi_cb_error  qmi_dsdi_get_system_status
( 
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
);  

static qmi_csi_cb_error  qmi_dsdi_system_status_change_req
(
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
);


static void qmi_dsdi_system_status_ind
(
  void                                  * client,
  dsd_system_status_ind_msg_v01         * system_status
);
#endif /* FEATURE_DATA_WLAN_MAPCON */

static void qmi_dsdi_process_cmd
(
  dcc_cmd_enum_type  cmd,
  void              *cmd_ptr
);

static void *qmi_dsdi_get_cmd_buf
(
);

#ifdef FEATURE_DATA_WLAN_MAPCON
static qmi_csi_cb_error  qmi_dsdi_pdn_policy_start_txn(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_add_pdn_policy(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_modify_pdn_policy(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_delete_pdn_policy_by_apn(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_get_pdn_policy_apn_list(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static qmi_csi_cb_error  qmi_dsdi_get_pdn_policy_settings_for_apn(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_pdn_policy_end_txn(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
#endif /* FEATURE_DATA_WLAN_MAPCON */

static qmi_csi_cb_error  qmi_dsdi_bind_subscription(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_get_bind_subscription(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error   qmi_dsdi_get_thermal_info
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_dsdi_indication_register
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);

static void dsdi_thermal_info_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
);

static void qmi_dsdi_generate_thermal_info_ind
(
  dsd_thermal_info_change_ind_msg_v01 * thermal_info,
  ps_sys_subscription_enum_type         subscription_id
);

static void qmi_dsdi_thermal_change_ind
(
  void                                  * client,
  dsd_thermal_info_change_ind_msg_v01   * thermal_info
);

static qmi_csi_cb_error  qmi_dsdi_notify_data_settings(
  qmi_dsdi_client_info_type  *clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void                       *req_c_struct,
  int                         req_c_struct_len,
  void                       *service_handle
);  

static qmi_csi_cb_error  qmi_dsdi_get_data_settings(
  qmi_dsdi_client_info_type  *clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void                       *req_c_struct,
  int                         req_c_struct_len,
  void                       *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_set_apn_info(
  qmi_dsdi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_get_apn_info(
  qmi_dsdi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
);

static void dsdi_start_wifi_info_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
);

static void dsdi_stop_wifi_info_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
);

static void qmi_dsdi_generate_start_wifi_meas_ind
(
  dsd_start_wifi_meas_ind_msg_v01       * start_wifi_meas_msg,
  ps_sys_subscription_enum_type         subscription_id
);

static void qmi_dsdi_generate_stop_wifi_meas_ind
(
  dsd_stop_wifi_meas_ind_msg_v01        * stop_wifi_meas_msg,
  ps_sys_subscription_enum_type         subscription_id
);

static qmi_csi_cb_error  qmi_dsdi_wifi_meas_report_config_status(
  qmi_dsdi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_wifi_meas_report(
  qmi_dsdi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_set_apn_call_type(
  qmi_dsdi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_set_wqe_profile_type(
  qmi_dsdi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
);

static void dsdi_wqe_profile_type_change_cb
(
  ps_sys_tech_enum_type            tech_type,
  ps_sys_event_enum_type           event_name,
  ps_sys_subscription_enum_type    subscription_id,
  void                             * event_info_ptr,  
  void                             * user_data_ptr
);

static void qmi_dsdi_generate_wqe_profile_type_ind
(
  dsd_wqe_profile_type_ind_msg_v01      * wqe_profile_ind_msg,
  ps_sys_subscription_enum_type         subscription_id
);


static void dsdi_wqe_profile_type_met_cb
(
  ps_sys_tech_enum_type            tech_type,
  ps_sys_event_enum_type           event_name,
  ps_sys_subscription_enum_type    subscription_id,
  void                             * event_info_ptr,  
  void                             * user_data_ptr
);

static void qmi_dsdi_generate_wqe_profile_type_met_ind
(
  dsd_wqe_profile_type_met_ind_msg_v01  * met_wqe_profile_ind_msg,
  ps_sys_subscription_enum_type         subscription_id
);

static void dsdi_nat_keep_alive_cb
(
  ps_sys_tech_enum_type            tech_type,
  ps_sys_event_enum_type           event_name,
  ps_sys_subscription_enum_type    subscription_id,
  void                             * event_info_ptr,  
  void                             * user_data_ptr
);

static void qmi_dsdi_generate_nat_keep_alive_ind
(
  dsd_nat_keep_alive_info_ind_msg_v01   * nat_keep_alive_ind_msg,
  ps_sys_subscription_enum_type         subscription_id
);

static qmi_csi_cb_error  qmi_dsdi_nat_keep_alive_config_status
(
  qmi_dsdi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_wqe_profile_quality_status
(
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
);

static void dsdi_wqe_profile_init_cb
(
  ps_sys_tech_enum_type            tech_type,
  ps_sys_event_enum_type           event_name,
  ps_sys_subscription_enum_type    subscription_id,
  void                             * event_info_ptr,  
  void                             * user_data_ptr
);

static void qmi_dsdi_generate_wqe_profile_init_ind
(
  dsd_wqe_profile_init_ind_msg_v01       * profile_init_info,
  ps_sys_subscription_enum_type            subscription_id
);

static qmi_csi_cb_error  qmi_dsdi_set_quality_measurement_info
(
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
);

static int qmi_dsdi_convert_ps_wqe_profile_to_qmi_wqe_profile
(
  ps_sys_wqe_profile_type ps_wqe,
  dsd_wqe_profile_type_enum_v01 *qmi_wqe
);

static int qmi_dsdi_convert_qmi_wqe_profile_to_ps_wqe_profile
(
  dsd_wqe_profile_type_enum_v01 qmi_wqe,
  ps_sys_wqe_profile_type *ps_wqe
);


static void dsdi_roaming_status_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
);

static void qmi_dsdi_generate_roaming_status_change_ind
(
  dsd_roaming_status_change_ind_msg_v01 * roaming_status_info,
  ps_sys_subscription_enum_type           subscription_id
);

static void qmi_dsdi_roaming_status_change_ind
(
  void                                  * client,
  dsd_roaming_status_change_ind_msg_v01 * roaming_status_info
);

static qmi_csi_cb_error   qmi_dsdi_get_roaming_status_info
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);
/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/


/* Define service handle table for DSD messages */
static qmi_csi_cb_error (* const req_handle_table[])
(
 qmi_dsdi_client_info_type        *clnt_info,
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
#ifdef FEATURE_DATA_WLAN_MAPCON
  qmi_dsdi_wlan_available,                 /* Request handler for message ID 0x20 */
  qmi_dsdi_wlan_not_available,             /* Request handler for message ID 0x21 */
  qmi_dsdi_set_wlan_preference,            /* Request handler for message ID 0x22 */
  qmi_dsdi_get_wlan_preference,            /* Request handler for message ID 0x23 */
  qmi_dsdi_get_system_status,              /* Request handler for message ID 0x24 */
  qmi_dsdi_system_status_change_req,       /* Request handler for message ID 0x25 */
#else
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
#endif /* FEATURE_DATA_WLAN_MAPCON */
  NULL,
  qmi_dsdi_bind_subscription,              /* Request handler for message ID 0x27 */
  qmi_dsdi_get_bind_subscription,          /* Request handler for message ID 0x28 */
#ifdef FEATURE_DATA_WLAN_MAPCON
  qmi_dsdi_set_apn_preference_system,      /* Request handler for message ID 0x29 */
#else
  NULL,
#endif /* FEATURE_DATA_WLAN_MAPCON */
  qmi_dsdi_get_modem_power_cost,             /* Request handler for message ID 0x2A */
#ifdef FEATURE_DATA_WLAN_MAPCON
  qmi_dsdi_pdn_policy_start_txn,           /* Request handler for message ID 0x2B */
  qmi_dsdi_add_pdn_policy,                 /* Request handler for message ID 0x2C */
  qmi_dsdi_modify_pdn_policy,              /* Request handler for message ID 0x2D */
  qmi_dsdi_delete_pdn_policy_by_apn,       /* Request handler for message ID 0x2E */
  qmi_dsdi_get_pdn_policy_apn_list,        /* Request handler for message ID 0x2F */
  qmi_dsdi_get_pdn_policy_settings_for_apn, /* Request handler for message ID 0x30 */ 
  qmi_dsdi_pdn_policy_end_txn,               /* Request handler for message ID 0x31 */ 
#else
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
#endif /* FEATURE_DATA_WLAN_MAPCON */
  qmi_dsdi_set_apn_info,                   /* Request handler for message ID 0x32 */  
  qmi_dsdi_get_apn_info,                   /* Request handler for message ID 0x33 */  
  qmi_dsdi_notify_data_settings,           /* Request handler for message ID 0x34 */ 
  qmi_dsdi_get_data_settings,              /* Request handler for message ID 0x35 */
  NULL,                                    /* Request handler for message ID 0x36 */ 
  qmi_dsdi_get_thermal_info,               /* Request handler for message ID 0x37 */ 
  qmi_dsdi_indication_register,            /* Request handler for message ID 0x38 */ 
  NULL,                                    /* Request handler for message ID 0x39 */ 
  NULL,                                    /* Request handler for message ID 0x3A */ 
  qmi_dsdi_wifi_meas_report_config_status, /* Request handler for message ID 0x3B */
  qmi_dsdi_wifi_meas_report,               /* Request handler for message ID 0x3C */
  qmi_dsdi_set_apn_call_type,              /* Request handler for message ID 0x3D */
  qmi_dsdi_set_wqe_profile_type,           /* Request handler for message ID 0x3E */
  NULL,                                    /* Request handler for message ID 0x3F */
  NULL,                                    /* Request handler for message ID 0x40 */
  NULL,                                    /* Request handler for message ID 0x41 */
  qmi_dsdi_nat_keep_alive_config_status,   /* Request handler for message ID 0x42 */
  qmi_dsdi_wqe_profile_quality_status,     /* Request handler for message ID 0x43 */
  qmi_dsdi_set_quality_measurement_info,   /* Request handler for message ID 0x44 */
  NULL,                                    /* Request handler for message ID 0x45 */ 
  NULL,                                    /* Request handler for message ID 0x46 */
  qmi_dsdi_get_roaming_status_info         /* Request handler for message ID 0x47 */  
};

extern rex_tcb_type dcc_tcb;

static int dsdi_compare_qmi_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_dsdi_client_info_type *clnt_info = (qmi_dsdi_client_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
}

static int dsdi_compare_connection_info_handle
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

#ifdef FEATURE_DATA_WLAN_MAPCON
static void qmi_dsdi_system_status_ind
(
  void                                  * client,
  dsd_system_status_ind_msg_v01         * system_status
);

static void dsdi_system_status_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

static void qmi_dsdi_generate_system_status_ind
(
  dsd_system_status_ind_msg_v01  * system_status,
  ps_sys_subscription_enum_type  subscription_id
);
#endif /* FEATURE_DATA_WLAN_MAPCON */

static void qmi_dsdi_reg_sys_events(void);

static boolean qmi_dsdi_verify_client_subs
(
  qmi_dsdi_client_info_type        *clnt_info,
  uint32                            event_subs_id
);

static uint32 qmi_dsdi_resolve_client_subs
(
  qmi_dsdi_client_info_type        *clnt_info
);
/*===========================================================================

                       EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_DSD_INIT()

  DESCRIPTION
    Register the DSD service with QCSI 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dsd_init
(
  void
)
{
  qmi_csi_error         rc;
  qmi_sap_error         src;
  qmi_csi_options       options;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1("QMI DSD service init %d", qmi_dsdi_inited);

  if (!qmi_dsdi_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)dcc_set_sig_handler(DCC_DSD_RDR_SIGNAL, 
                              qmi_dsdi_msg_hdlr_cb, &qmi_dsd_state);
    dcc_enable_sig(DCC_DSD_RDR_SIGNAL);

    /* Set the signal to init again */
    DCC_SET_SIGNAL(DCC_DSD_RDR_SIGNAL);
    return;
  }

  os_params.tcb = &dcc_tcb;
  os_params.sig = DSDI_QCSI_OS_SIGNAL;
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);
  
  /*-------------------------------------------------------------------------
    Register QMI DSD service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register_with_options(dsd_get_service_object_v01(),
                        qmi_dsdi_svc_connect_cb,
                        qmi_dsdi_svc_disconnect_cb, 
                        qmi_dsdi_handle_client_req_cb, 
                        &qmi_dsd_state, &os_params, 
                        &options,
                        &qmi_dsd_state.service_handle);
  
  LOG_MSG_INFO1_1("qmi_csi_register %d", rc);
  if(rc != QMI_CSI_NO_ERR)
  {
    ERR_FATAL("Unable to register DSD service! Error %d", rc, 0, 0);
    return;
  }

  //register with SAP
  src = qmi_sap_register(dsd_get_service_object_v01(), NULL, &qmi_dsd_state.sap_client_handle);
  if (src != QMI_SAP_NO_ERR)
  {
    ERR_FATAL("Unable to register DSD service with QSAP! Error %d", src, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
    Register at power up for interested System Events.
  -------------------------------------------------------------------------*/
  qmi_dsdi_reg_sys_events();

  /* Initialize the queue which will hold all handles of clients */
  (void) q_init(&qmi_dsd_state.client_q);
  qmi_dsd_state.num_of_clients = 0;

  /*-------------------------------------------------------------------------
    Setting QMI DSD service command handler with DCC task process
  -------------------------------------------------------------------------*/
  dcc_set_cmd_handler(DCC_DSD_SVC_CMD, qmi_dsdi_process_cmd);

} /* qmi_dsd_init */


static boolean qmi_dsdi_msg_hdlr_cb
(
  dcc_sig_enum_type  sig,
  void              *svc_ptr
)
{
  qmi_dsdi_state_info_type *dsd_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT((sig == DCC_DSD_RDR_SIGNAL) && svc_ptr);

  if (!qmi_dsdi_inited)
  {
    qmi_dsdi_inited = TRUE;
    /* Call init again to register with QCSI now
     * Wait for all tasks to be inited */
    qmi_dsd_init();
    return TRUE;
  }

  dsd_sp = (qmi_dsdi_state_info_type*) svc_ptr;

  qmi_csi_handle_event(dsd_sp->service_handle, &os_params);
  return TRUE;
} /* qmi_dsdi_msg_hdlr_cb */

static qmi_csi_cb_error qmi_dsdi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_dsdi_client_info_type    *clnt_info_ptr;
  qmi_dsdi_state_info_type     *dsd_svc_ptr;
  qmi_csi_cb_error              rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT(connection_handle && service_handle );

  dsd_svc_ptr = (qmi_dsdi_state_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &dsd_svc_ptr->client_q,
                                dsdi_compare_qmi_client_handle,
                                client_handle ) )
  {
    LOG_MSG_ERROR_1("dsdi_svc_connect_cb: DSD client 0x%p present",
                    client_handle);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  if (DSDI_MAX_CLIDS == dsd_svc_ptr->num_of_clients)
  {
    LOG_MSG_ERROR_1("dsdi_svc_connect_cb: Maximum number of clients already allocated %d",
                    DSDI_MAX_CLIDS);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(clnt_info_ptr, sizeof(qmi_dsdi_client_info_type),
                           qmi_dsdi_client_info_type*);
  if(!clnt_info_ptr)
  {
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  memset(clnt_info_ptr, 0, sizeof(qmi_dsdi_client_info_type));
  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;
  clnt_info_ptr->subscription_id = (DSD_PRIMARY_SUBS_V01 - 1);
  /* Add it to the queue of clients */
  q_put( &(dsd_svc_ptr->client_q), &(clnt_info_ptr->link) );
  dsd_svc_ptr->num_of_clients++;

  *connection_handle = clnt_info_ptr;
  LOG_MSG_INFO1_1("dsdi_svc_connect_cb: Alloc client 0x%p", clnt_info_ptr);
  rc = QMI_CSI_CB_NO_ERR;
bail:
  return rc;
} /* qmi_dsdi_svc_connect_cb() */

static void qmi_dsdi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_dsdi_state_info_type    *dsd_svc_ptr;
  qmi_dsdi_client_info_type    *clnt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle && service_handle);
  dsd_svc_ptr = (qmi_dsdi_state_info_type *)service_handle;

  LOG_MSG_INFO1_1("dsdi_svc_disconnect_cb: Releasing client 0x%p",
                  connection_handle);

  clnt_info_ptr = q_linear_search( &dsd_svc_ptr->client_q,
                                   dsdi_compare_connection_info_handle,
                                   connection_handle );
  if (NULL == clnt_info_ptr)
  {
    LOG_MSG_ERROR_1("dsdi_svc_connect_cb: DSD client 0x%p not found",
                    connection_handle);
    return;

  }

  if ( TRUE != q_delete_ext( &dsd_svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
  {
    LOG_MSG_ERROR_1("dsdi_svc_connect_cb: DSD client 0x%p could not be deleted from queue",
                    clnt_info_ptr);
    return;
  }
  PS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  dsd_svc_ptr->num_of_clients--;
} /* qmi_dsdi_svc_disconnect_cb() */


static qmi_csi_cb_error qmi_dsdi_handle_client_req_cb
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
  qmi_dsdi_client_info_type     *clnt_info_ptr;
  qmi_dsdi_state_info_type      *dsd_svc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle && service_handle);

  rc = QMI_CSI_CB_INTERNAL_ERR;
  dsd_svc_ptr = (qmi_dsdi_state_info_type*) service_handle;

  clnt_info_ptr = q_linear_search( &dsd_svc_ptr->client_q,
                                   dsdi_compare_connection_info_handle,
                                   connection_handle );
  if (NULL == clnt_info_ptr)
  {
    LOG_MSG_ERROR_1("dsdi_handle_req_cb: Invalid clnt 0x%p",
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
      LOG_MSG_ERROR_1("dsdi_handle_req_cb: NULL message ID handler: %d",
                      msg_id);
    }
  }
  else
  {
    LOG_MSG_ERROR_1("dsdi_handle_req_cb: Invalid message ID: %d", msg_id);
  }

  return rc;
} /* qmi_dsdi_handle_client_req_cb() */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
  FUNCTION QMI_DSDI_WLAN_AVAILABLE()

  DESCRIPTION
    Indicate that WLAN is available.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_wlan_available
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_wlan_available_req_msg_v01   *req_ptr;
  dsd_wlan_available_resp_msg_v01   resp_msg;

  int16                  ps_errno  = 0;
  uint16                 ps_result;
  boolean                is_default_route = TRUE;

  ps_sys_wlan_status_type wlan_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);

  req_ptr = (dsd_wlan_available_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1_1 ("Client 0x%p indicating WLAN available", clnt_info);

  memset(&resp_msg, 0, sizeof(dsd_wlan_available_resp_msg_v01));
  memset(&wlan_status, 0, sizeof(ps_sys_wlan_status_type));

  if (req_ptr->is_default_route_valid)
  {
    is_default_route = req_ptr->is_default_route;
  }
  /*---------------------------------------------------------------------
    Validate some input TLVs
      set is_wlan_available to TRUE, only if the Internet connectivity is verified by APSS(CNE)
      Even if IP is available, check for the connection_status as IP_CONNECTED.
  ---------------------------------------------------------------------*/
  if( (req_ptr->wlan_ipv4_address_valid || req_ptr->wlan_ipv6_address_valid) && 
        (req_ptr->connection_status_valid && req_ptr->connection_status == PS_SYS_WLAN_CONN_STATUS_IP_CONNECTED) &&
        (is_default_route == TRUE))
  {
    wlan_status.is_wlan_available = TRUE;
  }
  else
  {
    wlan_status.is_wlan_available = FALSE;
  }

  memscpy(&wlan_status.ap_mac_addr, 
          sizeof(wlan_status.ap_mac_addr),
          &req_ptr->wlan_ap_mac_address, 
          sizeof(wlan_status.ap_mac_addr));
  if (req_ptr->wlan_ipv4_address_valid)
  {
    wlan_status.ipv4_addr.type = IPV4_ADDR;
    /* Convert to network byte order before sending down */
    wlan_status.ipv4_addr.addr.v4.ps_s_addr = ps_htonl(req_ptr->wlan_ipv4_address);
    IPV4_ADDR_MSG(wlan_status.ipv4_addr.addr.v4.ps_s_addr);
  }
  else
  {
    wlan_status.ipv4_addr.type = IP_ADDR_INVALID;
  }
  if (req_ptr->wlan_ipv6_address_valid)
  {
    wlan_status.ipv6_prefix_len = req_ptr->wlan_ipv6_address.prefix_len;
    wlan_status.ipv6_addr.type = IPV6_ADDR;
    memscpy(&wlan_status.ipv6_addr.addr.v6.in6_u.u6_addr8, 
            sizeof(wlan_status.ipv6_addr.addr.v6.in6_u.u6_addr8),
            &req_ptr->wlan_ipv6_address.ipv6_address, 
           sizeof(wlan_status.ipv6_addr.addr.v6.in6_u.u6_addr8));
    IPV6_ADDR_MSG(wlan_status.ipv6_addr.addr.v6.in6_u.u6_addr64);
    LOG_MSG_INFO1_1("IPV6 prefix len %d", wlan_status.ipv6_prefix_len );
  }
  else
  {
    wlan_status.ipv6_addr.type = IP_ADDR_INVALID;
  }
  wlan_status.wqe_status = PS_SYS_WLAN_WQE_INACTIVE; /*Default */
  if (req_ptr->wqe_status_valid)
  {
    /* validate the parameter */
    if ((req_ptr->wqe_status < DSD_WQE_INACTIVE_V01 ) ||
        (req_ptr->wqe_status > DSD_WQE_ACTIVE_V01 ))
    {
      LOG_MSG_ERROR_1("Invalid WQE status %d", req_ptr->wqe_status );
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                        sizeof(dsd_wlan_available_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
    wlan_status.wqe_status = (ps_sys_wlan_wqe_status_enum_type)req_ptr->wqe_status;
    LOG_MSG_INFO1_1("WQE status %d", wlan_status.wqe_status );
  }

  /* DNS IPV4 address 1 */
  if (req_ptr->dns_ipv4_address_1_valid)
  {
    wlan_status.dns_ipv4_addr1.type = IPV4_ADDR;
    /* Convert to network byte order before sending down */
    wlan_status.dns_ipv4_addr1.addr.v4.ps_s_addr = ps_htonl(req_ptr->dns_ipv4_address_1);
    LOG_IPV4_ADDR_MSG_EX("DNS IPV4 Addr 1", wlan_status.dns_ipv4_addr1.addr.v4.ps_s_addr);
  }
  else
  {
    wlan_status.dns_ipv4_addr1.type = IP_ADDR_INVALID;
  }

  /* DNS IPV4 address 2 */
  if (req_ptr->dns_ipv4_address_2_valid)
  {
    wlan_status.dns_ipv4_addr2.type = IPV4_ADDR;
    /* Convert to network byte order before sending down */
    wlan_status.dns_ipv4_addr2.addr.v4.ps_s_addr = ps_htonl(req_ptr->dns_ipv4_address_2);
    LOG_IPV4_ADDR_MSG_EX("DNS IPV4 Addr 2", wlan_status.dns_ipv4_addr2.addr.v4.ps_s_addr);
  }
  else
  {
    wlan_status.dns_ipv4_addr2.type = IP_ADDR_INVALID;
  }

  /* DNS IPV6 address 1 */
  if (req_ptr->dns_ipv6_address_1_valid)
  {
    wlan_status.dns_ipv6_addr1.type = IPV6_ADDR;
    memscpy(&wlan_status.dns_ipv6_addr1.addr.v6.in6_u.u6_addr8, 
            sizeof(wlan_status.dns_ipv6_addr1.addr.v6.in6_u.u6_addr8),
            &req_ptr->dns_ipv6_address_1, 
           sizeof(req_ptr->dns_ipv6_address_1));
    LOG_IPV6_ADDR_MSG_EX("DNS IPV6 Addr 1", wlan_status.dns_ipv6_addr1.addr.v6.in6_u.u6_addr64);
  }
  else
  {
    wlan_status.dns_ipv6_addr1.type = IP_ADDR_INVALID;
  }

  /* DNS IPV6 address 2 */
  if (req_ptr->dns_ipv6_address_2_valid)
  {
    wlan_status.dns_ipv6_addr2.type = IPV6_ADDR;
    memscpy(&wlan_status.dns_ipv6_addr2.addr.v6.in6_u.u6_addr8, 
            sizeof(wlan_status.dns_ipv6_addr2.addr.v6.in6_u.u6_addr8),
            &req_ptr->dns_ipv6_address_2, 
           sizeof(req_ptr->dns_ipv6_address_2));
    LOG_IPV6_ADDR_MSG_EX("DNS IPV6 Addr 2", wlan_status.dns_ipv6_addr2.addr.v6.in6_u.u6_addr64);
  }
  else
  {
    wlan_status.dns_ipv6_addr2.type = IP_ADDR_INVALID;
  }


  /* EPDG IPV4 address 1 */
  if (req_ptr->epdg_ipv4_address_1_valid)
  {
    wlan_status.epdg_ipv4_addr1.type = IPV4_ADDR;
    /* Convert to network byte order before sending down */
    wlan_status.epdg_ipv4_addr1.addr.v4.ps_s_addr = ps_htonl(req_ptr->epdg_ipv4_address_1);
    LOG_IPV4_ADDR_MSG_EX("EPDG IPV4 Addr 1", wlan_status.epdg_ipv4_addr1.addr.v4.ps_s_addr);
  }
  else
  {
    wlan_status.epdg_ipv4_addr1.type = IP_ADDR_INVALID;
  }

  /* EPDG IPV4 address 2 */
  if (req_ptr->epdg_ipv4_address_2_valid)
  {
    wlan_status.epdg_ipv4_addr2.type = IPV4_ADDR;
    /* Convert to network byte order before sending down */
    wlan_status.epdg_ipv4_addr2.addr.v4.ps_s_addr = ps_htonl(req_ptr->epdg_ipv4_address_2);
    LOG_IPV4_ADDR_MSG_EX("EPDG IPV4 Addr 2", wlan_status.epdg_ipv4_addr2.addr.v4.ps_s_addr);
  }
  else
  {
    wlan_status.epdg_ipv4_addr2.type = IP_ADDR_INVALID;
  }

  /* EPDG IPV6 address 1 */
  if (req_ptr->epdg_ipv6_address_1_valid)
  {
    wlan_status.epdg_ipv6_addr1.type = IPV6_ADDR;
    memscpy(&wlan_status.epdg_ipv6_addr1.addr.v6.in6_u.u6_addr8, 
            sizeof(wlan_status.epdg_ipv6_addr1.addr.v6.in6_u.u6_addr8),
            &req_ptr->epdg_ipv6_address_1, 
           sizeof(req_ptr->epdg_ipv6_address_1));
    LOG_IPV6_ADDR_MSG_EX("EPDG IPV6 Addr 1", wlan_status.epdg_ipv6_addr1.addr.v6.in6_u.u6_addr64);
  }
  else
  {
    wlan_status.epdg_ipv6_addr1.type = IP_ADDR_INVALID;
  }

  /* EPDG IPV6 address 2 */
  if (req_ptr->epdg_ipv6_address_2_valid)
  {
    wlan_status.epdg_ipv6_addr2.type = IPV6_ADDR;
    memscpy(&wlan_status.epdg_ipv6_addr2.addr.v6.in6_u.u6_addr8, 
            sizeof(wlan_status.epdg_ipv6_addr2.addr.v6.in6_u.u6_addr8),
            &req_ptr->epdg_ipv6_address_2, 
           sizeof(req_ptr->epdg_ipv6_address_2));
    LOG_IPV6_ADDR_MSG_EX("EPDG IPV6 Addr 2", wlan_status.epdg_ipv6_addr2.addr.v6.in6_u.u6_addr64);
  }
  else
  {
    wlan_status.epdg_ipv6_addr2.type = IP_ADDR_INVALID;
  }

  /* SSID */
  if (req_ptr->ssid_valid)
  {
    memscpy(&wlan_status.ssid, sizeof(wlan_status.ssid),
            &req_ptr->ssid, req_ptr->ssid_len);
    /* Null terminate */
    wlan_status.ssid[MIN(req_ptr->ssid_len, sizeof(wlan_status.ssid)-1)] = '\0';
    LOG_MSG_INFO1_1("SSID %s", wlan_status.ssid );
  }

  /* Channel*/
  wlan_status.is_channel_valid = req_ptr->channel_valid;
  if (req_ptr->channel_valid)
  {
    wlan_status.channel = req_ptr->channel;
    LOG_MSG_INFO1_1("WLAN_STATUS.CHANNEL  %d", wlan_status.channel);
  }


  /*Channel Bandwidth*/
  wlan_status.is_channel_bandwidth_valid = 
    req_ptr->channel_bandwidth_valid;
  if (req_ptr->channel_bandwidth_valid)
  {
    wlan_status.channel_bandwidth = req_ptr->channel_bandwidth;
    LOG_MSG_INFO1_1("Channel bandwidth %d", wlan_status.channel_bandwidth);
  }

  /*Connection status Enum*/
  if (req_ptr->connection_status_valid)
  {
    wlan_status.connection_status = 
      (ps_sys_wlan_conn_status_type_enum)(req_ptr->connection_status);
       LOG_MSG_INFO1_1("Connection status %d", wlan_status.connection_status);
  }
  else
  {
    wlan_status.connection_status = 
      PS_SYS_WLAN_CONN_STATUS_INVALID;
  }

  /*WQE Profile type*/
  if (req_ptr->wqe_profile_type_valid)
  {
    wlan_status.wqe_profile_type = req_ptr->wqe_profile_type;
    LOG_MSG_INFO1_1("WQE profile type 0x%llx ",  wlan_status.wqe_profile_type);
  }
  else
  {
    //default is inactive
    wlan_status.wqe_profile_type = 0;
  }

  /* Network enum 
     Keep the ps sys and dsd network mode the same
  */
  if (req_ptr->network_mode_valid)
  {
    wlan_status.network_mode = 
      (ps_sys_wlan_network_mode_type_enum)req_ptr->network_mode;
    LOG_MSG_INFO1_1("Network enum %d",  wlan_status.network_mode);
  }
  else
  {
     //Default value
    wlan_status.network_mode = PS_SYS_WLAN_NETWORK_MODE_INVALID;
  }

  /* Wlan Wifi association type */
  if (req_ptr->wifi_assoc_type_valid)
  {
    wlan_status.wlan_assoc_type = 
      (ps_sys_wlan_assoc_type_enum)req_ptr->wifi_assoc_type;
    LOG_MSG_INFO1_1("WLAN_ASSOC_TYPE %d",  wlan_status.wlan_assoc_type);
  }
  else
  {
    //Default Value
    wlan_status.wlan_assoc_type = 
      PS_SYS_WLAN_ASSOC_INVALID;
  }

  /*Default Route*/
  wlan_status.is_default_route = is_default_route;

  ps_result = ps_sys_conf_set_ex(PS_SYS_TECH_WLAN, PS_SYS_CONF_WLAN_STATUS,
                                 (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                              (void *)&wlan_status, &ps_errno);

  if (ps_result)
  {
    LOG_MSG_ERROR_1("PS sys config failed for WLAN status available:%d",
                    ps_errno);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(dsd_wlan_available_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  } 

  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_wlan_available_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_wlan_available() */

/*===========================================================================
  FUNCTION QMI_DSDI_WLAN_NOT_AVAILABLE()

  DESCRIPTION
    Indicate that WLAN is not available.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_wlan_not_available
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_wlan_not_available_req_msg_v01   *req_ptr;
  dsd_wlan_not_available_resp_msg_v01   resp_msg;

  int16                  ps_errno  = 0;
  uint16                 ps_result;

  ps_sys_wlan_status_type wlan_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);

  req_ptr = (dsd_wlan_not_available_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1_1 ("Client 0x%p indicating WLAN not available", clnt_info);

  memset(&resp_msg, 0, sizeof(dsd_wlan_not_available_resp_msg_v01));
  memset(&wlan_status, 0, sizeof(ps_sys_wlan_status_type));

  wlan_status.is_wlan_available = FALSE;

  wlan_status.wqe_status = PS_SYS_WLAN_WQE_INACTIVE; /*Default */
  if (req_ptr->wqe_status_valid)
  {
    /* validate the parameter */
    if ((req_ptr->wqe_status < DSD_WQE_INACTIVE_V01 ) ||
        (req_ptr->wqe_status > DSD_WQE_ACTIVE_V01 ))
    {
      LOG_MSG_ERROR_1("Invalid WQE status %d", req_ptr->wqe_status );
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                        sizeof(dsd_wlan_available_resp_msg_v01));
      return QMI_CSI_CB_NO_ERR;
    }
    wlan_status.wqe_status = (ps_sys_wlan_wqe_status_enum_type)req_ptr->wqe_status;
    LOG_MSG_INFO1_1("WQE status %d", wlan_status.wqe_status );
  }
  ps_result = ps_sys_conf_set_ex(PS_SYS_TECH_WLAN, PS_SYS_CONF_WLAN_STATUS, 
                                 (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                              (void *)&wlan_status, &ps_errno);

  if (ps_result)
  {
    LOG_MSG_ERROR_1("PS sys config failed for WLAN status not available:%d",
                    ps_errno);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(dsd_wlan_not_available_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  } 

  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_wlan_not_available_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_wlan_not_available() */

/*===========================================================================
  FUNCTION QMI_DSDI_WLAN_SET_WLAN_PREFERENCE()

  DESCRIPTION
    Indicate preference for WLAN LB.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_set_wlan_preference
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_set_wlan_preference_req_msg_v01   *req_ptr;
  dsd_set_wlan_preference_resp_msg_v01   resp_msg;

  int16                  ps_errno  = 0;
  uint16                 ps_result;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);

  req_ptr = (dsd_set_wlan_preference_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1_1 ("Client 0x%p indicating WLAN preference", clnt_info);

  memset(&resp_msg, 0, sizeof(dsd_set_wlan_preference_resp_msg_v01));

  /* validate the parameter */
  if ((req_ptr->wlan_preference < DSD_WLAN_LB_PREFERRED_V01 ) ||
      (req_ptr->wlan_preference > DSD_WLAN_LB_NOT_PREFERRED_V01 ))
  {
    LOG_MSG_ERROR_1("Invalid WLAN preference %d", req_ptr->wlan_preference );
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(dsd_set_wlan_preference_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }
  LOG_MSG_INFO1_1("WLAN preference %d", req_ptr->wlan_preference );
  ps_result = ps_sys_conf_set_ex(PS_SYS_TECH_WLAN, PS_SYS_CONF_WLAN_PREFERENCE, 
                                 (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                              (void *)&req_ptr->wlan_preference, &ps_errno);

  if (ps_result)
  {
    LOG_MSG_ERROR_1("PS sys config failed for set WLAN preference:%d",
                    ps_errno);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(dsd_set_wlan_preference_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  } 

  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_set_wlan_preference_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_set_wlan_preference() */

/*===========================================================================
  FUNCTION QMI_DSDI_WLAN_GET_WLAN_PREFERENCE()

  DESCRIPTION
    Returns current WLAN preference.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_get_wlan_preference
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_get_wlan_preference_resp_msg_v01   resp_msg;

  int16                  ps_errno  = 0;
  uint16                 ps_result;

  ps_sys_wlan_preference_enum_type wlan_pref;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(clnt_info && service_handle);

  LOG_MSG_INFO1_1 ("Client 0x%p querying WLAN preference", clnt_info);

  memset(&resp_msg, 0, sizeof(dsd_get_wlan_preference_resp_msg_v01));
  memset(&wlan_pref, 0, sizeof(ps_sys_wlan_preference_enum_type));

  ps_result = ps_sys_conf_get_ex(PS_SYS_TECH_WLAN, PS_SYS_CONF_WLAN_PREFERENCE, 
                                 (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                              (void *)&wlan_pref, &ps_errno);

  if (ps_result)
  {
    LOG_MSG_ERROR_1("PS sys config get failed for WLAN preference:%d",
                    ps_errno);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(dsd_get_wlan_preference_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  } 

  LOG_MSG_INFO1_1("WLAN preference %d", wlan_pref );

  resp_msg.wlan_preference_valid = TRUE;
  resp_msg.wlan_preference = (dsd_wlan_preference_enum_v01) wlan_pref;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_get_wlan_preference_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_get_wlan_preference() */

/*===========================================================================
  FUNCTION QMI_DSDI_SYSTEM_STATUS_CHANGE_REQ()

  DESCRIPTION
    Registers system status event for interested control point.

  PARAMETERS
    clnt_info          : Client info pointer
    req_handle         : Request handle
    msg_id             : Message Id
    req_c_struct       : Incoming request 
    req_c_struct_len   : incoming request length
    service_handle     : Service hadle

  RETURN VALUE
    QMI_RESULT_SUCCESS in case of suscess and QMI_RESULT_FAILURE in case
    of failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_system_status_change_req
(
  qmi_dsdi_client_info_type       * clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                            * req_c_struct,
  int                               req_c_struct_len,
  void                            * service_handle
)
{
  qmi_dsdi_state_info_type                * dsd_sp;
  dsd_system_status_change_req_msg_v01    * req_ptr;
  dsd_system_status_change_resp_msg_v01    resp_msg;
  qmi_error_type_v01     errval;
  qmi_result_type_v01    result;
  qmi_dsdi_client_info_type  * clnt_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1("qmi_dsdi_system_status_change_req: msg_id %d ", msg_id);

  ASSERT(clnt_info && service_handle );
  
  errval = QMI_ERR_NONE_V01;
  memset(&resp_msg, 0, sizeof(dsd_system_status_change_resp_msg_v01));  
  req_ptr = (dsd_system_status_change_req_msg_v01 *)req_c_struct;
  
  dsd_sp = (qmi_dsdi_state_info_type *) service_handle;
  clnt_info_ptr = q_linear_search( &dsd_sp->client_q,
                                   dsdi_compare_connection_info_handle,
                                   clnt_info );
                                   
  if(clnt_info_ptr == NULL || req_ptr == NULL)
  {
    LOG_MSG_ERROR_2("No client or req found ,clnt_info_ptr=0x%x req_ptr=0x%x",
                         clnt_info_ptr,req_ptr);
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }               

  if ( req_ptr->report_data_system_status_changes_valid == TRUE)
  {
    if(req_ptr->report_data_system_status_changes)
    {
      clnt_info_ptr->report_status.report_system_status = TRUE;
    }
    else
    {
      clnt_info_ptr->report_status.report_system_status = FALSE;
    }
  }
  else /*Legacy behavior to send status change by default*/
  {
    clnt_info_ptr->report_status.report_system_status = TRUE;
  }

  if(req_ptr->limit_so_mask_change_ind_valid)
  {
    if(req_ptr->limit_so_mask_change_ind)
    {
      clnt_info_ptr->report_status.limit_so_mask_change_report_ind = TRUE;
    }
    else
    {
      clnt_info_ptr->report_status.limit_so_mask_change_report_ind = FALSE;
    }
  }
  else
  {
    /*Default value of limit_so_mask_change_report_ind is false*/
    clnt_info_ptr->report_status.limit_so_mask_change_report_ind = FALSE;
  }

send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.result = result;
  resp_msg.resp.error  = errval;  

  LOG_MSG_INFO2_2("qmi_dsdi_system_status_change_req result %d errval %d",
                   result, errval);     

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_system_status_change_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_system_status_change_req() */

/*===========================================================================
  FUNCTION QMI_DSDI_SYSTEM_STATUS_IND()

  DESCRIPTION
    This function sends system status indication to registered control point.

  PARAMETERS
    clid             : clientID
    system_status    : System Status

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dsdi_system_status_ind
(
  void                                  * client,
  dsd_system_status_ind_msg_v01         * system_status
)
{
  qmi_dsdi_client_info_type               * client_info;
  qmi_csi_error                             rc;
  uint32                                       cnt_network;
  uint32                                       cnt_apn;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_0("qmi_dsdi_system_status_ind ");
  
  rc = QMI_CSI_NO_ERR;
  client_info = (qmi_dsdi_client_info_type *) client;
  if (client_info == NULL)
  {
    DATA_ERR_FATAL("qmi_dsdi_system_status_ind : Null client_info");
    return;
  }
  
  LOG_MSG_INFO2_4("qmi_dsdi_system_status_ind ,"
                  "num of rats %d num of apns %d"
                  "avail_sys_valid %d apn_avail_sys_info_valid %d",
                  system_status->avail_sys_len,
                  system_status->apn_avail_sys_info_len,
                  system_status->avail_sys_valid,
                  system_status->apn_avail_sys_info_valid);
                  
  for (cnt_network=0; cnt_network < system_status->avail_sys_len; 
       cnt_network++)
  {
    LOG_MSG_INFO2_3("qmi_dsdi_system_status_ind "
                    "technology %d rat value %d so mask 0x%x",
                    system_status->avail_sys[cnt_network].technology,
                    system_status->avail_sys[cnt_network].rat_value,
                    system_status->avail_sys[cnt_network].so_mask);
  }
  
  for (cnt_apn=0; cnt_apn < system_status->apn_avail_sys_info_len; 
       cnt_apn++)
  { 
    LOG_MSG_INFO2_2("qmi_dsdi_system_status_ind , apn name %s apn avail sys len %d ",
                    system_status->apn_avail_sys_info[cnt_apn].apn_name,
                    system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys_len);

    for (cnt_network=0; cnt_network < system_status-> \
         apn_avail_sys_info[cnt_apn].apn_avail_sys_len; cnt_network++)
    {
      LOG_MSG_INFO2_4("qmi_dsdi_system_status_ind apn sys info[%d]"
                      "technology %d rat value %d so mask 0x%x",
                      cnt_network,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].technology,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].rat_value,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].so_mask);
    }    
  }
            
  rc = qmi_csi_send_ind(client_info->clnt, 
                        QMI_DSD_SYSTEM_STATUS_IND_V01,
                        system_status, 
                        sizeof(dsd_system_status_ind_msg_v01));
  
  if ( rc != QMI_CSI_NO_ERR )
  {
    LOG_MSG_ERROR_1("qmi_dsdi_system_status_ind error %d", rc);
  }

} /* qmi_dsdi_system_status_ind() */


/*===========================================================================
  FUNCTION QMI_DSDI_GET_SYSTEM_STATUS()

  DESCRIPTION
    Retrieves current system status and returns it to requesting control 
    point

  PARAMETERS
    clnt_info          : Client info pointer
    req_handle         : Request handle
    msg_id             : Message Id
    req_c_struct       : Incoming request 
    req_c_struct_len   : incoming request length
    service_handle     : Service hadle

  RETURN VALUE
    QMI_RESULT_SUCCESS in case of suscess and QMI_RESULT_FAILURE in case
    of failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_get_system_status
( 
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
)
{
  qmi_error_type_v01            errval;
  qmi_result_type_v01           result;
  ps_sys_system_status_ex_type        *conf_system_status;
  int16                         ps_errno = 0;
  uint32                           cnt_network;
  dsd_get_system_status_resp_msg_v01 *resp_msg;
  uint32                           cnt_apn;
  uint8                            recommended_pref;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1("qmi_dsdi_get_system_status : msg_id %d ", msg_id);

  ASSERT(clnt_info && service_handle);

  PS_SYSTEM_HEAP_MEM_ALLOC(resp_msg,
                          sizeof(dsd_get_system_status_resp_msg_v01),
                          dsd_get_system_status_resp_msg_v01*);
   if (NULL == resp_msg)
   {
     return QMI_CSI_CB_NO_MEM;
   }
  
  memset(resp_msg, 0, sizeof(dsd_get_system_status_resp_msg_v01));
  
  PS_SYSTEM_HEAP_MEM_ALLOC(conf_system_status,
                           sizeof(ps_sys_system_status_ex_type),
                           ps_sys_system_status_ex_type*);
  
   if (NULL == conf_system_status)
   {
      errval = QMI_ERR_NO_MEMORY_V01;
      goto send_result;
   }

  memset(conf_system_status, 0, sizeof(ps_sys_system_status_ex_type));
  
  errval = QMI_ERR_NONE_V01;

  /*-------------------------------------------------------------------------
    Retrieve data system status by calling into PS Sys Layer
  -------------------------------------------------------------------------*/
  result = ps_sys_conf_get_ex( PS_SYS_TECH_ALL,
                            PS_SYS_CONF_SYSTEM_STATUS_EX,
                               (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                            (void *)conf_system_status,
                            &ps_errno);
  if(result < 0)
  {
    LOG_MSG_ERROR_1("ps_sys_conf_get operation returned : (%d)", ps_errno);
    switch (ps_errno)
    {                                   
      case DS_NOMEMORY:
        errval = QMI_ERR_NO_MEMORY_V01;
        break;

      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    } /*  switch (ps_errno) */
    
    goto send_result;
  }
  
  LOG_MSG_INFO2_2("qmi_dsdi_get_system_status ,"
                  "num of rats %d num of apns %d",
                  conf_system_status->num_avail_sys,
                  conf_system_status->num_avail_apns);
  
  resp_msg->avail_sys_len =
    (uint32)conf_system_status->num_avail_sys;  

  if(resp_msg->avail_sys_len > 0)
  {
    resp_msg->avail_sys_valid = TRUE; 
    resp_msg->global_pref_sys_valid = TRUE;
  }
  else
  {
    resp_msg->avail_sys_valid = FALSE; 
    resp_msg->global_pref_sys_valid = FALSE;
  }  
  
  recommended_pref = 
    conf_system_status->recommended_pref_system_index;
  if (recommended_pref >
        conf_system_status->num_avail_sys)
  {
    LOG_MSG_ERROR_2("Recommended Index is out of bounds %d, num sys %d", 
                    recommended_pref, conf_system_status->num_avail_sys);
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }  
    
  resp_msg->apn_avail_sys_info_len = 
    (uint32)conf_system_status->num_avail_apns;  
  resp_msg->apn_pref_sys_len = 
    (uint32)conf_system_status->num_avail_apns; 


  if(resp_msg->apn_avail_sys_info_len > 0)
  {
    resp_msg->apn_avail_sys_info_valid = TRUE; 
    resp_msg->apn_pref_sys_valid = TRUE;
  }
  else
  {
    resp_msg->apn_avail_sys_info_valid = FALSE; 
    resp_msg->apn_pref_sys_valid = FALSE;
  } 
                  
  for (cnt_network=0; cnt_network < conf_system_status->num_avail_sys; 
       cnt_network++)
  {
    resp_msg->avail_sys[cnt_network].technology = (dsd_sys_network_enum_v01)
      conf_system_status->avail_sys[cnt_network].technology;
    resp_msg->avail_sys[cnt_network].rat_value = (dsd_sys_rat_ex_enum_v01)
      conf_system_status->avail_sys[cnt_network].rat_value;
    resp_msg->avail_sys[cnt_network].so_mask =
      conf_system_status->avail_sys[cnt_network].so_mask;

    LOG_MSG_INFO2_3("qmi_dsdi_get_system_status "
                    "technology %d rat value %d so mask 0x%x",
                    resp_msg->avail_sys[cnt_network].technology,
                    resp_msg->avail_sys[cnt_network].rat_value,
                    resp_msg->avail_sys[cnt_network].so_mask);
  }
  
  resp_msg->global_pref_sys.curr_pref_sys.technology = (dsd_sys_network_enum_v01)
    conf_system_status->avail_sys[0].technology;
  resp_msg->global_pref_sys.curr_pref_sys.rat_value = (dsd_sys_rat_ex_enum_v01)
    conf_system_status->avail_sys[0].rat_value;
  resp_msg->global_pref_sys.curr_pref_sys.so_mask =
    conf_system_status->avail_sys[0].so_mask;

   LOG_MSG_INFO2_3("qmi_dsdi_get_system_status global current"
                    "pref technology %d rat value %d so mask 0x%x",
                    resp_msg->global_pref_sys.curr_pref_sys.technology,
                    resp_msg->global_pref_sys.curr_pref_sys.rat_value,
                    resp_msg->global_pref_sys.curr_pref_sys.so_mask);

  resp_msg->global_pref_sys.recommended_pref_sys.technology = 
    (dsd_sys_network_enum_v01)
    conf_system_status->avail_sys[recommended_pref].technology;
  resp_msg->global_pref_sys.recommended_pref_sys.rat_value = (dsd_sys_rat_ex_enum_v01)
    conf_system_status->avail_sys[recommended_pref].rat_value;
  resp_msg->global_pref_sys.recommended_pref_sys.so_mask =
    conf_system_status->avail_sys[recommended_pref].so_mask;

   LOG_MSG_INFO2_3("qmi_dsdi_get_system_status global recommended"
                    "pref technology %d rat value %d so mask 0x%x",
                    resp_msg->global_pref_sys.recommended_pref_sys.technology,
                    resp_msg->global_pref_sys.recommended_pref_sys.rat_value,
                    resp_msg->global_pref_sys.recommended_pref_sys.so_mask);
  
  for (cnt_apn=0; cnt_apn < conf_system_status->num_avail_apns; 
       cnt_apn++)
  { 
    memscpy(&(resp_msg->apn_avail_sys_info[cnt_apn].apn_name),
            sizeof(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name),
           &(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name),
           sizeof(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name));        

    resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys_len =
      (uint32)conf_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys;

    LOG_MSG_INFO2_2("qmi_dsdi_get_system_status , apn name %s apn avail sys len %d",
                    conf_system_status->apn_pref_sys_info[cnt_apn].apn_name,
                    conf_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys);

    for (cnt_network=0; cnt_network < conf_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys; cnt_network++)
    {
      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].technology = 
        (dsd_sys_network_enum_v01) 
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].technology;

      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].rat_value = 
        (dsd_sys_rat_ex_enum_v01)
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].rat_value;

      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].so_mask =
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].so_mask;          

      LOG_MSG_INFO2_4("qmi_dsdi_get_system_status apn sys info[%d]"
                      "technology %d rat value %d so mask 0x%x",
                      cnt_network,
                      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].technology,
                      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].rat_value,
                      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].so_mask);
    }    
  }

  for (cnt_apn=0; cnt_apn < conf_system_status->num_avail_apns; 
       cnt_apn++)
  { 
    memscpy(&(resp_msg->apn_pref_sys[cnt_apn].apn_name),
            sizeof(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name),
           &(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name),
           sizeof(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name));        

    //Current preferred system topmost element
    resp_msg->apn_pref_sys[cnt_apn].curr_pref_sys.technology = 
        (dsd_sys_network_enum_v01) 
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].technology;

      resp_msg->apn_pref_sys[cnt_apn].curr_pref_sys.rat_value = 
        (dsd_sys_rat_ex_enum_v01)
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].rat_value;

      resp_msg->apn_pref_sys[cnt_apn].curr_pref_sys.so_mask =
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].so_mask;  
      
      recommended_pref = 
        conf_system_status->apn_pref_sys_info[cnt_apn].recommended_pref_system_index;

    if (recommended_pref >
          conf_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys)
    {
      LOG_MSG_ERROR_2("Recommended Index is out of bounds %d, num sys %d", 
                      recommended_pref, 
                      conf_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys);
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }

    //Recommended preferred system.
     resp_msg->apn_pref_sys[cnt_apn].recommended_pref_sys.technology = 
        (dsd_sys_network_enum_v01) 
        conf_system_status->apn_pref_sys_info[cnt_apn].\
       avail_sys[recommended_pref].technology;

      resp_msg->apn_pref_sys[cnt_apn].recommended_pref_sys.rat_value = 
        (dsd_sys_rat_ex_enum_v01)
        conf_system_status->apn_pref_sys_info[cnt_apn].\
        avail_sys[recommended_pref].rat_value;

      resp_msg->apn_pref_sys[cnt_apn].recommended_pref_sys.so_mask =
        conf_system_status->apn_pref_sys_info[cnt_apn].\
        avail_sys[recommended_pref].so_mask;  
  }



send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);

  resp_msg->resp.result = result;
  resp_msg->resp.error  = errval; 

  LOG_MSG_INFO2_2("qmi_dsdi_get_system_status result %d errval %d ",
                  result, errval);
                 
  qmi_csi_send_resp(req_handle, 
                    msg_id, 
                    resp_msg, 
                    sizeof(dsd_get_system_status_resp_msg_v01));
                    
  if(conf_system_status != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(conf_system_status);
  }

  if(resp_msg != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(resp_msg);
  }

  return QMI_CSI_CB_NO_ERR;
} /* qmi_dsdi_get_system_status() */

/*===========================================================================
  FUNCTION dsdi_system_status_change_cb()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    system status change event. 

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    subscription_id: Subscription id
    event_info_ptr : System status event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void dsdi_system_status_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_dsdi_cmd_buf_type           * cmd_ptr = NULL;
  dsd_system_status_ind_msg_v01   * dsd_system_status;
  ps_sys_system_status_ex_type    * sys_system_status;
  uint32                               cnt_network;
  uint32                               cnt_apn;
  uint8                             recommend_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_4("dsdi_system_status_change_cb tech_type %d event_name %d"
                  " event_info_ptr 0x%x Subscription id %d",
                  tech_type, event_name, event_info_ptr,subscription_id);
                 
  ASSERT(event_info_ptr);

  sys_system_status = (ps_sys_system_status_ex_type *)event_info_ptr;
 
  cmd_ptr = (qmi_dsdi_cmd_buf_type *)qmi_dsdi_get_cmd_buf();

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI DSD cmd buf");
    return;
  }
  
  memset(cmd_ptr,0,sizeof(qmi_dsdi_cmd_buf_type));

  dsd_system_status = &cmd_ptr->data.system_status;
  
  LOG_MSG_INFO2_2("dsdi_system_status_change_cb ,"
                  "num of rats %d num of apns %d",
                  sys_system_status->num_avail_sys,
                  sys_system_status->num_avail_apns);
  
  dsd_system_status->avail_sys_len =
    (uint32)sys_system_status->num_avail_sys;  

  if(dsd_system_status->avail_sys_len > 0)
  {
    dsd_system_status->avail_sys_valid = TRUE; 
    dsd_system_status->global_pref_sys_valid = TRUE;
  }
  else
  {
    dsd_system_status->avail_sys_valid = FALSE; 
    dsd_system_status->global_pref_sys_valid = FALSE;
  }  
    
  dsd_system_status->apn_avail_sys_info_len = 
    (uint32)sys_system_status->num_avail_apns;  

  if(dsd_system_status->apn_avail_sys_info_len > 0)
  {
    dsd_system_status->apn_avail_sys_info_valid = TRUE; 
  }
  else
  {
    dsd_system_status->apn_avail_sys_info_valid = FALSE; 
  } 
                  

  dsd_system_status->apn_pref_sys_len = 
    (uint32)sys_system_status->num_avail_apns; 
 
  if(dsd_system_status->apn_pref_sys_len > 0)
  {
    dsd_system_status->apn_pref_sys_valid = TRUE; 
  }
  else
  {
    dsd_system_status->apn_pref_sys_valid = FALSE; 
  } 
                  
  recommend_index = sys_system_status->recommended_pref_system_index;

  if (recommend_index > sys_system_status->num_avail_sys)
  {
    LOG_MSG_ERROR_2("Recommended Index is out of bounds %d, num sys %d", 
                    recommend_index,
                    sys_system_status->num_avail_sys);
    goto bail;
  }
                  
  for (cnt_network=0; cnt_network < sys_system_status->num_avail_sys; 
       cnt_network++)
  {
    LOG_MSG_INFO2_3("dsdi_system_status_change_cb "
                    "technology %d rat value %d so mask 0x%x",
                    sys_system_status->avail_sys[cnt_network].technology,
                    sys_system_status->avail_sys[cnt_network].rat_value,
                    sys_system_status->avail_sys[cnt_network].so_mask);
                  
    dsd_system_status->avail_sys[cnt_network].technology = (dsd_sys_network_enum_v01)
      sys_system_status->avail_sys[cnt_network].technology;

    dsd_system_status->avail_sys[cnt_network].rat_value = (dsd_sys_rat_ex_enum_v01)
      sys_system_status->avail_sys[cnt_network].rat_value;

    dsd_system_status->avail_sys[cnt_network].so_mask =
      sys_system_status->avail_sys[cnt_network].so_mask;
  }
  
  dsd_system_status->global_pref_sys.curr_pref_sys.technology = 
    (dsd_sys_network_enum_v01)
      sys_system_status->avail_sys[0].technology;
  dsd_system_status->global_pref_sys.curr_pref_sys.rat_value = 
    (dsd_sys_rat_ex_enum_v01)
      sys_system_status->avail_sys[0].rat_value;
  dsd_system_status->global_pref_sys.curr_pref_sys.so_mask = 
      sys_system_status->avail_sys[0].so_mask;


  dsd_system_status->global_pref_sys.recommended_pref_sys.technology = 
    (dsd_sys_network_enum_v01)
      sys_system_status->avail_sys[recommend_index].technology;
  dsd_system_status->global_pref_sys.recommended_pref_sys.rat_value = 
    (dsd_sys_rat_ex_enum_v01)
      sys_system_status->avail_sys[recommend_index].rat_value;
  dsd_system_status->global_pref_sys.recommended_pref_sys.so_mask = 
      sys_system_status->avail_sys[recommend_index].so_mask;
  
  for (cnt_apn=0; cnt_apn < sys_system_status->num_avail_apns; 
       cnt_apn++)
  { 
    LOG_MSG_INFO2_1("dsdi_system_status_change_cb , apn name %s ",
                    sys_system_status->apn_pref_sys_info[cnt_apn].apn_name);
      
    memscpy(&dsd_system_status->apn_avail_sys_info[cnt_apn].apn_name,
            sizeof(dsd_system_status->apn_avail_sys_info[cnt_apn].apn_name),
           &sys_system_status->apn_pref_sys_info[cnt_apn].apn_name,
           strlen(sys_system_status->apn_pref_sys_info[cnt_apn].apn_name));           

    dsd_system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys_len =
      sys_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys;

    LOG_MSG_INFO2_1("dsdi_system_status_change_cb , apn avail sys len %d ",
                    sys_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys);

    for (cnt_network=0; cnt_network < sys_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys; cnt_network++)
    {
      LOG_MSG_INFO2_4("dsdi_system_status_change_cb apn sys info[%d]"
                      "technology %d rat value %d so mask 0x%x",
                      cnt_network,
                      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].technology,
                      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].rat_value,
                      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].so_mask);
                    
      dsd_system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].technology =
        (dsd_sys_network_enum_v01)
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].technology;

      dsd_system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].rat_value =
        (dsd_sys_rat_ex_enum_v01)
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].rat_value;

      dsd_system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].so_mask =
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].so_mask;          
    }    
  }

  for (cnt_apn=0; cnt_apn < sys_system_status->num_avail_apns; 
       cnt_apn++)
  { 
    LOG_MSG_INFO2_1("dsdi_system_status_change_cb , apn name %s ",
                    sys_system_status->apn_pref_sys_info[cnt_apn].apn_name);
      
    memscpy(&dsd_system_status->apn_pref_sys[cnt_apn].apn_name,
            sizeof(dsd_system_status->apn_pref_sys[cnt_apn].apn_name),
           &sys_system_status->apn_pref_sys_info[cnt_apn].apn_name,
           strlen(sys_system_status->apn_pref_sys_info[cnt_apn].apn_name));  

    //Current
    dsd_system_status->apn_pref_sys[cnt_apn].curr_pref_sys.technology =
      (dsd_sys_network_enum_v01)
      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].technology;

    dsd_system_status->apn_pref_sys[cnt_apn].curr_pref_sys.rat_value =
      (dsd_sys_rat_ex_enum_v01)
      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].rat_value;

    dsd_system_status->apn_pref_sys[cnt_apn].curr_pref_sys.so_mask =
      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].so_mask; 


    
    recommend_index = 
      sys_system_status->apn_pref_sys_info[cnt_apn].recommended_pref_system_index;

    if (recommend_index > 
          sys_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys)
    {
      LOG_MSG_ERROR_2("Recommended Index is out of bounds %d, num_sys %d", 
                      recommend_index,
                      sys_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys);
      goto bail;
    }

    dsd_system_status->apn_pref_sys[cnt_apn].recommended_pref_sys.technology =
        (dsd_sys_network_enum_v01)
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[recommend_index].technology;

       dsd_system_status->apn_pref_sys[cnt_apn].recommended_pref_sys.rat_value =
        (dsd_sys_rat_ex_enum_v01)
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[recommend_index].rat_value;

      dsd_system_status->apn_pref_sys[cnt_apn].recommended_pref_sys.so_mask =
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[recommend_index].so_mask; 
  }

  cmd_ptr->cmd_id = QMI_CMD_DSD_SYSTEM_STATUS_CHANGE_EVT;
  cmd_ptr->subscription_id = subscription_id;

  dcc_send_cmd_ex (DCC_DSD_SVC_CMD, cmd_ptr);
  return;

bail:
  if (cmd_ptr)
  {
    PS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
  }
} /* dsdi_system_status_change_cb() */


/*===========================================================================
FUNCTION  QMI_WDSI_GENERATE_SYSTEM_STATUS_IND

DESCRIPTION
  This callback is called to notify clients about data system status changes.

PARAMETERS
  system_status - System status info like preferred network, rat mask,
                  so mask.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void qmi_dsdi_generate_system_status_ind
(
  dsd_system_status_ind_msg_v01 * system_status,
  ps_sys_subscription_enum_type  subscription_id
)
{
  qmi_dsdi_client_info_type                   * cl_sp;
  uint8                                         cnt_sys;
  uint8                                         cnt_apn;
  q_type                                      * client_q_ptr = NULL;
  void                                        * client_buf_ptr;
  void                                        * next_client_buf_ptr;
  qmi_dsdi_state_info_type                    * dsd_sp;
  boolean                                       diag_event_reported = FALSE;
  ps_sys_system_status_diag_event_logging_type  system_status_log;
  boolean                                       is_tech_rat_same = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&system_status_log, 0, sizeof(system_status_log));
  
  if(system_status == NULL)
  {
    ASSERT(0);
  }
  LOG_MSG_INFO2_4("qmi_dsdi_generate_system_status_ind ,"
                  "num of rats %d num of apns %d"
                  "avail_sys_valid %d apn_avail_sys_info_valid %d",
                  system_status->avail_sys_len,
                  system_status->apn_avail_sys_info_len,
                  system_status->avail_sys_valid,
                  system_status->apn_avail_sys_info_valid);
                  
  for (cnt_sys=0; cnt_sys < system_status->avail_sys_len; 
       cnt_sys++)
  {
    LOG_MSG_INFO2_3("qmi_dsdi_generate_system_status_ind "
                    "technology %d rat value %d so mask 0x%x",
                    system_status->avail_sys[cnt_sys].technology,
                    system_status->avail_sys[cnt_sys].rat_value,
                    system_status->avail_sys[cnt_sys].so_mask);
  }
  
    /* check if technology or rat value has changed*/ 
   if((qmi_dsdi_last_avail_sys_pref_network.technology != 
                 (ps_sys_network_enum_type)system_status->avail_sys[0].technology)||
       (qmi_dsdi_last_avail_sys_pref_network.rat_value != 
                  (ps_sys_rat_ex_enum_type)system_status->avail_sys[0].rat_value ))
    {
      is_tech_rat_same = FALSE;
    }
    else
    {
      LOG_MSG_INFO2_2("SO mask changed from %x to %x",
      qmi_dsdi_last_avail_sys_pref_network.so_mask,
      system_status->avail_sys[0].so_mask);
    }

  /* Copy the last preferred network tuple into global variable */
  qmi_dsdi_last_avail_sys_pref_network.technology = 
             (ps_sys_network_enum_type)system_status->avail_sys[0].technology;
  qmi_dsdi_last_avail_sys_pref_network.rat_value  = 
              (ps_sys_rat_ex_enum_type)system_status->avail_sys[0].rat_value;
  qmi_dsdi_last_avail_sys_pref_network.so_mask    = 
                               (uint64)system_status->avail_sys[0].so_mask;

  for (cnt_apn=0; cnt_apn < system_status->apn_avail_sys_info_len; 
       cnt_apn++)
  { 
    LOG_MSG_INFO2_2("qmi_dsdi_generate_system_status_ind , apn name %s apn avail sys len %d",
                    system_status->apn_avail_sys_info[cnt_apn].apn_name,
                    system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys_len);

    for (cnt_sys=0; cnt_sys < system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys_len; cnt_sys++)
    {
      LOG_MSG_INFO2_4("qmi_dsdi_generate_system_status_ind apn sys info[%d]"
                      "technology %d rat value %d so mask 0x%x",
                      cnt_sys,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_sys].technology,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_sys].rat_value,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_sys].so_mask);
    }    
  }

  dsd_sp         = &qmi_dsd_state;
  client_q_ptr   = &dsd_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);
  
  while(client_buf_ptr != NULL)
  {
    cl_sp = (qmi_dsdi_client_info_type *) client_buf_ptr;
    
    if ((TRUE == cl_sp->report_status.report_system_status) &&
        (TRUE == qmi_dsdi_verify_client_subs(cl_sp, (uint32)subscription_id)) )
    {    
      /* If limit_so_mask_change_report_ind is FALSE, send the indication, 
         If limit_so_mask_change_report_ind is TRUE, send the indication only
         when technology value or rat mask has changed */
      if((FALSE == cl_sp->report_status.limit_so_mask_change_report_ind) ||
        ((TRUE ==  cl_sp->report_status.limit_so_mask_change_report_ind) &&
        (FALSE == is_tech_rat_same)))
      {

        /* This flag is checked to send the diag event only once for all the clients */
        if(diag_event_reported == FALSE)
        {
          /* Diag event for system status ex indication */
          system_status_log.pref_network = system_status->avail_sys[0].technology;
          system_status_log.rat_value    = system_status->avail_sys[0].rat_value;
          system_status_log.so_mask      = system_status->avail_sys[0].so_mask;
          system_status_log.subs_id      = subscription_id;
          /* Set the flag to true so that the event is not generated for all the clients */
          diag_event_reported            = TRUE;
          event_report_payload( EVENT_QMI_SYSTEM_STATUS_EX, 
                                sizeof(ps_sys_system_status_diag_event_logging_type),
                                (void*) &system_status_log);
        }
        qmi_dsdi_system_status_ind(cl_sp,
                                   system_status
                                  );
      }
    }
    
    next_client_buf_ptr = 
      q_next(client_q_ptr, &((q_generic_item_type *) client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
  
} /* qmi_dsdi_generate_system_status_ind */
#endif /* FEATURE_DATA_WLAN_MAPCON */

/*===========================================================================
  FUNCTION QMI_DSDI_GET_CMD_BUF()

  DESCRIPTION
    Allocate and assign a QMI DSD command buffer from the PS MEM heap
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
static void *qmi_dsdi_get_cmd_buf
(
)
{
  qmi_dsdi_cmd_buf_type *cmd_buf_ptr = NULL;
  PS_SYSTEM_HEAP_MEM_ALLOC(cmd_buf_ptr, sizeof(qmi_dsdi_cmd_buf_type),
                            qmi_dsdi_cmd_buf_type*);
  return ((void*)cmd_buf_ptr);
} /* qmi_dsdi_get_cmd_buf */


/*===========================================================================
  FUNCTION qmi_dsdi_process_cmd()

  DESCRIPTION
    This function processes a QMI DSD command

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI DSD command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI DSD must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dsdi_process_cmd
(
  dcc_cmd_enum_type  cmd,
  void              *cmd_ptr
)
{
  qmi_dsdi_cmd_buf_type *cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);
  cmd_buf_ptr = (qmi_dsdi_cmd_buf_type *)cmd_ptr;

  LOG_MSG_INFO1_2("qmi_dsdi_process_cmd: cmdbuf's cmd_id:%d  cmd %d",
                  cmd_buf_ptr->cmd_id, cmd);

  switch(cmd_buf_ptr->cmd_id)
  {
#ifdef FEATURE_DATA_WLAN_MAPCON
    case QMI_CMD_DSD_SYSTEM_STATUS_CHANGE_EVT:
      qmi_dsdi_generate_system_status_ind
      (
        &(cmd_buf_ptr->data.system_status),
        cmd_buf_ptr->subscription_id
      );
      break;
#endif /* FEATURE_DATA_WLAN_MAPCON */

    case QMI_CMD_DSD_THERMAL_INFO_CHANGE_EVT:
      qmi_dsdi_generate_thermal_info_ind
      (
        &(cmd_buf_ptr->data.thermal_info),
        cmd_buf_ptr->subscription_id
      );
      break;

    case QMI_CMD_DSD_START_WIFI_MEAS_EVT:
      qmi_dsdi_generate_start_wifi_meas_ind
      (
        &(cmd_buf_ptr->data.start_wifi_meas_info),
        cmd_buf_ptr->subscription_id
      );
      break;

    case QMI_CMD_DSD_STOP_WIFI_MEAS_EVT:
      qmi_dsdi_generate_stop_wifi_meas_ind
      (
        &(cmd_buf_ptr->data.stop_wifi_meas_info),
        cmd_buf_ptr->subscription_id
      );
      break;

    case QMI_CMD_DSD_WQE_PROFILE_EVT:
      qmi_dsdi_generate_wqe_profile_type_ind
      (
        &(cmd_buf_ptr->data.wqe_profile_info),
        cmd_buf_ptr->subscription_id
      );
      break;

    case QMI_CMD_DSD_WQE_PROFILE_MET_EVT:
      qmi_dsdi_generate_wqe_profile_type_met_ind
      (
        &(cmd_buf_ptr->data.wqe_profile_met_info),
        cmd_buf_ptr->subscription_id
      );
      break;

    case QMI_CMD_DSD_NAT_KEEP_ALIVE_EVT:
      qmi_dsdi_generate_nat_keep_alive_ind
      (
        &(cmd_buf_ptr->data.nat_keep_alive_info),
        cmd_buf_ptr->subscription_id
      );
      break;

    case QMI_CMD_DSD_WQE_PROFILE_INIT_EVT:
      qmi_dsdi_generate_wqe_profile_init_ind
      (
        &(cmd_buf_ptr->data.wqe_profile_init_info),
        cmd_buf_ptr->subscription_id
      );
      break;

    case QMI_CMD_DSD_ROAMING_STATUS_CHANGE_EVT:
      qmi_dsdi_generate_roaming_status_change_ind
      (
        &(cmd_buf_ptr->data.roaming_status_info),
        cmd_buf_ptr->subscription_id
      );
      break;      

    default:
      ASSERT(0);
      
  }

  PS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
  return;
} /* qmi_dsdi_process_cmd() */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
  FUNCTION QMI_DSDI_WLAN_SET_APN_PREFERENCE_SYSTEM()

  DESCRIPTION
    This API allows client to set the highest technology preference for APN 

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_set_apn_preference_system
(
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
)
{
  qmi_csi_cb_error                             rc;
  dsd_set_apn_preferred_system_req_msg_v01   * req_ptr;
  dsd_set_apn_preferred_system_resp_msg_v01    resp_msg;
  ps_sys_apn_pref_sys_type                     apn_pref;

  int16                  ps_errno  = 0;
  int16                  ps_result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("qmi_dsdi_set_apn_preference_system : Client 0x%p "
                   "Setting APN preference", clnt_info);

  ASSERT(clnt_info && service_handle);

  memset(&resp_msg, 0, sizeof(dsd_set_apn_preferred_system_resp_msg_v01));
  
  rc = QMI_CSI_CB_INTERNAL_ERR;

  if(NULL == req_c_struct)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error  = QMI_ERR_MISSING_ARG_V01;
    goto send_result;
  }

  if(req_c_struct_len < 0)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error =  QMI_ERR_MALFORMED_MSG_V01;
    goto send_result;
  }
  
  req_ptr = (dsd_set_apn_preferred_system_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1_2("qmi_dsdi_set_apn_preference_system : apn_name %s "
                  "pref_sys %d ",
                  req_ptr->apn_pref_sys.apn_name,
                  req_ptr->apn_pref_sys.pref_sys);
  
  memscpy (&apn_pref.apn, 
           sizeof(req_ptr->apn_pref_sys.apn_name),
          &req_ptr->apn_pref_sys.apn_name, 
          sizeof(req_ptr->apn_pref_sys.apn_name));
          
  apn_pref.pref_sys = req_ptr->apn_pref_sys.pref_sys;
  
  ps_result = ps_sys_conf_set_ex (PS_SYS_TECH_ALL, 
                               PS_SYS_CONF_APN_PREF_SYS, 
                                  (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                              (void *)&apn_pref, 
                              &ps_errno);
                              
  if(ps_result < 0)
  {
    LOG_MSG_ERROR_1("qmi_dsdi_set_apn_preference_system : ps_sys_conf_set "
                    "operation returned : (%d)", ps_errno);
                  
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    
    switch (ps_errno)
    {                                   
      case DS_NOMEMORY:
        resp_msg.resp.error = QMI_ERR_NO_MEMORY_V01;
        break;
      
      case DS_EFAULT:
        resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
        break;
        
      default:
        resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
        break;
    } /*  switch (ps_errno) */
    
    goto send_result;
  }

  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  rc = QMI_CSI_CB_NO_ERR;

send_result:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_set_apn_preferred_system_resp_msg_v01));

  return rc;

} /* qmi_dsdi_set_apn_preference_system() */

/*===========================================================================
  FUNCTION dsdi_map_pdn_db_err_to_qmi()

  DESCRIPTION
    This function maps MPPM PDN db error code to QMI error code

  PARAMETERS
    txn_ret_code: Transaction return code
    failure_code: Specific failure code
    result      : QMI success or failure
    error       : QMI error code

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void dsdi_map_pdn_db_err_to_qmi
(
  ds_mppm_pdn_db_txn_ret_code_type txn_ret_code,
  ds_mppm_failure_code_type        failure_code,
  qmi_result_type_v01              * result,
  qmi_error_type_v01               * error
)
{
  switch (txn_ret_code)
  {
    case DS_MPPM_PDN_DB_TXN_SUCCESS:
      *result = QMI_RESULT_SUCCESS_V01;
      *error = QMI_ERR_NONE_V01;
      break;
    case DS_MPPM_PDN_DB_TXN_IN_PROGRESS:
      *result = QMI_RESULT_FAILURE_V01;
      *error = QMI_ERR_DEVICE_IN_USE_V01;
      break;
    case DS_MPPM_PDN_DB_TXN_INVALID_ID:
      *result = QMI_RESULT_FAILURE_V01;
      *error = QMI_ERR_INVALID_TX_ID_V01;
      break;
    case DS_MPPM_PDN_DB_TXN_FAILED:
      *result = QMI_RESULT_FAILURE_V01;
      switch (failure_code)
      {
        case DS_MPPM_PDN_DB_APN_ALREADY_EXISTS:
        case DS_MPPM_PDN_DB_APN_DOES_NOT_EXIST:
          *error = QMI_ERR_INVALID_INDEX_V01;
          break;
        case DS_MPPM_PDN_DB_MAX_PDN_CONN_REACHED:
          *error = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
          break;
        case DS_MPPM_PDN_DB_INVALID_APN_NAME:
        case DS_MPPM_PDN_DB_INVALID_RAT_PRIORITY_LIST:
          *error = QMI_ERR_INVALID_ARG_V01;
          break;
        default:
          *error = QMI_ERR_INTERNAL_V01;         
       } /* switch (failure_code) */
      break;
    default:
      *result = QMI_RESULT_FAILURE_V01;
      *error = QMI_ERR_INTERNAL_V01;
  }
}

/*===========================================================================
  FUNCTION QMI_DSDI_PDN_POLICY_START_TXN()

  DESCRIPTION
    Start a transaction to access PDN Policy DB.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_pdn_policy_start_txn
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_pdn_policy_start_txn_resp_msg_v01   qmi_resp_msg;
  ds_mppm_pdn_db_start_txn_request_s_type pdn_req;
  ds_mppm_pdn_db_start_txn_response_s_type pdn_resp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(clnt_info && service_handle);

  LOG_MSG_INFO1_1 ("Client 0x%p starting transaction", clnt_info);

  memset(&qmi_resp_msg, 0, sizeof(dsd_pdn_policy_start_txn_resp_msg_v01));
  memset(&pdn_req, 0, sizeof(ds_mppm_pdn_db_start_txn_request_s_type));
  memset(&pdn_resp, 0, sizeof(ds_mppm_pdn_db_start_txn_response_s_type));

  pdn_req.subscription_id = PS_SYS_PRIMARY_SUBS; //Would need to populate from client state
                                                 //once subscription binding is supported

  if (ds_mppm_pdn_db_start_txn(&pdn_req, &pdn_resp))
  {
    if (pdn_resp.txn_ret_code == DS_MPPM_PDN_DB_TXN_SUCCESS)
    {
      qmi_resp_msg.txn_id_valid = TRUE;
      qmi_resp_msg.txn_id = pdn_resp.txn_id;
    }
    dsdi_map_pdn_db_err_to_qmi(pdn_resp.txn_ret_code,
                               pdn_resp.failure_code,
                               &qmi_resp_msg.resp.result, 
                               &qmi_resp_msg.resp.error);
  }
  else
  {
    LOG_MSG_ERROR_0("Start txn failed");
    qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
  }

  qmi_csi_send_resp(req_handle, msg_id, &qmi_resp_msg, 
                    sizeof(dsd_pdn_policy_start_txn_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_pdn_policy_start_txn() */


/*===========================================================================
  FUNCTION QMI_DSDI_ADD_PDN_POLICY()

  DESCRIPTION
    Adds a row to PDN Policy DB.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_add_pdn_policy
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_add_pdn_policy_req_msg_v01          *qmi_req_ptr;
  dsd_add_pdn_policy_resp_msg_v01         qmi_resp_msg;
  ds_mppm_pdn_db_add_policy_request_s_type pdn_req;
  ds_mppm_pdn_db_add_policy_response_s_type pdn_resp;
  uint32 itr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(clnt_info && service_handle && req_c_struct && req_c_struct_len > 0);

  LOG_MSG_INFO1_1 ("Client 0x%p adding a row to PDN DB", clnt_info);

  qmi_req_ptr = (dsd_add_pdn_policy_req_msg_v01*)req_c_struct;

  memset(&qmi_resp_msg, 0, sizeof(dsd_add_pdn_policy_resp_msg_v01));
  memset(&pdn_req, 0, sizeof(ds_mppm_pdn_db_add_policy_request_s_type));
  memset(&pdn_resp, 0, sizeof(ds_mppm_pdn_db_add_policy_response_s_type));

  pdn_req.txn_id = qmi_req_ptr->txn_id; 

  memscpy(pdn_req.pdn_db_entry.apn_name.apn_string, sizeof(pdn_req.pdn_db_entry.apn_name.apn_string),
          &qmi_req_ptr->apn_name, strlen(qmi_req_ptr->apn_name));
  pdn_req.pdn_db_entry.apn_name.apn_string_len = MIN(strlen(qmi_req_ptr->apn_name), 
                                                     sizeof(pdn_req.pdn_db_entry.apn_name.apn_string));

  pdn_req.pdn_db_entry.supported_rat_priority_list_present = TRUE;
  pdn_req.pdn_db_entry.supported_rat_priority_list.rat_priority_list_len = qmi_req_ptr->supported_system_priority_list_len;
  for (itr=0;itr<MIN(qmi_req_ptr->supported_system_priority_list_len,PDN_CONFIG_MAX_SUPPORTED_RATS);itr++)
  {
    if (qmi_req_ptr->supported_system_priority_list[itr] > DSD_APN_PREF_SYS_IWLAN_V01)
    {
      qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      qmi_resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }
    pdn_req.pdn_db_entry.supported_rat_priority_list.ds_mppm_rat_list[itr] = 
      (ds_mppm_pdn_db_rat_type)qmi_req_ptr->supported_system_priority_list[itr];
  }

  if (qmi_req_ptr->is_default_valid)
  {
    pdn_req.pdn_db_entry.default_apn_present = TRUE;
    pdn_req.pdn_db_entry.default_apn = qmi_req_ptr->is_default ? TRUE : FALSE;
  }

  if (qmi_req_ptr->override_type_valid)
  {
    if (qmi_req_ptr->override_type > DSD_PDN_POLICY_OVERRIDE_OPTION_V01)
    {
      qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      qmi_resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }
    pdn_req.pdn_db_entry.override_type_present = TRUE;
    pdn_req.pdn_db_entry.override_type = (ds_mppm_override_type)qmi_req_ptr->override_type;
  }

  if (ds_mppm_pdn_db_add_policy(&pdn_req, &pdn_resp))
  {
    dsdi_map_pdn_db_err_to_qmi(pdn_resp.txn_ret_code,
                               pdn_resp.failure_code,
                               &qmi_resp_msg.resp.result, 
                               &qmi_resp_msg.resp.error);
  }
  else
  {
    LOG_MSG_ERROR_0("Add PDN db failed");
    qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
  }

send_result:
  qmi_csi_send_resp(req_handle, msg_id, &qmi_resp_msg, 
                    sizeof(dsd_add_pdn_policy_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_add_pdn_policy() */


/*===========================================================================
  FUNCTION QMI_DSDI_MODIFY_PDN_POLICY()

  DESCRIPTION
    Modifies a row in the PDN Policy DB.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_modify_pdn_policy
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_modify_pdn_policy_req_msg_v01          *qmi_req_ptr;
  dsd_modify_pdn_policy_resp_msg_v01         qmi_resp_msg;
  ds_mppm_pdn_db_modify_policy_request_s_type pdn_req;
  ds_mppm_pdn_db_modify_policy_response_s_type pdn_resp;
  uint32 itr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(clnt_info && service_handle && req_c_struct && req_c_struct_len > 0);

  LOG_MSG_INFO1_1 ("Client 0x%p modifying a row in PDN DB", clnt_info);

  qmi_req_ptr = (dsd_modify_pdn_policy_req_msg_v01*)req_c_struct;

  memset(&qmi_resp_msg, 0, sizeof(dsd_modify_pdn_policy_resp_msg_v01));
  memset(&pdn_req, 0, sizeof(ds_mppm_pdn_db_modify_policy_request_s_type));
  memset(&pdn_resp, 0, sizeof(ds_mppm_pdn_db_modify_policy_response_s_type));

  pdn_req.txn_id = qmi_req_ptr->txn_id; 

  memscpy(pdn_req.pdn_db_entry.apn_name.apn_string, sizeof(pdn_req.pdn_db_entry.apn_name.apn_string),
          &qmi_req_ptr->apn_name, strlen(qmi_req_ptr->apn_name));
  pdn_req.pdn_db_entry.apn_name.apn_string_len = MIN(strlen(qmi_req_ptr->apn_name), 
                                                     sizeof(pdn_req.pdn_db_entry.apn_name.apn_string));

  if (qmi_req_ptr->supported_system_priority_list_valid)
  {
    pdn_req.pdn_db_entry.supported_rat_priority_list_present = TRUE;
    pdn_req.pdn_db_entry.supported_rat_priority_list.rat_priority_list_len = qmi_req_ptr->supported_system_priority_list_len;
    for (itr=0;itr<MIN(qmi_req_ptr->supported_system_priority_list_len,PDN_CONFIG_MAX_SUPPORTED_RATS);itr++)
    {
      if (qmi_req_ptr->supported_system_priority_list[itr] > DSD_APN_PREF_SYS_IWLAN_V01)
      {
        qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
        qmi_resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
        goto send_result;
      }
      pdn_req.pdn_db_entry.supported_rat_priority_list.ds_mppm_rat_list[itr] = 
        (ds_mppm_pdn_db_rat_type)qmi_req_ptr->supported_system_priority_list[itr];
    }
  }

  if (qmi_req_ptr->is_default_valid)
  {
    pdn_req.pdn_db_entry.default_apn_present = TRUE;
    pdn_req.pdn_db_entry.default_apn = qmi_req_ptr->is_default ? TRUE : FALSE;
  }

  if (qmi_req_ptr->override_type_valid)
  {
    if (qmi_req_ptr->override_type > DSD_PDN_POLICY_OVERRIDE_OPTION_V01)
    {
      qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      qmi_resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }
    pdn_req.pdn_db_entry.override_type_present = TRUE;
    pdn_req.pdn_db_entry.override_type = (ds_mppm_override_type)qmi_req_ptr->override_type;
  }

  if (ds_mppm_pdn_db_modify_policy_settings(&pdn_req, &pdn_resp))
  {
    dsdi_map_pdn_db_err_to_qmi(pdn_resp.txn_ret_code,
                               pdn_resp.failure_code,
                               &qmi_resp_msg.resp.result, 
                               &qmi_resp_msg.resp.error);
  }
  else
  {
    LOG_MSG_ERROR_0("Modify PDN db failed");
    qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
  }
send_result:
  qmi_csi_send_resp(req_handle, msg_id, &qmi_resp_msg, 
                    sizeof(dsd_modify_pdn_policy_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_modify_pdn_policy() */


/*===========================================================================
  FUNCTION QMI_DSDI_DELETE_PDN_POLICY_BY_APN()

  DESCRIPTION
    Deletes a row in the PDN Policy DB.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_delete_pdn_policy_by_apn
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_delete_pdn_policy_by_apn_req_msg_v01          *qmi_req_ptr;
  dsd_delete_pdn_policy_by_apn_resp_msg_v01         qmi_resp_msg;
  ds_mppm_pdn_db_delete_policy_request_s_type pdn_req;
  ds_mppm_pdn_db_delete_policy_response_s_type pdn_resp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(clnt_info && service_handle && req_c_struct && req_c_struct_len > 0);

  LOG_MSG_INFO1_1 ("Client 0x%p deleting a row in PDN DB", clnt_info);

  qmi_req_ptr = (dsd_delete_pdn_policy_by_apn_req_msg_v01*)req_c_struct;

  memset(&qmi_resp_msg, 0, sizeof(dsd_delete_pdn_policy_by_apn_resp_msg_v01));
  memset(&pdn_req, 0, sizeof(ds_mppm_pdn_db_delete_policy_request_s_type));
  memset(&pdn_resp, 0, sizeof(ds_mppm_pdn_db_delete_policy_response_s_type));

  pdn_req.txn_id = qmi_req_ptr->txn_id; 

  memscpy(pdn_req.apn_name.apn_string, sizeof(pdn_req.apn_name.apn_string),
          &qmi_req_ptr->apn_name, strlen(qmi_req_ptr->apn_name));
  pdn_req.apn_name.apn_string_len = MIN(strlen(qmi_req_ptr->apn_name), 
                                        sizeof(pdn_req.apn_name.apn_string));

  if (ds_mppm_pdn_db_delete_policy_by_apn(&pdn_req, &pdn_resp))
  {
    dsdi_map_pdn_db_err_to_qmi(pdn_resp.txn_ret_code,
                               pdn_resp.failure_code,
                               &qmi_resp_msg.resp.result, 
                               &qmi_resp_msg.resp.error);
  }
  else
  {
    LOG_MSG_ERROR_0("Delete PDN db failed");
    qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
  }

  qmi_csi_send_resp(req_handle, msg_id, &qmi_resp_msg, 
                    sizeof(dsd_delete_pdn_policy_by_apn_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_delete_pdn_policy_by_apn() */

/*===========================================================================
  FUNCTION QMI_DSDI_GET_PDN_POLICY_APN_LIST()

  DESCRIPTION
    Retrieves a list of APNs from the PDN Policy DB.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_get_pdn_policy_apn_list
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_get_pdn_policy_apn_list_req_msg_v01          *qmi_req_ptr;
  dsd_get_pdn_policy_apn_list_resp_msg_v01         *qmi_resp_msg = NULL;
  ds_mppm_pdn_db_get_apn_list_request_s_type pdn_req;
  ds_mppm_pdn_db_get_apn_list_response_s_type  *pdn_resp = NULL;
  uint32 itr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(clnt_info && service_handle && req_c_struct && req_c_struct_len > 0);

  LOG_MSG_INFO1_1 ("Client 0x%p querying list of APNs from PDN DB", clnt_info);

  qmi_req_ptr = (dsd_get_pdn_policy_apn_list_req_msg_v01*)req_c_struct;

   PS_SYSTEM_HEAP_MEM_ALLOC(qmi_resp_msg,
                  sizeof(dsd_get_pdn_policy_apn_list_resp_msg_v01),
                            dsd_get_pdn_policy_apn_list_resp_msg_v01*);

   if (NULL == qmi_resp_msg)
   {
      return (qmi_csi_cb_error)QMI_CSI_NO_MEM;
   }

   PS_SYSTEM_HEAP_MEM_ALLOC(pdn_resp,
                sizeof(ds_mppm_pdn_db_get_apn_list_response_s_type),
                            ds_mppm_pdn_db_get_apn_list_response_s_type*);

   if (NULL == pdn_resp)
   {
      PS_SYSTEM_HEAP_MEM_FREE(qmi_resp_msg);
      return (qmi_csi_cb_error)QMI_CSI_NO_MEM;
   }


  memset(qmi_resp_msg, 0, sizeof(dsd_get_pdn_policy_apn_list_resp_msg_v01));
  memset(&pdn_req, 0, sizeof(ds_mppm_pdn_db_get_apn_list_request_s_type));
  memset(pdn_resp, 0, sizeof(ds_mppm_pdn_db_get_apn_list_response_s_type));

  pdn_req.txn_id = qmi_req_ptr->txn_id; 

  if (ds_mppm_pdn_db_get_apn_list(&pdn_req, pdn_resp))
  {
    if (pdn_resp->txn_ret_code == DS_MPPM_PDN_DB_TXN_SUCCESS)
    {
      qmi_resp_msg->apn_list_valid = TRUE;
      qmi_resp_msg->apn_list_len = MIN(pdn_resp->apn_list.apn_name_list_len,QMI_DSD_MAX_APNS_V01);
      for (itr=0;itr<qmi_resp_msg->apn_list_len;itr++)
      {
        memscpy(qmi_resp_msg->apn_list[itr].apn_name, sizeof(qmi_resp_msg->apn_list[itr].apn_name),
           pdn_resp->apn_list.apn_name_list[itr].apn_string, 
           pdn_resp->apn_list.apn_name_list[itr].apn_string_len);
      }
    }
    dsdi_map_pdn_db_err_to_qmi(pdn_resp->txn_ret_code,
                               pdn_resp->failure_code,
                               &qmi_resp_msg->resp.result, 
                               &qmi_resp_msg->resp.error);
  }
  else
  {
    LOG_MSG_ERROR_0("Get APN list from PDN db failed");
    qmi_resp_msg->resp.result = QMI_RESULT_FAILURE_V01;
    qmi_resp_msg->resp.error = QMI_ERR_INTERNAL_V01;
  }

  qmi_csi_send_resp(req_handle, msg_id, qmi_resp_msg, 
                    sizeof(dsd_get_pdn_policy_apn_list_resp_msg_v01));


  if(qmi_resp_msg != NULL)
  {  
    PS_SYSTEM_HEAP_MEM_FREE(qmi_resp_msg);
  }
  if(pdn_resp != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(pdn_resp);
  }

  return (qmi_csi_cb_error)QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_get_pdn_policy_apn_list() */

/*===========================================================================
  FUNCTION QMI_DSDI_GET_PDN_POLICY_SETTINGS_FOR_APN()

  DESCRIPTION
    Gets a row from the PDN Policy DB.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_get_pdn_policy_settings_for_apn
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_get_pdn_policy_settings_for_apn_req_msg_v01          *qmi_req_ptr;
  dsd_get_pdn_policy_settings_for_apn_resp_msg_v01         qmi_resp_msg;
  ds_mppm_pdn_db_get_settings_for_apn_request_s_type pdn_req;
  ds_mppm_pdn_db_get_settings_for_apn_response_s_type pdn_resp;
  uint32 itr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(clnt_info && service_handle && req_c_struct && req_c_struct_len > 0);

  LOG_MSG_INFO1_1 ("Client 0x%p querying a row in PDN DB", clnt_info);

  qmi_req_ptr = (dsd_get_pdn_policy_settings_for_apn_req_msg_v01*)req_c_struct;

  memset(&qmi_resp_msg, 0, sizeof(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01));
  memset(&pdn_req, 0, sizeof(ds_mppm_pdn_db_get_settings_for_apn_request_s_type));
  memset(&pdn_resp, 0, sizeof(ds_mppm_pdn_db_get_settings_for_apn_response_s_type));

  pdn_req.txn_id = qmi_req_ptr->txn_id; 

  memscpy(pdn_req.apn_name.apn_string, sizeof(pdn_req.apn_name.apn_string),
          &qmi_req_ptr->apn_name, strlen(qmi_req_ptr->apn_name));
  pdn_req.apn_name.apn_string_len = MIN(strlen(qmi_req_ptr->apn_name), 
                                        sizeof(pdn_req.apn_name.apn_string));

  if (ds_mppm_pdn_db_get_settings_for_apn(&pdn_req, &pdn_resp))
  {
    if (pdn_resp.txn_ret_code == DS_MPPM_PDN_DB_TXN_SUCCESS)
    {
      qmi_resp_msg.apn_name_valid = TRUE;
      memscpy(qmi_resp_msg.apn_name, sizeof(qmi_resp_msg.apn_name),
              qmi_req_ptr->apn_name, sizeof(qmi_req_ptr->apn_name));

      qmi_resp_msg.supported_system_priority_list_valid = TRUE;
      qmi_resp_msg.supported_system_priority_list_len = 
        MIN(pdn_resp.pdn_db_entry.supported_rat_priority_list.rat_priority_list_len,
            QMI_DSD_MAX_SYSTEMS_V01);
      for (itr=0;itr<qmi_resp_msg.supported_system_priority_list_len;itr++)
      {
        qmi_resp_msg.supported_system_priority_list[itr] = 
          (dsd_apn_pref_sys_enum_v01)pdn_resp.pdn_db_entry.supported_rat_priority_list.ds_mppm_rat_list[itr];
      }

      qmi_resp_msg.is_default_valid = TRUE;
      qmi_resp_msg.is_default = pdn_resp.pdn_db_entry.default_apn;

      qmi_resp_msg.override_type_valid = TRUE;
      qmi_resp_msg.override_type = (dsd_pdn_policy_override_enum_v01)pdn_resp.pdn_db_entry.override_type;
    }
    dsdi_map_pdn_db_err_to_qmi(pdn_resp.txn_ret_code,
                               pdn_resp.failure_code,
                               &qmi_resp_msg.resp.result, 
                               &qmi_resp_msg.resp.error);
  }
  else
  {
    LOG_MSG_ERROR_0("Get APN from PDN db failed");
    qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
  }

  qmi_csi_send_resp(req_handle, msg_id, &qmi_resp_msg, 
                    sizeof(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_get_pdn_policy_settings_for_apn() */

/*===========================================================================
  FUNCTION QMI_DSDI_PDN_POLICY_END_TXN()

  DESCRIPTION
    Commit or cancel the transaction on the PDN Policy DB.

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_pdn_policy_end_txn
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  dsd_pdn_policy_end_txn_req_msg_v01          *qmi_req_ptr;
  dsd_pdn_policy_end_txn_resp_msg_v01         qmi_resp_msg;
  ds_mppm_pdn_db_end_txn_request_s_type pdn_req;
  ds_mppm_pdn_db_end_txn_response_s_type pdn_resp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(clnt_info && service_handle && req_c_struct && req_c_struct_len > 0);

  LOG_MSG_INFO1_1 ("Client 0x%p ends the transaction on PDN DB", clnt_info);

  qmi_req_ptr = (dsd_pdn_policy_end_txn_req_msg_v01*)req_c_struct;

  memset(&qmi_resp_msg, 0, sizeof(dsd_pdn_policy_end_txn_resp_msg_v01));
  memset(&pdn_req, 0, sizeof(ds_mppm_pdn_db_end_txn_request_s_type));
  memset(&pdn_resp, 0, sizeof(ds_mppm_pdn_db_end_txn_response_s_type));

  pdn_req.txn_id = qmi_req_ptr->txn_id; 

  if ((qmi_req_ptr->txn_exec_type != DSD_PDN_POLICY_TRANSACTION_COMMIT_V01) &&
      (qmi_req_ptr->txn_exec_type != DSD_PDN_POLICY_TRANSACTION_CANCEL_V01))
  {
    qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;  
    goto send_result; 
  }

  pdn_req.txn_exec_type = (ds_mppm_pdn_db_txn_exec_type)qmi_req_ptr->txn_exec_type;

  if (ds_mppm_pdn_db_end_txn(&pdn_req, &pdn_resp))
  {
    dsdi_map_pdn_db_err_to_qmi(pdn_resp.txn_ret_code,
                               pdn_resp.failure_code,
                               &qmi_resp_msg.resp.result, 
                               &qmi_resp_msg.resp.error);
  }
  else
  {
    LOG_MSG_ERROR_0("Get APN list from PDN db failed");
    qmi_resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
  }
send_result:
  qmi_csi_send_resp(req_handle, msg_id, &qmi_resp_msg, 
                    sizeof(dsd_pdn_policy_end_txn_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_pdn_policy_end_txn() */
#endif /* FEATURE_DATA_WLAN_MAPCON */

/*===========================================================================
  FUNCTION QMI_DSDI_GET_MODEM_POWER_COST()

  DESCRIPTION
    Returns the current modem power cost

  PARAMETERS
    sp:          QMI_DSD's service instance state pointer for this qmi link
    cmd_buf_p:   Message Header
    cl_sp:       Coresponding client state pointer
    sdu_in:      input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_dsd_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_get_modem_power_cost
(
  qmi_dsdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
)
{
  int16                  ps_errno  = 0;
  int16                  ps_result;
  int16                  result = 0;

  dsd_get_modem_power_cost_resp_msg_v01  resp_msg;
  ps_sys_modem_power_cost_enum_type      modem_power_cost;
  ps_sys_tech_enum_type                  current_tech;
  ps_sys_system_status_ex_type           *conf_system_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(clnt_info && service_handle);

  LOG_MSG_INFO1_1 ("Client 0x%p querying modem power cost ", clnt_info);

  memset(&resp_msg, 0, sizeof(dsd_get_modem_power_cost_resp_msg_v01));
  memset(&modem_power_cost, 0, sizeof(ps_sys_modem_power_cost_enum_type));

  PS_SYSTEM_HEAP_MEM_ALLOC(conf_system_status,
                        sizeof(ps_sys_system_status_ex_type),
                           ps_sys_system_status_ex_type*);

   if (NULL == conf_system_status)
   {
      resp_msg.resp.error = QMI_ERR_NO_MEMORY_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto send_result;
   }

  memset(conf_system_status, 0, sizeof(ps_sys_system_status_ex_type));
  result = ps_sys_conf_get_ex ( PS_SYS_TECH_ALL,
                            PS_SYS_CONF_SYSTEM_STATUS_EX,
                                (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                            (void *)conf_system_status,
                            &ps_errno);
  if(result < 0)
  {
    LOG_MSG_ERROR_2("Error in getting preferred mode:"
                  "Unable to query modem power cost", result, ps_errno);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_result;
  }

  current_tech = 
    (ps_sys_tech_enum_type)conf_system_status->avail_sys[0].technology;

  if (current_tech != PS_SYS_TECH_3GPP && current_tech != PS_SYS_TECH_3GPP2)
  {
    LOG_MSG_ERROR_1("Tech %d not supported for quering modem power cost:",
                    current_tech);
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_result;
  }

  ps_result = ps_sys_ioctl_ex ( current_tech,
                             PS_SYS_IOCTL_MODEM_POWER_COST,
                                (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                             &modem_power_cost,
                             &ps_errno );

  if (ps_result)
  {
    LOG_MSG_ERROR_2("ps_sys_ioctl ret [%d] errno [%d]", ps_result, ps_errno);
    switch (ps_errno)
    {
      case DS_EINVAL:
        resp_msg.resp.error = QMI_ERR_INFO_UNAVAILABLE_V01;
        break;
      case DS_NOMEMORY:
        resp_msg.resp.error = QMI_ERR_NO_MEMORY_V01;
        break;
      default:
        resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
        break;
    }
    
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_result;
  } 

  LOG_MSG_INFO1_1("Modem power cost %d", modem_power_cost);

  resp_msg.power_cost_valid = TRUE;  
  resp_msg.power_cost = (uint32) modem_power_cost;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;

send_result:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_get_modem_power_cost_resp_msg_v01));
  if(conf_system_status != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(conf_system_status);
  }
  return QMI_CSI_CB_NO_ERR;
} /* qmi_dsdi_get_modem_power_cost() */

/*===========================================================================
  FUNCTION QMI_DSDI_BIND_SUBSCRIPTION()

  DESCRIPTION
    Binds user given subscription to this client

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer
 
  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_bind_subscription
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
)
{
  dsd_bind_subscription_req_msg_v01      *req_ptr;
  dsd_bind_subscription_resp_msg_v01      resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  req_ptr = (dsd_bind_subscription_req_msg_v01 *)req_c_struct;

  LOG_MSG_INFO1_2 ("Client 0x%p subscription %d", clnt_info, 
                   req_ptr->bind_subs);

  memset(&resp_msg, 0, sizeof(dsd_bind_subscription_resp_msg_v01));

  if (req_ptr->bind_subs > DSD_TERTIARY_SUBS_V01 || 
      req_ptr->bind_subs < DSD_PRIMARY_SUBS_V01)
  {
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
  }

  if ( QMI_ERR_NONE_V01 != resp_msg.resp.error )
  {
    LOG_MSG_ERROR_1("Invalid subs %d", req_ptr->bind_subs );
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(dsd_bind_subscription_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;
  }

  clnt_info->subscription_id =
    (dsd_bind_subscription_enum_v01)req_ptr->bind_subs;

  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_bind_subscription_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_bind_subscription() */

/*===========================================================================
  FUNCTION QMI_DSDI_GET_BIND_SUBSCRIPTION()

  DESCRIPTION
    Returns already bound subscription to this client

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error   qmi_dsdi_get_bind_subscription
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
)
{
  dsd_get_bind_subscription_resp_msg_v01  resp_msg;
  dsd_bind_subscription_enum_v01          subscription_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  memset(&resp_msg, 0, sizeof(dsd_get_bind_subscription_resp_msg_v01));
  memset(&subscription_id, 0, sizeof(dsd_bind_subscription_enum_v01));

  LOG_MSG_INFO1_2("Client 0x%p Subs id %d",clnt_info, 
                   clnt_info->subscription_id);
 
  resp_msg.bind_subscription_valid = TRUE;
  resp_msg.bind_subscription = 
    (dsd_bind_subscription_enum_v01)clnt_info->subscription_id;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_get_bind_subscription_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/* qmi_dsdi_get_bind_subscription */

/*===========================================================================
  FUNCTION QMI_DSDI_REG_SYS_EVENTS()

  DESCRIPTION
    Registers for PS system level events

  PARAMETERS
    None

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dsdi_reg_sys_events
( 
  void
)
{
  int                      sub_ind = 0;
  ps_sys_event_enum_type   event_id   = PS_SYS_EVENT_MIN;
  int16                    result = 0;
  int16                    dss_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (sub_ind = PS_SYS_PRIMARY_SUBS; sub_ind < PS_SYS_SUBS_MAX; sub_ind++)
  {
    #ifdef FEATURE_DATA_WLAN_MAPCON
      result = ps_sys_event_reg_ex (PS_SYS_TECH_ALL,
                                    PS_SYS_EVENT_SYSTEM_STATUS_EX,
                                    (ps_sys_subscription_enum_type) sub_ind,
                                    dsdi_system_status_change_cb,
                                    NULL,
                                    &dss_errno);
      if(result < 0)
      {
        event_id = PS_SYS_EVENT_SYSTEM_STATUS_EX;
        break;
      }
    #endif /* FEATURE_DATA_WLAN_MAPCON */
      
      result = ps_sys_event_reg_ex (PS_SYS_TECH_ALL,
                                    PS_SYS_EVENT_THERMAL_MITIGATION_INFO,
                                    (ps_sys_subscription_enum_type) sub_ind,
                                    dsdi_thermal_info_change_cb,
                                    NULL,
                                    &dss_errno);

      result = ps_sys_event_reg_ex (PS_SYS_TECH_WLAN,
                                    PS_SYS_EVENT_WLAN_START_WIFI_MEAS,
                                    (ps_sys_subscription_enum_type) sub_ind,
                                    dsdi_start_wifi_info_change_cb,
                                    NULL,
                                    &dss_errno);
      if(result < 0)
      {
        event_id = PS_SYS_EVENT_WLAN_START_WIFI_MEAS;
        break;
      }

      result = ps_sys_event_reg_ex (PS_SYS_TECH_WLAN,
                                    PS_SYS_EVENT_WLAN_STOP_WIFI_MEAS,
                                    (ps_sys_subscription_enum_type) sub_ind,
                                    dsdi_stop_wifi_info_change_cb,
                                    NULL,
                                    &dss_errno);

      if(result < 0)
      {
        event_id = PS_SYS_EVENT_WLAN_STOP_WIFI_MEAS;
        break;
      }

      result = ps_sys_event_reg_ex (PS_SYS_TECH_WLAN,
                                    PS_SYS_EVENT_WQE_PROFILE_CHANGE,
                                    (ps_sys_subscription_enum_type) sub_ind,
                                    dsdi_wqe_profile_type_change_cb,
                                    NULL,
                                    &dss_errno);

      
      if(result < 0)
      {
        event_id = PS_SYS_EVENT_WQE_PROFILE_CHANGE;
        break;
      }

      result = ps_sys_event_reg_ex (PS_SYS_TECH_WLAN,
                                    PS_SYS_EVENT_WQE_PROFILES_MET,
                                    (ps_sys_subscription_enum_type) sub_ind,
                                    dsdi_wqe_profile_type_met_cb,
                                    NULL,
                                    &dss_errno);
      if(result < 0)
      {
        event_id = PS_SYS_EVENT_WQE_PROFILES_MET;
        break;
      }


     
      result = ps_sys_event_reg_ex (PS_SYS_TECH_WLAN,
                                    PS_SYS_EVENT_NAT_KEEP_ALIVE_INFO,
                                    (ps_sys_subscription_enum_type) sub_ind,
                                    dsdi_nat_keep_alive_cb,
                                    NULL,
                                    &dss_errno);

       
      if(result < 0)
      {
        event_id = PS_SYS_EVENT_NAT_KEEP_ALIVE_INFO;
        break;
      }

      result = ps_sys_event_reg_ex (PS_SYS_TECH_WLAN,
                                    PS_SYS_EVENT_SET_WQE_PROFILE_QUALITY_MEAS_INFO,
                                    (ps_sys_subscription_enum_type) sub_ind,
                                    dsdi_wqe_profile_init_cb,
                                    NULL,
                                    &dss_errno);


      if(result < 0)
      {
        event_id = PS_SYS_EVENT_SET_WQE_PROFILE_QUALITY_MEAS_INFO;
        break;
      }

      result = ps_sys_event_reg_ex (PS_SYS_TECH_ALL,
                                    PS_SYS_EVENT_ROAMING_STATUS_CHANGE,
                                    (ps_sys_subscription_enum_type) sub_ind,
                                    dsdi_roaming_status_change_cb,
                                    NULL,
                                    &dss_errno);


      if(result < 0)
      {
        event_id = PS_SYS_EVENT_ROAMING_STATUS_CHANGE;
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
}/* qmi_dsdi_reg_sys_events */

/*===========================================================================
  FUNCTION QMI_DSDI_VERIFY_CLIENT_SUBS

  DESCRIPTION
    This function checks if the indication is needed to be sent to the
    client based on the registration state of the client

  PARAMETERS
    clnt_info        :  Client info
    event_subs_id    :  Subs id of the event

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_dsdi_verify_client_subs
(
  qmi_dsdi_client_info_type        *clnt_info,
  uint32                           event_subs_id
)
{
  boolean                        send_ind = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(clnt_info);

  if (event_subs_id >= PS_SYS_SUBS_MAX || 
      event_subs_id <= PS_SYS_DEFAULT_SUBS)
  {
    //event subs should be valid for resolving
     LOG_MSG_ERROR_1("Cannot resolve:Invalid subs passed %d",event_subs_id);
     return FALSE;
  }

  if (clnt_info->subscription_id == event_subs_id )
  {
    send_ind = TRUE;
  }
  else if ((clnt_info->subscription_id == (DSD_PRIMARY_SUBS_V01 - 1 )) &&
            event_subs_id == PS_SYS_PRIMARY_SUBS)
  {
    //Client not bound to any subs; Send events on Primary sub
    send_ind = TRUE;
  }
  else
  {
    send_ind = FALSE;
  }

  LOG_MSG_INFO1_3( "Cl subs %d, event_subs %d send_ind %d", 
                   clnt_info->subscription_id, event_subs_id, send_ind );
  return send_ind;
} /* qmi_dsdi_verify_client_subs */

/*===========================================================================
  FUNCTION QMI_DSDI_RESOLVE_CLIENT_SUBS

  DESCRIPTION
    This function resolves the client subscription.

  PARAMETERS
    clnt_info        :  Client info

  RETURN VALUE
    Resolved client subs id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint32 qmi_dsdi_resolve_client_subs
(
  qmi_dsdi_client_info_type        *clnt_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(clnt_info);

  if (clnt_info->subscription_id == (DSD_PRIMARY_SUBS_V01 - 1))
  {
   // Client not bound to any subs; Return Primary
    return DSD_PRIMARY_SUBS_V01;
  }
  else
  {
    return(clnt_info->subscription_id);
  }
}/* qmi_dsdi_resolve_client_subs */

/*===========================================================================
  FUNCTION DSDI_THERMAL_INFO_CHANGE_CB()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    thermal info change indication

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    subscription_id: Subscription id
    event_info_ptr : System status event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void dsdi_thermal_info_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_dsdi_cmd_buf_type                 * cmd_ptr = NULL;
  ps_sys_thermal_mitigation_action_enum_type *ps_thermal_info = NULL;
  dsd_thermal_mitigation_action_enum_v01      thermal_action;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_4("dsdi_thermal_info_change_cb tech_type %d event_name %d"
                  " event_info_ptr 0x%x Subscription id %d",
                  tech_type, event_name, event_info_ptr,subscription_id);
                 
  ASSERT(event_info_ptr);

  ps_thermal_info = (ps_sys_thermal_mitigation_action_enum_type *)event_info_ptr;
  
  cmd_ptr = (qmi_dsdi_cmd_buf_type *)qmi_dsdi_get_cmd_buf();

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI DSD cmd buf");
    return;
  }
  
  memset(cmd_ptr,0,sizeof(qmi_dsdi_cmd_buf_type));

  thermal_action = (*(dsd_thermal_mitigation_action_enum_v01*)ps_thermal_info);
  cmd_ptr->data.thermal_info.thermal_action_valid = TRUE;
  cmd_ptr->data.thermal_info.thermal_action = thermal_action;
  cmd_ptr->cmd_id = QMI_CMD_DSD_THERMAL_INFO_CHANGE_EVT;
  cmd_ptr->subscription_id = subscription_id;

  dcc_send_cmd_ex (DCC_DSD_SVC_CMD, cmd_ptr);
} /* dsdi_thermal_info_change_cb() */

/*===========================================================================
FUNCTION  QMI_WDSI_GENERATE_THERMAL_INFO_IND

DESCRIPTION
  This callback is called to notify clients about thermal info changes

PARAMETERS
  thermal_info : Thermal info change payload
  subscription_id : Subs identifier on which event was triggered
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void qmi_dsdi_generate_thermal_info_ind
(
  dsd_thermal_info_change_ind_msg_v01 * thermal_info,
  ps_sys_subscription_enum_type         subscription_id
)
{
  qmi_dsdi_client_info_type                   * cl_sp;
  q_type                                      * client_q_ptr = NULL;
  void                                        * client_buf_ptr;
  void                                        * next_client_buf_ptr;
  qmi_dsdi_state_info_type                    * dsd_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsd_sp         = &qmi_dsd_state;
  client_q_ptr   = &dsd_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);
  
  while(client_buf_ptr != NULL)
  {
    cl_sp = (qmi_dsdi_client_info_type *) client_buf_ptr;
    
    if ((TRUE == cl_sp->report_status.report_thermal_info) &&
        (TRUE == qmi_dsdi_verify_client_subs(cl_sp, (uint32)subscription_id)) )
    {    
        qmi_dsdi_thermal_change_ind(cl_sp, thermal_info);
    }
    
    next_client_buf_ptr = 
      q_next(client_q_ptr, &((q_generic_item_type *) client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
} /* qmi_dsdi_generate_thermal_info_ind */

/*===========================================================================
  FUNCTION QMI_DSDI_THERMAL_CHANGE_IND()

  DESCRIPTION
    This function sends thermal change indication to registered control point.

  PARAMETERS
    clid             : clientID
    thermal_info     : Thermal info
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dsdi_thermal_change_ind
(
  void                                  * client,
  dsd_thermal_info_change_ind_msg_v01   * thermal_info
)
{
  qmi_dsdi_client_info_type               * client_info;
  qmi_csi_error                             rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  rc = QMI_CSI_NO_ERR;
  client_info = (qmi_dsdi_client_info_type *) client;
  if (client_info == NULL)
  {
    DATA_ERR_FATAL("qmi_dsdi_thermal_change_ind : Null client_info");
    return;
  }
  
  rc = qmi_csi_send_ind(client_info->clnt, 
                        QMI_DSD_THERMAL_INFO_CHANGE_IND_V01,
                        thermal_info, 
                        sizeof(dsd_thermal_info_change_ind_msg_v01));
  
  if ( rc != QMI_CSI_NO_ERR )
  {
    LOG_MSG_ERROR_1("qmi_dsdi_thermal_change_ind error %d", rc);
  }

} /* qmi_dsdi_thermal_change_ind() */

/*===========================================================================
  FUNCTION QMI_DSDI_GET_THERMAL_INFO()

  DESCRIPTION
    Queries for the thermal info by calling into PS SYS layer

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error   qmi_dsdi_get_thermal_info
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
)
{
  dsd_get_thermal_mitigation_info_resp_msg_v01    resp_msg;
  ps_sys_thermal_mitigation_action_enum_type      ps_thermal_info;
  qmi_error_type_v01     errval;
  int16                  result = 0;
  int16                  ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  memset(&resp_msg, 0, sizeof(dsd_get_thermal_mitigation_info_resp_msg_v01));
  memset(&ps_thermal_info, 0, sizeof(ps_sys_thermal_mitigation_action_enum_type));
  
  errval = QMI_ERR_NONE_V01;
  result = QMI_RESULT_SUCCESS_V01;
  /*-------------------------------------------------------------------------
    Retrieve thermal info by calling into PS Sys Layer
  -------------------------------------------------------------------------*/
  result = ps_sys_conf_get_ex( PS_SYS_TECH_ALL,
                               PS_SYS_CONF_THERMAL_MITIGATION_INFO,
                               (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                               (void *)&ps_thermal_info,
                               &ps_errno);
  if(result < 0)
  {
    LOG_MSG_ERROR_1("ps_sys_conf_get operation returned : (%d)", ps_errno);
    switch (ps_errno)
    {                                   
      case DS_NOMEMORY:
        errval = QMI_ERR_NO_MEMORY_V01;
        break;

      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    } /*  switch (ps_errno) */

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = errval;
    goto send_result;
  }

  resp_msg.thermal_action_valid = TRUE;
  resp_msg.thermal_action = (dsd_thermal_mitigation_action_enum_v01)ps_thermal_info;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  
send_result:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_get_thermal_mitigation_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/* qmi_dsdi_get_thermal_info */

/*===========================================================================
  FUNCTION QMI_DSDI_INDICATION_REGISTER()

  DESCRIPTION
    This function registers the control point for different indications

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer
 
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dsdi_indication_register
(
   qmi_dsdi_client_info_type *clnt_info,
   qmi_req_handle             req_handle,
   int                        msg_id,
   void                      *req_c_struct,
   int                        req_c_struct_len,
   void                      *service_handle
)
{
   dsd_indication_register_req_msg_v01      *req_ptr;
   dsd_indication_register_resp_msg_v01      resp_msg;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  req_ptr = (dsd_indication_register_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_indication_register_resp_msg_v01));

  if ( TRUE ==  req_ptr->report_thermal_info_changes_valid )
  {
    clnt_info->report_status.report_thermal_info = 
      req_ptr->report_thermal_info_changes;
  }

  if ( TRUE == req_ptr->report_wifi_meas_trigger_valid )
  {
    clnt_info->report_status.start_wifi_meas = 
      req_ptr->report_wifi_meas_trigger;
  }

  if ( TRUE == req_ptr->report_wqe_profile_type_changes_valid)
  {
    clnt_info->report_status.report_wqe_profile_type_changes = 
      req_ptr->report_wqe_profile_type_changes;
  }

  if ( TRUE == req_ptr->report_wqe_profile_met_valid)
  {
    clnt_info->report_status.report_wqe_profile_met_changes = 
      req_ptr->report_wqe_profile_met;
  }

  if ( TRUE == req_ptr->report_nat_keep_alive_info_valid)
  {
    clnt_info->report_status.report_nat_keep_alive = 
      req_ptr->report_nat_keep_alive_info;
  }

  if ( TRUE == req_ptr->report_roaming_status_change_valid)
  {
    clnt_info->report_status.report_roaming_status_change = 
      req_ptr->report_roaming_status_change;
  }  

  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_indication_register_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/* qmi_dsdi_indication_register */


/*===========================================================================
  FUNCTION QMI_DSDI_NOTIFY_DATA_SETTINGS()

  DESCRIPTION
    The control point uses the request to notify modem with the data settings

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer
 
  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_notify_data_settings
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
)
{
  dsd_notify_data_settings_req_msg_v01 *req_ptr;
  dsd_notify_data_settings_resp_msg_v01 resp_msg;
  boolean    data_service_switch = FALSE;
  boolean    data_service_roaming_switch = FALSE;
  ps_sys_data_services_wifi_setting_type wifi_switch;
  int16      ps_errno;
  int16      ps_result = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  req_ptr = (dsd_notify_data_settings_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_notify_data_settings_resp_msg_v01));

  if(!((req_ptr->data_service_switch_valid) || 
       (req_ptr->data_service_roaming_switch_valid)||
       (req_ptr->wifi_switch_valid) ))
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
    goto send_result;
  }

  if (req_ptr->data_service_switch_valid)
  {
    data_service_switch = req_ptr->data_service_switch;
    ps_result = ps_sys_conf_set_ex (PS_SYS_TECH_ALL, 
                                    PS_SYS_CONF_DATA_SERVICE_SETTING, 
                                    (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                                    (void *)&data_service_switch, 
                                    &ps_errno);
    LOG_MSG_INFO1_3 ("Client 0x%p data setting %d ps_res %d", clnt_info, 
                     req_ptr->data_service_switch, ps_result);
    if ( ps_result != 0)
    {
      goto send_result;
    }
  }

  if (req_ptr->data_service_roaming_switch_valid)
  {
    data_service_roaming_switch = req_ptr->data_service_roaming_switch;
    ps_result = ps_sys_conf_set_ex (PS_SYS_TECH_ALL, 
                                    PS_SYS_CONF_DATA_SERVICE_ROAMING_SETTING, 
                                    (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                                    (void *)&data_service_roaming_switch, 
                                    &ps_errno);
    LOG_MSG_INFO1_3 ("Client 0x%p data roaming %d ps_res %d", clnt_info, 
                     req_ptr->data_service_roaming_switch, ps_result);
    if ( ps_result != 0)
    {
      goto send_result;
    }
  }

  if (req_ptr->wifi_switch_valid) 
  {
    wifi_switch = (ps_sys_data_services_wifi_setting_type)req_ptr->wifi_switch;
    ps_result = ps_sys_conf_set_ex (PS_SYS_TECH_ALL, 
                                    PS_SYS_CONF_DATA_SERVICE_WIFI_SETTING, 
                                    (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                                    (void *)&wifi_switch, 
                                    &ps_errno);
    LOG_MSG_INFO1_3 ("Client 0x%p wifi switch %d ps_res %d", clnt_info, 
                     req_ptr->wifi_switch, ps_result);
    if (ps_result != 0)
    {
      goto send_result;
    }
  }

send_result:
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
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_notify_data_settings_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_notify_data_settings() */

/*===========================================================================
  FUNCTION QMI_DSDI_GET_DATA_SETTINGS()

  DESCRIPTION
    Queries for the data settings

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error   qmi_dsdi_get_data_settings
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
)
{
  dsd_get_data_settings_resp_msg_v01  resp_msg;
  boolean                             data_setting = FALSE;
  boolean                             data_roaming = FALSE;
  ps_sys_data_services_wifi_setting_type wifi_switch;
  int16            ps_result = 0;
  int16            ps_errno = 0;
  boolean          data_setting_valid = FALSE;
  boolean          data_roaming_valid =  FALSE;
  boolean          wifi_switch_valid = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  memset(&resp_msg, 0, sizeof(dsd_get_data_settings_resp_msg_v01));
  ps_result = ps_sys_conf_get_ex (PS_SYS_TECH_ALL, 
                                  PS_SYS_CONF_DATA_SERVICE_SETTING, 
                                  (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                                  (void *)&data_setting, 
                                  &ps_errno);
  if ( ps_result == 0)
  {
    data_setting_valid = TRUE;
    resp_msg.data_service_switch_valid = TRUE;
    resp_msg.data_service_switch = data_setting;
  }

  ps_result = ps_sys_conf_get_ex (PS_SYS_TECH_ALL, 
                                  PS_SYS_CONF_DATA_SERVICE_ROAMING_SETTING, 
                                  (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                                  (void *)&data_roaming, 
                                  &ps_errno);

  if ( ps_result == 0)
  {
    data_roaming_valid = TRUE;
    resp_msg.data_service_roaming_switch_valid = TRUE;
    resp_msg.data_service_roaming_switch = data_roaming;
  }

  // WIFI switch
  ps_result = ps_sys_conf_get_ex (PS_SYS_TECH_ALL, 
                                  PS_SYS_CONF_DATA_SERVICE_WIFI_SETTING, 
                                  (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                                  (void *)&wifi_switch, 
                                  &ps_errno);
  if (ps_result == 0)
  {
    wifi_switch_valid = TRUE;
    resp_msg.wifi_switch_valid = TRUE;
    resp_msg.wifi_switch = (uint8)wifi_switch;
  }

  if ( !data_setting_valid &&
       !data_roaming_valid &&
       !wifi_switch_valid ) 
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  else
  {
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    resp_msg.resp.error = QMI_ERR_NONE_V01;
  }

send_result:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_get_data_settings_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/* qmi_dsdi_get_data_settings */

/*===========================================================================
  FUNCTION QMI_DSDI_SET_APN_INFO ()

  DESCRIPTION
    Sets the APN information

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer
    
  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_set_apn_info
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
)
{
  dsd_set_apn_info_req_msg_v01 *req_ptr;
  dsd_set_apn_info_resp_msg_v01 resp_msg;
  ps_sys_apn_params_type    apn_params;
  int16      ps_errno;
  int16      ps_result = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  req_ptr = (dsd_set_apn_info_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_set_apn_info_resp_msg_v01));
  memset(&apn_params, 0, sizeof(ps_sys_apn_params_type));

  apn_params.is_configured = TRUE;
  apn_params.apn_type = req_ptr->apn_info.apn_type;

  if ( req_ptr->apn_invalid_valid ==  TRUE )
  {
    if ( req_ptr->apn_invalid ==  TRUE )
    {
      // APN is deleted
      apn_params.is_configured = FALSE;
    }
  }

  if ( apn_params.is_configured == TRUE )
  {
    apn_params.is_configured = TRUE;
    memscpy( &apn_params.apn_name, sizeof(apn_params.apn_name),
             &req_ptr->apn_info.apn_name, sizeof(req_ptr->apn_info.apn_name) );
  }

  ps_result = ps_sys_conf_set_ex (PS_SYS_TECH_ALL, 
                                  PS_SYS_CONF_APN_INFO, 
                                  (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                                  (void *)&apn_params, 
                                  &ps_errno);
  LOG_MSG_INFO1_3 ("Client 0x%p apn type %d ps_res %d", clnt_info, 
                   apn_params.apn_type, ps_result);

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

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_set_apn_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_dsdi_set_apn_info() */

/*===========================================================================
  FUNCTION QMI_DSDI_GET_APN_INFO()

  DESCRIPTION
    Queries for the APN information

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error   qmi_dsdi_get_apn_info
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
)
{
  dsd_get_apn_info_req_msg_v01    *req_ptr;
  dsd_get_apn_info_resp_msg_v01    resp_msg;

  ps_sys_apn_info_type      ps_apn_info;
  int16    ps_result = 0;
  int16    ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  req_ptr = (dsd_get_apn_info_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_get_data_settings_resp_msg_v01));
  memset(&ps_apn_info, 0, sizeof(ps_sys_apn_info_type));

  if (req_ptr->apn_type >= DS_SYS_APN_TYPE_MAX || 
      req_ptr->apn_type < DSD_APN_TYPE_DEFAULT_V01) 
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  ps_result = ps_sys_conf_get_ex (PS_SYS_TECH_ALL, 
                                  PS_SYS_CONF_APN_INFO, 
                                  (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                                  (void *)&ps_apn_info, 
                                  &ps_errno);

  if ( ps_result != 0)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  else
  {
     if ( !ps_apn_info.curr_apn_info[req_ptr->apn_type].is_configured )
     {
       resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
       resp_msg.resp.error  = QMI_ERR_INFO_UNAVAILABLE_V01;
       goto send_result;       
     }
     resp_msg.apn_name_valid = TRUE;
     strlcpy((char*)resp_msg.apn_name,
             (char*)ps_apn_info.curr_apn_info[req_ptr->apn_type].apn_name,
             sizeof(resp_msg.apn_name));

     resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
     resp_msg.resp.error = QMI_ERR_NONE_V01;
  }

send_result:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_get_apn_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/* qmi_dsdi_get_apn_info */

/*===========================================================================
  FUNCTION QMI_DSDI_GET_ROAMING_STATUS_INFO()

  DESCRIPTION
    Queries roaming status by calling into PS SYS layer

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error   qmi_dsdi_get_roaming_status_info
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
)
{
  dsd_get_current_roaming_status_info_resp_msg_v01    resp_msg;
  ps_sys_roaming_status_info_type             ps_roaming_status_info;
  qmi_error_type_v01                          errval;
  int16                                       result = 0;
  int16                                       ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  memset(&resp_msg, 0, sizeof(dsd_get_current_roaming_status_info_resp_msg_v01));
  memset(&ps_roaming_status_info, 0, 
         sizeof(ps_sys_roaming_status_info_type));
  
  errval = QMI_ERR_NONE_V01;
  result = QMI_RESULT_SUCCESS_V01;
  /*-------------------------------------------------------------------------
    Retrieve thermal info by calling into PS Sys Layer
  -------------------------------------------------------------------------*/
  result = ps_sys_conf_get_ex( PS_SYS_TECH_ALL,
                               PS_SYS_CONF_ROAMING_STATUS_INFO,
                               (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                               (void *)&ps_roaming_status_info,
                               &ps_errno);
  if(result < 0)
  {
    LOG_MSG_ERROR_1("ps_sys_conf_get operation returned : (%d)", ps_errno);
    switch (ps_errno)
    {                                   
      case DS_NOMEMORY:
        errval = QMI_ERR_NO_MEMORY_V01;
        break;

      case DS_EINVAL:
        errval = QMI_ERR_INFO_UNAVAILABLE_V01;
        break;

      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    } /*  switch (ps_errno) */
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = errval;
    goto send_result;
  }

  if (DS_SYS_NETWORK_MAX == ps_roaming_status_info.technology)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    goto send_result;    
  }

  resp_msg.roaming_status = ps_roaming_status_info.roaming_status;
  resp_msg.roaming_status_valid = TRUE;
  resp_msg.technology = 
    (dsd_sys_network_enum_v01)ps_roaming_status_info.technology;
  resp_msg.technology_valid = TRUE;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  
send_result:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_get_current_roaming_status_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/* qmi_dsdi_get_roaming_status_info */

/*===========================================================================
  FUNCTION dsdi_start_wifi_info_change_cb()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    start wifi info change CB

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    subscription_id: Subscription id
    event_info_ptr : Start Wifi event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsdi_start_wifi_info_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_dsdi_cmd_buf_type                 * cmd_ptr = NULL;
  ps_sys_wlan_wifi_meas_start_type      * ps_wifi_meas_start_info = NULL;
  uint8                                  ssid_iter = 0;
  uint8                                  num_of_ssid = 0;
  uint8                                  bssid_iter = 0;
  uint8                                  num_of_bssid = 0; 
  uint8                                  num_channels = 0;
  dsd_start_wifi_meas_ind_msg_v01       * start_meas_info = NULL;                                
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_4("dsdi_start_wifi_info_change_cb tech_type %d event_name %d"
                  " event_info_ptr 0x%x Subscription id %d",
                  tech_type, event_name, event_info_ptr,subscription_id);
                 
  ASSERT(event_info_ptr);

  ps_wifi_meas_start_info = (ps_sys_wlan_wifi_meas_start_type *)event_info_ptr;
  
  cmd_ptr = (qmi_dsdi_cmd_buf_type *)qmi_dsdi_get_cmd_buf();

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI DSD cmd buf");
    return;
  }
  
  memset(cmd_ptr,0,sizeof(qmi_dsdi_cmd_buf_type));

  start_meas_info = &(cmd_ptr->data.start_wifi_meas_info);

  /*Copy Alpha if valid*/
  start_meas_info->alpha_valid = 
    ps_wifi_meas_start_info->is_valid_alpha;
  if (ps_wifi_meas_start_info->is_valid_alpha)
  {
    start_meas_info->alpha = 
      ps_wifi_meas_start_info->alpha;
  }

  /*Copy Average Interval if valid*/
  start_meas_info->report_interval_valid = 
    ps_wifi_meas_start_info->is_valid_report_interval;
  if (ps_wifi_meas_start_info->is_valid_report_interval)
  {
    start_meas_info->report_interval = 
      ps_wifi_meas_start_info->report_interval;
  }

  /*Copy Sampling timer if valid*/
  start_meas_info->sampling_timer_valid = 
    ps_wifi_meas_start_info->is_valid_sampling_timer;
  if (ps_wifi_meas_start_info->is_valid_sampling_timer)
  {
    start_meas_info->sampling_timer = 
      ps_wifi_meas_start_info->sampling_timer;
  }

  /*Copy Measurement Id*/
  start_meas_info->wifi_meas_id = 
    ps_wifi_meas_start_info->meas_id;

  /*Copy Param Mask*/
  start_meas_info->meas_param = 
    ps_wifi_meas_start_info->meas_param_mask;

  /*Copy the List of parameters based on which report 
    is to be generated*/
  start_meas_info->ident_param_list_valid = TRUE;
  start_meas_info->ident_param_list_len = 
    ps_wifi_meas_start_info->ssid_list.num_ssid_info;
  num_of_ssid = ps_wifi_meas_start_info->ssid_list.num_ssid_info;

  for(ssid_iter = 0; ssid_iter < num_of_ssid; ssid_iter++)
  {
     /** Copy SSID here */
    start_meas_info->ident_param_list[ssid_iter].ssid_len
      = PS_SYS_MAX_SSID_LEN - 1;

    memscpy(start_meas_info->ident_param_list[ssid_iter].ssid,
            sizeof(start_meas_info->ident_param_list[ssid_iter].ssid),
            ps_wifi_meas_start_info->ssid_list.ssid_info[ssid_iter].ssid,
            PS_SYS_MAX_SSID_LEN);

    start_meas_info->ident_param_list[ssid_iter].is_hidden = 
      ps_wifi_meas_start_info->ssid_list.ssid_info[ssid_iter].is_hidden;

    start_meas_info->ident_param_list[ssid_iter].bssid_list_len = 
      ps_wifi_meas_start_info->ssid_list.ssid_info[ssid_iter].num_bssid_info;

    /** Copy BSSID here */
    num_of_bssid = ps_wifi_meas_start_info->ssid_list.ssid_info[ssid_iter].num_bssid_info;

    for (bssid_iter = 0; bssid_iter < num_of_bssid; bssid_iter++)
    {
      memscpy(start_meas_info->ident_param_list[ssid_iter].bssid_list[bssid_iter].bssid,
              sizeof(start_meas_info->ident_param_list[ssid_iter].bssid_list[bssid_iter].bssid),
              ps_wifi_meas_start_info->ssid_list.ssid_info[ssid_iter].bssid_info[bssid_iter].bssid,
              sizeof(ps_wifi_meas_start_info->ssid_list.ssid_info[ssid_iter].bssid_info[bssid_iter].bssid));

      /** Copy the channel info */
      num_channels = ps_wifi_meas_start_info->ssid_list.ssid_info[ssid_iter].bssid_info[bssid_iter].num_channel_info;
      start_meas_info->ident_param_list[ssid_iter].bssid_list[bssid_iter].channel_list_len = 
        num_channels;

      memscpy(start_meas_info->ident_param_list[ssid_iter].bssid_list[bssid_iter].channel_list,
              sizeof(start_meas_info->ident_param_list[ssid_iter].bssid_list[bssid_iter].channel_list),
              ps_wifi_meas_start_info->ssid_list.ssid_info[ssid_iter].bssid_info[bssid_iter].channel_list,
              sizeof(ps_wifi_meas_start_info->ssid_list.ssid_info[ssid_iter].bssid_info[bssid_iter].channel_list));

    }/*end of BSSID info*/
  }/*end of SSID info*/
  
  cmd_ptr->cmd_id = QMI_CMD_DSD_START_WIFI_MEAS_EVT;
  cmd_ptr->subscription_id = subscription_id;
  dcc_send_cmd_ex (DCC_DSD_SVC_CMD, cmd_ptr);
} /* dsdi_start_change_cb() */

/*===========================================================================
FUNCTION  QMI_DSDI_GENERATE_START_WIFI_MEAS_IND

DESCRIPTION
  This callback is called to notify clients about wifi measurement
  parameters configuration

PARAMETERS
  start_wifi_meas_msg : Start Wifi Measurement message
  subscription_id : Subs identifier on which event was triggered
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_dsdi_generate_start_wifi_meas_ind
(
  dsd_start_wifi_meas_ind_msg_v01        * start_wifi_meas_msg,
  ps_sys_subscription_enum_type            subscription_id
)
{
  qmi_dsdi_client_info_type                   * cl_sp;
  q_type                                      * client_q_ptr = NULL;
  void                                        * client_buf_ptr;
  void                                        *next_client_buf_ptr;
  qmi_dsdi_state_info_type                    *dsd_sp;
  qmi_csi_error                               rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  dsd_sp         = &qmi_dsd_state;
  client_q_ptr   = &dsd_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);

  LOG_MSG_INFO1_0("In QMI DSDI Start Wifi Measurement indication");

  while (client_buf_ptr != NULL)
  {
    cl_sp = (qmi_dsdi_client_info_type *)client_buf_ptr;

    if (cl_sp == NULL)
    {
      DATA_ERR_FATAL("qmi_dsdi_start_wifi_meas_ind : Null client_info");
      return;
    }

    if ((TRUE == cl_sp->report_status.start_wifi_meas) &&
        (TRUE == qmi_dsdi_verify_client_subs(cl_sp, (uint32)subscription_id)))
    {
      
      rc = qmi_csi_send_ind(cl_sp->clnt,
                            QMI_DSD_START_WIFI_MEAS_IND_V01,
                            start_wifi_meas_msg,
                            sizeof(dsd_start_wifi_meas_ind_msg_v01));

      if (rc != QMI_CSI_NO_ERR)
      {
        LOG_MSG_ERROR_1("qmi_dsdi_generate_start_wifi_meas_ind error %d", rc);
      }
    }

    next_client_buf_ptr =
      q_next(client_q_ptr, &((q_generic_item_type *)client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
} /*qmi_dsdi_generate_start_wifi_meas_ind*/

/*===========================================================================
  FUNCTION dsdi_stop_wifi_info_change_cb()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    stop wifi info change CB

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    subscription_id: Subscription id
    event_info_ptr : Stop Wifi event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsdi_stop_wifi_info_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_dsdi_cmd_buf_type                * cmd_ptr = NULL;
  ps_sys_wlan_wifi_meas_stop_type      * ps_wifi_meas_stop_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_4("dsdi_stop_wifi_info_change_cb tech_type %d event_name %d"
                  " event_info_ptr 0x%x Subscription id %d",
                  tech_type, event_name, event_info_ptr,subscription_id);
                 
  ASSERT(event_info_ptr);

  ps_wifi_meas_stop_info = (ps_sys_wlan_wifi_meas_stop_type *)event_info_ptr;
  
  cmd_ptr = (qmi_dsdi_cmd_buf_type *)qmi_dsdi_get_cmd_buf();

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI DSD cmd buf");
    return;
  }
  
  memset(cmd_ptr,0,sizeof(qmi_dsdi_cmd_buf_type));

  cmd_ptr->data.stop_wifi_meas_info.wifi_meas_id = 
    ps_wifi_meas_stop_info->meas_id;

  cmd_ptr->cmd_id = QMI_CMD_DSD_STOP_WIFI_MEAS_EVT;
  cmd_ptr->subscription_id = subscription_id;
  dcc_send_cmd_ex (DCC_DSD_SVC_CMD, cmd_ptr);
}

/*===========================================================================
FUNCTION  QMI_DSDI_GENERATE_STOP_WIFI_MEAS_IND

DESCRIPTION
  This callback is called to notify clients to stop reporting
  wifi measurements.

PARAMETERS
  stop_wifi_meas_msg : Stop Wifi Measurement message
  subscription_id : Subs identifier on which event was triggered
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_dsdi_generate_stop_wifi_meas_ind
(
  dsd_stop_wifi_meas_ind_msg_v01        * stop_wifi_meas_msg,
  ps_sys_subscription_enum_type         subscription_id
)
{
  qmi_dsdi_client_info_type                   * cl_sp;
  q_type                                      * client_q_ptr = NULL;
  void                                        * client_buf_ptr;
  void                                        *next_client_buf_ptr;
  qmi_dsdi_state_info_type                    *dsd_sp;
  qmi_csi_error                               rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  dsd_sp         = &qmi_dsd_state;
  client_q_ptr   = &dsd_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);

  LOG_MSG_INFO1_0("In QMI DSDI Stop Wifi Measurement indication");
  while (client_buf_ptr != NULL)
  {
    cl_sp = (qmi_dsdi_client_info_type *)client_buf_ptr;

    if (cl_sp == NULL)
    {
      DATA_ERR_FATAL("qmi_dsdi_stop_wifi_meas_ind : Null client_info");
      return;
    }

    if ((TRUE == cl_sp->report_status.start_wifi_meas) &&
        (TRUE == qmi_dsdi_verify_client_subs(cl_sp, (uint32)subscription_id)))
    {
      LOG_MSG_INFO1_1("In QMI DSDI Stop Wifi Measurement "
                      "indication start_wifi_meas registered %d",
                      cl_sp->report_status.start_wifi_meas);
      
      rc = qmi_csi_send_ind(cl_sp->clnt,
                            QMI_DSD_STOP_WIFI_MEAS_IND_V01,
                            stop_wifi_meas_msg,
                            sizeof(dsd_stop_wifi_meas_ind_msg_v01));

      if (rc != QMI_CSI_NO_ERR)
      {
        LOG_MSG_ERROR_1("qmi_dsdi_generate_stop_wifi_meas_ind error %d", rc);
      }
    }

    next_client_buf_ptr =
      q_next(client_q_ptr, &((q_generic_item_type *)client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
} /*qmi_dsdi_generate_stop_wifi_meas_ind*/

/*===========================================================================
  FUNCTION QMI_DSDI_WIFI_MEAS_REPORT_CONFIG_STATUS()

  DESCRIPTION
   AP signals whether it is ok with the
   previously sent QMI_DSD_START_WIFI_MEAS_IND

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error  qmi_dsdi_wifi_meas_report_config_status(
  qmi_dsdi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
)
{
  dsd_wifi_meas_report_config_status_req_msg_v01    *req_ptr;
  dsd_wifi_meas_report_config_status_resp_msg_v01    resp_msg;
  ps_sys_wlan_wifi_meas_report_config_status_type    ps_config_status;
  int16    ps_result = 0;
  int16    ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  req_ptr = (dsd_wifi_meas_report_config_status_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_wifi_meas_report_config_status_resp_msg_v01));
  memset(&ps_config_status, 
         0, 
         sizeof(ps_sys_wlan_wifi_meas_report_config_status_type));
  LOG_MSG_INFO1_2("In QMI DSDI Wifi Meas Report Config Status message "
                  " Wifi meas id %d, status %d",
                  req_ptr->wifi_meas_id, req_ptr->status);

  if (req_ptr->status > DSD_WIFI_MEAS_REPORT_STOPPED_V01 || 
      req_ptr->status < DSD_WIFI_MEAS_REPORT_CONFIG_OK_V01)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  ps_config_status.status = req_ptr->status;
  ps_config_status.wifi_meas_id = req_ptr->wifi_meas_id;

  ps_result = ps_sys_ioctl_ex(PS_SYS_TECH_WLAN,
                              PS_SYS_IOCTL_WLAN_WIFI_MEAS_REPORT_CONFIG_STATUS,
                              (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                              (void*)&ps_config_status,
                              &ps_errno);

  if ( ps_result != 0)
  {
    if (ps_errno == DS_EFAULT)
    {
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_TX_ID_V01;
    }
    else
    {
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    }
    goto send_result;
  }
  else
  {
     resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
     resp_msg.resp.error = QMI_ERR_NONE_V01;
  }

send_result:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_wifi_meas_report_config_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/*qmi_dsdi_wifi_meas_report_config_status*/

/*===========================================================================
  FUNCTION QMI_DSDI_WIFI_MEAS_REPORT()

  DESCRIPTION
    Wifi Measurement report from the AP

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error  qmi_dsdi_wifi_meas_report(
  qmi_dsdi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
)
{
  dsd_wifi_meas_report_req_msg_v01    *req_ptr;
  dsd_wifi_meas_report_resp_msg_v01    resp_msg;
  ps_sys_wlan_wifi_meas_report_type    * ps_wifi_meas_report = NULL;
  int16    ps_result = 0;
  int16    ps_errno = 0;
  uint8    iter = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  req_ptr = (dsd_wifi_meas_report_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_wifi_meas_report_resp_msg_v01));

  LOG_MSG_INFO1_3("In QMI DSDI Wifi Meas Report msg "
                  "tx id %d, wifi_meas_id %d, "
                  "is_last_report_of_transaction %d",
                  req_ptr->tx_id, req_ptr->wifi_meas_id,
                  req_ptr->is_last_report_of_transaction);

  PS_SYSTEM_HEAP_MEM_ALLOC(ps_wifi_meas_report,
                           sizeof(ps_sys_wlan_wifi_meas_report_type),
                           ps_sys_wlan_wifi_meas_report_type*);
   if (NULL == ps_wifi_meas_report)
   {
     return QMI_CSI_CB_NO_MEM;
   }

   memset(ps_wifi_meas_report, 
          0, 
          sizeof(ps_sys_wlan_wifi_meas_report_type));

  ps_wifi_meas_report->meas_id = req_ptr->wifi_meas_id;
  ps_wifi_meas_report->tx_id   = req_ptr->tx_id;
  ps_wifi_meas_report->is_last_report_of_transaction = 
    req_ptr->is_last_report_of_transaction;

  memscpy(ps_wifi_meas_report->ssid,
          sizeof(ps_wifi_meas_report->ssid),
          req_ptr->ssid,
          req_ptr->ssid_len);

  ps_wifi_meas_report->ssid[MIN(req_ptr->ssid_len, sizeof( ps_wifi_meas_report->ssid)-1)] = '\0';

  if (req_ptr->per_bssid_info_valid)
  {
    ps_wifi_meas_report->num_bssid_info = req_ptr->per_bssid_info_len;
    for(iter = 0; iter < ps_wifi_meas_report->num_bssid_info; iter++)
    {
      memscpy( ps_wifi_meas_report->per_bssid_info[iter].bssid,
               PS_SYS_MAC_ADDR_LEN,
               req_ptr->per_bssid_info[iter].bssid,
               QMI_DSD_MAC_ADDR_LEN_V01);

      ps_wifi_meas_report->per_bssid_info[iter].channel_info.channel
        = req_ptr->per_bssid_info[iter].channel_info.channel;
      ps_wifi_meas_report->per_bssid_info[iter].channel_info.meas_param_mask
        = req_ptr->per_bssid_info[iter].channel_info.valid_params;
      ps_wifi_meas_report->per_bssid_info[iter].channel_info.rssi
        = req_ptr->per_bssid_info[iter].channel_info.rssi;
      ps_wifi_meas_report->per_bssid_info[iter].channel_info.sinr
        = req_ptr->per_bssid_info[iter].channel_info.sinr;

      memscpy(&(ps_wifi_meas_report->per_bssid_info[iter].channel_info.bss_load),
              sizeof(ps_wifi_meas_report->per_bssid_info[iter].channel_info.bss_load),
              &(req_ptr->per_bssid_info[iter].channel_info.bss_load),
              sizeof(req_ptr->per_bssid_info[iter].channel_info.bss_load)); 

    }
  }

  ps_result = ps_sys_ioctl_ex(PS_SYS_TECH_WLAN,
                              PS_SYS_IOCTL_WLAN_WIFI_MEAS_REPORT,
                              (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                              (void*)ps_wifi_meas_report,
                              &ps_errno);

  if ( ps_result != 0)
  {
    if (ps_errno == DS_EFAULT)
    {
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_TX_ID_V01;
    }
    else
    {
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    }
    goto send_result;
  }
  else
  {
     resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
     resp_msg.resp.error = QMI_ERR_NONE_V01;
  }

send_result:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_wifi_meas_report_resp_msg_v01));

  if(ps_wifi_meas_report != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(ps_wifi_meas_report);
  }
  
  return QMI_CSI_CB_NO_ERR;
}/*qmi_dsdi_wifi_meas_report*/

/*===========================================================================
  FUNCTION QMI_DSDI_SET_APN_CALL_TYPE ()

  DESCRIPTION
    Sets the APN information

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer
    
  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_set_apn_call_type
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
)
{
  dsd_set_apn_call_type_req_msg_v01  *req_ptr;
  dsd_set_apn_call_type_resp_msg_v01 resp_msg;
  ps_sys_ioctl_apn_call_type         apn_call_type;
  int16                              ps_errno;
  int16                              ps_result = 0; 
  ps_sys_subscription_enum_type      ps_subs;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);


  req_ptr = (dsd_set_apn_call_type_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_set_apn_call_type_resp_msg_v01));
  memset(&apn_call_type, 0, sizeof(ps_sys_ioctl_apn_call_type));

  apn_call_type.apn_call_type_mask = req_ptr->apn_call_type;

  memscpy( &apn_call_type.apn, 
           sizeof(apn_call_type.apn),
           &req_ptr->apn, 
           sizeof(req_ptr->apn));

  LOG_MSG_INFO1_1 ("Set APN CALL TYpe mask %d", apn_call_type.apn_call_type_mask);

  ps_subs = 
    (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info);

  ps_result = ps_sys_ioctl_ex(PS_SYS_TECH_3GPP,
                              PS_SYS_IOCTL_SET_APN_CALL_TYPE,
                              ps_subs,
                              (void*)&apn_call_type,
                              &ps_errno);
 
  LOG_MSG_INFO1_3 ("Client 0x%p apn call type mask %d ps_res %d", clnt_info, 
                   apn_call_type.apn_call_type_mask, ps_result);

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

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_set_apn_info_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_dsdi_set_apn_call_type() */


/*===========================================================================
  FUNCTION QMI_DSDI_SET_WQE_PROFILE_TYPE ()

  DESCRIPTION
    Sets the WQE Profile Type for the modem

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer
    
  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_set_wqe_profile_type
(
  qmi_dsdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
)
{
  dsd_set_wqe_profile_type_req_msg_v01  *req_ptr;
  dsd_set_wqe_profile_type_resp_msg_v01 resp_msg;
  ps_sys_wqe_profile_type               wqe_profile_type;
  int16                                 ps_errno;
  int16                                 ps_result = 0; 
  ps_sys_subscription_enum_type         ps_subs;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);


  req_ptr = (dsd_set_wqe_profile_type_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_set_wqe_profile_type_resp_msg_v01));

  wqe_profile_type = (ps_sys_wqe_profile_type)req_ptr->wqe_profile_type;

  ps_subs = 
    (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info);

  ps_result = ps_sys_ioctl_ex(PS_SYS_TECH_WLAN,
                              PS_SYS_IOCTL_SET_WQE_PROFILE_TYPE,
                              ps_subs,
                              (void*)&wqe_profile_type,
                              &ps_errno);
 
  LOG_MSG_INFO1_3 ("Client 0x%p wqe_profile_type %ld ps_res %d", clnt_info, 
                   wqe_profile_type, ps_result);

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

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_set_wqe_profile_type_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_dsdi_set_wqe_profile_type() */


/*===========================================================================
  FUNCTION dsdi_wqe_profile_type_change_cb()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    Wqe Profile type change CB

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    subscription_id: Subscription id
    event_info_ptr : WQE Profile type event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsdi_wqe_profile_type_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_dsdi_cmd_buf_type                * cmd_ptr = NULL;
  ps_sys_wqe_profile_type              * ps_wqe_profile = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_4(" tech_type %d event_name %d"
                  " event_info_ptr 0x%x Subscription id %d",
                  tech_type, event_name, event_info_ptr,subscription_id);
                 
  ASSERT(event_info_ptr);

  ps_wqe_profile = (ps_sys_wqe_profile_type *)event_info_ptr;
  
  cmd_ptr = (qmi_dsdi_cmd_buf_type *)qmi_dsdi_get_cmd_buf();

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI DSD cmd buf");
    return;
  }
  
  memset(cmd_ptr,0,sizeof(qmi_dsdi_cmd_buf_type));

  cmd_ptr->data.wqe_profile_info.set_wqe_profile_type = 
    *ps_wqe_profile;

  cmd_ptr->cmd_id = QMI_CMD_DSD_WQE_PROFILE_EVT;
  cmd_ptr->subscription_id = subscription_id;
  dcc_send_cmd_ex (DCC_DSD_SVC_CMD, cmd_ptr);
} /*dsdi_wqe_profile_type_change_cb*/

/*===========================================================================
FUNCTION  QMI_DSDI_GENERATE_WQE_PROFILE_TYPE_IND

DESCRIPTION
  This callback is called to notify clients about the wqe profile
  type mask set on the modem

PARAMETERS
  wqe_profile_type_ind : Wqe Profile type Ind
  subscription_id : Subs identifier on which event was triggered
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_dsdi_generate_wqe_profile_type_ind
(
  dsd_wqe_profile_type_ind_msg_v01      * wqe_profile_type_ind,
  ps_sys_subscription_enum_type         subscription_id
)
{
  qmi_dsdi_client_info_type                   * cl_sp;
  q_type                                      * client_q_ptr = NULL;
  void                                        * client_buf_ptr;
  void                                        *next_client_buf_ptr;
  qmi_dsdi_state_info_type                    *dsd_sp;
  qmi_csi_error                               rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  dsd_sp         = &qmi_dsd_state;
  client_q_ptr   = &dsd_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);

  while (client_buf_ptr != NULL)
  {
    cl_sp = (qmi_dsdi_client_info_type *)client_buf_ptr;

    if (cl_sp == NULL)
    {
      DATA_ERR_FATAL("qmi_dsdi_wqe_profile_type_ind : Null client_info");
      return;
    }

    if ((TRUE == cl_sp->report_status.report_wqe_profile_type_changes) &&
        (TRUE == qmi_dsdi_verify_client_subs(cl_sp, (uint32)subscription_id)))
    {
      LOG_MSG_INFO1_1("In qmi_dsdi_wqe_profile_type_ind "
                      "indication wqe_profile_type registered %d",
                      cl_sp->report_status.report_wqe_profile_type_changes);
      
      rc = qmi_csi_send_ind(cl_sp->clnt,
                            QMI_DSD_WQE_PROFILE_TYPE_IND_V01,
                            wqe_profile_type_ind,
                            sizeof(dsd_wqe_profile_type_ind_msg_v01));

      if (rc != QMI_CSI_NO_ERR)
      {
        LOG_MSG_ERROR_1("qmi_dsdi_wqe_profile_type_ind error %d", rc);
      }
    }

    next_client_buf_ptr =
      q_next(client_q_ptr, &((q_generic_item_type *)client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
} /*qmi_dsdi_generate_wqe_profile_type_ind*/


/*===========================================================================
  FUNCTION dsdi_wqe_profile_type_met_cb()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    Wqe Profile type Met CB

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    subscription_id: Subscription id
    event_info_ptr : WQE Profile type Met event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsdi_wqe_profile_type_met_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_dsdi_cmd_buf_type                * cmd_ptr = NULL;
  ps_sys_wqe_profile_type              * ps_wqe_profile = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_4(" tech_type %d event_name %d"
                  " event_info_ptr 0x%x Subscription id %d",
                  tech_type, event_name, event_info_ptr,subscription_id);
                 
  ASSERT(event_info_ptr);

  ps_wqe_profile = (ps_sys_wqe_profile_type *)event_info_ptr;
  
  cmd_ptr = (qmi_dsdi_cmd_buf_type *)qmi_dsdi_get_cmd_buf();

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI DSD cmd buf");
    return;
  }
  
  memset(cmd_ptr,0,sizeof(qmi_dsdi_cmd_buf_type));

  cmd_ptr->data.wqe_profile_met_info.met_wqe_profile_type = 
    *ps_wqe_profile;

  cmd_ptr->cmd_id = QMI_CMD_DSD_WQE_PROFILE_MET_EVT;
  cmd_ptr->subscription_id = subscription_id;
  dcc_send_cmd_ex (DCC_DSD_SVC_CMD, cmd_ptr);
} /* dsdi_wqe_profile_type_met_cb */

/*===========================================================================
FUNCTION  QMI_DSDI_GENERATE_WQE_PROFILE_TYPE_MET_IND

DESCRIPTION
  This callback is called to notify clients about the wqe profile
  type mask that was met or used for the application

PARAMETERS
  met_wqe_profile_type_ind : Met Wqe Profile type Ind
  subscription_id : Subs identifier on which event was triggered
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_dsdi_generate_wqe_profile_type_met_ind
(
  dsd_wqe_profile_type_met_ind_msg_v01  * met_wqe_profile_type_ind,
  ps_sys_subscription_enum_type         subscription_id
)
{
  qmi_dsdi_client_info_type                   * cl_sp;
  q_type                                      * client_q_ptr = NULL;
  void                                        * client_buf_ptr;
  void                                        *next_client_buf_ptr;
  qmi_dsdi_state_info_type                    *dsd_sp;
  qmi_csi_error                               rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  dsd_sp         = &qmi_dsd_state;
  client_q_ptr   = &dsd_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);

  while (client_buf_ptr != NULL)
  {
    cl_sp = (qmi_dsdi_client_info_type *)client_buf_ptr;

    if (cl_sp == NULL)
    {
      DATA_ERR_FATAL("qmi_dsdi_wqe_profile_type_met_ind : Null client_info");
      return;
    }

    if ((TRUE == cl_sp->report_status.report_wqe_profile_met_changes) &&
        (TRUE == qmi_dsdi_verify_client_subs(cl_sp, (uint32)subscription_id)))
    {
      LOG_MSG_INFO1_1("In qmi_dsdi_generate_wqe_profile_type_met_ind "
                      "indication met_wqe_profile_type registered %d",
                      cl_sp->report_status.report_wqe_profile_met_changes);
      
      rc = qmi_csi_send_ind(cl_sp->clnt,
                            QMI_DSD_WQE_PROFILE_TYPE_MET_IND_V01,
                            met_wqe_profile_type_ind,
                            sizeof(dsd_wqe_profile_type_met_ind_msg_v01));

      if (rc != QMI_CSI_NO_ERR)
      {
        LOG_MSG_ERROR_1("qmi_dsdi_generate_wqe_profile_type_met_ind error %d", rc);
      }
    }

    next_client_buf_ptr =
      q_next(client_q_ptr, &((q_generic_item_type *)client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
} /*qmi_dsdi_generate_wqe_profile_type_met_ind*/

/*===========================================================================
  FUNCTION dsdi_nat_keep_alive_cb()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    NAT Keep alive CB

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    subscription_id: Subscription id
    event_info_ptr : NAT keep alive information
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsdi_nat_keep_alive_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_dsdi_cmd_buf_type                        * cmd_ptr = NULL;
  ps_sys_event_nat_keep_alive_info_type        * ps_nat_keep_alive = NULL;
  uint32                                         tmp_v4 = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_4(" tech_type %d event_name %d"
                  " event_info_ptr 0x%x Subscription id %d",
                  tech_type, event_name, event_info_ptr,subscription_id);
                 
  ASSERT(event_info_ptr);

  ps_nat_keep_alive = 
    (ps_sys_event_nat_keep_alive_info_type *)event_info_ptr;
  
  cmd_ptr = (qmi_dsdi_cmd_buf_type *)qmi_dsdi_get_cmd_buf();

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI DSD cmd buf");
    return;
  }
  
  memset(cmd_ptr,0,sizeof(qmi_dsdi_cmd_buf_type));

  cmd_ptr->data.nat_keep_alive_info.trigger_nat_keep_alive = 
    ps_nat_keep_alive->is_start;
  
  if (IPV4_ADDR == ps_nat_keep_alive->dest_ip_addr.type)
  {
    cmd_ptr->data.nat_keep_alive_info.dest_v4_address_valid = TRUE;
    tmp_v4 = (uint32)ps_ntohl(ps_nat_keep_alive->dest_ip_addr.addr.v4.ps_s_addr);
    cmd_ptr->data.nat_keep_alive_info.dest_v4_address = tmp_v4;
  }
  else if (IPV6_ADDR == ps_nat_keep_alive->dest_ip_addr.type)
  {
    cmd_ptr->data.nat_keep_alive_info.dest_ipv6_address_valid= TRUE;
    memscpy(cmd_ptr->data.nat_keep_alive_info.dest_ipv6_address,
            sizeof(cmd_ptr->data.nat_keep_alive_info.dest_ipv6_address),
            &(ps_nat_keep_alive->dest_ip_addr.addr.v6.in6_u),
            sizeof(ps_nat_keep_alive->dest_ip_addr.addr.v6.in6_u));
  }
  else
  {
    LOG_MSG_ERROR_1("Unknown Ip version %d", 
                    ps_nat_keep_alive->dest_ip_addr.type);
  }
  cmd_ptr->data.nat_keep_alive_info.dest_port_valid = TRUE;
  cmd_ptr->data.nat_keep_alive_info.dest_port =
    ps_nat_keep_alive->dest_port;

  cmd_ptr->data.nat_keep_alive_info.source_port_valid = TRUE;
  cmd_ptr->data.nat_keep_alive_info.source_port =
    ps_nat_keep_alive->source_port;

  cmd_ptr->data.nat_keep_alive_info.timer_value_valid = TRUE;
  cmd_ptr->data.nat_keep_alive_info.timer_value =
    ps_nat_keep_alive->timer_value;

  cmd_ptr->cmd_id = QMI_CMD_DSD_NAT_KEEP_ALIVE_EVT;
  cmd_ptr->subscription_id = subscription_id;
  dcc_send_cmd_ex(DCC_DSD_SVC_CMD, cmd_ptr); 
} /* dsdi_nat_keep_alive_cb */

/*===========================================================================
FUNCTION  QMI_DSDI_GENERATE_NAT_KEEP_ALIVE_IND

DESCRIPTION
  This callback is called to notify clients about the nat keep 
  alive information that is required on the application

PARAMETERS
  nat_keep_alive_ind : NAT Keep alive Ind
  subscription_id    : Subs identifier on which event was triggered
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_dsdi_generate_nat_keep_alive_ind
(
  dsd_nat_keep_alive_info_ind_msg_v01    * nat_keep_alive_ind,
  ps_sys_subscription_enum_type            subscription_id
)
{
  qmi_dsdi_client_info_type                   * cl_sp;
  q_type                                      * client_q_ptr = NULL;
  void                                        * client_buf_ptr;
  void                                        *next_client_buf_ptr;
  qmi_dsdi_state_info_type                    *dsd_sp;
  qmi_csi_error                               rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  dsd_sp         = &qmi_dsd_state;
  client_q_ptr   = &dsd_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);

  while (client_buf_ptr != NULL)
  {
    cl_sp = (qmi_dsdi_client_info_type *)client_buf_ptr;

    if (cl_sp == NULL)
    {
      DATA_ERR_FATAL("qmi_dsdi_generate_nat_keep_alive_ind : Null client_info");
      return;
    }

    if ((TRUE == cl_sp->report_status.report_nat_keep_alive) &&
        (TRUE == qmi_dsdi_verify_client_subs(cl_sp, (uint32)subscription_id)))
    {
      rc = qmi_csi_send_ind(cl_sp->clnt,
                            QMI_DSD_NAT_KEEP_ALIVE_INFO_IND_V01,
                            nat_keep_alive_ind,
                            sizeof(dsd_nat_keep_alive_info_ind_msg_v01));

      if (rc != QMI_CSI_NO_ERR)
      {
        LOG_MSG_ERROR_1("qmi_dsdi_generate_nat_keep_alive_ind error %d", rc);
      }
    }

    next_client_buf_ptr =
      q_next(client_q_ptr, &((q_generic_item_type *)client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
} /*qmi_dsdi_generate_nat_keep_alive_ind*/

/*===========================================================================
  FUNCTION QMI_DSDI_NAT_KEEP_ALIVE_CONFIG_STATUS()

  DESCRIPTION
   AP signals whether it is ok with the
   previously sent QMI_DSD_NAT_KEEP_ALIVE_IND_V01

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error  qmi_dsdi_nat_keep_alive_config_status(
  qmi_dsdi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
)
{
  dsd_nat_keep_alive_config_status_req_msg_v01    *req_ptr;
  dsd_nat_keep_alive_config_status_resp_msg_v01    resp_msg;
  ps_sys_wlan_nat_keep_alive_config_type           ps_config_status;
  int16    ps_result = 0;
  int16    ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  req_ptr = (dsd_nat_keep_alive_config_status_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_nat_keep_alive_config_status_resp_msg_v01));
  memset(&ps_config_status, 
         0, 
         sizeof(ps_sys_wlan_nat_keep_alive_config_type));
  LOG_MSG_INFO1_1("In QMI DSDI NAT Keep Alive Config Status message "
                  "status %d", req_ptr->status);

  if (req_ptr->status > DSD_NAT_KEEP_ALIVE_CONFIG_ERROR_V01 || 
      req_ptr->status < DSD_NAT_KEEP_ALIVE_CONFIG_OK_V01)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  ps_config_status.status = 
    (ps_sys_wlan_nat_keep_alive_config_status_enum_type)req_ptr->status;

  ps_result = ps_sys_ioctl_ex(PS_SYS_TECH_WLAN,
                              PS_SYS_IOCTL_WLAN_NAT_KEEP_ALIVE_CONFIG_STATUS,
                              (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                              (void*)&ps_config_status,
                              &ps_errno);

  if (ps_result != 0)
  {

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;

  } 
  else
  {
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    resp_msg.resp.error = QMI_ERR_NONE_V01;
  }

send_result:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_wifi_meas_report_config_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/*qmi_dsdi_nat_keep_alive_config_status*/

/*===========================================================================
  FUNCTION QMI_DSDI_WQE_PROFILE_QUALITY_STATUS()

  DESCRIPTION
   AP signals the quality status for WQE profile

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error  qmi_dsdi_wqe_profile_quality_status
(
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
)
{
  dsd_set_wqe_profile_quality_status_req_msg_v01    *req_ptr;
  dsd_set_wqe_profile_quality_status_resp_msg_v01    resp_msg;
  ps_sys_wlan_set_wqe_profile_status_type            ps_wqe_quality_status;
  int16    ps_result = 0;
  int16    ps_errno = 0;
  int      result = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  req_ptr = (dsd_set_wqe_profile_quality_status_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_set_wqe_profile_quality_status_resp_msg_v01));
  memset(&ps_wqe_quality_status, 
         0, 
         sizeof(ps_sys_wlan_set_wqe_profile_status_type));
 
  result = qmi_dsdi_convert_qmi_wqe_profile_to_ps_wqe_profile(
                        req_ptr->wqe_profile,
                        &ps_wqe_quality_status.wqe_profile);

  if (result != 0)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  if (TRUE == req_ptr->quality_status_valid)
  {
    if ((req_ptr->quality_status > 
        QMI_DSD_WQE_PROFILE_QUALITY_UNKNOWN_V01) || 
        (req_ptr->quality_status < 
        QMI_DSD_WQE_PROFILE_QUALITY_MET_V01))
    {
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }

    ps_wqe_quality_status.wqe_status = 
      (ds_sys_wlan_wqe_profile_status_enum_type)req_ptr->quality_status;
  }
  else
  {
    ps_wqe_quality_status.wqe_status = 
      DS_SYS_WLAN_WQE_PROFILE_STATUS_QUALITY_INVALID;
  }

  if (TRUE == req_ptr->status_code_valid) 
  {
    if ((req_ptr->status_code > 
        QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_INTERNET_CONNECTIVITY_FAIL_V01) 
        || (req_ptr->status_code < QMI_DSD_WQE_PROFILE_QUALITY_STATUS_CODE_NONE_V01))
    {
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }

    ps_wqe_quality_status.cause_code = 
      (ds_sys_wqe_profile_cause_code_enum_type)req_ptr->status_code;
  }
  else
  {
    ps_wqe_quality_status.cause_code = 
      DS_SYS_WQE_PROFILE_ERROR_NONE;
  }

  ps_result = ps_sys_ioctl_ex(PS_SYS_TECH_WLAN,
                              PS_SYS_IOCTL_WLAN_SET_WQE_PROFILE_QUALITY_STATUS,
                              (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                              (void*)&ps_wqe_quality_status,
                              &ps_errno);

  if (ps_result != 0)
  {

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;

  } 
  else
  {
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    resp_msg.resp.error = QMI_ERR_NONE_V01;
  }

send_result:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_set_wqe_profile_quality_status_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/*dsd_set_wqe_profile_quality_status*/

/*===========================================================================
  FUNCTION dsdi_wqe_profile_init_cb()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    WQE Profile Init CB

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    subscription_id: Subscription id
    event_info_ptr : WQE Profile event 
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsdi_wqe_profile_init_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_dsdi_cmd_buf_type                               * cmd_ptr = NULL;
  ps_sys_event_set_wqe_profile_quality_meas_info_type * ps_wqe_profile_init = NULL;
  int                                                 result = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(event_info_ptr);

  ps_wqe_profile_init = 
    (ps_sys_event_set_wqe_profile_quality_meas_info_type *)event_info_ptr;
  
  cmd_ptr = (qmi_dsdi_cmd_buf_type *)qmi_dsdi_get_cmd_buf();

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI DSD cmd buf");
    return;
  }
  
  memset(cmd_ptr,0,sizeof(qmi_dsdi_cmd_buf_type));

  result = qmi_dsdi_convert_ps_wqe_profile_to_qmi_wqe_profile(
     ps_wqe_profile_init->wqe_profile,
     &(cmd_ptr->data.wqe_profile_init_info.wqe_profile));

  if (result)
  {
    LOG_MSG_ERROR_0("Unknown WQE Profile Type passed");
    if (cmd_ptr)
    {
      PS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
    }
    return;
  }

  cmd_ptr->data.wqe_profile_init_info.wifi_rssi_threshold_high_valid =
    ps_wqe_profile_init->rssi_threshold.is_rssi_high_valid;
  cmd_ptr->data.wqe_profile_init_info.wifi_rssi_threshold_high = 
    ps_wqe_profile_init->rssi_threshold.rssi_high;
  cmd_ptr->data.wqe_profile_init_info.wifi_rssi_threshold_low_valid = 
    ps_wqe_profile_init->rssi_threshold.is_rssi_low_valid;
  cmd_ptr->data.wqe_profile_init_info.wifi_rssi_threshold_low = 
    ps_wqe_profile_init->rssi_threshold.rssi_low;

 
  cmd_ptr->cmd_id = QMI_CMD_DSD_WQE_PROFILE_INIT_EVT;
  cmd_ptr->subscription_id = subscription_id;
  dcc_send_cmd_ex(DCC_DSD_SVC_CMD, cmd_ptr); 
} /* dsdi_wqe_profile_init_cb */

/*===========================================================================
FUNCTION  QMI_DSDI_GENERATE_WQE_PROFILE_INIT_IND

DESCRIPTION
  This callback is called to notify clients about the WQE profile
  init indication and details realted to them

PARAMETERS
  WQE Profile        : NAT Keep alive Ind
  subscription_id    : Subs identifier on which event was triggered
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_dsdi_generate_wqe_profile_init_ind
(
  dsd_wqe_profile_init_ind_msg_v01       * profile_init_info,
  ps_sys_subscription_enum_type            subscription_id
)
{
  qmi_dsdi_client_info_type                   * cl_sp;
  q_type                                      * client_q_ptr = NULL;
  void                                        * client_buf_ptr;
  void                                        *next_client_buf_ptr;
  qmi_dsdi_state_info_type                    *dsd_sp;
  qmi_csi_error                               rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  dsd_sp         = &qmi_dsd_state;
  client_q_ptr   = &dsd_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);

  while (client_buf_ptr != NULL)
  {
    cl_sp = (qmi_dsdi_client_info_type *)client_buf_ptr;

    if ((TRUE == cl_sp->report_status.report_wqe_profile_type_changes) &&
        (TRUE == qmi_dsdi_verify_client_subs(cl_sp, (uint32)subscription_id)))
    {
      rc = qmi_csi_send_ind(cl_sp->clnt,
                            QMI_DSD_WQE_PROFILE_INIT_IND_V01,
                            profile_init_info,
                            sizeof(dsd_wqe_profile_init_ind_msg_v01));

      if (rc != QMI_CSI_NO_ERR)
      {
        LOG_MSG_ERROR_1("qmi_dsdi_generate_wqe_profile_init_ind error %d", rc);
      }
    }

    next_client_buf_ptr =
      q_next(client_q_ptr, &((q_generic_item_type *)client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
} /*qmi_dsdi_generate_wqe_profile_init_ind*/

/*===========================================================================
  FUNCTION QMI_DSDI_SET_QUALITY_MEASUREMENT_INFO()

  DESCRIPTION
   AP signals the quality status for WQE profile

  PARAMETERS
    clnt_info     : client state pointer
    req_handle    : Request message handle
    msg_id        : Message ID
    req_c_struct  : Request structure
    req_c_struct_len : Length of request
    service_handle : Service state pointer

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error  qmi_dsdi_set_quality_measurement_info
(
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
)
{
  dsd_set_quality_measurement_info_req_msg_v01    *req_ptr;
  dsd_set_quality_measurement_info_resp_msg_v01    resp_msg;
  ps_sys_quality_meas_policy_change_type           *ps_wqe_quality_meas_info = NULL;
  int16    ps_result = 0;
  int16    ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && 
         service_handle);

  req_ptr = (dsd_set_quality_measurement_info_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(dsd_set_quality_measurement_info_resp_msg_v01));

 

  PS_SYSTEM_HEAP_MEM_ALLOC(ps_wqe_quality_meas_info, 
                           sizeof(ps_sys_quality_meas_policy_change_type),
                           ps_sys_quality_meas_policy_change_type*);
  if(!ps_wqe_quality_meas_info)
  {
    resp_msg.resp.error = QMI_ERR_NO_MEMORY_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto send_result;
  }

   memset(ps_wqe_quality_meas_info, 
         0, 
         sizeof(ps_sys_quality_meas_policy_change_type));

  //In call Wifi RSSI
  ps_wqe_quality_meas_info->in_call_wifi_rssi_threshold_params.is_rssi_high_valid
    = req_ptr->in_call_wifi_rssi_threshold_high_valid;
  ps_wqe_quality_meas_info->in_call_wifi_rssi_threshold_params.rssi_high
    = req_ptr->in_call_wifi_rssi_threshold_high;
  ps_wqe_quality_meas_info->in_call_wifi_rssi_threshold_params.is_rssi_low_valid
    = req_ptr->in_call_wifi_rssi_threshold_low_valid;
  ps_wqe_quality_meas_info->in_call_wifi_rssi_threshold_params.rssi_low
    = req_ptr->in_call_wifi_rssi_threshold_low;

  //Idle Wifi RSSI
  ps_wqe_quality_meas_info->idle_wifi_rssi_threshold_params.is_rssi_high_valid
    = req_ptr->idle_wifi_rssi_threshold_high_valid;
  ps_wqe_quality_meas_info->idle_wifi_rssi_threshold_params.rssi_high
    = req_ptr->idle_wifi_rssi_threshold_high;
  ps_wqe_quality_meas_info->idle_wifi_rssi_threshold_params.is_rssi_low_valid
    = req_ptr->idle_wifi_rssi_threshold_low_valid;
  ps_wqe_quality_meas_info->idle_wifi_rssi_threshold_params.rssi_low
    = req_ptr->idle_wifi_rssi_threshold_low;

  //In call Wifi SINR
  ps_wqe_quality_meas_info->in_call_wifi_sinr_threshold_params.is_sinr_high_valid
    = req_ptr->in_call_wifi_sinr_threshold_high_valid;
  ps_wqe_quality_meas_info->in_call_wifi_sinr_threshold_params.sinr_high
    = req_ptr->in_call_wifi_sinr_threshold_high;
  ps_wqe_quality_meas_info->in_call_wifi_sinr_threshold_params.is_sinr_low_valid
    = req_ptr->in_call_wifi_sinr_threshold_low_valid;
  ps_wqe_quality_meas_info->in_call_wifi_sinr_threshold_params.sinr_low
    = req_ptr->in_call_wifi_sinr_threshold_low;

  //Idle Wifi SINR
  ps_wqe_quality_meas_info->idle_wifi_sinr_threshold_params.is_sinr_high_valid
    = req_ptr->idle_wifi_sinr_threshold_high_valid;
  ps_wqe_quality_meas_info->idle_wifi_sinr_threshold_params.sinr_high
    = req_ptr->idle_wifi_sinr_threshold_high;
  ps_wqe_quality_meas_info->idle_wifi_sinr_threshold_params.is_sinr_low_valid
    = req_ptr->idle_wifi_sinr_threshold_low_valid;
  ps_wqe_quality_meas_info->idle_wifi_sinr_threshold_params.sinr_low
    = req_ptr->idle_wifi_sinr_threshold_low;


  //LTE In Call
  ps_wqe_quality_meas_info->in_call_lte_threshold_params.is_rsrp_low_valid
    = req_ptr->in_call_lte_rsrp_low_valid;
  ps_wqe_quality_meas_info->in_call_lte_threshold_params.rsrp_low
    = req_ptr->in_call_lte_rsrp_low;
  ps_wqe_quality_meas_info->in_call_lte_threshold_params.is_rsrp_high_valid
    = req_ptr->in_call_lte_rsrp_high_valid;
  ps_wqe_quality_meas_info->in_call_lte_threshold_params.rsrp_high
    = req_ptr->in_call_lte_rsrp_high;
  ps_wqe_quality_meas_info->in_call_lte_threshold_params.is_rsrp_mid_valid
    = req_ptr->in_call_lte_rsrp_mid_valid;
  ps_wqe_quality_meas_info->in_call_lte_threshold_params.rsrp_mid
    = req_ptr->in_call_lte_rsrp_mid;

  //LTE Idle
  ps_wqe_quality_meas_info->idle_lte_threshold_params.is_rsrp_low_valid
    = req_ptr->idle_lte_rsrp_low_valid;
  ps_wqe_quality_meas_info->idle_lte_threshold_params.rsrp_low
    = req_ptr->idle_lte_rsrp_low;
  ps_wqe_quality_meas_info->idle_lte_threshold_params.is_rsrp_high_valid
    = req_ptr->idle_lte_rsrp_high_valid;
  ps_wqe_quality_meas_info->idle_lte_threshold_params.rsrp_high
    = req_ptr->idle_lte_rsrp_high;
  ps_wqe_quality_meas_info->idle_lte_threshold_params.is_rsrp_mid_valid
    = req_ptr->idle_lte_rsrp_mid_valid;
  ps_wqe_quality_meas_info->idle_lte_threshold_params.rsrp_mid
    = req_ptr->idle_lte_rsrp_mid;

  //ECIO val
  ps_wqe_quality_meas_info->ecio_threshold_params.is_ecio_high_valid
    = req_ptr->ecio_1x_threshold_high_valid;
  ps_wqe_quality_meas_info->ecio_threshold_params.ecio_high
    = req_ptr->ecio_1x_threshold_high;
  ps_wqe_quality_meas_info->ecio_threshold_params.is_ecio_low_valid
    = req_ptr->ecio_1x_threshold_low_valid;
  ps_wqe_quality_meas_info->ecio_threshold_params.ecio_low
    = req_ptr->ecio_1x_threshold_low;

  ps_result = ps_sys_event_ind_ex(PS_SYS_TECH_WLAN,
                                  PS_SYS_EVENT_QUALITY_MEAS_POLICY_CHANGE,
                                  (ps_sys_subscription_enum_type)qmi_dsdi_resolve_client_subs(clnt_info),
                                  (void*)ps_wqe_quality_meas_info,
                                  &ps_errno);

  if (ps_result != 0)
  {

    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;

  } 
  else
  {
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    resp_msg.resp.error = QMI_ERR_NONE_V01;
  }

send_result:
  if (ps_wqe_quality_meas_info)
  {
    PS_SYSTEM_HEAP_MEM_FREE(ps_wqe_quality_meas_info);
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_set_quality_measurement_info_resp_msg_v01));


  return QMI_CSI_CB_NO_ERR;
}/*dsd_set_wqe_profile_quality_status*/

/*===========================================================================
  FUNCTION QMI_DSDI_CONVERT_PS_WQE_PROFILE_TO_QMI_WQE_PROFILE()

  DESCRIPTION
   INternal function to convert PS WQE to QMI

  PARAMETERS
    ps_wqe: Ps WQE
    qmi_wqe: QMI WQE

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qmi_dsdi_convert_ps_wqe_profile_to_qmi_wqe_profile
(
  ps_sys_wqe_profile_type ps_wqe,
  dsd_wqe_profile_type_enum_v01 *qmi_wqe
)
{
  int result = 0;

  if (ps_wqe ==
      DS_SYS_WQE_PROFILE_INACTIVE)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_DEFAULT_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_INTERNET)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_INTERNET_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_AUDIO)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_AUDIO_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_VIDEO)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_VIDEO_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_1)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_1_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_2)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_2_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_3)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_3_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_4)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_4_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_5)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_5_V01;
  }
   else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_6)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_6_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_7)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_7_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_8)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_8_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_9)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_9_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_10)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_10_V01;
  }
     else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_11)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_11_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_12)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_12_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_13)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_13_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_14)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_14_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_15)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_15_V01;
  }
     else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_16)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_16_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_17)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_17_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_18)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_18_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_19)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_19_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_20)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_20_V01;
  }
     else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_21)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_21_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_22)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_22_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_23)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_23_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_24)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_24_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_25)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_25_V01;
  }
     else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_26)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_26_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_27)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_27_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_28)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_28_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_29)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_29_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_30)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_30_V01;
  }
     else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_31)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_31_V01;
  }
  else if (ps_wqe ==
      DS_SYS_WQE_PROFILE_IMS_TYPE_32)
  {
    *qmi_wqe
      = QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_32_V01;
  }
  else
  {
    result = -1;
  }

  return result;
}

/*===========================================================================
  FUNCTION QMI_DSDI_CONVERT_QMI_WQE_PROFILE_TO_PS_WQE_PROFILE()

  DESCRIPTION
   INternal function to convert QMI WQE to PS WQE

  PARAMETERS
    ps_wqe: Ps WQE
    qmi_wqe: QMI WQE

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qmi_dsdi_convert_qmi_wqe_profile_to_ps_wqe_profile
(
  dsd_wqe_profile_type_enum_v01 qmi_wqe,
  ps_sys_wqe_profile_type *ps_wqe
)
{
  int result = 0;

  if (qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_DEFAULT_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_INACTIVE;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_INTERNET_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_INTERNET;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_AUDIO_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_AUDIO;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_VIDEO_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_VIDEO;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_1_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_1;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_2_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_2;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_3_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_3;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_4_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_4;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_5_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_5;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_6_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_6;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_7_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_7;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_8_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_8;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_9_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_9;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_10_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_10;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_11_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_11;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_12_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_12;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_13_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_13;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_14_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_14;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_15_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_15;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_16_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_16;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_17_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_17;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_18_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_18;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_19_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_19;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_20_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_20;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_21_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_21;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_22_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_22;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_23_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_23;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_24_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_24;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_25_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_25;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_26_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_26;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_27_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_27;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_28_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_28;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_29_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_29;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_30_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_30;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_31_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_31;
  }
  else if(qmi_wqe == QMI_DSD_WQE_PROFILE_TYPE_IMS_TYPE_32_V01)
  {
    *ps_wqe = DS_SYS_WQE_PROFILE_IMS_TYPE_32;
  }
  else
  {
    result = -1;
  }

  return result;
}
/*===========================================================================
  FUNCTION DSDI_ROAMING_STATUS_CHANGE_CB()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    roaming status change indication

  PARAMETERS
    tech_type      : technology for which roaming status has changed
    event_name     : Event name
    subscription_id: Subscription id
    event_info_ptr : Roaming status event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void dsdi_roaming_status_change_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_dsdi_cmd_buf_type                 * cmd_ptr = NULL;
  ps_sys_roaming_status_info_type       * ps_roaming_status_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_4("dsdi_roaming_status_change_cb tech_type %d event_name %d"
                  " event_info_ptr 0x%x Subscription id %d",
                  tech_type, event_name, event_info_ptr,subscription_id);
                 
  ASSERT(event_info_ptr);

  ps_roaming_status_ptr = (ps_sys_roaming_status_info_type *)event_info_ptr;
  
  cmd_ptr = (qmi_dsdi_cmd_buf_type *)qmi_dsdi_get_cmd_buf();

  if( cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Unable to allocate QMI DSD cmd buf");
    return;
  }
  
  memset(cmd_ptr,0,sizeof(qmi_dsdi_cmd_buf_type));

  cmd_ptr->data.roaming_status_info.roaming_status = 
    ps_roaming_status_ptr->roaming_status;
  cmd_ptr->data.roaming_status_info.roaming_status_valid = TRUE;      
  cmd_ptr->data.roaming_status_info.technology = 
    (dsd_sys_network_enum_v01)ps_roaming_status_ptr->technology;
  cmd_ptr->cmd_id = QMI_CMD_DSD_ROAMING_STATUS_CHANGE_EVT;
  cmd_ptr->subscription_id = subscription_id;

  dcc_send_cmd_ex (DCC_DSD_SVC_CMD, cmd_ptr);
} /* dsdi_roaming_status_change_cb() */

/*===========================================================================
FUNCTION  QMI_DSDI_GENERATE_ROAMING_STATUS_CHANGE_IND

DESCRIPTION
  This callback is called to notify clients about roaming status changes

PARAMETERS
  roaming_status_info : Roaming status info change payload
  subscription_id : Subs identifier on which event was triggered
 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void qmi_dsdi_generate_roaming_status_change_ind
(
  dsd_roaming_status_change_ind_msg_v01 * roaming_status_info,
  ps_sys_subscription_enum_type         subscription_id
)
{
  qmi_dsdi_client_info_type                   * cl_sp;
  q_type                                      * client_q_ptr = NULL;
  void                                        * client_buf_ptr;
  void                                        * next_client_buf_ptr;
  qmi_dsdi_state_info_type                    * dsd_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsd_sp         = &qmi_dsd_state;
  client_q_ptr   = &dsd_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);
  
  while(client_buf_ptr != NULL)
  {
    cl_sp = (qmi_dsdi_client_info_type *) client_buf_ptr;
    
    if ((TRUE == cl_sp->report_status.report_roaming_status_change) &&
        (TRUE == qmi_dsdi_verify_client_subs(cl_sp, (uint32)subscription_id)) )
    {    
        qmi_dsdi_roaming_status_change_ind(cl_sp, roaming_status_info);
    }
    
    next_client_buf_ptr = 
      q_next(client_q_ptr, &((q_generic_item_type *) client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
} /* qmi_dsdi_generate_roaming_status_change_ind */

/*===========================================================================
  FUNCTION QMI_DSDI_ROAMING_STATUS_CHANGE_IND()

  DESCRIPTION
    This function sends roaming status change indication to registered 
    control point.

  PARAMETERS
    clid                    : clientID
    roaming_status_info     : Roaming status info
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dsdi_roaming_status_change_ind
(
  void                                  * client,
  dsd_roaming_status_change_ind_msg_v01 * roaming_status_info
)
{
  qmi_dsdi_client_info_type               * client_info;
  qmi_csi_error                             rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  rc = QMI_CSI_NO_ERR;
  client_info = (qmi_dsdi_client_info_type *) client;
  if (client_info == NULL)
  {
    ASSERT(0);
    return;
  }
  
  rc = qmi_csi_send_ind(client_info->clnt, 
                        QMI_DSD_ROAMING_STATUS_CHANGE_IND_V01,
                        roaming_status_info, 
                        sizeof(dsd_roaming_status_change_ind_msg_v01));
  
  if ( rc != QMI_CSI_NO_ERR )
  {
    LOG_MSG_ERROR_1("qmi_dsdi_roaming_status_change_ind error %d", rc);
  }

} /* qmi_dsdi_roaming_status_change_ind() */