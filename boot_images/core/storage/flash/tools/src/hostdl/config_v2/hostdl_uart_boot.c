
/*==================================================================
 *
 * FILE:        hostdl_uart_boot.c
 *
 * SERVICES:    UART_BOOT Driver
 *
 * GENERAL DESCRIPTION
 *   This module calls UART_BOOT API's for HOSTDL over UART support.
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS
 *   All necessary initialization for normal CPU operation must have
 *   been performed before entering this module.  uart_boot_init()
 *   should be called before any other functions.
 *
 *        Copyright © 2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *            QUALCOMM Proprietary/GTDR
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/config_v2/hostdl_uart_boot.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 11/06/09     jz      Cleaned up, removed the wrappers and uart_loopback()
 * 11/04/09     jz      Initial version based on updates from Tom B.
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "comdef.h"
#include "hostdl.h"
#include "hostdl_config.h"
#include "uart_boot.h"

//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Extern and Forward Declarations
//--------------------------------------------------------------------------
extern void do_uart_check (void);
extern void packet_handle_incoming (int ch);

//--------------------------------------------------------------------------
// Global Data
//--------------------------------------------------------------------------

/*
 *  UART function dispatch table
 */
DISP uart_dispatch_table= {
   uart_boot_init,
   uart_boot_active,
   uart_boot_drain,
   do_uart_check,
   uart_boot_transmit_byte,
};


/*===========================================================================

  This function does a poll on the receive status register on the receive
  FIFO, and receives incoming data (up to 128 bytes) from the host.

===========================================================================*/

void do_uart_check (void)
{
  int ch;
  int count = 0;

  /* ABSOLUTELY DO NOT MOVE OR REMOVE THIS MACRO!!!!!!
   * Flash erase functions call this function while waiting for erase
   * to complete and expect the watchdog to be kicked here even
   * if nothing else is done
   */
  KICK_WATCHDOG ();

  while (count++ < 128)
  {
    ch = uart_boot_receive_byte ();
    if (ch < 0)
    {
      break;
    }

    /* Deal with the character. */
    packet_handle_incoming (ch);

    KICK_WATCHDOG ();
  }
} /* do_uart_check() */
