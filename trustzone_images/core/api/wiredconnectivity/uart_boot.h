#ifndef UART_BOOT_H
#define UART_BOOT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   U A R T   B O O T  D R I V E R

GENERAL DESCRIPTION
  This module contains a very simple-minded polling UART driver
  for use during boot (download, etc).  

EXTERNALIZED FUNCTIONS
   uart_boot_drain
      Wait for any outstand transmits to complete.

   uart_boot_init
      Initializes the UART for transmit and receive.

   uart_boot_init_from_reset
      Does the same thing as uart_boot_init, but also
      configures the UART hardware.  uart_boot_init_from_reset
      calls uart_boot_init.

   uart_boot_active
      Determines if there are chars in UART RX FIFO.

   uart_boot_receive_byte
      Receives a single byte from the UART.

   uart_boot_transmit_byte
      Transmits a single byte to the UART.

   uart_boot_transmit_bytes
      Transmits multiple bytes to the UART.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  All necessary initialization for normal CPU operation must have
  been performed before entering this module.  uart_boot_init() should
  be called before any other functions.

  Copyright (c) 2009 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uart_boot.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/17/09   tb      Initial version.

===========================================================================*/


/* Error return code from uart_boot_receive_byte */
/* This int value must not be any valid unsigned char value. */
#define  UART_RX_ERR    (-1)
#define  UART_NO_CHAR   (-2)
#define  UART_TIMEOUT   (-3)


/*===========================================================================

FUNCTION uart_boot_drain

DESCRIPTION
  This function waits for the last character in the UART's transmit
  FIFO to be transmitted.  This allows the caller to be sure that all
  characters are transmitted.

DEPENDENCIES
  If the transmit FIFO is not empty, the UART must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/

void uart_boot_drain(void);

/*===========================================================================

FUNCTION uart_boot_init


DESCRIPTION
  This function initializes the UART to 115,200 bps, 8N1, with
  no interrupts enabled but both receiver and transmitter running.

  Before initializing the UART, this function waits for the UART's
  transmit buffer to empty out.  This permits any previous UART
  usage to complete.

DEPENDENCIES
  If the transmit FIFO is not empty, the UART must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

RETURN VALUE
  Returns TRUE if the hardware was successfully initialized.  Currently, 
  it always returns TRUE.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/
extern boolean uart_boot_init(void);

/*===========================================================================

FUNCTION uart_boot_init_from_reset


DESCRIPTION
  This function initializes the UART hardware, then calls uart_boot_init.

  Use uart_boot_init_from_reset, instead of uart_boot_init, if you know that
  the UART hardware and its dependencies (e.g. clocks, GPIOs) needs to be
  initialized.  This is usually only done just after a reset.

DEPENDENCIES
  If the transmit FIFO is not empty, the UART must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

RETURN VALUE
  Returns TRUE if the hardware was successfully initialized.  Currently, 
  it always returns TRUE.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/
extern boolean uart_boot_init_from_reset(void);

/*===========================================================================

FUNCTION uart_boot_active

DESCRIPTION
  This function determines if there are chars in UART RX FIFO.

DEPENDENCIES
  The UART must be initialized and enabled.

RETURN VALUE
  Returns TRUE if the RX FIFO contains data and there are no errors.

SIDE EFFECTS

===========================================================================*/
extern boolean uart_boot_active (void);

/*===========================================================================

FUNCTION uart_boot_receive_byte

DESCRIPTION
  This function receives a single byte from the UART by polling.

DEPENDENCIES
  The UART must be initialized and enabled, or else this routine will
  wait forever.

RETURN VALUE
  If a character is received without error, returns the value of the
  character received.  If an error occurs, returns UART_RX_ERR.  If a
  timeout occurs, returns UART_TIMEOUT.

SIDE EFFECTS

===========================================================================*/
extern int uart_boot_receive_byte(void);


/*===========================================================================

FUNCTION uart_boot_transmit_bytes

DESCRIPTION
  This function transmits a multiple bytes through the UART.

DEPENDENCIES
  The UART transmitter must be initialized and enabled, or this routine
  will wait forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/
extern void uart_boot_transmit_bytes
(
  byte  *chr_ptr,   /* Characters to be transmitted */
  int   num_bytes
);

/*===========================================================================

FUNCTION uart_boot_transmit_byte

DESCRIPTION
  This function transmits a single byte through the UART.

DEPENDENCIES
  The UART transmitter must be initialized and enabled, or this routine
  will wait forever.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/
extern void uart_boot_transmit_byte
(
  byte  chr   /* Character to be transmitted */
);

/*===========================================================================

FUNCTION uart_boot_close

DESCRIPTION
  This function turns off clocks to UART hardware.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void uart_boot_close(void);

#endif /*UART_BOOT_H*/

