/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M T A S K . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

The module contains DSM Task functions to augment DSM memory pool at run time from external memory.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/src/dsmtask.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/13/13    sh     Implemented FEATURE_DSM_AUGMENT
===========================================================================*/

#include "core_variation.h"
#include "comdef.h"

#include "qw.h"
#include "dog.h"
#include "task.h"
#include "err.h"
#include "rex.h"
#include "rcevt.h"
#include "rcinit.h"
#include "dsmtask.h"
#include "timer.h"


#ifdef FEATURE_DSM_AUGMENT
#include "dsm_pool.h"
#include "dsm.h"
#include "dsmi.h"
#endif


rex_tcb_type *  dsm_tcb_ptr = NULL;

#ifdef FEATURE_DSM_AUGMENT
uint32 dsmtask_cleanup_timer_dur = DSMI_AUGMENT_BUFFER_FREE_TIMER_DURATION;
rex_timer_type dsmtask_cleanup_timer;
#endif

/*===========================================================================
FUNCTION dsm_task_main()

DESCRIPTION
  dsm task entry point

DEPENDENCIES
  The Task should run in higher priority than all the DSM Clients

PARAMETERS 

RETURN VALUE
  None

SIDE EFFECTS  
===========================================================================*/   

void dsm_task_main
(
  /* Unused parameter - required to match rex_task_func_type prototype */
  dword                           ignored    /*lint -esym(715,ignored) */
)
{

#ifdef FEATURE_DSM_AUGMENT 
    /* Signals returned from rex wait procedure */
    rex_sigs_type                   sigs;
#endif /*FEATURE_DSM_AUGMENT*/
    
    /* RCEVT_SIGEX term_sig; */    

    rcinit_handshake_startup();

    dsm_tcb_ptr = rex_self();

#ifdef FEATURE_DSM_AUGMENT 
    rex_def_timer( &dsmtask_cleanup_timer, dsm_tcb_ptr, DSM_TASK_CLEANUP_TIMER_SIG );

    /* Need not do DOG petting for starvation */
    
    for (;;)    
    {
      sigs = rex_wait
             (
               DSM_TASK_POOL_AUGMENT_REQ |
               DSM_TASK_POOL_AUGMENT_CLEANUP_REQ |
               DSM_TASK_CLEANUP_TIMER_SIG
             );

      (void) rex_clr_sigs( dsm_tcb_ptr, sigs );
   
      if( sigs & DSM_TASK_POOL_AUGMENT_REQ )
      {
         (void) rex_clr_sigs (rex_self(), DSM_TASK_POOL_AUGMENT_REQ);
         dsm_augment_pools();
      }
    
      if( sigs & DSM_TASK_POOL_AUGMENT_CLEANUP_REQ )
      {
        (void) rex_clr_sigs (rex_self(), DSM_TASK_POOL_AUGMENT_CLEANUP_REQ);
        if (!rex_get_timer(&dsmtask_cleanup_timer))
        {
          rex_set_timer( &dsmtask_cleanup_timer, dsmtask_cleanup_timer_dur );          
        }
      }

      if( sigs & DSM_TASK_CLEANUP_TIMER_SIG )
      {
        (void) rex_clr_sigs (rex_self(), DSM_TASK_CLEANUP_TIMER_SIG);
        if (!dsm_augment_free_buffer())/* Handle timer signal */
        {
          rex_set_timer( &dsmtask_cleanup_timer, dsmtask_cleanup_timer_dur );
        }
      }
      
      /*
         if( sigs & DSM_TERM_SIG )
         {
           rcinit_unregister_term_group(RCINIT_GROUP_6, RCEVT_SIGEX_TYPE_SIGREX, &term_sig);
           rex_clr_sigs(rex_self(), DSM_TERM_SIG);
           break;
          }
        */
      
    }
#endif /* FEATURE_DSM_AUGMENT */
    
  } /* dsm_task */

#ifdef FEATURE_DSM_AUGMENT
  /*===========================================================================
  FUNCTION dsm_task_augment_alloc_req()
  
  DESCRIPTION
     The function is called from DSM library to signal allocation request
     from external memory to DSM Task
      
  DEPENDENCIES
    None
  
  PARAMETERS
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
  ===========================================================================*/
  void dsm_task_augment_alloc_req(void)
  {
    (void) rex_set_sigs(dsm_tcb_ptr, DSM_TASK_POOL_AUGMENT_REQ );
  }
  
  /*===========================================================================
  FUNCTION dsm_task_augment_free_req()
  
  DESCRIPTION
    The function is called from DSM library to signal free request
     to DSM Task to free buffers to external memory.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
  ===========================================================================*/
  void dsm_task_augment_free_req(void)
  {
   (void) rex_set_sigs(dsm_tcb_ptr, DSM_TASK_POOL_AUGMENT_CLEANUP_REQ );
  }
  
#endif /* FEATURE_DSM_AUGMENT */





