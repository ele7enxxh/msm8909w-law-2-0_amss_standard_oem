#ifndef DS_WLAN_PROXY_REV_IP_XPORT_IF_H
#define DS_WLAN_PROXY_REV_IP_XPORT_IF_H
/*===========================================================================

                      DS_WLAN_PROXY_MODE_HDLR.H

DESCRIPTION


EXTERNALIZED FUNCTIONS

 Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/wlan/wlan_proxy/inc/ds_wlan_proxy_rev_ip_xport_if.h#1 $
  $Author: mplcsds1 $ 

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/11/14    op     Changes to support RM resync state
11/07/12    SC     Created Module
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#ifdef FEATURE_DATA_WLAN_MAPCON 

#include "ps_iface_defs.h"
#include "ps_iface_ioctl.h"
#include "ds_rev_ip_transport_hdlr.h"

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_REV_IP_XPORT_BRINGUP

DESCRIPTION   Given an iface ptr this function calls reverse sio bring up. 

PARAMETERS    ps_iface_type * : pointer to iface

RETURN VALUE  boolean. 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_wlan_proxy_rev_ip_xport_bringup
(
  ps_iface_type        *iface_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_REV_IP_XPORT_TEARDOWN

DESCRIPTION   Given an iface ptr this function calls reverse sio teardown. 

PARAMETERS    ps_iface_type * : pointer to iface

RETURN VALUE  boolean. 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_wlan_proxy_rev_ip_xport_teardown
(
  ps_iface_type        *iface_ptr
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_REV_IP_XPORT_IF_PROCESS_CONFIG_STATUS_IOCTL

DESCRIPTION   This function is called to process reverse SIO configuration 
              complete / failure status provided by QMI. 

PARAMETERS    ps_iface_type * : pointer to iface
              boolean : status of SIO configuration

RETURN VALUE  None. 

SIDE EFFECTS  None.
===========================================================================*/
void ds_wlan_proxy_rev_ip_xport_if_process_wds_config_status_ioctl
(
  ps_iface_type                                           *iface_ptr,
  ps_iface_ioctl_reverse_ip_transport_config_status_type   config_status_info
);
/*===========================================================================
FUNCTION      DS_WLAN_PROXY_REV_IP_XPORT_BRINGUP_CB

DESCRIPTION   This callback will be called by rev ip xport module to notify 
              status of bring up of xport 

PARAMETERS    ps_iface_type * : pointer to iface

RETURN VALUE  None. 

SIDE EFFECTS  None.
===========================================================================*/
void ds_wlan_proxy_rev_ip_xport_bringup_cb
(
  ps_iface_type * iface_ptr,
  boolean         result
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_REV_IP_XPORT_TEARDOWN_CB

DESCRIPTION   This callback will be called by rev ip xport module to notify 
              status of tear down of xport 

PARAMETERS    ps_iface_type * : pointer to iface

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_wlan_proxy_rev_ip_xport_teardown_cb
(
  ps_iface_type * iface_ptr,
  boolean         result
);

#endif /*FEATURE_DATA_WLAN_MAPCON*/

#endif /*DS_WLAN_PROXY_REV_IP_XPORT_IF_H*/
