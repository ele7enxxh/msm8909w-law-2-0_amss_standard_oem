/*=============================================================================

       T I M E   S E R V I C E   TIMER FUNCTION MAP FILE

GENERAL DESCRIPTION
  Initializes and maintains function pointers used in timer code. These 
  function pointers will be populated based on a specific device.

REGIONAL FUNCTIONS
  time_fn_map_initialize_fp
    Initialize the function pointers

    timer_init_fn_ptrs() 
 
Copyright (c) 2012 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/timer_fn_map.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/24/12   cp      Created File.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "timer_fn_map.h"

/* Interrupt ID Vector */
/* See if this is configurable in SCons */
#define TIMER_INTERRUPT_ID        3 /*qtmr_phy_irq[1]*/
#define TIMER_NPA_REQUEST_KHZ     384000 /* 384 MHz */

/** 
@ingroup Timer Function Pointers
         
@brief  
 Initializes the Timer Function Pointers
 \n\n

@param[in]                                                               
 timer_fn_map_fp_type - Pointer to structure containing function
                       pointer \n to be initialized
                       \n\n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                   \n\n

*/
void timer_fn_map_initialize_fp
(
  timer_fn_map_fp_type* timer_fn_map_fp
)
{
  timer_fn_map_fp->timer_npa_create_sync_client         = NULL;
  timer_fn_map_fp->timer_npa_complete_request           = NULL;
  timer_fn_map_fp->timer_npa_issue_suppressible_request = NULL;
  timer_fn_map_fp->timer_npa_resource_available_cb      = NULL;
  timer_fn_map_fp->timer_npa_max_state                  = 0;
  timer_fn_map_fp->timer_npa_clk_client                 = NULL;
  timer_fn_map_fp->timer_register_isr                   = NULL;
  timer_fn_map_fp->timer_interrupt_id                   = 0;
  timer_fn_map_fp->timer_interrupt_flag                 = 0;

  #ifndef FEATURE_TIMER_NO_NPA
  timer_fn_map_fp->timer_npa_client_required            = (npa_client_type)0;
  #else /*FEATURE_TIMER_NO_NPA*/
  timer_fn_map_fp->timer_npa_client_required            = 0;
  #endif /*FEATURE_TIMER_NO_NPA*/

  #ifndef FEATURE_TIMER_NO_NPA
  timer_fn_map_fp->timer_npa_create_sync_client         = npa_create_sync_client_ex;
  timer_fn_map_fp->timer_npa_complete_request           = npa_complete_request;
  timer_fn_map_fp->timer_npa_issue_suppressible_request = npa_issue_scalar_request;
  timer_fn_map_fp->timer_npa_resource_available_cb      = npa_resource_available_cb;
  timer_fn_map_fp->timer_npa_client_required            = NPA_CLIENT_REQUIRED;
  timer_fn_map_fp->timer_npa_max_state                  = TIMER_NPA_REQUEST_KHZ;
  timer_fn_map_fp->timer_npa_clk_client                 = NULL;
  #endif /*FEATURE_TIMER_NO_NPA*/

  #ifndef FEATURE_TIMER_USE_QURT_SYSCLOCK
  timer_fn_map_fp->timer_register_isr                   = DalInterruptController_RegisterISR;
  timer_fn_map_fp->timer_interrupt_id                   = TIMER_INTERRUPT_ID;
  timer_fn_map_fp->timer_interrupt_flag                 = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
  #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

}

