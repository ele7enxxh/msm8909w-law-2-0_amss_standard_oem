/*=============================================================================

                  T I M E R - I P C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements IPC time task 

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs

EXTERNALIZED FUNCTIONS
  time_ipc_task
    Initializes time Time_IPC task

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_ipc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/11/10   din     Moved efs write to Time IPC task.
02/19/10   din     Moved genoff RPC calls from modem proc to TIME_IPC task.
02/08/10   sri     Made changes for DOG_DYNAMIC_API
05/22/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "timetick.h"
#include "rex.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#ifdef DOG_DYNAMIC_API
/*  Dog variables - now we MAY be dynamic       */
extern uint32  dog_time_ipc_rpt;       /* dog report ID */
extern uint32  dog_time_ipc_rpt_time;            /* Report Period */

#define  DOG_TIME_IPC_RPT       dog_time_ipc_rpt        /* rpt_def */ 
#define  DOG_TIME_IPC_RPT_TIME  dog_time_ipc_rpt_time   /* time_def */ 
#endif /* DOG_DYNAMIC_API */

/* Signal for a time_tod_resync */
#define TIME_TOD_RESYNC_SIG      0x0008

/* Signal for a Genoff Update */
#define TIME_IPC_GENOFF_UPDATE   0x0010

/* Signal for a efs_write */
#define TIME_IPC_EFS_WRITE       0x0020

/*Signal for registration of mm*/
#define TIME_IPC_MM_REGISTRATION 0x0040

/*Signal to send over remote modem upate*/
#define TIME_IPC_SEND_REMOTE_MODEM_UDPATE 0x0080

/* TCB for Time_IPC task.   */
extern rex_tcb_type               time_ipc_tcb;                   

/* Stack for Time_IPC Task. */
extern rex_stack_word_type        time_ipc_stack [ ];


/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIME_IPC_TASK

DESCRIPTION
  Initializes the Time_IPC Task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void  time_ipc_task
(
  /* Unused parameter - required to match rex_task_func_type prototype */
  dword                           parm    /*lint -esym(715,ignored) */
);

