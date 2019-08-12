#ifndef DSMIP_METASM_H
#define DSMIP_METASM_H
/*===========================================================================

                         D S M I P _ M E T A S M . H

DESCRIPTION

 The Data Services Mobile IP Meta State Machine header file.

EXTERNALIZED FUNCTIONS
   mip_meta_post_event()
     Post an event to the Meta State Machine.
   mip_meta_sm_config_session()
     Load MIP session defaults from NV to all sessions.

Copyright (c) 2000-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_metasm.h_v   1.10   29 Dec 2002 15:45:36   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsmip_metasm.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/04/09    ss     CMI SU level modifications.
12/07/06    as     Added Bring Down MIP event used for MIP deregistration
06/22/04    kvd    Added  new cmd MSM_PHYS_IFACE_UP_CMD corresponding to
                   MSM_PHYS_IFACE_UP event, posted by ds707  on phys_link_up.
                   MIP metasm  uses event to do a deselect on MIP ICMP sockets
                   to hold reading Router advs until MIP metasm is UP.
04/16/04    jd     Use ps_iface_ioctl directly instead of dss_iface_ioctl.
02/02/04    jd     Add iface_id to post event.  Changed external event names
                   to reflect structural changes in Um iface controller.
                   Moved msmi_info to metasm.c file.
                   Removed flow control APIs, done via interface state now.
01/20/04    jd     Removed dependencies on ps_mip_compat.h
01/20/04    jd     Mainlined Multimode code (multimode only now)
11/14/02    jd     Removed netmodel_mode from msmi_info (unused)
                   mip_meta_sm_set_in_flow takes uint32 mask instead of 16
05/10/02    jd     moved session info array, ui session & password data
                   strutures, mip_meta_sm_config_session to dsmip.c in order
                   to seperate configuration functionality from metasm and
                   solve build dependency issues
10/24/01    js     Modified to support dsmip_info structure. 
10/09/01    js     Added declaration for externalized function
                   mip_meta_sm_config_session().
07/29/01    jd     Added accessor function to see if mip is active
06/14/01    mvl    Udated to support latest design.
12/18/00    mvl    Created module
===========================================================================*/


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                      INCLUDE FILES FOR MODULE

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

#include "datamodem_variation.h"
#include "comdef.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "ps_iface_defs.h"
#include "dsmip_api.h"

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                       PUBLIC TYPE DEFINITIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

typedef enum
{
  MSM_MIN_EV = -1,
  MSM_PHYS_IFACE_UP_EV   = 0, /* Interface is UP and M.IP is supported     */
  MSM_CFG_IFACE_EV       = 1, /* Interface is UP and M.IP is supported     */
  MSM_EXIT_MIP_EV        = 2, /* stop mip (Interface is down or up w/ sip) */
  MSM_INIT_RRQ_FAILED_EV = 3, /* Initial Reg Req failed                    */ 
  MSM_RE_RRQ_FAILED_EV   = 3, /* Re-Reg Req failed                         */
  MSM_BRING_DOWN_MIP_EV  = 4, /* start M.IP deregistration                 */
  MSM_MAX_EV
} mip_meta_sm_event_type;


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                      PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION MIP_META_SM_POST_EVENT()

DESCRIPTION
  This function posts an event to the meta state machine.

PARAMETERS
  event: The event that is being posted to the state machine.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mip_meta_sm_post_event
(
  mip_meta_sm_event_type  event,
  ps_iface_type *         iface_ptr
);


/*===========================================================================
FUNCTION MIP_META_SM_IS_INITIALIZED

DESCRIPTION
  Accessor function to see if MIP subsystem is initialized.

PARAMETERS
  None

RETURN VALUE
  TRUE  - yes, the MIP subsystem is initialized.
  FALSE - no, the MIP subsystem is not initialized.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mip_meta_sm_is_initialized
(
  void
);


/*===========================================================================
FUNCTION MIP_META_SM_OPEN()

DESCRIPTION
  Accessor function to see if Mobile IP is active or coming up.

PARAMETERS
  None

RETURN VALUE
  TRUE - mip is active or coming up
  FALSE - not active or coming up

DEPENDENCIES
  mip_meta_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mip_meta_sm_open
(
  void
);
#ifdef FEATURE_DS_MOBILE_IP_DMU

/*===========================================================================
FUNCTION MIP_DMU_IN_PROGRESS()

DESCRIPTION
  This function  returns TRUE if MDU is in progress right now on the default
  MIP session. Used by atcop to not to update the keys on profile change,
  while DMU is in progress.

PARAMETERS
  None

RETURN VALUE
  TRUE  DMU in progress
  FALSE DMU not in progress

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_dmu_in_progress
(
  void
);

#endif /* FEATURE_DS_MOBILE_IP_DMU */


#endif /* FEATURE_DS_MOBILE_IP */

#endif /* DSMIP_METASM_H */
