#ifndef PS_IFACE_FLOW_H
#define PS_IFACE_FLOW_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ I F A C E _ F L O W . H

DESCRIPTION
  Header file defining ps_flow specific ps_iface functions and definitions

EXTERNALIZED FUNCTIONS

Copyright (c) 2006-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iface_flow.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

  when        who    what, where, why
  --------    ---    --------------------------------------------------------
  08/02/11    aa     Added is_nw_initiated flag to ps_flow_create_param_type
  05/14/10   guru    Moved logical flow framework APIs to private header file

===========================================================================*/
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_iface_defs.h"
#include "ps_iface_ioctl.h"


/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Maximum number of secondary flows, an iface can have. Since a block of
  MAX_FLTR_PER_REQ is reserved in filter precedence space for each QOS
  request only these many flows can be supported
---------------------------------------------------------------------------*/
#define PS_IFACE_MAX_SEC_FLOWS  (256 / MAX_FLTR_PER_REQ)

/*---------------------------------------------------------------------------
  Instance of default flow of an iface and minimum instance that a secondary
  flow can use

  NOTE : PS_IFACE_MAX_SEC_FLOW MUST be <= PS_IFACE_MAX_SEC_FLOW_INST
---------------------------------------------------------------------------*/
#define PS_IFACE_DEFAULT_FLOW_INST  0
#define PS_IFACE_MIN_SEC_FLOW_INST  1
#define PS_IFACE_MAX_SEC_FLOW_INST  127

/*===========================================================================
TYPEDEF QOS_FLOW_VALIDATION_EX_F_PTR_TYPE

DESCRIPTION
  Function prototype for mode specific extended validation of a flow.

PARAMETERS
  flow_spec_ptr : ptr to flow spec to validate
  flow_type     : direction in which the flow spec will be applied,
                  set to QOS_MASK_RX_FLOW/QOS_MASK_RX_MIN_FLOW etc for
                  rx direction
                  set to QOS_MASK_TX_FLOW/QOS_MASK_TX_MIN_FLOW etc for
                  Tx direction

RETURN VALUE
  TRUE  : flow spec set is valid
  FALSE : one or more parameters in flow spec are invalid

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef boolean (*qos_flow_validation_ex_f_ptr_type)
(
  ip_flow_type                   * flow_spec_ptr,
  qos_spec_field_mask_enum_type    flow_type
);

/*===========================================================================
TYPEDEF QOS_FLTR_VALIDATION_EX_F_PTR_TYPE

DESCRIPTION
  Function prototype for mode specific extended validation of a filter set.

PARAMETERS
  fi_ptr    : ptr to filter set to validate
  flow_type : direction in which the filter set will be applied,
              set to QOS_MASK_RX_FLOW for rx direction
              set to QOS_MASK_TX_FLOW for Tx direction

RETURN VALUE
  TRUE  : filter set is valid
  FALSE : one or more parameters in filter set are invalid

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef boolean (*qos_fltr_validation_ex_f_ptr_type)
(
  ip_filter_type                 * fi_ptr,
  qos_spec_field_mask_enum_type    flow_type
);

/*===========================================================================
TYPEDEF PS_FLOW_CREATE_PARAM_TYPE

DESCIPTION
  Parameters that are needed to create a flow. Mode handler packages all the
  necessary information in to this structure and passes to
  ps_iface_create_flow().

FIELDS
  qos_spec            : QOS spec passed by application
  flow_validate_f_ptr : CB function which performs additional flow validation
  fltr_validate_f_ptr : CB function which performs additional fltr validation
  fltr_priority       : priority in which filters are ordered
  fltr_compare_f_ptr  : function which can be used to compare two filters.
                        It returns which filter has higher precedence which
                        is used while assigning precedence to filters.
  subset_id           : Subset ID assigned to filter spec passed in QOS spec
  enable_filtering    : Indicates whether to turn on filtering
  is_nw_initiated     : Indicates whether QoS is initiated by NW or UE
===========================================================================*/
typedef struct
{
  qos_spec_type                                    * qos_spec;
  qos_flow_validation_ex_f_ptr_type                flow_validate_f_ptr;
  qos_fltr_validation_ex_f_ptr_type                fltr_validate_f_ptr;
  fltr_priority_enum_type                          fltr_priority;
  ipfltr_comparison_f_ptr_type                     fltr_compare_f_ptr;
  ps_iface_ipfltr_subset_id_type                   subset_id;
  boolean                                          enable_filtering;
  boolean                                          is_nw_initiated;
  ps_iface_ioctl_qos_request_ex_opcode_enum_type   opcode;
} ps_flow_create_param_type;


/*===========================================================================
TYPEDEF PS_FLOW_MODIFY_PARAM_TYPE

DESCIPTION
  Parameters that are needed to modify a flow. Mode handler packages all the
  necessary information in to this structure and passes to
  ps_iface_modify_flow().

FIELDS
  qos_spec            : QOS spec passed by application
  flow_validate_f_ptr : CB function which performs additional flow validation
  fltr_validate_f_ptr : CB function which performs additional fltr validation
  fltr_priority       : priority in which filters are ordered
  fltr_compare_f_ptr  : function which can be used to compare two filters.
                        It returns which filter has higher precedence which
                        is used while assigning precedence to filters.
  subset_id           : Subset ID assigned to filter spec passed in QOS spec
===========================================================================*/
typedef struct
{
  qos_spec_type                       * qos_spec;
  qos_flow_validation_ex_f_ptr_type     flow_validate_f_ptr;
  qos_fltr_validation_ex_f_ptr_type     fltr_validate_f_ptr;
  fltr_priority_enum_type               fltr_priority;
  ipfltr_comparison_f_ptr_type          fltr_compare_f_ptr;
  ps_iface_ipfltr_subset_id_type        subset_id;
} ps_flow_modify_param_type;



/*===========================================================================

                                    MACROS

===========================================================================*/
/*===========================================================================
MACRO PS_IFACE_GET_DEFAULT_FLOW()

DESCRIPTION
  This macro will return the default flow associated with the specified
  iface.

PARAMETERS
  iface_ptr : pointer to the interface.

RETURN VALUE
  pointer to default flow : on sucess
  NULL                    : on failure
===========================================================================*/
#define PS_IFACE_GET_DEFAULT_FLOW(iface_ptr)                             \
  (PS_IFACE_IS_VALID(iface_ptr)                                          \
     ? PS_IFACEI_GET_DEFAULT_FLOW(iface_ptr)                             \
     : NULL)

/*===========================================================================
MACRO PS_IFACE_IS_FLOW_DEFAULT()

DESCRIPTION
  This macro will return whether the flow specified is the default flow for
  the particular iface

PARAMETERS
  iface_ptr : pointer to the interface.
  flow_ptr  : pointer to the flow

RETURN VALUE
  TRUE if Flow is default
  FALSE otherwise
===========================================================================*/
#define PS_IFACE_IS_FLOW_DEFAULT(iface_ptr, flow_ptr)                    \
  (PS_IFACE_IS_VALID(iface_ptr) && PS_FLOW_IS_VALID(flow_ptr) &&         \
   PS_IFACEI_GET_DEFAULT_FLOW(iface_ptr) == (flow_ptr))

/*===========================================================================
MACRO PS_IFACE_GET_NUM_SEC_FLOWS()

DESCRIPTION
  This macro returns the number of secondary flows associated with this
  iface.

PARAMETERS
  iface_ptr : Target iface ptr

RETURN VALUE
  Number of secondary flows : on success
  0                         : on failure
===========================================================================*/
#define PS_IFACE_GET_NUM_SEC_FLOWS(iface_ptr)                            \
  (PS_IFACE_IS_VALID(iface_ptr)                                          \
     ? PS_IFACEI_GET_NUM_SEC_FLOWS(iface_ptr)                            \
     : 0)

/*===========================================================================
MACRO PS_IFACE_GET_FLOW_INST_FROM_ID()

DESCRIPTION
  This macro returns the flow instance given an iface id.

PARAMETERS
  iface_id: Opaque id which uniquely identifies and iface

RETURN VALUE
  flow instance.
===========================================================================*/
#define PS_IFACE_GET_FLOW_INST_FROM_ID(iface_id)                         \
  ((uint8) ((iface_id) & 0x000000FF))



/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
);

/*===========================================================================
FUNCTION PS_IFACE_CREATE_FLOW()

DESCRIPTION
  This function creates a flow, processes the requested QOS spec and stores
  it in the flow control block for later use. Both flow spec and filter spec
  are processed and validated for each direction.  A generic validation is
  performed on the parameters, if mode handlers require specific validation
  they can pass an optional validation function ptr to perform extended
  validations. Note that generic validations will still be performed before
  calling extended validaiton functions.  Both Rx and Tx flow specs are
  stored as is after making sure all specified parameters are correct. Tx
  filter spec is validated and then filter sets are sorted and stored in
  decreasing order of their precedence.  Mode handlers should provide
  alternate camparision function if they do not wish to apply generic
  comparison algorithm. Tx filter spec is passed to IP filtering library,
  where it is sorted and stored to be applied to the outgoing IP traafic.

PARAMETERS
  iface_ptr      : corresponding iface on which QOS is requested
  phys_link_ptr  : ptr to phys link to which created flow will be bound to
  qos_req_params : structure containing the qos_spec and all the
                   information needed to process it
  flow_ptr_ptr   : flow that is being created. (OUT param)
  ps_errno       : specific error code in case operation fails

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
  ps_flow_type                    ** flow_ptr_ptr,
  int16                            * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_MODIFY_FLOW()

DESCRIPTION
  This function processes the requested QOS_MODIFY spec and stores it in the
  flow control block for later use. Both flow spec and filter spec
  are processed and validated for each direction.  A generic validation is
  performed on the parameters. If mode handlers require specific validation
  they can pass an optional validation function ptr to perform extended
  validations. Note that generic validations will still be performed before
  calling extended validaiton functions. Both Rx and Tx flow specs are
  stored as is after making sure all specified parameters are correct. Tx
  filter spec is validated and then filter sets are sorted and stored in
  decreasing order of their precedence. Tx filters are disabled until network
  comes back with a response. Mode handlers should provide alternate
  camparison function if they do not wish to apply generic comparison
  algorithm.

PARAMETERS
  iface_ptr         : corresponding iface on which QOS is requested
  flow_ptr          : flow to be modified
  qos_modify_params : structure containing modified_qos_spec and all the
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
);

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
);

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
);

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
  ps_iface_type  * iface_ptr,
  ps_flow_type   * flow_ptr,
  int16          * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_GET_SEC_FLOW_HANDLE()

DESCRIPTION
  This macro returns a handle which can be used to fetch secondary flows
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

  Do not try to access flow's contents outside of atomic operation. Since
  a flow can be deleted, this may lead to memory corruption or data aborts.

  Sample usage of this API is
    TAKLOCK()
      handle = PS_IFACE_GET_SEC_FLOW_HANDLE()
      while (handle != NULL)
      {
        PS_IFACE_GET_SEC_FLOW_BY_HANDLE()
        handle = new_handle
      }
    TASKFREE();

SIDE EFFECTS
  None
===========================================================================*/
void *ps_iface_get_sec_flow_handle
(
  ps_iface_type  * iface_ptr
);

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

  Do not try to access flow's contents outside of atomic operation. Since
  a flow can be deleted, this may lead to memory corruption or data aborts.

  Sample usage of this API is
    TAKLOCK()
      handle = PS_IFACE_GET_SEC_FLOW_HANDLE()
      while (handle != NULL)
      {
        PS_IFACE_GET_SEC_FLOW_BY_HANDLE()
        handle = new_handle
      }
    TASKFREE();

SIDE EFFECTS
  next_handle is updated so that if this function is called with that
  handle, next secondary flow is fetched
===========================================================================*/
boolean ps_iface_get_sec_flow_by_handle
(
  ps_iface_type  * iface_ptr,
  void           * sec_flow_handle,
  ps_flow_type  ** sec_flow_ptr,
  void          ** next_handle
);

/*===========================================================================
FUNCTION PS_IFACEI_GET_FLOW_FROM_ID()

DESCRIPTION
  This function fetches a flow which corresponds to given iface_id.

PARAMETERS
  iface_id:  iface id to extract info from

RETURN VALUE
  flow_ptr : if iface_id is valid
  NULL     : otherwise.

DEPENDENCIES
  iface_id should be a valid iface id.

SIDE EFFECTS
  None
===========================================================================*/
ps_flow_type *ps_ifacei_get_flow_from_id
(
  ps_iface_id_type  iface_id
);

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
  ps_iface_type  * iface_ptr,
  ps_flow_type   * flow_ptr,
  boolean          enable,
  int16          * ps_errno
);

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
);

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
    TASKFREE();

SIDE EFFECTS
  None
===========================================================================*/
void *ps_iface_get_tx_fltr_handle
(
  ps_iface_type         * ps_iface_ptr,
  ps_flow_type          * flow_ptr,
  ip_version_enum_type    ip_vsn,
  boolean                 is_modify
);

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
    TASKFREE();

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
);

/*===========================================================================
FUNCTION PS_IFACE_FLOW_GET_TX_FLTR_SPEC()

DESCRIPTION
  Given a iface ptr, this function returns all the Tx filters
  installed on an iface.

PARAMETERS
  iface_ptr      : pointer to iface
  flow_ptr       : pointer to the flow
  ip_fltr_spec   : OUT PARAM, pointer to Tx filter spec
  is_modify      : is the client interested in fetching Tx filters associated
                   with pending QOS_MODIFY? If so value must be TRUE
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
boolean ps_iface_flow_get_tx_fltr_spec
(
  ps_iface_type          * iface_ptr,
  ps_flow_type           * flow_ptr,
  ip_filter_spec_type    * ip_fltr_spec,
  boolean                  is_modify,
  uint8                    num_fltrs,
  int16                  * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_FLOW_SET_TX_FILTER_SPEC()

DESCRIPTION
  Given the ip filter spec, this fucntion sets the filter id and precedence
  of all the Tx filters.

PARAMETERS
  iface_ptr      : pointer to iface
  flow_ptr       : pointer to the flow
  ip_fltr_spec   : OUT PARAM, pointer to Rx filter spec
  is_modify      : is the client interested in fetching Tx filters associated
                   with pending QOS_MODIFY? If so value must be TRUE
RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
None

SIDE EFFECTS
None

===========================================================================*/
boolean ps_iface_flow_set_tx_filter_spec
(
  ps_iface_type         * iface_ptr,
  ps_flow_type          * flow_ptr,
  ip_filter_spec_type   * ip_fltr_spec,
  boolean                 is_modify
);


/*===========================================================================
FUNCTION PS_IFACE_FLOW_GET_TX_FLTR_HANDLE_EX()

DESCRIPTION
  This function returns a handle which can be used to fetch Tx filters
  associated with a ps_flow independent of the IP version.

  Tx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  ps_iface_ptr : iface where filters are stored
  flow_ptr     : pointer to the ps_flow whose filters need to be fetched
  is_modify    : is the client interested in fetching Tx filters associated
                 with pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  A handle using which Tx filters can be fetched : on success
  NULL                                           : otherwise

DEPENDENCIES
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations

  Sample usage to fetch Tx filters pertaining to QOS_REQUEST is
    TAKLOCK()
      handle = ps_ifacei_get_tx_fltr_handle_ex(iface_ptr, flow_ptr, FALSE)
      while (handle != NULL)
      {
        ps_ifacei_get_tx_fltr_by_handle_ex(iface_ptr, flow_ptr, FALSE,
                                        handle, fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE();

SIDE EFFECTS
  None
===========================================================================*/
void *ps_iface_flow_get_tx_fltr_handle_ex
(
  ps_iface_type         * ps_iface_ptr,
  ps_flow_type          * flow_ptr,
  boolean                 is_modify
);


/*===========================================================================
FUNCTION PS_IFACE_FLOW_GET_TX_FLTR_BY_HANDLE_EX()

DESCRIPTION
  Given a handle, this function returns a Tx filter, and its precedence
  independent of the IP version..
  Currently 0 is returned as precedence for all Tx filters.

  Tx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  ps_iface_ptr   : iface where filters are stored
  flow_ptr       : pointer to the flow
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

  Sample usage to fetch Tx filters pertaining to QOS_REQUEST is
    TAKLOCK()
      handle = ps_ifacei_get_tx_fltr_handle_ex(iface_ptr, flow_ptr, FALSE)
      while (handle != NULL)
      {
        ps_ifacei_get_tx_fltr_by_handle_ex(iface_ptr, flow_ptr, FALSE,
                                       handle, fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE();

SIDE EFFECTS
  next_handle is set so that if this function is called with that handle,
  next Tx filter is fetched
===========================================================================*/
boolean ps_iface_flow_get_tx_fltr_by_handle_ex
(
  ps_iface_type          * ps_iface_ptr,
  ps_flow_type           * flow_ptr,
  void                   * tx_fltr_handle,
  boolean                  is_modify,
  ip_filter_type        ** fltr,
  uint8                  * prcd,
  void                  ** next_handle
);


/*===========================================================================
FUNCTION PS_IFACE_FLOW_GET_TX_FLTR_CNT_EX()

DESCRIPTION
  Returns number of Tx filters, independent of the IP version, specified as an
  argument. Tx filters can pertain to either QOS_REQUEST/active QoS or
  pending QOS_MODIFY

PARAMETERS
  ps_iface_ptr : iface where filters are stored
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
int8 ps_iface_flow_get_tx_fltr_cnt_ex
(
  ps_iface_type         * ps_iface_ptr,
  ps_flow_type          * flow_ptr,
  boolean                 is_modify
);

#endif /* PS_IFACE_FLOW_H */

