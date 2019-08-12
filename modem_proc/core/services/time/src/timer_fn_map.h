#ifndef timer_fn_map_H
#define timer_fn_map_H

/*=============================================================================

       T I M E   S E R V I C E   TIME FUNCTION MAP FILE

GENERAL DESCRIPTION
  Initializes and maintains function pointers used in time code. These 
  function pointers will be populated based on a specific device. For example, 
  depending on which PMIC driver is used, the function pointers will be 
  populated based on the pmic model used.

REGIONAL FUNCTIONS
  time_fn_map_initialize_fp
    Initialize the function pointers

  time_fn_map_get_pmic_time
  time_fn_map_set_pmic_time
  time_fn_map_set_rtc_display_mode
  time_fn_map_get_rtc_display_mode
  time_fn_map_get_rtc_alarm
  time_fn_map_reset_rtc_alarm
  time_fn_map_is_rtc_valid
  time_fn_map_set_irq_handle
  time_fn_map_clear_irq
  time_fn_map_poll_time_for_boundary
  time_fn_map_rw_alarm

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_fn_map_initialize_fp() must be called after the PMIC initializes.


Copyright (c) 2009 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/timer_fn_map.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/24/12   cp      Created File.

=============================================================================*/

#include "comdef.h"

#ifndef FEATURE_TIMER_NO_NPA
#include "npa.h"
#endif

#include "DDIInterruptController.h"

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

/*=============================================================================
                           Structure
=============================================================================*/

/** Function pointer structure type */
typedef struct 
{
    /* Suppress compiler warnings */
    #ifndef FEATURE_TIMER_NO_NPA
    npa_client_handle (*timer_npa_create_sync_client)(const char*,  const char*, npa_client_type, unsigned int, void*);  
    void  (*timer_npa_complete_request)(npa_client_handle);
    void  (*timer_npa_issue_suppressible_request)(npa_client_handle, npa_resource_state);
    void  (*timer_npa_resource_available_cb)(const char*, 
                                             void (*timer_cpu_cb_fn)(void *context, 
                                                                     unsigned int event_type, 
                                                                     void *data, 
                                                                     unsigned int data_size), 
                                             void*);
    #else
    void* (*timer_npa_create_sync_client)(const char*,  const char*, uint32, uint32, void*);  
    void  (*timer_npa_complete_request)(void*);
    void  (*timer_npa_issue_suppressible_request)(const char*, uint32);
    void  (*timer_npa_resource_available_cb)(const char*, 
                                             void (*timer_cpu_cb_fn)(void *context, 
                                                                     unsigned int event_type, 
                                                                     void *data, 
                                                                     unsigned int data_size), 
                                             void*);
    #endif

    DALResult  (*timer_register_isr)(DalDeviceHandle * _h, 
                                     DALInterruptID  intrID,
                                     const DALISR,
                                     const DALISRCtx  ctx, 
                                     uint32  IntrFlags);

    #ifndef FEATURE_TIMER_NO_NPA
    npa_client_type timer_npa_client_required;
    uint32 timer_npa_max_state;
    void*  timer_npa_clk_client;
    #else
    uint32 timer_npa_client_required;
    uint32 timer_npa_max_state;
    void*  timer_npa_clk_client;
    #endif /*FEATURE_TIMER_NO_NPA*/

    uint32 timer_interrupt_id;
    uint32 timer_interrupt_flag;

}timer_fn_map_fp_type;

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
);

#endif /*timer_fn_map_H*/
