#ifndef CB_TASK_H
#define CB_TASK_H

/*===========================================================================

               CB TASK HEADER FILE (CB_TASK.H)

DESCRIPTION
  This file contains types, function prototypes and data declarations for
  Cell Broadcast task

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/cb_task.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/



/*===========================================================================

FUNCTION CB_MAIN

DESCRIPTION
  Entry point for the CB task.  This function performs task initialisation,
  then remains in an infinite loop, waiting on an input queue, and responding to
  messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_main( dword dummy ) ;
/*lint -esym(715,dummy) */


#endif /* CB_TASK_H */


