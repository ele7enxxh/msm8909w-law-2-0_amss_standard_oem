/*===========================================================================

                       D S _ R M N E T _ V 6 _ S M . C
DESCRIPTION

  The Data Services Rm Network interface State Machine source file.

  This state machine manages the Rm network interface and coordinates 
  wireless WAN connectivity between the Rm and Um interfaces over all 
  supported radio technologies.

EXTERNALIZED FUNCTIONS

  rmnet_v6_sm_init()
    Allocate and initialize all data required for the IPv6 RmNet State Machine.

  rmnet_v6_sm_post_event()
    Post an event to RmNet State Machine.

  rmnet_v6_sm_process_ev_cmd()  
    Process a cmd posted to Rm State Machine.

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/src/ds_rmnet_v6_sm.c#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
09/27/13    wc     Support dynamic port configuration
06/18/13    wc     Fix DHCP6 restart on prefix change event
10/05/12    svj    Stop DHCP6 server when adding a new prefix before starting 
                   DCHP6 server again  
09/12/12    sb     Upon UM IPv6 prefix change in Softap mode, just update 
                   the prefix and do not do anything else 
08/03/12    sn     RmNet should be moved to up state on runtime settings query
                   in IP mode only.
12/09/11    rt     Added the support for Stateless DHCPv6 server.
10/04/11    sa     Q6 free floating changes for QMI.
09/19/11    wc     Clear v6 guard timer and save the addr in UM DB if queried.
03/09/11    kk     Removing DCC command buffer dependency from RmNet.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
04/01/08    mct    Added support for IP Mode.
10/01/07    mct    Created module.
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

#ifdef FEATURE_DATA_PS_IPV6

#include "ps_iface_defs.h"
#include "ps_iface_addr_v6.h"
#include "ps_iface.h"
#include "ps_ifacei_event.h"
#include "ps_iface_ipfltr.h"
#include "ps_lan_llc.h"

#include "dcc_task_defs.h"
#include "dcc_task_svc.h"
#include "ds_rmnet_defs.h"
#include "ds_rmnet_smi.h"
#include "ds_rmnet_sm_ext.h"

#include "ps_icmp6_nd.h"

#include "ds_qmi_cflog.h"
#include "ds_Utils_DebugMsg.h"

#include "stdlib.h"
#include "dhcp6_sl_server_mgr.h"

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  RMNET_SM state management and event processing functions
---------------------------------------------------------------------------*/
#define RMNET_V6_SM_INFO_TO_INSTANCE(info) RMNET_META_SM_INFO_TO_INSTANCE(info)

/*---------------------------------------------------------------------------
  Max setup time (in seconds) allowed for the Rm to perform IPv6 Stateless 
  Address Autoconfiguration before the call is torn down.
---------------------------------------------------------------------------*/
#define RMNET_V6_SM_MAX_SETUP_TIME 120


/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  RMNET_SM state management and event processing functions
---------------------------------------------------------------------------*/

static void rmnet_v6_smi_transition_state
(
  rmnet_smi_info_type    *  info,
  rmnet_v6_smi_state_type   new_state            /* state to transition to */
);

static void rmnet_v6_smi_handle_stop_addr_cfg_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v6_smi_handle_start_addr_cfg_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v6_smi_handle_ra_sent_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v6_smi_handle_ll_addr_resol_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v6_smi_handle_um_prefix_added_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_v6_smi_handle_guard_timer_expiration_ev
(
  rmnet_smi_info_type *  info
);

/*---------------------------------------------------------------------------
  RMNET_SM callback functions registered with various external modules
---------------------------------------------------------------------------*/

static void rmnet_v6_smi_timer_cback
(
  timer_cb_data_type  user_data
);

static void rmnet_v6_smi_um_iface_ev_hdlr_cb
(
  ps_iface_type *this_iface_ptr,           /* Pointer to the interface     */
  ps_iface_event_enum_type   event,        /* Event type occurred          */
  ps_iface_event_info_u_type event_info,   /* Event specific info block    */
  void *user_data_ptr           /* Data passed during event registration   */
);

void rmnet_v6_smi_nd_ev_hdlr_cb
(
  uint8                        event,
  void                        *user_data_ptr
);

static void rmnet_v6_smi_dhcp6_started_cb
(
  void * userdata, 
  void * handle
);

static void rmnet_v6_smi_dhcp6_stopped_cb
(
  void *   userdata, 
  boolean  ok
);

static void rmnet_v6_smi_handle_dhcp6_start_or_fail_hdlr
(
  rmnet_smi_info_type *  info,
  void * handle
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
static char *rmnet_v6_smi_state_names[RMNET_V6_SM_MAX_STATE] =
{
  "Null state",
  "Waiting for DHCP6 server init",
  "IPv6 address configuration in progress",
  "IPv6 address autoconfig completed",
  "Waiting for LL address resolution"
};

static char *rmnet_v6_smi_event_names[RMNET_V6_SM_MAX_EV] =
{
  "Start address configuration",
  "Guard timer expired",
  "Rm config completed",
  "Um prefix added",
  "Stop address configuration",
  "LL Address Resolved"
};
#endif /* T_ARM */

/*---------------------------------------------------------------------------
  RMNET RMSM command handler array
---------------------------------------------------------------------------*/
typedef void (*rmnet_v6_smi_cmd_hdlr_type)( rmnet_smi_info_type *);

static rmnet_v6_smi_cmd_hdlr_type rmnet_v6_smi_cmd_hdlr[RMNET_V6_SM_MAX_EV] =
{
  rmnet_v6_smi_handle_start_addr_cfg_ev,                        /* event 0 */
  rmnet_v6_smi_handle_guard_timer_expiration_ev,                /* event 1 */
  rmnet_v6_smi_handle_ra_sent_ev,                               /* event 2 */
  rmnet_v6_smi_handle_um_prefix_added_ev,                       /* event 3 */
  rmnet_v6_smi_handle_stop_addr_cfg_ev,                         /* event 4 */
  rmnet_v6_smi_handle_ll_addr_resol_ev                          /* event 5 */
};


/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
  FUNCTION RMNET_V6_SM_INIT()

  DESCRIPTION
    This function initializes the IPv6 state machine variables.

  PARAMETERS
    instance: The RmNet instance

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v6_sm_init
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type * info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(instance < RMNET_INSTANCE_MAX);
  info = rmnet_smi_info[instance];
  ASSERT(info);

  timer_def( &(info->v6_sm.guard_timer),
             NULL,
             NULL,
             0,
             rmnet_v6_smi_timer_cback,
             (timer_cb_data_type)info );

  info->v6_sm.um_prefix_update_buf_ptr = ps_iface_alloc_event_cback_buf(
                                          rmnet_v6_smi_um_iface_ev_hdlr_cb,
                                          (void *)info
                                        );

  if(info->v6_sm.um_prefix_update_buf_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Event buf allocation failed");
    ASSERT(0);
    return;
  }
  info->v6_sm.ll_addr_resol_ev_recd = FALSE;
  info->v6_sm.inited = TRUE;
} /* rmnet_v6_sm_init() */

/*===========================================================================
  FUNCTION RMNET_V6_SM_INIT_CONFIG()

  DESCRIPTION
    This function initializes the IPv6 specific (Rm iface and LLC) parameters.

  PARAMETERS
    start_info: The LLC start information to populate.

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v6_sm_init_config
(
  rmnet_smi_info_type     * info,
  lan_llc_start_info_type * start_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Setup IPv6 specific LLC parameters.
  -------------------------------------------------------------------------*/
  start_info->nd_config.nd_cback_f_ptr = rmnet_v6_smi_nd_ev_hdlr_cb;
  start_info->nd_config.usr_data_ptr   = (void*)info;

  if( -1 == ps_iface_set_addr_family(info->rmnet_iface_ptr,
                                     IFACE_IPV6_ADDR_FAMILY))
  {
    LOG_MSG_ERROR_0("Failed to set Rm addr family to IPv6!");
    rmnet_meta_sm_post_event(RMNET_V6_SM_INFO_TO_INSTANCE(info), 
                             RMNET_META_SM_ADDR_CFG_FAILURE_EV);
    return;
  }

  if (RMNET_LINK_PROT(info) == RMNET_IP_MODE)
  {
    (void) ps_icmp6_nd_start(info->constants.lan_llc_instance,
                             &start_info->nd_config,
                             info->rmnet_iface_ptr,
                             NULL);
  }

  info->sub_sm_type = RMNET_V6_SM;

} /* rmnet_v6_sm_init_config() */


/*===========================================================================
  FUNCTION RMNET_V6_SM_POST_EVENT()

  DESCRIPTION
    This function posts an event to RMNet IPv6 SM and enqueues the
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
void rmnet_v6_sm_post_event
(
  rmnet_instance_e_type    instance,
  rmnet_v6_sm_event_type   event
)
{
  rmnet_smi_ev_cmd_type * cmd_ptr;
  rmnet_smi_info_type   * info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(instance < RMNET_INSTANCE_MAX);
  
  info = rmnet_smi_info[instance];
  ASSERT(info);

  if (event >= RMNET_V6_SM_MAX_EV)
  {
    ASSERT (0);
    LOG_MSG_ERROR_1 ("Invalid RMNET_V6_SM event %d, ignored", event);
    return;
  }

#ifdef T_ARM
  LOG_MSG_INFO1_2( "Event %d posted to RMNET_V6_SM in state %d",
                   event, info->v6_sm.state );
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "'%s' event posted to RMNET_V6_SM in state %d.\n",
                rmnet_v6_smi_event_names[event], info->v6_sm.state);
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
  cmd_ptr->sm_id             = RMNET_V6_SM;
  cmd_ptr->event             = event;
  cmd_ptr->rmnet_sm_info_ptr = (void *) info;

  dcc_send_cmd_ex(DCC_RMNET_SM_EV_CMD, (void *)cmd_ptr );

} /* rmnet_sm_post_event() */

/*===========================================================================
  FUNCTION RMNET_V6_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet IPv6 SM event cmd.

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
void rmnet_v6_sm_process_ev_cmd
(
  dcc_cmd_enum_type         cmd,
  void * user_data_ptr
)
{
  rmnet_smi_info_type     * info;
  rmnet_v6_sm_event_type    event;
  rmnet_smi_ev_cmd_type   * cmd_ptr;
  boolean                   is_err = FALSE;
  typedef enum{
  	ERR_NONE = 0,
  	ERR_INIT = 1,
	ERR_EVENT_V6_MAX = 2,
	ERR_V6_SM_MAX_STATE = 3,
  	}err_type;
  err_type err = ERR_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = (rmnet_smi_ev_cmd_type *)user_data_ptr;
  ASSERT (cmd_ptr && (cmd == DCC_RMNET_SM_EV_CMD) && (cmd_ptr->sm_id == RMNET_V6_SM));

  event = (rmnet_v6_sm_event_type) cmd_ptr->event;
  info  = (rmnet_smi_info_type *) cmd_ptr->rmnet_sm_info_ptr;
  
  LOG_MSG_INFO2_3( "RmNet V6 SM event %d in state %d, info %x",
                   event, info->v6_sm.state, info );

  /*-------------------------------------------------------------------------
    Verify that RMNET_SM is initialized
  -------------------------------------------------------------------------*/
  do
  {
    if(info->v6_sm.inited != TRUE)
    {
      is_err = TRUE;
      err = ERR_INIT;
      break;
    }
  
    /*-------------------------------------------------------------------------
      Validate posted event before processing
    -------------------------------------------------------------------------*/
    if (event >= RMNET_V6_SM_MAX_EV)
    {
      is_err = TRUE;
      err = ERR_EVENT_V6_MAX;
      break;
    }
  
    /*-------------------------------------------------------------------------
      Validate RMNET_SM state before processing event - this allows event handler
      functions to assume that state is valid.
    -------------------------------------------------------------------------*/
    if (info->v6_sm.state >= RMNET_V6_SM_MAX_STATE)
    {
      is_err = TRUE;
      err = ERR_V6_SM_MAX_STATE;
      break;
    }
  
    if ( RMNET_V6_SM != info->sub_sm_type )
    {
      MSG_ERROR("Invalid SUB SM type %d, ignoring event %d!", info->sub_sm_type, event, 0);
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
  (*(rmnet_v6_smi_cmd_hdlr[event]))(info);

} /* rmnet_v6_sm_process_ev_cmd() */


/*===========================================================================
  FUNCTION RMNET_V6_SM_PROCESS_CMD()

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
void rmnet_v6_sm_process_cmd
(
  dcc_cmd_enum_type         cmd,
  void * user_data_ptr
)
{
  rmnet_smi_cmd_type * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr = (rmnet_smi_cmd_type *)user_data_ptr;
  ASSERT (cmd_ptr && (cmd == DCC_RMNET_SM_CMD) && (cmd_ptr->sm_id == RMNET_V6_SM));

  switch(cmd_ptr->cmd_type)
  {
    case RMNET_SM_CMD_DHCP_START_OR_FAIL:
      rmnet_v6_smi_handle_dhcp6_start_or_fail_hdlr( cmd_ptr->info_ptr,
                                    cmd_ptr->data.dhcp_started.handle );
      break;
    default:
      LOG_MSG_INFO1_1("Received Unhandled rmnet_v6_sm cmd %d",
                      cmd_ptr->cmd_type);
      ASSERT(0);
      break;
  }
} /* rmnet_v6_sm_process_cmd() */


/*===========================================================================
FUNCTION RMNET_V6_SM_CLEAR_GUARD_TIMER

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
void rmnet_v6_sm_clear_guard_timer
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
  if (RMNET_LINK_PROT(info) != RMNET_IP_MODE)
  {
    LOG_MSG_INFO2_1 ("Rmnet Instance %d not in IP mode, ignoring Runtime "
                     "settings request to clear guard timer", instance);
    return;
  }
  
  switch(info->v6_sm.state)
  {
    case RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE:
    {
      /*---------------------------------------------------------------------
        Host is configuring IP address using runtime settings
        -- cancel guard timer and transition state
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0 ("Runtime settings queried, moving to UP state");
      (void) timer_clr( &(info->v6_sm.guard_timer), T_NONE );
      rmnet_v6_smi_transition_state(info, 
                                    RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE);
      break;
    }

    default:
      LOG_MSG_INFO2_1("Do nothing - clear guard timer called in V6_SM state %d",
                      info->v6_sm.state);
  }
} /* rmnet_v6_sm_clear_guard_timer */


/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_V6_SMI_TIMER_CBACK()

  DESCRIPTION
    IPv6 guard timer callback

  PARAMETERS
    user_data -> rmnet_smi_info

  RETURN VALUE
    None

  DEPENDENCIES
    rmnet_sio_init() must have been called previously

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v6_smi_timer_cback
(
  timer_cb_data_type  user_data
)
{
  rmnet_smi_info_type *  info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (user_data);
  info = (rmnet_smi_info_type *) user_data;

  LOG_MSG_INFO1_0 ("IPv6 Guard timer expired! Cleanup in DS task");
  rmnet_v6_sm_post_event(RMNET_V6_SM_INFO_TO_INSTANCE(info), 
                         RMNET_V6_SM_GUARD_TIMER_EXPIRATION_EV);

} /* rmnet_v6_smi_timer_cback() */


/*===========================================================================
  FUNCTION RMNET_V6_SMI_HANDLE_START_ADDR_CFG_EV()

  DESCRIPTION
    This function processes RMNET_V6_SM_START_ADDR_CFG_EV event. 

    This event indicates that the IPv6 address configuration state machine
    should begin IPv6 stateless address configuration on the Rm.

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
static void rmnet_v6_smi_handle_start_addr_cfg_ev
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(info->sub_sm_type == RMNET_V6_SM);

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd START_ADDR_CFG_EV, state %d", info->v6_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd START_ADDR_CFG_EV, state '%s'\n",
                rmnet_v6_smi_state_names[info->v6_sm.state]);
#endif

  switch( info->v6_sm.state )
  {
    case RMNET_V6_SM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
        Register for address change events. 
      ---------------------------------------------------------------------*/
      if(info->um_iface_ptr)
      {
        if ( ps_iface_event_cback_reg(info->um_iface_ptr,
                                      IFACE_PREFIX_UPDATE_EV,
                                      info->v6_sm.um_prefix_update_buf_ptr
                                     ) < 0 )
      {
        rmnet_meta_sm_post_event(RMNET_V6_SM_INFO_TO_INSTANCE(info), 
                                 RMNET_META_SM_ADDR_CFG_FAILURE_EV);
          LOG_MSG_ERROR_0("IFACE prefix update event reg failed!");
          return;
        }
      }
      else
      {
        rmnet_meta_sm_post_event(RMNET_V6_SM_INFO_TO_INSTANCE(info), 
                                   RMNET_META_SM_ADDR_CFG_FAILURE_EV);          
        LOG_MSG_ERROR_0("Um IFACE is NULL!");
        return;
      }

      rmnet_v6_smi_transition_state(info, 
                                    RMNET_V6_SM_DHCP6_WAITING_STATE);
      break;
    }

    case RMNET_V6_SM_DHCP6_WAITING_STATE:
    case RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE:
    case RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE:
    case RMNET_V6_SM_WAIT_FOR_LL_ADDR_RESOL_STATE:
      /* fall through */

    default:
    {
      LOG_MSG_ERROR_1("Got START_ADDR_CFG_EV in unexpected state %d",
                      info->v6_sm.state);
      break;
    }
  } /* switch( info->v6_sm.state ) */
} /* rmnet_v6_smi_handle_start_addr_cfg_ev() */


/*===========================================================================
  FUNCTION RMNET_V6_SMI_HANDLE_STOP_ADDR_CFG_EV()

  DESCRIPTION
    This function processes RMNET_V6_SM_STOP_ADDR_CFG_EV event. 

    This event indicates that the IPv6 address configuration state machine
    should halt and cleanup its resources.

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
static void rmnet_v6_smi_handle_stop_addr_cfg_ev
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
  LOG_MSG_INFO2_1( "Recvd STOP_ADDR_CFG_EV, state %d", info->v6_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd STOP_ADDR_CFG_EV, state '%s'\n",
                rmnet_v6_smi_state_names[info->v6_sm.state]);
#endif

  switch( info->v6_sm.state )
  {
    case RMNET_V6_SM_NULL_STATE:
      /*-------------------------------------------------------------------------
        Reset the sub state machine type.
      -------------------------------------------------------------------------*/
      info->sub_sm_type = RMNET_INVALID_SM;
      break;

    case RMNET_V6_SM_DHCP6_WAITING_STATE:
    case RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE:
    case RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE:
    case RMNET_V6_SM_WAIT_FOR_LL_ADDR_RESOL_STATE:
    {
      rmnet_v6_smi_transition_state (info, RMNET_V6_SM_NULL_STATE);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNet V6 SM state!");
      break;
    }
  } /* switch( info->v6_sm.state ) */
} /* rmnet_v6_smi_handle_stop_addr_cfg_ev() */


/*===========================================================================
  FUNCTION RMNET_V6_SMI_HANDLE_RA_SENT_EV

  DESCRIPTION
    This function processes the RA_SENT_EV which indicates the Rm has 
    successfully completed IPv6 address autoconfiguration successfully.
    
    This will cause the state to transition to WAITING_FOR_LL_ADDR_RESOL_STATE
    /ADDR_CONFIG_COMPLETE.
    
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
static void rmnet_v6_smi_handle_ra_sent_ev
(
  rmnet_smi_info_type *  info
)
{ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(info->sub_sm_type == RMNET_V6_SM);

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd RA_SENT_EV, state %d", info->v6_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd RA_SENT_EV, state '%s'\n",
                rmnet_v6_smi_state_names[info->v6_sm.state]);
#endif

  switch( info->v6_sm.state )
  {
    case RMNET_V6_SM_NULL_STATE:
    case RMNET_V6_SM_DHCP6_WAITING_STATE:
    case RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE:
    case RMNET_V6_SM_WAIT_FOR_LL_ADDR_RESOL_STATE:
      LOG_MSG_INFO2_1( "RA Sent Event not expected in state %d",
                       info->v6_sm.state );
      break;

    case RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE:
    {
      if ((RMNET_LINK_PROT(info) == RMNET_ETHERNET_MODE) &&
         (info->v6_sm.ll_addr_resol_ev_recd == FALSE))
      {
        rmnet_v6_smi_transition_state(info, RMNET_V6_SM_WAIT_FOR_LL_ADDR_RESOL_STATE);
      }
      else
      {
        rmnet_v6_smi_transition_state(info, RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE);
      }
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNet V6 SM state!");
      break;
    }
  } /* switch( info->v6_sm.state ) */
} /* rmnet_v6_smi_handle_ra_sent_ev() */

/*===========================================================================
  FUNCTION RMNET_V6_SMI_HANDLE_LL_ADDR_RESOL_EV

  DESCRIPTION
    This function processes the LL_ADDR_RESOL_EV which indicates the UE now 
    has the LL address of the tethered devide after successful IPv6 address 
    autoconfiguration. This will cause the state to transition to 
    ADDR_CONFIG_COMPLETE.
    
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
static void rmnet_v6_smi_handle_ll_addr_resol_ev
(
  rmnet_smi_info_type *  info
)
{ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(info->sub_sm_type == RMNET_V6_SM);

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd LL_ADDR_RESOL_EV, state %d", info->v6_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd LL_ADDR_RESOL_EV, state '%s'\n",
                rmnet_v6_smi_state_names[info->v6_sm.state]);
#endif

  switch( info->v6_sm.state )
  {
    case RMNET_V6_SM_NULL_STATE:
    case RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE:
    case RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE:
    case RMNET_V6_SM_DHCP6_WAITING_STATE:
      break;

    case RMNET_V6_SM_WAIT_FOR_LL_ADDR_RESOL_STATE:
    {
      rmnet_v6_smi_transition_state(info, RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNet V6 SM state!");
      break;
    }
  } /* switch( info->v6_sm.state ) */
} /* rmnet_v6_smi_handle_ll_addr_resol_ev() */

/*===========================================================================
  FUNCTION RMNET_V6_SMI_HANDLE_UM_PREFIX_ADDED_EV

  DESCRIPTION
    This function processes UM_PREFIX_ADDED_EV event. 

    This event indicates that a new prefix was added. This will cause the 
    Rm RX queues to be flushed and autoconfiguration of the Rm to commence.

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
static void rmnet_v6_smi_handle_um_prefix_added_ev
(
  rmnet_smi_info_type *  info
)
{ 
  ps_ip_addr_type          ip_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(info);

  if( RMNET_V6_SM != info->sub_sm_type )
  {
    MSG_ERROR("UM_PREFIX_ADDED_EV being processed when sub_meta_sm is not RMNET_V6_SM, ignoring", 0, 0, 0);
    return;
  }

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd UM_PREFIX_ADDED_EV, state %d", info->v6_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd UM_PREFIX_ADDED_EV, state '%s'\n",
                rmnet_v6_smi_state_names[info->v6_sm.state]);
#endif

  switch( info->v6_sm.state )
  {
    case RMNET_V6_SM_NULL_STATE:
    case RMNET_V6_SM_DHCP6_WAITING_STATE:
    case RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE:
      break;

    case RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE:
    case RMNET_V6_SM_WAIT_FOR_LL_ADDR_RESOL_STATE:
    {

      /*------------------------------------------------------------------------ 
       Perform icmp flush and post addr changed event only in non softap cases.
       We do not want these in softap mode because we send explicit unsolicited
       RA which performs the required action. Update the Rmnet prefix for Softap
       case.
      --------------------------------------------------------------------------*/
      if(info->meta_sm.softap_mode)
      {
        LOG_MSG_INFO1_0("Softap mode:update prefix only");
        ip_addr.type = IPV6_ADDR;
        ps_iface_get_addr(info->um_iface_ptr, &ip_addr);
        info->rmnet_iface_ptr->iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->prefix =
        ip_addr.addr.v6.ps_s6_addr64[0];
      }
      else
      {
        /*---------------------------------------------------------------------
         Stop the DHCP6 server before starting it again
        ---------------------------------------------------------------------*/
        if(info->v6_sm.dhcp6_handle)
        {
          dhcp6_sl_server_mgr_stop(&(info->v6_sm.dhcp6_handle),
                                 rmnet_v6_smi_dhcp6_stopped_cb,
                                 info);
        }
        ps_icmp6_nd_flush(info->constants.lan_llc_instance);
        rmnet_v6_smi_transition_state(info, RMNET_V6_SM_DHCP6_WAITING_STATE);

        rmnet_meta_sm_post_event(RMNET_V6_SM_INFO_TO_INSTANCE(info), 
                               RMNET_META_SM_ADDR_CFG_CHANGED_EV);
      }

      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNet V6 SM state!");
      break;
    }
  } /* switch( info->v6_sm.state ) */
} /* rmnet_v6_smi_handle_um_prefix_added_ev() */



/*===========================================================================
  FUNCTION RMNET_V6_SMI_HANDLE_GUARD_TIMER_EXPIRATION_EV

  DESCRIPTION
    This function processes V6_GUARD_TIMER_EXPIRATION_EV event. 

    This event indicates that the Rm did not succesfully solicit for the
    prefix. Tear down the call.

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
static void rmnet_v6_smi_handle_guard_timer_expiration_ev
(
  rmnet_smi_info_type *  info
)
{ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(info->sub_sm_type == RMNET_V6_SM);

#ifdef T_ARM
  LOG_MSG_INFO2_1( "Recvd V6_GUARD_TIMER_EXPIRATION_EV, state %d",
                   info->v6_sm.state );
#else
  MSG_SPRINTF_1(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd V6_GUARD_TIMER_EXPIRATION_EV, state '%s'\n",
                rmnet_v6_smi_state_names[info->v6_sm.state]);
#endif

  switch( info->v6_sm.state )
  {
    case RMNET_V6_SM_NULL_STATE:
    case RMNET_V6_SM_DHCP6_WAITING_STATE:
    case RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE:    
    case RMNET_V6_SM_WAIT_FOR_LL_ADDR_RESOL_STATE:
    {
      LOG_MSG_ERROR_1("Received V6 guard timer expiration in state %d",
                      info->v6_sm.state);
      ASSERT(0);
      break;
    }

    case RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE:
    {
      rmnet_meta_sm_post_event(RMNET_V6_SM_INFO_TO_INSTANCE(info), 
                               RMNET_META_SM_ADDR_CFG_FAILURE_EV);

      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNet V6 SM state!");
      break;
    }
  } /* switch( info->v6_sm.state ) */
} /* rmnet_v6_smi_handle_guard_timer_expiration_ev() */


/*===========================================================================
FUNCTION   RMNET_V6_SMI_ND_EV_HDLR_CB()

DESCRIPTION
  This callback function is called when a Neighbor Discovery event occurs.
  The IPv6 ND event is handled and a cmd posted to the IPv6 RmNet SM to
  handle it in DS task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rmnet_v6_smi_nd_ev_hdlr_cb
(
  uint8                        event,
  void                        *user_data_ptr
)
{
  rmnet_smi_info_type        *  info;
  ps_icmp6_nd_event_enum_type   nd_event;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (user_data_ptr);

  info     = (rmnet_smi_info_type *) user_data_ptr;
  nd_event = (ps_icmp6_nd_event_enum_type) event;

  LOG_MSG_INFO2_1( "Recvd ICMP6 ND ev %d", nd_event );

  switch( nd_event )
  {

    case ICMP6_ND_RA_SENT_EV:
    {
      switch( info->v6_sm.state )
      {
        case RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE:
        {
          LOG_MSG_INFO1_0( "Ra Sent, posting RMNET_V6_SM_RA_SENT_EV" );
          rmnet_v6_sm_post_event( RMNET_V6_SM_INFO_TO_INSTANCE(info),
                                  RMNET_V6_SM_RA_SENT_EV );
          break;
        }
    
        case RMNET_V6_SM_NULL_STATE:
        case RMNET_V6_SM_DHCP6_WAITING_STATE:
        case RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE:
          /* fall through */
        default:
        {

          LOG_MSG_INFO2_1("Ignoring RA_SENT_EV in state %d!",
                          info->v6_sm.state);
          break;
        }
      } /* switch( info->v6_sm.state ) */

      break;
    } /* nd_ra_sent_ev */

    case ICMP6_ND_LL_ADDR_RESOLVED_EV:
    {
      switch( info->v6_sm.state )
      {
        case RMNET_V6_SM_WAIT_FOR_LL_ADDR_RESOL_STATE:
        {
          LOG_MSG_INFO1_0( "LL ADDR Resolved, posting RMNET_V6_SM_LL_ADDR_RESOL_EV" );
          rmnet_v6_sm_post_event( RMNET_V6_SM_INFO_TO_INSTANCE(info),
                                  RMNET_V6_SM_LL_ADDR_RESOL_EV );
          break;
        }

        case RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE:
        {
          LOG_MSG_INFO1_0( "LL_ADDR_RESOL_EV recd in RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE" );
          info->v6_sm.ll_addr_resol_ev_recd = TRUE;
          break;
        }

        case RMNET_V6_SM_NULL_STATE:
        case RMNET_V6_SM_DHCP6_WAITING_STATE:
        case RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE:
          /* fall through */
        default:
        {

          LOG_MSG_INFO2_1("Ignoring LL_ADDR_RESOL_EV in state %d!",
                          info->v6_sm.state);
          break;
        }
      } /* switch( info->v6_sm.state ) */

      break;
    } /* nd_ll_addr_resolved_ev */

    default:
    {
      LOG_MSG_ERROR_1("Unregistered event %d recvd, ignoring", nd_event);
      break;
    }
  } /* switch( event ) */
} /* rmnet_v6_smi_nd_ev_hdlr_cb() */

/*===========================================================================
  FUNCTION RMNET_V6_SMI_DHCP6_STARTED_CB

  DESCRIPTION
    This is the dhcp6_server_mgr_start completion callback.

  PARAMETERS
    userdata:  the user data provided when dhcp6_server_mgr_start was called
    handle:    handle to the dhcp6 instance started

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v6_smi_dhcp6_started_cb
(
  void *  userdata, 
  void *  handle
)
{
  rmnet_smi_info_type *  info = (rmnet_smi_info_type *) userdata;
  rmnet_smi_cmd_type    * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0 ("DHCP6 server is ready");

  if(handle == NULL && info->sub_sm_type != RMNET_V6_SM)
  {
    LOG_MSG_INFO1_0("DHCP6 handle is NULL");
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
  cmd_ptr->sm_id        = RMNET_V6_SM;
  cmd_ptr->cmd_type     = RMNET_SM_CMD_DHCP_START_OR_FAIL;
  cmd_ptr->info_ptr     = (void *) info;
  cmd_ptr->data.dhcp_started.handle  = handle;
  dcc_send_cmd_ex(DCC_RMNET_SM_CMD, (void *)cmd_ptr );

} /* rmnet_v6_smi_dhcp6_started_cb() */


/*===========================================================================
  FUNCTION RMNET_V6_SMI_DHCP6_STOPPED_CB

  DESCRIPTION
    This is the dhcp6_server_mgr_stop completion callback.

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
static void rmnet_v6_smi_dhcp6_stopped_cb
(
  void *   userdata, 
  boolean  ok
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (userdata);

  LOG_MSG_INFO1_1 ("DHCP6 stopped callback (status = %d)", ok);

} /* rmnet_v6_smi_dhcp6_stopped_cb() */


/*===========================================================================
  FUNCTION RMNET_V6_SMI_HANDLE_DHCP6_START_OR_FAIL_HDLR

  DESCRIPTION
    This function handles the DHCP6 server start and fail event. 

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_v6_smi_handle_dhcp6_start_or_fail_hdlr
(
  rmnet_smi_info_type *  info,
  void * handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-------------------------------------------------------------------------
  Store the DHCP6 server handle and print the start/fail info.
-------------------------------------------------------------------------*/

  if(NULL == handle)
  {
    LOG_MSG_ERROR_0("Could not start the DHCP6 server");
    if (info->v6_sm.state == RMNET_V6_SM_DHCP6_WAITING_STATE)
    {
      info->v6_sm.dhcp6_handle = handle;
      rmnet_meta_sm_post_event(RMNET_V6_SM_INFO_TO_INSTANCE(info), 
                               RMNET_META_SM_ADDR_CFG_FAILURE_EV);
    }
    else
    {
      LOG_MSG_INFO1_1("Unexpected DHCP6 failure ev in state %s",
                      rmnet_v6_smi_state_names[info->v6_sm.state]);
    }
  }
  else
  {
    LOG_MSG_INFO1_0("DHCP6 server started successfully");
    if ( (info->v6_sm.state == RMNET_V6_SM_DHCP6_WAITING_STATE) &&
         (info->v6_sm.dhcp6_handle == NULL) )
    {
      info->v6_sm.dhcp6_handle = handle;
      PS_IFACE_SET_DHCP6_SERVER_HANDLE(info->rmnet_iface_ptr, handle);

      rmnet_v6_smi_transition_state(info,
                 RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE);
    }
    else
    {
      LOG_MSG_INFO1_1("Unexpected DHCP6 start ev in state %s, stop DHCP6",
                      rmnet_v6_smi_state_names[info->v6_sm.state]);

      dhcp6_sl_server_mgr_stop(&handle,
                             rmnet_v6_smi_dhcp6_stopped_cb,
                             info);
    }
  }

}/* rmnet_v6_smi_handle_dhcp6_start_or_fail_hdlr() */

/*===========================================================================
FUNCTION   RMNET_V6_SMI_IFACE_EV_HDLR_CB()
=============================================================================

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on Um interface.  Various Um iface events are registered based on
  call type during the call setup type.  Each iface event generates 
  appropriate rmnet_sm event .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rmnet_v6_smi_um_iface_ev_hdlr_cb
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
    case IFACE_PREFIX_UPDATE_EV:
    {
      /*---------------------------------------------------------------------
        Only handle ADDED events as that implies a handoff since we only
        support 1 prefix. When multiple prefix support is added this will
        be updated.
      ---------------------------------------------------------------------*/
      if(event_info.prefix_info.kind == PREFIX_ADDED)
      {
        LOG_MSG_INFO1_0( "Um IP addr changed, posting RMNET_V6_SM_PREFIX_UPDATE_EV" );
        rmnet_v6_sm_post_event( RMNET_V6_SM_INFO_TO_INSTANCE(info), 
                                RMNET_V6_SM_UM_PREFIX_ADDED_EV );
      }
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Unregistered event %d recvd, ignoring", event);
      break;
    }
  } /* switch( event ) */
} /* rmnet_v6_smi_um_iface_ev_hdlr_cb() */


/*===========================================================================
  FUNCTION RMNET_V6_SMI_TRANSITION_STATE()

  DESCRIPTION
    This function performs state transition actions for the RmNet IPv6 SM.

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
static void rmnet_v6_smi_transition_state
(
  rmnet_smi_info_type     * info,
  rmnet_v6_smi_state_type   new_state
)
{
  sint15                   ps_errno;
  ps_ip_addr_type          ip_addr;
  uint64                 * iid;
  boolean                  retval;
  int                      ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef T_ARM
  LOG_MSG_INFO2_2( "RMNET_V6_SM state goes %d to %d",
                   info->v6_sm.state, new_state );
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "RMNET_V6_SM state goes '%s' to '%s'\n",
                rmnet_v6_smi_state_names[info->v6_sm.state],
                rmnet_v6_smi_state_names[new_state]);
#endif

  ds_qmi_cflog_v6_sm_state_change( (uint32) info->v6_sm.state, 
                                   (uint32) new_state );

  info->v6_sm.state = new_state;
  ip_addr.type = IPV6_ADDR;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( new_state )
  {
    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_SM_NULL_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_V6_SM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
        Cancel the IPv6 guard timer.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0 ("Cancel IPv6 guard timer"); 

      (void) timer_clr( &(info->v6_sm.guard_timer), T_NONE );

      /*---------------------------------------------------------------------
        Stop the DHCP6 server.
      ---------------------------------------------------------------------*/
      if (info->v6_sm.dhcp6_handle != NULL)
      {
        if (TRUE != dhcp6_sl_server_mgr_stop(&(info->v6_sm.dhcp6_handle),
                                           rmnet_v6_smi_dhcp6_stopped_cb,
                                           info))
        {
          LOG_MSG_ERROR_0("Could not stop the DHCP6 server");
        }
      }
      else
      {
        LOG_MSG_INFO1_0("NULL DHCP6 handle");
      }

      PS_IFACE_SET_DHCP6_SERVER_HANDLE(info->rmnet_iface_ptr, NULL);

      if (RMNET_LINK_PROT(info) == RMNET_IP_MODE)
      {
        (void) ps_icmp6_nd_stop(info->constants.lan_llc_instance);
      }

      /*---------------------------------------------------------------------
        Delete the ICMPv6 filters on the Rm and Um.
      ---------------------------------------------------------------------*/

      (void) ps_iface_ipfltr_delete(info->rmnet_iface_ptr,
                                    IP_FLTR_CLIENT_SOCKETS,
                                    info->v6_sm.rm_fltr_handle,
                                    &ps_errno);

      if(info->um_iface_ptr)
      {
        ps_iface_event_cback_dereg(info->um_iface_ptr,
                                   IFACE_PREFIX_UPDATE_EV,
                                   info->v6_sm.um_prefix_update_buf_ptr);

      }
      /*-------------------------------------------------------------------------
        Reset the sub state machine type.
      -------------------------------------------------------------------------*/
      info->sub_sm_type = RMNET_INVALID_SM;
      info->v6_sm.ll_addr_resol_ev_recd = FALSE;
      break;
    }

    case RMNET_V6_SM_DHCP6_WAITING_STATE:
    {
      /*---------------------------------------------------------------------
        Set IP and gateway address. Hack before address based routing.
      ---------------------------------------------------------------------*/
      ps_iface_get_addr(info->um_iface_ptr, &ip_addr);
  
      info->rmnet_iface_ptr->iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->prefix =
        ip_addr.addr.v6.ps_s6_addr64[0];

      iid = &info->rmnet_iface_ptr->iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->iid;
      
      if (-1 == ps_iface_generate_ipv6_iid(info->um_iface_ptr, iid, &ps_errno))
      {
        LOG_MSG_ERROR_0("Failed to generate a random IPv6 IID");
      }

      info->rmnet_iface_ptr->v6_net_info.gateway_iid = 
        PS_IFACE_GET_IP_V6_IID(info->um_iface_ptr);


      ret = dhcp6_duid_ll_init(info->v6_sm.dhcp6_server_duid.server_duid,
                               DHCP6_MAX_DUID_LENGTH,
                               DHCP_HWTYPE_ETHERNET,
                               PS_IFACE_HW_ADDR_PTR(info->rmnet_iface_ptr),
                               PS_IFACE_MAX_HW_ADDR_LEN);

      if ( ret <= 0 )
      {
        LOG_MSG_ERROR_0 ("DHCPv6 server DUID initialisation failed"
                         "Cannot start the DHCPv6 server"); 
      }
      else
      {
        info->v6_sm.dhcp6_server_duid.server_duid_len = ret;
        /*---------------------------------------------------------------------
           Start the DHCP v6 server
        ---------------------------------------------------------------------*/
        retval = dhcp6_sl_server_mgr_start( info->rmnet_iface_ptr,
                                            info->um_iface_ptr,
                                            &info->v6_sm.dhcp6_server_duid,
                                            NULL,
                                            rmnet_v6_smi_dhcp6_started_cb,
                                            info );

        if (TRUE != retval)
        {
          LOG_MSG_ERROR_0 ("DHCPv6 server start failed"); 
        }
      }

      break;
    }

    case RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE:
    {
      /*---------------------------------------------------------------------
        Set Rm iface to CONFIGURING state.
      ---------------------------------------------------------------------*/
      (void) ps_iface_configuring_ind(info->rmnet_iface_ptr);

      /*---------------------------------------------------------------------
        Start IPv6 guard timer.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0("Start IPv6 guard timer");
      timer_set( &(info->v6_sm.guard_timer), 
                 RMNET_V6_SM_MAX_SETUP_TIME, 
                 0, 
                 T_SEC );

      break;
    }

    case RMNET_V6_SM_WAIT_FOR_LL_ADDR_RESOL_STATE:
    {
      LOG_MSG_INFO2_0("In WAIT_FOR_LL_ADDR_RESOL_STATE"); 

      (void) timer_clr( &(info->v6_sm.guard_timer), T_NONE );

      break;
    }

    case RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE:
    {
      /*---------------------------------------------------------------------
        Cancel the IPv6 guard timer.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0 ("Cancel IPv6 guard timer"); 

      (void) timer_clr( &(info->v6_sm.guard_timer), T_NONE );
      
      rmnet_meta_sm_post_event(RMNET_V6_SM_INFO_TO_INSTANCE(info), 
                               RMNET_META_SM_ADDR_CFG_COMPLETE_EV);
      break;
    }


     default:
    {
      LOG_MSG_ERROR_1("Invalid state %d", new_state);
      ASSERT(0);
    }
  } /* switch(new state) */
} /* rmnet_v6_smi_transition_state() */
#endif /* FEATURE_DATA_PS_IPV6 */
