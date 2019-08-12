#ifndef DS_707_NETMODEL_H
#define DS_707_NETMODEL_H
/*===========================================================================

                      D S _  7 0 7 _ N E T M O D E L . H

DESCRIPTION

 The PPP Network model header file. Contains shared variables and enums, as
 well as function prototypes.

Copyright (c) 1998-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_netmodel.h_v   1.3   15 Nov 2002 23:37:30   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_netmodel.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/02/11    ms     Send Term-Ack if a Term-Req is recieved from PC while cleaning PPP
                   on Rm.
03/26/03    mvl    removed authentication featurization
11/14/02   jd/usb  Added protoype for netmdl_set_resync_behaviour() and
                   netmdl_drop_behav_enum_type for parameter
11/04/02    mvl    Moved some #defines that were only used in .c to the .c.
08/13/02    mvl    Removed the iface_cb_array (all the other items are in the
                   PPP cb) moved the set/get_mode() functions to pppi.h.
07/25/02    mvl    Moved input params to ppp control block.  Updates for PPP
                   renaming. 
07/11/02    mvl    Updated for new PPP architecture.
05/23/02    mvl    Renamed iface_stack_type.
05/20/02     vr    Removed hdlc_enabled flag from iface_cb_type
02/07/02    vsk    Added ip_pkt_hdlr_f_ptr IP packet handler callback
                   to iface_cb_type.
11/12/01    dwp    Added hdlc_enabled to iface_cb_type.
07/24/01    sjy    Added prototypes for netmdl_compare_lcp_opts() and 
                   netmdl_compare_ipcp_opts().
07/22/01    mvl    removed iface_stack_enum_type and accompanying macro as
                   they have been moved to dsiface_def.h
06/21/01    mvl    Added prototypes for netmdl_get_meta_state() and
                   netmdl_reset_ppp()
05/18/01    mvl    Added prototype for netmdl_get_mode().
04/24/00    snn    Added support for the CHAP resync.
10/27/99    mvl    Code review changes and clean up.
08/20/99    mvl    Changed OUT_IFACE() macro so compiler doesn't throw
                   warnings.
08/10/99    snn    added support for DS_PAP feature in network model.
04/12/99    mvl    moved the OUT_IFACE() macro from the C file to this file.
10/04/98    mvl    created file
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "ps_ppp_defs.h"
#include "dsm.h"
#include "ps_ppp_netmodel.h"


/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================
  FUNCTION DS707_NETMDL_COMPARE_LCP_OPTS()

  DESCRIPTION
    Compares the LCP options on the Rm and Um.

  PARAMETERS
    None

  RETURN VALUE
    TRUE - Rm and Um LCP options match
    FALSE - Rm and Um LCP options differ

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds707_netmdl_compare_lcp_opts
(
  void
);

/*===========================================================================
  FUNCTION DS707_NETMDL_COMPARE_ICPC_OPTS()

  DESCRIPTION
    Compares the ICPC options on the Rm and Um.

  PARAMETERS
    TRUE - Rm and Um ICPC options match
    FALSE - Rm and Um ICPC options differ

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds707_netmdl_compare_ipcp_opts
(
  void
);

#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
  FUNCTION DS707_NETMDL_SET_RESYNC_BEHAVIOUR

  DESCRIPTION
    Accessor function to force resync failure to drop call

  PARAMETERS
    NETMDL_RESYNC_DLFT      - do the default behaviour
    NETMDL_RESYNC_DROP_CALL - force call to drop on resync failure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_netmdl_set_resync_behaviour
(
  netmdl_drop_behav_enum_type behav
);

/*===========================================================================
FUNCTION   DS707_NETMDL_REPORT_PPP_CLOSE_DIAG_EVENTS()

DESCRIPTION
  This function posts diag events corresponding to PPP close.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds707_netmdl_report_ppp_close_diag_events
(
  void
);

/*===========================================================================
FUNCTION   DS707_NETMDL_RESET_RESYNC_CALLBACKS()

DESCRIPTION
  This function does the processing required on end of PPP call, like
  enalbing flows, resetting PPP callbacks etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_netmdl_reset_resync_callbacks
(
  void
);

/*===========================================================================
FUNCTION   DS707_NETMDL_REG_PPP_INIT_CALLBACKS()

DESCRIPTION
  This function does the processing required during init of PPP setup for
  netmdl callbacks like registering lcp, ipcp callbacks etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_netmdl_reg_ppp_init_callbacks
(
  void
);

/*===========================================================================
FUNCTION   DS707_NETMDL_REG_PPP_UP_CALLBACKS()

DESCRIPTION
  This function does the processing required after PPP is UP, like
  resync, lcp callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_netmdl_reg_ppp_up_callbacks
(
  void
);

/*===========================================================================
FUNCTION   DS707_NETMDL_REG_PPP_RESYNC_CALLBACKS()

DESCRIPTION
  This function does the processing required during init of PPP setup for
  netmdl callbacks like registering lcp, ipcp callbacks etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_netmdl_reg_ppp_resync_callbacks
(
  void
);

/*===========================================================================
FUNCTION   DS707_NETMDL_TERMINATE_RM_PPP()

DESCRIPTION
  This function is used by netmodel clients to terminate Rm PPP (once
  Um PPP has been terminated.) If there is an outstanding LCP Term-Ack that 
  needs to be sent on Rm, it sends it out and aborts PPP. Else, it invokes 
  ppp_stop() to terminate Rm PPP by sending a Term-Req. This is called 
  only during LCP Phase.

DEPENDENCIES
  None

RETURN VALUE
  0: Success
 -1: Failure

SIDE EFFECTS
  None
===========================================================================*/
int ds707_netmdl_terminate_rm_ppp
(
  void
);
#endif /* PS_PPP_NETMODEL_H */
