#ifndef BOOT_LOGGER_TIMER_H
#define BOOT_LOGGER_TIMER_H

/*===========================================================================

                             boot logger timer 
                               Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for Boot Logger
  Timer functionality.Time unit in boot logger is microsecond(10^-6 second).

Copyright 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_logger_timer.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ 
  $Author: pwbldsvc $

when       who          what, where, why
--------   --------     ---------------------------------------------------------- 
08/11/10   dh           Initial port from 7x30 
============================================================================*/
#include "boot_comdef.h"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_init_timer

** ==========================================================================
*/
/*!
* 
* @brief
*  initialize the timer module's meta data
*  This function will also set the internal reference time to be 
*  the value in the old boot logger meta info structure passed by boot_log_init_data,
*  or 0 if this old meta info structure is not given.
*
* @param[in] init_data the boot logger's initialization data
*
* @retval
*  None
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_LOGGER_TIMER
  void boot_init_timer(boot_log_init_data *);
#else
  static void boot_init_timer(boot_log_init_data *init_data)
  {
  }
#endif


/*===========================================================================

**  Function :  boot_get_time

** ==========================================================================
*/
/*!
* 
* @brief
*  Returns a timestamp which is the current absolute time since bootup
*  subtracts the reference time.
*   
* @retval
*    Current time in microseconds. 
*    LOG_TIMESTAMP_OVERFLOW if timestamp overflows
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_LOGGER_TIMER
  uint32 boot_get_time(void);
#else
  static uint32 boot_get_time(void)
  {
    return 0;
  }
#endif


/*===========================================================================

**  Function :  boot_start_timer

** ==========================================================================
*/
/*!
* 
* @brief
* This function will start the boot_start_timer/boot_stop_timer sequence.
* It will record the time count at the point of this function's execution.
* If multiple starts are called, the timer will reset, throwing away the
* previous start's timestamp.
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_LOGGER_TIMER
  void boot_start_timer(void);
#else
  static inline void boot_start_timer(void)
  {
  }
#endif


/*===========================================================================

**  Function :  boot_stop_timer

** ==========================================================================
*/
/*!
* 
* @brief
* This function will return the delta time between the time recorded by previous 
* boot_start_timer call and the time this function is executed.
*   
* @retval
*   Delta Time between boot_start_timer and boot_stop_timer in microseconds.
*   If stopwatch had not been started then it will return 0.
*   LOG_TIMESTAMP_OVERFLOW if delta time overflows.
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_LOGGER_TIMER
  uint32 boot_stop_timer(void);
#else
  static inline uint32 boot_stop_timer(void)
  {
    return 0;
  }
#endif


/*===========================================================================

**  Function :  boot_get_delta_time

** ==========================================================================
*/
/*!
* 
* @brief
* This function will return the delta time between a given timestamp and now 
* in microseconds.
*
* @param[in] t the previous timestamp
*
* @retval
*   Delta time between the given timestamp and current time..
*   LOG_TIMESTAMP_OVERFLOW if delta time overflows.
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_LOGGER_TIMER
  uint32 boot_get_delta_time(uint32);
#else
  static inline uint32 boot_get_delta_time(uint32 t)
  {
    return 0;
  }
#endif

#endif  /* BOOT_LOGGER_TIMER_H */
