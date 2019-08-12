/*===========================================================================

                         D S _ Q M U X . C

DESCRIPTION

  The Data Services Qualcomm MSM Interface control channel message 
  multiplexing protocol source file.

EXTERNALIZED FUNCTIONS

  Upper interface APIs:

  qmux_init()
    Initialize a particular QMUX instance
  qmux_reg_service()
    Register service with a QMUX instance
  qmux_dereg_service()
    Deregister serivce with a QMUX instance
  qmux_send()
    Send a service message (PDU) over a specified QMI link
  qmux_link_down()
    Indicate that the specified QMUX link is down, results in cleanup of
    any outstanding QMUX and registered service resources.

  +----------------------------+
  |  Service(s)                |
  +----------------------------+
    |                        ^
    | qmux_send()            | service.rx_cb
    v                        |
  +----------------------------+
  | QMUX dispatch layer        |
  +----------------------------+
    |                        ^
    | sio_control_write()    | qmuxi_input()
    |                        |
    v                        |
  +----------------------------+
  |  SIO device control stream |
  +----------------------------+

  Lower interface APIs (internal functions):

  qmux_process_rx_sig()
    Called by serving task in response to QMI_RX_SIG signal

INTERNAL FUNCTIONS:

  qmuxi_sio_rx_cb()
    QMUX control rx watermark non-empty callback function - sends the
    QMUX_RX command to the serving task to process the input QMUX PDU
  qmuxi_input()
    Decapsulate the provided QMUX PDU and dispatch to the appropriate
    registered service

Copyright (c) 2004-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/src/ds_qmux.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
05/09/14    wc     Log msg_id in SMEM log
03/25/14    vrk    Fixed a race condition in QMUX_REG_SERVICE
09/27/13    wc     Support dynamic port configuration
04/12/13    wc     Replace rdm_assign_port() with rdm_assign_port_tmp()
03/21/13    wc     QMAP support
12/10/12    svj    Support 8 concurrent PDN's  
10/31/12    wc     Memory optimization
09/13/12    ab     Fixed Race Condition in QMUX_REG_SERVICE 
09/13/12    svj    Changes to block STX and SIM_INTERNAL services from Apps and USB 
                   on non QMI_DEV_MUX2 ports 
09/05/12    wc     Fix DTR race condition
08/21/12    sn     Fix to open Control channel when Data channel is not used.
04/15/12    wc     Make RDM timer non-deferrable
03/06/12    wc     Remove QMI UIM/CAT from Data
11/02/11    wc     Add DTR on SIO control channel feature
10/04/11    sa     Q6 free floating changes for QMI.
09/18/11    wc     Nikel and MPSS.NI.1.1 port configuration
02/09/11    kk     Enabling DROP_PENDING on all opened SMD ports.
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
12/24/10    kk     Featurizing calls to QMI VOICE APIs.
10/13/10    kk     Added log messages before ASSERTs.
06/19/10    vs     Changes for Dual IP support over single QMI instance
11/30/09    ar     Add QMUX PDU framing supprot for streaming devices
10/23/09    ar     Add supprot for power save indication filtering and 
                   port configuration from NVRAM.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
07/02/07    nl     Changes to map SIO_PORT_SMD_WINMOB_QMI_WWAN for 3rd party OS builds 
                   in qmi/rmnet code instead of relying on external modules' featurization. 
01/08/07    jd/ks  Added Winmobile multiple RmNet support
12/12/06    ks     Moved service initialization fn calls to external init().
09/02/06    jd     Changed DNE of RX wm to ensure complete commands received
08/23/06    ks     Changes to support multiple qmi/rmnet instances.
07/12/06    ks     Returning from qmux_process_rx_sig() if qmux is not 
                   initialized.Will temporarily mask the random sig issue.
04/05/06    jd     Dump entire QMUX PDU chain to F3 rather than single item
09/08/05    ks     Moved qmux_process_cmd() to file ds_qmi_task.c.
09/06/05    ks     Changed SIO_BITRATE_AUTO to SIO_BITRATE_BEST.
06/27/05    jd     Added QMI command handler.  Fixed bug in QMUX header ctl 
                   flags.  Process only one QMI command per signal to
                   minimize time spent in processing loop
05/31/05   jd/ks   Code review updates
05/17/05    jd     Make sure client ID callbacks are non-NULL before calling
                   Moved initialization of QMI services here from rmnet_sm
05/11/05    ks     fixed error and changed include file name.
03/14/05    ks     Removed featurization
03/08/05    ks     Clean up
11/21/04    jd     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Define FEATURE_DSM_WM_CB so dsm passes watermark and user data to 
  watermark callbacks
---------------------------------------------------------------------------*/
#define FEATURE_DSM_WM_CB

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

/* This should be the first include to allow stubs on test framework */
#if defined(T_WINNT)
#error code not present
#endif /* WINNT*/

#include "amssassert.h"
#include "msg.h"

#include "queue.h"
#include "rex.h"
#include "task.h"
#include "sio.h"
#include "rdevmap.h"
#include "dsm.h"
#include "ps_byte.h"
#include "ps_utils.h"
#include "modem_mem.h"
#include "ps_crit_sect.h"
#include "pstimer.h"
#include "qmi_modem_task_svc.h"
#include "qmi_modem_task_cmd.h"
#include "qmi_crit_sect.h"
#include "dcc_task_defs.h"
#include "dcc_task_svc.h"

#include "qmi_svc_defs.h"
#include "ds_qmux_ext.h"
#include "ds_qmux.h"
#include "ds_qmux_msg.h"
#include "ds_qmuxi.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_task.h"
#include "qmi_cmd_process.h"
#include "ds_qmi_ctl.h"
#include "ds_qmi_if.h"
#include "ds_qmux_logging.h"
#include "ds_qmi_svc_ext.h"
#include "ds_qmi_nv_util.h"
#include "qmi_charger.h"
#include "qmi_frameworki.h"
#include "dsutil.h"

#ifdef FEATURE_DATA_QMI_APPS_POWER_COLLAPSE
#include "ds_qmi_pwrmgmt_shim.h"
#endif /* FEATURE_DATA_QMI_APPS_POWER_COLLAPSE */

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds_wlan_util.h"
#endif  /* FEATURE_DATA_WLAN_MAPCON */

#include "ds_rmnet_sm_ext.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_rmnet_xport.h"

#ifdef FEATURE_HS_USB
  #include "hsu_config_selector.h"
#endif

#if defined(T_WINNT)
#error code not present
#endif /* WINNT*/

#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"

#include "smem_log.h"
#include "ds_rev_ip_transport_hdlr.h"
#include "ds_qmi_dpm.h"

#include "ttl_map.h"
#include "ps_utils.h"
#include "ds_qmux_qcsi_shim.h"
/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  QMUX Watermark level definitions
---------------------------------------------------------------------------*/
#define QMUX_RX_WM_LOW    (0)
#define QMUX_RX_WM_HIGH   (8192)
#define QMUX_RX_WM_DNE    (102400)

/*---------------------------------------------------------------------------
  Max number of outstanding commands per service
---------------------------------------------------------------------------*/
#define QMUXI_MAX_DSM_ITEMS_PER_SVC  (10)
/*-------------------------------------------------------------------------- 
  DSM SMALL ITEM SIZE TO USE (QMUXI_MAX_DSM_ITEMS_PER_SVC * 
                  QMUX_SERVICE_MAX * 
                  DSM_DS_SMALL_ITEM_SIZ)
---------------------------------------------------------------------------*/
#define QMUXI_TX_DNE           290560

#define SMEM_QMUX_RX_EVENT     (SMEM_LOG_DEBUG_EVENT_BASE +  3)
#define SMEM_QMUX_TX_EVENT     (SMEM_LOG_DEBUG_EVENT_BASE +  4)


/*===========================================================================

                                DATA TYPES

===========================================================================*/

/* This is the default QMI_QMUX command buffer. Make sure that it does not grow
 * beyond 512 bytes. In case you need a command buffer larger than 512 bytes,
 * declare a separate structure. */
typedef struct
{
  uint16  cmd_id; /* qmi_cmd_id_e_type */
  union
  {
    qmi_instance_e_type clean_instance;

  } data;
} qmi_qmux_cmd_buf_type;

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI control and data ports.
---------------------------------------------------------------------------*/
static boolean                 qmi_port_cfg_in_progress = FALSE;
static qmi_port_request_type * qmi_port_cfg_queue = NULL;

/*--------------------------------------------------------------------------
  QMUX state array (one element per QMI link/instance)
---------------------------------------------------------------------------*/
static qmux_state_type  qmux_state[QMI_INSTANCE_MAX];
static uint8            qmux_num_instances = 0;
static boolean          qmux_powerup_inited = FALSE;

/*---------------------------------------------------------------------------
 * To determine FIFO size
---------------------------------------------------------------------------*/
#define IS_TETHERED_PORT(port)                   \
        ( (SIO_PORT_SMD_DATA40 == port) ||       \
          (SIO_PORT_SMD_DATA39 == port) ||       \
          (SIO_PORT_SMD_DATA38 == port) ||       \
          (SIO_PORT_SMD_DATA37 == port) ||       \
          (SIO_PORT_SMD_DATA36 == port) ||       \
          (SIO_PORT_SMD_DATA35 == port) ||       \
          (SIO_PORT_SMD_DATA34 == port) ||       \
          (SIO_PORT_SMD_DATA33 == port) ||       \
          (SIO_PORT_SMD_DATA32 == port) )

/*---------------------------------------------------------------------------
  CRC table to be used for FCS calculation 
  Inserted from 3GPP TS27.010 V5
---------------------------------------------------------------------------*/
#define QMUX_CRC_TABLE_ELEMENTS          256
#define QMUX_FCS_INIT_VAL                (0xFFu)

static uint8  qmux_crc_table[QMUX_CRC_TABLE_ELEMENTS] = 
{   /* reversed, 8-bit, poly=0x07 */
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,  
    0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,  
    0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,  
    0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,  
    0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,

    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,  
    0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,  
    0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,  
    0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,  
    0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,

    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,  
    0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,  
    0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,  
    0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,  
    0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,

    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,  
    0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,  
    0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD,  
    0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,  
    0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};

#if defined(FEATURE_8960_SGLTE_FUSION)
#define QMI_SIM_INTERNAL  35
#define QMI_STX           38
#endif

/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/
static boolean qmuxi_init
(
  qmi_instance_e_type  qmi_instance
);

static void  qmuxi_sio_rx_cb
(
  struct dsm_watermark_type_s *  wm_ptr,
  void *                         user_data_ptr
);

static boolean  qmuxi_input
(
  qmux_state_type *  qmux_s,
  dsm_item_type **   qmux_pdu
);

static void qmuxi_process_msg
( 
  qmux_state_type *  qmux_s,
  dsm_item_type *    qmux_pdu
);

static boolean qmuxi_close_instance
(
  qmi_instance_e_type  qmi_instance
);

static boolean qmuxi_open_instance
(
  qmi_instance_e_type  qmi_instance,
  uint16               sio_port
);

static boolean qmuxi_process_rx_sig
( 
  qmi_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);
static void qmuxi_sio_ioctl
(
  qmux_state_type       * qmux_s,
  sio_ioctl_cmd_type      cmd,
  sio_ioctl_param_type  * ioctl_param
);

static void qmuxi_set_dtr_handler
(
  qmi_instance_e_type    inst,
  sio_vpu_func_ptr_type  dtr_cb
);

static void qmuxi_dtr_cb
(
  void    * user_data
);

static qmi_instance_e_type qmi_instance_by_qmux_state
(
  qmux_state_type * qmux_s
);

static void *qmi_qmux_get_cmd_buf
(
  qmi_cmd_id_e_type cmd_id
);

#define qmi_qmux_free_cmd_buf(buf_ptr) PS_SYSTEM_HEAP_MEM_FREE(buf_ptr)

static void qmi_qmux_process_cmd
(
  void * cmd_ptr
);

static void qmuxi_set_default_port_configuration
(
  void
);

static void qmuxi_port_config_from_queue
(
  void
);


/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMUX_POWERUP_INIT()

  DESCRIPTION
    QMUX power up initialization.

  PARAMETERS
    None

  RETURN VALUE
    TRUE on successful initialization
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_powerup_init
(
  void
)
{
  /*-------------------------------------------------------------------------
    Initialize the QMI state structure.
  -------------------------------------------------------------------------*/
  memset( qmux_state, 0, sizeof(qmux_state) );
  qmux_powerup_inited = TRUE;
 
  return TRUE;
} /* qmux_powerup_init */

/*===========================================================================
  FUNCTION QMUX_INIT()

  DESCRIPTION
    Initialize specified QMUX instance.  SIO control IO path will be set up,
    and QMUX instance state will be reset to default.

  PARAMETERS
    None

  RETURN VALUE
    TRUE on successful initialization
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_init
(
  void
)
{
  uint8              i;
  qmi_cmd_init_legacy_services_type * dcc_cmd_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the QMI state structure.
  -------------------------------------------------------------------------*/
  memset( qmux_state, 0x0, sizeof(qmux_state) );

  /*-------------------------------------------------------------------------
    Initialize the QMI EFS NV conf file.
  -------------------------------------------------------------------------*/
  if (qmi_nv_init_efs_conf_file() != QMI_NV_STATUS_OK)
  {
    LOG_MSG_ERROR_0("Failed to initialize QMI EFS NV conf file");
  }

  /*-------------------------------------------------------------------------
    Initialize the control path
  -------------------------------------------------------------------------*/
  for (i = 0; i < QMI_INSTANCE_MAX; i++)
  {
    if (qmuxi_init((qmi_instance_e_type)i) == FALSE)
    {
      return FALSE;
    }
    qmi_ctl_init((qmi_instance_e_type)i);    /* initialize QMI Control service           */
  }

  /* initialize QMI service on DCC task    */
  (void) dcc_set_cmd_handler( DCC_QMI_INIT_LEGACY_SERVICES_CMD, 
                              qmi_process_init_legacy_services);
  dcc_cmd_ptr = (qmi_cmd_init_legacy_services_type *) modem_mem_alloc(
                                         sizeof(qmi_cmd_init_legacy_services_type),
                                         MODEM_MEM_CLIENT_DATACOMMON );
  if(NULL == dcc_cmd_ptr)
  {
    DATA_ERR_FATAL("QMI can't get modem memory heap allocation");
    return FALSE;
  }
  memset(dcc_cmd_ptr, 0, sizeof(qmi_cmd_init_legacy_services_type));
  dcc_cmd_ptr->num_qmi_instance = QMI_INSTANCE_MAX;
  dcc_send_cmd_ex(DCC_QMI_INIT_LEGACY_SERVICES_CMD, (void *)dcc_cmd_ptr);

  /*-----------------------------------------------------------------------------
    Set cmd hander with DCC task for RmNet(port) instance open and close handling
    RmNet instance open/close should happen after legacy QMI services get initialized
  ------------------------------------------------------------------------------*/
  (void) dcc_set_cmd_handler( DCC_RMNET_XPORT_CMD, rmnet_xport_cmd_hdlr);

#ifdef FEATURE_DATA_WLAN_MAPCON
  /*-----------------------------------------------------------------------------
    Set cmd hander with DCC task for Reverse IP transport module
  ------------------------------------------------------------------------------*/
  (void) dcc_set_cmd_handler( DCC_REV_IP_TRANSPORT_PERFORM_OPERATION_CMD, 
                              rev_ip_transport_process_cmd);
#endif /* FEATURE_DATA_WLAN_MAPCON*/

#ifdef FEATURE_DATA_QMI_APPS_POWER_COLLAPSE
  if (!qmi_pwrmgmt_shim_init())
  {
    LOG_MSG_ERROR_0("Couldn't initialize QMI_PWRMGMT_SHIM module!");
  }
#endif /* FEATURE_DATA_QMI_APPS_POWER_COLLAPSE */
 

  /*-------------------------------------------------------------------------
    Set signal handler for QMUX_RX signal and enable the signal.
    One qmux sig for all instances.
  -------------------------------------------------------------------------*/
  (void)qmi_set_sig_handler(QMI_QMUX_RX_SIGNAL,
                            qmuxi_process_rx_sig,
                            NULL);
  qmi_enable_sig(QMI_QMUX_RX_SIGNAL);

  /*-------------------------------------------------------------------------
    Set cmd handlers with the QMI MODEM task.
  -------------------------------------------------------------------------*/ 
  (void) qmi_set_cmd_handler(QMI_CMD, qmi_process_cmd);
  (void) qmi_set_cmd_handler(QMI_QMUX_QCSI_SHIM_CMD, qmux_qcsi_shim_process_cmd);

  (void) qmi_task_set_qmux_cmd_handler(qmi_qmux_process_cmd);

  /*-------------------------------------------------------------------------
    Open default ports if they are configured -- for testing purpose
  -------------------------------------------------------------------------*/ 
  qmuxi_set_default_port_configuration();
  (void)qmi_dpm_init();
  qmux_qcsi_shim_init();
  return TRUE;
}

/*===========================================================================
  FUNCTION QMUX_REG_VS_SERVICE_LIST()

  DESCRIPTION
    Register service table with QMUX

  PARAMETERS
    svc_list             : array pointer to all services for qmi instance
    num_services         : total number of services in list that
                           are to be registered
    qmi_instance         : instance of QMI

  RETURN VALUE
    TRUE if table registeration was successful
    FALSE if table format/content was invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_reg_vs_service_list
(
  qmi_instance_e_type  qmi_instance,
  qmux_svc_info_type * svc_list,
  uint8                num_services
)
{
  LOG_MSG_INFO1_0("qmux_reg_vs_service_list is deprecated");
  return TRUE;
}/* qmux_reg_service_list() */


/*===========================================================================
  FUNCTION QMUX_REG_VS_SERVICE()

  DESCRIPTION
    Register the vendor specific service instance identified by 'service' 
    with the qmux instance identified by 'qmi_instance' with the 
    configuration provided in 'cfg'.     
    Function is used to register VS Services ONLY
   
  PARAMETERS
    qmi_instance : instance of QMI
    service      : service type
    cfg          : configuration parameters for the service instance

  RETURN VALUE
    NULL           -  service registration failed
    service handle -  qmux service handle 

  DEPENDENCIES
    qmux_reg_service_list() must be called to register service list before
    qmux is able to use it.

  SIDE EFFECTS
    None
===========================================================================*/
void * qmux_reg_vs_service
(
  qmi_instance_e_type     qmi_instance,
  qmux_service_e_type     service,
  qmux_svc_config_type *  cfg
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  return qmux_reg_service(qmi_instance, service, cfg);

} /* qmux_reg_vs_service() */



/*===========================================================================
  FUNCTION QMUX_REG_SERVICE()

  DESCRIPTION
    Register the service instance identified by 'service' with the qmux
    instance identified by 'qmi_instance' with the configuration provided
    in 'cfg'.
    
  PARAMETERS
    qmi_instance : instance of QMI
    service      : service type
    cfg          : configuration parameters for the service instance

  RETURN VALUE
    NULL           -  service registration failed
    service handle -  qmux service handle 

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
void * qmux_reg_service
(
  qmi_instance_e_type     qmi_instance,
  qmux_service_e_type     service,
  qmux_svc_config_type *  cfg
)
{
  qmux_svc_info_type *  svc_s;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (service >= QMUX_SERVICE_MAX)
  {
    LOG_MSG_ERROR_1("Service ID %d out of range!", service);
    return NULL;
  }

  /* check that qmi_instance is within range in case ASSERT is removed */
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1 ("QMI instance out of range (%d)!", qmi_instance);
    return NULL;
  }
  if( qmux_powerup_inited != TRUE )
  {
    ERR ("QMUX Powerup initialization has not happened!", 0, 0, 0);
    return NULL;
  }

  /* check that cfg pointer is not NULL in case ASSERT is removed */
  if (cfg == NULL)
  {
    LOG_MSG_ERROR_0("QMI service cfg is NULL!");
    return NULL;
  }

  if( !qmux_state[qmi_instance].initialized )
  {
    LOG_MSG_ERROR_1 ("QMI link %d not initialized!", qmi_instance);
    return NULL;
  }

  svc_s = qmux_state[qmi_instance].service[service];

  /* Alloc service info memory */
  if ( svc_s == NULL)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(svc_s, sizeof(qmux_svc_info_type),
                             qmux_svc_info_type*);
    if (svc_s == NULL)
    {
      return NULL;
    }

    memset(svc_s, 0, sizeof(qmux_svc_info_type));
    svc_s->service_id = service;  /* QMUX service type */
    svc_s->cfg        = *cfg;     /* store the provided config */
    qmux_state[qmi_instance].service[service] = svc_s;
  }


  if( svc_s->registered == TRUE)
  {
    LOG_MSG_ERROR_2 ("Service %d already registered with QMUX#%d!",
                     service, qmi_instance);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    validate passed config
    - service must have a non-null receive function.
    - Everything else is optional
    - if dealloc callback is not specified (using the service library),
      must specify a reset clid callback
  -------------------------------------------------------------------------*/
  if ( ((cfg->cbs.recv == NULL) && 
        ((cfg->cmd_hdlr_array == NULL) || (cfg->cmd_num_entries == 0))))/* ||
       ((cfg->cbs.recv != NULL) && 
        ((cfg->cmd_hdlr_array != NULL) || (cfg->cmd_num_entries != 0))) )*/
  {
    LOG_MSG_ERROR_0 ("Invalid QMI service config");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    if the service is not QCTL, then when it's using the common dealloc_clid,
    reset client must be registered, otherwise it would cause an error
  -------------------------------------------------------------------------*/
  if (service != QMUX_SERVICE_CTL)
  {
    if ((cfg->cbs.dealloc_clid == NULL) && (cfg->cbs.reset_client == NULL))
    {
      LOG_MSG_ERROR_0 ("Invalid QMI service config");
      return NULL;
    }
  }
  

  /*-------------------------------------------------------------------------
    state pointer will be passed back in callbacks to tie this to the 
    associated service instance
  -------------------------------------------------------------------------*/
  svc_s->registered = TRUE;     /* indication that service is registered */
  
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
  /*Notify Control Point about service availability*/
  qmux_notify_service_registration
               (svc_s->service_id,
                qmi_instance,
                TRUE); // Send TRUE here as this is an old QMUX based Service
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

  /*-------------------------------------------------------------------------
    Return service handle which service should use to deregister, etc.
  -------------------------------------------------------------------------*/
  return svc_s;

} /* qmux_reg_service() */

/*===========================================================================
  FUNCTION QMUX_DEREG_SERVICE()

  DESCRIPTION
    Deregister the previously registered service instance identified by 
    the passed service handle.

  PARAMETERS
    service handle -  qmux service handle returned by qmux_reg_service()

  RETURN VALUE
    None

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
void qmux_dereg_service
(
  void *  service_handle
)
{
  qmi_instance_e_type   qmi_instance;
  qmux_svc_info_type *  svc_s;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (service_handle);

  /* check that service_handle is not NULL in case ASSERT is removed */
  if (service_handle == NULL)
  {
    LOG_MSG_ERROR_0 ("Deregistering NULL service handle!");
    return;
  }

  qmi_instance = qmi_instance_by_qmux_handle( service_handle );
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1 ("Deregistering invalid service handle (0x%x)!",
                     service_handle);
    return;
  }

  svc_s = (qmux_svc_info_type *) service_handle;
  ASSERT (svc_s->registered);
  LOG_MSG_INFO2_2 ("Deregistering service %d from QMUX#%d",
                   svc_s->service_id, qmi_instance);

  /*-------------------------------------------------------------------------
    Clear service info block
  -------------------------------------------------------------------------*/
  memset (svc_s, 0, sizeof(qmux_svc_info_type));

} /* qmux_dereg_service() */

#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
/*===========================================================================
FUNCTION QMUX_NOTIFY_SERVICE_REGISTRATION()

DESCRIPTION
  Function to learn about qmi service registration with qmux. This function 
  posts a command to qmi_modem task to notify TE about the new 
  service availability. 

PARAMETERS 
  service_id   : QMI Service ID
  qmi_instance : QMI Instance (ignored for new QMUX based service)
  is_legacy    : The value is true if legacy QMI service,
                 else FALSE for new QMUX based Service
 
RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmux_notify_service_registration
(
  qmux_service_e_type                   service_id,
  qmi_instance_e_type                   qmi_instance,
  boolean                               is_legacy_svc
)
{
  qmi_cmd_data_buf_type         *  cmd_ptr;
/*-------------------------------------------------------------------------*/

  if (service_id >= QMUX_SERVICE_MAX)
  {
    LOG_MSG_ERROR_1("Unknown service [%d]", service_id);
    ASSERT(0);
    return;
  }
  cmd_ptr = qmi_get_cmd_data_buf();
  if (cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_1 ("No cmd buffer! service %d response message ",
                     service_id);
    ASSERT(0);
    return;
  }
  /*-------------------------------------------------------------------------
    Sending svc ID, qmi instance and is_legacy_svc response 
  -------------------------------------------------------------------------*/

  LOG_MSG_INFO1_1("Posting cmd [%d] to QMI Modem task.",
                  QMI_CMD_CTL_SVC_AVAILABLE);

  cmd_ptr->cmd.qmi.id = QMI_CMD_CTL_SVC_AVAILABLE;
  cmd_ptr->cmd.qmi.data.ctl_avail_ind.is_legacy_svc  = is_legacy_svc;
  cmd_ptr->cmd.qmi.data.ctl_avail_ind.service_id     = service_id;
  cmd_ptr->cmd.qmi.data.ctl_avail_ind.qmi_inst       = qmi_instance;
  qmi_send_cmd(QMI_CMD, cmd_ptr);

} /* qmux_notify_service_registration() */
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

/*===========================================================================
  FUNCTION QMUX_CALCULATE_FCS()

  DESCRIPTION
    Calculate frame checksum (FCS) over the passed SDU packet.
    Refer to 3GPP TS27.010 V5 Annex B for details of 8-bit FCS algorithm.

  PARAMETERS
    qmux_sdu       : service PDU to be sent
    fcs_ptr        : pointer to FCS result    
    
  RETURN VALUE
    TRUE on successful calcaulation
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmux_calculate_fcs
(
  dsm_item_type * qmux_sdu,
  uint8         * fcs_ptr
)
{
  uint32  fcs_num_bytes = 0;
  uint16  peek_offset = 0;
  uint8   fcs = QMUX_FCS_INIT_VAL;
  uint8   octet_buf = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( fcs_ptr );

  LOG_MSG_INFO3_0("qmux_calculate_fcs");
  
  fcs_num_bytes = dsm_length_packet( qmux_sdu );

  /* Loop over SDU size */
  while( fcs_num_bytes--)
  {
    /* Note: peek_offset is being incremented */
    if( FALSE == dsm_peek_byte( qmux_sdu, peek_offset++, &octet_buf ) )
    {
      LOG_MSG_INFO1_0("Error getting SDU octet");
      return FALSE;
    }

    fcs = qmux_crc_table[fcs ^ octet_buf];
  }

  /* Apply one's complement */
  fcs = QMUX_FCS_INIT_VAL - fcs;

  /* Return FCS value to caller */
  *fcs_ptr = fcs;
  return TRUE;
}


/*===========================================================================
  FUNCTION QMUX_FRAME_SDU()

  DESCRIPTION
    Apply framing to the QMUX SDU based on the framing mode/IFtype.
    There are two framing modes supported:
    
    (1) No framing - The packet is framed by the transport driver so
                     no PDU framing is required.

    (2) Frame Mode - The PDU is framed using start and stop flags, and
                     an checksum is computed.  The octects are
                     arranged as shown below.  No octet transparency
                     (escaping) is done in this format.

                     
       |Start|I/F  | QMUX PDU                     |     |Stop |
       |Flag |Type | Length   Info                | FCS |Flag |
       +-----+-----+--------+---------------------+-----+-----+
       |  1  |  1  |    2   | specified in Length |  1  |  1  |octets
       +-----+-----+--------+---------------------+-----+-----+
                     

  PARAMETERS
    qmux_sdu_pptr  : pointer to service PDU
    
  RETURN VALUE
    TRUE on successful framing operaiton
    FALSE otherwise

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmux_frame_sdu
(
  dsm_item_type **        qmux_sdu_pptr
)
{
  uint8   sdu_fcs = 0;
  uint8   flag = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( qmux_sdu_pptr );

  LOG_MSG_INFO3_0("qmux_frame_sdu");
  
  /*-------------------------------------------------------------------------
    Calculate FCS over SDU.  This includs the IFtype field and QMUX PDU.
  -------------------------------------------------------------------------*/
  if( !qmux_calculate_fcs( *qmux_sdu_pptr, &sdu_fcs ) )
  {
    DATA_ERR_FATAL("Error calculating FCS value");
    return FALSE;
  }
  
  /* Append FCS to SDU */
  if( sizeof(sdu_fcs) != dsm_pushdown_tail( qmux_sdu_pptr, 
                                            &sdu_fcs, 
                                            sizeof(sdu_fcs), 
                                            DSM_DS_SMALL_ITEM_POOL ) )
  {
    LOG_MSG_INFO1_0 ("Out of memory on SDU frame FCS");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Add start and stop flags to SDU
  -------------------------------------------------------------------------*/
  flag = QMUX_FRAME_FLAG_START;
  if( sizeof(flag) != PS_UTILS_DSM_PUSHDOWN_PACKED( qmux_sdu_pptr, 
                                    &flag, 
                                    sizeof(flag), 
                                    DSM_DS_SMALL_ITEM_POOL))
  {
    return FALSE;
  }
  
  flag = QMUX_FRAME_FLAG_STOP;
  if( sizeof(flag) != dsm_pushdown_tail( qmux_sdu_pptr, 
                                         &flag, 
                                         sizeof(flag), 
                                         DSM_DS_SMALL_ITEM_POOL ) )
  {
    LOG_MSG_INFO1_0 ("Out of memory on SDU frame append stop flag");
    return FALSE;
  }
  
  return TRUE;
} /* qmux_frame_sdu() */


/*===========================================================================
  FUNCTION QMUX_UNFRAME_SDU()

  DESCRIPTION
    Undo framing on the QMUX SDU based on the framing mode/iftype.
    See QMUX_FRAME_SDU() for discussion of frame structure.

    During unframing the START and STOP flags are tested for, the
    latter located using the length field within the QMUX PDU. If the
    QMUX PDU length exceeds the unframed SDU size, it is assumed the
    input is a partial SDU. Given there is no octet transparency done
    in framing, it may be the frame STOP flag appears within the frame
    payload.  This causes the SIO driver to deliver a partial SDU.
    When detetected, the partial SDU is cached for prepend on
    subsequent SDU delivery, and no SDU is passed back to the caller.

    Finally the FCS calcaulated for the unframed SDU is compared to
    the frame FCS field.  If values do not match, the SDU is discarded
    and no SDU is passed back to the caller.
    

  PARAMETERS
    qmux_sdu_pptr     : pointer to service PDU
    pending_sdu_pptr  : pointer to cached patrial PDU
    
  RETURN VALUE
    TRUE on successful unframing operaiton
    FALSE otherwise

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmux_unframe_sdu
(
  dsm_item_type **  qmux_sdu_pptr,	
  dsm_item_type **  pending_sdu_pptr
)
{
  uint8   sdu_fcs = 0;
  uint8   expected_fcs = 0;
  uint8   flag = 0;
  uint32  sdu_len = 0;   /* framed SDU length   */
  uint16  pdu_len = 0;   /* unframed PDU length */
  uint16  seek_len = sizeof(pdu_len);
  uint16  offset = QMUX_FRAME_PDU_LEN_OFFSET;
  dsm_item_type * sdu_ptr = NULL;
  dsm_item_type * pdu_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( qmux_sdu_pptr );
  ASSERT( *qmux_sdu_pptr );
  ASSERT( pending_sdu_pptr );

  LOG_MSG_INFO3_0("qmux_unframe_sdu");
  
  /*-------------------------------------------------------------------------
    If exists, prepend partial SDU from previous delivery. 
  -------------------------------------------------------------------------*/
  if( *pending_sdu_pptr )
  {
    LOG_MSG_INFO2_0("Prepending previous SDU to current");

    sdu_ptr = *pending_sdu_pptr;
    *pending_sdu_pptr = NULL;

    dsm_append( &sdu_ptr, qmux_sdu_pptr);
  }
  else
  {
    sdu_ptr = *qmux_sdu_pptr;
    *qmux_sdu_pptr = NULL; 
  }

  ASSERT( sdu_ptr );
  
  /*-------------------------------------------------------------------------
    Verify START and STOP flags are at head and tail of SDU.
  -------------------------------------------------------------------------*/
  if( !dsm_peek_byte(sdu_ptr, QMUX_FRAME_PDU_START_OFFSET, &flag) )
  {
    LOG_MSG_INFO2_0("Error extracting QMUX PDU start flag");
    dsm_free_packet( &sdu_ptr );
    return FALSE;
  }

  if( QMUX_FRAME_FLAG_START != flag )
  {
    LOG_MSG_INFO2_1("QMUX PDU start flag mismatch: 0x%X", flag);
    dsm_free_packet( &sdu_ptr );
    return FALSE;
  }

  sdu_len = dsm_length_packet( sdu_ptr );

  /*-------------------------------------------------------------------------
    Suppressing STOP flag validation for now.  Driver may deliver DSM
    packet based on stale data timeout, in which case there will be no
    valid STOP flag.  We will handle this case as a partial PDU
    delivery, caching the DSM packet for the next delivery cycle.
    This approach relies on the PDU length valiation to catch PDU
    corruption.
  -------------------------------------------------------------------------*/
#if 0
  if( !dsm_peek_byte (sdu_ptr,
		      (uint16)(sdu_len-QMUX_FRAME_PDU_STOP_OFFSET),
		      &flag ) )
  {
    LOG_MSG_INFO2_0("Error extracting QMUX PDU start flag");
    dsm_free_packet( &sdu_ptr );
    return FALSE;
  }
  
  if( QMUX_FRAME_FLAG_STOP != flag )
  {
    LOG_MSG_INFO2_1("QMUX PDU stop flag mismatch: 0x%X", flag);
    dsm_free_packet( &sdu_ptr );
    return FALSE;
  }
#endif /* 0 */
  
  /*-------------------------------------------------------------------------
    Determine length of QMUX PDU and compare to unframed SDU size.  If
    expected length exceeds SDU size, assume this is a partial SDU.
    Cache DSM packet in state info for next SDU delivery.
    Discard SDU if expected length less than SDU size.
  -------------------------------------------------------------------------*/
  if( !dsm_seek_extract( &sdu_ptr, &offset, &pdu_len, &seek_len) )
  {
    LOG_MSG_INFO2_0("Error extracting QMUX PDU length");
    dsm_free_packet( &sdu_ptr );
    return FALSE;
  }
    
  if( (sdu_len-QMUX_FRAME_PDU_LEN_OVERHEAD) != pdu_len )
  {
    /* Check for too small SDU; discard case */
    if( (uint32)(sdu_len-QMUX_FRAME_PDU_LEN_OVERHEAD) > pdu_len )
    {
      LOG_MSG_INFO2_2("SDU length greater than expected size: %d > %d",
                      	 (sdu_len-QMUX_FRAME_PDU_LEN_OVERHEAD), pdu_len);
      dsm_free_packet( &sdu_ptr );
      return FALSE;
    }

    /* Assume this is a partial SDU with STOP flag embedded */
    LOG_MSG_INFO2_0("Caching current SDU for next segmet");
    *pending_sdu_pptr = sdu_ptr;
    return TRUE;
  }
  
  /*-------------------------------------------------------------------------
    Extract just QMUX PDU and IFtype fields into new packet.  This will be 
    used for FCS calculation.
  -------------------------------------------------------------------------*/
  if( (pdu_len+1) !=
      dsm_dup_packet_pool( &pdu_ptr, DSM_DS_SMALL_ITEM_POOL,
                    sdu_ptr, QMUX_FRAME_PDU_IF_OFFSET, (pdu_len+1) ) )
  {
    LOG_MSG_INFO2_0("Error creating QMUX PDU");
    dsm_free_packet( &pdu_ptr );
    dsm_free_packet( &sdu_ptr );
    return FALSE;
  }


  /*-------------------------------------------------------------------------
    Calculate FCS over PDU and compare to FCS in SDU.  This includes
    the IFtype field and QMUX PDU.  If value does not match FCS from
    SDU field, discard SDU.
  -------------------------------------------------------------------------*/
  if( !dsm_peek_byte (sdu_ptr,
		      (uint16)(sdu_len-QMUX_FRAME_PDU_FCS_OFFSET),
		      &expected_fcs ) )
  {
    LOG_MSG_INFO2_0("Error extracting QMUX PDU FCS");
    dsm_free_packet( &pdu_ptr );
    dsm_free_packet( &sdu_ptr );
    return FALSE;
  }
    
  if( !qmux_calculate_fcs( pdu_ptr, &sdu_fcs ) )
  {
    LOG_MSG_INFO2_0("Error calculating FCS on PDU");
    dsm_free_packet( &pdu_ptr );
    dsm_free_packet( &sdu_ptr );
    return FALSE;
  }

  if( expected_fcs !=  sdu_fcs )
  {
    LOG_MSG_INFO2_2("FCS mismatch on SDU: 0x%X != 0x%X",
                    expected_fcs, sdu_fcs);
    dsm_free_packet( &pdu_ptr );
    dsm_free_packet( &sdu_ptr );
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Pass validated QMUX PDU to caller.
  -------------------------------------------------------------------------*/
  *qmux_sdu_pptr = pdu_ptr; 
  dsm_free_packet( &sdu_ptr );
  
  return TRUE;
} /* qmux_unframe_sdu() */



/*===========================================================================
  FUNCTION QMUX_SIO_SEND()

  DESCRIPTION
    Package and send the provided service layer PDU as a QMUX message on the
    link inferred by service.

  PARAMETERS
    qmi_instance   : qmi instance
    service        : service id
    qmux_sdu       : service PDU to be sent
    clid           : Client ID for which the message is destined
    
  RETURN VALUE
    None

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
void qmux_sio_send
(
  qmi_instance_e_type   qmi_instance,
  qmux_service_e_type   service, 
  uint32                clid,
  dsm_item_type *       qmux_sdu
)
{
  
  qmux_hdr_type         qmux_hdr;
  byte                  iftype;
  uint8                 msg_ctl;
  uint8                 tx_id_hb;
  uint8                 tx_id_lb;
  uint16                tx_id;
  uint8                 msg_id_hb;
  uint8                 msg_id_lb;
  uint16                msg_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT (qmi_instance < QMI_INSTANCE_MAX);

  /* check that qmi_instance is within range in case ASSERT is removed */
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!  Discarding",
                    qmi_instance);
    dsm_free_packet (&qmux_sdu);
    return;
  }

  if( dsm_length_packet(qmux_sdu) == 0)
  {
    LOG_MSG_INFO1_2 ("0-length QMUX SDU (%d type, clid %d)!  Discarding",
                     service, clid);
    dsm_free_packet (&qmux_sdu);
    return;
  }

  /*Enter crit section as port related info is being accessed in qmux_state*/
  QMI_ENTER_CRIT_SECTION(&qmi_port_cfg_crit_section);

  if (qmux_state[qmi_instance].io.sio_handle == SIO_NO_STREAM_ID)
  {
    LOG_MSG_INFO1_0( "SIO control stream not open!  freeing tx pkt");
    dsm_free_packet (&qmux_sdu);
    QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);
    return;
  }

  if (FALSE == qmux_state[qmi_instance].io.ports_active)
  {
    LOG_MSG_INFO1_0( "Ports not active!  freeing tx pkt");
    dsm_free_packet (&qmux_sdu);
    QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);
    return;
  }

  /*-------------------------------------------------------------------------
    Build and insert QMUX header.  Length is total of qmi header + payload.
  -------------------------------------------------------------------------*/
  qmux_hdr.len       = (uint16) (dsm_length_packet(qmux_sdu) + sizeof(qmux_hdr_type));
  qmux_hdr.ctl_flags = (QMUXI_CFLAG_SENDER_SERVICE) & ~QMUXI_CFLAG_MASK_RESERVED;
  qmux_hdr.svc_type  = service;
  qmux_hdr.clid      = (uint8) clid;

  /*-------------------------------------------------------------------------
    Log the QMUX hdr info, msg_ctl, QMI instance and tx_id in SMEM
  -------------------------------------------------------------------------*/
  dsm_peek_byte(qmux_sdu, 0, &msg_ctl );
  if (qmux_hdr.svc_type == QMUX_SERVICE_CTL)
  {
    // Tx id in CTL is 1 byte
    dsm_peek_byte(qmux_sdu, 1, &tx_id_lb );
    tx_id_hb = 0;
    dsm_peek_byte(qmux_sdu, 2, &msg_id_lb );
    dsm_peek_byte(qmux_sdu, 3, &msg_id_hb );
  }
  else
  {
    dsm_peek_byte(qmux_sdu, 1, &tx_id_lb );
    dsm_peek_byte(qmux_sdu, 2, &tx_id_hb );
    dsm_peek_byte(qmux_sdu, 3, &msg_id_lb );
    dsm_peek_byte(qmux_sdu, 4, &msg_id_hb );
  }

  tx_id = ((((uint16)(tx_id_hb) & 0x00FF) << 8) | (uint16)tx_id_lb);
  msg_id = ((((uint16)(msg_id_hb) & 0x00FF) << 8) | (uint16)msg_id_lb);
  SMEM_LOG_EVENT6(SMEM_QMUX_TX_EVENT, 
                  qmux_hdr.len, msg_id,
                  qmux_hdr.svc_type, qmux_hdr.clid,
                  ((((uint32)(qmi_instance) & 0x0000FFFF) << 16) | (uint32)msg_ctl),
                  tx_id);

  if (sizeof(qmux_hdr) != PS_UTILS_DSM_PUSHDOWN_PACKED( &qmux_sdu, 
                                (void *) &qmux_hdr, 
                                 sizeof(qmux_hdr),
                                 DSM_DS_SMALL_ITEM_POOL))
  {
    dsm_free_packet (&qmux_sdu);
    QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);
    return;
  }

  /*------------------------------------------------------------------------- 
    Add iftype (not part of qmux header)
  -------------------------------------------------------------------------*/
  /* The port configuration framing mode value is synonymous with iftype. */
  iftype = (byte)qmux_state[ qmi_instance ].io.port_info.frame_mode;

  if( sizeof(byte) != PS_UTILS_DSM_PUSHDOWN_PACKED( &qmux_sdu, 
                                   &iftype, 
                                   sizeof(byte), 
                                   DSM_DS_SMALL_ITEM_POOL))
  {
    dsm_free_packet( &qmux_sdu );
    QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);
    return;
  }

  /*-------------------------------------------------------------------------
    Log the outgoing QMI message before passing to SIO control.
  -------------------------------------------------------------------------*/
  qmi_log_packet(qmux_sdu, qmi_instance, QMUX_TX);

  /*-------------------------------------------------------------------------
    Check for requirement to perform framing on PDU.  This will be
    preconfigured for each QMI port.
  -------------------------------------------------------------------------*/
  if( IS_FRAMING_REQUIRED( iftype ) )
  {
    if( !qmux_frame_sdu( &qmux_sdu ) )
    {
      LOG_MSG_INFO1_2 ("Failed on PDU framing(%d type, clid %d) Discarding",
                       service, clid);
      dsm_free_packet (&qmux_sdu);
      QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);
      return;
    }
  }

  #if 0
    LOG_MSG_INFO2_0("Sending QMUX");
    pdu_p = qmux_sdu;
    while (pdu_p)
    {
      memdump( pdu_p->data_ptr, pdu_p->used );
      pdu_p = pdu_p->pkt_ptr;
    }
  #endif

  /*-------------------------------------------------------------------------
    Send the encapsulated response
  -------------------------------------------------------------------------*/
  if( QMUX_DEVSTREAM_CONTROL != qmux_state[qmi_instance].io.port_info.qmi_stream )
  {
    sio_transmit( qmux_state[qmi_instance].io.sio_handle, qmux_sdu );
  }
  else
  {
    sio_control_transmit( qmux_state[qmi_instance].io.sio_handle, qmux_sdu );
  }

  QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);

} /* qmux_sio_send */

/*===========================================================================
  FUNCTION QMUX_SEND()

  DESCRIPTION
    Package and send the provided service layer PDU as a QMUX message on the
    link inferred by service_handle

  PARAMETERS
    service_handle : handle to the service's qmux link
    qmux_sdu       : service PDU to be sent
    clid           : Client ID for which the message is destined
    
  RETURN VALUE
    None

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
void qmux_send 
(
void *           service_handle,
dsm_item_type *  qmux_sdu,
byte             clid
)
{
   qmux_svc_info_type *  svc_s;
   qmi_instance_e_type   qmi_instance;
   
   ASSERT(service_handle);
   ASSERT(qmux_sdu);

  /* check that service_handle is not NULL in case ASSERT is removed */
  if (service_handle == NULL)
  {
    LOG_MSG_ERROR_0 ("QMUX send with NULL service handle!");
    dsm_free_packet (&qmux_sdu);
    return;
  }

  /* check that qmux_sdu is not NULL in case ASSERT is removed */
  if (qmux_sdu == NULL)
  {
    LOG_MSG_ERROR_0("QMUX SDU NULL!");
    return;
  }

   svc_s = (qmux_svc_info_type *) service_handle;
   qmi_instance = qmi_instance_by_qmux_handle (svc_s);
   qmux_sio_send(qmi_instance,svc_s->service_id, clid, qmux_sdu); 
   return;
} /* qmux_send() */


/*===========================================================================
  FUNCTION QMUX_LINK_DOWN

  DESCRIPTION
    Callback to cleanup QMUX and associated service resources when their 
    serving QMI link is disconnected

  PARAMETERS
    instance - the QMI instance whose link was disconnected

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    QMI link disconnection closes all QMI application state
===========================================================================*/
void qmux_link_down
(
  qmi_instance_e_type  instance
)
{
  qmux_state_type *     qmux_s;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(instance < QMI_INSTANCE_MAX);

  /* check that instance is within range in case ASSERT is removed */
  if (instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!  Discarding", instance);
    return;
  }

  LOG_MSG_INFO1_1 ("QMI instance %d link down - reset all services", instance);
  qmux_s = &qmux_state[instance];

  qmux_clean_instance_resources(instance);

  /*-------------------------------------------------------------------------
    Free any QMI messages still in RX watermark.
  -------------------------------------------------------------------------*/
  dsm_empty_queue( &qmux_s->io.rx_wm );

} /* qmux_link_down() */

/*===========================================================================
   FUNCTION QMUX_CLEAN_LEGACY_SVC_RESOURCES

  DESCRIPTION
     Cleanup QMUX and associated service resources related to Legacy Services

  PARAMETERS
    instance - the QMI instance whose link was disconnected

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    Closes all QMI Legacy Service application state
===========================================================================*/
void  qmux_clean_legacy_svc_resources
(
  qmi_instance_e_type  instance
)
{
  qmux_state_type *     qmux_s;
  qmux_svc_info_type *  svci;
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check that instance is within range */
  if (instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!  Discarding", instance);
    return;
  }

  LOG_MSG_INFO1_1 ("QMI Legacy Service cleanup on instance %d", instance);
  qmux_s = &qmux_state[instance];

  /*-------------------------------------------------------------------------
    Cleanup each service's state
  -------------------------------------------------------------------------*/
  for (i = 0; i < QMUX_SERVICE_MAX; i++)
  {
    svci = qmux_s->service[i];
    if (svci == NULL)
    {
      continue;
    }

    if (svci->registered == TRUE)
    {
      if( svci->cfg.cbs.qmux_closed )
      {
        svci->cfg.cbs.qmux_closed( svci->cfg.sp );
      }
      else
      {
        qmi_svc_qmi_link_closed(svci->cfg.sp);
      }
    }
  }
}/*qmux_clean_legacy_svc_resources*/

/*===========================================================================
  FUNCTION QMUX_CLEAN_INSTANCE_RESOURCES

  DESCRIPTION
    Cleanup QMUX and associated service resources

  PARAMETERS
    instance - the QMI instance whose link was disconnected

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    Closes all QMI application state
===========================================================================*/
void qmux_clean_instance_resources
(
  qmi_instance_e_type  instance
)
{
  qmi_qmux_cmd_buf_type * cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check that instance is within range */
  if (instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!  Discarding", instance);
    return;
  }

  LOG_MSG_INFO1_1 ("QMI clean instance %d resources", instance);

  /*-------------------------------------------------------------------------
    Posting to DCC for QMI Legacy service Cleanup when link closed.
  -------------------------------------------------------------------------*/
  cmd_ptr = (qmi_qmux_cmd_buf_type *)qmi_qmux_get_cmd_buf(QMI_CMD_LEGACY_SVC_QMI_LINK_CLOSED);
  if(cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Out of QMUX cmd buf");
    return;
  }
  cmd_ptr->cmd_id = QMI_CMD_LEGACY_SVC_QMI_LINK_CLOSED;
  cmd_ptr->data.clean_instance = instance;

  dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
  
  /*-------------------------------------------------------------------------
    QMI FW service Cleanup when link closed.
  -------------------------------------------------------------------------*/
  qmi_framework_svc_qmi_link_closed(instance);

} /* qmux_clean_instance_resources */


#if 0 //Not used at present

/*===========================================================================
  FUNCTION ARRAY_IX_BY_PTR()

  DESCRIPTION
    To obtain an array index from a pointer to any field within the array

  PARAMETERS
    ptr          - pointer to resolve ix for
    array_base   - base address of array
    element_size - array element size
    n_elements   - number of elements in the array
    
  RETURN VALUE
    index of array element containing ptr
    -1 if handle is invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int array_ix_by_ptr
(
  uint8 *  ptr,
  uint8 *  array_base,
  int      element_size,
  int      n_elements
)
{
  uint32  retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate this does out of range ok
  -------------------------------------------------------------------------*/
  retval = ((ptr - array_base) / element_size);
  if (retval >= n_elements)
  {
    return -1;
  }

  return retval;
} /* array_ix_by_ptr() */
#endif //#if 0, Not used present


/*===========================================================================
  FUNCTION QMUX_GET_SERVICE_CFG()

  DESCRIPTION
    Get Service configuration

  PARAMETERS
    qmux_handle : handle assigned to each service by qmux
    svc_type    : type of qmux service    

  RETURN VALUE
    qmux_svc_config_type
    NULL if input service handle or service type is invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmux_svc_config_type *  qmux_get_service_cfg
(
  void *               qmux_handle,
  qmux_service_e_type  svc_type
)
{
  qmi_instance_e_type  qmi_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (qmux_handle);

  /* check that qmux_handle is not NULL in case ASSERT is removed */
  if (qmux_handle == NULL)
  {
    LOG_MSG_ERROR_0("QMUX handle NULL!");
    return NULL;
  }

 /*---------------------------------------------------------------------
    Return NULL if:
     - svc_type is not Internal QC service range or not in VS range
  ---------------------------------------------------------------------*/
  if (svc_type >= QMUX_SERVICE_MAX)
  {
    return NULL;
  }

  qmi_instance = qmi_instance_by_qmux_handle(qmux_handle);
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    return NULL;
  }

  /*---------------------------------------------------------------------
     First determine if svc_type is referring to QC internal services or 
     external services and then return appropriate cfg.
  ---------------------------------------------------------------------*/
  if ( (qmux_state[qmi_instance].service[svc_type] != NULL) &&
       (qmux_state[qmi_instance].service[svc_type]->registered == TRUE) )
  {
    return &qmux_state[qmi_instance].service[svc_type]->cfg;
  }

  return NULL;

} /* qmux_get_service_cfg() */



/*===========================================================================
  FUNCTION QMUX_PROCESS_RX_SIG

  DESCRIPTION
    Called when QMUX RX signal is set - will check each QMI link's rx 
    watermark and process all outstanding commands.

    Runs in QMI Task.

  PARAMETERS  
    None

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    TRUE if signal processed successfully
    FALSE otherwise

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmuxi_process_rx_sig
( 
  qmi_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
  qmux_state_type *    qmux_s;
  dsm_item_type *      qmux_pdu;
  qmi_instance_e_type  qmi_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qmux_s = qmux_state;
  
  /*-------------------------------------------------------------------------
    Process each QMI link
  -------------------------------------------------------------------------*/
  for (qmi_instance = QMI_INSTANCE_MIN;
       qmi_instance < QMI_INSTANCE_MAX;
       qmi_instance++, qmux_s++)
  {
    if(qmux_s->initialized == FALSE)
    {
      /* Skip uninitialized instances. */
      continue;
    }

    /*---------------------------------------------------------------------
      Retrieve and dispatch the pending QMUX command
    ---------------------------------------------------------------------*/
    qmux_pdu = dsm_dequeue( &qmux_s->io.rx_wm );
    if (qmux_pdu == NULL)
    {
      continue;
    }

    /*---------------------------------------------------------------------
      Unframe the SDU if the port has been preconfigured for framing.
    ---------------------------------------------------------------------*/
    if( IS_FRAMING_REQUIRED( qmux_s->io.port_info.frame_mode ) )
    {
      if( !qmux_unframe_sdu( &qmux_pdu, &qmux_s->io.pending_sdu_ptr ) )
      {
        LOG_MSG_INFO2_1 ("Error on unframing SDU: %d", qmi_instance);
        continue;
      }
      
      /* Check for partial PDU case, where SDU was cached for next cycle */
      if (qmux_pdu == NULL)
      {
        LOG_MSG_INFO2_1 ("Partial PDU cached for next cycle: %d",
                         qmi_instance);
        continue;
      }
    }

    qmuxi_process_msg( qmux_s, qmux_pdu );

    /*-----------------------------------------------------------------------
      Process one command per call.  Set signal if commands remain so we'll
      pass through the DS main loop again, allowing DS commands to be 
      processed.
    -----------------------------------------------------------------------*/
    if( q_cnt( qmux_s->io.rx_wm.q_ptr ) )
    {
      qmi_set_signal( QMI_QMUX_RX_SIGNAL );
    }
  }

  return TRUE;
} /* qmux_process_rx_sig() */



/*===========================================================================
  FUNCTION QMI_INSTANCE_BY_QMUX_HANDLE()

  DESCRIPTION
    To obtain QMI instance from QMUX handle. Qmux handle passed in must 
    ALWAYS be the service handle.

  PARAMETERS  
    qmux_handle : QMUX service handle returned to service by qmux_reg_service

  RETURN VALUE
    QMI Instance if qmux_handle is valid
    QMI_INSTANCE_MAX if qmux_handle is invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_instance_e_type  qmi_instance_by_qmux_handle
(
  qmux_svc_info_type * service_handle
)
{
  qmi_instance_e_type  retval;
  int                  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (retval = QMI_INSTANCE_MIN; retval < QMI_INSTANCE_MAX; retval++)
  {
    for (i = 0; i < QMUX_SERVICE_MAX; i++)
    {
      if (qmux_state[retval].service[i] == service_handle)
      {
        return retval;
      }
    }
  }
    
  if (retval >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1 ("Invalid QMUX Service handle! (0x%x)", service_handle);
    return QMI_INSTANCE_MAX;
  }

  return retval;

} /* qmi_instance_by_qmux_handle() */
  

/*===========================================================================
  FUNCTION QMI_GET_NUM_INSTANCES()

  DESCRIPTION
    Returns number of QMI instances.

  PARAMETERS  
    qmi_instance : instance of QMI

  RETURN VALUE
    Number of QMI instances defined

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint8 qmi_get_num_instances( void )
{
  return qmux_num_instances;
}

/*===========================================================================
  FUNCTION QMI_GET_INSTANCE_INITIALIZED()
   
  DESCRIPTION
    Returns instance initialized flag.

  PARAMETERS
    qmi_instance : instance of QMI

  RETURN VALUE
    TRUE if QMUX instance initialized
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_get_instance_initialized
(
  qmi_instance_e_type     qmi_instance
)
{
  return qmux_state[qmi_instance].initialized;
} /* qmi_instance_initialized() */

/*===========================================================================
  FUNCTION QMUXI_GET_QMI_CONTROL_DEVICE_ID()
   
  DESCRIPTION
    Returns instance QMUX device ID type associated with QMI instance.

  PARAMETERS
    qmi_instance : instance of QMI

  RETURN VALUE
    Device ID if valid instance.
    QMI_DEV_NONE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sio_port_id_type qmuxi_get_qmi_control_device_id
(
  qmi_instance_e_type     qmi_inst
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (QMI_INSTANCE_MAX <= qmi_inst)
  {
    LOG_MSG_ERROR_1 ("QMI instance %d out of range", qmi_inst);
    return SIO_PORT_NULL;
  }

  if (FALSE == qmux_state[qmi_inst].initialized)
  {
    LOG_MSG_ERROR_1 ("QMUX instance %d not initialized", qmi_inst);
    return SIO_PORT_NULL;
  }

  return qmux_state[qmi_inst].io.port_info.sio_port;

} /* qmuxi_get_qmi_control_device_id() */

/*===========================================================================
  FUNCTION QMUX_VERIFY_DTR_HIGH()

  DESCRIPTION
    Returns DTR status on QMI instance

  PARAMETERS
    qmi_inst : instance of QMI

  RETURN VALUE
    TRUE if DTR is asserted for the instance
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmux_verify_dtr_high
(
  qmi_instance_e_type     qmi_inst
)
{
  qmux_state_type       *qmux_s;
  sio_ioctl_param_type  ioctl_param;
  boolean               dtr_status = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( (qmi_inst >= QMI_INSTANCE_MIN) && (qmi_inst < QMI_INSTANCE_MAX) );
  qmux_s = &qmux_state[qmi_inst];

  if ( (qmux_s->io.sio_handle == 0 ) ||
       (qmux_s->io.sio_handle == SIO_NO_STREAM_ID) )
  {
    LOG_MSG_ERROR_1("QMI inst [%d] port not initialized", qmi_inst);
    return FALSE;
  }

  memset(&ioctl_param, 0, sizeof(ioctl_param));
  ioctl_param.dte_ready_asserted = &dtr_status;

  qmuxi_sio_ioctl(qmux_s,
              SIO_IOCTL_DTE_READY_ASSERTED,
              &ioctl_param);

  LOG_MSG_INFO2_3("QMI inst [%d] stream ID [%d] DTR status [%d]",
                  qmi_inst, qmux_s->io.sio_handle, dtr_status);

  return dtr_status;

} /* qmux_verify_dtr_high() */


/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMUXI_INIT()

  DESCRIPTION
    Initialize specified QMUX instance.  SIO control IO path will be set up,
    and QMUX instance state will be reset to default.

  PARAMETERS
    qmi_instance : qmi link to initialize 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmuxi_init
(
  qmi_instance_e_type  qmi_instance
)
{
  qmux_state_type *  qmux_s;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (qmi_instance < QMI_INSTANCE_MAX);
  
  /* check that qmi_instance is within range in case ASSERT is removed */
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!", qmi_instance);
    return FALSE;
  }
  
  qmux_s = &qmux_state[qmi_instance];

  if (qmux_s->initialized)
  {
    LOG_MSG_INFO1_0 ("QMUX instance already initialized!");
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Initialize the control stream RX and TX watermarks
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Initialize the watermarks and queues used for USB control stream
  -------------------------------------------------------------------------*/
  memset (&qmux_s->io.rx_wm, 0, sizeof(qmux_s->io.rx_wm));
  memset (&qmux_s->io.rx_wm_q, 0, sizeof(qmux_s->io.rx_wm_q));

  dsm_queue_init( &qmux_s->io.rx_wm,
                  QMUX_RX_WM_DNE,
                  &qmux_s->io.rx_wm_q );
  
  memset (&qmux_s->io.tx_wm, 0, sizeof(qmux_s->io.tx_wm));
  memset (&qmux_s->io.tx_wm_q, 0, sizeof(qmux_s->io.tx_wm_q));

  dsm_queue_init( &qmux_s->io.tx_wm,
                  QMUXI_TX_DNE,
                  &qmux_s->io.tx_wm_q );

  /*-------------------------------------------------------------------------
                             SETUP SIO TX WATERMARK
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    This DSM watermark queue holds data to be sent by sio on the QMUX control 
    channel to the host.

    If this watermark reaches the high water mark, we'll reset the connection
    to maintain order.  QMUX should never reach this point.
  -------------------------------------------------------------------------*/
  qmux_s->io.tx_wm.lo_watermark          = 0;
  qmux_s->io.tx_wm.hi_watermark          = 0;

  qmux_s->io.tx_wm.dont_exceed_cnt = QMUXI_TX_DNE;
  LOG_MSG_INFO1_1( "Set control tx_wm to be infinite size: %d",
                   qmux_s->io.tx_wm.dont_exceed_cnt );

  qmux_s->io.tx_wm.gone_empty_func_ptr   = NULL;
  qmux_s->io.tx_wm.non_empty_func_ptr    = NULL;
  qmux_s->io.tx_wm.lowater_func_ptr      = NULL;
  qmux_s->io.tx_wm.hiwater_func_ptr      = NULL;
  qmux_s->io.tx_wm.each_enqueue_func_ptr = NULL;

  /*-------------------------------------------------------------------------
                             SETUP SIO RX WATERMARK
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    This DSM watermark queue holds data received by sio on the QMUX control 
    channel from the host.

    Generally, when something arrives in this queue, the qmuxi_sio_rx_cb will
    be called and all outstanding QMUX messages will be processed.  If for
    some reason all messages can't be processed right then (e.g. QMUX channel
    is blocked on completion of some synchronous operation), the subroutine
    that later completes the operation is responsible for prompting the
    serving task to continue processing the QMUX tx queue.
    
    SIO watermark use should be synchonized with the ISR used by the
    serial driver.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Set low, high, and dont exceed limits.
  -------------------------------------------------------------------------*/
  qmux_s->io.rx_wm.lo_watermark           = QMUX_RX_WM_LOW;
  qmux_s->io.rx_wm.hi_watermark           = QMUX_RX_WM_HIGH;
  qmux_s->io.rx_wm.dont_exceed_cnt        = QMUX_RX_WM_DNE;
 
  qmux_s->io.rx_wm.lowater_func_ptr       = NULL;
  qmux_s->io.rx_wm.hiwater_func_ptr       = NULL; 
  qmux_s->io.rx_wm.gone_empty_func_ptr    = NULL;
  qmux_s->io.rx_wm.non_empty_func_ptr     = NULL; 
  qmux_s->io.rx_wm.each_enqueue_func_ptr  = qmuxi_sio_rx_cb; 
  qmux_s->io.rx_wm.each_enqueue_func_data = qmux_s; 

#ifdef FEATURE_DSM_MEM_CHK
  /*-------------------------------------------------------------------------
    Reset the total_rcvd_cnt, as a packet data call can really set this
    number very high.
  -------------------------------------------------------------------------*/
  qmux_s->io.tx_wm.total_rcvd_cnt        = 0;
  qmux_s->io.rx_wm.total_rcvd_cnt        = 0;
#endif

  qmux_s->initialized = TRUE;
  return TRUE;
} /* qmuxi_init() */



/*===========================================================================

  FUNCTION QMUXI_SIO_RX_CB

  DESCRIPTION
    Invoked when data arrives in the QMUX control channel receive watermark.

  PARAMETERS
    wm_ptr        -  pointer to control channel receive watermark
    user_data_ptr -  opaque handle to QMUX instance state
    
  DEPENDENCIES
    None
  
  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void  qmuxi_sio_rx_cb
(  
  struct dsm_watermark_type_s *  wm_ptr,
  void *                         user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Signal to serving task that QMI commands are outstanding.
    Multiplexing all QMI links on the same RX signal.
  -------------------------------------------------------------------------*/
  qmi_set_signal( QMI_QMUX_RX_SIGNAL );

} /* qmuxi_sio_rx_cb() */



/*===========================================================================
  FUNCTION QMUXI_INPUT

  DESCRIPTION
    Receive incoming QMUX message, decapsulate QMUX header and dispatch to
    the appropriate registered service input function.

  PARAMETERS  
    qmux_s   : pointer to QMUX link state for the receiving control interface
    qmux_pdu : incoming pdu
  
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    TRUE  - more commands pending on the receive queue for this QMI link
    FALSE - Done processing commands on this QMI link for now. Caller
            will free the input pdu

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmuxi_input
(
  qmux_state_type *  qmux_s,
  dsm_item_type **   qmux_pdu
)
{
  qmux_svc_info_type *  svci;
  qmux_hdr_type         qmux_hdr;
  boolean               status = FALSE;
  uint8                 msg_ctl;
  uint8                 tx_id_hb;
  uint8                 tx_id_lb;
  uint16                tx_id;
  uint8                 msg_id_hb;
  uint8                 msg_id_lb;
  uint16                msg_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (qmux_s);
  ASSERT (qmux_pdu);
  svci = NULL; 


  /* check that qmux_s is not NULL in case ASSERT is removed */
  if (qmux_s == NULL)
  {
    LOG_MSG_ERROR_0("QMUX state NULL!");
    return FALSE;
  }

  /* check that qmux_pdu is not NULL in case ASSERT is removed */
  if (qmux_pdu == NULL)
  {
    LOG_MSG_ERROR_0("QMUX PDU NULL!");
    return FALSE;
  }
  
  LOG_MSG_INFO2_2( "Received QMUX PDU (%d bytes) on QMI link %d",
                   dsm_length_packet(*qmux_pdu),
                   qmi_instance_by_qmux_state(qmux_s) );

  /*-------------------------------------------------------------------------
    Pull up QMUX header
  -------------------------------------------------------------------------*/
  if( sizeof(qmux_hdr) != 
      PS_UTILS_DSM_PULLUP( qmux_pdu, (void *) &qmux_hdr, sizeof(qmux_hdr)) )
  { 
    return FALSE;
  } 

  /*-------------------------------------------------------------------------
    Check if MSM can handle this message
    - Make sure PDU is exact length spec'd in hdr, otherwise throw it out.
    - Sender must be a control point
    - Service type must be defined and registered
    - Client ID must be valid
  -------------------------------------------------------------------------*/
  if( qmux_hdr.len != dsm_length_packet(*qmux_pdu) + sizeof(qmux_hdr) )
  {
    LOG_MSG_ERROR_2 ("Invalid control frame len (%d) expected (%d)!",
                     qmux_hdr.len, dsm_length_packet(*qmux_pdu));
    return FALSE;
  }
  
  if( (qmux_hdr.ctl_flags & QMUXI_CFLAG_MASK_SENDER) !=
      QMUXI_CFLAG_SENDER_CONTROL_POINT )
  {
    LOG_MSG_ERROR_0 ("QMI service message not handled");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
     Return error if:
     - svc_type is not Internal QC service range and not in VS range
  -----------------------------------------------------------------------*/
  if (qmux_hdr.svc_type >= QMUX_SERVICE_MAX)
  {
    LOG_MSG_ERROR_1 ("Invalid QMUX type (%d)!", qmux_hdr.svc_type);
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Block services STX and SIM_INTERNAL from Apps
  -----------------------------------------------------------------------*/
#if defined(FEATURE_8960_SGLTE_FUSION)
  if((qmux_hdr.svc_type == QMI_SIM_INTERNAL) || (qmux_hdr.svc_type == QMI_STX))
  {
    if (!(qmux_s->io.port_info.sio_port == SIO_PORT_DATA_MUX_2))
    {
      LOG_MSG_INFO2_1 ("Request is dropped. Blocking service %d to SGLTE",
                       qmux_hdr.svc_type);
      return FALSE;
    }
  }
#endif

  /*-----------------------------------------------------------------------
    Get service info.
  -----------------------------------------------------------------------*/
  svci = qmux_s->service[qmux_hdr.svc_type];

  /*-------------------------------------------------------------------------
    Log the QMUX hdr info, msg_ctl, QMI instance and tx_id in SMEM
  -------------------------------------------------------------------------*/
  dsm_peek_byte(*qmux_pdu, 0, &msg_ctl );
  if (qmux_hdr.svc_type == QMUX_SERVICE_CTL)
  {
    // Tx id in CTL is 1 byte
    dsm_peek_byte(*qmux_pdu, 1, &tx_id_lb );
    tx_id_hb = 0;
    dsm_peek_byte(*qmux_pdu, 2, &msg_id_lb );
    dsm_peek_byte(*qmux_pdu, 3, &msg_id_hb );
  }
  else
  {
    dsm_peek_byte(*qmux_pdu, 1, &tx_id_lb );
    dsm_peek_byte(*qmux_pdu, 2, &tx_id_hb );
    dsm_peek_byte(*qmux_pdu, 3, &msg_id_lb );
    dsm_peek_byte(*qmux_pdu, 4, &msg_id_hb );
  }

  tx_id = ((((uint16)(tx_id_hb) & 0x00FF) << 8) | (uint16)tx_id_lb);
  msg_id = ((((uint16)(msg_id_hb) & 0x00FF) << 8) | (uint16)msg_id_lb);
  SMEM_LOG_EVENT6(SMEM_QMUX_RX_EVENT, 
                  qmux_hdr.len, msg_id,
                  qmux_hdr.svc_type, qmux_hdr.clid,
                  ((((uint32)(qmi_instance_by_qmux_state(qmux_s)) & 0x0000FFFF) << 16) | (uint32)msg_ctl),
                  tx_id);
  /*-----------------------------------------------------------------------
    if service is not found in legacy framework or VS service, then
    check in new QMI framework.
  -----------------------------------------------------------------------*/
  if((svci == NULL) || (!(svci->registered)))
  {
    status =  qmi_framework_svc_recv( qmi_instance_by_qmux_state(qmux_s),
                               (qmux_service_e_type) qmux_hdr.svc_type,
                               qmux_hdr.clid,
                               *qmux_pdu );
    *qmux_pdu = NULL;
    return status;
  }

  /*-------------------------------------------------------------------------
    Dispatch incoming request to QCTL or legacy service.
    Since QCTL is running on QMI Modem task and has own svc recv input function
    the below check is required to differentiate the dispatch function(legacy
    or QCTL).
  -------------------------------------------------------------------------*/
  if (svci->service_id == QMUX_SERVICE_CTL)
  {
    LOG_MSG_INFO2_3 ("Dispatching svc_type %d msg (%d bytes) for client (%d)",
                     qmux_hdr.svc_type,
                     dsm_length_packet(*qmux_pdu),
                     qmux_hdr.clid );
    if (svci->cfg.cbs.recv)
    {
      (svci->cfg.cbs.recv)( svci->cfg.sp, qmux_hdr.clid, *qmux_pdu );
    }
    *qmux_pdu = NULL;  
    return TRUE;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Post the service request to legacy service.
    -----------------------------------------------------------------------*/
    status =  qmi_cmd_recv_legacy_svc_req(svci, qmux_hdr.clid, *qmux_pdu);
    *qmux_pdu = NULL;
    return status;
  }
} /* qmuxi_input() */



/*===========================================================================
  FUNCTION QMUX_PROCESS_MSG

  DESCRIPTION
    Process a received QMUX message

  PARAMETERS  
    qmux_s   -  QMI link state pointer
    qmux_pdu -  received QMI command to be processed
  
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmuxi_process_msg  // rename as qmuxi_process_pdu
( 
  qmux_state_type *  qmux_s,
  dsm_item_type *    qmux_pdu
)
{
  int16            iftype;
#if 0
  dsm_item_type *  pdu_p;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Log received QMI packet if logging is enabled
  -------------------------------------------------------------------------*/
  qmi_log_packet(qmux_pdu, 
                 qmi_instance_by_qmux_state(qmux_s),
                 QMUX_RX);

#if 0 /* replace with log frame when available */
  LOG_MSG_INFO2_0("Got QMUX");
  pdu_p = qmux_pdu;
  while (pdu_p)
  {
    memdump( pdu_p->data_ptr, pdu_p->used );
    pdu_p = pdu_p->pkt_ptr;
  }
#endif /* replace with log frame when available */

  /*-------------------------------------------------------------------------
    Pull off control message PDU type
  -------------------------------------------------------------------------*/
  iftype = dsm_pull8(&qmux_pdu);
  if( (qmux_frame_mode_type)iftype != qmux_s->io.port_info.frame_mode )
  {
    /* IFType does not match port configuration */
    LOG_MSG_ERROR_2( "IFType configuration mismatch (%d!=%d), dropping packet",
                     	 iftype, qmux_s->io.port_info.frame_mode );
  }
  else
  {
    /* IFType consistent with configuration, process packet */
    switch (iftype)
    {
      case QMUXI_IFTYPE_QMUX:
      case QMUXI_IFTYPE_QMUX_FRAMED:
      {
	/*-------------------------------------------------------------------
	  Ignore return value
	-------------------------------------------------------------------*/
	(void) qmuxi_input (qmux_s, &qmux_pdu);
	break;
      }
      default:
      {
	LOG_MSG_ERROR_1 ("Invalid control frame type (%d)!", iftype);
	break;
      }
    }
  }
  /*-------------------------------------------------------------------------
    Make sure input message is freed if input function hasn't done so
  -------------------------------------------------------------------------*/
  dsm_free_packet (&qmux_pdu);

} /* qmuxi_process_msg() */



/*===========================================================================
  FUNCTION QMUXI_OPEN_INSTANCE

  DESCRIPTION
    Opens up a QMI instance

  PARAMETERS  
    qmi_instance   -  QMI instance
    sio_port       -  SIO port ID
  
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmuxi_open_instance
(
  qmi_instance_e_type  qmi_instance,
  uint16               sio_port
)
{
  sio_open_type      sioctl_config;
  qmux_state_type *  qmux_s;
  sio_ioctl_param_type  ioctl_param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (qmi_instance < QMI_INSTANCE_MAX);

  /* check that qmi_instance is within range in case ASSERT is removed */
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!", qmi_instance);
    return FALSE;
  }

  qmux_s = &qmux_state[qmi_instance];

  /*-------------------------------------------------------------------------
    Init the qmi state for the provided instance.
    Note usb control bitrates should not affect sio behaviour.
  -------------------------------------------------------------------------*/
  memset(&sioctl_config, 0, sizeof(sio_open_type));
  sioctl_config.stream_mode    = SIO_GENERIC_MODE;
  sioctl_config.rx_queue       = &qmux_s->io.rx_wm;
  sioctl_config.tx_queue       = &qmux_s->io.tx_wm;
  sioctl_config.rx_bitrate     = SIO_BITRATE_BEST;
  sioctl_config.tx_bitrate     = SIO_BITRATE_BEST;
  sioctl_config.port_id        = (sio_port_id_type)sio_port;
  sioctl_config.rx_func_ptr    = NULL;
  sioctl_config.rx_flow        = SIO_FCTL_BEST;
  sioctl_config.tx_flow        = SIO_FCTL_BEST;

  /*-------------------------------------------------------------------------
    Assign tail char for streaming driver packetization if so
    configured. This ensures packets are delivered from SIO driver
    only when this octet is detected.
  -------------------------------------------------------------------------*/
  if( IS_FRAMING_REQUIRED( qmux_s->io.port_info.frame_mode ) )
  {
     sioctl_config.tail_char_used = TRUE;
     sioctl_config.tail_char      = QMUX_FRAME_FLAG_STOP;
  }
  else
  {
     sioctl_config.tail_char_used = FALSE;
     sioctl_config.tail_char      = 0;
  }

  /*-------------------------------------------------------------------------
    Open the Network interface control channel with these parameters
  -------------------------------------------------------------------------*/
  if( QMUX_DEVSTREAM_CONTROL == qmux_s->io.port_info.qmi_stream )
  {
    /* Set the SMD FIFO size to 8K for tethered ports, so that it can handle
       max USB packet size of 4K. If this is not set, default size is 1K */
    if (IS_TETHERED_PORT(sio_port))
    {
      PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(sioctl_config.open_param, sizeof(sio_open_param_type),
                                      sio_open_param_type*);
      if (NULL == sioctl_config.open_param)
      {
        return FALSE;
      }
      memset(sioctl_config.open_param, 0, sizeof(sio_open_param_type));
      sioctl_config.open_param->dsm_pool_param.smd_fifo_size = 0x2000; 
    }
    qmux_s->io.sio_handle = sio_control_open (&sioctl_config);
    if (sioctl_config.open_param)
    {
      PS_SYSTEM_HEAP_MEM_FREE(sioctl_config.open_param);
    }
  }
  else
  {
    qmux_s->io.sio_handle = sio_open( &sioctl_config );
  }
  
  if (qmux_s->io.sio_handle == SIO_NO_STREAM_ID)
  {
    LOG_MSG_ERROR_1( "SIO open failed on stream: %d",
                     qmux_s->io.port_info.qmi_stream);
    return FALSE;
  }
  else
  {
     /* We need to set the DROP_PENDING ioctl on SMD ports, in order to
      * enable the SMD module to drop packets when the onward link to apps
      * proc is not available. */
    if ( (sio_port >= SIO_PORT_SMD_DATA_FIRST) &&
         (sio_port <= SIO_PORT_SMD_DATA_LAST) )
    {
      memset( &ioctl_param, 0, sizeof(sio_ioctl_param_type) );
      ioctl_param.drop_asserted = TRUE;
      if (QMUX_DEVSTREAM_CONTROL == qmux_s->io.port_info.qmi_stream)
      {
        sio_control_ioctl(qmux_s->io.sio_handle,
                          SIO_IOCTL_SET_DROP_PEND,
                          &ioctl_param);
      }
      else
      {
        sio_ioctl(qmux_s->io.sio_handle,
                  SIO_IOCTL_SET_DROP_PEND,
                  &ioctl_param);
      }
    }
  }

  /*-------------------------------------------------------------------------
    Set DTR handler
  -------------------------------------------------------------------------*/
  qmuxi_set_dtr_handler(qmi_instance, qmuxi_dtr_cb);

  return TRUE;
} /* qmuxi_open_instance() */


/*===========================================================================
  FUNCTION QMUXI_CLOSE_INSTANCE

  DESCRIPTION
    Closes a QMI instance

  PARAMETERS  
    qmi_instance   -  QMI instance
  
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmuxi_close_instance
(
  qmi_instance_e_type  qmi_instance
)
{
  qmux_state_type *  qmux_s;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (qmi_instance < QMI_INSTANCE_MAX);
  
  /* check that qmi_instance is within range in case ASSERT is removed */
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!", qmi_instance);
    return FALSE;
  }
  
  qmux_s = &qmux_state[qmi_instance];

  qmux_link_down( qmi_instance );

  ASSERT (qmux_s->io.sio_handle != SIO_NO_STREAM_ID);

  /* check that qmux_s->io.sio_handle is set in case ASSERT is removed */
  if (qmux_s->io.sio_handle == SIO_NO_STREAM_ID)
  {
    LOG_MSG_ERROR_1("QMUX sio_handle stream ID not set (%d)!",
                    qmux_s->io.sio_handle);
    return FALSE;
  }

  /* Reset DTR handler */
  qmuxi_set_dtr_handler(qmi_instance, NULL);

  if( QMUX_DEVSTREAM_CONTROL == qmux_s->io.port_info.qmi_stream )
  {
    sio_control_close( qmux_s->io.sio_handle, NULL );
  }
  else
  {
    sio_close( qmux_s->io.sio_handle, NULL );
  }

  return TRUE;
}



/*===========================================================================
  FUNCTION QMUXI_SET_DEFAULT_PORT_CONFIGURATION

  DESCRIPTION
    This function assigns the default port configuraiton table.  It shoudl be
    used when NVRAM value has not been previously assigned (e.g. factory
    settings). 
  
  PARAMETERS  
    list_ptr   -  Pointer to list of port configurations
  
  DEPENDENCIES
    None

  RETURN VALUE
    Number of ports in configuraiton table

  SIDE EFFECTS
    None
===========================================================================*/
static void qmuxi_set_default_port_configuration
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Static port configuration, add data port first. Example:
      qmux_add_data_port(SIO_PORT_NULL, 1, 0, 0);
      qmux_add_control_port(SIO_PORT_SMD_DATA40, 1);
  -------------------------------------------------------------------------*/

} /* qmuxi_set_default_port_configuration() */

/*===========================================================================
  FUNCTION QMUX_RMNET_INST_OPEN_CLOSE_RESULT()

  DESCRIPTION
    Notify QMUX about the specified Rmnet instances open/close result. The
    QMUX post a cmd to qmi_modem task to process it.
 

  PARAMETERS
    result      : Open/Close result
    cmd         : cmd(open or close)
    instance    : Rmnet instance
    sio_port    : sio_port 

  RETURN VALUE
    None

  DEPENDENCIES
    QMI Data port(Rment) should open before control port.

  SIDE EFFECTS
    None
===========================================================================*/
void qmux_rmnet_inst_open_close_result(  
  boolean                                  result,
  qmi_cmd_id_e_type                        cmd,
  uint32                                   ep_id
)
{
  qmi_cmd_data_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Post commnd to host task for execution in its context.
  -----------------------------------------------------------------------*/
  cmd_ptr = qmi_get_cmd_data_buf();
  if( cmd_ptr != NULL )
  {
    cmd_ptr->cmd.qmi.id = cmd;
    cmd_ptr->cmd.qmi.data.rmnet_inst.result   = result;
    cmd_ptr->cmd.qmi.data.rmnet_inst.ep_id    = ep_id;
    qmi_send_cmd( QMI_CMD, cmd_ptr );
  }
  else
  {
    LOG_MSG_ERROR_1("Could not get cmd buffer from QMI modem task for cmd id %d",
                    cmd );
  }
} /* qmux_rmnet_inst_open_close_result() */

/*===========================================================================
  FUNCTION QMUX_PROCESS_RMNET_INST_OPEN_CLOSE_RESULT()

  DESCRIPTION
    This function processes RmNet (data) port open/close result in qmi modem
    task and then proceeds with the control port operation.
    
  PARAMETERS
    result      : Open/Close result
    cmd         : cmd(open or close)
    instance    : Rmnet instance
    sio_port    : sio_port 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmux_process_rmnet_inst_open_close_result
(  
  boolean                                  result,
  qmi_cmd_id_e_type                        cmd,
  uint32                                   ep_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("QMUX process rmnet open_close result:%d EP:0x%x",
                  result, ep_id);

  qmi_port_cfg_in_progress = FALSE;
  qmuxi_port_config_from_queue();

} /* qmux_process_rmnet_inst_open_close_result() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
  FUNCTION QMUXI_SIO_IOCTL

  DESCRIPTION
    This function calls sio ioctl function based on stream type.
  
  PARAMETERS  
    qmux_s:      qmux_state pointer
    cmd:         sio command
    ioctl_param: ioctl param pointer
  
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmuxi_sio_ioctl
(
  qmux_state_type       * qmux_s,
  sio_ioctl_cmd_type      cmd,
  sio_ioctl_param_type  * ioctl_param
)
{
  ASSERT ( (qmux_s != NULL) && (ioctl_param != NULL) );

  if (qmux_s->io.port_info.qmi_stream == QMUX_DEVSTREAM_CONTROL)
  {
    sio_control_ioctl(qmux_s->io.sio_handle, cmd, ioctl_param);
  }
  else
  {
    sio_ioctl(qmux_s->io.sio_handle, cmd, ioctl_param);
  }
} /* qmuxi_sio_ioctl() */

/*===========================================================================
  FUNCTION QMUXI_SET_DTR_HANDLER

  DESCRIPTION
    This function sets the DTR handler per the given qmi instance
  
  PARAMETERS  
    inst:   QMI instance
    dtr_cb: DTR callback function 
  
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmuxi_set_dtr_handler
(
  qmi_instance_e_type    inst,
  sio_vpu_func_ptr_type  dtr_cb
)
{
  qmux_state_type       *qmux_s;
  sio_ioctl_param_type  ioctl_param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( (inst >= QMI_INSTANCE_MIN) && (inst < QMI_INSTANCE_MAX) );
  qmux_s = &qmux_state[inst];

  memset(&ioctl_param, 0, sizeof(ioctl_param));
  ioctl_param.enable_dte_ready_event_ext.cb_func = dtr_cb;
  ioctl_param.enable_dte_ready_event_ext.cb_data = qmux_s;

  if (dtr_cb)
  {
    qmux_s->dtr_status = FALSE;
    qmuxi_sio_ioctl(qmux_s,
                SIO_IOCTL_ENABLE_DTR_EVENT_EXT,
                &ioctl_param);

    // DTR callback will be invoked immediately once registered with
    // SMD so there is no need to check DTR status here
  }
  else
  {
    qmuxi_sio_ioctl(qmux_s,
                SIO_IOCTL_DISABLE_DTR_EVENT_EXT,
                &ioctl_param);
  }

} /* qmuxi_set_dtr_handler() */

/*===========================================================================
  FUNCTION QMUXI_DTR_CB

  DESCRIPTION
    This is the QMI DTR CB function
  
  PARAMETERS  
    user_data:  QMUX state pointer
  
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmuxi_dtr_cb
(
  void    * user_data
)
{
  qmux_state_type       *qmux_s;
  qmi_instance_e_type   inst;
  sio_ioctl_param_type  ioctl_param;
  boolean               dtr_status = FALSE;
  qmi_cmd_data_buf_type *cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(user_data);
  qmux_s = (qmux_state_type *) user_data;

  inst = qmi_instance_by_qmux_state(qmux_s);
  ASSERT( (inst >= QMI_INSTANCE_MIN) && (inst < QMI_INSTANCE_MAX) );

  memset(&ioctl_param, 0, sizeof(ioctl_param));
  ioctl_param.dte_ready_asserted = &dtr_status;

  qmuxi_sio_ioctl(qmux_s,
              SIO_IOCTL_DTE_READY_ASSERTED,
              &ioctl_param);

  QMI_ENTER_CRIT_SECTION(&global_qmi_crit_section);
  if (dtr_status == qmux_s->dtr_status)
  {
    QMI_LEAVE_CRIT_SECTION(&global_qmi_crit_section);
    LOG_MSG_INFO2_2("Ignore DTR [%d] on QMI inst [%d]", dtr_status, inst);
    return;
  }
  qmux_s->dtr_status = dtr_status;
  QMI_LEAVE_CRIT_SECTION(&global_qmi_crit_section);

  LOG_MSG_INFO2_2("DTR [%d] on QMI inst [%d]", dtr_status, inst);

  // Free any QMI messages still in RX wmk if DTR drops
  if (dtr_status == FALSE)
  {
    dsm_empty_queue(&qmux_s->io.rx_wm);
  }

  // Send DTR status to QMI modem task
  cmd_ptr = qmi_get_cmd_data_buf();
  if (cmd_ptr != NULL)
  {
    cmd_ptr->cmd.qmi.id = QMI_CMD_DTR_STATUS;
    cmd_ptr->cmd.qmi.data.dtr_status.qmi_inst = inst;
    cmd_ptr->cmd.qmi.data.dtr_status.asserted = dtr_status;
    qmi_send_cmd(QMI_CMD, cmd_ptr);
  }
  else
  {
    LOG_MSG_ERROR_0("Can't allocate QMI cmd buf");
    return;
  }

} /* qmuxi_dtr_cb() */

/*===========================================================================
  FUNCTION QMI_INSTANCE_BY_QMUX_STATE()

  DESCRIPTION
    To obtain QMI instance from QMUX state. Qmux state passed in must 
    ALWAYS be the QMUX state address.

  PARAMETERS  
    qmux_s : QMUX state address

  RETURN VALUE
    QMI Instance if qmux_s is valid
    QMI_INSTANCE_MAX if qmux_s is invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_instance_e_type  qmi_instance_by_qmux_state
(
  qmux_state_type * qmux_s
)
{
  int qmi_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qmi_inst = qmux_s - qmux_state;

  if ( (qmi_inst >= QMI_INSTANCE_MIN) && (qmi_inst < QMI_INSTANCE_MAX) )
  {
    return (qmi_instance_e_type)qmi_inst;
  }

  LOG_MSG_ERROR_2("Invalid qmux state [%p], inst [%d]", qmux_s, qmi_inst);

  return QMI_INSTANCE_MAX;

} /* qmi_instance_by_qmux_state() */

/*===========================================================================
  FUNCTION QMI_IF_GET_CMD_GET_BUF

  DESCRIPTION
    Allocate and assign a QMI IF command buffer from the PS MEM heap
    based on the QMI IF type
    
  PARAMETERS
    cmd_id - QMI command type
   
  RETURN VALUE
    cmd_buf_ptr - Pointer to the allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void *qmi_qmux_get_cmd_buf
(
  qmi_cmd_id_e_type cmd_id
)
{
  switch (cmd_id)
  {
    case QMI_CMD_LEGACY_SVC_QMI_LINK_CLOSED:
    {
      qmi_qmux_cmd_buf_type *cmd_buf_ptr = NULL;
      PS_SYSTEM_HEAP_MEM_ALLOC(cmd_buf_ptr, sizeof(qmi_qmux_cmd_buf_type),
                               qmi_qmux_cmd_buf_type*);
      return ((void*)cmd_buf_ptr);
    }

    default:
      LOG_MSG_ERROR_1("Unknown QMI IF command %d", (int32)cmd_id);
  }
  return NULL;
} /* qmi_qmux_get_cmd_buf */

/*===========================================================================
  FUNCTION QMI_QMUX_PROCESS_CMD

  DESCRIPTION
    This function processes a QMI QMUX command or event.

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI IF command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qmux_process_cmd
(
  void * cmd_ptr
)
{
  qmi_qmux_cmd_buf_type *cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/

  ASSERT(cmd_ptr);
  cmd_buf_ptr = (qmi_qmux_cmd_buf_type *)cmd_ptr;
  switch(cmd_buf_ptr->cmd_id)
  {
    case QMI_CMD_LEGACY_SVC_QMI_LINK_CLOSED:
      qmi_legacy_svc_qmi_link_closed(cmd_buf_ptr->data.clean_instance);      
      break;

    default:
      LOG_MSG_INFO2_1 ("Ignoring QMI QMUX cmd %d", cmd_buf_ptr->cmd_id);
      break;

  }/* switch(cmd_buf_ptr->cmd_id) */

  qmi_qmux_free_cmd_buf(cmd_ptr);
  return;
} /* qmi_qmux_process_cmd() */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
  FUNCTION QMUXI_QUEUE_PORT_REQUEST

  DESCRIPTION
    This function queues the port config request if config is in progress
  
  PARAMETERS  
  
  DEPENDENCIES
    None

  RETURN VALUE
    TRUE: request is queued
    FALSE: request is not queued

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmuxi_queue_port_request
(
  boolean                 is_control_port,
  qmi_port_status_enum    status,
  sio_port_id_type        sio_port,
  uint32                  ep_id,
  uint32                  consumer_pipe_num,
  uint32                  producer_pipe_num
)
{
  qmi_port_request_type  * request;
  qmi_port_request_type  * elem_ptr;
/*-------------------------------------------------------------------------*/
  if (!qmi_port_cfg_in_progress)
  {
    return FALSE;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(request, sizeof(qmi_port_request_type),
                           qmi_port_request_type*);
  if (request == NULL)
  {
    return FALSE;
  }

  memset(request, 0, sizeof(qmi_port_request_type));
  request->is_control_port    = is_control_port;
  request->status             = status;
  request->sio_port           = sio_port;
  request->ep_id              = ep_id;
  request->consumer_pipe_num  = consumer_pipe_num;
  request->producer_pipe_num  = producer_pipe_num;
  request->next               = NULL;

  if (qmi_port_cfg_queue == NULL)
  {
    qmi_port_cfg_queue = request;
    return TRUE;
  }

  elem_ptr = qmi_port_cfg_queue;
  while (elem_ptr->next != NULL)
  {
    elem_ptr = elem_ptr->next;
  }

  elem_ptr->next = request;
  return TRUE;

} /* qmuxi_queue_port_request() */

/*===========================================================================
  FUNCTION QMUXI_PORT_CONFIG_FROM_QUEUE

  DESCRIPTION
    This function does port config from queue
  
  PARAMETERS
    None
  
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmuxi_port_config_from_queue
(
  void
)
{
  qmi_port_request_type  * elem_ptr;
/*-------------------------------------------------------------------------*/
  while (qmi_port_cfg_queue != NULL && !qmi_port_cfg_in_progress)
  {
    elem_ptr = qmi_port_cfg_queue;
    qmi_port_cfg_queue = elem_ptr->next;

    if (elem_ptr->is_control_port)
    {
      if (elem_ptr->status == QMI_PORT_TO_OPEN)
      {
        qmux_add_control_port(elem_ptr->sio_port, elem_ptr->ep_id);
        if (qmi_port_cfg_queue == NULL)
        {
          TTLBOOTUP(STARTUP_QMUX_DPM_OPEN_DONE);
        }
      }
      else
      {
        qmux_remove_control_port(elem_ptr->sio_port);
      }
    }
    else
    {
      if (elem_ptr->status == QMI_PORT_TO_OPEN)
      {
        qmux_add_data_port(elem_ptr->sio_port, elem_ptr->ep_id,
               elem_ptr->consumer_pipe_num, elem_ptr->producer_pipe_num);
      }
      else
      {
        qmux_remove_data_port(elem_ptr->ep_id);
      }
    }

    PS_SYSTEM_HEAP_MEM_FREE(elem_ptr);
  }

} /* qmuxi_port_config_from_queue() */

/*===========================================================================
  FUNCTION QMUX_GET_DEFAULT_EP_ID_FROM_INST

  DESCRIPTION
    This function returns default EP id of the QMI instance
  
  PARAMETERS  
    qmi_inst:  QMI instance
  
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint32 qmux_get_default_ep_id_from_inst
(
  qmi_instance_e_type  qmi_inst
)
{
  uint32  ep_id = 0;
/*-------------------------------------------------------------------------*/

  QMI_ENTER_CRIT_SECTION(&qmi_port_cfg_crit_section);
  if (qmi_inst < QMI_INSTANCE_MAX)
  {
    if (qmux_state[qmi_inst].io.ports_active)
    {
      ep_id = qmux_state[qmi_inst].io.port_info.default_ep_id;
    }
  }
  QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);

  return ep_id;

} /* qmux_get_default_ep_id_from_inst() */

/*===========================================================================
  FUNCTION QMUX_GET_INST_FROM_EP_ID

  DESCRIPTION
    This function returns the QMI instance which has ep_id as the default EP
  
  PARAMETERS
    ep_id:  physical data port end point id
  
  DEPENDENCIES
    None

  RETURN VALUE
    QMI instance if success
    QMI_INSTANCE_MAX if failure

  SIDE EFFECTS
    None
===========================================================================*/
extern qmi_instance_e_type qmux_get_inst_from_ep_id
(
  uint32  ep_id
)
{
  qmi_instance_e_type  qmi_inst = QMI_INSTANCE_MAX;
  int                  i;
/*-------------------------------------------------------------------------*/

  if (ep_id == 0)
  {
    return QMI_INSTANCE_MAX;
  }

  QMI_ENTER_CRIT_SECTION(&qmi_port_cfg_crit_section);
  for (i = 0; i < QMI_INSTANCE_MAX; i++)
  {
    if (qmux_state[i].io.ports_active &&
        qmux_state[i].io.port_info.default_ep_id == ep_id)
    {
      qmi_inst = (qmi_instance_e_type)i;
      break;
    }
  }
  QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);

  return qmi_inst;

} /* qmux_get_inst_from_ep_id() */

/*===========================================================================
  FUNCTION QMUX_ADD_CONTROL_PORT

  DESCRIPTION
    This function adds an QMI control port for open
  
  PARAMETERS
    sio_port:       SIO port id
    default_ep_id:  default end point id
  
  DEPENDENCIES
    None

  RETURN VALUE
    TRUE: success
    FALSE: failure

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_add_control_port
(
  sio_port_id_type  sio_port,
  uint32            default_ep_id
)
{
  boolean  result = FALSE;
  int      i;
/*-------------------------------------------------------------------------*/

  if (sio_port == SIO_PORT_NULL)
  {
    LOG_MSG_ERROR_0("Invalid SIO port");
    return FALSE;
  }

  if (qmuxi_queue_port_request(
         TRUE,  QMI_PORT_TO_OPEN, sio_port, default_ep_id, 0, 0) == TRUE)
  {
    LOG_MSG_INFO1_1("Control port 0x%x open queued", sio_port);
    return TRUE;
  }

  LOG_MSG_INFO1_2("Adding QMI control port 0x%x, default EP 0x%x",
                  sio_port, default_ep_id);

  QMI_ENTER_CRIT_SECTION(&qmi_port_cfg_crit_section);

  /* Check if the port already exists */
  for (i = 0; i < QMI_INSTANCE_MAX; i++)
  {
    if (qmux_state[i].io.ports_active &&
        qmux_state[i].io.port_info.sio_port == sio_port)
    {
 
      LOG_MSG_ERROR_1("QMI control port 0x%x exists", sio_port);
      QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);
      return FALSE;
    }
  }

  /* Find an empty slot */
  for (i = QMI_INSTANCE_MIN; i < QMI_INSTANCE_MAX; i++)
  {
    /* Reserve QMI_INSTANCE_SHIM for the QMUX QCSI shim layer */
    if (i == QMI_INSTANCE_SHIM)
    {
      continue;
    }
    if (qmux_state[i].initialized && !qmux_state[i].io.ports_active)
    {
      qmux_state[i].io.port_info.sio_port      = sio_port;
      qmux_state[i].io.port_info.default_ep_id = default_ep_id;
      qmux_state[i].io.port_info.frame_mode    = QMUX_FRAME_MODE_IF1;
      qmux_state[i].io.port_info.qmi_stream    = QMUX_DEVSTREAM_CONTROL;
      
      result = qmuxi_open_instance((qmi_instance_e_type)i, sio_port);
      if (result)
      {
        qmux_state[i].io.ports_active = TRUE;
        /* Generate SYNC indication for now opened port */ 
        qmi_ctl_send_sync((qmi_instance_e_type)i);
      }
      break;
    }
  }

  QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);

  if (!result)
  {
    LOG_MSG_ERROR_1("Failed to add control port 0x%x", sio_port);
  }
  return result;

} /* qmux_add_control_port() */

/*===========================================================================
  FUNCTION QMUX_REMOVE_CONTROL_PORT

  DESCRIPTION
    This function removes a QMI control port
  
  PARAMETERS  
    sio_port:  SIO port id
  
  DEPENDENCIES
    None

  RETURN VALUE
    TRUE: success
    FALSE: failure

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_remove_control_port
(
  sio_port_id_type  sio_port
)
{
  boolean  result = FALSE;
  int      i;
/*-------------------------------------------------------------------------*/

  if (sio_port == SIO_PORT_NULL)
  {
    LOG_MSG_ERROR_0("Invalid SIO port");
    return FALSE;
  }

  if (qmuxi_queue_port_request(
         TRUE,  QMI_PORT_TO_CLOSE, sio_port, 0, 0, 0) == TRUE)
  {
    LOG_MSG_INFO1_1("Control port 0x%x close queued", sio_port);
    return TRUE;
  }

  LOG_MSG_INFO1_1("Removing QMI control port 0x%x", sio_port);

  QMI_ENTER_CRIT_SECTION(&qmi_port_cfg_crit_section);

  /* Find the port */
  for (i = 0; i < QMI_INSTANCE_MAX; i++)
  {
    if (qmux_state[i].io.ports_active &&
        qmux_state[i].io.port_info.sio_port == sio_port)
    {
      result = qmuxi_close_instance((qmi_instance_e_type)i);
      qmux_state[i].io.ports_active = FALSE;
      break;
    }
  }

  QMI_LEAVE_CRIT_SECTION(&qmi_port_cfg_crit_section);

  if (!result)
  {
    LOG_MSG_ERROR_1("Failed to remove QMI control port 0x%x", sio_port);
  }
  return result;

} /* qmux_remove_control_port() */

/*===========================================================================
  FUNCTION QMUX_ADD_DATA_PORT

  DESCRIPTION
    This function adds an Rmnet data port for open
  
  PARAMETERS  
  
  DEPENDENCIES
    None

  RETURN VALUE
    TRUE: success
    FALSE: failure

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_add_data_port
(
  sio_port_id_type  sio_port,
  uint32            ep_id,
  uint32            consumer_pipe_num,
  uint32            producer_pipe_num
)
{
  boolean  result = FALSE;
/*-------------------------------------------------------------------------*/

  if (ep_id == 0)
  {
    LOG_MSG_ERROR_0("Invalid ep_id");
    return FALSE;
  }

  if (qmuxi_queue_port_request(
            FALSE,  QMI_PORT_TO_OPEN, sio_port,
            ep_id, consumer_pipe_num, producer_pipe_num) == TRUE)
  {
    LOG_MSG_INFO1_1("Data EP 0x%x open queued", ep_id);
    return TRUE;
  }

  result = rmnet_phys_xport_open_close_cmd(
                RMNET_XPORT_CMD_PORT_OPEN,
                sio_port, ep_id, consumer_pipe_num, producer_pipe_num);
  if (result)
  {
    qmi_port_cfg_in_progress = TRUE;
  }
  else
  {
    LOG_MSG_ERROR_1("Failed to add data EP 0x%x", ep_id);
  }
  return result;

} /* qmux_add_data_port() */

/*===========================================================================
  FUNCTION QMUX_REMOVE_DATA_PORT

  DESCRIPTION
    This function removes an Rmnet data port
  
  PARAMETERS  
    ep_id:  data port EP id
  
  DEPENDENCIES
    None

  RETURN VALUE
    TRUE: success
    FALSE: failure

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_remove_data_port
(
  uint32  ep_id
)
{
  boolean  result = FALSE;
/*-------------------------------------------------------------------------*/

  if (ep_id == 0)
  {
    LOG_MSG_ERROR_0("Invalid ep_id");
    return FALSE;
  }

  if (qmuxi_queue_port_request(
           FALSE,  QMI_PORT_TO_CLOSE, 0, ep_id, 0, 0) == TRUE)
  {
    LOG_MSG_INFO1_1("Data EP 0x%x close queued", ep_id);
    return TRUE;
  }

  result = rmnet_phys_xport_open_close_cmd(
                          RMNET_XPORT_CMD_PORT_CLOSE,
                          0, ep_id, 0, 0);
  if (result)
  {
    qmi_port_cfg_in_progress = TRUE;
  }
  else
  {
    LOG_MSG_ERROR_1("Failed to remove data EP 0x%x", ep_id);
  }
  return result;

} /* qmux_remove_data_port() */

