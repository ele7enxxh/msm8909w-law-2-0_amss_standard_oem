#ifndef QURT_EVENT_H
#define QURT_EVENT_H
/*======================================================================
                        qurt_event.h

GENERAL DESCRIPTION
  Prototypes of Kernel event API functions      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011
             by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 
                                  EDIT HISTORY FOR FILE
 
    This section contains comments describing changes made to the
    module. Notice that changes are listed in reverse chronological
    order.
 
   $Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_event.h#1 $ 
   $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
 ======================================================================*/
#include "qurt_consts.h"

/**
 * system evn object type
 */

typedef struct qurt_sysenv_swap_pools {
   unsigned int spoolsize;
   unsigned int spooladdr;
}qurt_sysenv_swap_pools_t;


typedef struct qurt_sysenv_app_heap {
   unsigned int heap_base;
   unsigned int heap_limit;
} qurt_sysenv_app_heap_t ;


typedef struct qurt_sysenv_arch_version {
   unsigned int arch_version;
}qurt_arch_version_t;


typedef struct qurt_sysenv_max_hthreads {
   unsigned int max_hthreads;
}qurt_sysenv_max_hthreads_t;

typedef struct qurt_sysenv_max_pi_prio {
    unsigned int max_pi_prio;
}qurt_sysenv_max_pi_prio_t;

typedef struct qurt_sysenv_timer {
   unsigned int base_prio;
   unsigned int intno_bitmask;
}qurt_sysenv_timer_t;

typedef struct qurt_sysenv_procname {
   unsigned int asid;
   char name[32];
}qurt_sysenv_procname_t;

/*=============================================================================
                                    FUNCTIONS
=============================================================================*/

/*======================================================================*/
/**
  qurt_sysenv_get_swap_spool0
  get environment swap pool 0  information from kernel.
               
   @param pools [OUT] returns pools info.
   @return            return EOK for SUCCESS
*/
int qurt_sysenv_get_swap_spool0 (qurt_sysenv_swap_pools_t *pools );

/**
  qurt_sysenv_get_swap_spool1
  get environment swap pool 1  information from kernel.
               
   @param pools [OUT] returns pools info.
   @return            return EOK for SUCCESS
*/
int qurt_sysenv_get_swap_spool1(qurt_sysenv_swap_pools_t *pools );

/**
  qurt_sysenv_get_app_heap
  get environment swap pool 1  information from kernel.
               
   @param pools [OUT] returns app heap info.
   @return            return EOK for SUCCESS
*/
int qurt_sysenv_get_app_heap(qurt_sysenv_app_heap_t *aheap );

/**
  qurt_sysenv_get_hw_timer
  get environment swap HW timer  information from kernel.
               
   @param pools [OUT] returns timer info.
   @return            return EOK for SUCCESS
*/
int qurt_sysenv_get_hw_timer(qurt_sysenv_timer_t *timer );

/**
  qurt_sysenv_get_serv_timer
  get environment swap Server  information from kernel.
               
   @param pools [OUT] returns timer info.
   @return            return EOK for SUCCESS
*/
int qurt_sysenv_get_serv_timer(qurt_sysenv_timer_t *timer);

/**
  qurt_sysenv_get_ist_timer
  get environment swap IST information from kernel.
               
   @param pools [OUT] returns timer info.
   @return            return EOK for SUCCESS
*/
int qurt_sysenv_get_ist_timer(qurt_sysenv_timer_t *timer);

/**
  qurt_sysenv_get_arch_version
  get environment arch version information from kernel.
               
   @param pools [OUT] returns arch version info.
   @return            return EOK for SUCCESS
*/
int qurt_sysenv_get_arch_version(qurt_arch_version_t *vers);

/**
  qurt_sysenv_get_max_hw_threads
  get environment max hw threads information from kernel.
               
   @param pools [OUT] returns max hw threads info.
   @return            return EOK for SUCCESS
*/
int qurt_sysenv_get_max_hw_threads(qurt_sysenv_max_hthreads_t *mhwt );

/**
  qurt_sysenv_get_max_pi_prio
  get environment max pi prio information from kernel.
               
   @param pools [OUT] returns max pi prio info.
   @return            return EOK for SUCCESS
*/
int qurt_sysenv_get_max_pi_prio(qurt_sysenv_max_pi_prio_t *mpip );

/**
  qurt_sysenv_get_process_name
  get environment process name information from kernel.
               
   @param pools [OUT] returns process name info.
   @return            return EOK for SUCCESS
*/
int qurt_sysenv_get_process_name(qurt_sysenv_procname_t *pname );

/**
 qurt_exception_wait
 register an error handler for an event. It will block the caller if the 
 subscribed event doesn't happen.
 If success, it returns with payload that describe the detail of the event
 (thread_id, fault PC, stack pointer, bavad, cause)

   @param ip - fault program counter
   @param sp - stack pointer
   @param badva - bad virtual address
   @param cause - error cause    

 @return: registry status
 - QURT_EFATAL for FAILURE
 - otherwise thread ID for SUCCESS
*/
unsigned int qurt_exception_wait (unsigned int *ip, unsigned int *sp,
                                  unsigned int *badva, unsigned int *cause);

/**
 qurt_exception_raise_nonfatal
  Exit called from user. Indicate Simulator about exit.
  On target, handle it like an exception. If exit is called
  from Exception handler, we have to treat it like a fatal
  error. In other words, Exception handler can use exit
  to shut down the processor.

 @param error

 @return: QURT_EFATAL
*/
int qurt_exception_raise_nonfatal (int error);


/**
 qurt_exception_raise_fatal
  Error Fatal exit called from user. it saves global registers
  It stops other hthreads, marks global interupt, saves tlb table entries
  flushes cache and updates cache flush status.

 @param None
 
 @return None
*/
void qurt_exception_raise_fatal (void);


/**
 qurt_enable_floating_point_exception
  Enable floating point exception called from user. it sets floating point
  exception bits in USR register, and return the content of USR register.
  This function need to call before floating point usage, so it triggers
  the floating point exception when it occures.

 @param mask - floating point bit mask
   QURT_FP_EXCEPTION_ALL        
   QURT_FP_EXCEPTION_INEXACT    
   QURT_FP_EXCEPTION_UNDERFLOW  
   QURT_FP_EXCEPTION_OVERFLOW   
   QURT_FP_EXCEPTION_DIVIDE0    
   QURT_FP_EXCEPTION_INVALID    
 @return USR register setting
*/
unsigned int qurt_enable_floating_point_exception(unsigned int mask);

int qurt_send_system_event(int, void *);

#endif /* QURT_EVENT_H */

