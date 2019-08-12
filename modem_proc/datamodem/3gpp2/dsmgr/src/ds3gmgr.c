/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             3 G   D A T A   S E R V I C E S   M A N A G E R

GENERAL DESCRIPTION
  This software unit contains routines to initialize the 3G Dsmgr sub-task,
  and to process received signals and commands. Also contains other misc.
  functions for setup/cleanup of Autodetect watermarks and mode-specific
  handler registration.

EXTERNALIZED FUNCTIONS
  ds3g_init()
    Initializes the 3G Dsmgr sub-task

  ds3g_process_cmds()
    Processes 3G Dsmgr commands

  ds3g_register_mode_sp_handler()
    Called by mode-specific handlers to register a handler function table
    for a specified mode

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds3g_rdm_init() and ds3g_init() must be called before any other 3G Dsmgr
  functions are invoked.  ds3g_rdm_init() must be called before calling
  the TMC function to start the data services task.


  Copyright (c) 2001 - 2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gmgr.c_v   1.7   28 Feb 2003 18:54:08   rchar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gmgr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/26/16   skc     Fixed issue with ATO command.
10/03/13   ssb     Inform QMI of the call termination triggered by LPM
09/13/13   tk      ATCoP changes for LTE and DSDS concurrency solution.
05/24/13   sd      TSTS changes for DS3G.
05/02/13   vs      NV item to check call origination is allowed before PS attach
01/25/13   sd      Mainlining Modem Statistics Feature
11/21/12   sd      Initialize DS3G PS SUBS ID to SUBS_ID_1.
11/21/12   sd      Feature Dual SIM related changes.
08/22/12   sk      Mainlining Nikel Changes.
07/13/12   jz      Support for emergency only mode when mandatory APN is disabled
03/12/12   ss      RPM: Fixed issue of oprt_mode and regd_serv_domain not
                   being set after UE power-up.
03/09/12   msh     Removed FEATURE_DATA_NIKEL
02/01/12   ss      RPM: Fixed of sending ATTACH request again due to PS session
                   initiation after PS ATTACH is ignored by network.
01/02/12   msh     Coan: Feature cleanup
01/13/12   sa      Added support for CSVT for CSFB targets.
09/20/11   ss      Fixed the issue of RPM parameters being read even when RPM
                   is disabled.
09/05/11   ss      Changes to support RPM.
08/09/11   sha     Global variable clean up.
03/23/11   sn      Globals cleanup for free floating of tasks.
03/15/11   mg      Global variable cleanup.
02/24/11   ttv     Fixed compilation warnings.
11/22/10   ttv     Added support for DSDS.
09/09/10   op      Migrated to MSG 2.0 macros
07/07/10   ms      Data statistics support for Field Test Display.
06/17/10   as      API to access ds3gi_mode for long term and Fusion purposes
09/22/09   vrk     Merged LTE changes.
08/22/08   ua      Added support for N/w initated Qos.
07/17/09   ls      Change FEATURE_CSIM to FEATURE_MMGSDI_SESSION_LIB
06/04/09   ls      Support CSIM feature
05/05/09   ss      Fixed Compiler Warnings.
03/04/09   sa      AU level CMI modifications.
11/26/08   sn      Fixed off-target lint errors.
01/09/08   yz      Added support for ISIM refresh voting feature.
10/01/08   yz      Fixed a return type mismatch for cm callback.
07/18/08   ar      Moved some routines to ds3gutil.c
04/02/08   sn      Fixed Klockwork errors.
01/04/08   psng    Fixed lint errors.
10/30/07   rsl     Allow CS+PS for GSM in ds3gi_call_limit_info[].
06/22/06   ar      Added MSH module initialization to ds3g_init().
10/24/05   ar      Added support for multiple CSD call instances.
09/16/05   as/gr   Added function ds3g_is_dorm_allowed_in_pkt_mode to allow
                   dormancy in WLAN mode.
09/09/05   vrk/gr  EPZID enhancements releated to L2/L3 ac
09/02/05   rc      Added support for Mobile Initiated QOS Modification.
08/26/05   ar      Added support for CS Data incall modification & fallback.
05/20/05   ar      Added support for DS_CM_CALL_PROGRESS_CMD event.  Lint
                   corrections.
02/16/04   rr      Added registration of data session counters callback
                   function with  CM in ds3g_init().
01/12/05   aku     Added support for SYS_SYS_MODE_WLAN in is_pref_pkt_mode
11/21/04   rc      Changed SYS_MAX_PRIMARY_PDP_CONTEXTS to SYS_MAX_PDP_CONTEXTS.
09/17/04   rc      Added support for power save mode operation. Added command
                   DS_CM_SS_PS_DATA_FAIL_CMD.
09/10/04   sb      S-PDP changes
07/12/04   dwp     Allow CS+PS for WCDMA in ds3gi_call_limit_info[].
10/07/03   vsk     Added ds3gi_call_limit_info to maintain the # calls
                   allowed in each system mode & also if con. cs&ps are allowed
09/16/03   ar      Added ds3g_get_call_was_connected() query.
08/21/03   rc      In ds_get_last_session_data_counters(), fixed code to
                   return the right result code to UI.
07/25/03   ak      When accessing the mode_hdlr_tbl, use the stored call
                   mode instead of ds3gi_mode.
07/21/03   usb     Added ds3g_is_pref_pkt_mode to validate currently
                   preferred mode for packet data calls.
06/02/03   snb     Downgrade "Invalid call Id" msg ERR to MSG_MED in
                   ds_get_current_session_data_counters. Called repeatedly
                   during +CGACT orig'd PS data call.
02/26/03   rc      Changed DS_CM_SS_SYS_MODE_CHG_CMD to DS_CM_SS_SRV_CHG_CMD.
                   Changed ds3gi_process_cm_ss_sys_mode_chg_cmd() to
                   ds3gi_process_cm_ss_srv_chg_cmd().
11/04/02   tmr     Added clearing of call_was_connected field in call info
                   structure.
10/14/02   vsk     Added support to get statistics for the call which has ended
08/19/02   ak      Updated for multiple calls.
08/13/02   dgy     Added return value to ds_get_call_information() and
                   ds_get_current_session_data_counters().
08/02/02   dgy     Added support for UI data call screen.
07/19/02   smp     Fix to ensure that the call is ended if DTR drops during
                   Data call setup.
06/18/02   tmr     Added ds3gi_process_msh_complete_ll_disconnect_cmd() handler.
05/10/02   ak      Added 707 signal processing.  Also removed some old
                   SIO & AT stuff.
04/19/02   vsk     Added PDP context preservation support
04/12/02   rc      Added DS3G_INITIATE_CALL_CMD event in ds3g_process_cmds().
03/15/02   smp     In ds3g_init(), initialize end_result state variable to
                   DSAT_NO_CARRIER.
02/26/02   tmr     Added handlers ds3gi_process_msh_complete_ll_connect_cmd()
                   and ds3gi_process_msh_release_call_cmd().
02/22/02   smp     Added variable to store mode-sp. handler sio_rx_func_ptr
                   (to allow for switching between online cmd/data). Added
                   handling for aborting AT commands.
02/19/02   rlp     Code review changes.
02/07/02   vsk     Added ds3gi_get_current_network_mode() function
02/05/02   rlp     Added online-command mode support.
01/02/02   tmr     Added conditional DS3G_GSM_MODE hard coding for GSM builds
12/14/01   smp     Initialize the ring_counter state variable in ds3g_init().
10/31/01   smp     Added error handling for 1ST_SIO_RX_SIG being set in an
                   invalid state.
08/17/01   smp     Renamed ATCOP files/functions/data types.
06/05/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */
#include "amssassert.h"
#include "cm.h"
#include "ds3gcmif.h"
#include "ds3gmgr.h"
#include "ds707_nv_util.h"
#include "ds3gmgrint.h"
#include "ds3gmshif.h"
#include "ds3grdmif.h"
#include "ds3gtimer.h"
#include "dsat_v.h"
#include "dsm.h"
#include "dstask.h"
#include "dstaski.h"
#include "dstask_v.h"
#include "err.h"
#include "msg.h"
#include "rex.h"
#include "sys.h"
#include "task.h"
#include "data_msg.h"
#include "dsati.h"
#include "dsmsgr.h"
#include "ds3gutil.h"
#include "ds3gsubsmgr.h"
#include "ds3gstkproc.h"
#include "ds3gdsdif.h"
#include "ds3gcsdhdlr.h"
#include "dsumtspdpregint.h"
#include "ds3gflowmgr.h"
#include "ds3gcfcmif.h"

#ifdef FEATURE_MMGSDI
#include "ds3gmmgsdiif.h"
#endif /* FEATURE_MMGSDI */

#include "dsucsd.h"
#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_CS)) || \
     (defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)))
#include "dsucsdhdlr.h"
#endif

#ifdef FEATURE_EHRPD
#include "ds707_data_session_profile.h"
#include "ds707_sys_chg_hdlr.h"
#endif /* FEATURE_EHRPD */
#include "ds3gcmif_stats.h"
#ifdef FEATURE_DATA_SV_MODE
#include "trm.h"
#endif /* FEATURE_DATA_SV_MODE */

#ifdef FEATURE_DATA_RPM
#include "ds_3gpp_rpm.h"
#endif /* FEATURE_DATA_RPM */

#ifdef FEATURE_8960_SGLTE_FUSION
#include "ds3g_qmi_nas_client.h"
#endif /* FEATURE_8960_SGLTE_FUSION */

#include "ds_dsd_ext_i.h"
#include "ds_dsd_apm_ext_i.h"
#include "ps_sys.h"

#include "ds_profile_db_internal.h"
#include "ds_3gpp_plmn_hdlr.h"
#include "ds3gdevmgr.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "dsgen_iface6_hdlr.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "ds3gstats.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Structure to store radio info received from CM via SS event
---------------------------------------------------------------------------*/
typedef struct
{
  sys_sys_mode_e_type     system_mode;
  sys_srv_status_e_type   srv_status;
}ds3g_radio_stack_info_s;

typedef struct
{
  ds3g_radio_stack_info_s  stack_info[CM_NO_STACKS];
}ds3g_radio_info_s;


/*---------------------------------------------------------------------------
  Structure passed with DS_CMD_DSMGR_LOW_LATENCY_CMD command.
---------------------------------------------------------------------------*/
typedef struct
{
  ps_sys_low_latency_traffic_info_type   latency_info;
  ps_sys_subscription_enum_type          subscription_id;
}ds3g_low_latency_traffic_info_type;



/*---------------------------------------------------------------------------
  Table of function ptrs based on Mode (GSM/WCDMA/CDMA) and call type (pkt
  or circuit-switched)
---------------------------------------------------------------------------*/
ds3g_hdlr_func_tbl_type  *ds3gi_hdlr_tbl[SYS_SYS_MODE_MAX][DS_NUM_CALL_TYPES] ={{NULL}};

/*---------------------------------------------------------------------------
  Call state info.
---------------------------------------------------------------------------*/
ds3gi_call_info_type  ds3gi_call_info[DS3GI_MAX_NUM_CALLS] = {{NULL}};

/*---------------------------------------------------------------------------
  Current CS subs ID
---------------------------------------------------------------------------*/
static sys_modem_as_id_e_type  ds3g_cs_subs_id = SYS_MODEM_AS_ID_1;

/*---------------------------------------------------------------------------
  Client id assigned by Call Manager.
---------------------------------------------------------------------------*/
static cm_client_id_type  ds3g_cm_client_id = 0;

/*---------------------------------------------------------------------------
  The operating mode the for the previous call
---------------------------------------------------------------------------*/
static sys_sys_mode_e_type  ds3g_last_call_mode = SYS_SYS_MODE_NONE;

/*---------------------------------------------------------------------------
  The call type for the previous call
---------------------------------------------------------------------------*/
static ds_call_type_e_type  ds3g_last_call_type = DS_CALL_TYPE_UNSPECIFIED;

/*-----------------------------------------------------------------------------
  Variable to store current oprt_mode_serv_domain_pref
-----------------------------------------------------------------------------*/
static cm_srv_domain_pref_e_type ds3g_oprt_mode_srv_domain[DS3GSUBSMGR_SUBS_ID_MAX];

static boolean ds3g_cm_client_initialized = FALSE;

/*---------------------------------------------------------------------------
  Cache to store the last reported radio info by CM via SS event.
  Stores details about the system and serving status on each radio stack
---------------------------------------------------------------------------*/
static ds3g_radio_info_s ds3g_cached_radio_info[DS3GSUBSMGR_SUBS_ID_COUNT];

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_SESSION_DATA_COUNTERS_CB

DESCRIPTION   This function should be invoked by UI for displaying data call
              statistics. It returns by reference the total count of bytes
              transmitted and received at RLC for the current call:

              *rx_from_network_byte_cnt - Total bytes received for the call
              *tx_to_network_byte_cnt   - Total bytes transmitted for the call

              The total byte count is accumulative for the call. If RAB is
              re-established after PDP context is preserved during the call,
              the total byte count includes bytes transmitted and received
              after RAB is re-established.

DEPENDENCIES  None

RETURN VALUE  This function returns the data statistics type, indicating if the
              information passed by reference, e.g., *rx_from_network_byte_cnt,
              is valid.

SIDE EFFECTS  None
===========================================================================*/
static boolean ds3g_get_current_session_data_counters_cb
(
  cm_call_id_type  call_id,                   /* Call ID */
  uint32          *rx_from_network_byte_cnt,  /* RX byte count for the call */
  uint32          *tx_to_network_byte_cnt     /* TX byte count for the call */
)
{
  ds_stats_e_type result;
  result = ds3g_get_current_session_data_counters(call_id,
                                                  rx_from_network_byte_cnt,
                                                  tx_to_network_byte_cnt);
  switch (result)
  {
    case (DS_STATS_VALID):
      return TRUE;
    case (DS_STATS_INVALID_CALL_ID):
      return FALSE;
    case (DS_STATS_UNAVAILABLE):
      return FALSE;
    default:
      // this case should never hit
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "ds_get_current_session_data_counters "
                      "returned incorrect type: %d", result);
      return FALSE;
  }
} /* ds_get_current_session_data_counters_cb() */

/*===========================================================================
FUNCTION      DS3G_RADIO_INFO_INIT

DESCRIPTION   Inits the global radio info cache to No System and No Srvc on
              all stacks

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ds3g_radio_info_init(void)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  uint8                       stack_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
      subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT;
      subs_indx++)
  {
    for(stack_indx = 0; stack_indx < CM_NO_STACKS; stack_indx++)
    {
      ds3g_cached_radio_info[subs_indx].stack_info[stack_indx].system_mode
        = SYS_SYS_MODE_NO_SRV;
      ds3g_cached_radio_info[subs_indx].stack_info[stack_indx].srv_status
        = SYS_SRV_STATUS_NO_SRV;
    }
  }
  return;
} /* ds3g_radio_info_init() */

/*=========================================================================== 
FUNCTION      DS3_LOW_LATENCY_IND_CB

DESCRIPTION   This callback is called to notify LOW Latency Traffic Indication

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void d3g_low_latency_ind_cb
(
  ps_sys_tech_enum_type           tech_type,
  ps_sys_event_enum_type          event_name,
  ps_sys_subscription_enum_type   subscription_id,
  void                           *event_info_ptr,
  void                           *user_data_ptr
)
{
  ds_cmd_type                         *cmd_ptr = NULL;
  ds3g_low_latency_traffic_info_type  *data_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3g_get_config_latency_info() )
  {
     DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Latency info Nv not enabled");
     return;
  } 

  if( event_info_ptr == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Event info pointer is Null");
    return;
  }


  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_low_latency_traffic_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_DSMGR_LOW_LATENCY_CMD;
    data_block_ptr = (ds3g_low_latency_traffic_info_type *)cmd_ptr->cmd_payload_ptr;
    memscpy(&(data_block_ptr->latency_info),
          sizeof(ps_sys_low_latency_traffic_info_type),
          event_info_ptr,
          sizeof(ps_sys_low_latency_traffic_info_type));
    data_block_ptr->subscription_id = subscription_id;
    ds_put_cmd(cmd_ptr);
  }

  return;
} /* d3g_low_latency_ind_cb */

/*=========================================================================== 

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3G_INIT

DESCRIPTION   This function performs power-up initialization of 3G Dsmgr.
              This includes initialization of some state variables and
              registration with Call Manager for handling of incoming/outgoing
              data calls.
              This function must be called once at data services task startup.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_init( void )
{
  uint8         i, j;
  ds_cmd_type  *cmd_ptr = NULL;
  int16         ps_errno = -1;

#if defined(FEATURE_EHRPD) && !defined(FEATURE_MMGSDI_SESSION_LIB)
  ds707_data_session_acl_info_type  *ds707_acl_info = NULL;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Register DSMGR command processing function with DS task*/
  ds_cmd_process_register(DS_MODULE_DSMGR, ds3g_process_cmds);

  /*-------------------------------------------------------------------------
    Initialize oprt_mode_srv_domain and RPLM_ID per subscription
  -------------------------------------------------------------------------*/
  for (i = DS3GSUBSMGR_SUBS_ID_MIN; i < DS3GSUBSMGR_SUBS_ID_COUNT; i++)
  {
    ds3g_oprt_mode_srv_domain[i] = CM_SRV_DOMAIN_PREF_NONE;
  }

  /*-------------------------------------------------------------------------
    Initialize the handler function table pointers to NULL.
  -------------------------------------------------------------------------*/
  for( i = 0; i <(uint8)SYS_SYS_MODE_MAX; i++ )
  {
    for( j = 0; j <(uint8)DS_NUM_CALL_TYPES; j++ )
    {
      ds3gi_hdlr_tbl[i][j] = NULL;
    }
  }

  /*-------------------------------------------------------------------------
    Explicitly initialize the call instance table
  -------------------------------------------------------------------------*/
  for (i =0; i < DS3GI_MAX_NUM_CALLS; i++)
  {
    ds3gi_call_info[i].call_id    = CM_CALL_ID_INVALID;
    ds3gi_call_info[i].call_state = DS3G_IDLE_CALL_STATE;
    ds3gi_call_info[i].call_was_connected = FALSE;
    ds3gi_call_info[i].call_mode  = SYS_SYS_MODE_NO_SRV;
  }

  /*-------------------------------------------------------------------------
    Setup critical section for call operations.
  -------------------------------------------------------------------------*/
  ds3gi_msh_init();

#ifdef FEATURE_DATA_IS707
  ds707_nv_util_traffic_ch_teardown_efs();
#endif /* FEATURE_DATA_IS707 */

  /*-------------------------------------------------------------------------
    Register data session counters callback function with CM
  -------------------------------------------------------------------------*/
  (void)cm_data_session_reg((cm_get_data_session_total_byte_ctrs_cb_f_type*)
                            ds3g_get_current_session_data_counters_cb);

  ds3g_mmgsdi_init();

  #if defined(FEATURE_EHRPD) && !defined(FEATURE_MMGSDI_SESSION_LIB)
#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
  if(ds3g_get_cm_client_init_status())
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/
  {
  /*-------------------------------------------------------------------------
    Because MMGSDI is not enabled, initialize APN Control list information
    to indicate no APN control list present.
  -------------------------------------------------------------------------*/
  ds707_acl_info = ds707_data_session_get_acl_info_ptr();
  ds707_acl_info->num_apn = -1;
  ds707_acl_info->ds3g_mmgsdi_init = TRUE;
  if ( ds707_acl_info->data_session_profile_init == TRUE &&
       ds707_acl_info->ds3g_mmgsdi_init == TRUE )
  {
    /*---------------------------------------------------------------------
      Check if mandatory APNs are enabled in profile and uicc card.
    ---------------------------------------------------------------------*/
    ds707_nv_util_data_session_check_mandatory_apns();
  }
  }
  #endif /* defined(FEATURE_EHRPD) && !defined(FEATURE_MMGSDI_SESSION_LIB) */

  /*-------------------------------------------------------------------------
    Initialize the last call's mode and type. Type is set to Packet.
    This will be updated at Run time
  -------------------------------------------------------------------------*/
  ds3g_last_call_mode = ds3g_get_current_network_mode();
  ds3g_last_call_type = DS_CALL_TYPE_PKT;

#ifdef FEATURE_8960_SGLTE_FUSION
  /* Check if DSD Sync is enabled */
  if(ds3g_qmi_nas_client_is_dsd_sync_enabled() == TRUE)
  {
    ds3g_qmi_nas_client_init();
  }
#endif /* FEATURE_8960_SGLTE_FUSION */

  /* Init the radio info cache to No Sys and No Srvc on power up */
  ds3g_radio_info_init();

  ds3gdevmgr_init();

  for ( i = PS_SYS_PRIMARY_SUBS; i < PS_SYS_SUBS_MAX; i++ )
  {
    if( 0 != ps_sys_event_reg_ex( PS_SYS_TECH_ALL,
                               PS_SYS_EVENT_LOW_LATENCY_TRAFFIC_STATUS,
                               (ps_sys_subscription_enum_type)i,
                               d3g_low_latency_ind_cb,
                               NULL,
                               &ps_errno ) )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Couldn't register for "
                                       "PS_SYS_EVENT_LOW_LATENCY_TRAFFIC_STATUS!");
    }
  }


  /*-------------------------------------------------------------------------
    Post a command to notify DS3G/DSD init is complete
  -------------------------------------------------------------------------*/
  if( (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_DSMGR_INIT_COMPLETE;
    ds_put_cmd(cmd_ptr);
  }

  return;
} /* ds3g_init() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_INIT_COMPLETE_CMD

DESCRIPTION   This function is called to process the init complete command.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_init_complete_cmd(void)
{
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_1;
  uint8                   index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
     Initialize ATCOP with mode for all subs id
  -------------------------------------------------------------------------*/
  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_process_init_complete_cmd");

  do
  {
    ds3g_dsat_init_service_mode(subs_id);
    index++;
    subs_id++;
  } while (index < DS3G_MAX_SUBS);

  /*------------------------------------------------------------------------- 
    Register for all DS3G Timers and timer events
  -------------------------------------------------------------------------*/
  ds3gi_timer_process_init_complete_cmd();
  ds3g_stats_init_complete_cmd();

  return;
}/* ds3g_process_init_complete_cmd() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_LOW_LATENCY_COMPLETE_CMD

DESCRIPTION   This function is called to process the low latency command.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_low_latency_cmd
(
  const ds_cmd_type  *cmd_ptr
)
{
  ds3geventmgr_event_info_type         event_info;
  ds3geventmgr_filter_type             filter_info;
  ds3g_low_latency_traffic_info_type  *data_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  data_block_ptr
    = (ds3g_low_latency_traffic_info_type *)cmd_ptr->cmd_payload_ptr;

  memset(&event_info, 0, sizeof(ds3geventmgr_event_info_type));
  memscpy(
     &(event_info.latency_info),
     sizeof(ps_sys_low_latency_traffic_info_type),
     (ps_sys_low_latency_traffic_info_type *)(&(data_block_ptr->latency_info)),
     sizeof(ps_sys_low_latency_traffic_info_type));

  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));
  filter_info.ps_subs_id = data_block_ptr->subscription_id;
  filter_info.tech = PS_SYS_TECH_ALL;

  if(ds3geventmgr_set_event_info(DS3GEVENTMGR_LATENCY_INFO_EV,
                                 (void *)&event_info,&filter_info) == FALSE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Unable to provide LATENCY info to registered clients");
  }

  return;
} /* ds3g_process_low_latency_cmd */

/*===========================================================================
FUNCTION      DS3G_INIT_CRIT_SECT

DESCRIPTION   Called once at mobile power-up.  Initializes all DS3G 
              critical sections 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3g_init_crit_sect(void)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize all DS3G CRITICAL SECTION
    -------------------------------------------------------------------------*/
  DS_INIT_CRIT_SECTION(&ds3g_timer_crit_sect);
}
/*===========================================================================
FUNCTION      DS3G_CLIENT_REG

DESCRIPTION   This function registers ds3gmgr as client with external modules
              like CM during task init

               This function must be called once at data services task startup.
               Since this function registers with external modules that might
               start interacting with DS immediately this function should be
               called at the very end of the task init

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_client_reg( void )
{
#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
  ds707_data_session_acl_info_type  *ds707_acl_info = NULL;
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Register as a Call Manager client, and register for call events.
    Note: For now only moving CM client reg part here
  -------------------------------------------------------------------------*/
  ds3gi_cm_init();
  ds3g_cm_client_initialized = TRUE;

  #ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
  /*-------------------------------------------------------------------------
    Do mandatory PDN check after CM registration
  -------------------------------------------------------------------------*/
  ds707_acl_info = ds707_data_session_get_acl_info_ptr();
  if ( (ds707_acl_info->data_session_profile_init == TRUE) &&
       (ds707_acl_info->ds3g_mmgsdi_init == TRUE) )
  {
    /*---------------------------------------------------------------------
      Check if mandatory APNs are enabled in profile and uicc card.
    ---------------------------------------------------------------------*/
    ds707_data_session_check_mandatory_apns();
  }
  #endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/
  return;
} /* ds3g_client_reg() */

/*===========================================================================
FUNCTION      DS3G_GET_CM_CLIENT_INIT_STATUS

DESCRIPTION   This fucntion checks if CM client registration has finished.

DEPENDENCIES  None

RETURN VALUE  TRUE: CM client registration done;
              FALSE: otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_cm_client_init_status(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_cm_client_initialized;
} /* ds3g_get_cm_client_init_status() */

/*===========================================================================
FUNCTION      DS3G_DSAT_INIT_SERVICE_MODE

DESCRIPTION   This function calls into dsat_init_service_mode depending on
              SV or NON-SV mode of operation.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dsat_init_service_mode
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_dsd_ext_radio_info_type  cdma_sys_info = {0};
  ds_dsd_ext_radio_info_type  hdr_sys_info = {0};
  ds_dsd_ext_radio_info_type  lte_sys_info = {0};
  sys_sys_mode_e_type         sys_mode1 = SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type         sys_mode2 = SYS_SYS_MODE_NO_SRV;
  ds_dsd_ext_radio_info_type  pref_sys_info = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "ds3g_dsat_init_service_mode");

  /* Get the default sys mode from NV */
  sys_mode1 = ds3gsubsmgr_mode_pref_to_sys_mode(
                            ds3gsubsmgr_get_mode_pref_for_subs_id(subs_id));

  /* Use DSD info if available */
  if(ds3gdsdif_get_pref_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                                   &pref_sys_info) == TRUE)
  {

    if(pref_sys_info.system_mode == SYS_SYS_MODE_HDR)
    {
      pref_sys_info.system_mode = SYS_SYS_MODE_CDMA;
    }

    ds3gdsdif_get_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                             SYS_SYS_MODE_CDMA,
                             &cdma_sys_info);

    ds3gdsdif_get_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                             SYS_SYS_MODE_HDR,
                             &hdr_sys_info);

    ds3gdsdif_get_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                             SYS_SYS_MODE_LTE,
                             &lte_sys_info);

    if( (cdma_sys_info.srv_status == SYS_SRV_STATUS_SRV) &&
        (hdr_sys_info.srv_status == SYS_SRV_STATUS_SRV) )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "ds3g_dsat_init_service_mode: HDR+CDMA");
      sys_mode1 = SYS_SYS_MODE_CDMA;
      sys_mode2  = SYS_SYS_MODE_CDMA;
    }
    else if( (cdma_sys_info.srv_status == SYS_SRV_STATUS_SRV) &&
             (lte_sys_info.srv_status == SYS_SRV_STATUS_SRV) )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "ds3g_dsat_init_service_mode: LTE+CDMA");
      sys_mode1 = SYS_SYS_MODE_CDMA;
      sys_mode2 = SYS_SYS_MODE_LTE;
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "ds3g_dsat_init_service_mode: %d",
                      pref_sys_info.system_mode);
      sys_mode1 = pref_sys_info.system_mode;
      sys_mode2  = SYS_SYS_MODE_NO_SRV;
    }
  }

  dsat_init_service_mode(subs_id, sys_mode1, sys_mode2);

  return;
} /* ds3g_dsat_init_service_mode() */

/*===========================================================================
FUNCTION      DS3G_DSAT_UPDATE_SERVICE_MODE

DESCRIPTION   This function updates the dsat srv mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dsat_update_service_mode
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_dsd_ext_radio_info_type  *cdma_sys_info = NULL;
  ds_dsd_ext_radio_info_type  *hdr_sys_info = NULL;
  ds_dsd_ext_radio_info_type  *lte_sys_info = NULL;
  sys_sys_mode_e_type         sys_mode1 = SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type         sys_mode2 = SYS_SYS_MODE_NO_SRV;
  ds_dsd_ext_radio_info_type  pref_sys_info = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"ds3g_dsat_update_service_mode");

  ds3gdsdif_get_pref_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                                &pref_sys_info);

  cdma_sys_info =  (ds_dsd_ext_radio_info_type *)modem_mem_alloc(
                                            sizeof(ds_dsd_ext_radio_info_type),
                                            MODEM_MEM_CLIENT_DATA_CRIT);
  hdr_sys_info  = (ds_dsd_ext_radio_info_type *)modem_mem_alloc(
                                            sizeof(ds_dsd_ext_radio_info_type),
                                            MODEM_MEM_CLIENT_DATA_CRIT);
  lte_sys_info  = (ds_dsd_ext_radio_info_type *)modem_mem_alloc(
                                            sizeof(ds_dsd_ext_radio_info_type),
                                            MODEM_MEM_CLIENT_DATA_CRIT);

  if(cdma_sys_info == NULL || hdr_sys_info == NULL || lte_sys_info == NULL)
  {
    DATA_MSG0_ERROR("memory allocation failed for ds3gi_dsat_update_service_mode");

    if (NULL != cdma_sys_info)
    {
      modem_mem_free(cdma_sys_info, MODEM_MEM_CLIENT_DATA_CRIT);
    }

     if (NULL != hdr_sys_info)
     {
        modem_mem_free(hdr_sys_info, MODEM_MEM_CLIENT_DATA_CRIT);
     }

     if (NULL != lte_sys_info)
     {
        modem_mem_free(lte_sys_info, MODEM_MEM_CLIENT_DATA_CRIT);
     }

    return;
  }

  memset((void*)cdma_sys_info,0, sizeof(ds_dsd_ext_radio_info_type));
  memset((void*)hdr_sys_info, 0, sizeof(ds_dsd_ext_radio_info_type));
  memset((void*)lte_sys_info, 0, sizeof(ds_dsd_ext_radio_info_type));

/*-------------------------------------------------------------------------
    Logic to notify ATCOP
      - If PS pref system from DSD is available, notify PS pref system
      - Else if CS domain mode is available, notify CS pref system
      - Else do not notify
  -------------------------------------------------------------------------*/
  if((pref_sys_info.srv_status == SYS_SRV_STATUS_SRV) ||
     (pref_sys_info.srv_status == SYS_SRV_STATUS_LIMITED))
  {
    if(pref_sys_info.system_mode == SYS_SYS_MODE_HDR)
    {
      pref_sys_info.system_mode = SYS_SYS_MODE_CDMA;
    }

    ds3gdsdif_get_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                             SYS_SYS_MODE_CDMA,
                             cdma_sys_info);

    ds3gdsdif_get_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                             SYS_SYS_MODE_HDR,
                             hdr_sys_info);

    ds3gdsdif_get_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                             SYS_SYS_MODE_LTE,
                             lte_sys_info);

    if( (cdma_sys_info->srv_status == SYS_SRV_STATUS_SRV) &&
        (hdr_sys_info->srv_status == SYS_SRV_STATUS_SRV) )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "ds3g_dsat_update_service_mode: HDR+CDMA");
      sys_mode1 = SYS_SYS_MODE_CDMA;
      sys_mode2  = SYS_SYS_MODE_CDMA;
    }
    else if( (cdma_sys_info->srv_status == SYS_SRV_STATUS_SRV) &&
             (lte_sys_info->srv_status == SYS_SRV_STATUS_SRV) )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "ds3g_dsat_update_service_mode: LTE+CDMA");
      sys_mode1 = SYS_SYS_MODE_CDMA;
      sys_mode2 = SYS_SYS_MODE_LTE;
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "ds3g_dsat_update_service_mode: %d",
                      pref_sys_info.system_mode);
      sys_mode1 = pref_sys_info.system_mode;
      sys_mode2  = SYS_SYS_MODE_NO_SRV;
    }
    dsat_init_service_mode(subs_id, sys_mode1, sys_mode2);
  }
  else
  {
    sys_mode1 = ds3gcsdhdlr_get_preferred_csd_mode(
                  ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id));
    if(sys_mode1 != SYS_SYS_MODE_NO_SRV)
    {
      dsat_init_service_mode(subs_id, sys_mode1, SYS_SYS_MODE_NO_SRV);
    }
  }

  modem_mem_free((void *)cdma_sys_info, MODEM_MEM_CLIENT_DATA_CRIT );
  modem_mem_free((void *)hdr_sys_info, MODEM_MEM_CLIENT_DATA_CRIT );
  modem_mem_free((void *)lte_sys_info, MODEM_MEM_CLIENT_DATA_CRIT );

  return;
} /* ds3g_dsat_update_service_mode() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_CMDS

DESCRIPTION   This function processes 3G Dsmgr commands. A command processing
              function is called based on the type of command received.

              This function should be called by the top-level task dispatcher
              whenever any 3G Dsmgr command is received.

DEPENDENCIES  3G Dsmgr should have been initialized prior to calling this
              function.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_cmds
(
  ds_cmd_type  *cmd_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd_ptr == NULL)
  {
    ASSERT(0);
    return;
  }
  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  switch( cmd_ptr->hdr.cmd_id )
  {
    /*-----------------------------------------------------------------------
      Processing of new commands
    -----------------------------------------------------------------------*/
    case DS_CMD_DSMGR_INIT_COMPLETE:
      ds3g_process_init_complete_cmd();
      break;

    case DS_CMD_STATS_INIT:
      ds3g_process_stats_init_cmd( cmd_ptr );
      break;

    case DS_CMD_MSGR_RECV:
      dsmsgr_process_ds_cmd( cmd_ptr );
      break;

    case DS_CMD_UT_SYNC:
      ds_ut_sync_cmd_hdlr();
      break;

    case DS_CMD_TIMER_EXPIRED:
      ds3gi_process_timer_expired_cmd( cmd_ptr );
      break;

    case DS_CMD_FLOW_MGR_TIMER_EXPIRED:
      ds3gflowmgr_process_timer_expired_cmd( cmd_ptr );
      break;

    case DS_CMD_MCFG_REFRESH_IND:
      ds3geventmgr_process_mcfg_refresh_ind_cmd( cmd_ptr );
      break;

    case DS_CMD_CFCM_THERMAL_RUNAWAY_IND:
      ds3gcfcmif_process_thermal_runaway_ind_cmd( cmd_ptr );
      break;

    case DS_CMD_RDM_OPEN:
    case DS_CMD_RDM_CLOSE:
    case DS_CMD_SIO_CLOSE_COMPLETE:
    case DS_CMD_SIO_DTR_CHANGED:
    case DS_CMD_SIO_CD_CHANGED:
    case DS_CMD_SIO_DSM_MEM_CTRL:
    case DS_CMD_ENTER_ONLINE_DATA_TX_FLUSH:
    case DS_CMD_COMPLETE_ONLINE_DATA_SWITCH:
    case DS_CMD_ENTER_ONLINE_DATA_NO_TX_FLUSH:
      ds3g_siolib_process_cmds( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_ORIG:
#ifdef FEATURE_DATA_IS707
      if(ds707_data_sess_is_ds_centric_emerg_only_mode())
      {
        ds707_data_sess_handle_cmd_in_emerg_only(cmd_ptr);
      }
#endif
      break;

    case DS_CMD_CM_CALL_EV_END:
#ifdef FEATURE_DATA_IS707
      if(ds707_data_sess_is_ds_centric_emerg_only_mode())
      {
        ds707_data_sess_handle_cmd_in_emerg_only(cmd_ptr);
      }
#endif
      ds3gi_process_cm_call_end_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_INCOM:
      ds3gi_process_cm_call_incom_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_CONNECT:
#ifdef FEATURE_DATA_IS707
      if(ds707_data_sess_is_ds_centric_emerg_only_mode())
      {
        ds707_data_sess_handle_cmd_in_emerg_only(cmd_ptr);
      }
#endif
      ds3gi_process_cm_call_connected_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_SETUP_IND:
      ds3gi_process_cm_call_setup_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_PROGRESS_INFO_IND:
      ds3gi_process_cm_call_progress_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_CALL_CONF:
      ds3gi_process_cm_call_conf_cmd( cmd_ptr );
      break;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
    #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

    case DS_CMD_CM_CALL_EV_PDP_NW_MODIFY_REQ:
      ds3gi_process_cm_call_nw_modify_req_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_PS_SIG_REL_CNF:
      ds3gi_process_cm_ps_sig_rel_cnf_cmd( cmd_ptr );
      break;

#ifdef FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT
    case DS_CMD_CM_CALL_EV_INCOMING_PAGE:
      ds3gi_process_cm_call_incoming_page_cmd( cmd_ptr );
      break;
#endif /* FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT */

#ifdef FEATURE_DATA_LTE
    case DS_CMD_CM_CALL_EV_GET_PDN_CONN_IND:
#ifdef FEATURE_EHRPD
      /*----------------------------------------------------------------------
       For NON-SV devices if UE maintains 1X data session and attempts to
       move to LTE, UE needs to send 1X Power Down Dereg cmd to network to
       indicate that it is locally cleaning up the 1X Data session. To perform
       the 1X dereg on Non-SV devices, LTE stack needs to be deactivated and
       1X needs to be acquired. So DS checks here if 1X data session needs
       special cleanup. If so, it triggers MMODE to start the cleanup procedure
       and suppresses the get_pdn_conn_ind to 3GPP MH. As part of this process
       NAS will get deactivated. When the cleanup procedure is completed, MMODE
       will put the UE back to the same pref mode UE was before the cleanup
       started and if the UE acquires LTE, NAS will again send this
       get_pdn_conn_ind

       If the cleanup procedure aborts even before NAS deactivates, DS caches
       this cmd and will replay the cmd to ds task so the attach procedure
       continues
      ----------------------------------------------------------------------*/
      if(TRUE == ds707_sys_chg_is_nonepc_sp_call_cleanup_needed
                                     (DS707_SYS_CHG_LTE_ATTACH_INIT, cmd_ptr) )
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "3GPP2 NON-EPC special call cleanup needed on LTE "
                        "Attach. Ignore DS_CM_CALL_EVENT_GET_PDN_CONN_IND");
      }
      else
#endif /* FEATURE_EHRPD */
      {
        ds3gi_process_cm_call_get_pdn_conn_ind_cmd(cmd_ptr);
      }
      break;

    case DS_CMD_CM_CALL_EV_PDN_CONN_REJ_IND:
      ds3gi_process_cm_call_pdn_conn_rej_ind_cmd(cmd_ptr);
      break;

    case DS_CMD_CM_CALL_EV_PDN_CONN_FAIL_IND:
      ds3gi_process_cm_call_pdn_conn_fail_ind_cmd(cmd_ptr);
      break;

    case DS_CMD_CM_CALL_EV_ACT_BEARER_IND:
      ds3gi_process_cm_call_act_bearer_ind_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_BEARER_ALLOC_REJ_IND:
      ds3gi_process_cm_call_bearer_alloc_rej_ind_cmd(cmd_ptr);
      break;

    case DS_CMD_CM_CALL_EV_BEARER_ALLOC_FAIL_IND:
      ds3gi_process_cm_call_bearer_alloc_fail_ind_cmd(cmd_ptr);
      break;

    case DS_CMD_CM_CALL_EV_BEARER_MOD_REJ_IND:
      ds3gi_process_cm_call_bearer_mod_rej_ind_cmd(cmd_ptr);
      break;

   case DS_CMD_CM_CALL_EV_MOD_BEARER_IND:
      ds3gi_process_cm_call_bearer_mod_ind_cmd(cmd_ptr);
      break;
#endif /* FEATURE_DATA_LTE */

    case DS_CMD_CM_CALL_EV_RAB_REL_IND:
      ds3gi_process_cm_call_rab_rel_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_RAB_REESTAB_IND:
      ds3gi_process_cm_call_rab_reestab_ind_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_RAB_REESTAB_REJ:
      ds3gi_process_cm_call_rab_reestab_rej_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_RAB_REESTAB_FAIL:
      ds3gi_process_cm_call_rab_reestab_fail_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_PDP_MODIFY_IND:
      ds3gi_process_cm_context_modify_ind_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_PDP_PROMOTE_IND:
      ds3gi_process_cm_context_promote_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_PDP_MODIFY_CONF:
      ds3gi_process_cm_context_modify_cnf_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_CALL_EV_PDP_MODIFY_REJ:
      ds3gi_process_cm_context_modify_rej_cmd( cmd_ptr );
      break;
#if defined(FEATURE_DATA_WCDMA_CS) || defined(FEATURE_DATA_GCSD)
    /* UUS data indication came in */
    case DS_CMD_CM_CALL_EV_USER_DATA_IND:
      ds_ucsd_process_cmds( cmd_ptr );
      break;

    /* UUS data congestion indication came in */
    case DS_CMD_CM_CALL_EV_USER_DATA_CONG_IND:
      ds_ucsd_process_cmds( cmd_ptr );
    break;
#endif /* FEATURE_DATA_WCDMA_CS || FEATURE_DATA_GCSD */

    case DS_CMD_CM_SS_EV_SRV_CHG:
      ds3gi_process_cm_ss_srv_chg_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_SS_EV_PS_DATA_FAIL:
      ds3gi_process_cm_ss_ps_data_fail_cmd( cmd_ptr );
      break;

#ifdef FEATURE_DATA_LTE
    case DS_CMD_CM_SS_EV_CNTXT_TRANSFER:
      ds3gi_process_cm_ss_3gpp_cntxt_transfer_cmd( cmd_ptr );
      break;
#endif /* FEATURE_DATA_LTE */

    case DS_CMD_CM_PH_EV_INFO:
      ds3gi_process_cm_ph_event_cmd( cmd_ptr );
      break;

    case DS_CMD_CM_PH_EV_SUB_INFO:
      ds3gi_process_cm_standby_sub_info_cmd( cmd_ptr );
      break;

#ifdef FEATURE_EHRPD
    case DS_CMD_CM_AC_STATE:
      ds707_sys_chg_process_cmd( cmd_ptr );
      break;

#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
    case DS_CMD_CM_AC_EVENT:
      ds707_data_sess_handle_cm_ac_emerg_event( cmd_ptr );
      break;
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/
#endif /* FEATURE_EHRPD */

    case DS_CMD_CM_PLMN_ID_CHG:
      ds3g_plmn_change_hdlr(cmd_ptr);
      break;

    case DS_CMD_CM_PDN_CONNECTIVITY_REQ_ERR:
      ds3g_process_pdn_conn_req_err_cmd(cmd_ptr);
      break;

#ifdef FEATURE_MMGSDI_SESSION_LIB
    case DS_CMD_MMGSDI_SIM_INFO:
      ds3g_mmgsdi_process_cmd( cmd_ptr );
      break;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

    case DS_CMD_DSD_TIMER_EXPIRED:
    case DS_CMD_DSD_EVENT_NOTIFY:
    case DS_CMD_DSD_RECOMPUTE:
    case DS_CMD_DSD_MODE_PREF_CHG:
    case DS_CMD_DSD_RESET:
    case DS_CMD_DSD_EXT_NOTIFY:
    case DS_CMD_DSD_LTE_EMERG_CALL_EVENT:
      ds_dsd_cmd_process( cmd_ptr );
      break;

    case DS_CMD_DSD_APM_APN_CHG:
    case DS_CMD_DSD_APM_REFRESH_ATTACH_PDN_LIST:
    case DS_CMD_DSD_APM_SET_APN_SWITCH_STATUS:
    case DS_CMD_DSD_APM_APN_DISABLE:
      ds_dsd_apm_cmd_process( cmd_ptr );
      break;

#ifdef FEATURE_DATA_WLAN_MAPCON
    case DS_CMD_DSD_DSC_LTE_ENABLE:
      ds_dsd_dsc_command_process( cmd_ptr );
      break;
#endif /*FEATURE_DATA_WLAN_MAPCON*/

    case DS_CMD_DS3GDSDIF_PREF_SYS_CHANGED:
      ds3gdsdif_process_dsd_pref_sys_chg_cmd( cmd_ptr );
      break;

    case DS_CMD_DS3GDSDIF_SS_EVENT_PROC_DONE:
      ds3gdsdif_process_dsd_ss_event_proc_done_cmd( cmd_ptr );
      break;

#ifdef FEATURE_DATA_WLAN_MAPCON
    case DS_CMD_DS3GDSDIF_WLAN_SYS_CHG:
      ds3gdsdif_process_wlan_svc_chg_cmd( cmd_ptr );
      break;
#endif /*FEATURE_DATA_WLAN_MAPCON*/

    case DS_CMD_DS3GDSDIF_VOICE_CALL_EVENT:
      ds3gdsdif_process_voice_call_event_cmd( cmd_ptr );
      break;

    case DS_CMD_DS3GDSDIF_MT_CSFB_HYST_TIMER_EXP:
      ds3gdsdif_process_mt_csfb_hyst_timer_exp_cmd( cmd_ptr );
      break;

    case DS_CMD_PROFILE_DB_EXT:
    case DS_CMD_PROFILE_DB_NV_REFRESH_EXT:
    case DS_CMD_PROFILE_DB_DEVICE_MODE_EXT:
      ds_profile_db_cmd_hdlr(cmd_ptr);
      break;

    case DS_CMD_PROCESS_DEVICE_SETTINGS:
    {
      ds3gdevmgr_process_cmds(cmd_ptr);
      break;
    }
    case DS_CMD_DSMGR_LOW_LATENCY_CMD:
    {
      ds3g_process_low_latency_cmd(cmd_ptr);
      break;
    }
#ifdef FEATURE_DATA_PS_IPV6
    case DS_CMD_DSSNET6_PHY_LINK_UP_EV:
    case DS_CMD_DSSNET6_PHY_LINK_GONE_EV:
    case DS_CMD_DSSNET6_PHY_LINK_DOWN_EV:
    case DS_CMD_DSSNET6_LINK_UP_EV:
    case DS_CMD_DSSNET6_LINK_DOWN_EV:
    case DS_CMD_DSSNET6_LINK_RESYNC_EV:
    case DS_CMD_DSSNET6_NET_UP_EV:
    case DS_CMD_DSSNET6_NET_DOWN_EV:
    case DS_CMD_DSSNET6_NET_ABORTED_EV:
    case DS_CMD_DSSNET6_PPP_CLOSE_EV:
    case DS_CMD_DSSNET6_FLOW_DELETED_EV:
      dssnet6_sm_process_event(cmd_ptr);
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "3G Dsmgr rx'ed unrecognized cmd: %d",
                      cmd_ptr->hdr.cmd_id);
      break;

  } /* switch( cmd_id ) */
  return;
} /* ds3g_process_cmds() */

/*===========================================================================
FUNCTION      DS3G_REGISTER_MODE_SP_HANDLER

DESCRIPTION   This function registers a mode-specific handler function table,
              for the specified mode and call type.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  If a mode-specific handler function table was previously
              registered for the specified mode and call type, it will be
              over-written.
===========================================================================*/
void ds3g_register_mode_sp_handler
(
  sys_sys_mode_e_type       mode,        /* Network mode             */
  ds_call_type_e_type       call_type,   /* Call type-Circuit or pkt */
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr /* Handler function table   */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( mode == SYS_SYS_MODE_NONE ) ||
      ( mode >= SYS_SYS_MODE_MAX )  ||
      (call_type >= DS_NUM_CALL_TYPES ))
  {
    DATA_3GMGR_MSG2(
       MSG_LEGACY_LOW,
       "Couldn't register mode specific handler mode:%d call_type:%d",
       mode, call_type);
    ASSERT(0);
    return;
  }

  ds3gi_hdlr_tbl[mode][call_type] = hdlr_tbl_ptr;

  return;
} /* ds3g_register_mode_sp_handler() */


/*===========================================================================
FUNCTION      DS3G_SEND_DOWNLINK_THROUGHPUT_ESTIMATION_IND

DESCRIPTION   This function invokes the mode handlers to send 
              the estimated downlink throughput rate for all active calls for the
              current sampling interval

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_send_downlink_throughput_estimation_ind
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
)
{
  sys_sys_mode_e_type       current_mode      = SYS_SYS_MODE_NONE;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr      = NULL;
  sys_modem_as_id_e_type    cm_subs_id        = SYS_MODEM_AS_ID_NONE;    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Determine the current network mode for the sub ID
    -------------------------------------------------------------------------*/
  cm_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id);
  current_mode = ds3g_get_current_network_mode_ex(cm_subs_id);

  /*-------------------------------------------------------------------------
    Return if there is no service
    -------------------------------------------------------------------------*/
  if (current_mode == SYS_SYS_MODE_NO_SRV)
  {
    return;
  }

  if( ( current_mode == SYS_SYS_MODE_NONE ) ||
      ( current_mode >= SYS_SYS_MODE_MAX ))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid current mode %d ", current_mode);
    return;
  }

  /*-------------------------------------------------------------------------
    Check if there is a handler registered for the current mode and invoke
    the handler
    -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[current_mode][DS_CALL_TYPE_PKT];

  if ((hdlr_tbl_ptr != NULL) &&
      (hdlr_tbl_ptr->send_downlink_throughput_estimation_indicaiton != NULL))
  {
    hdlr_tbl_ptr->send_downlink_throughput_estimation_indicaiton(cm_subs_id);
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,"No hdlr regd for mode %d to downlink tput "
                                     " subs id ",current_mode,ds3g_subs_id);
  }
  return;
} /* ds3g_send_downlink_throughput_estimation_ind() */


/*===========================================================================
FUNCTION      DS3G_REFRESH_ACTUAL_THROUGHPUT

DESCRIPTION   This function invokes the mode handlers to refresh/update 
              the actual throughput rate for all active calls for the
              current sampling interval

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_refresh_actual_throughput
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint16                      timer_expiry_count
)
{
  sys_sys_mode_e_type       current_mode      = SYS_SYS_MODE_NONE;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr      = NULL;
#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)
  sys_modem_as_id_e_type    cm_subs_id        = SYS_MODEM_AS_ID_NONE;    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }
  /*------------------------------------------------------------------------- 
    Determine the current network mode for the sub ID
    -------------------------------------------------------------------------*/
  cm_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id);
  current_mode = ds3g_get_current_network_mode_ex(cm_subs_id);
#else
  current_mode = ds3g_get_current_network_mode(); 
#endif

  /*------------------------------------------------------------------------- 
    Return if there is no service
    -------------------------------------------------------------------------*/
  if (current_mode == SYS_SYS_MODE_NO_SRV)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "act t'put: No service on ds3g ID %d",
                                      ds3g_subs_id);
    return;
  }

  if( ( current_mode == SYS_SYS_MODE_NONE ) ||
      ( current_mode >= SYS_SYS_MODE_MAX ))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid current mode %d ", current_mode);
    return;
  }

  /*------------------------------------------------------------------------- 
    Check if there is a handler registered for the current mode and invoke
    the handler
    -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[current_mode][DS_CALL_TYPE_PKT];

  if ((hdlr_tbl_ptr != NULL) &&
      (hdlr_tbl_ptr->refresh_actual_throughput_hdlr != NULL))
  {
    hdlr_tbl_ptr->refresh_actual_throughput_hdlr(ds3g_subs_id, timer_expiry_count);
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,"No hdlr regd for mode %d to refresh act "
                                     "t'put subs id ",current_mode,ds3g_subs_id);
  }
  return;
} /* ds3g_refresh_actual_throughput() */

/*===========================================================================
FUNCTION      DS3G_PREPARE_DOWNLINK_THROUGHPUT_ESTIMATION

DESCRIPTION   This function invokes the mode handlers to starting lower layer
              for estimated downlink throughput rate for all active calls for the
              current sampling interval

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_prepare_downlink_throughput_estimation
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
)
{
  sys_sys_mode_e_type       current_mode      = SYS_SYS_MODE_NONE;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr      = NULL;
  uint32 dl_estm_throughput_info_ind_freq     = 0;
  sys_modem_as_id_e_type    cm_subs_id        = SYS_MODEM_AS_ID_NONE;    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Determine the current network mode for the sub ID
    -------------------------------------------------------------------------*/
  cm_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id);
  current_mode = ds3g_get_current_network_mode_ex(cm_subs_id);

  /*------------------------------------------------------------------------- 
    Return if there is no service
    -------------------------------------------------------------------------*/
  if (current_mode == SYS_SYS_MODE_NO_SRV)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "act t'put: No service on ds3g ID %d",
                                      ds3g_subs_id);
    return;
  }

  if( ( current_mode == SYS_SYS_MODE_NONE ) ||
      ( current_mode >= SYS_SYS_MODE_MAX ))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid current mode %d ", current_mode);
    return;
  }

  dl_estm_throughput_info_ind_freq = 
      ds3gi_get_estimated_dl_throughput_interval(ds3g_subs_id);
  /*------------------------------------------------------------------------- 
    Check if there is a handler registered for the current mode and invoke
    the handler
  -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[current_mode][DS_CALL_TYPE_PKT];

  if ((hdlr_tbl_ptr != NULL) &&
      (hdlr_tbl_ptr->prepare_downlink_estimated_throughput_report != NULL))
  {
    hdlr_tbl_ptr->prepare_downlink_estimated_throughput_report(cm_subs_id, dl_estm_throughput_info_ind_freq);
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,"No hdlr regd for mode %d to refresh act "
                                     "t'put subs id ",current_mode,ds3g_subs_id);
  }
  return;
} /* ds3g_prepare_downlink_throughput_estimation() */

/*===========================================================================
FUNCTION      DS3G_GET_DL_TPUT_T_ACCUMUATE_VALUE

DESCRIPTION   This function invokes the mode handlers to to provide t_accumulate 
              value from the mode handler

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_get_dl_tput_t_accumuate_value
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint32*                      t_accumuate
)
{
  sys_sys_mode_e_type       current_mode = SYS_SYS_MODE_NONE;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;

  sys_modem_as_id_e_type    cm_subs_id        = SYS_MODEM_AS_ID_NONE;    
  //Check for null on t_accumulate
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Determine the current network mode for the sub ID
    -------------------------------------------------------------------------*/
  *t_accumuate = 0;
  cm_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id);
  current_mode = ds3g_get_current_network_mode_ex(cm_subs_id);

  /*------------------------------------------------------------------------- 
    Return if there is no service
    -------------------------------------------------------------------------*/
  if (current_mode == SYS_SYS_MODE_NO_SRV)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "act t'put: No service on ds3g ID %d",
                                      ds3g_subs_id);
    return;
  }

  if( ( current_mode == SYS_SYS_MODE_NONE ) ||
      ( current_mode >= SYS_SYS_MODE_MAX ))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid current mode %d ", current_mode);
    return;
  }

  /*------------------------------------------------------------------------- 
    Check if there is a handler registered for the current mode and invoke
    the handler
    -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[current_mode][DS_CALL_TYPE_PKT];

  if ((hdlr_tbl_ptr != NULL) &&
      (hdlr_tbl_ptr->get_downlink_throughput_t_accumulate != NULL))
  {
    hdlr_tbl_ptr->get_downlink_throughput_t_accumulate(cm_subs_id, t_accumuate);
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,"No hdlr regd for mode %d to refresh act "
                                     "t'put subs id ",current_mode,ds3g_subs_id);
  }
  return;
} /* ds3g_get_dl_tput_t_accumuate_value() */

/*===========================================================================
FUNCTION      DS3G_GET_TARGET_RAT_EX

DESCRIPTION   This function returns the target rat on passed subs
              (used during eHRPD - LTE reselection).

DEPENDENCIES  None

RETURN VALUE  NONE if in steady state
              LTE if in transient state during reselection

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_target_rat_ex
(
  sys_modem_as_id_e_type  subs_id
)
{
  sys_sys_mode_e_type  trat = SYS_SYS_MODE_NO_SRV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
     Subs id validity checking.
  ---------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return SYS_SYS_MODE_NO_SRV;
  }

  if(TRUE == ds3gdsdif_is_lte_attach_in_progress(subs_id))
  {
    trat = SYS_SYS_MODE_LTE;
  }
  return trat;
} /* ds3g_get_target_rat_ex() */

/*===========================================================================
FUNCTION      DS3G_GET_TARGET_RAT

DESCRIPTION   This function returns the target rat
              (used during eHRPD - LTE reselection)

DEPENDENCIES  None

RETURN VALUE  NONE if in steady state
              LTE if in transient state during reselection

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_target_rat(void)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_get_target_rat_ex(subs_id);
} /* ds3g_get_target_rat() */

/*===========================================================================
FUNCTION      DS3G_SET_TARGET_RAT_EX

DESCRIPTION   This function sets the target rat on passed sub
              (used during eHRPD - LTE reselection)

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_target_rat_ex
(
  sys_sys_mode_e_type                  trat,
  ds3geventmgr_attach_prof_info_type  *attach_prof_info,
  sys_modem_as_id_e_type               subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "ds3g_set_target_rat on sub:%d trat:%d",
                   subs_id, trat);

  /*---------------------------------------------------------------------
     Subs id validity checking.
  ---------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Notify DSD that ATTACH is going on in progress
  -------------------------------------------------------------------------*/
  ds3gdsdif_set_lte_attach_in_progress(subs_id,
                                       (trat == SYS_SYS_MODE_LTE),
                                       attach_prof_info);
  return;
} /* ds3g_set_target_rat_ex() */

/*===========================================================================
FUNCTION      DS3G_REPORT_THROUGHPUT_INFO

DESCRIPTION   This function invokes the mode handlers to to report
              'Throughput Info Indication' to PS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_report_throughput_info
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
)
{
  sys_sys_mode_e_type       current_mode = SYS_SYS_MODE_NONE;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_p = 0;
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Determine the current mode for the incoming subs ID
    -------------------------------------------------------------------------*/
  subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id);
  current_mode = ds3g_get_current_network_mode_ex(subs_id);

  /*-------------------------------------------------------------------------
    Return if there is no service
    -------------------------------------------------------------------------*/
  if (current_mode == SYS_SYS_MODE_NO_SRV)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "No service on ds3g Sub ID %d",ds3g_subs_id);
    return;
  }

  if( ( current_mode == SYS_SYS_MODE_NONE ) ||
      ( current_mode >= SYS_SYS_MODE_MAX ))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid current mode %d ", current_mode);
    return;
  }

  /*-------------------------------------------------------------------------
    Fill throughput Info list
    -------------------------------------------------------------------------*/
  hdlr_tbl_p = ds3gi_hdlr_tbl[current_mode][DS_CALL_TYPE_PKT];

  if ((hdlr_tbl_p!=NULL)&&
      (hdlr_tbl_p->report_throughput_info_hdlr != NULL))
  {
    hdlr_tbl_p->report_throughput_info_hdlr(ds3g_subs_id);
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "No hdlr regd to report t'put Ind for mode:%d subs id:%d",
                    current_mode, ds3g_subs_id);
  }
  return;
}/* ds3g_report_throughput_info() */

/*===========================================================================
FUNCTION      DS3G_IS_SYS_AVAILABLE_EX

DESCRIPTION   This function returns boolean depending on whether the system is
              available or not on the passed subs.

DEPENDENCIES  None

RETURN VALUE  boolean

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_sys_available_ex
(
  sys_sys_mode_e_type     sys,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_dsd_ext_radio_info_type  sys_info = {0};
  boolean                     ret_val = FALSE;
  sys_sys_mode_e_type         resel_target_rat = SYS_SYS_MODE_NO_SRV;
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
     Subs id validity checking.
  ---------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  ret_val = ds3gdsdif_get_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                                     sys,
                                     &sys_info);

  DATA_3GMGR_MSG3(MSG_LEGACY_LOW,
                  "ds3g_is_sys_available: sys %d, srv status %d, dsd ret_val %d",
                  sys, sys_info.srv_status, ret_val);

  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  resel_target_rat = ds3gdsdif_get_resel_target_rat(ds3g_subs_id);

  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "resel_target_rat %d, bsr_in_progress %d",
                  resel_target_rat, ds3g_bsr_is_in_progress(subs_id));

  /*-------------------------------------------------------------------------
    If CM has indicated that lower layer reselection process is on going,
    through the target_rat variable in SS event, then return TRUE. Else for
    anything other than no_srv serving status return TRUE.
  -------------------------------------------------------------------------*/
  if( (TRUE == ds3g_bsr_is_in_progress(subs_id)) ||
      (SYS_SYS_MODE_LTE == resel_target_rat) )
  {
    return TRUE;
  }
  else
  {
    if( (TRUE == ret_val) &&
        ((sys_info.srv_status == SYS_SRV_STATUS_LIMITED) ||
         (sys_info.srv_status == SYS_SRV_STATUS_SRV) ||
         (sys_info.srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL) ||
         (sys_info.srv_status == SYS_SRV_STATUS_PWR_SAVE))  )
    {
      return TRUE;
    }
    else
    {
      if((sys == SYS_SYS_MODE_LTE) &&
         (TRUE == ds3gdsdif_is_lte_attach_in_progress(subs_id)))
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
  }
} /* ds3g_is_sys_available_ex() */

/*===========================================================================
FUNCTION      DS3G_IS_SYS_AVAILABLE

DESCRIPTION   This function returns boolean depending on whether the system is
              available or not

DEPENDENCIES  None

RETURN VALUE  boolean

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_sys_available
(
  sys_sys_mode_e_type sys
)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_is_sys_available_ex(sys,subs_id);
} /* ds3g_is_sys_available() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_NETWORK_MODE_EX

DESCRIPTION   This function returns the current system that we are camped on
              for a SUBS ID.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr network mode type for the subs_id mentioned.
              If invalid subs_id, return SYS_SYS_MODE_NO_SRV.

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_network_mode_ex
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_dsd_ext_radio_info_type pref_radio = {0};
  sys_sys_mode_e_type         pref_mode = SYS_SYS_MODE_NO_SRV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
     Subs id validity checking.
  ---------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return SYS_SYS_MODE_NO_SRV;
  }

  /*-------------------------------------------------------------------------
    If LTE attach in progress, always return LTE as preferred system
  -------------------------------------------------------------------------*/
  if (SYS_SYS_MODE_LTE == ds3g_get_target_rat_ex(subs_id))
  {
    pref_mode = SYS_SYS_MODE_LTE;
  }
  else
  {
    /*-----------------------------------------------------------------------
     Get pref radio info from DSDIF
    -----------------------------------------------------------------------*/
    ds3gdsdif_get_pref_radio_info( ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                                   &pref_radio);
    pref_mode = pref_radio.system_mode;

    /*-------------------------------------------------------------------------
      We are now explicitly differentialting between CDMA and HDR mode,
      so to support legacy code, which considered CDMA and HDR as same,
      we will check here and return CDMA even if mode is HDR
    -------------------------------------------------------------------------*/
    if(pref_mode == SYS_SYS_MODE_HDR)
    {
      pref_mode = SYS_SYS_MODE_CDMA;
    }
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_LOW,
                "ds3g_get_current_network_mode_ex on sub %d: mode %d "
                "ds3g_get_target_rat_ex %d: ",
                subs_id, pref_mode, ds3g_get_target_rat_ex(subs_id));

  return pref_mode;
} /* ds3g_get_current_network_mode_ex() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_NETWORK_MODE

DESCRIPTION   This function returns the current system that we are camped on.
              The current system is the one available on the preferred radio.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr network mode type

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_network_mode(void)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_get_current_network_mode_ex(subs_id);
} /* ds3g_get_current_network_mode() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_PREFERRED_MODE_EX

DESCRIPTION   This function returns the current preferred system for a SUBS ID.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr network mode type for the subs_id mentioned.
              If invalid subs_id, return SYS_SYS_MODE_NO_SRV.

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_preferred_mode_ex
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_dsd_ext_radio_info_type pref_radio = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
     Subs id validity checking.
  ---------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return SYS_SYS_MODE_NO_SRV;
  }

  ds3gdsdif_get_pref_radio_info( ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                                 &pref_radio);


  if (SYS_SYS_MODE_LTE == ds3g_get_target_rat_ex(subs_id))
  {
    pref_radio.system_mode = SYS_SYS_MODE_LTE;
  }

  return (pref_radio.system_mode);
}/*ds3g_get_current_preferred_mode_ex() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_PREFERRED_MODE

DESCRIPTION   This function returns the current preferred system.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr network mode type

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_preferred_mode(void)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_get_current_preferred_mode_ex(subs_id);
} /* ds3g_get_current_preferred_mode() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_SRV_STATUS_EX

DESCRIPTION   This function returns the current service status of passed sub
              of the system that is indicated in the DS_CM_SS_SRV_CHG_CMD in
              the srv_status field.

DEPENDENCIES  None

RETURN VALUE  Returns uint32 which has current_srv_status maintained in ds3g

SIDE EFFECTS  None
===========================================================================*/
sys_srv_status_e_type ds3g_get_current_srv_status_ex
(
  sys_modem_as_id_e_type  asubs_id
)
{
  ds_dsd_ext_radio_info_type  pref_radio = {0};
  sys_srv_status_e_type       srv_status = SYS_SRV_STATUS_NO_SRV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
     Subs id validity checking.
  ---------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(asubs_id))
  {
    return SYS_SRV_STATUS_NO_SRV;
  }

  ds3gdsdif_get_pref_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id),
                                &pref_radio);

  if (pref_radio.srv_status != SYS_SRV_STATUS_NO_SRV)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_MED, "ds3g_get_current_srv_status: %d ",
                    pref_radio.srv_status);

    srv_status = pref_radio.srv_status;
  }
  else
  {
    //Get srv status from ds3g cache
    srv_status
      = dsd3gif_get_srv_status_from_ds3g_radio_cache(asubs_id, &pref_radio);
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, 
                  "ds3g_get_current_srv_status: %d ", srv_status);

  return srv_status;
}/* ds3g_get_current_srv_status_ex() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_SRV_STATUS

DESCRIPTION   This function returns the current service status of the system
              that is indicated in the DS_CM_SS_SRV_CHG_CMD in the srv_status
              field.

DEPENDENCIES  None

RETURN VALUE  Returns uint32 which has current_srv_status maintained in ds3g

SIDE EFFECTS  None
===========================================================================*/
sys_srv_status_e_type ds3g_get_current_srv_status(void)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_get_current_srv_status_ex(subs_id);
}/* ds3g_get_current_srv_status() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_CS_DOMAIN_MODE_EX

DESCRIPTION   This function returns the current CS domain mode this
              subscription has camped on.

DEPENDENCIES  None

RETURN VALUE  Returns the CS Domain mode

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_cs_domain_mode_ex
(
  sys_modem_as_id_e_type  subs_id
)
{
  sys_sys_mode_e_type  mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------
     Subs id validity checking.
  ---------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return SYS_SYS_MODE_NO_SRV;
  }

  mode = ds3gcsdhdlr_get_preferred_csd_mode(
           ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id));

  return mode;
}/* ds3g_get_current_cs_domain_mode_ex() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_CS_DOMAIN_MODE

DESCRIPTION   This utility function return radio which is CS domain registered.

DEPENDENCIES  None

RETURN VALUE  Returns mode which is CS domain registered

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_cs_domain_mode(void)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_voice_subs_id();
  return ds3g_get_current_cs_domain_mode_ex(subs_id);
}/* ds3g_get_current_cs_domain_mode() */

/*===========================================================================
FUNCTION      DS3G_GET_CALL_STATE

DESCRIPTION   This function returns the current call state.

DEPENDENCIES  None

RETURN VALUE  See above.

SIDE EFFECTS  None
===========================================================================*/
ds3gi_call_state_e_type ds3g_get_call_state
(
  cm_call_id_type  cm_call_id
)
{
  uint32 call_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_inst = ds3gi_find_call_instance(cm_call_id);

  if (call_inst < DS3GI_MAX_NUM_CALLS)
  {
    return(ds3gi_call_info[call_inst].call_state);
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid call id for call state");
    return (DS3G_INVALID_CALL_STATE);
  }
} /* ds3g_get_call_state() */

/*===========================================================================
FUNCTION      DS3G_GET_CALL_WAS_CONNECTED

DESCRIPTION   This function returns the was connected flag for the given call
              identifier.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr call_was_connected flag

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_call_was_connected
(
  cm_call_id_type    cm_call_id
)
{
  uint32 call_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_inst = ds3gi_find_call_instance(cm_call_id);

  if (call_inst < DS3GI_MAX_NUM_CALLS)
  {
    return(ds3gi_call_info[call_inst].call_was_connected);
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"Invalid call id for call state");
    return (DS3G_FAILURE);
  }
} /* ds3g_get_call_was_connected() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_SESSION_DATA_COUNTERS

DESCRIPTION   This function should be invoked by UI for displaying data call
              statistics. It returns by reference the total count of bytes
              transmitted and received at RLC for the current call:

              *rx_from_network_byte_cnt - Total bytes rxed for the call
              *tx_to_network_byte_cnt   - Total bytes txed for the call

              The total byte count is accumulative for the call. If RAB is
              re-established after PDP context is preserved during the call,
              the total byte count includes bytes transmitted and received
              after RAB is re-established.

DEPENDENCIES  None

RETURN VALUE  This function returns the data statistics type, indicating if
              the information passed by reference, e.g., *rx_from_network_byte_cnt,
              is valid.

SIDE EFFECTS  None
===========================================================================*/
ds_stats_e_type ds3g_get_current_session_data_counters
(
  cm_call_id_type  call_id,                   /* Call ID */
  uint32          *rx_from_network_byte_cnt,  /* RX byte count for the call */
  uint32          *tx_to_network_byte_cnt     /* TX byte count for the call */
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Get current session data counters");

  /*-------------------------------------------------------------------------
    Check if the call ID passed in matches the call ID stored for the current
    call.
  -------------------------------------------------------------------------*/
  call_inst = ds3gi_find_call_instance(call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    /*-----------------------------------------------------------------------
      Call ID is invalid.
    -----------------------------------------------------------------------*/
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid call id: %d", call_id);

    return ( DS_STATS_INVALID_CALL_ID );
  }

  /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
  ----------------------------------------------------------------------*/
  hdlr_tbl_ptr =
       ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

  if (hdlr_tbl_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
    return (DS_STATS_UNAVAILABLE);
  }

  /*-------------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    get_data_counters_handler.
  -------------------------------------------------------------------------*/
  if( ( hdlr_tbl_ptr->get_data_counters_handler != NULL ) )
  {
    if ( hdlr_tbl_ptr->get_data_counters_handler(
                                   call_id,
                                   rx_from_network_byte_cnt,
                                   tx_to_network_byte_cnt ) == DS3G_SUCCESS )
    {
      /*---------------------------------------------------------------------
        Data count is valid.
      ---------------------------------------------------------------------*/
      return ( DS_STATS_VALID );
    }
    else
    {
      /*---------------------------------------------------------------------
        Data count is not available.
      ---------------------------------------------------------------------*/
      return ( DS_STATS_UNAVAILABLE );
    }
  }
  else
  {
    return ( DS_STATS_UNAVAILABLE );
  }
} /* ds3g_get_current_session_data_counters() */

/*===========================================================================
FUNCTION      DS3G_SET_CURRENT_OPRT_MODE_SRV_DOMAIN_EX

DESCRIPTION   This function sets the new oprt_mode_srv_domain on passed sub

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_current_oprt_mode_srv_domain_ex
(
  cm_srv_domain_pref_e_type  new_srv_domain,
  sys_modem_as_id_e_type     subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*---------------------------------------------------------------------
     Subs id validity checking.
  ---------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }

  ds3g_oprt_mode_srv_domain[ds3g_subs_id] = new_srv_domain;
  return;
}/* ds3g_set_current_oprt_mode_srv_domain_ex() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_OPRT_MODE_SRV_DOMAIN_EX

DESCRIPTION   This function returns the current oprt_mode_srv_domain on
              passed sub.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
cm_srv_domain_pref_e_type ds3g_get_current_oprt_mode_srv_domain_ex
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*---------------------------------------------------------------------
     Subs id validity checking.
  ---------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return CM_SRV_DOMAIN_PREF_NONE;
  }

  return ds3g_oprt_mode_srv_domain[ds3g_subs_id];
}/* ds3g_get_current_oprt_mode_srv_domain_ex() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_REGD_SRV_DOMAIN_EX

DESCRIPTION   This function returns the current regd_srv_domain on pass sub

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
sys_srv_domain_e_type ds3g_get_current_regd_srv_domain_ex
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id;
  ds_dsd_ext_radio_info_type  pref_radio = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*---------------------------------------------------------------------
     Subs id validity checking.
  ---------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return SYS_SRV_DOMAIN_NO_SRV;
  }

  ds3gdsdif_get_pref_radio_info(ds3g_subs_id, &pref_radio);

  return pref_radio.srv_domain;
} /* ds3g_get_current_regd_srv_domain_ex() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_REGD_SRV_DOMAIN

DESCRIPTION   This function returns the current regd_srv_domain

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
sys_srv_domain_e_type ds3g_get_current_regd_srv_domain(void)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  return ds3g_get_current_regd_srv_domain_ex(subs_id);
} /* ds3g_get_current_regd_srv_domain() */

/*===========================================================================
FUNCTION      DS3G_SET_CS_SUBS_ID

DESCRIPTION   This function sets the subs id in DS3GMGR for call origination.
              (Currently used only for CSD call origination)

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_cs_subs_id
(
  sys_modem_as_id_e_type  as_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "setting CS subs_id to %d", as_id);
  ds3g_cs_subs_id = as_id;
}/* ds3g_set_cs_subs_id() */

/*===========================================================================
FUNCTION      DS3G_GET_CS_SUBS_ID

DESCRIPTION   This function get the subs id in DS3GMGR for call origination.
              (Currently used only for CSD call origination)

DEPENDENCIES  None

RETURN VALUE  Returns the current SUBS_ID set by CSD module

SIDE EFFECTS  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_get_cs_subs_id(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Getting CS subs_id: %d", ds3g_cs_subs_id);
  return ds3g_cs_subs_id;
} /* ds3g_get_cs_subs_id() */

/*===========================================================================
FUNCTION      DS3G_RESET_CS_SUBS_ID

DESCRIPTION   This function clears the subs id in DS3GMGR for call origination.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  This clears the cs subs id set by ds3g_set_cs_subs_id()
===========================================================================*/
void ds3g_reset_cs_subs_id(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_cs_subs_id = SYS_MODEM_AS_ID_NONE;
} /* ds3g_reset_cs_subs_id() */

/*===========================================================================
FUNCTION      DS3G_GET_PS_SUBS_ID

DESCRIPTION   This function gets the subs id for PS data call.

DEPENDENCIES  None

RETURN VALUE  Returns the subs_id on which PS data call can be made.

SIDE EFFECTS  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_get_ps_subs_id(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  //NOTE ruishi change name to ds3g_get_default_data_subs_id
  return ds3gsubsmgr_get_def_data_subs_id();
} /* ds3g_get_ps_subs_id() */

/*===========================================================================
FUNCTION      DS3G_SET_VOICE_SUBS_ID

DESCRIPTION   This function set the subs id in DS3GMGR for Voice subscription.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_voice_subs_id
(
  sys_modem_as_id_e_type  as_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3gsubsmgr_set_def_voice_subs_id(as_id);
  return;
}/* ds3g_set_voice_subs_id() */

/*===========================================================================
FUNCTION      DS3G_GET_VOICE_SUBS_ID

DESCRIPTION   This function gets the default voice subs id for VT data call.

DEPENDENCIES  None

RETURN VALUE  Returns the subs_id which is selected as default voice subs.

SIDE EFFECTS  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_get_voice_subs_id(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3gsubsmgr_get_def_voice_subs_id();
} /* ds3g_get_voice_subs_id() */

/*===========================================================================
FUNCTION      DS3G_GET_CM_CLIENT_ID

DESCRIPTION   Returns the value of client ID assigned by CM.

DEPENDENCIES  None

RETURN VALUE  cm_client_id_type - Client ID assigned by CM

SIDE EFFECTS  None
===========================================================================*/
cm_client_id_type ds3g_get_cm_client_id(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_cm_client_id;
} /* ds3g_get_cm_client_id() */

/*===========================================================================
FUNCTION      DS3G_SET_CM_CLIENT_ID

DESCRIPTION   Sets client ID assigned by CM in DS3G maintained cache.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_cm_client_id
(
  cm_client_id_type  cm_client_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_cm_client_id = cm_client_id;
  return;
} /* ds3g_set_cm_client_id() */

/*===========================================================================
FUNCTION      DS3G_SET_LAST_CALL_MODE

DESCRIPTION   Sets call type for the previous call.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_last_call_mode
(
  sys_sys_mode_e_type last_call_mode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_last_call_mode = last_call_mode;
} /* ds3g_set_last_call_mode() */

/*===========================================================================
FUNCTION      DS3G_SET_LAST_CALL_TYPE

DESCRIPTION   Sets operating mode for the previous call.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_last_call_type
(
  ds_call_type_e_type  last_call_type
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_last_call_type = last_call_type;
} /* ds3g_set_last_call_type() */

/*===========================================================================
FUNCTION      DS3G_SV_MODE_OF_OPERATION

DESCRIPTION   Returns boolean - TRUE if SV mode of operation else FALSE

DEPENDENCIES  None

RETURN VALUE  TRUE: SV mode of operation
              FALSE: otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_sv_mode_of_operation(void)
{
  boolean ret_val = FALSE;
#ifdef FEATURE_DATA_SV_MODE
  uint8 sv_mask = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sv_mask = trm_get_simult_cap();
  if( (sv_mask & TRM_SVDO_IS_ENABLED) ||
      (sv_mask & TRM_SVLTE_IS_ENABLED) )
  {
    ret_val = TRUE;
  }
#endif /* FEATURE_DATA_SV_MODE */
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "SV mode of operation %d ", ret_val);
  return ret_val;
} /* ds3g_sv_mode_of_operation() */

/*===========================================================================
FUNCTION      ds3g_bsr_is_in_progress

DESCRIPTION   Returns if a Better System Reselection is in progress.

DEPENDENCIES  None

RETURN VALUE  TRUE - BSR in in progress.
              FALSE otherwise.

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_bsr_is_in_progress
(
  sys_modem_as_id_e_type  asubs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3gdsdif_is_bsr_in_progress(
     ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id));
} /* ds3g_bsr_is_in_progress() */

/*===========================================================================
FUNCTION      DS3G_CACHE_RADIO_INFO

DESCRIPTION   Updates the global radio info cache with the info provided by
              CM in the ss event. This cache is updated everytime CM generates
              SS event for CM_SS_EVENT_SRV_CHANGED

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_cache_radio_info
(
  const cm_mm_msim_ss_info_s_type  *ss_info_ptr
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  uint8                       stack_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ss_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Null ss_info_ptr");
    return;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(ss_info_ptr->asubs_id);

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
  {
    return;
  }

  for(stack_indx = 0; stack_indx < ss_info_ptr->number_of_stacks; stack_indx++)
  {
    if(ss_info_ptr->stack_info[stack_indx].is_operational == TRUE)
    {
      ds3g_cached_radio_info[subs_indx].stack_info[stack_indx].system_mode
        = ss_info_ptr->stack_info[stack_indx].sys_mode;
      ds3g_cached_radio_info[subs_indx].stack_info[stack_indx].srv_status
        = ss_info_ptr->stack_info[stack_indx].srv_status;
    }
  }
  return;
} /* ds3g_cache_radio_info() */

/*===========================================================================
FUNCTION      DS3G_IS_HDR_IN_SRVC

DESCRIPTION   Checks the global radio info cache to see if HDR is in service

DEPENDENCIES  None

RETURN VALUE  TRUE - If HDR is in service
              FALSE - Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_hdr_in_srvc(void)
{
  sys_modem_as_id_e_type      subs_id;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  uint8                       stack_indx;
  boolean                     ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
  {
    return ret_val;
  }

  for(stack_indx = 0; stack_indx < CM_NO_STACKS; stack_indx++)
  {
    if((ds3g_cached_radio_info[subs_indx].stack_info[stack_indx].system_mode
         == SYS_SYS_MODE_HDR) &&
       (ds3g_cached_radio_info[subs_indx].stack_info[stack_indx].srv_status
         == SYS_SRV_STATUS_SRV))
    {
      ret_val = TRUE;
      break;
    }
  }
  return ret_val;
} /* ds3g_is_hdr_in_srvc() */

/*===========================================================================
FUNCTION      DS3G_GET_LTE_SRV_STATUS

DESCRIPTION   Checks the global radio info cache and returns the srv status
              if LTE is present in any of the stack. Else returns no serv by
              default.

DEPENDENCIES  None

RETURN VALUE  Limited/Full Serice
              No Service (If LTE not available on any stack)

SIDE EFFECTS  None
===========================================================================*/
sys_srv_status_e_type ds3g_get_lte_srv_status(void)
{
  sys_modem_as_id_e_type      subs_id;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  uint8                       stack_indx;
  sys_srv_status_e_type       srv_status = SYS_SRV_STATUS_NO_SRV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
  {
    return srv_status;
  }

  for(stack_indx = 0; stack_indx < CM_NO_STACKS; stack_indx++)
  {
    if(ds3g_cached_radio_info[subs_indx].stack_info[stack_indx].system_mode
         == SYS_SYS_MODE_LTE)
    {
      srv_status
        = ds3g_cached_radio_info[subs_indx].stack_info[stack_indx].srv_status;
      break;
    }
  }
  return srv_status;
} /* ds3g_get_lte_srv_status() */

/*===========================================================================
FUNCTION      DS3G_RESOLVE_SUBS_ID

DESCRIPTION   This is an external API function.
              The clients call this to resolve a DS internal subs id to CM
              subs id.

DEPENDENCIES  None

RETURN VALUE  Status of the resolve and the resolved subs id

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_resolve_subs_id
(
  ds_sys_subscription_enum_type  ds_subs_id,
  sys_modem_as_id_e_type        *ret_subs_id_ptr
)
{
  boolean  ret_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "ds3g_resolve_subs_id: sub id passed by application is %d",
                  ds_subs_id);

  /*-------------------------------------------------------------------------
    The output ptr must be valid
  -------------------------------------------------------------------------*/
  if( ret_subs_id_ptr == NULL)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_get_resolved_subs_id: %d w/ invalid output holder",
                    ds_subs_id);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Rules of resolve:
      - valid input ds subs id is mapped to the corresponding cm asid
      - ds special subs id for any is mapped to the dds
      - other input is mapped to invalid subs id
  -------------------------------------------------------------------------*/
  *ret_subs_id_ptr = SYS_MODEM_AS_ID_NONE;
  switch(ds_subs_id)
  {
    case DS_SYS_PRIMARY_SUBS:
      *ret_subs_id_ptr = SYS_MODEM_AS_ID_1;
    break;

    case DS_SYS_SECONDARY_SUBS:
      *ret_subs_id_ptr = SYS_MODEM_AS_ID_2;
    break;
#if defined FEATURE_TRIPLE_SIM
    case DS_SYS_TERTIARY_SUBS:
      *ret_subs_id_ptr = SYS_MODEM_AS_ID_3;
    break;
#endif /* FEATURE_TRIPLE_SIM */

    default:
      ret_val = FALSE;
    break;
  }

  return ret_val;
} /* ds3g_resolve_subs_id() */

/*===========================================================================
FUNCTION      DS3G_IS_HDR_HYBRID_STACK_ACTIVE

DESCRIPTION   This function returns the status of HDR hybrid stack.

DEPENDENCIES  None

RETURN VALUE  TRUE:  HDR Hybrid stack active
              FALSE: HDR Hybrid stack inactive

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_hdr_hybrid_stack_active
(
  sys_modem_as_id_e_type  as_id
)
{
  boolean                   ret_val = FALSE;
  ds3g_ss_event_info_type  *ss_event_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_event_info = ds3gstkproc_get_ss_event_info_instance(
                    ds3gsubsmgr_subs_id_cm_to_ds3g(as_id));

  if(ss_event_info != NULL)
  {
    if( (ss_event_info->stack_info[1].is_operational == TRUE) &&
        (ss_event_info->stack_info[1].sys_mode == SYS_SYS_MODE_HDR) )
    {
      ret_val = TRUE;
    }
  }
  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3g_is_hdr_hybrid_stack_active subs_id:%d ret_val:%d",
                  as_id, ret_val);
  return ret_val;
}/* ds3g_is_hdr_hybrid_stack_active() */

/*===========================================================================
FUNCTION      DS3G_IS_LTE_HYBRID_STACK_ACTIVE

DESCRIPTION   This function returns the status of GW hybrid stack.

DEPENDENCIES  None

RETURN VALUE  TRUE:  GW Hybrid stack active
              FALSE: GW Hybrid stack inactive

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_lte_hybrid_stack_active
(
  sys_modem_as_id_e_type  as_id
)
{
  boolean                   ret_val = FALSE;
  ds3g_ss_event_info_type  *ss_event_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_event_info = ds3gstkproc_get_ss_event_info_instance(
                    ds3gsubsmgr_subs_id_cm_to_ds3g(as_id));
  /*-------------------------------------------------------------------------
    Rules from MMODE:
    If is_stack2_operational is FALSE,then it is NON-SVLTE mode of operation.
    If is_stack2_operational is TRUE, then it is SVLTE mode of operation.
  -------------------------------------------------------------------------*/

  if(ss_event_info != NULL)
  {
    if( ss_event_info->is_stack2_operational == TRUE)
    {
      ret_val = TRUE;
    }
  }
  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3g_is_lte_hybrid_stack_active subs_id:%d ret_val:%d",
                  as_id, ret_val);
  return ret_val;
}/* ds3g_is_lte_hybrid_stack_active() */

/*===========================================================================
FUNCTION      DS3G_GET_PLMN_ID_INFO

DESCRIPTION   This function returns the various PLMN IDs.
              Note: memory has to be allocated by user.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_get_plmn_id_info
(
  ds_sys_subscription_enum_type   ds_subs_id,
  ds3g_plmn_id_info_type         *plmn_id_info_p
)
{
  sys_modem_as_id_e_type      subs_id;
  sys_plmn_id_s_type          rplmn;
  sys_plmn_id_s_type          current_plmn_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate ptr and subs_id
  -------------------------------------------------------------------------*/
  if( plmn_id_info_p == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_get_plmn_id_info NULL ptr passed");
    return;
  }

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm(ds_subs_id);
  if( ds3gsubsmgr_is_subs_id_valid(subs_id) == FALSE )
  {
    return;
  }

  memset(&rplmn, 0, sizeof(sys_plmn_id_s_type)); 
  memset(&current_plmn_id, 0, sizeof(sys_plmn_id_s_type));
  ds3g_get_current_plmn_id(&current_plmn_id, subs_id);

  /*-------------------------------------------------------------------------
    Get EPLMN list and RPLMN
  -------------------------------------------------------------------------*/
  ds_3gpp_get_eplmn_list_per_sub(subs_id,
                                 &(plmn_id_info_p->eplmn_list),
                                 &rplmn);

  /*-------------------------------------------------------------------------
    Compare RPLMN from NAS with cached current PLMN.
    If it does not match clear EPLMN list
  -------------------------------------------------------------------------*/
  if( memcmp(&rplmn,
             &current_plmn_id,
             sizeof(sys_plmn_id_s_type)) )
  {
    memset( &(plmn_id_info_p->eplmn_list), 0, sizeof(sys_plmn_list_s_type) );
  }

  /*-------------------------------------------------------------------------
    Copy RPLMN from local cache
  -------------------------------------------------------------------------*/
  memscpy(&(plmn_id_info_p->rplmn),
          sizeof(sys_plmn_id_s_type),
          &current_plmn_id,
          sizeof(sys_plmn_id_s_type));

  /*-------------------------------------------------------------------------
    Get EHPLMN list
  -------------------------------------------------------------------------*/
  ds_3gpp_get_ehplmn_list_per_sub( subs_id, &(plmn_id_info_p->ehplmn_list) );

  /*-------------------------------------------------------------------------
    Get HPLMN
  -------------------------------------------------------------------------*/
  ds_3gpp_get_hplmn_id_per_sub(subs_id, &(plmn_id_info_p->hplmn) );

  return;
} /* ds3g_get_plmn_id_info() */

/*===========================================================================
FUNCTION      DS3G_CHECK_IF_RPLMN_IS_VPLMN

DESCRIPTION   Utility function to check if passed RPLMN is VPLMN or not.

DEPENDENCIES  None

RETURN VALUE  TRUE:  Passed RPLMN is VPLMN.
              FALSE: Passed RPLMN is not VPLMN.
                i.e. Passed RPLM is neither in EHPLMN list nor the HPLMN

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_check_if_rplmn_is_vplmn
(
  ds3g_plmn_id_info_type  *plmn_id_info_p
)
{
  boolean  is_vplmn = TRUE;
  uint8    index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( plmn_id_info_p == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3g_check_if_rplmn_is_vplmn NULL ptr passed");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Check if RPLMN is same as HPLMN
  -------------------------------------------------------------------------*/
  if(sys_plmn_match(plmn_id_info_p->rplmn, plmn_id_info_p->hplmn) == TRUE )
  {
    is_vplmn = FALSE;
  }

  /*-------------------------------------------------------------------------
    Check if RPLMN is in EHPLMN list
  -------------------------------------------------------------------------*/
  for( index = 0; index < plmn_id_info_p->ehplmn_list.length; index++)
  {
    if(sys_plmn_match(plmn_id_info_p->rplmn,
                      plmn_id_info_p->ehplmn_list.plmn[index]) == TRUE )
    {
      is_vplmn = FALSE;
    }
  }

  return is_vplmn;
} /* ds3g_check_if_rplmn_is_vplmn() */

/*===========================================================================
FUNCTION DS3GMGR_SEND_TRAT_IND

DESCRIPTION
  This function will extract the Profile Context for a given attach profile
  and sent it along with TRAT indication to DS3G

PARAMETERS
  trat: Target RATs sys mode
  profile_id: Attach Profile ID
  subs_id: Current active subscription

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If indication was sent successfully
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gmgr_send_trat_ind
(
  sys_sys_mode_e_type       trat,
  uint16                    profile_id,
  ds_umts_pdp_profile_type *prof_context,
  sys_modem_as_id_e_type    subs_id
)
{
  boolean  ret_val = FALSE;
  ds_umts_pdp_profile_status_etype  status = DS_UMTS_PDP_FAIL;
  ds3geventmgr_attach_prof_info_type     profile_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(FALSE == ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }

  if(SYS_SYS_MODE_NONE == trat ||
     SYS_SYS_MODE_NO_SRV == trat)
  {
    ds3g_set_target_rat_ex(SYS_SYS_MODE_NONE,
                           NULL,
                           subs_id);
    ret_val = TRUE;
  }
  else
  {
    if(NULL != prof_context)
    {
      memset(prof_context, 0, sizeof(ds_umts_pdp_profile_type));
      status = ds_umts_get_pdp_profile_all_data_per_subs(profile_id,
                                dsumts_subs_mgr_get_subs_id(subs_id),
                                                      prof_context);

      if(DS_UMTS_PDP_SUCCESS == status)
      {
        memset(&profile_info, 0, sizeof(ds3geventmgr_attach_prof_info_type));
        strlcpy(profile_info.apn_name,
                (const char*)prof_context->context.apn,
                DS_SYS_MAX_APN_LEN);

        profile_info.emergency_calls_supported =
                             prof_context->emergency_calls_are_supported;
        ds3g_set_target_rat_ex(SYS_SYS_MODE_LTE,
                               &profile_info,
                               subs_id );
        ret_val = TRUE;
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Unable to extract context"
              "for Attach Prof: %d, No TRAT indication sent",profile_id);
      }
    }
    else
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Invalid Profile context passed: 0x%x",prof_context);
    }
  }
  return ret_val;
} /*ds3gmgr_send_trat_ind ()*/
/*===========================================================================
FUNCTION      DS3G_GET_ROAMING_STATUS

DESCRIPTION   This function is a wrapper to call the 3GPP get function. 
              This tells whether the UE is Roaming or not.
  
PARAMETERS    Subscription Id 
    
DEPENDENCIES  None.
  
RETURN VALUE  ds3geventmgr_roaming_status_info: is_roaming - in roam or not 
                                                sys_mode - current sys mode 
   
SIDE EFFECTS  None.
  
===========================================================================*/
ds3geventmgr_roaming_status_info_type ds3g_get_roaming_status
(
  sys_modem_as_id_e_type subs_id
)
{
  ds3geventmgr_roaming_status_info_type roaming_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&roaming_status,0,sizeof(ds3geventmgr_roaming_status_info_type));

  roaming_status.is_roaming = ds_3gpp_roaming_get_status(subs_id);
  roaming_status.sys_mode = ds3g_get_current_network_mode_ex(subs_id);

  return roaming_status;
} /* ds3g_get_lte_roaming_status */