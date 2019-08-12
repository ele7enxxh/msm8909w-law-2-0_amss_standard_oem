/*===========================================================================

                      D S _ R M N E T _ V 4 _ S M . C
DESCRIPTION

  The Data Services Rm Network interface State Machine source file.

  This state machine manages the IPv4 Rm network interface and 
  coordinates wireless WAN connectivity between the Rm and Um interfaces 
  over all supported radio technologies.

EXTERNALIZED FUNCTIONS

  rmnet_v4_sm_init()
    Allocate and initialize all data required for the IPv4 RmNet State Machine.

  rmnet_v4_sm_post_event()
    Post an event to RmNet State Machine.

  rmnet_v4_sm_process_ev_cmd()  
    Process a cmd posted to Rm State Machine.


Copyright (c) 2007-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/src/ds_rmnet_v4_sm.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
08/03/12    sn     RmNet should be moved to up state on runtime settings query
                   in IP mode only.
02/17/12    sb      Fixed Compiler and KW Warnings.
10/12/11    am     Changed calc_rm_iface_addr to get IP from associated 
                   DHCP server for v4.
10/04/11    sa     Q6 free floating changes for QMI.
08/25/11    sa     Fix to avoid DHCP server going into stale condition.
03/01/11    kk     Removing DCC command buffer dependency from RmNet.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
04/01/08    mct    Added support for IP mode.
10/01/07    mct    Initial implementation.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "rex.h"

#include "ps_iface_defs.h"
#include "ps_ifacei_event.h"
#include "ps_in.h"
#include "ps_lan_llc.h"

#include "dcc_task_defs.h"
#include "dcc_task_svc.h"
#include "dhcp.h"

#include "ds_rmnet_defs.h"
#include "ds_rmnet_smi.h"
#include "ds_rmnet_sm_ext.h"

#include "ps_arp.h"

#include "ds_qmi_cflog.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                                DATA TYPES

===========================================================================*/
extern rex_crit_sect_type lan_llc_inst_to_um_iface_ptr_crit_sec;
/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  RMNET_V4_SM state management and event processing functions
---------------------------------------------------------------------------*/
#define RMNET_V4_SM_INFO_TO_INSTANCE(info) RMNET_META_SM_INFO_TO_INSTANCE(info)

/*---------------------------------------------------------------------------
  Time to wait for Rm DHCP setup to complete. (in seconds)
 
  This time should cover the worst case scenario.
  - TODO:  Add guard timer for DHCP UP.
---------------------------------------------------------------------------*/
#define RMNET_DHCP_MAX_SETUP_TIME            120


/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  RMNET_V4_SM state management and event processing functions
---------------------------------------------------------------------------*/

static void rmnet_v4_smi_transition_state
(
  rmnet_smi_info_type    *  info,
  rmnet_v4_smi_state_type   new_state              /* state to tranistion to */
);

static void rmnet_v4_smi_handle_start_addr_cfg_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v4_smi_handle_stop_addr_cfg_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v4_smi_handle_dhcp_stop_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v4_smi_handle_dhcp_started_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v4_smi_handle_dhcp_failure_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v4_smi_handle_dhcp_complete_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v4_smi_handle_um_addr_change_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v4_smi_handle_guard_timer_expiration_ev
(
  rmnet_smi_info_type *  info
);


/*---------------------------------------------------------------------------
  RMNET_V4_SM callback functions registered with various external modules
---------------------------------------------------------------------------*/

static void rmnet_v4_smi_um_iface_ev_hdlr_cb
(
  ps_iface_type *this_iface_ptr,           /* Pointer to the interface     */
  ps_iface_event_enum_type   event,        /* Event type occurred          */
  ps_iface_event_info_u_type event_info,   /* Event specific info block    */
  void *user_data_ptr           /* Data passed during event registration   */
);

static void rmnet_v4_smi_dhcp_started_cb
(
  void * userdata, 
  void * handle
);

static void rmnet_v4_smi_dhcp_stopped_cb
(
  void *   userdata, 
  boolean  ok
);

static void rmnet_v4_smi_dhcp_msg_hdlr
(
  rmnet_smi_info_type *  info,
  dhcp_msg_type          msg_type,
  uint32                 ipaddr
);

static void rmnet_v4_smi_handle_dhcp_start_or_fail_hdlr
(
  rmnet_smi_info_type *  info,
  void * handle
);

static void rmnet_v4_smi_timer_cback
(
  timer_cb_data_type  user_data
);

static void rmnet_v4_smi_dhcp_msg_cback
(
  void * userdata,
  dhcp_msg_type msg_type,
  uint32 ipaddr
);

/*---------------------------------------------------------------------------
  LAN_LLC ARP proxy callback for RmNet interface
---------------------------------------------------------------------------*/
static boolean rmnet_v4_smi_arp_proxy
(
  lan_lle_enum_type instance,
  uint32 src_ip,
  uint32 target_ip
);


/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  Function table registered with siolib to manage serial port
---------------------------------------------------------------------------*/
#ifndef T_ARM
/*---------------------------------------------------------------------------
  These names need to be kept in sync with the enums which they describe.
---------------------------------------------------------------------------*/
static char *rmnet_v4_smi_state_names[RMNET_V4_SM_MAX_STATE] =
{
  "Null state",
  "Waiting for DHCP server init",
  "Waiting for DHCP to complete",
  "Rm address config complete"
};

#endif /* T_ARM */

/*---------------------------------------------------------------------------
  RMNET RMSM command handler array
---------------------------------------------------------------------------*/
typedef void (*rmnet_v4_smi_cmd_hdlr_type)( rmnet_smi_info_type *);

static rmnet_v4_smi_cmd_hdlr_type rmnet_v4_smi_cmd_hdlr[RMNET_V4_SM_MAX_EV] =
{
  rmnet_v4_smi_handle_start_addr_cfg_ev,                        /* event 0 */
  rmnet_v4_smi_handle_guard_timer_expiration_ev,                /* event 1 */
  rmnet_v4_smi_handle_dhcp_complete_ev,                         /* event 2 */
  rmnet_v4_smi_handle_um_addr_change_ev,                        /* event 3 */
  rmnet_v4_smi_handle_stop_addr_cfg_ev,                         /* event 4 */
  rmnet_v4_smi_handle_dhcp_stop_ev                              /* event 5 */
};


/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_V4_SM_INIT()

  DESCRIPTION
    This function initializes the IPv4 state machine variables.

  PARAMETERS
    instance: The RmNet instance

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v4_sm_init
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type * info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(instance < RMNET_INSTANCE_MAX);
  info = rmnet_smi_info[instance];
  ASSERT(info);

  timer_def( &(info->v4_sm.guard_timer),
             NULL,
             NULL,
             0,
             rmnet_v4_smi_timer_cback,
             (timer_cb_data_type)info );

  info->v4_sm.um_addr_changed_buf_ptr = 
    ps_iface_alloc_event_cback_buf(rmnet_v4_smi_um_iface_ev_hdlr_cb,
                                   (void *)info);

  if(info->v4_sm.um_addr_changed_buf_ptr  == NULL)
  {
    LOG_MSG_ERROR_0("Event buf allocation failed");
    ASSERT(0);
  }

  info->v4_sm.inited = TRUE;
} /* rmnet_v4_sm_init() */

/*===========================================================================
  FUNCTION RMNET_V4_SM_INIT_CONFIG()

  DESCRIPTION
    This function initializes the IPv4 specific (Rm iface and LLC) parameters.

  PARAMETERS
    start_info: The LLC start information to populate.

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v4_sm_init_config
(
  rmnet_smi_info_type     * info,
  lan_llc_start_info_type * start_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Setup IPv4 specific LLC parameters.
  -------------------------------------------------------------------------*/
  if (RMNET_LINK_PROT(info) == RMNET_ETHERNET_MODE)
  {
    start_info->arp_config.enable_cache_maintenance = FALSE;
    start_info->arp_config.enable_proxy             = TRUE;
    start_info->arp_config.arp_proxy_query_f_ptr    = rmnet_v4_smi_arp_proxy;
  }

  if(-1 == ps_iface_set_addr_family(info->rmnet_iface_ptr,
                                    IFACE_IPV4_ADDR_FAMILY))
  {
    LOG_MSG_ERROR_0("Failed to set Rm addr family to IPv4!");
    rmnet_meta_sm_post_event( RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                              RMNET_META_SM_ADDR_CFG_FAILURE_EV );
    return;
  }

  info->sub_sm_type = RMNET_V4_SM;

} /* rmnet_v4_sm_init_config() */



/*===========================================================================
  FUNCTION RMNET_V4_SM_POST_EVENT()

  DESCRIPTION
    This function posts an event to RMNetSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
    instance:  Instance of RmNetSm to post event to
    event: The event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v4_sm_post_event
(
  rmnet_instance_e_type    instance,
  rmnet_v4_sm_event_type   event
)
{
  rmnet_smi_ev_cmd_type * cmd_ptr;
  rmnet_smi_info_type *  info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(instance < RMNET_INSTANCE_MAX);
  info = rmnet_smi_info[instance];
  ASSERT(info);

  if (event >= RMNET_V4_SM_MAX_EV)
  {
    ASSERT (0);
    LOG_MSG_ERROR_1 ("Invalid RMNET_V4_SM event %d, ignored", event);
    return;
  }
#if 0
#ifdef T_ARM
  LOG_MSG_INFO1_2( "Event %d posted to RMNET_V4_SM in state %d",
                   event, info->v4_sm.state );
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_HIGH,
                "'%s' event posted to RMNET_V4_SM in state %d.",
                rmnet_v4_smi_event_names[event], info->v4_sm.state);
#endif
#endif
  /*-------------------------------------------------------------------------
    Get a DCC command buffer
  -------------------------------------------------------------------------*/
  cmd_ptr = (rmnet_smi_ev_cmd_type *)RMNET_SMI_GET_CMD_BUF_ASSERT(DCC_RMNET_SM_EV_CMD);
  if( cmd_ptr == NULL )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Post cmd to DCC task corresponding to each event
  -------------------------------------------------------------------------*/
  cmd_ptr->sm_id             = RMNET_V4_SM;
  cmd_ptr->event             = event;
  cmd_ptr->rmnet_sm_info_ptr = (void *) info;

  dcc_send_cmd_ex(DCC_RMNET_SM_EV_CMD, (void *)cmd_ptr );

} /* rmnet_v4_sm_post_event() */


/*===========================================================================
  FUNCTION RMNET_V4_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM event cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd_ptr:  serving task command buffer containing the RmNet sm
              event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v4_sm_process_ev_cmd
(
  dcc_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  rmnet_smi_info_type    *  info;
  rmnet_v4_sm_event_type    event;
  rmnet_smi_ev_cmd_type   * cmd_ptr;
  boolean                   is_err = FALSE;
  typedef enum{
  	ERR_NONE = 0,
  	ERR_INIT = 1,
	ERR_EVENT_V4_MAX = 2,
	ERR_V4_SM_MAX_STATE = 3,
  	}err_type;
  err_type err = ERR_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = (rmnet_smi_ev_cmd_type *)user_data_ptr;
  ASSERT (cmd_ptr && (cmd == DCC_RMNET_SM_EV_CMD) && (cmd_ptr->sm_id == RMNET_V4_SM));

  event = (rmnet_v4_sm_event_type) cmd_ptr->event;
  info  = (rmnet_smi_info_type *) cmd_ptr->rmnet_sm_info_ptr;

  LOG_MSG_INFO2_3( "RmNet IPv4 Sm event %d in state %d, info %x",
                   event, info->v4_sm.state, info );

  /*-------------------------------------------------------------------------
    Verify that RMNET_V4_SM is initialized
  -------------------------------------------------------------------------*/
  do
  {
    if(info->v4_sm.inited != TRUE)
    {
      is_err = TRUE;
      err = ERR_INIT;
      break;
    }
  
    /*-------------------------------------------------------------------------
      Validate posted event before processing
    -------------------------------------------------------------------------*/
    if (event >= RMNET_V4_SM_MAX_EV)
    {
      is_err = TRUE;
      err = ERR_EVENT_V4_MAX;
      break;
    }
  
    /*-------------------------------------------------------------------------
      Validate RMNET_V4_SM state before processing event - this allows event handler
      functions to assume that state is valid.
    -------------------------------------------------------------------------*/
    if (info->v4_sm.state >= RMNET_V4_SM_MAX_STATE)
    {
      is_err = TRUE;
      err = ERR_V4_SM_MAX_STATE;
      break;
    }
  
    if ( RMNET_V4_SM != info->sub_sm_type )
    {
      LOG_MSG_ERROR_2("Invalid SUB SM type %d, ignoring event %d!",
                      info->sub_sm_type, event);
      return;
    }
  }while(0);
  if(is_err == TRUE)
  {
    LOG_MSG_ERROR_2("error (%d) for event (%d)",err,event);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke associated command handler function
  -------------------------------------------------------------------------*/
  (*(rmnet_v4_smi_cmd_hdlr[event]))(info);

} /* rmnet_v4_sm_process_ev_cmd() */

/*===========================================================================
  FUNCTION RMNET_V4_SM_PROCESS_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM cmd.

  PARAMETERS
    cmd_ptr:  serving task command buffer containing the RmNet sm
              event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v4_sm_process_cmd
(
  dcc_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  rmnet_smi_cmd_type * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = (rmnet_smi_cmd_type *)user_data_ptr;
  ASSERT (cmd_ptr && (cmd == DCC_RMNET_SM_CMD) && (cmd_ptr->sm_id == RMNET_V4_SM));

  switch(cmd_ptr->cmd_type)
  {
    case RMNET_SM_CMD_DHCP_MSG:
      rmnet_v4_smi_dhcp_msg_hdlr( cmd_ptr->info_ptr,
                            (dhcp_msg_type)(cmd_ptr->data.dhcp_msg.msg_type),
                            cmd_ptr->data.dhcp_msg.ipv4_addr );
      break;

    case RMNET_SM_CMD_DHCP_START_OR_FAIL:
      rmnet_v4_smi_handle_dhcp_start_or_fail_hdlr( cmd_ptr->info_ptr,
                                    cmd_ptr->data.dhcp_started.handle );
      break;

    default:
      LOG_MSG_INFO1_1("Received Unhandled rmnet_v4_sm cmd %d",
                      cmd_ptr->cmd_type);
      ASSERT(0);
      break;
  }
} /* rmnet_v4_sm_process_cmd() */

/*===========================================================================
FUNCTION RMNET_V4_SM_CALC_RM_IFACE_ADDR

DESCRIPTION
  This function is used to calculate the RM iface IP address and subnet mask
  
PARAMETERS
  um_iface_ptr :  IN 
  rm_ip_addr:  OUT
  subnet_mask:  OUT

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.                                         
===========================================================================*/
void rmnet_v4_sm_calc_rm_iface_addr
(
  rmnet_instance_e_type instance,
  uint32 *              te_ip_addr,
  uint32 *              rm_ip_addr,
  uint32 *              subnet_mask
)
{
  rmnet_smi_info_type *info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT((NULL != te_ip_addr) && (NULL != rm_ip_addr) && 
           (NULL != subnet_mask) && (instance < RMNET_INSTANCE_MAX));
  info = rmnet_smi_info[instance];
  ASSERT(info);

  if (info->v4_sm.dhcp_handle)
  {
    dhcp_calc_gateway_info(info->v4_sm.dhcp_handle,
                           te_ip_addr,
                           rm_ip_addr,
                           subnet_mask);
  }
} /* rmnet_v4_sm_calc_rm_iface_addr */


/*===========================================================================
FUNCTION RMNET_V4_SM_CLEAR_GUARD_TIMER

DESCRIPTION
  This function is called by QMI_WDS to clear the guard timer and move rmnet
  sm to UP state when client querries ADDR CONFIG info from QMI_WDS
  
PARAMETERS
  instance       : RmNet instance
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.                                         
===========================================================================*/
void rmnet_v4_sm_clear_guard_timer
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type *  info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(instance < RMNET_INSTANCE_MAX);
  info = rmnet_smi_info[instance];
  ASSERT(info);

  /*---------------------------------------------------------------------
    For ethernet mode, IP address configuration is expected to happen 
    using DHCP, not using runtime settings query.
  ---------------------------------------------------------------------*/ 
#ifndef FEATURE_DATA_NO_QMI_TEST
  if (RMNET_LINK_PROT(info) != RMNET_IP_MODE)
  {
    LOG_MSG_INFO2_1 ("Rmnet Instance %d not in IP mode, ignoring Runtime "
                     "settings request to clear guard timer", instance);
    return;
  }
#endif /* FEATURE_DATA_NO_QMI_TEST */
  
  switch(info->v4_sm.state)
  {
    case RMNET_V4_SM_DHCP_IN_PROGRESS_STATE:
    {
      /*---------------------------------------------------------------------
        Host is configuring IP address using runtime settings
        -- cancel guard timer and transition state
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0 ("Runtime settings queried, moving to UP state");
      (void) timer_clr( &(info->v4_sm.guard_timer), T_NONE );
      rmnet_v4_smi_transition_state(info, 
                                    RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE);
      break;
    }

    default:
      LOG_MSG_INFO2_1("Do nothing - clear guard timer called in V4_SM state",
                      info->v4_sm.state);
  }
}
/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
  FUNCTION RMNET_V4_SMI_TIMER_CBACK()

  DESCRIPTION
    DHCP guard timer callback

  PARAMETERS
    user_data -> rmnet_smi_info

  RETURN VALUE
    None

  DEPENDENCIES
    rmnet_sio_init() must have been called previously

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_timer_cback
(
  timer_cb_data_type  user_data
)
{
  rmnet_smi_info_type *  info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (user_data);
  info = (rmnet_smi_info_type *) user_data;

  LOG_MSG_INFO1_0 ("DHCP guard timer expired -- tear down");
  rmnet_v4_sm_post_event(RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                         RMNET_V4_SM_GUARD_TIMER_EXPIRATION_EV);

} /* rmnet_v4_smi_timer_cback() */


/*===========================================================================
  FUNCTION RMNET_V4_SMI_HANDLE_START_ADDR_CFG_EV

  DESCRIPTION
    This function processes the IPv4 start address configuration event. 

    This event indicates that the meta state machine is requesting to 
    bringup an IPv4 session.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_handle_start_addr_cfg_ev
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(info->sub_sm_type == RMNET_V4_SM);

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd START_ADDR_CFG_EV, state %d", info->v4_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd START_ADDR_CFG_EV, state '%s'\n",
                rmnet_v4_smi_state_names[info->v4_sm.state] );
#endif

  switch( info->v4_sm.state )
  {
    case RMNET_V4_SM_NULL_STATE:
    {
      if(info->um_iface_ptr)
      {
        if ( ps_iface_event_cback_reg( info->um_iface_ptr,
                                       IFACE_ADDR_CHANGED_EV,
                                       info->v4_sm.um_addr_changed_buf_ptr
                                     ) < 0 )
        {
          LOG_MSG_ERROR_0("IFACE Address change event reg failed!");
          rmnet_meta_sm_post_event( RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                                    RMNET_META_SM_ADDR_CFG_FAILURE_EV );
          return;
        }

        rmnet_v4_smi_transition_state(info,RMNET_V4_SM_DHCP_WAITING_STATE);
      }
      break;
    }
    case RMNET_V4_SM_DHCP_WAITING_STATE:
    case RMNET_V4_SM_DHCP_IN_PROGRESS_STATE:
    case RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE:
      /* fall through */
    default:  /* all other states */
    {
      LOG_MSG_ERROR_1("Got START_ADDR_CFG_EV in unexpected state %d",
                      (int)(info->v4_sm.state));
      break;
    }
  } /* switch( info->v4_sm.state ) */
} /* rmnet_v4_smi_handle_start_addr_cfg_ev() */


/*===========================================================================
  FUNCTION RMNET_V4_SMI_HANDLE_STOP_ADDR_CFG_EV

  DESCRIPTION
    This function processes the IPv4 stop address configuration event. 

    This event indicates that the meta state machine is requesting to 
    tear down an IPv4 session.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_handle_stop_addr_cfg_ev
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!((info->meta_sm.net_params.is_mcast_iface == FALSE) &&
       (info->sub_sm_type != RMNET_INVALID_SM)))
  {
    LOG_MSG_INFO1_1( "Dropping STOP_ADDR_CFG_EV, state %d",
                     info->v4_sm.state );
    return;
  }

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd STOP_ADDR_CFG_EV, state %d", info->v4_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED, 
                "Recvd STOP_ADDR_CFG_EV, state '%s'\n",
                rmnet_v4_smi_state_names[info->v4_sm.state] );
#endif

  switch( info->v4_sm.state )
  {
    case RMNET_V4_SM_NULL_STATE:
      /*---------------------------------------------------------------------
        Do nothing already in NULL state. Do not ASSERT because start can
        fail due to event registration failure and won't transition from
        NULL
      ---------------------------------------------------------------------*/
      /*-------------------------------------------------------------------------
      Reset the sub state machine type.
      -------------------------------------------------------------------------*/
      info->sub_sm_type = RMNET_INVALID_SM;
      break;
      
    case RMNET_V4_SM_DHCP_IN_PROGRESS_STATE:
    case RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE:
    case RMNET_V4_SM_DHCP_WAITING_STATE:
    {
      rmnet_v4_smi_transition_state(info, 
                                    RMNET_V4_SM_NULL_STATE);
      break;
    }

    default:  /* all other states */
    {
      LOG_MSG_ERROR_0("Got stop address cfg event in unexpected state");
      break;
    }
  } /* switch( info->v4_sm.state ) */

} /* rmnet_v4_smi_handle_stop_addr_cfg_ev() */


/*===========================================================================
  FUNCTION RMNET_V4_SMI_HANDLE_DHCP_START_OR_FAIL_HDLR

  DESCRIPTION
    This function handles the DHCP server start and fail event. 

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_handle_dhcp_start_or_fail_hdlr
(
  rmnet_smi_info_type *  info,
  void * handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-------------------------------------------------------------------------
  If DHCP server fails to initiate cleanup.
-------------------------------------------------------------------------*/
  if(NULL == (info->v4_sm.dhcp_handle = handle))
  {
    rmnet_v4_smi_handle_dhcp_failure_ev(info);
    return;
  }

  rmnet_v4_smi_handle_dhcp_started_ev(info);
}
/*===========================================================================
  FUNCTION RMNET_V4_SMI_HANDLE_DHCP_STARTED_EV

  DESCRIPTION
    This function processes the DHCP server started event. 

    This event indicates that the DHCP server was started ok, and now
    we can send media connect back to the tethered device.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_handle_dhcp_started_ev
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd DHCP_STARTED_EV, state %d", info->v4_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd DHCP_STARTED_EV, state '%s'\n",
                rmnet_v4_smi_state_names[info->v4_sm.state]);
#endif

  switch( info->v4_sm.state )
  {
    case RMNET_V4_SM_NULL_STATE:
    {
      LOG_MSG_INFO1_0("Got DHCP started ev in NULL state. Stop DHCP");
      if(info->v4_sm.dhcp_handle != NULL)
      {
        (void) dhcp_stop(&(info->v4_sm.dhcp_handle), 
                         rmnet_v4_smi_dhcp_stopped_cb, 
                         info);
      }
      break;
    }

    case RMNET_V4_SM_DHCP_IN_PROGRESS_STATE:
    case RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE:
      LOG_MSG_ERROR_0("Received DHCP started ev in invalid state.");
      break;

    case RMNET_V4_SM_DHCP_WAITING_STATE:
    {
      rmnet_v4_smi_transition_state(info, 
                                    RMNET_V4_SM_DHCP_IN_PROGRESS_STATE);
      break;
    }

    default:  /* all other states */
    {
      LOG_MSG_ERROR_0("Got DHCP started event in unexpected state");
      break;
    }
  } /* switch( info->v4_sm.state ) */

} /* rmnet_v4_smi_handle_dhcp_started_ev() */



/*===========================================================================
  FUNCTION RMNET_V4_SMI_HANDLE_DHCP_FAILURE_EV

  DESCRIPTION
    This function processes the DHCP server failure event. 

    This event indicates that the DHCP server failed to start. Indicate to 
    the meta SM that it should cleanup.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_handle_dhcp_failure_ev
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd DHCP_FAILURE_EV, state %d", info->v4_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED, 
                "Recvd DHCP_FAILURE_EV, state '%s'\n",
                rmnet_v4_smi_state_names[info->v4_sm.state]);
#endif

  switch( info->v4_sm.state )
  {
    case RMNET_V4_SM_DHCP_WAITING_STATE:
    {
      rmnet_meta_sm_post_event(RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                               RMNET_META_SM_ADDR_CFG_FAILURE_EV);
      break;
    }

    case RMNET_V4_SM_NULL_STATE:
    case RMNET_V4_SM_DHCP_IN_PROGRESS_STATE:
    case RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE:
      /* Fall through */
    default:  /* all other states */
    {
      LOG_MSG_INFO1_0("Got DHCP failure event in unexpected state");
      break;
    }
  } /* switch( info->v4_sm.state ) */
} /* rmnet_v4_smi_handle_dhcp_failure_ev() */

  /*===========================================================================
  FUNCTION RMNET_V4_SMI_HANDLE_DHCP_STOP_EV

  DESCRIPTION
    This function processes the DHCP server stop event. 

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_handle_dhcp_stop_ev
(
  rmnet_smi_info_type *  info
)
{
#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd DHCP_STOP_EV, state %d", info->v4_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd DHCP_STOP_EV, state '%s'\n",
                rmnet_v4_smi_state_names[info->v4_sm.state]);
#endif

  info->v4_sm.dhcp_handle = NULL;
}


/*===========================================================================
  FUNCTION RMNET_V4_SMI_HANDLE_DHCP_COMPLETE_EV

  DESCRIPTION
    This function processes the DHCP server started event. 

    This event indicates that the DHCP server was started ok, and now
    we can send media connect back to the tethered device.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_handle_dhcp_complete_ev
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(info->sub_sm_type == RMNET_V4_SM);

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd DHCP_COMPLETE_EV, state %d", info->v4_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd DHCP_COMPLETE_EV, state '%s'\n",
                rmnet_v4_smi_state_names[info->v4_sm.state]);
#endif

  switch( info->v4_sm.state )
  {

    case RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE:
      /*---------------------------------------------------------------------
        DHCP ACK and INFORM can be sent at any time, which will generate the
        DHCP_COMPLETE_EV
      ---------------------------------------------------------------------*/
      break;

    case RMNET_V4_SM_DHCP_IN_PROGRESS_STATE:
    {
      rmnet_v4_smi_transition_state(info, 
                                    RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE);
      break;
    }

    case RMNET_V4_SM_NULL_STATE:
    {
      /* case where DHCP sends ACK before Iface down, but Rmnet process
         Iface down Ev before processing this, so Rmnet can be in NULL
         state in this case 
         Ignore the EV for now, Later see if this case can be closed */
      LOG_MSG_INFO1_0("Ignoring DHCP Complete event in NULL state");
      break;
    }

    case RMNET_V4_SM_DHCP_WAITING_STATE:
    /*---------------------------------------------------------------------
      ASSERT since DHCP hasn't even been started yet, and fall through.
    ---------------------------------------------------------------------*/

    default:  /* all other states */
    {
      LOG_MSG_ERROR_0("Got DHCP complete event in unexpected state");
      break;
    }
  } /* switch( info->v4_sm.state ) */
} /* rmnet_v4_smi_handle_dhcp_complete_ev() */


/*===========================================================================
  FUNCTION RMNET_V4_SMI_HANDLE_UM_ADDR_CHANGE_EV

  DESCRIPTION
    Called when the Um interface's IP address changes - will result in
    sending a media disconnect back to the host, restarting the DHCP server
    and then sending media connect.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_handle_um_addr_change_ev
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(info->sub_sm_type == RMNET_V4_SM);

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd UM_ADDR_CHANGE_EV, state %d", info->v4_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd UM_ADDR_CHANGE_EV, state '%s'\n",
                rmnet_v4_smi_state_names[info->v4_sm.state]);
#endif

  switch( info->v4_sm.state )
  {
    case RMNET_V4_SM_DHCP_IN_PROGRESS_STATE:
    case RMNET_V4_SM_DHCP_WAITING_STATE:
      LOG_MSG_INFO1_1("Ignoring UM_ADDR_CHANGE_EV in state %d",
                      info->v4_sm.state);
      break;

    case RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE:
    {
      if(info->v4_sm.dhcp_handle)
      {
        (void) dhcp_stop(&(info->v4_sm.dhcp_handle), 
                         rmnet_v4_smi_dhcp_stopped_cb, 
                         info);

        (void) arp_flush(info->constants.lan_llc_instance);  
      }

      rmnet_v4_smi_transition_state(info, 
                                    RMNET_V4_SM_DHCP_WAITING_STATE);

      rmnet_meta_sm_post_event(RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                               RMNET_META_SM_ADDR_CFG_CHANGED_EV);
      break;
    }
    case RMNET_V4_SM_NULL_STATE:
      /* fall through */
    default:  /* all other states */
    {
      LOG_MSG_ERROR_0("Got DHCP started event in unexpected state");
      break;
    }
  } /* switch( info->v4_sm.state ) */
} /* rmnet_v4_smi_handle_um_addr_change_ev() */



/*===========================================================================
  FUNCTION RMNET_V4_SMI_HANDLE_GUARD_TIMER_EXPIRATION_EV

  DESCRIPTION
    Called when the DHCP guard timer expires. Forces the call to be torn
    down.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_handle_guard_timer_expiration_ev
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(info->sub_sm_type == RMNET_V4_SM);

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd GUARD_TIMER_EXPIRED_EV, state %d",
                   info->v4_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd GUARD_TIMER_EXPIRED_EV, state '%s'\n",
                rmnet_v4_smi_state_names[info->v4_sm.state]);
#endif

  switch( info->v4_sm.state )
  {
    case RMNET_V4_SM_DHCP_IN_PROGRESS_STATE:
  
      rmnet_meta_sm_post_event(RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                               RMNET_META_SM_ADDR_CFG_FAILURE_EV);
      break;

    case RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE:
      LOG_MSG_INFO1_0( "Host didn't reconfigure IPaddr after DHCP release. " 
                       "Tear down call!");
      rmnet_meta_sm_post_event(RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                               RMNET_META_SM_RM_LINK_DOWN_EV);
      break;

    case RMNET_V4_SM_NULL_STATE:
    case RMNET_V4_SM_DHCP_WAITING_STATE:
      /* Fall through */
    default:  /* all other states */
    {
      LOG_MSG_ERROR_1("Got GUARD_TIMER_EXPIRED_EV in unexpected state %d",
                      (int)(info->v4_sm.state));
      break;
    }
  } /* switch( info->v4_sm.state ) */
} /* rmnet_v4_smi_handle_guard_timer_expiration_ev() */


/*===========================================================================
FUNCTION   RMNET_V4_SMI_IFACE_EV_HDLR_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on Um interface.  Various Um iface events are registered based on
  call type during the call setup type.  Each iface event generates 
  appropriate rmnet_v4_sm event .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rmnet_v4_smi_um_iface_ev_hdlr_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{
  rmnet_smi_info_type *  info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (user_data_ptr);

  info = (rmnet_smi_info_type *) user_data_ptr;

  LOG_MSG_INFO2_2( "Recvd iface ev %d from 1x i/f 0x%x",
                   event, this_iface_ptr );

  /*-------------------------------------------------------------------------
    Process flow control events first since they may get posted during data
    transfer
  -------------------------------------------------------------------------*/

  switch( event )
  {
    case IFACE_ADDR_CHANGED_EV:
    {
      LOG_MSG_INFO1_0( "Um IP address changed, posting RMNET_V4_SM_UM_ADDR_CHANGE_EV" );
      rmnet_v4_sm_post_event( RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                              RMNET_V4_SM_UM_ADDR_CHANGE_EV );
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Unregistered event %d recvd, ignoring", event);
      break;
    }
  } /* switch( event ) */
} /* rmnet_v4_smi_um_iface_ev_hdlr_cb() */


/*===========================================================================
  FUNCTION RMNET_V4_SMI_DHCP_STARTED_CB

  DESCRIPTION
    This is the dhcp_start completion callback.

  PARAMETERS
    userdata:  the user data provided when dhcp_start was called
    handle:    handle to the dhcp instance started

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_dhcp_started_cb
(
  void *  userdata, 
  void *  handle
)
{
  rmnet_smi_info_type *  info = (rmnet_smi_info_type *) userdata;
  rmnet_smi_cmd_type    * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0 ("DHCP server is ready");

  if(handle == NULL && info->sub_sm_type != RMNET_V4_SM)
  {
    LOG_MSG_INFO1_0("DHCP handle is NULL");
    return;
  }

  /*-------------------------------------------------------------------------
  Get a DCC command buffer
  -------------------------------------------------------------------------*/
  cmd_ptr = (rmnet_smi_cmd_type *)RMNET_SMI_GET_CMD_BUF_ASSERT(DCC_RMNET_SM_CMD);
  if( cmd_ptr == NULL )
  {
    return;
  }
  
  /*-------------------------------------------------------------------------
    Post cmd to DCC task with the details
  -------------------------------------------------------------------------*/
  cmd_ptr->sm_id        = RMNET_V4_SM;
  cmd_ptr->cmd_type     = RMNET_SM_CMD_DHCP_START_OR_FAIL;
  cmd_ptr->info_ptr     = (void *) info;
  cmd_ptr->data.dhcp_started.handle  = handle;
  dcc_send_cmd_ex(DCC_RMNET_SM_CMD, (void *)cmd_ptr );

} /* rmnet_v4_smi_dhcp_started_cb() */



/*===========================================================================
  FUNCTION RMNET_V4_SMI_DHCP_STOPPED_CB

  DESCRIPTION
    This is the dhcp_stop completion callback.

  PARAMETERS
    userdata:  the user data provided when dhcp_start was called
    handle:    handle to the dhcp instance started

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_dhcp_stopped_cb
(
  void *   userdata, 
  boolean  ok
)
{
  rmnet_smi_info_type *  info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (userdata);
  info = (rmnet_smi_info_type *) userdata;

  LOG_MSG_INFO1_0( "DHCP stopped cb recvd, posting RMNET_V4_SM_DHCP_STOP_EV" );
  rmnet_v4_sm_post_event( RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                              RMNET_V4_SM_DHCP_STOP_EV );

  LOG_MSG_INFO2_1 ("DHCP stopped callback (status = %d)", ok);

} /* rmnet_v4_smi_dhcp_stopped_cb() */


/*===========================================================================
  FUNCTION RMNET_V4_SMI_DHCP_MSG_CBACK

  DESCRIPTION
    Callback invoked on various DHCP messages being rx'ed/tx'ed.

  PARAMETERS
    userdata:  the user data provided when dhcp_start was called
               this was set to the rmnet_v4_smi_info *
    msg_type:  DHCP message of note
    ipaddr:    current yiaddr value

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_dhcp_msg_cback
(
  void *         user_data,
  dhcp_msg_type  msg_type,
  uint32         ipaddr
)
{
  rmnet_smi_info_type  *  info;
  rmnet_smi_cmd_type    * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(user_data);
  info = (rmnet_smi_info_type *) user_data;

  LOG_MSG_INFO2_2( "DHCP msg cback for msg %d in RMNET_V4_SM state %d",
                   msg_type, info->v4_sm.state);

  /*-------------------------------------------------------------------------
    Get a DCC command buffer
  -------------------------------------------------------------------------*/
  cmd_ptr = (rmnet_smi_cmd_type *)RMNET_SMI_GET_CMD_BUF_ASSERT(DCC_RMNET_SM_CMD);
  if( cmd_ptr == NULL )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Post cmd to DCC task with the details
  -------------------------------------------------------------------------*/
  cmd_ptr->sm_id        = RMNET_V4_SM;
  cmd_ptr->cmd_type     = RMNET_SM_CMD_DHCP_MSG;
  cmd_ptr->info_ptr     = (void *) info;
  cmd_ptr->data.dhcp_msg.msg_type  = msg_type;
  cmd_ptr->data.dhcp_msg.ipv4_addr = ipaddr;
  dcc_send_cmd_ex(DCC_RMNET_SM_CMD, (void *)cmd_ptr );

} /* rmnet_v4_smi_dhcp_msg_cback() */

/*===========================================================================
  FUNCTION RMNET_DHCP_MSG_HDLR()

  DESCRIPTION
    This function processes a RmNet RmSM cmd.

  PARAMETERS
    cmd_ptr:  serving task command buffer containing the RmNet sm
              event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_dhcp_msg_hdlr
(
  rmnet_smi_info_type *  info,
  dhcp_msg_type          msg_type,
  uint32                 ipaddr
)
{
  ps_ip_addr_type  um_ip_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT (info && msg_type);

  if(info->v4_sm.inited != TRUE)
  {
    DATA_ERR_FATAL("RMNET_V4_SM not inited");
    return;
  }

  if(info->sub_sm_type != RMNET_V4_SM)
  {
    LOG_MSG_ERROR_2 ("Ignoring DHCP msg cback for msg %d in RMNET_V4_SM state %d",
                     msg_type, info->v4_sm.state);
    return;
  }

  LOG_MSG_INFO2_2( "DHCP msg (%d) ipaddr (%x)", msg_type, ps_ntohl(ipaddr) );

  /*-------------------------------------------------------------------------
    Do stuff when DHCP messages are sent or received
    - DCTM 
  -------------------------------------------------------------------------*/
  switch (msg_type)
  {
    case DHCP_DHCPINFORM:
    {
      /*---------------------------------------------------------------------
        DHCP INFORM implies TE provisioning a static IP
      ---------------------------------------------------------------------*/
      um_ip_addr.type           = IPV4_ADDR;
      um_ip_addr.addr.v4.ps_s_addr = 0;
      ps_iface_get_addr(info->um_iface_ptr, &um_ip_addr);
      if (um_ip_addr.type != IPV4_ADDR || um_ip_addr.addr.v4.ps_s_addr != ipaddr)
      {
        LOG_MSG_ERROR_1("Host is configured for wrong static IP address (%x)!",
                        ipaddr );
        /*-------------------------------------------------------------------
          Wait for guard timer to expire
        -------------------------------------------------------------------*/
        break;
      }
      /*---------------------------------------------------------------------
        Static IP matches that assigned by network
        -- cancel guard timer and proceed to UP state
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1 ("Host has configured correct IPv4 address (%x)",
                       ipaddr);
      (void) timer_clr( &(info->v4_sm.guard_timer), T_NONE );
      rmnet_v4_sm_post_event( RMNET_V4_SM_INFO_TO_INSTANCE(info),
                              RMNET_V4_SM_DHCP_COMPLETE_EV );
      break;
    }

    case DHCP_DHCPACK:
    {
      /*---------------------------------------------------------------------
        Host is using the offered IP address -- cancel guard timer and
        proceed to UP state
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1 ("Host has configured correct IPv4 address (%x)",
                       ipaddr);
      (void) timer_clr( &(info->v4_sm.guard_timer), T_NONE );
      rmnet_v4_sm_post_event( RMNET_V4_SM_INFO_TO_INSTANCE(info),
                              RMNET_V4_SM_DHCP_COMPLETE_EV );
      break;
    }

    case DHCP_DHCPDECLINE:
    {
      /*---------------------------------------------------------------------
        Host doesn't like the offered IP address
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Host declined IP address (%x)!", ipaddr );
      /*-------------------------------------------------------------------
        Wait for guard timer to expire
      -------------------------------------------------------------------*/
      break;
    }

    case DHCP_DHCPRELEASE:
    {
      /*---------------------------------------------------------------------
        Host is releasing IP address -- host either shut down its IP 
        interface or is going to reconfigure.

        Start a guard timer -- if it expires before the tethered device
        reconfigures the IP address, tear down the call.

        Maybe check if already started? Don't want to refresh timer in a cycle
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Host released IP address (%x)!", ps_ntohl(ipaddr) );
      LOG_MSG_INFO1_0("Start DHCP guard timer. Wait for host to reconfigure IPaddr");
      timer_set( &(info->v4_sm.guard_timer), 
                 RMNET_DHCP_MAX_SETUP_TIME, 
                 0, 
                 T_SEC );
      break;
    }
    default:
    {
      /*---------------------------------------------------------------------
        Unknown message
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_0("New DHCP message defined but not handled");
      break;
    }
  }
} /* rmnet_v4_smi_dhcp_msg_hdlr() */


/*===========================================================================
FUNCTION RMNET_V4_SMI_ARP_PROXY

DESCRIPTION
  This function is called to determine if we should ARP proxy for the peer.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: if we should proxy for the peer
  FALSE: otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean rmnet_v4_smi_arp_proxy
(
  lan_lle_enum_type  instance,
  uint32             src_ip,
  uint32             target_ip
)
{
 ps_iface_type *        um_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // TODO - check if host will send us data if we don't proxy arp for the 
  //        network.  Are routers supposed to respond to ARP requests for 
  //        others in general?

  /*-------------------------------------------------------------------------
  HBA>>
  The following logic assumes that the Rm PS_IFACE IP address is not the same
  as TE2, i.e, is not the same as the one allocated by the network to the Um
  PS IFACE.
  
  In this case, we proxy for all ARP request comming on the Rm interface 
  except when the peer is tryig to detect address duplication.
  
  if target_ip == peer ip address we do not proxy
  else we proxy.
  -------------------------------------------------------------------------*/

  rex_enter_crit_sect(&lan_llc_inst_to_um_iface_ptr_crit_sec);
  um_iface_ptr = lan_llc_inst_to_um_iface_ptr_lut[instance];
  rex_leave_crit_sect(&lan_llc_inst_to_um_iface_ptr_crit_sec);

  if (um_iface_ptr == NULL)
  {
    LOG_MSG_INFO1_0("UmIface ptr is NULL. Discarding proxy ARP response ");
    return FALSE;
  }
  
  if( target_ip == PS_IFACE_IP_V4_ADDR(um_iface_ptr) || src_ip == 0 ) 
  {
    LOG_MSG_INFO2_0("Peer performing duplicate address detection");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    In any other case, we should proxy
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_1 ("Send proxy ARP response for target IP (%x)",
                   ps_ntohl(target_ip));
  return TRUE;
} /* rmnet_v4_smi_arp_proxy() */


/*===========================================================================
  FUNCTION RMNET_V4_SMI_TRANSITION_STATE()

  DESCRIPTION
    This function performs state transition actions.

  PARAMETERS
    new_state: the state to transition to

  RETURN VALUE
    None

  DEPENDENCIES
    Its the caller's responsibility that it initiates only valid state 
    transitions. This function does not check if a state transition is 
    valid or not.

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v4_smi_transition_state
(
  rmnet_smi_info_type     * info,
  rmnet_v4_smi_state_type   new_state
)
{
  boolean                  retval;
  dhcp_arp_cache_updt_type arp_cache_update_cb;

  ps_ip_addr_type pri_dns_addr;  /* temp storage for primary DNS address */
  ps_ip_addr_type sec_dns_addr;  /* temp storage for secondary DNS addr  */
  ps_ip_addr_type *pri_dns_ptr, *sec_dns_ptr;

  ip_addr_type    pri_dns_ip_addr;
  ip_addr_type    sec_dns_ip_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  LOG_MSG_INFO2_2( "RMNET_V4_SM state goes %d to %d",
                   info->v4_sm.state, new_state );
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "RMNET_V4_SM state goes '%s' to '%s'\n",
                rmnet_v4_smi_state_names[info->v4_sm.state],
                rmnet_v4_smi_state_names[new_state]);
#endif

  ds_qmi_cflog_v4_sm_state_change( (uint32) info->v4_sm.state, 
                                   (uint32) new_state );

  info->v4_sm.state = new_state;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( new_state )
  {
    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_V4_SM_NULL_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_V4_SM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
        Cancel the IPv4 DHCP guard timer.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0 ("Cancel DHCP guard timer");

      (void) timer_clr( &(info->v4_sm.guard_timer), T_NONE );

      if(info->v4_sm.dhcp_handle != NULL)
      {
        /*-------------------------------------------------------------------
          Terminate associated DHCP instance
        -------------------------------------------------------------------*/
        (void) dhcp_stop (&(info->v4_sm.dhcp_handle), 
                          rmnet_v4_smi_dhcp_stopped_cb, 
                          info);
        (void) arp_flush(info->constants.lan_llc_instance);
      }

      if (info->um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Deregister all the Um interface event callbacks
        -------------------------------------------------------------------*/
        ps_iface_event_cback_dereg( info->um_iface_ptr,
                                    IFACE_ADDR_CHANGED_EV,
                                    info->v4_sm.um_addr_changed_buf_ptr );
      }

      /*-------------------------------------------------------------------------
        Reset the sub state machine type.
      -------------------------------------------------------------------------*/
      info->sub_sm_type = RMNET_INVALID_SM;

      break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          RMNET_V4_SM_DHCP_WAITING_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_V4_SM_DHCP_WAITING_STATE:
    {
      /*---------------------------------------------------------------------
        Start DHCP.  Dhcp will assign the IP address obtained on the Um 
        to the peer on the Rm interface and assign a private address to 
        the local side of Rm interface.  DHCP will also provide the DNS 
        addresses obtained during Um PPP to the Rm peer.
      ---------------------------------------------------------------------*/
      if (RMNET_LINK_PROT(info) == RMNET_ETHERNET_MODE) 
      {
        arp_cache_update_cb = arp_ipv4_cache_update;
      }
      else
      {
        arp_cache_update_cb = NULL;
      }

      /*---------------------------------------------------------------------
        RM is waiting for DHCP to start which means PPP has already negotiated
        credentials with the network.  If the 'over-ride' values are set for
        DNS, set these values via PS_IFACE now so that they will be used instead
        of the PPP values.
      ---------------------------------------------------------------------*/
      pri_dns_ptr = sec_dns_ptr = NULL;
      
      /* Get manual IPv4 DNS settings */
      memset(&pri_dns_ip_addr, 0, sizeof(ip_addr_type));
      memset(&sec_dns_ip_addr, 0, sizeof(ip_addr_type));
      memset(&pri_dns_addr, 0, sizeof(ip_addr_type));
      memset(&sec_dns_addr, 0, sizeof(ip_addr_type));
      
      rmnet_meta_sm_get_ipv4_manual_dns_addrs(
                                             RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                                             &pri_dns_ip_addr, 
                                             &sec_dns_ip_addr);
      
      /* if the pri is set (type==IPV4) and non-zero, save ptr for later */
      if ((IPV4_ADDR == pri_dns_ip_addr.type) && 
          (0 != pri_dns_ip_addr.addr.v4))
      {
        pri_dns_addr.type              = pri_dns_ip_addr.type;
        pri_dns_addr.addr.v4.ps_s_addr = ps_htonl(pri_dns_ip_addr.addr.v4);
        pri_dns_ptr = &pri_dns_addr;
      }
      /* if the sec is set (type==IPV4) and non-zero, save ptr for later */
      if ((IPV4_ADDR == sec_dns_ip_addr.type) && 
          (0 != sec_dns_ip_addr.addr.v4))
      {
        sec_dns_addr.type              = sec_dns_ip_addr.type;
        sec_dns_addr.addr.v4.ps_s_addr = ps_htonl(sec_dns_ip_addr.addr.v4);
        sec_dns_ptr = &sec_dns_addr;
      }

      /* if either DNS address is set, read the current values from ps_iface
         use the current values for either PRI/SEC if they are inactive */
      if ((NULL != pri_dns_ptr) || (NULL != sec_dns_ptr))
      {
        memset(&pri_dns_ip_addr, 0, sizeof(ip_addr_type));
        memset(&sec_dns_ip_addr, 0, sizeof(ip_addr_type));
        
        LOG_MSG_INFO2_0("Reading current DNS addresses");
        ps_iface_get_v4_dns_addrs( info->um_iface_ptr, 
                                   &pri_dns_ip_addr, 
                                   &sec_dns_ip_addr );
        if (NULL == pri_dns_ptr)
        {
          LOG_MSG_INFO2_0("Using ps_iface pri DNS address");
          pri_dns_addr.type              = pri_dns_ip_addr.type;
          pri_dns_addr.addr.v4.ps_s_addr = pri_dns_ip_addr.addr.v4;
          pri_dns_ptr = &pri_dns_addr;
        }
        if (NULL == sec_dns_ptr)
        {
          LOG_MSG_INFO2_0("Using ps_iface sec DNS address");
          sec_dns_addr.type              = sec_dns_ip_addr.type;
          sec_dns_addr.addr.v4.ps_s_addr = sec_dns_ip_addr.addr.v4;
          sec_dns_ptr = &sec_dns_addr;
        }

        LOG_MSG_INFO2_2("Setting IPv4 pri (%x) and sec (%x) DNS server addrs",
                        pri_dns_addr.addr.v4.ps_s_addr,
                        sec_dns_addr.addr.v4.ps_s_addr);
        if( -1 == ps_iface_set_v4_dns_addrs( info->um_iface_ptr,
                                             pri_dns_ptr,
                                             sec_dns_ptr ) )
        {
          LOG_MSG_INFO1_2("Couldn't set pri (%x) and sec (%x) DNS server addrs!",
                          pri_dns_addr.addr.v4.ps_s_addr,
                          sec_dns_addr.addr.v4.ps_s_addr);
        }
      }

      retval = dhcp_start( info->rmnet_iface_ptr,
                           info->um_iface_ptr,
                           rmnet_v4_smi_dhcp_started_cb,
                           rmnet_v4_smi_dhcp_msg_cback,
                           arp_cache_update_cb,
                           info );
      if (TRUE != retval)
      {
        LOG_MSG_ERROR_0("Starting DHCP failed.");
      }
      /*---------------------------------------------------------------------
        Wait now for dhcp_started_cb to be invoked
      ---------------------------------------------------------------------*/
      break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_V4_SM_DHCP_IN_PROGRESS_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_V4_SM_DHCP_IN_PROGRESS_STATE:
    {
      /*---------------------------------------------------------------------
        HBA>>
        Before bring iface, set up properly net info of bridge interface: 
        currently the default gateway it set to the address assign to us by
        UM which is supposed to be assigned to TE2 by DHCP server
        // reexamine 
       --------------------------------------------------------------------*/
       info->rmnet_iface_ptr->v4_net_info.gateway.ps_s_addr = 
         PS_IFACE_IP_V4_ADDR(info->um_iface_ptr);

      /*---------------------------------------------------------------------
        Set Rm iface to CONFIGURING state.
      ---------------------------------------------------------------------*/
      (void) ps_iface_configuring_ind(info->rmnet_iface_ptr);

      /*---------------------------------------------------------------------
        Start IPv4 DHCP guard timer.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0("Start DHCP guard timer");
      timer_set( &(info->v4_sm.guard_timer), 
                 RMNET_DHCP_MAX_SETUP_TIME, 
                 0, 
                 T_SEC );

#ifdef FEATURE_DATA_NO_QMI_TEST
      /*-------------------------------------------------------------------------
        NO QMI DATA CALL BRINGUP TEST
      -------------------------------------------------------------------------*/
      rmnet_v4_sm_clear_guard_timer(RMNET_V4_SM_INFO_TO_INSTANCE(info));
#endif /* FEATURE_DATA_NO_QMI_TEST */

      break;

    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE:
    {
      /*---------------------------------------------------------------------
        Cancel the IPv4 DHCP guard timer.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0 ("Cancel DHCP guard timer"); 

      (void) timer_clr( &(info->v4_sm.guard_timer), T_NONE );
      
      rmnet_meta_sm_post_event(RMNET_V4_SM_INFO_TO_INSTANCE(info), 
                               RMNET_META_SM_ADDR_CFG_COMPLETE_EV);
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Invalid state %d", new_state);
      ASSERT(0);
    }
  } /* switch(new state) */
} /* rmnet_v4_smi_transition_state() */
