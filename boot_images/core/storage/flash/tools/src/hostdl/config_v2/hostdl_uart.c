
/*==================================================================
 *
 * FILE:        hostdl_uart.c
 *
 * SERVICES:    UART Driver
 *
 * GENERAL DESCRIPTION
 *   This module contains a very simple-minded polling UART driver
 *   for ARMPRG.  This version supports only MSM6250.
 *
 *   This module must be ported to all other MSM architectures.
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS
 *   All necessary initialization for normal CPU operation must have
 *   been performed before entering this module.  uart_init() should
 *   be called before any other functions.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/config_v2/hostdl_uart.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 08/22/09     op      Upper case uart_reg_sym macro
 * 02/27/09     jz      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "comdef.h"
#include "hostdl.h"
#include "hostdl_config.h"

//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Extern and Forward Declarations
//--------------------------------------------------------------------------
extern boolean uart_init (void);
extern boolean uart_active (void);
extern void do_uart_check (void);
extern void uart_drain (void);

extern void packet_handle_incoming (int ch);
extern byte uart_receive_byte (void);
extern void uart_transmit_byte (byte);

//--------------------------------------------------------------------------
// Global Data
//--------------------------------------------------------------------------

/*
 *  UART function dispatch table
 */
DISP uart_dispatch_table= {
   uart_init,
   uart_active,
   uart_drain,
   do_uart_check,
   uart_transmit_byte,
};

//--------------------------------------------------------------------------
// Function Declarations
//--------------------------------------------------------------------------

/*===========================================================================

  This function waits for the last character in the UART's transmit
  FIFO to be transmitted.  This allows the caller to be sure that all
  characters are transmitted.

  If the transmit FIFO is not empty, the UART must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

===========================================================================*/

void
uart_drain (void)
{

  /* Wait while the UART's transmitter drains.  This lets the ACK to
   * the GO command that started FLASHPRG get back to the PC before
   * we reset the UART. */
  while((HWIO_IN(UART_REG_SYM(SR)) &
       HWIO_FMSK(UART_REG_SYM(SR),TXEMT) ) == 0)
  {
    KICK_WATCHDOG ();           /* Don't let the watchdog expire while we wait  */
  }

} /* uart_drain() */

/*===========================================================================

  This function returns the status of whether the UART is active or not.

===========================================================================*/

boolean
uart_active (void)
{
  byte status; // to read and save the UART status register contents.
  int ret = FALSE;

  status = HWIO_IN(UART_REG_SYM(SR));
  if ((status & HWIO_FMSK(UART_REG_SYM(SR),RXRDY)) != 0)
  {
     /* Make sure that there are no errors logged in the status register.
      * If there are any errors, issue a reset on the error status and
      * UART receiver. And, reenable the UART receiver.
      */

     if((status & UART_SR_RX_ERROR) != 0)
     {
       HWIO_OUT(UART_REG_SYM(CR), UART_CR_RESET_ERR);  /* Reset error status     */
       HWIO_OUT(UART_REG_SYM(CR), UART_CR_RESET_RX);   /* Reset the receiver     */
       HWIO_OUT(UART_REG_SYM(CR), UART_CR_RESET_RX);   /* Reset the receiver     */
       HWIO_OUT(UART_REG_SYM(CR), UART_CR_RX_ENA);     /* Re-enable the receiver */
       ret = FALSE;
     }
     else
     {
       /*
        * Looks like the host is using the UART interface to communicate
        * with the ARMPRG.  DO NOT READ anything from UART FIFO at this
        * point.  That will be done in main loop, even though there is
        * currently a character or more waiting.
        */
       ret = TRUE;
     }
  }

  return ret;
} /* uart_active() */


/*===========================================================================

  This function initializes the MSM 6050 UART to 115,200 bps, 8N1, with
  no interrupts enabled but both receiver and transmitter running.

  Before initializing the UART, this function waits for the UART's
  transmit buffer to empty out.  This permits the ACK to the GO
  command that started the program (or any other pending transmission)
  to get back to the host, instead of being lost when the UART is
  reset.

===========================================================================*/

boolean
uart_init (void)
{
  int chr;                      /* character variable for RX FIFO drain */

  /* Ensure that IRDA mode is disabled */
  HWIO_OUT(UART_REG_SYM(IRDA), 0x00);

  chr = chr * 0; /* Dummy operation to make RVCT2.1 happy */

  /* drain anything in the UART transmitter */
  uart_drain ();

  /* Wait while the UART's reciever drains.  After resetting the UART, we
   * may have a couple of spurious characters in the RX FIFO.  This can
   * happen especially when the phone is not plugged into a DIP, thus
   * causing the UART to re-recognize the start of break condtion.
   */
  while((HWIO_IN(UART_REG_SYM(SR)) &
       HWIO_FMSK(UART_REG_SYM(SR),RXRDY)) != 0)
  {
    chr = HWIO_IN(UART_REG_SYM(RF));     /* Get the received character */
    KICK_WATCHDOG ();  /* Don't let the watchdog expire while we wait  */
  }
  return TRUE;
} /* uart_init() */


/*===========================================================================

  This function does a poll on the receive status register on the receive
  FIFO, and receives incoming data (up to 128 bytes) from the host.

===========================================================================*/

void
do_uart_check (void)
{
  int status;
  int ch;
  int count = 0;

  /* ABSOLUTELY DO NOT MOVE OR REMOVE THIS MACRO!!!!!!
   * Flash erase functions call this function while waiting for erase
   * to complete and expect the watchdog to be kicked here even
   * if nothing else is done
   */
  KICK_WATCHDOG ();

  status = HWIO_IN(UART_REG_SYM(SR));
  while (count++ < 128 && (status &
                         HWIO_FMSK(UART_REG_SYM(SR),RXRDY)) != 0)
  {
    ch = HWIO_IN(UART_REG_SYM(RF));

    if ((status & UART_SR_RX_ERROR) != 0)
    {
      HWIO_OUT(UART_REG_SYM(CR), UART_CR_RESET_ERR);  /* Reset error status     */
      HWIO_OUT(UART_REG_SYM(CR), UART_CR_RESET_RX);   /* Reset the receiver     */
      HWIO_OUT(UART_REG_SYM(CR), UART_CR_RESET_RX);   /* Reset the receiver     */
      HWIO_OUT(UART_REG_SYM(CR), UART_CR_RX_ENA);     /* Re-enable the receiver */
#ifdef DEBUG_UART_RCV_ERROR
      if ((status & UART_SR_BREAK_RXD) != 0)
        term_put ('B');
      if ((status & UART_SR_PF_ERR) != 0)
        term_put ('P');
      if ((status & UART_SR_OVR_ERR) != 0)
        term_put ('O');
#endif
    }
    else
    {
      /* Deal with the character. */
      packet_handle_incoming (ch);
    }

    status = HWIO_IN(UART_REG_SYM(SR));
    KICK_WATCHDOG ();
  }
} /* do_uart_check() */

/*===========================================================================

  This function transmits a single byte through the UART.

  The UART transmitter must be initialized and enabled, or this routine
  will wait forever.

===========================================================================*/

void
uart_transmit_byte (byte chr)            /* Character to be transmitted */
{
  /* Wait until the transmit FIFO is not too full */
  while((HWIO_IN(UART_REG_SYM(ISR)) &
       HWIO_FMSK(UART_REG_SYM(ISR),TXLEV)) == 0)
  {
    /* Don't let the watchdog expire while we wait */
    KICK_WATCHDOG ();
  }

  /* Put the byte into the Tx FIFO  */
  (void) HWIO_OUT(UART_REG_SYM(TF), chr);
}/* uart_transmit_byte() */

/*===========================================================================

  This function does a poll on the receive status register on the receive
  FIFO, and receives 1 byte from the host.

  The UART hardware must be initialized.

===========================================================================*/

byte
uart_receive_byte (void)
{
  int status;
  byte ch;
  int valid_char = 0;

  do
  {
    KICK_WATCHDOG ();
    status = HWIO_IN(UART_REG_SYM(SR));

    while((status & HWIO_FMSK(UART_REG_SYM(SR),RXRDY)) == 0)
    {
      KICK_WATCHDOG ();
      status = HWIO_IN(UART_REG_SYM(SR));
    }

    ch = HWIO_IN(UART_REG_SYM(RF));

    if ((status & UART_SR_RX_ERROR) != 0)
    {
      HWIO_OUT(UART_REG_SYM(CR), UART_CR_RESET_ERR);  /* Reset error status  */
      HWIO_OUT(UART_REG_SYM(CR), UART_CR_RESET_RX);   /* Reset the receiver  */
      HWIO_OUT(UART_REG_SYM(CR), UART_CR_RESET_RX);   /* Reset the receiver  */
      HWIO_OUT(UART_REG_SYM(CR), UART_CR_RX_ENA);     /* Re-enable receiver  */
#ifdef DEBUG_UART_RCV_ERROR
      if ((status & UART_SR_BREAK_RXD) != 0)
    {
        term_put ('B');
    }
      if ((status & UART_SR_PF_ERR) != 0)
    {
        term_put ('P');
    }
      if ((status & UART_SR_OVR_ERR) != 0)
      {
        term_put ('O');
    }
#endif
    }
    else
    {
      /* Have a valid character. */
      valid_char = 1;
    }
  }
  while (!valid_char);

  return ch;
} /* uart_receive_byte() */

/**************************************************************************
 * Debug loopback helper function
 *   Get this echoing characters to a terminal emulator before trying
 *   to use ARMPRG with QPST.  If this does not work reliably,
 *   there is no use trying to debug why ARMPRG will not communicate with
 *   QPST.
 ***************************************************************************/

#ifdef DEBUG_LOOPBACK_UART
/*  debug function to be called from main for loopback emulation */
void
uart_loopback (void)
{
  byte ch;

  while (1)
  {
    ch = uart_receive_byte ();
    uart_transmit_byte (ch);
  }
} /* uart_loopback() */
#endif /* DEBUG_LOOPBACK_UART */
