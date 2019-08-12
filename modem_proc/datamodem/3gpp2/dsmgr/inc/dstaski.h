#ifndef DSTASKI_H
#define DSTASKI_H
/*===========================================================================

                     D A T A   S E R V I C E S   T A S K

                   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This is the internal header file for the Data Services (DS) Task. This file
  contains all the functions, definitions and data types needed for other
  units within the Data Services Task.


  Copyright (c) 2001-2012 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dstaski.h_v   1.0   08 Aug 2002 11:19:50   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/dstaski.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/12   msh     TCB changes due to Core changes  
01/02/12   msh     Coan: Feature cleanup
12/07/10   ad      Added DUAL IMEL support for DSDS.
09/22/09   vrk	   Merged changes for LTE.
06/01/04   ar      Externalized dsi_wait() routine.
04/22/04   ak      Added DS_FORCE_ASSERT to make lint cleanup easier.
07/14/02   ak      Added IS_IN_DS_TASK macro().
10/01/01   ar      Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"
#include "rex.h"
#include "task.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC MACRO DECLARATIONS

===========================================================================*/
/*===========================================================================
MACRO         IS_IN_DS_TASK

DESCRIPTION   Wrapper for dsi_is_in_ds_task()

RETURN VALUE  TRUE - executing in DS task
              FALSE - not executing in DS TASK

DEPENDENCIES  Called only in task-context.

SIDE EFFECTS  None
===========================================================================*/
#define IS_IN_DS_TASK() (dsi_is_in_ds_task())


/*===========================================================================
MACRO         DS_FORCE_ASSERT

DESCRIPTION   Called by DS when it wants to force an ASSERT.  Since LINT
              hates an ASSERT(0), do it here witht the LINT comments.

RETURN VALUE  NONE

DEPENDENCIES  NONE

SIDE EFFECTS  NONE
===========================================================================*/
#define DS_FORCE_ASSERT()                                                  \
    /*lint -save -e506 -e774 */                                                  \
      ASSERT(0);                                                           \
    /*lint -restore */



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DSI_WAIT

DESCRIPTION   This function suspends execution of the Data Services task
              until one (or more) signals from the requested signal mask is
              set.
              Watchdog kicking is performed in this function.

DEPENDENCIES  None

RETURN VALUE  The signal mask returned by rex_wait().

SIDE EFFECTS  None
===========================================================================*/
rex_sigs_type  dsi_wait
(
  rex_sigs_type  requested_signal_mask       /* Signal mask to suspend on */
);

/*===========================================================================
FUNCTION      DSI_IS_IN_DS_TASK

DESCRIPTION   Returns TRUE if executing in DS task.

RETURN VALUE  TRUE - executing in DS task
              FALSE - not executing in DS TASK

DEPENDENCIES  Called only in task-context.

SIDE EFFECTS  None
===========================================================================*/
boolean dsi_is_in_ds_task( void );

/*===========================================================================
FUNCTION  DSI_GET_DS_TCB

DESCRIPTION   Returns DS TCB pointer

RETURN VALUE  ds_tcb_ptr

DEPENDENCIES  Should be called after ds_tcb_ptr has been inited .

SIDE EFFECTS  None
===========================================================================*/
rex_tcb_type* dsi_get_ds_tcb( void );

/*===========================================================================
FUNCTION      DS_UT_SYNC_CMD_HDLR

DESCRIPTION   UT Sync command

RETURN VALUE  None

DEPENDENCIES  None

SIDE EFFECTS  None
===========================================================================*/
void ds_ut_sync_cmd_hdlr( void );

#endif /* DSTASKI_H */
