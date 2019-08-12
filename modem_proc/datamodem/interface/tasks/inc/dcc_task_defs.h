#ifndef DCC_TASK_DEFS_H
#define DCC_TASK_DEFS_H
/*===========================================================================

                        D C C _ D E F S . H

DESCRIPTION

  Data Common Control Task header file.
  All definitions and structures needed by DCC task modules.


Copyright (c) 2009-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/inc/dcc_task_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/14   skc     Removing FEATURE_UW_FMC
10/21/14   sk      QMI-DMS Service QCSI migration.
10/10/14   ssh     IP Translation Engine 464XLAT 
11/01/13   rk      Removed feature mobileap.
09/27/13   wc      Support dynamic port configuration
09/14/12   ss      Added TFTP ALG client command.
08/16/12   wc      Reserve QSAP signal
02/10/12   rt      Added support for SoftAPv6
02/01/12   am      Added QCMAP DCC signal and commands.
11/03/11   sm      RTSP ALG Changes.
08/03/11   sa      GSDI cleanup changes.
08/12/11   ss      Added DCC_SIP_ALG_CLIENT_CMD.
08/12/11   cp      Added the support for PPTP ALG.
08/10/11   wc      Data aggregation based on CPU utilization.
03/09/11   kk      Removing DCC command buffer dependency from RmNet.
12/17/10   kk      Removing DCC command buffer dependency from QMI services.
11/08/10   mct     Support for QMI AKAv2.
10/19/10   sy      Removing Iface/Flow/QMG DCC data buffers.
09/24/10   pp      Facilitate user memory instead of DCC data buffers
                   while posting DCC commands.
08/20/10   bd      Fixed CAT_MAX_DATA_LEN value
07/09/10   sk      Added QMI PBM related changes
06/11/10   mib     Add GSTK decoded format
10/23/09   ar      Add supprot for QMUX power save indication filtering
                   and port configuration from NVRAM.
10/08/09   kk      Windows7 compliance - DMS changes.
02/19/09   am      Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#ifdef __cplusplus
extern "C"
{
#endif

#ifndef FEATURE_WINCE
#include "rex.h"
#include "task.h"
#endif /* FEATURE_WINCE */

#include "queue.h"
#include "sio.h"
#include "rdevmap.h"

#include "ps_iface_defs.h"
#include "ds_Utils_DebugMsg.h"

#ifdef FEATURE_DATA_QMI
#include "ds_qmi_if.h"
#include "ds_qmi_if_ext.h"

#ifdef FEATURE_DATA_QMI_PDS_COUPLED
#include "lbs.h"
#endif /* FEATURE_DATA_QMI_PDS_COUPLED */

#ifdef FEATURE_DATA_QMI_EAP
#include "ps_eap.h"
#include "ps_eap_sim_aka.h"
#include "ps_utils_aka.h"
#endif


#ifdef FEATURE_DATA_QMI_CAT
#include "gstk_exp.h"
#endif /* FEATURE_DATA_QMI_CAT */

#ifdef FEATURE_DATA_QMI_AT
#include "dsatclient.h"
#endif /* FEATURE_DATA_QMI_AT */

#endif /* FEATURE_DATA_QMI */

#ifdef FEATURE_DUAL_SIM
#include "cm_dualsim.h"
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type * rex_dcc_tcb;
#endif

#define DCC_SIG_ARR_LEN REX_SIGS_ARRAY_LEN

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  #define DCC_SET_SIGNAL( sig )                                             \
  {                                                                         \
    (void)rex_set_sigs( rex_dcc_tcb,                                        \
                       (rex_sigs_type) 1 << (rex_sigs_type)( sig ) );       \
  }
#else
  #define DCC_SET_SIGNAL( sig )                                             \
    (void)rex_set_sigs( &dcc_tcb, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )
#endif

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  #define DCC_CLR_SIGNAL( sig )                                             \
  {                                                                         \
    (void)rex_clr_sigs( rex_dcc_tcb,                                        \
                        (rex_sigs_type) 1 << (rex_sigs_type)( sig ) );      \
  }
#else
  #define DCC_CLR_SIGNAL( sig )                                             \
    (void)rex_clr_sigs( &dcc_tcb, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )
#endif

/*--------------------------------------------------------------------------
  DCC Task Signal Definitions.  The value of existing signals
  should not be changed while adding or deleting a signal.  Some signals
  (marked Reserved) have fixed value that should stay as it is.
--------------------------------------------------------------------------*/
typedef enum
{
  DCC_CMD_Q_SIGNAL                =  1,   /* DCC command Q signal          */
  DCC_DOG_HB_REPORT_SIGNAL        =  3,   /* Dog heart beat report signal   */
  DCC_NV_CMD_SIGNAL               =  4,   /* NV cmd event signal           */
  DCC_SIG_MASK_CHANGE_SIGNAL      =  5,   /* Indicates sig mask has changed*/

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

#if defined(FEATURE_RMNET_DATA_AGG_TIMER) && defined(FEATURE_CPU_BASED_FLOW_CONTROL)
  DCC_RMNET_FC_SIGNAL             = 10,   /* FC signal to rmnet            */
#endif /* FEATURE_RMNET_DATA_AGG_TIMER && FEATURE_CPU_BASED_FLOW_CONTROL */

/*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  DCC_TASK_OFFLINE_SIGNAL         = 13,   /* Reserved TASK_OFFLINE_SIG     */
  DCC_TASK_STOP_SIGNAL            = 14,   /* Reserved TASK_STOP_SIG        */
  DCC_TASK_START_SIGNAL           = 15,   /* Reserved TASK_START_SIG       */

#ifdef FEATURE_QSAP_SERVICE
  DCC_QSAP_SIGNAL                 = 16,   /* Reserved for QSAP             */
#endif /* FEATURE_QSAP_SERVICE */

#ifdef FEATURE_8960_SGLTE_FUSION  
  DCC_DS_PROFILE_REMOTE_NOTIFY_CLIENT_SIGNAL  = 17, /* QCCI-notify wds service up ind */
  DCC_DS_PROFILE_REMOTE_SEND_MSG_WAIT_SIGNAL  = 18, /* QCCI Client Init Sig */
  DCC_DS_PROFILE_REMOTE_OS_TIMER_SIGNAL = 19, /* QCCI Timer expire sig */
#endif /* FEATURE_8960_SGLTE_FUSION */
  DCC_DSD_RDR_SIGNAL            = 20,   /* Reader sig for QMI DSD    */
  
  DCC_DPM_RM_WAIT_SIGNAL        = 21,   /* DPM Wait signal */
  DCC_DFS_RDR_SIGNAL            = 22,   /* Reader sig for QMI DFS    */
  DCC_ATP_RDR_SIGNAL            = 23,   /* Reader sig for QMI ATP    */
  DCC_DMS_RDR_SIGNAL            = 24,   /* Reader sig for QMI DMS   */
  DCC_MAX_SIGNALS                         /* use next value for enum       */
} dcc_sig_enum_type;


/*---------------------------------------------------------------------------
  Data Task Command Definitions. These are the commands that can be sent to
  one.
---------------------------------------------------------------------------*/
typedef enum
{
  DCC_MIN_CMD                                  = -1,

#ifdef FEATURE_DATA_QMH
  /*-------------------------------------------------------------------------
    Command for QMH logical iface module
  -------------------------------------------------------------------------*/
  DCC_QMH_PROXY_IFACE_MSG_CMD     =  1,
#endif /* FEATURE_DATA_QMH */

  /*-------------------------------------------------------------------------
    Network interface - command to handle WWAN Rm state machine events
  -------------------------------------------------------------------------*/
  DCC_RMNET_SM_EV_CMD = 5,

  /*-------------------------------------------------------------------------
    Network interface - commands for RmNet
  -------------------------------------------------------------------------*/
  DCC_RMNET_SM_CMD = 6,

#ifdef FEATURE_DATA_QMI
  /*-------------------------------------------------------------------------
    Network interface - QMI receive command
  -------------------------------------------------------------------------*/
  DCC_QMUX_RX_CMD = 10,

  /*-------------------------------------------------------------------------
    Network interface - command to QMI module
  -------------------------------------------------------------------------*/
  DCC_QMI_CMD = 11,

  /*-------------------------------------------------------------------------
    Network interface - command to QMI Charger module
  -------------------------------------------------------------------------*/
  DCC_QMI_CHARGER_CMD = 12,

  DCC_CM_MSG_IFACE_CMD = 13,

  DCC_QMI_VOICE_CMD = 14,

  DCC_QMI_VOICE_CM_IF_CMD = 15,

  DCC_QMI_PBM_CMD = 16,

  DCC_QMI_PBM_IF_CMD = 17,
#endif /* FEATURE_DATA_QMI */

  /*-------------------------------------------------------------------------
    MIP CCoA cmds
  -------------------------------------------------------------------------*/
#if defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DATA_PS_MIP_CCOA)
   DCC_MIPCCOA_START_CMD                       = 20,
   DCC_MIPCCOA_SIP_IFACE_UP_CMD                = 21,
   DCC_MIPCCOA_SIP_IFACE_UNAVAILABLE_CMD       = 22,
   DCC_MIPCCOA_MIP_UP_CMD                      = 23,
   DCC_MIPCCOA_MIP_FAILURE_CMD                 = 24,
#ifdef FEATURE_DS_MOBILE_IP_DEREG
   DCC_MIPCCOA_MIP_DEREGED_CMD                 = 25,
#endif
   DCC_MIPCCOA_CLOSE_CMD                       = 26,
#endif /* defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DATA_PS_MIP_CCOA) */

#ifdef FEATURE_DATA_PS_MIPV6
  DCC_MIP6_MSM_START_CMD                       = 30,
  DCC_MIP6_MSM_BOOTSTRAP_SUCCESS_CMD           = 31,
  DCC_MIP6_MSM_BOOTSTRAP_FAIL_CMD              = 32,
  DCC_MIP6_MSM_SIP6_IFACE_UP_CMD               = 33,
  DCC_MIP6_MSM_SIP6_IFACE_UNAVAILABLE_CMD      = 34,
  DCC_MIP6_MSM_SIP6_IFACE_CONFIGURING_CMD      = 35,
  DCC_MIP6_MSM_SIP6_IFACE_PREFIX_UPDATE_CMD    = 36,
  DCC_MIP6_MSM_IPSEC_TPORT_IFACE_UP_CMD        = 37,
  DCC_MIP6_MSM_IPSEC_TPORT_IFACE_DOWN_CMD      = 38,
  DCC_MIP6_MSM_IPSEC_TUNNEL_IFACE_UP_CMD       = 39,
  DCC_MIP6_MSM_IPSEC_TUNNEL_IFACE_DOWN_CMD     = 40,
  DCC_MIP6_MSM_RSM_UP_CMD                      = 41,
  DCC_MIP6_MSM_RSM_DOWN_CMD                    = 42,
  DCC_MIP6_MSM_CLOSE_CMD                       = 43,
#endif /* FEATURE_DATA_PS_MIPV6 */

#ifdef FEATURE_DATA_PS_IWLAN
  /*-------------------------------------------------------------------------
    IWLAN IFACE commands.
  -------------------------------------------------------------------------*/
  DCC_IWLAN_IFACE_BRING_UP_CMD      = 50,   /* bring up IWLAN iface      */
  DCC_IWLAN_IFACE_TEAR_DOWN_CMD     = 51,   /* tear down IWLAN iface     */
  DCC_IWLAN_IFACE_IPSEC_UP_EV_CMD   = 52,   /* IPSEC iface is UP         */
  DCC_IWLAN_IFACE_IPSEC_DOWN_EV_CMD = 53,   /* IPSEC iface is DOWN       */
  DCC_IWLAN_IFACE_WLAN_UP_EV_CMD    = 54,   /* WLAN iface is UP          */
  DCC_IWLAN_IFACE_WLAN_DOWN_EV_CMD  = 55,   /* WLAN iface is DOWN        */
  DCC_IWLAN_IFACE_DNS_CB_CMD        = 56,   /* DNS Callback              */
#ifdef FEATURE_DATA_PS_IWLAN_3GPP
  DCC_IWLAN_MMGSDI_CMD              = 57,   /* MMSGSDI API callbacks     */
#endif /* FEATURE_DATA_PS_IWLAN_3GPP */
#endif /* FEATURE_DATA_PS_IWLAN */

#ifdef FEATURE_DATA_PS_UICC
  /*-------------------------------------------------------------------------
    UICC command
  -------------------------------------------------------------------------*/
  DCC_UICC_CMD                       = 60,   /* UICC N/W interface cmds  */
  DCC_UICC_SM_CMD                    = 61,   /* UICC State Machine cmds  */
#endif /* FEATURE_DATA_PS_UICC */

  /*-------------------------------------------------------------------------
    SLIP IFACE call control commands
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_SLIP
  DCC_SLIP_IFACE_HDLR_CP_DEACTIVATING_EV_CMD     = 80,
  DCC_SLIP_IFACE_HDLR_CP_DOWN_EV_CMD             = 81,
  DCC_SLIP_IFACE_HDLR_CP_UP_EV_CMD               = 82,
  DCC_SLIP_IFACE_HDLR_ADDR_FAILURE_EV_CMD        = 83,
  DCC_SLIP_IFACE_HDLR_ADDR_RELEASED_EV_CMD       = 84,
  DCC_SLIP_IFACE_HDLR_ADDR_CONFIG_SUCCESS_EV_CMD = 85,

#endif /* FEATURE_DATA_PS_SLIP */

  /*-------------------------------------------------------------------------
    Command for processing Logical Iface Callback
  -------------------------------------------------------------------------*/
  DCC_LOGICAL_IFACE_ASSOC_IFACE_EV_CMD  = 86,

  /*-------------------------------------------------------------------------
    Command for processing associated flow events in the logical flow framework
  -------------------------------------------------------------------------*/
  DCC_LOGICAL_FLOW_ASSOC_FLOW_EV_CMD    = 87,

  /*-------------------------------------------------------------------------
    NAT iface control/ALG specific commands
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    QMI Service init and Request cmd
  -------------------------------------------------------------------------*/
  DCC_QMI_INIT_LEGACY_SERVICES_CMD     = 91,
  DCC_QMI_RECV_LEGACY_SERVICES_REQ_CMD = 92,
  DCC_RMNET_XPORT_CMD                  = 93,

#ifdef FEATURE_8960_SGLTE_FUSION
  DCC_DS_PROFILE_REMOTE_PERFORM_OPERATION_CMD = 107,
#endif /* FEATURE_8960_SGLTE_FUSION */

  DCC_DSD_SVC_CMD = 109,
  DCC_REV_IP_TRANSPORT_PERFORM_OPERATION_CMD = 110,
  DCC_DFS_SVC_CMD = 111,
  DCC_ATP_SVC_CMD = 112,
  
#ifdef FEATURE_DATA_PS_464XLAT
  DCC_CLAT_SM_START_CMD = 112,
  DCC_CLAT_SM_STOP_CMD = 113,
  DCC_CLAT_SM_IPV4_IFACE_UP_CMD = 114,
  DCC_CLAT_SM_IPV4_IFACE_DOWN_CMD = 115,
  DCC_CLAT_SM_IPV6_IFACE_UP_CMD = 116,
  DCC_CLAT_SM_IPV6_IFACE_DOWN_CMD = 117,
  DCC_CLAT_SM_IPV6_IFACE_PREFIX_UPDATE_CMD = 118,
  DCC_CLAT_SM_PLAT_AVAILABLE_CMD = 119,
  DCC_CLAT_SM_PLAT_UNAVAILABLE_CMD = 120,
  DCC_CLAT_SM_DNS_PROXY_ENABLED_CMD = 121,
  DCC_CLAT_SM_DNS_PROXY_DISABLED_CMD = 122,
  DCC_CLAT_SM_DNS64_CALLBACK_CMD = 123,
  DCC_CLAT_SM_DPM_CONTEXT_REGISTERED_CMD = 124,
#endif /* FEATURE_DATA_PS_464XLAT */
  DCC_DMS_SVC_CMD = 125,
  /* Always must be last */
  DCC_MAX_DEFINED_CMD_TYPES
} dcc_cmd_enum_type;

/*--------------------------------------------------------------------------
  Type definition for the signal handler.
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
---------------------------------------------------------------------------*/

typedef boolean (*dcc_sig_handler_type)
(
  dcc_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*--------------------------------------------------------------------------
  Type definition for the command handler
---------------------------------------------------------------------------*/

typedef void (*dcc_cmd_handler_type)
(
  dcc_cmd_enum_type cmd,        /* Actual command to be processed         */
  void *user_data_ptr           /* Command specific user parameters       */
);

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/

typedef struct
{
  dcc_sig_handler_type sig_handler;
  void *user_data_ptr;
} dcci_sig_handler_info_type;

/*--------------------------------------------------------------------------
  DCC Task Command queues and Command Data Buffers
---------------------------------------------------------------------------*/

typedef struct
{
  q_link_type       link;           /* For linking onto cmd queue          */
  dcc_cmd_enum_type cmd;            /* Actual command to be processed      */
  boolean           user_allocated; /* True indicates user_data_ptr is
                                       client's own memory, not from DCC cmd
                                       data buffer memory                  */
  void              *user_data_ptr; /* Command specific user parameters    */
} dcci_cmd_type;

/*---------------------------------------------------------------------------
  Struct used for all QMI commands
---------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_QMI
typedef struct
{
  uint16  id;
  union
  {
#ifdef FEATURE_DATA_QMI_PDS_COUPLED
    union
    {
      qmi_lbs_ipc_event_params_struct lbs_msg;
    } pds;
#endif /* FEATURE_DATA_QMI_PDS_COUPLED */

  #if (defined FEATURE_DATA_QMI_CAT)
    #define CAT_MAX_DATA_LEN  258 /* BER-TLV data object for proactive command has
                                     Tag     - 1 byte
                                     Length  - 1 or 2 bytes with max value upto 255
                                     Value */
    union
    {
      struct
      {
          uint32                    cmd_ref_id;
          int32                     raw_data_len;
          uint8                     raw_data_buffer[ CAT_MAX_DATA_LEN ];
      } cat_raw_proactive_cmd;
      struct
      {
          void *                                user_data;
          gstk_general_envelope_rsp_enum_type   cat_env_result;
      } cat_env_rsp;
      struct
      {
          uint32                    pc_setup_evt_list;
      } cat_setup_evt_list_cmd;
      struct
      {
          uint8                    proactive_session_end_type;
      } cat_end_proactive_cmd;
      struct
      {
        gstk_cmd_from_card_type    decoded_cmd;
      } cat_decoded_proactive_cmd;
      struct
      {
        uint16  refresh_mode;
        uint16  refresh_stage;
      } cat_refresh_cmd;
      struct
      {
        mmgsdi_cnf_enum_type  cnf_type;
        mmgsdi_cnf_type       cnf_data;
      } cat_refresh_reg_info;
    } cat;
  #endif /* (defined FEATURE_DATA_QMI_CAT) */

  #if (defined FEATURE_DATA_QMI_UIM)
  union
  {
    struct
    {
      mmgsdi_return_enum_type status;
      mmgsdi_cnf_enum_type    cnf_type;
      mmgsdi_cnf_type         cnf_value;
    } operation_complete;
    struct
    {
      mmgsdi_event_data_type  evt;
    } event;
  } uim;
  #endif /* (defined FEATURE_DATA_QMI_UIM) */

    /* Generic data payload */
    void * payload_ptr;

  } data;
} dcc_qmi_cmd_type;

#endif /* FEATURE_DATA_QMI */

/*---------------------------------------------------------------------------
  DCC command user data type
---------------------------------------------------------------------------*/
typedef struct
{
  union
  {
#ifdef FEATURE_DATA_QMI
    /*-----------------------------------------------------------------------
      QMI commands
    -----------------------------------------------------------------------*/
    dcc_qmi_cmd_type  qmi;  /* Contains rmsm event number  */
#endif

    void                                *user_data_ptr;
  }cmd;
} dcc_cmd_data_buf_type;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DCC_TASK_DEFS_H */
