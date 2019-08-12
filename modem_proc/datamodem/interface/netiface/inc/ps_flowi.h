#ifndef PS_FLOWI_H
#define PS_FLOWI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ F L O W I . H


GENERAL DESCRIPTION
  This file defines internal functions used by PS to create and manipulate
  a flow.

EXTERNAL FUNCTIONS
  PS_FLOWI_CREATE()
    Creates a flow

  PS_FLOWI_DELETE()
    Deletes a flow

  PS_FLOWI_BIND_PHYS_LINK()
    Binds a phys link to a flow

  PS_FLOWI_UNBIND_PHYS_LINK()
    Unbinds a phys link from a flow

  PS_FLOWI_REBIND_PHYS_LINK()
    Binds a phys link to a flow different from its current binding

  PS_FLOWI_VALIDATE_QOS_SPEC()
    Validates a given QOS spec

  PS_FLOWI_VALIDATE_QOS_MODIFY_SPEC()
    Validates a given QOS_MODIFY spec

  PS_FLOWI_VALIDATE_PRIMARY_QOS_MODIFY_SPEC()
    Validates a given PRIMARY_QOS_MODIFY spec

  PS_FLOWI_STORE_QOS_SPEC()
    Stores a QOS spec in a ps_flow

  PS_FLOWI_STORE_QOS_MODIFY_SPEC()
    Stores a QOS_MODIFY spec in a ps_flow

  PS_FLOWI_DELETE_QOS_SPEC()
    Deletes QOS spec from a ps_flow

  PS_FLOWI_QOS_MODIFY_ACCEPTED_IND()
    Indicates that QOS_MODIFY is accepted by the network

  PS_FLOWI_QOS_MODIFY_REJECTED_IND()
    Indicates that QOS_MODIFY is rejected by the network

  PS_FLOWI_DELETE_QOS_MODIFY_SPEC()
    Deletes QOS_MODIFY spec from a ps_flow

  PS_FLOWI_DELETE_AUX_FLOW_SPEC()
    Deletes auxiliary flow specs from a qos_info_ptr

  PS_FLOWI_DELETE_RX_FLTR_SPEC()
    Deletes Rx fltr spec from a qos_info_ptr

  PS_FLOWI_GET_QOS_INFO_PTR()
    Given a flow_ptr returns qos_info_ptr where all QoS related info is
    stored pertaining to active QoS or pending QOS_MODIFY

Copyright (c) 2005-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_flowi.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
05/14/10    guru   Removed ps_iface_create_logical_flows and moved it to 
                   an internal header file
12/14/08    pp     Common Modem Interface: Public/Private API split.
07/31/06    msr    Added support for PRIMARY_QOS_MODIFY
12/05/05    msr    Taking iface family as a parameter to validation functions
08/29/05    msr    Moved ps_flowi_get_aux_flow_spec_handle() and
                   ps_flowi_get_aux_flow_spec_by_handle() to ps_flow.h
08/15/05    msr    Added ps_flowi_get_aux_flow_spec_handle() and
                   ps_flowi_get_aux_flow_spec_by_handle
08/10/05    msr    Added support for QOS_MODIFY
06/10/05    mct    Added support for multiple flow specs.
05/03/05    msr    Using int16 instead of errno_enum_type
04/17/05    msr    Created file.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ps_iface_defs.h"
#include "ps_iface.h"
#include "ps_svc.h"

#ifdef FEATURE_DATA_PS_QOS
#include "ps_flow.h"
#include "ps_qos_defs.h"
#include "ps_iface_ipfltr.h"
#include "ps_qos_net_initiated_req.h"
#endif /* FEATURE_DATA_PS_QOS */



#ifdef FEATURE_DATA_PS_QOS
/*===========================================================================

                      EXTERNAL DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
TYPEDEF PS_FLOW_RX_QOS_FLTR_BUF_TYPE

DESCRIPTION
  This is the type that is used to enqueue Rx QOS filters in a ps_flow
===========================================================================*/
typedef struct
{
  list_link_type  link;
  ip_filter_type  filter;
  uint8           precedence;
} ps_flow_rx_fltr_buf_type;

/*===========================================================================
TYPEDEF PS_FLOW_IP_FLOW_SPEC_TYPE

DESCRIPTION
  This is the type that is used to enqueue QOS IP flow specs in a ps_flow.
===========================================================================*/
typedef struct
{
  list_link_type    link;
  ip_flow_type      flow_spec;
} ps_flow_ip_flow_spec_type;
#endif /* FEATURE_DATA_PS_QOS */



/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOWI_CREATE()

DESCRIPTION
  This function creates a new flow and initializes it.

PARAMETERS
  is_qos_supported : is QOS supported on the flow being created?
  flow_ptr_ptr     : OUT PARAM, flow that is created
  ps_errno         : error returned in case of a failure

RETURN VALUE
   0 : if flow is created
  -1     : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flowi_create
(
  boolean          is_qos_supported,
  ps_flow_type  ** flow_ptr_ptr,
  int16          * ps_errno
);

/*===========================================================================
FUNCTION PS_FLOWI_BIND_PHYS_LINK()

DESCRIPTION
  Binds a flow to a phys link

PARAMETERS
  flow_ptr      : pointer to the flow
  phys_link_ptr : flow_ptr's binding

RETURN VALUE
   0 : on sucess
  -1 : on failure

DEPENDENCIES
  parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_bind_phys_link
(
  ps_flow_type       * flow_ptr,
  ps_phys_link_type  * phys_link_ptr
);

/*===========================================================================
FUNCTION PS_FLOWI_UNBIND_PHYS_LINK()

DESCRIPTION
  Unbinds a flow from its current binding to a phys link.

PARAMETERS
  flow_ptr : pointer to the flow

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  flow_ptr must be valid

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_unbind_phys_link
(
  ps_flow_type  * flow_ptr
);

/*===========================================================================
FUNCTION PS_FLOWI_REBIND_PHYS_LINK()

DESCRIPTION
  Binds a flow to a phys link, which is different from its current binding

PARAMETERS
  flow_ptr      : pointer to the flow
  phys_link_ptr : flow_ptr's new binding

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  flow must be bound already and parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_rebind_phys_link
(
  ps_flow_type       * flow_ptr,
  ps_phys_link_type  * phys_link_ptr
);

/*===========================================================================
FUNCTION PS_FLOWI_FLOW_CTRL_CMD_HANDLER()

DESCRIPTION
  This function processes PS_FLOW_FLOW_CTRL_CMD.

PARAMETERS
  ps_cmd       : Must be PS_FLOW_FLOW_CTRL_CMD.
  cmd_data_ptr : cmd data

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
  void              * cmd_data_ptr
);

#ifdef FEATURE_DATA_PS_QOS
/*===========================================================================
FUNCTION PS_FLOWI_DELETE()

DESCRIPTION
  This function deletes a flow

PARAMETERS
  flow_ptr : flow to be deleted

RETURN VALUE
   0 : on success
  -1 : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_delete
(
  ps_flow_type  * flow_ptr
);

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
  flow_ptr must be valid and must not be a default flow. Must be called
  inside a TASKLOCK()

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_qos_modify_accepted_ind
(
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code
);

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
  flow_ptr must be valid and must not be a default flow. Must be called
  inside a TASKLOCK()

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_qos_modify_rejected_ind
(
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code
);

/*===========================================================================
MACRO PS_FLOWI_GET_RX_GRANTED_FLOW2()

DESCRIPTION
  This macro returns a ptr to the previously granted Rx flow type (when
  ps_flow is suspended for example) if available, NULL otherwise.

  NOTE: Currently granted Rx flow must be fetched and accessed in one atomic
        operation as network may choose not to apply QOS at any time, in
        which case granted Rx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  ip_flow_ptr : Cached granted ip_flow type
===========================================================================*/
#define PS_FLOWI_GET_RX_GRANTED_FLOW2(flow_ptr)                            \
  ps_flowi_get_rx_granted_flow2(flow_ptr)

ip_flow_type * ps_flowi_get_rx_granted_flow2
(
  const ps_flow_type * flow_ptr
);

/*===========================================================================
MACRO PS_FLOWI_GET_TX_GRANTED_FLOW2()

DESCRIPTION
  This macro returns a ptr to the previously granted TX flow type (when
  ps_flow is suspended for example) if granted, NULL otherwise.

  NOTE: Currently granted Tx flow must be fetched and accessed in one atomic
        operation as network may choose not to apply QOS at any time, in
        which case granted Tx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  ip_flow_ptr : Cached granted ip_flow type
===========================================================================*/
#define PS_FLOWI_GET_TX_GRANTED_FLOW2(flow_ptr)                           \
  ps_flowi_get_tx_granted_flow2(flow_ptr)

ip_flow_type * ps_flowi_get_tx_granted_flow2
(
  const ps_flow_type * flow_ptr
);

/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_NET_INITIATED_QOS_SPEC()

DESCRIPTION
This function validates the requested QOS spec. Both flow spec and filter
spec are validated in each direction. A generic validation is performed on
the parameters and if mode handlers require extra validation they can pass
an optional validation function ptr to perform extended validation. Note
that generic validation will still be performed before calling extended
validaiton functions.

PARAMETERS
qos_req_param     : structure containing qos_spec and information necessary
to validate it.
iface_addr_family : addr family of iface on which QoS is requested
ps_errno          : specific error code in case operation fails

RETURN VALUE
0 : if QOS spec is valid
-1 : otherwise

DEPENDENCIES
None

SIDE EFFECTS
None
===========================================================================*/
int ps_flowi_validate_net_initiated_qos_spec
(
  const net_initiated_qos_spec_type  *   net_initiated_qos_spec,
  ps_iface_addr_family_type              iface_addr_family,
  int16  *                               ps_errno
);


/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_QOS_SPEC()

DESCRIPTION
  This function validates the requested QOS spec. Both flow spec and filter
  spec are validated in each direction. A generic validation is performed
  on the parameters. If mode handlers require specific validation they can
  pass an optional validation function ptr to perform extended validations.
  Note that generic validations will still be performed before calling
  extended validaiton functions.

PARAMETERS
  qos_req_param     : structure containing qos_spec and information necessary
                      to validate it.
  iface_addr_family : addr family of iface on which QoS is requested
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_validate_qos_spec
(
  const ps_flow_create_param_type  * qos_req_param,
  ps_iface_addr_family_type          iface_addr_family,
  int16                            * ps_errno
);

/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_QOS_MODIFY_SPEC()

DESCRIPTION
  This function validates the modified QOS spec. If modified, flow spec and
  filter spec are validated in each direction. A generic validation is
  performed on the parameters. If mode handlers require specific validation
  they can pass an optional validation function ptr to perform extended
  validations. Note that generic validations will still be performed before
  calling extended validaiton functions.

PARAMETERS
  flow_ptr          : flow to be modified
  qos_modify_param  : structure containing qos_spec and information necessary
                      to validate it.
  iface_addr_family : addr family of iface to which "ps_flow" belongs to
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : if QOS spec is valid
  -1 : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flowi_validate_qos_modify_spec
(
  ps_flow_type                     * flow_ptr,
  const ps_flow_modify_param_type  * qos_modify_param,
  ps_iface_addr_family_type          iface_addr_family,
  int16                            * ps_errno
);

/*===========================================================================
FUNCTION PS_FLOWI_VALIDATE_PRIMARY_QOS_MODIFY_SPEC()

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
int ps_flowi_validate_primary_qos_modify_spec
(
  primary_qos_spec_type              * primary_qos_spec_ptr,
  qos_flow_validation_ex_f_ptr_type    flow_validate_f_ptr,
  qos_spec_field_mask_type             requested_qos_field_mask,
  int16                              * ps_errno
);

/*===========================================================================
FUNCTION PS_FLOWI_STORE_QOS_SPEC()

DESCRIPTION
  This function stores the requested flow spec, and Rx filter spec in the
  flow.

PARAMETERS
  flow_ptr           : flow where part of QOS spec is to be stored.
  qos_spec           : QOS spec to be stored.
  fltr_priority      : priority class of filters
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  Flow must not be a default flow as QOS is not supported on default flows.

SIDE EFFECTS
  If error occurs during storage, none of QOS spec is stored, i.e semantics
  is all or nothing
===========================================================================*/
int ps_flowi_store_qos_spec
(
  ps_flow_type                  * flow_ptr,
  const qos_spec_type           * qos_spec,
  fltr_priority_enum_type         fltr_priority,
  ipfltr_comparison_f_ptr_type    fltr_compare_f_ptr,
  int16                         * ps_errno
);

/*===========================================================================
FUNCTION PS_FLOWI_STORE_QOS_MODIFY_SPEC()

DESCRIPTION
  This function stores the modified flow spec, and modified Rx filter spec
  in the flow.

PARAMETERS
  flow_ptr           : flow where part of QOS spec is to be stored.
  qos_modify_spec    : QOS spec to be stored.
  fltr_priority      : priority class of filters
  fltr_compare_f_ptr : fltr comparator
  ps_errno           : error code passed back when storage fails.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  Must be called inside TASKLOCK()

SIDE EFFECTS
  If error occurs during storage, none of QOS spec is stored, i.e semantics
  is all or nothing
===========================================================================*/
int32 ps_flowi_store_qos_modify_spec
(
  ps_flow_type                        * flow_ptr,
  const qos_spec_type                 * qos_modify_spec_ptr,
  fltr_priority_enum_type               fltr_priority,
  ipfltr_comparison_f_ptr_type          fltr_compare_f_ptr,
  int16                               * ps_errno
);

/*===========================================================================
FUNCTION PS_FLOWI_DELETE_QOS_SPEC()

DESCRIPTION
  This function deletes previously stored flow spec, Rx filter spec and
  frees up any memory buffers used.

PARAMETERS
  flow_ptr : ptr to flow, where flow spec is stored

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  Flow must not be a default flow as QOS is not supported on default flows.
  rx.fltr_list of qos_info_ptr must be initialized

SIDE EFFECTS
  None
===========================================================================*/
int ps_flowi_delete_qos_spec
(
  ps_flow_type  * flow_ptr
);

/*===========================================================================
FUNCTION PS_FLOWI_DELETE_QOS_MODIFY_SPEC()

DESCRIPTION
  This function deletes previously stored modified flow_spec,
  modified Rx filter spec and frees up any memory buffers used.

PARAMETERS
  flow_ptr : ptr to flow, where qos_modify_spec spec is stored

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  Flow must not be a default flow as QOS is not supported on default flows.
  rx.fltr_list of qos_modify_info_ptr must be initialized

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flowi_delete_qos_modify_spec
(
  ps_flow_type  * flow_ptr
);

/*===========================================================================
FUNCTION PS_FLOWI_DELETE_AUX_FLOW_SPEC()

DESCRIPTION
  Deletes auxiliary flow spec from specified location based on flow_type

PARAMETERS
  qos_info_ptr : Address where auxiliary flow spec is stored. Could be
                 qos_info_ptr or modify_qos_info_ptr of ps_flow
  flow_type    : Specifies direction of flow spec

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_delete_aux_flow_spec
(
  const qos_info_type            * qos_info_ptr,
  qos_spec_field_mask_enum_type    flow_type
);

/*===========================================================================
FUNCTION PS_FLOWI_DELETE_RX_FLTR_SPEC()

DESCRIPTION
  Deletes Rx fltr spec from specified location

PARAMETERS
  qos_info_ptr : Address where Rx fltr spec is stored. Could be qos_info_ptr
                 or modify_qos_info_ptr of ps_flow

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_delete_rx_fltr_spec
(
  qos_info_type  * qos_info_ptr
);

/*===========================================================================
FUNCTION PS_FLOWI_GET_QOS_INFO_PTR()

DESCRIPTION
  Returns an address where QoS info is stored. This is different for
  QOS_REQUEST/active QoS, and pending QOS_MODIFY

PARAMETERS
  flow_ptr  : pointer to the flow serving the QOS
  is_modify : is the client interested in qos_info_ptr associated with
              pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  qos_info_ptr : on success
  NULL         : otherwise

DEPENDENCIES
  Must be called in a TASKLOCK()

SIDE EFFECTS
  None
===========================================================================*/
qos_info_type *ps_flowi_get_qos_info_ptr
(
  ps_flow_type  * flow_ptr,
  boolean         is_modify
);

/*===========================================================================
FUNCTION PS_FLOWI_GET_RX_FLTR_HANDLE_EX()

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
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations

  Sample usage to fetch Rx filters pertaining to QOS_REQUEST is
    TAKLOCK()
      handle = ps_flowi_get_rx_fltr_handle_ex(flow_ptr, FALSE)
      while (handle != NULL)
      {
        ps_flow_get_rx_fltr_by_handle_ex(flow_ptr, FALSE, handle,
                                      fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE();

SIDE EFFECTS
  None
===========================================================================*/
void *ps_flowi_get_rx_fltr_handle_ex
(
  ps_flow_type          * flow_ptr,
  boolean                 is_modify
);

/*===========================================================================
FUNCTION PS_FLOWI_GET_RX_FLTR_BY_HANDLE_EX()

DESCRIPTION
  Given a handle, this function returns a Rx filter, and also the
  precedence of the filter independent of the IP version.
  A new handle is returned so that next 
  filter is fetched when this function is invoked with that handle.

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
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations.

  Do not try to access filter contents outside of atomic operation. Since
  a filter can be deleted, this may lead to memory corruption or data aborts.

  Sample usage to fetch Rx filters pertaining to QOS_REQUEST is
    TAKLOCK()
      handle = ps_flowi_get_rx_fltr_handle_ex(flow_ptr, FALSE)
      while (handle != NULL)
      {
        ps_flowi_get_rx_fltr_by_handle_ex(flow_ptr, FALSE, handle,
                                          fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE();

SIDE EFFECTS
  next_handle is set so that if this function is called with that handle,
  next Rx filter is fetched
===========================================================================*/
boolean ps_flowi_get_rx_fltr_by_handle_ex
(
  ps_flow_type           * flow_ptr,
  void                   * rx_fltr_handle,
  boolean                  is_modify,
  ip_filter_type        ** fltr,
  uint8                  * prcd,
  void                  ** next_handle
);

/*===========================================================================
FUNCTION PS_FLOWI_GET_RX_FLTR_CNT_EX()

DESCRIPTION
  Returns number of Tx filters, independent of the IP version, specified as an
  argument. Tx filters can pertain to either QOS_REQUEST/active QoS or
  pending QOS_MODIFY

PARAMETERS
  flow_ptr     : pointer to the ps_flow whose filters need to be fetched
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
int8 ps_flowi_get_rx_fltr_cnt_ex
(
  ps_flow_type          * flow_ptr,
  boolean                 is_modify
);

/*===========================================================================
MACRO PS_FLOWI_GET_QOS_HANDLE()

DESCRIPTION
  This macro returns the opaque id for a QOS instance given an iface and
  flow which represents the QOS instance

PARAMETERS
  iface_ptr : iface on which QOS instance is supported
  flow_ptr  : flow which represents the QOS instance

RETURN VALUE
  A valid QOS handle or NULL

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_FLOWI_GET_QOS_HANDLE(iface_ptr, flow_ptr)                     \
  ((PS_IFACE_IS_VALID(iface_ptr) && PS_FLOW_IS_VALID(flow_ptr))          \
      ? (PS_IFACE_GET_ID3(iface_ptr->name,                               \
                          iface_ptr->instance,                           \
                          PS_FLOWI_GET_COOKIE(flow_ptr)))                \
      : 0)

/*===========================================================================
MACRO PS_FLOWI_GET_ASSOC_COOKIE()

DESCRIPTION
  Gets cookie which uniquely identifies this flow across its incarnations

PARAMETERS
  flow_ptr : ptr to flow

RETURN VALUE
  a cookie uniquely identifying this flow across its incarnations
===========================================================================*/
#define PS_FLOWI_GET_ASSOC_COOKIE(flow_ptr)                                 \
  (((ps_flow_type *)flow_ptr)->flow_private.logical_flow.assoc_flow_cookie)

/*===========================================================================
MACRO PS_FLOWI_SET_ASSOC_COOKIE()

DESCRIPTION
  Sets cookie for this flow. A cookie uniquely identifies a flow across its
  incarnations

PARAMETERS
  flow_ptr : ptr to flow
  cookie   : a cookie uniquely identifying this flow across its incarnations

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOWI_SET_ASSOC_COOKIE(flow_ptr, flow_cookie)                    \
  (((ps_flow_type *)flow_ptr)->flow_private.logical_flow.assoc_flow_cookie = flow_cookie)

#endif /* FEATURE_DATA_PS_QOS */

#endif /* PS_FLOWI_H */
