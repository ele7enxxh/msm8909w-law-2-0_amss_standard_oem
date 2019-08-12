#ifndef CUSTREX_H
#define CUSTREX_H
/*===========================================================================

DESCRIPTION
  Configuration for REX.

  Copyright (c) 2002  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custrex.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/26/06   hs      turn on FEATURE_REX_DIAG always
11/11/02   cah     Added GSM features.
05/01/02   jct     Created
===========================================================================*/

/* Pre-emptive interrupts
*/
#define FEATURE_REX_PREEMPT_INTS

/* Use a larger FIQ stack for more processing in the FIQ interrupt
*/
#define FEATURE_REX_LARGE_FIQ_STACK

/* Turn on ability to obtain CPU utilization information for each
** task via the UI debug menu
*/
#undef FEATURE_SOFTWARE_PROFILE

/* Turn on run time task utilization information
*/
#define FEATURE_REX_PROFILE

/* Turn on DIAG interface to REX (rex profiling)
*/
#define FEATURE_REX_DIAG

/* Turn on ability to create tasks with dynamically allocated TCB's
** and stacks
*/
#undef FEATURE_REX_CREATE_TASK

#ifdef FEATURE_GSM
  /* Enable multi-threading support in the memory heap
  */
  #define FEATURE_MEMHEAP_MT
  
  /* Enable Deferred Procedure Calls
  */
  #define FEATURE_REX_DPC
#endif /* FEATURE_GSM */


#define FEATURE_REX_IPC

/* Auto-kick the dog while a task is busy to reduce dog timeouts
*/
#define FEATURE_ENHANCED_REX_TASK

#ifdef FEATURE_L4
  /* Define to enable REX Heap Manager support
  */
  #define FEATURE_REX_DYNA_MEM

  /* disable memheap assert */
  #define MEMHEAP_ASSERT_NONE

  #define FEATURE_REX_TIMER_EX

  #define L4_ASYNC_IPC
#endif

/* Turn on qmutex support
*/
#define FEATURE_QMUTEX

#endif /* CUSTREX_H */

