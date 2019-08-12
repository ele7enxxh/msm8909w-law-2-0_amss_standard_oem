/*=============================================================================

                             boot logger timer  

GENERAL DESCRIPTION
  This file contains the implementation of boot logger timer functions .
  Time unit in boot logger is microsecond(10^-6 second).
  
Copyright 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_logger_timer.c#1 $
  $DateTime: 2015/09/01 00:30:35 $ 
  $Author: pwbldsvc $
  
when       who        what, where, why
--------   ---        ---------------------------------------------------------
09/27/10   dh         Add meta info structure to boot logger
08/11/10   dh           Initial port from 7x30 
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_msm.h"
#include "boot_target.h"
#include "boot_logger.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

**  Function :  boot_get_time_count

** ==========================================================================
*/
/*!
* 
* @brief
*  Returns the time count since bootup. Internal use only.
*
* @retval
*   a 32 bits counter value as the current value in TIMETICK_CLK register.
* 
* @par Side Effects
*   None
* 
*/
static uint32 boot_get_time_count()
{
  uint32 curr_count;
  uint32 last_count;

  /*Grab current time count*/
  curr_count = HWIO_IN(TIMETICK_CLK);

  /*Keep grabbing the time until a stable count is given*/
  do 
  {
    last_count = curr_count;
    curr_count = HWIO_IN(TIMETICK_CLK);
  } while (curr_count != last_count);
  
  return curr_count;
}
/* boot_get_time_count*/


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
void boot_init_timer(boot_log_init_data *init_data)
{
  struct boot_log_meta_info *log_meta_info_ptr = boot_log_get_meta_info();
  if(log_meta_info_ptr != NULL)
  {
    /*Set stopwatch to unlock so start_timer can be used*/
    log_meta_info_ptr->stopwatch_locked = FALSE;    
    /*Set start time to 0*/
    log_meta_info_ptr->start_time = 0;
    
    /*Set reference time to the old value if the old meta info exists,otherwise to 0*/
    if(init_data->old_meta_info_start != NULL)
    {
      log_meta_info_ptr->ref_time = ((struct boot_log_meta_info *)
                                    (init_data->old_meta_info_start))->ref_time;
    }
    else
    {
      log_meta_info_ptr->ref_time = 0;
    }
  }
}


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
uint32 boot_get_time()
{
  struct boot_log_meta_info *log_meta_info_ptr = boot_log_get_meta_info();
  uint32 curr_time_count = boot_get_time_count();
  
  /* check if the count overflows*/
  if(curr_time_count < LOG_MAX_COUNT_VAL)
  {
    /*Convert current time count into microseconds*/
    curr_time_count = CALCULATE_TIMESTAMP(curr_time_count);
    if(log_meta_info_ptr != NULL)
    {
       curr_time_count -= log_meta_info_ptr->ref_time;
    }
  }
  else
  {
    curr_time_count = LOG_TIMESTAMP_OVERFLOW;
  }
  
  return curr_time_count;
}
/* boot_get_time*/


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
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_start_timer()
{
  struct boot_log_meta_info *log_meta_info_ptr = boot_log_get_meta_info();
  if(log_meta_info_ptr != NULL)
  {
    log_meta_info_ptr->start_time = boot_get_time_count();
    log_meta_info_ptr->stopwatch_locked = TRUE;
  }
}
/*boot_start_timer*/


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
uint32 boot_stop_timer()
{
  uint32 end_time = 0;
  uint32 delta = 0;
  struct boot_log_meta_info *log_meta_info_ptr = boot_log_get_meta_info();
  
  if(log_meta_info_ptr != NULL)
  {
    /*Timer has been started, calculate elapsed time*/
    if(log_meta_info_ptr->stopwatch_locked == TRUE) 
    {
      log_meta_info_ptr->stopwatch_locked = FALSE;
      end_time = boot_get_time_count();
    
      /*check if both time points do not overflow and end time is bigger than 
        start time*/
      if(log_meta_info_ptr->start_time < LOG_MAX_COUNT_VAL &&
        end_time < LOG_MAX_COUNT_VAL && 
        end_time >= log_meta_info_ptr->start_time)
      {
        delta = CALCULATE_TIMESTAMP(end_time - log_meta_info_ptr->start_time);
      }
      else
      {
        delta = LOG_TIMESTAMP_OVERFLOW;
      }
    }
  }
  return delta;
}
/*boot_stop_timer*/


/*===========================================================================

**  Function :  boot_get_delta_time

** ==========================================================================
*/
/*!
* 
* @brief
* This function will return the delta time between old_time and now 
* in microseconds.
*
* @param[in] old_time the previous timestamp.
*
* @retval
*   Delta time between old_time and current time.
*   LOG_TIMESTAMP_OVERFLOW if delta time overflows.
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_get_delta_time(uint32 old_time)
{
  uint32 delta = LOG_TIMESTAMP_OVERFLOW;
  uint32 curr_time = 0;
  
  if(old_time != LOG_TIMESTAMP_OVERFLOW)
  {
    curr_time = boot_get_time();
    if(curr_time != LOG_TIMESTAMP_OVERFLOW && curr_time >= old_time)
    {
      delta = curr_time - old_time;
    }
  }
  
  return delta;
}
/*boot_get_delta_time*/
