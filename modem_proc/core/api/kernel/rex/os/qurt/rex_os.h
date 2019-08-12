#ifndef __REX_OS_H__
#define __REX_OS_H__
/*==========================================================================

                      REX OS HEADER FILE

DESCRIPTION
   This header contains the OS specific data type definitions and APIs
   required for the REX - Real Time Executive port on BLAST.

   Platform specific defines for the REX Real Time Executive on Q6 RTOS
   port. REX clients SHOULD NOT include this header file in
   their source code or use any datatype or API in this header.

Copyright (c) 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rex/os/qurt/rex_os.h#1 $

===========================================================================*/

#include "qurt.h"
#include "rex_types.h"
#include "pthread.h"

#define FEATURE_REXL4_CRIT_SECT_TO_QUBE_MUTEX

/* These have been deprecated and will be removed in the future */
#define REX_COMMON_SMT_MASK                     REX_SINGLE_CPU_AFFINITY_MASK
#define REX_ANY_SMT_MASK                        REX_ANY_CPU_AFFINITY_MASK
#define REX_ANY_OTHER_SMT_MASK                  REX_ANY_CPU_AFFINITY_MASK
#define REX_SMD_TASK_SMT_MASK                   REX_ANY_SMT_MASK
#define REX_PS_TASK_SMT_MASK                    REX_ANY_SMT_MASK
#define REX_OS_TASK_MAX_PRIORITY                (255 - (MODEM_FW_NUM_PRIO))

struct rex_os_tcb;
typedef struct _rex_tcb_type
{
   pthread_t            thread_id;
   struct rex_os_tcb    *p;
} rex_tcb_type;

/* --------------------------------------------------------------------
** Inter-Process Communication
** -------------------------------------------------------------------- */
#define REX_IPC_MQ_SIZE 320
typedef struct
{
   uint32                    node_id;
   uint32                    service_id;
   rex_sigs_type             receive_signal;
   uint32                    msg_wanted;
   rex_tcb_type              *tcb_ptr;
   struct pq_msg_queue_type* mq_ptr;
   uint8                     mq[REX_IPC_MQ_SIZE];
} rex_ipc_info_type;

/* ----------------------------------------------------------------------
** REX Critical Section Type
** - structure contains a lock and a pointer to the first TCB waiting
**   on the lock.  TCBs then form the rest of the list with the cs_list
**   pointer in the TCB.
** ---------------------------------------------------------------------- */
typedef qurt_mutex_t rex_os_crit_sect_type;

/* ----------------------------------------------------------------------
** Advance translation of rex_timer_struct to timer_struct, since
** rex_tcb_struct might declare: struct rex_timer_struct *sync_timer_ptr;
** ---------------------------------------------------------------------- */
#define rex_os_timer_struct         timer_struct
#define rex_timer_struct            rex_os_timer_struct

/*
** This needs to be done here unfortunately since clients are relying on rex.h to provide
** REX_TIMER_TCB macro and not including timer.h directly :(. Will cleanup in subsequent
** releases.
*/
#include "timer.h"
#define rex_os_timer_type           timer_type

/* REX_ATTR: Required for now as there are clients using REX_OS_DIS(EN)ABLE_PREEMPTION */
#define REX_OS_DISABLE_PREEMPTION()             {\
                                                   rex_priority_type old_prio;          \
                                                   rex_tcb_type *_p_tcb = rex_self();   \
                                                   old_prio = rex_set_task_pri(_p_tcb, REX_OS_TASK_MAX_PRIORITY)

#define REX_OS_ENABLE_PREEMPTION()                 (void) rex_set_task_pri(_p_tcb, old_prio);  \
                                                }

#define REX_OS_ISR_LOCK(my_global_lock_ptr)     rex_enter_crit_sect((my_global_lock_ptr))
#define REX_OS_ISR_UNLOCK(my_global_lock_ptr)   rex_leave_crit_sect((my_global_lock_ptr))

#define REX_OS_TASK_PREEMPTION_DISABLE()        REX_OS_DISABLE_PREEMPTION()
#define REX_OS_TASK_PREEMPTION_ENABLE()         REX_OS_ENABLE_PREEMPTION()

/* Moved this macro out of the primary interface file rex.h as INTLOCK APIs are OS specific */
int rex_ints_are_locked(void);
#define INTS_ARE_LOCKED()           rex_ints_are_locked()

#ifdef __cplusplus
   extern "C"
   {
#endif

// hw bitmask version of rex_def_task()
void rex_def_task_ext3(
  unsigned int        hw_bitmask,
  rex_tcb_type         *p_tcb,                 /* valid tcb for new task   */
  void *               p_stack,                /* stack for new task       */
  rex_stack_size_type  p_stksiz,               /* stack size for new task  */
  rex_priority_type    p_pri,                  /* priority for new task    */
  rex_task_func_type   p_task,                 /* task startup function    */
  dword                p_param                 /* parameter for new task   */
);



// hw bitmask version of rex_def_task_ext()
void rex_def_task_ext4(
  unsigned int        hw_bitmask,
  rex_tcb_type         *p_tcb,                  /* valid tcb for new task   */
  unsigned char*        p_stack,                /* stack for new task       */
  rex_stack_size_type   p_stksiz,               /* stack size for new task  */
  rex_priority_type     p_pri,                  /* priority for new task    */
  rex_task_func_type    p_task,                 /* task startup function    */
  dword                 p_param,                /* parameter for new task   */
  char                 *p_tskname,              /* A/N string for task name */
  boolean               suspended               /* is task initially susp?  */
);


// hw bitmask version of rex_def_task_ext2()
void rex_def_task_ext5(
  unsigned int          hw_bitmask,
  rex_tcb_type         *p_tcb,                  /* valid tcb for new task   */
  unsigned char*        p_stack,                /* stack for new task       */
  rex_stack_size_type   p_stksiz,               /* stack size for new task  */
  rex_priority_type     p_pri,                  /* priority for new task    */
  rex_task_func_type    p_task,                 /* task startup function    */
  dword                 p_param,                /* parameter for new task   */
  char                 *p_tskname,              /* A/N string for task name */
  boolean               suspended,              /* is task initially susp?  */
  int                   dog_report_val          /* Dog report value         */
);

/**
Initializes the REX subsystem

@param[in]   hw_bitmask  --    This parameter is used to specify the hardware bitmask affinity
                               that is used for the "Main Task" (TMC task).
@param[in]   p_istack    --    This parameter is ignored.
@param[in]   p_istksiz   --    This parameter is ignored.
@param[in]   p_tcb       --    Valid TCB pointer for the "Main Task" (TMC task)
@param[in]   p_stack     --    Stack base pointer for the "Main Task" (TMC task)
@param[in]   stack_size  --    Stack size for for the "Main Task" (TMC task)
@param[in]   priority    --    Priority for for the "Main Task" (TMC task)
@param[in]   p_task      --    Pointer to the task entry function.
@param[in]   param       --    Parameter to be passed into the task entry function.

@return
None.

@dependencies
Heap initialization should have taken place as this can allocate
memory dynamically.
*/
void rex_init(
  unsigned int           hw_bitmask,         /* hardware bitmask     */
  void *                 p_istack,           /* interrupt stack      */
  rex_stack_size_type    istksiz,            /* interrupt stack size */
  rex_tcb_type           *p_tcb,             /* task control block   */
  void *                 p_stack,            /* stack                */
  rex_stack_size_type    stack_size,         /* stack size           */
  rex_priority_type      priority,           /* task priority        */
  rex_task_func_type     p_task,             /* task function        */
  dword                  param               /* task parameter       */
);

/**
This function will kill all REX tasks. This is used in core shutdown sequence

@note This function will not return at all (as the calling task goes away).

@return
None.

@dependencies
None.
*/
void rex_kill_all(void);

/**
This function is used when a non-REX task/thread needs a REX context in order to use
REX APIs and services.

@param[in]   p_tcb       -- Pointer to the REX TCB which will needs to obtain a REX context.
@param[in]   stack_limit -- The stack base address of the thread.
@param[in]   stack_size  -- The stack size of the thread.

@return
None.

@dependencies
Data structures may be allocated during this call. rexl4_fake_tcb_destroy() should be
called with the same p_tcb when the thread is destroyed, to prevent any memory leaks.
*/
void rexl4_fake_tcb(rex_tcb_type *p_tcb, void *stack_limit, uint32 stack_size);
#define l4rex_fake_tcb rexl4_fake_tcb /* temporary backwards compatiblity */

void * rexqube_fake_tcb
(
   rex_tcb_type *      p_tcb,          /*!< valid tcb for new task */
   unsigned char *     stack_limit,    /*!< stack for new task */
   rex_stack_size_type stack_size      /*!< stack size in "bytes" */
);

void * rex_create_fake_tcb(void *stack_addr, size_t stack_size, char* taskname);
void rex_destroy_fake_tcb(void * fake_tcb);

/**
This function is used to cleanup a fake REX tcb that was initialized
using rexl4_fake_tcb.

@param[in]   p_tcb       -- Pointer to the REX TCB which will needs to relinquish its
                           REX context.

@return
None.

@dependencies
The non REX thread should have called rexl4_fake_tcb() when creating the REX context.
*/
void rexl4_fake_tcb_destroy(rex_tcb_type *p_tcb);

//This was added since TMC does a disable before all tasks are spawned and then enable.
//Modem code uses this. Will eventually move to restricted API.
/**
Enables REX INTLOCK profiling

@return
None.

@note The functionality of this API is controlled by cust feature FEATURE_REX_STAT.
@note The underlying OS needs to support this funtionality. Check the OS documentation
to see if this API is supported or not.

@dependencies
None.
*/
void rex_stat_disable(void);

/**
Disables REX INTLOCK profiling

@return
None.

@note The functionality of this API is controlled by cust feature FEATURE_REX_STAT.
@note The underlying OS needs to support this funtionality. Check the OS documentation
to see if this API is supported or not.

@dependencies
None.
*/
void rex_stat_enable(void);
#define REX_STAT_DISABLE() rex_stat_disable()
#define REX_STAT_ENABLE()  rex_stat_enable()

#ifdef __cplusplus
   }
#endif

#endif //__REX_OS_H__
