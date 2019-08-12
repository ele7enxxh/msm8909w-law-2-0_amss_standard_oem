/*==================================================================
 *
 * FILE:        hostdl.c
 *
 * SERVICES:    main (C) program for the DMSS Flash Programmer
 *
 * DESCRIPTION:
 *    This module is the main (C) program for the DMSS Flash Programmer
 *    Utility.  This utility is a standalone module intended to be
 *    downloaded into the RAM of a phone with Flash EPROM program memory.
 *    The utility conforms to the DMSS Async Download Protocol.
 *
 *
 * Copyright (c) 2008-2010, 2012, 2014-2015 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/28/15     sb      Remove KW Banned Functions
 * 09/30/14     sb      Configure GPIOs for NAND interface
 * 09/17/12     eo      Temporary disable USE_USB_ONLY support
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 07/12/10     jz      Added GPIO config for interleaved support
 * 11/09/09     jz      Cleaned up USE_USB_ONLY flag and removed uart_loopback()
 * 05/08/09     sv      Enable crc check flag if UART
 * 04/23/09     sv      Fix compilation issue with PRINT LOG
 * 10/29/08     mm      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "hostdl.h"
#include "hostdl_config.h"
#include "hostdl_startup.h"
#include "hostdl_flash.h"
#include "hostdl_log.h"

#include "stringl/stringl.h"

/* Global Data */

/* Pointer to UART/USB function dispatch table */
DISPPTR disptbl;


/* System mode stacks
 *  These are defined as dword and therefore use 1/4 the number
 *  of stack elements, hence the divide by 4.  The defines for
 *  the size are left the same in the header file.
 *  Defining them as dword prevents unaligned access
 */
dword svc_stack[SVC_Stack_Size/4];
dword irq_stack[IRQ_Stack_Size/4];
dword abort_stack[Abort_Stack_Size/4];
dword undef_stack[Undef_Stack_Size/4];

/* Extern Definitions */
/* Dispatch tables for transports.*/
#ifndef USE_USB_ONLY
  extern DISP uart_dispatch_table;
#endif

#ifndef USE_NO_TRANSPORT
  extern DISP usb_dispatch_table;
#endif

/* crc enable/disable flag */
extern dword enable_crc_check;

/* Function Prototypes */
#ifndef USE_NO_TRANSPORT
void init_interface_to_host (void);
#endif

/* Workaround for RVCT 4.0 hostdl/ehostdl failures */
void* hostdl_memcpy(void *out, const void *in, uint32 n)
{
  uint32 s_addr, d_addr;

  s_addr = (uint32) out;
  d_addr = (uint32) in;

  /* Use byte copy if the addresses are not 4-byte alined */
  if ((s_addr & 0x3) || (d_addr & 0x3))
  {
    uint32 i;

    for (i=0; i<n; ++i)
    {
      *((char*)out+i) = *((char*)in+i);
    }
  }
  else
  {
    memscpy(out, n, in, n);
  }

  return out;
}

/* Function Declarations */

/*===========================================================================
  This function is the main (C) routine for the DMSS Flash Downloader
  Utility.  It initializes the UART and calls the packet processor to
  execute the protocol.

  This function is called main_c rather than main because the name
  main is special to the compiler.  It tries to bring in the C runtime
  library.

===========================================================================*/

void main_c (void)
{

  FLASHLOG(1,("main_c: enter\n"));

  /* This function sets PS HOLD to warm reset */
  hostdl_pmic_init();

  /* This function sets TLMM for NAND GPIOs */
  hostdl_gpio_init();

  /*  Do any one time only init of data used to access flash devices.
   *  This init is on data that is independent of the type of flash
   *  eventually found during the probe sequence.
   */
  FLASHLOG(1,("main_c: flash_init_data\n"));
  flash_device_init_data ();

  KICK_WATCHDOG ();

#ifndef USE_NO_TRANSPORT
  /* initializes the UART interface and identifies the interface used by
   * the host to perform the software download in to the MSM (UART/USB)
   */
  FLASHLOG(1,("main_c: init_interface_to_host\n"));
  init_interface_to_host ();
#endif

  packet_init ();               // Initialize the packet module.

#ifndef USE_NO_TRANSPORT
  /*  Stuff a packet and handle commands - never returns */
  FLASHLOG(1,("main_c: packet loop\n"));
  packet_loop ();
#else
  FLASHLOG(1,("main_c: looping forever kicking dog\n"));
  while(1)
  {
    KICK_WATCHDOG();
    /* Temp change: comment out for now */
    //mdelay(0xFFFF);

  }
#endif /* USE_NO_TRANSPORT */
}


#ifndef USE_NO_TRANSPORT
/*===========================================================================

  This function does the required hardware initialization and identifies the
  interface used by the host to download the software into the MSM. It uses
  the functions provided by the UART/USB modules to do all the
  necessary checks on the UART/USB status registers.

  The dispatch table pointer for the proper transport interface (UART/USB)
  is set up.
===========================================================================*/

void init_interface_to_host (void)
{
#ifndef USE_UART_ONLY
  boolean usb_running = FALSE;

  /*
   * Call USB init function which may return TRUE or FALSE.  If USB is
   * running, it is because DLOAD used it to download this binary to the
   * target, just use it, do not even call any UART functions.
   */
  usb_running = (usb_dispatch_table.init) ();
  if (usb_running == TRUE)
  {
    /* Wait here until USB becomes active with characters */
    while ((usb_dispatch_table.active) () == FALSE);
    disptbl = &usb_dispatch_table;
    return;
  }
#endif

#ifndef USE_USB_ONLY
  /*
   * If we get here, either there is no USB interface, or it is not in
   * use.  Call UART init function.  No need to check return code as it
   * is always TRUE.  Transport will always be UART if we get here.
   */
  (void)(uart_dispatch_table.init) ();
  disptbl = &uart_dispatch_table;

  /* Enable CRC check flag for UART */
  enable_crc_check = 1;

#endif /* !USE_USB_ONLY */

  return;
}
#endif /* USE_NO_TRANSPORT */
