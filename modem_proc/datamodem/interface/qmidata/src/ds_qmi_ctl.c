/*===========================================================================

                         D S _ Q M I _ C T L . C

DESCRIPTION

  The Data Services QMI control service source file.  This is the external
  API from the QMI control service which handles client ID management and
  versioning for other QMI services.

EXTERNALIZED FUNCTIONS

  qmi_ctl_init()
    Initialize the QMI control service
  
  qmi_ctl_get_instance_id()           
    Retrive the 16-bit instance ID of the specified QMI link based on the
    8-bit instance provided by the host driver and local 8-bit QMI link
    instance.  

Copyright (c) 2005 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_ctl.c#1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/21/14    sk     QMI-DMS Service QCSI migration.
09/27/13    wc     Support dynamic port configuration
06/13/13    ab     Fixed no response issue for Unrecognized messages 
06/11/13    ms     KW error fixes.
05/15/13    ab     Added QMI SVC available notification feature
05/10/12    ab     Added power save response reporting feature
09/11/12    gk     Made sure that rmenet set_data_format is not called on second 
                   instance when NO_EFFECT error is returned in first call
09/10/12    sb     Len checks for QMI TLVs
06/07/12    wc     Fix writing of wrong rmnet_config.txt
03/01/12    sa     Changes for Set/Get autoconnect and updating efs.
10/04/11    sa     Q6 free floating changes for QMI.
03/05/11    ua     Added optional QoS TLV in response to SET Data Format.
02/14/11    ua     Added support for DUAL IP Autoconnect. 
12/17/10    kk     Added log messages before ASSERTs.
06/19/10    vs     Changes for Dual IP support over single QMI instance
11/10/09    ar     Moved qmi_ctli_sync_ind from ctl_init to qmi_ctl_send_sync
10/23/09    ar     Add supprot for power save indication filtering
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
04/01/08    mct    Added support for IP mode.
10/20/07    ks     Added new TLV for giving Addendum version info.
10/09/07    ks     Updated minor verison to 1, after set_data_format msg
04/30/07    ks     Added message to set the data format used by the driver
08/23/06    ks     Changes to support multiple qmi/rmnet instances.
03/13/06    ks     Changed parsing of requests to return error for fixed 
                   length tlvs when tlv lengh passed is incorrect
03/13/06    ks     Changed QMI_ERR_INVALID_ARG to QMI_ERR_MALFORMED_MSG. 
                   Added reporting of new error code 
                   QMI_ERR_INVALID_SERVICE_TYPE.
05/31/05   jd/ks   Code review updates
05/17/05    jd     Make sure client ID callbacks are non-NULL before calling
03/14/05    ks     Removed featurization
03/08/05    ks     clean up
01/25/05    jd     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "msg.h"
#include "amssassert.h"

#include "dsm.h"
#include "ps_in.h"
#include "modem_mem.h"

#include "ds_qmux.h"
#include "ds_qmuxi.h"                                /* QMUX internal data */

#include "ds_qmi_svc.h"
#include "qmi_svc_defs.h"
#include "ds_qmi_svc_ext.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_if.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_rmnet_xport.h"
#include "ds_qmi_ctl.h"
#include "qmi_modem_task_svc.h"
#include "qmi_modem_task_cmd.h"
#include "ds_qmi_ctl_ext.h"
#include "qmi_frameworki.h"

#include "timer.h"

#include "cm.h"
#include "ds_qmi_nv_util.h"
#include "ds_qmi_task.h"
#include "ps_system_heap.h"

#include <stringl/stringl.h>

#ifdef TEST_FRAMEWORK
#error code not present
#endif
#include "ds_Utils_DebugMsg.h"

#include "ds_qmi_ctl_remote.h"
#include "dcc_task_svc.h"
#include "ps_utils.h"
/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Client ID for control service peer (QMUX layer on host)
---------------------------------------------------------------------------*/
#define QMI_CTL_CLIENT_ID  (0)

/*---------------------------------------------------------------------------
  QMI CTL BASE Version and ADDENDUM Version
  Addendum Major Ver 0 indicates NO Addedum to the base version
---------------------------------------------------------------------------*/

#define QMI_CTL_BASE_VER_MAJOR  (1)
#define QMI_CTL_BASE_VER_MINOR  (5)

#define QMI_CTL_ADDENDUM_VER_MAJOR  (0)
#define QMI_CTL_ADDENDUM_VER_MINOR  (0)

#define QMI_PRM_TYPE_ADDENDUM_VER_LIST (0x10)

/*---------------------------------------------------------------------------
  QMI header constants
  - QMI control flags differ from standard qmux service since QMI CTL 
    doesn't support compound messages.
---------------------------------------------------------------------------*/
#define QMI_CTL_FLAG_MASK_MSGTYPE  (0x03) 

/*---------------------------------------------------------------------------
  Message-internal TLV type field values
---------------------------------------------------------------------------*/
/* The new optional TLV to set the underlying link layer protocol mode. */
#define QMI_TYPE_LINK_PROTOCOL_MODE   (0x10)
#define QMI_TYPE_UL_TLP_SETTING       (0x11)
#define QMI_TYPE_QOS_SETTING          (0x12)

#define QMI_TYPE_PWRSAV_INDSET        (0x11)

#define QMI_TYPE_PWRSAV_INDSET_EXT    (0x10)

/* PWRSAVE_MODE_IND TLVs */
#define CTLI_TYPE_PWRSAVE_MODE_RPT    (0x10) 

#ifdef FEATURE_DATA_QMI_RESP_FILTERING
/*PWRSAVE_MODE RESPONSE REPORTING TLV*/
#define CTLI_TYPE_PWRSAVE_MODE_RESP_RPT (0x10)
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
/*QMI Service Available List TLV*/
#define CTLI_TYPE_SVC_AVAIL_LIST        (0x10) 
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

typedef enum
{
  QMI_CTL_FLAG_MSGTYPE_CMD  = 0x00,
  QMI_CTL_FLAG_MSGTYPE_RESP = 0x01,
  QMI_CTL_FLAG_MSGTYPE_IND  = 0x02
} qmi_ctl_msgtype_e_type;

/*---------------------------------------------------------------------------
  MACRO to check temp return var during message processing
---------------------------------------------------------------------------*/
#define FREE_AND_RETURN_ON_FAILURE()  if (temp == -1)               \
                                      { dsm_free_packet( &sdu_in ); \
                                        return; }

#define QMI_CTL_SYNC_IND_START_PERIOD         (1)
#define QMI_CTL_SYNC_IND_BACK_OFF_MULTIPLIER  (2)
#define QMI_CTL_SYNC_IND_MAX_BACK_OFF_PERIOD  (64)

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI control message header
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8  msg_type : 2;  /* LSbits */
  uint8  reserved : 6;
} qmi_ctli_flag_type;

typedef PACKED struct PACKED_POST
{
  qmi_ctli_flag_type  ctl_flags;
  uint8               transaction_id;
} qmi_ctli_hdr_type;

/*---------------------------------------------------------------------------
  QMI control indication type enum
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_CTL_REVOKE_CLIENT_ID_IND  = 0x0001,
  QMI_CTL_INVALID_CLIENT_ID_IND = 0x0002
} qmi_ctli_ind_e_type;

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
/*---------------------------------------------------------------------------
  QMUX service available list structures
---------------------------------------------------------------------------*/
typedef struct svc_avail_list_node
{
  uint8                        svc_id;
  struct svc_avail_list_node   *next;
} qmi_svc_avail_list_type;
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

/*---------------------------------------------------------------------------
  QMI control synchronization state type enum
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_CTL_SYNC_INIT_STATE                 = 1,
  QMI_CTL_SYNC_WAITING_FOR_REQUEST_STATE  = 2,
  QMI_CTL_SYNC_SYNCHRONIZED_STATE         = 3
} qmi_ctli_sync_state_e_type;

/*---------------------------------------------------------------------------
  QMI_CTL instance state structure
---------------------------------------------------------------------------*/
typedef struct
{
  void *   qmux_svc_handle;
  uint16   qmi_link_id;             /* QMI CTL instance = QMUX instance id */
  struct ctl_constants_s
  {
    qmi_instance_e_type        qmi_inst;
    qmi_instance_e_type        qmi_ctl_remote_instance;
  } constants;
  struct
  {
    qmi_ctli_sync_state_e_type state;
    timer_type                 timer;
    int                        back_off_period;
  } sync_state;
  struct 
  {                      
    boolean     report_pwrsave;
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
    boolean     report_svc_avail;
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
  } report_status;

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
  /*---------------------------------------------------------------------------
    QMUX service available list
  ---------------------------------------------------------------------------*/
  qmi_svc_avail_list_type *svc_avail_list;
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

} qmi_ctli_state_type;

/*---------------------------------------------------------------------------
  QMI control message command type enum

  DO NOT REORDER THIS TABLE!
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_CTL_CMD_MIN                = 0,
  QMI_CTL_CMD_SET_INSTANCE_ID    = 0,
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
  QMI_CTL_SET_EVENT_REPORT       = 1,
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
  QMI_CTL_CMD_GET_VERSION        = 2,
  QMI_CTL_CMD_GET_CLIENT_ID      = 3,
  QMI_CTL_CMD_RELEASE_CLIENT_ID  = 4,
  QMI_CTL_CMD_REVOKE_CLIENT_ID   = 5,
  QMI_CTL_CMD_INVALID_CLIENT_ID  = 6,
  QMI_CTL_CMD_SET_DATA_FORMAT    = 7,
  QMI_CTL_SYNC                   = 8,
  QMI_CTL_CMD_REG_PWRSAVE_MODE       = 9,
  QMI_CTL_CMD_CFG_PWRSAVE_MODE       = 10,
  QMI_CTL_CMD_SET_PWRSAVE_MODE       = 11,
  QMI_CTL_CMD_GET_PWRSAVE_MODE       = 12,
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
  QMI_CTL_CONFIGURE_RESPONSE_FILTERING_IN_PWR_SAVE   = 13,
  QMI_CTL_GET_RESPONSE_FILTERING_SETTING_IN_PWR_SAVE = 14,
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
  QMI_CTL_SET_SVC_AVAIL_LIST     = 15,
  QMI_CTL_GET_SVC_AVAIL_LIST     = 16,
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
  QMI_CTL_CMD_CFG_PWRSAVE_MODE_EXT    = 17,
  QMI_CTL_CMD_MAX,
  QMI_CTL_CMD_WIDTH              = 0xFFFF
} qmi_ctli_cmd_e_type;

typedef enum
{
  QMI_CTLI_CMD_VAL_SET_INSTANCE_ID    = 0x0020,
  QMI_CTLI_CMD_VAL_GET_VERSION        = 0x0021,
  QMI_CTLI_CMD_VAL_GET_CLIENT_ID      = 0x0022,
  QMI_CTLI_CMD_VAL_RELEASE_CLIENT_ID  = 0x0023,
  QMI_CTLI_CMD_VAL_REVOKE_CLIENT_ID   = 0x0024,
  QMI_CTLI_CMD_VAL_INVALID_CLIENT_ID  = 0x0025,
  QMI_CTLI_CMD_VAL_SET_DATA_FORMAT    = 0x0026,
  QMI_CTLI_CMD_VAL_SYNC               = 0x0027,
  QMI_CTLI_CMD_VAL_REG_PWRSAVE_MODE   = 0x0028,
  QMI_CTLI_CMD_VAL_CFG_PWRSAVE_MODE   = 0x0029,
  QMI_CTLI_CMD_VAL_SET_PWRSAVE_MODE   = 0x002A,
  QMI_CTLI_CMD_VAL_GET_PWRSAVE_MODE   = 0x002B,
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
  QMI_CTLI_CMD_VAL_CONFIGURE_RESPONSE_FILTERING_IN_PWR_SAVE_MODE   = 0x002C,
  QMI_CTLI_CMD_VAL_GET_RESPONSE_FILTERING_SETTING_IN_PWR_SAVE_MODE = 0x002D,
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
  QMI_CTLI_CMD_VAL_SET_SVC_AVAIL_LIST = 0x002E,
  QMI_CTLI_CMD_VAL_GET_SVC_AVAIL_LIST = 0x002F,
  QMI_CTLI_CMD_VAL_SET_EVENT_REPORT   = 0x0030,
  QMI_CTLI_CMD_VAL_SVC_AVAIL_LIST_IND = 0x0031,
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
  
  QMI_CTLI_CMD_VAL_CFG_PWRSAVE_MODE_EXT = 0x0032,
/* QC EXTERNAL QMI COMMAND RANGE IS 0x0000 - 0x5555. 
   Add the next external QMI Command here */

/* VENDOR SPECIFIC QMI COMMAND RANGE IS 0x5556 - 0xAAAA.
   IMPORTANT!
   Add the vendor specific QMI Commands within this range only to avoid 
   conflicts with QC QMI commands that would get released in future */

/* RESERVED QC QMI COMMAND RANGE IS 0xAAAB - 0xFFFE 
   Internal QMI Commands must be added in DECREASING ORDER from Below */
  QMI_CTLI_CMD_VAL_WIDTH              = 0xFFFF
} qmi_ctli_cmd_val_e_type;

/*---------------------------------------------------------------------------
  CTL Event report indication bitmask values
---------------------------------------------------------------------------*/
typedef enum
{
  CTLI_REPORT_STATUS_EV_PWRSAVE          = 0x00000001,
  CTLI_REPORT_WIDTH         = 0x7FFFFFFF  /* forces enum values to 32 bits */
} qmi_ctli_report_status_ev_e_type;

/*---------------------------------------------------------------------------
  QMI CTL command handler type
---------------------------------------------------------------------------*/
typedef dsm_item_type* (* qmi_ctli_hdlr_ftype)( void * sp,
                                                uint8  clid,
                                                uint8  tx_id,
                                                dsm_item_type ** sdu );

typedef struct
{
  uint16               cmd_type;
  char *               msg_desc;
  qmi_ctli_hdlr_ftype  request_hdlr;
} qmi_ctli_cmd_hdlr_type;

/* This is the default QMI_CTL command buffer. Make sure that it does not grow
 * beyond 512 bytes. In case you need a command buffer larger than 512 bytes,
 * declare a separate structure. */
typedef struct
{
  uint16  cmd_id; /* qmi_cmd_id_e_type */
  union
  {
    struct
    {
      void * qmux_svc_handle;
      uint8 qmux_type;
      uint8 clid;
      uint16 tx_id;
    } client_reg;

  } data;
} qmi_ctli_cmd_buf_type;

/*===========================================================================

                         INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI CTL service message handlers
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
static dsm_item_type*
         qmi_ctli_set_event_report( void*, uint8, uint8, dsm_item_type** );
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
static dsm_item_type*
         qmi_ctli_set_instance_id( void*, uint8, uint8, dsm_item_type** );
static dsm_item_type*
         qmi_ctli_get_version( void*, uint8, uint8, dsm_item_type**);
static dsm_item_type*
         qmi_ctli_get_client_id( void*, uint8, uint8, dsm_item_type** );
static dsm_item_type*
         qmi_ctli_release_client_id( void*, uint8, uint8, dsm_item_type** );
static dsm_item_type*
         qmi_ctli_set_data_format( void*, uint8, uint8, dsm_item_type** );
static dsm_item_type*
         qmi_ctli_sync( void*, uint8, uint8, dsm_item_type** );
static dsm_item_type*
         qmi_ctli_reg_pwrsave_mode( void*, uint8, uint8, dsm_item_type** );
static dsm_item_type*
         qmi_ctli_cfg_pwrsave_mode( void*, uint8, uint8, dsm_item_type** );
static dsm_item_type*
         qmi_ctli_set_pwrsave_mode( void*, uint8, uint8, dsm_item_type** );
static dsm_item_type*
         qmi_ctli_get_pwrsave_mode( void*, uint8, uint8, dsm_item_type** );
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
static dsm_item_type*
         qmi_ctli_configure_response_filtering_in_pwrsave_mode
                                  ( void*, uint8, uint8, dsm_item_type** );
static dsm_item_type*
         qmi_ctli_get_response_filtering_setting_in_pwrsave_mode
                                  ( void*, uint8, uint8, dsm_item_type** );
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
static dsm_item_type*
         qmi_ctli_set_svc_avail_list( void*, uint8, uint8, dsm_item_type** );
static dsm_item_type*
         qmi_ctli_get_svc_avail_list( void*, uint8, uint8, dsm_item_type** );
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
static dsm_item_type*
         qmi_ctli_cfg_pwrsave_mode_ext( void*, uint8, uint8, dsm_item_type** );

#define QMI_CTL_HDLR(a,b)  QMI_SVC_HDLR( a, (qmi_ctli_hdlr_ftype) b )

static qmi_ctli_cmd_hdlr_type  qmi_ctl_cmd_callbacks[QMI_CTL_CMD_MAX] =
{
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY 
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_SET_EVENT_REPORT,  qmi_ctli_set_event_report),
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_SET_INSTANCE_ID,   qmi_ctli_set_instance_id  ),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_GET_VERSION,       qmi_ctli_get_version      ),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_GET_CLIENT_ID,     qmi_ctli_get_client_id    ),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_RELEASE_CLIENT_ID, qmi_ctli_release_client_id),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_REVOKE_CLIENT_ID,  NULL),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_INVALID_CLIENT_ID, NULL),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_SET_DATA_FORMAT, qmi_ctli_set_data_format),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_SYNC, qmi_ctli_sync),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_REG_PWRSAVE_MODE,  qmi_ctli_reg_pwrsave_mode),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_CFG_PWRSAVE_MODE,  qmi_ctli_cfg_pwrsave_mode),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_SET_PWRSAVE_MODE,  qmi_ctli_set_pwrsave_mode),
  QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_GET_PWRSAVE_MODE,  qmi_ctli_get_pwrsave_mode)
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
  ,QMI_CTL_HDLR(
     QMI_CTLI_CMD_VAL_CONFIGURE_RESPONSE_FILTERING_IN_PWR_SAVE_MODE,
     qmi_ctli_configure_response_filtering_in_pwrsave_mode)
  ,QMI_CTL_HDLR(
     QMI_CTLI_CMD_VAL_GET_RESPONSE_FILTERING_SETTING_IN_PWR_SAVE_MODE, 
     qmi_ctli_get_response_filtering_setting_in_pwrsave_mode)
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
  ,QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_SET_SVC_AVAIL_LIST,  qmi_ctli_set_svc_avail_list)
  ,QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_GET_SVC_AVAIL_LIST,  qmi_ctli_get_svc_avail_list)
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
  ,QMI_CTL_HDLR(QMI_CTLI_CMD_VAL_CFG_PWRSAVE_MODE_EXT,  qmi_ctli_cfg_pwrsave_mode_ext),
};

/*---------------------------------------------------------------------------
  QMI_CTL instance state structure
---------------------------------------------------------------------------*/
static qmi_ctli_state_type  qmi_ctl_state[QMI_INSTANCE_MAX];

/*---------------------------------------------------------------------------
  QMI_CTL instance configurations
  - Version (Major, Minor)
  - Callbacks (Alloc clid, Is_valid clid, dealloc clid, link_down, recv)
  - User data parameter to be passed back when callback functions are
    called
---------------------------------------------------------------------------*/

static boolean qmi_ctli_is_valid_client_id(void *  sp, uint8 clid);
static void    qmi_ctli_qmi_link_closed_cb(void *  sp);
static void    qmi_ctli_recv(void * sp, uint8 clid, dsm_item_type * sdu_in);

static qmux_svc_config_type  qmi_ctl_cfg[QMI_INSTANCE_MAX] = {{{0},}};

#define QMI_ADDENDUM_LABEL "NILL"
#define MAX_QMI_ADDENDUM_LABEL_LEN 255

#ifdef FEATURE_DATA_QMI_RESP_FILTERING
#define MAX_QMI_PWRSAVE_STATE_LEN  255

typedef PACKED struct PACKED_POST
{
  uint8  n_pwrsave_state;

  PACKED struct PACKED_POST
  {
    qmi_svc_pwr_state_handle_type pwrsave_state_handle;
    boolean                       response_reporting;
  } reporting_tuple[MAX_QMI_PWRSAVE_STATE_LEN];
} response_reporting_type;
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/
static struct
{
  boolean     inited;
  int         num_qmi_instances;
  boolean     sync_resets_state;
} qmi_ctl_global = {0, };

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
typedef PACKED struct PACKED_POST
{
  uint8  num_svcs;

  PACKED struct PACKED_POST
  {
    uint8     svc_id;
  } svc_tuple[QMUX_SERVICE_MAX];
} svc_list_type;
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

/*===========================================================================

                            FORWARD DECLARATIONS

===========================================================================*/
static boolean qmi_ctli_send
(
  void *           sp,
  uint8            msg_type,
  uint8            trans_id,
  dsm_item_type *  msg
);

static void qmi_ctli_sync_timer_cback
(
  timer_cb_data_type  user_data
);

void qmi_ctli_sync_ind
(
  void *  sp
);

static qmi_error_e_type qmi_ctli_sync_oprt_mode_online
(
  void
);

static void qmi_ctli_process_cmd
(
  void * cmd_ptr
);

static void *qmi_ctli_get_cmd_buf
(
  qmi_cmd_id_e_type cmd_id
);

#define qmi_ctli_free_cmd_buf(buf_ptr) PS_SYSTEM_HEAP_MEM_FREE(buf_ptr)

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
static boolean ctli_send_svc_avail_ind
(
  void *                        sp,
  uint8                         svc_id
);

static boolean ctli_store_service_available_list
(
   void                         * service_handle,
   uint8                          num_svcs,
   void                         * in_svc_list
);

static boolean ctli_get_available_service_list
(
  void                         * service_handle,
  void                         * svc_list,
  uint8                        * num_svcs
);
static boolean ctli_report_service_availability
(
  void                         * sp,
  qmux_service_e_type            service_id
);
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

/*===========================================================================

                        EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CTL_INIT()

  DESCRIPTION
    Initialize all instances (one for each physical link supporting QMUX)
    of the QMI_CTL service

  PARAMETERS
    QMI Instance

  RETURN VALUE
    None

  DEPENDENCIES
    Must be called at startup

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_ctl_init
(
  qmi_instance_e_type  qmi_inst
)
{
  qmi_ctli_state_type *  ctl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (QMI_INSTANCE_MAX <= qmi_inst)
  {
    LOG_MSG_ERROR_1("Recd invalid inst_id '%d' in qmi_ctl_init",
                    (int32)qmi_inst);
    ASSERT(0);
    return;
  }

  ctl_sp = &qmi_ctl_state[qmi_inst];
  
  qmi_ctl_cfg[qmi_inst].base_version.major = QMI_CTL_BASE_VER_MAJOR;
  qmi_ctl_cfg[qmi_inst].base_version.minor = QMI_CTL_BASE_VER_MINOR;
  
  qmi_ctl_cfg[qmi_inst].addendum_version.major = QMI_CTL_ADDENDUM_VER_MAJOR;
  qmi_ctl_cfg[qmi_inst].addendum_version.minor = QMI_CTL_ADDENDUM_VER_MINOR;
  
  qmi_ctl_cfg[qmi_inst].cbs.alloc_clid         = NULL;
  qmi_ctl_cfg[qmi_inst].cbs.is_valid_clid      = qmi_ctli_is_valid_client_id;
  qmi_ctl_cfg[qmi_inst].cbs.dealloc_clid       = NULL;
  qmi_ctl_cfg[qmi_inst].cbs.qmux_closed        = qmi_ctli_qmi_link_closed_cb;
  qmi_ctl_cfg[qmi_inst].cbs.recv               = qmi_ctli_recv;
  qmi_ctl_cfg[qmi_inst].cbs.get_next_client_sp = NULL;
  qmi_ctl_cfg[qmi_inst].cbs.reset_client       = NULL;
  
  qmi_ctl_cfg[qmi_inst].cmd_hdlr_array = NULL;
  qmi_ctl_cfg[qmi_inst].cmd_num_entries = 0;
  qmi_ctl_cfg[qmi_inst].sp = &qmi_ctl_state[qmi_inst];


  /*-----------------------------------------------------------------------
    Initialize each instance's state, then reg with QMUX
    - Set local link ID (byte 1) to the QMI link instance ID
  -----------------------------------------------------------------------*/
  memset( ctl_sp, 0, sizeof(qmi_ctli_state_type) );
  ctl_sp->qmi_link_id = (qmi_inst & 0xFF) << 8;
  ctl_sp->qmux_svc_handle = qmux_reg_service(qmi_inst,
                                             QMUX_SERVICE_CTL,
                                             &qmi_ctl_cfg[qmi_inst]);
  ctl_sp->constants.qmi_inst = qmi_inst;

  ctl_sp->constants.qmi_ctl_remote_instance = qmi_ctl_remote_init(qmi_inst);

  ctl_sp->sync_state.state = QMI_CTL_SYNC_INIT_STATE;

  timer_def( &ctl_sp->sync_state.timer,
               NULL,
               rex_self(),
               0,
               qmi_ctli_sync_timer_cback,
             (timer_cb_data_type)qmi_inst );
    
  if( FALSE == qmi_ctl_global.inited )
  {    
    qmi_ctl_global.num_qmi_instances = 0;

    if (QMI_NV_STATUS_OK != qmi_nv_read(QMI_NV_ITEM_SYNC_RESET_MODEM_STATE,
                                        0,
                                        0,
                                        &qmi_ctl_global.sync_resets_state,
                                      sizeof(qmi_ctl_global.sync_resets_state)))
    {
      qmi_ctl_global.sync_resets_state = FALSE;
    }
    qmi_ctl_global.inited = TRUE;
  }

  qmi_task_set_svc_cmd_handler(QMUX_SERVICE_CTL,qmi_ctli_process_cmd);

  qmi_ctl_global.num_qmi_instances++;
} /* qmi_ctl_init() */



/*===========================================================================
  FUNCTION QMI_CTLI_PROCESS_CMD

  DESCRIPTION
    This function processes a QMI CTL command or event.

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI CTL command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI CTL must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_ctli_process_cmd
(
  void *cmd_ptr
)
{
  qmi_ctli_cmd_buf_type *cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);
  cmd_buf_ptr = (qmi_ctli_cmd_buf_type *)cmd_ptr;

  switch(cmd_buf_ptr->cmd_id)
  {
    case QMI_CMD_LEGACY_SVC_ALLOC_CLID:
      qmi_legacy_svc_alloc_clid(cmd_buf_ptr->data.client_reg.qmux_svc_handle,
                                cmd_buf_ptr->data.client_reg.qmux_type,
                                cmd_buf_ptr->data.client_reg.clid,
                                (uint8)cmd_buf_ptr->data.client_reg.tx_id);
      break;
    case QMI_CMD_LEGACY_SVC_DEALLOC_CLID:
      qmi_legacy_svc_dealloc_clid(cmd_buf_ptr->data.client_reg.qmux_svc_handle,
                                  cmd_buf_ptr->data.client_reg.qmux_type,
                                  cmd_buf_ptr->data.client_reg.clid,
                                  (uint8)cmd_buf_ptr->data.client_reg.tx_id);
      break;
  }/* switch(cmd_buf_ptr->cmd_id) */

  qmi_ctli_free_cmd_buf(cmd_buf_ptr);
  return;
} /* qmi_ctli_process_cmd() */

/*===========================================================================
  FUNCTION QMI_CTLI_GET_CMD_BUF

  DESCRIPTION
    Allocate and assign a QMI CTL command buffer from the PS MEM heap
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
static void *qmi_ctli_get_cmd_buf
(
  qmi_cmd_id_e_type cmd_id
)
{
  LOG_MSG_INFO1_1("ctli_get_cmd_buf cmd_id = %d",cmd_id);
  switch (cmd_id)
  {
    case QMI_CMD_LEGACY_SVC_ALLOC_CLID:
    case QMI_CMD_LEGACY_SVC_DEALLOC_CLID:
    {
      qmi_ctli_cmd_buf_type *cmd_buf_ptr = NULL;
      PS_SYSTEM_HEAP_MEM_ALLOC(cmd_buf_ptr, sizeof(qmi_ctli_cmd_buf_type),
                               qmi_ctli_cmd_buf_type*);
      return ((void*)cmd_buf_ptr);
    }

    default:
	break;
  }
  return NULL;
} /* qmi_ctli_get_cmd_buf */

/*===========================================================================
  FUNCTION QMI_CTL_GET_INSTANCE_ID()

  DESCRIPTION
    Retrive the 16-bit instance ID of the specified QMI link based on the
    8-bit instance provided by the host driver and local 8-bit QMI link
    instance.

  PARAMETERS
    qmi_instance    - the QMI instance whose QMI instance ID is being queried
    out_instance_id - set to qmi link ID if successful, unchanged otherwise

  RETURN VALUE
    TRUE  -  instance ID returned in out_instance_id
    FALSE -  invalid QMI instance specified or
             QMI instance ID has not been set

  DEPENDENCIES
    qmi_init must have been called at startup

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_ctl_get_instance_id
(
  qmi_instance_e_type  qmi_instance,
  uint16 *             out_instance_id
)
{
  qmi_ctli_state_type *   ctl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == out_instance_id)
  {
    DATA_ERR_FATAL("Recd NULL ptr in qmi_ctl_get_instance_id");
    return FALSE;
  }

  if (QMI_INSTANCE_MAX <= qmi_instance)
  {
    LOG_MSG_ERROR_1("Recd invalid inst_id '%d' in qmi_ctl_get_instance_id",
                    (int32)qmi_instance);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    QMI link instance is always equal to QMI_CTL service instance, since
    there must always be a QMI_CTL on every QMUX link.
  -------------------------------------------------------------------------*/
  ctl_sp = &qmi_ctl_state[qmi_instance];

  *out_instance_id = ctl_sp->qmi_link_id;
  return TRUE;

} /* qmi_ctl_get_instance_id() */

/*===========================================================================
  FUNCTION QMI_CTL_PROCESS_CLIENT_REGISTRATION_RESULT()

  DESCRIPTION
    To report if client registration succeeded or failed.

  PARAMETERS
    result            : Client ID allocation result
    client_reg_result : Client reg result.
    qmi_cmd           : reg/dereg.

  RETURN VALUE
    None
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
void qmi_ctl_process_client_registration_result
(
  qmi_result_e_type                 reg_result,
  qmi_ctl_client_reg_result_type *  client_reg_result,
  uint16                            qmi_cmd,
  qmi_error_e_type                  errval
)
{
  dsm_item_type *         response_ptr;
  qmi_result_e_type       result;
  qmi_ctli_cmd_val_e_type msg_type;

  struct
  {
    uint8  qmux_type;
    uint8  clid;
  } v_out_required;
/*-------------------------------------------------------------------------*/

  response_ptr = NULL;
  result = QMI_RESULT_SUCCESS;

  if((qmi_cmd_id_e_type)qmi_cmd == QMI_CMD_GET_CLIENT_ID_RESP )
  {
    msg_type = QMI_CTLI_CMD_VAL_GET_CLIENT_ID;
  }
  else if((qmi_cmd_id_e_type)qmi_cmd == QMI_CMD_RELEASE_CLIENT_ID_RESP)
  {
    msg_type = QMI_CTLI_CMD_VAL_RELEASE_CLIENT_ID;
  }
  else
  {
    LOG_MSG_ERROR_1 ("Invalid QCTL cmd:%d ", QMI_CTLI_CMD_VAL_GET_CLIENT_ID);
    return;
  }

  if(reg_result != QMI_RESULT_SUCCESS)
  {
    goto send_result;
  }

  v_out_required.qmux_type = client_reg_result->service_id;
  v_out_required.clid      = client_reg_result->cl_id;

  if( FALSE == qmi_svc_put_param_tlv(&response_ptr,
                                     QMI_TYPE_REQUIRED_PARAMETERS,
                                     sizeof (v_out_required),
                                     &v_out_required ) )
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response_ptr);
    goto send_result;
  }
  
  LOG_MSG_INFO2_2 ("Sending QMI_CTL Client %d for service %d",
                   client_reg_result->cl_id, client_reg_result->service_id);

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                         : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response_ptr, result, errval) )
  {
    dsm_free_packet(&response_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    stick the QMI_CTL cmd/response_ptr header in front.
    response_ptr cmd id must be the same as request.
  -------------------------------------------------------------------------*/
  if( FALSE == qmi_svc_prepend_msg_hdr( &response_ptr, msg_type) )
  {
    /*-----------------------------------------------------------------------
      Note - no need to free response_ptr, done in prepend_msg_hdr()
    -----------------------------------------------------------------------*/
    return;
  }

  if(client_reg_result->inst_id >= QMI_INSTANCE_MAX)
  {
    dsm_free_packet(&response_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Now send the QMI_CTL PDU
  -------------------------------------------------------------------------*/
  if(FALSE == qmi_ctli_send( &qmi_ctl_state[client_reg_result->inst_id],
                 QMI_CTL_FLAG_MSGTYPE_RESP,
                 (uint8)client_reg_result->tx_id,
                 response_ptr ))
  {
    LOG_MSG_INFO2_0 ("Unable to send a response!");
  }

  return;
} /* qmi_ctl_process_client_registration_result() */

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
/*===========================================================================
  FUNCTION QMI_CTL_PROCESS_SERVICE_AVAILABILITY

  DESCRIPTION
    To report if a service needs to be notified to control point
    for its availability

  PARAMETERS
    svc_id            : QMI Service ID
    qmi_inst          : QMI Instance
    is_legacy_svc     : TRUE if QMI Service based on legacy QMUX, 
                        FALSE otherwise

  RETURN VALUE
    None
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
void qmi_ctl_process_service_availability
(
  qmux_service_e_type               svc_id,
  qmi_instance_e_type               qmi_inst,
  boolean                           is_legacy_svc
)
{
  qmi_ctli_state_type *         ctl_sp;
  uint8                         index = 0;
  boolean                       report_svc_avail = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( QMUX_SERVICE_MAX > svc_id );

  LOG_MSG_INFO3_0 ("qmi_ctl_notify_service_ready");

  ctl_sp = NULL;

  if(!is_legacy_svc)
  {
    /* Ignore QMI INST parameter for new qmux services as the service registration
       with qmux is independent of ports. Send the indication to registered
       clients on all the ports.*/
    for(index= QMI_INSTANCE_MIN; index < QMI_INSTANCE_MAX; index++)
    {
      ctl_sp = &qmi_ctl_state[index];

      report_svc_avail = ctli_send_svc_avail_ind(ctl_sp, svc_id);

      if (!report_svc_avail)
      {
        LOG_MSG_INFO1_1("No Indication sent for service [%d]", svc_id);
      }
    }
  }

  /*Legacy services register per port, so find the the right client state*/
  else
  {
    ctl_sp = &qmi_ctl_state[qmi_inst];

    report_svc_avail = ctli_send_svc_avail_ind(ctl_sp, svc_id);

    if (!report_svc_avail)
    {
      LOG_MSG_INFO1_1("No Indication sent for service [%d]", svc_id);
      return;
    }
  }
} /* qmi_ctl_process_service_availability() */
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

/*===========================================================================

                        INTERNAL FUNCTION DEFINTIONS

===========================================================================*/
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
/*===========================================================================
  FUNCTION QMI_CTLI_SEND_SVC_AVAIL_IND()

  DESCRIPTION
    Function to notify about service availability to registered control
    points. And send indication if the client is registered to recieve
    indication and if the particular QMI service is available in
    service available list.

  PARAMETERS
    sp:           QMI_CTL's service instance state pointer for this qmi link
    svc_id:       QMI Service ID

  RETURN VALUE
    TRUE:   Client ID is valid (assigned)
    FALSE:  Client ID is invalid (no assigned)

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static boolean  ctli_send_svc_avail_ind
(
  void *                        sp,
  uint8                         svc_id
)
{
  boolean                       report_svc_avail;
  dsm_item_type *               ind;

  qmi_ctli_state_type *             ctl_sp = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify control point registered for indication; skip otherwise.
  -------------------------------------------------------------------------*/
  ASSERT(sp);
  ASSERT( QMUX_SERVICE_MAX > svc_id );

  ctl_sp = (qmi_ctli_state_type *)  sp;

  if((ctl_sp != NULL) && (ctl_sp->report_status.report_svc_avail))
  {
    LOG_MSG_INFO2_0 ("Client registered for service availability "
                     "notification!");

    /*Check if service availability needs to be reported to control point*/
    report_svc_avail = ctli_report_service_availability(ctl_sp, svc_id);

    if (!report_svc_avail)
    {
      LOG_MSG_INFO1_1("Service [%d] not found in svc_avail_list.", svc_id);
      return FALSE;
    }
    else
    {
      /*-------------------------------------------------------------------------
        Build indication from the back to the front
      -------------------------------------------------------------------------*/
      ind = NULL;

      LOG_MSG_INFO2_1( "Indicate QMI Service [%d] Ready on instance: [%d]",
                       svc_id );

      if( FALSE == qmi_svc_prepend_msg_hdr
                   (&ind,QMI_CTLI_CMD_VAL_SVC_AVAIL_LIST_IND) )
      {
        dsm_free_packet(&ind);
        return FALSE;
      }

      /*-------------------------------------------------------------------------
        Send indication to client
      -------------------------------------------------------------------------*/
      if( FALSE == qmi_ctli_send( ctl_sp,
                                  QMI_CTL_FLAG_MSGTYPE_IND,
                                  0, //transaction id
                                  ind ) )
      {
        LOG_MSG_ERROR_0 ("Unable to generate Event Report IND");
        return FALSE;
      }

      return TRUE;
    }
  }
  return FALSE;
} /* qmi_ctli_send_svc_avail_ind() */
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

/*===========================================================================
  FUNCTION QMI_CTLI_IS_VALID_CLID()

  DESCRIPTION
    Check if passed client id is valid for the QMUX CTL instance identified
    by passed qmux service state pointer (returned by registerign qmi ctl
    service).

  PARAMETERS
    sp:    QMI_CTL's service instance state pointer for this qmi link
           returned by qmux_reg_service()
    clid:  Client ID to validate

  RETURN VALUE
    TRUE:   Client ID is valid (assigned)
    FALSE:  Client ID is invalid (no assigned)

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static boolean  qmi_ctli_is_valid_client_id
(
  void *  user_data,
  uint8   clid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( clid == QMI_CTL_CLIENT_ID )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* qmi_ctli_is_valid_client_id() */

/*===========================================================================
  FUNCTION QMI_CTLI_QMI_CLOSED_CB()

  DESCRIPTION
    Callback called by QMUX layer if the link is administratively closed
    (e.g. physical cable is pulled).  QMUX CTL service cleans up any client
    state that may be assigned.

  PARAMETERS
    sp:    QMI_CTL's service instance state pointer for this qmi link
           returned by qmux_reg_service()

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static void  qmi_ctli_qmi_link_closed_cb
(
  void *  sp
)
{
  qmi_instance_e_type    qmi_ctl_instance;
  qmi_ctli_state_type *  ctl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    DATA_ERR_FATAL("Can't perform close on NULL svc ptr");
    return;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  /*-------------------------------------------------------------------------
    Clean up QMI CTL state
    - Set local link ID to the QMI link instance ID
  -------------------------------------------------------------------------*/
  qmi_ctl_instance = (qmi_instance_e_type) (ctl_sp - qmi_ctl_state);
  ctl_sp->qmi_link_id = (qmi_ctl_instance & 0xFF) << 8;
  LOG_MSG_INFO2_0 ("QMI link closed, reset QMI link ID");

} /* qmi_ctli_qmi_link_closed_cb() */

/*===========================================================================
  FUNCTION QMI_CTLI_INPUT()

  DESCRIPTION
    Called by ctl_recv function to process input command (request) messages

  PARAMETERS
    sp:              QMI_CTL's service instance state pointer for this qmi
                     link returned by qmux_reg_service()
    ctl_flags:       control flags from QMI PDU
    transaction_id:  transaction ID for this message
    qmi_ctl_cmd:     input command type
    sdu_len:         expected data length
    sdu_in:          input command data

  RETURN VALUE
    TRUE:   Message was valid and
    FALSE:  Client ID is invalid (no assigned)

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static int qmi_ctli_input
(
  void *                   sp,
  uint8                    ctl_flags,
  uint8                    transaction_id,
  qmi_ctli_cmd_val_e_type  qmi_ctl_cmd,
  uint16                   sdu_len,
  dsm_item_type **         sdu_in
)
{
  dsm_item_type *          response_ptr = NULL;
  qmi_ctli_cmd_hdlr_type *  cmd_hdlr;
  qmi_ctli_hdlr_ftype       hdlr;
  qmi_ctli_cmd_e_type      cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    goto send_result;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, dropping message");
    goto send_result;
  }

  LOG_MSG_INFO2_2 ("Got QMI_CTL command type %d (%d bytes)",
                   qmi_ctl_cmd, sdu_len);

  /*-------------------------------------------------------------------------
    Lookup QMI CTL command handler.  If not present, ignore the command.
  -------------------------------------------------------------------------*/
  cmd_hdlr = qmi_ctl_cmd_callbacks;
  for (cmd = QMI_CTL_CMD_MIN; cmd < QMI_CTL_CMD_MAX; cmd++, cmd_hdlr++)
  {
    if (qmi_ctl_cmd == cmd_hdlr->cmd_type)
    {
      break;
    }
  }

  if( cmd == QMI_CTL_CMD_MAX )
  {
    LOG_MSG_INFO2_1 ("Unrecognized msg! (%d) ignoring", qmi_ctl_cmd);

    if( FALSE == qmi_svc_put_result_tlv
             (&response_ptr, QMI_RESULT_FAILURE, QMI_ERR_INVALID_QMI_CMD) )
    {
      LOG_MSG_INFO2_1 ("Unable to send response for qmi_ctl cmd %d",
                       qmi_ctl_cmd);
      dsm_free_packet(&response_ptr);
      return FALSE;
    }
    goto send_result;
  }

  hdlr = NULL;
  switch (ctl_flags & QMI_CTL_FLAG_MASK_MSGTYPE)
  {
    case QMI_CTL_FLAG_MSGTYPE_CMD:
      hdlr = cmd_hdlr->request_hdlr;
      break;

    default:
      LOG_MSG_INFO1_1("Invalid msg type! (%d) ignoring",
                      ctl_flags & QMI_CTL_FLAG_MASK_MSGTYPE);
      return FALSE;
  }

  /*-------------------------------------------------------------------------
    client id is always 0 for qmi_ctl (else fails above)
  -------------------------------------------------------------------------*/
  if (hdlr)
  {
    MSG_SPRINTF_1( MSG_SSID_DS, MSG_LEGACY_MED,
                   "Handling %s", cmd_hdlr->msg_desc);
    response_ptr = hdlr( sp, QMI_CTL_CLIENT_ID, transaction_id, sdu_in );
  }
  else
  {
    LOG_MSG_INFO2_1 ("No action for msg (%d), ignoring", qmi_ctl_cmd);
    response_ptr = NULL;
  }

send_result:
  /*-------------------------------------------------------------------------
    Send response if ready.  Check for need to queue this transaction_id
  -------------------------------------------------------------------------*/
  if (response_ptr == NULL)
  {
    LOG_MSG_INFO2_0 ("(CTL msg generated no response)");
    return FALSE;
  }
  else if (response_ptr == QMI_SVC_RESPONSE_PENDING)
  {
    LOG_MSG_INFO2_0 ("(CTL response is pending)");
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    stick the QMI_CTL cmd/response header in front.
    response cmd id must be the same as request.
  -------------------------------------------------------------------------*/
  if( FALSE == qmi_svc_prepend_msg_hdr( &response_ptr, qmi_ctl_cmd) )
  {
    /*-----------------------------------------------------------------------
      Note - no need to free response_ptr, done in prepend_msg_hdr()
    -----------------------------------------------------------------------*/
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Now send the QMI_CTL PDU
  -------------------------------------------------------------------------*/
  return qmi_ctli_send( sp,
                        QMI_CTL_FLAG_MSGTYPE_RESP,
                        transaction_id,
                        response_ptr );

} /* qmi_ctli_input() */

/*===========================================================================
  FUNCTION QMI_CTLI_RECV()

  DESCRIPTION
    Called by the QMUX layer to process incoming QMUX CTL PDU.
    Processes QMI_CTL PDU header and dispatches command (SDU).

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static void  qmi_ctli_recv
(
  void *           sp,
  uint8            clid,
  dsm_item_type *  sdu_in
)
{
  int32                 temp;
  uint8                 ctl;
  uint8                 transaction_id;
  uint16                sdu_type;
  uint16                sdu_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if (NULL != sdu_in)
    {
      dsm_free_packet(&sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if (NULL != sdu_in)
    {
      dsm_free_packet(&sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return;
  }

  /*-------------------------------------------------------------------------
    Remove QMI_CTL header:

      field                          length
    +----------------------------+
    | QMI CTL control flags      |    1
    +----------------------------+
    | QMI CTL transaction ID     |    1
    +----------------------------+
    | QMI CTL command number     |    2
    +----------------------------+
    | QMI CTL message length     |    2
    +----------------------------+
        ...
    +----------------------------+
    | QMI CTL message body       |    length bytes
    +----------------------------+
  -------------------------------------------------------------------------*/
  temp = dsm_pull8( &sdu_in );
  FREE_AND_RETURN_ON_FAILURE();
  ctl = (uint8) temp;
  temp = dsm_pull8( &sdu_in );
  FREE_AND_RETURN_ON_FAILURE();
  transaction_id = (uint8) temp;
  temp = dsm_pull16( &sdu_in );
  FREE_AND_RETURN_ON_FAILURE();
  sdu_type = ps_ntohs(temp);
  temp = dsm_pull16( &sdu_in );
  FREE_AND_RETURN_ON_FAILURE();
  sdu_len = ps_ntohs(temp);

  /*-------------------------------------------------------------------------
    Check that the payload length is the same as listed in the header.
    No bundling of commands is allowed
  -------------------------------------------------------------------------*/
  if (sdu_len != dsm_length_packet(sdu_in))
  {
    LOG_MSG_INFO2_2( "Got short QMI_CTL message! (expected=0x%X, got=0x%X bytes",
                     sdu_len, dsm_length_packet(sdu_in) );
    dsm_free_packet( &sdu_in );
    return;
  }

  /*-------------------------------------------------------------------------
    Parse the incoming QMI_CTL message.  No bundling is allowed
  -------------------------------------------------------------------------*/
  (void) qmi_ctli_input( sp,
                         ctl,
                         transaction_id,
                         (qmi_ctli_cmd_val_e_type)sdu_type,
                         sdu_len,
                         &sdu_in );
  dsm_free_packet (&sdu_in);

} /* qmi_ctli_recv() */

/*===========================================================================
  FUNCTION QMI_CTLI_SEND()

  DESCRIPTION
    Called to send the provided QMI CTL response or indication.

  PARAMETERS
    sp:              QMI_CTL's service instance state pointer for the qmi
                     link over which this message is to be sent
    msg_type:        command type to be sent
    transaction_id:  Transaction ID for the message
    msg:             Message data to be sent

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_ctli_send
(
  void *           sp,
  uint8            msg_type,
  uint8            transaction_id,
  dsm_item_type *  msg
)
{
  qmi_ctli_state_type *  ctl_sp;
  qmi_ctli_hdr_type      hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if (NULL != msg)
    {
      dsm_free_packet(&msg);
    }
    DATA_ERR_FATAL("Unable to send pkt on NULL svc ptr");
    return FALSE;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  /*-------------------------------------------------------------------------
    Prepend QMI CTL PDU header and send message over QMUX link
  -------------------------------------------------------------------------*/
  hdr.ctl_flags.msg_type = (msg_type & QMI_CTL_FLAG_MASK_MSGTYPE);
  hdr.ctl_flags.reserved = 0;
  hdr.transaction_id     = transaction_id;

  if( sizeof(hdr) != PS_UTILS_DSM_PUSHDOWN_PACKED( &msg,
                                          (void *) &hdr,
                                          sizeof (hdr),
                                     DSM_DS_SMALL_ITEM_POOL))
  {
    dsm_free_packet (&msg);
    return FALSE;
  }

  LOG_MSG_INFO2_2 ("Send QMI_CTL type %d (%d bytes)",
                   msg_type, dsm_length_packet(msg));

  qmux_send( ctl_sp->qmux_svc_handle, msg, QMI_CTL_CLIENT_ID );

  return TRUE;

} /* qmi_ctli_send() */

/*===========================================================================
  FUNCTION QMI_CTLI_SET_INSTANCE_ID()

  DESCRIPTION
    Handle the QMI CTL set instance ID message.  Prepends the host instance
    ID contained in the input message with the local QMI link instance, and
    saves the 16-bit result in the QMI CTL instance state.  Generates a
    reply containing this 16-bit value to be sent back to the host.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_set_instance_id
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type *  ctl_sp;
  uint8                  type;
  uint16                 len;
  uint16                 expected_len;
  void *                 value;
  
  PACKED struct PACKED_POST
  {
    uint8  instance;
  } v_in_required = { 0, };
  boolean                got_v_in_required;
  
  PACKED struct PACKED_POST {
    uint16  qmi_link_id;
  } v_out_required;
  
  dsm_item_type *    response;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  memset(&v_in_required, 0, sizeof(v_in_required));

  ctl_sp = (qmi_ctli_state_type *) sp;

  /*-------------------------------------------------------------------------
    Parse out expected TLVs, toss unexpected.  If duplicates, last wins.
  -------------------------------------------------------------------------*/
  errval = QMI_ERR_NONE;
  got_v_in_required = FALSE;
  response = NULL;

  while (*sdu_in)
  {
    if( !qmi_svc_get_param_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
      {
        expected_len = sizeof(v_in_required);
        value = &v_in_required;
        got_v_in_required = TRUE;
        break;
      }
      
      default:
      {
        LOG_MSG_INFO2_1 ("Unrecognized TLV type (%d)", type);
        break;
      }
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_2 ("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if( len != PS_UTILS_DSM_PULLUP( sdu_in, value, len))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  LOG_MSG_INFO2_1 ("Got QMI_CTL set instance command (instance = %d)",
                   v_in_required.instance);

  /*-------------------------------------------------------------------------
    Ensure required TLV parameters are present
  -------------------------------------------------------------------------*/
  if (!got_v_in_required)
  {
    LOG_MSG_INFO2_0 ("Missing required parameters TLV!");
    errval = QMI_ERR_MISSING_ARG;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Perform action associated with message -
      Concatenate the provided host link ID with the local link ID to form
      the unique QMI link identifier.
    -----------------------------------------------------------------------*/
    ctl_sp->qmi_link_id &= ~(0xFF);
    ctl_sp->qmi_link_id |= v_in_required.instance;
    v_out_required.qmi_link_id = ctl_sp->qmi_link_id;

    if( FALSE == qmi_svc_put_param_tlv(&response,
                                       QMI_TYPE_REQUIRED_PARAMETERS,
                                       sizeof (v_out_required),
                                       &v_out_required))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet( &response );
    }
  }

send_result:
  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS : QMI_RESULT_FAILURE;
  if(FALSE == qmi_svc_put_result_tlv(&response, result, errval))
  {
    dsm_free_packet( &response );
    return NULL;
  }
  return response;
} /* qmi_ctli_set_instance_id() */

/*===========================================================================
  FUNCTION QMI_CTLI_GET_VERSION()

  DESCRIPTION
    Handle the QMI CTL get version message.

    Generates a reply containing the QMI CTL service's major and minor
    version numbers.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_get_version
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type *   ctl_sp;
  qmux_svc_config_type *  svc_cfg;
  qmi_framework_svc_config_type *  framework_svc_cfg;
  qmux_version_type       svc_base_version;
  qmux_version_type       svc_addendum_version;
  qmi_error_e_type        errval;
  qmi_result_e_type       result;
  dsm_item_type *         response;
  
  typedef PACKED struct PACKED_POST
  {
    uint8  n_services;
    struct
    {
      qmux_service_e_type  svc_type;
      qmux_version_type    base_version;
    } tuple[QMUX_SERVICE_MAX];
  } v_out_reqd_type;

  v_out_reqd_type * v_out_reqd = NULL;
  byte * pout;
  qmux_service_e_type  svc;
 
  typedef PACKED struct PACKED_POST
  {
    uint8  n_services;
    struct
    {
      qmux_service_e_type  svc_type;
      qmux_version_type    base_version;
    } tuple[QMUX_SERVICE_MAX];
  } v_out_addendum_opt_type;

  v_out_addendum_opt_type * v_out_addendum_opt = NULL;
  byte * addendum_opt_out;
  char * addendum_label;
  uint16  label_len;
  uint16 addendum_svc_ver_info_len;
  uint16 total_bytes_pushed;
  uint8   addendum_tlv_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

  /*-------------------------------------------------------------------------
    Perform validity checks (none)
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Allocate memory
  -------------------------------------------------------------------------*/
   
  PS_SYSTEM_HEAP_MEM_ALLOC(v_out_reqd, sizeof(v_out_reqd_type),
                           v_out_reqd_type*);

  if (v_out_reqd == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }
  memset(v_out_reqd, 0, sizeof(v_out_reqd_type));

  PS_SYSTEM_HEAP_MEM_ALLOC(v_out_addendum_opt, sizeof(v_out_addendum_opt_type),
                           v_out_addendum_opt_type*);

  if (v_out_addendum_opt == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }
  memset(v_out_addendum_opt, 0, sizeof(v_out_addendum_opt_type));

  /*-------------------------------------------------------------------------
    Perform action/gather required information for response
    - retrieve the version numbers from the service configuration block
    If no error so far, build response payload from the back to the front
  -------------------------------------------------------------------------*/
  pout = (byte *) v_out_reqd;
  v_out_reqd->n_services = 0;
  pout++; //leave 1st location for num_services

  addendum_opt_out = (byte *) v_out_addendum_opt;
  v_out_addendum_opt->n_services = 0;
  addendum_opt_out++; //leave 1st location for num_services

  // process QC services
  for (svc = QMUX_SERVICE_MIN; svc < QMUX_SERVICE_MAX; svc++)
  {
    svc_cfg = qmux_get_service_cfg( ctl_sp->qmux_svc_handle, svc );
    if (!svc_cfg)
    {
      framework_svc_cfg = qmi_framework_get_service_cfg(svc);
      if (!framework_svc_cfg)
      {
        // skip this service - not registered on this qmux link e.g. RRDM
        continue;
      }
      svc_base_version.major = framework_svc_cfg->base_version.major; 
      svc_base_version.minor = framework_svc_cfg->base_version.minor;
      svc_addendum_version.major = framework_svc_cfg->addendum_version.major;  
      svc_addendum_version.minor = framework_svc_cfg->addendum_version.minor;  
    }
    else
    {
      svc_base_version.major = svc_cfg->base_version.major; 
      svc_base_version.minor = svc_cfg->base_version.minor;
      svc_addendum_version.major = svc_cfg->addendum_version.major;  
      svc_addendum_version.minor = svc_cfg->addendum_version.minor;  
    }
    v_out_reqd->n_services++;
    *pout++ = svc;
    memscpy( pout, 
             sizeof(qmux_version_type), 
             &svc_base_version, 
             sizeof(qmux_version_type) );
    pout += sizeof(qmux_version_type);

    /* addendum ver info */
    if(svc_addendum_version.major == 0)
    {
      /* NO addendum ver to report for this service */
      continue;
    }

    v_out_addendum_opt->n_services++;
    *addendum_opt_out++ = svc;
    memscpy( addendum_opt_out, 
             sizeof(qmux_version_type), 
             &svc_addendum_version, 
             sizeof(qmux_version_type) );
    addendum_opt_out += sizeof(qmux_version_type);
  }
	
  /*------------------------------------------------------------------------- 
    send addendum ver TLV only if there is any addendum ver 
    to report for any service
  -------------------------------------------------------------------------*/
  if(v_out_addendum_opt->n_services > 0)
  {
    addendum_label = QMI_ADDENDUM_LABEL;
    label_len = strlen(addendum_label);
    label_len = MIN(MAX_QMI_ADDENDUM_LABEL_LEN, label_len);

    addendum_svc_ver_info_len = (uint16) (addendum_opt_out - (byte*) v_out_addendum_opt);

    total_bytes_pushed = addendum_svc_ver_info_len + label_len + sizeof(uint8);
    addendum_tlv_type = QMI_PRM_TYPE_ADDENDUM_VER_LIST;

    if(! (QMI_SVC_PKT_PUSH(&response, v_out_addendum_opt, addendum_svc_ver_info_len) &&
          QMI_SVC_PKT_PUSH(&response, addendum_label, label_len) &&
          QMI_SVC_PKT_PUSH(&response, &label_len, sizeof(uint8)) &&
          QMI_SVC_PKT_PUSH(&response, &total_bytes_pushed, sizeof(uint16)) &&
          QMI_SVC_PKT_PUSH(&response, &addendum_tlv_type, sizeof(uint8))))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

  if( FALSE == qmi_svc_put_param_tlv(&response,
                                     QMI_TYPE_REQUIRED_PARAMETERS,
                                     (uint16) (pout - (byte*) v_out_reqd),
                                     v_out_reqd) )
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
  }

  /*-------------------------------------------------------------------------
    Prepend response result and return to input function which will
    finalize the response command header and send it back via QMUX
  -------------------------------------------------------------------------*/
send_result:
  PS_SYSTEM_HEAP_MEM_FREE(v_out_reqd);
  PS_SYSTEM_HEAP_MEM_FREE(v_out_addendum_opt);
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(FALSE == qmi_svc_put_result_tlv(&response, result, errval))
  {
    dsm_free_packet(&response);
    return NULL;
  }

  return response;

} /* qmi_ctli_get_version() */



/*===========================================================================
  FUNCTION QMI_CTLI_GET_CLIENT_ID()

  DESCRIPTION
    Handle the QMI CTL get client ID message.

    Dispatches a request to the specified service to allocate a client ID.
    Returns the allocated client ID in the response to the issuer.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_get_client_id
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type *   ctl_sp;
  uint8                   type;
  uint16                  len;
  uint16                  expected_len;
  void *                  value;
  PACKED struct PACKED_POST
  {
    uint8  qmux_type;
  } v_in_required = { 0, };
  boolean                 got_v_in_reqd = TRUE;
  qmux_svc_config_type *  svc_cfg;
  dsm_item_type *         response;
  qmi_result_e_type       result;
  qmi_error_e_type        errval;
  qmi_error_e_type        retval = QMI_ERR_NONE;
  qmi_framework_common_msg_hdr_type common_msg_hdr;
  qmi_ctli_cmd_buf_type  * cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  memset(&v_in_required, 0, sizeof(v_in_required));

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

  /*-------------------------------------------------------------------------
    Parse incoming command
    - identify and save expected TLVs,
      if duplicate TLV found, last present is used.
    - discard unexpected TLVs
  -------------------------------------------------------------------------*/
  got_v_in_reqd = FALSE;
  while (*sdu_in)
  {
    if( !qmi_svc_get_param_tl( sdu_in, &type, &len ) )
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
        got_v_in_reqd = TRUE;
        value = &v_in_required;
        break;

      default:
        LOG_MSG_INFO2_1 ("Unexpected TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_2 ("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if( len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  /*-------------------------------------------------------------------------
    Perform validity checks (none)
  -------------------------------------------------------------------------*/
  if (!got_v_in_reqd)
  {
    LOG_MSG_INFO2_0 ("Mandatory TLV not present!");
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Perform action/gather required information for response
    - get the service configuration block of the specified QMI service
    - allocate a client ID from the specified QMI service
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_1 ("Got QMI_CTL get CLID command (svc = %d)",
                   v_in_required.qmux_type);

  if( (qmux_service_e_type)v_in_required.qmux_type == QMUX_SERVICE_CTL)
  {
    errval = QMI_ERR_INVALID_SERVICE_TYPE;
    goto send_result;
  }

  svc_cfg = qmux_get_service_cfg( ctl_sp->qmux_svc_handle,
                                  (qmux_service_e_type)v_in_required.qmux_type );

  if(svc_cfg == NULL)
  {
    LOG_MSG_INFO2_0 ("QMI_CTL get CLID - Service based on new framework");
    memset(&common_msg_hdr, 0, sizeof(common_msg_hdr));

    common_msg_hdr.service = 
                    (qmux_service_e_type)v_in_required.qmux_type;
    common_msg_hdr.qmi_instance = 
                    qmi_instance_by_qmux_handle(ctl_sp->qmux_svc_handle);
    common_msg_hdr.transaction_id = tx_id;

    retval = qmi_framework_svc_alloc_clid(&common_msg_hdr);
    if(retval != QMI_ERR_NONE)
    {
      errval = retval;
      goto send_result;
    }
    LOG_MSG_INFO2_2 ("QMI_CTL get CLID pending response svc: %d, tx_id: %d",
                     common_msg_hdr.service, common_msg_hdr.transaction_id);
    return QMI_SVC_RESPONSE_PENDING;
  }
  
  cmd_ptr = (qmi_ctli_cmd_buf_type *)qmi_ctli_get_cmd_buf(QMI_CMD_LEGACY_SVC_ALLOC_CLID);

  if(cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0 ("Unable to allocate QMI CTL cmd buf");
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  cmd_ptr->cmd_id = QMI_CMD_LEGACY_SVC_ALLOC_CLID;
  cmd_ptr->data.client_reg.qmux_svc_handle = ctl_sp->qmux_svc_handle;
  cmd_ptr->data.client_reg.qmux_type = v_in_required.qmux_type;
  cmd_ptr->data.client_reg.tx_id = tx_id;  

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;
  
  /*-------------------------------------------------------------------------
    Prepend response result and return to input function which will
    finalize the response command header and send it back via QMUX
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
} /* qmi_ctli_get_client_id() */



/*===========================================================================
  FUNCTION QMI_CTLI_RELEASE_CLIENT_ID()

  DESCRIPTION
    Handle the QMI CTL release client ID message.

    Dispatches a request to the specified service to deallocate a client ID.
    Returns the result of the operation to the issuer.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_release_client_id
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type *  ctl_sp;
  uint8                  type;
  uint16                 len;
  uint16                 expected_len;
  void *                 value;
  PACKED struct PACKED_POST {
    uint8  qmux_type;
    uint8  clid;
  } v_io_required;
  boolean                got_v_io_required;
  dsm_item_type *        response;
  qmi_error_e_type       errval;
  qmi_result_e_type      result;
  qmux_svc_config_type * svc_cfg;
  qmi_error_e_type       retval = QMI_ERR_NONE;
  qmi_framework_common_msg_hdr_type common_msg_hdr;
  qmi_ctli_cmd_buf_type * cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    LOG_MSG_ERROR_0("Recd NULL svc ptr, dropping message");
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    LOG_MSG_ERROR_0("Recd pkt from invalid client, dropping message");
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    LOG_MSG_ERROR_0("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  memset(&v_io_required, 0, sizeof(v_io_required));

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

  /*-------------------------------------------------------------------------
    Parse incoming command (none)
    - identify and save expected TLVs,
      if duplicate TLV found, last present is used.
    - discard unexpected TLVs
  -------------------------------------------------------------------------*/
  got_v_io_required = FALSE;
  while (*sdu_in)
  {
    if( !qmi_svc_get_param_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(v_io_required);
        got_v_io_required = TRUE;
        value = &v_io_required;
        break;

      default:
        LOG_MSG_INFO2_1 ("Unrecognized TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_2 ("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if( len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  /*-------------------------------------------------------------------------
    Perform validity checks (none)
  -------------------------------------------------------------------------*/
  if (!got_v_io_required)
  {
    LOG_MSG_INFO2_0 ("Mandatory TLV not present!");
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Perform action/gather required information for response
    - get the service configuration block of the specified QMI service
    - deallocate the client ID from the specified QMI service
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_2 ("Got QMI_CTL release CLID command (svc = %d, clid = %d)",
                   v_io_required.qmux_type, v_io_required.clid);

  svc_cfg = qmux_get_service_cfg( ctl_sp->qmux_svc_handle,
                                    (qmux_service_e_type)v_io_required.qmux_type );

  if(svc_cfg == NULL)
  {
    memset(&common_msg_hdr, 0, sizeof(common_msg_hdr));

    common_msg_hdr.service        = 
                        (qmux_service_e_type)v_io_required.qmux_type;
    common_msg_hdr.client_id      = v_io_required.clid;
    common_msg_hdr.qmi_instance   = 
                        qmi_instance_by_qmux_handle(ctl_sp->qmux_svc_handle);
    common_msg_hdr.transaction_id = tx_id;

    retval = qmi_framework_svc_dealloc_clid(&common_msg_hdr);
    if(retval != QMI_ERR_NONE)
    {
      errval = retval;
      goto send_result;
    }
    LOG_MSG_INFO2_2 ("QMI_CTL release CLID pending response svc: %d, tx_id: %d",
                     common_msg_hdr.service, common_msg_hdr.transaction_id);
    return QMI_SVC_RESPONSE_PENDING;
  }

  // qctl itself doesn't allow dealloc client
  if ((qmux_service_e_type)v_io_required.qmux_type == QMUX_SERVICE_CTL)
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  cmd_ptr = (qmi_ctli_cmd_buf_type *)qmi_ctli_get_cmd_buf(QMI_CMD_LEGACY_SVC_DEALLOC_CLID);
  if(cmd_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  cmd_ptr->cmd_id = QMI_CMD_LEGACY_SVC_DEALLOC_CLID;
  cmd_ptr->data.client_reg.qmux_svc_handle = ctl_sp->qmux_svc_handle;
  cmd_ptr->data.client_reg.qmux_type = v_io_required.qmux_type;
  cmd_ptr->data.client_reg.clid = v_io_required.clid;  
  cmd_ptr->data.client_reg.tx_id = tx_id;  

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

  /*-------------------------------------------------------------------------
    Prepend response result and return to input function which will
    finalize the response command header and send it back via QMUX
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if(FALSE == qmi_svc_put_result_tlv(&response, result, errval))
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
} /* qmi_ctli_release_client_id() */


/*===========================================================================
  FUNCTION QMI_CTLI_SET_DATA_FORMAT()

  DESCRIPTION
    Handle the QMI CTL get client ID message.

    Dispatches a request to the specified service to allocate a client ID.
    Returns the allocated client ID in the response to the issuer.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_set_data_format
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type *   ctl_sp;
  uint8                   type;
  uint16                  len;
  uint16                  expected_len;
  void *                  value;
  dsm_item_type *         response;
  qmi_result_e_type       result;
  qmi_error_e_type        errval;
  rmnet_data_format_type  data_format;
  uint32                  data_format_mask = 0;
  uint16                  link_prot = 0;
  uint32                  ep_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;
  memset((void *)&data_format, 0, sizeof(rmnet_data_format_type));

  /*-------------------------------------------------------------------------
    Parse incoming command
    - identify and save expected TLVs,
      if duplicate TLV found, last present is used.
    - discard unexpected TLVs
  -------------------------------------------------------------------------*/
  while (*sdu_in)
  {
    if( !qmi_svc_get_param_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(uint8);
        data_format_mask |= RMNET_DATA_FORMAT_MASK_QOS;
        value = (void *)&data_format.qos;
        break;

      case QMI_TYPE_LINK_PROTOCOL_MODE:
        expected_len = sizeof(uint16);
        data_format_mask |= RMNET_DATA_FORMAT_MASK_LINK_PROT;
        value = (void *)&link_prot;
        break;

      case QMI_TYPE_UL_TLP_SETTING:
        expected_len = sizeof(boolean);
        data_format_mask |= RMNET_DATA_FORMAT_MASK_UL_DATA_AGG;
        value = (void *)&data_format.ul_data_agg_protocol;
        break;

      default:
        LOG_MSG_INFO2_1 ("Unexpected TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_2 ("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if( len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  } /* end while */

  /*-------------------------------------------------------------------------
    Perform validity checks (none)
  -------------------------------------------------------------------------*/
  if (!(data_format_mask & RMNET_DATA_FORMAT_MASK_QOS))
  {
    LOG_MSG_INFO2_0 ("Mandatory TLV not present!");
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Perform validity checks for Link Protocol Mode
  -------------------------------------------------------------------------*/
  if( (data_format_mask & RMNET_DATA_FORMAT_MASK_LINK_PROT) && 
      ((link_prot == ((uint16) RMNET_INVALID_MODE)) ||
       (link_prot & ((uint16) ~RMNET_ALL_MODE))))
  {
    LOG_MSG_INFO2_1 ("Invalid link protocol(s) specified: 0x%x!", link_prot);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
  data_format.link_prot = (rmnet_sm_link_prot_e_type)link_prot;

   /*-------------------------------------------------------------------------
    Perform action/gather required information for response
    - get the service configuration block of the specified QMI service
    - allocate a client ID from the specified QMI service
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_3 ("Got QMI_CTL set data format:Qos[%d] Link Prot[%d] UL TLP [%d]",
                   data_format.qos,
                   data_format.link_prot,
                   data_format.ul_data_agg_protocol);

  if (ctl_sp->constants.qmi_inst >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_INFO2_0 ("Got QMI_CTL set data format on reverse IP port, just return ");
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  ep_id = qmux_get_default_ep_id_from_inst(ctl_sp->constants.qmi_inst);
  if (RMNET_SET_DATA_FORMAT_QMI_WDA == rmnet_phys_xport_data_format_client(ep_id))
  {
    LOG_MSG_ERROR_0("Data format already set by WDA");
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  if (rmnet_phys_xport_set_data_format(ep_id,
                                       &data_format,
                                       data_format_mask,
                                       RMNET_SET_DATA_FORMAT_QMI_CTL) == FALSE)
  {
    errval = QMI_ERR_INVALID_DATA_FORMAT;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Set the port formats on the corresponding remote port
   -------------------------------------------------------------------------*/
  link_prot = data_format.link_prot;
  qmi_ctl_remote_set_data_format (ctl_sp->constants.qmi_ctl_remote_instance,
                                link_prot,
                                data_format.qos,
                                data_format.ul_data_agg_protocol);

  /*-------------------------------------------------------------------------
    Attach the link protocol TLV response
  -------------------------------------------------------------------------*/
  if (data_format_mask & RMNET_DATA_FORMAT_MASK_UL_DATA_AGG) 
  {
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      QMI_TYPE_UL_TLP_SETTING,
                                      sizeof (uint8),
                                      (void *)&data_format.ul_data_agg_protocol))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

  if(data_format_mask & RMNET_DATA_FORMAT_MASK_LINK_PROT)
  {
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      QMI_TYPE_LINK_PROTOCOL_MODE,
                                      sizeof (uint16),
                                      (void *)&link_prot))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }
  /*-------------------------------------------------------------------------
    Attach the QoS TLV response
  -------------------------------------------------------------------------*/

    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      QMI_TYPE_QOS_SETTING,
                                      sizeof (uint8),
                                      (void *)&data_format.qos))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }


  /*-------------------------------------------------------------------------
    build response
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
} /* qmi_ctli_set_data_format() */

/*===========================================================================
  FUNCTION QMI_CTLI_SYNC()

  DESCRIPTION
    Handle the QMI CTL synchronize request from the client.

    Returns message that is used to notify client that synchronization 
    is complete.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_sync
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type *   ctl_sp;
  dsm_item_type *         response;
  qmi_result_e_type       result;
  qmi_error_e_type        errval;
  qmi_instance_e_type     qmi_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

  LOG_MSG_INFO2_0 ("Received QMI_CTL_SYNC_REQ, sending QMI_CTL_SYNC_RESP");

  // change state to synchronized
  ctl_sp->sync_state.state = QMI_CTL_SYNC_SYNCHRONIZED_STATE;
  (void) timer_clr( &ctl_sp->sync_state.timer, T_NONE );

  // clear the instance's info
  qmi_instance = qmi_instance_by_qmux_handle(ctl_sp->qmux_svc_handle);
  qmux_clean_instance_resources( qmi_instance );

  if (qmi_ctl_global.sync_resets_state)
  {
    errval = qmi_ctli_sync_oprt_mode_online();
  }

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
} /* qmi_ctli_sync() */

/*===========================================================================
  FUNCTION QMI_CTLI_SYNC_IND()

  DESCRIPTION
    Called when initialization is complete and when synchronization timer
    expires.
    Causes QMI_CTL_SYNC_IND to be sent.
    
  PARAMETERS
    sp:                QMI_CTL's service instance state pointer for this qmi 
                       link returned by qmux_reg_service()
    transaction_id:    transaction ID for this message

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_ctli_sync_ind
(
  void *  sp
)
{
  dsm_item_type *               ind;

  if (NULL == sp)
  {
    DATA_ERR_FATAL("Can't perform sync on NULL svc ptr");
    return;
  }

  LOG_MSG_INFO2_0 ("Sending QMI_CTL_SYNC indication");

  ind = NULL;

  if( FALSE == qmi_svc_prepend_msg_hdr(&ind, QMI_CTLI_CMD_VAL_SYNC) )
  {
    dsm_free_packet(&ind);
    return;
  }

  /*-------------------------------------------------------------------------
    Now send the QMI_CTL PDU
  -------------------------------------------------------------------------*/
  if( FALSE == qmi_ctli_send( sp,
                              QMI_CTL_FLAG_MSGTYPE_IND,
                              0, //transaction id
                              ind ) )
  {
    LOG_MSG_INFO2_0 ("Can't send QMI_CTL_SYNC indication!");
  }

  return ;
} /* qmi_ctli_sync_ind() */

/*===========================================================================
  FUNCTION QMI_CTL_SEND_SYNC()

  DESCRIPTION
    Called during QMI device port open processing.
    
  PARAMETERS
    qmi_inst          - the QMI instance to operate upon

  RETURN VALUE
    None
    
  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_ctl_send_sync
(
  qmi_instance_e_type  qmi_inst
)
{
  qmi_ctli_state_type  *ctl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1("qmi_ctl_send_sync: %d", qmi_inst);

  if( FALSE == qmi_ctl_global.inited )
  {
    DATA_ERR_FATAL("Port init attempted before QMI init completed");
    return;
  }

  ctl_sp = &qmi_ctl_state[ qmi_inst ];
  ctl_sp->sync_state.back_off_period = QMI_CTL_SYNC_IND_START_PERIOD;
  ctl_sp->sync_state.state = QMI_CTL_SYNC_WAITING_FOR_REQUEST_STATE;

  /*-------------------------------------------------------------------------
    Generate SYNC indicatcation and set timer for retransmission.
  -------------------------------------------------------------------------*/
  qmi_ctli_sync_ind(ctl_sp);

  timer_set( &ctl_sp->sync_state.timer,
             ctl_sp->sync_state.back_off_period,
	       0,
	       T_SEC );

  return;
} /* qmi_ctl_send_sync() */

/*===========================================================================
  FUNCTION QMI_CTLI_SYNC_TIMER_CBACK()

  DESCRIPTION
    This is the callback function which is called when the synchronization
    timer expires. It generates QMI_CTL_SYNC_IND indication.    

  PARAMETERS
    none

  RETURN VALUE
    None
        
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_ctli_sync_timer_cback
(
  timer_cb_data_type  user_data
)
{
  qmi_ctli_state_type   *ctl_sp;
  qmi_instance_e_type    qmi_inst;
  qmi_cmd_data_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qmi_inst = (qmi_instance_e_type)user_data;
  ctl_sp = &qmi_ctl_state[qmi_inst];

  cmd_ptr = qmi_get_cmd_data_buf();
  if( cmd_ptr != NULL )
  {
    cmd_ptr->cmd.qmi.id = QMI_CMD_CTL_SEND_SYNC_IND;
    cmd_ptr->cmd.qmi.data.ctl.sp = ctl_sp;

    qmi_send_cmd( QMI_CMD, cmd_ptr );
  }
  else
  {
    LOG_MSG_ERROR_1( "Could not get cmd buffer from QMI task for cmd id %d",
                     QMI_CMD_CTL_SEND_SYNC_IND );
    ASSERT(0);
  }
} /* qmi_ctli_sync_timer_cback() */


/*===========================================================================
  FUNCTION QMI_CTLI_REG_PWRSAVE_MODE()

  DESCRIPTION
    Handle the QMI CTL register for power save state change.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_ctli_reg_pwrsave_mode
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *               response;
  qmi_ctli_state_type *         ctl_sp;
  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;
  boolean            got_v_in_reqd;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;

  PACKED struct PACKED_POST
  {
    boolean report_pwrsave;
  } v_in1 = { 0, };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  response = NULL;
  errval = QMI_ERR_NONE;
  got_v_in_reqd  = FALSE;

  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, dropping message");
    return NULL;
  }

  LOG_MSG_INFO3_0 ("qmi_ctli_reg_pwrsave_mode");
  
  ctl_sp = (qmi_ctli_state_type *) sp;
  
  memset(&v_in1, 0x0, sizeof(v_in1));

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    /* special value 0 = variable length or don't care (unrecognzied TLV) */
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(v_in1);
        got_v_in_reqd = TRUE;
        value = (void *) &v_in1;
        break;

      default:
        LOG_MSG_INFO2_1 ("Unrecognized TLV type (%d)", type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_CTL,__LINE__) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      if value is NULL, skip past the current TLV
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len  ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  /*-------------------------------------------------------------------------
    Perform validity checks
  -------------------------------------------------------------------------*/
  if (!got_v_in_reqd)
  {
    LOG_MSG_INFO2_0 ("Mandatory TLV not present!");
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Perform action/gather required information for response
    - Store power save reporting flag
  -------------------------------------------------------------------------*/
  ctl_sp->report_status.report_pwrsave =
    ( v_in1.report_pwrsave )? TRUE : FALSE;
  

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);
  
  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
    
  return response;

} /* qmi_ctli_reg_pwrsave_mode() */

/*===========================================================================
  FUNCTION QMI_CTLI_PWRSAVE_MODE_IND()

  DESCRIPTION
    Is called when power save mode changes. Sends an indication to the
    client.

  PARAMETERS
    sp               : service provided state pointer (user data)
    clid             : clientID
    event_mask       : mask to indicate what caused the indication to be called

  RETURN VALUE
    void
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_ctli_pwrsave_mode_ind
(
  void *                                sp,
  uint8                                 clid,
  uint32                                event_mask,
  void *                                prev_state_handle
)
{
  dsm_item_type *               ind;
  qmi_ctli_state_type *         ctl_sp;

  PACKED struct PACKED_POST
  {
    qmi_svc_pwr_state_handle_type curr_state_handle;
    qmi_svc_pwr_state_handle_type prev_state_handle;
  } v_out1;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_0 ("qmi_ctli_pwrsave_mode_ind");

  ctl_sp = (qmi_ctli_state_type *) sp;
  if( NULL == ctl_sp ) 
  {
    DATA_ERR_FATAL("Null ctl_sp in ctl_evt_rpt_ind");
    return;
  }
  
  /*-------------------------------------------------------------------------
    Verify control point registered for indication; abort otherwise.
  -------------------------------------------------------------------------*/
  if( !ctl_sp->report_status.report_pwrsave )
  {
    LOG_MSG_INFO2_0 ("Client not registered for power save indication!");
    return;
  }
  
  /*-------------------------------------------------------------------------
    Build indication from the back to the front
  -------------------------------------------------------------------------*/
  ind = NULL;

  /* Query current power save state */
  if( !qmi_svc_get_pwrsave_state( ctl_sp->qmux_svc_handle,
                                  &v_out1.curr_state_handle ) )
  {
    LOG_MSG_INFO2_0 ("Failed to query power save state!");
    return;
  }
  v_out1.prev_state_handle = prev_state_handle;
    
  LOG_MSG_INFO2_3( "Indicate power save state change: inst=%d curr=0x%x, prev=0x%x",
                   (ctl_sp-qmi_ctl_state),
                   v_out1.curr_state_handle,
                   v_out1.prev_state_handle );

  if(FALSE == qmi_svc_put_param_tlv(&ind, 
                                    CTLI_TYPE_PWRSAVE_MODE_RPT,
                                    sizeof(v_out1),
                                    &v_out1))
  {
    dsm_free_packet(&ind);
    return;
  }

  if( FALSE == qmi_svc_prepend_msg_hdr(&ind,
				       QMI_CTLI_CMD_VAL_REG_PWRSAVE_MODE) )
  {
    dsm_free_packet(&ind);
    return;
  }
	
  /*-------------------------------------------------------------------------
    Send indication to client
  -------------------------------------------------------------------------*/
  if( FALSE == qmi_ctli_send( sp,
                              QMI_CTL_FLAG_MSGTYPE_IND,
			      0, //transaction id
			      ind ) )
  {
    LOG_MSG_INFO2_0 ("Unable to generate Event Report IND");
  }

} /* qmi_ctli_pwrsave_mode_ind() */



/*===========================================================================
  FUNCTION QMI_CTLI_CONFIG_PWRSAVE()

  DESCRIPTION
    Handle the QMI CTL configure power save settings message.

    Dispatches a request to the QMUX to register the specified
    indication filters.  Filters are managed per-issuer.
    Returns the result of the operation to the issuer.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_cfg_pwrsave_mode
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type *   ctl_sp;
  uint8                   type;
  uint16                  len = 0;
  uint16                  expected_len;
  void *                  value;
  dsm_item_type *         response;
  qmi_result_e_type       result;
  qmi_error_e_type        errval;
  boolean                 got_v_in_reqd = FALSE;
  boolean                 spec_stored = FALSE;
  PACKED struct PACKED_POST
  {
    void*      state_handle;
    uint8      svc_id;
  } v_in_required = { 0, };
  uint16   ind_list[QMI_SVC_PWRSAVE_INDS_PER_SVC_MAX] = { 0, };
  uint8    num_inds = 0;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

  LOG_MSG_INFO3_0 ("qmi_ctli_cfg_pwrsave_mode");
  
  /*-------------------------------------------------------------------------
    Parse incoming command
    - identify and save expected TLVs,
      if duplicate TLV found, last present is used.
    - discard unexpected TLVs
  -------------------------------------------------------------------------*/
  while (*sdu_in)
  {
    if( !qmi_svc_get_param_tl( sdu_in, &type, &len ) )
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
        got_v_in_reqd = TRUE;
        break;

      case QMI_TYPE_PWRSAV_INDSET:
        if( len > sizeof(ind_list) )
        {
          LOG_MSG_INFO2_3 ("Invalid TLV len (%d>%d) for type (0x%X)",
                           len, sizeof(ind_list), type);
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        expected_len = 0;
        value = ind_list;
        num_inds = len / sizeof(ind_list[0]);
        break;

      default:
        LOG_MSG_INFO2_1 ("Unexpected TLV type (0x%X)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_3 ("Invalid TLV len (%d!=%d) for type (0x%X)",
                       len, expected_len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if( len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

  } /* end while */

  /*-------------------------------------------------------------------------
    Perform validity checks
  -------------------------------------------------------------------------*/
  if (!got_v_in_reqd)
  {
    LOG_MSG_INFO2_0 ("Mandatory TLV not present!");
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /* Ensure power save state in valid range */
  if( !IS_PWRSAVE_STATE_VALID( v_in_required.state_handle ) )
  {
    LOG_MSG_INFO2_1 ("Invalid power save state: %d",
                     v_in_required.state_handle);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
  
  /* Ensure QMI service indication is in valid range */
  if( v_in_required.svc_id >= QMUX_SERVICE_MAX )
  {
    LOG_MSG_INFO2_1 ("Invalid service identifier: %d", v_in_required.svc_id);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
  
  /*-------------------------------------------------------------------------
    Perform action/gather required information for response
    - Store power save filter in specification set
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_3( "Got QMI_CTL configure power save mode "
                   "(state=0x%X service=0x%X inds=%d)",
                   v_in_required.state_handle, v_in_required.svc_id, num_inds);

  if( QMI_SVC_PWRSAVE_INDS_PER_SVC_MAX < num_inds )
  {
    LOG_MSG_INFO2_1 ("Invalid num_inds: (%d) ", num_inds);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  spec_stored = qmi_svc_pwrsave_store_spec( ctl_sp->qmux_svc_handle,
                                            v_in_required.state_handle,
                                            v_in_required.svc_id,
                                            num_inds,
                                            ind_list );
  
  /* Confirm the power save spec was stored successfully. */
  if( !spec_stored )
  {
    LOG_MSG_INFO2_0 ("Error storing power save spec!");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
    
  /*-------------------------------------------------------------------------
    build response
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
} /* qmi_ctli_config_pwrsave() */



/*===========================================================================
  FUNCTION QMI_CTLI_SET_PWRSAVE_MODE()

  DESCRIPTION
    Handle the QMI CTL set power save mode message.

    Dispatches a request to the QMUX to modify the indication filters
    for the specified power save state.  Filters should have been
    previously registered.
    Returns the result of the operation to the issuer.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_set_pwrsave_mode
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type    *ctl_sp;
  uint8                   type;
  uint16                  len;
  uint16                  expected_len;
  void *                  value;
  boolean                 got_v_in_reqd     = FALSE;
  dsm_item_type *         response;
  qmi_result_e_type       result;
  qmi_error_e_type        errval;
  qmi_svc_pwr_state_handle_type new_state_handle = QMI_SVC_PWRSAVE_STATE_INVALID;
  qmi_svc_pwr_state_handle_type old_state_handle = QMI_SVC_PWRSAVE_STATE_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

  LOG_MSG_INFO3_0 ("qmi_ctli_set_pwrsave_mode");
  
  /*-------------------------------------------------------------------------
    Parse incoming command
    - identify and save expected TLVs,
      if duplicate TLV found, last present is used.
    - discard unexpected TLVs
  -------------------------------------------------------------------------*/
  while (*sdu_in)
  {
    if( !qmi_svc_get_param_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(new_state_handle);
        got_v_in_reqd = TRUE;
        value = &new_state_handle;
        break;

      default:
        LOG_MSG_INFO2_1 ("Unexpected TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_2 ("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if( len != PS_UTILS_DSM_PULLUP( sdu_in, value, len  ) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  } /* end while */

  /*-------------------------------------------------------------------------
    Perform validity checks (none)
  -------------------------------------------------------------------------*/
  if (!got_v_in_reqd)
  {
    LOG_MSG_INFO2_0 ("Mandatory TLV not present!");
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if( !IS_PWRSAVE_STATE_VALID( new_state_handle ) )
  {
    LOG_MSG_INFO2_1 ("Invalid power save state: %d", new_state_handle);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
    
  /*-------------------------------------------------------------------------
    Perform action/gather required information for response
    - Change the power save indication filter
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_1( "Got QMI_CTL set power save mode (state = 0x%x)",
                   new_state_handle);

  if( !qmi_svc_set_pwrsave_state( ctl_sp->qmux_svc_handle,
				  new_state_handle,
				  &old_state_handle ) )
  {
    LOG_MSG_INFO2_0 ("Error setting power save mode!");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  
  /*-------------------------------------------------------------------------
    build response
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
} /* qmi_ctli_set_pwrsave_mode() */


/*===========================================================================
  FUNCTION QMI_CTLI_GET_PWRSAVE_MODE()

  DESCRIPTION
    Handle the QMI CTL get power save mode message.

    Dispatches a request to the QMUX to query the current power save state.
    Returns the current power save state in the response to the issuer.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_get_pwrsave_mode
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type *   ctl_sp;
  dsm_item_type *         response;
  qmi_result_e_type       result;
  qmi_error_e_type        errval;
  struct
  {
    void*  state_handle;
  } v_out_required;
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
  PACKED struct PACKED_POST
  {
    boolean response_reporting;
  } v_out_opt;
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

  LOG_MSG_INFO3_0 ("qmi_ctli_get_pwrsave_mode");
  
  /*-------------------------------------------------------------------------
    Perform action/gather required information for response
    - get the power save mode for the QMI instance
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_0 ("Got QMI_CTL get power save mode");

  if( !qmi_svc_get_pwrsave_state( ctl_sp->qmux_svc_handle,
                                  &v_out_required.state_handle ) )
  {
    LOG_MSG_INFO2_0 ("Error getting power save mode!");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  LOG_MSG_INFO2_1 ("Query returned state: 0x%x", v_out_required.state_handle);
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
  if( !qmi_svc_get_response_reporting_in_current_pwrsave_mode
                                  ( ctl_sp->qmux_svc_handle,
                                  &v_out_opt.response_reporting ) )
  {
    LOG_MSG_INFO2_0 ("Error getting power save mode response reporting!");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Since no error so far, build response payload from the back to the front
  -------------------------------------------------------------------------*/

  if( FALSE == qmi_svc_put_param_tlv(&response,
                                     CTLI_TYPE_PWRSAVE_MODE_RESP_RPT,
                                     sizeof(v_out_opt),
                                     &v_out_opt) )
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
  }
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/
  if( FALSE == qmi_svc_put_param_tlv( &response,
                                      QMI_TYPE_REQUIRED_PARAMETERS,
                                      sizeof (v_out_required),
                                      &v_out_required ) )
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Prepend response result and return to input function which will
    finalize the response command header and send it back via QMUX
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
} /* qmi_ctli_get_pwrsave_mode() */

#ifdef FEATURE_DATA_QMI_RESP_FILTERING
/*===========================================================================
  FUNCTION   qmi_ctli_configure_response_filtering_in_pwrsave_mode 

  DESCRIPTION
    Configures Multiple Power Save States for filtering or reporting
    of QMI responses. Handle QMI CTL request to configure response filtering
    in power save mode. Allows configuration of whether responses are
    filtered or reported for each power save mode.
 
    Dispatches a request to the QMUX to register all the specified
    power save states in the request. It configures response reporting for each
    mentioned power save state in the list. Returns the result of the operation
    to the issuer.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data
    tx_id:   Transaction ID
  DEPENDENCIES
    qmi_ctl_init() must have been called
 
  RETURNS
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_configure_response_filtering_in_pwrsave_mode
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type    *ctl_sp;
  uint8                   type;
  uint16                  len;
  uint16                  expected_len;
  void *                  value;
  boolean                 got_v_in_reqd     = FALSE;
  dsm_item_type *         response = NULL;
  qmi_result_e_type       result;
  qmi_error_e_type        errval;
  boolean                 spec_stored       = FALSE;
  boolean                 response_reporting_spec = FALSE;
  uint8                   index             = 0;

  response_reporting_type * v_out_reporting_opt = NULL;
  response_reporting_type * v_in_required       = NULL;
  byte                    * reporting_opt_out;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;
  v_in_required = NULL;

  LOG_MSG_INFO3_0 ("qmi_ctli_configure_response_filtering_in_pwrsave_mode");
  
  /*-------------------------------------------------------------------------
    Parse incoming command
    - identify and save expected TLVs,
      if duplicate TLV found, last present is used.
    - discard unexpected TLVs
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC (v_in_required, sizeof(response_reporting_type),
                            response_reporting_type*);

  if ( v_in_required == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  memset(v_in_required, 0, sizeof(response_reporting_type));
  while (*sdu_in)
  {
    if( !qmi_svc_get_param_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
       case QMI_TYPE_REQUIRED_PARAMETERS:
         if (len > sizeof(response_reporting_type))
         {
           LOG_MSG_INFO2_2("T (%d) Invalid L (%d bytes) V - discarding",
                           type, len);
           errval = QMI_ERR_ARG_TOO_LONG;
           goto send_result;
         }
         expected_len = 0;
         value = v_in_required;
         got_v_in_reqd = TRUE;
         break;

      default:
        LOG_MSG_INFO2_1 ("Unexpected TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_2 ("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if( len != PS_UTILS_DSM_PULLUP( sdu_in, value, len) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  } /* end while */

  /*-------------------------------------------------------------------------
    Perform validity checks (none)
  -------------------------------------------------------------------------*/
  if (!got_v_in_reqd)
  {
    LOG_MSG_INFO2_0 ("Mandatory TLV not present!");
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if (v_in_required->n_pwrsave_state == 0)
  {
    LOG_MSG_INFO2_1 ("Invalid number of power save state: %d",
                     v_in_required->n_pwrsave_state);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
  Check if each power state provided in the request is valid or not
  -------------------------------------------------------------------------*/

  for (index = 0 ; index < v_in_required->n_pwrsave_state;index++)
  {
    if( !IS_PWRSAVE_STATE_VALID( 
       v_in_required->reporting_tuple[index].pwrsave_state_handle) )
    {
      LOG_MSG_ERROR_1 ("Invalid power save state: %d",
                       v_in_required->reporting_tuple[index].pwrsave_state_handle);
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  for (index = 0 ; index < v_in_required->n_pwrsave_state;index++)
  {
    spec_stored = qmi_svc_pwrsave_store_response_reporting(
                  ctl_sp->qmux_svc_handle,
                  v_in_required->reporting_tuple[index].pwrsave_state_handle,
                  v_in_required->reporting_tuple[index].response_reporting);

    /* Confirm the power save spec was stored successfully. */
    if( !spec_stored )
    {
      LOG_MSG_ERROR_0 ("Error storing power save spec!");
      errval = QMI_ERR_INTERNAL;
      goto send_result;
    }
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(v_out_reporting_opt, sizeof(response_reporting_type),
                           response_reporting_type*);

  if ( v_out_reporting_opt == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Get the list of all configured power save states 
    with their respective response_reporting
  -------------------------------------------------------------------------*/

  memset(v_out_reporting_opt, 0, sizeof(response_reporting_type));

  reporting_opt_out = (byte *) v_out_reporting_opt;
  response_reporting_spec = 
    qmi_svc_get_response_reporting_in_configured_pwrsave_modes(
       ctl_sp->qmux_svc_handle, 
       &reporting_opt_out, 
       &v_out_reporting_opt->n_pwrsave_state);

  /*-------------------------------------------------------------------------
    Confirm the power save spec was stored successfully 
    FALSE is only returned if there is an invalid QMI Instance
  -------------------------------------------------------------------------*/

  if( !response_reporting_spec )
  {
    LOG_MSG_ERROR_0 ("Error: Invalid QMI INSTANCE!");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (v_out_reporting_opt->n_pwrsave_state > 0)
  {
    /*-------------------------------------------------------------------------
      build response
    -------------------------------------------------------------------------*/
    if( FALSE == qmi_svc_put_param_tlv(&response,
                                       CTLI_TYPE_PWRSAVE_MODE_RESP_RPT,
                                       (uint16) (reporting_opt_out - 
                                                (byte*) v_out_reporting_opt),
                                                 v_out_reporting_opt) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    } 
  }

send_result:

  if (v_out_reporting_opt != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(v_out_reporting_opt);
  }

  if (v_in_required != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(v_in_required);
  }

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
}/*qmi_ctli_configure_response_filtering_in_pwrsave_mode*/

/*===========================================================================
  FUNCTION   qmi_ctli_get_response_filtering_setting_in_pwrsave_mode 

  DESCRIPTION
    Handle the QMI CTL get response filtering setting in power save
    mode message.

    Dispatches a request to the QMUX to query all configured power save state.
    Returns the list of all power save states and their corresponding response
    reporting flag in the response to the issuer.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data
    tx_id:   Transaction ID

  DEPENDENCIES
    qmi_ctl_init() must have been called

  RETURNS
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded
 
  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  
qmi_ctli_get_response_filtering_setting_in_pwrsave_mode
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type    *ctl_sp;
  dsm_item_type *         response = NULL;

  qmi_result_e_type       result;
  qmi_error_e_type        errval;
  boolean                 response_reporting_spec = FALSE;

  response_reporting_type * v_out_reporting_opt;
  byte                    * reporting_opt_out;

/*-------------------------------------------------------------------------------*/
  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    ASSERT(0);
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

  v_out_reporting_opt = NULL;

  LOG_MSG_INFO3_0 (
"qmi_ctli_get_response_filtering_setting_in_pwrsave_mode");

   PS_SYSTEM_HEAP_MEM_ALLOC (v_out_reporting_opt, sizeof(response_reporting_type),
                             response_reporting_type*);

  if ( v_out_reporting_opt == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  memset(v_out_reporting_opt, 0, sizeof(response_reporting_type));

  reporting_opt_out = (byte *) v_out_reporting_opt;
  response_reporting_spec = 
    qmi_svc_get_response_reporting_in_configured_pwrsave_modes
    (ctl_sp->qmux_svc_handle, 
     &reporting_opt_out, 
     &v_out_reporting_opt->n_pwrsave_state);

  /* Confirm the power save spec was stored successfully. */
  if( !response_reporting_spec )
  {
    LOG_MSG_ERROR_0 ("Error: Invalid QMI INSTANCE Found!");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (v_out_reporting_opt->n_pwrsave_state > 0)
  {
    /*-------------------------------------------------------------------------
      build response
    -------------------------------------------------------------------------*/
    if( FALSE == qmi_svc_put_param_tlv(&response,
                                       CTLI_TYPE_PWRSAVE_MODE_RESP_RPT,
                                       (uint16) (reporting_opt_out - 
                                                (byte*) v_out_reporting_opt),
                                                 v_out_reporting_opt) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    } 
  }

send_result:

  if (v_out_reporting_opt != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(v_out_reporting_opt);
  }

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
}/*qmi_ctli_get_response_filtering_setting_in_pwrsave_mode*/
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/

/*===========================================================================
  FUNCTION QMI_CTL_PWRSAVE_MODE_CHANGE_NOTIFY

  DESCRIPTION
    Function to apply the new power save state to a given instance.
    If the state changes, an indication may be generated towards the
    client to notify the state transition.
  
  PARAMETERS
    qmi_inst          - the QMI instance to operate upon
    new_state_handle  - the new power save state handle
    old_state_handle  - the previous power save state handle

  RETURN VALUE
    TRUE if power save state changed
    FALSE otherwise
    
  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    May enqueue indication to associated client.
===========================================================================*/
boolean qmi_ctl_pwrsave_mode_change_notify
(
  qmi_instance_e_type    qmi_inst,
  void*                  new_state_handle,
  void*                  old_state_handle
)
{
  qmi_ctli_state_type        *ctl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (QMI_INSTANCE_MAX <= qmi_inst)
  {
    LOG_MSG_ERROR_1("Recd invalid inst_id '%d' in qmi_ctl_pwrsave_mode_change_notify",
                    (int32)qmi_inst);
    ASSERT(0);
    return FALSE;
  }

  ctl_sp = &qmi_ctl_state[ qmi_inst ];

  LOG_MSG_INFO3_0("qmi_ctl_pwrsave_mode_change_notify");
 
  /* Generate indication for state transition on return to normal power mode. */
  if( QMI_SVC_PWRSAVE_STATE_NORMAL == new_state_handle )
  {
    LOG_MSG_INFO2_2("Generate power save indication: new=0x%X old=0x%X",
                    new_state_handle, old_state_handle);

    qmi_ctli_pwrsave_mode_ind( (void*)ctl_sp,
                               0,
                               CTLI_REPORT_STATUS_EV_PWRSAVE,
                               old_state_handle );
  }

  return TRUE;
} /* qmi_ctl_pwrsave_mode_change_notify() */

/*===========================================================================
  FUNCTION QMI_CTL_SYNC_IND_CMD_HDLR()

  DESCRIPTION
    Called to process QMI_CMD_CTL_SEND_SYNC_IND message.
    
  PARAMETERS
    sp          - QMI_CTL's service instance state pointer for this qmi link
                  returned by qmux_reg_service()

  RETURN VALUE
    None
    
  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_ctl_sync_ind_cmd_hdlr
(
  void *sp
)
{
  qmi_ctli_state_type   *ctl_sp;
  ctl_sp = (qmi_ctli_state_type *) sp;

  if( NULL != ctl_sp->qmux_svc_handle && QMI_CTL_SYNC_WAITING_FOR_REQUEST_STATE == ctl_sp->sync_state.state )
  {
    qmi_ctli_sync_ind(sp);

    if( QMI_CTL_SYNC_IND_MAX_BACK_OFF_PERIOD > ctl_sp->sync_state.back_off_period )
    {
      ctl_sp->sync_state.back_off_period *= QMI_CTL_SYNC_IND_BACK_OFF_MULTIPLIER;
      
      timer_set( &ctl_sp->sync_state.timer,
                 ctl_sp->sync_state.back_off_period,
                 0,
                 T_SEC );
    }else
    {
      (void) timer_clr( &ctl_sp->sync_state.timer, T_NONE );
    }
  }

  return ;
}

/*===========================================================================
FUNCTION   qmi_ctli_sync_oprt_mode_online

DESCRIPTION
  Set operating mode to online if there is no NV set for PLPM and FTM

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static qmi_error_e_type qmi_ctli_sync_oprt_mode_online
(
  void
)
{
  nv_item_type      *qmi_nv_item_ptr;
  qmi_if_info_type  *qmi_if_info_ptr;
  qmi_error_e_type   errval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_0("Enter qmi_ctli_sync_oprt_mode_online");

  errval = QMI_ERR_NONE;

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  qmi_nv_item_ptr = (nv_item_type *)
    modem_mem_alloc(sizeof(nv_item_type), MODEM_MEM_CLIENT_QMI_CRIT);
  if( qmi_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
    return QMI_ERR_NO_MEMORY;
  }
  
  /* if it's in persistence LPM, then return */
  if ((NV_DONE_S == qmi_get_nv_item( NV_LPM_PERSISTENCE_I, qmi_nv_item_ptr )) &&
      (qmi_nv_item_ptr->lpm_persistence))
  {
    LOG_MSG_INFO2_0("Target in P-LPM mode, bailing out");
    goto bail_out;
  }

  /* if it's in FTM, then return */
  if ((NV_DONE_S == qmi_get_nv_item( NV_FTM_MODE_I, qmi_nv_item_ptr )) &&
      (qmi_nv_item_ptr->ftm_mode))
  {
    LOG_MSG_INFO2_0("Target in FTM mode, bailing out");
    goto bail_out;
  }

  qmi_if_info_ptr = qmi_if_info_get_ptr();
  /* Issue ONLINE command to CM */
  if(!cm_ph_cmd_oprt_mode (NULL, NULL, qmi_if_info_ptr->cm_client_id, 
                           SYS_OPRT_MODE_ONLINE))
  {
    LOG_MSG_ERROR_0("Unable to issue a Online command ");
    errval = QMI_ERR_INTERNAL;
  }

bail_out:
  /*-------------------------------------------------------------------------
    Free the temporary memory allocated for NV item.
  -------------------------------------------------------------------------*/
  modem_mem_free(qmi_nv_item_ptr, MODEM_MEM_CLIENT_QMI_CRIT);
  return errval;
} /* qmi_ctli_sync_oprt_mode_online() */

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
/*===========================================================================
FUNCTION   qmi_ctli_set_svc_avail_list 

  DESCRIPTION
    Handle the QMI CTL set svc avail list message.

    Function to set the service available list. This function 
    can configure multiple QMI services to be notified to the 
    control point when available to the device. The service available list 
    is overridden everytime a new valid request to configure a list comes in.
 
  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data
 
  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_set_svc_avail_list
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type    *ctl_sp;
  uint8                   type;
  uint16                  len;
  uint16                  expected_len;
  void *                  value;
  boolean                 got_v_in_reqd     = FALSE;
  dsm_item_type *         response = NULL;
  qmi_result_e_type       result;
  qmi_error_e_type        errval;
  boolean                 svc_avail_spec = FALSE;
  uint8                   index             = 0;
  boolean                 response_reporting_spec = FALSE;
  svc_list_type                   * v_in_svc_list       = NULL;
  svc_list_type                   * v_out_svc_list      = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

  PS_SYSTEM_HEAP_MEM_ALLOC(v_in_svc_list, sizeof(svc_list_type),
                           svc_list_type*);

  if ( v_in_svc_list == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }
  memset(v_in_svc_list, 0, sizeof(svc_list_type));

  LOG_MSG_INFO3_0 ("qmi_ctli_set_svc_avail_list");
  
  /*-------------------------------------------------------------------------
    Parse incoming command
    - identify and save expected TLVs,
      if duplicate TLV found, last present is used.
    - discard unexpected TLVs
  -------------------------------------------------------------------------*/

  while (*sdu_in)
  {
    if( !qmi_svc_get_param_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
       case QMI_TYPE_REQUIRED_PARAMETERS:
         if (len > sizeof(svc_list_type))
         {
           LOG_MSG_INFO2_2("T (%d) Invalid L (%d bytes) V - discarding",
                           type, len);
           errval = QMI_ERR_ARG_TOO_LONG;
           goto send_result;
         }
         expected_len = 0;
         value = v_in_svc_list;
         got_v_in_reqd = TRUE;
         break;

      default:
        LOG_MSG_INFO2_1 ("Unexpected TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_2 ("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if( len != PS_UTILS_DSM_PULLUP( sdu_in, value, len) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  } /* end while */

  /*-------------------------------------------------------------------------
    Perform validity checks (none)
  -------------------------------------------------------------------------*/
  if (!got_v_in_reqd)
  {
    LOG_MSG_INFO2_0 ("Mandatory TLV not present!");
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }
  /*-------------------------------------------------------------------------
    Perform Validity Checks here   
  -------------------------------------------------------------------------*/

  if ((v_in_svc_list->num_svcs > QMUX_SERVICE_MAX) || (v_in_svc_list->num_svcs == 0))
  {
    LOG_MSG_INFO2_1 ("Invalid service set length: %d",
                     v_in_svc_list->num_svcs);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  for(index=0; index<v_in_svc_list->num_svcs; index++)
  {
    if( v_in_svc_list->svc_tuple[index].svc_id>= QMUX_SERVICE_MAX ||
        v_in_svc_list->svc_tuple[index].svc_id == 0)
    {
      LOG_MSG_INFO2_2 ("Invalid service identifier on index[%d]: %d",
                       index, v_in_svc_list->svc_tuple[index].svc_id);
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  svc_avail_spec = ctli_store_service_available_list
                  (ctl_sp, 
                   v_in_svc_list->num_svcs, 
                   v_in_svc_list->svc_tuple);

  if( !svc_avail_spec )
  {
    LOG_MSG_ERROR_0 ("Error: Invalid QMI INSTANCE!");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
   The spec was successfully stored. Now return the configured list here
  -------------------------------------------------------------------------*/
  v_out_svc_list = NULL;

  PS_SYSTEM_HEAP_MEM_ALLOC (v_out_svc_list, sizeof(svc_list_type),
                            svc_list_type*);

  if ( v_out_svc_list == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }
  memset(v_out_svc_list, 0, sizeof(svc_list_type));

  response_reporting_spec = ctli_get_available_service_list
                           (ctl_sp, 
                            &v_out_svc_list->svc_tuple, 
                            &v_out_svc_list->num_svcs);

  /* Confirm the service available list was stored successfully. */
  if( !response_reporting_spec )
  {
    LOG_MSG_ERROR_0 ("Error: Invalid QMI Service Available List!");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (v_out_svc_list->num_svcs > 0)
  {
    if( FALSE == qmi_svc_put_param_tlv(&response,
                                       CTLI_TYPE_SVC_AVAIL_LIST,
                                       (uint16) (v_out_svc_list->num_svcs + 1),
                                                 v_out_svc_list) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    } 
  }

  /*-------------------------------------------------------------------------
    build response
  -------------------------------------------------------------------------*/
send_result:

  if (v_in_svc_list != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(v_in_svc_list);
  }

  if (v_out_svc_list != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(v_out_svc_list);
  }

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
} /* qmi_ctli_set_svc_avail_list() */

/*===========================================================================
  FUNCTION   qmi_ctli_get_svc_avail_list 

  DESCRIPTION
    Handle the QMI CTL get svc avail list message.

    Query to see if there is a configured svc available list and return it
    in the response.
 
  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data
 
  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_get_svc_avail_list
(
   void *            sp,
   uint8             clid,
   uint8             tx_id,
   dsm_item_type **  sdu_in
 )
 {
   qmi_ctli_state_type    *ctl_sp;
   dsm_item_type *         response = NULL;

   qmi_result_e_type       result;
   qmi_error_e_type        errval;
   boolean                 response_reporting_spec = FALSE;

   svc_list_type           * v_out_svc_list = NULL;

 /*-------------------------------------------------------------------------------*/
   if (NULL == sp)
   {
     if ((NULL != sdu_in) && (NULL != *sdu_in))
     {
       dsm_free_packet(sdu_in);
     }
     DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
     return NULL;
   }
   else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
   {
     if ((NULL != sdu_in) && (NULL != *sdu_in))
     {
       dsm_free_packet(sdu_in);
     }
     DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
     return NULL;
   }
   else if (NULL == sdu_in)
   {
     DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
     return NULL;
   }

   ctl_sp = (qmi_ctli_state_type *) sp;

   errval = QMI_ERR_NONE;
   response = NULL;

   v_out_svc_list = NULL;

   LOG_MSG_INFO3_0 ("qmi_ctli_get_svc_avail_list");

   PS_SYSTEM_HEAP_MEM_ALLOC (v_out_svc_list, sizeof(svc_list_type),
                             svc_list_type*);

   if ( v_out_svc_list == NULL)
   {
     errval = QMI_ERR_NO_MEMORY;
     goto send_result;
   }

   memset(v_out_svc_list, 0, sizeof(svc_list_type));

   response_reporting_spec = ctli_get_available_service_list
                            (ctl_sp, 
                             &v_out_svc_list->svc_tuple, 
                             &v_out_svc_list->num_svcs);

   /* Confirm the power save spec was stored successfully. */
   if( !response_reporting_spec )
   {
     LOG_MSG_ERROR_0 ("Error: Invalid QMI INSTANCE Found!");
     errval = QMI_ERR_INTERNAL;
     goto send_result;
   }

   if (v_out_svc_list->num_svcs > 0)
   {
     if( FALSE == qmi_svc_put_param_tlv(&response,
                                    CTLI_TYPE_SVC_AVAIL_LIST,
                                    (uint16) (v_out_svc_list->num_svcs + 1),
                                              v_out_svc_list) )
     {
       errval = QMI_ERR_NO_MEMORY;
       dsm_free_packet(&response);
     } 
   }

   /*-------------------------------------------------------------------------
     build response
   -------------------------------------------------------------------------*/
 send_result:

   if (v_out_svc_list != NULL)
   {
     PS_SYSTEM_HEAP_MEM_FREE(v_out_svc_list);
   }

   result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                    : QMI_RESULT_FAILURE);

   if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
   {
     dsm_free_packet(&response);
     return NULL;
   }
   return response;
} /* qmi_ctli_get_svc_avail_list() */

/*===========================================================================
  FUNCTION   qmi_ctli_set_event_report 

  DESCRIPTION
    To set state for event reporting

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data
 
  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_set_event_report
(
   void *            sp,
   uint8             clid,
   uint8             tx_id,
   dsm_item_type **  sdu_in
)
{
  dsm_item_type *               response;
  qmi_ctli_state_type    *ctl_sp;
  
  PACKED struct PACKED_POST
  {
    boolean report_svc_avail;
  } v_in1;

  boolean  got_svc_avail_list_tlv = FALSE;
  uint8    type;
  uint16   len;
  uint16   expected_len;
  void *   value;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;

  dsm_item_type *               ind = NULL;
 /*-------------------------------------------------------------------------------*/
  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

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
      case CTLI_TYPE_SVC_AVAIL_LIST:
        expected_len = sizeof(v_in1);
        got_svc_avail_list_tlv = TRUE;
        value = &v_in1;
        break;

       default:
        LOG_MSG_INFO2_1 ("Unrecognized TLV type (%d)", type);
        break;
    }

    if(!qmi_svc_validate_type_length( type,
                                      len,
                                      expected_len,
                                      FILE_ID_DS_QMI_CTL,__LINE__))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    /*-----------------------------------------------------------------------
      If type is unrecognized, value will be NULL, and qmi_svc_dsm_pullup will 
      free the unrecognized value, i.e. we skip past unrecognized TLVs
    -----------------------------------------------------------------------*/
    if(len != PS_UTILS_DSM_PULLUP( sdu_in, value, len ))
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  /*-------------------------------------------------------------------------
    If ALL optional TLVs are absent return error or else carry on and
    process the request.
  -------------------------------------------------------------------------*/

  if(!(got_svc_avail_list_tlv))
  {
      errval = QMI_ERR_MISSING_ARG;
      goto send_result;
  }
  /*-------------------------------------------------------------------------
    change svc avail list TLV if included in the request
  -------------------------------------------------------------------------*/
  if( got_svc_avail_list_tlv )
  {
    if (0 < v_in1.report_svc_avail)
    {
      ctl_sp->report_status.report_svc_avail = TRUE;
    }
    else
    {
      ctl_sp->report_status.report_svc_avail = FALSE;
    }
  }
  /*-------------------------------------------------------------------------
    Build the response from back to front
    No Failure possible upto here so result is success
  -------------------------------------------------------------------------*/

  if( got_svc_avail_list_tlv )
  {
    if( FALSE == qmi_svc_put_param_tlv(&response,
                                       CTLI_TYPE_SVC_AVAIL_LIST,
                                       sizeof(boolean),
                                       &ctl_sp->report_status.report_svc_avail) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  if(FALSE == qmi_svc_put_result_tlv(&response, result, errval))
  {
    dsm_free_packet(&response);
    return NULL;
  }

  if(ctl_sp->report_status.report_svc_avail)
  {
    if( FALSE == qmi_svc_prepend_msg_hdr
                 (&ind,QMI_CTLI_CMD_VAL_SVC_AVAIL_LIST_IND) )
    {
      dsm_free_packet(&ind);
      return NULL;
    }
    /*-------------------------------------------------------------------------
      Send indication to client
    -------------------------------------------------------------------------*/
    if( FALSE == qmi_ctli_send( ctl_sp,
                                QMI_CTL_FLAG_MSGTYPE_IND,
                                0, //transaction id
                                ind ) )
    {
      LOG_MSG_ERROR_0 ("Unable to generate Event Report IND");
      return NULL;
    }
  }
  return response;
} /* qmi_ctli_set_event_report() */

/*===========================================================================
  FUNCTION CTLI_STORE_SERVICE_AVAILABLE_LIST

  DESCRIPTION
    Function to store the Service Available List which gets configured
    everytime after a valid request. With each new request
    from control point, the configured service available list for the
    client gets overwritten.
  
  PARAMETERS
    service_handle  - qmux service handle returned by qmux_reg_service()
    num_svcs        - number of QMI Svcs to be configured
    svc_list        - List of QMI svcs to be configured
 
  RETURN VALUE
    TRUE if svc_avail_list is successfully configured,
    FALSE otherwise
    
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
static boolean ctli_store_service_available_list
(
  void                         * sp,
  uint8                          num_svcs,
  void                         *in_svc_list
)
{
  qmi_ctli_state_type         *ctl_sp = NULL;
  uint8                       i;
  qmi_svc_avail_list_type     *current        = NULL;
  qmi_svc_avail_list_type     *head           = NULL;
  qmi_svc_avail_list_type     *temp           = NULL;
  uint8                       *svc_list       = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check that service_handle or svc_list is not NULL
     in case ASSERT is removed */
  if (sp == NULL || in_svc_list == NULL)
  {
    DATA_ERR_FATAL("QMI_SVC list NULL!");
    return FALSE;
  }
  if (num_svcs == 0)
  {
    LOG_MSG_ERROR_0 ("Incorrect Service Available list req!");
    return FALSE;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  LOG_MSG_INFO3_0("ctli_store_service_available_list");
  
  current = ctl_sp->svc_avail_list;
  /*-------------------------------------------------------------------------
      A configured service available list already exists for this client,
      Free it.
    -------------------------------------------------------------------------*/
  while(current != NULL)
  {
    temp = current->next;

    /*Free all the previous stored nodes for available service list*/
    PS_SYSTEM_HEAP_MEM_FREE(current);
    current = temp;
  }

  /*-------------------------------------------------------------------------
    Store svc avail list sepcification in the client state
  -------------------------------------------------------------------------*/
  
  PS_SYSTEM_HEAP_MEM_ALLOC (ctl_sp->svc_avail_list, sizeof(qmi_svc_avail_list_type),
                            qmi_svc_avail_list_type*);

  if(ctl_sp->svc_avail_list == NULL )
  {
    return FALSE;
  }

  memset(ctl_sp->svc_avail_list, 0, sizeof(qmi_svc_avail_list_type));

  head = ctl_sp->svc_avail_list;

  svc_list = (uint8 *) in_svc_list;

  head->svc_id = svc_list[0];
  current = head;
  /*Do a sorted insert on the service available list*/
  for (i=1; i<num_svcs; i++)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(temp, sizeof(qmi_svc_avail_list_type),
                                    qmi_svc_avail_list_type*);

    if ( temp == NULL)
    {
      PS_SYSTEM_HEAP_MEM_FREE(ctl_sp->svc_avail_list);
      return FALSE;
    }
    memset(temp, 0, sizeof(qmi_svc_avail_list_type));

    temp->svc_id = svc_list[i];

    /*Dont insert Duplicate value in head*/
  
    if (temp->svc_id == head->svc_id)
    {
      PS_SYSTEM_HEAP_MEM_FREE(temp);
      continue;
    }
    else if (head->svc_id > temp->svc_id)
    {
      temp->next = head;
      head = temp;
    }
    else
    {
      while (current->next !=NULL && current->next->svc_id <= temp->svc_id)
      {
        current = current->next;
      }
      /*Dont insert Duplicate value*/
  
      if (current->svc_id == temp->svc_id)
      {
        PS_SYSTEM_HEAP_MEM_FREE(temp);
        continue;
      }

      temp->next = current->next;
      current->next = temp;
    }
    /*Make current back to head for next iteration*/
    current = head;
  }
  ctl_sp->svc_avail_list = head;
  return TRUE;
} /*ctli_store_service_available_list() */


/*===========================================================================
  FUNCTION QMI_CTLI_GET_AVAIL_LIST

  DESCRIPTION
    Function to query the service available list which was previously
    configured by a client
  
  PARAMETERS
    service_handle  - qmux service handle returned by qmux_reg_service()
    num_svcs        - number of QMI Svcs to be configured
    svc_list        - List of QMI svcs to be configured
 
  RETURN VALUE
    TRUE is returned if svc_avail_list exists,
    FALSE otherwise
    
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
static boolean ctli_get_available_service_list
(
  void                         * sp,
  void                         * svc_list,
  uint8                        * num_svcs
)
{

  qmi_ctli_state_type           *ctl_sp = NULL;
  uint8                         svc_len = 0;
  uint8                          *out_svc_list = NULL;
  qmi_svc_avail_list_type       *svc_avail_list = NULL;
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO3_0("ctli_get_available_service_list");
  
  /* check that service_handle,svc_list,num_svcs is
     not NULL in case ASSERT is removed */
  
  if (sp == NULL || svc_list == NULL || num_svcs == NULL)
  {
    DATA_ERR_FATAL("QMI_SVC state pointer,svc list, num_svcs NULL!");
    return FALSE;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  if(ctl_sp->svc_avail_list == NULL)
  {
    LOG_MSG_ERROR_0 ("QMI Service Available List not initialized!");
    return FALSE;
  }

  svc_avail_list = ctl_sp->svc_avail_list;

  out_svc_list = (uint8 *) svc_list;

  svc_len = 0;

/*---------------------------------------------------------------------------
  For each qmi service, check if a spec exists, then store &
  return the service available list
---------------------------------------------------------------------------*/
  
  while (svc_avail_list != NULL)
  {
    //store them
    out_svc_list[svc_len] = svc_avail_list->svc_id;
    svc_len++;

    svc_avail_list = svc_avail_list->next;
  }
  *num_svcs = svc_len;

  return TRUE;  
} /*ctli_get_available_service_list() */

/*===========================================================================
  FUNCTION CTLI_REPORT_SERVICE_AVAILABILITY

  DESCRIPTION
    Function to check if the queried service exists in the configured 
    service available list which was previously configured for a valid client

  PARAMETERS
    qmi_instance    - the QMI instance being used
    service_id      - QMI service identifier
    
  RETURN VALUE
    TRUE if message should be filtered
    FALSE if message should be sent
    
  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static boolean ctli_report_service_availability
(
  void                      *sp,
  qmux_service_e_type       service_id
)
{
  qmi_ctli_state_type       *ctl_sp;
  qmi_svc_avail_list_type   *svc_avail_list        = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( QMUX_SERVICE_MAX > service_id );
  
  /* check that service_id is valid in case ASSERT is removed */
  if (service_id >= QMUX_SERVICE_MAX )
  {
    LOG_MSG_ERROR_1("QMI service ID not valid (%d)!  Discarding", service_id);
    return FALSE;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  svc_avail_list = ctl_sp->svc_avail_list;

  /* check that service available list is available */
  if (svc_avail_list == NULL )
  {
    LOG_MSG_ERROR_0("QMI SVC Avail List is not initialized!");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Check if Control point is registered or not to recieve indication
  -------------------------------------------------------------------------*/

  while (svc_avail_list != NULL)
  {
    if (svc_avail_list->svc_id == service_id)
    {
      LOG_MSG_INFO1_1("Notifying Control Point about service [%d] availability",
                      service_id);
      return TRUE;
    }
    svc_avail_list = svc_avail_list->next;
  }

  LOG_MSG_ERROR_1("Ignoring Notification to Control Point about "
                  "service [%d] availability", service_id);

  return FALSE;
} /* ctli_report_service_availability() */
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

/*===========================================================================
  FUNCTION QMI_CTLI_CONFIG_PWRSAVE_MODE_EXT()

  DESCRIPTION
    Handle the QMI CTL configure power save settings message.

    Dispatches a request to the QMUX to register the specified
    indication filters.  Filters are managed per-issuer.
    Returns the result of the operation to the issuer.

  PARAMETERS
    sp:      QMI_CTL's service instance state pointer for this qmi link
             returned by qmux_reg_service()
    clid:    coresponding client id
    sdu_in:  input command data

  RETURN VALUE
    dsm * pointing to the response to be sent to host
    NULL if no response was generated or command was discarded

  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type *  qmi_ctli_cfg_pwrsave_mode_ext
(
  void *            sp,
  uint8             clid,
  uint8             tx_id,
  dsm_item_type **  sdu_in
)
{
  qmi_ctli_state_type *   ctl_sp;
  uint8                   type;
  uint16                  len = 0;
  uint16                  expected_len;
  uint16                   ind_set_len = 0;
  void *                  value;
  dsm_item_type *         response;
  qmi_result_e_type       result;
  qmi_error_e_type        errval;
  boolean                 got_v_in_reqd = FALSE;
  boolean                 got_v_in_opt  = FALSE;
  boolean                 spec_stored = FALSE;

  PACKED struct PACKED_POST
  {
    void*      state_handle;
    uint8      svc_id;
  } v_in_required = { 0, };

  typedef PACKED struct PACKED_POST
  {
    uint8    num_inds;
    uint16   ind_list[0];
  } v_in_opt_type;

  v_in_opt_type * v_in_list = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == sp)
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd NULL svc ptr, dropping message");
    return NULL;
  }
  else if (FALSE == qmi_ctli_is_valid_client_id(sp,clid))
  {
    if ((NULL != sdu_in) && (NULL != *sdu_in))
    {
      dsm_free_packet(sdu_in);
    }
    DATA_ERR_FATAL("Recd pkt from invalid client, dropping message");
    return NULL;
  }
  else if (NULL == sdu_in)
  {
    DATA_ERR_FATAL("Recd NULL SDU ptr, bailing out");
    return NULL;
  }

  ctl_sp = (qmi_ctli_state_type *) sp;

  errval = QMI_ERR_NONE;
  response = NULL;

  LOG_MSG_INFO3_0 ("qmi_ctli_cfg_pwrsave_mode_ext");
  
  /*-------------------------------------------------------------------------
    Parse incoming command
    - identify and save expected TLVs,
      if duplicate TLV found, last present is used.
    - discard unexpected TLVs
  -------------------------------------------------------------------------*/
  while (*sdu_in)
  {
    if( !qmi_svc_get_param_tl( sdu_in, &type, &len ) )
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
        got_v_in_reqd = TRUE;
        break;

      case QMI_TYPE_PWRSAV_INDSET_EXT:
        if( len > (sizeof(uint16)*QMI_SVC_PWRSAVE_EXT_INDS_PER_SVC_MAX) + 1)
        {
          LOG_MSG_INFO2_3 ("Invalid TLV len (%d>%d) for type (0x%X)",
                           len, sizeof(v_in_opt_type), type);
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }

        PS_SYSTEM_HEAP_MEM_ALLOC(v_in_list, len,  v_in_opt_type*);

        if ( v_in_list == NULL)
        {
          errval = QMI_ERR_NO_MEMORY;
          goto send_result;
        }

        memset( v_in_list, 0, len );

        expected_len = 0;
        ind_set_len = len;
        got_v_in_opt = TRUE;
        value = v_in_list;
        break;

      default:
        LOG_MSG_INFO2_1 ("Unexpected TLV type (0x%X)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      LOG_MSG_INFO2_3 ("Invalid TLV len (%d!=%d) for type (0x%X)",
                       len, expected_len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if( len != PS_UTILS_DSM_PULLUP( sdu_in, value, len  ) )
    {
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

  } /* end while */

  /*-------------------------------------------------------------------------
    Perform validity checks
  -------------------------------------------------------------------------*/
  if (!got_v_in_reqd)
  {
    LOG_MSG_INFO2_0 ("Mandatory TLV not present!");
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /* Ensure power save state in valid range */
  if( !IS_PWRSAVE_STATE_VALID( v_in_required.state_handle ) )
  {
    LOG_MSG_INFO2_1 ("Invalid power save state: %d",
                     v_in_required.state_handle);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
  
  /* Ensure QMI service indication is in valid range */
  if( v_in_required.svc_id >= QMUX_SERVICE_MAX )
  {
    LOG_MSG_INFO2_1 ("Invalid service identifier: %d", v_in_required.svc_id);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Perform action/gather required information for response
    - Store power save filter in specification set
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Validity checks for optional TLV happens here
    Validity Check when optional TLV (0x010) is passed to the function.
  -------------------------------------------------------------------------*/
  if (got_v_in_opt && v_in_list != NULL)
  {
    if(((ind_set_len - 1)/sizeof(uint16)) != v_in_list->num_inds)
    {    
      LOG_MSG_INFO2_1 ("Invalid num_inds: (%d) ", v_in_list->num_inds);
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }
  }

  /*-------------------------------------------------------------------------
      Validity Check when no optional TLV (0x010) is passed to the function. 
    - Allocate 1 byte for "num_inds" to be passed on for storing the spec
  -------------------------------------------------------------------------*/
  else if (v_in_list == NULL)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(v_in_list, sizeof(v_in_opt_type), v_in_opt_type*);

    if ( v_in_list == NULL)
    {
      errval = QMI_ERR_NO_MEMORY;
      goto send_result;
    }

    memset( v_in_list, 0, sizeof(v_in_opt_type) );

    v_in_list->num_inds = 0;
  }    

  LOG_MSG_INFO2_3( "Got QMI_CTL configure power save mode ext "
                   "(state=0x%X service=0x%X inds=%d)",
                   v_in_required.state_handle,
                   v_in_required.svc_id,
                   v_in_list->num_inds);

  spec_stored = qmi_svc_pwrsave_store_spec( ctl_sp->qmux_svc_handle,
                                            v_in_required.state_handle,
                                            v_in_required.svc_id,
                                            v_in_list->num_inds,
                                            v_in_list->ind_list );
  
  /* Confirm the power save spec was stored successfully. */
  if( !spec_stored )
  {
    LOG_MSG_INFO2_0 ("Error storing power save spec!");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
    
  /*-------------------------------------------------------------------------
    build response
  -------------------------------------------------------------------------*/
send_result:

  if (v_in_list != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(v_in_list);
  }

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  if( FALSE == qmi_svc_put_result_tlv(&response, result, errval) )
  {
    dsm_free_packet(&response);
    return NULL;
  }
  return response;
} /* qmi_ctli_config_pwrsave_mode_ext() */
