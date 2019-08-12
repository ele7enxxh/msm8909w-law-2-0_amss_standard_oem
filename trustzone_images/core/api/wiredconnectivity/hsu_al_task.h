#ifndef HSU_AL_TASK_H
#define HSU_AL_TASK_H

/*=============================================================================

                          H S U _ A L _ T A S K . H

=============================================================================*/
/**
@file hsu_al_task.h

  Contians the High-Speed USB implementation of the Adaptation Layer
  Command Task.

  The Adaptation Layer Command Task is a REX task in charge of executing 
  High-Speed USB adaptation layer commands. An adaptation layer command is a
  code section that must be executed after the occurrence of an interrupt, but
  cannot be executed within the interrupt context itself (for example, when
  this code section is blocking). Therefore, the interrupt service routine
  queues a command in the commands queue. Later, another task (the Adaptation
  Layer task) retrieves this item from the command queue and executes the
  command outside the interrupt context.

  This task manages a cyclic queue of commands. Each queue entry contains a
  function address and an actual parameter for this function, which is of the
  type void*. When commands are inserted into the queue (using the
  hsu_al_task_enqueue_cmd function), a signal is sent to this task to awaken
  it. This task then processes all the queued commands in a FIFO order. For
  each queued command, this task calls the supplied function, passing it its
  argument.

  While this task is processing command queue items, the Giant mutex is locked.
  When this task finishes processing the queue (i.e., it becomes empty again),
  this task releases the Giant mutex and resumes waiting for its signal.

  The command queue is implemented using a cyclic buffer that has a head index
  and a tail index. The tail index points to the location of the next inserted
  item, and the head index points to the location of the next popped item. When
  head index == tail index, the queue is empty, and when head index == tail
  index + 1, the queue is full. This way, the queue can be managed without
  mutex protection, since no item counter is needed and there is no
  simultaneous access to the same end of the queue.

  Externalized functions
  - hsu_al_task_main is externalized outside of the HS-USB stack
    adaptation layer.

  - hsu_al_task_enqueue_cmd is externalized internally in the HS-USB
    stack adaptation layer.
   
  Initialization and sequencing requirements:\n
  The HS-USB adaptation layer task must be created before using the
  API defined in this file.
*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_al_task.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who     what, where, why
  --------  ---     -----------------------------------------------------------
  06/24/10  lo      (Tech Pubs) Edited/added Doxygen comments and markup.
  09/18/06  ds      Created.
     
=============================================================================*/

/*=============================================================================
Copyright (c) 2006-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include <rex.h>

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================
  Constants and Macros
=============================================================================*/
/** HS-USB AL task name.
*/
#define HSU_AL_TASK_NAME "HSU AL TASK"

/* Enumeration of HS-USB AL task stack size.
*/
enum
{
  AL_TASK_STACK_SIZE_IN_BYTES = 2048
};

/** HS-USB AL task stack TCB.
*/
extern rex_tcb_type   hsu_al_task_tcb;

/** HS-USB AL task stack.
*/
extern unsigned char  hsu_al_task_stack[AL_TASK_STACK_SIZE_IN_BYTES];

/*==============================================================================
  Typedefs
=============================================================================*/
/** HS-USB AL task entry point.
*/
typedef void (*hsu_al_task_cmd_func_type) (void*);

/*=============================================================================
  Externalized Variables
=============================================================================*/


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================
FUNCTION    hsu_al_task_enqueue_cmd
=============================================================================*/
/**
  Enters a new command into the HS-USB adaptation layer command queue
  for execution by the High-Speed USB adaptation layer task.

@param command_func  [IN] -- Command to handle.
@param *command_arg  [IN] -- Parameter to provide to the handler.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_task_enqueue_cmd(hsu_al_task_cmd_func_type  command_func,
                             void*                      command_arg);

/*=============================================================================
FUNCTION    hsu_al_task_enqueue_cmd_ex
=============================================================================*/
/**
  Enters a new command into the HS-USB adaptation layer command queue
  for execution by the High-Speed USB adaptation layer task.

@param command_func         [IN] -- Command to handle.
@param *command_arg         [IN] -- Parameter to provide to the handler.
@param acquire_giant_mutex  [IN] -- Whether to lock the giant mutex before calling the command

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_task_enqueue_cmd_ex(hsu_al_task_cmd_func_type  command_func,
                             void*                      command_arg,
                             boolean                    acquire_giant_mutex);

/*=============================================================================
FUNCTION    hsu_al_task_main
=============================================================================*/
/**
  The adaptation layer command task's main function.
  
  Infinitely waits for new AL task commands to be entered into the command
  queue (this event is notified by a REX signal) and executes them.
  Additionally, this function handles common AMSS signals (watchdog task
  signals, offline, and stop).

@param param [IN] -- Generic unused parameter for task entry point.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_task_main
(
  unsigned long param 
);  /* hsu_al_task_main */

/** @} */ /* endgroup hsusb_qc_api */

#endif /* HSU_AL_TASK_H */
