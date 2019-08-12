#ifndef __OSAL_REX_TYPES_H__
#define __OSAL_REX_TYPES_H__

 /***********************************************************************
 * osal_rex_types.h
 * This file maps the OSAL types to native REX types.
 *
 * Copyright (C) 2010-2013 QUALCOMM Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/services/osal/rex/inc/osal_rex_types.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   --------------------------------------------------------- 
2013-09-20   tbg   Added support for pthread thread local storage 
2013-02-15    sr   Moved to new Dog heartbeat implementation   
2011-02-18    is   Include customer.h to get custdiag.h
2010-11-01    vs   Added atomic types typedef
2010-01-18    JV   Moved defines to osal_os.h
2009-09-18    JV   Change in type of argument passed to thread, to be able
                   to abstract more from diag code.
2009-09-09    vs   mutex implementation based on feature DIAG_REX_QDSP_EMULATION
2009-09-10    JV   Moved the definition of struct osal_mutex_arg_t to here.
2009-09-09    JV   osal_lock_mutex() and osal_inlock_mutex() are no more function calls. 
                   They map to INTLOCK() and INTFREE()
2009-08-11    JV   osal_mutex_t is the uint32 variable that saves the state 
                   of the interrupt in the INTLOCK_SAV/INTLOCK_FREE calls.
2009-08-11    JV   Added parameters to the rex_thread_attr_t structure.
2008-10-21    JV   Created

===========================================================================*/

#include "core_variation.h"
#include "rex.h"
#if defined(DIAG_DOG_SUPPORT)
#include "dog_hb_rex.h"
#endif

#if defined (IMAGE_MODEM_PROC) && defined (IMAGE_QDSP6_PROC)
	#define DIAG_REX_QDSP_EMULATION 
#endif

#ifdef DIAG_REX_QDSP_EMULATION
#include "atomic_ops.h"
#endif

#include "types.h" /* for pthread_key_t */

/* Thread attributes object */
typedef struct
{
  dword stack_size;
  void (*start_func)(uint32);
  void * arg;
  void *name;
  dword priority;
  void *stack_address;
  char  *p_tskname;              /* A/N string for task name */
  boolean suspended;              /* is task initially susp?  */
  int dog_report_val;          /* Dog report value         */
} rex_thread_attr_t;

 /* Bind OSAL structures to REX structures */

 typedef rex_sigs_type                      osal_sigs_t;
 typedef rex_tcb_type                       osal_tcb_t;
 typedef rex_timer_type                     osal_timer_t;
 typedef rex_crit_sect_type                 osal_crit_sect_t;
 typedef rex_crit_sect_type                 osal_mutex_t;
 typedef rex_timer_cnt_type                 osal_timer_cnt_t;

 typedef rex_stack_word_type                osal_stack_word_type;
 typedef rex_priority_type                  osal_priority_type;
 typedef rex_timer_cb_type                  osal_timer_cb_type;
 typedef rex_thread_attr_t                  osal_thread_attr_t;
 typedef unsigned long                      osal_timer_param_type;
#ifdef DIAG_REX_QDSP_EMULATION
 typedef atomic_word_t                      osal_atomic_word_t;
 typedef atomic_plain_word_t                osal_atomic_plain_word_t;
#else
 typedef volatile int                       osal_atomic_word_t;
 typedef int                                osal_atomic_plain_word_t;
#endif
typedef pthread_key_t                       osal_tls_key_t;

 #if defined(DIAG_DOG_SUPPORT)
typedef dog_report_type osal_dog_report_type;
#endif

 /* Mutex object containing the handle to the mutex and a name for the mutex */
typedef struct
{
  osal_mutex_t handle;
  const char *name;
} osal_mutex_arg_t;


#endif /* not __OSAL_REX_TYPES_H__ */

