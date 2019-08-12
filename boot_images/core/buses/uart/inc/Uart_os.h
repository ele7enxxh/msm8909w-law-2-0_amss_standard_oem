#ifndef UART_OS_H
#define UART_OS_H
/*==================================================================================================

FILE: Uart_os.h

DESCRIPTION: This module provides the OS specific APIs to the UART driver software.

                              Copyright (c) 2013 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/uart/inc/Uart_os.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/24/13   VV      Initial revision.

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "Uart.h"
#include "Uart_defs.h"

UartResult Uart_tlmm_open(UART_CONTEXT* h);
UartResult Uart_tlmm_close(UART_CONTEXT* h);
UartResult Uart_get_driver_properties(void);
UartResult Uart_get_properties(UART_CONTEXT* h);
UartResult Uart_clock_close(UART_CONTEXT* h);
UartResult Uart_clock_open(UART_CONTEXT* h,uint32 input_freq);
void       Uart_busy_wait(uint32 uSecs);

#endif

