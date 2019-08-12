#ifndef UART_H
#define UART_H
/*==================================================================================================

FILE: Uart.h

DESCRIPTION: This module provides the driver software for the UART.

                           Copyright (c) 2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   Uart_deinit
   Uart_init
   Uart_receive
   Uart_register_event_callback
   Uart_transmit

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/uart_lite/Uart.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/25/13   VV      Included client callback support for RX Events.
06/24/13   VV      Initial revision.

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "Uart_ComDefs.h"


typedef enum
{
   UART_MAIN_PORT = 0,
   UART_MAX_PORTS,
}UartPortID;

typedef enum
{
   UART_SUCCESS = 0,
   UART_ERROR,
}UartResult;

typedef enum
{
   UART_EVENT_RX = 0,
}UartEvent;

typedef void* UartHandle;
typedef void(*UART_CALLBACK)(UartEvent ue,uint32 num_bytes);

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

UartResult      Uart_deinit(UartHandle h);
UartResult      Uart_init(UartHandle* h,UartPortID PortID);
uint32          Uart_receive(UartHandle h,char* buf,uint32 bytes_to_rx);
UartResult      Uart_register_event_callback(UartHandle h, UART_CALLBACK client_callback);
uint32          Uart_transmit(UartHandle h,char* buf,uint32 bytes_to_tx);

#endif
