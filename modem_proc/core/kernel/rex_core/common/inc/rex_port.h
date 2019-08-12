#ifndef __REX_PORT_H__
#define __REX_PORT_H__

#include "rex.h"
#include "assert.h"

#define REX_ATTR_VALIDATION_OK                     0
#define REX_ATTR_VALIDATION_BADSTACK               1
#define REX_ATTR_VALIDATION_BADPRIORITY            2
#define REX_ATTR_VALIDATION_BADENTRYFUNC           3
#define REX_ATTR_VALIDATION_BADCPUAFFINITY         4
#define REX_ATTR_VALIDATION_BADTASKNAME            5
#define REX_ATTR_VALIDATION_BADINITSUSPENDSTATE    6

#ifdef __cplusplus
extern "C"
{
#endif

/**
Function that returns the minimum permitted stack size in number of bytes 
that is supported by the underlying OS.

@return
Minimum stack size in bytes.
*/
unsigned int rex_os_get_task_min_stack_size(void);

/**
This function returns a boolean value indicating whether the 
underlying OS supports the creation and deletion of user stacks
in case a user does not provide a stack area.

@return
TRUE   --  This functionality is supported.
FALSE  --  This functionality is unsupported.
*/
boolean rex_os_is_null_user_stack_supported(void);

/**
Function that returns the minimum permitted priority
that is supported by the underlying OS.

@return
Minimum priority value.
*/
unsigned int rex_os_get_task_min_priority(void);

/**
Function that returns the maximum permitted priority
that is supported by the underlying OS.

@return
Maximum priority value.
*/
unsigned int rex_os_get_task_max_priority(void);

/**
Function that returns the supported CPU affinity bitmask
that is supported by the underlying OS/platform.

@return
A valid CPU bitmask.
*/
unsigned int rex_os_get_task_valid_cpu_affinity_bitmask(void);

/**
This function return a boolean value indicating whether the 
underlying OS supports starting a task in a initially suspended
state is supported or not. Tasks started in intially suspended 
state can resume execution when an another task calls rex_resume()
and pass in the rex_tcb_type handle for the suspended task.

@note In the event the underlying OS cannot support this feature, 
OS teams should explicity assert in their implementation of 
rex_suspend() and rex_resume().

@return
TRUE  --  This feature is supported.
FALSE --  This feature is unsupported
*/
boolean rex_os_is_task_start_in_initially_suspended_state_supported(void);

/**
The REX OS porting API to assert in case a runtime error condition is encountered in 
the REX Core library.

@param[in]  -- fmt_string     Pointer to the error message string.
@param[in]  -- file_name      Pointer to the file name where the error occured.
@param[in]  -- func_name      Pointer to the function name in which the error occured.
@param[in]  -- line_num       The line number where the assertion occured.

@return
None.

@note This API is a required API that has to be implemented by the OS porting layer.

@dependencies
None.
*/
void rex_os_assert
(
   const char *fmt_string, 
   const char *file_name, 
   const char *func_name, 
   unsigned int line_num,
   ...
);

#define REX_ERR_FATAL(fmt_string) ASSERT(!fmt_string)

/**
This is the REX OS port wrapper function to kill a REX task. 
This is called from the REX task destroy API i.e. rex_kill_task().

@param[in]  -- p_tcb          Pointer to REX TCB handle of the task to be destroyed.

@return
REX_SUCCESS          -- Indicates task was successfully created.
All other codes      -- Indicate some form of an error and it is upto the OS to specify one.

@note This API is a required API that has to be implemented by the OS porting layer.

@dependencies
None.
*/
void rex_os_kill_task(rex_tcb_type *p_tcb);

/*================================================================================================
         OPTIONAL AMSS (QXDM/DIAG) PROFILING PORT APIS
================================================================================================*/

/**
OS routine to initialize the system and task profiling.

@return
None.

@dependencies
None.

@sideeffects
None.
*/
void rex_os_init_profiling(void);

/**
OS routine to start the system and task profiling.

@return
None.

@dependencies
None.

@sideeffects
None.
*/
void rex_os_start_task_profiling(void);

/**
OS routine to stop the system and task profiling.

@return
None.

@dependencies
None.

@sideeffects
None.
*/
void rex_os_stop_task_profiling(void);

/**
Function that fills in and issues a per task profile packet which contains each task's profile 
information.

@param[in]  param    Value to be passed into the callback function.

@return
None.

@dependencies
rex_os_init_profiling() should have been called.

@sideeffects
None.
*/
void rex_os_log_per_task_profiling_info(void);

/**
Function that logs the system profile packet, which contains system profile information.

@param[in]  param    Value to be passed into the callback function.

@return
None.

@dependencies
None.

@sideeffects
rex_os_init_profiling() should have been called.
None.
*/
void rex_os_log_sys_profiling_info(void);

/*================================================================================================
         OTHER REX OS PORT HELPER APIs
================================================================================================*/
/**
This function is a helper function that validates the input task attributes passed in 
by the user when attempting to create a REX task.

@param[in]  -- p_task_attr    Pointer to task attributes data types specified at the time of task
                              creation.
@param[out] -- p_reason       In case any of the user supplied attributes are incorrect, the
                              error code reason can be specified using this pointer. This routine
                              returns the following error codes: REX_ATTR_VALIDATION_OK (if all
                              attributes are correctly specified), REX_ATTR_VALIDATION_BADSTACK (if 
                              the stack address or size is incorrect), 
                              REX_ATTR_VALIDATION_BADPRIORITY (if the priority attribute is 
                              incorrect), REX_ATTR_VALIDATION_BADENTRYFUNC (if the task entry 
                              function pointer is invalid) and  REX_ATTR_VALIDATION_BADCPUAFFINITY 
                              (if the affinity bitmask is incorrect).
@return
REX_SUCCESS          -- Indicates all the specified attributes are valid.
REX_ERROR_BAD_PARAM  -- Indicates incorrect attribute(s) were specified.

@dependencies
This function can only perform validation checks upto an extent, this function depends on 
rex_os_task_attr_validate() which the OS porting layer has to implement to further validate the
user supplied attributes.
*/
int rex_task_attr_validate(rex_task_attributes_type *p_task_attr, int *p_reason);

/**
This is the routine which is required to be called by the OS porting layer during their
OS initialization routine. This function performs some environment initialization and 
starts REX service tasks.

@return
REX_SUCCESS    --    Indicates success.
REX_ERROR      --    Indicates an error condition.

@note The OS porting layer implementation should assert in case an error is encountered.

@dependencies
Dynamic memory allocation services should be available.
*/
int rex_core_common_init(void);

/**
Invokes callback functions that have been registered with the REX Callback registry when REX 
is first initialized. 

@return
REX_SUCCESS    --    Indicates success.
REX_ERROR      --    Indicates an error occured.

@note If the OS init routine calls rex_core_common_init() there is no need to use this function as
the init occurs only once.
@note This function has been made available to the REX OS porting layer in case the implementation
cannot call rex_core_common_init().

@dependencies
None. 

@sideeffects
In the event any registered callback function returns an error, this function will assert.
*/
int rex_invoke_cbs_on_init(void);

/**
Invokes callback functions that have been registered with the REX Callback registry when a 
new REX task is created via REX task creation APIs ex rex_def_task(), rex_common_def_task() etc.

@return
REX_SUCCESS    --    Indicates success.
REX_ERROR      --    Indicates an error occured.

@note This function has been made available to the REX OS porting layer in case the implementation
cannot call rex_core_common_init() or rex_task_entry_func().

@dependencies
None. 

@sideeffects
In the event any registered callback function returns an error, this function will assert.
*/
int rex_invoke_cbs_on_thread_create(rex_tcb_type *p_tcb);

/**
Invokes callback functions that have been registered with the REX Callback registry when a 
REX task is killed via rex_kill_task().

@return
REX_SUCCESS    --    Indicates success.
REX_ERROR      --    Indicates an error occured.

@note This function has been made available to the REX OS porting layer in case the implementation
cannot call rex_core_common_init() or rex_task_entry_func().

@dependencies
None. 

@sideeffects
In the event any registered callback function returns an error, this function will assert.
*/
int rex_invoke_cbs_on_thread_destroy(rex_tcb_type *p_tcb);

INLINE void stack_align_helper
(
   rex_stack_pointer_type  orig_stack_start_ptr, 
   rex_stack_size_type     orig_stack_size,
   unsigned int            align_to,   
   rex_stack_pointer_type  *pp_adj_stack_start_ptr,
   rex_stack_size_type     *p_adj_stack_size,
   rex_stack_pointer_type  *pp_adj_stack_end_ptr
)
{
   /* In this case, stacks are passed as arrays of bytes with the size
   ** indicating the number of bytes. The adjustment of the stack pointer
   ** is to assure alignment. 
   */
   rex_stack_pointer_type adj_stack_ptr;
   rex_stack_size_type adj_stksize = orig_stack_size;

   /* Adjust stack alignment to input byte boundary */
   adj_stack_ptr =
   (
      ((uint32)orig_stack_start_ptr % align_to)
      ?
      (rex_stack_pointer_type)
      (
         ((uint32)orig_stack_start_ptr + align_to) - 
         ((uint32)orig_stack_start_ptr % align_to)
      )
      :
      orig_stack_start_ptr
   );

   /* Adjusted stack size after alignment */
   adj_stksize -= (rex_stack_size_type)((uint32)adj_stack_ptr - (uint32)(orig_stack_start_ptr));

   /* Ensure that the entire stack (start to end) is aligned so drop off the last odd bytes */
   adj_stksize &= ~(align_to - 1);

   *pp_adj_stack_start_ptr  = adj_stack_ptr;
   *p_adj_stack_size        = adj_stksize;
   *pp_adj_stack_end_ptr    = (rex_stack_pointer_type)((uint32)adj_stack_ptr + adj_stksize);
}

#ifdef __cplusplus
   } // extern "C"
#endif

#endif //__REX_PORT_H__
