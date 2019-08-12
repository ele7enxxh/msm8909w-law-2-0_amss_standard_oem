#ifndef CMSSIDMI_H
#define CMSSIDMI_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
                            Idle  Digital  Mode ( IDM )

                 I N T E R N A L    H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the internal types necessary for implementing
  the CM Idle Digital Mode ( IDM ) module.

Copyright (c) 2003 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmssidmi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/12   qf      Add a timer to restore HDR back to phone mode mask
06/14/11   fj       Add support to suspend and resume IDM timer.
03/01/09   sv      Lint cleanup.
02/06/04   ws      Initial jaguar/mainline merge.
11/21/03   ic      Fixed FEATURE_HDR_HANDOFF featurization bug. 
11/19/03   ic      Featurized SS IDM object's functionality under
                   FEATURE_HDR_HANDOFF.
                   Replaced CMSSIDM_AVOID_HANDUP_TIMER with
                   CMSSIDM_AVOID_HANDUP_TIME.
11/06/03   SJ      Initial release.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */

#ifdef FEATURE_HDR_HANDOFF


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to correctly align function definitions (cosmetic).
*/
#ifndef EXTERN
#define EXTERN
#endif


/*===========================================================================
=============================================================================
=============================================================================
==================== PREF IMPLEMENTATION RELATED TYPES ==================
=============================================================================
=============================================================================
===========================================================================*/



/* Reserved value; to indicate timer not running.
*/
#define CMSSIDM_UPTIME_NONE             (dword)(-1)

/* "Avoid_handup_timer" time out time, set to 1 seconds.
*/
#define CMSSIDM_AVOID_HANDUP_TIME      (dword)(1)

/* "Handdown_timer" time out time, set to 5 seconds.
*/
#define CMSSIDM_HANDDOWN_TIME          (dword)(5)

/* Reselect_hdr time out, set to 30 seconds.
*/
#define CMSSIDM_RESTORE_HDR_TIME      (dword)(30)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of CM IDM states.
*/
typedef enum {

  CMSSIDM_STATE_NONE,                  /* Default powerup state.
                                       */

  CMSSIDM_STATE_MAIN,                  /* Current IDM corresponds to main
                                       ** SS instance.
                                       */

  CMSSIDM_STATE_HYBR_HDR,              /* Current IDM corresponds to Hybrid
                                       ** HDR SS Instance.
                                       */

  CMSSIDM_STATE_MAX                    /* For internal use. */

} cmssidm_state_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that holds the state and other information of the
** IDM module.
*/
typedef struct {

  /* Current state of IDM module, default state = CMSSIDM_STATE_NONE.
  */
  cmssidm_state_e_type                state;

  /* IDM module's generic timer.
  */
  dword                               idm_timer_uptime;

  /* Timer to restore hdr back to mode pref which was disabled because of
  ** CM_HDR_RESELECT_EXIT_HDR_F.
  */
  dword                               restore_hdr_uptime;
  
  /* Previous Idle digital mode.
  */
  sys_sys_mode_e_type                 prev_idm;

  /* Current idle digital mode.
  */
  sys_sys_mode_e_type                 curr_idm;

  /* Current suspended state for IDM timer.
  */
  boolean                             is_idm_timer_suspended;

} cmssidm_info_s_type;



/*===========================================================================
=============================================================================
=============================================================================
==================== IDM module's INTERNAL FUNCTIONS ========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmssidm_get_info_ptr

DESCRIPTION
  This function will fetch the pointer to the IDM's info.

DEPENDENCIES
  None.

RETURN VALUE
  cmssidm_info_s_type* - Pointer to PREF's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN cmssidm_info_s_type*             cmssidm_get_info_ptr
(
       void

);


/*===========================================================================

FUNCTION cmssidm_update_suspend_state

DESCRIPTION
  This function sets the IDM's suspended state.

DEPENDENCIES
  cmssidm_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN void                            cmssidm_update_suspend_state
(

       boolean            is_idm_timer_suspended
          /* The suspended state of the IDM.
          */

);

#endif // FEATURE_HDR_HANDOFF
#endif /* CMSSIDM_H */
