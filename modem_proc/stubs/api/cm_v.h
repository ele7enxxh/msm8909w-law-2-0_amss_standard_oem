#ifndef CM_V_H
#define CM_V_H

/**==========================================================================
@FILE_DOC

              C A L L   M A N A G E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for
  clients to interface with the Call Manager module.

Copyright (c) 2002 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/cm_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Possible PD events
*/
typedef enum cm_pd_event_e {

  CM_PD_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PD_EVENT_SESSION_START,
    /**< A PD session start event */

  CM_PD_EVENT_SESSION_GRANTED,
    /**< A PD session granted event */

  CM_PD_EVENT_REVA_MSG,
    /**<A PD event with RevA message*/

  CM_PD_EVENT_SESSION_END,
    /**< A PD session end event */

  /* 4 */
  CM_PD_EVENT_FIX_START,
    /**< Blocked HDR data call origination to start GPS search */

  CM_PD_EVENT_FIX_END,
    /**< Unblocked HDR data call origination */

  CM_PD_EVENT_SESSION_DENIED,
    /**< PD Session denied */

  CM_PD_EVENT_SESSION_DORMANT_CHGD,
    /**< The session dormancy changed */

  /* 8 */
  CM_PD_EVENT_MAX
    /**< @internal */


} cm_pd_event_e_type;


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*CM_V_H*/
