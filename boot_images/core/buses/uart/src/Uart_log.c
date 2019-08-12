#if ( defined(UART_LOG_ALL) || defined(UART_LOG_ERROR) )
/*==================================================================================================

FILE: Uart_log.c

DESCRIPTION: This module provides the logging feature for the UART driver Software.

                           Copyright (c) 2013 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/

/*==================================================================================================
Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/uart/src/Uart_log.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/10/13   VV      Initial revision.

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "Uart_log.h"


/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
#define LOG_BUF_SIZE 512
#define MSG_HEADER_SIZE (sizeof(MSG_HEADER))
#define MSG_START 0x1234ABCD

typedef struct
{
   uint32  msg_start;
   uint16  file_id;
   uint16  line;
   uint16  num_args;
   uint16  msg_seq;
}MSG_HEADER;

typedef struct
{
   uint8* ptr;
   uint8  log[LOG_BUF_SIZE];
   uint16 bytes_left;
   uint16 next_msg_seq;
}UART_LOG_STRUCT;

UART_LOG_STRUCT uart_log_struct =
{
   NULL,
   {0},
   LOG_BUF_SIZE,
   0
};


/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void fill_msg_header(MSG_HEADER* msg_ptr, uint8 file_id, uint16 line, uint8 num_args);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
static void fill_msg_header(MSG_HEADER* msg_ptr,uint8 file_id,uint16 line, uint8 num_args)
{

   msg_ptr->msg_start = MSG_START;
   msg_ptr->file_id   = file_id;
   msg_ptr->line      = line;
   msg_ptr->num_args  = num_args;
   msg_ptr->msg_seq   = uart_log_struct.next_msg_seq++ ;

}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
void uart_log(uint8 file_id,uint16 line, uint8 num_args, uint32 a1, uint32 a2, uint32 a3)
{
   uint32   msg_bytes = 0;
   uint32   args[3];
   uint32*  data_ptr;
   uint8    i;
   uint8*   ptr;
   if(num_args >= 3)
   {
      return;
   }

   args[0] = a1;
   args[1] = a2;
   args[2] = a3;

   // Calculate the total msg size.
   msg_bytes = MSG_HEADER_SIZE + num_args * sizeof( uint32 );

   // If it is the first time the UART_LOG is called or the buffer is full, roll over.
   if ((uart_log_struct.ptr == NULL) || (msg_bytes > uart_log_struct.bytes_left))
   {
      uart_log_struct.ptr = uart_log_struct.log;
      uart_log_struct.bytes_left = LOG_BUF_SIZE;
   }

   uart_log_struct.bytes_left -= msg_bytes;
   ptr = uart_log_struct.ptr;
   uart_log_struct.ptr += msg_bytes;

   // Fill in the header
   fill_msg_header((MSG_HEADER*)ptr,file_id,line,num_args);
   ptr = ptr + MSG_HEADER_SIZE;
   data_ptr = (uint32*) ptr;

   // Fill in the data
   for (i = 0;i < num_args; i++)
   {
      *data_ptr++ = args[i];
   }
}

#endif
