#ifndef MBA_LOGGER_H
#define MBA_LOGGER_H

/**
* @file mba_logger.c
* @brief MBA Boot Logger
*
* This file implements the MBA Boot Logger
*
*/
/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/inc/mba_logger.h#1 $
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who          what, where, why
--------   --------     ------------------------------------------------------
02/13/12   rsahay         Initial version. Integrated from Boot 2.0
============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/**
* return value when timestamp overflow happens
*/
#define LOG_TIMESTAMP_OVERFLOW  0xFFFFFFFF

/* Boot messages log 
*  Define FEATURE_MBA_LOGGER to use Logger
*/
#ifdef FEATURE_MBA_LOGGER
#define MBA_LOG_MSG(x) \
        mba_boot_log_message(x);  // -> this will be logged as a message

#define MBA_LOG_MSG_START_TIMER() \
        mba_boot_log_start_timer();  // -> start timer

#define MBA_LOG_MSG_END(x) \
        mba_boot_log_stop_timer(x); //-> this will also be logged

#define MBA_LOG_INIT() \
        mba_boot_logger_main();
#else
#define MBA_LOG_MSG(x)
#define MBA_LOG_MSG_START_TIMER()  
#define MBA_LOG_MSG_END(x)
#define MBA_LOG_INIT()  
#endif


/**
 * This struct defines the structure of boot logger's meta info, which keeps
 * track of the logger's internal states.
 * It contains information about current logger's logging buffer and timer.
 * The size of this structure must be smaller than SCL_BOOT_LOG_META_INFO_SIZE
 */
struct mba_boot_log_meta_info
{
  /**
   *pointer that points to beginning of the logging buffer
   */
  uint8  *log_buf_start;
  
  /**
   *pointer that points to the next empty byte in the logging buffer
   */
  uint8  *log_buf_ptr;
  
  /**
   * total size of the logging buffer in bytes
   */
  uint32 log_buf_size;
  
  /**
   *status of logging buffer, initialized(TRUE) or uninitialized(FALSE)
   */
  uint32 log_buf_init;
  
  /**
  * stores the timestamp which serves as a reference point
  */
  uint32 ref_time;
  
  /**
   *stores the timestamp the stopwatch records 
   */
  uint32 start_time;
  
  /**
   *the status of stopwatch, locked(TRUE) or unlocked(FALSE)
   */
  uint32 stopwatch_locked;
};


/**
 * Structure that holds the start address/size of 
 * the boot logger's logging buffer and meta data.
 * This structure can be expanded to store other information.
 * log_buffer_start and meta_info_start must be assigned to 
 * valid addresses prior to use boot logger.
 * boot_log_init must be called before using any
 * boot logger function.
 */
typedef struct mba_boot_log_init_data_t
{
  /**
  * Start address of the current logging buffer 
  */
  void *log_buffer_start;
  
  /**
  * Size of the current logging buffer 
  */
  uint32 log_buffer_size;
  
  /**
  * Start address of the current meta info structure
  */
  void *meta_info_start;
  
  /**
  * Size of the current meta info structure
  */
  uint32 meta_info_size;
  
 /**
  * Start address of the old meta info structure.
  * It is used to retrieve reference time from the old boot logger
  * Set this value to NULL will default the reference time of current logger
  * to 0.
  */
  void *old_meta_info_start;
  
}mba_boot_log_init_data;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  mba_boot_log_set_meta_info

** ==========================================================================
*/
/*!
* 
* @brief
*  Boot Logger uses a global pointer to store the address of meta info structure.
*  In the circumstances of any code initializes RW section of the load image 
*  after the boot logger is initialized, this function should be called to 
*  restore the meta info address that is kept in RW section of boot logger module.
*  By doing this we can log the time takes to initilize RW section of the image.
*
* @param[in] addr start address of this boot logger's meta info structure
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
void mba_boot_log_set_meta_info(void *addr);


/*===========================================================================

**  Function :  mba_boot_log_get_meta_info

** ==========================================================================
*/
/*!
* 
* @brief
*  This function returns a pointer to the meta info structure used by current logger.
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
struct mba_boot_log_meta_info* mba_boot_log_get_meta_info(void);


/*===========================================================================

**  Function :  mba_boot_log_message

** ==========================================================================
*/
/*!
* 
* @brief
*  Logs the given message.It will prefix a timestamp, which is the current 
*  absolute time minus the reference time, to the message string.
*
* @param[in] message  log message
*
* @par Dependencies
*   mba_boot_logger_timer
*   mba_boot_logger_ram
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void mba_boot_log_message(char *);


/*===========================================================================

**  Function :  mba_boot_log_start_timer

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will start the stopwatch and mark the beginning of the timed
*  section and store the current timestamp internally.
*
* @par Dependencies
*   mba_boot_logger_timer
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void mba_boot_log_start_timer(void);


/*===========================================================================

**  Function :  mba_boot_log_stop_timer

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will substract the current timestamp with the previus stored 
*  timestamp by boot_log_start_timer(if any) to get the delta time value and 
*  then prefix the delta time to the log message.
*
* @param[in] message  log message
*
* @par Dependencies
*  mba_boot_logger_timer
*  mba_boot_logger_ram
*   
* @retval
*   delta time
* 
* @par Side Effects
*   None
* 
*/
uint32 mba_boot_log_stop_timer(char *);


/*===========================================================================

**  Function :  mba_boot_log_get_time

** ==========================================================================
*/
/*!
* 
* @brief
* This will return the current timestamp which is the absolute time since boot
* subtracts the reference time.
*
* @par Dependencies
*  mba_boot_logger_timer
*   
* @retval
*   current timestamp minus the reference time, LOG_TIMESTAMP_OVERFLOW 
*   if overflow happens
* 
* @par Side Effects
*   None
* 
*/
uint32 mba_boot_log_get_time(void);

/*===========================================================================

**  Function :  mba_boot_logger_main

** ==========================================================================
*/
/*!
* 
* @brief
*  This is the main function that configures the MBA boot logger.
*  This funcion will set the address of logging buffer and initialize it.
*  This function will set the internal reference time to be 
*  the value of the old boot logger meta info structure,
*  which is passed through boot_log_init_data.
*  It will be set to 0 if this old meta info structure pointer is set to NULL 
*  in mba_boot_log_init_data.
*  This function must be called before any other boot logger function is used.
*
* @param[in] init_data  Initialization data passed to boot logger
*
* @par Dependencies
*      Clocks must be enabled. Generally placed before MBA deinitialization.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void mba_boot_logger_main( void );

#endif  /* MBA_LOGGER_H */
