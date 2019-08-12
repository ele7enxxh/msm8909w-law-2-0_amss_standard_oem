/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides error reporting services for both fatal and
  non-fatal errors.  This module is not a task, but rather a set of
  callable procedures which run in the context of the calling task.

Copyright (c) 1992-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/15   din     Adapted from err.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "qurt.h"
#include "qurt_event.h"
#include "erri.h"
#include <stdlib.h>
#include <stringl.h>
#include "msg.h"
#include "DALSys.h"
#include "DDITimetick.h"


/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/
void err_fatal_handler( void ) ;
void err_fatal_jettison_core (unsigned int line, const char *file_name,
  const char *format, uint32 param1, uint32 param2, uint32 param3);



/*===========================================================================

                      Prototypes for external functions

===========================================================================*/
extern void dog_force_kick( void );
extern void err_update_image_versioning_info (void);
extern void err_emergency_error_recovery(void);
extern void err_raise_to_kernel(void);
extern void err_pd_init(void);

/*===========================================================================

                      External Data Vairables

===========================================================================*/

extern err_fatal_params_type err_fatal_params;

/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/
#define ERR_SET_AND_FLUSH_PTR(PTR, VAL) \
  do { \
     PTR = (void*)VAL; \
     asm volatile ("dccleana(%0)" : : "r" ((qurt_addr_t )( &PTR )));\
   } while (0)

#define ERR_FLUSH_ADDR(ADDR) \
  do { \
     asm volatile ("dccleana(%0)" : : "r" ((qurt_addr_t )( &ADDR )));\
   } while (0)

  
/* Struct used to hold coredump data */
coredump_type coredump;

static DalDeviceHandle* phTimetickHandle ;

/* The function tables below are processed by the error handler
 * in the following order:
 *   1. err_preflush_internal (one time)
 *   2. err_preflush_external - Part of coredump structure (one time)
 *   3. err_flush_internal    (one time - transitions to non-returning kernel code)
 */

/* Decalared in err_target_*.c */
extern const err_cb_ptr err_preflush_internal[];

/* Flush will be done by kernel */
static const err_cb_ptr err_flush_internal[] =
{
  err_raise_to_kernel,   /* Does not return */
  /* NULL must be last in the array */
  NULL
};

/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_INITIALIZE_COREDUMP

DESCRIPTION
  Initializes coredump

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void err_initialize_coredump (void)
{
  /* Set type and version values */
  coredump.version = ERR_COREDUMP_VERSION;
  coredump.arch.type = ERR_ARCH_COREDUMP_TYPE;
  coredump.arch.version = ERR_ARCH_COREDUMP_VER;  
  coredump.os.type = ERR_OS_COREDUMP_TYPE;
  coredump.os.version = ERR_OS_COREDUMP_VER;
  coredump.err.version = ERR_COREDUMP_VER;

} /* err_initialize_coredump */

/*===========================================================================

FUNCTION ERR_INIT

DESCRIPTION
  This function initializes error services and calls into target & process 
  based error intializing routine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  For Root PD on MPSS, Error log is loaded from NVM into RAM resident cache.

===========================================================================*/

void err_init (void)
{
  /* Init Timer */
  DalTimetick_Attach("SystemTimer", &phTimetickHandle);

  if(!err_fatal_params.err_fatal_mutex_init)
  {
    ERR_MUTEX_INIT(&err_fatal_params.err_fatal_mutex);
    err_fatal_params.err_fatal_mutex_init=TRUE;
  }

  err_pd_init();

  err_initialize_coredump();
  
  err_update_image_versioning_info();

  err_fatal_params.err_services_init_complete = TRUE;

} /* err_init */

/*===========================================================================

FUNCTION ERR_GET_TIMETICK

DESCRIPTION
  Add timetick value to specified address if timetick handle is initialized

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None .

===========================================================================*/

void err_get_timetick(DalTimetickTime64Type * tick)
{
  if (NULL != phTimetickHandle)
  {
    /* Best effort, no failure action */
    DalTimetick_GetTimetick64(phTimetickHandle, tick );
  }  
} /* err_get_timetick */

/*===========================================================================

FUNCTION ERR_UPDATE_COREDUMP_TID_AND_IS_EXCEPTION

DESCRIPTION
 Updates tid information to coredump only the tid is not updated earlier.

DEPENDENCIES

RETURN VALUE
  No return.

SIDE EFFECTS

===========================================================================*/

void err_update_coredump_tid_and_is_exception(uint32 tid, boolean is_exception)
{
  if ( coredump.err.tid == 0 )
  {
    coredump.err.tid = tid;
  }
  coredump.err.is_exception = is_exception;

} /* err_update_coredump_tid_and_is_exception */

/*===========================================================================

FUNCTION ERR_CALL_PRE_STM_CB

DESCRIPTION
 Calls err pre STM cb

DEPENDENCIES

RETURN VALUE
  No return.

SIDE EFFECTS

===========================================================================*/

void err_call_pre_STM_CB(uint32 tid)
{
  /* Kick Dog */
  dog_force_kick();
    
  /* Set the start time for preSTM callbacks */
  err_get_timetick( &(coredump.err.err_pre_STM.cb_start_tick) );
  
  ERR_FLUSH_ADDR( 
       coredump.err.err_pre_STM.cb_start_tick );

  if ( coredump.err.err_pre_STM.err_cb )
  {
    coredump.err.err_current_cb = 
	      (void*)coredump.err.err_pre_STM.err_cb;
    coredump.err.err_pre_STM.cb_start = TRUE;

    ERR_FLUSH_ADDR(coredump.err.err_current_cb);
    ERR_FLUSH_ADDR(coredump.err.err_pre_STM.cb_start);
 
    coredump.err.err_pre_STM.err_cb(tid);
  }

  /* Kick Dog */
  dog_force_kick();

} /* err_call_pre_STM_CB */

/*===========================================================================

FUNCTION ERR_CALL_NEXT_TO_STM_CB

DESCRIPTION
 Calls err next to STM cb

DEPENDENCIES

RETURN VALUE
  No return.

SIDE EFFECTS

===========================================================================*/

void err_call_next_to_STM_CB(void)
{
  /* Kick Dog */
  dog_force_kick();
    
  /* Set the start time for postflush callbacks */
  err_get_timetick( &(coredump.err.err_next_to_STM.cb_start_tick) );
  
  ERR_FLUSH_ADDR( 
       coredump.err.err_next_to_STM.cb_start_tick );

  if ( coredump.err.err_next_to_STM.err_cb )
  {
    coredump.err.err_current_cb = 
	      (void*)coredump.err.err_next_to_STM.err_cb;
    coredump.err.err_next_to_STM.cb_start = TRUE;

    ERR_FLUSH_ADDR(coredump.err.err_current_cb);
    ERR_FLUSH_ADDR(coredump.err.err_next_to_STM.cb_start);
 
    coredump.err.err_next_to_STM.err_cb();
  }

  /* Kick Dog */
  dog_force_kick();

} /* err_call_next_to_STM_CB */

/*===========================================================================

FUNCTION ERROR_FATAL_HANDLER

DESCRIPTION
  This function is invoked from err_fatal_jettison_core. When using JTAG,
  default breakpoint for ERR_FATAL should be placed at this function.
  Will log error to SMEM, kill the PA, and copy the coredump data into
  the err_data structure in unintialized memory.


DEPENDENCIES

RETURN VALUE
  No return.

SIDE EFFECTS
  **************************************************************
  ************ THERE IS NO RETURN FROM THIS FUNCTION ***********
  **************************************************************

===========================================================================*/
void err_fatal_handler ( void )
{
  int fptr_index;
  static uint32 err_count=0;

  /* Clean Cache */
  qurt_mem_cache_clean(0,0, QURT_MEM_CACHE_FLUSH_ALL, QURT_MEM_DCACHE);

  err_count++;

  if((err_count>1) || (err_fatal_params.err_services_init_complete!=TRUE))
  {
    err_initialize_coredump();
  
    /* May not return */
    err_emergency_error_recovery();
  }

  fptr_index=0;
  while(err_preflush_internal[fptr_index] != NULL)
  {
    /* Cycle through internal functions */
    ERR_SET_AND_FLUSH_PTR(coredump.err.err_current_cb, err_preflush_internal[fptr_index]); 
    err_preflush_internal[fptr_index]();
    fptr_index++;
  }

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_LOG_EXTERNEL_CBS_PRE);
	  
  for(fptr_index=0; fptr_index<ERR_MAX_PREFLUSH_CB; fptr_index++)
  {
    err_get_timetick( 
      &(coredump.err.err_preflush_external[fptr_index].cb_start_tick));

    ERR_FLUSH_ADDR(coredump.err.err_preflush_external[fptr_index].cb_start_tick);
    
    /* Cycle through external functions */
    if(coredump.err.err_preflush_external[fptr_index].err_cb!= 0)
    {
      /* Kick Dog */
      dog_force_kick();

      coredump.err.err_current_cb = 
	      (void*)coredump.err.err_preflush_external[fptr_index].err_cb;
      coredump.err.err_preflush_external[fptr_index].cb_start = TRUE;

      ERR_FLUSH_ADDR(coredump.err.err_current_cb);
      ERR_FLUSH_ADDR(coredump.err.err_preflush_external[fptr_index].cb_start);
 
      coredump.err.err_preflush_external[fptr_index].err_cb();
    }
  }

  /* Set the start time for non existent external cb, Can be used to calculate 
   * total time spanned by callbacks */
  err_get_timetick( 
      &(coredump.err.err_preflush_external[ERR_MAX_PREFLUSH_CB].cb_start_tick));
  
  /* err_preflush_external[] is of size ERR_MAX_PREFLUSH_CB + 1 */
  ERR_FLUSH_ADDR( 
       coredump.err.err_preflush_external[ERR_MAX_PREFLUSH_CB].cb_start_tick );

  /* Kick Dog */
  dog_force_kick();

  /* Set the start time for postflush callbacks */
  err_get_timetick( &(coredump.err.err_postflush_external.cb_start_tick));
  
  /* err_preflush_external[] is of size ERR_MAX_PREFLUSH_CB + 1 */
  ERR_FLUSH_ADDR( 
       coredump.err.err_postflush_external.cb_start_tick );

  if ( coredump.err.err_postflush_external.err_cb )
  {
    coredump.err.err_current_cb = 
	      (void*)coredump.err.err_postflush_external.err_cb;
    coredump.err.err_postflush_external.cb_start = TRUE;

    ERR_FLUSH_ADDR(coredump.err.err_current_cb);
    ERR_FLUSH_ADDR(coredump.err.err_postflush_external.cb_start);
 
    coredump.err.err_postflush_external.err_cb();

    /* Kick Dog */
    dog_force_kick();
  }

  /* Main loop (cache flush happens here, along with other
   * one-time post-flush operations */
  fptr_index=0;
  while(err_flush_internal[fptr_index] != NULL)
  {
    /* Cycle through internal functions */
    ERR_SET_AND_FLUSH_PTR(coredump.err.err_current_cb, err_flush_internal[fptr_index]); 
    err_flush_internal[fptr_index]();
    fptr_index++;
  }

} /* err_fatal_handler */


/*===========================================================================

FUNCTION ERR_FATAL_TRY_LOCK
DESCRIPTION
  Updates Error functionality at the entry of error. This is intentionally 
  used from exception handler.
============================================================================*/
void err_fatal_try_lock(void)
{
  if(err_fatal_params.err_fatal_mutex_init==TRUE)
  {
    ERR_MUTEX_TRY_LOCK(&err_fatal_params.err_fatal_mutex);
  }
  
  /* err_get_timetick is not island mode safe */
  err_get_timetick( &(coredump.err.err_handler_start_time));

  /* Kick Dog */
  dog_force_kick();

} /* err_fatal_try_lock */


/*===========================================================================

FUNCTION ERR_FATAL_CORE_DUMP
DESCRIPTION
  Logs fatal error information, including a core dump.

  NOTE: There is no return from this function.
============================================================================*/
void err_fatal_core_dump (
  unsigned int line,      /* From __LINE__ */
  const char   *file_name, /* From __FILE__ */
  const char   *format   /* format string */
)
{
  err_fatal_lock();
  err_fatal_jettison_core(line, file_name, format, 0, 0, 0);
}


/*===========================================================================

FUNCTION ERR_FATAL_JETTISON_CORE
DESCRIPTION
  Logs fatal error information, including a core dump.
  Not to be called directly by outside code -- for that, use the function
  err_fatal_core_dump().

  NOTE: There is no return from this function.
============================================================================*/
#define ERR_FATAL_EXCEPTION_REENTRANCY "ERR_FATAL_EXCEPTION_REENTRANCY"

void err_fatal_jettison_core (
  unsigned int line,       /* From __LINE__ */
  const char   *file_name, /* From __FILE__ */
  const char   *format,    /* format string */
  uint32 param1,
  uint32 param2,
  uint32 param3
)
{
  /* Store line number */
  coredump.err.linenum = line;

  /* Copy file name */
  if(file_name != 0)
  {
    (void) strlcpy((char *)coredump.err.filename,
                       (char *)file_name,
                       ERR_LOG_MAX_FILE_LEN);
  }

  /* Copy message string */
  if(format != 0)
  {
    (void) strlcpy((char *)coredump.err.message,
                       (char *)format,
                       ERR_LOG_MAX_MSG_LEN);
  }

  coredump.err.param[0]=param1;
  coredump.err.param[1]=param2;
  coredump.err.param[2]=param3;

  if (err_fatal_params.aux_msg[0] != 0)
  {
    (void) strlcpy((char *)coredump.err.aux_msg,
                       (char *)err_fatal_params.aux_msg,
                       ERR_LOG_MAX_MSG_LEN);
  }

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_ERR_FATAL_JETTISION_CORE_POST);

  /* Update the stack_error info on Coredump */
  coredump.err.stack_error |= STACK_ERR_NO_INFO;

  /* Update the coredump pid */
  coredump.err.pd_id = qurt_process_get_id(); 

  /* Check if an user exceptions and ERR_FATAL arrived simultaneously */
  if ( err_fatal_params.crumb_trail_bmsk != 0 && 
        coredump.err.is_exception == TRUE)
  {
    /* Record secondary failure to coredump */
    strlcpy(coredump.err.int_msg, ERR_FATAL_EXCEPTION_REENTRANCY, 
	    sizeof(ERR_FATAL_EXCEPTION_REENTRANCY));   
  } 

  /* Call ERR_FATAL handler (no return) */
  err_fatal_handler();

}


/*=========================================================================

FUNCTION err_crash_cb_register

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called after an ERR_FATAL
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.

  !!!These functions MUST NOT call ERR_FATAL/ASSERT under ANY circumstances!!!

DEPENDENCIES
  None

RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  None

===========================================================================*/
boolean err_crash_cb_register(err_cb_ptr cb)
{
  int i;
  boolean rval = FALSE;

  for(i=0; i<ERR_MAX_PREFLUSH_CB; i++)
  {
	if(coredump.err.err_preflush_external[i].err_cb == NULL)
	{
	  coredump.err.err_preflush_external[i].err_cb = cb;
	  rval = TRUE;
	  break;
	}
  }

  return rval;
}


/*=========================================================================

FUNCTION err_crash_cb_dereg

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
boolean err_crash_cb_dereg(err_cb_ptr cb)
{
  int i;
  boolean rval = FALSE;

  for(i=0; i<ERR_MAX_PREFLUSH_CB; i++)
  {
	if(coredump.err.err_preflush_external[i].err_cb == cb)
	{
	  coredump.err.err_preflush_external[i].err_cb = NULL;
	  rval = TRUE;
	  break;
	}
  }

  return rval;
}

/*=========================================================================

FUNCTION err_crash_cb_reg_pre_STM

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called immediately before
  STM API call 
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.
 
  !!!These functions MUST NOT call ERR_FATAL under ANY circumstances!!!

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  Only one registration of such API is supported so if its used by more than one
  clients than it will overwrite the old registered callback.
  
===========================================================================*/
boolean err_crash_cb_reg_pre_STM(err_cb_ptr_u32 cb)
{
  if(NULL == coredump.err.err_pre_STM.err_cb )
  {
    /* Check if already a callback registered*/
    coredump.err.err_pre_STM.err_cb = cb;
    return TRUE;
  }
  else{
     return FALSE;
  }
} /* err_crash_cb_reg_pre_STM */

/*=========================================================================

FUNCTION err_crash_cb_dereg_pre_STM

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
boolean err_crash_cb_dereg_pre_STM(err_cb_ptr_u32 cb)
{
  if(coredump.err.err_pre_STM.err_cb == cb)
  {
    coredump.err.err_pre_STM.err_cb = NULL;
    return TRUE;
  }
  else{
    return FALSE;
  }
} /* err_crash_cb_dereg_pre_STM */

/*=========================================================================

FUNCTION err_crash_cb_reg_next_to_STM

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called immediately after
  STM API call 
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.
 
  !!!These functions MUST NOT call ERR_FATAL under ANY circumstances!!!

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  Only one registration of such API is supported so if its used by more than one
  clients than it will overwrite the old registered callback,
  this API was provided only for special case handling to stop ULT Audio Core
  in DPM PL
===========================================================================*/
boolean err_crash_cb_reg_next_to_STM(err_cb_ptr cb)
{
  if(NULL == coredump.err.err_next_to_STM.err_cb )
  {
    /* Check if already a callback registered*/
    coredump.err.err_next_to_STM.err_cb = cb;
    return TRUE;
  }
  else{
     return FALSE;
  }
}
/*=========================================================================

FUNCTION err_crash_cb_dereg_next_to_STM

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
boolean err_crash_cb_dereg_next_to_STM(err_cb_ptr cb)
{
  if(coredump.err.err_next_to_STM.err_cb == cb)
  {
    coredump.err.err_next_to_STM.err_cb = NULL;
    return TRUE;
  }
  else{
    return FALSE;
  }
}

/*===========================================================================
FUNCTION err_crash_cb_postflush_register
DESCRIPTION
  Register for callback function. The Callback function will be called after
  external functions are already called.
  It will be upto the callback to resolve its cache issues. 
============================================================================*/


boolean err_crash_cb_postflush_register (err_cb_ptr cb)
{
  if(NULL == coredump.err.err_postflush_external.err_cb )
  {
    /* Check if already a callback registered*/
    coredump.err.err_postflush_external.err_cb = cb;
    return TRUE;
  }
  else{
     return FALSE;
  }

} /* err_crash_cb_postflush_register*/

boolean err_crash_cb_postflush_deregister (err_cb_ptr cb)
{
  if(coredump.err.err_postflush_external.err_cb == cb)
  {
    coredump.err.err_postflush_external.err_cb = NULL;
    return TRUE;
  }
  else{
    return FALSE;
  }
} /* err_crash_cb_postflush_deregister */
	

/*=========================================================================

FUNCTION err_update_crumb_trail_mask

DESCRIPTION
  Updates Crumb trail mask

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void err_update_crumb_trail_mask(uint32 bits)
{
  ERR_CRUMB_TRAIL_BMSK(bits);

} /* err_update_crumb_trail_mask*/

/*=========================================================================

FUNCTION err_update_reentracny_flag

DESCRIPTION
  Updates Crumb trail mask

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define ERR_REENTRANCY_STRING "ERR_FATAL reentrancy violation, remove cb until resolved"
void err_update_reentrancy_flag(void)
{
  /* Record secondary failure to coredump */
  strlcpy(coredump.err.int_msg, ERR_REENTRANCY_STRING, 
          sizeof(ERR_REENTRANCY_STRING));

} /* err_update_reentrancy_flag */
