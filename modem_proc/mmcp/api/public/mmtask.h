#ifndef _MM_TASK_H
#define _MM_TASK_H

/*===========================================================================
                          MM Interface

DESCRIPTION

 This module defines contains the defines, structures and function
 prototypes required to send a command to the MM task.


EXTERNALIZED FUNCTIONS

 mm_put_cmd           Put a MM message on the MM command queue
 mm_get_cmd_buf       Get a MM command buffer

 mm_rrc_get_cmd_buf   RRC specific get - fills in msg set and length

 mm_timer_expiry      Callback function when a MM timer expires

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmtask.h_v   1.20   18 Jul 2002 11:41:56   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/mmtask.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/


extern void mm_main( dword dummy );
#endif 
