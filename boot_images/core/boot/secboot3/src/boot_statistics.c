/*===========================================================================

                         Boot Statistics Interface File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for boot statistics

Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who         what, where, why
--------   ---         --------------------------------------------------
10/28/14   jz          Fixed the incorrect flash_total_time_us
07/22/14   jz          Fixed divide-by-zero exception in flashless boot case
06/02/14   ck          Initial version  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include <stdio.h>
#include "boot_statistics.h"
#include "boot_error_if.h"
#include "boot_logger.h"
#include "boot_target.h"
#include "boot_msm.h"


static uint32 flash_timer_start_timetick = 0;
static uint32 flash_total_time_us = 0;
static uint32 flash_total_bytes_read = 0;
static uint32 flash_byte_counter = 0;


/*=========================================================================
                            
                       FUNCTION DEFINITIONS

=========================================================================*/

/*===========================================================================

**  Function :  boot_statistics_get_time_count

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
static uint32 boot_statistics_get_time_count()
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


/*===========================================================================

**  Function : boot_statistics_flash_read_start

** ==========================================================================
*/
/*!
* 
* @brief  
*   Called before reading from flash.  This gets the current timer value and
*   stores it for use when boot_statistics_flash_read_stop is called.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_statistics_flash_read_start(void)
{
  flash_timer_start_timetick = boot_statistics_get_time_count();
}


/*===========================================================================

**  Function : boot_statistics_flash_read_stop

** ==========================================================================
*/
/*!
* 
* @brief  
*   Called after reading from flash.  This gets the current timer value
*   calculates the delta from the start value, and updates the flash statistics.
*
* @par Input
*   bytes_read    Number of bytes read from flash
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_statistics_flash_read_stop(uint32 bytes_read)
{
  uint32 flash_timer_stop_timetick = boot_statistics_get_time_count();

  /* Update the flash statistics, total time, bytes read and byte counter. */
  flash_total_time_us +=
    CALCULATE_TIMESTAMP(flash_timer_stop_timetick - flash_timer_start_timetick);

  flash_total_bytes_read += bytes_read;
  flash_byte_counter += bytes_read;
}


/*===========================================================================

**  Function : boot_statistics_log_flash

** ==========================================================================
*/
/*!
* 
* @brief  
*   Reports the flash throughput (speed, total bytes read, total time)
*   via the boot logger.
*
* @par Input
*   None
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_statistics_log_flash(void)
{
  uint32 flash_kb_per_second = 0;
  char temp_buffer[BOOT_LOG_TEMP_BUFFER_SIZE];

  /* Calculate throughput. */
  if (flash_total_time_us && flash_total_bytes_read)
  {
    flash_kb_per_second = flash_total_bytes_read / flash_total_time_us; // bytes per us
    flash_kb_per_second *= 1000000; // us to Seconds
    flash_kb_per_second /= 1000; // Bytes to KB
  }
  
  /* Generate the flash statistic boot log message */
  snprintf(temp_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "Throughput, %d KB/s  (%d Bytes,  %d us)",
           flash_kb_per_second,
           flash_total_bytes_read,
           flash_total_time_us);

  /* Write results to boot log */
  boot_log_message_raw(temp_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}


/*===========================================================================

**  Function : boot_statistics_reset_flash_byte_counter

** ==========================================================================
*/
/*!
* 
* @brief  
*   A counter seperate from the total flash bytes counter exists so functions
*   can access the number of flash bytes read during a particular period.  This
*   function resets that counter.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_statistics_reset_flash_byte_counter(void)
{
  flash_byte_counter = 0;
}


/*===========================================================================

**  Function : boot_statistics_get_flash_byte_counter

** ==========================================================================
*/
/*!
* 
* @brief  
*   A counter seperate from the total flash bytes counter exists so functions
*   can access the number of flash bytes read during a particular period.  This
*   function returns the value in that counter.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
uint32 boot_statistics_get_flash_byte_counter(void)
{
  return flash_byte_counter;
}
