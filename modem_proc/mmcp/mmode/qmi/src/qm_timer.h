#ifndef _QM_TIMER_H
#define _QM_TIMER_H

/*===========================================================================

                         QM_TIMER.H

DESCRIPTION

  QMI_MMODE header file for timer module.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/04/13    sk     Created file
===========================================================================*/

#include "timer.h"
#include "qm_comdef.h"

/*--------------------------------------------------------------------------- 
  data types
---------------------------------------------------------------------------*/

enum qm_timer_id_e
{
  QM_TIMER_ID_MIN          = -1,
  QM_TIMER_ID_INCOM_EVT_RESEND,
  QM_TIMER_ID_MAX
};


/*===========================================================================
  FUNCTION QM_TIMER_INIT()

  DESCRIPTION
    This function initializes the QM TIMER module

  PARAMETERS
  None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_timer_init( void );


/*===========================================================================
  FUNCTION QM_TIMER_IS_ENABLED()

  DESCRIPTION
    This function checks if the timer is enabled.
    
  PARAMETERS
    NONE

  RETURN VALUE
    TRUE if timer is initialized with valid interval, otherwise FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_timer_is_enabled( enum qm_timer_id_e timer_id);


/*===========================================================================
  FUNCTION QM_TIMER_START()

  DESCRIPTION
     This functions starts running a timer. If the timer is already running, it restarts the timer.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_timer_start (enum qm_timer_id_e timer_id, enum qm_subs_e subs);


/*===========================================================================
  FUNCTION QM_TIMER_STOP()

  DESCRIPTION
    This functions stops the running timer 
    
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_timer_stop (enum qm_timer_id_e timer_id);


/*===========================================================================
  FUNCTION QM_IS_TIMER_RUNNING()

  DESCRIPTION
    This functions determines if the timer is running.

  PARAMETERS
    None

  RETURN VALUE
    TRUE if timer is active, FALSE otherwise (even if timer is paused)

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_is_timer_running(enum qm_timer_id_e timer_id);


#endif // !_QM_TIMER_H

