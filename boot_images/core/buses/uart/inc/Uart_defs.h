#ifndef UART_DEFS_H
#define UART_DEFS_H
/*==================================================================================================

FILE: Uart_defs.h

DESCRIPTION: This module provides the driver software for the UART.

                           Copyright (c) 2013 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/uart/inc/Uart_defs.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/24/13   VV      Initial revision.

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "Uart.h"

typedef struct
{
   uint32 uart_base;
   uint32 bit_rate;
   uint32 is_loopback;
   uint32 gpio_tx_config;
   uint32 gpio_rx_config;
   uint32 gpio_cts_config;
   uint32 gpio_rfr_config;
   uint32 clock_id_index;
}UART_PROPERTIES;

typedef struct uart_context
{
   char*                port_id;
   boolean              is_port_open;
   UART_PROPERTIES      properties;
   char*                read_buf;
   uint32               rx_write_offset;
   uint32               rx_read_offset;
   uint32               rx_bytes_left;
}UART_CONTEXT;

typedef struct
{
   char*    device_names[UART_MAX_PORTS];
}UART_DRIVER_PROPERTIES;

#endif
