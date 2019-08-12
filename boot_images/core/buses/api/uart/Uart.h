#ifndef UART_H
#define UART_H
/*==================================================================================================

FILE: Uart.h

DESCRIPTION: This module provides the driver software for the UART.

                           Copyright (c) 2013 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   Uart_deinit
   Uart_init
   Uart_is_break_active
   Uart_receive
   Uart_transmit

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/api/uart/Uart.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/07/13   VV      Included break detection API
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

typedef void* UartHandle;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

UartResult      Uart_deinit(UartHandle h);
UartResult      Uart_init(UartHandle* h,UartPortID PortID);
boolean         Uart_is_break_active(UartHandle h);
uint32          Uart_receive(UartHandle h,char* buf,uint32 bytes_to_rx);
uint32          Uart_transmit(UartHandle h,char* buf,uint32 bytes_to_tx);

#endif
