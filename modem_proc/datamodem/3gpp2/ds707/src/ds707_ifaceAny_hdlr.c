/*===========================================================================

                     D S G E N _ I F A C E A N Y _ H D L R . C
                   
DESCRIPTION
  The Data Services sockets network state machine implementation for
  IP Iface Handler. 
  This contains all the functions used by the sockets library to access the 
  network and communicate with the DS task. (This is an INTERNAL sockets
  file).
 
EXTERNALIZED FUNCTIONS
  dssnetAny_sm_create()
    Initialize the IP Iface Handler State machine
  dssnetAny_sm_post_event()
    Post events to the DSSNETANY state machine

 Copyright (c)2004 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_ifaceAny_hdlr.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/18/12   jz      Seperate old JCDMA and Simplified JCDMA 
01/02/12    msh     Coan: Feature cleanup
06/06/11    ash    Added support for PPP Partial Context
03/15/11    mg     Global variable cleanup
10/14/10    sa     Adding debug messages before ASSERT.
10/13/10    op     Migrated to MSG 2.0 macros
04/29/09    ms     Fixed Lint High Errors.
03/04/09    sa     AU level CMI modifications.
07/25/08    hs     Fixed compilation warnings
01/22/08    scb/sy Mainlined features under FEATURE_JCDMA.
02/12/07    az     Support to request VJ compression during PPP negotiation
05/25/06    kvd    Set bring_up flag in scl policy to TRUE before route_get.
08/15/05    mct    Fixed naming causing some issues w/C++ compilers.
04/28/05    mvl    Made fix to ensure that when PPP start is called on the
                   transition from Handup to Handdown that the NCP modes are
                   correctly set (i.e. the non-pref NCP is active).
04/18/05    mct    Added ANY ioctl handler. Now passively associating with 
                   preferred iface.
02/08/05    ks     Removed calls for link and flow logging.
01/12/05    ks     Added support for Data Protocol Logging
12/14/04    mct    Added is_sock_orig = TRUE so fallback would work for
                   sockets calls when V6 is enabled.
08/30/04    mvl    Removed many header files, and continued modularization.
08/20/04    mvl    Restructured further to make module more stand alone.
08/18/04    kvd    Moved failover iface settings to dssnetANy_sm_create()
                   from ds707_pkt_mgr_init(); Do route_get on assoc_iface
                   before active_associate() to block other from using it.
08/18/04    mvl    changed ppp_en/disable_failover() to lower case.  Changed 
                   ps iface_down_ev handler to generate down in opening state
                   only when phys_link is gone (i.e down & not dormant).
08/11/04    mvl    Made fixes to design and improved messaging.
08/10/04    mct    Updated teardown and bringup cmds for logical iface,
                   and updated some state change info.
07/20/04    kvd    Created Module
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "dstask.h"
#include "dstaski.h"
#include "dsat_v.h"
#include "ds707_ifaceAny_hdlr.h"
#include "ps_iface.h"
#include "amssassert.h"
#include "msg.h"
#include "data_msg.h"
#include "ps_ppp.h"
#include "ps_route.h"
#include "nv.h"
#include "dshdr_dhcp_mgr.h"

#include "ps_iface_logging.h"
#include "ps_logging_defs.h"

#include "ds707_pkt_mgr.h"
#include "ds707_roaming.h"
#include "ps_iface_addr_v6.h"

#if 0
dssnetAny_sm_cb_type   ds707_any_instances[DS707_MAX_ANY_INSTS];
#endif 

/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
MACRO DSSNETANY_SMI_IS_VALID

DESCRIPTION
  Make sure the SM instance passed in is valid
---------------------------------------------------------------------------*/
#define DSSNETANY_SMI_IS_VALID( instance_ptr )                              \
  (((instance_ptr) != NULL) && ((instance_ptr->this_sm_ptr) == instance_ptr))

/*===========================================================================

                      FORWARD  DECLARATIONS FOR MODULE

===========================================================================*/
static void dssnetAnyi_transition_state
(
  dssnetAny_sm_cb_type    *dssnet_sb_ptr,
  dssnetAnyi_state_type   new_state
);

static void dssnetAnyi_process_iface_up_ev
(
  dssnetAny_sm_cb_type *instance_ptr          /* dssnetAny instance ptr    */
);

static void dssnetAnyi_process_iface_down_ev
(
  dssnetAny_sm_cb_type *instance_ptr          /* dssnetAny instance ptr    */
);

static void dssnetAnyi_process_open_ev
(
  dssnetAny_sm_cb_type *instance_ptr          /* dssnetAny instance ptr    */
);

static void dssnetAnyi_process_handoff_ev
(
  dssnetAny_sm_cb_type *instance_ptr          /* dssnetAny instance ptr    */
);

static void dssnetAnyi_process_teardown_ev
(
  dssnetAny_sm_cb_type *instance_ptr         /* dssnetAny instance ptr     */
);

static void dssnetAnyi_process_close_ev
(
  dssnetAny_sm_cb_type *instance_ptr         /* dssnetAny instance ptr     */
);

static void dssnetAnyi_707_pkt_event_handler_cb
(
  ps_iface_type *this_iface_ptr,           /* Pointer to the interface     */
  ps_iface_event_enum_type   event,        /* Event type occurred          */
  ps_iface_event_info_u_type event_info,   /* Event specific info block    */
  void *user_data_ptr           /* Data passed during event registration   */
);

static int dssnetAnyi_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  void *client_data
);

static int dssnetAnyi_bring_up_cmd
(
  ps_iface_type *this_iface_ptr,
  void *client_data
);

static int dssnetAnyi_iface_ioctl_hdlr
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
);

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSNETANY_SM_CREATE()

DESCRIPTION
  This function is is used to initialize the IP Iface Handler state machine
  control block. Called by the mode specific handler. The mode specific
  handler allocates memory for the control block as well as the ps iface.

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int dssnetAny_sm_create
(
  dssnetAny_sm_cb_type  *instance_ptr,      /* Ptr to Any Iface SM instance*/
  void                  *client_data_ptr,
  acl_fptr_type          acl_f_ptr,
  ps_iface_type         *ipv4_iface_ptr,
  ps_iface_type         *ipv6_iface_ptr,
  ps_iface_ip_ver_failover_e_type failover_mode
)
{
  acl_type              *any_acl_ptr;       /* ptr to acl                  */
  ps_iface_type         *any_iface_ptr;     /* This Interface Ptr          */
  int16                  ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Make sure the DSSNETANY control block and the ps iface are not NULL.
  -------------------------------------------------------------------------*/
  if (instance_ptr == NULL)
  {
    DATA_ERR_FATAL("DSSNetAny instance_ptr is NULL");
    return -1;
  }

  if(instance_ptr->this_sm_ptr == instance_ptr)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "DSSNetAny SM 0x%p already initialized", instance_ptr);
    return 0;
  }

  if((failover_mode == IPV4_ONLY) || (failover_mode == IPV6_ONLY))
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                    "DSSNetAny SM 0x%p invalid failover mode %d",
                    instance_ptr, failover_mode);
    return 0;
  }

  any_acl_ptr                 = &(instance_ptr->local_acl);
  any_acl_ptr->acl_fptr       = acl_f_ptr;
  any_acl_ptr->post_proc_fptr = NULL;

  /*-------------------------------------------------------------------------
    Create the PS_IFACE.
  -------------------------------------------------------------------------*/
  any_iface_ptr = &(instance_ptr->local_iface);
  if(ps_iface_create(any_iface_ptr,
                     CDMA_SN_IFACE,
                     any_acl_ptr,
                     NULL,
                     NULL,
                     0) < 0)
  {
    ERR_FATAL("Could not create Any ps iface",0,0,0);
  }
  
  /*-------------------------------------------------------------------------
    To support Data Protocol Logging on the ANY iface
  -------------------------------------------------------------------------*/

  (void) ps_iface_dpl_support_network_logging(any_iface_ptr,
                                       DPL_IID_IFNAME_CDMA_SN_IFACE_ANY_PKT);

  /*-------------------------------------------------------------------------
    Set the ifaces client data pointer to point to this SM instance so that
    in bring_up() and tear_down() the instance can be identified.
  -------------------------------------------------------------------------*/
  any_iface_ptr->client_data_ptr = instance_ptr;

  /*-------------------------------------------------------------------------
    Set up the bring up and tear down functions for DSSNETAny called
    by the Any applications to bring up and teardown ANY iface.
  -------------------------------------------------------------------------*/
  any_iface_ptr->bring_up_cmd_f_ptr   = dssnetAnyi_bring_up_cmd;
  any_iface_ptr->tear_down_cmd_f_ptr  = dssnetAnyi_tear_down_cmd;

  /*-------------------------------------------------------------------------
    This is for doing ioctls specific to 1x pkt interface.
  -------------------------------------------------------------------------*/
  any_iface_ptr->iface_ioctl_f_ptr    = dssnetAnyi_iface_ioctl_hdlr;

  /*-------------------------------------------------------------------------
    This is for dorm status actions.
  -------------------------------------------------------------------------*/
  any_iface_ptr->dorm_action_f_ptr    = NULL;
  
  /*-------------------------------------------------------------------------
    Set the rx processing function ptr.
  -------------------------------------------------------------------------*/
  ps_iface_set_rx_function(any_iface_ptr, NULL);

  /*-------------------------------------------------------------------------
    Enable the PS Iface
  -------------------------------------------------------------------------*/
  ps_iface_enable_ind(any_iface_ptr);

  /*-------------------------------------------------------------------------
    Set all of the failover related variables based on the mode
  -------------------------------------------------------------------------*/
  switch(failover_mode)
  {
    case IPV6_PREFERRED:
      instance_ptr->pref_iface_ptr = ipv6_iface_ptr;
      instance_ptr->non_pref_iface_ptr = ipv4_iface_ptr;
      instance_ptr->ppp_failover_mode = PPP_FAILOVER_V6_THEN_V4;
      instance_ptr->any_pref_mode = DSSNETANY_IFACE_PREFERRED;
      break;

    case IPV6_DESIRED:
      instance_ptr->pref_iface_ptr = ipv6_iface_ptr;
      instance_ptr->non_pref_iface_ptr = ipv4_iface_ptr;
      instance_ptr->ppp_failover_mode = PPP_FAILOVER_V6_THEN_V4;
      instance_ptr->any_pref_mode = DSSNETANY_IFACE_DESIRED;
      break;

    case IPV4_PREFERRED:
      instance_ptr->pref_iface_ptr = ipv4_iface_ptr;
      instance_ptr->non_pref_iface_ptr = ipv6_iface_ptr;
      instance_ptr->ppp_failover_mode = PPP_FAILOVER_V4_THEN_V6;
      instance_ptr->any_pref_mode = DSSNETANY_IFACE_PREFERRED;
      break;

    case IPV4_DESIRED:
      instance_ptr->pref_iface_ptr = ipv4_iface_ptr;
      instance_ptr->non_pref_iface_ptr = ipv6_iface_ptr;
      instance_ptr->ppp_failover_mode = PPP_FAILOVER_V4_THEN_V6;
      instance_ptr->any_pref_mode = DSSNETANY_IFACE_DESIRED;
      break;

    default:
      ERR_FATAL("Invalid failover type %d!", (uint8)failover_mode, 0, 0);
      break;
  } /* switch(failove_mode) */

  /*------------------------------------------------------------------------
    Allocate buffer for Iface UP/Down events on the currrent ( V6/V4) iface
    chosen, registered on entering OPENING state for the initial call and
    from HNADUP, HANDDOWN & FAILOVER states thereafter.
  ------------------------------------------------------------------------*/
  if((instance_ptr->assoc_iface_up_cbuf =
      ps_iface_alloc_event_cback_buf(dssnetAnyi_707_pkt_event_handler_cb,
                                     instance_ptr)) == NULL)
  {
    ERR_FATAL("No PS IFACE Event Buffers Avail",0,0,0);
  }

  if((instance_ptr->assoc_iface_down_cbuf =
      ps_iface_alloc_event_cback_buf(dssnetAnyi_707_pkt_event_handler_cb,
                                     instance_ptr)) == NULL)
  {
    ERR_FATAL("No PS IFACE Event Buffers Avail",0,0,0);
  }

  if((instance_ptr->assoc_iface_config_cbuf =
      ps_iface_alloc_event_cback_buf(dssnetAnyi_707_pkt_event_handler_cb,
                                     instance_ptr)) == NULL)
  {
    ERR_FATAL("No PS IFACE Event Buffers Avail",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Initialize the remaining DssnetAny instance variables.
  -------------------------------------------------------------------------*/
  instance_ptr->state = DSSNETANY_CLOSED_STATE;
  instance_ptr->client_data_ptr = client_data_ptr;

  if( (ps_iface_passive_associate( &(instance_ptr->local_iface),
                                   instance_ptr->pref_iface_ptr,
                                   TRUE,
                                   &ps_errno)) < 0)
  {
    ERR_FATAL("Can't passively assoc ANY iface.",0,0,0);
  }
  /*------------------------------------------------------------------------
    This state machine is now initialized
  ------------------------------------------------------------------------*/
  instance_ptr->this_sm_ptr = instance_ptr;
  DATA_IS707_MSG3(MSG_LEGACY_MED, "DSSNetAny SM 0x%x for iface %x:%d initialized",
                  instance_ptr, any_iface_ptr->name, any_iface_ptr->instance);

  return 0;

} /* dssnetAny_sm_create() */


/*===========================================================================
FUNCTION DSSNETANY_SM_POST_EVENT()

DESCRIPTION
  This function is used to post events to the dssnet_sm by other modules.  

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int dssnetAny_sm_post_event
(
  dssnetAny_sm_cb_type      *instance_ptr,   /* Ptr to SM instance         */
  dssnetAny_sm_event_type   event            /* dssnetAny event type       */
)
{
  ds_cmd_type        *cmd_ptr;                              /* ds cmd type */
  ds_cmd_enum_type    cmd_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(event < DSSNETANY_MAX_EV);

  /*-------------------------------------------------------------------------
    Verify that this state machine has been initialied.
  -------------------------------------------------------------------------*/
  if( !DSSNETANY_SMI_IS_VALID(instance_ptr) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Tried posting to invalid DSSNetAny instance");
    return -1;
  }

  DATA_IS707_MSG3(MSG_LEGACY_MED, 
                  "Posting event %d to DSSNetAny(0x%x) in state %d",
                  event, instance_ptr, instance_ptr->state);

  /*-------------------------------------------------------------------------
    Initialize command name to DS_MIN_CMD.
  -------------------------------------------------------------------------*/
  cmd_id = DS_MIN_CMD;

  /*-------------------------------------------------------------------------
    Based on the event type, set the appropriate PS cmd 
  -------------------------------------------------------------------------*/
  switch(event)
  {

    case DSSNETANY_IFACE_DOWN_EV:
    {
      cmd_id = DS_CMD_DSSNETANY_IFACE_DOWN_EV;
      break;
    }

    case DSSNETANY_IFACE_UP_EV:
    {
      cmd_id = DS_CMD_DSSNETANY_IFACE_UP_EV;
      break;
    }

    case DSSNETANY_CONFIGURING_EV:
    {
      cmd_id = DS_CMD_DSSNETANY_CONFIGURING_EV;
      break;
    }

    case DSSNETANY_OPEN_EV:
    {
      cmd_id = DS_CMD_DSSNETANY_OPEN_EV;
      break;
    }

    case DSSNETANY_TEARDOWN_EV:
    {
      cmd_id = DS_CMD_DSSNETANY_TEARDOWN_EV;
      break;
    }

    case DSSNETANY_CLOSE_EV:
    {
      cmd_id = DS_CMD_DSSNETANY_CLOSE_EV;
      break;
    }

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return -1;
  } /* switch(event) */

  /*-------------------------------------------------------------------------
    Enqueue cmd with DS task if it has been set to a valid value 
  -------------------------------------------------------------------------*/
  if (cmd_id != DS_MIN_CMD)
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(dssnetAny_sm_cb_type *));
    /** Get command buffer to do task switch */
    if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
    {
      ASSERT(0);
      return -1;
    }
    else
    {
      cmd_ptr->hdr.cmd_id        = cmd_id;
      cmd_ptr->cmd_payload_ptr   = instance_ptr;

      ds_put_cmd( cmd_ptr );
    }
  }

  return 0;
} /* dssnetAny_sm_post_event() */


/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSNETANY_REG_IFACE_EVENTS()

DESCRIPTION
  Q: Who calls thsi function? add the description
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void dssnetAny_reg_iface_event_cbs
( 
  dssnetAny_sm_cb_type *instance_ptr,
  ps_iface_type        *iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Register for IFACE UP event.
  -------------------------------------------------------------------------*/
  if( 0 != ps_iface_event_cback_reg( iface_ptr,
                                     IFACE_UP_EV,
                                     instance_ptr->assoc_iface_up_cbuf))
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Could not reg global cback for evt %d",
                    IFACE_UP_EV);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE DOWN event.
  -------------------------------------------------------------------------*/
  if(0 != ps_iface_event_cback_reg( iface_ptr,
                                    IFACE_DOWN_EV,
                                    instance_ptr->assoc_iface_down_cbuf))
  {
    DATA_ERR_FATAL("Could not reg global cback for evt IFACE_DOWN_EV");
    return;
  }

  /*------------------------------------------------------------------------
    Register for IFACE CONFIGURING event.
  ------------------------------------------------------------------------*/
  if(0 != ps_iface_event_cback_reg( iface_ptr,
                                    IFACE_CONFIGURING_EV,
                                    instance_ptr->assoc_iface_config_cbuf))
  {
    DATA_ERR_FATAL("Could not reg global cback for evt IFACE_CONFIGURING_EV");
    return;
  }

} /* dssnetAny_reg_iface_event_cbs() */


/*===========================================================================
FUNCTION DSSNETANY_DEREG_IFACE_EVENTS()

DESCRIPTION
  Q: Who calls this function? add the description
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void dssnetAny_dereg_iface_event_cbs
( 
  dssnetAny_sm_cb_type *instance_ptr,
  ps_iface_type        *iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    DeRegister for IFACE UP event.
  -------------------------------------------------------------------------*/
  ps_iface_event_cback_dereg( iface_ptr,
                              IFACE_UP_EV,
                              instance_ptr->assoc_iface_up_cbuf);

  /*-------------------------------------------------------------------------
    DeRegister for IFACE DOWN event.
  -------------------------------------------------------------------------*/
  ps_iface_event_cback_dereg( iface_ptr,
                              IFACE_DOWN_EV,
                              instance_ptr->assoc_iface_down_cbuf);

  /*-------------------------------------------------------------------------
    DeRegister for IFACE CONFIGURING event.
  -------------------------------------------------------------------------*/
  ps_iface_event_cback_dereg(iface_ptr,
                             IFACE_CONFIGURING_EV,
                             instance_ptr->assoc_iface_config_cbuf);


} /* dssnetAny_dereg_iface_event_cbs() */


/*===========================================================================
FUNCTION DSSNETANY_REG_IFACE_UP_DOWN_EVENT_CBS()

DESCRIPTION
  Q: Who calls thsi function? add the description
  
DEPENDENCIES
  None.

RETURN VALUE
  0: on success
 -1: on failure
 
SIDE EFFECTS
  None.
===========================================================================*/
void dssnetAny_reg_iface_up_down_event_cbs
( 
  dssnetAny_sm_cb_type *instance_ptr,
  ps_iface_type        *iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Register for IFACE UP event.
  -------------------------------------------------------------------------*/
  if( 0 != ps_iface_event_cback_reg( iface_ptr,
                                     IFACE_UP_EV,
                                     instance_ptr->assoc_iface_up_cbuf))
  {
    DATA_ERR_FATAL("Could not reg global cback for evt IFACE_UP_EV");
    return;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE DOWN event.
  -------------------------------------------------------------------------*/
  if(0 != ps_iface_event_cback_reg( iface_ptr,
                                    IFACE_DOWN_EV,
                                    instance_ptr->assoc_iface_down_cbuf))
  {
    DATA_ERR_FATAL("Could not reg global cback for evt IFACE_DOWN_EV");
    return;
  }

} /* dssnetAny_reg_iface_up_down_event_cbs() */


/*===========================================================================
FUNCTION DSSNETANY_DEREG_IFACE_UP_DOWN_EVENT_CBS()

DESCRIPTION
  Q: Who calls this function? add the description
  
DEPENDENCIES
  None.

RETURN VALUE
  0: on success
 -1: on failure
 
SIDE EFFECTS
  None.
===========================================================================*/
void dssnetAny_dereg_iface_up_down_event_cbs
( 
  dssnetAny_sm_cb_type *instance_ptr,
  ps_iface_type        *iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    DeRegister for IFACE UP event.
  -------------------------------------------------------------------------*/
  ps_iface_event_cback_dereg( iface_ptr,
                              IFACE_UP_EV,
                              instance_ptr->assoc_iface_up_cbuf);

  /*-------------------------------------------------------------------------
    DeRegister for IFACE DOWN event.
  -------------------------------------------------------------------------*/
  ps_iface_event_cback_dereg( iface_ptr,
                              IFACE_DOWN_EV,
                              instance_ptr->assoc_iface_down_cbuf);

} /* dssnetAny_dereg_iface_up_down_event_cbs() */


/*===========================================================================
FUNCTION   DSSNETANYI_707_PKT_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface 
  (V4/V6) events occur on IS707_PKT_IFACE. Registraion is done for
  various events during the  Any Iface Bring Up/teardown.
  As part of handling appropriate event is posted to dssnetAny 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void dssnetAnyi_707_pkt_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data
)
{
  dssnetAny_sm_cb_type    *instance_ptr;
  dssnetAny_sm_event_type   dssnet_event;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  instance_ptr = (dssnetAny_sm_cb_type*)(user_data);

  if(this_iface_ptr == instance_ptr->pref_iface_ptr)
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "Iface event %d from preferred iface in state %d",
                    event, instance_ptr->state);
  }
  else
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "Iface event %d from non preferred iface in state %d",
                    event, instance_ptr->state);
  }

  /*-------------------------------------------------------------------------
    Make sure the interface onwhcih we are geetin iface events is teh current
    iface we are tryign to associate with / associated with. 
  -------------------------------------------------------------------------*/
  switch(event)
  {
    case IFACE_UP_EV:
      dssnet_event = DSSNETANY_IFACE_UP_EV;
      break;

    case IFACE_DOWN_EV:
    {
      /*-----------------------------------------------------------------------
        If the down event happens and hte previous state was down ignore the
        event.
      -----------------------------------------------------------------------*/
      if(event_info.iface_down_info.state == IFACE_DOWN)
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED, "Iface Down event ignored as iface was already down!");
        return;
      }

      /*-----------------------------------------------------------------------
        The down event has sematics the depend on the state in which it is
        received.  It usually generates the IFACE_DOWN event except under the
        following circumstances: in FAILOVER, HANDDOWN and OPEN states as
        well as the OPENING state IFF the phys_link is down.  Under thos
        circumstances it generates the CLOSE event.
      -----------------------------------------------------------------------*/
      dssnet_event = DSSNETANY_IFACE_DOWN_EV;

      switch(instance_ptr->state)
      {
        case DSSNETANY_OPENING_STATE:
          if(ps_iface_phys_link_is_gone(instance_ptr->assoc_iface_ptr) == FALSE)
          {
            break;
          }      
          /* fall through */

        case DSSNETANY_OPEN_STATE:
        case DSSNETANY_HANDDOWN_STATE:
        case DSSNETANY_FAILOVER_STATE:
          dssnet_event = DSSNETANY_CLOSE_EV;
          break;

        default:
          break;
      }
    } /* IFACE_DOWN_EV */
    break;

    case IFACE_CONFIGURING_EV:
      /*-----------------------------------------------------------------------
        If the down event happens and hte previous state was down ignore the
        event.
      -----------------------------------------------------------------------*/
      if(event_info.state == IFACE_CONFIGURING)
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED, "Iface Configuring event ignored as iface was already there!");
        return;
      }
      dssnet_event = DSSNETANY_CONFIGURING_EV;
      break;

    default:
      return;
  } /* switch( event ) */

  (void) dssnetAny_sm_post_event(instance_ptr,dssnet_event);
} /* dssnetAnyi_707_pkt_event_handler_cb() */


/*===========================================================================
FUNCTION DSSNETANYI_PROCESS_IFACE_UP_EV()

DESCRIPTION
  This function processes the DSSNETANY_IFACE_UP_EV event. 

  State will be changed to DSSNETANYI
   event and state transition description:

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnetAnyi_process_iface_up_ev
(
  dssnetAny_sm_cb_type *instance_ptr        /* dssnetAny instance ptr      */
)
{
  int16 ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "IFACE_UP_EV recd for DSSNetAny 0x%x in state %d",
                  instance_ptr, instance_ptr->state);

  switch(instance_ptr->state)
  {
    case DSSNETANY_HANDUP_STATE:
    {
      if( (ps_iface_active_associate( &(instance_ptr->local_iface),
                                      instance_ptr->pref_iface_ptr,
                                      TRUE,
                                      &ps_errno)) < 0)
      {
        if( ps_errno != DS_EWOULDBLOCK )
        {
          /*-----------------------------------------------------------------
            Assoc failed, post appropriate event to cleanup  DSSNetAny
            Check with Marcello.
          -----------------------------------------------------------------*/
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "UM bringup failed, posting UM_IFACE_DOWN");
          (void) dssnetAny_sm_post_event(instance_ptr,DSSNETANY_IFACE_DOWN_EV);
          break;
        }
      }
      instance_ptr->assoc_iface_ptr = instance_ptr->pref_iface_ptr;
      ps_iface_event_cback_dereg(instance_ptr->non_pref_iface_ptr,
                                 IFACE_CONFIGURING_EV,
                                 instance_ptr->assoc_iface_config_cbuf);

      if(0 > ps_iface_event_cback_reg(instance_ptr->pref_iface_ptr,
                                      IFACE_CONFIGURING_EV,
                                      instance_ptr->assoc_iface_config_cbuf))
      {
        DATA_ERR_FATAL("Could not reg global cback for evt IFACE_CONFIGURING_EV");
        return;
      }

      /*---------------------------------------------------------------------
        As we have done a bringup on the pref_iface in the handup state, and
        we just did an active_associate to the same - we need to do a
        tear_down() on it to make sure the reference counts are correct.
      ---------------------------------------------------------------------*/
      if(ps_iface_tear_down_cmd(instance_ptr->assoc_iface_ptr,
                                &ps_errno,
                                NULL) < 0)
      {
        ERR_FATAL("tear_down() failed when it should have succeeded!",0,0,0);
      }
    }
    /* fall thru */

    case DSSNETANY_HANDDOWN_STATE:
    case DSSNETANY_OPENING_STATE:
    case DSSNETANY_HANDOVER_STATE:
    case DSSNETANY_FAILOVER_STATE:
    {
      dssnetAnyi_transition_state(instance_ptr,
                                  DSSNETANY_OPEN_STATE);
      break;
    }

    case DSSNETANY_OPEN_STATE:
    case DSSNETANY_CLOSED_STATE:
    case DSSNETANY_CLOSING_STATE:
    {
      /* Ignoring IFACE_UP_EV */
      break;
    }

    default:
      /* Invalid state */
      ASSERT(0);
      break;
  } /* switch */

} /* dssnetAnyi_process_iface_up_ev() */

/*===========================================================================
FUNCTION DSSNETANYI_PROCESS_IFACE_DOWN_EV()

DESCRIPTION
  This function processes the DSSNETANY_IFACE_DOWN_EV event. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnetAnyi_process_iface_down_ev
(
  dssnetAny_sm_cb_type *instance_ptr        /* dssnetAny instance ptr      */
)
{
  ppp_dev_opts_type ppp_config;
  int16 ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "IFACE_DOWN_EV rcvd for DSSNetAny 0x%x in state %d",
                  instance_ptr, instance_ptr->state);

  switch(instance_ptr->state)
  {
    case DSSNETANY_OPENING_STATE:
      dssnetAny_dereg_iface_event_cbs(instance_ptr,
                                      instance_ptr->assoc_iface_ptr);
      dssnetAnyi_transition_state(instance_ptr,DSSNETANY_FAILOVER_STATE);
      break;

    case DSSNETANY_HANDUP_STATE:
    {
      /*-----------------------------------------------------------------------
        Setup PPP configuration options as they are on the Sn interface.
      -----------------------------------------------------------------------*/
      (void) ppp_get_dev_setup(PPP_UM_SN_DEV, &ppp_config);
      (void) ppp_get_protocol_opts(PPP_UM_SN_DEV, &ppp_config);

      /*-----------------------------------------------------------------------
        We want to start PPP on the associated (non-preferred) interface so
        enable the non-preferred NCP and disable the preferred NCP.  If the
        associated interface is v4 then the non-pref NCP is IPCP and visa versa
      -----------------------------------------------------------------------*/
      if(ps_iface_addr_family_is_v4(instance_ptr->assoc_iface_ptr))
      {
        ppp_config.ipv6cp_info.mode = PPP_CP_MODE_DISCARD;
        ppp_config.ipcp_info.mode   = PPP_CP_MODE_ACTIVE;
      }
      else
      {
        ppp_config.ipcp_info.mode   = PPP_CP_MODE_DISCARD;
        ppp_config.ipv6cp_info.mode = PPP_CP_MODE_ACTIVE;
      }

      if (ds707_roaming_is_curr_mode_jcdma() == TRUE)
      {
        ds707_configure_VJ_compression_for_PPP(&ppp_config);
        ds707_configure_jcdma_params_for_ppp(&ppp_config);
      }
      /*Simplified JCDMA: 
        5.1.26 Enable VJ compression   [D1-No.78]*/
      else if(ds707_roaming_is_simplified_jcdma_mode())
      {
        ds707_configure_VJ_compression_for_PPP(&ppp_config);
      }

      (void) ppp_start(PPP_UM_SN_DEV,&ppp_config);
      dssnetAnyi_transition_state(instance_ptr,DSSNETANY_HANDDOWN_STATE);
    }
    break;

    case DSSNETANY_HANDOVER_STATE:
    {
      if((ps_iface_active_associate(&(instance_ptr->local_iface),
                                    instance_ptr->non_pref_iface_ptr,
                                    TRUE,
                                    &ps_errno)) <0)
      {
        if( ps_errno != DS_EWOULDBLOCK )
        {
          /*-------------------------------------------------------------------
            Assoc failed, post appropriate event to cleanup  DSSNetAny
            Check with Marcello.
          -------------------------------------------------------------------*/
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "UM bringup failed, posting UM_IFACE_DOWN");
          (void) dssnetAny_sm_post_event(instance_ptr,DSSNETANY_CLOSE_EV);
          break;
        }
      }

      instance_ptr->assoc_iface_ptr = instance_ptr->non_pref_iface_ptr;
      ps_iface_event_cback_dereg(instance_ptr->pref_iface_ptr,
                                 IFACE_CONFIGURING_EV,
                                 instance_ptr->assoc_iface_config_cbuf);

      if(0 > ps_iface_event_cback_reg(instance_ptr->non_pref_iface_ptr,
                                      IFACE_CONFIGURING_EV,
                                      instance_ptr->assoc_iface_config_cbuf))
      {
        DATA_ERR_FATAL("Could not reg global cback for evt IFACE_CONFIGURING_EV");
        return;
      }

      dssnetAnyi_transition_state(instance_ptr,DSSNETANY_HANDDOWN_STATE);
    } /* DSSNETANY_HANDOVER_STATE */
    break;

    case DSSNETANY_CLOSING_STATE:
      /*-----------------------------------------------------------------------
        Teardown complete - transition to CLOSED state.
      -----------------------------------------------------------------------*/
      dssnetAnyi_transition_state(instance_ptr,DSSNETANY_CLOSED_STATE);
      break;

    case DSSNETANY_CLOSED_STATE:
      /*-----------------------------------------------------------------------
        Ignore event.
      -----------------------------------------------------------------------*/
      break;

    case DSSNETANY_FAILOVER_STATE:
    case DSSNETANY_OPEN_STATE:
    case DSSNETANY_HANDDOWN_STATE:
    default:
      ASSERT(0);
      break;
  } /* switch */

} /* dssnetAnyi_process_iface_down_ev() */

/*===========================================================================
FUNCTION DSSNETANYI_PROCESS_CONFIGURING_EV()

DESCRIPTION
  This function processes the DSSNETANY_CONFIGURING_EV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnetAnyi_process_handoff_ev
(
  dssnetAny_sm_cb_type *instance_ptr            /* dssnetAny instance ptr  */
)
{
  ps_iface_type *assoc_iface;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "CONFIGURING_EV recd for DSSNetAny 0x%x in state %d",
                  instance_ptr, instance_ptr->state);

  switch(instance_ptr->state)
  {
    case DSSNETANY_OPEN_STATE:
    case DSSNETANY_HANDDOWN_STATE:
    {
      ps_iface_configuring_ind(&(instance_ptr->local_iface));
      assoc_iface = PS_IFACE_GET_ASSOC_IFACE(&(instance_ptr->local_iface));

      if(assoc_iface == instance_ptr->pref_iface_ptr)
      {
        dssnetAnyi_transition_state(instance_ptr,DSSNETANY_HANDOVER_STATE);
      } 
      else if(assoc_iface == instance_ptr->non_pref_iface_ptr)
      {
        dssnetAnyi_transition_state(instance_ptr,DSSNETANY_HANDUP_STATE);
      }
      else
      {
        DATA_ERR_FATAL("Associated Iface is not pref or non-pref Iface");
        return;
      }
      break;
    }

    case DSSNETANY_FAILOVER_STATE:
    case DSSNETANY_OPENING_STATE:
    {
      /*---------------------------------------------------------------------
        We get a config event when we start MIP registration. So, we need to 
        ignore this event here.  We need to come up with some other way to 
        handle a PDSN handoff when PPP is in the process of being established
        We also get a configuring EV when dssnet6 begins registration in 
        DSSNETANY_OPENING_STATE. Because we need to send an RS the v6 iface 
        must be in configuring state.
      ---------------------------------------------------------------------*/
      break;
    }

    case DSSNETANY_CLOSED_STATE:
    case DSSNETANY_HANDUP_STATE:
    case DSSNETANY_HANDOVER_STATE:
    case DSSNETANY_CLOSING_STATE:
    {
      /* Ignore */
      break;
    }

    default:
      ASSERT(0);
      break;
  } /* end switch */

} /* dssnetAnyi_process_handoff_ev() */


/*===========================================================================
FUNCTION DSSNETANYI_PROCESS_OPEN_EV()

DESCRIPTION
  This function processes the DSSNETANY_OPEN_EV. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnetAnyi_process_open_ev
(
  dssnetAny_sm_cb_type *instance_ptr            /* dssnetAny instance ptr  */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "OPEN_EV recd for DSSNetAny 0x%x in state %d",
                  instance_ptr, instance_ptr->state);

  switch(instance_ptr->state)
  {
    /*lint -save -e655*/
    case DSSNETANY_CLOSED_STATE:
    {
      (void) ppp_failover_enable(PPP_UM_SN_DEV,instance_ptr->ppp_failover_mode);

      if( ( instance_ptr->any_pref_mode == DSSNETANY_IFACE_PREFERRED) &&
          ( !(ps_iface_state(instance_ptr->pref_iface_ptr) &
             (IFACE_ROUTEABLE | IFACE_UP | IFACE_GOING_DOWN)) ) &&
          ( !(ps_iface_state(instance_ptr->non_pref_iface_ptr) &
             (IFACE_DISABLED | IFACE_DOWN | IFACE_GOING_DOWN)) ) )
      {
        dssnetAnyi_transition_state(instance_ptr, DSSNETANY_FAILOVER_STATE);
      }
      else
      {
        dssnetAnyi_transition_state(instance_ptr, DSSNETANY_OPENING_STATE);
      }
      break;
    }
    /*lint -restore Restore lint error 655*/

    case DSSNETANY_OPENING_STATE:
    case DSSNETANY_OPEN_STATE:
    case DSSNETANY_HANDUP_STATE:
    case DSSNETANY_HANDOVER_STATE:
    case DSSNETANY_HANDDOWN_STATE:
    case DSSNETANY_FAILOVER_STATE:
    case DSSNETANY_CLOSING_STATE:
    {
      /* Ignore */
      break;
    }

    default:
      break;
  } /* end switch */

} /* dssnetAnyi_process_open_ev() */


/*===========================================================================
FUNCTION DSSNETANYI_PROCESS_TEARDOWN_EV()

DESCRIPTION
  This function processes the DSSNETANY_TEARDOWN_EV. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnetAnyi_process_teardown_ev
(
  dssnetAny_sm_cb_type *instance_ptr            /* dssnetAny instance ptr  */
)
{
  ps_iface_type *assoc_iface;
  int result;
  int16 ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "TEARDOWN_EV recd for DSSNetAny 0x%x in state %d",
                  instance_ptr, instance_ptr->state);

  switch(instance_ptr->state)
  {
    case DSSNETANY_OPEN_STATE:
    case DSSNETANY_OPENING_STATE:
    case DSSNETANY_HANDUP_STATE:
    case DSSNETANY_HANDOVER_STATE:
    case DSSNETANY_HANDDOWN_STATE:
    case DSSNETANY_FAILOVER_STATE:
    {
      (void) ppp_failover_disable(PPP_UM_SN_DEV);
      assoc_iface = PS_IFACE_GET_ASSOC_IFACE(&(instance_ptr->local_iface));
      instance_ptr->assoc_iface_ptr = assoc_iface;

      ASSERT(assoc_iface != NULL);

      result = ps_iface_active_disassociate(
        &(instance_ptr->local_iface),
        &ps_errno,
        assoc_iface->client_data_ptr);

      if( (ps_iface_passive_associate(&(instance_ptr->local_iface),
                                      instance_ptr->pref_iface_ptr,
                                      TRUE,
                                      &ps_errno)) < 0)
      {
        ERR_FATAL("Can't passively assoc ANY iface.",0,0,0);
      }
      if(result == 0)
      {
        dssnetAnyi_transition_state(instance_ptr,DSSNETANY_CLOSED_STATE);
      }
      else if(ps_errno == DS_EWOULDBLOCK) 
      {
        dssnetAnyi_transition_state(instance_ptr,DSSNETANY_CLOSING_STATE);
      }
      else
      {
        DATA_IS707_MSG2(MSG_LEGACY_ERROR, "result = %d .ps_errno = %d",
                        result,ps_errno);
        ASSERT(0);
        return;
      }
      break;
    }

    case DSSNETANY_CLOSED_STATE:
    case DSSNETANY_CLOSING_STATE:
    {
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      break;
    }

    default:
      ASSERT(0);
      break;
  } /* end switch */

} /* dssnetAnyi_process_teardown_ev() */


/*===========================================================================
FUNCTION DSSNETANYI_PROCESS_CLOSE_EV()

DESCRIPTION
  This function processes the DSSNETANY_CLOSE_EV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnetAnyi_process_close_ev
(
  dssnetAny_sm_cb_type *instance_ptr            /* dssnetAny instance ptr  */
)
{
  ps_iface_type *assoc_iface;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "CLOSE_EV recd for DSSNetAny 0x%x in state %d",
                  instance_ptr, instance_ptr->state);

  switch(instance_ptr->state)
  {
    case DSSNETANY_OPEN_STATE:
    {
      /*---------------------------------------------------------------------
        Deregister for IFACE CONFIGURING event.
      ---------------------------------------------------------------------*/
      assoc_iface = PS_IFACE_GET_ASSOC_IFACE(&(instance_ptr->local_iface));
      instance_ptr->assoc_iface_ptr =  assoc_iface;
      /* Fall through to transition to CLOSED state */
    }
        /* fall through */
    case DSSNETANY_OPENING_STATE:
    case DSSNETANY_HANDUP_STATE:
    case DSSNETANY_HANDOVER_STATE:
    case DSSNETANY_HANDDOWN_STATE:
    case DSSNETANY_FAILOVER_STATE:
    {
      (void) ppp_failover_disable(PPP_UM_SN_DEV);
      dssnetAnyi_transition_state(instance_ptr,DSSNETANY_CLOSED_STATE);
      break;
    }

    case DSSNETANY_CLOSING_STATE:
    case DSSNETANY_CLOSED_STATE:
    {
      break;
    }

    default:
      ASSERT(0);
      break;
  } /* end switch */

} /* dssnetAnyi_process_close_ev() */


/*===========================================================================
FUNCTION DSSNETANY_SM_PROCESS_EVENT()

DESCRIPTION
  This function is used to process events posted to the dssnet4 state 
  machine. 

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
void dssnetAny_sm_process_event
(
  ds_cmd_type *cmd_ptr                          /* Pointer to ds_cmd_type  */
)
{
  dssnetAny_sm_cb_type    *instance_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(cmd_ptr != NULL);
  ASSERT(cmd_ptr->cmd_payload_ptr != NULL);

  instance_ptr = (dssnetAny_sm_cb_type* ) cmd_ptr->cmd_payload_ptr;

  switch (cmd_ptr->hdr.cmd_id) 
  {
    case DS_CMD_DSSNETANY_IFACE_DOWN_EV:
    {
      dssnetAnyi_process_iface_down_ev(instance_ptr);
      break;
    }

    case DS_CMD_DSSNETANY_IFACE_UP_EV:
    {
      dssnetAnyi_process_iface_up_ev(instance_ptr);
      break;
    }

    case DS_CMD_DSSNETANY_CONFIGURING_EV:
    {
      dssnetAnyi_process_handoff_ev(instance_ptr);
      break;
    }

    case DS_CMD_DSSNETANY_OPEN_EV:
    {
      dssnetAnyi_process_open_ev(instance_ptr);
      break;
    }

    case DS_CMD_DSSNETANY_TEARDOWN_EV:
    {
      dssnetAnyi_process_teardown_ev(instance_ptr);
      break;
    }

    case DS_CMD_DSSNETANY_CLOSE_EV:
    {
      dssnetAnyi_process_close_ev(instance_ptr);
      break;
    }

    default:
      /*---------------------------------------------------------------------
        Sanity check
      ---------------------------------------------------------------------*/
      break;
  }/* end switch() */
              
  return;

} /* dssnetAny_sm_process_event() */


/*===========================================================================
FUNCTION DSSNETANYI_TRANSITION_STATE()

DESCRIPTION
  This function is called whenever the state changes.
  The variable is set and housekeeping done for each state.
  
  Any change to state should be through this function only.
  
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  Changes the state.
===========================================================================*/
static void dssnetAnyi_transition_state
(
  dssnetAny_sm_cb_type     *instance_ptr,      /* DSSNETANY SM instance    */
  dssnetAnyi_state_type     new_state          /*   new state to change to */
)
{
  dssnetAnyi_state_type old_state;             /* old state                */
  acl_policy_info_type  any_acl_policy_info;
  ps_iface_type        *um_iface_ptr;
  int16                 ps_errno;
  ps_iface_down_mh_event_info_type iface_down_ind_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Changing the state variable.
  -------------------------------------------------------------------------*/
  old_state = instance_ptr->state;
  DATA_IS707_MSG3(MSG_LEGACY_MED, "DSSNetANY 0x%x transition state %d to %d",
                  instance_ptr, old_state, new_state);
  instance_ptr->state = new_state;

  /*-------------------------------------------------------------------------
    Executing state based functionality.
  -------------------------------------------------------------------------*/
  switch (instance_ptr->state)
  {
  /*-------------------------------------------------------------------------
                                Closed State
  -------------------------------------------------------------------------*/
  case DSSNETANY_CLOSED_STATE:

    /* Stop the DHCP instance before giving down indication
      */
    (void) dshdr_stop_dhcp(&(instance_ptr->local_iface));

    dssnetAny_dereg_iface_event_cbs(instance_ptr,
                                    instance_ptr->assoc_iface_ptr);
    memset(&iface_down_ind_info, 0, sizeof(iface_down_ind_info));
    ds707_pkt_mgr_get_bearer_tech_info_ex( &iface_down_ind_info.bearer_tech);
    iface_down_ind_info.down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
    ps_iface_down_ind(&(instance_ptr->local_iface), &iface_down_ind_info);
    break;

  /*-------------------------------------------------------------------------
                                Opening State
  -------------------------------------------------------------------------*/
  case DSSNETANY_OPENING_STATE:
  {
    memset( &any_acl_policy_info, 0, sizeof(acl_policy_info_type) );
    acl_init_policy_info(&any_acl_policy_info);

    any_acl_policy_info.iface.kind      = DSS_IFACE_NAME;
    any_acl_policy_info.iface.info.name = (dss_iface_name_enum_type) CDMA_SN_IFACE;
    any_acl_policy_info.policy_flag     = DSS_IFACE_POLICY_ANY;
    any_acl_policy_info.proc_id    = 0;
    any_acl_policy_info.bring_up        = TRUE;
    if(ps_iface_addr_family_is_v6(instance_ptr->pref_iface_ptr))
    {
      any_acl_policy_info.ip_family       = IFACE_IPV6_ADDR_FAMILY;
    }
    else if(ps_iface_addr_family_is_v4(instance_ptr->pref_iface_ptr))
    {
      any_acl_policy_info.ip_family       = IFACE_IPV4_ADDR_FAMILY;
    }
    else
    {
      DATA_ERR_FATAL("Preferred Iface is not IPv4 or IPv6 addr family");
      return;
    }

    um_iface_ptr = NULL;

    if(route_get(NULL,
                 &any_acl_policy_info,
                 FALSE,
                 NULL,
                 &um_iface_ptr ) < 0 )
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "route_get: assoc iface not available");
      /*----------------------------------------------------------------------
       Assoc ifaec currently in use, close ANY iface
      ----------------------------------------------------------------------*/
      (void) dssnetAny_sm_post_event(instance_ptr,DSSNETANY_CLOSE_EV);
      return;
    }
    
    ps_iface_configuring_ind(&(instance_ptr->local_iface));

    if((ps_iface_active_associate(&(instance_ptr->local_iface),
                                  instance_ptr->pref_iface_ptr,
                                  TRUE,
                                  &ps_errno)) < 0 &&
       ps_errno != DS_EWOULDBLOCK)
    {
      /*---------------------------------------------------------------------
        Assoc failed, post appropriate event to cleanup  DSSNetAny
        Check with Marcello.
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "UM bringup failed, posting UM_IFACE_DOWN");
      (void) dssnetAny_sm_post_event(instance_ptr,DSSNETANY_CLOSE_EV);
    }
    else
    {
      instance_ptr->assoc_iface_ptr = instance_ptr->pref_iface_ptr;
      dssnetAny_reg_iface_event_cbs(instance_ptr,
                                    instance_ptr->assoc_iface_ptr);
    }
  } /* Opening State */
  break;

  /*-------------------------------------------------------------------------
                                 Open State
  -------------------------------------------------------------------------*/
  case DSSNETANY_OPEN_STATE:
    if(instance_ptr->assoc_iface_ptr == instance_ptr->pref_iface_ptr)
    {
      DATA_IS707_MSG4(
            MSG_LEGACY_MED,
            "In Open state, Any if %x:%d assoc with preferred if %x:%d!",
            instance_ptr->local_iface.name,
            instance_ptr->local_iface.instance,
            instance_ptr->assoc_iface_ptr->name,
            instance_ptr->assoc_iface_ptr->instance);
    }
    else
    {
      DATA_IS707_MSG4(
            MSG_LEGACY_MED,
            "In Open state, Any if %x:%d assoc with non preferred if %x:%d!",
            instance_ptr->local_iface.name,
            instance_ptr->local_iface.instance,
            instance_ptr->assoc_iface_ptr->name,
            instance_ptr->assoc_iface_ptr->instance);
    }

    /* Send IFACE UP indication
     */
    ps_iface_up_ind(&(instance_ptr->local_iface));

    dshdr_dhcp_trigger_negotiation (instance_ptr->pref_iface_ptr);    

    break;

  /*-------------------------------------------------------------------------
                                Handup State
  -------------------------------------------------------------------------*/
  case DSSNETANY_HANDUP_STATE:
    dssnetAny_dereg_iface_up_down_event_cbs(
      instance_ptr,
      instance_ptr->non_pref_iface_ptr);

    if(ps_iface_bring_up_cmd(instance_ptr->pref_iface_ptr,&ps_errno,NULL) < 0
       &&
       ps_errno != DS_EWOULDBLOCK)
    {
      (void) dssnetAny_sm_post_event(instance_ptr, DSSNETANY_CLOSE_EV);
    }
    else
    {
      dssnetAny_reg_iface_up_down_event_cbs(instance_ptr,
                                            instance_ptr->pref_iface_ptr);
    }
    break;

  /*-------------------------------------------------------------------------
                               Handdown State
  -------------------------------------------------------------------------*/
  case DSSNETANY_HANDDOWN_STATE:
    dssnetAny_dereg_iface_up_down_event_cbs(instance_ptr,
                                            instance_ptr->pref_iface_ptr);
    dssnetAny_reg_iface_up_down_event_cbs(instance_ptr,
                                          instance_ptr->non_pref_iface_ptr);
    break;

  /*-------------------------------------------------------------------------
                               Failover State
  -------------------------------------------------------------------------*/
  case DSSNETANY_FAILOVER_STATE:
  {
    memset( &any_acl_policy_info, 0, sizeof(acl_policy_info_type) );
    acl_init_policy_info(&any_acl_policy_info);

    any_acl_policy_info.iface.kind      = DSS_IFACE_NAME;
    any_acl_policy_info.iface.info.name = (dss_iface_name_enum_type) CDMA_SN_IFACE;
    any_acl_policy_info.policy_flag     = DSS_IFACE_POLICY_ANY;
    any_acl_policy_info.ip_family       = IFACE_ANY_ADDR_FAMILY;
    any_acl_policy_info.proc_id    = 0;

    if(ps_iface_addr_family_is_v6(instance_ptr->non_pref_iface_ptr))
    {
      any_acl_policy_info.ip_family = IFACE_IPV6_ADDR_FAMILY;
    }
    else if(ps_iface_addr_family_is_v4(instance_ptr->non_pref_iface_ptr))
    {
      any_acl_policy_info.ip_family = IFACE_IPV4_ADDR_FAMILY;
    }
    else
    {
      DATA_ERR_FATAL("Preferred Iface is not IPv4 or IPv6 addr family");
      return;
    }

    um_iface_ptr = NULL;

    if ( route_get( NULL,
                    &any_acl_policy_info,
                    FALSE,
                    NULL,
                    &um_iface_ptr ) < 0 )
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "route_get: assoc iface not available");
      /*----------------------------------------------------------------------
       Assoc iface currently in use, close ANY iface
      ----------------------------------------------------------------------*/
      (void) dssnetAny_sm_post_event(instance_ptr,DSSNETANY_CLOSE_EV);
      return;
    }

    if((ps_iface_active_associate(&(instance_ptr->local_iface),
                                  instance_ptr->non_pref_iface_ptr,
                                  TRUE,
                                  &ps_errno))<0)
    {
      if( ps_errno != DS_EWOULDBLOCK )
      {
        /*--------------------------------------------------------------------
          Assoc failed, post appropriate event to cleanup  DSSNetAny
          Check with Marcello.
        --------------------------------------------------------------------*/
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "UM bringup failed, posting UM_IFACE_DOWN");
        (void) dssnetAny_sm_post_event(instance_ptr,DSSNETANY_CLOSE_EV);
        break;
      }
    }
    instance_ptr->assoc_iface_ptr = instance_ptr->non_pref_iface_ptr;
    dssnetAny_reg_iface_event_cbs(instance_ptr,
                                  instance_ptr->assoc_iface_ptr);
  } /* failover state */
  break;

  /*-------------------------------------------------------------------------
    Handover and Closing States
  -------------------------------------------------------------------------*/
  case DSSNETANY_HANDOVER_STATE:
  case DSSNETANY_CLOSING_STATE:
    /*-----------------------------------------------------------------------
      Nothing to do : wait here for IFACE DOWN
    -----------------------------------------------------------------------*/
    break;

  default:
    ASSERT(0);
    break;
  } /* switch(state) */

} /* dssnetAnyi_transition_state() */



/*===========================================================================
FUNCTION DSSNETANYI_BRING_UP_CMD()

DESCRIPTION
   This function is registered with ps_iface to bring up the associated iface
   during call establishment.

PARAMETERS
  Pointer to ps_iface on which to operate
  Void ptr to client data

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int dssnetAnyi_bring_up_cmd
(
  ps_iface_type *this_iface_ptr,
  void *client_data
)
{
  dssnetAny_sm_cb_type    *instance_ptr;   /* Ptr to DSSNETANY SM instance */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure dssnetAny instance is initialized
  -------------------------------------------------------------------------*/
  instance_ptr = (dssnetAny_sm_cb_type*)(this_iface_ptr->client_data_ptr);
  if(DSSNETANY_SMI_IS_VALID(instance_ptr) == FALSE)
  {
    ERR_FATAL("Invalid instance 0x%p passed into any_bring_up_cmd()",
              (int)instance_ptr, 0, 0);
  }

  DATA_IS707_MSG3(MSG_LEGACY_HIGH, "Bring Up hdlr for ANY iface %x:%d",
             this_iface_ptr, this_iface_ptr->name, this_iface_ptr->instance);

  /*-------------------------------------------------------------------------
    Jump start the dssnetAny state m/c with OPEN event
  -------------------------------------------------------------------------*/
  (void) dssnetAny_sm_post_event(instance_ptr, DSSNETANY_OPEN_EV);

  return 0;

} /* dssnetAnyi_bring_up_cmd() */


/*===========================================================================
FUNCTION DSSNETANYI_TEAR_DOWN_CMD()

DESCRIPTION
   This function is registered with ps_iface to tear down the associated
   iface during call teardown.

PARAMETERS
  Pointer to ps_iface on which to operate
  Void ptr to client data

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int dssnetAnyi_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  void *client_data
)
{
  dssnetAny_sm_cb_type     *instance_ptr;  /* Ptr to DSSNETANY SM instance */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure dssnetAny instance is initialized
  -------------------------------------------------------------------------*/
  instance_ptr = (dssnetAny_sm_cb_type*)(this_iface_ptr->client_data_ptr);
  if(DSSNETANY_SMI_IS_VALID(instance_ptr) == FALSE)
  {
    ERR_FATAL("Invalid instance 0x%p passed into any_tear_down_cmd()",
              (int)instance_ptr, 0, 0);
  }

  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "Tear down hdlr for ANY iface %x:%d",
                  this_iface_ptr->name, this_iface_ptr->instance);

  /*-------------------------------------------------------------------------
    Start bringing down the logical iface and the associated iface.
  -------------------------------------------------------------------------*/
  (void) dssnetAny_sm_post_event(instance_ptr,
                          DSSNETANY_TEARDOWN_EV);

  return 0;

} /* dssnetAnyi_tear_down_cmd() */

static int dssnetAnyi_iface_ioctl_hdlr
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ps_iface_type * assoc_iface_ptr;
  
  if(ps_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "NULL dss_errno");
    return -1;
  }
  
  if( !(PS_IFACE_IS_VALID(iface_ptr)) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid iface_ptr");
    *ps_errno = DS_EINVAL;
    return -1;
  }
  
  assoc_iface_ptr = PS_IFACE_GET_ASSOC_IFACE(iface_ptr);
  
  if( !(PS_IFACE_IS_VALID(assoc_iface_ptr)) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid associated iface_ptr");
    *ps_errno = DS_EINVAL;
    return -1;
  }

  return assoc_iface_ptr->iface_ioctl_f_ptr(assoc_iface_ptr,
                                            ioctl_name,
                                            argval_ptr,
                                            ps_errno);

} /* dssnetAny_iface_ioctl_hdlr() */

#endif /* FEATURE_DATA_PS_IPV6 */
