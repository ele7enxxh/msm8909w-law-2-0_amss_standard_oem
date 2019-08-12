#ifndef __REX_TYPES_H__
#define __REX_TYPES_H__
/*==========================================================================

                      REX TYPES HEADER FILE

DESCRIPTION
   This header contains the data type definitions required for the 
   REX - Real Time Executive


Copyright (c) 1990-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rex/rex_types.h#1 $

===========================================================================*/

/* REXTODO Will turn on deprecation warnings later */
#define REX_DEPRECATED        //__attribute__ ((deprecated))

/**
Defines the length of REX task name *exclusive* of the \n character.
*/
#define REX_TASK_NAME_LEN                       (11)

/**
Defines the bitmask that specifies a single CPU affinity.
*/
#define REX_SINGLE_CPU_AFFINITY_MASK            (0)

/**
Defines the bitmask which specifies any CPU affinity.
*/
#define REX_ANY_CPU_AFFINITY_MASK               (~0)

/**
Defines the number of uint32 words used as the REX signal bitmask.
*/
#define REX_SIGS_ARRAY_LEN                      (2)

/* ----------------------------------------------------------------------
** REX Data Types
** - These are sized based on the target
**   - Stack word, size of a word declared for stack variables
**   - Priority word, holds REX task priorities
**   - Time Count word, holds timer count downs
**   - Signal word, holds signals for a task
** ---------------------------------------------------------------------- */
typedef dword rex_stack_word_type;
typedef dword rex_stack_size_type;
typedef unsigned char *rex_stack_pointer_type;
typedef dword rex_priority_type;
typedef dword rex_timer_cnt_type;
typedef dword rex_sigs_type;
typedef dword rex_vect_type;
typedef dword rex_cpu_set_type;
typedef struct
{
   unsigned long sleep_time_samples;
   unsigned long total_time_samples;
} rex_sleep_report_type;

typedef word rex_process_id_type;

/* Function pointer type for REX tasks.
 ** The start-up function for a REX task must be of this type.
 */
typedef uint32 rex_task_func_param_type;
typedef void (*rex_task_func_type)( rex_task_func_param_type );
 
/* Type for IRQ and FIQ interrupt vectors. */ 
typedef void (*rex_isr_func_type)( void );

/* --------------------------------------------------------------------
** Asynchronous Procedure Calls
**   Call the function with the given signature in a named context
** -------------------------------------------------------------------- */
/* Signature of functions which can be called as APCs */
typedef void (*rex_apc_func_type)(rex_task_func_param_type);

/* Type for REX timer callback functions. */
typedef rex_apc_func_type rex_timer_cb_type;

typedef struct {
   rex_apc_func_type apc;
   unsigned long    apc_param;
} rex_apc_item_type;

/**
@brief
Structure to store REX task attributes when creating a REX task.

@note REX clients should not reference elements of this structure directly. Instead use the 
      REX task attribute APIs.
*/
typedef struct _rex_task_attributes_type
{
  rex_stack_pointer_type   p_orig_stack_limit;           /**< Stack start address for new task  */
  rex_stack_size_type      orig_stack_size;              /**< Stack size for new task           */
  rex_priority_type        priority;                     /**< Priority for new task             */
  rex_task_func_type       p_task_entry_func;            /**< Task entry function               */
  rex_task_func_param_type task_entry_func_param;        /**< Task entry function parameter     */
  rex_cpu_set_type         cpu_affinity;                 /**< CPU affinity for new task         */
  char                     task_name[REX_TASK_NAME_LEN+1]; /**< A /n string for task name       */
  boolean                  is_initially_suspended;       /**< Flag to indicate if the task is to be
                                                              suspended before the task entry 
                                                              function is called                */
  void                     *p_opt;                       /**< Optional pointer placeholder      */
} rex_task_attributes_type;

#endif //__REX_TYPES_H__
