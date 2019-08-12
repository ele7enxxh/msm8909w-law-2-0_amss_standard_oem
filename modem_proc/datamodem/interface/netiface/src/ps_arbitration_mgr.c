
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     P S _ A R B I T R A T I O N _ M G R . C

DESCRIPTION
  File defining the API exported by arbitration manager module

EXTERNAL FUNCTIONS
  PS_ARBITRATION_MGR_NET_ARBITRATE()
    Checks if apps on some interface can be preempted to support high priority
    app

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007-2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_arbitration_mgr.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
10/07/11    sun    To fix compiler warning 
05/09/11    asn    Adding support for new active Iface event
07/31/10    asn    Support for Arbitration
07/09/10    asn    Support for Arbitration
04/24/10    ss     Adding events for arbitration.
11/18/08    am     Fixed off-target VS compiler warnings and hig/med lints.
09/23/08    am     Removed ASSERT for invalid priority case.
05/14/08    am     Removed priority arg from net_arbitrate().
04/29/08    am     Updated Arbitration/Policy Manager now
                   supports OMH phase-2.
09/27/07    msr    Created the file

===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "amssassert.h"
#include "msg.h"

#include "ps_iface.h"
#include "ps_ifacei_utils.h"
#include "ps_policyi_mgr.h"
#include "ps_acl.h"
#include "ps_arbitration_mgr.h"
#include "event.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_ifacei.h"


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_ARBITRATION_MGR_NET_ARBITRATE()

DESCRIPTION
  This function arbitrates among a list of interfaces which are already in
  use by some other application. Checks if applications on any interface
  can be preempted to support the passed in app

PARAMETERS
  acl_pi_ptr            : Policy info of the app which wants to come up
  iface_in_use_ptr_arr  : Array of interfaces which are already in use
  num_iface_in_use      : Number of elements in iface_in_use_ptr_arr

RETURN VALUE
  ps_iface_ptr : on success
  NULL         : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  If a suitable interface is found, it is torn down in order to preempt
  all the apps using that interface
===========================================================================*/
ps_iface_type *ps_arbitration_mgr_net_arbitrate
(
  acl_policy_info_type  * acl_pi_ptr,
  ps_iface_type        ** iface_in_use_ptr_arr,
  uint8                   num_iface_in_use
)
{
  ps_iface_type                    *ps_iface_ptr   = NULL;
#define PS_IFACE_PRIORITY_UINT32_MAX 0xFFFFFFFF
  uint32                            iface_priority = 
                                      PS_IFACE_PRIORITY_UINT32_MAX;
  int32                             app_priority;
  int16                             ps_errno;
  uint8                             index;
  ps_arbitration_event_payload_type ps_arbitration_event_payload;
  uint64                            tmp_app_id = 0;

  int                               ret_val = -1;
  ps_iface_down_event_info_type     iface_out_of_use_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (iface_in_use_ptr_arr == NULL || acl_pi_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_arbitration_mgr_net_arbitrate(): NULL param");
    return NULL;
  }

  tmp_app_id   = acl_pi_ptr->app_identifier;
  LOG_MSG_INFO1_3("ps_arbitration_mgr_net_arbitrate(): "
                  "bringup: req app_id [0x%x%x], profile [%d]",
                  (uint32)(tmp_app_id&0xFFFFFFFF),
                  (uint32)(tmp_app_id>>32),
                  acl_pi_ptr->data_session_profile_id );

  app_priority = ps_policy_mgr_get_priority
                 (
                   (int64)acl_pi_ptr->app_identifier,
                   acl_pi_ptr->data_session_profile_id
                 );
  if (app_priority == PS_POLICY_MGR_PRIORITY_INVALID)
  {
    LOG_MSG_ERROR_2("ps_arbitration_mgr_net_arbitrate(): "
                    "Invalid app 0x%x, profile %d",
                    acl_pi_ptr->app_identifier,
                    acl_pi_ptr->data_session_profile_id);
    return NULL;
  }
  
  LOG_MSG_INFO1_2("ps_arbitration_mgr_net_arbitrate(): "
                  "bringup: app priority [%d], arb pool size [%d]",
                  app_priority, num_iface_in_use );
  /*------------------------------------------------------------------------
    Set event payload values.
  ------------------------------------------------------------------------*/
  ps_arbitration_event_payload.app_type     = acl_pi_ptr->app_identifier;
  ps_arbitration_event_payload.app_priority = app_priority;
  ps_arbitration_event_payload.profile_id   = 
    acl_pi_ptr->data_session_profile_id;

  /*------------------------------------------------------------------------
    Loop through the list of passed in interfaces and find the interface
    with the least priority
  ------------------------------------------------------------------------*/
  for (index = 0; index < num_iface_in_use; index++)
  {
    if (!PS_IFACE_IS_VALID( iface_in_use_ptr_arr[index]))
    {
      LOG_MSG_ERROR_2("ps_arbitration_mgr_net_arbitrate(): "
                      "Invalid iface 0x%p, idx [%d], skipping",
                      iface_in_use_ptr_arr[index], index );
      continue;
    }

    /* very verbose, remove following msgs later */
    LOG_MSG_INFO1_3("ps_arbitration_mgr_net_arbitrate(): "
                    "bringup: In-use Iface [0x%p:%d], prio [0x%lx]",
                    iface_in_use_ptr_arr[index]->name,
                    iface_in_use_ptr_arr[index]->instance,
                    PS_IFACEI_GET_PRIORITY_MASK( iface_in_use_ptr_arr[index]));

    if (PS_IFACEI_GET_PRIORITY_MASK( iface_in_use_ptr_arr[index]) <
          iface_priority)
    {
      iface_priority =
        PS_IFACEI_GET_PRIORITY_MASK( iface_in_use_ptr_arr[index]);
      ps_iface_ptr = iface_in_use_ptr_arr[index];
      LOG_MSG_INFO1_3("ps_arbitration_mgr_net_arbitrate(): "
                      "bringup: candidate in-use Iface [0x%p:%d], prio mask [0x%x]",
                      ps_iface_ptr->name,
                      ps_iface_ptr->instance,
                      iface_priority );
    }
  }/* loop over all inout Ifaces */

  /*------------------------------------------------------------------------
    Tear down interface if its priority is less than passed in app's 
    priority
  ------------------------------------------------------------------------*/
  if ( NULL != ps_iface_ptr && 
     (uint32)( 1 << app_priority) > iface_priority)
  {
    LOG_MSG_INFO1_3("ps_arbitration_mgr_net_arbitrate(): "
                    "bringup: tearing down iface [0x%p:%d] for app [0x%x]",
                    ps_iface_ptr->name,
                    ps_iface_ptr->instance,
                    acl_pi_ptr->app_identifier);

    PS_IFACEI_SET_ARBITRATION_IN_PROGRESS( ps_iface_ptr );

    memset(&iface_out_of_use_info, 0, sizeof(ps_iface_down_event_info_type));
    iface_out_of_use_info.proc_id = ps_iface_ptr->iface_private.proc_id;
    iface_out_of_use_info.state   = PS_IFACEI_GET_STATE(ps_iface_ptr);
    iface_out_of_use_info.netdown_reason = PS_NET_DOWN_REASON_APP_PREEMPTED;
    iface_out_of_use_info.bearer_tech.rat_value =  DS_SYS_RAT_UNKNOWN;
    
    ret_val = ps_iface_arbitration_tear_down_cmd( ps_iface_ptr, 
                                                 &ps_errno, 
                                                  NULL );
    /* generate additional generic indication */
    LOG_MSG_INFO1_3("ps_arbitration_mgr_net_arbitrate(): "
                    "arb teardown iface [0x%p:%d], ret [%d]",
                    ps_iface_ptr->name, ps_iface_ptr->instance, ret_val);
    if ( ret_val == 0 )
    {
      /* generate additional generic indication */
      LOG_MSG_INFO1_0("ps_arbitration_mgr_net_arbitrate(): "
                      "teardown success" ); 
      ps_iface_generic_ind( ps_iface_ptr, 
                            IFACE_ACTIVE_OUT_OF_USE_EV, 
                            &iface_out_of_use_info);
    }
    else
    {
      /* handle failure */
      LOG_MSG_INFO1_0("ps_arbitration_mgr_net_arbitrate(): "
                      "teardown failed, wait for indication from MH" );
    }
   
    ps_iface_generic_ind( ps_iface_ptr, IFACE_APP_PREEMPTED_EV, NULL);

    /*-----------------------------------------------------------------------
      Low priority followed by high priority app. 
      Generate iface granted event.
    -----------------------------------------------------------------------*/
    ps_arbitration_event_payload.ps_arbitration_event = 
      EVENT_PS_ARBITRATION_APP_GRANTED_IFACE;
    event_report_payload( EVENT_PS_ARBITRATION,
                          sizeof(ps_arbitration_event_payload_type),
                          (void *)&ps_arbitration_event_payload );
    return ps_iface_ptr;
  }
  else
  {
    LOG_MSG_INFO1_0("ps_arbitration_mgr_net_arbitrate(): "
                    "bringup: arb failed to find Iface" );
  }

  /*------------------------------------------------------------------------
    Generate App denied iface event only if we actually have some in-use 
    ifaces.
  ------------------------------------------------------------------------*/
  if(num_iface_in_use > 0)
  {
    /*------------------------------------------------------------------------
      High priority followed by low priority app. 
      Generate iface denied event.
    ------------------------------------------------------------------------*/
    ps_arbitration_event_payload.ps_arbitration_event = 
      EVENT_PS_ARBITRATION_APP_DENIED_IFACE;
    event_report_payload( EVENT_PS_ARBITRATION,
                          sizeof(ps_arbitration_event_payload_type),
                          (void *)&ps_arbitration_event_payload );
  }
  return NULL;
} /* ps_arbitration_mgr_net_arbitrate() */

/*===========================================================================
FUNCTION PS_ARBITRATION_MGR_NET_ARBITRATE_LOOKUP()

DESCRIPTION
  This function arbitrates among a list of interfaces which are already in
  use by some other application. Just selects and Iface.

PARAMETERS
  acl_pi_ptr            : Policy info of the app which wants to come up
  iface_in_use_ptr_arr  : Array of interfaces which are already in use
  num_iface_in_use      : Number of elements in iface_in_use_ptr_arr

RETURN VALUE
  ps_iface_ptr : on success
  NULL         : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  If a suitable interface is found, it is torn down in order to preempt
  all the apps using that interface
===========================================================================*/
ps_iface_type *ps_arbitration_mgr_net_arbitrate_lookup
(
  acl_policy_info_type  * acl_pi_ptr,
  ps_iface_type        ** iface_in_use_ptr_arr,
  uint8                   num_iface_in_use
)
{
  ps_iface_type                    *ps_iface_ptr   = NULL;
#define PS_IFACE_PRIORITY_UINT32_MAX 0xFFFFFFFF
  uint32                            iface_priority = 
                                      PS_IFACE_PRIORITY_UINT32_MAX;
  int32                             app_priority;
  uint8                             index;
  ps_arbitration_event_payload_type ps_arbitration_event_payload;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (iface_in_use_ptr_arr == NULL || acl_pi_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_arbitration_mgr_net_arbitrate_lookup(): NULL param");
    return NULL;
  }

  app_priority = ps_policy_mgr_get_priority
                 (
                   (int64)acl_pi_ptr->app_identifier,
                   acl_pi_ptr->data_session_profile_id
                 );
  if (app_priority == PS_POLICY_MGR_PRIORITY_INVALID)
  {
    LOG_MSG_ERROR_2("ps_arbitration_mgr_net_arbitrate_lookup(): "
                    "Invalid app 0x%x, profile %d",
                    acl_pi_ptr->app_identifier,
                    acl_pi_ptr->data_session_profile_id);
    return NULL;
  }

  /*------------------------------------------------------------------------
    Set event payload values.
  ------------------------------------------------------------------------*/
  ps_arbitration_event_payload.app_type     = acl_pi_ptr->app_identifier;
  ps_arbitration_event_payload.app_priority = app_priority;
  ps_arbitration_event_payload.profile_id   = 
    acl_pi_ptr->data_session_profile_id;

  LOG_MSG_INFO2_3("ps_arbitration_mgr_net_arbitrate_lookup(): "
                  "lookup: arb for app_id [0x%llx], profile [0x%x], arb list sz [%d]",
                  acl_pi_ptr->app_identifier,
                  acl_pi_ptr->data_session_profile_id,
                  num_iface_in_use);

  /*------------------------------------------------------------------------
    Loop through the list of passed in interfaces and find the interface
    with the least priority
  ------------------------------------------------------------------------*/
  for (index = 0; index < num_iface_in_use; index++)
  {
    if (!PS_IFACE_IS_VALID( iface_in_use_ptr_arr[index]))
    {
      LOG_MSG_ERROR_2("ps_arbitration_mgr_net_arbitrate_lookup(): "
                      "Invalid iface 0x%p, idx [%d], skipping",
                      iface_in_use_ptr_arr[index], index );
      continue;
    }

    if (PS_IFACEI_GET_PRIORITY_MASK( iface_in_use_ptr_arr[index]) <
          iface_priority)
    {
      iface_priority =
        PS_IFACEI_GET_PRIORITY_MASK( iface_in_use_ptr_arr[index]);
      ps_iface_ptr = iface_in_use_ptr_arr[index];
    }
  }/* loop over all inout Ifaces */


  /*------------------------------------------------------------------------
    Select interface if its priority is less than passed in app's 
    priority
  ------------------------------------------------------------------------*/
  if ( NULL != ps_iface_ptr && 
     (uint32)( 1 << app_priority) > iface_priority)
  {
    LOG_MSG_INFO1_3("ps_arbitration_mgr_net_arbitrate_lookup(): "
                    "lookup: chosen in-use Iface [0x%p:%d], app [0x%x]",
                    ps_iface_ptr->name,
                    ps_iface_ptr->instance,
                    acl_pi_ptr->app_identifier);
    /* Arbitration is not performed
    (void) ps_iface_go_null_cmd( ps_iface_ptr, &ps_errno, NULL);

    ps_iface_generic_ind( ps_iface_ptr, IFACE_APP_PREEMPTED_EV, NULL);
    */

    /*-----------------------------------------------------------------------
      Low priority followed by high priority app. 
      Generate iface granted event.
    -----------------------------------------------------------------------*/
    ps_arbitration_event_payload.ps_arbitration_event = 
      EVENT_PS_ARBITRATION_APP_GRANTED_IFACE_LOOKUP;
    event_report_payload( EVENT_PS_ARBITRATION,
                          sizeof(ps_arbitration_event_payload_type),
                          (void *)&ps_arbitration_event_payload );
    return ps_iface_ptr;
  }
  else
  {
    LOG_MSG_INFO1_0("ps_arbitration_mgr_net_arbitrate_lookup(): "
                    "lookup: arb failed to find Iface" );
  }

  /*------------------------------------------------------------------------
    Generate App denied iface event only if we actually have some in-use 
    ifaces.
  ------------------------------------------------------------------------*/
  if(num_iface_in_use > 0)
  {
    /*------------------------------------------------------------------------
      High priority followed by low priority app. 
      Generate iface denied event.
    ------------------------------------------------------------------------*/
    ps_arbitration_event_payload.ps_arbitration_event = 
      EVENT_PS_ARBITRATION_APP_DENIED_IFACE;
    event_report_payload( EVENT_PS_ARBITRATION,
                          sizeof(ps_arbitration_event_payload_type),
                          (void *)&ps_arbitration_event_payload );
  }
  return NULL;
} /* ps_arbitration_mgr_net_arbitrate_lookup() */


