/*========================================================================
Copyright (c) 2011, 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
====================================================================== */
/**
@file qurt_thread_wrappers.h

This file contains wrapper functions for QuRT thread APIs
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_thread_wrappers.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
11/14/11   rkc      Created file.

========================================================================== */

#ifndef __QURT_THREAD_WRAPPERS_H__
#define __QURT_THREAD_WRAPPERS_H__

#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_thread_wrappers
@{ */

/**
  Real (unwrapped) qurt_thread_create function. ??need a better comment

  @param [out] tid         Pointer to the thread ID.
  @param [in] attr         Pointer to the thread attributes.
  @param [in] entry_func   Pointer to the entry functions for the thread.
  @param [in] arg          Pointer to the arguments to be passed to the entry
                           function.

  @return
  None.

  @dependencies
  None. @newpage
*/
int __real_qurt_thread_create (qurt_thread_t *tid, 
                                    qurt_thread_attr_t *attr, 
                                    void (*entry_func) (void *), 
                                    void *arg);

/**
  Real (unwrapped) qurt_thread_stop function.

  @return
  None.

  @dependencies
  None.
*/
void __real_qurt_thread_stop(void);

/**
  Real (unwrapped) qurt_thread_resume function.

  @return
  None.

  @dependencies
  None.
*/
int __real_qurt_thread_resume(unsigned int tid);

/**
  Real (unwrapped) qurt_thread_exit function.

  @return
  None.

  @dependencies
  None.
*/
void __real_qurt_thread_exit(int status);

/**
  Real (unwrapped) qurt_thread_join function.

  @return
  None.

  @dependencies
  None. @newpage
*/
int __real_qurt_thread_join(unsigned int tid, int *status); 

/** Stores the statistics of a software thread.
*/
typedef struct {
   qurt_thread_t     tid;           
   /**< Thread ID. */

   char               name[QURT_THREAD_ATTR_NAME_MAXLEN]; 
   /**< Thread name. */

   void               (*entry_func)(void *); 
   /**< Thread entry function. */

   void               *arg;     
   /**< Arguments to the thread entry function. */

   int                thread_state;  
   /**< Thread state. */

   unsigned int       stack_size; 
   /**< Size in bytes of the thread stack. */

} thread_stats_t;

/** List of thread statistics.
*/
typedef struct
{
   thread_stats_t stats_list[QURT_ELITE_MAX_SW_THREADS];
   /**< List of thread statistics. */

   qurt_elite_mutex_t lock;
   /**< Mutex to protect access to the list. */

} thread_stats_list_t;

/** List of non-Elite thread statistics */
extern thread_stats_list_t thread_wrap_stats_list;

/**
  Resets thread Pcycles in a given list.

  @return
  None.

  @dependencies
*/
void thread_wrap_reset_pcycles(thread_stats_list_t *stats);

/** @} */ /* end_addtogroup qurt_elite_thread_wrappers */

#ifdef __cplusplus
}
#endif //__cplusplus
       
#endif //__QURT_THREAD_WRAPPERS_H__
