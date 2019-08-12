#ifndef COMPTASK_H
#define COMPTASK_H
/*===========================================================================

                              C O M P  T A S K

                            H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the COMP Task. This file
  contains all the functions, definitions and data types needed for other
  tasks to interface to the COMP Task.

  Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/comptask.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/09   sa     Initial creation.

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION COMP_TASK

DESCRIPTION
  This is the entry point for the Compression Task. This function contains
  the main processing loop that waits for events (signals or commands) and
  dispatches each event to the appropriate entity for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/

extern void  comp_task
(
  dword ignored
    /* lint -esym(715,ignored) */
);

#endif /* COMPTASK_H */
