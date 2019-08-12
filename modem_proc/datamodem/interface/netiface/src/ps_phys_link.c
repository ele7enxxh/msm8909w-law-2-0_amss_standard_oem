
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ P H Y S _ L I N K . C

DESCRIPTION
  File defining all of the data types and the interface control block
  for the ps physical link architecture.

EXTERNAL FUNCTIONS

  PS_PHYS_LINK_CREATE()
    Used to create ps_phys_links.

  PS_PHYS_LINK_UP_CMD()
    Used to bring up the phys link.  Can be called by external entity.

  PS_PHYS_LINK_DOWN_CMD()
    Used to tear down the phys link.  Can be called by external entity.

  PS_PHYS_LINK_ENABLE_FLOW()
    Enable data flow on the phys link.  User passes in a mask indicating the
    flow control bit being enabled.

  PS_PHYS_LINK_DISABLE_FLOW()
    Disable data flow on the phys link.  User passes in a mask indicating the
    flow control bit being disabled.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None for the module.  Each phys_link is created by calling
  ps_phys_link_create().

Copyright (c) 2003-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_phys_link.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/30/11    su     Adding log code support for DPL logging  
06/18/08    pp     Metainfo optimizations.
02/27/07    scb    Fixed HIGH lint errors
02/09/07    ac     EMPA fix the cleanup issue for link protocol.
02/01/07    msr    Added ps_phys_sdb_status_handler()
12/12/06    ifk    Featurized IPHC and ROHC code.
09/12/06    msr    Removed redundant state field in event_info structure
06/05/06    mp     Moved DPL link logging control block from ps_iface to
                   ps_phys_link
04/25/06    msr    L4/Tasklock code review changes
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
01/17/06    ssh    Cast assignments for ppp_input and ppp_output
12/23/05    rt     Changed one F3 message in ps_phys_link_tx_cmd from HIGH
                   to LOW.
12/09/05    sv     Added support for new data path framework.
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/15/05    msr    Removed support for flow logging. Using ps_flows instead
08/12/05    sv     Return ENETGOINGDORMANT when bringup command is called in
                   GOING_DOWN state.
05/12/05    mct    Lint changes.
04/17/05    msr    Moved support for QOS to ps_flow.
04/16/05    ks     Changes due to addition of PHYS_LINK_NULL state
01/12/05    msr    Added code review comments.
01/10/05    msr    Added support for flow logging.
01/08/05    msr    Added ps_phys_link_get_first_rx_fltr(),
                   ps_phys_link_get_rx_filter_handle(), and
                   ps_phys_link_get_rx_filter_by_handle().
12/02/04    msr    Initializing phys link's state to PHYS_LINK_DOWN when it
                   is created.
11/02/04    mct    Added QOS parameter validation.
10/31/04   msr/ks  Changed the variable name in the data protocol logging
                   control block which is initialized in
                   ps_phys_link_create().
10/14/04   ks/msr  Added support for Data Protocol Logging.
08/12/04    sv     Flow control the physlink when we are bringing up/tearing
                   down the physlink.
05/10/04    mct    Fixed lint errors.
02/13/04    ak     Moved sdb interface into this file.
12/20/03    ak     Updated for phys_link/iface separation.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"

#include "ds_flow_control.h"
#include "dserrno.h"
#include "err.h"
#include "msg.h"
#include "ps_ifacei_event.h"
#include "ps_phys_link.h"
#include "ps_pkt_info.h"
#include "ps_phys_linki_event.h"

#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ps_system_heap.h"
#include "ds_Utils_DebugMsg.h"

#include "ps_logging_diag.h"

/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Data type used in PS_PHYS_LINK_FLOW_CTRL_CMD
---------------------------------------------------------------------------*/
typedef struct
{
  ps_phys_link_type  * ps_phys_link_ptr;
  uint32               flow_mask;
  boolean              is_enabled;
} ps_phys_linki_flow_ctrl_cmd_info_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
void ps_phys_linki_default_tx_cmd
(
  ps_phys_link_type     * this_phys_link_ptr,
  dsm_item_type        ** pkt_ref_ptr,
  ps_tx_meta_info_type  * meta_info_ptr,
  void                  * tx_cmd_info
);


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_PHYS_LINK_CREATE()

DESCRIPTION
  This will memset()s the private data struct to 0, initializes the
  "this_phys_link_ptr" to point to the interface control block itself,
  and initializes the queues.

PARAMETERS
  this_phys_link_ptr: Ptr to interface control blocks on which to operate on.
  n_links:            Number of phys links to create.

RETURN VALUE
  -1 - error in phys link creation
   0 - on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_create
(
  ps_phys_link_type      *this_phys_link_ptr,
  uint8                   n_links
)
{
  uint8 instance;
  int loop;
  ps_phys_link_type     *curr_phys_link_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(this_phys_link_ptr == NULL || n_links == 0)
  {
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    No need to enter critical section as phys links are created at power up
    and PS knows about phys links only after they are created and until then
    only DS context accesses them
  -------------------------------------------------------------------------*/

  for(instance=0; instance < n_links; instance++)
  {
    /*-----------------------------------------------------------------------
      Make sure that the phys link has not already been created.
    -----------------------------------------------------------------------*/
    curr_phys_link_ptr = &(this_phys_link_ptr[instance]);

    if (curr_phys_link_ptr->phys_private.this_phys_link == curr_phys_link_ptr)
    {
      continue;
    }

    LOG_MSG_INFO2_2("ps_phys_link_create(): "
                    "Creating phys link 0x%p, instance %d",
                    curr_phys_link_ptr, instance);

    /*-----------------------------------------------------------------------
      Reset the private info and initialize all of the queue structures
    -----------------------------------------------------------------------*/
    memset(&(curr_phys_link_ptr->phys_private),
           0,
           sizeof(curr_phys_link_ptr->phys_private));
    curr_phys_link_ptr->phys_private.state = PHYS_LINK_NULL;

    /*-----------------------------------------------------------------------
      clear out al logging related fields
    -----------------------------------------------------------------------*/
    memset(&(curr_phys_link_ptr->dpl_link_cb),
           0,
           sizeof(curr_phys_link_ptr->dpl_link_cb));

    /*-----------------------------------------------------------------------
      Initialize the event queues.
    -----------------------------------------------------------------------*/
    for (loop = 0; loop < MAX_EVENT_QUEUES; loop++)
    {
      (void)q_init((curr_phys_link_ptr->phys_private.event_q_array) + loop);
    }

    curr_phys_link_ptr->phys_private.this_phys_link = curr_phys_link_ptr;
    curr_phys_link_ptr->phys_private.instance       = instance;
    curr_phys_link_ptr->dormancy_info_code          = PS_EIC_NETWORK_NOT_SPECIFIED;

    memset (&curr_phys_link_ptr->event_info_cache,
            0,
            sizeof (phys_link_event_info_cache_type));
    curr_phys_link_ptr->event_info_cache.phys_link_event_info.state =
      PHYS_LINK_NULL;
      
    PS_PHYS_LINK_RESET_BEARER_ID(&this_phys_link_ptr[0]);   
  }

  PS_PHYS_LINKI_SET_CAPABILITY(&this_phys_link_ptr[0],
                               PS_PHYS_LINK_CAPABILITY_PRIMARY);

#ifdef FEATURE_DATACOMMON_PS_IFACE_IO
  this_phys_link_ptr[0].tx_f_ptr = ps_phys_linki_default_tx_cmd;
#else
  this_phys_link_ptr[0].tx_f_ptr = NULL;
#endif

  return 0;

} /* ps_phys_link_create() */


/*===========================================================================
                      BRING UP/TEAR DOWN COMMANDS
===========================================================================*/
/*===========================================================================
FUNCTION PS_PHYS_LINK_UP_CMD()

DESCRIPTION
  Brings up the physical link

PARAMETERS
  this_phys_link_ptr: ptr to interface control block on which to operate on.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
   0: on success
  -1: on failure (which includes DS_EWOULDBLOCK)

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_up_cmd
(
  ps_phys_link_type *this_phys_link_ptr,
  int16             *ps_errno,
  void              *client_data_ptr
)
{
  ps_iface_event_info_u_type        event_info;
  int                               ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_phys_link_up_cmd(): NULL parameter is passed");
    return -1;
  }

  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    ASSERT(0);
    return(ret_val);
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  LOG_MSG_INFO2_2("ps_phys_link_up_cmd(): "
                  "PS PHYS LINK UP CMD 0x%p in state %d",
                  this_phys_link_ptr,
                  PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr));

  /*-------------------------------------------------------------------------
    Take action based on current phys link state
  -------------------------------------------------------------------------*/

  switch(PS_PHYS_LINK_GET_STATE(this_phys_link_ptr))
  {
    case PHYS_LINK_UP:
      /*---------------------------------------------------------------------
        Physical link is up: return success
      ---------------------------------------------------------------------*/
      ret_val = 0;
      break;

    case PHYS_LINK_COMING_UP:
    case PHYS_LINK_RESUMING:

      /*---------------------------------------------------------------------
        Physical link is coming up: return would block
      ---------------------------------------------------------------------*/
      *ps_errno = DS_EWOULDBLOCK;
      ret_val = -1;
      break;

    case PHYS_LINK_DOWN:

     /*----------------------------------------------------------------------
        Physical link is coming up from DORMANCY: if the bring up handler is
        registered set the state to coming up and call it.
      ---------------------------------------------------------------------*/
      if(this_phys_link_ptr->phys_link_up_cmd_f_ptr != NULL)
      {
        event_info.phys_link_event_info.state =
          PS_PHYS_LINK_GET_STATE(this_phys_link_ptr);
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;

        PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr, PHYS_LINK_RESUMING);

        if(this_phys_link_ptr->phys_link_up_cmd_f_ptr(this_phys_link_ptr,
                                                      client_data_ptr) < 0)
        {
          /*-----------------------------------------------------------------
            can't bring up the network for some reason, so return net no net
            and set the state to down
          -----------------------------------------------------------------*/
          PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr, PHYS_LINK_DOWN);
          *ps_errno = DS_ENETDOWN;
        }
        else
        {
          /*-----------------------------------------------------------------
            Need to disable flow so that DS_WRITE_EVENT is not posted in
            sockets layer when phys link is in this state
          -----------------------------------------------------------------*/
          ps_phys_link_disable_flow(this_phys_link_ptr,
                                    DS_FLOW_PHYS_LINK_MASK);
          ps_ifacei_invoke_event_cbacks(NULL,
                                        this_phys_link_ptr,
                                        PHYS_LINK_RESUMING_EV,
                                        event_info);

          /*-----------------------------------------------------------------
            bring up is in progress so return would block
          -----------------------------------------------------------------*/
          *ps_errno = DS_EWOULDBLOCK;
        }
      } /* if(f_ptr registered) */

      /*---------------------------------------------------------------------
        The handler is not registered, so return option not supported
      ---------------------------------------------------------------------*/
      else
      {
        *ps_errno = DS_EOPNOTSUPP;
      }
      ret_val = -1;
      break;

    case PHYS_LINK_NULL:

     /*----------------------------------------------------------------------
        Physical link is coming up from NULL state: if the bring up handler
        is registered set the state to coming up and call it.
      ---------------------------------------------------------------------*/
      if(this_phys_link_ptr->phys_link_up_cmd_f_ptr != NULL)
      {
        event_info.phys_link_event_info.state =
          PS_PHYS_LINK_GET_STATE(this_phys_link_ptr);
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;

        PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr, PHYS_LINK_COMING_UP);

        if(this_phys_link_ptr->phys_link_up_cmd_f_ptr(this_phys_link_ptr,
                                                      client_data_ptr) < 0)
        {
          /*-----------------------------------------------------------------
            can't bring up the network for some reason, so return net no net
            and set the state back to NULL
          -----------------------------------------------------------------*/
          PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr, PHYS_LINK_NULL);
          *ps_errno = DS_ENETNONET;
        }
        else
        {
          /*-----------------------------------------------------------------
            Need to disable flow so that DS_WRITE_EVENT is not posted in
            sockets layer when phys link is in this state
          -----------------------------------------------------------------*/
          ps_phys_link_disable_flow(this_phys_link_ptr,
                                    DS_FLOW_PHYS_LINK_MASK);
          ps_ifacei_invoke_event_cbacks(NULL,
                                        this_phys_link_ptr,
                                        PHYS_LINK_COMING_UP_EV,
                                        event_info
                                       );

          *ps_errno = DS_EWOULDBLOCK;
        }
      } /* if(f_ptr registered) */

      /*---------------------------------------------------------------------
        The handler is not registered, so return option not supported
      ---------------------------------------------------------------------*/
      else
      {
        *ps_errno = DS_EOPNOTSUPP;
      }
      ret_val = -1;
      break;

    case PHYS_LINK_GOING_DOWN:
      *ps_errno = DS_ENETGOINGDORMANT;
      ret_val = -1;
      break;

    case PHYS_LINK_GOING_NULL:
      /*---------------------------------------------------------------------
        Physical link is going down: return net close in progress
      ---------------------------------------------------------------------*/
      *ps_errno = DS_ENETCLOSEINPROGRESS;
      ret_val = -1;
      break;

    default:
      /*---------------------------------------------------------------------
        This should NEVER happen!
      ---------------------------------------------------------------------*/
      ASSERT(0);
      *ps_errno = DS_EOPNOTSUPP;
      ret_val = -1;
      break;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return(ret_val);

} /* ps_phys_link_up_cmd() */

 
/*===========================================================================
FUNCTION PS_PHYS_LINK_DOWN_CMD()

DESCRIPTION
  This function is used to make the physical link go dormant.

PARAMETERS
  this_phys_link_ptr: ptr to phys link to tear down
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_down_cmd
(
  ps_phys_link_type   *this_phys_link_ptr,
  int16               *ps_errno,
  void                *client_data_ptr
)
{
  ps_iface_event_info_u_type event_info;
  int ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_phys_link_down_cmd(): NULL parameter is passed");
    return -1;
  }

  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  LOG_MSG_INFO2_2("ps_phys_link_down_cmd(): "
                  "PS PHYS LINK DOWN CMD 0x%p in state %d",
                  this_phys_link_ptr,
                  PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr));

  switch(PS_PHYS_LINK_GET_STATE(this_phys_link_ptr))
  {
  case PHYS_LINK_DOWN:
    ret_val = 0;
    break;

  case PHYS_LINK_UP:
  case PHYS_LINK_COMING_UP:
  case PHYS_LINK_RESUMING:

    if(this_phys_link_ptr->phys_link_down_cmd_f_ptr != NULL)
    {
      event_info.phys_link_event_info.state =
          PS_PHYS_LINK_GET_STATE(this_phys_link_ptr);
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;

      PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr, PHYS_LINK_GOING_DOWN);

      if (this_phys_link_ptr->phys_link_down_cmd_f_ptr(this_phys_link_ptr,
                                                       client_data_ptr) < 0)
      {
        /*-------------------------------------------------------------------
          can't bring down the network for some reason, so return net no net
          and set the state back to
        -------------------------------------------------------------------*/
        PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr,
                                event_info.phys_link_event_info.state);
        /* *ps_errno = ; //what error?  */

      }
      else
      {
        /*-------------------------------------------------------------------
          Need to disable flow so that DS_WRITE_EVENT is not posted in
          sockets layer when phys link is in this state
        -------------------------------------------------------------------*/
        ps_phys_link_disable_flow(this_phys_link_ptr,
                                  DS_FLOW_PHYS_LINK_MASK);
        ps_ifacei_invoke_event_cbacks(NULL,
                                      this_phys_link_ptr,
                                      PHYS_LINK_GOING_DOWN_EV,
                                      event_info
                                     );
        *ps_errno = DS_EWOULDBLOCK;
      }
    }
    else
    {
      *ps_errno = DS_EOPNOTSUPP;
    }
    ret_val = -1;
    break;

  case PHYS_LINK_GOING_DOWN:
    *ps_errno = DS_EWOULDBLOCK;
    ret_val = -1;
    break;

  case PHYS_LINK_GOING_NULL:
    *ps_errno = DS_ENETCLOSEINPROGRESS;
    ret_val = -1;
    break;

  case PHYS_LINK_NULL:
    *ps_errno = DS_ENETDOWN;
    ret_val = -1;
    break;

  default:
    ASSERT(0);
    *ps_errno = DS_EOPNOTSUPP;
    ret_val = -1;
    break;

  } /* switch(state) */

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ps_phys_link_down_cmd() */

/*===========================================================================
FUNCTION PS_PHYS_LINK_GO_NULL_CMD()

DESCRIPTION
  This function is used to tear down the physical link.

PARAMETERS
  this_phys_link_ptr: ptr to phys link to tear down
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_go_null_cmd
(
  ps_phys_link_type   *this_phys_link_ptr,
  int16               *ps_errno,
  void                *client_data_ptr
)
{
  ps_iface_event_info_u_type event_info;
  int ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_phys_link_go_null_cmd(): NULL parameter is passed");
    return -1;
  }

  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  LOG_MSG_INFO2_2("ps_phys_link_go_null_cmd(): "
                  "PS PHYS LINK GO NULL CMD 0x%p in state %d",
                  this_phys_link_ptr,
                  PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr));

  /*-------------------------------------------------------------------------
    Return SUCCESS immediately if this phys link is still used by other
    clients. This check MUST not be performed if phys link is COUPLED as
    in that case flows are transparent to mode handlers and hence phys link's
    ref cnt will be 1
  -------------------------------------------------------------------------*/
  if (PS_PHYS_LINKI_GET_CAPABILITY(this_phys_link_ptr,
                                   PS_PHYS_LINK_CAPABILITY_FLOW_DECOUPLED))
  {
    if (PS_PHYS_LINKI_GET_REF_CNT(this_phys_link_ptr) > 0)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_INFO2_1("ps_phys_link_go_null_cmd(): "
                      "Flows are still bound to this phys link. Not tearing "
                      "phys link, 0x%p down", this_phys_link_ptr);
      return 0;
    }
  }

  switch (PS_PHYS_LINKI_GET_STATE(this_phys_link_ptr))
  {
  case PHYS_LINK_NULL:
    ret_val = 0;
    break;

  case PHYS_LINK_UP:
  case PHYS_LINK_COMING_UP:
  case PHYS_LINK_RESUMING:
  case PHYS_LINK_GOING_DOWN:
  case PHYS_LINK_DOWN:

    if(this_phys_link_ptr->phys_link_go_null_cmd_f_ptr != NULL)
    {
      event_info.phys_link_event_info.state =
          PS_PHYS_LINK_GET_STATE(this_phys_link_ptr);
        event_info.phys_link_event_info.info_code = PS_EIC_NOT_SPECIFIED;

      PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr, PHYS_LINK_GOING_NULL);

      if (this_phys_link_ptr->phys_link_go_null_cmd_f_ptr(this_phys_link_ptr,
                                                          client_data_ptr) < 0)
      {
        PS_PHYS_LINKI_SET_STATE(this_phys_link_ptr,
                                event_info.phys_link_event_info.state);
        /* *ps_errno =; //error? */
      }
      else
      {
        /*-------------------------------------------------------------------
          Need to disable flow so that DS_WRITE_EVENT is not posted in
          sockets layer when phys link is in this state
        -------------------------------------------------------------------*/
        ps_phys_link_disable_flow(this_phys_link_ptr,
                                  DS_FLOW_PHYS_LINK_MASK);
        ps_ifacei_invoke_event_cbacks(NULL,
                                      this_phys_link_ptr,
                                      PHYS_LINK_GOING_NULL_EV,
                                      event_info
                                     );

        *ps_errno = DS_EWOULDBLOCK;
      }
    }
    else
    {
      *ps_errno = DS_EOPNOTSUPP;
    }
    ret_val = -1;
    break;

  case PHYS_LINK_GOING_NULL:
    *ps_errno = DS_EWOULDBLOCK;
    ret_val = -1;
    break;

  default:
    ASSERT(0);
    *ps_errno = DS_EOPNOTSUPP;
    ret_val = -1;
    break;

  } /* switch(state) */

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ps_phys_link_go_null_cmd() */


/*===========================================================================
                         FLOW CONTROL COMMANDS
===========================================================================*/
/*===========================================================================
FUNCTION PS_PHYS_LINK_ENABLE_FLOW()

DESCRIPTION
  Interface user enables flow on the interface.

PARAMETERS
  this_phys_link_ptr: ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_enable_flow
(
  ps_phys_link_type *this_phys_link_ptr,
  uint32             flow_mask
)
{
  ps_iface_event_info_u_type event_info;
  ds3g_flow_e_type           flow_type = DS_FLOW_ENABLE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)) )
  {
    ASSERT(0);
    return;
  }

  memset(&event_info, 0, sizeof(ps_iface_event_info_u_type));

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    store the previous flow mask in the event info variable, and remove the
    client mask from the tx_flow_mask.
  -------------------------------------------------------------------------*/
  event_info.flow_mask = this_phys_link_ptr->phys_private.tx_flow_mask;
  DS_FLOW_CTRL_SET_MASK(flow_type,
                        this_phys_link_ptr->phys_private.tx_flow_mask,
                        flow_mask);

  /*-------------------------------------------------------------------------
    only call the callback on the transition to the flow being enabled
  -------------------------------------------------------------------------*/
  if( (PS_PHYS_LINK_FLOW_ENABLED(this_phys_link_ptr)) &&
      (event_info.flow_mask != DS_FLOW_IS_ENABLED)
    )
  {
    LOG_MSG_INFO2_3("ps_phys_link_enable_flow(): "
                    "client 0x%x enabling flow on phys link 0x%p -> mask 0x%x",
                    flow_mask,
                    this_phys_link_ptr,
                    this_phys_link_ptr->phys_private.tx_flow_mask);

    ps_ifacei_invoke_event_cbacks(NULL,
                                  this_phys_link_ptr,
                                  PHYS_LINK_FLOW_ENABLED_EV,
                                  event_info);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_phys_link_enable_flow() */


/*===========================================================================
FUNCTION PS_PHYS_LINK_DISABLE_FLOW()

DESCRIPTION
  client disables flow on the interface.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_phys_link_ptr: Ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_disable_flow
(
  ps_phys_link_type *this_phys_link_ptr,
  uint32             flow_mask
)
{
  ps_iface_event_info_u_type event_info;
  ds3g_flow_e_type           flow_type = DS_FLOW_DISABLE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!(PS_PHYS_LINK_IS_VALID(this_phys_link_ptr)))
  {
    ASSERT(0);
    return;
  }

  memset(&event_info, 0, sizeof(ps_iface_event_info_u_type));

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    store the previous flow mask in the event info variable, and insert the
    client mask into the tx_flow_mask.
  -------------------------------------------------------------------------*/
  event_info.flow_mask = this_phys_link_ptr->phys_private.tx_flow_mask;
  DS_FLOW_CTRL_SET_MASK(flow_type,
                        this_phys_link_ptr->phys_private.tx_flow_mask,
                        flow_mask);

  /*-------------------------------------------------------------------------
    only call the callback on the transition to the flow being disabled
  -------------------------------------------------------------------------*/
  if(event_info.flow_mask == DS_FLOW_IS_ENABLED &&
     this_phys_link_ptr->phys_private.tx_flow_mask != DS_FLOW_IS_ENABLED)
  {
    LOG_MSG_INFO2_3("ps_phys_link_disable_flow(): "
                    "client 0x%x disabling flow on phys link 0x%p -> mask 0x%x",
                    flow_mask,
                    this_phys_link_ptr,
                    this_phys_link_ptr->phys_private.tx_flow_mask);

    ps_ifacei_invoke_event_cbacks(NULL,
                                  this_phys_link_ptr,
                                  PHYS_LINK_FLOW_DISABLED_EV,
                                  event_info);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_phys_link_disable_flow() */

/*===========================================================================
FUNCTION PS_PHYS_LINKI_ENABLE_DISABLE_FLOW_EX()

DESCRIPTION
  Helper function which posts a cmd to PS task to either disable/enable flow on the phys link

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  phys_link_ptr: Ptr to phys link on which to operate on.
  flow_mask: bit mask that identifies the caller.
  flow_param: boolean to decide whether to enable or disable.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_linki_enable_disable_flow_ex
(
  ps_phys_link_type  * phys_link_ptr,
  uint32               flow_mask,
  boolean              flow_param
)
{
  ps_phys_linki_flow_ctrl_cmd_info_type  * flow_ctrl_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(flow_ctrl_info_ptr, 
                                  sizeof(ps_phys_linki_flow_ctrl_cmd_info_type),
                                  ps_phys_linki_flow_ctrl_cmd_info_type*);

  if (NULL == flow_ctrl_info_ptr)
  {
    return;
  }

  flow_ctrl_info_ptr->ps_phys_link_ptr = phys_link_ptr;
  flow_ctrl_info_ptr->flow_mask        = flow_mask;
  flow_ctrl_info_ptr->is_enabled       = flow_param;

  ps_send_cmd(PS_PHYS_LINK_FLOW_CTRL_CMD, flow_ctrl_info_ptr);
  return;

} /* ps_phys_linki_enable_disable_flow_ex() */

/*===========================================================================
FUNCTION PS_PHYS_LINK_ENABLE_FLOW_EX()

DESCRIPTION
  Posts a cmd to PS task enable flow on the phys link

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  phys_link_ptr: Ptr to phys link on which to operate on.
  flow_mask: bit mask that identifies the caller.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_enable_flow_ex
(
  ps_phys_link_type  * phys_link_ptr,
  uint32               flow_mask
)
{
  ps_phys_linki_enable_disable_flow_ex(phys_link_ptr, flow_mask, TRUE);
  return;

} /* ps_phys_link_enable_flow_ex() */

/*===========================================================================
FUNCTION PS_PHYS_LINK_DISABLE_FLOW_EX()

DESCRIPTION
  Posts a cmd to PS task disable flow on the phys link

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  phys_link_ptr: Ptr to phys link on which to operate on.
  flow_mask: bit mask that identifies the caller.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_disable_flow_ex
(
  ps_phys_link_type  * phys_link_ptr,
  uint32               flow_mask
)
{
  ps_phys_linki_enable_disable_flow_ex(phys_link_ptr, flow_mask, FALSE);
  return;

} /* ps_phys_link_disable_flow_ex() */


/*===========================================================================
FUNCTION PS_PHYS_LINK_FLOW_CTRL_CMD_HANDLER()

DESCRIPTION
  This function processes PS_PHYS_LINK_FLOW_CTRL_CMD.

PARAMETERS
  ps_cmd          : Must be PS_PHYS_LINK_FLOW_CTRL_CMD.
  ps_cmd_data_ptr : cmd data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_flow_ctrl_cmd_handler
(
  ps_cmd_enum_type    ps_cmd,
  void              * ps_cmd_data_ptr
)
{
  ps_phys_linki_flow_ctrl_cmd_info_type  * flow_ctrl_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ps_cmd_data_ptr)
  {
    LOG_MSG_ERROR_1("NULL cmd data, cmd %d", ps_cmd);
    return;
  }

  do
  {
    flow_ctrl_info_ptr =
      (ps_phys_linki_flow_ctrl_cmd_info_type *) ps_cmd_data_ptr;

    if (PS_PHYS_LINK_FLOW_CTRL_CMD != ps_cmd)
    {
      LOG_MSG_ERROR_1("Invalid cmd %d", ps_cmd);
      break;
    }

    if (TRUE == flow_ctrl_info_ptr->is_enabled)
    {
      ps_phys_link_enable_flow(flow_ctrl_info_ptr->ps_phys_link_ptr,
                               flow_ctrl_info_ptr->flow_mask);
    }
    else
    {
      ps_phys_link_disable_flow(flow_ctrl_info_ptr->ps_phys_link_ptr,
                                flow_ctrl_info_ptr->flow_mask);
    }
  } while (0);

  PS_SYSTEM_HEAP_MEM_FREE(flow_ctrl_info_ptr);
  return;

} /* ps_phys_link_flow_ctrl_cmd_handler() */

