#ifndef PS_IFACE_RX_QOS_FLTR_H
#define PS_IFACE_RX_QOS_FLTR_H

/*===========================================================================

                  P S _ I F A C E _ R X _ Q O S _ F L T R . H

DESCRIPTION
  Header file for declaration of functions to manipulate Rx QOS filters

EXTERNALIZED FUNCTIONS
  PS_IFACEI_RX_QOS_FLTR_ADD()
    Adds pointers to Rx filters to the iface. Rx filters are actually
    stored in ps_flow

  PS_IFACEI_RX_QOS_FLTR_MODIFY()
    Adds pointers to Rx filters to the iface. Rx filters are actually
    stored in ps_flow. This function is called when existing Rx fltrs are
    modified in QOS_MODIFY

  PS_IFACEI_RX_QOS_FLTR_DELETE()
    Deletes pointers to Rx filters from the iface. Rx filters are actually
    stored in ps_flow

Copyright (c) 2005-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface_ipfltr.h_v   1.0   07 Feb 2003 20:14:34   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_iface_rx_qos_fltr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/14/08    pp     Common Modem Interface: Public/Private API split.
08/03/05    msr    Added ps_ifacei_rx_qos_fltr_modify().
05/03/05    msr    Using int16 instead of errno_enum_type.
04/17/05    msr    created file
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_QOS
#include "list.h"
#include "ps_iface_defs.h"
#include "ps_flowi.h"

/*===========================================================================

                          REGIONAL DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
TYPEDEF PS_IFACE_RX_QOS_FLTR_NODE_TYPE

DESCRIPTION
  This is the type that is used to enqueue pointers to Rx QOS filters in a
  ps_iface.
===========================================================================*/
typedef struct
{
  list_link_type                      link;
  ps_flow_rx_fltr_buf_type          * fltr_buf_ptr;
  ps_iface_rx_qos_fltr_handle_type    fltr_handle;
} ps_iface_rx_qos_fltr_node_type;



/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_IFACEI_RX_QOS_FLTR_ADD()

DESCRIPTION
  This function adds a set of Rx QOS filters to the specified iface. The
  filter set is tied to a filter id, which uniquely idenifies a filter
  for the given iface for the given QOS client.

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
);

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
);

/*===========================================================================
FUNCTION PS_IFACEI_RX_QOS_FLTR_DELETE()

DESCRIPTION
  This function deletes all the existing Rx QOS filters ientified by the
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
);

#endif /* FEATURE_DATA_PS_QOS */
#endif /* PS_IFACE_RX_QOS_FLTR_H */
