/*==================================================================
 *
 * FILE:        hostdl_usb.c
 *
 * SERVICES:    USB driver
 *
 * GENERAL DESCRIPTION
 *   This module contains code for the MSM6500 USB driver
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS
 *   All necessary initialization for normal CPU operation must have
 *   been performed before entering this module.  usb_init() should
 *   be called before any other functions.
 *
 * Copyright (c) 2009-2010, 2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/config_v2/hostdl_usb.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 04/04/13     eo      Add usb deinit support
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
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
#include "hostdl_usb.h"
#include "task.h"
#define CPU ARM
#include <string.h>
#include "comdef.h"
#include "boothw.h"
#include "usbdc.h"
#include "usbcdc.h"
#include "usbdci.h"
#include "sio.h"
#include "bio.h"


//--------------------------------------------------------------------------
// Extern and Forward Function Declarations
//--------------------------------------------------------------------------
extern boolean usb_init (void);
extern boolean usb_active (void);
extern void usb_drain (void);
extern void do_usb_check (void);
extern void usb_transmit_byte (byte);
extern void usb_deinit (void);

extern boolean process_packet_flag;  // flag indicates active packet processing
extern void packet_handle_incoming (int);


//--------------------------------------------------------------------------
// Global Data
//--------------------------------------------------------------------------

/*
 *  USB function dispatch table
 */
DISP usb_dispatch_table= {
   usb_init,
   usb_active,
   usb_drain,
   do_usb_check,
   usb_transmit_byte,
   usb_deinit
};

static boolean wait_for_tx_done = FALSE;
static byte *process_rx_buffer = 0;

static serial_iface_info_type default_serial_info =
{
  115200,                          /* 115,200 baud data rate */
  USBDC_CHAR_FORMAT_1STOP_BIT,     /* 1 stop bit */
  USBDC_PARITY_NONE,               /* no parity */
  8,                               /* 8 data bits/byte */
  FALSE,                           /* carrier detect */
  FALSE,                           /* ring indicator */
  FALSE,                           /* data set ready */
  FALSE,                           /* state of break detection */
  FALSE                            /* request to send */
};

/* Local data required for all bus transfers */
static usb_data_type usb_data;

/* Local pointer to current buffer for RX DMA */
byte *current_rx_buffer;


/*===========================================================================
            GLOBAL DEFINITION
===========================================================================*/
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Local data required for all bus transfers */
static usb_data_type usb_data;

static void usb_wait_for_dma_done
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
);

static void usb_ep_clear_xbuf_int_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
);

static void usb_ep_clear_ybuf_int_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
);

static void usb_ep_disable_done_int
(
  int ep_num,               /* logical EP number */
  usbdc_ep_direction_type ep_dir, /* disable IN, OUT or both directions
                               DONE int of a logic EP             */
  boolean flush_flag
);

static void usb_ep_disable_ready
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
);

/*===========================================================================
  This function retune the address of DWORD0 of EP's descriptor.

===========================================================================*/
static dword * usb_ep_get_descriptor_addr
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
)
{
  dword *ep_mem_ptr;

  ep_mem_ptr = (dword *) HWIO_ADDR(USB_ENDPOINT_MEMORY_BASE);
  /* point to dowrd0 of OUT EP */
  ep_mem_ptr += (4*2*ep_num );

  if ( ep_dir == USBDC_EP_DIR_IN)
  {
    ep_mem_ptr += 4;
  }

  return(ep_mem_ptr);
}

/*===========================================================================

  This clears the Xfilled register corresponding to the ep_num's OUT EndPoint.
  This indicates to the HW that Xbuffer has beed drained and function cntroller
  can begin receiving data from the next OUT token.

===========================================================================*/
static void usb_ep_clear_xfilled
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
)
{
  dword ep_xfilled_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_xfilled_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_xfilled_mask = 2 << ep_num*2;
    break;
  }

  if (HWIO_IN( USB_F_X_FILLED_STATUS) & ep_xfilled_mask)
  {
      HWIO_OUT( USB_F_X_FILLED_STATUS, ep_xfilled_mask);
  }
  /* else, xfilled is already cleared, do nothing */
}

/*===========================================================================

  This clears the Yfilled register corresponding to the ep_num's OUT EndPoint.
  This indicates to the HW that Ybuffer has beed drained and function cntroller
  can begin receiving data from the next OUT token.

===========================================================================*/
static void usb_ep_clear_yfilled
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_yfilled_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_yfilled_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_yfilled_mask = 2 << ep_num*2;
    break;
  }

  if (HWIO_IN( USB_F_Y_FILLED_STATUS) & ep_yfilled_mask)
  {
    HWIO_OUT( USB_F_Y_FILLED_STATUS, ep_yfilled_mask);
  }
}

/*===========================================================================

  This clears the USB_F_ENDPOINT_DONE_STATUS register corresponding to the
  ep_num's ep_dir EndPoint.

  Note:   USB_F_ENDPOINT_DONE_STATUS register bits flip when write to it.

===========================================================================*/
static void usb_ep_clear_done_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
)
{
  dword ep_clear_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_clear_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_clear_mask = 2 << ep_num*2;
    break;
  }

  if ( HWIO_IN(USB_F_ENDPOINT_DONE_STATUS) & ep_clear_mask )
  { /* clear DONE_STATUS only if it is set */
    HWIO_OUT( USB_F_ENDPOINT_DONE_STATUS, ep_clear_mask);
  }
}

/*===========================================================================

  This routine enables EndPoint Done interrupt associated with an
  endpoint.

===========================================================================*/
static void usb_ep_enable_done_int
(
  int ep_num,
  usbdc_ep_direction_type ep_dir /* enable IN, OUT or both directions
                                    DONE int of a logic EP           */
)
{
  dword ep_done_enable_val;

  dword ep_done_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_done_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_done_mask = 2 << ep_num*2;
    break;
  }

  ep_done_enable_val = HWIO_IN(USB_ENDPOINT_DONE_ENA);
  ep_done_enable_val |= ( ep_done_mask );  /*enable both IN and OUT DONE */
  HWIO_OUT( USB_ENDPOINT_DONE_ENA, ep_done_enable_val);

} /* usbdc_ep_enable_done_int */

/*===========================================================================
  This routine enables endpoint.
===========================================================================*/
static void usb_ep_enable_endpoint
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_enable_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_enable_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_enable_mask = 2 << ep_num*2;
    break;
  }

  if ( !(HWIO_IN(USB_ENDPOINT_ENA) & ep_enable_mask) )
  {
    /* note, This register is "Write to Clear" type */
    /* the goal here is to make sure it has "1" as desired*/
    HWIO_OUT(USB_ENDPOINT_ENA, ep_enable_mask);
  }
}

/*===========================================================================
  This routine disables endpoint.
===========================================================================*/
static void usb_ep_disable_endpoint
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_disable_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_disable_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_disable_mask = 2 << ep_num*2;
    break;
  }

  if ( HWIO_IN(USB_ENDPOINT_ENA) & ep_disable_mask )
  {
    HWIO_OUT( USB_ENDPOINT_ENA, ep_disable_mask);
  }
}

/*===========================================================================
This routine enables endpoint to ready.
===========================================================================*/
static void usb_ep_enable_ready
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_ready_mask =0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_ready_mask = 1<<ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_ready_mask = 2 << ep_num*2;
    break;
  }

  if ( !(HWIO_IN(USB_ENDPOINT_READY) & ep_ready_mask))
  {
    HWIO_OUT( USB_ENDPOINT_READY, ep_ready_mask);
  }
}

/*===========================================================================
This routine disables ready endpoint.
===========================================================================*/
static void usb_ep_disable_ready
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{

  dword ep_ready_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_ready_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_ready_mask = 2 << ep_num*2;
    break;
  }

  /* Clear the ready bit */
#ifdef HWIO_USB_ENDPOINT_READY_CLR_ADDR
  HWIO_OUT(USB_ENDPOINT_READY_CLR, ep_ready_mask);
#else
  /*---------------------------------------------------------------------
     USB_ENDPOINT_READY register toggles, HW can write to it too.
     TDI provided HW fix, see CITS#74 for detail.
  ----------------------------------------------------------------------*/
  HWIO_OUT( USB_ENDPOINT_READY, (ep_ready_mask|0x40000000) );
#endif
}

/*===========================================================================
Sets up the receive buffer and enables DMA
===========================================================================*/
static void usb_setup_rx_DMA
(
  int ep_num,
  byte *rx_buffer,
  int total_byte_count
)
{
  dword *ep_mem_ptr;
  dword temp_dword;

  /* set DWORD3 of EP for ttlbtecnt */
  ep_mem_ptr = usb_ep_get_descriptor_addr(ep_num, USBDC_EP_DIR_OUT);
  temp_dword = *(ep_mem_ptr +3 );
  temp_dword &= ~USBDC_ENDPOINT_TTLBTECNT_M;
  temp_dword += total_byte_count;
  *(ep_mem_ptr +3 ) = temp_dword;

  /* set system mem start addr */
  HWIO_USB_EPn_SYS_MEM_START_ADDR_OUTI((2*ep_num), rx_buffer);

  /* Tell USB core that RX buffer is drained & ready for rxing more data */
  usb_ep_clear_xfilled(ep_num, USBDC_EP_DIR_OUT);
  usb_ep_clear_yfilled(ep_num, USBDC_EP_DIR_OUT);
  usb_ep_clear_xbuf_int_status(ep_num,  USBDC_EP_DIR_OUT);
  usb_ep_clear_ybuf_int_status(ep_num,  USBDC_EP_DIR_OUT);

  /* Enable DMA */
  HWIO_OUT (USB_EP_DMA_ENA,  ( 1 << 2*ep_num ) );

  /* Clear the done status from any previous transfer */
  usb_ep_clear_done_status(ep_num, USBDC_EP_DIR_OUT);

  /* Enable the done interrupt status */
  usb_ep_enable_done_int(ep_num, USBDC_EP_DIR_OUT);

  /* Enable EP */
  usb_ep_enable_ready(ep_num, USBDC_EP_DIR_OUT);

} /* usbdc_setup_rx_DMA */

/*===========================================================================
  Sets up the transmit buffer and enables DMA
===========================================================================*/
static void usb_setup_tx_DMA
(
  int ep_num,
  byte *tx_buffer,
  int total_byte_count
)
{
  dword *ep_mem_ptr;
  dword temp_dword;

  ep_mem_ptr = usb_ep_get_descriptor_addr(ep_num, USBDC_EP_DIR_IN );

  /* There should not an active transfer */
  if ( ((*(ep_mem_ptr +3)) & USBDC_ENDPOINT_TTLBTECNT_M) != 0)
  {
    /* Flush to get back to a known state */
    usb_ep_disable_done_int(ep_num, USBDC_EP_DIR_IN, TRUE);
  }

  /* set DWORD3 of EP for ttlbtecnt */
  temp_dword = *(ep_mem_ptr +3 );
  temp_dword &= ~USBDC_ENDPOINT_TTLBTECNT_M;
  temp_dword += total_byte_count;
  *(ep_mem_ptr +3 ) = temp_dword;

  /* point to the data */
  HWIO_USB_EPn_SYS_MEM_START_ADDR_OUTI(((2*ep_num) + 1),tx_buffer);

  /* enable this DMA transfer */
  HWIO_OUT(USB_EP_DMA_ENA,  ( 2 << 2*ep_num ) );

  /* Clear X and Y filled status bits */
  usb_ep_clear_xfilled(ep_num, USBDC_EP_DIR_IN);
  usb_ep_clear_yfilled(ep_num, USBDC_EP_DIR_IN);
  usb_ep_clear_xbuf_int_status(ep_num,  USBDC_EP_DIR_IN);
  usb_ep_clear_ybuf_int_status(ep_num,  USBDC_EP_DIR_IN);

  /* Enable the done interrupt status */
  usb_ep_enable_done_int(ep_num, USBDC_EP_DIR_IN);

  /* ready EP */
  usb_ep_enable_ready(ep_num, USBDC_EP_DIR_IN);

}  /* usb_setup_tx_DMA */

/*===========================================================================
  Sets the toggle bit
===========================================================================*/
static void usb_ep_set_toggle_bit
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{

  dword ep_toggle_val;
  dword ep_toggle_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_toggle_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_toggle_mask = 2 << ep_num*2;
    break;
  }

  ep_toggle_val = HWIO_IN(USB_ENDPOINT_TOGGLE_BITS);
  if ((ep_toggle_val & ep_toggle_mask) == 0)
  {
    /* Disable endpoint */
    usb_ep_disable_endpoint(ep_num, ep_dir);

    ep_toggle_val |=  ep_toggle_mask;
    HWIO_OUT(USB_ENDPOINT_TOGGLE_BITS, ep_toggle_val);

    /* Enable endpoint */
    usb_ep_enable_endpoint(ep_num, ep_dir);
  }

}

/*===========================================================================
  Clears the toggle bit.
===========================================================================*/
static void usb_ep_clear_toggle_bit
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{

  dword ep_toggle_val;
  dword ep_toggle_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_toggle_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_toggle_mask = 2 << ep_num*2;
    break;
  }

  ep_toggle_val = HWIO_IN(USB_ENDPOINT_TOGGLE_BITS);
  if (ep_toggle_val & ep_toggle_mask)
  {
    /* Disable endpoint */
    //usb_ep_disable_endpoint(ep_num, ep_dir);

    if (ep_toggle_val & ep_toggle_mask)
    {
      HWIO_OUT( USB_ENDPOINT_TOGGLE_BITS, ep_toggle_mask);
    }

    /* Enable endpoint */
    //usb_ep_enable_endpoint(ep_num, ep_dir);
  }
}

/*===========================================================================

  This function clears an end point stall status.

===========================================================================*/
static void usb_ep_clear_stall
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword *ep_mem_ptr;

  ep_mem_ptr = usb_ep_get_descriptor_addr(ep_num,ep_dir );

  (*ep_mem_ptr) &= ~USBDC_ENDPOINT_STALL_M;
}

/*===========================================================================
  Set the Xfilled registor for the bit corresponding to ep_num's IN Endpoint.
  This indicates to HW that EP's Xbuffer has been filled and the function
  controller can begin sending the data on the next IN token.

===========================================================================*/
static void usb_ep_set_xfilled
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{

  dword ep_xfilled_mask = 0;

  ep_xfilled_mask = 2 << ep_num*2;

  if ( !(HWIO_IN(USB_F_X_FILLED_STATUS) & ep_xfilled_mask) )
  { /* note:  This Register toggles when white to it */
    /* write to set only if it is not set yet */
    HWIO_OUT( USB_F_X_FILLED_STATUS, ep_xfilled_mask);
  }
}


/*===========================================================================
  Per TDI OTG-IP data sheet, this would "clear the DMA channel corresponding
  too whichever bit is set and also disable the DMA transfer.  It can be used to
  abort a DMA transfer."   It "also clear the DMA enable bit for the EP".

===========================================================================*/
static void usb_clear_DMA_channel
(
  int ep_num,
  usbdc_ep_direction_type ep_dir
)
{
  dword ep_mask = 0;

  switch (ep_dir)
  {
  case  USBDC_EP_DIR_OUT:
    ep_mask =  1 << (2*ep_num);
    break;

  case USBDC_EP_DIR_IN:
    ep_mask =  2 << (2*ep_num);
    break;
  }
  if ( HWIO_IN(USB_EP_DMA_ENA) & ep_mask)
  {
    HWIO_OUT(USB_EP_DMA_CHANNEL_CLEAR, ep_mask );
  }
}  /* usb_clear_DMA_channel */

/*===========================================================================

  This routine disables EndPoint DONE interrupt associated with an
  endpoint IN or OUT. It will also optionly flush the buffer.

===========================================================================*/
static void usb_ep_disable_done_int
(
  int ep_num,               /* logical EP number */
  usbdc_ep_direction_type ep_dir, /* disable IN, OUT or both directions
                               DONE int of a logic EP             */
  boolean flush_flag
)
{
  dword ep_done_enable_val;
  dword ep_done_mask = 0;
  dword *ep_mem_ptr;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_done_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_done_mask = 2 << ep_num*2;
    break;
  }

  ep_done_enable_val = HWIO_IN(USB_ENDPOINT_DONE_ENA);
  ep_done_enable_val &= ~( ep_done_mask );   /*disable EP DONE INT*/
  HWIO_OUT(USB_ENDPOINT_DONE_ENA, ep_done_enable_val);

  if (flush_flag )
  {
    usb_ep_disable_ready(ep_num,ep_dir);
    usb_ep_clear_done_status(ep_num,ep_dir);

    /* clear total_byte_cnt */
    ep_mem_ptr = usb_ep_get_descriptor_addr(ep_num,ep_dir);
    *(ep_mem_ptr+3) &=  ~USBDC_ENDPOINT_TTLBTECNT_M;

    /* Only clear the DMA channel if byte count has not beene reset */
    if ( (*(ep_mem_ptr+3) &  ~USBDC_ENDPOINT_TTLBTECNT_M) != 0 )
    {
      usb_clear_DMA_channel(ep_num,ep_dir);
    }

    usb_ep_clear_xfilled(ep_num,ep_dir);
    usb_ep_clear_yfilled(ep_num,ep_dir);
  }

  /* clear INT_DONE_STATUS register */
  usb_ep_clear_done_status(ep_num,ep_dir);

} /* usb_ep_disable_done_int */

/*===========================================================================
  Clears the status buffer
===========================================================================*/
static void usb_ep_clear_xbuf_int_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_mask = 2 << ep_num*2;
    break;
  }

  if ( HWIO_IN(USB_F_X_BUFFER_INT_STATUS) & ep_mask )
  {
    HWIO_OUT(USB_F_X_BUFFER_INT_STATUS, ep_mask);
  }
}

/*===========================================================================
  Clears the status buffer
===========================================================================*/
static void usb_ep_clear_ybuf_int_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_mask = 2 << ep_num*2;
    break;
  }

  if ( HWIO_IN(USB_F_Y_BUFFER_INT_STATUS) & ep_mask )
  {
    HWIO_OUT(USB_F_Y_BUFFER_INT_STATUS, ep_mask);
  }
}

/*===========================================================================
   Initiate a 0 lenght packet transfer for the given IN endpoint.

===========================================================================*/
static void usb_send_zero_length_pkt
(
  int ep_num                       /* logic ep umber */
)
{
  dword *ep_mem_ptr;

  ep_mem_ptr = ep_mem_ptr + 1 - 1; /* Dummy operation to make RVCT2.1 happy */

  ep_mem_ptr = usb_ep_get_descriptor_addr(ep_num, USBDC_EP_DIR_IN);

  /* Clear X and Y filled status bits */
  usb_ep_clear_xfilled(ep_num, USBDC_EP_DIR_IN);
  usb_ep_clear_yfilled(ep_num, USBDC_EP_DIR_IN);
  usb_ep_clear_xbuf_int_status(ep_num,  USBDC_EP_DIR_IN);
  usb_ep_clear_ybuf_int_status(ep_num,  USBDC_EP_DIR_IN);

  usb_ep_set_xfilled(ep_num, USBDC_EP_DIR_IN);

  /* Enable the done interrupt status */
  usb_ep_enable_done_int(ep_num, USBDC_EP_DIR_IN);

  /* ready the EP */
  usb_ep_enable_ready(ep_num, USBDC_EP_DIR_IN);
}

/*===========================================================================
  Wait for the DMA hardware to indicate the completion
  of a transfer.
===========================================================================*/
static void usb_wait_for_dma_done
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
)
{
  dword ep_mask = 0;

  switch (ep_dir)
  {
  case  USBDC_EP_DIR_OUT:
    ep_mask =  1 << (2*ep_num);
    break;

  case USBDC_EP_DIR_IN:
    ep_mask =  2 << (2*ep_num);
    break;
  }

  /* Wait for completion */
  while (HWIO_IN(USB_EP_DMA_ENA) & ep_mask)
  {
  }

  /* Clear done condition */
  usb_ep_disable_done_int(ep_num, ep_dir, FALSE);
}

/*===========================================================================

  Processes a received setup request message.

===========================================================================*/

static void usb_do_handle_setup_msgs
(
  void
)
{
  word reply_length;
  byte *reply_data_ptr;
  dword temp_dword;
  dword *buf_dword_ptr = (dword *) HWIO_ADDR(USB_DATA_MEMORY_BASE);
  dword *data_mem_ptr, *ep_mem_ptr;

  /* struct to save the setup request message parameters */
  usbdc_setup_type setup;

  int ep_num;                        /* logic ep umber */
  usbdc_ep_direction_type ep_dir;    /* ep direction, IN or OUT*/

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Assume no data to return */
  reply_length = 0;
  reply_data_ptr = NULL;

  /* Process any pending setup message */
  KICK_WATCHDOG ();

  if (usb_data.setup_data_stage_flag)
  {
    /* Process the data stage of standard usb setup request. So far, only
       SET_LINE_CODING has this stage. */
    usb_data.setup_data_stage_flag = FALSE;

    data_mem_ptr = (dword *) HWIO_ADDR(USB_DATA_MEMORY_BASE);

    *((dword *) &usb_data.line_coding[0]) =
       *(data_mem_ptr++);

    temp_dword = *(data_mem_ptr++);
    usb_data.line_coding[4] = (byte) (temp_dword & 0xff);

    temp_dword = temp_dword >>8;
    usb_data.line_coding[5] = (byte) (temp_dword & 0xff);

    temp_dword = temp_dword >>8;
    usb_data.line_coding[6] =  (byte) (temp_dword & 0xff);
  }
  else
  {
    /* Read in the setup message from EP 0 OUT X buffer */
    temp_dword = *(buf_dword_ptr++);
    setup.bmRequest_and_Type = temp_dword;
    setup.wValue.value = temp_dword >> 16;
    temp_dword = *(buf_dword_ptr++);
    setup.wIndex = temp_dword;
    setup.wLength = temp_dword >> 16;

    /*
    ** For standard device requests that require no Data stage, a device
    ** must be able to complete the request and be able to sucessfully
    ** complete the Status stage of the request within 50 ms of receipt
    ** of the request.
    **/

    switch (setup.bmRequest_and_Type)
    {
    /* Section 6.2.1 Send Encapsulated Command */
    case USBDC_GET_LINE_CODING:
      {
        /* The line coding information is already extracted from the
           usbdc_device structure and stored in the line_coding array. */
        reply_data_ptr = usb_data.line_coding;
        reply_length = setup.wLength;
        break;
      }

    /* Section 6.2.1 Send Encapsulated Command */
    case USBDC_SET_CONTROL_LINE_STATE_I:
      {
        break;
      }

    /* Section 6.2.1 Send Encapsulated Command */
    case USBDC_SET_LINE_CODING:
      {
        /* The set line coding (setup) message transfer includes a data
           packet with all of the line information present. We read the data
           from the next OUT transfer. */
        usb_data.setup_data_stage_flag = TRUE;
        break;
      }  /* end case - USBDC_SET_LINE_CODING */

    case USBDC_SETUP_CLEAR_FEATURE_E:
      {
        /*
         *  A clear_feature endpoint_stall message. Therefore,
         *  clear the stalled endpoint.
         */
        if ( (setup.wIndex & 0x80) == 0)
        {
          ep_dir = USBDC_EP_DIR_OUT;
        }
        else
        {
          ep_dir = USBDC_EP_DIR_IN;
        }
        ep_num = (int)  (setup.wIndex & 0x0f);


        usb_ep_clear_toggle_bit(ep_num, ep_dir);

        usb_ep_clear_stall(ep_num, ep_dir);
        break;
      }

    default:
      /* for now, let's just say that we are happy with any command from
         the host */
      break;
    } /* end of switch (bmRequest_and_Type) */
  }

  usb_ep_clear_xfilled(USBDC_CONTROL_OUT_EP, USBDC_EP_DIR_OUT);
  usb_ep_clear_yfilled(USBDC_CONTROL_OUT_EP, USBDC_EP_DIR_OUT);
  usb_ep_clear_xbuf_int_status(USBDC_CONTROL_OUT_EP,  USBDC_EP_DIR_OUT);
  usb_ep_clear_ybuf_int_status(USBDC_CONTROL_OUT_EP,  USBDC_EP_DIR_OUT);

  /* Clear Setup Message Received */
  ep_mem_ptr = (dword *) HWIO_ADDR(USB_ENDPOINT_MEMORY_BASE);
  *ep_mem_ptr &= ~USBDC_ENDPOINT_SETUP_RXED_M;

  /* Increment by 3 dwords for EP 0 OUT dword3 */
  ep_mem_ptr += 3;

  /* clear TBC before set its value */
  (*ep_mem_ptr) &= ~USBDC_ENDPOINT_TTLBTECNT_M;

  if (usb_data.setup_data_stage_flag)
  {
    *(ep_mem_ptr) += setup.wLength;
  }
  else
  {
    /* Set the transfer count */
     *(ep_mem_ptr) += (USBDC_CONTROL_PKT_SIZE << 1);
  }

  usb_ep_enable_ready(USBDC_CONTROL_OUT_EP, USBDC_EP_DIR_OUT);

  if (usb_data.setup_data_stage_flag == FALSE)
  {
    /* Initiate transfer of data or 0 lenght packet */
    if (reply_length > 0)
    {
      usb_setup_tx_DMA(USBDC_CONTROL_IN_EP,
                         reply_data_ptr,
                         reply_length);
    }
    else
    {
      /* Send 0 lenght packet to complete control transfer */
      usb_send_zero_length_pkt(USBDC_CONTROL_IN_EP);
    }
  }  /* end of--- if(!usbdc_setup_data_stage_flag) */
} /* end of usb_do_handle_setup_msgs() */

/*======================================================================

  Processes interrupt associated with the control IN endpoint.

=======================================================================*/
static void usb_process_ctrl_done
(
  void
)
{
  dword *ep_mem_ptr;
  int num_bytes_leftover;

  ep_mem_ptr = usb_ep_get_descriptor_addr(USBDC_CONTROL_IN_EP,
                                            USBDC_EP_DIR_IN);

  /* read DWORD3 of EP for ttlbtecnt to insure transfer is done */
  num_bytes_leftover =( *(ep_mem_ptr + 3) ) & USBDC_ENDPOINT_TTLBTECNT_M;

  /* Work around for HW anomaly, CITS56b */
  if (num_bytes_leftover > 0)
  {
    return;
  }
  usb_ep_clear_xbuf_int_status(USBDC_CONTROL_IN_EP, USBDC_EP_DIR_IN);
  usb_ep_clear_ybuf_int_status(USBDC_CONTROL_IN_EP, USBDC_EP_DIR_IN);

  /* Write is complete */
  usb_ep_disable_done_int(USBDC_CONTROL_IN_EP,
                            USBDC_EP_DIR_IN,
                            TRUE);

  /* clear_done_staus to eliminate any possible false DONE int */
  usb_ep_clear_done_status(USBDC_CONTROL_IN_EP, USBDC_EP_DIR_IN);

  /* Insure that the next transfer starts with data1 */
  usb_ep_set_toggle_bit(USBDC_CONTROL_IN_EP, USBDC_EP_DIR_IN);

  /* Update address if necessary */
  if ( (HWIO_IN(USB_DEVICE_ADDRESS) == 0)  &&
        (usb_data.address > 0) )
  {
    HWIO_OUT(USB_DEVICE_ADDRESS, usb_data.address);
  }
}

/*==========================================================================
  Processes interrupt associated with the bulk IN endpoint.

===========================================================================*/
static void usb_process_tx_done
(
  int ep_num
)
{
  dword *ep_mem_ptr;
  int num_bytes_leftover;

  ep_mem_ptr = usb_ep_get_descriptor_addr(ep_num, USBDC_EP_DIR_IN);

  /* read DWORD3 of EP for ttlbtecnt to insure transfer is done */
  num_bytes_leftover =( *(ep_mem_ptr + 3) ) & USBDC_ENDPOINT_TTLBTECNT_M;

  /* Work around for HW anomaly, CITS56b */
  if (num_bytes_leftover > 0)
  {
    return;
  }
  usb_ep_clear_xbuf_int_status(ep_num, USBDC_EP_DIR_IN);
  usb_ep_clear_ybuf_int_status(ep_num, USBDC_EP_DIR_IN);

  /* Write is complete */
  usb_ep_disable_done_int(ep_num,
                            USBDC_EP_DIR_IN,
                            FALSE);

  /* clear_done_staus to eliminate any possible false DONE int */
  usb_ep_clear_done_status(ep_num, USBDC_EP_DIR_IN);


} /* usb_process_tx_done */

/*===========================================================================
  This function checks for the assertion of any enabled
  USB function controller interrupt.

===========================================================================*/
void do_usb_check
(
  void
)
{
  word num_bytes_read = 0;
  dword sys_int_status;
  dword which_ep;
  dword ep_mask = 0;
  dword *ep_mem_ptr;
  int i;

  if (HWIO_USB_INT_STATUS_INM(HWIO_USB_INT_STATUS_FUNC_INT_STATUS_BMSK)
      == 0)
  {
    /* No function controller interrupt to process */
    return;
  }
  else
  {
    /* Clear function controller interrupt */
    HWIO_OUT(USB_CORE_INT_STATUS, HWIO_USB_CORE_INT_STATUS_FCINT_BMSK);
  }

  KICK_WATCHDOG ();

  /* Process all function controller interrupts */
  sys_int_status = HWIO_IN(USB_F_SYSTEM_INT_STATUS);

  /*-------------------------------------------------------------------------
    Enumeration support
  -------------------------------------------------------------------------*/
  /* Check for cable pull (suspend) */
  if (sys_int_status & USBDC_F_INT_SUSPEND_M)
  {
    /* indicate to HW that we get suspend */
    HWIO_OUT(USB_FUNCTION_CMD_STATUS,
             HWIO_USB_FUNCTION_CMD_STATUS_SUSPDET_BMSK);

    /* Clear the suspend and resume interrupt status */
    HWIO_OUT(USB_F_SYSTEM_INT_STATUS,
             USBDC_F_INT_SUSPEND_M | USBDC_F_INT_RESET_M);

    sys_int_status = HWIO_IN(USB_F_SYSTEM_INT_STATUS);
  }

  if (sys_int_status & USBDC_F_INT_DONE_M)
  {
    /* which EP is done */
    which_ep = HWIO_IN(USB_F_ENDPOINT_DONE_STATUS);

    /* only serve and clear those be enabled */
    which_ep &= HWIO_IN( USB_ENDPOINT_DONE_ENA);

    /* clear out the EP bits that has been set */
    HWIO_OUT( USB_F_ENDPOINT_DONE_STATUS, which_ep );

    /*------------------------------------------------------------
      Clear interrupt after USB_F_ENDPOINT_DONE_STATUS is cleared
    -------------------------------------------------------------*/
    HWIO_OUT( USB_F_SYSTEM_INT_STATUS,  USBDC_F_INT_DONE_M);

    if ( which_ep & USBDC_CONTROL_IN_EP_M)
    {
      /* EP0 IN done */
      usb_process_ctrl_done();
    }

    if (which_ep & USBDC_CONTROL_OUT_EP_M)
    {
      /* EP0 OUT done */

      /* Put the IN endpoint in a known state */
      usb_ep_disable_done_int(USBDC_CONTROL_IN_EP,
                                   USBDC_EP_DIR_IN,
                                   TRUE);
      /* Process setup message */
      usb_do_handle_setup_msgs();
    }

    /* Check for completion of IN bulk transfer */
    ep_mask =  2 << (2*usb_data.usb_in_endpoint);
    if (which_ep & ep_mask)
    {
      usb_process_tx_done(usb_data.usb_in_endpoint);
      wait_for_tx_done = FALSE;
    }

    /* Check for completion of OUT bulk transfer */
    ep_mask =  1 << (2*usb_data.usb_out_endpoint);
    if (which_ep & ep_mask)
    {
      /* save pointer to buffer which has current data just received */
      process_rx_buffer = current_rx_buffer;
    }

    if (wait_for_tx_done)
    {
      return;
    }

    if (process_rx_buffer)
    {
      /* Determine data received count */

      /* Get dword3 */
      ep_mem_ptr = usb_ep_get_descriptor_addr(
           usb_data.usb_out_endpoint,
           USBDC_EP_DIR_OUT);

     /* Calculate bytes received */
      num_bytes_read = USB_RX_BUFFER_SIZE -
        ((word) (*(ep_mem_ptr +3) & USBDC_ENDPOINT_TTLBTECNT_M));

      /* Setup a new transfer in the other buffer.  This allows us to
       * process the most recently received buffer while the DMA brings
       * in more data, thereby increasing processing speed and decreasing
       * latency.
       */

      /* If num bytes read is zero - we received a ZLP - we need to setup the
       * DMA again.
       */
      if (num_bytes_read > 0)
      {
        if (current_rx_buffer == usb_data.rx_buffer1)
        {
          current_rx_buffer = usb_data.rx_buffer2;
        }
        else
        {
          current_rx_buffer = usb_data.rx_buffer1;
        }
      }

      usb_setup_rx_DMA(USBDC_DIAG_STREAM_OUT_EP,
                            current_rx_buffer,
                            USB_RX_BUFFER_SIZE);
    }
  }

  /* If we have received the data from the host, then we just
   * pass it to the packet module for further processing.
   */

  if(num_bytes_read)
  {
     /* We have received all the bytes in the receive buffer. Now it is time to
      * pass these characters to the packet module for further processing.
      */
     for(i = 0; i < num_bytes_read; i++)
     {
       /* The data is available in the rx_buffer. So, we pass the next byte
        * based on the rx_index to the packet process function.
        */
       packet_handle_incoming ((int) (process_rx_buffer[usb_data.rx_index++]));
     }

     /* Set index back to zero for next transfer */
     usb_data.rx_index = 0;

     process_rx_buffer = 0;
  }

} /* end of do_usb_check() */


/*===========================================================================

  This function buffers a byte into USB tx buffer. If the end of message
  (0x7E) is received from the packet module, then this function will send
  the data.

===========================================================================*/
void usb_transmit_byte
(
  byte chr
)
{
  /* Copy the byte to the trasmit buffer. */
  usb_data.tx_buffer[usb_data.tx_buf_index++] = chr;

  /* Check for terminating character */
  if ((chr == 0x7E) && (usb_data.tx_buf_index != 1))
  {
    KICK_WATCHDOG ();

    /* Send this response */
    usb_setup_tx_DMA(usb_data.usb_in_endpoint,
                          usb_data.tx_buffer,
                          usb_data.tx_buf_index);

    /* Reset Index */
    usb_data.tx_buf_index = 0;

    /* Wait for transfer to complete */
    wait_for_tx_done = TRUE;

    while (wait_for_tx_done)
    {
      /* Delay */
      do_usb_check();
    }
  }
} /* end of usb_transmit_byte() */

/*===========================================================================
  This function waits for the IN transfer to complete.
  This allows the caller to be sure that all
  characters are transmitted.

===========================================================================*/

void usb_drain
(
  void
)
{
  /* Wait for transfer to complete */
  usb_wait_for_dma_done(usb_data.usb_in_endpoint,
                             USBDC_EP_DIR_IN);

} /* end of usb_drain() */

/*===========================================================================

  This function is a stub

===========================================================================*/

boolean
usb_active(void)
{

  KICK_WATCHDOG ();

  /* If we called usb_init() and returned TRUE, which is the only way we
   * call this function, we are guaranteed to have data, so just return
   * TRUE.
   */
  return TRUE;

}

/*===========================================================================
  Check the USB_F_ENDPOINT_DONE_STATUS register corresponding to the
  ep_num's ep_dir EndPoint.

===========================================================================*/
static boolean usb_ep_done_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
)
{
  dword ep_done_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_done_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_done_mask = 2 << ep_num*2;
    break;
  }

  /* get DONE_STATUS for this endpoint */
  if ( HWIO_IN(USB_F_ENDPOINT_DONE_STATUS) & ep_done_mask )
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

  This function initializes the MSM USB device controller with no
  interrupts enabled but both receiver and transmitter running.

===========================================================================*/

boolean usb_init
(
  void
)
{
  int i = 0;
  dword *ep_mem_ptr;


  FLASHLOG(1,("usb_init: entry\n"));

  /* Check if USB is configured */
  if (HWIO_IN(USB_DEVICE_ADDRESS) == 0)
  {
    FLASHLOG(1,("usb_init: NO USB exit\n"));

    /* No, don't touch the interface at all */
    return FALSE;
  }


  /* USB is configured, set up the hardware */

  usb_data.tx_buf_index = 0;
  usb_data.rx_index = 0;
  usb_data.rx_num_bytes_read = 0;
  usb_data.setup_data_stage_flag = FALSE;
  usb_data.address = 0;
  for (i = 0; i < USB_RX_BUFFER_SIZE; i++)
  {
    usb_data.rx_buffer1[i] = 0;
    usb_data.rx_buffer2[i] = 0;
  }
  /* Init line coding data */
  hostdl_memcpy(usb_data.line_coding,
    (byte*) &(default_serial_info.data_rate), sizeof(dword));
  usb_data.line_coding[4] = default_serial_info.stop_bits;
  usb_data.line_coding[5] = default_serial_info.parity;
  usb_data.line_coding[6] = default_serial_info.data_bits;


  /* Indentify endpoints */
  ep_mem_ptr = usb_ep_get_descriptor_addr(USBDC_DIAG_STREAM_OUT_EP,
                                          USBDC_EP_DIR_OUT);

  /* Check for active endpoint */
  if ( (word) (*(ep_mem_ptr +3) & USBDC_ENDPOINT_TTLBTECNT_M) )
  {
    usb_data.usb_out_endpoint = USBDC_DIAG_STREAM_OUT_EP;
    usb_data.usb_in_endpoint  = USBDC_DIAG_STREAM_IN_EP;
  }
  else
  {
    usb_data.usb_out_endpoint = USBDC_DATA_STREAM_OUT_EP;
    usb_data.usb_in_endpoint  = USBDC_DATA_STREAM_IN_EP;
  }

  /* Wait for an in progress transfer fron host to complete */
  while (usb_ep_done_status(usb_data.usb_out_endpoint, USBDC_EP_DIR_OUT)
      == FALSE)
  {
  }

  FLASHLOG(1,("usb_init: b4 DMA setup\n"));

  /* Setup to receive data from the host */
  usb_ep_enable_done_int(USBDC_CONTROL_OUT_EP, USBDC_EP_DIR_OUT);
  current_rx_buffer = usb_data.rx_buffer1;
  usb_setup_rx_DMA(USBDC_DIAG_STREAM_OUT_EP,
                        current_rx_buffer,
                        USB_RX_BUFFER_SIZE);

  FLASHLOG(1,("usb_init: hae USB exit\n"));

  return TRUE;

} /* end of usb_init() */
