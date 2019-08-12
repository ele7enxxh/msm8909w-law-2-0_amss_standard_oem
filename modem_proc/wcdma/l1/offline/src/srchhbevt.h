#ifndef SRCH_EVT_H
#define SRCH_EVT_H

/*===========================================================================
                            S R C H _ E V T . H

GENERAL DESCRIPTION

  This file contains the prototypes for the search module sequencer event
  handlers.
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

  Copyright (c) 2000, 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchevt.h_v   1.1   27 Apr 2001 11:48:50   wnew  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchhbevt.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/13   jd      Searcher redesign
===========================================================================*/

extern boolean srch_allow_hb_to_schedule_searches;


/*===========================================================================
FUNCTION        SRCH_HEARTBEAT_EVENT_HANDLER

DESCRIPTION     This function handles the search heartbeat event.

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void srch_heartbeat_event_handler_sched(void);

extern void srch_evt_ctrl_hb_searches(boolean enable_hb);

/*===========================================================================

FUNCTION SRCHCRUTILS_IS_RESEL_IN_PROGRESS

DESCRIPTION
  This function will be called from scheduler to check if Resel is in progress and issue hb searches

DEPENDENCIES
  None.

RETURN VALUE
TRUE - If Intra/Inter-freq resel is in progress
FALSE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srchcrutils_is_resel_in_progress(void);


#endif  /* SRCH_EVT_H */

