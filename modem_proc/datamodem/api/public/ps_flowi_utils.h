#ifndef PS_FLOWI_UTILS_H
#define PS_FLOWI_UTILS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ F L O W I _ U T I L S . H


GENERAL DESCRIPTION
  This file defines internal macros which can be used to manipulate
  a ps_flow. As validation is not performed what so ever, all input parameters
  are assumed to be valid

EXTERNAL FUNCTIONS
  None

Copyright (c) 2005-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_flowi_utils.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
09/29/10    guru   Converting ps_flowi_is_tx_enabled to a function  
05/14/10    guru   Using ALL_FLOWS_ENABLED to remove dependency on 
                   ds_flow_control.h 
12/28/08    pp     Common Modem Interface: Public/Private API split.
09/01/07    ssh    QoS support for logical ifaces
6/28/07    msr/ssh QoS supportfor M.IP CCoA iface (currently featurized)
05/01/06    msr    Added support for DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2
02/15/06    msr    Aliased PS_FLOW_SET_CURRENT_FLOW() to
                   PS_FLOW_SET_GRANTED_FLOW()
01/09/06    msr    Conforming to new QOS_MODIFY API
08/15/05    mct    Added qos configure support.
08/02/05    msr    Added new macros to fetch Rx/Tx fltr handles and to
                   support QOS_MODIFY
05/02/05    msr    Replaced PS_FLOWI_[GS]ET_HANDLE with PS_FLOWI_[GS]ET_COOKIE
                   and wrapped macros in parenthesis
04/17/05    msr    Created file.
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                            MACRO DEFINITIONS

===========================================================================*/
/*===========================================================================
MACRO PS_FLOWI_GET_STATE()

DESCRIPTION
  Returns a flow's current state

PARAMETERS
  flow_ptr : pointer to the flow

RETURN VALUE
  flow's state
===========================================================================*/
#define PS_FLOWI_GET_STATE(flow_ptr)  ((flow_ptr)->flow_private.state)

/*===========================================================================
MACRO PS_FLOWI_SET_STATE()

DESCRIPTION
  Sets a flow's state

PARAMETERS
  flow_ptr  : pointer to the flow
  new_state : flow's new state

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOWI_SET_STATE(flow_ptr, new_state)                          \
  ((flow_ptr)->flow_private.state = new_state)

/*===========================================================================
MACRO PS_FLOWI_SET_CAPABILITY()

DESCRIPTION
  Sets a capability of a flow.

  NOTE : Only one capability can be set each time.

PARAMETERS
  flow_ptr   : pointer to flow
  capability : capability which needs to be set

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOWI_SET_CAPABILITY(flow_ptr, capability)                    \
  ((flow_ptr)->flow_private.capability_mask |= (0x1 << (uint8)(capability)))

/*===========================================================================
MACRO PS_FLOWI_RESET_CAPABILITY()

DESCRIPTION
  Resets a capability of a flow.

  NOTE : Only one capability can be reset each time.

PARAMETERS
  flow_ptr   : pointer to flow
  capability : capability which needs to be reset

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOWI_RESET_CAPABILITY(flow_ptr, capability)                  \
  ((flow_ptr)->flow_private.capability_mask &= ~(0x1 << (uint8)(capability)))

/*===========================================================================
MACRO PS_FLOWI_GET_CAPABILITY()

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
#define PS_FLOWI_GET_CAPABILITY(flow_ptr, capability)                    \
  ((flow_ptr)->flow_private.capability_mask & (0x1 << (uint8)(capability)))

/*===========================================================================
MACRO PS_FLOWI_GET_ASSOC_PS_FLOW()

DESCRIPTION
  Returns a flow's current associated ps_flow ptr

PARAMETERS
  flow_ptr : pointer to the flow

RETURN VALUE
  an associated ps_flow_ptr
===========================================================================*/
#define PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr)                             \
  ((flow_ptr)->flow_private.logical_flow.assoc_flow_ptr)

/*===========================================================================
MACRO PS_FLOWI_GET_PHYS_LINK()

DESCRIPTION
  Returns a flow's current binding, which could be NULL if flow is not bound
  yet

PARAMETERS
  flow_ptr : pointer to the flow

RETURN VALUE
  phys_link_ptr, a flow is bound to
===========================================================================*/
#define PS_FLOWI_GET_PHYS_LINK  ps_flowi_get_phys_link
ps_phys_link_type * ps_flowi_get_phys_link( ps_flow_type * flow_ptr );

/*===========================================================================
FUNCTION PS_FLOWI_IS_TX_ENABLED()

DESCRIPTION
  Checks if flow can transmit traffic

PARAMETERS
  flow_ptr : ptr to flow

RETURN VALUE
  TRUE  : if flow is enabled for Tx traffic
  FALSE : otherwise
===========================================================================*/
#define PS_FLOWI_IS_TX_ENABLED ps_flowi_is_tx_enabled
boolean ps_flowi_is_tx_enabled
(
  ps_flow_type * flow_ptr 
);

/*===========================================================================
MACRO PS_FLOWI_GET_TX_MASK()

DESCRIPTION
  Returns the flow mask of the passed in flow.

PARAMETERS
  flow_ptr : ptr to flow

RETURN VALUE
  flow mask
===========================================================================*/
#define PS_FLOWI_GET_TX_MASK(flow_ptr)                                 \
  ((flow_ptr)->flow_private.tx_flow_mask)

/*===========================================================================
MACRO PS_FLOWI_GET_COOKIE()

DESCRIPTION
  Gets cookie which uniquely identifies this flow across its incarnations

PARAMETERS
  flow_ptr : ptr to flow

RETURN VALUE
  a cookie uniquely identifying this flow across its incarnations
===========================================================================*/
#define PS_FLOWI_GET_COOKIE(flow_ptr)  ((flow_ptr)->flow_private.cookie)

/*===========================================================================
MACRO PS_FLOWI_SET_COOKIE()

DESCRIPTION
  Sets cookie for this flow. A cookie uniquely identifies a flow across its
  incarnations

PARAMETERS
  flow_ptr : ptr to flow
  cookie   : a cookie uniquely identifying this flow across its incarnations

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOWI_SET_COOKIE(flow_ptr, flow_cookie)                       \
  ((flow_ptr)->flow_private.cookie = flow_cookie)

/*===========================================================================
MACRO PS_FLOWI_GET_QOS_FIELD_MASK()

DESCRIPTION
  This macro returns the QOS spec field mask.  The bits in the mask defines
  whether QOS is valid in rx/tx direction and which flow blocks are valid.

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  A valid QOS mask : if QOS is requested on this flow
  QOS_MASK_INVALID : if QOS is not requested on this flow or if flow doesn't
                     support QOS
===========================================================================*/
#define PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr)                            \
  (((flow_ptr)->flow_private.qos_info_ptr != NULL)                       \
      ? (flow_ptr)->flow_private.qos_info_ptr->field_mask                \
      : QOS_MASK_INVALID)

/*===========================================================================
MACRO PS_FLOWI_GET_TX_FLTR_HANDLE()

DESCRIPTION
  This macro returns Tx fltr handle for filters associated with current QOS

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  A valid handle : if QOS is requested on this flow
  0              : otherwise
===========================================================================*/
#define PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr)                            \
  (((flow_ptr)->flow_private.qos_info_ptr != NULL)                       \
      ? (flow_ptr)->flow_private.qos_info_ptr->tx.fltr_handle            \
      : 0)

/*===========================================================================
MACRO PS_FLOWI_SET_TX_FLTR_HANDLE()

DESCRIPTION
  This macro sets Tx fltr handle for filters associated with current QOS

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOWI_SET_TX_FLTR_HANDLE(flow_ptr, tx_fltr_handle)            \
  if ((flow_ptr)->flow_private.qos_info_ptr != NULL)                     \
  {                                                                      \
    (flow_ptr)->flow_private.qos_info_ptr->tx.fltr_handle =              \
      tx_fltr_handle;                                                    \
  }

/*===========================================================================
MACRO PS_FLOWI_GET_RX_FLTR_HANDLE()

DESCRIPTION
  This macro returns Rx fltr handle for filters associated with current QOS

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  A valid handle : if QOS is requested on this flow
  0              : otherwise
===========================================================================*/
#define PS_FLOWI_GET_RX_FLTR_HANDLE(flow_ptr)                            \
  (((flow_ptr)->flow_private.qos_info_ptr != NULL)                       \
      ? (flow_ptr)->flow_private.qos_info_ptr->rx.fltr_handle            \
      : 0)

/*===========================================================================
MACRO PS_FLOWI_SET_RX_FLTR_HANDLE()

DESCRIPTION
  This macro sets Rx fltr handle for filters associated with current QOS

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOWI_SET_RX_FLTR_HANDLE(flow_ptr, rx_fltr_handle)            \
  if ((flow_ptr)->flow_private.qos_info_ptr != NULL)                     \
  {                                                                      \
    (flow_ptr)->flow_private.qos_info_ptr->rx.fltr_handle =              \
      rx_fltr_handle;                                                    \
  }

/*===========================================================================
MACRO PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS()

DESCRIPTION
  This macro checks if QPS_MODIFY procedure is pending on a ps_flow.

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if QOS_MODIFY is pending
  FALSE : otherwise
===========================================================================*/
#define PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr)                     \
  ((flow_ptr)->flow_private.qos_modify_info_ptr != NULL)

/*===========================================================================
MACRO PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK()

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
#define PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr)                     \
  (PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr)                          \
     ? (flow_ptr)->flow_private.qos_modify_info_ptr->field_mask          \
     : QOS_MASK_INVALID)

/*===========================================================================
MACRO PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE()

DESCRIPTION
  This macro returns Tx fltr handle for filters specified in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  A valid handle : if QOS_MODIFY is requested on this flow
  NULL           : otherwise
===========================================================================*/
#define PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr)                     \
  (PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr)                          \
     ? (flow_ptr)->flow_private.qos_modify_info_ptr->tx.fltr_handle      \
     : 0)

/*===========================================================================
MACRO PS_FLOWI_SET_MODIFY_TX_FLTR_HANDLE()

DESCRIPTION
  This macro sets Tx fltr handle for filters specified in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOWI_SET_MODIFY_TX_FLTR_HANDLE(flow_ptr, tx_fltr_handle)     \
  if (PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))                      \
  {                                                                      \
    (flow_ptr)->flow_private.qos_modify_info_ptr->tx.fltr_handle =       \
      tx_fltr_handle;                                                    \
  }

/*===========================================================================
MACRO PS_FLOWI_GET_MODIFY_RX_FLTR_HANDLE()

DESCRIPTION
  This macro returns Rx fltr handle for filters specified in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  A valid handle : if QOS_MODIFY is requested on this flow
  NULL           : otherwise
===========================================================================*/
#define PS_FLOWI_GET_MODIFY_RX_FLTR_HANDLE(flow_ptr)                     \
  (PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr)                          \
     ? (flow_ptr)->flow_private.qos_modify_info_ptr->rx.fltr_handle      \
     : 0)

/*===========================================================================
MACRO PS_FLOWI_SET_MODIFY_RX_FLTR_HANDLE()

DESCRIPTION
  This macro sets Rx fltr handle for filters specified in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  None
===========================================================================*/
#define PS_FLOWI_SET_MODIFY_RX_FLTR_HANDLE(flow_ptr, rx_fltr_handle)     \
  if (PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))                      \
  {                                                                      \
    (flow_ptr)->flow_private.qos_modify_info_ptr->rx.fltr_handle =       \
      rx_fltr_handle;                                                    \
  }

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_CREATED()

DESCRIPTION
  This macro checks if Tx flow spec is created in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx flow spec is created
  FALSE : if Tx flow spec is not created or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_CREATED(flow_ptr)                \
  ((PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) & QOS_MASK_TX_FLOW) &&  \
   !(PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) & QOS_MASK_TX_FLOW))

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_MODIFIED()

DESCRIPTION
  This macro checks if Tx flow spec is modified in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx flow spec is modified
  FALSE : if Tx flow spec is not modified or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_MODIFIED(flow_ptr)                   \
  ((PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) & QOS_MASK_TX_FLOW) &&      \
   (PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) & QOS_MASK_TX_FLOW) &&             \
   (flow_ptr)->flow_private.qos_modify_info_ptr->tx.ipflow.req.field_mask != \
      IPFLOW_MASK_NONE)

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_DELETED()

DESCRIPTION
  This macro checks if Tx flow spec is deleted in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx flow spec is deleted
  FALSE : if Tx flow spec is not deleted or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_DELETED(flow_ptr)                    \
  ((PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) & QOS_MASK_TX_FLOW) &&      \
   (flow_ptr)->flow_private.qos_modify_info_ptr->tx.ipflow.req.field_mask == \
      IPFLOW_MASK_NONE)

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_CREATED()

DESCRIPTION
  This macro checks if fltr spec in Tx direction is created in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx fltr spec is created
  FALSE : if Tx fltr spec is not created or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_CREATED                          \
  PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_CREATED

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_MODIFIED()

DESCRIPTION
  This macro checks if fltr spec in Tx direction is modified in QOS_MODIFY,
  i.e. if existing filter parameters have been changed

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx fltr spec is modified
  FALSE : if Tx fltr spec is not modified or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_MODIFIED(flow_ptr)               \
  (PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) &                        \
     QOS_MODIFY_MASK_TX_FLTR_MODIFY)

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_DELETED()

DESCRIPTION
  This macro checks if fltr spec is deleted in Tx direction in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Tx fltr spec is deleted
  FALSE : if Tx fltr spec is not deleted or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_DELETED                          \
  PS_FLOWI_IS_MODIFY_TX_FLOW_SPEC_DELETED

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_CREATED()

DESCRIPTION
  This macro checks if Rx flow spec is created in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx flow spec is created
  FALSE : if Rx flow spec is not created or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_CREATED(flow_ptr)                \
  ((PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) & QOS_MASK_RX_FLOW) &&  \
   !(PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) & QOS_MASK_RX_FLOW))

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_MODIFIED()

DESCRIPTION
  This macro checks if Rx flow spec is modified in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx flow spec is modified
  FALSE : if Rx flow spec is not modified or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_MODIFIED(flow_ptr)                   \
  ((PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) & QOS_MASK_RX_FLOW) &&      \
   (PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr) & QOS_MASK_RX_FLOW) &&             \
   (flow_ptr)->flow_private.qos_modify_info_ptr->rx.ipflow.req.field_mask != \
      IPFLOW_MASK_NONE)

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_DELETED()

DESCRIPTION
  This macro checks if Rx flow spec is deleted in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx flow spec is deleted
  FALSE : if Rx flow spec is not deleted or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_DELETED(flow_ptr)                    \
  ((PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) & QOS_MASK_RX_FLOW) &&      \
   (flow_ptr)->flow_private.qos_modify_info_ptr->rx.ipflow.req.field_mask == \
      IPFLOW_MASK_NONE)

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_CREATED()

DESCRIPTION
  This macro checks if fltr spec in Rx direction is created in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx fltr spec is created
  FALSE : if Rx fltr spec is not created or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_CREATED                          \
  PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_CREATED

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_MODIFIED()

DESCRIPTION
  This macro checks if fltr spec in Rx direction is modified in QOS_MODIFY,
  i.e. if existing filter parameters have been changed

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx fltr spec is modified
  FALSE : if Rx fltr spec is not modified or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_MODIFIED(flow_ptr)               \
  (PS_FLOWI_GET_QOS_MODIFY_FIELD_MASK(flow_ptr) &                        \
     QOS_MODIFY_MASK_RX_FLTR_MODIFY)

/*===========================================================================
MACRO PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_DELETED()

DESCRIPTION
  This macro checks if fltr spec is deleted in Rx direction in QOS_MODIFY

PARAMETERS
  flow_ptr : pointer to the flow serving the QOS.

RETURN VALUE
  TRUE  : if Rx fltr spec is deleted
  FALSE : if Rx fltr spec is not deleted or if it is not specified at all
===========================================================================*/
#define PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_DELETED                          \
  PS_FLOWI_IS_MODIFY_RX_FLOW_SPEC_DELETED

/*===========================================================================
MACRO PS_FLOWI_SET_CB_F_PTR()

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
#define PS_FLOWI_SET_CB_F_PTR(flow_ptr, activate_cmd_f_ptr,              \
                              configure_cmd_f_ptr, suspend_cmd_f_ptr,    \
                              go_null_cmd_f_ptr, ioctl_cb_f_ptr)         \
  (flow_ptr)->ps_flow_activate_cmd_f_ptr = activate_cmd_f_ptr;           \
  (flow_ptr)->ps_flow_configure_cmd_f_ptr = configure_cmd_f_ptr;         \
  (flow_ptr)->ps_flow_suspend_cmd_f_ptr  = suspend_cmd_f_ptr;            \
  (flow_ptr)->ps_flow_go_null_cmd_f_ptr  = go_null_cmd_f_ptr;            \
  (flow_ptr)->ps_flow_ioctl_f_ptr        = ioctl_cb_f_ptr;

#ifdef __cplusplus
}
#endif

#endif /* PS_FLOWI_UTILS_H */
