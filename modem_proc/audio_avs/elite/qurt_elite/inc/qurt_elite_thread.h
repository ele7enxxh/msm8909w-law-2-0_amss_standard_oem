/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/**
@file qurt_elite_thread.h

@brief This file contains utilities for threads. Threads must be joined to
avoid memory leaks. This file provides functions to create and destroy threads,
and to change thread priorities.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_thread.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13    sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11    leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10    brs      Edited for doxygen-to-latex process.
02/04/10    mwc      Created file.
========================================================================== */

#ifndef QURT_ELITE_THREAD_H
#define QURT_ELITE_THREAD_H

#include "qurt_elite_memory.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

/** @addtogroup qurt_elite_thread
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/** @name Maximum Number of Hardware and Software Threads Available on the Platform
@{ */

//Support Q6v3 (variable HW threads) & Q6v4 (3 HW threads). For others issue error until supported.
#if (__QDSP6_ARCH__<=3)
/** Maximum number of hardware threads. */
#define QURT_ELITE_MAX_HW_THREADS       MAX_HTHREADS

#elif (__QDSP6_ARCH__>=4)
/** Maximum number of hardware threads. */
#define QURT_ELITE_MAX_HW_THREADS       4

#else
#error "Unknown Q6 Arch Version"
#endif

/** Maximum number of software threads. */
#define QURT_ELITE_MAX_SW_THREADS       MAX_THREADS

/** Generic thread mask. */
#define QURT_ELITE_GENERIC_THREAD_MASK  0xFF

/** @} */ /* end_namegroup Maximum Number of Hardware and Software Threads */

/** @name Thread Stack Fill Pattern and Spacing
@{ */

/** Stack fill word. */
#define QURT_ELITE_STACK_FILL_WORD             0xF8F8F8F8L

/** Stack fill spacing. */
#define QURT_ELITE_STACK_FILL_SPACING          16

/** @} */ /* end_namegroup Thread Stack Fill Pattern and Spacing */

/** Handle to a thread. */
typedef int qurt_elite_thread_t;

/****************************************************************************
** Threads
*****************************************************************************/

/**
  Creates and launches a thread.

  The thread stack can be passed in as the pStack argument, or pStack=NULL
  indicates QuRT_Elite allocates the stack internally. If the caller provides
  the stack, the caller is responsible for freeing the stack memory after
  joining the thread.

  Preallocated stacks must be freed after the dying thread is joined. The
  caller must specify the heap in which the thread stack is to be allocated.

  This function is for Low-Power Audio (LPA) and can be used by all services
  by passing #QURT_ELITE_HEAP_DEFAULT as the heap_id if no specific heap ID
  is required.

  @datatypes
  #qurt_elite_thread_t \n
  #QURT_ELITE_HEAP_ID

  @param[out] pTid           Thread ID.
  @param[in]  pzThreadName   Thread name.
  @param[in]  pStack         Location of a preallocated stack (NULL causes a
                             new allocation). pStack must point to the lowest
                             address in the stack.
  @param[in]  nStackSize     Size of the thread stack.
  @param[in]  nPriority      Thread priority, where 0 is the lowest and 255 is
                             the highest.
  @param[in]  pfStartRoutine Entry function of the thread.
  @param[in]  arg            Passed to the entry function; this argument can
                             be a case to any pointer type.
  @param[in]  heap_id        Specifies the heap to which the thread stack is
                             allocated.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  None.
*/
ADSPResult qurt_elite_thread_launch(qurt_elite_thread_t *pTid, char *pzThreadName, char* pStack,
   size_t nStackSize, int nPriority, int (*pfStartRoutine)(void *), void* arg, QURT_ELITE_HEAP_ID heap_id);

/**
  Waits for a specified thread to exit and collects its exit status.

  @datatypes
  #qurt_elite_thread_t

  @param[in]  nTid     Thread ID for which to wait.
  @param[out] nStatus  Pointer to the value returned by the pfStartRoutine()
                       function called in qurt_elite_thread_launch().

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
void qurt_elite_thread_join(qurt_elite_thread_t nTid, int* nStatus);

/**
  Intializes the thread context structure.

  @return
  None.

  @dependencies
  None.
*/
void qurt_elite_thread_list_init(void);


/**
  Queries the caller's thread priority.

  @return
  The caller's thread priority, where 255 is the highest and 0 is the lowest.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
static inline int qurt_elite_thread_prio_get(void)
{
   return 255 - qurt_thread_get_priority(qurt_thread_get_id());
}

/**
  Changes the caller's thread priority.

  @param[in] nPrio  New priority, where 255 is the highest and 0 is the lowest.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
static inline void qurt_elite_thread_set_prio(int nPrio)
{
   (void) qurt_thread_set_priority(qurt_thread_get_id(), 255 - nPrio);
}

/** @} */ /* end_addtogroup qurt_elite_thread */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_THREAD_H

