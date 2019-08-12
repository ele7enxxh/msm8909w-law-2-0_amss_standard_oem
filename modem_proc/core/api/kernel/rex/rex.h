#ifndef REX_H
#define REX_H
/*=================================================================================================

                      R E X    H E A D E R    F I L E

DESCRIPTION
   API for the REX - Real Time Executive


Copyright (c) 1990-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
=================================================================================================*/

/*=================================================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rex/rex.h#1 $

=================================================================================================*/

/*=================================================================================================

                           INCLUDE FILES

=================================================================================================*/
#include "comdef.h"

#include "rex_types.h"
#include "rex_os.h"

/*=================================================================================================

                            MACRO DECLARATIONS

=================================================================================================*/
/** 
Macro define to let other services know that this version of REX supports DAC.
*/
#define HAVE_DAC_MEMORY_PROTECTION

/**
Macro define that specifies the availability of Extended REX Signal APIs.
*/
#define HAVE_REX_SIGS_EXT

/**
Macro define that specifies this header is CMI compliant. This was done for 
backwards compatibility.
*/
#define FEATURE_REX_CMI

/**
Macro that defines that APIs to support an opaque REX TCB are supported.
*/
#define FEATURE_REX_OPAQUE_TCB_APIS

/**
Macro that indicates extended timer API rex_def_timer_ex() is supported.
*/
#define FEATURE_REX_TIMER_EX

/**
Macro that indicates APIs rex_create_timer_ex() and rex_delete_timer_ex() are supported.
*/
#define FEATURE_REX_DYNA_MEM

/*--------------------------------------------------------------------------------------------------
   REX Error Codes
--------------------------------------------------------------------------------------------------*/
/**
Defines a success.
*/
#define  REX_SUCCESS                            0
/**
Defines a general error.
*/
#define  REX_ERROR                              -1
/**
Defines an invalid parameter error.
*/
#define  REX_ERROR_BAD_PARAM                    -2
/**
Defines an out of memory error.
*/
#define  REX_ERROR_NO_MEM                       -3
/**
Defines an unsupported operation error.
*/
#define  REX_ERROR_UNSUPPORTED                  -4
/**
Defines an invalid parameter error.
*/
#define  REX_ERROR_INVALID                      -5
/**
Defines a general operation/function not permitted error.
*/
#define  REX_ERROR_NOT_ALLOWED                  -6

/*=================================================================================================

                           DATA DECLARATIONS

=================================================================================================*/
/**
@brief
Data structure/handle used for timers.
*/
typedef rex_os_timer_type        rex_timer_type;
/**
@brief
Data structure used for REX critical sections.
*/
typedef rex_os_crit_sect_type    rex_crit_sect_type;

/*==================================================================================================

                     FUNCTION DECLARATIONS

==================================================================================================*/
#ifdef __cplusplus
   extern "C"
   {
#endif

/*=================================================================================================
         REX TASK ATTRIBUTE APIs
=================================================================================================*/
/**
This function initialises a REX task attributes object with the default value for all of the 
individual attributes used by the underlying OS implementation. This initialized object can then 
be used to create a REX task using API rex_common_def_task(). A single attribute object may be 
used to create multiple tasks using the aforementioned API but care must be taken to re-initialize
the task attributes that need to be changed for every task like stack address, name etc.

@note When done with the attributes object, user code must call rex_task_attr_delete().

@param[in]   p_task_attr   Valid pointer to a REX task attributes object.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates a bad parameter was passed in.
REX_ERROR            -- Indicates a general error.

@dependencies
None.
*/
int rex_task_attr_init(rex_task_attributes_type *p_task_attr);

/**
This function initializes the REX task attributes object reference 
referred to by p_task_attr with the actual attribute values describing 
an active REX task.

@param[in]   p_tcb         Valid pointer to a REX TCB.
@param[in]   p_task_attr   Valid pointer to a REX task attributes object.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.
REX_ERROR            -- Indicates that an invalid REX TCB pointer was used

@dependencies
None.
*/
int rex_task_get_attr(rex_tcb_type *p_tcb, rex_task_attributes_type *p_task_attr);

/**
This function is used to specify the address for a stack region to be used by a new REX task.

@param[in]  p_task_attr    Valid pointer to a REX task attributes object.
@param[in]  p_stack        Pointer to a region of memory to be used as the task's stack area. Some
                           OS implementations may support a null stack value which would imply that 
                           the underlying OS will allocate and manage the stack address. Users 
                           should check the return value when using this API.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.
REX_ERROR_INVALID    -- Indicates that the specified attribute value is invalid.

@dependencies
rex_task_attr_init() must have been called on the input REX task attributes pointer prior to using 
this function.

@sa
rex_task_attr_set_stack_addr
*/
int rex_task_attr_set_stack_addr(rex_task_attributes_type *p_task_attr, 
                                    rex_stack_pointer_type p_stack);

/**
This function is used to specify the size of stack region to be used by a new REX task.

@param[in]  p_task_attr    Valid pointer to a REX task attributes object.
@param[in]  stack_size     Size of the stack in number of bytes. The size of the stack should be 
                           at least REX_TASK_MIN_STACK_SIZE. By default a stack size of 
                           REX_TASK_DEFAULT_STACK_SIZE will be used when creating a task in case the
                           user does not call this API to explicitly set a specific size.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.
REX_ERROR_INVALID    -- Indicates that the specified attribute value is invalid.

@dependencies
rex_task_attr_init() must have been called on the input REX task attributes pointer prior to using 
this function.

@sa
rex_task_attr_set_stack_addr
*/
int rex_task_attr_set_stack_size(rex_task_attributes_type *p_task_attr, 
                                    rex_stack_size_type stack_size);

/**
This accessor function returns the stack start address and size values 
present in the REX task attributes object reference referred to by p_task_attr.

@param[in]  p_task_attr    Valid pointer to a REX task attributes object.
@param[out] pp_stack       Valid pointer to a rex_stack_pointer_type data type in which the 
                           start stack address will be copied over.
@param[out] p_stack_size   Valid pointer to a rex_stack_size_type data type in which the 
                           stack size will be copied over.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.

@dependencies
None.
*/
int rex_task_attr_get_stack_attr
(
   rex_task_attributes_type *p_task_attr, 
   rex_stack_pointer_type *pp_stack,
   rex_stack_size_type *p_stack_size
);

/**
This function is used to specify the priority of a new REX task.

@param[in]  p_task_attr Valid pointer to a REX task attributes object.
@param[in]  priority    A valid priority for the new task. As far as REX is concerned, priorities 
                        are to be specified in ascending order i.e. a priority of 10 will create a
                        task with a lower priority than a task with priority 11. Please note the 
                        underlying OS may or may not follow this priority scheme but will adhere to 
                        this priority policy when creating a task. Priority must be specified within
                        REX_TASK_MIN_PRIORITY and REX_TASK_MAX_PRIORITY.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.
REX_ERROR_INVALID    -- Indicates that the specified attribute value is invalid.

@dependencies
rex_task_attr_init() must have been called on the input REX task attributes pointer prior to using 
this function.
*/
int rex_task_attr_set_prio(rex_task_attributes_type *p_task_attr, rex_priority_type priority);

/**
This accessor function returns the priority value present in the 
REX task attributes object referred to by p_task_attr.

@param[in]  p_task_attr    Valid pointer to a REX task attributes object.
@param[out] p_priority     Valid pointer to a rex_priority_type data type in which the 
                           priority will be copied over.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.

@dependencies
None.
*/
int rex_task_attr_get_prio(rex_task_attributes_type *p_task_attr, rex_priority_type *p_priority);

/**
This function is used to specify a REX task's entry function and an input paramter to be passed 
into the entry function when the REX task begins execution.

@param[in]  p_task_attr       Valid pointer to a REX task attributes object.
@param[in]  p_task_entry_func Valid rex_task_func_type which is essentially a pointer to 
                              the REX task entry (or startup) function.
@param[in]  param             User defined parameter value for the task entry function.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.

@dependencies
rex_task_attr_init() must have been called on the input REX task attributes pointer prior to using 
this function.
*/
int rex_task_attr_set_entry_func
(
   rex_task_attributes_type *p_task_attr, 
   rex_task_func_type p_task_entry_func, 
   rex_task_func_param_type param
);

/**
This function is used to specify whether the REX task is to be suspended (halted) before its
task entry function is invoked. Eventually, the task can resume execution by calling 
rex_resume_task() on the suspended task.

@param[in]  p_task_attr    Valid pointer to a REX task attributes object.
@param[in]  suspended      Flag to indicate whether the task should be started in a suspended state
                           or not. A value of FALSE would imply that the task should not be 
                           suspended and TRUE would imply that it should. It should be noted that 
                           the underlying OS may not support the starting of the task in suspended
                           state and users should check the return value for this.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.
REX_ERROR_INVALID    -- Indicates that the specified attribute value is invalid.

@dependencies
rex_task_attr_init() must have been called on the input REX task attributes pointer prior to using 
this function.
*/
int rex_task_attr_set_initially_suspended(rex_task_attributes_type *p_task_attr, boolean suspended);

/**
This function is used to specify a name for a REX task.

@param[in]  p_task_attr    Valid pointer to a REX task attributes object.
@param[in]  p_task_name    A null terminated character buffer containing the task name. Characters
                           from p_task_name will be copied over until a null character is encountered
                           or a max of (REX_TASK_NAME_LEN+1) characters inclusive of the null 
                           termination are copied over. Any excess characters will be ignored. 
                           If a null pointer is specified, a default name will be assigned 
                           to the task.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.

@dependencies
rex_task_attr_init() must have been called on the input REX task attributes pointer prior to using 
this function.
*/
int rex_task_attr_set_task_name(rex_task_attributes_type *p_task_attr, char *p_task_name);
/**
This accessor function returns the task name present in the 
REX task attributes object referred to by p_task_attr.

@param[in]  p_task_attr    Valid pointer to a REX task attributes object.
@param[out] dest           Non null character buffer which will contain the task name. 
@param[in]  size           Size of the input buffer dest.
@param[out] p_len          Pointer to a size_t type in which the length of task name will be 
                           returned. The returned length would be similar to what strlen would return. 
                           A null value may be passed in by the user in case there is no interest.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.

@dependencies
None.

@sa rex_get_task_name
*/
int rex_task_attr_get_task_name
(
   rex_task_attributes_type *p_task_attr, 
   char *dest,
   size_t size,
   size_t *p_len
);

/**
This function is used to specify a CPU affinity for a REX task. The affinity is specified via a 
bitmask pattern. Ex. if bitmask 0...010001 is specified, the task is bound to CPUs 1 and 5.
A value of zero corresponds to REX_SINGLE_CPU_AFFINITY_MASK which implies that the task will be
bound to a single system defined CPU. This has been done for backwards compatibility reasons.

@param[in]  p_task_attr    Valid pointer to a REX task attributes object.
@param[in]  cpu_bitmask    A bitmask pattern that specifies what CPU(s) to bind the task to. REX
                           has provided the following bitmask patterns for use with this API:
                           REX_ANY_CPU_AFFINITY_MASK specifies that the task can be scheduled on 
                           any core. REX_SINGLE_CPU_AFFINITY_MASK specifies that the task is to be
                           scheduled on a single predefined core as specified by the OS. If the user
                           does not call this API to specify a CPU affinity, the default 
                           behavior is as if the CPU bitmask were set to REX_ANY_CPU_AFFINITY_MASK.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.
REX_ERROR_INVALID    -- Indicates that the specified attribute value is invalid.

@dependencies
rex_task_attr_init() must have been called on the input REX task attributes pointer prior to using 
this function.
*/
int rex_task_attr_set_cpu_affinity
(
   rex_task_attributes_type   *p_task_attr, 
   unsigned int               cpu_bitmask
);

/**
This accessor function returns the CPU affinity bitmask present in the 
REX task attributes object referred to by p_task_attr.

@param[in]  p_task_attr    Valid pointer to a REX task attributes object.
@param[out] p_cpu_bitmask  Valid pointer to an unsigned int in which the 
                           CPU affinity bitmask will be returned.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.

@dependencies
None.
*/
int rex_task_attr_get_cpu_affinity
(
   rex_task_attributes_type   *p_task_attr,
   unsigned int               *p_cpu_bitmask
);

/**
This function returns size of data type rex_tcb_type. This is useful
for client code that need to deliver libraries and ensure they work
accross REX ports. 

@return
Size of data type rex_tcb_type.

@dependencies
None.
*/
int rex_get_rex_tcb_type_size(void);

/**
This function returns size of data type rex_crit_sect_type. 
This is useful for client code that need to deliver libraries 
and ensure they work across REX ports. 

@return
Size of data type rex_crit_sect_type.

@dependencies
None.
*/
int rex_get_rex_crit_sect_type_size(void);

/**
This function returns size of data type rex_timer_type. 
This is useful for client code that need to deliver libraries 
and ensure they work across REX ports. 

@return
Size of data type rex_timer_type.

@dependencies
None.
*/
int rex_get_rex_timer_type_size(void);

/*=================================================================================================
         REX TASK APIs
=================================================================================================*/
/**
Function to create a REX Task.

@param[in]  p_tcb       Valid pointer to a REX TCB.
@param[in]  p_attr      Valid pointer to a REX task attributes data type.

@return
REX_SUCCESS             -- Indicates that a task was successfully created.
REX_ERROR_BAD_PARAM     -- Indicates that bad input parameters were specified.
REX_ERROR_NO_MEM        -- Indicates an out of memory error.
REX_ERROR_UNSUPPORTED   -- Indicates that the specified attributes cannot be supported by the OS.
REX_ERROR_INVALID       -- Indicates that the specified attributes are invalid.
REX_ERROR               -- Indicates an error for all other failures.

@dependencies
None.

@sideeffects
The task starts running only if it is the highest priority ready task. \n
The TCB passed in will be zero'ed out. \n
REX may change the stack base address to suit the platform alignment requirements.
*/
int rex_common_def_task(rex_tcb_type *p_tcb, rex_task_attributes_type *p_attr);

/**
Creates a REX Task.

@param[in]  p_tcb       Valid pointer to a REX TCB.
@param[in]  p_stack     Non null pointer to a stack block.
@param[in]  stack_size  Size of the stack block in units of rex_stack_word_type
@param[in]  priority    A valid priority for the new task. As far as REX is concerned, priorities 
                        are to be specified in ascending order i.e. a priority of 10 will create a
                        task with a lower priority than a task with priority 11. Please note the 
                        underlying OS may or may not follow this priority scheme but will adhere to 
                        this priority policy when creating a task. Priority must be specified within
                        REX_TASK_MIN_PRIORITY and REX_TASK_MAX_PRIORITY.
@param[in]  p_task      Pointer to task entry function.
@param[in]  param       Parameter to be passed into the task entry function.

@note
This API has been deprecated and should not be used for new code. Please use rex_common_def_task()
instead.
  
@return
None.

@dependencies
None.

@sideeffects
The task starts running only if it is the highest priority ready task. \n
In case bad parameters are passed in or an error is encountered, this function will assert.
The TCB passed in will be zero'ed out. \n
REX may change the stack base address to suit the platform alignment requirements.
*/
void rex_def_task
(
   rex_tcb_type               *p_tcb,
   void                       *p_stack,
   rex_stack_size_type        stack_size,
   rex_priority_type          priority,
   rex_task_func_type         p_task,
   rex_task_func_param_type   param
) REX_DEPRECATED;

/**
Creates a REX Task. This function is similar to rex_def_task() except that it accepts two more input 
parameters, task name and initially suspended flag. Additionally, the stack size is to be specified
in units of bytes.

@param[in]  p_tcb       Valid pointer to a REX TCB.
@param[in]  p_stack     Non null pointer to a stack block.
@param[in]  stack_size  Size of the stack block in units of bytes
@param[in]  priority    A valid priority for the new task. As far as REX is concerned, priorities 
                        are to be specified in ascending order i.e. a priority of 10 will create a
                        task with a lower priority than a task with priority 11. Please note the 
                        underlying OS may or may not follow this priority scheme but will adhere to 
                        this priority policy when creating a task. Priority must be specified within
                        REX_TASK_MIN_PRIORITY and REX_TASK_MAX_PRIORITY.
@param[in]  p_task      Pointer to the task entry function.
@param[in]  param       Parameter to be passed into the task entry function.
@param[in]  task_name   A null terminated string with the task name. A max of REX_TASK_NAME_LEN+1
                        characters inclusive of the null character will be used to name the task. Any 
                        excess characters will be ignored. If a null pointer is specified, a default
                        name will be used.
@param[in]  suspended   Flag that indicates whether the task should be started in suspended mode or 
                        not. If suspended, the task's entry function will not be called and the task
                        will be in a suspended state. To continue execution of the task,
                        rex_resume_task() should be called on the suspended task.

@note
This API has been deprecated and should not be used for new code. Please use rex_common_def_task()
instead.
  
@return
None.

@dependencies
None.

@sideeffects
The task starts running only if it is the highest priority ready task and not suspended. \n
In case bad parameters are passed in or an error is encountered, this function will assert. \n
The TCB passed in will be zero'ed out. \n
REX may change the stack base address to suit the platform alignment requirements.
*/
void rex_def_task_ext
(
   rex_tcb_type               *p_tcb,
   rex_stack_pointer_type     p_stack,
   rex_stack_size_type        stack_size,
   rex_priority_type          priority,
   rex_task_func_type         p_task,
   rex_task_func_param_type   param,
   char                       *task_name,
   boolean                    suspended
) REX_DEPRECATED;

/**
Creates a REX Task. This function is similar to rex_def_task_ext() except that it accepts an 
additional parameter.

@param[in]  dog_report_val    The use of this parameter has now been deprecated and is igonred.
                              Formerly this parameter was used to setup the watchdog services 
                              for the new task at the time of creating a task. This has now
                              been deprecated and tasks are explicitly required to register
                              with the watchdog service.

@note Please see the documentation for rex_def_task_ext() for a description of the other input
parameters and side effects.
@note
This API has been deprecated and should not be used for new code. Please use rex_common_def_task()
instead.
  
@return
None.

@dependencies
None.

@sideeffects
Please see the documentation for rex_def_task_ext().
*/
void rex_def_task_ext2
(
   rex_tcb_type               *p_tcb,
   rex_stack_pointer_type     p_stack,
   rex_stack_size_type        stack_size,
   rex_priority_type          priority,
   rex_task_func_type         p_task,
   rex_task_func_param_type   param,
   char                       *task_name,
   boolean                    suspended,
   int                        dog_report_val
) REX_DEPRECATED;

/**
Kills/destroys a REX task.

@param[in]  p_tcb       Valid pointer to a REX TCB.

@note Behavior is undefined if the same task is killed multiple times.

@return
None.

@dependencies
None.

@sideeffects
In case a bad parameter is passed in or an error is encountered, this function will assert. \n
The TCB passed in will be zero'ed out.
*/
void rex_kill_task(rex_tcb_type *p_tcb);
#define rex_remove_task rex_kill_task

/**
Fills in the name of the task for the provided TCB reference in the user provided 
destination buffer. This function takes the size of the destination buffer and 
copies at most (size - 1) or REX_TASK_NAME_LEN characters into dest whichever 
is smaller and always guarantees to null terminate the string. 
Note, the maximum permitted size of a REX task name is (REX_TASK_NAME_LEN + 1).

@param[in]     p_tcb       Valid pointer to a REX TCB for which the task name needs to be obtained.
@param[out]    dest        Non null character buffer which will contain the task name. 
@param[in]     size        Size of the input buffer dest.
@param[out]    p_len       Pointer to a size_t type in which the length of task name will be 
                           returned. The returned length would be similar to what strlen would return. 
                           A null value may be passed in by the user in case there is no interest.

@return
REX_SUCCESS          -- Indicates a success.
REX_ERROR_BAD_PARAM  -- Indicates bad parameters were passed in.
REX_ERROR_INVALID    -- Indicates an invalid TCB reference was passed in.

@dependencies
None.
*/
int rex_get_task_name(rex_tcb_type *p_tcb, char *dest, size_t size, size_t *p_len);

/**
Suspends a task so that it is not eligible for scheduling. If the task being suspended is the 
current task, then a context switch will occur.

@param[in]  p_tcb       Valid pointer to a REX TCB.

@note If this function is called to suspend a task that is in IRQ mode, an assertion will 
be triggered.

@return
None.

@dependencies
None.

@sideeffects
In case a bad parameter is passed in or an error is encountered, this function will assert.
*/
void rex_suspend_task(rex_tcb_type *p_tcb);

/**
Resumes a REX task that was either started in the suspended mode or suspended during the course of 
its execution by calling rex_suspend_task(). 

@param[in]  p_tcb       Valid pointer to a REX TCB.

@note If this function is called to resume a task that is in IRQ mode, an assertion will 
be triggered as IRQ tasks cannot be suspended.
@note If a task is not suspended, this function has no effect.

@return
None.

@dependencies
None.

@sideeffects
In case a bad parameter is passed in or an error is encountered, this function will assert. \n
If the resumed task is of higher priority than the calling task, then a context switch will occur.
*/
void rex_resume_task(rex_tcb_type *p_tcb);

/**
Blocks the current thread for a specified time. The thread becomes schedulable after the specified 
time has elapsed. This is equivalent to creating a REX timer and calling rex_timed_wait on the 
timer with only the associated timer signal being waited upon

@param[in] msecs        Time in milliseconds to force the task to sleep.

@return
None.

@dependencies
None.

@sideeffects
None.
*/
void rex_sleep(unsigned long msecs);

/**
This function causes the calling REX task to yield, thereby causing the OS to reschedule.

@return
None.

@dependencies
None.

@sideeffects
Reschedule any runnable highest priority threads in system.
*/
void rex_yield(void);

/**
Sets the priority of the specified task to the input priority value.

@param[in]  p_tcb       Valid pointer to a REX TCB.
@param[in]  priority    A valid priority for the new task. As far as REX is concerned, priorities 
                        are to be specified in ascending order i.e. a task with priority of 10 
                        is lower than a task with priority 11. Please note the underlying OS may 
                        or may not follow this priority scheme but will adhere to this priority 
                        policy when changing the priority. Priority must be specified within
                        REX_TASK_MIN_PRIORITY and REX_TASK_MAX_PRIORITY.

@return
Previous priority of the task.

@note rex_task_pri() has been deprecated and mapped to rex_set_task_pri. This has been done for 
backward compatibility. Please use rex_set_task_pri for new code.

@dependencies
None.

@sideeffects
The priority change will cause a change in the whole scheduling order for all of the tasks. This 
could result in an immediate task swap.
*/
rex_priority_type rex_set_task_pri(rex_tcb_type *p_tcb, rex_priority_type priority);
#define rex_task_pri rex_set_task_pri

/**
Sets the current task's (i.e. the calling task's) priority to the specified input priority value.

@param[in]  priority    A valid priority for the new task. As far as REX is concerned, priorities 
                        are to be specified in ascending order i.e. a task with priority of 10 
                        is lower than a task with priority 11. Please note the underlying OS may 
                        or may not follow this priority scheme but will adhere to this priority 
                        policy when changing the priority. Priority must be specified within
                        REX_TASK_MIN_PRIORITY and REX_TASK_MAX_PRIORITY.
@return
Previous priority of the task.

@dependencies
None.

@sideeffects
The priority change will cause a change in the whole scheduling order for all of the tasks. This 
could result in an immediate task swap.
*/
#define rex_set_pri(priority) rex_set_task_pri(rex_self(), priority)

/**
Get the priority of the current task.

@return
Current priority of the task.

@dependencies
None.

@sideeffects
None.
*/
rex_priority_type rex_get_pri(void);

/**
Gets a pointer to the TCB of the currently running task.

@return
Pointer to the TCB of the calling task.

@dependencies
None.

@sideeffects
None.
*/
rex_tcb_type* rex_self(void);

/**
Returns a boolean indicating whether the task is an Interrupt Service Task (IST).

@param[in]   p_tcb      Valid pointer to the REX TCB to determine whether it is an IST task or not.

@return
TRUE     -- Indicates that the task is in IRQ mode.
FALSE    -- Indicates that the task is not in IRQ mode.

@dependencies
None.

@sideeffects
None.
*/
boolean rex_check_if_task_is_an_ist_task(rex_tcb_type *p_tcb);
#define rex_is_in_irq_mode() rex_check_if_task_is_an_ist_task(rex_self())

/**
Sets the mode of the task to IST mode. This funciton is used after creating an
Interrupt Service Task (IST)

@return
None.

@dependencies
None.

@sideeffects
None.
*/
void rex_set_task_as_ist_task(rex_tcb_type *p_tcb);
#define rex_set_irq_mode rex_set_task_as_ist_task

/*=================================================================================================
            REX TIMER APIs
=================================================================================================*/
/**
Initializes a REX timer. This function must be called before using any timer APIs.

@param[in] p_timer      Pointer to a valid timer data structure.
@param[in] p_tcb        A valid REX TCB pointer to associate with the timer.
@param[in] sigs         REX signals which will be set in the TCB pointer upon timer
                        expiration.
@return
None.

@dependencies
None.

@sideeffects
In case bad parameters are passed in or an error is encountered, this function will assert.
*/
void rex_def_timer(rex_timer_type *p_timer, rex_tcb_type *p_tcb, rex_sigs_type sigs);

/**
*** NOT TO BE CONFUSED WITH rex_delete_timer_ex() ***

Undefines a REX timer.  This will invoke underlying timer undef API.  Users using
rex_def_timer() must call this API *only once*.

@param[in] p_timer      Pointer to a valid timer data structure.

@return
None.

@dependencies
None.

@sideeffects
In case bad parameters are passed in or an error is encountered, this function will assert.
*/
void rex_undef_timer(rex_timer_type *p_timer);

/**
Clears a running timer.

@param[in] p_timer      Pointer to a valid timer data structure which needs to be cleared.

@return
The countdown of the timer (in ms).

@dependencies
The timer pointer passed in must have been initialized using rex_def_timer.

@sideeffects
In case bad parameters are passed in or an error is encountered, this function will assert.
*/
rex_timer_cnt_type rex_clr_timer(rex_timer_type *p_timer);

/**
Sets a REX timer to expire after the specified interval (in ms).

@param[in] p_timer      Pointer to a valid timer data structure which needs to be set with a timer
                        expiration value
@param[in] msecs        Timer expiration value to be set in milliseconds.

@return
The last countdown of the timer.

@dependencies
The timer pointer passed in must have been initialized using rex_def_timer.

@sideeffects
In case bad parameters are passed in or an error is encountered, this function will assert. \n
Could cause a call to set sigs and thus induce a task swap.
*/
rex_timer_cnt_type rex_set_timer(rex_timer_type *p_timer, rex_timer_cnt_type msecs);

/**
Gets the current countdown value of a REX timer in milliseconds.

@param[in] p_timer      Pointer to a valid timer data structure

@return
The countdown remaining (in ms)

@dependencies
The timer pointer passed in must have been initialized using rex_def_timer.

@sideeffects
In case bad parameters are passed in or an error is encountered, this function will assert.
*/
rex_timer_cnt_type rex_get_timer(rex_timer_type *p_timer);

/**
Pauses a timer.

@param[in] p_timer      Pointer to a valid timer data structure

@return
None.

@dependencies
The timer pointer passed in must have been initialized using rex_def_timer.

@sideeffects
In case bad parameters are passed in or an error is encountered, this function will assert.
*/
void rex_pause_timer(rex_timer_type *timer_ptr);

/**
Resumes a timer.

@param[in] p_timer      Pointer to a valid timer data structure

@return
None.

@dependencies
The timer pointer passed in must have been initialized using rex_def_timer.

@sideeffects
In case bad parameters are passed in or an error is encountered, this function will assert.
*/
void rex_resume_timer(rex_timer_type *timer_ptr);

/**
Performs a rex_set_timer followed by a rex_wait on the calling task.

@param[in] sigs         REX signals which the task will wait on.
@param[in] p_timer      Pointer to a valid timer data structure to set.
@param[in] msecs        Timer expiration value to be set in milliseconds.

@return
Signals that were asserted when the rex_wait() function returns.

@dependencies
The timer pointer passed in must have been initialized using rex_def_timer.

@sideeffects
In case bad parameters are passed in or an error is encountered, this function will assert. \n
A call to rex_set_sigs could be called and thus induce a task swap.
*/
rex_sigs_type rex_timed_wait(rex_sigs_type sigs, rex_timer_type *p_timer, rex_timer_cnt_type msecs);

/**
Extended version of timer-definition function. Allows the specification of a callback function and 
an argument to this function. The callback is called when the timer expires. The callback happens 
in the context of the task that calls rex_def_timer_ex.

@param[in] p_timer         Pointer to a valid timer data structure.
@param[in] timer_cb_ptr    Pointer to the timer callback function which will be invoked on timer
                           expiration.
@param[in] param           Parameter to be passed into the callback function when that is invoked.
                        
@return
None.

@dependencies
None.

@sideeffects
In case bad parameters are passed in or an error is encountered, this function will assert.
*/
void rex_def_timer_ex(rex_timer_type *p_timer, rex_timer_cb_type timer_cb_ptr, unsigned long param);

/**
Similar to rex_def_timer_ex, except that this function allocates the memory for the timer 
dynamically. A pointer to the timer is returned.

@param[in] timer_cb_ptr    Pointer to the timer callback function which will be invoked on timer
                           expiration.
@param[in] param           Parameter to be passed into the callback function when that is invoked.
                        
@return
Pointer to the new dynamically allocated timer.
NULL if the timer could not be allocated.

@dependencies
None.

@sideeffects
None.
*/
rex_timer_type *rex_create_timer_ex(rex_timer_cb_type timer_cb_ptr, unsigned long param);

/**
Function to free the memory of a dynamically allocated timer.

@param[in] p_timer         Pointer to a valid timer data structure.

@return
None.

@dependencies
The timer pointer passed in must have been initialized using rex_create_timer_ex() only.

@sideeffects
None.
*/
void rex_delete_timer_ex(rex_timer_type *p_timer);

/*=================================================================================================
            REX CRITICAL SECTION APIs
=================================================================================================*/
/**
Initializes a REX critical section.

@param[in]  p_crit_sect       Valid pointer to a REX critical section.

@return
None.

@dependencies
None.

@sideeffects
In case a bad parameter is passed in or an error is encountered, this function will assert.
*/
void rex_init_crit_sect(rex_crit_sect_type  *p_crit_sect);

/**
Acquire the lock on a critical section. If it's not available, task suspends and is placed on a 
waiting list until the lock is free.

@param[in]  p_crit_sect       Valid pointer to a REX critical section.

@return
None.

@dependencies
The critical section variable must have been initialized using rex_init_crit_sect().

@sideeffects
In case a bad parameter is passed in or an error is encountered, this function will assert. \n
The task will block if lock is not available. Highest priority non-blocked task will be swapped in.
*/
void rex_enter_crit_sect(rex_crit_sect_type *p_crit_sect);

/**
Release the lock on a critical section. If there is a list of tasks waiting on the critical section,
choose the highest priority task and grant that task the lock.

@param[in]  p_crit_sect       Valid pointer to a REX critical section.

@return
None.

@dependencies
The critical section variable must have been initialized using rex_init_crit_sect().
Calling task must be holding the critical section or else an assertion will be triggered.

@sideeffects
In case a bad parameter is passed in or an error is encountered, this function will assert. \n
If the highest priority task on the list of waiting tasks is higher priority than the calling task, 
then the calling task will be swapped out and the higher priority task will be swapped in.
*/
void rex_leave_crit_sect(rex_crit_sect_type *p_crit_sect);

/**
Acquire the lock on a critical section. If it's not available, this function will just return 
without the object and the calling task will not be blocked.

@param[in]  p_crit_sect       Valid pointer to a REX critical section.

@return
TRUE     -- If crit_sect is available.
FALSE    -- If crit_sect is not available.

@dependencies
The critical section variable must have been initialized using rex_init_crit_sect().

@sideeffects
In case a bad parameter is passed in or an error is encountered, this function will assert. \n
*/
boolean rex_try_enter_crit_sect(rex_crit_sect_type *p_crit_sect);

/**
Deletes a critical section.

@param[in]  p_crit_sect       Valid pointer to a REX critical section.

@return
TRUE     -- The REX crit section was deleted sucessfully.
FALSE    -- There was an error when deleting the critical section.

@dependencies
The critical section variable must have been initialized using rex_init_crit_sect().

@sideeffects
The behavior is undefined if there are waiting threads in the critical section and 
rex_del_crit_sect() is called.
*/
boolean rex_del_crit_sect(rex_crit_sect_type *p_crit_sect);

/*=================================================================================================
            REX SIGNAL APIs
=================================================================================================*/
/**
Waits on specified signals.

@param[in]  sigs        Signals to block the task on.

@return
Signals that were asserted.

@dependencies
None.

@sideeffects
Will cause a task swap if the signals for the task are not already set at the time of the call.
*/
rex_sigs_type rex_wait(rex_sigs_type sigs);

/**
Clears the specified signal bits for the specified TCB.

@param[in]  p_tcb       Valid pointer to the REX TCB for which the signals will be cleared.
@param[in]  sigs        Signal bitmask to clear in the task's TCB.

@return
Signal mask after clearing.

@dependencies
None.

@sideeffects
None.
*/
rex_sigs_type rex_clr_sigs(rex_tcb_type *p_tcb, rex_sigs_type sigs);

/**
Sets signals for specified TCB, causes a system reschedule.

@param[in]  p_tcb       Valid pointer to the REX TCB for which the signals will be set.
@param[in]  sigs        Signal bitmask to set in the task's TCB.

@return
Signal mask after setting the input sigs.

@note If the set causes a higher priority task to run, and that task clears the sigs, the mask 
returned may indicate that the signal was never set (i.e. it was set and processed).

@dependencies
None.

@sideeffects
None.
*/
rex_sigs_type rex_set_sigs(rex_tcb_type *p_tcb, rex_sigs_type sigs);

/**
Gets the currently asserted signals for the specified task.

@param[in]  p_tcb       Valid pointer to the REX TCB for which the signals need to be obtained.

@return
Signal mask currently asserted for the specified task.

@dependencies
None.

@sideeffects
None.
*/
rex_sigs_type rex_get_sigs(rex_tcb_type *p_tcb);

/**
This function is used by REX task to wait for both base and extended signals.

@param[in]   p_sigs        Pointer to the extended signals array to block the task on.
@param[out]  p_rcvd_sigs   Pointer to the extended signals array that will be filled in with the 
                           signals that were asserted.

@return
TRUE     -- If the operation was successful.
FALSE    -- If the operation was unsuccessful.

@dependencies
None.

@sideeffects
Will cause a task swap if the signals for the task are not already set at the time of the call.
*/
boolean rex_wait_ext(const rex_sigs_type* p_sigs, rex_sigs_type* p_rcvd_sigs);

/**
Clears the specified signal mask array bits for the specified REX task.

@param[in]  p_tcb       Valid pointer to the REX TCB for which the signals will be cleared.
@param[in]  p_sigs      Pointer to the signal bitmask array to clear in the task's TCB.

@return
TRUE     -- If the operation was successful.
FALSE    -- If the operation was unsuccessful.

@dependencies
The buffer pointed by p_sigs has to be allocated before calling this function.

@sideeffects
None. 
*/
boolean rex_clr_sigs_ext(rex_tcb_type* p_tcb, const rex_sigs_type* p_sigs);

/**
Sets the specified signal mask array bits for the specified REX task.

@param[in]  p_tcb       Valid pointer to the REX TCB for which the signals will be set.
@param[in]  p_sigs      Pointer to the signal bitmask array to set in the task's TCB.

@return
TRUE     -- If the operation was successful.
FALSE    -- If the operation was unsuccessful.

@note If the set causes a higher priority task to run, and that task clears the sigs, the mask 
returned may indicate that the signal was never set (i.e. it was set and processed).

@dependencies
None.

@sideeffects
None.
*/
boolean rex_set_sigs_ext(rex_tcb_type* p_tcb, const rex_sigs_type* p_sigs);

/**
Gets the currently asserted signals array.

@param[in]   p_tcb       Valid pointer to the REX TCB for which the signals need to be obtained.
@param[out]  p_sigs      Pointer to the signal bitmask array in which the signals will be returned.

@return
Signal mask currently asserted for the specified task.

@dependencies
None.

@sideeffects
None.
*/
boolean rex_get_sigs_ext(rex_tcb_type* p_tcb, rex_sigs_type* p_sigs);

/*=================================================================================================
            REX WATCHDOG APIS
=================================================================================================*/
/**
Performs initialization needed to set up the auto dog service for a task.
When auto-dog is on, the task does not need to wake up periodically to
kick the dog when making a blocking REX call. REX takes care of notifying
Dog that the task is blocked and that there is no need to monitor the
task until further notice. When the task is scheduled again, Dog is
notified that the task is ready to be monitored again.

@param[in]   dog_report_val   This parameter is ignored. In legacy 
                              implementations, this used to  to be the 
                              calling task's registered watchdog handle. 
                              To register with the watchdog service please
                              consult the Watchdog services API.

@note For all new code please use rex_autodog_enable_self().

@return
Previous value of the auto dog state of the calling task.

@dependencies
None.

@sideeffects
None.
*/
int rex_autodog_enable(int dog_report_val)  REX_DEPRECATED;
int rex_autodog_enable_self(void);

/**
Disables the auto dog service for a task.

@return
Previous value of the auto dog state of the calling task.

@dependencies
None.

@sideeffects
None.
*/
int rex_autodog_disable(void);

/**
Returns the REX task's registered watchdog service identifier/handle.

@return
0 - In case the task has not yet registered with the watchdog service or error.
Non zero - The registered watchdog services identifier/handle.

@dependencies
The REX task should have preregistered with the watchdog service.

@sideeffects
None.
*/
int rex_get_dog_report_val_self(void);

/*=================================================================================================
            REX INTERRUPT APIs
=================================================================================================*/

/**
REX_DISABLE_PREMPTION() prevents the preemption of the calling REX task by the OS scheduler 
even if IRQ/IST tasks are ready to run. The typical use of this macro is to allow time 
sensitive code or a sequence of instructions to execute to completion i.e. until 
REX_ENABLE_PREMPTION() is called.

@note On single core implementations this could be implemented by disabling interrupts. On SMT/SMP 
cores, it is expected that the task will not be preempted on the core/hardware thread that it is 
running on. Other tasks/threads including IRQ/IST tasks could be scheduled on other cores/hardware 
threads, therefore this API is not meant to be used for mutual exclusion.

@note This macro may not be implemented on all OSes or may exhibit different behavior. 
Please check the OS documentation.
@note It has been noted that this macro has been misspelled but this is a legacy 
macro and cannot be changed for backward compatibility reasons.

@return
None.

@dependencies
None.
*/
#define REX_DISABLE_PREMPTION()              REX_OS_DISABLE_PREEMPTION()

/**
REX_ENABLE_PREMPTION() is to be used in conjunction with REX_DISABLE_PREMPTION().
REX_DISABLE_PREMPTION() prevents the preemption of the calling REX task by the OS scheduler 
even if IRQ/IST tasks are ready to run. The typical use of this macro is to allow time 
sensitive code or a sequence of instructions to execute to completion i.e. until 
REX_ENABLE_PREMPTION() is called.

@note On single core implementations this could be implemented by disabling interrupts. On SMT/SMP 
cores, it is expected that the task will not be preempted on the core/hardware thread that it is 
running on. Other tasks/threads including IRQ/IST tasks could be scheduled on other cores/hardware 
threads, therefore this API is not meant to be used for mutual exclusion.

@note This macro may not be implemented on all OSes or may exhibit different behavior. 
Please check the OS documentation.
@note It has been noted that this macro has been misspelled but this is a legacy 
macro and cannot be changed for backward compatibility reasons.

@return
None.

@dependencies
None.
*/
#define REX_ENABLE_PREMPTION()               REX_OS_ENABLE_PREEMPTION()

/**
REX_ISR_LOCK is a helper macro to be used for mutual exclusion between IST/IRQ tasks and also 
between non IST/IRQ and IST/IRQ tasks. REX_ISR_LOCK() is to be called before entering the critical
code region and REX_ISR_UNLOCK is to be called at the end.

@param p_crit_sect  A valid pointer to a REX critical section.

@note This macro may not be implemented on all OSes or may exhibit different behavior. 
Please check the OS documentation.

@return
None.

@dependencies
None.
*/
#define REX_ISR_LOCK(p_crit_sect)     REX_OS_ISR_LOCK(p_crit_sect)

/**
REX_ISR_UNLOCK is to be used in conjunction with REX_ISR_LOCK().
REX_ISR_LOCK is a helper macro to be used for mutual exclusion between IST/IRQ tasks and also 
between non IST/IRQ and IST/IRQ tasks. REX_ISR_LOCK() is to be called before entering the critical
code region and REX_ISR_UNLOCK is to be called at the end.

@param my_global_lock_ptr  A valid pointer to a REX critical section.

@note This macro may not be implemented on all OSes or may exhibit different behavior. 
Please check the OS documentation.

@return
None.

@dependencies
None.
*/
#define REX_ISR_UNLOCK(my_global_lock_ptr)   REX_OS_ISR_UNLOCK(my_global_lock_ptr)

/**
rex_task_lock prevents context switching from the point at which it is
invoked until a matching rex_task_free is invoked. While tasks are locked,
the current task is guaranteed to not be replaced by a different task, even
if the other task has a higher priority.  However, rex_task_lock does not
block interrupts from occurring.  An ISR will pre-empt the task, but even
if the ISR causes a higher priority task to become ready, control will
still resume at the interrupted task.

@return
None.

@note Note that a rex_wait() while a rex_task_lock is in effect will not work as
normal, as the calling task will not be suspended.
@note rex_task_lock has no effect if called from within an ISR.
@note This API may not be implemented on all OSes. Please check the OS
documentation to see if this is available.
@note This API has been deprecated and will be removed in future releases. Any
new code should not be written using this API. Instead consider using macro
REX_TASK_PREEMPTION_DISABLE().

@dependencies
None.

@sideeffects
None.
*/
void rex_task_lock(void) REX_DEPRECATED;

/**
rex_task_free re-enables context switching after a rex_task_lock. Context
switching is re-enabled if rex_task_free has been called as many times as
rex_task_lock.

@return
None.

@note rex_task_lock has no effect if called from within an ISR.
@note This API may not be implemented on all OSes. Please check the OS
documentation to see if this is available.
@note This API has been deprecated and will be removed in future releases. Any
new code should not be written using this API. Instead consider using macro
REX_TASK_PREEMPTION_ENABLE().

@dependencies
None.

@sideeffects
None.
*/
void rex_task_free(void) REX_DEPRECATED;

/**
This function checks whether a TASKLOCK is in effect.

@return
0 -- If tasks are not locked \n
1 -- If tasks are currently locked

@note This may not be supported on every OS platform so please check the OS documentation
to see if this function has been implemented and/or varies in behavior.
@note For mutual exclusion please use REX critical sections instead and do not rely on TASKLOCKs
@note Macros TASKS_ARE_LOCKED() and rex_tasks_are_locked() have been deprecated and
are mapped to rex_check_if_tasks_are_locked. This has been done for backward
compatibility.
@note This API has been deprecated and will be removed in future releases. Any
new code should not be written using this API.


@dependencies
None.

@sideeffects
None.
*/
int rex_check_if_tasks_are_locked(void) REX_DEPRECATED;
/* Note: This is being done in this way as TASKS_ARE_LOCKED is more like 
 * a statement than a question which has led to confusion during REX porting.
 */
#define rex_tasks_are_locked     rex_check_if_tasks_are_locked
#define TASKS_ARE_LOCKED()       rex_check_if_tasks_are_locked()

/**
REX_TASK_PREEMPTION_DISABLE elevates the priority of the calling task to a
system defined high priority value. This API is to be used in conjunction with 
REX_TASK_PREEMPTION_ENABLE which then restores the task's priority back to 
what it was before the call to REX_TASK_PREEMPTION_DISABLE.

This is done so as to provide the calling task with enough MIPS to complete 
any operations within the REX_TASK_PREEMPTION_DISABLE/REX_TASK_PREEMPTION_ENABLE
block. This could be used for example to complete a sequence of hardware register
reads/writes. Please note that this API does not guarantee mutual exclusion.

@return
None.

@note Consider using this API as a replacement for the deprecated task lock APIs.
@note REX_TASK_PREEMPTION_DISABLE will not block interrupts from occurring.
On single core CPUs, this has the implication that ISTs will preempt the task, 
that called REX_TASK_PREEMPTION_DISABLE but after the IST runs to completion, due 
to the priority elevation, the scheduler will schedule the thread that called
REX_TASK_PREEMPTION_DISABLE. On SMP/IMT cores, it is expected that the task will 
not be preempted on the core/hardware thread that it is running on. 
Other tasks/threads including IST tasks could be scheduled on other cores/hardware 
threads, therefore this API is not meant to be used for mutual exclusion.
@note It is not advised to perform any blocking calls when in the 
REX_TASK_PREEMPTION_DISABLE/REX_TASK_PREEMPTION_ENABLE block as that will 
cause a reschedule. If this is done, a situation could arise where some other task 
calls REX_TASK_PREEMPTION_DISABLE. If this occurs then depending on the scheduler
implementation and underlying CPU hardware we could potentially have round robin 
scheduling among the tasks that have been bumped up to the system defined 
high priority value. This may not be desirable for applications.

@dependencies
None.

@sideeffects
Priority of the calling task will be changed.

@sa
REX_TASK_PREEMPTION_ENABLE
*/
#define REX_TASK_PREEMPTION_DISABLE()  REX_OS_TASK_PREEMPTION_DISABLE()

/**
REX_TASK_PREEMPTION_ENABLE restores the task's priority back to 
what it was before the call to REX_TASK_PREEMPTION_DISABLE. 
This API is to be used in conjunction with REX_TASK_PREEMPTION_DISABLE 
which elevates the priority of the calling task to a system defined 
high priority value.

This is done so as to provide the calling task with enough MIPS to complete 
any operations within the REX_TASK_PREEMPTION_DISABLE/REX_TASK_PREEMPTION_ENABLE
block. This could be used for example to complete a sequence of hardware register
reads/writes. Please note that this API does not guarantee mutual exclusion.

@return
None.

@note Consider using this API as a replacement for the deprecated task lock APIs.

@dependencies
None.

@sideeffects
Priority of the calling task will be changed.

@sa
REX_TASK_PREEMPTION_DISABLE
*/
#define REX_TASK_PREEMPTION_ENABLE()   REX_OS_TASK_PREEMPTION_ENABLE()

/*=================================================================================================
            REX Asynchronous Procedure Calls
=================================================================================================*/
/**
Accepts a function pointer which will be executed out of the calling task's context, hence the name
Deferred Procedure Call (DPC). Typically used to execute functionality out of an IST task context. 
The APC is executed in the context of a REX owned task called "DPC Task".

@param[in]  apc      Pointer to the APC (function pointer) to execute.
@param[in]  param    Parameter to pass to the APC.

@return
TRUE  --    If a DPC was queued.
FALSE --    Otherwise.

@dependencies
None.
*/
boolean rex_queue_dpc(rex_apc_func_type apc, unsigned long param);
/* Done for backward compatibility */
#define rexl4_queue_dpc rex_queue_dpc

/**
This function is similar to rex_queue_dpc(). Please see the documentation
for that API.

@note This function has been deprecated.

@param[in]  apc      Pointer to the APC (function pointer) to execute.
@param[in]  p_tcb    Formerly pointer to the TCB of a REX task in which the APC should be
                     executed. This is no longer supported and this parameter is ignored.
@param[in]  param    Parameter to pass to the APC.

@return
TRUE  --    If a DPC was queued.
FALSE --    Otherwise.

@dependencies
None.
*/
boolean
rex_queue_apc(rex_apc_func_type apc, rex_tcb_type *p_tcb, unsigned long param) REX_DEPRECATED;

#ifdef __cplusplus
   }
#endif

#endif /* END REX_H */
