
/*!
  @file
  RF Task main control.

  @brief
  This file contains functions specific/internal to the RF Task.

  @defgroup RF_TASK
  @ingroup RF_TASK
  @{
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright(c) 2009 - 2012 by Qualcomm Technologies, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rf_task_common_services.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------- 
10/03/12    av     Initial revision
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rf_task_common_services.h"
#include "ftm.h"


rf_task_data_type rf_task_data[RF_MAX_TASK];


/*!
  @brief
  Function to initialize the rf_task_data.
*/

void rf_task_initialize_data(rf_task_num_type task_num)
{
    rf_task_data[task_num].is_rfa_task_running = FALSE;
    rf_task_data[task_num].rf_task_dog_rpt_var = 0;
    rf_task_data[task_num].rf_task_dog_rpt_var = 0xffff;
    rf_task_data[task_num].tcb_ptr = rex_self();
    

}

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

/*!
  @brief
  Inline function to kick the watchdog timer and set a timer to wake it again.
*/
void rf_task_kick_watchdog( rf_task_num_type task_num )
{
  /* If dynamic watch dog is supported then the report time could be zero. Zero means don't report. */
  if(rf_task_data[task_num].rf_task_dog_rpt_time_var > 0)    
  {
    /* Report DOG */
    dog_report(rf_task_data[task_num].rf_task_dog_rpt_var);
  
    /* Set timer for next report */
    (void) rex_set_timer(&(rf_task_data[task_num].rf_rpt_timer), rf_task_data[task_num].rf_task_dog_rpt_time_var);
  }

  return;
}

/*!
  @brief
  Function to initialize the watchdog (static or dynamic) timer.
*/
void rf_task_init_watchdog( rf_task_num_type task_num )
{

  #ifdef FEATURE_RF_TASK_WDOG_DYNAMIC
  /*DOG_DEADLKCHK_ENABLE enables checking for deadlocks where an ERR_FATAL is
  **thrown if the task blocks on a mutex or critical section for excessive periods.
  **If for any reason, the task does expect that normal operation to block for 
  **long periods on a mutex or critical section then the DOG_DEADLKCHK_DISABLE
  **option should be used to prevent dog from performing deadlock monitoring on this task.*/

    rf_task_data[task_num].rf_task_dog_rpt_var      = dog_register( rf_task_data[task_num].tcb_ptr, \
                                             DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
    rf_task_data[task_num].rf_task_dog_rpt_time_var = dog_get_report_period( rf_task_data[task_num].rf_task_dog_rpt_var );

  /* If the return value from dog_get_report_period() was zero then we MUST
  ** NOT report to dog directly, as DOG will monitor us in a different manner.
  ** Hence we won't initialize or start any timer.
  */
  if(rf_task_data[task_num].rf_task_dog_rpt_time_var > 0)
  #endif /* ! FEATURE_RF_TASK_WDOG_DYNAMIC */

  /* Initialize taskname watchdog report timer */
  rex_def_timer(&(rf_task_data[task_num].rf_rpt_timer), rf_task_data[task_num].tcb_ptr, RF_RPT_TIMER_SIG); /* Dog Report */

  rex_def_timer( &(rf_task_data[task_num].rf_rpt_timer), rf_task_data[task_num].tcb_ptr, FTM_CLK_WAIT_SIG );

  return;
}


/*!
  @brief
  rfa_is_task_running

  @details
  Returns the status of the RF Task. If task is running returns TRUE else FALSE.

  @param void
*/
boolean rfa_is_task_running(rf_task_num_type task_num)
{
    return(rf_task_data[task_num].is_rfa_task_running);
}/* rfa_is_task_running */


/*!
  @brief
  rfa_is_task_running

  @details
  sets status of the rf task

  @param void
*/
void rfa_set_task_status(rf_task_num_type task_num, boolean active)
{
    rf_task_data[task_num].is_rfa_task_running = active;
}/* rfa_is_task_running */


/*!
  @brief
  Function to initialize the watchdog (static or dynamic) timer.
*/
void rf_task_deinit_watchdog( rf_task_num_type task_num )
{
 /* Cancel the watchdog report timer. */
   rex_clr_timer(&(rf_task_data[task_num].rf_rpt_timer));

  #ifdef FEATURE_RF_TASK_WDOG_DYNAMIC
         /*deregister dog service */
    dog_deregister(rf_task_data[task_num].rf_task_dog_rpt_var);
  #endif /* FEATURE_RF_TASK_WDOG_DYNAMIC */
}

/*!
  @brief
  rfa_is_task_running

  @details
  sets status of the rf task

  @param void
*/
rex_timer_type* rfa_get_watchdog_timer(rf_task_num_type task_num)
{
    return(&rf_task_data[task_num].rf_rpt_timer);
}/* rfa_is_task_running */

/*!
  @brief
  rfa_is_task_running

  @details
  sets status of the rf task

  @param void
*/
rex_timer_type* rfa_get_wait_timer(rf_task_num_type task_num)
{
    return(&rf_task_data[task_num].rf_wait_timer);
}/* rfa_is_task_running */
