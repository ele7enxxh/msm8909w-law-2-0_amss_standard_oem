#ifndef DS707_PKT_MGR_IOCTL_H
#define DS707_PKT_MGR_IOCTL_H

/*===========================================================================

                        D S 7 0 7 _ P K T _ M G R _ I O C T L . H
GENERAL DESCRIPTION
  Header file for ioctls specific to 1x.

EXTERNALIZED FUNCTIONS
  ds707_pkt_ioctl()
    Handles general ioctls on 1x iface.

  ds707_pkt_phys_link_ioctl()
    Handles ioctls related to phys links.

  ds707_pkt_ps_flow_ioctl()
    Handles ioctls related to ps flows.

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2004 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_mgr_ioctl.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/26/14    ms     Data+MMS - DSDA changes Phase 1.
07/26/05    vas    Unfeaturized ps_flow_ioctl processing function
05/25/05    atp    Added support for semi-dynamic QOS on DO.
05/23/05    spn    Changed signature of ds707_pkt_ioctl
03/29/05    gr     Moved the GO_ACTIVE and GO_DORMANT ioctls to phys link
                   ioctl handler since these are operations on phys link
09/07/04    sv     Removed DSS_IFACE_IOCTL dependency.
01/05/04    ak     First revision.  Copied IOCTL over to this file.

===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#include "ps_iface.h"
#include "dss_iface_ioctl.h"
#include "ps_sys_ioctl.h"



/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/

/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_IOCTL

DESCRIPTION   Called by ps_iface to do ioctls specific to 1x pkt iface.

DEPENDENCIES  Registered once, at startup.

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_ioctl
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SYS_IOCTL_INIT

DESCRIPTION   Called by ds707_pkt_mgr_init to register the sys ioctl cb

DEPENDENCIES  Registered once, at startup.

RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_sys_ioctl_init
(
  void
);

/*===========================================================================
FUNCTION      DS707_SYS_IOCTL

DESCRIPTION   Called by ps_iface to perform sys specific ioctls

DEPENDENCIES  Registered once, at startup.

RETURN VALUE  0 - on Success 
             -1 - on Failure 

SIDE EFFECTS  None.
===========================================================================*/
int16 ds707_sys_ioctl
(
  ps_sys_ioctl_enum_type   ioctl_name,
  void                     *argval_ptr,
  ps_sys_subscription_enum_type       subs_id,
  int16                    *ps_errno
);

/*===========================================================================
FUNCTION      DS707_PKT_PHYS_LINK_IOCTL

DESCRIPTION   Does the processing for the physlink specific ioctls

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
int ds707_pkt_phys_link_ioctl
(
  ps_phys_link_type        *phys_link_ptr,
  ps_phys_link_ioctl_type  ioctl_name,
  void                     *argval_ptr,
  int16                    *ps_errno
);


/*===========================================================================
FUNCTION      DS707_PKT_PS_FLOW_IOCTL

DESCRIPTION   Does the processing for the psflow specific ioctls

DEPENDENCIES  None.

RETURN VALUE   0 - on success
              -1 - on failure

SIDE EFFECTS  None
===========================================================================*/
int ds707_pkt_ps_flow_ioctl
(
  ps_flow_type         * ps_flow_ptr,
  ps_flow_ioctl_type     ioctl_name,
  void                 * argval_ptr,
  int16                * ps_errno
);

//fjia: TODO: try to remove it
#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_NIQ_EHRPD
/*===========================================================================
FUNCTION      DS707_PKT_MGR_FLOW_IOCTL_DELETE_FLOWS

DESCRIPTION   This function deletes all the flows

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_flow_ioctl_delete_flows
(
  ps_flow_type  ** ps_flow, 
  uint8            num_of_specs,
  ps_iface_type   *iface_ptr
);
#endif /*FEATURE_NIQ_EHRPD*/
#endif /*FEATURE_HDR_QOS*/
#endif /* FEATURE_DATA_IS707    */
#endif /* DS707_PKT_MGR_IOCTL_H */

