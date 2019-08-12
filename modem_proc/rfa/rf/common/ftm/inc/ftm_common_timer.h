#ifndef FTM_COMMON_TIMER_H
#define FTM_COMMON_TIMER_H

/*!
  @file
  ftm_common_timer.h
 
  @brief
  Common FTM Timer.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_timer.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/01/13   aro     Removed DAL detach
03/01/13   aro     Added command to perform HW timer test
03/01/13   aro     Added on-target test case for timer
02/18/13   aro     Added initial implementation of ATS timer
03/02/12   aro     Updated Timer interfaces to return enumeration as timer
                   action status
03/02/12   aro     Added variables to be used for ISR overrun detection
08/25/11   aro     Renamed functions to better match the functionality
08/18/11   aro     Added execution status return all timer function
08/18/11   aro     Added timer services based install and uninstall functions
08/18/11   aro     Partitioned Precise and TimerServices based timer data into
                   separate data structure
08/18/11   aro     Added structure variables to support programming then strobe
                   time based on the offset from one reference time. This is
                   required to avoid timing drift issue
08/17/11   aro     Renamed callback function type
08/17/11   aro     Added data structrues to hold timer hardware specific data
08/12/11   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "timer.h"
#include "DDITimetick.h"
#include "rfcommon_time_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Enumeration for Timer configuration status */
typedef enum
{

  FTM_TIMER_FAILURE = 0,
  /*!< Enumeration indicating that Timer installation/update/uninstallation 
  has failed to completion */

  FTM_TIMER_INSTALL_DONE = 1,
  /*!< Enumeration indicating that Timer installation was done successfully */

  FTM_TIMER_UNINSTALL_DONE = FTM_TIMER_INSTALL_DONE,
  /*!< Enumeration indicating that Timer uninstallation was done successfully */

  FTM_TIEMR_UPDATE_DONE = FTM_TIMER_INSTALL_DONE,
  /*!< Enumeration indicating that Timer update was done successfully */

  FTM_TIMER_ISR_OVERRUN_DETECTED = 2,
  /*!< Enumeration indicating that ISR overrun was detected atleast once during 
  this instance of timer. However all other timer actions are completed 
  successfully */ 

} ftm_common_timer_status_type;

/*----------------------------------------------------------------------------*/
/*! ISR callback function to be fired when interrupt is generated */
typedef void (*ftm_common_isr_cb) (void);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Data structure to hold data pertaining to timer hardware
*/
typedef struct
{
  uint32 reference_time;
  /*!< Reference time at which timer has been triggerd. Setting the strobe 
  point will be done based on this reference time to avoid timer drift issue. 
  The units will be number of ticks. */ 

  uint32 strobe_time;
  /*!< Variable to store the current strobe time. This will store the 
  accumalated strobe time. The time will be wrapped around when it reached 
  the limit */ 

  uint32 irq_periodicity;
  /*!< Current periodicity at which IRQ strobes are fired. The units is
  number of ticks */

  uint32 irq_periodicity_us;
  /*!< Current periodicity in micorseconds at which IRQ strobes are fired. */

} ftm_common_timer_hw_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data for precise timer */
typedef struct
{
  boolean timer_acquired;
  /*!< 
  @brief
  Flag to indicate if the timer is acquired or not 
   
  @details 
  This flag will be set to high when Timer is installed by FTM. Until the timer 
  is uninstalled, the falg will be set to high. This flag will be used 
  to prevent multiple installation of the same timer. 
  */

  ftm_common_timer_hw_data_type timer_hw_data;
  /*!< Data structure to hold data pertaining to timer hardware */

  ftm_common_isr_cb client_cb;
  /*!< Callback function from the client to be fired after Interrupt is 
  generated */

  boolean wraparound_occured;
  /*!< Flag indicating, if the warparound in timer counter would occur when 
  the next scheduled strobe is fired */

  boolean timer_overrun;
  /*!< Flag to indicate, if Timer overrun is detected in atleast once within 
  the this instance of timer installation */

  boolean first_client_cb_run;
  /*!< Flag indicating if the client_cb is run for the first time */

  boolean enable_f3;
  /*!< Flag to indicate if the debug F3 is to be enabled or not */

} ftm_common_precise_timer_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data for TimerServices based non-precise timer */
typedef struct
{

  timer_type  *cb_struct;
  /*!< Pointer to store to hold timer service data */

} ftm_common_timer_services_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data timetick data for ATS timer */
typedef struct
{
  DalDeviceHandle* phTimetickHandle;
  /*!< Handle to be used by ATS timer to perform timetick operation */

  DalTimetickTime64Type reference_time;
  /*!< Reference time at which timer has been triggerd. Setting the strobe 
  point will be done based on this reference time to avoid timer drift issue. 
  The units will be number of ticks. */

  DalTimetickTime64Type strobe_time;
  /*!< Variable to store the current strobe time. This will store the 
  accumalated strobe time. The time will be wrapped around when it reached 
  the limit */ 

  DalTimetickTime64Type irq_periodicity;
  /*!< Current periodicity at which IRQ strobes are fired. The units is
  number of ticks */

  uint32 irq_periodicity_us;
  /*!< Current periodicity in micorseconds at which IRQ strobes are fired. */
   
} ftm_common_ats_timetick_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data Timer data for ATS timer */
typedef struct
{

  uint32 bTimerDefined;
  /*!< Flag to indicate if the time is defined or not */

  timer_type sTimer;
  /*!< Structure to hold timer info */

  timer_ptr_type pTimer;
  /*!< Pointer to timer data */
  
} ftm_common_ats_data_type;


/*----------------------------------------------------------------------------*/
/*! Structure to hold data for timer based on ATS */
typedef struct
{
  boolean timer_acquired;
  /*!< 
  @brief
  Flag to indicate if the timer is acquired or not 
   
  @details 
  This flag will be set to high when Timer is installed by FTM. Until the timer 
  is uninstalled, the falg will be set to high. This flag will be used 
  to prevent multiple installation of the same timer. 
  */

  ftm_common_isr_cb client_cb;
  /*!< Callback function from the client to be fired after Interrupt is 
  generated */

  ftm_common_ats_timetick_data_type timetick_data;
  /*!< Structure to hold data timetick data for ATS timer */

  ftm_common_ats_data_type timer_data;
  /*!< Structure to hold data Timer data for ATS timer */

  boolean timer_overrun;
  /*!< Flag to indicate, if Timer overrun is detected in atleast once within 
  the this instance of timer installation */

  boolean first_client_cb_run;
  /*!< Flag indicating if the client_cb is run for the first time */

  boolean enable_f3;
  /*!< Flag to indicate if the debug F3 is to be enabled or not */

} ftm_common_ats_timer_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold the Timer Test Data
*/
typedef struct
{
  uint16 interval;
  /*!< Interval for the first segment in microseconds to be used during 
  Timer Test */

  uint16 num_of_segments;
  /*!< Number of segments to be used during testing */

  boolean var_seg_len;
  /*!< Flagh indicating variable segment length */

  uint16 segment_count;
  /*!< Counter to count the current segment */

  rf_time_tick_type isr_time_tick;
  /*!< Variable to store time tick for ISR profiling */

} ftm_common_timer_test_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Structure containing the timer information
*/
typedef struct
{
  ftm_common_timer_services_type timer_services;
  /*!< Structure to hold data for TimerServices based non-precise timer */

  ftm_common_precise_timer_type precise_timer;
  /*!< Structure to hold data for precise timer */

  ftm_common_ats_timer_type ats_timer;
  /*!< Structure to hold data for timer based on ATS */

  ftm_common_timer_test_data_type test_data;
  /*!< Structure to hold the Timer Test Data */

  boolean use_ats_timer;
  /*!< Flag to indicate whether to use ATS timer or precise timer */

} ftm_common_timer_type;

/*----------------------------------------------------------------------------*/
ftm_common_timer_status_type 
ftm_common_install_hw_timer
( 
  uint32 interval_us,
  ftm_common_isr_cb cb_func
);

/*----------------------------------------------------------------------------*/
ftm_common_timer_status_type 
ftm_common_update_hw_timer
( 
  uint32 interval_us
);

/*----------------------------------------------------------------------------*/
ftm_common_timer_status_type 
ftm_common_uninstall_hw_timer
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_timer_hw_test
(
  uint16 interval_us,
  uint16 num_of_segments,
  boolean var_seg_len,
  uint8 timer_flag
);

/*----------------------------------------------------------------------------*/
void 
ftm_common_install_sw_timer
( 
  uint32 interval,
  ftm_common_isr_cb cb_func 
);

/*----------------------------------------------------------------------------*/
void 
ftm_common_uninstall_sw_timer
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_COMMON_TIMER_H */


