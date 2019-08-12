

#ifndef _HITAPP_H
#define _HITAPP_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          HIT Test Application

                             Header File
                             
GENERAL DESCRIPTION
  This file contains the functions in the HIT Test App task 


EXTERNALIZED FUNCTIONS
    hitapp_wait
    
REGIONAL FUNCTIONS
    None

  Copyright (c) 2006 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
    
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -------------------------------------------------------
03/20/2012   cnx     Resolved TCB compiler errors for Dime. 
03/16/2012   arm     Resolve TCB errors for Dime. 
03/28/2007   vh      Created

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "rcinit.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/
extern rex_tcb_type *hdrhitapp_tcb_ptr;

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/

typedef enum {
  HITAPP_RPT_TIMER_SIG = 0x0001,
  /* This signal is set when the hitapp_rpt_timer expires.
     This timer is used to enable the task to kick the watchdog on time */

  HITAPP_CMD_Q_SIG = 0x0002,
  /* This signal is set when an item is put on the hitapp_queue. */

  HITAPP_CMD_DONE_SIG = 0x0004,
  /* This signal is set to signal the task that a command execution is 
     completed */

  HITAPP_NV_READ_SIG = 0x0008,
  /* This signal is set when a NV item is read */

  HITAPP_TIMER_SIG = 0x0010,
  /* This signal is set timer expires */

  HITAPP_MSG_Q_SIG = 0x0020,
  /* This signal is set when a message is put in hitapp message queue */

  HITAPP_TASK_SIGS_SIZER = 0xffff

} hitapp_task_sigs_enum_type;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/


/*===========================================================================

FUNCTION hitapp_wait

DESCRIPTION
  This procedure performs a REX wait on a signal that the caller specifies. 
  In additions to that the function must handle watchdog timer expiry and 
  STOP and offline signals

DEPENDENCIES
  None

RETURN VALUE
  word 

SIDE EFFECTS
  None

===========================================================================*/

uint16 hitapp_wait
(
  uint16 sigs
    /* signal to wait for */
);

#endif /* _HITAPP_H */
