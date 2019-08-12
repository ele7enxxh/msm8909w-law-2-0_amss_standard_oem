/*===========================================================================

                         D S _ Q M I _ C S V T . C

DESCRIPTION

 The Qualcomm MSM Interface for Circuit-switched Video Telephony source file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_csvt.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
12/12/13    sah    Fixed kw errors
11/01/13    gk     Added DSDS support
08/22/13    gk     Made sure that cmd_callbacks array doesn't have 
                   a NULL entry.
08/01/13    gk     MO call failure when trying to originate call.
07/19/13    gk     QRD changes porting
09/10/12    sb     Len checks for QMI TLVs
08/06/12    gk     Added support for Logging of what Messages/TLVs are 
                   supported by this service
08/01/12    sb     Fixed KW Errors.
07/31/12    sb     Fixed KW Errors.
07/26/12    sb     Fixed KW Errors.
03/16/12    sa     Adding new messages and indication.
02/17/12    sb      Fixed Compiler and KW Warnings.
01/05/12    sa     New QMI CSVT service.
08/22/11    kk     Initial version.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "amssassert.h"

#include "ds_qmi_svc_ext.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_task.h"
#include "ds_qmi_csvt.h"
#include "dcc_task_defs.h"
#include "dcc_task_svc.h"
#include "ps_system_heap.h"
#include "ds_qmux.h"

#include "dsucsdappif_apis.h"

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  #include "ds3gmgr.h"
#endif /* (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM) */

#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "circuit_switched_video_telephony_v01.h"
#include "circuit_switched_video_telephony_impl_v01.h"

#include <stringl/stringl.h>

#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"
#include "qmi_framework.h"
#include "ds_qmi_fw_common.h"
#include "ps_utils.h"

#if (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS))

/*===========================================================================

                          VARIABLE TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================
                  Service management macros and variables
===========================================================================*/

#define CSVTI_BASE_VER_MAJOR (1)
#define CSVTI_BASE_VER_MINOR (1)

#define CSVTI_ADDENDUM_VER_MAJOR (0)
#define CSVTI_ADDENDUM_VER_MINOR (0)

/* The UCSD module allows atmost two clients to register at once */
#define CSVTI_MAX_CLIDS (DSUCSD_API_MAX_CLIENTS)
#define MAX_UCSD_CALLS 2
#define CSVTI_INVALID_CLIENT     255
#define CSVTI_INVALID_INSTANCE    -1

#define QMI_CSVT_INCOMING_NUMBER_MAX_SIZE       16
#define QMI_CSVT_UUS_ID_MAX_SIZE                32
#define MAX_DIAL_STRING_LEN                     80
#define QMI_CSVT_DEFAULT_NETWORK_DATA_RATE      134

#define CSVTI_PRM_TYPE_CALL_TYPE               (0x10)

/* Call Stats TLV's */
#define CSVTI_PRM_TYPE_STATS_CALL_ACTIVE       (0x10)
#define CSVTI_PRM_TYPE_STATS_TX_COUNTER        (0x11)
#define CSVTI_PRM_TYPE_STATS_RX_COUNTER        (0x12)

/* set rlp tlv's */
#define CSVTI_PRM_TYPE_RLP_PARAMS              (0x10)
#define CSVTI_PRM_TYPE_V42_PARAMS              (0x11)
#define CSVTI_PRM_TYPE_RLP2_PARAMS             (0x12)
#define CSVTI_PRM_TYPE_RLP3_PARAMS             (0x13)

/*Modify req tlv's */
#define CSVTI_PRM_TYPE_NEW_CALL_TYPE           (0x02)
#define CSVTI_PRM_TYPE_NEW_REQUEST             (0x03)

/*Ack Modify req tlv's */
#define CSVTI_PRM_TYPE_ACCEPT_REQUEST          (0x02)

/*Originate call tlv's */
#define CSVTI_PRM_TYPE_INST_ID                 (0x01)
#define CSVTI_PRM_TYPE_CALL_MODE               (0x02)
#define CSVTI_PRM_TYPE_DIAL_STRING             (0x03)
#define CSVTI_PRM_TYPE_NETWORK_DATARATE        (0x10)
#define CSVTI_PRM_TYPE_AIR_IFACE_DATA_RATE     (0x11)
#define CSVTI_PRM_TYPE_SYNC_CALL               (0x12)
#define CSVTI_PRM_TYPE_TRANSPARENT_CALL        (0x13)
#define CSVTI_PRM_TYPE_CLI_ENABLED             (0x14)
#define CSVTI_PRM_TYPE_CUG_ENABLED             (0x15)
#define CSVTI_PRM_TYPE_CUG_INDEX               (0x16)
#define CSVTI_PRM_TYPE_SUPRESS_PREFERRED_CUG   (0x17)
#define CSVTI_PRM_TYPE_SUPRESS_OUTGOING_ACCESS (0x18)
#define CSVTI_PRM_TYPE_UUS_ID_TYPE             (0x19)

/* Event report ind tlv's */
#define CSVTI_PRM_TYPE_INSTANCE_ID             (0x01)
#define CSVTI_PRM_TYPE_EVENT_TYPE              (0x02)
#define CSVTI_PRM_TYPE_CALL_TYPE_IND           (0x10)
#define CSVTI_PRM_TYPE_SYNC_CALL_IND           (0x11)
#define CSVTI_PRM_TYPE_TRANSPARENT_CALL_IND    (0x12)
#define CSVTI_PRM_TYPE_NETWORK_TYPE_IND        (0x13)
#define CSVTI_PRM_TYPE_NETWORK_SPEED_IND       (0x14)
#define CSVTI_PRM_TYPE_MAX_FRAME_SIZE_IND      (0x15)
#define CSVTI_PRM_TYPE_INCOMING_NUM_IND        (0x16)
#define CSVTI_PRM_TYPE_UUS_ID_TYPE_IND         (0x17)
#define CSVTI_PRM_TYPE_MODIFY_ALLOWED_IND      (0x18)
#define CSVTI_PRM_TYPE_CALL_END_IND            (0x19)
#define CSVTI_PRM_TYPE_PORT_DATA_IND           (0x1A)
#define CSVTI_PRM_TYPE_INCOMING_NUM_LEN_IND    (0x1B)

#define CSVTI_PRM_TYPE_CALL_ACCEPT             (0x02)
#define CSVTI_PRM_TYPE_REJECT_VALUE            (0x10)

#define CSVTI_PRM_TYPE_REPORT_CALL_EVENTS      (0x10)
#define CSVTI_PRM_TYPE_CALL_TYPES_MASK         (0x11)

#define CSVTI_PRM_TYPE_CALL_REQUEST_STATUS     (0x02)
#define CSVTI_PRM_TYPE_CALL_END_CAUSE          (0x10)

/*---------------------------------------------------------------------------
  bind_subscription TLV definitions
---------------------------------------------------------------------------*/
#define CSVTI_PRM_TYPE_BIND_SUBSCRIPTION (0x10)

/* Macro for permitted call types */
#define CSVT_CALL_TYPES_MASK \
  ( (uint8)DS_UCSD_ASYNC_CSD_CALL | \
    (uint8)DS_UCSD_SYNC_CSD_CALL | \
    (uint8)DS_UCSD_VIDEO_TELEPHONY_CALL )

/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

#define HANDLE_FAILURE()  dsm_free_packet(&response); \
                          errval = QMI_ERR_NO_MEMORY; \
                          goto send_result

typedef struct
{
  uint16  cmd_id; /* qmi_cmd_id_e_type */
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
      uint8               inst_id;
      sys_sys_mode_e_type network_mode;
      uint8               speed;
      uint8               name;
      uint8               connection_element;
      uint8               waiur;
      uint8               caller_number[QMI_CSVT_INCOMING_NUMBER_MAX_SIZE];
      uint8               caller_number_len;
      uint8               uus_data[QMI_CSVT_UUS_ID_MAX_SIZE];
      uint8               uus_data_len;
    } incoming_evt_info;

    struct
    {
      uint8              inst_id;
      ds_ucsd_call_type  call_type;
      boolean            modify_allowed;
    } confirm_evt_info;

    struct
    {
      uint8              inst_id;
      ds_ucsd_call_type  call_type;
      boolean            modify_allowed;
      sio_port_id_type   port;
    } connect_evt_info;

    struct
    {
      uint8              inst_id;
      ds_ucsd_call_type  call_type;
      boolean            modify_allowed;
    } setup_evt_info;

    struct
    {
      uint8              inst_id;
      ds_ucsd_call_type  call_type;
      sio_port_id_type   port;
    } progress_evt_info;

    struct
    {
      uint8              inst_id;
      cm_call_end_e_type end_cause;
    } end_evt_info;
  } data;
} qmi_csvti_cmd_buf_type;

/*---------------------------------------------------------------------------
  CSVT client sunscription preference enum
---------------------------------------------------------------------------*/
typedef enum
{
  CSVTI_BIND_SUBS_PRIMARY   = 0x01,
#ifdef FEATURE_DUAL_SIM
  CSVTI_BIND_SUBS_SECONDARY = 0x02,
#ifdef FEATURE_TRIPLE_SIM
  CSVTI_BIND_SUBS_TERTIARY  = 0x03,
#endif
#endif
  CSVTI_BIND_SUBS_MAX
} qmi_csvti_client_subs_e_type;

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  #define CSVTI_MAX_ALLOWED_CLIENTS           CSVTI_BIND_SUBS_MAX - 1
#else
  #define CSVTI_MAX_ALLOWED_CLIENTS 1
#endif

/* The client state structure. */
typedef struct
{
  qmi_common_client_state_type common;
  boolean                      report_call_evt;
  int16                        instance; 
  qmi_csvti_client_subs_e_type subscription_id;
} qmi_csvti_client_state_type;

typedef qmi_csvti_client_state_type* qmi_csvti_client_state_ptr_type; 

/* The service state structure. */
typedef struct
{
  uint16                           num_qmi_instances;  /*Not used at the moment. May be used in the future*/
  qmi_csvti_client_state_ptr_type client[CSVTI_MAX_CLIDS];
} qmi_csvti_state_type;

/*---------------------------------------------------------------------------
  CSVT command enum type.
  This structure is used ONLY for lookup and does not contain any actual
  command id values. The order of the values should be in alignment with the
  'qmi_csvti_cmd_val_e_type' and  'qmi_csvti_cmd_callbacks' below.
---------------------------------------------------------------------------*/
typedef enum
{
  CSVTI_CMD_MIN                   = 0,
  CSVTI_CMD_RESET                 = CSVTI_CMD_MIN,
  CSVTI_CMD_SET_EVENT_REPORT,
  CSVTI_CMD_ORIGINATE_CALL,
  CSVTI_CMD_CONFIRM_CALL,
  CSVTI_CMD_ANSWER_CALL,
  CSVTI_CMD_END_CALL,
  CSVTI_CMD_MODIFY_CALL,
  CSVTI_CMD_ACK_CALL_MODIFY,
  CSVTI_CMD_GET_RLP_PARAMS,
  CSVTI_CMD_SET_RLP_PARAMS,
  CSVTI_CMD_GET_ACTIVE_CALL_LIST,
  CSVTI_CMD_GET_CALL_INFO,
  CSVTI_CMD_GET_CALL_STATS,
  CSVTI_CMD_SET_SUBSCRIPTION_BINDING,
  CSVTI_CMD_GET_BIND_SUBSCRIPTION,
  CSVTI_CMD_MAX, 
  CSVTI_CMD_WIDTH                 = 0xFFFF
} qmi_csvti_cmd_e_type;

/*---------------------------------------------------------------------------
  CSVT command value enum type.
  This structure is used ONLY for lookup and does not contain any actual
  command id values. The order of the values should be in alignment with the
  'qmi_csvti_cmd_e_type' above and  'qmi_csvti_cmd_callbacks' below.
---------------------------------------------------------------------------*/
typedef enum
{
  CSVTI_CMD_VAL_RESET                 = 0x0001,
  CSVTI_CMD_VAL_SET_EVENT_REPORT      = 0x0020,
  CSVTI_CMD_VAL_SET_EVENT_REPORT_IND  = 0x0021,
  CSVTI_CMD_VAL_ORIGINATE_CALL        = 0x0022,
  CSVTI_CMD_VAL_CONFIRM_CALL          = 0x0023,
  CSVTI_CMD_VAL_ANSWER_CALL           = 0x0024,
  CSVTI_CMD_VAL_END_CALL              = 0x0025,
  CSVTI_CMD_VAL_MODIFY_CALL           = 0x0026,
  CSVTI_CMD_VAL_ACK_CALL_MODIFY       = 0x0027,
  CSVTI_CMD_VAL_GET_RLP_PARAMS        = 0x0028,
  CSVTI_CMD_VAL_SET_RLP_PARAMS        = 0x0029,
  CSVTI_CMD_VAL_GET_ACTIVE_CALL_LIST  = 0x002A,
  CSVTI_CMD_VAL_GET_CALL_INFO         = 0x002B,
  CSVTI_CMD_VAL_GET_CALL_STATS        = 0x002C,
  CSVTI_CMD_VAL_SET_SUBSCRIPTION_BINDING      = 0x002D,
  CSVTI_CMD_VAL_GET_BIND_SUBSCRIPTION         = 0x002E,
  CSVTI_CMD_VAL_WIDTH                 = 0xFFFF
} qmi_csvti_cmd_val_e_type;

typedef enum
{
  QMI_CSVTI_PENDING_MIN            = 0,
  QMI_CSVTI_ORIGINATE_CALL,
  QMI_CSVTI_CONFIRM_CALL,
  QMI_CSVTI_ANSWER_CALL,
  QMI_CSVTI_END_CALL,
  QMI_CSVTI_MODIFY_CALL
} qmi_csvti_pending_cmd_e_type;

typedef enum
{
  CSVTI_CALL_CONFIRMATION_EVT            = 0x01,
  CSVTI_CALL_PROGRESS_NOTIFICATION_EVT   = 0x02,
  CSVTI_CALL_CONNECT_NOTIFICATION_EVT    = 0x03,
  CSVTI_CALL_SETUP_NOTIFICATION_EVT      = 0x04,
  CSVTI_CALL_INCOMING_EVT                = 0x05,
  CSVTI_CALL_END_EVT                     = 0x06,
  CSVTI_CALL_MODIFICATION_EVT            = 0x07
} qmi_csvti_call_evt;

typedef struct
{
  uint32  port_family;
  uint32  port_number;
} port_data;

typedef enum
{
  CALL_NOT_ACTIVE                        = 0x00,
  CALL_IN_PROGRESS                       = 0x01,
  CALL_ACTIVE                            = 0x02,
  CALL_ENDED                             = 0x03
} qmi_csvti_call_state;

typedef struct
{
  qmi_csvti_call_state  call_state;
  uint32                call_type;
  boolean               is_synchronous;
  boolean               is_transparent;
  uint32                network_type;
  uint16                network_speed;
  uint8                 max_frame_size;
  uint8                 dial_string[MAX_DIAL_STRING_LEN];
  uint8                 dial_string_len;
  boolean               modify_allowed;
  uint32                call_end_cause;
  port_data             port;
} qmi_csvti_call_info;
/*===========================================================================

                  FORWARD DECLARATIONS FOR INTERNAL FUNCTIONS

===========================================================================*/
static void qmi_csvti_process_svc_init
(
  uint16 num_instances
);

static void qmi_csvti_process_alloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
);

static void qmi_csvti_process_dealloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
);

static void qmi_csvti_process_cmd_hdlr
(
  qmi_framework_msg_hdr_type* msg_hdr,
  dsm_item_type * sdu 
);

static boolean qmi_csvti_send_response
(
  qmi_cmd_buf_type *               cmd_buf_p,
  dsm_item_type *                  msg_ptr
);


static void qmi_csvti_send_ind
(
  qmi_csvti_cmd_val_e_type               cmd,
  dsm_item_type                       *  ind,
  uint8                                 client
);

static uint32  qmi_csvti_get_csvt_client_id_by_cl_sp (
                                qmi_csvti_client_state_type *  cl_sp );

static void qmi_csvti_reset_client( void * );

static dsm_item_type* qmi_csvti_reset( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_set_event_report( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_originate_call( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_confirm_call( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_answer_call( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_end_call( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_modify_call( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_ack_call_modify( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_get_rlp_params( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_set_rlp_params( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_get_call_stats( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_get_call_info( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_get_active_call_list( void *,
                                       void *,
                                       void *,
                                       dsm_item_type **);

static dsm_item_type* qmi_csvti_set_subscription_binding(void *,
                                       void *,
                                       void *,
                                       dsm_item_type ** );

static dsm_item_type* qmi_csvti_get_bind_subscription(void *,
                                       void *,
                                       void *,
                                       dsm_item_type ** );

void qmi_csvt_ucsd_call_event_cb
( 
  const ds_ucsd_call_event_type *event_ptr,
  void                          *user_info_ptr 
);

static boolean qmi_csvti_send_pending_response
(
  qmi_csvti_cmd_val_e_type  cmd_val
);

static void qmi_csvti_process_cmd(void *);

/*---------------------------------------------------------------------------
  QMI callback definition
---------------------------------------------------------------------------*/
static boolean qmi_csvti_alloc_clid_cb
(
  qmi_framework_common_msg_hdr_type * svc_common_hdr  
);

static void qmi_csvti_dealloc_clid_cb
(
  qmi_framework_common_msg_hdr_type * svc_common_hdr 
); 

boolean  qmi_csvt_validate_client_id
(
  uint32   clid,
  uint16   line_num
);

boolean  qmi_csvt_validate_call_instance
(
  uint32   clid,
  uint32   inst_id,
  uint16   line_num
);

static void qmi_csvti_init_cb
(
  uint16 num_instances
);

static void qmi_csvti_cmd_hdlr_cb
( 
  qmi_framework_msg_hdr_type* msg_hdr,
  dsm_item_type ** sdu 
);

static qmi_csvti_client_state_type * qmi_csvti_alloc_cl_sp
(
  uint8 clid
);

static boolean qmi_csvti_dealloc_cl_sp
(
  uint8 clid
);

static void    qmi_csvti_initialize_client(qmi_csvti_client_state_type *);

#define qmi_csvti_free_cmd_buf(buf_ptr) PS_SYSTEM_HEAP_MEM_FREE(buf_ptr)

static boolean    qmi_csvti_read_nv_qmss_enabled(uint8 *);
/*===========================================================================

                            VARIABLE DECLARATIONS

===========================================================================*/

/* The message handler table for all the valid QMI CSVT messages, ordered by
 * message ids. */
static qmi_svc_cmd_hdlr_type qmi_csvti_cmd_callbacks[CSVTI_CMD_MAX] =
{
  QMI_SVC_HDLR(CSVTI_CMD_VAL_RESET,
               qmi_csvti_reset),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_SET_EVENT_REPORT,
               qmi_csvti_set_event_report),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_ORIGINATE_CALL,
               qmi_csvti_originate_call),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_CONFIRM_CALL,
               qmi_csvti_confirm_call),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_ANSWER_CALL,
               qmi_csvti_answer_call),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_END_CALL,
               qmi_csvti_end_call),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_MODIFY_CALL,
               qmi_csvti_modify_call),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_ACK_CALL_MODIFY,
               qmi_csvti_ack_call_modify),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_GET_RLP_PARAMS,
               qmi_csvti_get_rlp_params),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_SET_RLP_PARAMS,
               qmi_csvti_set_rlp_params),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_GET_ACTIVE_CALL_LIST,
               qmi_csvti_get_active_call_list),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_GET_CALL_INFO,
               qmi_csvti_get_call_info),
  QMI_SVC_HDLR(CSVTI_CMD_VAL_GET_CALL_STATS,
               qmi_csvti_get_call_stats),
  QMI_SVC_HDLR( CSVTI_CMD_VAL_SET_SUBSCRIPTION_BINDING,
                qmi_csvti_set_subscription_binding),
  QMI_SVC_HDLR( CSVTI_CMD_VAL_GET_BIND_SUBSCRIPTION,
                qmi_csvti_get_bind_subscription)
};

/* The service configuration structure shared with QMUX. */
static ds_qmi_fw_svc_cfg_type  qmi_csvti_cfg;

/* The service state structure to maintain instance-specific service data. */
static qmi_csvti_state_type qmi_csvti_state;

/* The structure containing global level (across all instances) service state
 * data.*/
static struct
{
  boolean inited;
  void *  controlling_client_sp[CSVTI_MAX_ALLOWED_CLIENTS];
  int csvt_client_id;
  boolean csvt_client_id_inited;
  uint32 csvt_call_inst[MAX_UCSD_CALLS];  //store token inside this corresponding to call instance 0 or 1
                          //This token will be sent to us as instance id.
  uint32 csvt_token;
  boolean pending_orig_ind;
  qmi_csvti_call_info csvt_call_info[MAX_UCSD_CALLS];

  struct
  {
    boolean inited;
    uint8 call_mask;
  } client_pref;

  struct
  {
    qmi_csvti_cmd_val_e_type cmd;
    boolean active;
    void *cmd_buf_ptr;
  } pending;

  /* To check whether QRD feature NV "qmss_enabled" is enabled or not */
  boolean is_qmss_enabled;
  uint32 active_cs_subs;
} qmi_csvti_global;

/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CSVT_INIT()

  DESCRIPTION
  Register the CSVT service with QMI Framework

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_csvt_init
(
  void
)
{

  qmi_framework_err_e_type  reg_result = QMI_FRAMEWORK_ERR_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_0("QMI CSVT service init");

  /*-------------------------------------------------------------------------
    Setting QMI CSVT service command handler with dcc task process
  -------------------------------------------------------------------------*/
  qmi_task_set_svc_cmd_handler(QMUX_SERVICE_CSVT, qmi_csvti_process_cmd);

  /*-------------------------------------------------------------------------
    QMI CSVT service configuration setup
  -------------------------------------------------------------------------*/
  qmi_csvti_cfg.fw_cfg.base_version.major     = CSVTI_BASE_VER_MAJOR;
  qmi_csvti_cfg.fw_cfg.base_version.minor     = CSVTI_BASE_VER_MINOR;

  qmi_csvti_cfg.fw_cfg.addendum_version.major = CSVTI_ADDENDUM_VER_MAJOR;
  qmi_csvti_cfg.fw_cfg.addendum_version.minor = CSVTI_ADDENDUM_VER_MINOR;

  qmi_csvti_cfg.fw_cfg.cbs.alloc_clid         = qmi_csvti_alloc_clid_cb;
  qmi_csvti_cfg.fw_cfg.cbs.dealloc_clid       = qmi_csvti_dealloc_clid_cb;
  qmi_csvti_cfg.fw_cfg.cbs.init_cback         = qmi_csvti_init_cb;
  qmi_csvti_cfg.fw_cfg.cbs.cmd_hdlr           = qmi_csvti_cmd_hdlr_cb;

  qmi_csvti_cfg.cmd_hdlr_array                = qmi_csvti_cmd_callbacks;
  qmi_csvti_cfg.cmd_num_entries               = CSVTI_CMD_MAX;

  /*-------------------------------------------------------------------------
    Calling QMI Framework API to register the service with Framework
  -------------------------------------------------------------------------*/
  reg_result = qmi_framework_reg_service( QMUX_SERVICE_CSVT,
                                          &qmi_csvti_cfg.fw_cfg );

  if (reg_result != QMI_FRAMEWORK_ERR_NONE)
  {
    return;
  }
  qmi_csvti_cfg.registered  = TRUE;

} /* qmi_csvt_init() */

/*===========================================================================
  FUNCTION QMI_CSVTI_GET_CMD_BUF()

  DESCRIPTION
    Allocate and assign a QMI CSVT command buffer from the PS MEM heap
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
static void *qmi_csvti_get_cmd_buf
(
  qmi_cmd_id_e_type cmd_id
)
{
  LOG_MSG_INFO1_1("csvti_get_cmd_buf cmd_id = %d",cmd_id);
  switch (cmd_id)
  {
    case QMI_CMD_PROCESS_CSVT_CALL_CONNECT_EVT:
    case QMI_CMD_PROCESS_CSVT_PROGRESS_INFO_EVT:
    case QMI_CMD_PROCESS_CSVT_CONFIRM_INFO_EVT:
    case QMI_CMD_PROCESS_CSVT_SETUP_INFO_EVT:
    case QMI_CMD_PROCESS_CSVT_END_INFO_EVT:
    case QMI_CMD_PROCESS_CSVT_INCOMING_INFO_EVT:
    case QMI_CMD_PROCESS_CSVT_MODIFY_INFO_EVT:
    case QMI_CMD_CSVT_INIT_CB:
    case QMI_CMD_CSVT_ALLOC_CLID_CB:
    case QMI_CMD_CSVT_DEALLOC_CLID_CB:
    case QMI_CMD_CSVT_CMD_HDLR_CB:
    {
      qmi_csvti_cmd_buf_type *cmd_buf_ptr = NULL;
      PS_SYSTEM_HEAP_MEM_ALLOC(cmd_buf_ptr, sizeof(qmi_csvti_cmd_buf_type),
                               qmi_csvti_cmd_buf_type*);
      return ((void*)cmd_buf_ptr);
    }

    default:
      break;
  }
  return NULL;
} /* qmi_csvti_get_cmd_buf */

/*===========================================================================
  FUNCTION QMI_CSVT_UCSD_CALL_EVENT_CB()

  DESCRIPTION
    Callback function called by ucsd to report events

  PARAMETERS
    event_ptr  : Info about the event generated.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_csvt_ucsd_call_event_cb
( 
  const ds_ucsd_call_event_type *event_ptr,
  void                          *user_info_ptr 
)
{
  qmi_csvti_cmd_buf_type    *cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(event_ptr && user_info_ptr);

  LOG_MSG_INFO2_1( "Recvd csvt ev %d from ucsd", event_ptr->call_event );

  switch(event_ptr->call_event)
  {
    case CM_CALL_EVENT_CONNECT:
      cmd_ptr = qmi_csvti_get_cmd_buf(QMI_CMD_PROCESS_CSVT_CALL_CONNECT_EVT);
      if( cmd_ptr == NULL)
      {
        return;
      }
      cmd_ptr->cmd_id = QMI_CMD_PROCESS_CSVT_CALL_CONNECT_EVT;
      cmd_ptr->data.connect_evt_info.inst_id =
             event_ptr->event_info.connect.inst_id;
      cmd_ptr->data.connect_evt_info.call_type =
             event_ptr->event_info.connect.call_type;
      cmd_ptr->data.connect_evt_info.modify_allowed =
             event_ptr->event_info.connect.modify_allowed;
      if(NULL != event_ptr->event_info.connect.datapath)
      {
        cmd_ptr->data.connect_evt_info.port =
                (event_ptr->event_info.connect.datapath)->info.sio_info.sio_port;
      }
      else
      {
       //TODO: Might need to return from here or leave port tlv and send other tlv's
        LOG_MSG_ERROR_0("UCSD didn't send port info in connect evt!!");
      }
      break;

    case CM_CALL_EVENT_INCOM:
      cmd_ptr = qmi_csvti_get_cmd_buf(QMI_CMD_PROCESS_CSVT_INCOMING_INFO_EVT);
      if( cmd_ptr == NULL)
      {
        return;
      }
      cmd_ptr->cmd_id = QMI_CMD_PROCESS_CSVT_INCOMING_INFO_EVT;
      cmd_ptr->data.incoming_evt_info.inst_id =
             event_ptr->event_info.incoming.inst_id;
      cmd_ptr->data.incoming_evt_info.network_mode =
             event_ptr->event_info.incoming.network_mode;
      cmd_ptr->data.incoming_evt_info.speed =
             event_ptr->event_info.incoming.speed;
      cmd_ptr->data.incoming_evt_info.name =
             event_ptr->event_info.incoming.name;
      cmd_ptr->data.incoming_evt_info.connection_element =
             event_ptr->event_info.incoming.connection_element;
      cmd_ptr->data.incoming_evt_info.waiur =
             event_ptr->event_info.incoming.waiur;
      cmd_ptr->data.incoming_evt_info.caller_number_len =
             event_ptr->event_info.incoming.caller_number_len;
      if(event_ptr->event_info.incoming.caller_number_len >
              QMI_CSVT_INCOMING_NUMBER_MAX_SIZE)
      {
        LOG_MSG_ERROR_0("Incoming no len sent by ucsd is too large");
        qmi_csvti_free_cmd_buf(cmd_ptr);
        return;
      }
      memscpy(cmd_ptr->data.incoming_evt_info.caller_number,
              event_ptr->event_info.incoming.caller_number_len,
              event_ptr->event_info.incoming.caller_number,
              event_ptr->event_info.incoming.caller_number_len);
      cmd_ptr->data.incoming_evt_info.uus_data_len =
             event_ptr->event_info.incoming.uus_data_len;
      if(event_ptr->event_info.incoming.uus_data_len >
          QMI_CSVT_UUS_ID_MAX_SIZE)
      {
        LOG_MSG_ERROR_0("Incoming UUS data len sent by ucsd is too large");
        qmi_csvti_free_cmd_buf(cmd_ptr);
        return;
      }
      memscpy(cmd_ptr->data.incoming_evt_info.uus_data,
              event_ptr->event_info.incoming.uus_data_len,
              event_ptr->event_info.incoming.uus_data,
              event_ptr->event_info.incoming.uus_data_len);
    break;

    case CM_CALL_EVENT_END:
      cmd_ptr = qmi_csvti_get_cmd_buf(QMI_CMD_PROCESS_CSVT_END_INFO_EVT);
      if( cmd_ptr == NULL)
      {
        return;
      }
      cmd_ptr->cmd_id = QMI_CMD_PROCESS_CSVT_END_INFO_EVT;
      cmd_ptr->data.end_evt_info.inst_id =
                event_ptr->event_info.end.inst_id;
      if(NULL != event_ptr->event_info.end.end_param)
      {
        if(qmi_csvti_global.is_qmss_enabled)
        {
        cmd_ptr->data.end_evt_info.end_cause =
          (event_ptr->event_info.end.end_param)->cc_param.cc_cause;
        }
        else
        {
          cmd_ptr->data.end_evt_info.end_cause =
          (event_ptr->event_info.end.end_param)->end_cause;
        }
      }
      else
      {
        cmd_ptr->data.end_evt_info.end_cause = -1;
        LOG_MSG_INFO2_0("No end cause sent by ucsd in end evt");
      }
    break;

    case CM_CALL_EVENT_PROGRESS_INFO_IND:
      cmd_ptr = qmi_csvti_get_cmd_buf(QMI_CMD_PROCESS_CSVT_PROGRESS_INFO_EVT);
      if( cmd_ptr == NULL)
      {
        return;
      }
      cmd_ptr->cmd_id = QMI_CMD_PROCESS_CSVT_PROGRESS_INFO_EVT;
      cmd_ptr->data.progress_evt_info.inst_id =
                event_ptr->event_info.progress.inst_id;
      cmd_ptr->data.progress_evt_info.call_type =
                event_ptr->event_info.progress.call_type;
      if(NULL != event_ptr->event_info.progress.datapath)
      {
        cmd_ptr->data.progress_evt_info.port =
                (event_ptr->event_info.progress.datapath)->info.sio_info.sio_port;
      }
      else
      {
        //TODO: Might need to return from here or leave port tlv and send other tlv's
        LOG_MSG_ERROR_0("UCSD didn't send port info in progress evt!!");
      }
    break;

    case CM_CALL_EVENT_SETUP_IND:
      cmd_ptr = qmi_csvti_get_cmd_buf(QMI_CMD_PROCESS_CSVT_SETUP_INFO_EVT);
      if( cmd_ptr == NULL)
      {
        return;
      }
      cmd_ptr->cmd_id = QMI_CMD_PROCESS_CSVT_SETUP_INFO_EVT;
      cmd_ptr->data.setup_evt_info.inst_id =
             event_ptr->event_info.setup.inst_id;
      cmd_ptr->data.setup_evt_info.call_type =
             event_ptr->event_info.setup.call_type;
      cmd_ptr->data.setup_evt_info.modify_allowed =
             event_ptr->event_info.setup.modify_allowed;
    break;

    case CM_CALL_EVENT_CALL_CONF:
      cmd_ptr = qmi_csvti_get_cmd_buf(QMI_CMD_PROCESS_CSVT_CONFIRM_INFO_EVT);
      if( cmd_ptr == NULL)
      {
        return;
      }
      cmd_ptr->cmd_id = QMI_CMD_PROCESS_CSVT_CONFIRM_INFO_EVT;
      cmd_ptr->data.confirm_evt_info.inst_id =
             event_ptr->event_info.confirm.inst_id;
      cmd_ptr->data.confirm_evt_info.call_type =
             event_ptr->event_info.confirm.call_type;
      cmd_ptr->data.confirm_evt_info.modify_allowed =
             event_ptr->event_info.confirm.modify_allowed;
    break;

    case CM_CALL_EVENT_MODIFY_IND:
      cmd_ptr = qmi_csvti_get_cmd_buf(QMI_CMD_PROCESS_CSVT_MODIFY_INFO_EVT);
      if( cmd_ptr == NULL)
      {
        return;
      }
      cmd_ptr->cmd_id = QMI_CMD_PROCESS_CSVT_MODIFY_INFO_EVT;
      
    break;

    default:
      LOG_MSG_ERROR_1("Unregistered csvt event %d recvd, ignoring",
                      event_ptr->call_event);
      return;
    break;
  }

  LOG_MSG_INFO2_0( "sending cmd to dcc!!" );
  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
}

/*===========================================================================

                            INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CSVTI_RESET_CLIENT()

  DESCRIPTION
    Clears the client state for the given client and adds it to the free pool

  PARAMETERS
    cl_sp  - the client state pointer

  RETURN VALUE
    None

  DEPENDENCIES
    QMI CSVT service must have been initialized before this API is called.
    The client state pointer must point to an active QMI CSVT client.

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_reset_client
(
  void * cl_sp_in
)
{
  qmi_csvti_client_state_type * cl_sp;
  int i;
  qmi_transaction_type *  x_p;
  uint32                      client_id = CSVTI_INVALID_CLIENT;   
  uint32                      subs_id;
  LOG_MSG_INFO2_0( "qmi_csvti_reset_client " );
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  cl_sp = (qmi_csvti_client_state_type *)cl_sp_in;
  if(NULL == cl_sp)
  {
    LOG_MSG_ERROR_0("NULL client sp");
    return;
  }
  client_id = qmi_csvti_get_csvt_client_id_by_cl_sp(cl_sp);

  if(!qmi_csvt_validate_client_id(client_id,__LINE__))
  {
    /* Client not registered for AT Fwding */
    return;
  }

  //check if reset has come for control point
  subs_id = cl_sp->subscription_id;
  if((subs_id >= CSVTI_BIND_SUBS_MAX) || (subs_id < CSVTI_BIND_SUBS_PRIMARY))
  {
    LOG_MSG_ERROR_0("Invalid subscription binding");
    return;
  }

  if(cl_sp == (qmi_csvti_client_state_type *)qmi_csvti_global.controlling_client_sp[subs_id - 1])
  {
    for(i=0;i<MAX_UCSD_CALLS;i++)
    {
      if(qmi_csvti_global.csvt_call_inst[i] != 0)
      {
        //if active call is there, end it
        LOG_MSG_INFO1_1("Ending the call inst %d in reset", i);
        ds_ucsd_api_end_call(qmi_csvti_global.csvt_client_id,i);
      }
    }

    qmi_csvti_global.controlling_client_sp[subs_id - 1] = NULL;
    qmi_csvti_global.pending_orig_ind = FALSE;
    qmi_csvti_global.csvt_token = 0x7fffffff;
    memset(&qmi_csvti_global.pending,0,sizeof(qmi_csvti_global.pending));
    memset(&qmi_csvti_global.client_pref,0,sizeof(qmi_csvti_global.client_pref));
    memset(qmi_csvti_global.csvt_call_info,0,sizeof(qmi_csvti_global.csvt_call_info));
  }

  /*-------------------------------------------------------------------------
    set report call event as FALSE for this client
  -------------------------------------------------------------------------*/
  cl_sp->report_call_evt = FALSE;
  cl_sp->instance = CSVTI_INVALID_INSTANCE;


  /*-------------------------------------------------------------------------
    Free resources for client
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    q_init() is used here as a way to clear the queue and not as a way to 
    initialize the queue.
  -------------------------------------------------------------------------*/ 
  (void) q_init( &cl_sp->common.x_free_q );
  for( i = 0; i < MAX_PENDING_TRANSACTIONS; i++ )
  {
    x_p = &cl_sp->common.transactions[i];
    ds_qmi_fw_free_transaction( &x_p );
  }
} /* qmi_csvti_reset_client() */

/*===========================================================================
  FUNCTION QMI_CSVTI_RESET()

  DESCRIPTION
    Reset the issuing client's csvt state

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
static dsm_item_type* qmi_csvti_reset
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *              response;
  boolean                      retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  LOG_MSG_INFO1_0("CSVT: Reset");

  /*-------------------------------------------------------------------------
    Reset the client's state
  -------------------------------------------------------------------------*/
  qmi_csvti_reset_client(cl_sp);

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
} /* qmi_csvti_reset() */

/*===========================================================================
  FUNCTION QMI_CSVTI_SET_EVENT_REPORT()

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
static dsm_item_type* qmi_csvti_set_event_report
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  uint8    type;
  uint16   len;
  uint16   expected_len;
  void *   value;

  PACKED struct PACKED_POST
  {
    boolean report_call_events;
  } v_in1;

  PACKED struct PACKED_POST
  {
    uint64 call_types_mask;
  } v_in2;

  boolean  got_report_call_events_tlv= FALSE;
  boolean  got_call_types_tlv          = FALSE;

  uint8 call_types;

  qmi_csvti_client_state_type *  csvt_client_sp;
  uint32 subs_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  memset(&v_in1, 0, sizeof(v_in1));
  memset(&v_in2, 0, sizeof(v_in2));

  response = NULL;
  errval = QMI_ERR_NONE;
  csvt_client_sp = (qmi_csvti_client_state_type *)cl_sp;

  if(csvt_client_sp != NULL)
  {
    subs_id = csvt_client_sp->subscription_id;
    if((subs_id >= CSVTI_BIND_SUBS_MAX) || (subs_id < CSVTI_BIND_SUBS_PRIMARY))
    {
      errval = QMI_ERR_INTERNAL;
      goto send_result;
    }
  }
  else
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  LOG_MSG_INFO1_0("CSVT: Set Event Report");

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
      case CSVTI_PRM_TYPE_REPORT_CALL_EVENTS:
        expected_len = sizeof(v_in1);
        value = &v_in1;
        got_report_call_events_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_CALL_TYPES_MASK:
        expected_len = sizeof(v_in2);
        value = &v_in2;
        got_call_types_tlv = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if (QMI_ERR_NONE != errval)
    {
      break;
    }

    if(!qmi_svc_validate_type_length( type,
				      len,
				      expected_len,
                                      FILE_ID_DS_QMI_CSVT,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and qmi_svc_dsm_pullup will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }
  }

  if((NULL != qmi_csvti_global.controlling_client_sp[subs_id - 1]) &&
     (csvt_client_sp != qmi_csvti_global.controlling_client_sp[subs_id - 1]))
  {
    //Stray client trying to do set event report while there is already a active control point.
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  if(!got_report_call_events_tlv)
  {
    //no change required and ignore all other tlv's
    goto send_result;
  }
  else
  {
    if(TRUE == v_in1.report_call_events)
    {
      csvt_client_sp->report_call_evt = TRUE;
    }
    else
    {
      csvt_client_sp->report_call_evt = FALSE;
    }
  }

  if(!got_call_types_tlv)
  {
    call_types = CSVT_CALL_TYPES_MASK;
  }
  else
  {
    if(v_in2.call_types_mask >= 0xFF)
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
    call_types = (uint8)v_in2.call_types_mask;
  }

  //TODO:first deregister if control point is trying to register for the 2nd time.
  ds_ucsd_api_deregister_client(qmi_csvti_global.csvt_client_id);
  qmi_csvti_global.csvt_client_id = ds_ucsd_api_register_client(call_types,
                        qmi_csvt_ucsd_call_event_cb,&qmi_csvti_global);

  if(DSUCSD_INVALID_CLIENT_ID == qmi_csvti_global.csvt_client_id)
  {
    LOG_MSG_ERROR_0("CSVT: csvt registration with ucsd failed");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  else
  {
    LOG_MSG_INFO1_2("CSVT: initing client id %d client inited %d",
                    qmi_csvti_global.csvt_client_id,
                    (int)qmi_csvti_global.csvt_client_id_inited);
    qmi_csvti_global.csvt_client_id_inited = TRUE;
  }

  //save the client state ptr of this controlling point in global variable
#if !(defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  qmi_csvti_global.controlling_client_sp[subs_id - 1] = cl_sp;
#else
  // To make sure the client bound to PRIMARY subscription is allowed by default
  if(qmi_csvti_global.controlling_client_sp[subs_id - 1] == NULL)
  {
    qmi_csvti_global.controlling_client_sp[subs_id - 1] = cl_sp;
  }
#endif

  LOG_MSG_INFO2_2("controlling client sp (0x%x) for csvt,qmss_enabled:%d", 
                 qmi_csvti_global.controlling_client_sp[subs_id - 1], qmi_csvti_global.is_qmss_enabled);

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_csvti_set_event_report() */

/*===========================================================================
  FUNCTION QMI_CSVTI_ORIGINATE_CALL()

  DESCRIPTION
    Originates the call by sending originate command to dsucsd.

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
static dsm_item_type* qmi_csvti_originate_call
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response = NULL;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  uint8    type;
  uint16   len;
  uint16   expected_len;
  void *   value;

  struct
  {
    uint32 inst_id;
  } v_in;

  PACKED struct PACKED_POST
  {
    uint32 call_mode;
  } v_in1;

  uint8 dial_string[MAX_DIAL_STRING_LEN];

  PACKED struct PACKED_POST
  {
    uint8 network_datarate;
  } v_in3;

  PACKED struct PACKED_POST
  {
    uint8 air_iface_datarate;
  } v_in4;

  PACKED struct PACKED_POST
  {
    uint8 sync_call;
  } v_in5;

  PACKED struct PACKED_POST
  {
    uint8 transparent_call;
  } v_in6;

  PACKED struct PACKED_POST
  {
    uint8 cli_enabled;
  } v_in7;

  PACKED struct PACKED_POST
  {
    uint8 cug_enabled;
  } v_in8;

  PACKED struct PACKED_POST
  {
    uint8 cug_index;
  } v_in9;

  PACKED struct PACKED_POST
  {
    uint8 supress_preferred_cug;
  } v_in10;

  PACKED struct PACKED_POST
  {
    uint8 supress_outgoing_access;
  } v_in11;

  PACKED struct PACKED_POST
  {
    uint32 uus_id_type;
    uint8  uus_id_len;
    uint8 uus_id[QMI_CSVT_UUS_ID_MAX_SIZE];
  } v_in12;

  boolean  got_inst_id_tlv              = FALSE;
  boolean  got_call_mode_tlv            = FALSE;
  boolean  got_dial_string_tlv          = FALSE;
  boolean  got_network_datarate_tlv     = FALSE;
  //boolean  got_air_iface_datarate_tlv   = FALSE;
  boolean  got_sync_call_tlv            = FALSE;
  boolean  got_transparent_call_tlv     = FALSE;
  boolean  got_cli_enabled_tlv          = FALSE;
  boolean  got_cug_enabled_tlv          = FALSE;
  boolean  got_cug_index_tlv            = FALSE;
  boolean  got_supress_preferred_cug_tlv    = FALSE;
  boolean  got_supress_outgoing_access_tlv  = FALSE;
  boolean  got_uus_id_type_tlv              = FALSE;

  uint8 dial_string_len = 0;
  uint8 uus_id_len = 0;

  ds_ucsd_dial_modifier_info_type dial_modifier;
  ds_ucsd_uus_info_type uus_info;
  uint8 inst_id;
  uint32 subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_0("CSVT: originate call");

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  memset(&v_in, 0, sizeof(v_in));
  memset(&v_in1, 0, sizeof(v_in1));

  memset(&v_in3, 0, sizeof(v_in3));
  memset(&v_in4, 0, sizeof(v_in4));
  memset(&v_in5, 0, sizeof(v_in5));
  memset(&v_in6, 0, sizeof(v_in6));
  memset(&v_in7, 0, sizeof(v_in7));
  memset(&v_in8, 0, sizeof(v_in8));
  memset(&v_in9, 0, sizeof(v_in9));
  memset(&v_in10, 0, sizeof(v_in10));
  memset(&v_in11, 0, sizeof(v_in11));
  memset(&v_in12, 0, sizeof(v_in12));

  memset(dial_string,0,MAX_DIAL_STRING_LEN);
  memset(&dial_modifier,0,sizeof(ds_ucsd_dial_modifier_info_type));
  memset(&uus_info,0,sizeof(ds_ucsd_uus_info_type));

  response = NULL;
  errval = QMI_ERR_NONE;
  subs_id = ((qmi_csvti_client_state_ptr_type)cl_sp)->subscription_id;

  if((subs_id >= CSVTI_BIND_SUBS_MAX) || (subs_id < CSVTI_BIND_SUBS_PRIMARY))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  
  /* check if this is the controlling control point. */
  if (cl_sp != qmi_csvti_global.controlling_client_sp[subs_id - 1])
  {
    LOG_MSG_ERROR_0("Ignoring Originate call request from stray control point");
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }
  
  qmi_csvti_global.active_cs_subs = subs_id;

  if(TRUE == qmi_csvti_global.pending.active)
  {
    LOG_MSG_ERROR_0("Ignoring originate call as there is a pending request");
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

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
      case CSVTI_PRM_TYPE_INST_ID:
        expected_len = sizeof(v_in);
        value = &v_in;
        got_inst_id_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_CALL_MODE:
        expected_len = sizeof(v_in1);
        value = &v_in1;
        got_call_mode_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_DIAL_STRING:
        expected_len = dial_string_len = (uint8)(0xFF & len);
        if(dial_string_len > MAX_DIAL_STRING_LEN)
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        value = dial_string;
        got_dial_string_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_NETWORK_DATARATE:
        expected_len = sizeof(v_in3);
        value = &v_in3;
        got_network_datarate_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_AIR_IFACE_DATA_RATE:
        expected_len = sizeof(v_in4);
        value = &v_in4;
        //got_air_iface_datarate_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_SYNC_CALL:
        expected_len = sizeof(v_in5);
        value = &v_in5;
        got_sync_call_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_TRANSPARENT_CALL:
        expected_len = sizeof(v_in6);
        value = &v_in6;
        got_transparent_call_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_CLI_ENABLED:
        expected_len = sizeof(v_in7);
        value = &v_in7;
        got_cli_enabled_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_CUG_ENABLED:
        expected_len = sizeof(v_in8);
        value = &v_in8;
        got_cug_enabled_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_CUG_INDEX:
        expected_len = sizeof(v_in9);
        value = &v_in9;
        got_cug_index_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_SUPRESS_PREFERRED_CUG:
        expected_len = sizeof(v_in10);
        value = &v_in10;
        got_supress_preferred_cug_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_SUPRESS_OUTGOING_ACCESS:
        expected_len = sizeof(v_in11);
        value = &v_in11;
        got_supress_outgoing_access_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_UUS_ID_TYPE:
        expected_len = (uint8)(0xFF & len);
        uus_id_len = (uint8)len -5;
        if(uus_id_len > QMI_CSVT_UUS_ID_MAX_SIZE)
        {
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        value = &v_in12;
        got_uus_id_type_tlv = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if (QMI_ERR_NONE != errval)
    {
      break;
    }

    if(!qmi_svc_validate_type_length( type,
				      len,
				      expected_len,
				      FILE_ID_DS_QMI_CSVT,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and qmi_svc_dsm_pullup will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }
  }

  if((!got_inst_id_tlv) || (!got_call_mode_tlv) || (!got_dial_string_tlv))
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if(!got_network_datarate_tlv)
  {
    v_in3.network_datarate = QMI_CSVT_DEFAULT_NETWORK_DATA_RATE;
  }

  if(!got_sync_call_tlv)
  {
    v_in5.sync_call = TRUE;
  }

  if(!got_transparent_call_tlv)
  {
    v_in6.transparent_call = FALSE;
  }

  if(got_cli_enabled_tlv)
  {
    dial_modifier.modifier_present = TRUE;
    dial_modifier.modifier_info.cli.cli_enabled = (FALSE != v_in7.cli_enabled);
  }

  if(got_cug_enabled_tlv)
  {
    dial_modifier.modifier_present = TRUE;
    dial_modifier.modifier_info.cug.cug_enabled = (FALSE != v_in8.cug_enabled);
    if(got_cug_index_tlv)
    {
      dial_modifier.modifier_info.cug.cug_index_enabled = TRUE;
      dial_modifier.modifier_info.cug.cug_index_val = v_in9.cug_index;
    }
    if(got_supress_preferred_cug_tlv)
    {
      dial_modifier.modifier_info.cug.suppress_pref_cug = v_in10.supress_preferred_cug;
    }
    if(got_supress_outgoing_access_tlv)
    {
      dial_modifier.modifier_info.cug.suppress_outgoing_access = v_in11.supress_outgoing_access;
    }
  }

  if(got_cli_enabled_tlv && got_cug_enabled_tlv)
  {
    dial_modifier.modifier = DS_UCSD_CLI_AND_CUG_MODIFIER;
  }
  else if(got_cli_enabled_tlv)
  {
    dial_modifier.modifier = DS_UCSD_CLI_DIAL_MODIFIER;
  }
  else if(got_cug_enabled_tlv)
  {
    dial_modifier.modifier = DS_UCSD_CUG_DIAL_MODIFIER;
  }

  if(got_uus_id_type_tlv)
  {
    uus_info.present = TRUE;
    uus_info.uus_id = v_in12.uus_id_type;
    uus_info.data_len = v_in12.uus_id_len;
    uus_info.uus_data = v_in12.uus_id;
  }

  inst_id = ds_ucsd_api_originate_call(qmi_csvti_global.csvt_client_id,
                                      (uint8)v_in1.call_mode,
                                       v_in3.network_datarate,
                                       v_in5.sync_call,
                                       v_in6.transparent_call,
                                       v_in4.air_iface_datarate,
                                       dial_string,
                                       dial_string_len,
                                       &dial_modifier,
                                       &uus_info);

  if(DSUCSD_INVALID_INST_ID == inst_id)
  {
    LOG_MSG_INFO2_0("Error while originating call from ucsd");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if( inst_id < MAX_UCSD_CALLS )
  {
    qmi_csvti_global.csvt_call_inst[inst_id] = v_in.inst_id;
    qmi_csvti_global.pending_orig_ind = TRUE;
  
    //set call info for this in-progress call
    memset(&qmi_csvti_global.csvt_call_info[inst_id],0,
           sizeof(qmi_csvti_call_info));
    qmi_csvti_global.csvt_call_info[inst_id].call_state
      = CALL_IN_PROGRESS;
    qmi_csvti_global.csvt_call_info[inst_id].is_synchronous
      = v_in5.sync_call;
    qmi_csvti_global.csvt_call_info[inst_id].is_transparent
      = v_in6.transparent_call;
    qmi_csvti_global.csvt_call_info[inst_id].network_speed
      = v_in3.network_datarate;
    memscpy(qmi_csvti_global.csvt_call_info[inst_id].dial_string,
            dial_string_len,
            dial_string,
            dial_string_len);
    qmi_csvti_global.csvt_call_info[inst_id].dial_string_len =
      dial_string_len;
  }

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_csvti_originate_call() */

/*===========================================================================
  FUNCTION QMI_CSVTI_CONFIRM_CALL()

  DESCRIPTION
    Confirms the call by sending confirm to dsucsd in response to the recieved setup event.

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
static dsm_item_type* qmi_csvti_confirm_call
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  uint8    type;
  uint16   len;
  uint16   expected_len;
  void *   value;

  struct
  {
    uint32 inst_id;
  } v_in1;

  struct
  {
    boolean call_confirm;
  } v_in2;

  boolean  got_inst_id_tlv          = FALSE;
  boolean  got_call_confirm_tlv      = FALSE;

  uint8 i;
  uint8 call_id;
  uint8 call_type;
  uint32 subs_id;
  ds_ucsd_answer_param_type answer_param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  LOG_MSG_INFO1_0("CSVT: Confirm call");
  subs_id = ((qmi_csvti_client_state_ptr_type)cl_sp)->subscription_id;

  if((subs_id >= CSVTI_BIND_SUBS_MAX) || (subs_id < CSVTI_BIND_SUBS_PRIMARY))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /* check if this is the controlling control point. */
  if (cl_sp != qmi_csvti_global.controlling_client_sp[subs_id - 1] ||
      (subs_id != qmi_csvti_global.active_cs_subs))
  {
    LOG_MSG_ERROR_2("Ignoring Confirm call request from stray control point.subs= %d, active_cs_subs=%d",
                  subs_id,qmi_csvti_global.active_cs_subs);
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  if(TRUE == qmi_csvti_global.pending.active)
  {
    LOG_MSG_ERROR_0("Ignoring confirm call as there is a pending request");
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  memset(&v_in1, 0, sizeof(v_in1));
  memset(&v_in2, 0, sizeof(v_in2));
  memset(&answer_param,0,sizeof(ds_ucsd_answer_param_type));

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
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(v_in1);
        value = &v_in1;
        got_inst_id_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_CALL_ACCEPT:
        expected_len = sizeof(v_in2);
        value = &v_in2;
        got_call_confirm_tlv = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if (QMI_ERR_NONE != errval)
    {
      break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_CSVT,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and qmi_svc_dsm_pullup will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }
  }

  if(!got_inst_id_tlv || !got_call_confirm_tlv)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  //map client's instance id to our instance id
  for(i=0;i<MAX_UCSD_CALLS;i++)
  {
    if(qmi_csvti_global.csvt_call_inst[i] == v_in1.inst_id)
      break;
  }

  if(!qmi_csvt_validate_call_instance(v_in1.inst_id,i,__LINE__))
  {
    errval = QMI_ERR_INVALID_ID;
    goto send_result;
  }

  //fetch call type which will be passed to confirm api
  call_id = ds_ucsd_api_lookup_cm_callid(i);

  if(FALSE == ds_ucsd_api_get_calltype(call_id,&call_type))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  answer_param.reject = !(v_in2.call_confirm);
  if(FALSE == ds_ucsd_api_confirm_call(qmi_csvti_global.csvt_client_id,
                     i,call_type,&answer_param))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /*
     If user is rejecting the incoming call we can send the success response
     right away.
  */
  if(TRUE == answer_param.reject)
  {
    LOG_MSG_INFO2_0("User rejected the incoming call");
    goto send_result;
  }

  qmi_csvti_global.pending.active = TRUE;
  qmi_csvti_global.pending.cmd = CSVTI_CMD_VAL_CONFIRM_CALL;
  qmi_csvti_global.pending.cmd_buf_ptr = cmd_buf_p;
  return QMI_SVC_RESPONSE_PENDING;

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_csvti_confirm_call() */

/*===========================================================================
  FUNCTION QMI_CSVTI_ANSWER_CALL()

  DESCRIPTION
    Sends answer to dsucsd in response to the recieved incoming event.

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
static dsm_item_type* qmi_csvti_answer_call
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  uint8    type;
  uint16   len;
  uint16   expected_len;
  void *   value;

  struct
  {
    uint32 inst_id;
  } v_in1;

  struct
  {
    boolean call_answer;
  } v_in2;

  struct
  {
    uint8 reject_value;
  }v_in3;

  boolean  got_reject_value_tlv     = FALSE;
  boolean  got_inst_id_tlv          = FALSE;
  boolean  got_call_accept_tlv      = FALSE;

  uint8 i;
  uint32 subs_id;
  ds_ucsd_answer_param_type ans_param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  LOG_MSG_INFO1_0("CSVT: Answer call");

  subs_id = ((qmi_csvti_client_state_ptr_type)cl_sp)->subscription_id;
  if((subs_id >= CSVTI_BIND_SUBS_MAX) || (subs_id < CSVTI_BIND_SUBS_PRIMARY))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /* check if this is the controlling control point. */
  if (cl_sp != qmi_csvti_global.controlling_client_sp[subs_id - 1]||
      (subs_id != qmi_csvti_global.active_cs_subs))
  {
    LOG_MSG_ERROR_2("Ignoring Confirm call request from stray control point.subs= %d, active_cs_subs=%d",
                  subs_id,qmi_csvti_global.active_cs_subs);
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  if(TRUE == qmi_csvti_global.pending.active)
  {
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  memset(&v_in1, 0, sizeof(v_in1));
  memset(&v_in2, 0, sizeof(v_in2));
  memset(&v_in3, 0, sizeof(v_in3));
  memset(&ans_param,0,sizeof(ans_param));
  
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
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(v_in1);
        value = &v_in1;
        got_inst_id_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_CALL_ACCEPT:
        expected_len = sizeof(v_in2);
        value = &v_in2;
        got_call_accept_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_REJECT_VALUE:
        expected_len = sizeof(v_in3);
        value = &v_in3;
        got_reject_value_tlv = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if (QMI_ERR_NONE != errval)
    {
      break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_CSVT,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and qmi_svc_dsm_pullup will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }
  }

  if(!got_inst_id_tlv || !got_call_accept_tlv)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  //map client's instance id to our instance id
  for(i=0;i<MAX_UCSD_CALLS;i++)
  {
    if(qmi_csvti_global.csvt_call_inst[i] == v_in1.inst_id)
      break;
  }

  if(!qmi_csvt_validate_call_instance(v_in1.inst_id,i,__LINE__))
  {
    errval = QMI_ERR_INVALID_ID;
    goto send_result;
  }

  ans_param.reject = !(v_in2.call_answer);

  if(qmi_csvti_global.is_qmss_enabled)
  {
    if (TRUE == ans_param.reject)
    {
      if(got_reject_value_tlv)
      {  
        MSG_HIGH("ANSWER CSVT,reject value: %d",v_in3.reject_value,0,0);
        ans_param.cause_info.cc_cause_present = TRUE;
        ans_param.cause_info.cc_cause = v_in3.reject_value;
      }
    }
  }

  if(FALSE == ds_ucsd_api_answer_call(qmi_csvti_global.csvt_client_id,
                     i,&ans_param))
  {
    LOG_MSG_INFO2_0("Error while answering call from ucsd");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /*
     If user is rejecting the incoming call we can send the success response
     right away.
  */
  if(TRUE == ans_param.reject)
    goto send_result;

  qmi_csvti_global.pending.active = TRUE;
  qmi_csvti_global.pending.cmd = CSVTI_CMD_VAL_ANSWER_CALL;
  qmi_csvti_global.pending.cmd_buf_ptr = cmd_buf_p;
  return QMI_SVC_RESPONSE_PENDING;

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_csvti_answer_call() */

/*===========================================================================
  FUNCTION QMI_CSVTI_END_CALL()

  DESCRIPTION
    Ends the call.

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
static dsm_item_type* qmi_csvti_end_call
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  uint8    type;
  uint16   len;
  uint16   expected_len;
  void *   value;

  uint8    i;
  uint32    inst_id = 0;
  uint32    subs_id;
  boolean  got_inst_id_tlv          = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  LOG_MSG_INFO1_0("CSVT: End call");
 
  subs_id = ((qmi_csvti_client_state_ptr_type)cl_sp)->subscription_id;
  if((subs_id >= CSVTI_BIND_SUBS_MAX) || (subs_id < CSVTI_BIND_SUBS_PRIMARY))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /* check if this is the controlling control point. */
  if (cl_sp != qmi_csvti_global.controlling_client_sp[subs_id - 1]||
      (subs_id != qmi_csvti_global.active_cs_subs))
  {
    LOG_MSG_ERROR_2("Ignoring Confirm call request from stray control point.subs= %d, active_cs_subs=%d",
                  subs_id,qmi_csvti_global.active_cs_subs);
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

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
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(uint32);
        value = &inst_id;
        got_inst_id_tlv = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if (QMI_ERR_NONE != errval)
    {
      break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_CSVT,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and qmi_svc_dsm_pullup will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }
  }
  if(!got_inst_id_tlv)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  //map client's instance id to our instance id
  for(i=0;i<MAX_UCSD_CALLS;i++)
  {
    if(qmi_csvti_global.csvt_call_inst[i] == inst_id)
      break;
  }

  if(!qmi_csvt_validate_call_instance(inst_id,i,__LINE__))
  {
    errval = QMI_ERR_INVALID_ID;
    goto send_result;
  }

  if(FALSE == ds_ucsd_api_end_call(qmi_csvti_global.csvt_client_id,i))
  {
    LOG_MSG_INFO2_0("Error while ending call from ucsd");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  //if we recieve end from client while there is already a pending confirm or
  //answer , we should first send the response for those and then send for end.
  if((qmi_csvti_global.pending.cmd == CSVTI_CMD_VAL_CONFIRM_CALL) || 
     (qmi_csvti_global.pending.cmd == CSVTI_CMD_VAL_ANSWER_CALL))
  {
    if(FALSE == qmi_csvti_send_pending_response(CSVTI_CMD_VAL_END_CALL))
    {
      LOG_MSG_INFO2_1 ("Unable to send response for pending CSVT operation %d",
                       qmi_csvti_global.pending.cmd);
    }
  }

  qmi_csvti_global.pending.active = TRUE;
  qmi_csvti_global.pending.cmd = CSVTI_CMD_VAL_END_CALL;
  qmi_csvti_global.pending.cmd_buf_ptr = cmd_buf_p;
  return QMI_SVC_RESPONSE_PENDING;

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_csvti_end_call() */

/*===========================================================================
  FUNCTION QMI_CSVTI_MODIFY_CALL()

  DESCRIPTION
    Modifies the call.

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
static dsm_item_type* qmi_csvti_modify_call
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  LOG_MSG_INFO1_0("CSVT: Modify call");

#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#else
  LOG_MSG_ERROR_0("SCUDIF support not available");
  errval = QMI_ERR_INVALID_OPERATION;
  goto send_result;
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_csvti_modify_call() */

/*===========================================================================
  FUNCTION QMI_CSVTI_ACK_CALL_MODIFY()

  DESCRIPTION
    Accepts/Rejects the modification initiated by the network.

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
static dsm_item_type* qmi_csvti_ack_call_modify
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  LOG_MSG_INFO1_0("CSVT: ACK CALL Modify");

#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#else
  LOG_MSG_ERROR_0("SCUDIF support not available");
  errval = QMI_ERR_INVALID_OPERATION;
  goto send_result;
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_csvti_modify_call() */

/*===========================================================================
  FUNCTION QMI_CSVTI_GET_RLP_PARAMS()

  DESCRIPTION
    Gets the Radio Link Parameters from dsucsd and sends it to client..

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
static dsm_item_type* qmi_csvti_get_rlp_params
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  ds_ucsd_rlp_sets_type rlp_params;

  PACKED struct PACKED_POST
  {
    uint32 rlp_version;
    uint32 rlp_tx_window_size;
    uint32 rlp_rx_window_size;
    uint32 rlp_ack_timer;
    uint32 rlp_retrans_attempts;
    uint32 rlp_reseq_timer;
  } v_out1;

  PACKED struct PACKED_POST
  {
    uint32 v42_direction;
    uint32 v42_negotiation;
    uint32 v42_max_dict;
    uint32 v42_max_string;
  } v_out2;

  PACKED struct PACKED_POST
  {
    uint32 rlp_version;
    uint32 rlp_tx_window_size;
    uint32 rlp_rx_window_size;
    uint32 rlp_ack_timer;
    uint32 rlp_retrans_attempts;
    uint32 rlp_reseq_timer;
  } v_out3;

  PACKED struct PACKED_POST
  {
    uint32 rlp_version;
    uint32 rlp_tx_window_size;
    uint32 rlp_rx_window_size;
    uint32 rlp_ack_timer;
    uint32 rlp_retrans_attempts;
    uint32 rlp_reseq_timer;
  } v_out4;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  LOG_MSG_INFO1_0("CSVT: get RLP params");

  memset(&rlp_params,0,sizeof(rlp_params));
  memset(&v_out1, 0, sizeof(v_out1));
  memset(&v_out2, 0, sizeof(v_out2));
  memset(&v_out3, 0, sizeof(v_out3));
  memset(&v_out4, 0, sizeof(v_out4));

  if(FALSE == ds_ucsd_api_get_rlp(qmi_csvti_global.csvt_client_id,&rlp_params))
  {
    LOG_MSG_INFO2_0("Error while getting rlp params from ucsd");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if(rlp_params.rlp_sets[2].present == TRUE)
  {
    v_out4.rlp_version = (uint32)rlp_params.rlp_sets[2].version;
    v_out4.rlp_tx_window_size = (uint32)rlp_params.rlp_sets[2].iws;
    v_out4.rlp_rx_window_size = (uint32)rlp_params.rlp_sets[2].mws;
    v_out4.rlp_ack_timer = (uint32)rlp_params.rlp_sets[2].T1;
    v_out4.rlp_retrans_attempts = (uint32)rlp_params.rlp_sets[2].N2;
    v_out4.rlp_reseq_timer = (uint32)rlp_params.rlp_sets[2].T4;

    if (FALSE == qmi_svc_put_param_tlv(&response,
                                       CSVTI_PRM_TYPE_RLP3_PARAMS,
                                       sizeof(v_out4),
                                       &v_out4))
    {
      dsm_free_packet(&response);
      errval = QMI_ERR_NO_MEMORY;
    }
  }

  if(rlp_params.rlp_sets[1].present == TRUE)
  {
    v_out3.rlp_version = (uint32)rlp_params.rlp_sets[1].version;
    v_out3.rlp_tx_window_size = (uint32)rlp_params.rlp_sets[1].iws;
    v_out3.rlp_rx_window_size = (uint32)rlp_params.rlp_sets[1].mws;
    v_out3.rlp_ack_timer = (uint32)rlp_params.rlp_sets[1].T1;
    v_out3.rlp_retrans_attempts = (uint32)rlp_params.rlp_sets[1].N2;
    v_out3.rlp_reseq_timer = (uint32)rlp_params.rlp_sets[1].T4;

    if (FALSE == qmi_svc_put_param_tlv(&response,
                                       CSVTI_PRM_TYPE_RLP2_PARAMS,
                                       sizeof(v_out3),
                                       &v_out3))
    {
      dsm_free_packet(&response);
      errval = QMI_ERR_NO_MEMORY;
    }
  }

  if(rlp_params.v42_info.present == TRUE)
  {
    v_out2.v42_direction = (uint32)rlp_params.v42_info.direction;
    v_out2.v42_negotiation = (uint32)rlp_params.v42_info.negotiation;
    v_out2.v42_max_dict = (uint32)rlp_params.v42_info.max_dict;
    v_out2.v42_max_string = (uint32)rlp_params.v42_info.max_string;

    if (FALSE == qmi_svc_put_param_tlv(&response,
                                       CSVTI_PRM_TYPE_V42_PARAMS,
                                       sizeof(v_out2),
                                       &v_out2))
    {
      dsm_free_packet(&response);
      errval = QMI_ERR_NO_MEMORY;
    }
  }

  if(rlp_params.rlp_sets[0].present == TRUE)
  {
    v_out1.rlp_version = (uint32)rlp_params.rlp_sets[0].version;
    v_out1.rlp_tx_window_size = (uint32)rlp_params.rlp_sets[0].iws;
    v_out1.rlp_rx_window_size = (uint32)rlp_params.rlp_sets[0].mws;
    v_out1.rlp_ack_timer = (uint32)rlp_params.rlp_sets[0].T1;
    v_out1.rlp_retrans_attempts = (uint32)rlp_params.rlp_sets[0].N2;
    v_out1.rlp_reseq_timer = (uint32)rlp_params.rlp_sets[0].T4;

    if (FALSE == qmi_svc_put_param_tlv(&response,
                                       CSVTI_PRM_TYPE_RLP_PARAMS,
                                       sizeof(v_out1),
                                       &v_out1))
    {
      dsm_free_packet(&response);
      errval = QMI_ERR_NO_MEMORY;
    }
  }

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_csvti_get_rlp_params() */

/*===========================================================================
  FUNCTION QMI_CSVTI_SET_RLP_PARAMS()

  DESCRIPTION
    Sets the Radio Link Parameters.

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
static dsm_item_type* qmi_csvti_set_rlp_params
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  uint8    type;
  uint16   len;
  uint16   expected_len;
  void *   value;

  PACKED struct PACKED_POST
  {
    uint32 rlp_version;
    uint32 rlp_tx_window_size;
    uint32 rlp_rx_window_size;
    uint32 rlp_ack_timer;
    uint32 rlp_retrans_attempts;
    uint32 rlp_reseq_timer;
  } v_in1;

  PACKED struct PACKED_POST
  {
    uint32 v42_direction;
    uint32 v42_negotiation;
    uint32 v42_max_dict;
    uint32 v42_max_string;
  } v_in2;

  boolean got_rlp_tlv                 = FALSE;
  boolean got_v42_tlv                 = FALSE;

  ds_ucsd_nt_info_type  rlp_params;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  memset(&v_in1, 0, sizeof(v_in1));
  memset(&v_in2, 0, sizeof(v_in2)); 

  memset(&rlp_params, 0, sizeof(ds_ucsd_nt_info_type));

  response = NULL;
  errval = QMI_ERR_NONE;

  LOG_MSG_INFO1_0("CSVT: set rlp param");

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
      case CSVTI_PRM_TYPE_RLP_PARAMS:
        expected_len = sizeof(v_in1);
        value = &v_in1;
        got_rlp_tlv = TRUE;
        break;

      case CSVTI_PRM_TYPE_V42_PARAMS:
        expected_len = sizeof(v_in2);
        value = &v_in2;
        got_v42_tlv = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if (QMI_ERR_NONE != errval)
    {
      break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_CSVT,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and qmi_svc_dsm_pullup will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }
  }

  if(got_rlp_tlv)
  {
    rlp_params.rlp_info.present = TRUE;
    rlp_params.rlp_info.version = (uint8)v_in1.rlp_version;
    rlp_params.rlp_info.iws = (uint16)v_in1.rlp_tx_window_size;
    rlp_params.rlp_info.mws = (uint16)v_in1.rlp_rx_window_size;
    rlp_params.rlp_info.T1 = (uint16)v_in1.rlp_ack_timer;
    rlp_params.rlp_info.N2 = (uint16)v_in1.rlp_retrans_attempts;
    rlp_params.rlp_info.T4 = (uint16)v_in1.rlp_reseq_timer;
  }
  
  if(got_v42_tlv)
  {
    rlp_params.v42_info.present = TRUE;
    rlp_params.v42_info.direction = (uint8)v_in2.v42_direction;
    rlp_params.v42_info.negotiation = (uint8)v_in2.v42_negotiation;
    rlp_params.v42_info.max_dict = (uint16)v_in2.v42_max_dict;
    rlp_params.v42_info.max_string = (uint8)v_in2.v42_max_string;
  }

  if(FALSE == ds_ucsd_api_set_rlp(qmi_csvti_global.csvt_client_id,&rlp_params))
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
} /* qmi_csvti_set_rlp_params() */

/*===========================================================================
  FUNCTION QMI_CSVTI_GET_ACTIVE_CALL_LIST()

  DESCRIPTION
    Sends the active call list to the client

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
static dsm_item_type* qmi_csvti_get_active_call_list
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;
  uint16             num_bytes_pushed = 0;
  uint8              active_instances = 0;
  uint8              type = 0;
  uint8              i = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  LOG_MSG_INFO1_0("CSVT: get active call list");

  for(i = 0; i < MAX_UCSD_CALLS; i++)
  {
    if(qmi_csvti_global.csvt_call_inst[i] != 0)
    {
      active_instances++;
      if(!((QMI_SVC_PKT_PUSH(&response,
                             &qmi_csvti_global.csvt_call_inst[i],
                             sizeof(qmi_csvti_global.csvt_call_inst[i])))
           ))
      {
        HANDLE_FAILURE();
      }
      num_bytes_pushed += sizeof(qmi_csvti_global.csvt_call_inst[i]);
    }
  }

  if(0 != active_instances)
  {
    num_bytes_pushed += sizeof(active_instances);
    type = 0x10;
    if(!((QMI_SVC_PKT_PUSH(&response,
                             &active_instances,
                             sizeof(active_instances))) &&
           (QMI_SVC_PKT_PUSH(&response,
                             &num_bytes_pushed,
                             sizeof(uint16))) &&
           (QMI_SVC_PKT_PUSH(&response,
                             &type,
                             sizeof(uint8)))
           ))
      {
        HANDLE_FAILURE();
      }
  }

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
}

/*===========================================================================
  FUNCTION QMI_CSVTI_GET_CALL_INFO()

  DESCRIPTION
    Sends the call info of the current active call or last active call.

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
static dsm_item_type* qmi_csvti_get_call_info
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  uint8    type;
  uint16   len;
  uint16   expected_len;
  void *   value;

  PACKED struct PACKED_POST
  {
    uint32 inst_id;
  } v_in1;

  boolean got_inst_id_tlv = FALSE;
  qmi_csvti_call_state call_status;
  uint32 inst_id;
  uint8  i=0;

  struct
  {
    uint32 call_end_cause;
  } v_out1;

  struct
  {
    boolean is_sync;
  } v_out2;

  struct
  {
    uint32 is_transparent;
  } v_out3;

  struct
  {
    uint32 network_type;
  } v_out4;

  struct
  {
    uint16 network_speed;
  } v_out5;

  struct
  {
    uint32 port_family;
    uint32 port_number;
  } v_out6;

  uint8 dial_no_len = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;
  memset(&v_in1, 0, sizeof(v_in1));
  memset(&v_out1, 0, sizeof(v_out1));
  memset(&v_out2, 0, sizeof(v_out2));
  memset(&v_out3, 0, sizeof(v_out3));
  memset(&v_out4, 0, sizeof(v_out4));
  memset(&v_out5, 0, sizeof(v_out5));
  memset(&v_out6, 0, sizeof(v_out6));

  LOG_MSG_INFO1_0("CSVT: get call info");

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
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(v_in1);
        value = &v_in1;
        got_inst_id_tlv = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if (QMI_ERR_NONE != errval)
    {
      break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_CSVT,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and qmi_svc_dsm_pullup will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }
  }

  if(!got_inst_id_tlv)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  //map client's instance id to our instance id
  for(i=0;i<MAX_UCSD_CALLS;i++)
  {
    if(qmi_csvti_global.csvt_call_inst[i] == v_in1.inst_id)
      break;
  }

  if(!qmi_csvt_validate_call_instance(v_in1.inst_id,i,__LINE__))
  {
    errval = QMI_ERR_INVALID_ID;
    goto send_result;
  }

  inst_id = i;
  call_status = qmi_csvti_global.csvt_call_info[inst_id].call_state;

  switch(call_status)
  {
    case CALL_NOT_ACTIVE:
      errval = QMI_ERR_INVALID_ID;
      goto send_result;
    break;

    case CALL_ENDED:
      v_out1.call_end_cause = 
       qmi_csvti_global.csvt_call_info[inst_id].call_end_cause;
      if (FALSE == qmi_svc_put_param_tlv(&response,
                                         0x19,
                                         sizeof(v_out1),
                                         &v_out1))
      {
        dsm_free_packet(&response);
        errval = QMI_ERR_NO_MEMORY;
      }
    break;

    case CALL_IN_PROGRESS:
    case CALL_ACTIVE:
      v_out2.is_sync = 
        qmi_csvti_global.csvt_call_info[inst_id].is_synchronous;
      if (FALSE == qmi_svc_put_param_tlv(&response,
                                         0x11,
                                         sizeof(v_out2),
                                         &v_out2))
      {
        dsm_free_packet(&response);
        errval = QMI_ERR_NO_MEMORY;
      }

      v_out3.is_transparent =
        qmi_csvti_global.csvt_call_info[inst_id].is_transparent;
      if (FALSE == qmi_svc_put_param_tlv(&response,
                                         0x12,
                                         sizeof(v_out3),
                                         &v_out3))
      {
        dsm_free_packet(&response);
        errval = QMI_ERR_NO_MEMORY;
      }

      if(0 != qmi_csvti_global.csvt_call_info[inst_id].network_type)
      {
        v_out4.network_type =
          qmi_csvti_global.csvt_call_info[inst_id].network_type;
        if (FALSE == qmi_svc_put_param_tlv(&response,
                                           0x13,
                                           sizeof(v_out4),
                                           &v_out4))
        {
          dsm_free_packet(&response);
          errval = QMI_ERR_NO_MEMORY;
        }
      }

      if(0 != qmi_csvti_global.csvt_call_info[inst_id].network_speed)
      {
        v_out5.network_speed =
          qmi_csvti_global.csvt_call_info[inst_id].network_speed;
        if (FALSE == qmi_svc_put_param_tlv(&response,
                                           0x14,
                                           sizeof(v_out5),
                                           &v_out5))
        {
          dsm_free_packet(&response);
          errval = QMI_ERR_NO_MEMORY;
        }
      }

      dial_no_len = qmi_csvti_global.csvt_call_info[inst_id].dial_string_len;
      if(dial_no_len < MAX_DIAL_STRING_LEN)
      {
        if (FALSE == qmi_svc_put_param_tlv(&response,
                                           0x16,
                                           dial_no_len,
                qmi_csvti_global.csvt_call_info[inst_id].dial_string))
        {
           dsm_free_packet(&response);
           errval = QMI_ERR_NO_MEMORY;
        }
      }

      v_out6.port_family = qmi_csvti_global.csvt_call_info[inst_id].port.port_family;
      v_out6.port_number = qmi_csvti_global.csvt_call_info[inst_id].port.port_number;
      if((0 != v_out6.port_family) && (0 != v_out6.port_number))
      {
        if (FALSE == qmi_svc_put_param_tlv(&response,
                                           0x1A,
                                           sizeof(v_out6),
                                           &v_out6))
        {
           dsm_free_packet(&response);
           errval = QMI_ERR_NO_MEMORY;
        }
      }

    break;

    default:
    break;
  }

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_csvti_get_call_info() */

/*===========================================================================
  FUNCTION QMI_CSVTI_GET_CALL_STATS()

  DESCRIPTION
    Gets the call stats from dsucsd of the recieved call instance and sends it to client..

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
static dsm_item_type* qmi_csvti_get_call_stats
(
  void * sp,
  void * cmd_buf_p,
  void * cl_sp,
  dsm_item_type ** sdu_in 
)
{
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  uint8    type;
  uint16   len;
  uint16   expected_len;
  void *   value;

  uint8 i;
  uint32    inst_id = 0;
  boolean  got_inst_id_tlv          = FALSE;
  ds_ucsd_call_stats_type  call_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

  LOG_MSG_INFO1_0("CSVT: get call stats");

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
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(uint32);
        value = &inst_id;
        got_inst_id_tlv = TRUE;
        break;

      default:
        LOG_MSG_INFO2_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if (QMI_ERR_NONE != errval)
    {
      break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_CSVT,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and qmi_svc_dsm_pullup will
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      break;
    }
  }
  if(!got_inst_id_tlv)
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  //get cs data call id from this instance id
  for(i=0;i<MAX_UCSD_CALLS;i++)
  {
    if(qmi_csvti_global.csvt_call_inst[i] == inst_id)
      break;
  }

  if(!qmi_csvt_validate_call_instance(inst_id,i,__LINE__))
  {
    errval = QMI_ERR_INVALID_ID;
    goto send_result;
  }

  if(FALSE == ds_ucsd_api_get_callstats(qmi_csvti_global.csvt_client_id,i,&call_stats))
  {
    LOG_MSG_INFO2_0("Error while getting callstats from ucsd");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if(call_stats.present == TRUE)
  {
    if (FALSE == qmi_svc_put_param_tlv(&response,
                                         CSVTI_PRM_TYPE_STATS_CALL_ACTIVE,
                                         sizeof(call_stats.call_active),
                                         &call_stats.call_active))
    {
      dsm_free_packet(&response);
      errval = QMI_ERR_NO_MEMORY;
    }

    if (FALSE == qmi_svc_put_param_tlv(&response,
                                         CSVTI_PRM_TYPE_STATS_TX_COUNTER,
                                         sizeof(call_stats.tx_data_counter),
                                         &call_stats.tx_data_counter))
    {
      dsm_free_packet(&response);
      errval = QMI_ERR_NO_MEMORY;
    }

    if (FALSE == qmi_svc_put_param_tlv(&response,
                                         CSVTI_PRM_TYPE_STATS_RX_COUNTER,
                                         sizeof(call_stats.rx_data_counter),
                                         &call_stats.rx_data_counter))
    {
      dsm_free_packet(&response);
      errval = QMI_ERR_NO_MEMORY;
    }
  }
  else
  {
    errval = QMI_ERR_INTERNAL;
  }

  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
} /* qmi_csvti_get_call_stats() */

/*===========================================================================
  FUNCTION QMI_CSVTI_SET_SUBSCRIPTION_BINDING()

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
static dsm_item_type * qmi_csvti_set_subscription_binding
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
  qmi_csvti_client_state_type *  csvt_client_sp;
  boolean  got_v_in_reqd = FALSE;
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);

  response = NULL;
  errval = QMI_ERR_NONE;

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  csvt_client_sp = (qmi_csvti_client_state_type *)cl_sp;

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
        LOG_MSG_ERROR_1( "Unrecognized TLV type (%d)",type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                          len,
                                          expected_len,
                                          FILE_ID_DS_QMI_CSVT,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and qmi_svc_dsm_pullup will
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
 
 if((subs_id >= CSVTI_BIND_SUBS_MAX) || (subs_id < CSVTI_BIND_SUBS_PRIMARY))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
    
#ifndef FEATURE_TRIPLE_SIM
  if(subs_id > CSVTI_BIND_SUBS_SECONDARY)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
    goto send_result;
  }
#endif

  if(qmi_csvti_global.controlling_client_sp[subs_id - 1] != NULL)
  {
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }
  
  csvt_client_sp->subscription_id = (qmi_csvti_client_subs_e_type)subs_id ;
  qmi_csvti_global.controlling_client_sp[subs_id - 1] = cl_sp;
  
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
} /* qmi_csvti_bind_subscription */
  
/*===========================================================================
  FUNCTION QMI_CSVTI_GET_BIND_SUBSCRIPTION()
  
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
static dsm_item_type * qmi_csvti_get_bind_subscription
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
  qmi_csvti_client_state_type *  csvt_client_sp;
  uint32 subs_id;
#endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp && cmd_buf_p && cl_sp && sdu_in);
    
  response = NULL;
  errval = QMI_ERR_NONE;

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  csvt_client_sp = (qmi_csvti_client_state_type *)cl_sp;
  
  if(csvt_client_sp == NULL)
  {
    LOG_MSG_ERROR_0("client state pointer is NULL");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  subs_id = csvt_client_sp->subscription_id;
  if((subs_id >= CSVTI_BIND_SUBS_MAX) || (subs_id < CSVTI_BIND_SUBS_PRIMARY))
  {
    LOG_MSG_ERROR_1("Invalid subscription id stored for client %d",csvt_client_sp->common.clid);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if(csvt_client_sp != qmi_csvti_global.controlling_client_sp[subs_id - 1])
  {
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  if(FALSE == qmi_svc_put_param_tlv(&response, 
                                    CSVTI_PRM_TYPE_BIND_SUBSCRIPTION,
                                    sizeof(subs_id),
                                    &subs_id))
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
}/* qmi_csvti_get_bind_subscription */
  
/*===========================================================================
  FUNCTION QMI_CSVTI_SEND_CALL_STATUS_IND()

  DESCRIPTION
    Sends the call status indication to the corresponding client with
    appropriate call end cause.

  PARAMETERS
    clid                : client id
    cmd_val             : CMD Callback Value
    request_status      : Call Request status
    call_end_cause      : Call End Cause
 
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_send_call_status_ind
(
  uint8                    clid,
  qmi_csvti_cmd_val_e_type cmd_val,
  uint32                   inst_id,
  boolean                  request_status,
  cm_call_end_e_type       call_end_cause
)
{
  dsm_item_type *  ind;

  struct
  {
    uint32 inst_id;
  } v_out1;

  struct
  {
    boolean request_status;
  } v_out2;

  struct
  {
    uint32 end_cause;
  } v_out3;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Inside send call status ind for %d cmd val!", cmd_val);
  if(!qmi_csvt_validate_call_instance(clid,inst_id,__LINE__))
  {
    return;
  }

  ind = NULL;

  memset(&v_out1, 0, sizeof(v_out1));
  memset(&v_out2, 0, sizeof(v_out2));
  memset(&v_out3, 0, sizeof(v_out3));

  if(call_end_cause != 0)
  {
    //add end_cause tlv
    v_out3.end_cause = call_end_cause;
    if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_CALL_END_CAUSE,
                                       sizeof (v_out3),
                                       &v_out3,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
    {
      dsm_free_packet(&ind);
      return;
    }
  }

  //add mandatory tlv
  v_out2.request_status = request_status;
  if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_CALL_REQUEST_STATUS,
                                       sizeof (v_out2),
                                       &v_out2,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
  {
    dsm_free_packet(&ind);
    return;
  }

  v_out1.inst_id = qmi_csvti_global.csvt_call_inst[inst_id];
  if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_INSTANCE_ID,
                                       sizeof (v_out1),
                                       &v_out1,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
  {
    dsm_free_packet(&ind);
    return;
  }

  qmi_csvti_send_ind(cmd_val, ind, clid);
}

/*===========================================================================
  FUNCTION QMI_CSVTI_EVENT_REPORT_IND()

  DESCRIPTION
    Sends an indication to the client.

  PARAMETERS
    sp               : service provided state pointer (user data)
    clid             : clientID

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
  static void qmi_csvti_event_report_ind
(
  uint8                  clid,
  qmi_csvti_call_evt     call_event,
  uint8                  inst_id,
  ds_ucsd_call_type      call_type,
  boolean                sync_call,
  boolean                transparent_call,
  sys_sys_mode_e_type    network_type,
  uint8                  network_speed,
  uint8                  max_frame_size,
  char *                 incoming_no,
  uint8                  incoming_no_len,
  ds_ucsd_uus_id_e_type  uus_id_type,
  char *                 uus_id,
  uint8                  uus_id_len,
  boolean                modify_allowed,
  cm_call_end_e_type     call_end_cause,
  sio_port_id_type       port_data
)
{
  dsm_item_type *  ind;
  
  PACKED struct PACKED_POST
  {
    uint32 event_type;
  } v_out1;

  PACKED struct PACKED_POST
  {
    uint32 inst_id;
  } v_out2;

  PACKED struct PACKED_POST
  {
    uint32 call_type;
  } v_out3;

  PACKED struct PACKED_POST
  {
    uint32 port_type;
    uint32 port_no;
  } v_out4;

  PACKED struct PACKED_POST
  {
    boolean modify_allowed;
  } v_out5;

  PACKED struct PACKED_POST
  {
    uint32 end_cause;
  } v_out6;

  PACKED struct PACKED_POST
  {
    boolean sync_call;
  } v_out7;

  PACKED struct PACKED_POST
  {
    uint32 transparent_call;
  } v_out8;

  PACKED struct PACKED_POST
  {
    uint32 network_type;
  } v_out9;

  PACKED struct PACKED_POST
  {
    uint16 network_speed;
  } v_out10;

  PACKED struct PACKED_POST
  {
    uint8 uus_id_len;
    uint32 uus_id_type;
  } v_out11;

  struct
  {
    uint32 incom_num_len;
  }v_out12;

  uint32 token;

  struct
  {
    uint8 port_type;
    uint8 port_no;
  } port;

  uint8 type = 0;
  uint16 num_bytes_pushed = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Inside Event Report indication for %d event!", call_event);
  if(!qmi_csvt_validate_call_instance(clid,inst_id,__LINE__))
  {
    return;
  }

  ind = NULL;

  memset(&v_out1, 0, sizeof(v_out1));
  memset(&v_out2, 0, sizeof(v_out2));
  memset(&v_out3, 0, sizeof(v_out3));
  memset(&v_out4, 0, sizeof(v_out4));
  memset(&v_out5, 0, sizeof(v_out5));
  memset(&v_out6, 0, sizeof(v_out6));
  memset(&v_out7, 0, sizeof(v_out7));
  memset(&v_out8, 0, sizeof(v_out8));
  memset(&v_out9, 0, sizeof(v_out9));
  memset(&v_out10, 0, sizeof(v_out10));
  memset(&v_out11, 0, sizeof(v_out11));
  memset(&v_out12, 0, sizeof(v_out12));

  switch(call_event)
  {
    case CSVTI_CALL_PROGRESS_NOTIFICATION_EVT:
      v_out3.call_type = call_type;
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_CALL_TYPE_IND,
                                       sizeof (v_out3),
                                       &v_out3,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }

      memscpy(&port,sizeof(uint16), &port_data,sizeof(uint16));
      v_out4.port_no = port.port_no;
      v_out4.port_type = port.port_type;

      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_PORT_DATA_IND,
                                       sizeof (v_out4),
                                       &v_out4,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }
      break;

    case CSVTI_CALL_CONFIRMATION_EVT:
    case CSVTI_CALL_SETUP_NOTIFICATION_EVT:
      v_out3.call_type = call_type;
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_CALL_TYPE_IND,
                                       sizeof (v_out3),
                                       &v_out3,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }

      v_out5.modify_allowed = modify_allowed;
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_MODIFY_ALLOWED_IND,
                                       sizeof (v_out5),
                                       &v_out5,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }
      break;

    case CSVTI_CALL_CONNECT_NOTIFICATION_EVT:
      v_out3.call_type = call_type;
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_CALL_TYPE_IND,
                                       sizeof (v_out3),
                                       &v_out3,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }

      v_out5.modify_allowed = modify_allowed;
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_MODIFY_ALLOWED_IND,
                                       sizeof (v_out5),
                                       &v_out5,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }

      memscpy(&port,sizeof(uint16), &port_data,sizeof(uint16));
      v_out4.port_no = port.port_no;
      v_out4.port_type = port.port_type;

      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_PORT_DATA_IND,
                                       sizeof (v_out4),
                                       &v_out4,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }

      //save some more call info for this acive call instance
      qmi_csvti_global.csvt_call_info[inst_id].call_state = CALL_ACTIVE;
      qmi_csvti_global.csvt_call_info[inst_id].modify_allowed = 
        modify_allowed;
      qmi_csvti_global.csvt_call_info[inst_id].port.port_family
        = v_out4.port_type;
      qmi_csvti_global.csvt_call_info[inst_id].port.port_number
        = v_out4.port_no;

    break;

    case CSVTI_CALL_END_EVT:
      v_out6.end_cause = call_end_cause;
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                         CSVTI_PRM_TYPE_CALL_END_IND,
                                         sizeof (v_out6),
                                         &v_out6,
                                         FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }

      //save end cause for this call instance
      memset(&qmi_csvti_global.csvt_call_info[inst_id],0,
        sizeof(qmi_csvti_call_info));
      qmi_csvti_global.csvt_call_info[inst_id].call_state = 
        CALL_ENDED;
      qmi_csvti_global.csvt_call_info[inst_id].call_end_cause =
        call_end_cause;

    break;

    case CSVTI_CALL_INCOMING_EVT:
      //add relevant checks for incoming no and uus id
      v_out7.sync_call = sync_call;
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                         CSVTI_PRM_TYPE_SYNC_CALL_IND,
                                         sizeof (v_out7),
                                         &v_out7,
                                         FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }

      v_out8.transparent_call = transparent_call;
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                         CSVTI_PRM_TYPE_TRANSPARENT_CALL_IND,
                                         sizeof (v_out8),
                                         &v_out8,
                                         FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }

      v_out9.network_type = network_type;
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                         CSVTI_PRM_TYPE_NETWORK_TYPE_IND,
                                         sizeof (v_out9),
                                         &v_out9,
                                         FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }

      v_out10.network_speed = network_speed;
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                         CSVTI_PRM_TYPE_NETWORK_SPEED_IND,
                                         sizeof (v_out10),
                                         &v_out10, 
                                         FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }

      if((NULL == incoming_no) || (MAX_DIAL_STRING_LEN <= incoming_no_len))
      {
        LOG_MSG_ERROR_0("Invalid incoming number or incoming number length");
        dsm_free_packet(&ind);
        return;
      }
      if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                         CSVTI_PRM_TYPE_INCOMING_NUM_IND,
                                         incoming_no_len,
                                         incoming_no,
                                         FILE_ID_DS_QMI_CSVT,__LINE__) )
      {
        dsm_free_packet(&ind);
        return;
      }
      if(qmi_csvti_global.is_qmss_enabled)
      {
        if (MAX_DIAL_STRING_LEN > incoming_no_len)
        {        
          v_out12.incom_num_len = incoming_no_len;
          if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                             CSVTI_PRM_TYPE_INCOMING_NUM_LEN_IND,
                                             sizeof (v_out12),
                                             &v_out12,
                                             FILE_ID_DS_QMI_CSVT,__LINE__) )
          {
            dsm_free_packet(&ind);
            return;
          }
        }
      }
      if((uus_id != NULL) && (uus_id_len < QMI_CSVT_UUS_ID_MAX_SIZE))
      {
        type = CSVTI_PRM_TYPE_UUS_ID_TYPE_IND;
        num_bytes_pushed = uus_id_len + 5;
        v_out11.uus_id_len = uus_id_len;
        v_out11.uus_id_type = uus_id_type;
  
        if(!((QMI_SVC_PKT_PUSH(&ind,
                               uus_id,
                               uus_id_len)) &&
             (QMI_SVC_PKT_PUSH(&ind,
                               &v_out11.uus_id_len,
                               sizeof(uint8))) &&
             (QMI_SVC_PKT_PUSH(&ind,
                               &v_out11.uus_id_type,
                               sizeof(uint32))) &&
             (QMI_SVC_PKT_PUSH(&ind,
                               &num_bytes_pushed,
                               sizeof(uint16))) &&
             (QMI_SVC_PKT_PUSH(&ind,
                               &type,
                               sizeof(uint8)))
           ))
        {
          LOG_MSG_INFO2_0("Unable to generate Event Report indication!");
          dsm_free_packet(&ind);
          return;
        }
      }

      //save call info for this instance
      memset(&qmi_csvti_global.csvt_call_info[inst_id],0,
        sizeof(qmi_csvti_call_info));
      qmi_csvti_global.csvt_call_info[inst_id].call_state = 
        CALL_IN_PROGRESS;
      qmi_csvti_global.csvt_call_info[inst_id].is_synchronous = 
        sync_call;
      qmi_csvti_global.csvt_call_info[inst_id].is_transparent = 
        transparent_call;
      qmi_csvti_global.csvt_call_info[inst_id].network_type = 
        network_type;
      qmi_csvti_global.csvt_call_info[inst_id].network_speed = 
        network_speed;
      memscpy(qmi_csvti_global.csvt_call_info[inst_id].dial_string,
              incoming_no_len,
              incoming_no,
              incoming_no_len);
      qmi_csvti_global.csvt_call_info[inst_id].dial_string_len =
        incoming_no_len;
    break;

    case CSVTI_CALL_MODIFICATION_EVT:

    break;

  }

  //Add mandatory TLV's
  //get token to be sent to apps
  token = qmi_csvti_global.csvt_call_inst[inst_id];
  v_out2.inst_id = token;
  if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_INSTANCE_ID,
                                       sizeof (v_out2),
                                       &v_out2,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
  {
    dsm_free_packet(&ind);
    return;
  }

  v_out1.event_type = call_event;
  if( FALSE == qmi_svc_put_param_tlv2(&ind, 
                                       CSVTI_PRM_TYPE_EVENT_TYPE,
                                       sizeof (v_out1),
                                       &v_out1,
                                       FILE_ID_DS_QMI_CSVT,__LINE__) )
  {
    dsm_free_packet(&ind);
    return;
  }

  qmi_csvti_send_ind(CSVTI_CMD_VAL_SET_EVENT_REPORT_IND, ind, clid);
} /* qmi_csvti_event_report_ind() */

/*===========================================================================
  FUNCTION QMI_CSVTI_SEND_PENDING_RESPONSE()

  DESCRIPTION
    Sends the response message to a pending request message.

  PARAMETERS
    cmd_val: command value type of the pending response.

  RETURN VALUE
    boolean  : True on success, false otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_csvti_send_pending_response
(
  qmi_csvti_cmd_val_e_type  cmd_val
)
{
  qmi_cmd_buf_type *      cmd_buf_p;

  dsm_item_type *         response = NULL;
  qmi_error_e_type        errval = QMI_ERR_NONE;
  qmi_result_e_type       result;
  boolean                 retval;

  cmd_buf_p = (qmi_cmd_buf_type *) 
                 qmi_csvti_global.pending.cmd_buf_ptr;
  if(cmd_buf_p == NULL)
  {
    LOG_MSG_INFO1_0("Null cmd_buf ptr in process_progress_info");
    memset(&qmi_csvti_global.pending,0,sizeof(qmi_csvti_global.pending));
    return FALSE;
  }

  LOG_MSG_INFO2_1("Received cmd_buf_ptr(user_data) %x in send_pending_response",
                  cmd_buf_p);

  if( NULL == cmd_buf_p->x_p || NULL == cmd_buf_p->x_p->cl_sp )
  {
    LOG_MSG_INFO1_0("Transaction ptr or client state pointer is NULL, most likely client was released.");
    return FALSE;
  }

  LOG_MSG_INFO2_2("Sending pending response for %d cmd 0x%x cmd buf ptr",
                  cmd_val, cmd_buf_p);

  /*
    It is possible that we recieve end notification from ucsd for answer,confirm etc. so
    in that case we should send the failed response for the pending cmd. But if we 
    recieve end notification for a pending end command only then we should send a
    success response.
  */

  if((cmd_val == CSVTI_CMD_VAL_END_CALL) &&
       (qmi_csvti_global.pending.cmd != CSVTI_CMD_VAL_END_CALL))
  {
    cmd_val = qmi_csvti_global.pending.cmd;
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  if(FALSE == retval)
  {
    dsm_free_packet(&response);
    ds_qmi_fw_free_cmd_buf( &cmd_buf_p );
    return FALSE;
  }

  if( FALSE == qmi_csvti_send_response(cmd_buf_p, response) )
  {
    LOG_MSG_INFO2_1 ("Unable to send response for pending CSVT operation %d",
                     cmd_val);
    dsm_free_packet(&response);
    ds_qmi_fw_free_cmd_buf( &cmd_buf_p );
    return FALSE;
  }
  
  LOG_MSG_INFO2_0("pending resonse sent!!");
  return TRUE;
}

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_PROGRESS_INFO()

  DESCRIPTION
    Process the progress event recieved from ucsd and send indication to the client.

  PARAMETERS
    cmd_ptr :  Pointer to the command buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_progress_info
(
  qmi_csvti_cmd_buf_type *cmd_ptr
)
{
  qmi_csvti_client_state_type *  cl_sp;
  uint32 active_cs_subs;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);

 active_cs_subs = qmi_csvti_global.active_cs_subs;
 if((active_cs_subs >= CSVTI_BIND_SUBS_MAX) || (active_cs_subs < CSVTI_BIND_SUBS_PRIMARY))
 {
   LOG_MSG_INFO1_0("invalid wcdma subs");
   return;
 }

  cl_sp = (qmi_csvti_client_state_type *)qmi_csvti_global.controlling_client_sp[active_cs_subs - 1];
  
  if(NULL == cl_sp || cmd_ptr->data.end_evt_info.inst_id >= MAX_UCSD_CALLS)
  {
    LOG_MSG_INFO2_1("No controlling client sp set till now or Invalid instance id %d recieved!!",
                    cmd_ptr->data.end_evt_info.inst_id);
    return;
  }

  if(!qmi_csvti_global.is_qmss_enabled)
  {
    //send orig indication
    qmi_csvti_send_call_status_ind(cl_sp->common.clid,
                                   CSVTI_CMD_VAL_ORIGINATE_CALL,
                                   cmd_ptr->data.progress_evt_info.inst_id,
                                   0,
                                   0);
  
    qmi_csvti_global.pending_orig_ind = FALSE;
  }

  LOG_MSG_INFO2_0("Sending progress indication ");
  //send event report indication
  qmi_csvti_event_report_ind(cl_sp->common.clid,
                             CSVTI_CALL_PROGRESS_NOTIFICATION_EVT,
                             cmd_ptr->data.progress_evt_info.inst_id,
                             cmd_ptr->data.progress_evt_info.call_type,
                             0,
                             0,
                             0,
                             0,
                             0,
                             NULL,
                             0,
                             0,
                             NULL,
                             0,
                             0,
                             0,
                             cmd_ptr->data.progress_evt_info.port);

}/*qmi_csvti_process_progress_info*/

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_CONFIRM_INFO()

  DESCRIPTION
    Process the confirm event recieved from ucsd and send respnse and 
    indication to the client.

  PARAMETERS
    cmd_ptr :  Pointer to the command buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_confirm_info
(
  qmi_csvti_cmd_buf_type *cmd_ptr
)
{
  qmi_csvti_client_state_type *  cl_sp; 
  uint32 active_cs_subs;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);  
 
  active_cs_subs = qmi_csvti_global.active_cs_subs;
  if(CSVTI_BIND_SUBS_MAX <= active_cs_subs)
  {
    LOG_MSG_ERROR_1("Invalid wcdma subs %d.Ignoring confirm info",active_cs_subs);
    return;
  }
  cl_sp = (qmi_csvti_client_state_type *)qmi_csvti_global.controlling_client_sp[active_cs_subs - 1];
  
  if(NULL == cl_sp || cmd_ptr->data.end_evt_info.inst_id >= MAX_UCSD_CALLS)
  {
    LOG_MSG_INFO2_1("No controlling client sp set till now!! or Invalid instance id %d",
                    cmd_ptr->data.end_evt_info.inst_id);
    return;
  }

  if(qmi_csvti_global.is_qmss_enabled)
  {
    //send orig indication
    qmi_csvti_send_call_status_ind(cl_sp->common.clid,
                                   CSVTI_CMD_VAL_ORIGINATE_CALL,
                                   cmd_ptr->data.progress_evt_info.inst_id,
                                   0,
                                   0);
 
    qmi_csvti_global.pending_orig_ind = FALSE;
  }

  //send indication
  LOG_MSG_INFO2_0("Sending confirm indication ");

  qmi_csvti_event_report_ind(cl_sp->common.clid,
                             CSVTI_CALL_CONFIRMATION_EVT,
                             cmd_ptr->data.confirm_evt_info.inst_id,
                             cmd_ptr->data.confirm_evt_info.call_type,
                             0,
                             0,
                             0,
                             0,
                             0,
                             NULL,
                             0,
                             0,
                             NULL,
                             0,
                             cmd_ptr->data.confirm_evt_info.modify_allowed,
                             0,
                             0);

}/*qmi_csvti_process_confirm_info*/

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
/*===========================================================================
  FUNCTION QMI_CSVTI_CONVERT_CM_SUBS_ID()

  DESCRIPTION
    Converts CM subs_id to QMI_CSVT subs_id.

  PARAMETERS
    subs_id :  CM subs_idPointer to the command buffer

  RETURN VALUE
    qmi_csvti_client_subs_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csvti_client_subs_e_type qmi_csvti_convert_cm_subs_id
(
  sys_modem_as_id_e_type subs_id
)
{
  switch (subs_id)
  {
    case SYS_MODEM_AS_ID_1:
      return CSVTI_BIND_SUBS_PRIMARY;
      break;

#ifdef FEATURE_DUAL_SIM
    case SYS_MODEM_AS_ID_2:
      return CSVTI_BIND_SUBS_SECONDARY;
#endif

#ifdef FEATURE_TRIPLE_SIM
    case SYS_MODEM_AS_ID_3:
    return CSVTI_BIND_SUBS_TERTIARY;
#endif

    default:
     return CSVTI_BIND_SUBS_MAX;    
  }
}/* qmi_csvti_convert_cm_subs_id */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_SETUP_INFO()

  DESCRIPTION
    Process the setup event recieved from ucsd and send indication to the client.

  PARAMETERS
    cmd_ptr :  Pointer to the command buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_setup_info
(
  qmi_csvti_cmd_buf_type *cmd_ptr
)
{
  uint8  inst_id;
  qmi_csvti_client_state_type *  cl_sp;
#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  sys_modem_as_id_e_type subs_id;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  qmi_csvti_client_subs_e_type qmi_csvt_subs_id = CSVTI_BIND_SUBS_MAX;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);

  inst_id = cmd_ptr->data.setup_evt_info.inst_id;
  if(inst_id >= MAX_UCSD_CALLS)
  {
    LOG_MSG_INFO2_0 ("Invalid instance id recvd in incoming event");
    return;
  }

#if 0
  //assign the token corresponding to the CS call inst id in the global variable
  //Also keeping a check so that we dont assign 0 inst id because of
  //wrapping up.
  if(qmi_csvti_global.csvt_token == 0xFF)
  {
    ++qmi_csvti_global.csvt_token;
  }
#endif

  //send indication
  LOG_MSG_INFO2_0("Sending setup indication ");

#if (defined FEATURE_DUAL_SIM || defined FEATURE_TRIPLE_SIM)
  subs_id = ds3g_get_cs_subs_id();
  qmi_csvt_subs_id = qmi_csvti_convert_cm_subs_id(subs_id);

  if((qmi_csvt_subs_id >= CSVTI_BIND_SUBS_MAX)|| (qmi_csvt_subs_id < CSVTI_BIND_SUBS_PRIMARY))
  {
    LOG_MSG_INFO2_0("No active WCDMA subscription available. Ignoring incoming evt");
    qmi_csvti_global.active_cs_subs = CSVTI_BIND_SUBS_MAX;
    return;
  }

  qmi_csvti_global.active_cs_subs = qmi_csvt_subs_id;
#else
 qmi_csvti_global.active_cs_subs = CSVTI_BIND_SUBS_PRIMARY;
 qmi_csvt_subs_id = CSVTI_BIND_SUBS_PRIMARY;
#endif

  cl_sp = (qmi_csvti_client_state_type *)qmi_csvti_global.controlling_client_sp[qmi_csvt_subs_id - 1];

  if(NULL == cl_sp)
  {
    LOG_MSG_INFO2_0("No controlling client sp set till now!!");
    return;
  }

  qmi_csvti_global.csvt_call_inst[inst_id] = --qmi_csvti_global.csvt_token;  

  qmi_csvti_event_report_ind(cl_sp->common.clid,
                             CSVTI_CALL_SETUP_NOTIFICATION_EVT,
                             cmd_ptr->data.setup_evt_info.inst_id,
                             cmd_ptr->data.setup_evt_info.call_type,
                             0,
                             0,
                             0,
                             0,
                             0,
                             NULL,
                             0,
                             0,
                             NULL,
                             0,
                             cmd_ptr->data.setup_evt_info.modify_allowed,
                             0,
                             0);

}/*qmi_csvti_process_setup_info*/

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_CONNECT_INFO()

  DESCRIPTION
    Process the connect event recieved from ucsd and send indication to the client.

  PARAMETERS
    cmd_ptr :  Pointer to the command buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_connect_info
(
  qmi_csvti_cmd_buf_type *cmd_ptr
)
{
  qmi_csvti_client_state_type *  cl_sp;
  uint32 active_cs_subs;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);

  active_cs_subs = qmi_csvti_global.active_cs_subs;
  if(CSVTI_BIND_SUBS_MAX <= active_cs_subs)
  {
    LOG_MSG_ERROR_1("Invalid wcdma subs %d.Ignoring connect info",active_cs_subs);
    return;
  }
  cl_sp = (qmi_csvti_client_state_type *)qmi_csvti_global.controlling_client_sp[active_cs_subs - 1];
  
  if(NULL == cl_sp || cmd_ptr->data.end_evt_info.inst_id >= MAX_UCSD_CALLS)
  {
    LOG_MSG_INFO2_1("No controlling client sp set till now or Invalid instance id %d recieved!!",
                    cmd_ptr->data.end_evt_info.inst_id);
    return;
  }

  /*
    send response if we got connect notification for our answer req. to an incoming call,
    for outgoing call we will just send the indication as response has already been sent.
  */
  if((qmi_csvti_global.pending.active == TRUE) &&
      (qmi_csvti_global.pending.cmd == CSVTI_CMD_VAL_ANSWER_CALL))
  {
    if(FALSE == qmi_csvti_send_pending_response(CSVTI_CMD_VAL_ANSWER_CALL))
    {
      LOG_MSG_INFO2_1 ("Unable to send response for pending CSVT operation %d",
                       CSVTI_CMD_VAL_ANSWER_CALL);
      return;
    }

    //send answer indication to the client
    qmi_csvti_send_call_status_ind(cl_sp->common.clid,
                                 CSVTI_CMD_VAL_ANSWER_CALL,
                                 cmd_ptr->data.connect_evt_info.inst_id,
                                 0,
                                 0);

    memset(&qmi_csvti_global.pending,0,sizeof(qmi_csvti_global.pending));
  }

  LOG_MSG_INFO2_0("Sending connect indication ");

  //send indication
  qmi_csvti_event_report_ind(cl_sp->common.clid,
                             CSVTI_CALL_CONNECT_NOTIFICATION_EVT,
                             cmd_ptr->data.connect_evt_info.inst_id,
                             cmd_ptr->data.connect_evt_info.call_type,
                             0,
                             0,
                             0,
                             0,
                             0,
                             NULL,
                             0,
                             0,
                             NULL,
                             0,
                             cmd_ptr->data.connect_evt_info.modify_allowed,
                             0,
                             cmd_ptr->data.connect_evt_info.port);

}/*qmi_csvti_process_connect_info*/

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_END_INFO()

  DESCRIPTION
    Process the end event recieved from ucsd and send response and 
    indication to the client.

  PARAMETERS
    cmd_ptr :  Pointer to the command buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_end_info
(
  qmi_csvti_cmd_buf_type *cmd_ptr
)
{
  qmi_csvti_client_state_type *  cl_sp;
  uint32 active_cs_subs;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);

  if(cmd_ptr->data.end_evt_info.inst_id >= MAX_UCSD_CALLS)
  {
    LOG_MSG_INFO2_1 ("Invalid instance id %d recieved in end event info ",
                     cmd_ptr->data.end_evt_info.inst_id);
    return;
  }

  if(qmi_csvti_global.pending.active == TRUE)
  {
    //send response
    if(FALSE == qmi_csvti_send_pending_response(CSVTI_CMD_VAL_END_CALL))
    {
      LOG_MSG_INFO2_1 ("Unable to send response for pending CSVT operation %d",
                       CSVTI_CMD_VAL_END_CALL);
      return;
    }
  }

  //send indication
  LOG_MSG_INFO2_0("Sending end indication ");

  active_cs_subs = qmi_csvti_global.active_cs_subs;
  if(CSVTI_BIND_SUBS_MAX <= active_cs_subs)
  {
    LOG_MSG_ERROR_1("Invalid wcdma subs %d.Ignoring end info",active_cs_subs);
    return;
  }
  cl_sp = (qmi_csvti_client_state_type *)qmi_csvti_global.controlling_client_sp[active_cs_subs - 1];

  if(NULL == cl_sp)
  {
    LOG_MSG_INFO2_0("No controlling client sp set till now!!");
    return;
  }

  //check if we need to send 1 shot orig/confirm/answer/end indication
  if(TRUE == qmi_csvti_global.pending_orig_ind)
  {
    //orig status indication is pending so send it now
    qmi_csvti_send_call_status_ind(cl_sp->common.clid,
                                   CSVTI_CMD_VAL_ORIGINATE_CALL,
                                   cmd_ptr->data.end_evt_info.inst_id,
                                   1,
                                   cmd_ptr->data.end_evt_info.end_cause);

    qmi_csvti_global.pending_orig_ind = FALSE;
  }
  else if(TRUE == qmi_csvti_global.pending.active)
  {
    if((CSVTI_CMD_VAL_CONFIRM_CALL == qmi_csvti_global.pending.cmd)
       ||(CSVTI_CMD_VAL_ANSWER_CALL == qmi_csvti_global.pending.cmd))
    {
      //send confirm/answer status indication
      qmi_csvti_send_call_status_ind(cl_sp->common.clid,
                                     qmi_csvti_global.pending.cmd,
                                     cmd_ptr->data.end_evt_info.inst_id,
                                     1,
                                     cmd_ptr->data.end_evt_info.end_cause);
    }
    else if(CSVTI_CMD_VAL_END_CALL == qmi_csvti_global.pending.cmd)
    {
      //send end call status indication
      qmi_csvti_send_call_status_ind(cl_sp->common.clid,
                                     CSVTI_CMD_VAL_END_CALL,
                                     cmd_ptr->data.end_evt_info.inst_id,
                                     0,
                                     0);
    }
  }

  qmi_csvti_event_report_ind(cl_sp->common.clid,
                             CSVTI_CALL_END_EVT,
                             cmd_ptr->data.end_evt_info.inst_id,
                             0,
                             0,
                             0,
                             0,
                             0,
                             0,
                             NULL,
                             0,
                             0,
                             NULL,
                             0,
                             0,
                             cmd_ptr->data.end_evt_info.end_cause,
                             0);

  //Reset call inst.
  qmi_csvti_global.csvt_call_inst[cmd_ptr->data.end_evt_info.inst_id] = 0;
  memset(&qmi_csvti_global.pending,0,sizeof(qmi_csvti_global.pending));
}/*qmi_csvti_process_end_info*/

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_INCOMING_INFO()

  DESCRIPTION
    Process the incoming event recieved from ucsd and send response and 
    indication to the client.

  PARAMETERS
    cmd_ptr :  Pointer to the command buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_incoming_info
(
  qmi_csvti_cmd_buf_type *cmd_ptr
)
{
  qmi_csvti_client_state_type *  cl_sp;
  uint32 active_cs_subs;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);

  if(qmi_csvti_global.pending.active == TRUE && 
         qmi_csvti_global.pending.cmd == CSVTI_CMD_VAL_CONFIRM_CALL)
  {
    //send response
    if(FALSE == qmi_csvti_send_pending_response(CSVTI_CMD_VAL_CONFIRM_CALL))
    {
      LOG_MSG_INFO2_1 ("Unable to send response for pending CSVT operation %d",
                       CSVTI_CMD_VAL_CONFIRM_CALL);
      return;
    }
  }

  active_cs_subs = qmi_csvti_global.active_cs_subs;
  if(CSVTI_BIND_SUBS_MAX <= active_cs_subs)
  {
    LOG_MSG_ERROR_1("Invalid wcdma subs %d.Ignoring incoming info",active_cs_subs);
    return;
  }
  cl_sp = (qmi_csvti_client_state_type *)qmi_csvti_global.controlling_client_sp[active_cs_subs - 1];

  if (cl_sp == NULL || cmd_ptr->data.end_evt_info.inst_id >= MAX_UCSD_CALLS)
  {
     LOG_MSG_INFO2_1("cl_sp is NULL or inst_id is invalid, inst_id=%d",
                     cmd_ptr->data.end_evt_info.inst_id);
     return;
  }
  //send confirm indication to the client
  qmi_csvti_send_call_status_ind(cl_sp->common.clid,
                                 CSVTI_CMD_VAL_CONFIRM_CALL,
                                 cmd_ptr->data.incoming_evt_info.inst_id,
                                 0,
                                 0);

  //send indication

  //properly generate instance id
  qmi_csvti_event_report_ind(cl_sp->common.clid,
                             CSVTI_CALL_INCOMING_EVT,
                             cmd_ptr->data.incoming_evt_info.inst_id,
                             0,
                             cmd_ptr->data.incoming_evt_info.name,
                             cmd_ptr->data.incoming_evt_info.connection_element,
                             cmd_ptr->data.incoming_evt_info.network_mode,
                             cmd_ptr->data.incoming_evt_info.speed,
                             0,
                             (char *)cmd_ptr->data.incoming_evt_info.caller_number,
                             cmd_ptr->data.incoming_evt_info.caller_number_len,
                             0,
                             (char *)cmd_ptr->data.incoming_evt_info.uus_data,
                             cmd_ptr->data.incoming_evt_info.uus_data_len,
                             0,
                             0,
                             0);

  memset(&qmi_csvti_global.pending,0,sizeof(qmi_csvti_global.pending));
}/*qmi_csvti_process_incoming_info*/

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_MODIFY_INFO()

  DESCRIPTION
    Process the modify event recieved from ucsd and send response and
    indication to the client.

  PARAMETERS
    cmd_ptr :  Pointer to the command buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_modify_info
(
  qmi_csvti_cmd_buf_type *cmd_ptr
)
{
  qmi_csvti_client_state_type *  cl_sp;
  uint32 active_cs_subs;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);

  if(qmi_csvti_global.pending.active == TRUE && 
         qmi_csvti_global.pending.cmd == CSVTI_CMD_VAL_MODIFY_CALL)
  {
    //send response
    if(FALSE == qmi_csvti_send_pending_response(CSVTI_CMD_VAL_MODIFY_CALL))
    {
      LOG_MSG_INFO2_1 ("Unable to send response for pending CSVT operation %d",
                       CSVTI_CMD_VAL_MODIFY_CALL);
      return;
    }
    memset(&qmi_csvti_global.pending,0,sizeof(qmi_csvti_global.pending));
  }

  if(cmd_ptr->data.end_evt_info.inst_id >= MAX_UCSD_CALLS)
  {
      LOG_MSG_INFO2_1 ("Invalid instance id %d recieved ",
                       cmd_ptr->data.end_evt_info.inst_id);
      return;
  }


  active_cs_subs = qmi_csvti_global.active_cs_subs;
  if(CSVTI_BIND_SUBS_MAX <= active_cs_subs)
  {
    LOG_MSG_ERROR_1("Invalid wcdma subs %d.Ignoring modify info",active_cs_subs);
    return;
  }
  cl_sp = (qmi_csvti_client_state_type *)qmi_csvti_global.controlling_client_sp[active_cs_subs - 1];
  
  if (cl_sp == NULL)
  {
    LOG_MSG_INFO2_0("cl_sp is NULL!!" );
    return;
  }

  qmi_csvti_event_report_ind(cl_sp->common.clid,
                             CSVTI_CALL_MODIFICATION_EVT,
                             cmd_ptr->data.end_evt_info.inst_id,
                             0,
                             0,
                             0,
                             0,
                             0,
                             0,
                             NULL,
                             0,
                             0,
                             NULL,
                             0,
                             0,
                             0,
                             0);

}/*qmi_csvti_process_modify_info*/

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_CMD()

  DESCRIPTION
    This function processes a QMI CSVT command.

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI WDS command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI CSVT must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_cmd
(
  void *cmd_ptr
)
{
  qmi_csvti_cmd_buf_type *cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);
  cmd_buf_ptr = (qmi_csvti_cmd_buf_type *)cmd_ptr;
  LOG_MSG_INFO2_1( "csvt: process command %d", cmd_buf_ptr->cmd_id );

  switch(cmd_buf_ptr->cmd_id)
  {
    case QMI_CMD_CSVT_INIT_CB:
      qmi_csvti_process_svc_init(cmd_buf_ptr->data.init_cb.num_instances);
      break;

    case QMI_CMD_CSVT_ALLOC_CLID_CB:
      qmi_csvti_process_alloc_clid(&cmd_buf_ptr->data.clid_cb.common_msg_hdr);
      break;

    case QMI_CMD_CSVT_DEALLOC_CLID_CB:
      qmi_csvti_process_dealloc_clid(&cmd_buf_ptr->data.clid_cb.common_msg_hdr);
      break;

    case QMI_CMD_CSVT_CMD_HDLR_CB:
      qmi_csvti_process_cmd_hdlr(&cmd_buf_ptr->data.cmd_hdlr_cb.msg_hdr,
                                    cmd_buf_ptr->data.cmd_hdlr_cb.sdu_in );
      break;

    case QMI_CMD_PROCESS_CSVT_CALL_CONNECT_EVT:
        qmi_csvti_process_connect_info(cmd_buf_ptr);
    break;

    case QMI_CMD_PROCESS_CSVT_PROGRESS_INFO_EVT:
        qmi_csvti_process_progress_info(cmd_buf_ptr);
    break;

    case QMI_CMD_PROCESS_CSVT_CONFIRM_INFO_EVT:
        qmi_csvti_process_confirm_info(cmd_buf_ptr);
    break;

    case QMI_CMD_PROCESS_CSVT_SETUP_INFO_EVT:
      qmi_csvti_process_setup_info(cmd_buf_ptr);
    break;

    case QMI_CMD_PROCESS_CSVT_END_INFO_EVT:
      qmi_csvti_process_end_info(cmd_buf_ptr);
    break;

    case QMI_CMD_PROCESS_CSVT_INCOMING_INFO_EVT:
      qmi_csvti_process_incoming_info(cmd_buf_ptr);
    break;

    case QMI_CMD_PROCESS_CSVT_MODIFY_INFO_EVT:
      qmi_csvti_process_modify_info(cmd_buf_ptr);
    break;

    default:
      LOG_MSG_ERROR_1("Unexpected csvt cmd(%d) recd in QMI CSVT cmd handler",
                      cmd_buf_ptr->cmd_id);
    break;
  }/* switch(cmd_buf_ptr->cmd_id) */

  qmi_csvti_free_cmd_buf(cmd_ptr);
  return;
} /* qmi_csvti_process_cmd() */

/*===========================================================================
  FUNCTION QMI_CSVTI_GET_CSVT_CLIENT_ID_BY_CL_SP()

  DESCRIPTION
    This function returns the client id from a valid client state ptr.
    
  PARAMETERS
    cl_sp : client state ptr

  RETURN VALUE
    client id
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static uint32  qmi_csvti_get_csvt_client_id_by_cl_sp
(
  qmi_csvti_client_state_type *  cl_sp
)
{
  if (cl_sp != NULL)
  {
    return cl_sp->common.clid;
  }

  LOG_MSG_ERROR_0("Invalid Client index found");

  return CSVTI_INVALID_CLIENT;

} /* qmi_csvti_get_csvt_client_id_by_cl_sp() */

/*===========================================================================
FUNCTION QMI_CSVTI_INIT_CB()

  DESCRIPTION
  Function gets called from framework whenever the QMI CSVT service registers with
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
static void qmi_csvti_init_cb
(
  uint16 num_instances
)
{
  qmi_csvti_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr = (qmi_csvti_cmd_buf_type *)qmi_csvti_get_cmd_buf(QMI_CMD_CSVT_INIT_CB);
  if( cmd_ptr == NULL)
  {
    return;
  }

  memset(cmd_ptr, 0, sizeof(qmi_csvti_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_CSVT_INIT_CB;
  cmd_ptr->data.init_cb.num_instances = num_instances;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);

} /* qmi_csvti_init_cb() */

/*===========================================================================
FUNCTION QMI_CSVTI_ALLOC_CLID_CB()

DESCRIPTION
  Function gets called from framework whenever a clid is allocated by the
  framework for QMI CSVT service. This function posts a DCC command for the
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
static boolean qmi_csvti_alloc_clid_cb
(  
  qmi_framework_common_msg_hdr_type*   common_msg_hdr  
)
{
  qmi_csvti_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(common_msg_hdr);

  LOG_MSG_INFO1_2("QMI CSVT alloc clid cb: clid:%d, QMI Instances:%d",
                  common_msg_hdr->client_id, common_msg_hdr->qmi_instance);

  cmd_ptr = (qmi_csvti_cmd_buf_type *)qmi_csvti_get_cmd_buf(QMI_CMD_CSVT_ALLOC_CLID_CB);
  if( cmd_ptr == NULL)
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    copy the framework common message header to local struct
  -------------------------------------------------------------------------*/
  memset(cmd_ptr, 0, sizeof(qmi_csvti_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_CSVT_ALLOC_CLID_CB;
  cmd_ptr->data.clid_cb.common_msg_hdr.service = common_msg_hdr->service;
  cmd_ptr->data.clid_cb.common_msg_hdr.client_id = common_msg_hdr->client_id;
  cmd_ptr->data.clid_cb.common_msg_hdr.transaction_id = common_msg_hdr->transaction_id;
  cmd_ptr->data.clid_cb.common_msg_hdr.qmi_instance = common_msg_hdr->qmi_instance;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
  return TRUE;
} /* qmi_csvti_alloc_clid_cb */

/*===========================================================================
FUNCTION QMI_CSVI_DEALLOC_CLID_CB()

DESCRIPTION 
  Function gets called from framework whenever a clid is deallocated by the
  framework for QMI CSVT service. This function posts a DCC command for the
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
static void qmi_csvti_dealloc_clid_cb
(  
  qmi_framework_common_msg_hdr_type*   common_msg_hdr  
)
{
  qmi_csvti_cmd_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(common_msg_hdr);
  LOG_MSG_INFO1_2("QMI CSVT dealloc clid cb: clid:%d, QMI Instances:%d",
                  common_msg_hdr->client_id, common_msg_hdr->qmi_instance);

  cmd_ptr = (qmi_csvti_cmd_buf_type *)qmi_csvti_get_cmd_buf(QMI_CMD_CSVT_DEALLOC_CLID_CB);
  if( cmd_ptr == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    copy the framework common message header to local struct
  -------------------------------------------------------------------------*/
  memset(cmd_ptr, 0, sizeof(qmi_csvti_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_CSVT_DEALLOC_CLID_CB;
  cmd_ptr->data.clid_cb.common_msg_hdr = *common_msg_hdr;
  cmd_ptr->data.clid_cb.common_msg_hdr.client_id = common_msg_hdr->client_id;
  cmd_ptr->data.clid_cb.common_msg_hdr.transaction_id = common_msg_hdr->transaction_id;
  cmd_ptr->data.clid_cb.common_msg_hdr.qmi_instance = common_msg_hdr->qmi_instance;

  /*-------------------------------------------------------------------------
    Positing command to QMI Service task to handle the clid callback
  -------------------------------------------------------------------------*/
  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
} /* qmi_csvti_dealloc_clid_cb */

/*===========================================================================
  FUNCTION FRAMEWORK_CSVTI_CMD_HDLR_CB()

  DESCRIPTION
    Function gets called from framework whenever a new QMI CSVT request
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
static void  qmi_csvti_cmd_hdlr_cb
( 
  qmi_framework_msg_hdr_type * msg_hdr,
  dsm_item_type             ** sdu 
)
{
  qmi_csvti_cmd_buf_type *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(msg_hdr);
  LOG_MSG_INFO1_3("qmi_csvti_cmd_hdlr_cb: clid:%d, QMI Instances:%d, tx_id: %d",
                  msg_hdr->common_hdr.client_id,
                  msg_hdr->common_hdr.qmi_instance,
                  msg_hdr->common_hdr.transaction_id);
  
  cmd_ptr = (qmi_csvti_cmd_buf_type *)qmi_csvti_get_cmd_buf(QMI_CMD_CSVT_CMD_HDLR_CB);
  if( cmd_ptr == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    copy the framework message header to local struct
  -------------------------------------------------------------------------*/
  memset(cmd_ptr, 0, sizeof(qmi_csvti_cmd_buf_type));
  cmd_ptr->cmd_id = QMI_CMD_CSVT_CMD_HDLR_CB;
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
} /* qmi_csvti_cmd_hdlr_cb */

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_SVC_INIT()

  DESCRIPTION
    This function processes a init callback request and intialize the service
    related information.

  PARAMETERS
    num_instances:  number of QMI Instances

  RETURN VALUE
    None

  DEPENDENCIES
    QMI CSVT must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_svc_init
(
  uint16 num_instances
)
{
  qmi_csvti_state_type *         csvt_sp;
  qmi_idl_service_object_type    svc_obj;
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO2_0( "qmi_csvti init callback " );
  csvt_sp = &qmi_csvti_state;
  memset( csvt_sp, 0, sizeof(qmi_csvti_state_type) );

  svc_obj = csvt_get_service_object_v01();
  (void) qmi_si_register_object ( svc_obj,
                                    0, /* Service Instance */
                                    csvt_get_service_impl_v01() );

  /*-------------------------------------------------------------------------
    Max QMI Instances
  -------------------------------------------------------------------------*/
  csvt_sp->num_qmi_instances = num_instances;
  /*-------------------------------------------------------------------------
    initialize client state non-zero fields
  -------------------------------------------------------------------------*/
  if (FALSE == qmi_csvti_global.inited)
  {
    memset(&qmi_csvti_global, 0, sizeof(qmi_csvti_global));
    qmi_csvti_global.csvt_client_id = DSUCSD_INVALID_CLIENT_ID;
    qmi_csvti_global.inited = TRUE;
    qmi_csvti_global.csvt_token = 0x7fffffff;

    qmi_csvti_global.is_qmss_enabled = FALSE;
    qmi_csvti_read_nv_qmss_enabled(&(qmi_csvti_global.is_qmss_enabled));
    qmi_csvti_global.active_cs_subs = CSVTI_BIND_SUBS_PRIMARY;
  }
} /* qmi_csvti_process_svc_init */

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_ALLOC_CLID()

  DESCRIPTION
    This function processes the client alloc callback request. This function
    allocates and initialize the new client state for the allocated client ID.

  PARAMETERS
    common_msg_hdr:  QMI Framework common message header containing the clid
    alloc request information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI CSVT must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_alloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_csvti_client_state_type		   *  cl_sp;  
  qmi_result_e_type                result  = QMI_RESULT_FAILURE;
  qmi_error_e_type                 errval = QMI_ERR_NONE;
/*-------------------------------------------------------------------------*/

  ASSERT(common_msg_hdr && (common_msg_hdr->service == QMUX_SERVICE_CSVT));

  cl_sp = NULL;

  cl_sp = qmi_csvti_alloc_cl_sp(common_msg_hdr->client_id);

  if(cl_sp)
  {
    qmi_csvti_initialize_client(cl_sp);
    cl_sp->common.clid = common_msg_hdr->client_id;
    cl_sp->instance    = (uint16)common_msg_hdr->qmi_instance;
    
    LOG_MSG_INFO2_1( "qmi_csvti_clid set to local client state ptr: clid %d",
                     cl_sp->common.clid );
    result = QMI_RESULT_SUCCESS; 
    errval = QMI_ERR_NONE;
  }
  else
  {
    LOG_MSG_INFO1_0 ("No available service clids!" );
    errval = QMI_ERR_INTERNAL;
  }
  
  /* Notifying the clid allocation back to client via QMI Framework*/
  qmi_framework_svc_send_alloc_clid_result_ex(result, common_msg_hdr,
                                              errval);

} /* qmi_csvti_process_alloc_clid */

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_DEALLOC_CLID()

  DESCRIPTION
    This function processes the client dealloc callback request. This function
    resets and deallocates the new client state for the allocated client ID.

  PARAMETERS
    common_msg_hdr:  QMI Framework common message header containing the clid
    dealloc request information.
 
  RETURN VALUE
    None

  DEPENDENCIES
    QMI CSVT must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_dealloc_clid
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
)
{
  qmi_csvti_client_state_type		   *  cl_sp;   
  qmi_result_e_type                result  = QMI_RESULT_FAILURE; 
  qmi_error_e_type                 errval = QMI_ERR_NONE;
/*-------------------------------------------------------------------------*/

  ASSERT(common_msg_hdr);

  /*Check if the client id sent is valid or not, it could be a 
    stale command for which alloc was rejected*/
  if(!qmi_csvt_validate_client_id(common_msg_hdr->client_id, __LINE__))
  {
    return;
  }

  cl_sp = NULL;
  cl_sp = (qmi_csvti_client_state_type *) 
          qmi_csvti_state.client[common_msg_hdr->client_id  - 1];

  if (cl_sp != NULL && cl_sp->common.clid == common_msg_hdr->client_id)
  {
    qmi_csvti_reset_client(cl_sp);
    qmi_csvti_dealloc_cl_sp(common_msg_hdr->client_id);
    LOG_MSG_INFO2_1( "qmi_csvti_clid free to local client state ptr: clid%d",
                     common_msg_hdr->client_id );
    result = QMI_RESULT_SUCCESS;
  }
  else
  {
    errval = QMI_ERR_INVALID_CLIENT_ID;
    LOG_MSG_INFO1_0 ("Not a valid Client!" );
  }

  /* Notifying the clid deallocation back to client via QMI Framework*/
  qmi_framework_svc_send_dealloc_clid_result_ex(result, common_msg_hdr,
                                                errval);
} /* qmi_csvti_process_dealloc_clid */

/*===========================================================================
FUNCTION QMI_CSVTI_ALLOC_CL_SP()

  DESCRIPTION
  This function gets called whenever a client state need to be allocated
  given a valid client id. The client state has allocated and deallocated
  dynamically at runtime.
    
  PARAMETERS
  clid: client ID

  RETURN VALUE
  qmi_csvti_client_state_type * : ptr to QMI CSVT client state.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csvti_client_state_type * qmi_csvti_alloc_cl_sp
(
  uint8 clid
)
{
  qmi_csvti_client_state_type *  cl_sp = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(qmi_csvt_validate_client_id(clid,__LINE__))
  {
     
    PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(cl_sp, sizeof(qmi_csvti_client_state_type),
                                    qmi_csvti_client_state_type*);

      qmi_csvti_state.client[clid - 1] = cl_sp;
  }
  return cl_sp;
} /* qmi_csvti_alloc_cl_sp */

/*===========================================================================
FUNCTION QMI_CSVTI_DEALLOC_CL_SP()

DESCRIPTION
  This function gets called whenever a client state needs to be deallocated
  given a valid client id. The client state is allocated and deallocated
  dynamically at runtime.

PARAMETERS
    clid: client ID

RETURN VALUE
  TRUE: client Id deallocated.
  FALSE: client ID not deallocated. Error.
 
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean qmi_csvti_dealloc_cl_sp
(
  uint8 clid
)
{
  qmi_csvti_client_state_type *  cl_sp = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1( "qmi_csvti_dealloc_cl_sp clid :%d", clid );

  if(!qmi_csvt_validate_client_id(clid,__LINE__))
  {
    return FALSE;
  }

  cl_sp = (qmi_csvti_client_state_type *) qmi_csvti_state.client[clid - 1];
  if(cl_sp)
  {
    q_destroy(&cl_sp->common.x_free_q);
    q_destroy(&cl_sp->common.cmd_free_q);

    PS_SYSTEM_HEAP_MEM_FREE(cl_sp);
    qmi_csvti_state.client[clid - 1] = NULL;
  }
  else
  {
    LOG_MSG_ERROR_0("Client state pointer is already NULL");
    return FALSE;
  }

  return TRUE;
} /* qmi_csvti_dealloc_cl_sp */


/*===========================================================================
  FUNCTION QMI_CSVT_VALIDATE_CLIENT_ID()

  DESCRIPTION
    This function validates client id.
    
  PARAMETERS
    clid : client ID
    line_num : indicates line number of caller.

  RETURN VALUE
    boolean
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_csvt_validate_client_id
(
  uint32   clid,
  uint16   line_num
)
{
  if ((clid == QMI_SVC_CLID_UNUSED ) ||
	  (clid > CSVTI_MAX_CLIDS ))
  {
    LOG_MSG_INFO2_1( "Invalid client ID clid :%d", clid );
    return FALSE;
  }
  return TRUE;//client id is valid
} /* qmi_csvt_validate_client_id() */



/*===========================================================================
  FUNCTION QMI_CSVT_VALIDATE_CALL_INSTANCE()

  DESCRIPTION
    This function validates call instance.
    
  PARAMETERS
    clid : client ID

  RETURN VALUE
    boolean
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_csvt_validate_call_instance
(
  uint32   clid,
  uint32   inst_id,
  uint16   line_num
)
{
  if(0 == clid || inst_id >= MAX_UCSD_CALLS)
  {
    LOG_MSG_ERROR("clid or inst id is invalid clid= %d , inst id= %d line num = %d",
					  clid, inst_id,line_num);
    return FALSE;
  }
  return TRUE;//client id is valid
} /* qmi_csvt_validate_call_instance() */

/*===========================================================================
  FUNCTION QMI_CSVTI_INITIALIZE_CLIENT()

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
static void qmi_csvti_initialize_client
(
  qmi_csvti_client_state_type * cl_sp
)
{
  qmi_csvti_state_type *      csvt_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cl_sp);
  csvt_sp = &qmi_csvti_state;
  memset( cl_sp , 0, sizeof(qmi_csvti_client_state_type) );

  LOG_MSG_INFO2_0( "qmi_csvti_initialize_client " );
  cl_sp->instance     = CSVTI_INVALID_INSTANCE;
  cl_sp->common.clid  = QMI_SVC_CLID_UNUSED;
  
  ds_qmi_fw_common_cl_init(csvt_sp, &cl_sp->common);

    cl_sp->subscription_id = CSVTI_BIND_SUBS_PRIMARY;
} /* qmi_csvti_initialize_client() */
/*===========================================================================
  FUNCTION QMI_CSVTI_READ_NV_QMSS_ENABLED()

  DESCRIPTION
    Reads the NV item and retrieves the configured values.
    
  PARAMETERS
    is_qmss_enabled :  the address to which the NV value is stored
    
  RETURN VALUE
      TRUE: NV read is success
      FALSE: NV read is unsuccessful
      
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_csvti_read_nv_qmss_enabled
(
  uint8 * is_qmss_enabled
)
{
  #define QMSS_ENABLE_F_NAME \
                               (char*)"/nv/item_files/modem/mmode/qmss_enabled"
  int efs_ret = -1,fd;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  fd = mcfg_fopen( QMSS_ENABLE_F_NAME,
                   MCFG_FS_O_RDONLY, 
                   MCFG_FS_ALLPERMS,
                   file_type,
                   mcfg_subs_id);
  if(!(fd < 0))
  {
    efs_ret = mcfg_fread(fd, is_qmss_enabled,
                         sizeof(uint8), file_type);
  }
  if (efs_ret < 0)
  {
    return FALSE;
  }
  return TRUE;
}/* qmi_csvti_read_nv_qmss_enabled */

/*===========================================================================
  FUNCTION QMI_CSVTI_PROCESS_CMD_HDLR()

  DESCRIPTION
    This function processes the service command handler request callback.
    This function creates a transaction and dispatches to the appropriate
    message handler
    
  PARAMETERS
    msg_hdr:  Framework message header for the request.
    sdu_in : request

  RETURN VALUE
    None

  DEPENDENCIES
    QMI CSVT must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_csvti_process_cmd_hdlr
(
  qmi_framework_msg_hdr_type      * msg_hdr,
  dsm_item_type                   * sdu_in 
)
{
  qmi_csvti_client_state_type 	  * cl_sp;
  
/*-------------------------------------------------------------------------*/

  ASSERT(msg_hdr && sdu_in);

  if(!qmi_csvt_validate_client_id(msg_hdr->common_hdr.client_id,__LINE__))
  {
    return;
  }

  
  LOG_MSG_INFO2_3( "Process QMI CSVT svc command handlr callback function: clid%d, tx_id:%d, ctl_flag:%d ",
                   msg_hdr->common_hdr.client_id,
                   msg_hdr->common_hdr.transaction_id,
                   msg_hdr->msg_ctl_flag );

  /*-------------------------------------------------------------------------
    Get client state pointer
  -------------------------------------------------------------------------*/
  cl_sp = (qmi_csvti_client_state_type *) 
             qmi_csvti_state.client[msg_hdr->common_hdr.client_id - 1];
  

  if (cl_sp == NULL)
  {
    LOG_MSG_ERROR_0 ("Process QMI CSVT svc command handlr callback function: No valid client state!");
    ASSERT(cl_sp);
  }
  else
  {
    ds_qmi_fw_recv(&qmi_csvti_cfg, (void*)&qmi_csvti_state,
  	             msg_hdr, &cl_sp->common, sdu_in);
  }
  
} /* qmi_csvti_process_cmd_hdlr */

/*===========================================================================
FUNCTION QMI_CSVTI_SEND_RESPONSE()

DESCRIPTION
  This function calls QMI Framework API to send a QMI CSVT Service response
  message to clients.

PARAMETERS 
  cmd_buf_p: command buffer
  msg_ptr : dsm item

RETURN VALUE
  TRUE: If responses send to Framework
  FALSE: otherwise

DEPENDENCIES
  QMI CSVT service must be initialized and registered with Framework

SIDE EFFECTS
  None
===========================================================================*/
static boolean qmi_csvti_send_response
(
  qmi_cmd_buf_type *               cmd_buf_p,
  dsm_item_type *                  msg_ptr
)
{
  qmi_csvti_client_state_type *	       cl_sp;
  qmi_framework_common_msg_hdr_type    common_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if ( (cmd_buf_p == NULL) || (msg_ptr == NULL) ||
       (cmd_buf_p->x_p == NULL) || (cmd_buf_p->x_p->cl_sp == NULL) )
  {
    DATA_ERR_FATAL("Unable to send response to framework!");
    return FALSE;
  }
  
  cl_sp = (qmi_csvti_client_state_type *)cmd_buf_p->x_p->cl_sp;
  
  common_hdr.service        = QMUX_SERVICE_CSVT;
  common_hdr.client_id	    = cl_sp->common.clid;
  common_hdr.transaction_id = cmd_buf_p->x_p->x_id;
  common_hdr.qmi_instance   = cl_sp->instance;
  
  return ds_qmi_fw_send_response(&common_hdr, cmd_buf_p, msg_ptr);

} /* qmi_csvti_send_response */

/*===========================================================================
  FUNCTION QMI_CSVTI_SEND_IND()

  DESCRIPTION
    This function calls QMI Framework API to send a QMI CSVT Service
    indication message to clients.
    
  PARAMETERS
    cmd             : The csvt command for which the indication is sent
    ind             : The indication that will be sent
    client          : client id

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qmi_csvti_send_ind
(
  qmi_csvti_cmd_val_e_type                cmd,
  dsm_item_type                         * ind,
  uint8                                   client
)
{
  qmi_framework_common_msg_hdr_type      common_hdr;

  common_hdr.client_id = client;
  /* if unicast indication, qmi framework map the instance and no need to fill*/
  common_hdr.qmi_instance = CSVTI_INVALID_INSTANCE;
  common_hdr.service = QMUX_SERVICE_CSVT;
  /* for indication the transaction ID can be ignored */
  common_hdr.transaction_id = 0;
  
  if( FALSE == ds_qmi_fw_send_ind( &common_hdr, 
                                   cmd, 
                                   ind ) )
  {
    LOG_MSG_ERROR_0("Unable to send at event report indication!");
    dsm_free_packet(&ind);
  }
}/*qmi_csvti_send_ind*/

#endif /* (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) */
