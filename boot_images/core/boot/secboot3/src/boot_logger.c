/*=============================================================================

                                boot logger 

GENERAL DESCRIPTION
  This file contains the function implementation for logging messages during boot.
  Time unit in boot logger is microsecond(10^-6 second).

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_logger.c#1 $
  $DateTime: 2015/09/01 00:30:35 $ 
  $Author: pwbldsvc $
  
when       who        what, where, why
--------   ---        ---------------------------------------------------------
11/26/14   jz         Removed call to boot_log_apps_frequency (clock is not initialized yet)
06/10/14   ck         Added boot_log_apps_frequency
06/10/14   ck         Added boot_log_image_version
06/10/14   ck         Added boot_log_ddr_frequency
06/10/14   ck         Added boot_log_boot_config_register
06/02/14   ck         Added boot_log_delta_time_optional_data
06/02/14   ck         Added boot_log_stop_timer_optional_data
06/02/14   ck         Added boot_log_message_optional_data
06/02/14   ck         Added boot_log_message_raw
05/28/14   ck         Added boot_log_insert_pbl_milestones
03/14/14   ck         Fixed missing uart logging
08/23/13   lm         Added UART Logger support
12/10/12   plc        Fix Klockwork Errors
08/30/12   dh         Change boot_log_copy to boot_log_relocate
05/10/12   dh         Change boot_log_set_ref_time to take an argument as reference time
06/21/11   kpa        Add boot_log_forever_loop_check api
11/10/10   dh         Add boot_log_copy api
09/27/10   dh         Add meta info structure to boot logger
08/11/10   dh         Initial port from 7x30 
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <stdio.h>
#include BOOT_PBL_H
#include "boot_logger.h"
#include "boot_logger_ram.h"
#include "boot_logger_jtag.h"
#include "boot_logger_timer.h"
#include "boot_logger_uart.h"
#include "boot_extern_clk_interface.h"
#include "image_version.h"
#include "boot_target.h"
#include "boot_msm.h"
#include "msmhwioreg.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/**
* Pointer to the meta info of current boot logger
*/
static struct boot_log_meta_info *log_meta_info_ptr;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_log_set_meta_info

** ==========================================================================
*/
/*!
* 
* @brief
*  Boot Logger uses a global pointer to store the address of meta info.
*  In the circumstances of any code initializes RW section of the load image 
*  after the boot logger is initialized, this function should be called to 
*  restore the meta info address that is kept in RW section of boot logger module.
*  By doing this we can log the time takes to initilize RW section of the image.
*
* @param[in] addr Address of boot logger's buffer
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
void boot_log_set_meta_info(void *addr)
{
  log_meta_info_ptr = (struct boot_log_meta_info *)addr;
}
/* boot_log_set_meta_info */


/*===========================================================================

**  Function :  boot_log_get_meta_info

** ==========================================================================
*/
/*!
* 
* @brief
*  This function returns a pointer to the meta info structure used by this logger.
*
* @par Dependencies
*   None
*   
* @retval
*   a pointer to the meta info structure used by this logger.
* 
* @par Side Effects
*   None
* 
*/
struct boot_log_meta_info* boot_log_get_meta_info()
{
  return log_meta_info_ptr;
}


/*===========================================================================

**  Function :  boot_log_init

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will set the address of logging buffer and initialize it.
*  This function will set the internal reference time to be 
*  the value of the old boot logger meta info structure,
*  which is passed through boot_log_init_data.
*  It will be set to 0 if this old meta info structure pointer is set to NULL 
*  in boot_log_init_data.
*  This function must be called before any other boot logger function is used.
*
* @param[in] init_data  Initialization data passed to boot logger
*
* @par Dependencies
*   boot_logger_ram
*   boot_logger_timer
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_init(boot_log_init_data *init_data)
{
  /*we must first set meta info becasue boot_log_init_ram and
   * boot_log_init_timer will use the meta info structure*/
  boot_log_set_meta_info(init_data->meta_info_start);
  boot_log_init_ram(init_data);
  boot_init_timer(init_data);
  boot_log_init_uart();

  /* Write image version values out to the log. */
  boot_log_image_version();

  /* Write the Boot Config register out to the log. */
  boot_log_boot_config_register();
}
/* boot_log_init */


/*===========================================================================

**  Function :  boot_log_set_ref_time

** ==========================================================================
*/
/*!
* 
* @brief
* This function will set the  timestamp(unit is microsecond) passed in the argument 
* as a reference time to be used by all logging functions.
* the reference time can be set at the beginning in order to get an accurate result.
*
* @param[in] ref_time  reference time that all logging function will use. Unit should
*                      be microsecond
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
void boot_log_set_ref_time(uint32 ref_time)
{
  if(log_meta_info_ptr != NULL)
  {
    log_meta_info_ptr->ref_time = ref_time;
  }
}

/*===========================================================================

**  Function :  boot_log_message

** ==========================================================================
*/
/*!
* 
* @brief
*  Logs the given message using various logging methods.It will prefix a timestamp
*  to the message string. The timestamp is calculated as the current absolute time 
*  subtracts the reference time.
*
* @param[in] message           log message
*
* @par Dependencies
*   boot_logger_ram
*   boot_logger_timer  
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_message(char * message)
{
  /* Call boot_log_message_optional_data so two functions do not have
     redundant logic. */
  boot_log_message_optional_data(message,
                                 NULL);
}
/* boot_log_message */


/*===========================================================================

**  Function :  boot_log_message_optional_data

** ==========================================================================
*/
/*!
* 
* @brief
*  Logs the given message using various logging methods.It will prefix a timestamp
*  to the message string. The timestamp is calculated as the current absolute time 
*  subtracts the reference time.
*
* @param[in] message           log message
*
* @param[in] optional_data     Optional data that can be added to entry
*
* @par Dependencies
*   boot_logger_ram
*   boot_logger_timer  
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_message_optional_data(char * message,
                                    char * optional_data)
{
  uint32 current_timestamp = boot_get_time();

  /*Logs message with time stamp in ram, must be initialized first.*/
  boot_log_message_ram(message,
                       current_timestamp,
                       LOG_MSG_TYPE_BOOT,
                       optional_data);

  /* Transmit the message with time stamp */
  boot_log_message_uart(message,
                        current_timestamp,
                        LOG_MSG_TYPE_BOOT,
                        optional_data);
}
/* boot_log_message_optional_data */


/*===========================================================================

**  Function :  boot_log_message_raw

** ==========================================================================
*/
/*!
* 
* @brief
*  Logs the given message.  Unlike boot_log_message, all data points must be
*  provided.
*
* @param[in] message  log message
*
* @param[in] timestamp  timestamp to log
*   Value of BOOT_DND_TIMESTAMP indicates timestamp will not be printed
*
* @param[in] log_type  type of the log
*
* @param[in] optional_data  Optional data that can be added to entry
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
void boot_log_message_raw(char * message,
                          uint32 timestamp,
                          char log_type,
                          char * optional_data)
{
  /*Logs message with time stamp in ram, must be initialized first.*/
  boot_log_message_ram(message,
                       timestamp,
                       log_type,
                       optional_data);

  /* Transmit the message with time stamp */
  boot_log_message_uart(message,
                        timestamp,
                        log_type,
                        optional_data);
}


/*===========================================================================

**  Function :  boot_log_start_timer

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will mark the beginning of the timed section and store the 
*  current timestamp interally.
*
* @par Dependencies
*   boot_logger_timer
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_start_timer()
{
  boot_start_timer();
}
/* boot_log_start_timer */


/*===========================================================================

**  Function :  boot_log_stop_timer

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will subtract the current timestamp with the previus stored 
*  timestamp by boot_log_start_timer(if any) to get the delta time value and 
*  then prefix the delta time to the log message.
*
* @param[in] message           log message
*
* @param[in] optional_data     Optional data that can be added to entry
*
* @par Dependencies
*  boot_logger_timer
*  boot_logger_ram
*   
* @retval
*   delta time
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_stop_timer(char * message)
{
  /* Call boot_log_stop_timer_optional_data so two functions do not have
     redundant logic. */

  return boot_log_stop_timer_optional_data(message,
                                           NULL);
}
/* boot_log_stop_timer */
 

/*===========================================================================

**  Function :  boot_log_stop_timer_optional_data

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will subtract the current timestamp with the previus stored 
*  timestamp by boot_log_start_timer(if any) to get the delta time value and 
*  then prefix the delta time to the log message.
*
* @param[in] message           log message
*
* @param[in] optional_data     Optional data that can be added to entry
*
* @par Dependencies
*  boot_logger_timer
*  boot_logger_ram
*   
* @retval
*   delta time
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_stop_timer_optional_data(char * message,
                                         char * optional_data)
{
  /*Logs message and time into ram.*/
  uint32 delta_time = boot_stop_timer();

  /* Logs message with time stamp in ram. */
  boot_log_message_ram(message,
                       delta_time,
                       LOG_MSG_TYPE_DELTA,
                       optional_data);

  /* Transmit the message with time stamp */
  boot_log_message_uart(message,
                        delta_time,
                        LOG_MSG_TYPE_DELTA,
                        optional_data);

  return delta_time;
}
/* boot_log_stop_timer_optional_data */


/*===========================================================================

**  Function :  boot_log_dump

** ==========================================================================
*/
/*!
* 
* @brief
* This function will dump the logged messages to various outputs.  Currently, 
* it only supports JTAG.  It may be expanded to support other methods of displaying,
* such as UART, etc.
*
* @param[in] init_data  boot logger intialization data which contains boot
*                       logger's buffer address
*
* @par Dependencies
*  boot_logger_jtag
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_dump(boot_log_init_data *init_data)
{
  /*Dumps RAM buffer to JTAG terminal*/
  boot_log_jtag_dump_buffer(init_data->log_buffer_start, init_data->log_buffer_size);
}
/*boog_log_dump*/


 
/*===========================================================================

**  Function :  boot_log_forever_loop_check

** ==========================================================================
*/
/*!
* 
* @brief
* This function will loop infinitely to enable ram dump extraction from memory. 
* it checks a memory location for magic number value. If desired flag value found
* it loops infinitely so as to enable running Only till end of bootloaders.
* The magic number being checked by the api is set from a T32 cmm script that
* collects bootup times [boot_debug.cmm]
*
* @param[in]  loop_flag_value  Value from imem memory location to be checked.                                
*
* @par Dependencies
*  None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_forever_loop_check( uint32 loop_flag_value)
{

  /* Disable the UART logger to save power */
  boot_log_deinit_uart();
   
  /* if magic number set to run, only till end of bootloaders
   to collect boot-time, then loop forever */
  if( loop_flag_value == BOOT_LOGGER_LOOP_MAGIC_NUM )
  {
    INFINITE_LOOP();
  }
}
/*boot_log_forever_loop_check*/



/*===========================================================================

**  Function :  boot_log_get_time

** ==========================================================================
*/
/*!
* 
* @brief
* This will return the current timestamp which is the current absolute time 
*  subtracts the reference time. 
*
* @par Dependencies
*  boot_logger_timer
*   
* @retval
*   current timestamp, LOG_TIMESTAMP_OVERFLOW if overflow happens
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_get_time()
{
  return boot_get_time();
}
/*boot_log_get_time*/


/*===========================================================================

**  Function :  boot_log_delta_time

** ==========================================================================
*/
/*!
* 
* @brief
* This will log the delta time, the time between the old_time that was given
* and the time this function is executed.  It will also return this delta time. 
*
* @param[in] old_time          previous time reference point 
*
* @param[in] message           log message
*
* @par Dependencies
*  boot_logger_timer
*   
* @retval
*   delta time between the old_time that was given
*   and the time this function is executed.
*   LOG_TIMESTAMP_OVERFLOW if overflow happens
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_delta_time(uint32 old_time, 
                           char * message)
{
  /* Call boot_log_delta_time_optional_data so there isn't redundant logic. */
  return boot_log_delta_time_optional_data(old_time,
                                           message,
                                           NULL);
}
/*boot_log_delta_time*/


/*===========================================================================

**  Function :  boot_log_delta_time_optional_data

** ==========================================================================
*/
/*!
* 
* @brief
* This will log the delta time, the time between the old_time that was given
* and the time this function is executed.  It will also return this delta time. 
*
* @param[in] old_time          previous time reference point 
*
* @param[in] message           log message
*
* @param[in] optional_data     Optional data that can be added to entry
*
* @par Dependencies
*  boot_logger_timer
*   
* @retval
*   delta time between the old_time that was given
*   and the time this function is executed.
*   LOG_TIMESTAMP_OVERFLOW if overflow happens
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_delta_time_optional_data(uint32 old_time, 
                                         char * message,
                                         char * optional_data)
{
  uint32 delta = boot_get_delta_time(old_time);

  boot_log_message_ram(message,
                       delta,
                       LOG_MSG_TYPE_DELTA,
                       optional_data);

  boot_log_message_uart(message,
                        delta,
                        LOG_MSG_TYPE_DELTA,
                        optional_data);

  return delta;
}
/* boot_log_delta_time_optional_data */


/*===========================================================================

**  Function :  boot_log_relocate

** ==========================================================================
*/
/*!
* 
* @brief
* Copy the log and meta info in one memory location to some other 
* memory location and continue to save logs at that location.
*
* @param[in] dst_meta_info_addr  destination address to save meta info
*
* @param[in] dst_log_buf_addr  destination address to save message log
*
* @param[in] dst_log_buf_size  size of the destination message log buf size
*
* @param[in] src_meta_info_addr  source address to read meta info
*
* @param[in] src_log_buf_addr  source address to read message log
*
* @param[in] src_log_buf_size  size of the source message log buf size
*
* @par Dependencies
*  boot_logger_ram
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_relocate
(
  void *dst_meta_info_addr,
  void *dst_log_buf_addr,
  uint32 dst_log_buf_size,  
  void *src_meta_info_addr,
  void *src_log_buf_addr,
  uint32 src_log_buf_size
)
{
  
  boot_log_copy_ram(dst_meta_info_addr,
                    dst_log_buf_addr,
                    dst_log_buf_size,
                    src_meta_info_addr,
                    src_log_buf_addr,
                    src_log_buf_size);
                    
  boot_log_set_meta_info(dst_meta_info_addr);
}
/*boot_log_copy*/


/*===========================================================================

**  Function : boot_log_boot_config_register

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the boot config register value.
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
void boot_log_boot_config_register(void)
{
  uint32 boot_config_register_value = 0;
  char temp_buffer[BOOT_LOG_TEMP_BUFFER_SIZE];


   /* Get the boot config register value and put it into boot log string. */
  boot_config_register_value = HWIO_BOOT_CONFIG_IN;
  snprintf(temp_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "Boot Config, 0x%08x",
           boot_config_register_value);

  /* Add entry to beginning of log for Boot Config register */
  boot_log_message_raw(temp_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}


/*===========================================================================

**  Function : boot_log_ddr_frequency

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the ddr frequency.
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
void boot_log_ddr_frequency(void)
{
  uint32 ddr_frequency_mhz = 0;
  char temp_buffer[BOOT_LOG_TEMP_BUFFER_SIZE];

  /* boot_clock_get_ddr_speed returns DDR frequency in KHz, convert to Mhz
     and use. */
  ddr_frequency_mhz = boot_clock_get_ddr_speed() / 1000;

  snprintf(temp_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "DDR Frequency, %d MHz",
           ddr_frequency_mhz);

  /* Add entry to beginning of log */
  boot_log_message_raw(temp_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}


/*===========================================================================

**  Function : boot_log_image_version

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the SBL image version.
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
void boot_log_image_version(void)
{
  char temp_buffer[BOOT_LOG_TEMP_BUFFER_SIZE];

  /* Log QC Image Version.  Truncate to BOOT_LOG_TEMP_BUFFER_SIZE */
  snprintf(temp_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "%s",
           QC_IMAGE_VERSION_STRING_AUTO_UPDATED);

  boot_log_message_raw(temp_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);


  /* Log Image Variant.  Truncate to BOOT_LOG_TEMP_BUFFER_SIZE */
  snprintf(temp_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "%s",
           IMAGE_VARIANT_STRING_AUTO_UPDATED);

  boot_log_message_raw(temp_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);


  /* Log OEM Image Version. Truncate to BOOT_LOG_TEMP_BUFFER_SIZE */
  snprintf(temp_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "%s",
           OEM_IMAGE_VERSION_STRING_AUTO_UPDATED);

  boot_log_message_raw(temp_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}


/*===========================================================================

**  Function : boot_log_apps_frequency

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the clock frequency of apps core specified.
*
* @par
*   core - Core number to log frequency of.
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
void boot_log_apps_frequency(uint32 core)
{
  uint32 core_frequency_mhz = 0;
  char temp_buffer[BOOT_LOG_TEMP_BUFFER_SIZE];

  /* boot_clock_get_core_speed returns processor frequency in KHz,
     convert to Mhz and use. */
  core_frequency_mhz = boot_clock_get_apps_speed(core) / 1000;

  snprintf(temp_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "Core 0 Frequency, %d MHz",
           core_frequency_mhz);

  /* Add entry to beginning of log */
  boot_log_message_raw(temp_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}
