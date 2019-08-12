

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ F L O W . C

DESCRIPTION
  File defining functions that can be used to manipulate a flow

EXTERNAL FUNCTIONS
  PS_FLOW_BIND_PHYS_LINK()
    Binds a flow to a phys link

  PS_FLOW_UNBIND_PHYS_LINK()
    Unbinds a phys link from ps_flow

  PS_FLOW_REBIND_PHYS_LINK()
    Binds a flow to a phys link, which is different from its current binding

  PS_FLOW_ENABLE_TX()
    Enables a flow to transmit traffic

  PS_FLOW_DISABLE_TX()
    Disables a flow from transmitting traffic

  PS_FLOW_SET_ASSOC_FLOW()
    Associates a logical PS flow to another PS flow.

  PS_FLOW_ACTIVATE_CMD()
    Activates/Resumes a flow

  PS_FLOW_RESUME_CMD()
    Resumes a flow. This command is an alias of ps_flow_activate_cmd()

  PS_FLOW_SUSPEND_CMD()
    Suspends a flow

  PS_FLOW_GO_NULL_CMD()
    Tears down a flow

  PS_FLOW_ACTIVATE_IND()
    Indicates that a flow is activated

  PS_FLOW_SUSPEND_IND()
    Indicates that a flow is suspended

  PS_FLOW_GO_NULL_IND()
    Indicates that a flow is torn down

  PS_FLOW_GENERIC_IND()
    Posts a generic ps_flow event

  PS_FLOW_RX_FLTR_UPDATED_IND()
    Indicates that the number of Rx filters installed on a flow have changed

  PS_FLOW_QOS_MODIFY_ACCEPTED_IND()
    Indicates that QOS_MODIFY is accepted by the network

  PS_FLOW_QOS_MODIFY_REJECTED_IND()
    Indicates that QOS_MODIFY is rejected by the network

  PS_FLOW_SET_GRANTED_FLOW()
    Sets granted QOS in ps_flow

  PS_FLOW_GET_RX_FLTR_CNT()
    Returns number of Rx fltrs. Generic to handle Rx fltrs in requested QOS
    and pending QOS_MODIFY

  PS_FLOW_GET_RX_FLTR_HANDLE()
    Returns a handle to fetch Rx filters. Generic to handle Rx fltrs in
    requested QOS and pending QOS_MODIFY

  PS_FLOW_GET_RX_FLTR_BY_HANDLE()
    Given a handle, returns a Rx filter, and also the precedence of that
    filter. Also returns a new handle to fetch next Rx fltr. Generic to
    handle Rx fltrs in requested QOS and pending QOS_MODIFY

  PS_FLOW_GET_AUX_FLOW_SPEC_CNT()
    Returns number of auxiliary flow specs. Generic to handle
    Rx/Tx auxiliary flow specs in requested QOS and pending QOS_MODIFY

  PS_FLOW_GET_AUX_FLOW_SPEC_HANDLE()
    Returns a handle to fetch auxiliary flow specs. Generic to handle
    Rx/Tx auxiliary flow specs in requested QOS and pending QOS_MODIFY

  PS_FLOW_GET_AUX_FLOW_SPEC_BY_HANDLE()
    Returns an auxiliary flow specs based on given handle. Also returns a new
    handle to fetch next auxiliary flow spec. Generic to handle
    Rx/Tx auxiliary flow specs in requested QOS and pending QOS_MODIFY

  PS_FLOW_VALIDATE_PRIMARY_QOS_MODIFY_SPEC()
    Validates primary QoS spec

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None for the module.  Each flow is created by calling
  ps_iface_create_flow().

Copyright (c) 2005-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_flow.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/06/13    bb     Fixing crash on logical flow activate in due to unbound physlink 
11/09/12    kr     Fixing futex leak during modify qos
09/12/08    pp     Metainfo optimizations.
09/01/07    ssh    QoS support for logical ifaces
6/28/07    msr/ssh QoS supportfor M.IP CCoA iface (currently featurized)
02/27/07    scb    Fixed LINT errors
02/09/07    scb    Fixed Klocworks High errors
01/05/07    msr    Added support to log info code in FLOW_INFO_CODE_UPDATED_EV
11/16/06    msr    Added FLOW_INFO_CODE_UPDATED_EV
09/12/06    msr    Removed redundant state field in event_info structure
07/31/06    msr    Added support for PRIMARY_QOS_MODIFY
07/17/06    msr    Reverted checkin on 7/14. DS team reorganized their code
                   to rule out the need to revert ps_flow state
07/14/06    msr    Removed all state checks from ps_flow commands to allow
                   DS to revert operations
07/06/06    sk     Handling CONFIGURING state in ps_flow_suspend_ind()
06/14/06    msr    Changed flow cmd processing to handle states correctly
05/01/06    msr    Moved logging QoS state for QOS_MODIFY from ps_iface.c
04/25/06    msr    L4/Tasklock code review changes
04/13/06    msr    Unbinding ps_flow even when it is in NULL state already
04/12/06    msr    Logging info_code along with QoS state
03/27/06    msr    Reverted deprecation of PS_FLOW_SET_CURRENT_FLOW()
02/27/06    msr    Deprecated PS_FLOW_SET_CURRENT_FLOW(), and added
                   PS_FLOW_SET_[TR]X_GRANTED_FLOW()
02/23/06    msr    Printing info codes in F3 messages
02/22/06    msr    Using single critical section
02/17/06    msr    Exporting ps_flow_unbind_phys_link()
02/15/06    msr    Aliased PS_FLOW_SET_CURRENT_FLOW() to
                   PS_FLOW_SET_GRANTED_FLOW() and logging granted QoS spec in
                   ps_flow_activate_ind() instead of ps_flow_set_granted_flow()
02/06/06    msr    Updated for L4 tasklock/crit sections.
01/09/06    msr    Conforming to new QOS_MODIFY API
12/06/05    msr    Reorganized cmds to support NULL cmd callback function
11/30/05    msr    Removed FEATURE_DATA_PS_QOS_SPEC_LOGGING
10/31/05    mct    Updated to support new qos bundle api.
10/18/05    msr    Removed support for FLOW_ACTIVATE_MODIFIED_EV and
                   PHYS_LINK_UP_MODIFIED_EV
08/31/05    mct    Removed asserts for logical iface binding.
08/29/05    msr    Added ps_flow_get_aux_flow_spec_cnt(),
                   ps_flow_get_aux_flow_spec_handle(), and
                   ps_flow_get_aux_flow_spec_by_handle(). Changed API to fetch
                   Rx fltrs
08/17/05    msr    Logging QOS states only for secondary flows
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/15/05    mct    Support for QOS_CONFIGURE.
08/15/05    msr    Logging various QOS states and added
                   ps_flow_set_current_flow()
08/10/05    msr    Added support for QOS_MODIFY.
05/12/05    ks     Fixed lint errors.
05/03/05    msr    Not enforcing that flow is bound to a phys link in flow
                   cmds
05/03/05    msr    Reorganized flow cmds
04/17/05    msr    Created the file

===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "ps_flow.h"
#include "ps_flowi.h"
#include "ds_flow_control.h"
#include "ps_phys_link.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "ps_mem.h"
#include "ps_utils.h"
#include "ps_flowi_event.h"
#include "ps_qos_spec_logging.h"
#include "ps_crit_sect.h"
#include "ps_system_heap.h"
#include "ds_Utils_DebugMsg.h"
#include <stringl/stringl.h>

/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Data type used in PS_FLOW_FLOW_CTRL_CMD
---------------------------------------------------------------------------*/
typedef struct
{
  ps_flow_type  * ps_flow_ptr;
  uint32          flow_mask;
  boolean         is_enabled;
} ps_flowi_flow_ctrl_cmd_info_type;


/*===========================================================================

                                  COMMANDS

===========================================================================*/
#ifdef FEATURE_DATA_PS_QOS
/*===========================================================================
FUNCTION PS_FLOW_ACTIVATE_CMD()

DESCRIPTION
  Activates a flow

PARAMETERS
  flow_ptr : ptr to flow on which to operate on.

RETURN VALUE
   0 : on success
  -1 : on failure (which includes DS_EWOULDBLOCK)

DEPENDENCIES
  If function fails, caller is responsible for cleaning up this ps_flow

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS
  Changes flow's state to FLOW_ACTIVATING and posts FLOW_ACTIVATING_EV
===========================================================================*/
int ps_flow_activate_cmd
(
  ps_flow_type  * flow_ptr,
  int16         * ps_errno,
  void          * client_data_ptr
)
{
  ps_iface_event_info_u_type    event_info;
  int                           ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_flow_activate_cmd(): NULL parameter passed");
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_activate_cmd(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    No commands or indications are allowed on default flow except for
    ACTIVATE_IND and GO_NULL_IND. Although its good to make default flow
    transparent to mode handler, its not possible since we have to bind flow
    in ps_iface_create(), which incremente phys link's ref cnt when it is not
    in use
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_activate_cmd(): "
                    "FLOW ACTIVATE CMD can't be called on default flow, 0x%p",
                    flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  LOG_MSG_INFO2_2("ps_flow_activate_cmd(): "
                  "FLOW ACTIVATE CMD 0x%p, state 0x%x",
                  flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));

  switch (PS_FLOWI_GET_STATE(flow_ptr))
  {
    case FLOW_ACTIVATED:
      ret_val = 0;
      break;

    case FLOW_ACTIVATING:
      *ps_errno = DS_EWOULDBLOCK;
      ret_val = -1;
      break;

    case FLOW_NULL:
      /*---------------------------------------------------------------------
        Since HDR wants to post a cmd to DS in ioctl callback function in
        order to handle bundled QoS API, its ok for a ps_flow not to
        have activate_cmd registered
      ---------------------------------------------------------------------*/

      /*---------------------------------------------------------------------
        Need to update state before cmd callback is called so that mode
        handler knows if flow is getting activated or resumed
      ---------------------------------------------------------------------*/
      event_info.flow_event_info.state     = PS_FLOWI_GET_STATE(flow_ptr);
      event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
      PS_FLOWI_SET_STATE(flow_ptr, FLOW_ACTIVATING);

      if (flow_ptr->ps_flow_activate_cmd_f_ptr != NULL &&
          flow_ptr->ps_flow_activate_cmd_f_ptr(flow_ptr, client_data_ptr) < 0)
      {
        PS_FLOWI_SET_STATE(flow_ptr, event_info.flow_event_info.state);
        LOG_MSG_ERROR_1("ps_flow_activate_cmd(): "
                        "FLOW ACTIVATE CMD failed 0x%p", flow_ptr);
        *ps_errno = DS_ENETDOWN;
      }
      else
      {
        /*-------------------------------------------------------------------
          Need to disable tx traffic so that DS_WRITE_EVENT is not posted
          in sockets layer when flow is in this state
        -------------------------------------------------------------------*/
        ps_flow_disable_tx(flow_ptr, DS_FLOW_PS_FLOW_MASK);
        ps_flowi_invoke_event_cbacks(flow_ptr, FLOW_ACTIVATING_EV, event_info);

        *ps_errno = DS_EWOULDBLOCK;
      }

      ret_val = -1;
      break;

    case FLOW_GOING_NULL:
      *ps_errno = DS_ENETCLOSEINPROGRESS;
      ret_val = -1;
      break;

    case FLOW_SUSPENDED:
    case FLOW_SUSPENDING:
    case FLOW_RESUMING:
    case FLOW_CONFIGURING:
      LOG_MSG_ERROR_2("ps_flow_activate_cmd(): "
                      "FLOW ACTIVATE CMD is not supported when ps_flow 0x%p is "
                      "in state 0x%x", flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      *ps_errno = DS_EOPNOTSUPP;
      ret_val = -1;
      break;

    default:
      LOG_MSG_ERROR_2("ps_flow_activate_cmd(): "
                      "Flow, 0x%p, is in invalid state, 0x%x",
                      flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      ret_val = -1;
      break;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ps_flow_activate_cmd() */



/*===========================================================================
FUNCTION PS_FLOW_CONFIGURE_CMD()

DESCRIPTION
  Configures a flow

PARAMETERS
  flow_ptr : ptr to flow on which to operate on.

RETURN VALUE
   0 : on success
  -1 : on failure (which includes DS_EWOULDBLOCK)

DEPENDENCIES
  If function fails, caller is responsible for cleaning up this ps_flow

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS
  Changes flow's state to FLOW_CONFIGURING and posts FLOW_CONFIGURING_EV
===========================================================================*/
int ps_flow_configure_cmd
(
  ps_flow_type  * flow_ptr,
  int16         * ps_errno,
  void          * client_data_ptr
)
{
  ps_iface_event_info_u_type    event_info;
  int32                         ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_flow_configure_cmd(): NULL parameter passed");
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_configure_cmd(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    No commands or indications are allowed on default flow except for
    ACTIVATE_IND and GO_NULL_IND. Although its good to make default flow
    transparent to mode handler, its not possible since we have to bind flow
    in ps_iface_create(), which incremente phys link's ref cnt when it is not
    in use
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_configure_cmd(): "
                    "FLOW ACTIVATE CMD can't be called on default flow, 0x%p",
                    flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  LOG_MSG_INFO2_2("ps_flow_configure_cmd(): "
                  "FLOW CONFIGURE CMD 0x%p, state 0x%x",
                  flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));

  switch (PS_FLOWI_GET_STATE(flow_ptr))
  {
    case FLOW_SUSPENDED:
      ret_val = 0;
      break;

    case FLOW_CONFIGURING:
      *ps_errno = DS_EWOULDBLOCK;
      ret_val = -1;
      break;

    case FLOW_NULL:
      /*---------------------------------------------------------------------
        Since HDR wants to post a cmd to DS in ioctl callback function in
        order to handle bundled QoS API, its ok for a ps_flow not to
        have configure_cmd registered
      ---------------------------------------------------------------------*/

      /*---------------------------------------------------------------------
        Need to update state before cmd callback is called so that mode
        handler knows that flow is getting configured
      ---------------------------------------------------------------------*/
      event_info.flow_event_info.state     = PS_FLOWI_GET_STATE(flow_ptr);
      event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
      PS_FLOWI_SET_STATE(flow_ptr, FLOW_CONFIGURING);

      if (flow_ptr->ps_flow_configure_cmd_f_ptr != NULL &&
          flow_ptr->ps_flow_configure_cmd_f_ptr(flow_ptr, client_data_ptr) < 0)
      {
        PS_FLOWI_SET_STATE(flow_ptr, event_info.flow_event_info.state);
        LOG_MSG_ERROR_1("ps_flow_configure_cmd(): "
                        "FLOW CONFIGURE CMD failed 0x%p", flow_ptr);
        *ps_errno = DS_ENETDOWN;
      }
      else
      {
        /*-------------------------------------------------------------------
          Need to disable tx traffic so that DS_WRITE_EVENT is not posted
          in sockets layer when flow is in this state
        -------------------------------------------------------------------*/
        ps_flow_disable_tx(flow_ptr, DS_FLOW_PS_FLOW_MASK);
        ps_flowi_invoke_event_cbacks(flow_ptr,
                                     FLOW_CONFIGURING_EV,
                                     event_info);

        *ps_errno = DS_EWOULDBLOCK;

      }

      ret_val = -1;
      break;

    case FLOW_GOING_NULL:
      *ps_errno = DS_ENETCLOSEINPROGRESS;
      ret_val = -1;
      break;

    case FLOW_ACTIVATED:
    case FLOW_ACTIVATING:
    case FLOW_SUSPENDING:
    case FLOW_RESUMING:
      LOG_MSG_ERROR_2("ps_flow_configure_cmd(): "
                      "FLOW CONFIGURE CMD is not supported when ps_flow 0x%p is "
                      "in state 0x%x", flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      *ps_errno = DS_EOPNOTSUPP;
      ret_val = -1;
      break;

    default:
      LOG_MSG_ERROR_2("ps_flow_configure_cmd(): "
                      "Flow, 0x%p, is in invalid state, 0x%x",
                      flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      ret_val = -1;
      break;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ps_flow_configure_cmd() */



/*===========================================================================
FUNCTION PS_FLOW_RESUME_CMD()

DESCRIPTION
  Resumes a flow

PARAMETERS
  flow_ptr : ptr to flow on which to operate on.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
   0 : on success
  -1 : on failure (which includes DS_EWOULDBLOCK)

DEPENDENCIES
  None

SIDE EFFECT
  Changes flow's state to FLOW_RESUMING and posts FLOW_RESUMING_EV
===========================================================================*/
int ps_flow_resume_cmd
(
  ps_flow_type  * flow_ptr,
  int16         * ps_errno,
  void          * client_data_ptr
)
{
  ps_iface_event_info_u_type    event_info;
  int                           ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_flow_resume_cmd(): NULL parameter passed");
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_resume_cmd(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    No commands or indications are allowed on default flow except for
    ACTIVATE_IND and GO_NULL_IND. Although its good to make default flow
    transparent to mode handler, its not possible since we have to bind flow
    in ps_iface_create(), which incremente phys link's ref cnt when it is not
    in use
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_resume_cmd(): "
                    "FLOW RESUME CMD can't be called on default flow, 0x%p",
                    flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  LOG_MSG_INFO2_2("ps_flow_resume_cmd(): "
                  "FLOW RESUME CMD 0x%p, state 0x%x",
                  flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));

  switch (PS_FLOWI_GET_STATE(flow_ptr))
  {
    case FLOW_ACTIVATED:
      ret_val = 0;
      break;

    case FLOW_RESUMING:
      *ps_errno = DS_EWOULDBLOCK;
      ret_val = -1;
      break;

    case FLOW_SUSPENDED:
    case FLOW_SUSPENDING:
      /*---------------------------------------------------------------------
        Since HDR wants to post a cmd to DS in ioctl callback function in
        order to handle bundled QoS API, its ok for a ps_flow not to
        have activate_cmd registered
      ---------------------------------------------------------------------*/

      /*---------------------------------------------------------------------
        Need to update state before cmd callback is called so that mode
        handler knows if flow is getting activated or resumed
      ---------------------------------------------------------------------*/
      event_info.flow_event_info.state     = PS_FLOWI_GET_STATE(flow_ptr);
      event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
      PS_FLOWI_SET_STATE(flow_ptr, FLOW_RESUMING);

      if (flow_ptr->ps_flow_activate_cmd_f_ptr != NULL &&
          flow_ptr->ps_flow_activate_cmd_f_ptr(flow_ptr, client_data_ptr) < 0)
      {
        PS_FLOWI_SET_STATE(flow_ptr, event_info.flow_event_info.state);
        LOG_MSG_ERROR_1("ps_flow_resume_cmd(): "
                        "FLOW RESUME CMD failed 0x%p", flow_ptr);
        *ps_errno = DS_ENETDOWN;
      }
      else
      {
        /*-------------------------------------------------------------------
          Need to disable tx traffic so that DS_WRITE_EVENT is not posted
          in sockets layer when flow is in this state
        -------------------------------------------------------------------*/
        ps_flow_disable_tx(flow_ptr, DS_FLOW_PS_FLOW_MASK);
        ps_flowi_invoke_event_cbacks(flow_ptr, FLOW_RESUMING_EV, event_info);

        *ps_errno = DS_EWOULDBLOCK;
      }

      ret_val = -1;
      break;

    case FLOW_GOING_NULL:
      *ps_errno = DS_ENETCLOSEINPROGRESS;
      ret_val = -1;
      break;

    case FLOW_NULL:
      LOG_MSG_ERROR_2("ps_flow_resume_cmd(): "
                      "FLOW RESUME CMD is not supported when ps_flow 0x%p is "
                      "in state 0x%x", flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      *ps_errno = DS_EOPNOTSUPP;
      ret_val = -1;
      break;

    case FLOW_ACTIVATING:
    case FLOW_CONFIGURING:
      LOG_MSG_ERROR_2("ps_flow_resume_cmd(): "
                      "FLOW RESUME CMD is not supported when ps_flow 0x%p is "
                      "in state 0x%x", flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      *ps_errno = DS_ENETINPROGRESS;
      ret_val = -1;
      break;

    default:
      LOG_MSG_ERROR_2("ps_flow_resume_cmd(): "
                      "Flow, 0x%p, is in invalid state, 0x%x",
                      flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      ret_val = -1;
      break;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ps_flow_resume_cmd() */



/*===========================================================================
FUNCTION PS_FLOW_SUSPEND_CMD

DESCRIPTION
  Suspends a flow

PARAMETERS
  flow_ptr : ptr to flow on which to operate on.

RETURN VALUE
   0 : on success
  -1 : on failure (which includes DS_EWOULDBLOCK)

DEPENDENCIES
  If function fails, caller is responsible for cleaning up this ps_flow

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS
  Changes flow's state to FLOW_SUSPENDING and posts FLOW_SUSPENDING_EV
===========================================================================*/
int ps_flow_suspend_cmd
(
  ps_flow_type  * flow_ptr,
  int16         * ps_errno,
  void          * client_data_ptr
)
{
  ps_iface_event_info_u_type    event_info;
  int                           ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_flow_suspend_cmd(): NULL parameter passed");
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_suspend_cmd(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    No commands or indications are allowed on default flow except for
    ACTIVATE_IND and GO_NULL_IND. Although its good to make default flow
    transparent to mode handler, its not possible since we have to bind flow
    in ps_iface_create(), which incremente phys link's ref cnt when it is not
    in use
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_suspend_cmd(): "
                    "FLOW SUSPEND CMD can't be called on default flow, 0x%p",
                    flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  LOG_MSG_INFO2_2("ps_flow_suspend_cmd(): "
                  "FLOW SUSPEND CMD 0x%p, state 0x%x",
                  flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));

  switch (PS_FLOWI_GET_STATE(flow_ptr))
  {
    case FLOW_SUSPENDED:
      ret_val = 0;
      break;

    case FLOW_SUSPENDING:
      *ps_errno = DS_EWOULDBLOCK;
      ret_val = -1;
      break;

    case FLOW_ACTIVATED:
    case FLOW_ACTIVATING:
    case FLOW_RESUMING:
    case FLOW_CONFIGURING:
      /*---------------------------------------------------------------------
        Since HDR wants to post a cmd to DS in ioctl callback function in
        order to handle bundled QoS API, its ok for a ps_flow not to
        have configure_cmd registered
      ---------------------------------------------------------------------*/

      /*---------------------------------------------------------------------
        Need to update state before cmd callback is called so that mode
        handler knows flow's new state in the callback
      ---------------------------------------------------------------------*/
      event_info.flow_event_info.state     = PS_FLOWI_GET_STATE(flow_ptr);
      event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
      PS_FLOWI_SET_STATE(flow_ptr, FLOW_SUSPENDING);

      if (flow_ptr->ps_flow_suspend_cmd_f_ptr != NULL &&
          flow_ptr->ps_flow_suspend_cmd_f_ptr(flow_ptr, client_data_ptr) < 0)
      {
        PS_FLOWI_SET_STATE(flow_ptr, event_info.flow_event_info.state);
        LOG_MSG_ERROR_1("ps_flow_suspend_cmd(): "
                        "FLOW SUSPEND CMD failed 0x%p", flow_ptr);
        *ps_errno = DS_ENETDOWN;
      }
      else
      {
        /*-------------------------------------------------------------------
          Need to disable tx traffic so that DS_WRITE_EVENT is not posted
          in sockets layer when flow is in this state
        -------------------------------------------------------------------*/
        ps_flow_disable_tx(flow_ptr, DS_FLOW_PS_FLOW_MASK);
        ps_flowi_invoke_event_cbacks(flow_ptr,
                                     FLOW_SUSPENDING_EV,
                                     event_info);

        *ps_errno = DS_EWOULDBLOCK;
      }

      ret_val = -1;
      break;

    case FLOW_GOING_NULL:
      *ps_errno = DS_ENETCLOSEINPROGRESS;
      ret_val = -1;
      break;

    case FLOW_NULL:
      *ps_errno = DS_ENETDOWN;
      ret_val = -1;
      break;

    default:
      LOG_MSG_ERROR_2("ps_flow_suspend_cmd(): "
                      "Flow, 0x%p, is in invalid state, 0x%x",
                      flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      ret_val = -1;
      break;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ps_flow_suspend_cmd() */



/*===========================================================================
FUNCTION PS_FLOW_GO_NULL_CMD

DESCRIPTION
  Tears down a flow

PARAMETERS
  flow_ptr : ptr to flow on which to operate on.

RETURN VALUE
   0 : on success
  -1 : on failure (which includes DS_EWOULDBLOCK)

DEPENDENCIES
  If function fails, caller is responsible for cleaning up this ps_flow

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS
  Changes flow's state to FLOW_GOING_NULL and posts FLOW_GOING_NULL_EV
===========================================================================*/
int ps_flow_go_null_cmd
(
  ps_flow_type  * flow_ptr,
  int16         * ps_errno,
  void          * client_data_ptr
)
{
  ps_iface_event_info_u_type    event_info;
  int                           ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_flow_go_null_cmd(): NULL parameter passed");
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_go_null_cmd(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    No commands or indications are allowed on default flow except for
    ACTIVATE_IND and GO_NULL_IND. Although its good to make default flow
    transparent to mode handler, its not possible since we have to bind flow
    in ps_iface_create(), which incremente phys link's ref cnt when it is not
    in use
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_go_null_cmd(): "
                    "FLOW GO NULL CMD can't be called on default flow, 0x%p",
                    flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  LOG_MSG_INFO2_2("ps_flow_go_null_cmd(): "
                  "FLOW GO NULL CMD 0x%p, state 0x%x",
                  flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));

  switch (PS_FLOWI_GET_STATE(flow_ptr))
  {
    case FLOW_NULL:
      ret_val = 0;
      break;

    case FLOW_GOING_NULL:
      *ps_errno = DS_EWOULDBLOCK;
      ret_val = -1;
      break;

    case FLOW_ACTIVATING:
    case FLOW_CONFIGURING:
    case FLOW_ACTIVATED:
    case FLOW_SUSPENDING:
    case FLOW_SUSPENDED:
    case FLOW_RESUMING:
      /*---------------------------------------------------------------------
        Since HDR wants to post a cmd to DS in ioctl callback function in
        order to handle bundled QoS API, its ok for a ps_flow not to
        have configure_cmd registered
      ---------------------------------------------------------------------*/

      /*---------------------------------------------------------------------
        Need to update state before cmd callback is called so that mode
        handler knows flow's new state in the callback
      ---------------------------------------------------------------------*/
      event_info.flow_event_info.state     = PS_FLOWI_GET_STATE(flow_ptr);
      event_info.flow_event_info.info_code = PS_EIC_NOT_SPECIFIED;
      PS_FLOWI_SET_STATE(flow_ptr, FLOW_GOING_NULL);

      if (flow_ptr->ps_flow_go_null_cmd_f_ptr != NULL &&
          flow_ptr->ps_flow_go_null_cmd_f_ptr(flow_ptr, client_data_ptr) < 0)
      {
        PS_FLOWI_SET_STATE(flow_ptr, event_info.flow_event_info.state);
        LOG_MSG_ERROR_1("ps_flow_go_null_cmd(): "
                        "FLOW GO NULL CMD failed 0x%p", flow_ptr);
        *ps_errno = DS_ENETDOWN;
      }
      else
      {
        /*-------------------------------------------------------------------
          Need to disable tx traffic so that DS_WRITE_EVENT is not posted
          in sockets layer when flow is in this state
        -------------------------------------------------------------------*/
        ps_flow_disable_tx(flow_ptr, DS_FLOW_PS_FLOW_MASK);
        ps_flowi_invoke_event_cbacks(flow_ptr,
                                     FLOW_GOING_NULL_EV,
                                     event_info);

        *ps_errno = DS_EWOULDBLOCK;
      }

      ret_val = -1;
      break;

    default:
      LOG_MSG_ERROR_2("ps_flow_go_null_cmd(): "
                      "Flow, 0x%p, is in invalid state, 0x%x",
                      flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      ret_val = -1;
      break;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ps_flow_go_null_cmd() */
#endif /* FEATURE_DATA_PS_QOS */



/*===========================================================================

                                INDICATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOW_ACTIVATE_IND

DESCRIPTION
  Indicates that a flow is activated

PARAMETERS
  flow_ptr  : ptr to flow on which to operate on
  info_code : extended info code for the indication

RETURN VALUE
   None

DEPENDENCIES
  All ps_flow indications must be posted from one task context
  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS
  Changes flow's state to FLOW_ACTIVATED and posts FLOW_ACTIVATED_EV
===========================================================================*/
void ps_flow_activate_ind
(
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code
)
{
  ps_iface_event_info_u_type    event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_activate_ind(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Although its not strictly necessary for a flow to be bound to a
    phys link, its meaningless to activate QOS without binding it to over the
    air link
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_PHYS_LINK(flow_ptr) == NULL)
  {
    if (NULL == PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr))
    {
      LOG_MSG_ERROR_1("ps_flow_activate_ind(): "
                      "Assoc Flow, 0x%p, is not bound to a phys link",
                      flow_ptr);
      ASSERT(0);
    }
    
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return;
  }

  LOG_MSG_INFO2_3("ps_flow_activate_ind(): "
                  "FLOW ACTIVATE IND 0x%p, state 0x%x, info code %d",
                  flow_ptr, PS_FLOWI_GET_STATE(flow_ptr), info_code);

  switch (PS_FLOWI_GET_STATE(flow_ptr))
  {
    case FLOW_GOING_NULL:
    case FLOW_CONFIGURING:
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_2("ps_flow_activate_ind(): "
                      "Flow, 0x%p, can't be activated in state 0x%x",
                      flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      return;

    case FLOW_NULL:
    case FLOW_ACTIVATING:
    case FLOW_ACTIVATED:
    case FLOW_SUSPENDING:
    case FLOW_SUSPENDED:
    case FLOW_RESUMING:
      event_info.flow_event_info.state     = PS_FLOWI_GET_STATE(flow_ptr);
      event_info.flow_event_info.info_code = info_code;

      PS_FLOWI_SET_STATE(flow_ptr, FLOW_ACTIVATED);

#ifdef FEATURE_DATA_PS_QOS
      /*---------------------------------------------------------------------
        Since mode handler doesn't care about default flow's state, don't
        generate event
      ---------------------------------------------------------------------*/
      if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
      {
        ps_flowi_invoke_event_cbacks(flow_ptr, FLOW_ACTIVATED_EV, event_info);

        /*-------------------------------------------------------------------
          If flow is already ACTIVATED, it means that mobile didn't initiate
          any QOS operation. It means that network modified QOS autonomously
          If flow is SUSPENDED, it means that network assigned QOS because of
          uplink data transfer. If flow is in transient state, it means that
          mobile initiated some QOS operation
        -------------------------------------------------------------------*/
        if (event_info.flow_event_info.state != FLOW_ACTIVATED)
        {
          ps_qsl_log_qos_state(flow_ptr, QSL_QOS_GRANTED, info_code);
        }
        else
        {
          ps_qsl_log_qos_state(flow_ptr, QSL_QOS_NW_MODIFIED, info_code);
        }
      }
#endif /* FEATURE_DATA_PS_QOS */

      /*---------------------------------------------------------------------
        Need to enable tx traffic so that DS_WRITE_EVENT is posted in
        sockets layer when flow is in this state
      ---------------------------------------------------------------------*/
      ps_flow_enable_tx(flow_ptr, DS_FLOW_PS_FLOW_MASK);
      break;

    default:
      LOG_MSG_ERROR_2("ps_flow_activate_ind(): "
                      "Flow, 0x%p, is in invalid state, 0x%x",
                      flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      ASSERT(0);
      break;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_flow_activate_ind() */



#ifdef FEATURE_DATA_PS_QOS
/*===========================================================================
FUNCTION PS_FLOW_SUSPEND_IND

DESCRIPTION
  Indicates that a flow is suspended

PARAMETERS
  flow_ptr  : ptr to flow on which to operate on.
  info_code : extended info code for the indication.

RETURN VALUE
   None

DEPENDENCIES
  All ps_flow indications must be posted from one task context
  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS
  Changes flow's state to FLOW_SUSPENDED and posts FLOW_SUSPENDED_EV
===========================================================================*/
void ps_flow_suspend_ind
(
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code
)
{
  ps_iface_event_info_u_type    event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_suspend_ind(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    No commands or indications are allowed on default flow except for
    ACTIVATE_IND and GO_NULL_IND. Although its good to make default flow
    transparent to mode handler, its not possible since we have to bind flow
    in ps_iface_create(), which incremente phys link's ref cnt when it is not
    in use
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_suspend_ind(): "
                    "FLOW SUSPEND IND can't be posted on default flow, 0x%p",
                    flow_ptr);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_3("ps_flow_suspend_ind(): "
                  "FLOW SUSPEND IND 0x%p, state 0x%x, info code %d",
                  flow_ptr, PS_FLOWI_GET_STATE(flow_ptr), info_code);

  switch (PS_FLOWI_GET_STATE(flow_ptr))
  {
    case FLOW_NULL:
    case FLOW_CONFIGURING:
    case FLOW_ACTIVATING:
    case FLOW_ACTIVATED:
    case FLOW_SUSPENDING:
    case FLOW_RESUMING:
      event_info.flow_event_info.state     = PS_FLOWI_GET_STATE(flow_ptr);
      event_info.flow_event_info.info_code = info_code;

      PS_FLOWI_SET_STATE(flow_ptr, FLOW_SUSPENDED);
      ps_flowi_invoke_event_cbacks(flow_ptr, FLOW_SUSPENDED_EV, event_info);
      ps_qsl_log_qos_state(flow_ptr, QSL_QOS_SUSPENDED, info_code);

      /*---------------------------------------------------------------------
        Need to enable tx traffic so that DS_WRITE_EVENT is posted in
        sockets layer when flow is in this state
      ---------------------------------------------------------------------*/
      ps_flow_enable_tx(flow_ptr, DS_FLOW_PS_FLOW_MASK);
      break;

    case FLOW_SUSPENDED:
      ps_flow_generic_ind(flow_ptr, FLOW_INFO_CODE_UPDATED_EV, &info_code);
      break;

    case FLOW_GOING_NULL:
      LOG_MSG_ERROR_2("ps_flow_suspend_ind(): "
                      "Flow, 0x%p, can't be suspended in state 0x%x",
                      flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      break;

    default:
      LOG_MSG_ERROR_2("ps_flow_suspend_ind(): "
                      "Flow, 0x%p, is in invalid state 0x%x",
                      flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
      ASSERT(0);
      break;

  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_flow_suspend_ind() */
#endif /* FEATURE_DATA_PS_QOS */



/*===========================================================================
FUNCTION PS_FLOW_GO_NULL_IND

DESCRIPTION
  Indicates that a flow is torn down

PARAMETERS
  flow_ptr  : ptr to flow on which to operate on.
  info_code : extended info code for the indication.

RETURN VALUE
   None

DEPENDENCIES
  All ps_flow indications must be posted from one task context
  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

SIDE EFFECTS
  Changes flow's state to FLOW_NULL and posts FLOW_NULL_EV
===========================================================================*/
void ps_flow_go_null_ind
(
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code
)
{
  ps_iface_event_info_u_type    event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_go_null_ind(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_3("ps_flow_go_null_ind(): "
                  "FLOW GO NULL IND 0x%p, state 0x%x, info code %d",
                  flow_ptr, PS_FLOWI_GET_STATE(flow_ptr), info_code);

  if (PS_FLOWI_GET_STATE(flow_ptr) != FLOW_NULL)
  {
    /*-----------------------------------------------------------------------
      Change state and post FLOW_NULL_EV
    -----------------------------------------------------------------------*/
    event_info.flow_event_info.state     = PS_FLOWI_GET_STATE(flow_ptr);
    event_info.flow_event_info.info_code = info_code;

    PS_FLOWI_SET_STATE(flow_ptr, FLOW_NULL);

#ifdef FEATURE_DATA_PS_QOS
    /*-----------------------------------------------------------------------
      Since mode handler doesn't care about default flow's state, don't
      generate event
    -----------------------------------------------------------------------*/
    if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
    {
      ps_flowi_invoke_event_cbacks(flow_ptr, FLOW_NULL_EV, event_info);
      ps_qsl_log_qos_state(flow_ptr, QSL_QOS_RELEASED, info_code);
    }
#endif /* FEATURE_DATA_PS_QOS */

    /*-----------------------------------------------------------------------
      Reset flow control mask. Otherwise, iface will forever be flow
      controlled forever if client doesn't flow enable.

      Need to enable tx traffic so that DS_WRITE_EVENT is posted in
      sockets layer when flow is in this state
    -----------------------------------------------------------------------*/
    ps_flow_enable_tx(flow_ptr, flow_ptr->flow_private.tx_flow_mask);
  }

  /*-------------------------------------------------------------------------
    Since this flow is no longer in use decrease phys link's ref cnt.
    Need to unbind even when ps_flow is in NULL state, to handle ppp_close()
    called immediately after ppp_open()
  -------------------------------------------------------------------------*/
  if (PS_FLOW_GET_ASSOC_PS_FLOW(flow_ptr) == NULL)
  {
    (void) ps_flowi_unbind_phys_link(flow_ptr);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_flow_go_null_ind() */



#ifdef FEATURE_DATA_PS_QOS
/*===========================================================================
FUNCTION PS_FLOW_QOS_MODIFY_ACCEPTED_IND()

DESCRIPTION
  This fucntion is called to indicate that QOS_MODIFY is accepted by network

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  flow_ptr  : ps_flow, whose QOS_MODIFY is accepted

RETURN VALUE
  None

DEPENDENCIES
  All ps_flow indications must be posted from one task context
  flow_ptr must be valid and must not be a default flow. Must be called
  inside a TASKLOCK()

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_qos_modify_accepted_ind
(
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code
)
{
  qos_info_type               * qos_info_ptr;
  qos_info_type               * qos_modify_info_ptr;
  ps_iface_event_info_u_type    event_info;
  qos_spec_field_mask_type      qos_modify_mask; /* QOS mask in QOS_MODIFY */
  qos_spec_field_mask_type      field_mask;      /* QOS mask in existing QOS*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(flow_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_flow_qos_modify_accepted_ind(): "
                    "NULL flow_ptr");
    return;
  }

  LOG_MSG_INFO2_1("ps_flow_qos_modify_accepted_ind(): "
                  "FLOW QOS MODIFY ACCEPTED IND 0x%p", flow_ptr);

  if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    qos_info_ptr        = flow_ptr->flow_private.qos_info_ptr;
    qos_modify_info_ptr = flow_ptr->flow_private.qos_modify_info_ptr;

    ASSERT(qos_info_ptr != NULL && qos_modify_info_ptr != NULL);

    qos_modify_mask = PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr);
    field_mask      = PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr);

    /*-----------------------------------------------------------------------
      Delete old Rx fltr spec if either Rx flow spec is deleted or Rx fltr
      spec is modified
    -----------------------------------------------------------------------*/
    if (PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_DELETED(flow_ptr) ||
        PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_MODIFIED(flow_ptr))
    {
      ps_flowi_delete_rx_fltr_spec(qos_info_ptr);
    }

    /*-----------------------------------------------------------------------
      Migrate Rx fltr spec from qos_modify_info_ptr to qos_info_ptr if it is
      either newly added or modified
    -----------------------------------------------------------------------*/
    if (PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_CREATED(flow_ptr) ||
        PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_MODIFIED(flow_ptr))
    {
      ASSERT(PS_FLOWI_GET_RX_FLTR_HANDLE(flow_ptr) == 0);
      qos_info_ptr->rx.fltr_list = qos_modify_info_ptr->rx.fltr_list;
      PS_FLOWI_SET_RX_FLTR_HANDLE(flow_ptr,
                                  PS_FLOWI_GET_MODIFY_RX_FLTR_HANDLE(flow_ptr));
    }

    /*-----------------------------------------------------------------------
      Update Rx flow spec if it is specified in QOS_MODIFY
    -----------------------------------------------------------------------*/
    if (qos_modify_mask & QOS_MASK_RX_FLOW)
    {
      /*---------------------------------------------------------------------
        Delete Rx auxiliary flow specs that are specified in old QOS spec
      ---------------------------------------------------------------------*/
      if (field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
      {
        ps_flowi_delete_aux_flow_spec(qos_info_ptr,
                                      QOS_MASK_RX_AUXILIARY_FLOWS);
      }

      /*---------------------------------------------------------------------
        Unset QOS field mask. Need this to handle deletion/modification of Rx
        flow spec correctly
      ---------------------------------------------------------------------*/
      qos_info_ptr->field_mask &= ~(QOS_MASK_RX_FLOW | QOS_MASK_RX_MIN_FLOW |
                                      QOS_MASK_RX_AUXILIARY_FLOWS);

      /*---------------------------------------------------------------------
        If app is not deleting Rx flow spec, update Rx flow spec
      ---------------------------------------------------------------------*/
      if (!PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_DELETED(flow_ptr))
      {
        qos_info_ptr->field_mask |= QOS_MASK_RX_FLOW;
        qos_info_ptr->rx.ipflow.req = qos_modify_info_ptr->rx.ipflow.req;

        if (qos_modify_mask & QOS_MASK_RX_MIN_FLOW)
        {
          qos_info_ptr->field_mask |= QOS_MASK_RX_MIN_FLOW;
          qos_info_ptr->rx.ipflow.min_req =
            qos_modify_info_ptr->rx.ipflow.min_req;
        }

        if (qos_modify_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
        {
          qos_info_ptr->field_mask |= QOS_MASK_RX_AUXILIARY_FLOWS;
          qos_info_ptr->rx.ipflow.aux_flow_list =
            qos_modify_info_ptr->rx.ipflow.aux_flow_list;
        }
      } /* if Rx flow is not deleted */
    } /* if Rx flow spec is requested in QOS_MODIFY */

    /*-----------------------------------------------------------------------
      Update Tx flow spec if it is specified in QOS_MODIFY
    -----------------------------------------------------------------------*/
    if (qos_modify_mask & QOS_MASK_TX_FLOW)
    {
      /*---------------------------------------------------------------------
        Delete Tx auxiliary flow specs that are specified in old QOS spec
      ---------------------------------------------------------------------*/
      if (field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
      {
        ps_flowi_delete_aux_flow_spec(qos_info_ptr,
                                      QOS_MASK_TX_AUXILIARY_FLOWS);
      }

      /*---------------------------------------------------------------------
        Unset QOS field mask. Need this to handle deletion/modification of Tx
        flow spec correctly
      ---------------------------------------------------------------------*/
      qos_info_ptr->field_mask &= ~(QOS_MASK_TX_FLOW | QOS_MASK_TX_MIN_FLOW |
                                      QOS_MASK_TX_AUXILIARY_FLOWS);

      /*---------------------------------------------------------------------
        If app is not deleting Tx flow spec, update Tx flow spec
      ---------------------------------------------------------------------*/
      if (!PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_DELETED(flow_ptr))
      {
        qos_info_ptr->field_mask |= QOS_MASK_TX_FLOW;
        qos_info_ptr->tx.ipflow.req = qos_modify_info_ptr->tx.ipflow.req;

        if (qos_modify_mask & QOS_MASK_TX_MIN_FLOW)
        {
          qos_info_ptr->field_mask |= QOS_MASK_TX_MIN_FLOW;
          qos_info_ptr->tx.ipflow.min_req =
            qos_modify_info_ptr->tx.ipflow.min_req;
        }

        if (qos_modify_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
        {
          qos_info_ptr->field_mask |= QOS_MASK_TX_AUXILIARY_FLOWS;
          qos_info_ptr->tx.ipflow.aux_flow_list =
            qos_modify_info_ptr->tx.ipflow.aux_flow_list;
        }
      } /* if Tx flow spec is not deleted */
    } /* if Tx flow spec is requested in QOS_MODIFY */

    /*-----------------------------------------------------------------------
      Reset qos_modify_info_ptr and free it
    -----------------------------------------------------------------------*/
    qos_modify_info_ptr->field_mask = QOS_MASK_INVALID;
    PS_MEM_FREE(flow_ptr->flow_private.qos_modify_info_ptr);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Cache default flow modify result
    -----------------------------------------------------------------------*/
    flow_ptr->event_info_cache.primary_qos_modify_result_info.
      is_modify_succeeded = TRUE;
  }

  /*-------------------------------------------------------------------------
    Post FLOW_MODIFY_ACCEPTED_EV
  -------------------------------------------------------------------------*/
  event_info.flow_event_info.state     = PS_FLOWI_GET_STATE(flow_ptr);
  event_info.flow_event_info.info_code = info_code;

  ps_flowi_invoke_event_cbacks(flow_ptr, FLOW_MODIFY_ACCEPTED_EV, event_info);
  ps_qsl_log_qos_state(flow_ptr, QSL_QOS_MODIFY_ACCEPTED, info_code);

} /* ps_flow_qos_modify_accepted_ind() */



/*===========================================================================
FUNCTION PS_FLOW_QOS_MODIFY_REJECTED_IND()

DESCRIPTION
  This fucntion is called to indicate that QOS_MODIFY is rejected by network

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  flow_ptr  : ps_flow, whose QOS_MODIFY is rejected

RETURN VALUE
  None

DEPENDENCIES
  All ps_flow indications must be posted from one task context
  flow_ptr must be valid and must not be a default flow. Must be called
  inside a TASKLOCK()

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_qos_modify_rejected_ind
(
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code
)
{
  ps_iface_event_info_u_type  event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("ps_flow_qos_modify_rejected_ind(): "
                  "FLOW QOS MODIFY REJECTED IND 0x%p", flow_ptr);

  if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    /*-----------------------------------------------------------------------
      Delete state associated with QOS_MODIFY
    -----------------------------------------------------------------------*/
    (void) ps_flowi_delete_qos_modify_spec(flow_ptr);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Cache default flow modify result
    -----------------------------------------------------------------------*/
    flow_ptr->event_info_cache.primary_qos_modify_result_info.
      is_modify_succeeded = FALSE;
  }

  /*-------------------------------------------------------------------------
    Post FLOW_MODIFY_REJECTED_EV
  -------------------------------------------------------------------------*/
  event_info.flow_event_info.state     = PS_FLOWI_GET_STATE(flow_ptr);
  event_info.flow_event_info.info_code = info_code;

  ps_flowi_invoke_event_cbacks(flow_ptr, FLOW_MODIFY_REJECTED_EV, event_info);
  ps_qsl_log_qos_state(flow_ptr, QSL_QOS_MODIFY_REJECTED, info_code);

} /* ps_flow_qos_modify_rejected_ind() */



/*===========================================================================
FUNCTION PS_FLOW_GENERIC_IND()

DESCRIPTION
  This fucntion is called to indicate that an event occured on ps_flow. These
  events doesn't change ps_flow state and are generic.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  ps_flow_ptr    : ps_flow on which event is posted
  event          : event that has occurred
  event_data_ptr : data associated with the event

RETURN VALUE
  None

DEPENDENCIES
  All ps_flow indications must be posted from one task context

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_generic_ind
(
  ps_flow_type              * ps_flow_ptr,
  ps_iface_event_enum_type    event,
  const void                * event_data_ptr
)
{
  ps_iface_event_info_u_type  event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(ps_flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_generic_ind(): "
                    "Invalid ps_flow 0x%p is passed", ps_flow_ptr);
    ASSERT(0);
    return;
  }

  memset(&event_info, 0, sizeof(ps_iface_event_info_u_type));
  LOG_MSG_INFO2_2("ps_flow_generic_ind(): "
                  "FLOW GENERIC IND 0x%p, event %d", ps_flow_ptr, event);

  switch (event)
  {
    case FLOW_PRIMARY_MODIFY_RESULT_EV:
      if (event_data_ptr == NULL)
      {
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        DATA_ERR_FATAL("ps_flow_generic_ind(): NULL parameter is passed");
        return;
      }

      if (!PS_FLOWI_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
      {
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        LOG_MSG_ERROR_2("ps_flow_generic_ind(): "
                        "Event %d is not supported on secondary ps_flow 0x%p",
                        event, ps_flow_ptr);
        ASSERT(0);
        return;
      }

      event_info.primary_qos_modify_result_info.is_modify_succeeded =
        *((boolean *) event_data_ptr);
      break;

    case FLOW_INFO_CODE_UPDATED_EV:
      if (event_data_ptr == NULL)
      {
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        DATA_ERR_FATAL("ps_flow_generic_ind(): NULL parameter is passed");
        return;
      }

      if (PS_FLOWI_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
      {
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        LOG_MSG_ERROR_2("ps_flow_generic_ind(): "
                        "Event %d is not supported on default ps_flow 0x%p",
                        event, ps_flow_ptr);
        ASSERT(0);
        return;
      }

      event_info.flow_event_info.state     = PS_FLOWI_GET_STATE( ps_flow_ptr);
      event_info.flow_event_info.info_code =
        *( ( ps_extended_info_code_enum_type *) event_data_ptr);

      ps_qsl_log_qos_info_code_update( ps_flow_ptr,
                                       event_info.flow_event_info.info_code);
      break;

    case FLOW_FLTR_AUX_INFO_UPDATED_EV:
      break;

    default:
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_flow_generic_ind(): "
                      "Event %d is not supported", event);
      ASSERT(0);
      return;
  }

  ps_flowi_invoke_event_cbacks(ps_flow_ptr, event, event_info);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_flow_generic_ind() */
#endif /* FEATURE_DATA_PS_QOS */



/*===========================================================================

                             UTILITY FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOW_BIND_PHYS_LINK()

DESCRIPTION
  Binds a flow to a phys link

PARAMETERS
  flow_ptr      : pointer to the flow serving the QOS. This should not be
                  a logical flow.
  phys_link_ptr : flow_ptr's binding

RETURN VALUE
   0 : on sucess
  -1 : on failure

DEPENDENCIES
  This function must be called from the same task context in which mode handler
  is running. Flow must not already be bound and must not be a default flow
  unless it is the default flow of a logical interface or of a physical
  interface supporting phys link dynamic binding.

SIDE EFFECTS
  None
===========================================================================*/
int ps_flow_bind_phys_link
(
  ps_flow_type       * flow_ptr,
  ps_phys_link_type  * phys_link_ptr
)
{
  int  ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr) || !PS_PHYS_LINK_IS_VALID(phys_link_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_flow_bind_phys_link(): "
                    "Invalid parameters are passed flow 0x%p, phys link 0x%p",
                    flow_ptr, phys_link_ptr);
    ASSERT(0);
    return -1;
  }

  if (PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr) != NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("Logical flow_ptr 0x%p passed to ps_flow_bind_phys_link",
                    flow_ptr);
    return -1;
  }

  ret_val = ps_flowi_bind_phys_link(flow_ptr, phys_link_ptr);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ps_flow_bind_phys_link() */



/*===========================================================================
FUNCTION PS_FLOW_UNBIND_PHYS_LINK()

DESCRIPTION
  Unbinds a phys link from ps_flow

PARAMETERS
  flow_ptr : pointer to the ps_flow serving the QOS. This should not be a
  logical flow.

RETURN VALUE
   0 : on sucess
  -1 : on failure

DEPENDENCIES
  This function must be called from the same task context in which mode handler
  is running. Flow must not be a default ps_flow.

SIDE EFFECTS
  None
===========================================================================*/
int ps_flow_unbind_phys_link
(
  ps_flow_type  * flow_ptr
)
{
  int  ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_unbind_phys_link(): "
                    "Invalid ps_flow 0x%p is passed", flow_ptr);
    ASSERT(0);
    return -1;
  }

  if (PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr) != NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("Logical flow_ptr 0x%p passed to ps_flow_unbind_phys_link",
                    flow_ptr);
    return -1;
  }

  ret_val = ps_flowi_unbind_phys_link(flow_ptr);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ps_flow_unbind_phys_link() */



/*===========================================================================
FUNCTION PS_FLOW_REBIND_PHYS_LINK()

DESCRIPTION
  Binds a flow to a phys link, which is different from its current binding

PARAMETERS
  flow_ptr      : pointer to the flow serving the QOS. This should not be a
                  logical flow.
  phys_link_ptr : flow_ptr's new binding

RETURN VALUE
   0 : on sucess
  -1 : on failure

DEPENDENCIES
  This function must be called from the same task context in which mode handler
  is running. Flow must have been already bound and flow must not be a default
  flow.

SIDE EFFECTS
  None
===========================================================================*/
int ps_flow_rebind_phys_link
(
  ps_flow_type       * flow_ptr,
  ps_phys_link_type  * phys_link_ptr
)
{
  int  ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr) || !PS_PHYS_LINK_IS_VALID(phys_link_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_flow_rebind_phys_link(): "
                    "Invalid parameters are passed flow 0x%p, phys link 0x%p",
                    flow_ptr, phys_link_ptr);
    ASSERT(0);
    return -1;
  }

  if (PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr) != NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("Logical flow_ptr 0x%p passed to ps_flow_rebind_phys_link",
                    flow_ptr);
    return -1;
  }

  if (PS_FLOWI_GET_PHYS_LINK(flow_ptr) == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_rebind_phys_link(): "
                    "Flow, 0x%p, is not bound", flow_ptr);
    ASSERT(0);
    return -1;
  }
  else if (PS_FLOWI_GET_PHYS_LINK(flow_ptr) == phys_link_ptr)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO2_2("ps_flow_rebind_phys_link(): "
                    "Flow, 0x%p, is rebinding to the same phys link 0x%p, it is "
                    "already bound to", flow_ptr, phys_link_ptr);
    return 0;
  }

  ret_val = ps_flowi_rebind_phys_link(flow_ptr, phys_link_ptr);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ps_flow_rebind_phys_link() */



/*===========================================================================
FUNCTION PS_FLOW_ENABLE_TX

DESCRIPTION
  Enables a flow to transmit traffic

PARAMETERS
  flow_ptr  : ptr to flow
  flow_mask : bit mask that identifies the flow controlling client

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_enable_tx
(
  ps_flow_type  * flow_ptr,
  uint32          flow_mask
)
{
  ps_iface_event_info_u_type    event_info;
  ds3g_flow_e_type              flow_type = DS_FLOW_ENABLE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_enable_tx(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    /* This function is called from any context. So it is possible that DS
       deleted ps_flow just before this function is called */
    return;
  }

  /*-------------------------------------------------------------------------
    Store the previous flow mask in the event info variable, and remove the
    client mask from the tx_flow_mask.
  -------------------------------------------------------------------------*/
  event_info.flow_mask = PS_FLOWI_GET_TX_MASK(flow_ptr);

  DS_FLOW_CTRL_SET_MASK(flow_type,
                        flow_ptr->flow_private.tx_flow_mask,
                        flow_mask);

  /*-------------------------------------------------------------------------
    Call the callbacks only on the transition to the flow being enabled
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_IS_TX_ENABLED(flow_ptr) &&
      event_info.flow_mask != ALL_FLOWS_ENABLED)
  {
#if 0
    LOG_MSG_INFO2_3("ps_flow_enable_tx(): "
                    "client 0x%x enabling flow on flow 0x%p -> mask 0x%x",
                    flow_mask, flow_ptr, flow_ptr->flow_private.tx_flow_mask);
#endif

    ps_flowi_invoke_event_cbacks(flow_ptr, FLOW_TX_ENABLED_EV, event_info);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_flow_enable_tx() */



/*===========================================================================
FUNCTION PS_FLOW_DISABLE_TX

DESCRIPTION
  Disables a flow from transmitting traffic

PARAMETERS
  flow_ptr  : ptr to flow
  flow_mask : bit mask that identifies the flow controlling client

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_disable_tx
(
  ps_flow_type  * flow_ptr,
  uint32          flow_mask
)
{
  ps_iface_event_info_u_type    event_info;
  ds3g_flow_e_type              flow_type = DS_FLOW_DISABLE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_disable_tx(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    /* This function is called from any context. So it is possible that DS
       deleted ps_flow just before this function is called */
    return;
  }

  /*-------------------------------------------------------------------------
    Store the previous flow mask in the event info variable, and remove the
    client mask from the tx_flow_mask.
  -------------------------------------------------------------------------*/
  event_info.flow_mask = PS_FLOWI_GET_TX_MASK(flow_ptr);

  DS_FLOW_CTRL_SET_MASK(flow_type,
                        flow_ptr->flow_private.tx_flow_mask,
                        flow_mask);

  /*-------------------------------------------------------------------------
    Call callbacks only on the transition to the flow being disabled
    Since we disable just now, no need to check if flow is disabled.
  -------------------------------------------------------------------------*/
  if (event_info.flow_mask == ALL_FLOWS_ENABLED)
  {
    LOG_MSG_INFO2_3("ps_flow_disable_tx(): "
                    "client 0x%x disabling flow on flow 0x%p -> mask 0x%x",
                    flow_mask, flow_ptr, flow_ptr->flow_private.tx_flow_mask);

    ps_flowi_invoke_event_cbacks(flow_ptr, FLOW_TX_DISABLED_EV, event_info);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_flow_disable_tx() */


/*===========================================================================
FUNCTION PS_FLOWI_ENABLE_DISABLE_TX_EX()

DESCRIPTION
  Helper function which posts a cmd to PS task to enable or disable a flow to transmit traffic

PARAMETERS
  flow_ptr  : ptr to flow
  flow_mask : bit mask that identifies the caller.
  flow_param: boolean to decide whether to enable or disable the flow. 

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_enable_disable_tx_ex
(
  ps_flow_type  * flow_ptr,
   uint32          flow_mask,
   boolean         flow_param
)
{
  ps_flowi_flow_ctrl_cmd_info_type  * flow_ctrl_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(flow_ctrl_info_ptr, sizeof(ps_flowi_flow_ctrl_cmd_info_type),
                                   ps_flowi_flow_ctrl_cmd_info_type*);

  if (NULL == flow_ctrl_info_ptr)
  {
    return;
  }

  flow_ctrl_info_ptr->ps_flow_ptr = flow_ptr;
  flow_ctrl_info_ptr->flow_mask   = flow_mask;
  flow_ctrl_info_ptr->is_enabled  = flow_param;

  ps_send_cmd(PS_FLOW_FLOW_CTRL_CMD, flow_ctrl_info_ptr);
  return;

} /* ps_flowi_enable_disable_tx_ex() */


/*===========================================================================
FUNCTION PS_FLOW_ENABLE_TX_EX()

DESCRIPTION
  Posts a cmd to PS task enable a flow to transmit traffic

PARAMETERS
  flow_ptr  : ptr to flow
  flow_mask : bit mask that identifies the caller.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_enable_tx_ex
(
  ps_flow_type  * flow_ptr,
  uint32          flow_mask
)
{
  ps_flowi_enable_disable_tx_ex(flow_ptr, flow_mask, TRUE);
  return;

} /* ps_flow_enable_tx_ex() */


/*===========================================================================
FUNCTION PS_FLOW_DISABLE_TX_EX()

DESCRIPTION
  Posts a cmd to PS task to disable a flow from transmitting traffic

PARAMETERS
  flow_ptr  : ptr to flow
  flow_mask : bit mask that identifies the caller.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_disable_tx_ex
(
  ps_flow_type  * flow_ptr,
  uint32          flow_mask
)
{
  ps_flowi_enable_disable_tx_ex(flow_ptr, flow_mask, FALSE);
  return;

} /* ps_flow_disable_tx_ex() */


/*===========================================================================
FUNCTION PS_FLOWI_FLOW_CTRL_CMD_HANDLER()

DESCRIPTION
  This function processes PS_FLOW_FLOW_CTRL_CMD.

PARAMETERS
  ps_cmd          : Must be PS_FLOW_FLOW_CTRL_CMD.
  ps_cmd_data_ptr : cmd data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_flow_ctrl_cmd_handler
(
  ps_cmd_enum_type    ps_cmd,
  void              * ps_cmd_data_ptr
)
{
  ps_flowi_flow_ctrl_cmd_info_type  * flow_ctrl_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ps_cmd_data_ptr)
  {
    LOG_MSG_ERROR_1("NULL cmd data, cmd %d", ps_cmd);
    return;
  }

  do
  {
    flow_ctrl_info_ptr = (ps_flowi_flow_ctrl_cmd_info_type *) ps_cmd_data_ptr;

    if (PS_FLOW_FLOW_CTRL_CMD != ps_cmd)
    {
      LOG_MSG_ERROR_1("Invalid cmd %d", ps_cmd);
      break;
    }

    if (TRUE == flow_ctrl_info_ptr->is_enabled)
    {
      ps_flow_enable_tx(flow_ctrl_info_ptr->ps_flow_ptr,
                        flow_ctrl_info_ptr->flow_mask);
    }
    else
    {
      ps_flow_disable_tx(flow_ctrl_info_ptr->ps_flow_ptr,
                         flow_ctrl_info_ptr->flow_mask);
    }
  } while (0);

  PS_SYSTEM_HEAP_MEM_FREE(flow_ctrl_info_ptr);
  return;

} /* ps_flowi_flow_ctrl_cmd_handler() */

#ifdef FEATURE_DATA_PS_QOS
/*===========================================================================
FUNCTION PS_FLOW_SET_ASSOC_FLOW()

DESCRIPTION
  This function sets associated ps_flow for a given ps_flow, allocates
  the meta-info, and registers the specified event callback function to
  handle events received from the associated flow. If no event callback
  is supplied, a default handler is used.

  If this function returns error, the caller is expected to delete
  the logical flow. Else the behaviour is undeterministic.

PARAMETERS
  flow_ptr       : pointer to the flow serving the QOS.
  assoc_flow_ptr : associated ps_flow
  ps_errno       : error

RETURN VALUE
     0 : on success
    -1 : on failure (any of the passed in parameters is bad)

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flow_set_assoc_flow
(
  ps_flow_type                        * flow_ptr,
  ps_flow_type                        * assoc_flow_ptr,
  ps_flow_assoc_flow_event_cback_type   ev_cback_func,
  int16                               * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    LOG_MSG_ERROR_0("ps_flow_set_assoc_flow(): "
                    "NULL errno");
    return -1;
  }

  if (!PS_FLOW_IS_VALID(flow_ptr) || !PS_FLOW_IS_VALID(assoc_flow_ptr))
  {
    LOG_MSG_ERROR_0("ps_flow_set_assoc_flow(): "
                    "Invalid ps_flow is passed");
    *ps_errno =  DS_EINVAL;
    return -1;
  }

  flow_ptr->flow_private.logical_flow.assoc_flow_ptr = assoc_flow_ptr;
  PS_FLOWI_SET_ASSOC_COOKIE(flow_ptr, PS_FLOWI_GET_COOKIE(assoc_flow_ptr));
  /*-------------------------------------------------------------------------
    Set filtering result to associated flow
  -------------------------------------------------------------------------*/
  if (NULL == flow_ptr->flow_private.logical_flow.mi_ptr)
  {
    LOG_MSG_ERROR_0("ps_flow_set_assoc_flow(): "
                    "Logical flow mi ptr is null");
    *ps_errno =  DS_EINVAL;
    return -1;
  }
  else
  {
    PS_TX_META_SET_FILTER_RESULT(flow_ptr->flow_private.logical_flow.mi_ptr,
                                 IP_FLTR_CLIENT_QOS_OUTPUT,
                                 assoc_flow_ptr );
  }

  /*-------------------------------------------------------------------------
    Register for events on the associated flow
  -------------------------------------------------------------------------*/
  if (0 != ps_flowi_assoc_flow_ev_cback_reg(flow_ptr, ev_cback_func, ps_errno))
  {
    LOG_MSG_ERROR_1("ps_flow_set_assoc_flow(): "
                    "ps_flowi_assoc_flow_ev_cback_reg failed, err %d",
                    *ps_errno);
    return -1;
  }

  return 0;
} /* ps_flow_set_assoc_flow() */


/*===========================================================================
FUNCTION PS_FLOW_SET_GRANTED_FLOW()

DESCRIPTION
  This function stores the currently granted flow spec in each direction
  (if non null). If mode handler wants to indicate that QoS is not granted in
  a direction, pass flow type with field mask set to IPFLOW_MASK_NONE

PARAMETERS
  flow_ptr  : pointer to the flow serving the QOS.
  rx_fl_ptr : Ptr to Rx flow type
  tx_fl_ptr : Ptr to Tx flow type

RETURN VALUE
  None

DEPENDENCIES
  This function must be called from the same task context in which mode handler
  is running. Flow must not be a default flow.

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_set_granted_flow
(
  const ps_flow_type  * flow_ptr,
  const ip_flow_type  * rx_ip_flow_ptr,
  const ip_flow_type  * tx_ip_flow_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_set_granted_flow(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_set_granted_flow(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return;
  }

  if (PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr) != NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_set_granted_flow(): "
                    "Setting QoS is not supported on logical ps_flow 0x%p",
                    flow_ptr);
    ASSERT(0);
    return;
  }

  if (rx_ip_flow_ptr)
  {
    flow_ptr->flow_private.qos_info_ptr->rx.ipflow.granted = *rx_ip_flow_ptr;
  }

  if (tx_ip_flow_ptr)
  {
    flow_ptr->flow_private.qos_info_ptr->tx.ipflow.granted = *tx_ip_flow_ptr;
  }

  ps_qsl_log_granted_qos_spec(flow_ptr);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ps_flow_set_granted_flow() */



/*===========================================================================
FUNCTION PS_FLOW_GET_RX_FLTR_CNT()

DESCRIPTION
  Returns number of Rx filters, that match the IP version, specified as an
  argument. Tx filters can pertain to either QOS_REQUEST/active QoS or
  pending QOS_MODIFY

PARAMETERS
  flow_ptr  : pointer to the flow serving the QOS
  ip_vsn    : set of filters, a client is interested in. Mix of V4 and
              V6 filters can be installed on a flow and client may
              choose to get only V4 filters
  is_modify : is the client interested in fetching Rx filters associated with
              pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  Number of Rx filters : on success
  -1                   : otherwise

DEPENDENCIES
  This function must be called from the same task context in which mode handler
  is running. Flow must not be a default flow.

SIDE EFFECTS
  None
===========================================================================*/
int8 ps_flow_get_rx_fltr_cnt
(
  ps_flow_type          * flow_ptr,
  ip_version_enum_type    ip_vsn,
  boolean                 is_modify
)
{
  qos_info_type             * qos_info_ptr;
  ps_flow_rx_fltr_buf_type  * curr_fltr_buf;
  int8                        rx_fltr_cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_cnt(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return -1;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_cnt(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return -1;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return -1;
  }

  if (0 == list_is_initialized(&(qos_info_ptr->rx.fltr_list)))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_flow_get_rx_fltr_cnt(): "
                    "Uninitialized rx fltr list, ps_flow 0x%p", flow_ptr);
    return 0;
  }

  /*-------------------------------------------------------------------------
    Count the number of filters which match the given IP version. Consider
    only those filters that belong to given ps_flow
  -------------------------------------------------------------------------*/
  curr_fltr_buf = list_peek_front(&(qos_info_ptr->rx.fltr_list));
  while (curr_fltr_buf != NULL)
  {
    if (curr_fltr_buf->filter.ip_vsn == ip_vsn)
    {
      rx_fltr_cnt++;
    }

    curr_fltr_buf =
      list_peek_next(&(qos_info_ptr->rx.fltr_list), &(curr_fltr_buf->link));
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return rx_fltr_cnt;

} /* ps_flow_get_rx_fltr_cnt() */



/*===========================================================================
FUNCTION PS_FLOW_GET_RX_FLTR_HANDLE()

DESCRIPTION
  This function returns a handle which can be used to fetch Rx filters
  associated with a flow matching IP version specified as an argument.

  Rx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  flow_ptr  : pointer to the flow whose filters need to be fetched
  ip_vsn    : set of filters, a client is interested in. Mix of V4 and
              V6 filters can be installed on a flow and client may
              choose to get only V4 filters
  is_modify : is the client interested in fetching Rx filters associated with
              pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  A handle using which Rx filters can be fetched : on success
  NULL                                           : otherwise

DEPENDENCIES
  Flow must not be a default flow.

  This function must be called from the same task context in which mode handler
  is running. Otherwise filters could be deleted (for example because
  QOS_MODIFY is accepted/rejected) while client is fetching them which could
  lead to incorrect operations.

  Sample usage to fetch Rx IPV4 filters pertaining to QOS_REQUEST is
    handle = ps_flow_get_rx_fltr_handle(flow_ptr, IPV4, FALSE)
    while (handle != NULL)
    {
      ps_flow_get_rx_fltr_by_handle(flow_ptr, IPV4, FALSE, handle,
                                    fltr, prcd, new_handle)
      handle = new_handle
    }

SIDE EFFECTS
  None
===========================================================================*/
void *ps_flow_get_rx_fltr_handle
(
  ps_flow_type          * flow_ptr,
  ip_version_enum_type    ip_vsn,
  boolean                 is_modify
)
{
  qos_info_type             * qos_info_ptr;
  ps_flow_rx_fltr_buf_type  * curr_fltr_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_handle(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_handle(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return NULL;
  }

  if (0 == list_is_initialized(&(qos_info_ptr->rx.fltr_list)))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_flow_get_rx_fltr_handle(): "
                    "Uninitialized rx fltr list, ps_flow 0x%p", flow_ptr);
    return NULL;
  }


  /*-------------------------------------------------------------------------
    Return a handle to the filter which matches IP version
  -------------------------------------------------------------------------*/
  curr_fltr_buf = list_peek_front(&(qos_info_ptr->rx.fltr_list));
  while (curr_fltr_buf != NULL && curr_fltr_buf->filter.ip_vsn != ip_vsn)
  {
    curr_fltr_buf =
      list_peek_next(&(qos_info_ptr->rx.fltr_list), &(curr_fltr_buf->link));
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return curr_fltr_buf;

} /* ps_flow_get_rx_fltr_handle() */

/*===========================================================================
FUNCTION PS_FLOW_GET_RX_FLTR_HANDLE_EX()

DESCRIPTION
  This function returns a handle which can be used to fetch Rx filters
  associated with a flow independent of the IP version.

  Rx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  flow_ptr  : pointer to the flow whose filters need to be fetched
  is_modify : is the client interested in fetching Rx filters associated with
              pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  A handle using which Rx filters can be fetched : on success
  NULL                                           : otherwise

DEPENDENCIES
  Flow must not be a default flow.

  This function must be called from the same task context in which mode handler
  is running. Otherwise filters could be deleted (for example because
  QOS_MODIFY is accepted/rejected) while client is fetching them which could
  lead to incorrect operations.

  Sample usage to fetch Rx ilters pertaining to QOS_REQUEST is
    handle = ps_flow_get_rx_fltr_handle(_exflow_ptr, FALSE)
    while (handle != NULL)
    {
      ps_flow_get_rx_fltr_by_handle_ex(flow_ptr, FALSE, handle,
                                    fltr, prcd, new_handle)
      handle = new_handle
    }

SIDE EFFECTS
  None
===========================================================================*/
void *ps_flow_get_rx_fltr_handle_ex
(
  ps_flow_type          * flow_ptr,
  boolean                 is_modify
)
{
  qos_info_type             * qos_info_ptr;
  ps_flow_rx_fltr_buf_type  * curr_fltr_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_handle_ex(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_handle_ex(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return NULL;
  }

  if (0 == list_is_initialized(&(qos_info_ptr->rx.fltr_list)))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_flow_get_rx_fltr_handle_ex(): "
                    "Uninitialized rx fltr list, ps_flow 0x%p", flow_ptr);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Return a handle to the filter
  -------------------------------------------------------------------------*/
  curr_fltr_buf = list_peek_front(&(qos_info_ptr->rx.fltr_list));

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return curr_fltr_buf;

} /* ps_flow_get_rx_fltr_handle_ex() */


/*===========================================================================
FUNCTION PS_FLOW_GET_RX_FLTR_BY_HANDLE()

DESCRIPTION
  Given a handle, this function returns a Rx filter, and also the
  precedence of the filter. The precedence is calculated among all the
  filters installed on all flows assocaited with the iface.
  A new handle is returned so that next filter, matching IP version specified
  as an argument, is fetched when this function is invoked with that handle.

  Rx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  flow_ptr       : pointer to the flow
  ip_vsn         : set of filters, a client is interested in. Mix of V4 and
                   V6 filters can be installed on a flow and client may
                   choose to get only V4 filters
  rx_fltr_handle : handle using which a filter is fetched
  is_modify      : is the client interested in fetching Rx filters associated
                   with pending QOS_MODIFY? If so value must be TRUE
  fltr           : OUT PARAM, pointer to Rx filter
  prcd           : OUT PARAM, precedence of Rx filter
  next_handle    : OUT PARAM, new handle which can be used to fetch next
                   Rx filter
RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
  Flow must not be a default flow.

  This function must be called from the same task context in which mode handler
  is running. Otherwise filters could be deleted (for example because
  QOS_MODIFY is accepted/rejected) while client is fetching them which could
  lead to incorrect operations.

  Do not cache filter and access its contents later. Since a filter can be
  deleted, this may lead to memory corruption or data aborts.

  ip_vsn must match IP version of filter that rx_fltr_handle points to. So
  if client gets a handle to V4 filter using ps_flow_get_rx_fltr_handle, it
  MUST fetch only V4 filters in ps_flow_get_rx_fltr_by_handle.

  Sample usage to fetch Rx IPV4 filters pertaining to QOS_REQUEST is
    handle = ps_flow_get_rx_fltr_handle(flow_ptr, IPV4, FALSE)
    while (handle != NULL)
    {
      ps_flow_get_rx_fltr_by_handle(flow_ptr, IPV4, FALSE, handle,
                                    fltr, prcd, new_handle)
      handle = new_handle
    }

SIDE EFFECTS
  next_handle is set so that if this function is called with that handle,
  next Rx filter is fetched
===========================================================================*/
boolean ps_flow_get_rx_fltr_by_handle
(
  ps_flow_type           * flow_ptr,
  ip_version_enum_type     ip_vsn,
  void                   * rx_fltr_handle,
  boolean                  is_modify,
  ip_filter_type        ** fltr,
  uint8                  * prcd,
  void                  ** next_handle
)
{
  qos_info_type             * qos_info_ptr;
  ps_flow_rx_fltr_buf_type  * rx_fltr_buf;
  ps_flow_rx_fltr_buf_type  * tmp_rx_fltr_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (fltr == NULL || prcd == NULL || next_handle == NULL)
  {
    DATA_ERR_FATAL("ps_flow_get_rx_fltr_by_handle(): Invalid parameters are passed");
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_by_handle(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_by_handle(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (rx_fltr_handle == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    *next_handle = NULL;
    *fltr        = NULL;

    return FALSE;
  }

  rx_fltr_buf = (ps_flow_rx_fltr_buf_type *) rx_fltr_handle;
  if (!ps_mem_is_valid(rx_fltr_buf, PS_MEM_RX_QOS_FLTR_BUF_TYPE))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_by_handle(): "
                    "Buffer (0x%X) doesn't belong to Rx filter's mem pool",
                    rx_fltr_handle);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Make sure that API is used correctly. Its invalid to get handle for a
    specific ip_vsn and use it for another ip_vsn
  -------------------------------------------------------------------------*/
  if (rx_fltr_buf->filter.ip_vsn != ip_vsn)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_3("ps_flow_get_rx_fltr_by_handle(): "
                    "IP version, %d, of passed in handle, 0x%p, is different from "
                    "passed in IP version, %d",
                    rx_fltr_buf->filter.ip_vsn, rx_fltr_buf, ip_vsn);
    ASSERT(0);
    return FALSE;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return FALSE;
  }

  if (0 == list_is_initialized(&(qos_info_ptr->rx.fltr_list)))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_flow_get_rx_fltr_by_handle(): "
                    "Uninitialized rx fltr list, ps_flow 0x%p", flow_ptr);
    return FALSE;
  }

  tmp_rx_fltr_buf = list_peek_front(&(qos_info_ptr->rx.fltr_list));
  while (tmp_rx_fltr_buf != NULL && tmp_rx_fltr_buf != rx_fltr_buf)
  {
    tmp_rx_fltr_buf =
      list_peek_next(&(qos_info_ptr->rx.fltr_list), &(tmp_rx_fltr_buf->link));
  }

  if (tmp_rx_fltr_buf == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_by_handle(): "
                    "Unknown buffer (0x%X) is passed", rx_fltr_handle);
    ASSERT(0);
    return FALSE;
  }

  *fltr = &(rx_fltr_buf->filter);
  *prcd = rx_fltr_buf->precedence;

  /*-------------------------------------------------------------------------
    Return a handle to the filter which matches IP version
  -------------------------------------------------------------------------*/
  tmp_rx_fltr_buf =
    list_peek_next(&(qos_info_ptr->rx.fltr_list), &(rx_fltr_buf->link));
  while (tmp_rx_fltr_buf != NULL && tmp_rx_fltr_buf->filter.ip_vsn != ip_vsn)
  {
    tmp_rx_fltr_buf =
      list_peek_next(&(qos_info_ptr->rx.fltr_list), &(tmp_rx_fltr_buf->link));
  }

  *next_handle = tmp_rx_fltr_buf;
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;

} /* ps_flow_get_rx_fltr_by_handle() */


/*===========================================================================
FUNCTION PS_FLOW_GET_RX_FLTR_BY_HANDLE_EX()

DESCRIPTION
  Given a handle, this function returns a Rx filter, and also the
  precedence of the filter independent of the IP version.
  A new handle is returned so that next filter is fetched when
  this function is invoked with that handle.

  Rx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  flow_ptr       : pointer to the flow
  rx_fltr_handle : handle using which a filter is fetched
  is_modify      : is the client interested in fetching Rx filters associated
                   with pending QOS_MODIFY? If so value must be TRUE
  fltr           : OUT PARAM, pointer to Rx filter
  prcd           : OUT PARAM, precedence of Rx filter
  next_handle    : OUT PARAM, new handle which can be used to fetch next
                   Rx filter
RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
  Flow must not be a default flow.

  This function must be called from the same task context in which mode handler
  is running. Otherwise filters could be deleted (for example because
  QOS_MODIFY is accepted/rejected) while client is fetching them which could
  lead to incorrect operations.

  Do not cache filter and access its contents later. Since a filter can be
  deleted, this may lead to memory corruption or data aborts.

  Sample usage to fetch Rx filters pertaining to QOS_REQUEST is
    handle = ps_flow_get_rx_fltr_handle_ex(flow_ptr, FALSE)
    while (handle != NULL)
    {
      ps_flow_get_rx_fltr_by_handle_ex(flow_ptr, FALSE, handle,
                                    fltr, prcd, new_handle)
      handle = new_handle
    }

SIDE EFFECTS
  next_handle is set so that if this function is called with that handle,
  next Rx filter is fetched
===========================================================================*/
boolean ps_flow_get_rx_fltr_by_handle_ex
(
  ps_flow_type           * flow_ptr,
  void                   * rx_fltr_handle,
  boolean                  is_modify,
  ip_filter_type        ** fltr,
  uint8                  * prcd,
  void                  ** next_handle
)
{
  qos_info_type             * qos_info_ptr;
  ps_flow_rx_fltr_buf_type  * rx_fltr_buf;
  ps_flow_rx_fltr_buf_type  * tmp_rx_fltr_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (fltr == NULL || prcd == NULL || next_handle == NULL)
  {
    DATA_ERR_FATAL("ps_flow_get_rx_fltr_by_handle_ex(): Invalid parameters are passed");
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_by_handle_ex(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_by_handle_ex(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (rx_fltr_handle == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    *next_handle = NULL;
    *fltr        = NULL;
    return FALSE;
  }

  rx_fltr_buf = (ps_flow_rx_fltr_buf_type *) rx_fltr_handle;
  if (!ps_mem_is_valid(rx_fltr_buf, PS_MEM_RX_QOS_FLTR_BUF_TYPE))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_by_handle_ex(): "
                    "Buffer (0x%X) doesn't belong to Rx filter's mem pool",
                    rx_fltr_handle);
    ASSERT(0);
    return FALSE;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return FALSE;
  }

  if (0 == list_is_initialized(&(qos_info_ptr->rx.fltr_list)))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_flow_get_rx_fltr_by_handle_ex(): "
                    "Uninitialized rx fltr list, ps_flow 0x%p", flow_ptr);
    return FALSE;
  }

  tmp_rx_fltr_buf = list_peek_front(&(qos_info_ptr->rx.fltr_list));
  while (tmp_rx_fltr_buf != NULL && tmp_rx_fltr_buf != rx_fltr_buf)
  {
    tmp_rx_fltr_buf =
      list_peek_next(&(qos_info_ptr->rx.fltr_list), &(tmp_rx_fltr_buf->link));
  }

  if (tmp_rx_fltr_buf == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_by_handle_ex(): "
                    "Unknown buffer (0x%X) is passed", rx_fltr_handle);
    ASSERT(0);
    return FALSE;
  }

  *fltr = &(rx_fltr_buf->filter);
  *prcd = rx_fltr_buf->precedence;

  /*-------------------------------------------------------------------------
    Return a handle to the filter
  -------------------------------------------------------------------------*/
  tmp_rx_fltr_buf =
    list_peek_next(&(qos_info_ptr->rx.fltr_list), &(rx_fltr_buf->link));

  *next_handle = tmp_rx_fltr_buf;
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;

} /* ps_flow_get_rx_fltr_by_handle_ex() */

/*===========================================================================
FUNCTION PS_FLOW_GET_RX_FLTR_SPEC()

DESCRIPTION
  Given a flow ptr, this function returns all the Rx filters
  associated with a flow.

PARAMETERS
  iface_ptr      : pointer to iface
  flow_ptr       : pointer to the flow
  ip_fltr_spec   : OUT PARAM, pointer to Rx filter spec
  num_fltrs      : Number of filters to be retrieved
  ps_errno       : specific error code in case operation fails

RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
None

SIDE EFFECTS
None
===========================================================================*/
boolean ps_flow_get_rx_fltr_spec
(
  ps_iface_type       * iface_ptr,
  ps_flow_type        * flow_ptr,
  ip_filter_spec_type * ip_fltr_spec,
  boolean               is_modify,
  uint8                 num_fltrs,
  int16               * ps_errno
)
{
  ip_filter_type            * filter;
  uint8                       precedence = 0;
  uint8                       rx_fltr_cnt;
  void                      * rx_handle;   /* Handle to the Tx filter         */
  void                      * new_handle;  /* Handle to the next Tx filter         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_flow_get_rx_fltr_spec(): NULL parameter is passed");
    return FALSE;
  }

  if(ip_fltr_spec == NULL)
  {
    DATA_ERR_FATAL("ps_flow_get_rx_fltr_spec(): Null value passed to get_all_rx_fltrs()");
    *ps_errno = DS_EINVAL;
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_spec(): "
                    "Invalid ps_iface, 0x%p, is passed", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return FALSE;
  }

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_spec(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_rx_fltr_spec(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return FALSE;
  }

  rx_fltr_cnt = ps_flowi_get_rx_fltr_cnt_ex(flow_ptr, is_modify);

  if(num_fltrs < rx_fltr_cnt)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_2("ps_flow_get_rx_fltr_spec(): "
                    "The number of filters supplied do not match the number of"
                    "filters already installed for the flow",
                    num_fltrs, rx_fltr_cnt);
    *ps_errno = DS_ETRUNCATED;
    return FALSE;
  }

  rx_handle = ps_flowi_get_rx_fltr_handle_ex( flow_ptr, is_modify );

  while ( rx_handle != NULL && num_fltrs > 0)
  {
    if(ps_flowi_get_rx_fltr_by_handle_ex(flow_ptr,
                                         rx_handle,
                                         is_modify,
                                         &filter,
                                         &precedence,
                                         &new_handle) == FALSE)
    {
      LOG_MSG_INFO1_1("ps_flow_get_rx_fltr_spec(): "
                      "Could not get RX filter by handle for flow %d",
                      flow_ptr);
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return FALSE;
    }

    rx_handle = new_handle;
    memscpy((void *)&(ip_fltr_spec->list_ptr[ip_fltr_spec->num_filters]),
            sizeof (ip_filter_type),
            (const void *)filter, 
            sizeof (ip_filter_type));
    ip_fltr_spec->num_filters++;
    num_fltrs--;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;
} /* ps_flow_get_rx_fltr_spec() */

/*===========================================================================
FUNCTION PS_FLOW_SET_RX_FILTER_SPEC()

DESCRIPTION
  Given the ip filter spec, this fucntion sets the filter id and precedence
  of all the Rx filters for which they previous values have been modified.

PARAMETERS
  iface_ptr      : pointer to iface
  flow_ptr       : pointer to the flow
  ip_fltr_spec   : OUT PARAM, pointer to Rx filter spec

RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
None

SIDE EFFECTS
None

===========================================================================*/
boolean ps_flow_set_rx_filter_spec
(
  ps_iface_type         * iface_ptr,
  ps_flow_type          * flow_ptr,
  ip_filter_spec_type   * ip_fltr_spec,
  boolean                 modify_flag
)
{
  ps_flow_rx_fltr_buf_type  * curr_rx_fltr_buf = NULL;
  uint8                       index = 0;
  uint8                       fltr_cnt;
  uint8                       rx_fltr_cnt = 0;
  qos_info_type             * qos_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ip_fltr_spec == NULL)
  {
    LOG_MSG_ERROR_0("ps_flow_set_rx_filter_spec(): "
                    "Null value passed to set_rx_filter_spec()");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Validate that number of filters is in between (0, MAX_FLTR_PER_REQ]
  -----------------------------------------------------------------------*/
  if (ip_fltr_spec->num_filters == 0)
  {
    LOG_MSG_INFO1_0("ps_flow_set_rx_filter_spec(): "
                    "Zero filters specified");
    return FALSE;
  }

  if (ip_fltr_spec->num_filters > MAX_FLTR_PER_REQ)
  {
    LOG_MSG_INFO1_3("ps_flow_set_rx_filter_spec(): "
                    "Too many filters specified: %d, max = %d, for flow 0x%p",
                    ip_fltr_spec->num_filters, MAX_FLTR_PER_REQ, flow_ptr);
    return FALSE;
  }

  if (ip_fltr_spec->list_ptr == NULL)
  {
    LOG_MSG_INFO1_1("ps_flow_set_rx_filter_spec(): "
                    "NULL list_ptr in fltr spec for flow 0x%p", flow_ptr);
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_set_rx_filter_spec(): "
                    "Invalid ps_iface, 0x%p, is passed", iface_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_set_rx_filter_spec(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_set_rx_filter_spec(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, modify_flag);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    ASSERT(0);
    return FALSE;
  }

  if (0 == list_is_initialized(&(qos_info_ptr->rx.fltr_list)))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_flow_set_rx_filter_spec(): "
                    "Uninitialized rx fltr list, ps_flow 0x%p", flow_ptr);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Count the number of filters. Consider only those filters that
    belong to given ps_flow
  -------------------------------------------------------------------------*/
  rx_fltr_cnt = (uint8)list_size(&(qos_info_ptr->rx.fltr_list));
  fltr_cnt = ip_fltr_spec->num_filters;

  if (rx_fltr_cnt != fltr_cnt)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_flow_set_rx_filter_spec(): "
                    "The number of flows supplied 0x%d do not match with the flows"
                    "stored in iface, 0x%d",
                    ip_fltr_spec->num_filters, rx_fltr_cnt);
    return FALSE;
  }

  curr_rx_fltr_buf = list_peek_front(&(qos_info_ptr->rx.fltr_list));

  while (curr_rx_fltr_buf != NULL)
  {
    memscpy(&curr_rx_fltr_buf->filter,
            sizeof (ip_filter_type),
            &ip_fltr_spec->list_ptr[index], 
            sizeof (ip_filter_type));
     curr_rx_fltr_buf =
       list_peek_next(&(qos_info_ptr->rx.fltr_list), &(curr_rx_fltr_buf->link));
     index++;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;
} /* ps_flow_set_rx_filter_spec() */

/*===========================================================================
FUNCTION PS_FLOW_GET_AUX_FLOW_SPEC_CNT()

DESCRIPTION
  Returns number of auxiliary flow specs for a given direction
  pertaining to either QOS_REQUEST/active QoS or pending QOS_MODIFY

PARAMETERS
  flow_ptr  : pointer to the flow serving the QOS
  flow_type : direction the client is interested in. Auxiliary flow specs are
              supported in both Rx and Tx direction and client is required to
              specify a direction. Its value can be either QOS_MASK_TX_FLOW
              or QOS_MASK_RX_FLOW
  is_modify : is the client interested in fetching auxiliary flow specs
              associated with pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  Number of auxiliary flow specs : on success
  -1                             : otherwise

DEPENDENCIES
  This function must be called from the same task context in which mode handler
  is running. Flow must not be a default flow.

SIDE EFFECTS
  None
===========================================================================*/
list_size_type ps_flow_get_aux_flow_spec_cnt
(
  ps_flow_type                   * flow_ptr,
  qos_spec_field_mask_enum_type    flow_type,
  boolean                          is_modify
)
{
  qos_info_type       * qos_info_ptr;
  list_type           * aux_flow_list_ptr;
  list_size_type        aux_flow_spec_cnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_aux_flow_spec_cnt(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return 0;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_aux_flow_spec_cnt(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return 0;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return 0;
  }

  /*-------------------------------------------------------------------------
    Point to appropriate aux flow list based on flow_type
  -------------------------------------------------------------------------*/
  if (flow_type == QOS_MASK_RX_FLOW)
  {
    aux_flow_list_ptr = &(qos_info_ptr->rx.ipflow.aux_flow_list);
  }
  else if (flow_type == QOS_MASK_TX_FLOW)
  {
    aux_flow_list_ptr = &(qos_info_ptr->tx.ipflow.aux_flow_list);
  }
  else
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_flow_get_aux_flow_spec_cnt(): "
                    "Invalid flow_type %d, ps_flow 0x%p", flow_type, flow_ptr);
    ASSERT(0);
    return 0;
  }

  if (0 == list_is_initialized(aux_flow_list_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_2("ps_flow_get_aux_flow_spec_cnt(): "
                    "Uninitialized aux flow spec, ps_flow 0x%p, flow_type %d",
                    flow_ptr, flow_type);
    return 0;
  }

  aux_flow_spec_cnt = list_size(aux_flow_list_ptr);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return aux_flow_spec_cnt;

} /* ps_flow_get_aux_flow_spec_cnt() */



/*===========================================================================
FUNCTION PS_FLOW_GET_AUX_FLOW_SPEC_HANDLE()

DESCRIPTION
  Returns a handle which can be used to fetch auxiliary flow
  specs for a given direction pertaining to either QOS_REQUEST/active QoS or
  pending QOS_MODIFY

PARAMETERS
  flow_ptr  : pointer to the flow whose auxiliary flow specs need to be
              fetched
  flow_type : direction the client is interested in. Auxiliary flow specs are
              supported in both Rx and Tx direction and client is required to
              specify a direction. Its value can be either QOS_MASK_TX_FLOW
              or QOS_MASK_RX_FLOW
  is_modify : is the client interested in fetching auxiliary flow specs
              associated with pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  A handle using which auxiliary flow specs can be fetched : on success
  NULL                                                     : otherwise

DEPENDENCIES
  Flow must not be a default flow.

  This function must be called from the same task context in which mode handler
  is running. Otherwise they could be deleted (for example, because flow specs
  are modified using QOS_MODIFY or because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations.

  Sample usage to fetch Tx auxiliary flow specs related to active QoS is
    handle =
      ps_flow_get_aux_flow_spec_handle(flow_ptr, QOS_MASK_TX_FLOW, FALSE)
    while (handle != NULL)
    {
      ps_flow_get_aux_flow_spec_by_handle(flow_ptr, QOS_MASK_TX_FLOW, FALSE,
                                          handle, aux_flow_spec, new_handle)
      handle = new_handle
    }

SIDE EFFECTS
  None
===========================================================================*/
void *ps_flow_get_aux_flow_spec_handle
(
  ps_flow_type                   * flow_ptr,
  qos_spec_field_mask_enum_type    flow_type,
  boolean                          is_modify
)
{
  qos_info_type              * qos_info_ptr;
  list_type                  * aux_flow_list_ptr;
  ps_flow_ip_flow_spec_type  * aux_flow_spec_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_aux_flow_spec_handle(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_aux_flow_spec_handle(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Point to appropriate aux flow list based on flow_type
  -------------------------------------------------------------------------*/
  if (flow_type == QOS_MASK_RX_FLOW)
  {
    aux_flow_list_ptr = &(qos_info_ptr->rx.ipflow.aux_flow_list);
  }
  else if (flow_type == QOS_MASK_TX_FLOW)
  {
    aux_flow_list_ptr = &(qos_info_ptr->tx.ipflow.aux_flow_list);
  }
  else
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_flow_get_aux_flow_spec_handle(): "
                    "Invalid flow_type %d for ps_flow 0x%p",
                    flow_type, flow_ptr);
    ASSERT(0);
    return NULL;
  }

  if (0 == list_is_initialized(aux_flow_list_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_2("ps_flow_get_aux_flow_spec_handle(): "
                    "Uninitialized aux flow spec, ps_flow 0x%p, flow_type %d",
                    flow_ptr, flow_type);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Return a handle to an auxiliary flow spec
  -------------------------------------------------------------------------*/
  aux_flow_spec_buf = list_peek_front(aux_flow_list_ptr);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return aux_flow_spec_buf;

} /* ps_flow_get_aux_flow_spec_handle() */



/*===========================================================================
FUNCTION PS_FLOW_GET_AUX_FLOW_SPEC_BY_HANDLE()

DESCRIPTION
  Given a handle, this function returns an auxiliary flow spec.
  A new handle is returned so that next auxiliary flow spec for a given
  direction pertaining to either QOS_REQUEST/active QoS or pending QOS_MODIFY
  is fetched when this function is invoked with that handle.

  NOTE : The parameters, flow_type and is_modify, determine the handle that
         a client gets. It is not possible to get handle for one combination
         and to get auxiliary flow spec that belong to another combination
         using that handle

PARAMETERS
  flow_ptr             : pointer to the flow whose Tx auxiliary flow specs
                         need to be fetched
  aux_flow_spec_handle : handle using which an auxiliary flow spec is
                         fetched
  flow_type            : direction the client is interested in.
                         Auxiliary flow specs are supported in both Rx and Tx
                         direction and client is required to specify a
                         direction. Its value can be either QOS_MASK_TX_FLOW
                         or QOS_MASK_RX_FLOW
  is_modify            : is the client interested in fetching auxiliary flow
                         specs associated with pending QOS_MODIFY? If so value
                         must be TRUE
  aux_flow_spec        : OUT PARAM, pointer to an auxiliary flow spec
  next_handle          : OUT PARAM, new handle which can be used to fetch
                         next auxiliary flow spec

RETURN VALUE
  TRUE  : if auxiliary flow spec can be fetched
  FALSE : otherwise

DEPENDENCIES
  Flow must not be a default flow.

  This function must be called from the same task context in which mode handler
  is running. Otherwise they could be deleted (for example, because flow specs
  are modified using QOS_MODIFY or because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations.

  Do not cache flow spec and access its contents later. Since flow spec can be
  deleted, this may lead to memory corruption or data aborts.

  Sample usage to fetch Tx auxiliary flow specs related to active QoS is
    handle =
      ps_flow_get_aux_flow_spec_handle(flow_ptr, QOS_MASK_TX_FLOW, FALSE)
    while (handle != NULL)
    {
      ps_flow_get_aux_flow_spec_by_handle(flow_ptr, QOS_MASK_TX_FLOW, FALSE,
                                          handle, aux_flow_spec, new_handle)
      handle = new_handle
    }

SIDE EFFECTS
  next_handle is set so that if this function is called with that handle,
  next auxiliary flow spec is fetched
===========================================================================*/
boolean ps_flow_get_aux_flow_spec_by_handle
(
  ps_flow_type                    * flow_ptr,
  void                            * aux_flow_spec_handle,
  qos_spec_field_mask_enum_type     flow_type,
  boolean                           is_modify,
  ip_flow_type                   ** aux_flow_spec,
  void                           ** next_handle
)
{
  qos_info_type              * qos_info_ptr;
  list_type                  * aux_flow_list_ptr;
  ps_flow_ip_flow_spec_type  * aux_flow_spec_buf;
  ps_flow_ip_flow_spec_type  * tmp_flow_spec_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (aux_flow_spec == NULL || next_handle == NULL)
  {
    LOG_MSG_ERROR_2("ps_flow_get_aux_flow_spec_by_handle(): "
                    "NULL parameters are passed", aux_flow_spec, next_handle);
    ASSERT(0);
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_aux_flow_spec_by_handle(): "
                    "Invalid ps_flow 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_aux_flow_spec_by_handle(): "
                    "QoS is not supported on default ps_flow 0x%p", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (aux_flow_spec_handle == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    *next_handle   = NULL;
    *aux_flow_spec = NULL;

    return FALSE;
  }

  aux_flow_spec_buf = (ps_flow_ip_flow_spec_type *) aux_flow_spec_handle;
  if (!ps_mem_is_valid(aux_flow_spec_buf, PS_MEM_PS_FLOW_IP_FLOW_SPEC_TYPE))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_aux_flow_spec_by_handle(): "
                    "Buffer (0x%X) doesn't belong to aux flow spec's mem pool",
                    aux_flow_spec_handle);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Point to appropriate aux flow list based on flow_type
  -------------------------------------------------------------------------*/
  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, is_modify);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    /* Appropriate message is printed in ps_flowi_get_qos_info_ptr() */
    ASSERT(0);
    return FALSE;
  }

  if (flow_type == QOS_MASK_RX_FLOW)
  {
    aux_flow_list_ptr = &(qos_info_ptr->rx.ipflow.aux_flow_list);
  }
  else if (flow_type == QOS_MASK_TX_FLOW)
  {
    aux_flow_list_ptr = &(qos_info_ptr->tx.ipflow.aux_flow_list);
  }
  else
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_flow_get_aux_flow_spec_by_handle(): "
                    "Invalid flow_type %d for ps_flow 0x%p",
                    flow_type, flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (0 == list_is_initialized(aux_flow_list_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_2("ps_flow_get_aux_flow_spec_by_handle(): "
                    "Uninitialized aux flow spec, ps_flow 0x%p, flow_type %d",
                    flow_ptr, flow_type);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Make sure that API is used correctly. Its invalid to get handle for a
    specific (flow_type, is_modify) combination and use it for another
    (flow_type, is_modify) combination
  -------------------------------------------------------------------------*/
  tmp_flow_spec_buf = list_peek_front(aux_flow_list_ptr);
  while (tmp_flow_spec_buf != NULL && tmp_flow_spec_buf != aux_flow_spec_buf)
  {
    tmp_flow_spec_buf = list_peek_next(aux_flow_list_ptr,
                                       &(tmp_flow_spec_buf->link));
  }

  if (tmp_flow_spec_buf == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_flow_get_aux_flow_spec_by_handle(): "
                    "Unknown buffer (0x%X) is passed", aux_flow_spec_handle);
    ASSERT(0);
    return FALSE;
  }

  *aux_flow_spec = &(aux_flow_spec_buf->flow_spec);

  /*-------------------------------------------------------------------------
    Return a handle to the auxiliary flow spec
  -------------------------------------------------------------------------*/
  *next_handle = list_peek_next(aux_flow_list_ptr,
                                &(aux_flow_spec_buf->link));
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;

} /* ps_flow_get_aux_flow_spec_by_handle() */


/*===========================================================================
FUNCTION PS_FLOW_GET_AUX_FLOW_SPEC()

DESCRIPTION
  This function returns the entire Rx or Tx aux flow spec for a given ps flow

PARAMETERS
  flow_ptr         : Flow pointer whose Aux Flows have to be returned
  num_aux_flows    : OUT PARAM. Flow that is created
  flow_type        : Enum type to return RX/TX Aux flow specs.
  ps_errno         : error returned back in case of an error

RETURN VALUE
   0 : if flow can be created
  -1 : otherwise

DEPENDENCIES
  Pass FALSE to create a default flow or pass TRUE otherwise

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flow_get_aux_flow_spec
(
  ps_flow_type                    * flow_ptr,
  int32                           * num_aux_flow_ptr,
  ip_flow_type                    * aux_flow_list_ptr,
  qos_spec_field_mask_enum_type     flow_type,
  boolean                           is_modify,
  int16                           * ps_errno
)
{
  void            * aux_flow_spec_handle_ptr;
  void            * new_aux_flow_spec_handle_ptr;
  int32             flow_index;
  ip_flow_type    * temp_aux_flow_ptr;
  list_size_type    aux_flow_cnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("ps_flow_get_aux_flow_spec(): "
                           "Flow ptr 0x%p type %d is_modify %d",
                           flow_ptr, flow_type, is_modify);

  do
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    if (!PS_FLOW_IS_VALID(flow_ptr))
    {
      LOG_MSG_INVALID_INPUT_1("ps_flow_get_aux_flow_spec(): "
                              "Invalid ps_flow 0x%p, is passed", flow_ptr);
      *ps_errno = DS_EINVAL;
      break;
    }

    if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
    {
      LOG_MSG_INVALID_INPUT_1("ps_flow_get_aux_flow_spec(): "
                              "QoS is not supported on default ps_flow 0x%p",
                              flow_ptr);
      *ps_errno = DS_EINVAL;
      break;
    }

    if (num_aux_flow_ptr == NULL)
    {
      LOG_MSG_INVALID_INPUT_0("ps_flow_get_aux_flow_spec(): "
                              "NULL num_aux_flow_ptr");
      *ps_errno = DS_EINVAL;
      break;
    }

    aux_flow_cnt =
      ps_flow_get_aux_flow_spec_cnt(flow_ptr, flow_type, is_modify);
    if (aux_flow_cnt > (list_size_type)(*num_aux_flow_ptr))
    {
      LOG_MSG_INVALID_INPUT_2("ps_flow_get_aux_flow_spec(): "
                              "Number of aux flows %d less than aux "
                              "flow array size %d",
                              aux_flow_cnt, *num_aux_flow_ptr);
      *ps_errno = DS_EINVAL;
      break;
    }

    aux_flow_spec_handle_ptr =
      ps_flow_get_aux_flow_spec_handle(flow_ptr, flow_type, is_modify);

    for (flow_index = 0; aux_flow_spec_handle_ptr != NULL; flow_index++)
    {
      if (!ps_flow_get_aux_flow_spec_by_handle(flow_ptr,
                                              aux_flow_spec_handle_ptr,
                                              flow_type,
                                              is_modify,
                                              &temp_aux_flow_ptr,
                                              &new_aux_flow_spec_handle_ptr))
      {
        LOG_MSG_INFO3_0("ps_flow_get_aux_flow_spec_by_handle failed");
        *ps_errno = DS_EINVAL;
        break;
      }

      aux_flow_list_ptr[flow_index] = *temp_aux_flow_ptr;
      aux_flow_spec_handle_ptr = new_aux_flow_spec_handle_ptr;
    }

    *num_aux_flow_ptr = flow_index;

    LOG_MSG_FUNCTION_EXIT_2("ps_flow_get_aux_flow_spec(): "
                            "Copied %d aux flows, flow 0x%p",
                            *num_aux_flow_ptr, flow_ptr);

    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return 0;

  } while (0);

  LOG_MSG_FUNCTION_EXIT_2("ps_flow_get_aux_flow_spec(): "
                          "Fail, flow 0x%p err %d", flow_ptr, *ps_errno);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return -1;

} /* ps_flow_get_aux_flow_spec() */



/*===========================================================================
FUNCTION PS_FLOW_VALIDATE_PRIMARY_QOS_MODIFY_SPEC()

DESCRIPTION
  This function validates primary QOS spec. Flow spec is validated in each
  direction. A generic validation is performed on the parameters and if mode
  handlers require extra validation they can pass an optional validation
  function ptr to perform extended validation. Note that generic validation
  will still be performed before calling extended validaiton functions.

PARAMETERS
  primary_qos_spec_ptr     : Primary QoS spec to be validated
  flow_validate_f_ptr      : Optional extended validation function
  requested_qos_field_mask : Field mask indicating the QoS that was requested
                             previously
  ps_errno                 : specific error code in case operation fails

RETURN VALUE
   0 : if primary QOS spec is valid
  -1 : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_flow_validate_primary_qos_modify_spec
(
  primary_qos_spec_type              * primary_qos_spec_ptr,
  qos_flow_validation_ex_f_ptr_type    flow_validate_f_ptr,
  qos_spec_field_mask_type             requested_qos_field_mask,
  int16                              * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (primary_qos_spec_ptr == NULL || ps_errno == NULL)
  {
    LOG_MSG_ERROR_2("ps_flow_validate_primary_qos_modify_spec(): "
                    "NULL parameters are passed, qos_spec 0x%p, ps_errno 0x%p",
                    primary_qos_spec_ptr, ps_errno);
    ASSERT(0);
    return -1;
  }

  if (requested_qos_field_mask == QOS_MASK_INVALID)
  {
    LOG_MSG_ERROR_1("ps_flow_validate_primary_qos_modify_spec(): "
                    "Invalid requested_qos_field_mask 0x%x",
                    requested_qos_field_mask);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  return ps_flowi_validate_primary_qos_modify_spec(primary_qos_spec_ptr,
                                                   flow_validate_f_ptr,
                                                   requested_qos_field_mask,
                                                   ps_errno);

} /* ps_flow_validate_primary_qos_modify_spec() */


/*===========================================================================
FUNCTION PS_FLOW_GET_CAPABILITY()

DESCRIPTION
  Returns TRUE if a capability of a flow is turned on.

  NOTE : Only one capability can be fetched each time.

PARAMETERS
  flow_ptr   : pointer to flow
  capability : capability which needs to be tested

RETURN VALUE
  TRUE  : if capability is turned on
  FALSE : otherwise
===========================================================================*/
boolean ps_flow_get_capability
(
  ps_flow_type                  * flow_ptr,
  ps_flow_capability_enum_type    capability
)
{
  ps_flow_type  * assoc_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_ERROR_1("ps_flow_get_capability(): "
                    "Invalid flow 0x%p", flow_ptr);
    return FALSE;
  }

  assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  while (assoc_flow_ptr != NULL)
  {
    if (!PS_FLOW_IS_VALID(assoc_flow_ptr))
    {
      LOG_MSG_ERROR_1("ps_flow_get_capability(): "
                      "Invalid flow 0x%p", assoc_flow_ptr);
      return FALSE;
    }

    flow_ptr = assoc_flow_ptr;
    assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  }

  if (capability < PS_FLOW_CAPABILITY_MAX)
  {
    return PS_FLOWI_GET_CAPABILITY(flow_ptr, capability);
  }
  else
  {
    return FALSE;
  }

} /* ps_flow_get_capability() */


/*===========================================================================
FUNCTION PS_FLOW_GET_RX_GRANTED_FLOW()

DESCRIPTION
  This macro returns a ptr to the currently granted Rx flow type if
  available, NULL otherwise.

  NOTE: Currently granted Rx flow must be fetched and accessed in one atomic
        operation as network may choose not to apply QOS at any time, in
        which case granted Rx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  ip_flow_ptr : if Rx flow is granted
  NULL        : if flow_ptr is invalid of if Rx flow is not granted
===========================================================================*/
ip_flow_type * ps_flow_get_rx_granted_flow
(
  ps_flow_type * flow_ptr
)
{
  ps_flow_type  * assoc_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_ERROR_1("ps_flow_get_rx_granted_flow(): "
                    "Invalid flow 0x%p", flow_ptr);
    return NULL;
  }

  assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  while (assoc_flow_ptr != NULL)
  {
    if (!PS_FLOW_IS_VALID(assoc_flow_ptr))
    {
      LOG_MSG_ERROR_1("ps_flow_get_rx_granted_flow(): "
                      "Invalid flow 0x%p", assoc_flow_ptr);
      return NULL;
    }

    flow_ptr       = assoc_flow_ptr;
    assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  }

  if (PS_FLOW_GET_STATE(flow_ptr) == FLOW_ACTIVATED)
  {
    return &(flow_ptr->flow_private.qos_info_ptr->rx.ipflow.granted);
  }
  else
  {
    return NULL;
  }

} /* ps_flow_get_rx_granted_flow() */


/*===========================================================================
FUNCTION PS_FLOW_GET_TX_GRANTED_FLOW()

DESCRIPTION
  This macro returns a ptr to the currently applied TX flow type if
  granted, NULL otherwise.

  NOTE: Currently granted Tx flow must be fetched and accessed in one atomic
        operation as network may choose not to apply QOS at any time, in
        which case granted Tx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  ip_flow_ptr : if Rx flow is applied
  NULL        : if flow_ptr is invalid of if Rx flow is not applied
===========================================================================*/
ip_flow_type * ps_flow_get_tx_granted_flow
(
  ps_flow_type * flow_ptr
)
{
  ps_flow_type  * assoc_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_ERROR_1("ps_flow_get_tx_granted_flow(): "
                    "Invalid flow 0x%p", flow_ptr);
    return NULL;
  }

  assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  while (assoc_flow_ptr != NULL)
  {
    if (!PS_FLOW_IS_VALID(assoc_flow_ptr))
    {
      LOG_MSG_ERROR_1("ps_flow_get_tx_granted_flow(): "
                      "Invalid flow 0x%p", assoc_flow_ptr);
      return NULL;
    }

    flow_ptr = assoc_flow_ptr;
    assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
  }

  if (PS_FLOW_GET_STATE(flow_ptr) == FLOW_ACTIVATED)
  {
    return &(flow_ptr->flow_private.qos_info_ptr->tx.ipflow.granted);
  }
  else
  {
    return NULL;
  }

} /* ps_flow_get_tx_granted_flow() */

#endif /* FEATURE_DATA_PS_QOS */

