/*===========================================================================

             D S  _ 3 G P P _ R M S M _ A U T O C O N F I G _ S M  . C

DESCRIPTION
  This file handles IPv6 autoconfiguration and DHCPv6 on the Rm link.
  This file is used as a Sub-State Machine by RmSm IP to handle IPv6
  related functionality.
  
EXTERNALIZED FUNCTIONS
Copyright (c) 2002 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/src/ds_3gpp_rmsm_autoconfig_sm.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
03/25/15    skc    Provided new API to create unique address
04/09/14    ash    Added Prefix Delegation support.
02/21/12    vs     Code reviews comments for Ipv6 over PPP + Some cleanup
02/08/12    vs     Added support for 3GPP IPv6 over PPP 
 
============================================================================*/

#include "ps_lan_llc.h"
#include "dsumts_rmsm_ip.h"
#include "ps_in.h"
#include "ds_3gpp_rmsm_autoconfig_sm.h"
#include "ps_icmp6_nd.h"
#include "ds_3gppi_utils.h"
#include "ps_ipfltr_defs.h"
#include "ps_iface_ipfltr.h"
#include "ps_in.h"
#include "ps_iface_defs.h"
#include "ps_iface.h"
#include "ps_ifacei_addr_mgmt.h"
#include "dhcp6_sl_server_mgr.h"

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DECLARATIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

typedef struct
{
  uint32  event;
  void   *sm_ptr;
}ds_3gpp_rmsm_autoconfig_sm_type;

/*---------------------------------------------------------------------------
  Autoconfig SM state management and event processing functions
---------------------------------------------------------------------------*/
LOCAL void ds_3gpp_rmsm_autoconfig_sm_handle_start_addr_cfg_ev
(
  dsumts_rmsmi_sm_info_type* sm_ptr
);

LOCAL void ds_3gpp_rmsm_autoconfig_sm_handle_ra_sent_ev
(
  dsumts_rmsmi_sm_info_type* sm_ptr
);

LOCAL void ds_3gpp_rmsm_autoconfig_sm_handle_um_prefix_added_ev
(
  dsumts_rmsmi_sm_info_type* sm_ptr
);

LOCAL void ds_3gpp_rmsm_autoconfig_sm_handle_stop_addr_cfg_ev
(
  dsumts_rmsmi_sm_info_type* sm_ptr
);

LOCAL void ds_3gpp_rmsm_autoconfig_sm_handle_dhcp_start_ev
(
  dsumts_rmsmi_sm_info_type* sm_ptr
);
LOCAL void ds_3gpp_rmsm_autoconfig_sm_transition_state
(
  dsumts_rmsmi_sm_info_type*            sm_ptr,
  ds_3gpp_rmsm_autoconfig_sm_state_type new_state
);

/*---------------------------------------------------------------------------
  Autoconfig SM command handler array
---------------------------------------------------------------------------*/
typedef void (*ds_3gpp_rmsm_autoconfig_sm_cmd_hdlr_type)
               ( dsumts_rmsmi_sm_info_type*);

LOCAL ds_3gpp_rmsm_autoconfig_sm_cmd_hdlr_type 
           ds_3gpp_rmsm_autoconfig_sm_cmd_hdlr[AUTOCONFIG_SM_MAX_EV] =
{
  ds_3gpp_rmsm_autoconfig_sm_handle_start_addr_cfg_ev,          /* event 0 */
  ds_3gpp_rmsm_autoconfig_sm_handle_ra_sent_ev,                 /* event 1 */
  ds_3gpp_rmsm_autoconfig_sm_handle_um_prefix_added_ev,         /* event 2 */
  ds_3gpp_rmsm_autoconfig_sm_handle_stop_addr_cfg_ev,           /* event 3 */
  ds_3gpp_rmsm_autoconfig_sm_handle_dhcp_start_ev
};

/*---------------------------------------------------------------------------
  Autoconfig SM Callback function registered with Neighbor Discovery module
---------------------------------------------------------------------------*/

LOCAL void ds_3gpp_rmsm_autoconfig_sm_nd_ev_hdlr_cb
(
  uint8                        event,
  void                        *user_data_ptr
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          FUNCTION DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*===========================================================================
FUNCTION   DS_3GPP_RMSM_AUTOCONFIG_SM_ND_EV_HDLR_CB()

DESCRIPTION
  This callback function is called when a Neighbor Discovery event occurs.
  The IPv6 ND event is handled and a cmd posted to the Autoconfig SM to
  handle it in DS task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_autoconfig_sm_nd_ev_hdlr_cb
(
  uint8                        event,
  void                        *user_data_ptr
)
{
  ps_icmp6_nd_event_enum_type   nd_event;
  dsumts_rmsmi_sm_info_type*    sm_ptr = NULL;
  dsumts_rmsmi_info_type*       rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type     inst;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(user_data_ptr != NULL);
  sm_ptr = (dsumts_rmsmi_sm_info_type*)user_data_ptr;

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if(rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info Ptr is NULL");
    return;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  nd_event = (ps_icmp6_nd_event_enum_type) event;

  DS_3GPP_MSG1_HIGH( "Recvd ICMP6 ND ev %d", nd_event);

  switch( nd_event )
  {
    case ICMP6_ND_RA_SENT_EV:
    {
      switch( sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state )
      {
        case AUTOCONFIG_SM_AUTOCFG_IN_PROGRESS_STATE:
        {
          DS_3GPP_MSG0_HIGH ( "Ra Sent, posting AUTOCONFIG_SM_RA_SENT_EV");
          ds_3gpp_rmsm_autoconfig_sm_post_event(&inst, 
                                                AUTOCONFIG_SM_RA_SENT_EV);
          break;
        }
    
        case AUTOCONFIG_SM_NULL_STATE:
        case AUTOCONFIG_SM_ADDR_CFG_COMPLETE_STATE:
          /* fall through */
        default:
        {

          DS_3GPP_MSG1_HIGH("Ignoring RA_SENT_EV in state !",
                            sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state);
          break;
        }
      } /* switch( info->v6_sm.state ) */

      break;
    } /* nd_ra_sent_ev */

    default:
    {
      DS_3GPP_MSG1_ERROR("Unregistered event %d recvd, ignoring", nd_event);
      ASSERT(0);
      break;
    }
  } /* switch( event ) */
} /* ds_3gpp_rmsm_autoconfig_sm_nd_ev_hdlr_cb */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_AUTOCONFIG_SM_UM_EV_HDLR_CB()

DESCRIPTION
  This callback function is called when a Um Iface event occurs.
  A cmd posted to the Autoconfig SM to handle the event in DS task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_autoconfig_sm_um_iface_ev_hdlr_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{
  dsumts_rmsmi_sm_info_type* sm_ptr = NULL;
  dsumts_rmsmi_info_type*    rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type  inst;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(user_data_ptr != NULL);
  sm_ptr = (dsumts_rmsmi_sm_info_type*)user_data_ptr;

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if(rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info Ptr is NULL");
    return;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  DS_3GPP_MSG2_HIGH( "Recvd iface ev %d from 1x i/f 0x%x", event, this_iface_ptr );

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
        DS_3GPP_MSG0_HIGH( "Um IP addr changed, posting PREFIX_UPDATE_EV");
        ds_3gpp_rmsm_autoconfig_sm_post_event( &inst, 
                                               AUTOCONFIG_SM_UM_PREFIX_ADDED_EV );
      }
      break;
    }

    default:
    {
      DS_3GPP_MSG1_ERROR("Unregistered event %d recvd, ignoring", event);
      break;
    }
  } /* switch( event ) */
} /*ds_3gpp_rmsm_autoconfig_sm_um_iface_ev_hdlr_cb*/


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_INIT

  DESCRIPTION
    This function initializes the IPv6 state machine variables.

  PARAMETERS
    sm_ptr: Pointer to an RmSm IP instance

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_autoconfig_sm_init(dsumts_rmsmi_sm_info_type* sm_ptr)
{

  if(sm_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm pointer is NULL");
    return FALSE;
  }


  sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.um_prefix_update_buf_ptr = 
    ps_iface_alloc_event_cback_buf
    (
      ds_3gpp_rmsm_autoconfig_sm_um_iface_ev_hdlr_cb,
      (void *)sm_ptr
    );

  if(sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.um_prefix_update_buf_ptr 
       == NULL)
  {
    DS_3GPP_MSG0_ERROR("Event buf allocation failed");
    ASSERT(0);
    return FALSE;
  }

  sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state = AUTOCONFIG_SM_NULL_STATE;
  sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.gateway_iid = 0;
  sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.te_iid = 0;
  sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.rm_fltr_handle = 0;
  sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.inited = TRUE;
  sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.dhcp6_handle = NULL;
  return TRUE;

} /* ds_3gpp_rmsm_autoconfig_sm_init */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_INIT_CONFIG

  DESCRIPTION
    This function configures the Neighbor discovery parameters.
    
  PARAMETERS
    start_info: The Neighbor discovery information to populate.

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_autoconfig_sm_init_config 
(
  dsumts_rmsmi_sm_info_type* sm_ptr
)
{
  lan_llc_start_info_type start_info;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sm_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm pointer is NULL");
    return;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if(rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi Info pointer is NULL");
    return;
  }

  memset(&start_info, 0, sizeof(start_info));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Setup IPv6 specific LLC parameters.
  -------------------------------------------------------------------------*/
  start_info.nd_config.nd_cback_f_ptr = ds_3gpp_rmsm_autoconfig_sm_nd_ev_hdlr_cb;
  start_info.nd_config.usr_data_ptr   = (void*)sm_ptr;

  if( -1 == ps_iface_set_addr_family(&sm_ptr->sio_iface,
                                     IFACE_IPV6_ADDR_FAMILY))
  {
    DATA_ERR_FATAL("Failed to set Rm addr family to IPv6!");
    return;
  }

  (void) ps_icmp6_nd_start  (LAN_LLE_3GPP_RMSM,
                             &start_info.nd_config,
                             &sm_ptr->sio_iface,
                             NULL);

} /* ds_3gpp_rmsm_autoconfig_sm_init_config */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_PORT_EVENT

  DESCRIPTION
 	This function posts an event to Autoconfig Sub-SM and
	enqueues the corresponding command to be processed in the DS task.

  PARAMETERS
    sm_instance:  Instance of RmSm IP
    event: The event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_autoconfig_sm_post_event
(
  dsumts_rmsm_instance_type* inst_ptr,
  ds_3gpp_rmsm_autoconfig_sm_event_type  event
)
{
  dsumts_rmsmi_sm_info_type        *sm_info_ptr = NULL;
  dsumts_rmsmi_info_type           *rmsmi_info_ptr = NULL;
  ds_cmd_type                      *cmd_ptr = NULL;
  ds_3gpp_rmsm_autoconfig_sm_type  *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(inst_ptr == NULL)
  {
    DATA_ERR_FATAL("Inst ptr is NULL");
    return;
  }

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if(rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi Info pointer is NULL");
    return;
  }

  sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);

  if(sm_info_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm pointer is NULL");
    return;
  }

  if(sm_info_ptr->pdp_specific_info.pdp_ip.v6_sm_info.inited != TRUE)
  {
    ASSERT(0);
    DS_3GPP_MSG1_ERROR ("Can't post ev %d, AUTOCONFIG_SM not init'd", 
                         event);
    return;
  }

  if (event >= AUTOCONFIG_SM_MAX_EV)
  {
    ASSERT (0);
    DS_3GPP_MSG1_ERROR ("Invalid AUTOCONFIG_SM event %d, ignored", event);
    return;
  }

  DS_3GPP_MSG3_HIGH( "Event %d posted to AUTOCONFIG_SM in state %d",
                      event, 
                      sm_info_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state, 
                      0 );

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_rmsm_autoconfig_sm_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  /*-------------------------------------------------------------------------
    Post cmd to DCC task corresponding to each event
  -------------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_AUTOCONFIG_SM;
  data_ptr = (ds_3gpp_rmsm_autoconfig_sm_type*)cmd_ptr->cmd_payload_ptr;
  data_ptr->event  = (uint32)event;
  data_ptr->sm_ptr = (void *)sm_info_ptr;

  ds_put_cmd( cmd_ptr );

} /* ds_3gpp_rmsm_autoconfig_sm_post_event */


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_PROCESS_EV_CMD

  DESCRIPTION
 	This function posts an event to Autoconfig Sub-SM and
	enqueues the corresponding command to be processed in the DS task.

  PARAMETERS
    cmd_ptr - Pointer to the commnd which has to be processed.

  RETURN VALUE
    None

  DEPENDENCIES
    State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_autoconfig_sm_process_ev_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  dsumts_rmsmi_sm_info_type              *sm_ptr = NULL;
  ds_3gpp_rmsm_autoconfig_sm_event_type   event;
  ds_3gpp_rmsm_autoconfig_sm_type        *data_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  ASSERT (cmd_ptr);
  ASSERT (cmd_ptr->cmd_payload_ptr);

  data_ptr = (ds_3gpp_rmsm_autoconfig_sm_type*)cmd_ptr->cmd_payload_ptr;
  
  event = (ds_3gpp_rmsm_autoconfig_sm_event_type)data_ptr->event;

  /*-------------------------------------------------------------------------
    Validate posted event before processing
  -------------------------------------------------------------------------*/
  if (event >= AUTOCONFIG_SM_MAX_EV)
  {
    ASSERT(0);            
    DS_3GPP_MSG1_ERROR("Invald AUTOCONFIG_SM event %d, ignoring", event );
    return;
  }

  sm_ptr  = (dsumts_rmsmi_sm_info_type*) data_ptr->sm_ptr;

  if(sm_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm pointer is NULL");
    return;
  }

  /*-------------------------------------------------------------------------
    Verify that AUTOCONFIG_SM is initialized
  -------------------------------------------------------------------------*/
  if(sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.inited != TRUE)
  {
    ASSERT(0);
    DS_3GPP_MSG1_ERROR("Can't process event %d, AUTOCONFIG_SM not init'd", 
                       event);
    return;
  }

  /*-------------------------------------------------------------------------
    Validate AUTOCONFIG_SM state before processing event - this allows event handler
    functions to assume that state is valid.
  -------------------------------------------------------------------------*/
  if (sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state >= 
      AUTOCONFIG_SM_MAX_STATE)
  {
    ASSERT( 0 );
    DS_3GPP_MSG1_ERROR("Invalid AUTOCONFIG_SM state %d", 
                       sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state);
    return;
  }


  DS_3GPP_MSG3_HIGH( "Autoconfig SM event %d in state %d, info %x",
                     event, 
                     sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state, 
                     sm_ptr );

  /*-------------------------------------------------------------------------
    Invoke associated command handler function
  -------------------------------------------------------------------------*/
  (*(ds_3gpp_rmsm_autoconfig_sm_cmd_hdlr[event]))(sm_ptr);

} /* ds_3gpp_rmsm_autoconfig_sm_process_ev_cmd */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_HANDLE_START_ADDR_CFG_EV()

  DESCRIPTION
    This function processes START_ADDR_CFG_EV event. 

    This event indicates that the IPv6 address configuration state machine
    should begin IPv6 stateless address configuration on the Rm.

  PARAMETERS
    sm ptr - Pointer to an RmSM IP instance

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher 
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_autoconfig_sm_handle_start_addr_cfg_ev
(
  dsumts_rmsmi_sm_info_type* sm_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sm_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm pointer is NULL");
    return;
  }

  DS_3GPP_MSG1_HIGH( "Recvd START_ADDR_CFG_EV, state %d",
                     sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state )
  {
    case AUTOCONFIG_SM_NULL_STATE:
    {
      ds_3gpp_rmsm_autoconfig_sm_transition_state(sm_ptr , 
                                    AUTOCONFIG_SM_AUTOCFG_IN_PROGRESS_STATE);
      break;
    }

    case AUTOCONFIG_SM_AUTOCFG_IN_PROGRESS_STATE:
    case AUTOCONFIG_SM_ADDR_CFG_COMPLETE_STATE:
      /* fall through */

    default:
    {
      DS_3GPP_MSG1_ERROR("Got START_ADDR_CFG_EV in unexpected state %d",
                sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state);
      break;
    }
  } /* switch( info->v6_sm.state ) */

} /* ds_3gpp_rmsm_autoconfig_sm_handle_start_addr_cfg_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_HANDLE_RA_SENT_EV()

  DESCRIPTION
    This function processes RA_SENT_EV event. 

    This event indicates that a Router Advertisement has been sent in
    response to the Router Solicitation by tethered device. 

  PARAMETERS
    sm ptr - Pointer to an RmSM IP instance

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher 
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_autoconfig_sm_handle_ra_sent_ev
(
  dsumts_rmsmi_sm_info_type* sm_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sm_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm pointer is NULL");
    return;
  }


  DS_3GPP_MSG1_HIGH( "Recvd RA_SENT_EV, state %d",
             sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state );

  switch( sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state )
  {
    case AUTOCONFIG_SM_NULL_STATE:
    case AUTOCONFIG_SM_ADDR_CFG_COMPLETE_STATE:

      DS_3GPP_MSG1_ERROR( "RA Sent Event not expected in state %d", 
                          sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state);
      break;

    case AUTOCONFIG_SM_AUTOCFG_IN_PROGRESS_STATE:
    {
      ds_3gpp_rmsm_autoconfig_sm_transition_state
      (
        sm_ptr, 
        AUTOCONFIG_SM_ADDR_CFG_COMPLETE_STATE
      );
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad Autoconfig SM state!");
      break;
    }
  } /* switch( info->v6_sm.state ) */

} /* ds_3gpp_rmsm_autoconfig_sm_handle_ra_sent_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_HANDLE_UM_PREFIX_ADDED_EV()

  DESCRIPTION
    This function processes UM_PREFIX_ADDED_EV event. 

    This event indicates that a new prefix was added. This will cause the 
    Rm ND queues to be flushed and autoconfiguration of the Rm to commence.

  PARAMETERS
    sm ptr - Pointer to an RmSM IP instance

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher 
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_autoconfig_sm_handle_um_prefix_added_ev
(
  dsumts_rmsmi_sm_info_type* sm_ptr
)
{
  dsumts_rmsm_instance_type inst;
  dsumts_rmsmi_info_type* rmsmi_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sm_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm pointer is NULL");
    return;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if(rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("rmsmi_info_ptr is NULL");
    return;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  DS_3GPP_MSG1_HIGH( "Recvd UM_PREFIX_ADDED_EV, state %d", 
                     sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state )
  {
    case AUTOCONFIG_SM_NULL_STATE:
    case AUTOCONFIG_SM_AUTOCFG_IN_PROGRESS_STATE:
      break;

    case AUTOCONFIG_SM_ADDR_CFG_COMPLETE_STATE:
    {
      ps_icmp6_nd_flush(LAN_LLE_3GPP_RMSM);
      ds_3gpp_rmsm_autoconfig_sm_transition_state
      (
        sm_ptr, 
        AUTOCONFIG_SM_AUTOCFG_IN_PROGRESS_STATE
      );

      dsumts_rmsm_post_event(&inst, UMTS_RMSM_ADDR_CFG_CHANGED_EV);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad Autoconfig SM state!");
      break;
    }
  } /* switch( info->v6_sm.state ) */

} /* ds_3gpp_rmsm_autoconfig_sm_handle_um_prefix_added_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_HANDLE_STOP_ADDR_CFG_EV()

  DESCRIPTION
    This function processes STOP_ADDR_CFG_EV event. 

    This event indicates that the IPv6 address configuration state machine
    should now be stopped.

  PARAMETERS
    sm ptr - Pointer to an RmSM IP instance

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher 
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_autoconfig_sm_handle_stop_addr_cfg_ev
(
  dsumts_rmsmi_sm_info_type* sm_ptr
)
{
  DS_3GPP_MSG1_HIGH( "Recvd STOP_ADDR_CFG_EV, state %d",
                     sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state )
  {
    case AUTOCONFIG_SM_NULL_STATE:
      break;

    case AUTOCONFIG_SM_AUTOCFG_IN_PROGRESS_STATE:
    case AUTOCONFIG_SM_ADDR_CFG_COMPLETE_STATE:
    {
      ds_3gpp_rmsm_autoconfig_sm_transition_state (sm_ptr,
                                                   AUTOCONFIG_SM_NULL_STATE);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad Autoconfig SM state!");
      break;
    }
  } /* switch( info->v6_sm.state ) */
} /* ds_3gpp_rmsm_autoconfig_sm_handle_stop_addr_cfg_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_HANDLE_DHCP_START_EV()

  DESCRIPTION
    This function processes DHCP_START_EV event. 

    This event indicates that DHCPv6 Server started successfully.

  PARAMETERS
    sm ptr - Pointer to an RmSM IP instance

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher 
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_autoconfig_sm_handle_dhcp_start_ev
(
  dsumts_rmsmi_sm_info_type* sm_ptr
)
{
  void* handle = NULL;

  if(sm_ptr == NULL)
  {
    DATA_ERR_FATAL("sm ptr is NULL");
    return;
  }

  handle = sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.dhcp6_handle;

  if(NULL == handle)
  {
    DS_3GPP_MSG0_ERROR("Handle is NULL. Couldnot start dhcp server");
    return;
  }
  else
  {
    PS_IFACE_SET_DHCP6_SERVER_HANDLE(&sm_ptr->sio_iface, handle);
    DS_3GPP_MSG0_HIGH("DHCP6 server started successfully");
  }
} /* ds_3gpp_rmsm_autoconfig_sm_handle_dhcp_start_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_DHCP6_STARTED_CB

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
static void ds_3gpp_rmsm_autoconfig_sm_dhcp6_started_cb
(
  void *  userdata, 
  void *  handle
)
{
  dsumts_rmsmi_sm_info_type   *sm_ptr = (dsumts_rmsmi_sm_info_type *) userdata;
  ds_cmd_type                      *cmd_ptr = NULL;
  ds_3gpp_rmsm_autoconfig_sm_type  *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH ("DHCP6 server is ready");

  if(userdata == NULL)
  {
    DATA_ERR_FATAL("Userdata is NULL");
    return;
  }

  sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.dhcp6_handle = handle;

  if(handle == NULL)
  {
    DS_3GPP_MSG0_ERROR("Handle is NULL. DHCP Server didnot start");
    return;
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_rmsm_autoconfig_sm_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  /*-------------------------------------------------------------------------
    Post cmd to DCC task corresponding to each event
  -------------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_AUTOCONFIG_SM;
  data_ptr = (ds_3gpp_rmsm_autoconfig_sm_type*)cmd_ptr->cmd_payload_ptr;
  data_ptr->event = (uint32)AUTOCONFIG_SM_DHCP_START_EV;
  data_ptr->sm_ptr = (void *)sm_ptr;

  ds_put_cmd( cmd_ptr );

} /*ds_3gpp_rmsm_autoconfig_sm_dhcp6_started_cb() */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_DHCP6_STOPPED_CB

  DESCRIPTION
    This is the dhcp6_server_mgr_stop completion callback.

  PARAMETERS
    userdata:  the user data provided when dhcp6_server_mgr_stop was called
    ok: Indicates whether the server stopped successfully.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds_3gpp_rmsm_autoconfig_sm_dhcp6_stopped_cb
(
  void *   userdata, 
  boolean  ok
)
{
  dsumts_rmsmi_sm_info_type* sm_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG1_HIGH ("DHCP6 stopped callback (status = %d)", ok);
  ASSERT (userdata);

  sm_ptr = (dsumts_rmsmi_sm_info_type*)userdata;

  sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.dhcp6_handle = NULL;

} /*ds_3gpp_rmsm_autoconfig_sm_dhcp6_started_cb() */

/*===========================================================================
  FUNCTION DS_RMSM_AUTOCONFIG_SM_TRANSITION_STATE()

  DESCRIPTION
    This function performs state transition actions for the Autoconfig SM.

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
LOCAL void ds_3gpp_rmsm_autoconfig_sm_transition_state
(
  dsumts_rmsmi_sm_info_type*           sm_ptr,
  ds_3gpp_rmsm_autoconfig_sm_state_type new_state
)
{
  sint15                   ps_errno;
  ps_ip_addr_type          um_ip_addr;
  ps_ip_addr_type          te_ip_addr;
  uint64                 * gateway_iid = NULL;
  uint64                 * te_iid = NULL;
  uint64                   prefix;
  ps_iface_addr_mgmt_handle_type handle;
  ps_iface_addr_mgmt_alloc_type  alloc_info;
  int16 ps_errno_addr_mgmt = 0;
  dhcp6_sl_server_duid* dhcp6_server_duid_ptr = NULL;
  int ret = -1;
  void* dhcp6_handle = NULL;
  dsumts_rmsm_instance_type inst;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sm_ptr == NULL)
  {
    DATA_ERR_FATAL("SM Ptr is NULL");
    return;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if(rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info Ptr is NULL");
    return;
  }

  DS_3GPP_MSG2_HIGH( "AUTOCONFIG_SM state goes %d to %d", 
                     sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state, 
                     new_state);

  sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.state = new_state;

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  memset(&te_ip_addr, 0, sizeof(ps_ip_addr_type));
  memset(&um_ip_addr, 0, sizeof(ps_ip_addr_type));

  te_ip_addr.type = IPV6_ADDR;
  um_ip_addr.type = IPV6_ADDR;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( new_state )
  {
    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        AUTOCONFIG_SM_NULL_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case AUTOCONFIG_SM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
        Stop the DHCP6 server.
      ---------------------------------------------------------------------*/
      dhcp6_handle = 
        sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.dhcp6_handle;

      if (dhcp6_handle != NULL)
      {
        if (TRUE != dhcp6_sl_server_mgr_stop
                    (
                      &dhcp6_handle,
                      ds_3gpp_rmsm_autoconfig_sm_dhcp6_stopped_cb,
                      sm_ptr
                    ) )
        {
          DS_3GPP_MSG0_ERROR("Could not stop the DHCP6 server");
        }
  
        PS_IFACE_SET_DHCP6_SERVER_HANDLE(&sm_ptr->sio_iface, NULL);
      }

      else
      {
        DS_3GPP_MSG0_HIGH("DHCP6 handle was NULL. ");
      }


      ps_icmp6_nd_stop( LAN_LLE_3GPP_RMSM );

       /*---------------------------------------------------------------------
        Delete the ICMPv6 filters on the Rm and Um.
      ---------------------------------------------------------------------*/

      ret = ps_iface_ipfltr_delete
             (
               &sm_ptr->sio_iface,
               IP_FLTR_CLIENT_SOCKETS,
               sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.rm_fltr_handle,
               &ps_errno
             );

      if(ret == -1)
      {
        DS_3GPP_MSG1_ERROR("Error while deleting ICMPv6 filters, sm_ptr: %x",
                           sm_ptr);
      }

      if(sm_ptr->um_iface_ptr)
      {
        ps_iface_event_cback_dereg
        (
          sm_ptr->um_iface_ptr,
          IFACE_PREFIX_UPDATE_EV,
          sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.um_prefix_update_buf_ptr
        );

      }

      dsumts_rmsm_post_event(&inst,UMTS_RMSM_ADDR_CFG_END_EV);

      break;
    }

    case AUTOCONFIG_SM_AUTOCFG_IN_PROGRESS_STATE:
    {

      if(sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.dhcp6_handle == NULL)
      {
        dhcp6_server_duid_ptr = 
          &sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.dhcp6_server_duid;
  
        ret = dhcp6_duid_ll_init(dhcp6_server_duid_ptr->server_duid,
                                 DHCP6_MAX_DUID_LENGTH,
                                 DHCP_HWTYPE_ETHERNET,
                                 PS_IFACE_HW_ADDR_PTR(&sm_ptr->sio_iface),
                                 PS_IFACE_MAX_HW_ADDR_LEN);
  
        if ( ret <= 0 )
        {
          DS_3GPP_MSG0_ERROR ("DHCPv6 server DUID initialisation failed"
                               "Cannot start the DHCPv6 server"); 
        }
        else
        {
          dhcp6_server_duid_ptr->server_duid_len = ret;
          /*---------------------------------------------------------------------
             Start the DHCP v6 server
          ---------------------------------------------------------------------*/
          if (dhcp6_sl_server_mgr_start
              ( 
                &sm_ptr->sio_iface,
                sm_ptr->um_iface_ptr,
                dhcp6_server_duid_ptr,
                NULL,
                ds_3gpp_rmsm_autoconfig_sm_dhcp6_started_cb,
                sm_ptr
              ) == FALSE )
          {
            DS_3GPP_MSG0_ERROR ("DHCPv6 server start failed"); 
          }
        }
  
      }

      /*---------------------------------------------------------------------
        Get the prefix from Um Iface
      ---------------------------------------------------------------------*/
      ps_iface_get_addr(sm_ptr->um_iface_ptr, &um_ip_addr);
      prefix = um_ip_addr.addr.v6.ps_s6_addr64[0];

      DS_3GPP_MSG1_HIGH("Prefix: %lx", prefix);

      /*---------------------------------------------------------------------
        Generate a new IID to be used by T.E.
      ---------------------------------------------------------------------*/
      te_iid = &(sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.te_iid);      
      
      if (-1 == ps_iface_generate_ipv6_iid
                  (sm_ptr->um_iface_ptr, te_iid, &ps_errno))
      {
        DS_3GPP_MSG0_ERROR("Failed to generate a random IPv6 IID");
      }

      /*---------------------------------------------------------------------
        Create the TE IP Address
      ---------------------------------------------------------------------*/

      te_ip_addr.addr.v6.ps_s6_addr64[0] = prefix;
      te_ip_addr.addr.v6.ps_s6_addr64[1] = *te_iid;

      DS_3GPP_MSG3_HIGH("Prefix: %lx, TE IID: %lx", 
                        te_ip_addr.addr.v6.ps_s6_addr64[0],
                        te_ip_addr.addr.v6.ps_s6_addr64[1],
                        0);

      /*---------------------------------------------------------------------
        Check the uniqueness of the TE IP Address
      ---------------------------------------------------------------------*/

      handle = ps_iface_addr_mgmt_get_handle_from_ip
                (&sm_ptr->sio_iface, &(te_ip_addr.addr.v6));
      if (-1 == handle)
      {
        /* No match found, add to database */
        memset(&alloc_info, 0, sizeof(alloc_info));
        alloc_info.ip_addr.type    = IPV6_ADDR;
        alloc_info.ip_addr.addr.v6 = te_ip_addr.addr.v6;
        alloc_info.addr_type       = IPV6_ADDR_TYPE_EXTERNAL;
        if ( -1 == ps_iface_addr_mgmt_alloc_unique_addr(
                     &sm_ptr->sio_iface,
                     &handle,
                     &alloc_info, 
                     &ps_errno_addr_mgmt))
        {
          DS_3GPP_MSG1_ERROR("Failed to add IPv6 addr to UM database. "
                             "errno [%d]", ps_errno_addr_mgmt);
        }
        else
        {
          DS_3GPP_MSG0_ERROR("Added IPv6 addr to UM database");
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Invalid handle obtained from ps iface addr mgmt!");
      }

      /*---------------------------------------------------------------------
        Set the prefix used on SIO Iface
      ---------------------------------------------------------------------*/
      sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.prefix = prefix;
      sm_ptr->sio_iface.iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->prefix = 
        prefix;


      /*---------------------------------------------------------------------
        Generate a new IID to be used by SIo IFace
      ---------------------------------------------------------------------*/
      gateway_iid = &sm_ptr->sio_iface.iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->iid;

      if (-1 == ps_iface_generate_ipv6_iid
                  (sm_ptr->um_iface_ptr, gateway_iid, &ps_errno))
      {
        DS_3GPP_MSG0_ERROR("Failed to generate a random IPv6 IID");
      }

      /*---------------------------------------------------------------------
        Save the gateway IID for internal bookkeeping
      ---------------------------------------------------------------------*/

      sm_ptr->pdp_specific_info.pdp_ip.v6_sm_info.gateway_iid = *gateway_iid;

      /*---------------------------------------------------------------------
        Save the gateway IID on the SIO Iface
      ---------------------------------------------------------------------*/

      sm_ptr->sio_iface.v6_net_info.gateway_iid = *gateway_iid;

      if (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->configure_ppp_f_ptr
            == NULL)
      {
        DATA_ERR_FATAL("Configure PPP Function is NULL");
        return;
      }

      if (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->configure_ppp_f_ptr 
           ((void*)rmsmi_info_ptr, PPP_PROTOCOL_IPV6CP) == FALSE)
      {
        dsumts_rmsm_post_event(&inst,UMTS_RMSM_ADDR_CFG_FAILURE_EV);

      }

      break;
    }

    case AUTOCONFIG_SM_ADDR_CFG_COMPLETE_STATE:
    {
      dsumts_rmsm_post_event(&inst, UMTS_RMSM_ADDR_CFG_COMPLETE_EV);
      break;
    }


    default:
    {
      DS_3GPP_MSG1_ERROR("Invalid state %d", new_state);
      ASSERT(0);
    }
  } /* switch(new state) */
} /* ds_3gpp_rmsm_autoconfig_sm_transition_state */

