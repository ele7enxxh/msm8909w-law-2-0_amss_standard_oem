/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              G L O B A L    C O M M A N D    H E A D E R  

GENERAL DESCRIPTION
  This module contains the procedure which processes the global command
  header after a command has been processed.
  
EXTERNALIZED FUNCTIONS
  cmd_done
    Processes global command header.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1990,1991,1992 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Copyright (c) 1998,1999      by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/cmd.c_v   1.1   29 Jan 2001 10:53:54   dyang  $
$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/cmd.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                     Added support for MSM5000_IRAM_FWD
12/16/98   jct     Removed 80186 support
09/21/98   jct     Incorporate ARM porting changes, updated copyright
07/14/92   jai     Create module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "queue.h"
#include "cmd.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifndef MSM5000_IRAM_REV
/*===========================================================================

FUNCTION CMD_DONE

DESCRIPTION
  This procedure finishes processing a report or command.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cmd_done
( 
  cmd_hdr_type *hdr_ptr
    /* pointer to header of received command */
)
{
  rex_tcb_type *task_ptr;
    /* task to signal */
  rex_sigs_type sigs;
    /* signal to set */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* temporarily store task to signal and signal to set */
  task_ptr = hdr_ptr->task_ptr;
  sigs = hdr_ptr->sigs;

  /* place buffer on done queue if requested */
  if( hdr_ptr->done_q_ptr != NULL )
  {
    /* place command on requested queue */
    q_put( hdr_ptr->done_q_ptr, &hdr_ptr->link );
  }

  /* signal task if requested */
  if( task_ptr != NULL )
  {
    ( void )rex_set_sigs( task_ptr, sigs );
  }
} /* cmd_done */
#endif /*MSM5000_IRAM_REV*/
