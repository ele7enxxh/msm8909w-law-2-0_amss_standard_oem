/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   N E T I F A C E _ S T U B S . C

DESCRIPTION

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/netiface_stubs.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/08/09    pp     Added DPL stubs.
01/31/09    pp     Initial release.
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"

#ifdef FEATURE_DATA_PS_IPV6
  #define _NO_IPV6_STUBS
#endif /* FEATURE_DATA_PS_IPV6 */




#define NETIFACE_STUBS_EXT_PERROR_NO_FEAT(api, feat)                      \
  LOG_MSG_ERROR_0(#api": "#feat" feature(s) turned off."                  \
           "Returning error.")

#define QOS_PERROR(api)                                                   \
  NETIFACE_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_QOS)

#define MCAST_PERROR(api)                                                 \
  NETIFACE_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_MCAST)

#define IPV6_PERROR(api)                                                  \
  NETIFACE_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_IPV6)

#define DPL_PERROR(api)                                                  \
  NETIFACE_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_DATA_LOGGING)
  
#include "ps_flow.h"
#include "ps_flow_event.h"
#include "ps_iface.h"
#include "ps_iface_addr_v6.h"
#include "ps_iface_flow.h"
#include "ps_iface_rx_qos_fltr.h"
#include "ps_iface_logging.h"

#ifndef FEATURE_DATA_PS_QOS
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_activate_cmd);
  return -1;
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
int32 ps_flow_configure_cmd
(
  ps_flow_type  * flow_ptr,
  int16         * ps_errno,
  void          * client_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_configure_cmd);
  return -1;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_resume_cmd);
  return -1;
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
  QOS_PERROR(ps_flow_suspend_cmd);
  return -1;
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
  QOS_PERROR(ps_flow_go_null_cmd);
  return -1;
} /* ps_flow_go_null_cmd() */

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
  QOS_PERROR(ps_flow_suspend_ind);
  return;
} /* ps_flow_suspend_ind() */

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
  QOS_PERROR(ps_flow_qos_modify_accepted_ind);
  return;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_qos_modify_rejected_ind);
  return;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_generic_ind);
  return;
} /* ps_flow_generic_ind() */

/*===========================================================================
FUNCTION PS_FLOW_SET_ASSOC_FLOW()

DESCRIPTION
  This function sets associated ps_flow for a given ps_flow, allocates
  the meta-info, and registers the specified event callback function to
  handle events received from the associated flow. If no event callback
  is supplied, a default handler is used.

PARAMETERS
  flow_ptr       : pointer to the flow serving the QOS.
  assoc_flow_ptr : associated ps_flow

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void ps_flow_set_assoc_flow
(
  ps_flow_type                        * flow_ptr,
  ps_flow_type                        * assoc_flow_ptr,
  ps_flow_assoc_flow_event_cback_type   ev_cback_func
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_set_assoc_flow);
  return;
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
  QOS_PERROR(ps_flow_set_granted_flow);
  return;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_get_rx_fltr_cnt);
  return -1;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_get_rx_fltr_handle);
  return NULL;
} /* ps_flow_get_rx_fltr_handle() */


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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_get_rx_fltr_by_handle);
  return FALSE;
} /* ps_flow_get_rx_fltr_by_handle() */


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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_get_aux_flow_spec_cnt);
  return -1;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_get_aux_flow_spec_handle);
  return NULL;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_flow_get_aux_flow_spec_by_handle);
  return FALSE;
} /* ps_flow_get_aux_flow_spec_by_handle() */


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
  QOS_PERROR(ps_flow_validate_primary_qos_modify_spec);
  return -1;
} /* ps_flow_validate_primary_qos_modify_spec() */

/*===========================================================================
FUNCTION PS_IFACE_FLOW_INIT()

DESCRIPTION
  Initializes ps_iface_flow module
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_flow_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_flow_init);
  return;
}

/*===========================================================================
FUNCTION PS_IFACE_CREATE_FLOW

DESCRIPTION
  This function creates a flow, processes the requested QOS spec and stores
  it in the flow control block for later use. Both flow spec and filter spec
  are processed and validated for each direction.  A generic validation is
  performed on the parameters. If mode handlers require specific validation
  they can pass an optional validation function ptr to perform extended
  validations. Note that generic validations will still be performed before
  calling extended validaiton functions.  Both Rx and Tx flow specs are
  stored as is after making sure all specified parameters are correct. Tx
  filter spec is validated and stored in decreasing order of fltr precedence.
  Mode handlers should provide alternate camparision function if they do not
  wish to apply generic comparison algorithm. Tx filter spec is passed to IP
  filtering library, where it is sorted and stored to be applied to the
  outgoing IP traffic.

PARAMETERS
  iface_ptr        : corresponding iface on which QOS is requested
  phys_link_ptr    : ptr to phys link to which created flow will be bound to
  qos_req_params   : structure containing the qos_spec and all the
                     information needed to process it
  ps_errno         : specific error code in case operation fails
  out_flow_ptr_ptr : flow that is being created. (OUT param)

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_create_flow
(
  ps_iface_type                    * iface_ptr,
  ps_phys_link_type                * phys_link_ptr,
  const ps_flow_create_param_type  * qos_req_param,
  ps_flow_type                    ** out_flow_ptr_ptr,
  int16                            * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_create_flow);
  return -1; 
}

/*===========================================================================
FUNCTION PS_IFACE_CREATE_LOGICAL_FLOWS

DESCRIPTION
  This function is intended to be used by logical iface handlers for 
  "default" handling of the QoS Request and QoS Request Ex 
  ioctls. Iface handlers requiring special handling need to implement 
  similar functionality on their own by making use of related functions in 
  this module.

  The steps performed by this function are outlined below.
  (1) Creates the appropriate number of logical flows (L_flow(s)) on the 
      requested iface (L_iface) by invoking ps_iface_create_flow()
  (2) Invokes ps_flow_activate_cmd() to activate all L_flow(s)
  (3) Invokes the requested ioctl (PS_IFACE_IOCTL_QOS_REQUEST or 
      PS_IFACE_IOCTL_QOS_REQUEST_EX) on A_iface - the iface 
      associated with L_iface, creating requested number of flows 
      (A_flow(s)) on A_iface. This ioctl call uses the QoS parameters 
      contained in ioctl_argval_ptr. Note that these QoS parameters may
      be either parameters that the application requested (i.e. same
      as qos_req_param) or the translated ones.
  (4) Associates each L_flow with the corresponding A_flow
  (5) Registers for events on A_flow(s) 

PARAMETERS
  iface_ptr         : corresponding iface on which QOS is requested.
                      This MUST be a logical iface, else an error is
                      returned
  ioctl_name        : ioctl to invoke on the underlying iface
                      Only PS_IFACE_IOCTL_QOS_REQUEST and 
                      PS_IFACE_IOCTL_QOS_REQUEST_EX are allowed.
  qos_req_param     : pointer to structure containing the qos_spec for 
                      the logical flows to be created. This MUST not
                      be NULL.
  ioctl_argval_ptr  : ioctl paramters for the ioctl call on 
                      underlying iface. This parameter MUST not 
                      be NULL.
  out_flow_ptr_ptr  : flow(s) that are being created. (OUT param)
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  For the PS_IFACE_IOCTL_QOS_REQUEST_EX case, it is assumed that the
  number of flows requested via ioctl_argval_ptr are the same as number
  of logical flows to be created on the top-level iface.

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_create_logical_flows
(
  ps_iface_type                    * iface_ptr,
  ps_iface_ioctl_type                ioctl_name,
  const ps_flow_create_param_type  * qos_req_param,
  void                             * ioctl_argval_ptr,
  ps_flow_type                    ** out_flow_ptr_ptr,
  int16                            * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_create_logical_flows);
  return -1;
}

/*===========================================================================
FUNCTION PS_IFACE_MODIFY_FLOW()

DESCRIPTION
  This function processes the QOS_MODIFY spec and stores it in the flow
  control block for later use. Both flow spec and filter spec
  are processed and validated for each direction. A generic validation is
  performed on the parameters. If mode handlers require specific validation
  they can pass an optional validation function ptr to perform extended
  validation. Note that generic validation will still be performed before
  calling extended validaiton function. If Rx and/or Tx flow specs are
  modified, they are stored as is after making sure all specified parameters
  are correct. Tx filter spec is validated and stored in decreasing order
  of fltr precedence. Tx filters are disabled until network comes back with
  a response. Mode handlers should provide alternate camparison function if
  they do not wish to apply generic comparison algorithm.

PARAMETERS
  iface_ptr         : corresponding iface on which QOS is requested
  flow_ptr          : flow to be modified
  qos_modify_params : structure containing qos_modified_spec and all the
                      information needed to process it
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_iface_modify_flow
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * flow_ptr,
  const ps_flow_modify_param_type  * qos_modify_param,
  int16                            * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_modify_flow);
  return -1;
}

/*===========================================================================
FUNCTION PS_IFACE_DELETE_FLOW

DESCRIPTION
  This function deletes a flow along with its state

PARAMETERS
  iface_ptr : iface to which flow_ptr belongs
  flow_ptr  : ptr to flow

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_delete_flow
(
  ps_iface_type    * iface_ptr,
  ps_flow_type     * flow_ptr,
  int16            * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_delete_flow);
  return -1;
}

/*===========================================================================
FUNCTION PS_IFACE_MODIFY_FLOW_ACCEPTED

DESCRIPTION
  Indicates that QOS_MODIFY/PRIMARY_QOS_MODIFY is accepted by network.
  If it is QOS_MODIFY, migrates all state associated with QOS_MODIFY to
  ps_flow so that ps_flow represents QOS granted by network and cleans old QOS.

PARAMETERS
  iface_ptr : iface to which flow_ptr belongs
  flow_ptr  : ptr to flow
  info_code : reason for accepting QOS_MODIFY
  ps_errno  : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  Previously granted QOS is not valid anymore and FLOW_MODIFY_ACCEPTED_EV is
  posted
===========================================================================*/
int32 ps_iface_modify_flow_accepted
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code,
  int16                            * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_modify_flow_accepted);
  return -1; 
}

/*===========================================================================
FUNCTION PS_IFACE_MODIFY_FLOW_REJECTED

DESCRIPTION
  Indicates that QOS_MODIFY/PRIMARY_QOS_MODIFY is rejected by network.
  If QOS_MODIFY, cleans all state associated with it

PARAMETERS
  iface_ptr : iface to which flow_ptr belongs
  flow_ptr  : ptr to flow
  info_code : reason for rejecting QOS_MODIFY
  ps_errno  : error code returned back if operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  FLOW_MODIFY_REJECTED_EV is posted
===========================================================================*/
int32 ps_iface_modify_flow_rejected
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code,
  int16                            * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_modify_flow_rejected);
  return -1;
}

/*===========================================================================
FUNCTION PS_IFACE_GET_SEC_FLOW_HANDLE()

DESCRIPTION
  This function returns a handle which can be used to fetch secondary flows
  associated with an iface

PARAMETERS
  iface_ptr : pointer to an iface

RETURN VALUE
  A handle using which secondary flows can be fetched : on success
  NULL                                                : on failure

DEPENDENCIES
  All flows must be fetched in one atomic operation. Otherwise flows
  could be deleted while client is fetching them which could lead to
  incorrect operations.

  Do not try to access flows contents outside of atomic operation. Since
  a flow can be deleted, this may lead to memory corruption or data aborts.

  Sample usage of this API is
    TAKLOCK()
      handle = PS_IFACE_GET_SEC_FLOW_HANDLE()
      while (handle != NULL)
      {
        PS_IFACE_GET_SEC_FLOW_BY_HANDLE()
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  None.
===========================================================================*/
void *ps_iface_get_sec_flow_handle
(
  ps_iface_type  * iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_get_sec_flow_handle);
  return NULL;
}

/*===========================================================================
FUNCTION PS_IFACE_GET_SEC_FLOW_BY_HANDLE()

DESCRIPTION
  Given a handle, returns a secondary flow. A new handle is returned so that
  next secondary flow is fetched when this function is invoked with new
  handle.

PARAMETERS
  iface_ptr       : pointer to the iface.
  sec_flow_handle : handle using which next secondary flow is fetched
  sec_flow_ptr    : pointer to fetched secondary flow (OUT param)
  next_handle     : new handle which can be used to fetch next secondary
                    flow (OUT param)

RETURN VALUE
  TRUE  : on success
  FALSE : on failure

DEPENDENCIES
  All flows must be fetched in one atomic operation. Otherwise flows
  could be deleted while client is fetching them which could lead to
  incorrect operations.

  Do not try to access flows contents outside of atomic operation. Since
  a flow can be deleted, this may lead to memory corruption or data aborts.

  Sample usage of this API is
    TAKLOCK()
      handle = PS_IFACE_GET_SEC_FLOW_HANDLE()
      while (handle != NULL)
      {
        PS_IFACE_GET_SEC_FLOW_BY_HANDLE()
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  next_handle is updated so that if this function is called with that
  handle, next secondary flow is fetched
===========================================================================*/
boolean ps_iface_get_sec_flow_by_handle
(
  ps_iface_type    * iface_ptr,
  void             * sec_flow_handle,
  ps_flow_type    ** sec_flow_ptr,
  void            ** next_handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_get_sec_flow_by_handle);
  return FALSE;
}

/*===========================================================================
FUNCTION PS_IFACE_FILTERING_CONTROL()

DESCRIPTION
  This function enables or disables IP packet filtering over a flow.
  When enabled, all filters installed for this flow are enabled and
  traffic may get routed over this flow. When disbaled, any such traffic
  will go over default flow.

PARAMETERS
  iface_ptr : iface where filters are installed
  flow_ptr  : ptr to flow to which owns these filters
  enable    : Whether to enable or disable filtering on this flow

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  flow_ptr must not be a default flow, no filtering is performed on default
  flow as it always carries default traffic.

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_filtering_control
(
  ps_iface_type    * iface_ptr,
  ps_flow_type     * flow_ptr,
  boolean            enable,
  int16            * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_filtering_control);
  return -1;
}

/*===========================================================================
FUNCTION PS_IFACE_GET_TX_FLTR_CNT()

DESCRIPTION
  Returns number of Tx filters, that match the IP version, specified as an
  argument. Tx filters can pertain to either QOS_REQUEST/active QoS or
  pending QOS_MODIFY

PARAMETERS
  ps_iface_ptr : iface where filters are stored
  flow_ptr     : pointer to the ps_flow whose filters need to be fetched
  ip_vsn       : set of filters, a client is interested in. Mix of V4 and
                 V6 filters can be installed on a flow and client may
                 choose to get only V4 filters
  is_modify    : is the client interested in fetching Tx filters associated
                 with pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  Number of Tx filters : on success
  -1                   : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int8 ps_iface_get_tx_fltr_cnt
(
  ps_iface_type         * ps_iface_ptr,
  ps_flow_type          * flow_ptr,
  ip_version_enum_type    ip_vsn,
  boolean                 is_modify
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_get_tx_fltr_cnt);
  return -1;
}

/*===========================================================================
FUNCTION PS_IFACE_GET_TX_FLTR_HANDLE()

DESCRIPTION
  This function returns a handle which can be used to fetch Tx filters
  associated with a ps_flow. Only those filters which match IP version,
  specified as an argument, are returned

  Tx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  ps_iface_ptr : iface where filters are stored
  flow_ptr     : pointer to the ps_flow whose filters need to be fetched
  ip_vsn       : set of filters, a client is interested in. Mix of V4 and
                 V6 filters can be installed on a flow and client may
                 choose to get only V4 filters
  is_modify    : is the client interested in fetching Tx filters associated
                 with pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  A handle using which Tx filters can be fetched : on success
  NULL                                           : otherwise

DEPENDENCIES
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations

  Sample usage to fetch Tx IPV4 filters pertaining to QOS_REQUEST is
    TAKLOCK()
      handle = ps_iface_get_tx_fltr_handle(iface_ptr, flow_ptr, IPV4, FALSE)
      while (handle != NULL)
      {
        ps_iface_get_tx_fltr_by_handle(iface_ptr, flow_ptr, IPV4, FALSE,
                                       handle, fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  None
===========================================================================*/
void *ps_iface_get_tx_fltr_handle
(
  ps_iface_type         * ps_iface_ptr,
  ps_flow_type          * flow_ptr,
  ip_version_enum_type    ip_vsn,
  boolean                 is_modify
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_get_tx_fltr_handle);
  return NULL;
}

/*===========================================================================
FUNCTION PS_IFACE_GET_TX_FLTR_BY_HANDLE()

DESCRIPTION
  Given a handle, this function returns a Tx filter, and its precedence.
  Currently 0 is returned as precedence for all Tx filters. Only those
  filters which match IP version, specified as an argument, are returned

  Tx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  ps_iface_ptr   : iface where filters are stored
  flow_ptr       : pointer to the flow
  ip_vsn         : set of filters, a client is interested in. Mix of V4 and
                   V6 filters can be installed on a flow and client may
                   choose to get only V4 filters
  tx_fltr_handle : handle using which a filter is fetched
  is_modify      : is the client interested in fetching Tx filters associated
                   with pending QOS_MODIFY? If so value must be TRUE
  fltr           : OUT PARAM, pointer to Tx filter
  prcd           : OUT PARAM, precedence of Tx filter
  next_handle    : OUT PARAM, new handle which can be used to fetch next
                   Tx filter

RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations.

  Do not try to access filter contents outside of atomic operation. Since
  a filter can be deleted, this may lead to memory corruption or data aborts.

  Sample usage to fetch Tx IPV4 filters pertaining to QOS_REQUEST is
    TAKLOCK()
      handle = ps_iface_get_tx_fltr_handle(iface_ptr, flow_ptr, IPV4, FALSE)
      while (handle != NULL)
      {
        ps_iface_get_tx_fltr_by_handle(iface_ptr, flow_ptr, IPV4, FALSE,
                                       handle, fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  next_handle is set so that if this function is called with that handle,
  next Tx filter is fetched
===========================================================================*/
boolean ps_iface_get_tx_fltr_by_handle
(
  ps_iface_type          * ps_iface_ptr,
  ps_flow_type           * flow_ptr,
  ip_version_enum_type     ip_vsn,
  void                   * tx_fltr_handle,
  boolean                  is_modify,
  ip_filter_type        ** fltr,
  uint8                  * prcd,
  void                  ** next_handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_iface_get_tx_fltr_by_handle);
  return FALSE; 
}

/*===========================================================================
FUNCTION PS_IFACEI_RX_QOS_FLTR_ADD()

DESCRIPTION
  This function adds a set of Rx QOS filters to the specified iface. The
  filter set is tied to a filter id, which uniquely idenifies a set of
  filters for the given iface for the given ps_flow.

PARAMETERS
  iface_ptr          : Iface to add filters to
  qos_info_ptr       : address where Rx fltrs are stored in a ps_flow. Could
                       be qos_info_ptr or qos_modify_info_ptr
  fltr_priority      : priority class of filters
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
  A unique ID which is assigned to set of added filters : on sucess
  PS_IFACE_RX_QOS_FLTR_INVALID_HANDLE                   : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_rx_qos_fltr_handle_type ps_ifacei_rx_qos_fltr_add
(
  ps_iface_type                 * iface_ptr,
  qos_info_type                 * qos_info_ptr,
  fltr_priority_enum_type         fltr_priority,
  ipfltr_comparison_f_ptr_type    fltr_compare_f_ptr,
  int16                         * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_ifacei_rx_qos_fltr_add);
  return PS_IFACE_IPFLTR_INVALID_HANDLE; 
}

/*===========================================================================
FUNCTION PS_IFACEI_RX_QOS_FLTR_MODIFY()

DESCRIPTION
  This function adds a set of Rx QOS filters to the specified iface. The
  filter set is tied to a filter id, which uniquely idenifies a set of
  filters for the given iface for the given ps_flow.

PARAMETERS
  iface_ptr          : Iface to add filters to
  qos_info_ptr       : address where Rx fltrs are stored in a ps_flow. Has
                       to be qos_modify_info_ptr
  rx_fltr_handle     : handle to existing Rx fltrs specified in current QOS
  fltr_priority      : priority class of filters
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
  A unique ID which is assigned to set of added filters : on sucess
  PS_IFACE_RX_QOS_FLTR_INVALID_HANDLE                   : on failure

DEPENDENCIES
  Must be called only if existing Rx fltrs are modified in QOS_MODIFY
  iface critical section must have been entered

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_rx_qos_fltr_handle_type ps_ifacei_rx_qos_fltr_modify
(
  ps_iface_type                     * iface_ptr,
  qos_info_type                     * qos_info_ptr,
  ps_iface_rx_qos_fltr_handle_type    rx_fltr_handle,
  fltr_priority_enum_type             fltr_priority,
  ipfltr_comparison_f_ptr_type        fltr_compare_f_ptr,
  int16                             * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_ifacei_rx_qos_fltr_modify);
  return PS_IFACE_IPFLTR_INVALID_HANDLE; 
}

/*===========================================================================
FUNCTION PS_IFACEI_RX_QOS_FLTR_DELETE()

DESCRIPTION
  This function deletes all the existing Rx QOS filters identified by the
  specified filter handle from the specified iface.

PARAMETERS
  iface_ptr      : Iface to delete filters from
  rx_fltr_handle : handle identifying filters to be deleted

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ifacei_rx_qos_fltr_delete
(
  ps_iface_type                     * iface_ptr,
  ps_iface_rx_qos_fltr_handle_type    rx_fltr_handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QOS_PERROR(ps_ifacei_rx_qos_fltr_delete);
  return; 
}

#endif /* QOS_STUBS */


#ifndef _NO_IPV6_STUBS
/*===========================================================================
FUNCTION PS_IFACE_GET_ALL_V6_PREFIXES()

DESCRIPTION
  This function will retrieve all of the prefixes on an interface along
  with the state and length of each prefix.

PARAMETERS
  this_iface_ptr: The pointer to the interface on which to cleanup the
                  neighbor discovery caches.
  prefix_info:    The prefix and its state and length.
  num_prefixes:   The space alloc'd for prefixes and the number passed back

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_all_v6_prefixes
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_prefix_info_type *prefix_info,
  uint8                     *num_prefixes
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  IPV6_PERROR(ps_iface_get_all_v6_prefixes);
  return;
}

/*===========================================================================
FUNCTION PS_IFACE_GET_V6_IID

DESCRIPTION
  This function returns the IPV6 IID of an iface.  If the iface is
  NULL or IPV4, then it returns NULL.

PARAMETERS
  this_iface_ptr: Target iface ptr

RETURN VALUE
  IPv6 interface identifier (last 64 bits of the address), 0 if interface
  pointer is NULL or iface is IPV4 family.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 ps_iface_get_v6_iid
(
  ps_iface_type       *this_iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  IPV6_PERROR(ps_iface_get_v6_iid);
  return (0);
}

/*===========================================================================
FUNCTION PS_IFACE_GET_V6_PREFIX

DESCRIPTION
  This function returns the IPV6 PREFIX of an iface.  If the iface is
  NULL or IPV4, then it returns NULL.

PARAMETERS
  this_iface_ptr: Target iface ptr

RETURN VALUE
  IPv6 prefix (first 64 bits of the address), 0 if interface
  pointer is NULL or iface is IPV4 family.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 ps_iface_get_v6_prefix
(
  ps_iface_type       *this_iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  IPV6_PERROR(ps_iface_get_v6_prefix);
  return (0);
}

/*===========================================================================
FUNCTION PS_IFACE_SET_V6_IID

DESCRIPTION
  This function sets the IPV6 IID of an iface.  If the iface is
  NULL or IPV4, then it returns FALSE.

PARAMETERS
  this_iface_ptr: pointer to the interface in question.
  iid:       64-bit IPv6 interface identifier (the v6 address suffix)

RETURN VALUE
  FALSE if interface pointer is NULL or IPV4 or Logical, TRUE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_set_v6_iid
(
  ps_iface_type       *this_iface_ptr,
  uint64               iid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  IPV6_PERROR(ps_iface_set_v6_iid);
  return FALSE;
}

/*===========================================================================
FUNCTION PS_IFACE_APPLY_V6_PREFIX()

DESCRIPTION
  This function will apply a prefix to a particular interface.  In it's
  initial incarnation it will only store a single prefix - the only way to
  write a new prefix is to .  In future a more
  sophisticated method will be used to store prefixes.

PARAMETERS
  this_iface_ptr:  ptr to interface control block on which to operate on.
  gateway_iid: interface identifier of the router
  prefix: prefix being added.
  valid_lifetime: lifetime of prefix (seconds); see rfc 2461 (Section 4.6.2)
  pref_lifetime: preferred lifetime for prefix; see also rfc 2462 (Section 2)
  prefix_length

RETURN VALUE
  0 on successly applying prefix
 -1 on failure or prefix not applied

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_apply_v6_prefix
(
  ps_iface_type *this_iface_ptr,
  uint64         gateway_iid,
  uint64         prefix,
  uint32         valid_lifetime,
  uint32         pref_lifetime,
  uint8          prefix_length
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  IPV6_PERROR(ps_iface_apply_v6_prefix);
  return -1;
}

/*===========================================================================
FUNCTION PS_IFACE_REMOVE_V6_PREFIX()

DESCRIPTION
  This function will remove a prefix from the interface.  It will only fail if
  the prefix doesn't exist on this interface.

PARAMETERS
  this_iface_ptr:  ptr to interface control block on which to operate on.
  prefix: prefix being removed

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_remove_v6_prefix
(
  ps_iface_type *this_iface_ptr,
  uint64         prefix
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  IPV6_PERROR(ps_iface_remove_v6_prefix);
  return -1;
}

/*===========================================================================
FUNCTION PS_IFACE_DELETE_ALL_V6_PREFIXES()

DESCRIPTION
  This function will remove all prefixes associated with the interface.

PARAMETERS
  this_iface_ptr:  ptr to interface control block on which to operate on.

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  Currently only ONE prefix is supported on an interface.

SIDE EFFECTS
  Deletes all V6 prefixes.
===========================================================================*/
int ps_iface_delete_all_v6_prefixes
(
  ps_iface_type *this_iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  IPV6_PERROR(ps_iface_delete_all_v6_prefixes);
  return -1;
}

/*===========================================================================
FUNCTION PS_IFACE_GENERATE_IPV6_IID()

DESCRIPTION
  This function generates a random IPv6 IID, ensures that the IID generated
  is unique on the interface, and begins DAD (if necessary).

PARAMETERS
  *this_iface_ptr - Pointer to the interface to operate on.
  *iid            - Pointer to the IID to be returned by this function.
  *ps_errno       - Pointer to the error number to be returned.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_generate_ipv6_iid
(
  ps_iface_type *this_iface_ptr, /* Pointer to the interface to operate on */
  uint64        *iid,            /* Pointer to interface ID to be returned */
  int16         *ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  IPV6_PERROR(ps_iface_generate_ipv6_iid);
  return -1;
}

/*===========================================================================
FUNCTION PS_IFACE_V6_ADDR_MATCH

DESCRIPTION
  This function matches the passed IPv6 address with the possible IPv6
  addresses of the passed interface.

PARAMETERS
  struct ps_in6_addr * - Ptr to IPv6 address to match.
  ps_iface_type *   - Interface pointer.

RETURN VALUE
  TRUE  - If the passed address matches any of the IPv6 addr's of the iface.
  FALSE - Otherwise.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_v6_addr_match
(
  struct ps_in6_addr *v6_addr_ptr,
  ps_iface_type      *if_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  IPV6_PERROR(ps_iface_v6_addr_match);
  return FALSE;
}

/*===========================================================================
FUNCTION PS_IFACE_GET_LINKLOCAL_ADDR()

DESCRIPTION
  This function is used to get the link local address of the V6 interface.
  The function sets the addr type to invalid if the call fails.

  Since the IP address can either be stored in this IFACE or an associated
  IFACE, find the IP address, based on the inherit_ip_info boolean.

  This implies recursion (though enforced through a static var to depth = 1).
  Recursion is enforced via ASSERT.  In the first call to this function,
  we drill down to the iface which actually contains the linklocal addr (i.e.
  the base iface).  Then we call this function again (recursion), passing
  the base iface.  Since we are at the base iface, there is no farther
  to drill down, and we get the linklocal addr.

  Note that to return a valid IP addr, both the passed in IFACE and the
  associated PS_IFACE must be in a valid IFACE state.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be will be stored here

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Copies the ipv6 addr from ps_iface_ptr to ip_addr_ptr.
===========================================================================*/
void ps_iface_get_linklocal_addr
(
  ps_iface_type    *this_iface_ptr,
  ps_ip_addr_type  *ip_addr_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  IPV6_PERROR(ps_iface_get_linklocal_addr);
  return;
} /* ps_iface_get_linklocal_addr() */

#endif /* _NO_IPV6_STUBS */


