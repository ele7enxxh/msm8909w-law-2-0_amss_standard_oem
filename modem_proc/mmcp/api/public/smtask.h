#ifndef SMTASK_H
#define SMTASK_H
/*===========================================================================
                          SM Interface

DESCRIPTION

 This module defines contains the defines, structures and function 
 prototypes required to send a command to the Session Management task.
   
FUNCTIONS

  Functions used by all Entities to put SM command :
  --------------------------------------------------
  sm_get_cmd_buf()       Get a SM command buffer.
  sm_put_cmd_buf()       Put a SM message on the SM command queue.

  Function used by Call Manager to get a connection Id.
  -----------------------------------------------------
  sm_get_connection_id Gets a connection Id.

  Function used by TMC controller task to start the SM task.
  ----------------------------------------------------------
  sm_task()              SM task entry
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
 

Copyright (c) 2000 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/smtask.h_v   1.7   18 Jul 2002 15:45:48   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/smtask.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

/* SM task is the entry point in to the SM task */
extern void sm_task(dword parm); 

#endif
