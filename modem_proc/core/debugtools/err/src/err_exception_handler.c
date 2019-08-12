/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the AMSS exception handler

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2007-2015 by Qualcomm Technologies, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_exception_handler.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   abh     Added changes for extended smem logging
08/27/14   din     Replaced snprintf with mutex free calls.
07/31/14   din     Made some changes to support User PD.
07/02/14   din     Accomodate Qurt argument change to complete ERR_FATAL call flow.
04/18/14   din     Added changes for ADSP island mode.
04/03/14   psu     Update diag macros using optimized versions.
09/22/08   tbg     Merged in changes to support FEATURE_QUARTZ_20
07/25/07   tbg     Initial verion (pulled code from mobile.c)
===========================================================================*/

#include <stdio.h>
#include "comdef.h"
#include "erri.h"
#include "qurt/err_qurt.h"
#include "qurt_event.h"
#include "tms_utils.h"

typedef enum {
  EXCP_UNKNOWN,
  EXCP_SWI,
  EXCP_UNDEF_INST,
  EXCP_MIS_ALIGN,
  EXCP_PAGE_FAULT,
  EXCP_EXE_FAULT,
  EXCP_DIV_BY_0,
  EXCP_MAX_NUM
} exception_type;

char  exception_info[EXCP_MAX_NUM][14] = {
  "    :Excep  ",
  "    :No SWI ",
  "    :Undef  ",
  "    :MisAlgn",
  "    :PFault ",
  "    :XFault ",
  "    :DivBy0 ",
};

char              qxdm_dbg_msg[LEN_QXDM_DBG_MSG];

extern coredump_type coredump;
void err_exception_handler( void );
extern void err_fatal_post_exception_processing(void);
extern void err_fatal_jettison_core (unsigned int line, const char *file_name,
  const char *format, uint32 param1, uint32 param2, uint32 param3);
extern void err_emergency_error_recovery( void );
extern void sys_m_register_fatal_notification(void);
extern void err_update_coredump_tid_and_is_exception(uint32 tid, boolean is_exception);
extern void err_call_pre_STM_CB(uint32 tid);
extern void err_call_next_to_STM_CB(void);
extern void err_fatal_try_lock(void);

#ifdef UERR_ISLAND_MODE
extern void ubootstrap_exception_handler(void);
#endif /* UERR_ISLAND_MODE*/

#define MAX_FAILURE_COUNT 10

/*===========================================================================
FUNCTION err_has_err_fatal_occured

DESCRIPTION
  Checks if exception is raised because of error fatal

===========================================================================*/

static boolean err_has_err_fatal_occured
(
  unsigned int badva,
  unsigned int cause
)
{
   /*  Test for BADVA is an actual address */
   if ( QURT_EXCEPT_EXIT == cause)
   {
     return ERR_TEST_FOR_ERR_FATAL(badva);
   }

   return FALSE;

} /* err_has_err_fatal_occured */


/*===========================================================================
FUNCTION err_exception_handler

DESCRIPTION
  Handle IPC from QURT Kernel when exceptions occur.

===========================================================================*/
void err_exception_handler(void)
{
  unsigned int     tid;
  qurt_sysevent_error_t sys_err;
  union arch_coredump_union *p_regs=&coredump.arch.regs;
  unsigned int failure_count=0;
  
  sys_m_register_fatal_notification();
  
  #ifdef UERR_ISLAND_MODE
  ubootstrap_exception_handler();
  #endif /* UERR_ISLAND_MODE */
  
  for (;;)
  {
    /* Register self as Exception_Handler. */	
    tid = qurt_exception_wait2(&sys_err);

    /* Try acquiring err_fatal_mutex*/
    err_fatal_try_lock();

    if (-1==tid)
    {
      if ( sizeof(qxdm_dbg_msg) <= tms_utils_fmt(qxdm_dbg_msg, sizeof(qxdm_dbg_msg),
        "Failed to register with qurt_reg_error_handler: tid=%x", tid))
      {
        ERR_LOG_MSG_ERROR("Failed to copy failure result in qxdm_dbg_msg ");
      }
      ERR_LOG_MSG_FATAL_1("Failed to register with qurt_reg_error_handler: tid=%x", tid);
      failure_count++;
      if(failure_count >= MAX_FAILURE_COUNT)
      {
        /* Failed to register with qurt_reg_error_handler */
        err_emergency_error_recovery();
      }
      continue;
    }

    /* Executes Pre STM Callback */
    err_call_pre_STM_CB(tid);

    /* Halt other cores */
    ERR_FATAL_ENTER_SINGLE_THREADED_MODE();

    /* Update the registers */
    p_regs->name.pc    = sys_err.fault_pc;
    p_regs->name.sp    = sys_err.sp;
    p_regs->name.badva = sys_err.badva;
    p_regs->name.ssr   = sys_err.cause;

    /* There are four ways to reach here 
     * 1. Exception Caused by Main thread. ( Handled by if Part )
     * 2. Non recoverable exception raised by island exception handler. ( Handled by if Part )
     * 3. Error fatal in Main image. ( Handled by else Part )
     * 4. Error fatal in uImage. ( Handled by else Part )
     * */

    if ( !err_has_err_fatal_occured(sys_err.badva, sys_err.cause) )
    {
      /* Update tid and is_exception to coredump */
      err_update_coredump_tid_and_is_exception(tid, TRUE);
    
      ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_EXCEPTION_PRE);
	      
      err_call_next_to_STM_CB();

      if ( sizeof(qxdm_dbg_msg) <= tms_utils_fmt(qxdm_dbg_msg, sizeof(qxdm_dbg_msg),
        "ExIPC: Exception recieved tid=%x inst=%x", tid, sys_err.fault_pc) )
      {
        MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,"Failed to copy exception in qxdm_dbg_msg ");
      }
    
      /* Perform higher level error logging - no return */
      err_fatal_jettison_core ( 0, exception_info[EXCP_UNKNOWN], "Exception detected", 0, 0, 0);
    }
    else 
    {
      /* Update tid and is_exception to coredump */
      err_update_coredump_tid_and_is_exception(tid, FALSE);
      
      err_fatal_post_exception_processing();
    }
  }
} /* end of err_exception_handler */



