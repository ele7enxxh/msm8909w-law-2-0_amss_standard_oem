/*=============================================================================
  Copyright (c) 1990, 1992-1995, 1998-2010 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

            DOG Q6 THREAD MONITOR HEADER

DESCRIPTION
  This file contains external procedure declarations necessary for
  accessing procedures contained within the dog q6 thread monitor file.


===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/dog/src/dog_q6_thread_monitor.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------

===========================================================================*/

#ifndef DOG_Q6_THREAD_MONITOR_H
#define DOG_Q6_THREAD_MONITOR_H

void T0_MONITOR_task(dword ignored);
void T1_MONITOR_task(dword ignored);
void T2_MONITOR_task(dword ignored);
void T3_MONITOR_task(dword ignored);
void T4_MONITOR_task(dword ignored);
void T5_MONITOR_task(dword ignored);

#define T0_MONITOR_TASK_THREAD_MASK     0x1
#define T1_MONITOR_TASK_THREAD_MASK     0x2
#define T2_MONITOR_TASK_THREAD_MASK     0x4
#define T3_MONITOR_TASK_THREAD_MASK     0x8
#define T4_MONITOR_TASK_THREAD_MASK     0x10
#define T5_MONITOR_TASK_THREAD_MASK     0x20


#endif  /* DOG_Q6_THREAD_MONITOR_H*/

