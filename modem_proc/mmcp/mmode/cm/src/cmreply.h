#ifndef CMREPLY_H
#define CMREPLY_H
/*===========================================================================

              C A L L   M A N A G E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions for manipulating reply (from LL)
  object by the Call Manager.

  It should NOT be included by any source file outside the Call Manager


Copyright (c) 2002 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmreply.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
01/06/06   ic      Lint cleanup
02/06/04   ws      Initial jaguar/mainline merge.
01/21/03   ws      Updated copyright information for 2003
08/09/02   vt      Initial Version.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "cmi.h"        /* External interface to cm.c */


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* Type for CM reply.
** Handles the details that are associated with setting up a CM command
** to wait for a reply from MC.
*/

/* To avoid circular dependency, cm_reply_s_type is defined in cmi.h */


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cm_reply_init

DESCRIPTION
  Initialize a CM reply object.

  This function must be called on a CM reply object before it being used,
  in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_reply_init(

    cm_reply_s_type    *reply_ptr
        /* Pointer to a reply object to be initialized */
);



/*===========================================================================

FUNCTION cm_reply_set

DESCRIPTION
  Setup necessary conditions for putting a CM command on a wait
  for reply from MC.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_reply_set(

    cm_reply_s_type          *reply_ptr,
        /* Pointer to a reply object */

    cm_cmd_type            *cmd_ptr,
        /* Pointer to a CM command waiting for a reply from MC */

    cm_name_type             mc_rpt_id,
        /* Indicate the MC report we are waiting for */

    dword                    reply_time
        /* Indicate an uptime upper bound for how long we should wait
        ** for the reply to come */
);




/*===========================================================================

FUNCTION cm_reply_get

DESCRIPTION
  Get a pointer to the CM command waiting for a replay from MC.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  A pointer to the CM command waiting for a replay from MC.
  If no command is waiting for reply, return NULL.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_cmd_type  *cm_reply_get(

    cm_reply_s_type    *reply_ptr
        /* Pointer to a reply object */
);




/*===========================================================================

FUNCTION cm_reply_check

DESCRIPTION
  Check whether a specified mc report is the reply for which
  we are waiting.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  TRUE if the given mc report matches the replay for which
  we are waiting, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_reply_check(

    const cm_reply_s_type    *reply_ptr,
        /* Pointer to a reply object */

    cm_name_type       cm_mc_rpt
        /* Are we waiting for this mc report */
);



/*===========================================================================

FUNCTION cm_reply_is_waiting

DESCRIPTION
  Check whether we are currently waiting for a reply.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  TRUE if we are currently waiting for a rely, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_reply_is_waiting(

    const cm_reply_s_type    *reply_ptr
        /* Pointer to a reply object */
);

/*===========================================================================

FUNCTION cm_is_proto_deact_in_progress

DESCRIPTION
  Determines whetehr protocol being deactivated due to opert mode change .

DEPENDENCIES
None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_proto_deact_in_progress(void);



/*===========================================================================

FUNCTION cm_reply_too_long

DESCRIPTION
  Check whether are waiting for too long on a reply

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  TRUE if are waiting for too long on a reply, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_reply_too_long(

    const cm_reply_s_type    *reply_ptr
        /* Pointer to a reply object */
);



/*===========================================================================

FUNCTION cm_reply_should_cmd_wait

DESCRIPTION
  This function determines if a new command can be processed or is it waiting
  for a reply from MC.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if current system is waiting for response from MC. False otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_reply_should_cmd_wait
(
    const cm_cmd_type    *cmd_ptr
        /* Pointer to a command object */
);


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#ifdef FEATURE_OTASP

/*===========================================================================

FUNCTION cm_commit_reply_set

DESCRIPTION
  Setup necessary conditions to wait for MC's OTASP commit status command.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_commit_reply_set(

    cm_reply_s_type    *reply_ptr,
        /* Pointer to a reply object */

    cm_cmd_type      *cmd_ptr,
        /* Pointer to a CM command waiting for a reply from MC */

    cm_name_type       mc_rpt_id,
        /* Indicate the MC report we are waiting for */

    dword              reply_time
        /* Indicate an uptime upper bound for how long we should wait
        ** for the reply to come */
);



/*===========================================================================

FUNCTION cm_commit_reply_check

DESCRIPTION
  Check whether a specified mc report is the reply for which
  we are waiting: OTASP commit status command.

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  TRUE if the given mc report matches the replay for which
  we are waiting, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_commit_reply_check(

    const cm_reply_s_type    *reply_ptr,
        /* Pointer to a reply object */

    cm_name_type       cm_mc_rpt
        /* Are we waiting for this mc report */
);



/*===========================================================================

FUNCTION cm_commit_reply_too_long

DESCRIPTION
  Check whether are waiting for too long on a reply

DEPENDENCIES
  Reply object must have already been initialized with
  cm_reply_init().

RETURN VALUE
  TRUE if are waiting for too long on a reply, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_commit_reply_too_long(

    const cm_reply_s_type    *reply_ptr
        /* Pointer to a reply object */
);
#endif /* FEATURE_OTASP */
#endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900  */

#endif /* CMREPLY_H */
