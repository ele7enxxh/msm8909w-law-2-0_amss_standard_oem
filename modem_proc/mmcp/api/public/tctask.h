#ifndef TCTASK_H
#define TCTASK_H
/*===========================================================================
                          TC Interface

DESCRIPTION

 This module defines contains the defines, structures and function 
 prototypes required to send a command to the Test Control (TC) task.
   
FUNCTIONS

  Functions used by all Entities to put TC command :
  --------------------------------------------------
  tc_get_cmd_buf()       Get a TC command buffer.
  tc_put_cmd_buf()       Put a TC message on the TC command queue.

  Function used by TMC controller task to start the TC task.
  ----------------------------------------------------------
  tc_task()              TC task entry
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
 

Copyright (c) 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/tctask.h_v   1.3   06 Feb 2002 18:34:02   madiraju  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/tctask.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/* TC task is the entry point in to the TC task */
extern void tc_task(dword parm); 

#endif
