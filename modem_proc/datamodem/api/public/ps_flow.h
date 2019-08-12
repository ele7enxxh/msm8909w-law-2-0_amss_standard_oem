#ifndef PS_FLOW_H
#define PS_FLOW_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ F L O W . H

DESCRIPTION
  Header file defining the control block to represent an IP flow and
  functions to manipulate it.

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
    Indicates that the Rx filters installed on a flow have changed

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

Copyright (c) 2005-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_flow.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/07/12    pgm    New Capability added to drop data packets on flow.
11/28/11    sun    Adding support for DPL logging using log codes
09/15/11    msr    New API to post a cmd to PS task for flow control
08/02/11    aa     Added is_nw_initiated flag to ps_flow and
                   PS_FLOW_IS_NW_INITIATED macro
07/21/11    msr    Added a cookie field to ps_flow
11/2/10     gn     Added assoc_flow_cookie to track underlying flow
08/16/10    pp     Replaced internal MACRO with a public API
                   PS_TX_META_INFO_IS_VALID ==> !(PS_TX_META_INFO_IS_NULL).
07/20/10   guru    Changed the return type of ps_flow_configure_cmd
05/14/10   guru    Changed PS_FLOW_GET_CAPABILITY, PS_FLOW_GET_RX_GRANTED_FLOW,
                   PS_FLOW_GET_TX_GRANTED_FLOW into functions to return
                   the values from logical flows
02/28/10    pp     PS_FLOW_GET_FLOW_FROM_META_INFO update.
12/28/08    pp     Common Modem Interface: Public/Private API split.
09/12/08    pp     Metainfo optimizations.
09/01/07    ssh    QoS support for logical ifaces
6/28/07    msr/ssh QoS supportfor M.IP CCoA iface (currently featurized)
11/17/06    msr    Added support for timestamping packets
07/31/06    msr    Added support for PRIMARY_QOS_MODIFY
04/25/06    msr    L4/Tasklock code review changes
03/27/06    msr    Reverted deprecation of PS_FLOW_SET_CURRENT_FLOW()
02/27/06    msr    Deprecated PS_FLOW_SET_CURRENT_FLOW(), and added
                   PS_FLOW_SET_[TR]X_GRANTED_FLOW()
02/22/06    msr    Using single critical section
02/17/06    msr    Exporting ps_flow_unbind_phys_link()
02/15/06    msr    Aliased PS_FLOW_SET_CURRENT_FLOW() to
                   PS_FLOW_SET_GRANTED_FLOW()
02/13/06    msr    Fixed compilation errors
02/06/06    msr    Updated for L4 tasklock/crit sections.
01/09/06    msr    Conforming to new QOS_MODIFY API
12/15/05    msr    Changed PS_FLOW_GET_FLOW_FROM_META_INFO()
12/05/05    msr    Changed PS_FLOW_GET_FLOW_FROM_META_INFO()
10/18/05    msr    Removed support for FLOW_ACTIVATE_MODIFIED_EV and
                   PHYS_LINK_UP_MODIFIED_EV
08/29/05    msr    Added ps_flow_get_aux_flow_spec_cnt(),
                   ps_flow_get_aux_flow_spec_handle(), and
                   ps_flow_get_aux_flow_spec_by_handle(). Removed macros that
                   are defined to access auxiliary flow specs. Changed API
                   to fetch Rx filters
08/16/05    msr    Fixed extra parenthesis in PS_FLOW_GET_QOS_FIELD_MASK()
08/15/05    mct    Support for QOS_CONFIGURE.
08/15/05    msr    Added control block to support flow logging. Added macros
                   to access auxiliary flow specs and also added
                   ps_flow_set_current_flow()
08/15/05    kr     Added struct flow_i_stats to struct ps_flow_s for
                   stats update
08/10/05    msr    Added support for QOS_MODIFY.
06/10/05    mct    Add support for auxiliary flow specs.
05/04/05    msr    Changed PS_META_FILTER_ID() to PS_META_FILTER_RESULT().
05/02/05    msr    Added ASSERT in SET and RESET_CAPABILITY macros and
                   removed event buffer which iface used to register for
                   FLOW_NULL_EV.
04/22/05    ks     Removed ASSERT in SET and RESET_CAPABILITY macros.
04/17/05    msr    Created the file.

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "list.h"
#include "ps_iface_defs.h"
#include "ps_flow_ioctl.h"
#include "ps_flow_event.h"
#include "ps_iface_flow.h"
#include "ps_iface_ipfltr.h"
#include "ps_ipfltr_defs.h"
#include "ps_qos_defs.h"
#include "ps_flowi_utils.h"
#include "ps_logging_defs.h"
#include "ps_tx_meta_info.h"


/*===========================================================================

                               F_PTR TYPES

===========================================================================*/
/*===========================================================================
TYPEDEF PS_FLOW_CMD_F_PTR_TYPE

DESCRIPTION
  Definition of the function for flow command call back functions.

PARAMETERS
  flow_ptr        : flow to operate upon.
  client_data_ptr : pointer to any client data that needs to be passed in

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef int (*ps_flow_cmd_f_ptr_type)
(
  ps_flow_type  * flow_ptr,
  void          * client_data_ptr
);



/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  A cookie to indicate if ps_flow is valid
---------------------------------------------------------------------------*/
#define PS_FLOW_VALID_COOKIE  0xADFCEB73UL

/*---------------------------------------------------------------------------
  Maximum number of secondary ps_flows that are supported by the mobile
---------------------------------------------------------------------------*/
#define PS_FLOW_MAX_SEC_PS_FLOWS  40

/*---------------------------------------------------------------------------
  Maximum number of ps_flows that are supported by the mobile
---------------------------------------------------------------------------*/
#define PS_FLOW_MAX_PS_FLOWS  (MAX_SYSTEM_IFACES + PS_FLOW_MAX_SEC_PS_FLOWS)

/*===========================================================================
TYPEDEF QOS_INFO_TYPE

DESCRIPTION
  QOS information block stored with the flow providing the QOS.
===========================================================================*/
typedef struct
{
  qos_spec_field_mask_type  field_mask;

  /* Structure holding Rx QOS parameters */
  struct
  {
    struct
    {
      ip_flow_type   req;             /* Requested flow spec               */
      list_type      aux_flow_list;   /* List of auxiliary Rx flows specs  */
      ip_flow_type   min_req;         /* Requested min flow spec           */
      ip_flow_type   granted;         /* Granted flow spec                 */
    } ipflow;

    list_type        fltr_list;       /* List of Rx filters                */

    /* Handle to list of nodes in iface that point to Rx filters in
       ps_flow */
    ps_iface_rx_qos_fltr_handle_type  fltr_handle;

  } rx;

  /* Structure holding Tx QOS parameters */
  struct
  {
    struct
    {
      ip_flow_type   req;             /* Requested flow spec               */
      list_type      aux_flow_list;   /* List of auxiliary Tx flows specs  */
      ip_flow_type   min_req;         /* Requested min flow spec           */
      ip_flow_type   granted;         /* Granted flow spec                 */
    } ipflow;

    ps_iface_ipfltr_handle_type       fltr_handle; /* Handle to list of
                                                      Tx fltrs in iface    */
  } tx;
} qos_info_type;


/*===========================================================================
TYPEDEF PS_FLOW_CAPABILITY_ENUM_TYPE

DESCRIPTION
  Lists various capabilities of a flow. If a particular bit is ON, then
  flow will have corresponding capability

  NOTE : Enums are not bit masks. They represent bit position in a mask which
         represents a particular capability

    PS_FLOW_CAPABILITY_DEFAULT         - if this bit is ON, flow is a default
                                         flow
    PS_FLOW_CAPABILITY_DELAY_SENSITIVE - if this bit is ON, packets sent on
                                         this flow are delay sensitive
    PS_FLOW_CAPABILITY_DATA_DISALLOWED - if this bit is ON, all data packets
                                         are dropped on this flow by the
                                         framework

  NOTE: If more than PS_FLOW_CAPABILITY_MAX_ALLOWED capabilities are
        defined then one may have to change type definition of
        capability_mask in ps_flow control block
===========================================================================*/
typedef enum
{
  PS_FLOW_CAPABILITY_MIN              = 0x0,
  PS_FLOW_CAPABILITY_DEFAULT          = PS_FLOW_CAPABILITY_MIN,
  PS_FLOW_CAPABILITY_DELAY_SENSITIVE  = 0x1,
  PS_FLOW_CAPABILITY_DATA_DISALLOWED  = 0x2,
  PS_FLOW_CAPABILITY_MAX,
  PS_FLOW_CAPABILITY_MAX_ALLOWED      = 0x8,
  PS_FLOW_CAPABILITY_FORCE_SIGNED_32_BIT = 0x7FFFFFFF
} ps_flow_capability_enum_type;

typedef uint8  ps_flow_capability_mask_type;


/*---------------------------------------------------------------------------
  Flow logging control block for Tx direction. It is not supported in Rx
  direction
    - snaplen                    : partial logging length for network protocols
    - mask                       : A set bit implies that logging is enabled for
                                   corresponding protocol (see netprot enum)
    - seq_num                    : Sequence number sent in last log packet
    - is_logging_enabled         : Indicates if logging is enabled or not
---------------------------------------------------------------------------*/
typedef struct
{
  uint32        mask;
  uint32        snaplen[DPL_IID_NETPROT_MAX];
  uint16        seq_num[DPL_IID_NETPROT_MAX];
  boolean       is_logging_enabled;
} dpl_flow_dir_cb_type;

/*---------------------------------------------------------------------------
  These two values are defined to have a same numerical value as their DS
  layer counterparts
---------------------------------------------------------------------------*/
typedef enum
{
  PS_FLOW_MODIFY_RESULT_ACCEPTED = 1,
  PS_FLOW_MODIFY_RESULT_REJECTED = 2
} ps_flow_modify_result_type;


typedef struct flow_event_info_cache_s
{
  struct
  {
    ps_flow_state_enum_type          state;
    ps_extended_info_code_enum_type  info_code;
  } flow_event_info;

  ps_flow_modify_result_type         modify_result;

  struct
  {
    boolean                          is_modify_succeeded;
  } primary_qos_modify_result_info;

  uint32                             flow_mask;

} flow_event_info_cache_type;

/*===========================================================================
TYPEDEF PS_FLOW_TYPE

DESCIPTION
  Control block for a flow. An instance of this type is created when a flow
  is created and is destroyed when flow is deleted.
===========================================================================*/
struct ps_flow_s
{
  list_link_type          link;             /* Ptr to next node in a list  */
  /*-------------------------------------------------------------------------
    The private information that should only ever be changed through ps_flow
    functions. The data in here should NOT be changed directly by a client.
  -------------------------------------------------------------------------*/
  struct
  {
    ps_flow_type        * this_flow;        /* If NULL, flow is invalid    */
    uint32                valid_cookie;
    q_type                event_q_array[MAX_EVENT_QUEUES];
    qos_info_type       * qos_info_ptr;     /* Ptr to QOS info structure   */
    qos_info_type       * qos_modify_info_ptr; /* QOS_MODIFY info structure*/
    ps_phys_link_type   * phys_link_ptr;    /* Associated phys link        */
    uint32                tx_flow_mask;     /* Mask used to flow control   */
    ps_iface_type       * iface_ptr;        /* Associated iface            */

    /*-----------------------------------------------------------------------
      Since flows can be created and deleted arbitrarily, one memory buffer
      could be used to represent more than one flow while mobile is powered
      on. So cookie is used to uniquely identify a flow across incarnations,
      which is useful in detecting if app is trying to use a flow it has
      already deleted
    -----------------------------------------------------------------------*/
    uint8                 cookie;

    /* Mask identifying verious capabilities of this flow */
    ps_flow_capability_mask_type   capability_mask;

    ps_flow_state_enum_type        state;   /* This flow's state           */

    /*-----------------------------------------------------------------------
      Structure to hold logical flow related information
    -----------------------------------------------------------------------*/
    struct
    {
      ps_flow_type        * assoc_flow_ptr;   /* Associated ps_flow        */
      int                   assoc_flow_cookie;/* Cookie of associated flow  */
      ps_tx_meta_info_type * mi_ptr;          /* Pointer to TX meta-info  */

      /*---------------------------------------------------------------------
        Event buffers to register for associated ps_flow events if
        assoc_flow_ptr is non-NULL
      ---------------------------------------------------------------------*/
      struct
      {
        void  * activated;
        void  * suspended;
        void  * null;
        void  * info_code_updated;
        void  * modify_accepted;
        void  * modify_rejected;
        void  * tx_enabled;
        void  * tx_disabled;
      } ps_flow_ev_cback_buf;
    } logical_flow;

    /*-----------------------------------------------------------------------
      Event buffers to register for phys link events if phys link is
      FLOW_COUPLED
    -----------------------------------------------------------------------*/
    struct
    {
      void  * coming_up;
      void  * up;
      void  * going_down;
      void  * down;
      void  * resuming;
      void  * going_null;
      void  * null;
    } phys_link_ev_cback_buf;

    /*-----------------------------------------------------------------------
      Flag indicates whether the new flow was initiated by the network or by a
      request from  an application on the UE.
    -----------------------------------------------------------------------*/
    boolean  is_nw_initiated;

  } flow_private;

  /*-------------------------------------------------------------------------
    DPL control block for flow logging
      - tx_cb     : control block for Tx side
      - tx_dpl_id : DPL IID of this ps_flow. Protocol field must always be 0
      - desc      : unique description of this ps_flow

    Note that there is no control block for Rx side as we don't support
    flow logging on Rx path. Basically MSM doesn't know how the incoming pkt
    is filtered by the network
  -------------------------------------------------------------------------*/
  struct
  {
    dpl_flow_dir_cb_type   tx_cb;
    dpl_iid_type           tx_dpl_id;
    char                   desc[DPL_FLOW_DESC_S_LEN];
  } dpl_flow_cb;

  struct flow_stats
  {
    uint32  pkts_tx;
    uint64  bytes_tx;
    uint32  queried;
  } flow_i_stats;

  void                  * client_data_ptr;  /* Ptr to user specific data   */

  /*-------------------------------------------------------------------------
    Command callback functions - called from PS->Flow client
  -------------------------------------------------------------------------*/
  ps_flow_cmd_f_ptr_type  ps_flow_activate_cmd_f_ptr;
  ps_flow_cmd_f_ptr_type  ps_flow_configure_cmd_f_ptr;
  ps_flow_cmd_f_ptr_type  ps_flow_suspend_cmd_f_ptr;
  ps_flow_cmd_f_ptr_type  ps_flow_go_null_cmd_f_ptr;

  ps_flow_ioctl_f_ptr_type  ps_flow_ioctl_f_ptr; /* FLOW IOCTL CB function */

  flow_event_info_cache_type  event_info_cache;

};

/*===========================================================================

                                    MACROS

===========================================================================*/
/*===========================================================================
MACRO PS_FLOW_IS_VALID()

DESCRIPTION
  This macro returns TRUE if a flow is valid.

PARAMETERS
  flow_ptr : pointer to the flow

RETURN VALUE
  TRUE  : if flow is valid
  FALSE : if flow is invalid
===========================================================================*/
#define PS_FLOW_IS_VALID(flow_ptr)                                       \
  ((flow_ptr) != NULL &&                                                 \
   (flow_ptr)->flow_private.this_flow == (flow_ptr) &&                   \
   (flow_ptr)->flow_private.valid_cookie == PS_FLOW_VALID_COOKIE)

/*===========================================================================
MACRO PS_FLOW_GET_STATE()

DESCRIPTION
  Returns a flow's current state

PARAMETERS
  flow_ptr : pointer to the flow

RETURN VALUE
  flow's state       : on success
  FLOW_STATE_INVALID : on failure
===========================================================================*/
#define PS_FLOW_GET_STATE(flow_ptr)                                      \
  (PS_FLOW_IS_VALID(flow_ptr) ? PS_FLOWI_GET_STATE(flow_ptr)             \
                              : FLOW_STATE_INVALID)

/*===========================================================================
MACRO PS_FLOW_SET_CAPABILITY()

DESCRIPTION
  Sets a capability of a flow.

  NOTE : Only one capability can be set each time.

  NOTE : External clients must not try to set PS_FLOW_CAPABILITY_DEFAULT.

PARAMETERS
  flow_ptr   : pointer to flow
  capability : capability which needs to be set

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOW_SET_CAPABILITY(flow_ptr, capability)                       \
  if (PS_FLOW_IS_VALID(flow_ptr) && (capability) < PS_FLOW_CAPABILITY_MAX) \
  {                                                                        \
    ASSERT((capability) != PS_FLOW_CAPABILITY_DEFAULT);                    \
    PS_FLOWI_SET_CAPABILITY(flow_ptr, capability);                         \
  }

/*===========================================================================
MACRO PS_FLOW_RESET_CAPABILITY()

DESCRIPTION
  Resets a capability of a flow.

  NOTE : Only one capability can be reset each time.

  NOTE : External clients must not try to reset PS_FLOW_CAPABILITY_DEFAULT.

PARAMETERS
  flow_ptr   : pointer to flow
  capability : capability which needs to be reset

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOW_RESET_CAPABILITY(flow_ptr, capability)                     \
  if (PS_FLOW_IS_VALID(flow_ptr) && (capability) < PS_FLOW_CAPABILITY_MAX) \
  {                                                                        \
    ASSERT((capability) != PS_FLOW_CAPABILITY_DEFAULT);                    \
    PS_FLOWI_RESET_CAPABILITY(flow_ptr, capability);                       \
  }

/*===========================================================================
MACRO PS_FLOW_GET_CAPABILITY()

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
#define PS_FLOW_GET_CAPABILITY(flow_ptr, capability)                        \
  ps_flow_get_capability(flow_ptr, capability)

/*===========================================================================
MACRO PS_FLOW_GET_ASSOC_PS_FLOW()

DESCRIPTION
  Returns a flow's current associated ps_flow ptr

PARAMETERS
  flow_ptr : pointer to the flow

RETURN VALUE
  an associated ps_flow_ptr : on success
  NULL                      : on failure
===========================================================================*/
#define PS_FLOW_GET_ASSOC_PS_FLOW(flow_ptr)                              \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr)                              \
     : NULL)

/*===========================================================================
MACRO PS_FLOW_GET_PHYS_LINK()

DESCRIPTION
  Returns a flow's current binding, which could be NULL if flow is not
  bound yet

PARAMETERS
  flow_ptr : pointer to the flow

RETURN VALUE
  phys_link_ptr : on success
  NULL          : on failure
===========================================================================*/
#define PS_FLOW_GET_PHYS_LINK(flow_ptr)                                  \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_GET_PHYS_LINK(flow_ptr)                                  \
     : NULL)

/*===========================================================================
MACRO PS_FLOW_IS_TX_ENABLED()

DESCRIPTION
  Checks if flow can transmit traffic

PARAMETERS
  flow_ptr : ptr to flow

RETURN VALUE
  TRUE  : if flow is enabled
  FALSE : otherwise
===========================================================================*/
#define PS_FLOW_IS_TX_ENABLED(flow_ptr)                                  \
  (PS_FLOW_IS_VALID(flow_ptr) ? PS_FLOWI_IS_TX_ENABLED(flow_ptr)         \
                              : FALSE)

/*===========================================================================
MACRO PS_FLOW_GET_TX_MASK()

DESCRIPTION
  Returns the flow mask of the passed in flow.

PARAMETERS
  flow_ptr : ptr to flow

RETURN VALUE
  flow mask : on success
  0         : on failure
===========================================================================*/
#define PS_FLOW_GET_TX_MASK(flow_ptr)                                    \
   ( PS_FLOW_IS_VALID(flow_ptr) ? PS_FLOWI_GET_TX_MASK(flow_ptr)     \
                                : 0 )

/*===========================================================================
MACRO PS_FLOW_SET_CB_F_PTR()

DESCRIPTION
  This macro sets command and IOCTL call back functions for a flow.

PARAMETERS
  flow_ptr           : pointer to the flow
  activate_cmd_f_ptr : call back function for activate cmd
  configure_cmd_f_ptr : call back function for configure cmd
  suspend_cmd_f_ptr  : call back function for suspend cmd
  go_null_cmd_f_ptr  : call back function for go_null cmd
  ioctl_cb_f_ptr     : call back function for flow IOCTLs

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOW_SET_CB_F_PTR(flow_ptr, activate_cmd_f_ptr,               \
                             configure_cmd_f_ptr, suspend_cmd_f_ptr,     \
                             go_null_cmd_f_ptr, ioctl_cb_f_ptr)          \
  if (PS_FLOW_IS_VALID(flow_ptr))                                        \
  {                                                                      \
    PS_FLOWI_SET_CB_F_PTR(flow_ptr,                                      \
                          activate_cmd_f_ptr,                            \
                          configure_cmd_f_ptr,                           \
                          suspend_cmd_f_ptr,                             \
                          go_null_cmd_f_ptr,                             \
                          ioctl_cb_f_ptr);                               \
  }

/*===========================================================================
MACRO PS_FLOW_GET_FLOW_FROM_META_INFO()

DESCRIPTION
  This macro gets flow ptr from the specified meta info, if present.
  Flow ptr corresponds to the ps_flow on which data is going to be
  transmitted on, and could be NULL which represents the default flow.

PARAMETERS
  mi_ptr : pointer to the TX meta info.

RETURN VALUE
  flow_ptr : if metainfo is valid
  NULL     : if data is going on default flow or if metainfo is invalid
===========================================================================*/
#define PS_FLOW_GET_FLOW_FROM_META_INFO(mi_ptr)                             \
  (!PS_TX_AND_RT_META_INFO_IS_NULL(mi_ptr)                                  \
     ? (ps_flow_type *) PS_TX_META_GET_FILTER_RESULT(mi_ptr,                \
                                                  IP_FLTR_CLIENT_QOS_OUTPUT)\
     : NULL)

#define PS_FLOW_GET_FLOW_FROM_RT_META_INFO(rt_mi_ptr)                       \
  (!PS_RT_META_INFO_IS_NULL(rt_mi_ptr)                                      \
     ? (ps_flow_type *) PS_RT_META_GET_FILTER_RESULT(rt_mi_ptr,             \
                                                  IP_FLTR_CLIENT_QOS_OUTPUT)\
     : NULL)

/*===========================================================================
MACRO PS_FLOW_GET_META_INFO_FROM_FLOW()

DESCRIPTION
  Returns the meta-info pointer stored in the logical control block
  of the flow

PARAMETERS
  flow_ptr: Pointer to the flow

RETURN VALUE
  mi_ptr  : Pointer to the meta-info
===========================================================================*/
#define PS_FLOW_GET_META_INFO_FROM_FLOW(flow_ptr)                          \
  (PS_FLOW_IS_VALID(flow_ptr)                                              \
     ? flow_ptr->flow_private.logical_flow.mi_ptr : NULL)

/*===========================================================================
MACRO PS_FLOW_SET_META_INFO_IN_FLOW()

DESCRIPTION
  This macro sets the meta-info pointer in the flow pointer to the
  supplied meta-info pointer. Both the flow and meta-info ptrs are validated.

PARAMETERS
  flow_ptr : pointer to the flow.
  mi_ptr   : pointer to the packet meta info.

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOW_SET_META_INFO_IN_FLOW(flow_ptr,meta_info_ptr)               \
  if(PS_FLOW_IS_VALID(flow_ptr) && !(PS_TX_META_INFO_IS_NULL(meta_info_ptr))) \
  {                                                                         \
    flow_ptr->flow_private.logical_flow.mi_ptr = meta_info_ptr;             \
  }

/*===========================================================================
MACRO PS_FLOW_RESET_META_INFO_IN_FLOW()

DESCRIPTION
  This macro sets the meta-info pointer in the flow pointer to NULL.

PARAMETERS
  flow_ptr : pointer to the flow.

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOW_RESET_META_INFO_IN_FLOW(flow_ptr)               \
  if(PS_FLOW_IS_VALID(flow_ptr))                                \
  {                                                             \
    flow_ptr->flow_private.logical_flow.mi_ptr = NULL;          \
  }


/*===========================================================================
MACRO PS_FLOW_GET_QOS_FIELD_MASK()

DESCRIPTION
  This macro returns the QOS spec field mask.  The bits in the mask defines
  whether QOS is valid in rx/tx direction and which flow blocks are valid.

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  QOS mask         : on success
  QOS_MASK_INVALID : on failure
===========================================================================*/
#define PS_FLOW_GET_QOS_FIELD_MASK(flow_ptr)                             \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr)                             \
     : QOS_MASK_INVALID)

/*===========================================================================
MACRO PS_FLOW_GET_RX_GRANTED_FLOW()

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
#define PS_FLOW_GET_RX_GRANTED_FLOW(ps_flow_ptr)                          \
  ps_flow_get_rx_granted_flow(ps_flow_ptr)

/*===========================================================================
MACRO PS_FLOW_GET_TX_GRANTED_FLOW()

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
#define PS_FLOW_GET_TX_GRANTED_FLOW(ps_flow_ptr)                          \
  ps_flow_get_tx_granted_flow(ps_flow_ptr)

/*===========================================================================
MACRO PS_FLOW_SET_GRANTED_FLOW()

DESCRIPTION
  This macro stores the currently granted flow spec in each direction (if non
  null). If mode handler wants to indicate that QoS is not granted in
  a direction, pass flow type with field mask set to IPFLOW_MASK_NONE

PARAMETERS
  flow_ptr  : pointer to the flow serving the QOS.
  rx_fl_ptr : Ptr to Rx flow type
  tx_fl_ptr : Ptr to Tx flow type

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOW_SET_CURRENT_FLOW  ps_flow_set_granted_flow
#define PS_FLOW_SET_GRANTED_FLOW  ps_flow_set_granted_flow


/*===========================================================================
MACRO PS_FLOW_GET_RX_REQUESTED_FLOW()

DESCRIPTION
  This macro returns the ptr to the requested Rx flow type if available,
  NULL otherwise.

  NOTE: Requested Rx flow must be fetched and accessed in one atomic
        operation as a flow can be deleted at any time, in which case
        requested Rx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  Requested Rx flow spec : on success
  NULL                   : otherwise
===========================================================================*/
#define PS_FLOW_GET_RX_REQUESTED_FLOW(flow_ptr)                          \
  ((PS_FLOW_IS_VALID(flow_ptr) &&                                        \
    (PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) & QOS_MASK_RX_FLOW))          \
       ? (&(flow_ptr)->flow_private.qos_info_ptr->rx.ipflow.req)         \
       : NULL)

/*===========================================================================
MACRO PS_FLOW_GET_RX_MIN_FLOW()

DESCRIPTION
  This macro returns the ptr to the minimum requested Rx flow type if
  available, NULL otherwise.

  NOTE: Requested min Rx flow must be fetched and accessed in one atomic
        operation as a flow can be deleted at any time, in which case
        requested min Rx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  Requested minimum Rx flow spec : on success
  NULL                           : otherwise
===========================================================================*/
#define PS_FLOW_GET_RX_MIN_FLOW(flow_ptr)                                \
  ((PS_FLOW_IS_VALID(flow_ptr) &&                                        \
    (PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) & QOS_MASK_RX_MIN_FLOW))      \
       ? (&(flow_ptr)->flow_private.qos_info_ptr->rx.ipflow.min_req)     \
       : NULL)

/*===========================================================================
MACRO PS_FLOW_GET_TX_REQUESTED_FLOW()

DESCRIPTION
  This macro returns the ptr to the requested Tx flow type if
  available, NULL otherwise.

  NOTE: Requested Tx flow must be fetched and accessed in one atomic
        operation as a flow can be deleted at any time, in which case
        requested Tx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  Requested Tx flow spec : on success
  NULL                   : otherwise
===========================================================================*/
#define PS_FLOW_GET_TX_REQUESTED_FLOW(flow_ptr)                          \
  ((PS_FLOW_IS_VALID(flow_ptr) &&                                        \
    (PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) & QOS_MASK_TX_FLOW))          \
       ? (&(flow_ptr)->flow_private.qos_info_ptr->tx.ipflow.req)         \
       : NULL)

/*===========================================================================
MACRO PS_FLOW_GET_TX_MIN_FLOW()

DESCRIPTION
  This macro returns the ptr to the minimum requested Tx flow type if
  available, NULL otherwise.

  NOTE: Requested min Tx flow must be fetched and accessed in one atomic
        operation as a flow can be deleted at any time, in which case
        requested min Tx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  Requested minimum Tx flow spec : on success
  NULL                           : otherwise
===========================================================================*/
#define PS_FLOW_GET_TX_MIN_FLOW(flow_ptr)                                \
  ((PS_FLOW_IS_VALID(flow_ptr) &&                                        \
    (PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) & QOS_MASK_TX_MIN_FLOW))      \
       ? (&(flow_ptr)->flow_private.qos_info_ptr->tx.ipflow.min_req)     \
       : NULL)

/*===========================================================================
MACRO PS_FLOW_GET_QOS_MODIFY_FIELD_MASK()

DESCRIPTION
  This macro returns the QOS spec field mask of QOS_MODIFY. The bits in the
  mask defines whether QOS is valid in rx/tx direction and which flow blocks
  are valid.

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  A valid QOS mask : if QOS_MODIFY is requested on this flow
  QOS_MASK_INVALID : if QOS_MODIFY is not requested on this flow or if flow
                     doesn't support QOS
===========================================================================*/
#define PS_FLOW_GET_QOS_MODIFY_FIELD_MASK(flow_ptr)                      \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr)                      \
     : QOS_MASK_INVALID)

/*===========================================================================
MACRO PS_FLOW_GET_MODIFY_RX_FLOW()

DESCRIPTION
  This macro returns the ptr to the Rx flow type in QOS_MODIFY if available,
  NULL otherwise.

  NOTE: Modified Rx flow must be fetched and accessed in one atomic
        operation as a flow can be deleted at any time, in which case
        modified Rx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  Modified Rx flow : on success
  NULL             : otherwise
===========================================================================*/
#define PS_FLOW_GET_MODIFY_RX_FLOW(flow_ptr)                             \
  ((PS_FLOW_IS_VALID(flow_ptr) &&                                        \
    (PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) & QOS_MASK_RX_FLOW))   \
       ? (&(flow_ptr)->flow_private.qos_modify_info_ptr->rx.ipflow.req)  \
       : NULL)

/*===========================================================================
MACRO PS_FLOW_GET_MODIFY_RX_MIN_FLOW()

DESCRIPTION
  This macro returns the ptr to the minimum Rx flow type in QOS_MODIFY if
  available, NULL otherwise.

  NOTE: Modified min Rx flow must be fetched and accessed in one atomic
        operation as a flow can be deleted at any time, in which case
        modified min Rx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  Modified minimum Rx flow : on success
  NULL                     : otherwise
===========================================================================*/
#define PS_FLOW_GET_MODIFY_RX_MIN_FLOW(flow_ptr)                            \
  ((PS_FLOW_IS_VALID(flow_ptr) &&                                           \
    (PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) & QOS_MASK_RX_MIN_FLOW))  \
       ? (&(flow_ptr)->flow_private.qos_modify_info_ptr->rx.ipflow.min_req) \
       : NULL)

/*===========================================================================
MACRO PS_FLOW_GET_MODIFY_TX_FLOW()

DESCRIPTION
  This macro returns the ptr to the Tx flow type in QOS_MODIFY if available,
  NULL otherwise.

  NOTE: Modified Tx flow must be fetched and accessed in one atomic
        operation as a flow can be deleted at any time, in which case
        modified Tx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  Modified Tx flow : on success
  NULL             : otherwise
===========================================================================*/
#define PS_FLOW_GET_MODIFY_TX_FLOW(flow_ptr)                             \
  ((PS_FLOW_IS_VALID(flow_ptr) &&                                        \
    (PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) & QOS_MASK_TX_FLOW))   \
       ? (&(flow_ptr)->flow_private.qos_modify_info_ptr->tx.ipflow.req)  \
       : NULL)

/*===========================================================================
MACRO PS_FLOW_GET_MODIFY_TX_MIN_FLOW()

DESCRIPTION
  This macro returns the ptr to the minimum Tx flow type in QOS_MODIFY if
  available, NULL otherwise.

  NOTE: Modified min Tx flow must be fetched and accessed in one atomic
        operation as a flow can be deleted at any time, in which case
        modified min Tx flow is memset to 0

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  Modified minimum Tx flow : on success
  NULL                     : otherwise
===========================================================================*/
#define PS_FLOW_GET_MODIFY_TX_MIN_FLOW(flow_ptr)                            \
  ((PS_FLOW_IS_VALID(flow_ptr) &&                                           \
    (PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) & QOS_MASK_TX_MIN_FLOW))  \
       ? (&(flow_ptr)->flow_private.qos_modify_info_ptr->tx.ipflow.min_req) \
       : NULL)

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_RX_FLOW_SPEC_CREATED()

DESCRIPTION
  This macro checks if Rx flow spec is created in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx flow spec is created
  FALSE : if Rx flow spec is not created or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_RX_FLOW_SPEC_CREATED(flow_ptr)                 \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_CREATED(flow_ptr)                 \
     : FALSE)

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_RX_FLOW_SPEC_MODIFIED()

DESCRIPTION
  This macro checks if Rx flow spec is modified in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx flow spec is modified
  FALSE : if Rx flow spec is not modified or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_RX_FLOW_SPEC_MODIFIED(flow_ptr)                \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_MODIFIED(flow_ptr)                \
     : FALSE)

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_RX_FLOW_SPEC_DELETED()

DESCRIPTION
  This macro checks if Rx flow spec is deleted in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx flow spec is deleted
  FALSE : if Rx flow spec is not deleted or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_RX_FLOW_SPEC_DELETED(flow_ptr)                 \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_DELETED(flow_ptr)                 \
     : FALSE)

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_RX_FLTR_SPEC_CREATED()

DESCRIPTION
  This macro checks if fltr spec in Rx direction is created in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx fltr spec is created
  FALSE : if Rx fltr spec is not created or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_RX_FLTR_SPEC_CREATED                           \
  PS_FLOW_IS_MODIFY_RX_FLOW_SPEC_CREATED

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_RX_FLTR_SPEC_MODIFIED()

DESCRIPTION
  This macro checks if fltr spec in Rx direction is modified in QOS_MODIFY,
  i.e. if existing filter parameters have been changed

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx fltr spec is modified
  FALSE : if Rx fltr spec is not modified or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_RX_FLTR_SPEC_MODIFIED(flow_ptr)                \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_MODIFIED(flow_ptr)                \
     : FALSE)

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_RX_FLTR_SPEC_DELETED()

DESCRIPTION
  This macro checks if fltr spec is deleted in Rx direction in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx fltr spec is deleted
  FALSE : if Rx fltr spec is not deleted or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_RX_FLTR_SPEC_DELETED                           \
  PS_FLOW_IS_MODIFY_RX_FLOW_SPEC_DELETED

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_TX_FLOW_SPEC_CREATED()

DESCRIPTION
  This macro checks if Tx flow spec is created in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx flow spec is created
  FALSE : if Tx flow spec is not created or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_TX_FLOW_SPEC_CREATED(flow_ptr)                 \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_CREATED(flow_ptr)                 \
     : FALSE)

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_TX_FLOW_SPEC_MODIFIED()

DESCRIPTION
  This macro checks if Tx flow spec is modified in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx flow spec is modified
  FALSE : if Tx flow spec is not modified or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_TX_FLOW_SPEC_MODIFIED(flow_ptr)                \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_MODIFIED(flow_ptr)                \
     : FALSE)

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_TX_FLOW_SPEC_DELETED()

DESCRIPTION
  This macro checks if Tx flow spec is deleted in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx flow spec is deleted
  FALSE : if Tx flow spec is not deleted or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_TX_FLOW_SPEC_DELETED(flow_ptr)                 \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_DELETED(flow_ptr)                 \
     : FALSE)

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_TX_FLTR_SPEC_CREATED()

DESCRIPTION
  This macro checks if fltr spec in Rx direction is created in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx fltr spec is created
  FALSE : if Tx fltr spec is not created or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_TX_FLTR_SPEC_CREATED                           \
  PS_FLOW_IS_MODIFY_TX_FLOW_SPEC_CREATED

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_TX_FLTR_SPEC_MODIFIED()

DESCRIPTION
  This macro checks if fltr spec in Tx direction is modified in QOS_MODIFY,
  i.e. if existing filter parameters have been changed

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx fltr spec is modified
  FALSE : if Tx fltr spec is not modified or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_TX_FLTR_SPEC_MODIFIED(flow_ptr)                \
  (PS_FLOW_IS_VALID(flow_ptr)                                            \
     ? PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_MODIFIED(flow_ptr)                \
     : FALSE)

/*===========================================================================
MACRO PS_FLOW_IS_MODIFY_TX_FLTR_SPEC_DELETED()

DESCRIPTION
  This macro checks if fltr spec is deleted in Tx direction in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx fltr spec is deleted
  FALSE : if Tx fltr spec is not deleted or if it is not specified at all
===========================================================================*/
#define PS_FLOW_IS_MODIFY_TX_FLTR_SPEC_DELETED                           \
  PS_FLOW_IS_MODIFY_TX_FLOW_SPEC_DELETED

/*===========================================================================
MACRO PS_FLOW_GET_IFACE()

DESCRIPTION
  This macro returns the iface associated with the given flow

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  iface_ptr : if the given flow is valid
  NULL      : if the given flow is invalid
===========================================================================*/
#define PS_FLOW_GET_IFACE(flow_ptr)                                      \
   (PS_FLOW_IS_VALID(flow_ptr)                                           \
       ? (flow_ptr->flow_private.iface_ptr)                              \
       : NULL)

/*===========================================================================
MACRO PS_FLOW_IS_NW_INITIATED()

DESCRIPTION
This macro checks if the flow was initiated by the network or by a request
from  an application on the UE.

PARAMETERS
flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
TRUE  : if the flow was initiated by the network
FALSE : if the flow was initiated by the UE
===========================================================================*/
#define PS_FLOW_IS_NW_INITIATED(flow_ptr)                                 \
   (PS_FLOW_IS_VALID(flow_ptr)                                            \
       ? flow_ptr->flow_private.is_nw_initiated                           \
       : FALSE)

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

                                COMMANDS

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

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
);

/*===========================================================================
FUNCTION PS_FLOW_CONFIGURE_CMD()

DESCRIPTION
  Configures a flow

PARAMETERS
  flow_ptr : ptr to flow on which to operate on.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
   0 : on success
  -1 : on failure (which includes DS_EWOULDBLOCK)

DEPENDENCIES
  None

SIDE EFFECTS
  Changes flow's state to FLOW_CONFIGURING and posts FLOW_CONFIGURING_EV
===========================================================================*/
int ps_flow_configure_cmd
(
  ps_flow_type  * flow_ptr,
  int16         * ps_errno,
  void          * client_data_ptr
);

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
);

/*===========================================================================
FUNCTION PS_FLOW_SUSPEND_CMD()

DESCRIPTION
  Suspends a flow

PARAMETERS
  flow_ptr : ptr to flow on which to operate on.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
   0 : on success
  -1 : on failure (which includes DS_EWOULDBLOCK)

DEPENDENCIES
  None

SIDE EFFECTS
  Changes flow's state to FLOW_SUSPENDING and posts FLOW_SUSPENDING_EV
===========================================================================*/
int ps_flow_suspend_cmd
(
  ps_flow_type  * flow_ptr,
  int16         * ps_errno,
  void          * client_data_ptr
);

/*===========================================================================
FUNCTION PS_FLOW_GO_NULL_CMD()

DESCRIPTION
  Tears down a flow

PARAMETERS
  flow_ptr : ptr to flow on which to operate on.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
   0 : on success
  -1 : on failure (which includes DS_EWOULDBLOCK)

DEPENDENCIES
  None

SIDE EFFECTS
  Changes flow's state to FLOW_GOING_NULL and posts FLOW_GOING_NULL_EV
===========================================================================*/
int ps_flow_go_null_cmd
(
  ps_flow_type  * flow_ptr,
  int16         * ps_errno,
  void          * client_data_ptr
);



/*===========================================================================

                              INDICATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOW_ACTIVATE_IND()

DESCRIPTION
  Indicates that a flow is activated

PARAMETERS
  flow_ptr  : ptr to flow on which to operate on.
  info_code : extended info code for the indication.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
   None

DEPENDENCIES
  None

SIDE EFFECTS
  Changes flow's state to FLOW_ACTIVATED and posts FLOW_ACTIVATED_EV
===========================================================================*/
void ps_flow_activate_ind
(
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code
);

/*===========================================================================
FUNCTION PS_FLOW_SUSPEND_IND()

DESCRIPTION
  Indicates that a flow is suspended

PARAMETERS
  flow_ptr  : ptr to flow on which to operate on.
  info_code : extended info code for the indication.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
   None

DEPENDENCIES
  None

SIDE EFFECTS
  Changes flow's state to FLOW_SUSPENDED and posts FLOW_SUSPENDED_EV
===========================================================================*/
void ps_flow_suspend_ind
(
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code
);

/*===========================================================================
FUNCTION PS_FLOW_GO_NULL_IND()

DESCRIPTION
  Indicates that a flow is torn down

PARAMETERS
  flow_ptr  : ptr to flow on which to operate on.
  info_code : extended info code for the indication.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
   None

DEPENDENCIES
  None

SIDE EFFECTS
  Changes flow's state to FLOW_NULL and posts FLOW_NULL_EV
===========================================================================*/
void ps_flow_go_null_ind
(
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code
);

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
);



/*===========================================================================

                             UTILITY FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOW_BIND_PHYS_LINK()

DESCRIPTION
  Binds a flow to a phys link

PARAMETERS
  flow_ptr      : pointer to the flow serving QOS
  phys_link_ptr : flow_ptr's binding

RETURN VALUE
   0 : on sucess
  -1 : on failure

DEPENDENCIES
  Flow must not already be bound and flow must not be a default flow

SIDE EFFECTS
  None
===========================================================================*/
int ps_flow_bind_phys_link
(
  ps_flow_type       * flow_ptr,
  ps_phys_link_type  * phys_link_ptr
);

/*===========================================================================
FUNCTION PS_FLOW_UNBIND_PHYS_LINK()

DESCRIPTION
  Unbinds a phys link from ps_flow

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
   0 : on sucess
  -1 : on failure

DEPENDENCIES
  Flow must already be bound and must not be a default flow

SIDE EFFECTS
  None
===========================================================================*/
int ps_flow_unbind_phys_link
(
  ps_flow_type  * flow_ptr
);

/*===========================================================================
FUNCTION PS_FLOW_REBIND_PHYS_LINK()

DESCRIPTION
  Binds a flow to a phys link, which is different from its current binding

PARAMETERS
  flow_ptr      : pointer to the flow serving QOS
  phys_link_ptr : flow_ptr's new binding

RETURN VALUE
   0 : on sucess
  -1 : on failure

DEPENDENCIES
  flow must be bound already and flow must not be a default flow

SIDE EFFECTS
  None
===========================================================================*/
int ps_flow_rebind_phys_link
(
  ps_flow_type       * flow_ptr,
  ps_phys_link_type  * phys_link_ptr
);

/*===========================================================================
FUNCTION PS_FLOW_ENABLE_TX()

DESCRIPTION
  Enables a flow to transmit traffic

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
void ps_flow_enable_tx
(
  ps_flow_type  * flow_ptr,
  uint32          flow_mask
);

/*===========================================================================
FUNCTION PS_FLOW_DISABLE_TX()

DESCRIPTION
  Disables a flow from transmitting traffic

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
void ps_flow_disable_tx
(
  ps_flow_type  * flow_ptr,
  uint32          flow_mask
);

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
);

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
);

/*===========================================================================
FUNCTION PS_FLOW_SET_ASSOC_FLOW()

DESCRIPTION
  This function sets associated ps_flow for a given ps_flow, allocates
  the meta-info, and registers the specified event callback function to
  handle events received from the associated flow. If no event callback
  is supplied, a default handler is used.

PARAMETERS
  flow_ptr          : pointer to the flow serving the QOS.
  assoc_flow_ptr    : associated ps_flow
  ev_cback_func_ptr : event callback function to be called if the specified
                      event is received on the assoc_flow_ptr
  ps_errno          : specific error code in case operation fails

RETURN VALUE
     0 : on success
    -1 : on failure (any of the passed in parameters is bad)

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int32 ps_flow_set_assoc_flow
(
  ps_flow_type                        * flow_ptr,
  ps_flow_type                        * assoc_flow_ptr,
  ps_flow_assoc_flow_event_cback_type   ev_cback_func_ptr,
  int16                               * ps_errno
);

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
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_set_granted_flow
(
  const ps_flow_type  * flow_ptr,
  const ip_flow_type  * rx_ip_flow_ptr,
  const ip_flow_type  * tx_ip_flow_ptr
);

/*===========================================================================
FUNCTION PS_FLOW_GET_RX_FLTR_CNT()

DESCRIPTION
  Returns number of Rx filters, that match the IP version, specified as an
  argument. Rx filters can pertain to either QOS_REQUEST/active QoS or
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
  None

SIDE EFFECTS
  None
===========================================================================*/
int8 ps_flow_get_rx_fltr_cnt
(
  ps_flow_type          * flow_ptr,
  ip_version_enum_type    ip_vsn,
  boolean                 is_modify
);

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
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations

  Sample usage to fetch Rx IPV4 filters pertaining to QOS_REQUEST is
    TAKLOCK()
      handle = ps_flow_get_rx_fltr_handle(flow_ptr, FALSE, IPV4)
      while (handle != NULL)
      {
        ps_flow_get_rx_fltr_by_handle(flow_ptr, FALSE, IPV4, handle,
                                      fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE();

SIDE EFFECTS
  None
===========================================================================*/
void *ps_flow_get_rx_fltr_handle
(
  ps_flow_type          * flow_ptr,
  ip_version_enum_type    ip_vsn,
  boolean                 is_modify
);

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
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations.

  Do not try to access filter contents outside of atomic operation. Since
  a filter can be deleted, this may lead to memory corruption or data aborts.

  ip_vsn must match IP version of filter that rx_fltr_handle points to. So
  if client gets a handle to V4 filter using ps_flow_get_rx_fltr_handle, it
  MUST fetch only V4 filters in ps_flow_get_rx_fltr_by_handle.

  Sample usage to fetch Rx IPV4 filters pertaining to QOS_REQUEST is
    TAKLOCK()
      handle = ps_flow_get_rx_fltr_handle(flow_ptr, FALSE, IPV4)
      while (handle != NULL)
      {
        ps_flow_get_rx_fltr_by_handle(flow_ptr, FALSE, IPV4, handle,
                                      fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE();

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
);

/*===========================================================================
FUNCTION PS_FLOW_GET_RX_FLTR_SPEC()

DESCRIPTION
  Given a flow ptr, this function returns all the Rx filters
  associated with a flow.

PARAMETERS
  iface_ptr      : pointer to iface
  flow_ptr       : pointer to the flow
  ip_fltr_spec   : OUT PARAM, pointer to Rx filter spec
  is_modify      : is the client interested in fetching Rx filters associated
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
boolean ps_flow_get_rx_fltr_spec
(
  ps_iface_type          * iface_ptr,
  ps_flow_type           * flow_ptr,
  ip_filter_spec_type    * ip_fltr_spec,
  boolean                  is_modify,
  uint8                    num_fltrs,
  int16                  * ps_errno
);

/*===========================================================================
FUNCTION PS_FLOW_SET_RX_FILTER_SPEC()

DESCRIPTION
  Given the ip filter spec, this function sets the filter id and precedence
  of all the Rx filters.

PARAMETERS
  iface_ptr      : pointer to iface
  flow_ptr       : pointer to the flow
  ip_fltr_spec   : OUT PARAM, pointer to Rx filter spec
  is_modify      : is the client interested in fetching Rx filters associated
                   with pending QOS_MODIFY? If so value must be TRUE

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
  boolean                 is_modify
);

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
  None

SIDE EFFECTS
  None
===========================================================================*/
list_size_type ps_flow_get_aux_flow_spec_cnt
(
  ps_flow_type                   * flow_ptr,
  qos_spec_field_mask_enum_type    flow_type,
  boolean                          is_modify
);

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
);

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
  All auxiliary flow specs must be fetched in one atomic operation.
  Otherwise they could be deleted (for example, because flow specs are
  modified using QOS_MODIFY or because QOS_MODIFY is accepted/rejected) while
  client is fetching them which could lead to incorrect operations

  Sample usage to fetch Tx auxiliary flow specs related to active QoS is
    TASKLOCK()
      handle =
        ps_flow_get_aux_flow_spec_handle(flow_ptr, QOS_MASK_TX_FLOW, FALSE)
      while (handle != NULL)
      {
        ps_flow_get_aux_flow_spec_by_handle(flow_ptr, QOS_MASK_TX_FLOW, FALSE,
                                            handle, aux_flow_spec, new_handle)
        handle = new_handle
      }
    TASKFREE();

SIDE EFFECTS
  None
===========================================================================*/
void *ps_flow_get_aux_flow_spec_handle
(
  ps_flow_type                   * flow_ptr,
  qos_spec_field_mask_enum_type    flow_type,
  boolean                          is_modify
);

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
  All auxiliary flow specs must be fetched in one atomic operation.
  Otherwise they could be deleted (for example, because flow specs are
  modified using QOS_MODIFY or because QOS_MODIFY is accepted/rejected) while
  client is fetching them which could lead to incorrect operations

  Do not try to access flow spec contents outside of atomic operation. Since
  it can be deleted, this may lead to memory corruption or data aborts.

  Sample usage to fetch Tx auxiliary flow specs related to active QoS is
    TAKLOCK()
      handle =
        ps_flow_get_aux_flow_spec_handle(flow_ptr, QOS_MASK_TX_FLOW, FALSE)
      while (handle != NULL)
      {
        ps_flow_get_aux_flow_spec_by_handle(flow_ptr, QOS_MASK_TX_FLOW, FALSE,
                                            handle, aux_flow_spec, new_handle)
        handle = new_handle
      }
    TASKFREE();

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
);

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
);

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
);

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
);

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
);

#ifdef __cplusplus
}
#endif

#endif /* PS_FLOW_H */

