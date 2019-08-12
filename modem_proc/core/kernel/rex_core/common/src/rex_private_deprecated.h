#ifndef __REX_PRIVATE_DEPRECATED_H__
#define __REX_PRIVATE_DEPRECATED_H__

//#define  REX_ERROR_MISALIGNED_DATA                          0x01
//#define  REX_ERROR_DATA_ABORT                               0x02
//#define  REX_ERROR_PREFETCH_ABORT                           0x03
//#define  REX_ERROR_DOUBLE_ABORT_EXCEPTION                   0x04
//#define  REX_ERROR_UNDEFINED_INSTR                          0x05
//#define  REX_ERROR_INVALID_PARAMETER                        0x06
//#define  REX_ERROR_USER_DEVICE_ACCESS                       0x07

/* --------------------------------------------------------------------
** Deferred Procedure Calls
**   Provides context in which to run ISR level functions at task context.
**   DPC's are the highest priority task in the system
** -------------------------------------------------------------------- */
typedef boolean (rex_dpc_func_type)(
   unsigned long param1,
   unsigned long param2,
   unsigned long param3
);
   /* DPC Function Type
   **   Signature of function which can be called as a DPC
   */
typedef struct {
   rex_dpc_func_type *dpc_ptr;
   unsigned long      dpc_param1;
   unsigned long      dpc_param2;
   unsigned long      dpc_param3;
} rex_dpc_item_type;
/* Type for items on DPC ring buffer.
*/

/* ----------------------------------------------------------------------
** Time test variables and Profiling
** ---------------------------------------------------------------------- */
//Potential REXOPTIONAL
extern word rex_led_buf;

extern byte  rex_profile_enabled;

/* The maximum number of tasks for which profiling will be done.
*/
#ifndef REX_MAX_PROFILE_ENTRY
   #define REX_MAX_PROFILE_ENTRY 32
#endif

/* Boolean state variable indicating whether or not the rex is
 * currently involved a kernel intlock.
 */
extern unsigned long rex_intlocked;

/* ------------------------------------------------------------------------
** CPU Registers
** ------------------------------------------------------------------------ */
// typedef PACKED union {
   // unsigned long arg;
   // unsigned long val;
   // void          *addr;
   // void         (*func)( void );
   // void         (*task)( unsigned long );
   // void         (*preamble)( void (*func)(unsigned long), unsigned long );
// } rex_cpu_register_type;

/* ------------------------------------------------------------------------
** Context Frame
** ------------------------------------------------------------------------ */
// typedef PACKED struct {
   // rex_cpu_register_type spsr;
   // rex_cpu_register_type r[13];  /* r0-r7,r8-r12 */
   // rex_cpu_register_type lr;    /* r14        */
   // rex_cpu_register_type pc;    /* r15        */
// } rex_context_frame_type;

/* ------------------------------------------------------------------------
** Frame pushed on the IRQ stack in the IRQ handler
** ------------------------------------------------------------------------ */
// typedef PACKED struct {
   // rex_cpu_register_type spsr;
   // rex_cpu_register_type r[6];  /* r0-r3,r10,r12               */
   // rex_cpu_register_type lr;    /* IRQ mode r14 = SVC mode PC  */
// } rex_saved_irq_frame_type;


extern dword rex_IRQnumticks;
extern word  rex_pnumticks;
extern dword rex_time_tick2;

typedef struct {
  byte  name[4];
  dword tick;
} sw_profile_type;

/* ------------------------------------------------------------------------
** Program Counter Dump array size.
** ------------------------------------------------------------------------ */
//Potential REXOPTIONAL
#ifndef PC_ARRAY_SIZE
   #define PC_ARRAY_SIZE 0x100000
#endif

/*===========================================================================
MACROS REX_INTLOCK/REX_INTFREE

DESCRIPTION
   Locks/unlocks interrupts. If FEATURE_REX_PROFILE is defined, sets a
   variable to note that the interrupt locking/freeing was done from
   within Rex. This distinction is useful for profiling.
   
   NOTE: These macros are to be used internally in Rex.

===========================================================================*/
/* REX_ATTR:    Private
 *              Required
 */
#define REX_INTLOCK( )  REX_OS_INTLOCK ( )
#define REX_INTFREE( )  REX_OS_INTFREE ( )


#define REX_ASSERT(xx_condition) REX_OS_ASSERT(xx_condition)


/*===========================================================================
FUNCTION REX_SET_INTERRUPT_VECTOR

DESCRIPTION
  Installs interrupt vector call

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
void rex_set_interrupt_vector(
   rex_vect_type     v,
   rex_isr_func_type fnc_ptr
);

/*===========================================================================
FUNCTION REX_ENABLE_INTERRUPT

DESCRIPTION
  free interrupts

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
void rex_enable_interrupt( rex_vect_type v );


/*===========================================================================
FUNCTION  REX_DISABLE_INTERRUPT

DESCRIPTION
  Locks interrupts

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
void rex_disable_interrupt( rex_vect_type v );

/*===========================================================================

FUNCTION REX_APC_EPILOG

DESCRIPTION
  Performs processing needed after a rex_apc_call call. After the APC
  has been executed, there are two cases to consider:
  (a) A context switch is not needed.
      In this case, just pop the top context frame off the stack of the
      current thread and restore registers using this frame.

  (b) A context switch is needed.
      In this case, save the stack pointer of the current thread in its
      TCB. It is not necessary to save a context frame because one gets
      pushed when an APC is queued.

  NOTE 1: This function is for internal REX use only.

  NOTE 2: This function restores registers and may cause a context switch.

===========================================================================*/
/* REX_ATTR:    Public
 *              Optional; Deprecate if needed
 */
void rex_apc_epilog( void );

/*===========================================================================
FUNCTION REX_TICK

DESCRIPTION
  If timers are enabled, calls rex_decrement_timers to decrement the timer
  value for all valid timers in the active timer list. Returns immediately
  if timers are disabled.

DEPENDENCIES
  Interrupts must be disabled.

RETURN VALUE
  None.

SIDE EFFECTS
  If a timer expires, rex_set_sigs is called and a task swap may be induced.
===========================================================================*/
void rex_tick(
  rex_timer_cnt_type p_ticks      /* number of rex ticks elapsed */
);

/*===========================================================================
FUNCTION REX_SCHED

DESCRIPTION
  Does the task swap. Is called within rex and should not be called from
  outside of rex. Rex_start_task is an entry point from rex_sched to restore
  context (called only by rex_init).

DEPENDENCIES
  Rex_sched assumes that the following global variables have reasonable
  values: rex_curr_task, rex_best_task.

RETURN VALUE
  None.

SIDE EFFECTS
  Rex_sched may set the current task pointer (rex_curr_task) equal to
  the best task pointer (rex_best_task) if they are not already the same.
  Also, rex_sched may cause a task swap to occur from the old current
  task to the new current task.
===========================================================================*/
/* REX_ATTR:    Private
 *              Deprecate; Not used in L4
 */
void rex_sched( void );

/*===========================================================================
FUNCTION REX_ASSIGN_BEST_TASK

DESCRIPTION
  Sets the rex_best_task variable. The user must ensure that the input
  tcb is indeed the highest-priority runnable task.
  This function is for *internal* REX use only, and furthermore, the calling
  function is required to have locked interrupts before making the call.

===========================================================================*/
void rex_assign_best_task(
 rex_tcb_type *tcb_ptr
);

/*===========================================================================
FUNCTION REX_SET_BEST_TASK

DESCRIPTION
  This function searches a portion of the TCB list to find the highest
  priority TCB which has is eligible for scheduling.  Since REX is
  pre-emptive, normally the highest priority non-blocked task will be
  running at any given time (except for a few situations like TASKLOCK).
  As such, normally one can start traversing the TCB list at the first task
  of lower priority than the one that was just blocked.  rex_set_best_task()
  accepts a pointer to a TCB as an argument, and will begin searching the
  list at the next TCB after the argument.  It will set rex_best_task to
  the first (and therefore highest priority) task it finds that is not
  blocked awaiting a resource.

  There are three conditions that need to be met for a task to be runnable.

  1. The task is not blocked on a rex_wait() function call.  This will be
     true if the task's TCB->wait variable is zero.

  2. The task is not blocked on a rex_crit_sect_enter() function call. This
     will be true if the task's TCB->cs_link.next_ptr is NULL.

  3. The task was not created in the suspended mode.  This will be true if the
     task's TCB->suspended flag is false.

  This function will call the macro REX_TASK_RUNNABLE to test the three
  conditions.

  Note that a given task can be blocked on only one of these constructs at
  a time.  For example, if the task comes up in the SUSPENDED mode, then
  it will never get the change to execute either rex_wait() or rex_enter_crit_
  section().  As such, at various places throughout REX, when a condition is
  met, there isn't a need to check for the other conditions before making a
  scheduling decision.  For example, in rex_set_sigs() a task that was blocked
  on a rex_wait() call is potentially woken up.  The code that does the wakeup
  needs only look at the tasks "sigs" and "wait" fields in the TCB to decide.
  The fact that the task was blocked on rex_wait() means that it could not
  be SUSPENDED nor could it be blocked on a critical section.


DEPENDENCIES
  This function is for *internal* REX use only, and furthermore, the calling
  function is required to have locked interrupts before making the call.

RETURN VALUE
  void

SIDE EFFECTS
  rex_best_task is modified.
===========================================================================*/
void rex_set_best_task
(
  rex_tcb_type *start_tcb
);

/* ==================================================================
FUNCTION REX_SET_BEST_TASK_FROM_ALL

DESCRIPTION
   Searches the entire task list and sets the rex_best_task variable.

   NOTE: Interrupts must be locked when this function is called.

===================================================================== */
void rex_set_best_task_from_all( void );

/*===========================================================================
FUNCTION REX_KILL_TASK_EXT

DESCRIPTION
  Kill a specified task by yanking its TCB from the active list.

DEPENDENCIES
  None.

RETURN VALUE
  Void in normal cases.
  If a thread calls rex_kill_task on itself, then this function will not
  return at all (as the thread goes away).

===========================================================================*/
void rex_kill_task_ext(
   rex_tcb_type *p_tcb,
   boolean       schedule_new_task
);

/*===========================================================================
FUNCTION REX_KILL_TASK_EXT

DESCRIPTION
  Kill a specified task by yanking its TCB from the active list.

DEPENDENCIES
  None.

RETURN VALUE
  Void in normal cases.
  If a thread calls rex_kill_task on itself, then this function will not
  return at all (as the thread goes away).

===========================================================================*/
void rex_kill_task_ext(
   rex_tcb_type *p_tcb,
   boolean       schedule_new_task
);

/* ==================================================================
FUNCTION REX_DELETE_TASK_TIMERS

DESCRIPTION
   Removes all the timers that belong to a specified task from the
   REX timer list.
===================================================================== */
void rex_delete_task_timers(
   rex_tcb_type *tcb_ptr
);

/* ==================================================================
FUNCTION REX_SYS_GET_TIMER_LIST
DESCRIPTION
   Returns a list of the current rex timers, must pass in an array
   of REX_SYS_MAX_TIMER_IDS of rex_timer_types
===================================================================== */
void rex_sys_get_timer_list(
   rex_timer_type **timer_list
);

/*===========================================================================
FUNCTION REX_INIT_TIMER_LIST

DESCRIPTION
  Ininitializes the REX timer list by inserting the null timer after the head
  node.

===========================================================================*/
void rex_init_timer_list( void );

/*===========================================================================
FUNCTION rex_set_dac

DESCRIPTION
  Sets the DAC variable in the rex TCB.

DEPENDENCIES
  Should be called after rex_init().

===========================================================================*/
void rex_set_dac( rex_tcb_type* tcb, unsigned int dac );


/* ==================================================================
FUNCTION  REX_QUEUE_DPC
DESCRIPTION
   A function that executes in a context which is deferred from
   interrupt context.  This can only be called at interrupt context
   and otherwise causes a diagnostics to be logged and the requested
   call to be ignored.

   Operates much like an APC except is executed in the context of a
   REX owned thread.
===================================================================== */
boolean rex_queue_dpc(
   rex_dpc_func_type *dpc_ptr,
      /* Pointer to the function to call
      */
   unsigned long      param1,
      /* Parameter to pass to the dpc
      */
   unsigned long      param2,
      /* Parameter to pass to the dpc
      */
   unsigned long      param3
      /* Parameter to pass to the dpc
      */
)

/*  ===========================================================================
  *
  *  FUNCTION rexl4_init_crit_sect
  *
  *  DESCRIPTION
  *    Initializes a critical section. Must be done prior to use.
  *
  *  DEPENDENCIES
  *    None.
  *
  *  PARAMETERS
  *    crit_sect - pointer of the critical section structure
  *
  *  RETURN VALUE
  *    None.
  *
  *  SIDE EFFECTS
  *    None.
  *  ========================================================================*/
void rexl4_init_crit_sect(rex_crit_sect_type *crit_sect);

/*  ===========================================================================
  *
  *  FUNCTION rexl4_def_task_internal
  *
  *  DESCRIPTION
  *    Define a new rex task. All these REXL4 functions should be called by 
  *    REX API functions.
  *
  *  DEPENDENCIES
  *    This is an REXL4 internal function.    
  *    p_stack must be on an aligned boundary and the size must be the number
  *    of bytes in the stack
  * 
  *  PARAMETERS
  *    p_tcb - valid tcb for new task
  *    p_stack -stack for new task
  *    p_stksiz - stack size in bytes
  *    p_pri - priority for new task
  *    p_task - task startup function
  *    p_param -parameter for new task
  *    p_tskname -string for task name
  *    p_susp - is task initially suspended?
  *    p_parent - opaque handle to container
  *    p_dynamic -stack/tcb alloc'd via dyna mem
  *    dog_report_val -dog report value
  *
  *  RETURN VALUE
  *    None.
  *
  *  SIDE EFFECTS
  *    None.
  *  ========================================================================*/
void rexl4_def_task_internal
(
#ifdef FEATURE_QDSP6
   unsigned int        hw_bitmask,
#endif
   rex_tcb_type        *p_tcb,             /* valid tcb for new task */
   unsigned char*       p_stack,            /* stack for new task */
   rex_stack_size_type  p_stksiz,           /* stack size in bytes */
   rex_priority_type    p_pri,              /* priority for new task */
   rex_task_func_type   p_task,             /* task startup function */
   dword                p_param,            /* parameter for new task */
   char                *p_tskname,         /* A/N string for task name */
   boolean              p_susp,             /* is task initially suspended? */
   void                *p_parent,          /* opaque handle to container */
   boolean              p_dynamic,      /* stack/tcb alloc'd via dyna mem */
   int                  dog_report_val      /* Dog report value */
);

/*  ===========================================================================
  *
  *  FUNCTION rexl4_def_task_ext2
  *
  *  DESCRIPTION
  *    Define a new rex task. All these REXL4 functions should be called by 
  *    REX API functions.
  *
  *  DEPENDENCIES
  *    This is an REXL4 internal function.    
  *    p_stack must be on an aligned boundary and the size must be the number
  *    of bytes in the stack
  * 
  *  PARAMETERS
  *    rexl4_def_task_ext2 takes one parameter in addition to the ones taken by
  *    rexl4_def_task_ext. This parameter allows the user to specify a dog report
  *    value for the task. Just as with rexl4_def_task_ext, the stack size should
  *    be specified in units of bytes.
  *
  *  RETURN VALUE
  *    None.
  *
  *  SIDE EFFECTS
  *    None.
  *  ========================================================================*/
void rexl4_def_task_ext2
(
#ifdef FEATURE_QDSP6
   unsigned int        hw_bitmask,
#endif
   rex_tcb_type         *p_tcb,              /* valid tcb for new task       */
   unsigned char*        p_stack,            /* stack for new task           */
   rex_stack_size_type   p_stksiz,           /* stack size in bytes          */
   rex_priority_type     p_pri,              /* priority for new task        */
   rex_task_func_type    p_task,             /* task startup function      */
   dword                 p_param,            /* parameter for new task      */
   char                 *p_tskname,          /* A/N string for task name    */
   boolean               p_susp,             /* is task initially suspended? */
   int                   dog_report_val      /* Dog report value             */
);

/*  ===========================================================================
  *
  *  FUNCTION rexl4_def_task_ext
  *
  *  DESCRIPTION
  *    Define a new rex task. All these REXL4 functions should be called by 
  *    REX API functions.
  *
  *  DEPENDENCIES
  *    This is an REXL4 internal function.    
  *    p_stack must be on an aligned boundary and the size must be the number
  *    of bytes in the stack
  * 
  *  PARAMETERS
  *    rexl4_def_task_ext takes two parameters in addition to the ones taken by
  *    rexl4_def_task. These allow the user to specify a name for the task and
  *    whether the task should be started in suspended mode or not. Note also
  *    that rex_def_task_ext treats stacks as arrays of characters and expects
  *    the stack size to be specified in units of bytes.
  *
  *  RETURN VALUE
  *    None.
  *
  *  SIDE EFFECTS
  *    None.
  *  ========================================================================*/
void rexl4_def_task_ext
(
#ifdef FEATURE_QDSP6
   unsigned int        hw_bitmask,
#endif
   rex_tcb_type         *p_tcb,              /* valid tcb for new task       */
   unsigned char*        p_stack,            /* stack for new task          */
   rex_stack_size_type   p_stksiz,           /* stack size in bytes        */
   rex_priority_type     p_pri,              /* priority for new task         */
   rex_task_func_type    p_task,             /* task startup function     */
   dword                 p_param,            /* parameter for new task     */
   char                 *p_tskname,          /* A/N string for task name   */
   boolean               p_susp              /* is task initially suspended?  */
);

/*  ===========================================================================
  *
  *  FUNCTION rexl4_def_task
  *
  *  DESCRIPTION
  *    Define a new rex task. All these REXL4 functions should be called by 
  *    REX API functions.
  *
  *  DEPENDENCIES
  *    This is an REXL4 internal function.    
  *    p_stack must be on an aligned boundary and the size must be the number
  *    of rex_stack_word_type in the stack
  * 
  *  PARAMETERS
  *    p_tcb - valid tcb for new task
  *    p_stack -stack for new task
  *    p_stksiz - stack size in units of rex_stack_word_type       
  *    p_pri - priority for new task
  *    p_task - task startup function
  *    p_param -parameter for new task
  *
  *  RETURN VALUE
  *    None.
  *
  *  SIDE EFFECTS
  *    None.
  *  ========================================================================*/
void rexl4_def_task
(
#ifdef FEATURE_QDSP6
   unsigned int        hw_bitmask,
#endif
   rex_tcb_type        *p_tcb,              /* valid tcb for new task       */
   void *               p_stack,            /* stack for new task           */
   rex_stack_size_type  p_stksiz,           /* stack size in units of
                                               rex_stack_word_type          */
   rex_priority_type    p_pri,              /* priority for new task        */
   void                 (*p_task)( dword ), /* task startup function        */
   dword                p_param             /* parameter for new task       */
);

/*  ===========================================================================
  *
  *  FUNCTION rexl4_init
  * 
  *  DESCRIPTION
  *    Initializes REX in L4 environment. 
  *  
  *  DEPENDENCIES
  *    p_tcb must be a valid task control block.
  *    p_task must be a valid function pointer.
  *
  *  PARAMETERS
  *    p_istack - interrupt stack      
  *    p_istksiz - interrupt stack size 
  *    p_tcb - task control block   
  *    p_stack - stack            
  *    p_stksiz - stack size           
  *    p_pri - task priority        
  *    p_task - task function        
  *    p_param - task parameter      
  *
  * RETURN VALUE
  *    None.
  *
  *  SIDE EFFECTS
  *    None.
  *  ========================================================================*/
void rexl4_init(
   rex_tcb_type       *p_tcb,             /* task control block   */
   void *              p_stack,            /* stack                */
   rex_stack_size_type p_stksiz,           /* stack size           */
   rex_priority_type   p_pri,              /* task priority        */
   void                (*p_task)( dword ), /* task function        */
   dword               p_param             /* task parameter       */
);

/* ============================================================================
  *
  *  FUNCTION rexl4_autodog_enable
  *
  *  DESCRIPTION
  *    Performs initialization needed to set up the auto dog service for a task.
  *    When auto-dog is on, the task does not need to wake up periodically to
  *    kick the dog when making a blocking REX call. REX takes care of notifying
  *    Dog that the task is blocked and that there is no need to monitor the
  *    task until further notice. When the task is scheduled again, Dog is
  *    notified that the task is ready to be monitored again.
  *
  *  DEPENDENCIES
  *    None.
  *
  *  PARAMETERS
  *    dog_report_val -dog report value
  *
  *  RETURN VALUE
  *    Returns the previous state of the autodog_enabled field in the TCB.
  *
  *  SIDE EFFECTS
  *    None.     
  *  ========================================================================*/
int rexl4_autodog_enable(int dog_report_val);

/*  ===========================================================================
  *
  *  FUNCTION rexl4_autodog_disable
  *
  *  DESCRIPTION
  *    Disables the auto dog service for a task.
  *
  *  DEPENDENCIES
  *    None.
  *
  *  PARAMETERS
  *    None.
  *
  *  RETURN VALUE
  *    Returns the previous state of the autodog_enabled field in the TCB.
  *
  *  SIDE EFFECTS
  *    None.        
  *  ========================================================================*/
int rexl4_autodog_disable(void);

/*===========================================================================
FUNCTION REX_START_TASK

DESCRIPTION
  Does the task initialization.It is called within REX and should not be
  called from outside of rex.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Starts the task by restoring its context
===========================================================================*/
/* REX_ATTR:    Private
 *              Required
 */
void rex_start_task( rex_tcb_type *p_tcb );

/*===========================================================================
FUNCTION REX_REMOVE_TASK

DESCRIPTION
  Function that implements the process of removing a TCB from
  the REX task list.
===========================================================================*/
void rex_remove_task(
  rex_tcb_type *p_tcb   /* pointer to tcb */
);

/* --------------------------------------------------------------------------
**
** REX Program Counter Trace Support
**
** -------------------------------------------------------------------------- */
/* ==================================================================
FUNCTION REX_PCDUMP_INIT_REGISTERS

DESCRIPTION
   Initializes FIQ-mode registers used by the PC dumping section of
   the FIQ handler.
===================================================================== */
void rex_pcdump_init_registers(
   unsigned long start_address,
   unsigned long end_address,
   unsigned long current_address
);

/* ==================================================================
FUNCTION REX_ENABLE_PC_PROFILE

DESCRIPTION
   Turns on PC dumping.
===================================================================== */
void rex_enable_pc_profile( void );

/* ==================================================================
FUNCTION REX_DISABLE_PC_PROFILE

DESCRIPTION
   Turns off PC dumping.
===================================================================== */
void rex_disable_pc_profile( void );

/* ==================================================================
FUNCTION REX_CLEAR_PC_PROFILE

DESCRIPTION
   Turns off PC dumping.
===================================================================== */
void rex_clear_pc_profile( void );

/*  ===========================================================================
  *
  *  FUNCTION rex_create_thread
  *
  *  DESCRIPTION
  *    This is an internal function which will be called by REX API function 
  *    to create new L4 (Iguana) thread.
  *
  *  DEPENDENCIES
  *      stack_ptr must be on an aligned boundary.
  *      stack size must be the number  of bytes in the stack.
  *      this function should only be called by other REX functions.
  *    
  *  PARAMETERS
  *
  *    entry_point - new task entry point
  *    param - parameter for new task entry point
  *    priority - new task's priority
  *    stack_ptr - stack pointer
  *    tcb_ptr - the pointer of the Task TCB
  *    p_susp - will this task be created initially suspended?
  *
  *  RETURN VALUE
  *    REX_SUCCESS/REX_FAILURE.
  *  
  *  SIDE EFFECTS
  *    The thread starts running only if it is the highest priority ready thread.
  *  ========================================================================*/
L4_Word_t rex_create_thread
(
   rex_task_func_type         entry_point,
   rex_task_func_param_type   param,  /* parameter for new task */
   rex_priority_type          priority,
   byte*                      stack_ptr,
   rex_tcb_type*              tcb_ptr,
   boolean                    p_susp /* is thread initially suspended? */  
);

/*===========================================================================
FUNCTION REX_FIRST_TASK

DESCRIPTION
  Assign input iterator to first task on rex_task_list.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the first task on rex_task_list.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type * rex_first_task(
   rex_tcb_type** iter
);

/*===========================================================================
FUNCTION REX_NEXT_TASK

DESCRIPTION
  Assign input iterator to the next task on rex_task_list.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the next task on rex_task_list.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type * rex_next_task(
   rex_tcb_type** iter
);

/*===========================================================================
FUNCTION REX_PREV_TASK

DESCRIPTION
  Assign input iterator to the prev task on rex_task_list.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the prev task on rex_task_list.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type * rex_prev_task(
   rex_tcb_type** iter
);

/* --------------------------------------------------------------------------
**
** REX Profiling Support
** Optional feature for OSes
** -------------------------------------------------------------------------- */
/* ==================================================================
FUNCTION REX_SET_TOTAL_TIME_SAMPLES

DESCRIPTION
   Sets the total number of time samples to the specified number.
===================================================================== */
void rex_set_total_time_samples( unsigned long );

/* ------------------------------------------------------------------------
**
** REX Dynamic Memory Management
**
** ------------------------------------------------------------------------ */

/* ==================================================================
FUNCTION REX_MALLOC
DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
void *rex_malloc(
   unsigned long num_bytes
);

/* ==================================================================
FUNCTION REX_CALLOC
DESCRIPTION
   Dynamically allocates a specified number of blocks of the specified
   size and returns a pointer to this chunk of memory.
===================================================================== */
void *rex_calloc(
   unsigned long num_blocks,
   unsigned long block_size
);

/* ==================================================================
FUNCTION REX_REALLOC
DESCRIPTION
  Resizes the ptr block of memory to be size bytes while preserving the
  block's contents.  If the block is shortened, bytes are discarded off the
  end.  If the block is lengthened, the new bytes added are not initialized
  and will have garbage values.  If heap_ptr is NULL, the contents of ptr are
  unchanged and the function silently returns NULL.  If ptr is NULL, the
  functions behaves exactly like mem_malloc (see above).  If ptr is not NULL
  and size is 0, the function behaves exactly like mem_free (see below).  If
  the block cannot be resized, ptr is not NULL and size is not 0, then NULL
  is returned and the original block is left untouched.  If the ptr block IS
  successfully resized and the returned value is different from the ptr value
  passed in, the old block passed in must be considered deallocated and no
  longer useable (i.e. do NOT try to mem_free it!)  This function will ASSERT
  if it can detect a bad pointer or a pointer to a free block within the
  range of memory managed by heap_ptr.  However, it's not always possible to
  do this.  Passing in a ptr block that is outside the memory managed by
  heap_ptr will result in a silent return of NULL with the contents of ptr
  unchanged.  This function will call mem_malloc if it can't grow the block
  in place, so the allocater failed function WILL be called if the heap is
  out of room to grow the block.

  Returns a pointer to the beginning of the resized block of memory (which
  may be different than ptr) or NULL if the block cannot be resized.
===================================================================== */
void *rex_realloc(
   void         *ptr,
   unsigned long size
);

/* ==================================================================
FUNCTION REX_FREE
DESCRIPTION
   Frees a chunk of memory allocated using rex_malloc.
===================================================================== */
void rex_free(
   void *mem_ptr
);

/* --------------------------------------------------------------------------
**
** REX Profiling Support
** Optional feature for OSes
** -------------------------------------------------------------------------- */
/* Profiling APIs */
void rex_enable_profiling( void );

void rex_disable_profiling( void );


/* ==================================================================
FUNCTION REX_GET_SLEEP_REPORT

DESCRIPTION
   Gets the number of sleep task time samples and the total number
   of time samples.
===================================================================== */
rex_sleep_report_type  rex_get_sleep_report( void );

/* ==================================================================
FUNCTION REX_ENABLE_SLEEP_REPORTING

DESCRIPTION
   Gets the number of sleep task time samples and the total number
   of time samples.
===================================================================== */
void  rex_enable_sleep_reporting(void);

/* ==================================================================
FUNCTION REX_DISABLE_SLEEP_REPORTING

DESCRIPTION
   Gets the number of sleep task time samples and the total number
   of time samples.
===================================================================== */
void  rex_disable_sleep_reporting(void);

/*===========================================================================

FUNCTION REX_CLR_PROFILE

DESCRIPTION
  Clears stored profiling information. Clears the numticks and
  rex_IRQnumticks fields in each TCB in the task list. The rex_pnumticks
  does not need to be cleared because it would be in sync with rex_time_tick2.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_clr_profile( void );

/*===========================================================================

FUNCTION REX_INIT_PROFILE

DESCRIPTION
  Performs initialization necessary for software profiling. Sets the
  rex_time_tick2 variable to the address passed in.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_init_profile(
   dword rex_time_tick2_addr
);

/*===========================================================================

FUNCTION REX_ENABLE_PROFILE

DESCRIPTION
  Enables software profiling by setting rex_enable_profile, clearing
  rex_pnumticks and restarting the clock.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_enable_profile( void );

/*===========================================================================

FUNCTION REX_DISABLE_PROFILE

DESCRIPTION
  Disables software profiling by resetting rex_profile_enabled, clearing
  rex_pnumticks and stopping the clock.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_disable_profile( void );

/*===========================================================================

FUNCTION REX_GET_PROFILE

DESCRIPTION
  Retrieves software profiling information. Traverses rex_task_list and gets
  the numticks field from each TCB in the task list. In addition, gets
  rex_IRQnumticks as well.

DEPENDENCIES
  Assumes that the TCB of each task is in rex_task_list.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
// void rex_get_profile(
   // sw_profile_type *sw_profile,
   // byte *num_task
// );

/* --------------------------------------------------------------------------
**
** REX Profiling Support
** -------------------------------------------------------------------------- */

/* ==================================================================
FUNCTION REX_GET_TOTAL_TIME_SAMPLES

DESCRIPTION
   Returns the total number of time samples.
===================================================================== */
unsigned long rex_get_total_time_samples( void );

/*===========================================================================
FUNCTION  REX_CREATE_TASK
DESCRIPTION
   Similar to rex_def_task. REX allocates space for the tcb and
   the stack. Returns a handle to the thread or NULL on error.  Creates
   an independently dispatchable context.
===========================================================================*/
rex_tcb_type*
   /* TCB pointer on success, NULL otherwise
   */
rex_create_task(
   rex_stack_size_type  stack_size,
      /* Requested stack size
      */
   rex_priority_type    priority,
      /* Requested priority
      */
   rex_task_func_type   thread_ptr,
      /* Thread function
      */
   unsigned long        param,
      /* Parameter to pass to thread function when it is executed
      */
   char                 *thread_name,
      /* ASCII string name for the thread, can be NULL
      */
   boolean              suspended
      /* Indicated whether or not the thread should be started
      ** in the suspended state.  When TRUE, the thread will be
      ** created but only scheduable after a call to zrex_resume_thread,
      ** FALSE indicates the thread is immediately scheduable after
      ** creation
      */
);

#endif //__REX_PRIVATE_DEPRECATED_H__
