 /***********************************************************************
 * osal_qurt_types.h
 * This file maps the osal types to native QURT types.
 *
 * Copyright (C) 2011-2013 QUALCOMM Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/services/osal/qurt/inc/osal_qurt_types.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   --------------------------------------------------------- 
2013-09-20   tbg   Added support for pthread thread local storage 
2013-12-03    sr   Added support for Dog HB implementation 
2011-09-15    sg   Created

===========================================================================*/

#ifndef __OSAL_QURT_TYPES_H__
#define __OSAL_QURT_TYPES_H__

#include "qube.h"
#include "qurt.h"
#include "atomic_ops.h"

/* just a random value picked for size of table that maps the tcb pointer to the
thread ID. Can be increased to whatever value seems enough */
#define QURT_MAX_THREADS 30

typedef unsigned int                     osal_sigs_t;

typedef unsigned long                osal_timer_param_type;

#if defined(DIAG_DOG_SUPPORT)
typedef unsigned int osal_dog_report_type;
#endif

/* this structure contains elements that unique to each thread */
typedef struct
{
/* The handle created to store the thread_id of the QURT thread */
  int       thd_id;

/* the currently asserted messages for the thread */
  qurt_anysignal_t       current_sigs;

  qurt_mutex_t         tcb_msg_lock;

  char             task_name[30]; /* random number */

}qurt_tcb_t;


/* This structure contains the handle to the timer, the tcb pointer
of the thread associated with the timer and teh message to be sent
when the timer expires. */
typedef struct
{
	qurt_tcb_t        *qube_tcb;
	osal_sigs_t        sig;
	qurt_timer_t           timer_handle;
}qtimer_struct;

/* This table maps the tcb pointer of the thread to the thread ID. Needed
for the thread_self() API. */
typedef struct
{
	int              thd_id;
	qurt_tcb_t      *tcb_id;
}tcb_map_table;

/* Thread attributes object */
typedef struct
{
  dword stack_size;
  void (*start_func)(uint32 );
  void *arg;
  void *name;
  dword priority;
  void *stack_address;
  char  *p_tskname;              /* A/N string for task name */
  boolean suspended;              /* is task initially susp?  */
  int dog_report_val;          /* Dog report value         */
} blast_thread_attr_t;


/* Bind OSAL structures to Qube structures */
typedef qurt_tcb_t                      osal_tcb_t;
typedef qtimer_struct                    osal_timer_t;
typedef qurt_mutex_t                    osal_crit_sect_t;
typedef qurt_mutex_t                    osal_mutex_t;
typedef qurt_timer_duration_t                osal_timer_cnt_t;
typedef size_t                           osal_stack_word_type;
typedef qmsgq_callback_t                 osal_timer_cb_type;
typedef int                              osal_priority_type;
typedef blast_thread_attr_t               osal_thread_attr_t;
typedef atomic_word_t                      osal_atomic_word_t;
 typedef atomic_plain_word_t                osal_atomic_plain_word_t;
 typedef qurt_thread_t                     osal_thread_t;
typedef int                             osal_tls_key_t;

 /* Mutex object containing the handle to the mutex and a name for the mutex */
typedef struct
{
  osal_mutex_t handle;
  const char *name;
} osal_mutex_arg_t;

#endif /* not __OSAL_QURT_TYPES_H__ */

