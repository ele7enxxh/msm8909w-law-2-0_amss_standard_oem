/*!
  @file
  ftm_common_timer.c
 
  @brief
  This file contains APIs needed for FTM timer operation.
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common_timer.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/17/13   aro     Cleaned up unused time profiler code
04/02/13   aro     Debug flag around F3
03/11/13   sar     Updated file for APQ, GNSS only target.
03/05/13   aro     Migrated to XO for time profiling
03/05/13   aro     Created common time snapshot and conversion functions to
                   be used by FTM timer
03/01/13   aro     Enable ATS timer
03/01/13   aro     Removed DAL detach
03/01/13   aro     Added command to perform HW timer test
03/01/13   aro     Added on-target test case for timer
03/01/13   aro     Flag to switch between ATS and OSTMR timer
03/01/13   aro     Added ATS timer update and deconfigure function
02/28/13   aro     Doxygen Update
02/28/13   aro     Added initial implementation of ATS timer
03/18/12   jfc     Remove tramp.h references
03/02/12   aro     F3 message update to print error for timer failure only
03/02/12   aro     Updated Timer interfaces to return enumeration as timer
                   action status
09/20/11   aro     Documentation Update
08/25/11   aro     Renamed functions to better match the functionality
08/18/11   aro     Documentation Update
08/18/11   aro     Added execution status return all timer functions
08/18/11   aro     Compiler Error Fix
08/18/11   aro     Added timer services based install and uninstall functions
08/18/11   aro     Partitioned Precise and TimerServices based timer data into
                   separate data structure
08/17/11   aro     Enabled dynamic update of timer duration
08/17/11   aro     Renamed callback function type
08/17/11   aro     Replaced timer functions with new IRQ enable and disable
                   functions
08/17/11   aro     F3 message update
08/12/11   aro     Added interface to calculate the irq strobe count for given
                   timer interval
08/12/11   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "ftm_common_timer.h"
#include "ftm_common_data.h"
#include "ftm_common_msm.h"
#include "ftm_msg.h"
#include "ftm_common_locks.h"
#include "rf_test_task_v.h"
#include "rfcommon_time_profile.h"

/*============================================================================*/
/*!
  @addtogroup FTM_TIMER_ATS
 
  @brief
  This module contains high priority ATS based timer to be used by FTM
  @{
*/

/*----------------------------------------------------------------------------*/
/*! Enumeration to define the O_STMR system time warp around value */
#define FTM_ATS_WRAP_AROUND -1

/*----------------------------------------------------------------------------*/
/*! Timer overrun tolerance in the units of microseconds */
#define FTM_ATS_OVERRUN_TOLERANCE_US 100

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Schedule callback
 
  @details
  Based on the requested interval, this function schedules the execution of
  callback for futurte
 
  @param interval_us
  Interval in microseconds after which the callback function is to be
  executed.
 
  @param ats_timer
  Pointer to the data structure containing timer data
 
  @param time_update
  Flag indicating, if the strobe confuguration is to be done for new interval
  update.
*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
static void
ftm_timer_ats_schedule_cb
(
  uint32 interval_us,
  ftm_common_ats_timer_type *ats_timer,
  boolean time_update
)
{
  DalTimetickTime64Type irq_period = 0;
  DalTimetickTime64Type prev_irq_periodicity = 0;
  int64 irq_periodicity_diff = 0;

  /* Store the previous periodicity */
  prev_irq_periodicity = ats_timer->timetick_data.irq_periodicity;

  /* Store IRQ periodicity in microseconds */
  ats_timer->timetick_data.irq_periodicity_us = interval_us;

  /* Convert the interval passed into the function to hardware tick */
  irq_period = rf_time_microsecond_to_xotick( interval_us );

  /* Update the data structure with the period */
  ats_timer->timetick_data.irq_periodicity = irq_period;
  
  /* Calculate new strobe time. */
  if ( time_update == FALSE )
  {
    /* If IRQ periodicity difference is zero, this means that the periodicity
    has not changed. Thus new strobe time is the summation of current strobe 
    time and the periodicity */
    ats_timer->timetick_data.strobe_time += 
                 ats_timer->timetick_data.irq_periodicity;

  }
  else
  {
    /* Get the IRQ periodicity difference between the last period and
    current period */
    irq_periodicity_diff = 
       ( ats_timer->timetick_data.irq_periodicity - prev_irq_periodicity );

    /* If IRQ periodicity difference is non-zero, then this means that the
    periodicity between last period and this period has changed. Negative
    difference means that the periodicity is decreased compared to last
    period. Similarly, positive differebce means that the periodicity is
    increased compared to last period. Thus adjust the periodicity with
    thye delta. */
    ats_timer->timetick_data.strobe_time += irq_periodicity_diff;

  }
  
  timer_set_absolute( ats_timer->timer_data.pTimer,
                      ats_timer->timetick_data.strobe_time );


} /* ftm_timer_ats_schedule_cb */
#endif
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  IRQ callback function
 
  @details
  This is the callback function which will be registerd with ATS timer. This 
  function will be called when the ATS timer expires
 
  @param data
  Opaque data passed to the function
*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
static void
ftm_timer_ats_cb
(
  timer_cb_data_type  data  
)
{

  ftm_common_timer_type *timer_data; /* Timer Data */
  ftm_common_ats_timer_type *ats_timer; /* ATS timer */
  DalTimetickTime64Type o_stmr_curr_time = 0; /* Current time */
  DalTimetickTime64Type tolerance_tick = 0;
  DalTimetickTime64Type o_stmr_overrun_time = 0; /* ISR overrun time */
  DalTimetickTime64Type o_stmr_overrun_time_us = 0; /* ISR overrun time */
  static DalTimetickTime64Type o_stmr_prev_strobe = 0; /* Previous Strobe */
  boolean possible_overrun = FALSE; /* Flag to indicate possible overrun */

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();
  ats_timer = &(timer_data->ats_timer);

  if ( ats_timer->timer_acquired == FALSE )
  {
    FTM_MSG( FTM_ERROR, "ftm_timer_ats_cb: Called after Timer Uninstall" );
    return;
  } /* if ( ats_timer->timer_acquired == FALSE ) */

  /* Get the current time to be used for ISR overrun detection */
  o_stmr_curr_time = rf_time_get_timetick64_snapshot();

  /* Configure o_stmr IRQ strobe with new strobe point */
  ftm_timer_ats_schedule_cb( ats_timer->timetick_data.irq_periodicity_us, 
                             ats_timer, FALSE );

  /* Call the callback function registered by the client to perform action
  for the interrupt. */
  ats_timer->client_cb();

  /* Calculate Overrun time */
  if ( ats_timer->first_client_cb_run == FALSE )
  {
    if ( o_stmr_curr_time > o_stmr_prev_strobe )
    {
      o_stmr_overrun_time = o_stmr_curr_time - o_stmr_prev_strobe;
      possible_overrun = TRUE;
    }
    else
    {
      o_stmr_overrun_time = o_stmr_prev_strobe - o_stmr_curr_time;
      possible_overrun = FALSE;
    }
  }

  /* Send F3 message for debugging purpose */
  if ( ats_timer->enable_f3 == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "FTM ATS Timer : [%8d] - [%8d] = [%8d]",
               o_stmr_curr_time, o_stmr_prev_strobe, o_stmr_overrun_time);
  } /* if ( ats_timer->enable_f3 == TRUE ) */

  /* Convert the tolerance in microseconds to ticks */
  tolerance_tick = rf_time_microsecond_to_xotick(FTM_ATS_OVERRUN_TOLERANCE_US);

  if ( ( o_stmr_overrun_time >= tolerance_tick ) &&
       ( possible_overrun == TRUE ) )
  {

    ats_timer->timer_overrun = TRUE;

    /* Convert to microseconds */
    o_stmr_overrun_time_us = rf_time_xotick_to_microsecond(o_stmr_overrun_time);

    FTM_MSG_2( FTM_ERROR, "FTM ATS Timer : **** Overrun Detected "
               "[ %d = %d uS ] ****", o_stmr_overrun_time,
               o_stmr_overrun_time_us );
  } /* if ( o_stmr_overrun_time >= tolerance_tick ) */

  /* Store the current strobe time as "previous Strobe time", as this will
  be used in next ISR execution as previous strobe time to calculate ISR
  overrun. */
  o_stmr_prev_strobe = ats_timer->timetick_data.strobe_time;

  ats_timer->first_client_cb_run = FALSE;

} /* ftm_timer_ats_cb */
#endif
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable Periodic IRQ
 
  @details
  This function will enable periodic interrupt request with the periodicity
  passed to this function. This funciton will also register the ISR callback
  function to tramp.
 
  @param interval_us
  Interval in microseconds to be used as periodicity for IRQ
 
  @param cb_func
  Pointer to callback function to be registered for IRQ
 
  @return
  Status indicating if the IRQ was enabled successfully or not.
*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
static ftm_common_timer_status_type
ftm_timer_ats_configure
(
  uint32 interval_us,
  ftm_common_isr_cb cb_func
)
{
  ftm_common_timer_type *timer_data; /* Timer Data */
  ftm_common_ats_timer_type *ats_timer; /* ATS timer */

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();
  ats_timer = &(timer_data->ats_timer);

  /* Make sure that this is the first timer acquisition */
  if ( ats_timer->timer_acquired == TRUE )
  {
    FTM_MSG( FTM_ERROR, "ftm_timer_ats_configure: Timer already acquired" );
    return FTM_TIMER_FAILURE;
  } /* if ( ats_timer->timer_acquired == TRUE ) */

  /* Flag that Timer is acquired */
  ats_timer->timer_acquired = TRUE;

  /* Initialize Timer Data */
  ats_timer->timer_overrun = FALSE;
  ats_timer->enable_f3 = FALSE;
  ats_timer->first_client_cb_run = TRUE;

  /* Update the Data structure with the client call back function */
  ats_timer->client_cb = cb_func;

  /* Take the snapshot */
  ats_timer->timetick_data.reference_time = rf_time_get_timetick64_snapshot();

  /* Initialize strobe time with the reference time */
  ats_timer->timetick_data.strobe_time = 
                  ats_timer->timetick_data.reference_time;
  
  /* Define the timer */
  if ( !ats_timer->timer_data.bTimerDefined )
  {
    /* Set pointer */
    ats_timer->timer_data.pTimer = &(ats_timer->timer_data.sTimer);

    /* define the timer */
    timer_def_priority( ats_timer->timer_data.pTimer,
                        NULL,
                        NULL,
                        0,
                        ftm_timer_ats_cb,
                        0,
                        TIMER_PRIORITY_0);
 
    /* We do not want to redefine this timer */
    ats_timer->timer_data.bTimerDefined = 1;

  } /* if ( !timer_data->ats_timer.timer_data.bTimerDefined ) */

  /* Schedule the callback */
  ftm_timer_ats_schedule_cb( interval_us, ats_timer, FALSE );

  /* Send F3 message for debugging purpose */
  FTM_MSG_2( FTM_HIGH, "ftm_timer_ats_configure: [Status 1] Updated - "
             "StartTime 0x%x NextStrobe 0x%x", 
             ats_timer->timetick_data.reference_time, 
             ats_timer->timetick_data.strobe_time );

  return FTM_TIMER_INSTALL_DONE;

} /* ftm_timer_ats_configure */
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update Periodicity of IRQ
 
  @details
  This function will update the periodicity of interrupt request. Since the
  callback function is already registered, there is no need of
  re-registration
 
  @param interval_us
  Interval in microseconds to be used as periodicity for IRQ
 
  @return
  Status indicating if the IRQ was enabled successfully or not.
*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
static ftm_common_timer_status_type
ftm_timer_ats_update
(
  uint32 interval_us
)
{
  ftm_common_timer_status_type ret_val;
  ftm_common_timer_type *timer_data; /* Timer Data */
  ftm_common_ats_timer_type *ats_timer; /* ATS timer */

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();
  ats_timer = &(timer_data->ats_timer);

  /* Make sure timer is already acquired */
  if ( ats_timer->timer_acquired == FALSE )
  {
    FTM_MSG( FTM_ERROR, "ftm_timer_ats_update: Timer not acquired" );
    return FTM_TIMER_FAILURE;
  } /* if ( ats_timer->timer_acquired == TRUE ) */

  ftm_timer_ats_schedule_cb( interval_us, ats_timer, TRUE );

  /* Flag error, if ISR overrun was detected */
  if ( ats_timer->timer_overrun == TRUE )
  {
    ret_val = FTM_TIMER_ISR_OVERRUN_DETECTED;
  }
  else
  {
    ret_val = FTM_TIEMR_UPDATE_DONE;
  }

  /* Send F3 message for debugging purpose */
  if ( ats_timer->enable_f3 == TRUE )
  {
    FTM_MSG_2( FTM_HIGH, "ftm_timer_ats_update: [Status %d] Updated - "
               "NextStrobe 0x%x", ret_val, 
               ats_timer->timetick_data.strobe_time );
  } /* if ( ats_timer->enable_f3 == TRUE ) */

  return ret_val;

} /* ftm_timer_ats_update */
#endif /*FEATURE_GNSS_ONLY_NO_WWAN */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable Periodic IRQ
 
  @details
  This function will disable periodic interrupt request. This function will 
  de-register the ISR callback function to tramp.
*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
static ftm_common_timer_status_type
ftm_timer_ats_deconfigure
(
  void
)
{
  ftm_common_timer_status_type ret_val = FTM_TIMER_UNINSTALL_DONE;
  ftm_common_timer_type *timer_data; /* Timer Data */
  ftm_common_ats_timer_type *ats_timer; /* ATS timer */
  time_timetick_type rem_time;

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();
  ats_timer = &(timer_data->ats_timer);

  /* Make sure timer is already acquired */
  if ( ats_timer->timer_acquired == FALSE )
  {
    FTM_MSG( FTM_ERROR, "ftm_timer_ats_deconfigure: Timer not acquired" );
    return FTM_TIMER_FAILURE;
  } /* if ( ats_timer->timer_acquired == TRUE ) */

  /* Make sure that the time is acquired */
  ats_timer->timer_acquired = FALSE;

  /* Clear the timer */
  rem_time = timer_clr_64( ats_timer->timer_data.pTimer, T_USEC );

  /* Send F3 message for debugging purpose */
  FTM_MSG_2( FTM_HIGH, "ftm_timer_ats_deconfigure: [Status %d] Ceased - "
             "RemTime %d us", ret_val, rem_time );

  return ret_val;

} /* ftm_timer_ats_deconfigure */
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
/*! @} */

/*============================================================================*/
/*!
  @addtogroup FTM_TIMER_HARDWARE
 
  @brief
  This module contains interfaces for FTM to use hardware timer based timers
  @{
*/

/*============================================================================*/
/*!
  @name Hardware based timer

  @brief
  This section holds interfaces needed by FTM to accomplish Timer
  functionality. This is a precise timer.
  
  @image html ftm_common_timer_drift_solution.jpg
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Install Timer
 
  @param interval
  ISR Periodicity in microseconds. This function will update the Timer
  Match Value for next ISR. This will also register the callback function
  with TRAMP.

  @param cb_func
  Callback function which will be invoked every interval
*/
ftm_common_timer_status_type 
ftm_common_install_hw_timer
( 
  uint32 interval_us,
  ftm_common_isr_cb cb_func 
)
{
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  ftm_common_timer_status_type ret_val;
  ftm_common_timer_type *timer_data; /* Timer Data */

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();

  timer_data->use_ats_timer = TRUE;

    if ( timer_data->use_ats_timer == TRUE )
    {
      ret_val = ftm_timer_ats_configure( interval_us, cb_func );
    }
    else
    {
      ret_val = ftm_common_msm_configure_hw_timer( interval_us, cb_func );
    }

  return ret_val;
#else
  return FTM_TIMER_FAILURE;
#endif

} /* ftm_common_install_hw_timer */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Update Timer Match value
 
  @param interval
  New ISR Periodicity in microseconds. This function will update the Timer
  Match Value for next ISR. This does not require an update with TRAMP as
  the Callback function that was used during install will be used.
*/
ftm_common_timer_status_type 
ftm_common_update_hw_timer
( 
  uint32 interval_us
)
{
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  ftm_common_timer_status_type ret_val;
  ftm_common_timer_type *timer_data; /* Timer Data */

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();

  if ( timer_data->use_ats_timer == TRUE )
    {
    ret_val = ftm_timer_ats_update( interval_us );
    }
    else
    {
    ret_val = ftm_common_msm_update_hw_timer( interval_us );
  }

  return ret_val;
#else
  return FTM_TIMER_FAILURE;
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
} /* ftm_common_update_hw_timer */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Uninstall Timer

  @details
  This function will uninstall timer and un-register the ISR CB funciton with
  TRAMP.
*/
ftm_common_timer_status_type 
ftm_common_uninstall_hw_timer
(
  void
)
{
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  ftm_common_timer_status_type ret_val;
  ftm_common_timer_type *timer_data; /* Timer Data */

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();

  if ( timer_data->use_ats_timer == TRUE )
  {
    ret_val = ftm_timer_ats_deconfigure();
  }
  else
  {
    ret_val = ftm_common_msm_deconfigure_hw_timer();
  }

  return ret_val;

#else 
  return FTM_TIMER_FAILURE;
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
} /* ftm_common_uninstall_hw_timer */

/*! @} */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup FTM_TIMER_HARDWARE_TEST
 
  @brief
  This module contains interfaces for FTM to test FTN hardware timers
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Test call back function for ATS timer
 
  @details
  This function will be called by ATS timer during timer testing
 */
static void
ftm_timer_hw_test_cb
(
  void
)
{
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  ftm_common_timer_type *timer_data; /* Timer Data */
  ftm_common_timer_test_data_type *test_data;
  ftm_common_timer_status_type timer_status;
  static boolean delta_increment = TRUE;

  static uint16 new_interval_bkp;
  rf_time_type a_isr_time = 0;
  int16 isr_time_diff = 0;

  static uint64 a_cumm_time = 0;
  static uint64 e_cumm_time = 0;

  static rf_time_tick_type ref_isr_time_tick;

  int64 cumm_time_diff = 0;

  boolean int_var_reset = FALSE;

  #define FTM_HW_TIMER_TEST_INC_VAL 5000
  #define FTM_HW_TIMER_TEST_DEC_VAL 3000

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();

  /* Get Test data */
  test_data = &(timer_data->test_data);

  if ( test_data->segment_count > 0 )
  {
    a_isr_time = rf_time_get_elapsed( test_data->isr_time_tick, RF_USEC );
    a_cumm_time = rf_time_get_elapsed( ref_isr_time_tick, RF_USEC );
    e_cumm_time += test_data->interval;
  }

  test_data->segment_count++;

  if ( test_data->segment_count <=  test_data->num_of_segments )
  {
    if ( test_data->var_seg_len )
    {
      if ( delta_increment == TRUE )
      {
        test_data->interval += FTM_HW_TIMER_TEST_INC_VAL;
        delta_increment = FALSE;
      }
      else
      {
        test_data->interval -= FTM_HW_TIMER_TEST_DEC_VAL;
        delta_increment = TRUE;
      }
    }

    timer_status = ftm_common_update_hw_timer( test_data->interval );
  }
  else
  {
    timer_status = ftm_common_uninstall_hw_timer();
    int_var_reset = TRUE;
    FTM_MSG( FTM_HIGH, "FTM_TIMER_TEST - ****** UNINSTALL" );
  }

  
  test_data->isr_time_tick = rf_time_get_tick();
  
  if ( test_data->segment_count == 1 )
  {
    ref_isr_time_tick = test_data->isr_time_tick;
  }

  isr_time_diff = new_interval_bkp - a_isr_time;
  cumm_time_diff = e_cumm_time - a_cumm_time;
  
  FTM_MSG_8( FTM_HIGH, "FTM_TIMER_TEST - %d PROFILE #%5d == "
             "ISR [E %5d v A %5d] = [D %5d] ::: Cum [E %10d v A %10d] = "
             "[D %5d]",
             timer_status, (test_data->segment_count - 1),
             new_interval_bkp, a_isr_time, isr_time_diff,
             e_cumm_time, a_cumm_time, cumm_time_diff  );

  new_interval_bkp = test_data->interval;

  if ( int_var_reset == TRUE )
  {
    new_interval_bkp = 0;
    delta_increment = TRUE;
    a_cumm_time = 0;
    e_cumm_time = 0;
    memset( &ref_isr_time_tick, 0x0, sizeof(rf_time_tick_type) );
  }

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
} /* ftm_timer_hw_test_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to test ATS timer
 
  @details
  This function can be called to test the ATS timer execution
 
  @param interval_us
  Interval in microseconds used for timer testing
 
  @param num_of_segments
  Number of ISR segment
 
  @param var_seg_len
  Flag to indicate, if variable segment length is to be used
 
  @param timer_flag
  Flag indicating which timer to test
*/
void
ftm_timer_hw_test
(
  uint16 interval_us,
  uint16 num_of_segments,
  boolean var_seg_len,
  uint8 timer_flag
)
{
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  ftm_common_timer_type *timer_data; /* Timer Data */
  ftm_common_timer_test_data_type *test_data;
  ftm_common_timer_status_type timer_status;

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();

  if ( timer_flag == 0 )
  {
    timer_data->use_ats_timer = FALSE;
  }
  else
  {
    timer_data->use_ats_timer = TRUE;
  }

  /* Get Test data */
  test_data = &(timer_data->test_data);

  /* Update the data structure with the params */
  test_data->interval = interval_us;
  test_data->num_of_segments = num_of_segments;
  test_data->var_seg_len = var_seg_len;
  test_data->segment_count = 0;

  /* Install Timer */
  timer_status = ftm_common_install_hw_timer( interval_us, 
                                              ftm_timer_hw_test_cb );

  FTM_MSG_1( FTM_HIGH, "FTM_TIMER_TEST - ****** INSTALL %d", timer_status );
#endif /* FEATFEATURE_GNSS_ONLY_NO_WWAN */
} /* ftm_timer_hw_test */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup FTM_TIMER_SOFTWARE
 
  @brief
  This module contains interfaces for FTM to use software timer based timers
  @{
*/

/*============================================================================*/
/*!
  @name Timer-Services Timer

  @brief
  This section holds interfaces needed by FTM to accomplish Timer
  functionality using Timer services. This is non-precise timer
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Install Timer
 
  @param interval
  ISR Periodicity in millieconds. This function will update the Timer
  Match Value for next ISR. This will also register the callback function
  with TRAMP.

  @param cb_func
  Callback function which will be invoked every interval
*/
void 
ftm_common_install_sw_timer
( 
  uint32 interval,
  ftm_common_isr_cb cb_func 
)
{  
  /* local var to check if CB is registered and print error msg if
     CB was already installed */
  boolean installed_timer = FALSE;
  ftm_common_timer_type *timer_data; /* Timer Data */

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();

  /* need to change cb_struct in Critical Section to ensure data integrity */
  ftm_common_enter_critical_section();

  /* check if cb-struct is to be initialized */
  if (timer_data->timer_services.cb_struct == NULL) 
  {
    /* cb_struct was not initialized - so safe to init it */
    timer_data->timer_services.cb_struct = 
                                (timer_type*)ftm_malloc(sizeof(timer_type));
  
    /* Initialize and register a callback */
    timer_def2( timer_data->timer_services.cb_struct, NULL );
    timer_reg( timer_data->timer_services.cb_struct, (timer_t2_cb_type)cb_func,
               (timer_cb_data_type)NULL, interval, interval);

    installed_timer = TRUE;
  } /* if (timer_data->timer_services.cb_struct == NULL) */

  /* release critical section */
  ftm_common_leave_critical_section(); 

  /* need to throw err message if CB was already installed - used local flag 
     "installed_timer" to prevent printing message in the CS */ 
  if (installed_timer == FALSE)
  {
    FTM_MSG( FTM_ERROR, "ftm_common_install_sw_timer : Already Installed");
  }
  else
  {
    FTM_MSG( FTM_HIGH, "ftm_common_install_sw_timer: Installed" );
  }
  
} /* ftm_common_install_sw_timer */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Uninstall Timer

  @details
  This function will uninstall timer and un-register the ISR CB funciton with
  TRAMP.
*/
void 
ftm_common_uninstall_sw_timer
(
  void
)
{
  ftm_common_timer_type *timer_data; /* Timer Data */

  /* need to change cb_struct in Critical Section to ensure data integrity */
  ftm_common_enter_critical_section();

  /* Get Timer Data from FTM common data structure */
  timer_data = ftm_common_data_get_timer_ptr();

  /* If cb_struct is not null, then the callback was installed , and it is
     safe to uninstall it */
  if (timer_data->timer_services.cb_struct != NULL)
  {
    /* uninstall the callback */
    (void)timer_clr(timer_data->timer_services.cb_struct, T_NONE);

    /* release memory */
    ftm_free(timer_data->timer_services.cb_struct);

    /* re-init to NULL for future use */
    timer_data->timer_services.cb_struct = NULL;

    FTM_MSG( FTM_HIGH, "ftm_common_uninstall_sw_timer: Uninstalled" );

  } /* if (cb_struct != NULL)*/

  /* release CS */
  ftm_common_leave_critical_section();
  
} /* ftm_common_uninstall_sw_timer */


/*! @} */

/*! @} */

