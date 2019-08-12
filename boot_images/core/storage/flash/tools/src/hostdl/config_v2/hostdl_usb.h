#ifndef HOSTDL_USB_H
#define HOSTDL_USB_H

/*==================================================================
 *
 * FILE:        hostdl_usb.h
 *
 * SERVICES:    USB driver header
 *
 * GENERAL DESCRIPTION
 *   This header file is for USB driver in HOSTDL
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/config_v2/hostdl_usb.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 02/27/09     jz      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "usbdc.h"
#include "usbcdc.h"
#include "usbdci.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Error return code from dloadusb_receive_byte */
/* This int value must not be any valid unsigned char value. */
#define  USB_NO_CHAR    (-1)


/* Size of the USB receive buffers.  Must be larger than the packet size
 * including encapsulation and must be an even multiple of 64
 */
#define USB_RX_BUFFER_SIZE 1152

/* USB data to be stored in upper RAM */
typedef struct
{
  /* Buffer to save the information about the line coding. */
  byte line_coding[8];

  /* This points to the transmit buffer on the IN end-point.
   * This must be larger than the transmit buffer back to the
   * host and a multiple of 64 bytes.
   */
  byte tx_buffer[1040];

  /* Ping Pong buffers to store the data received from the host.
   * We keep one DMA going all the time while processing data
   * from the already received buffer.
   */
  byte rx_buffer1[USB_RX_BUFFER_SIZE];
  byte rx_buffer2[USB_RX_BUFFER_SIZE];

  /* Indexes */
  word tx_buf_index;
  word rx_index;

  /* Counter indicates how many bytes are read into the rx_buffer. */
  word rx_num_bytes_read;

  /* The following two attributes are to save the out and in end points
     used by the diag task to communicate with the host. */
  byte usb_in_endpoint;
  byte usb_out_endpoint;

  /* Host defined device address */
  word address;

  /* Setup message flag */
  boolean setup_data_stage_flag;

} usb_data_type;


/*===========================================================================

  This function waits for the last character in the USB's transmit
  FIFO to be transmitted.  This allows the caller to be sure that all
  characters are t

  If the transmit FIFO is not empty, the USB must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

===========================================================================*/

void usb_drain(void);

/*===========================================================================

  This function initializes the MSM 6500 USB device controller with no
  interrupts enabled but both receiver and transmitter running.

  Before initializing the USB, this function waits for the USB's
  transmit buffer to empty out.  This permits the ACK to the GO
  command that started the program (or any other pending transmission)
  to get back to the host, instead of being lost when the USB reset.

  If the transmit FIFO is not empty, the USB must be initialized enough
  for it to eventually empty itself, or else this routine will wait
  forever.

===========================================================================*/

boolean usb_init(void);

/*===========================================================================

  This function receives an incoming data from the respective usb out fifos and
  returns one character at a time to the calling function. Though it receives
  a bigger packet at once, it always retuns one character to the calling function.
  This approach is choosen to have a consitancy between the UART and USB modules.

  The USB transmitter must be initialized and enabled, or this routine
  will wait forever.

===========================================================================*/

int usb_receive_byte(void);

/*===========================================================================

  This function transmits a single dword through the USB. If the end of message
  (0x7E) is received from the packet module, then this function will send the
  data without waiting for the remaining bytes to fill the 4 byte buffer.

===========================================================================*/

void usb_transmit_byte (byte  chr);


#endif // #ifndef HOSTDL_USB_H
