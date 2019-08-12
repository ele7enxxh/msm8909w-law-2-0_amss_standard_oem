/*===========================================================================

                    BOOT LOGGER UART DRIVER 

DESCRIPTION
  This file contains the function interface for logging messages during boot
  to the uart buffer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: 
$DateTime: 
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/28/14   ck      Added optional data argument to boot_log_message_uart
05/28/14   ck      Updated header for new bootlogger layout
08/20/13   lm      Initial Creation.

===========================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_util.h"
#include "boot_logger_uart.h"
#include "boot_uart.h"
#include "boot_logger.h"

/*=============================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
=============================================================================*/

/**
 * Header and formating info.
 * Used to centralize the formatting strings so that if a change was made to
 * these it would propogate throughout the logger module correctly.
 */
static char LEGEND_HEADER[] = "Format: Log Type - Time(microsec) - Message - Optional Info";
static char LEGEND_KEY[]    = "Log Type: B - Since Boot(Power On Reset),  D - Delta,  S - Statistic";
static char FORMATTING_STRING_1[] = " - ";
static char OVERFLOW_MSG[] = " OVERFLOW";
static char NEWLINE[] = "\r\n";

/*Buffer to store timer digit*/
static char ascii_char[LOG_MAX_NUM_DIGIT_DISPLAY];

/*Buffer to store display string*/
static char uart_buf[UART_BUF_SIZE]; 
static char *uart_buf_ptr = &uart_buf[0];
static boolean uart_init_flag = FALSE;

/* Variable indicating UartHandler */
static UartHandle serial_port;

#define HEADER_LEN              (sizeof(LEGEND_HEADER) -1)
#define KEY_LEN                 (sizeof(LEGEND_KEY) -1)
#define FORMATTING_STRING_1_LEN (sizeof(FORMATTING_STRING_1) -1)
#define OVERFLOW_MSG_LEN        (sizeof(OVERFLOW_MSG) -1)
#define NEWLINE_LEN             (sizeof(NEWLINE) -1)


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_log_uart_put_string

** ==========================================================================*/
/*!
* 
* @brief
* This function will copy string with given length to uart buffer.
* It does not care if the string is NULL terminated or not, only blindly copy
* "len" bytes of chars to the buffer.
*
* @param[in] str  pointer to the start of string
*
* @param[in] len  length of the string in bytes
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
static void boot_log_uart_put_string(char *str, uint32 len)
{
  qmemcpy(uart_buf_ptr, str, len);
  uart_buf_ptr += len;
}

/*===========================================================================

**  Function :  boot_log_uart_put_time

** ==========================================================================*/
/*!
* 
* @brief
* This function will take a uint32 timestamp and turn it into a decimal
* ASCII number.  Then it will write it to the uart buffer.
 
* Note:  It will cap out at 999 seconds (16 minutes, 39 seconds).
* This cap was chosen because it should never be maxed out during boot
* and by keeping it shorter it saves space in the buffer.  The size
* can be changed by adjusting LOG_MAX_NUM_DIGIT_DISPLAY to the desired
* number of decimal digits.
*
* @param[in] timestamp  32bits timestamp
*
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
static void boot_log_uart_put_time(uint32 timestamp)
{
  /*Limits the number of decimals that the ASCII number is represented as. 
    In this case it can represent 999 seconds (999,999,999 microseconds).  */
  int ascii_char_len = LOG_MAX_NUM_DIGIT_DISPLAY;
  int i;
  
  /*check if timestamp is overflowed*/
  if(timestamp < LOG_MAX_TIMESTAMP_VAL)
  {
    /*Determine decimal ASCII string, starting 
      with the least siginificant digits.*/
    for(i = (ascii_char_len - 1); i >=0 ; i--)
    {
      ascii_char[i] = (char) (timestamp % 10) + '0';
      timestamp /= 10;
    }
    /*Replaced leading zeros with a space, leaving a single 0 if timestamp==0*/
    for(i = 0; ascii_char[i] == '0' && i <= (ascii_char_len - 2); i++)
    {
      ascii_char[i] = ' ';
    }
    
    boot_log_uart_put_string(ascii_char,ascii_char_len);
  }
  else
  {
    boot_log_uart_put_string(OVERFLOW_MSG, OVERFLOW_MSG_LEN);
  }
}

/*===========================================================================

**  Function :  boot_log_init_uart

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will initiate the uart driver.
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
void boot_log_init_uart (void)
{
	boot_uart_init(&serial_port,UART_MAIN_PORT);
}

/*===========================================================================

**  Function :  boot_log_deinit_uart

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will deinitilize the uart driver.
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
void boot_log_deinit_uart(void)
{
	boot_uart_deinit(serial_port);
}
/*===========================================================================

**  Function :  boot_log_message_uart

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will transmit the message available in the uart buffer.
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
void boot_log_message_uart(char * message,
                           uint32 timestamp,
                           char log_type,
                           char * optional_data)
{
  uint32 message_len = 0;
  uint32 optional_data_len = 0;
  uint32 total_message_len = 0;
  	
  do
  {
    if(message == NULL)
    {
      break;	
    }
    
    /* Calculate message and optional data lengths. */
    message_len = boot_strlen(message);

    if (optional_data != NULL)
    {
      optional_data_len = boot_strlen(optional_data);
    }
    

    /* Sum up the total length of the output to the buffer */
    if (timestamp != BOOT_DND_TIMESTAMP)
    {
      total_message_len += LOG_MAX_NUM_DIGIT_DISPLAY
                        + FORMATTING_STRING_1_LEN;
    }

    total_message_len += sizeof(log_type)             
                      + FORMATTING_STRING_1_LEN
                      + message_len
                      + NEWLINE_LEN;
    
    /* If optional data exists then add to total message length. */
    if (optional_data != NULL)
    {
      total_message_len += optional_data_len
                        + FORMATTING_STRING_1_LEN;
    }


    if(uart_init_flag == FALSE)
    {	    
      /*Writes out the legend information to explain output formatting*/
      boot_log_uart_put_string(NEWLINE,NEWLINE_LEN);
      boot_log_uart_put_string(LEGEND_HEADER,HEADER_LEN);
      boot_log_uart_put_string(NEWLINE,NEWLINE_LEN);
      boot_log_uart_put_string(LEGEND_KEY,KEY_LEN);
      boot_log_uart_put_string(NEWLINE,NEWLINE_LEN);
      boot_uart_tx(serial_port,uart_buf,(KEY_LEN+HEADER_LEN+(3*NEWLINE_LEN)));
      
      uart_init_flag = TRUE;
    }
    /*Buffer must be initialized.*/
    qmemset(uart_buf,0x0,UART_BUF_SIZE);
    uart_buf_ptr = &uart_buf[0];
    
    /*Copy log_type*/
    boot_log_uart_put_string(&log_type,
                             sizeof(log_type));
    
    /*Formatting string, for readability*/
    boot_log_uart_put_string(FORMATTING_STRING_1,
                             FORMATTING_STRING_1_LEN);
    
    if (timestamp != BOOT_DND_TIMESTAMP)
    {
      /*Copy timestamp*/
      boot_log_uart_put_time(timestamp);
    
      /*Formatting string, for readability*/
      boot_log_uart_put_string(FORMATTING_STRING_1,
                               FORMATTING_STRING_1_LEN);
    }
    
    /*Write out message to uart buffer, checks for null ptr*/
    boot_log_uart_put_string(message,
                             message_len);

    /* Write out optional data if applicable. */
    if (optional_data != NULL)
    {
      boot_log_uart_put_string(FORMATTING_STRING_1,
                               FORMATTING_STRING_1_LEN);

      boot_log_uart_put_string(optional_data,
                               optional_data_len);
    }

    boot_log_uart_put_string(NEWLINE,
                             NEWLINE_LEN);
		
    boot_uart_tx(serial_port,
                 &uart_buf[0],
                 total_message_len);
    
  }while(0); 
}
