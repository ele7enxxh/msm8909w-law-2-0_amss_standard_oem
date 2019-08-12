#ifndef UART_OS_H
#define UART_OS_H
/*==================================================================================================

FILE: Uart_os.h

DESCRIPTION: This module provides the OS specific APIs to the UART driver software.

                              Copyright (c) 2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/uart_lite/inc/Uart_os.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/25/13   VV      Included interrupt APIs to support driver on DAL env.
06/24/13   VV      Initial revision.

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "Uart.h"
#include "Uart_defs.h"

void       Uart_busy_wait(uint32 uSecs);
UartResult Uart_clock_close(UART_CONTEXT* h);
UartResult Uart_clock_open(UART_CONTEXT* h,uint32 input_freq);
UartResult Uart_get_driver_properties(void);
UartResult Uart_get_properties(UART_CONTEXT* h);
UartResult Uart_interrupt_close(UART_CONTEXT* h);
UartResult Uart_interrupt_done(UART_CONTEXT* h);
UartResult Uart_interrupt_open(UART_CONTEXT* h, void* isr);
UartResult Uart_tlmm_open(UART_CONTEXT* h);
UartResult Uart_tlmm_close(UART_CONTEXT* h);

#endif

