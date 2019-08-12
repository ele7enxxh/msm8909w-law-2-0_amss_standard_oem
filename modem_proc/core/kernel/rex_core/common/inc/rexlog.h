#ifndef REXLOG_H
#define REXLOG_H
/*===========================================================================

                   L O G  P A C K E T S  F O R  R E X

DESCRIPTION
  This header file contains the definitions of log packets for task
  profiling, memory debug, code coverage, core dump, etc.
 
Copyright (c) 2002-2011      by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/kernel/rex_core/common/inc/rexlog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/19/07   cz      Adding REX signal extension support.
03/31/04   tbg     Added ENUM for rex_core log, to simplify referencing the
                   various registers.
07/20/02    gr     Created

===========================================================================*/

/* -----------------------------------------------------------------------
** Includes 
** ----------------------------------------------------------------------- */
#include "comdef.h"
#include "log.h"
#include "memheap.h"
#include "rex.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/*============================================================================

PACKET   Memory Debug Information

ID       LOG_MEMDEBUG_C

DESCRIPTION
  Contains memory debug information.

  alloc_num:       A unique ID for this memory allocation operation

  mem_ptr:         A pointer to the allocated memory

  size:            The size in bytes of the allocated memory

  alloc_tcb_id:    The ID of the task that performed this particular
                   allocation

  alloc_line_num:  The line number in the file where the call to allocate
                   memory was made 

  alloc_heap_used: The number of bytes used in the heap after the memory
                   allocation

  free_tcb_id:     The ID of the task that freed the allocated memory. This
                   will be zero if the memory has not been freed yet.

  free_line_num:   The line number in the file where the call to free
                   memory was made. This will be zero if the memory has not
                   been freed yet.

  free_heap_used:  The number of bytes used in the heap just before the memory
                   was freed.  This will be zero if the memory has not been
                   freed yet.
 
  status:          A field of type mem_debug_status_type that indicates the
                   status of the allocation. The possible values are
                   MEM_DEBUG_STATUS_OK             = 1
                   MEM_DEBUG_STATUS_MULTIPLY_FREED = 2
                   MEM_DEBUG_STATUS_BAD_ADDRESS    = 3
                   MEM_DEBUG_STATUS_OVERRUN        = 4
                   MEM_DEBUG_STATUS_POSSIBLE_LEAK  = 5

  heap_name:       A null-terminated string containing the name of the heap
                   from which memory was allocated

  alloc_file_name: A null-terminated string containing the name of the file
                   from which the call to allocate memory was made.

  free_file_name:  A null-terminated string containing the name of the file
                   from which the call to free memory was made. This string
                   will be empty if the memory has not been freed yet.

============================================================================*/

LOG_RECORD_DEFINE(LOG_MEMDEBUG_C)

   uint32   alloc_num;
   void    *mem_ptr;
   uint32   size;
   uint32   alloc_tcb_id;
   uint32   alloc_line_num;
   uint32   alloc_heap_used;
   uint32   free_tcb_id;
   uint32   free_line_num;
   uint32   free_heap_used;
   uint32   status;
   char     heap_name[1];         /* variable length field */
   char     alloc_file_name[1];   /* variable length field */
   char     free_file_name[1];    /* variable length field */

LOG_RECORD_END


/*============================================================================

PACKET   System Level Task Profiling Information

ID       LOG_SYSPROFILE_C

DESCRIPTION
  Contains system-level information such as the number of tasks and their
  ids, total time samples elapsed etc.

  total_num_tasks:    Total number of tasks in the system (in REX's task list).

  task_id:      A unique identifier for a task.

  stack_size:   The stack size (in number of bytes) for a task. For an ISR,
                this will be the size of the system stack.

  task_sl:      Stack limit value. This value will be limit of the system
                stack for an ISR.

  max_intlock_time: The maximum time for which a task or ISR has kept
                interrupts locked.

  max_intlock_lr: The value of the link register during the call to
                rex_int_lock that corresponds to the maximum lock time for a
                task or ISR. This indicates from where the call to
                rex_int_lock was made.

  task_name:    Null-terminated string name of task or ISR up to a max of 
                LOG_REX_SYSPROFILE_TASK_NAME_LEN characters - 1 

============================================================================*/

#define LOG_REX_SYSPROFILE_TASK_NAME_LEN 16  

LOG_RECORD_DEFINE(LOG_SYSPROFILE_C)

   uint32 total_num_tasks;
   uint32 task_id;
   uint32 stack_size;
   uint32 stack_limit;
   uint32 max_intlock_time;
   uint32 max_intlock_lr;
   char   task_name[LOG_REX_SYSPROFILE_TASK_NAME_LEN]; 

LOG_RECORD_END

/*============================================================================

PACKET   BLAST System Level Task Profiling Information

ID       LOG_BLAST_SYSPROFILE_C

DESCRIPTION
  Contains system-level information such as the number of tasks and their
  ids, total time samples elapsed etc.

  ver:          Version no. to handle future changes. For this format
                ver = 1

  rsvd:         3 bytes reserved to force 4-byte alignment to rest of fields

  total_num_tasks:    Total number of tasks in the system (in REX's task list).

  task_id:      A unique identifier for a task.

  stack_size:   The stack size (in number of bytes) for a task. For an ISR,
                this will be the size of the system stack.

  task_sl:      Stack limit value. This value will be limit of the system
                stack for an ISR.

  max_intlock_time: The maximum time for which a task or ISR has kept
                interrupts locked.

  max_intlock_lr: The value of the link register during the call to
                rex_int_lock that corresponds to the maximum lock time for a
                task or ISR. This indicates from where the call to
                rex_int_lock was made.

  task_name:    Null-terminated string name of task or ISR up to a max of 
                LOG_REX_SYSPROFILE_TASK_NAME_LEN characters - 1 

============================================================================*/
LOG_RECORD_DEFINE(LOG_BLAST_SYSPROFILE_C)

   uint8  ver;
   uint8  rsvd[3];
   uint32 total_num_tasks;
   uint32 task_id;
   uint32 stack_size;
   uint32 stack_limit;
   uint32 max_intlock_time;
   uint32 max_intlock_lr;
   char   task_name[LOG_REX_SYSPROFILE_TASK_NAME_LEN]; 

LOG_RECORD_END

/*============================================================================

PACKET   Per Task Profiling Information

ID       LOG_TASKPROFILE_C

DESCRIPTION
  Contains profiling information for a specific task. The same packet is
  also used to send profiling information about ISRs. Many of the fields
  will be zero or null for ISRs because the fields do not make sense in
  that context.

  task_id:    Unique identifier for the task or ISR.

  sp:         The stack pointer stored in the TCB of the task (except for
              the current task, for which we return the contents of the
              stack pointer register)
  
  sigs:       The value of the sigs field from the task's TCB. This represents
              the mask of signals set for the task.

  wait:       The value of the wait field from the task's TCB. This represents
              the mask of signals on which the task is currently waiting.

  pri:        The value of the pri field from the task's TCB. This denotes
              the priority of the task.

  time_samples: A measure of the CPU time used by the task. This field contains
              the number of time samples during which this task was running,
              if this is available. Otherwise this field is set to the
              value of the slices field in the TCB. The slices field denotes
              the number of times a task has been scheduled. Bit 2 of the
              info_mask field determines which of the two meanings applies.

  total_samples: The total number of time samples elapsed so far, or the total
                 number of scheduling operations performed by REX so far. Bit 2 
                 of the info_mask field determines which of the two meanings
                 applies.
	
  flags:
                 Bit [ 0 ] - interrupt state
                             1 - interrupts are locked
                             0 - interrupts are not locked
									
                 Bit [ 1 ] - suspend state
                             1 - task is suspended (by way of a request
                                 via rex_suspend_task)
                             0 - task is not suspended
									 
                 Bit [ 2 ] - slice type
                             0 - time_samples field contains the value of the
                                 slices field in the TCB. The slices field is a
                                 count of the number of times a task has been
                                 scheduled, and is a very rough measure of the
                                 CPU time allotted to a task.
                             1 - time_samples field contains more accurate profiling
                                 information generated by using a timer to
                                 measure how long the task has run. The bit is 0
                                 typically if the build does not have software task
                                 profiling turned on.
============================================================================*/

#define LOG_TASKPROFILE_REX_INT_STATE_BIT  0x01
#define LOG_TASKPROFILE_REX_SUSPENDED_BIT  0x02
#define LOG_TASKPROFILE_REX_SLICE_TYPE_BIT 0x04

LOG_RECORD_DEFINE(LOG_TASKPROFILE_C)

   uint32    task_id;
   uint32    sp;
   rex_sigs_type    sigs;
   rex_sigs_type    wait;
   uint32    pri;
   uint32    time_samples;
   uint32    total_samples;
   uint8     flags;
   
LOG_RECORD_END

/*============================================================================

PACKET   BLAST Per Task Profiling Information

ID       LOG_TASKPROFILE_C

DESCRIPTION
  Contains profiling information for a specific task. The same packet is
  also used to send profiling information about ISRs and HW threads. Many
  of the fields will be zero or null for ISRs because the fields do not
  make sense in that context.

  ver:        Version no. to handle future changes. For this format
              ver = 1

  app:        Field to distinguish multiple BLAST systems on the same
              target.
              0 - Modem SW
              1 - Modem FW
              2 - Multi Media

  special_task: Field to indicate profiling information not to related to
              regular tasks.
              0 - regular task
              1 - IDLE thread, consists of IDLE time of HW threads.

  task_id:    Unique identifier for the task or ISR. task_id is "-1" for
              special tasks.

  sp:         Same as LOG_TASKPROFILE_C

  sigs:       Same as LOG_TASKPROFILE_C

  wait:       Same as LOG_TASKPROFILE_C

  pri:        The value of the pri field from the task's TCB. This denotes
              the priority of the task.

  time_samples[x]: A measure of the HW thread time used by the task. This
              field contains the number of time samples during which this
              task was running on HW thread x, if this is available.  In
              otherwords, this field is set to the value of the "totalcycles"
              field in the TCB. Since HW thread x doesn't run on each core
              clock cycle, we need to divide it by 6 to get the TCYCLES.

  core_clk_freq: The frequency of Q6 core in MHz. This is the rate at which
              time samaples increment.

  total_samples: The total number of core cycles elapsed so far.

  total_sclk_samples: The total number of sleep clock ticks elapsed so far.
              Sleep clock runs at fixed frequency of 32768 Hz.
	
  flags:      Same as LOG_TASKPROFILE_C
============================================================================*/
LOG_RECORD_DEFINE(LOG_BLAST_TASKPROFILE_C)

   uint8     ver; // curerent version = 3
   uint8     app;
   uint8     special_task;
   uint8     flags;
   uint8     hw_threads_num; // <=6
   uint8     reserved[3];   // for future use
   uint32    task_id;
   uint32    sp;
   rex_sigs_type    sigs;
   rex_sigs_type    wait;
   uint32    pri;
   uint64    total_samples;
   uint32    core_clk_freq;
   uint32    total_sclk_samples;
   uint64    time_samples[6];
   
LOG_RECORD_END

/* version 2 
LOG_RECORD_DEFINE(LOG_BLAST_TASKPROFILE_C)

   uint8     ver;
   uint8     app;
   uint8     special_task;
   uint8     flags;
   uint32    task_id;
   uint32    sp;
   rex_sigs_type    sigs;
   rex_sigs_type    wait;
   uint32    pri;
   uint64    total_samples;
   uint64    time_samples[6];
   uint32    core_clk_freq;
   uint32    total_sclk_samples;

LOG_RECORD_END
*/

/*============================================================================

PACKET   BLAST Performance Monitoring information

ID       LOG_BLAST_PM_C

DESCRIPTION
  Contains performance information collected from QDSP6 for the requested
  events.

  ver:        Version no. to handle future changes. For this format
              ver = 1

  app:        Field to distinguish multiple BLAST systems on the same
              target.
              0 - Modem SW
              1 - Modem FW
              2 - Multi Media

  rsvd:       2 bytes reseved for future use.

  counters[x]: Performance information collected from QDSP6 corresponding to
              the requested events.

  total_samples: The total number of core cycles elapsed so far.

  total_sclk_samples: The total number of sleep clock ticks elapsed so far.
              Sleep clock runs at fixed frequency of 32768 Hz.

  idle_samples[x]: A measure of the HW thread time used for running tasks.
              This field is set to how many pcycles each HW thread was
              sleeping.

	
  flags:      Same as LOG_TASKPROFILE_C
============================================================================*/
LOG_RECORD_DEFINE(LOG_BLAST_PM_C)

   uint8     ver;
   uint8     app;
   uint8     rsvd[2];
   uint32    counters[4];
   uint32    total_sclk_samples;
   uint64    total_samples;
   uint64    idle_samples[6];
   
LOG_RECORD_END

/*============================================================================

PACKET   Per Task Profiling Information

ID       LOG_TASKPROFILE_C_V2

DESCRIPTION
  Contains profiling information for a specific task. The same packet is
  also used to send profiling information about ISRs. Many of the fields
  will be zero or null for ISRs because the fields do not make sense in
  that context.

  task_id:    Unique identifier for the task or ISR.

  sp:         The stack pointer stored in the TCB of the task (except for
              the current task, for which we return the contents of the
              stack pointer register)
  
  sigs:       The value of the sigs field from the task's TCB. This represents
              the mask of signals set for the task.

  wait:       The value of the wait field from the task's TCB. This represents
              the mask of signals on which the task is currently waiting.

  pri:        The value of the pri field from the task's TCB. This denotes
              the priority of the task.

  time_samples: A measure of the CPU time used by the task. This field contains
              the number of time samples during which this task was running,
              if this is available. Otherwise this field is set to the
              value of the slices field in the TCB. The slices field denotes
              the number of times a task has been scheduled. Bit 2 of the
              info_mask field determines which of the two meanings applies.

  total_samples: The total number of time samples elapsed so far, or the total
                 number of scheduling operations performed by REX so far. Bit 2 
                 of the info_mask field determines which of the two meanings
                 applies.
	
  flags:
                 Bit [ 0 ] - interrupt state
                             1 - interrupts are locked
                             0 - interrupts are not locked
									
                 Bit [ 1 ] - suspend state
                             1 - task is suspended (by way of a request
                                 via rex_suspend_task)
                             0 - task is not suspended
									 
                 Bit [ 2 ] - slice type
                             0 - time_samples field contains the value of the
                                 slices field in the TCB. The slices field is a
                                 count of the number of times a task has been
                                 scheduled, and is a very rough measure of the
                                 CPU time allotted to a task.
                             1 - time_samples field contains more accurate profiling
                                 information generated by using a timer to
                                 measure how long the task has run. The bit is 0
                                 typically if the build does not have software task
                                 profiling turned on.
============================================================================*/
LOG_RECORD_DEFINE(LOG_TASKPROFILE_C_V2)

   uint32    task_id;
   uint32    sp;
   rex_sigs_type    sigs[REX_SIGS_ARRAY_LEN];
   rex_sigs_type    wait[REX_SIGS_ARRAY_LEN];
   uint32    pri;
   uint32    time_samples;
   uint32    total_samples;
   uint8     flags;
   
LOG_RECORD_END

#endif /* REXLOG_H */

