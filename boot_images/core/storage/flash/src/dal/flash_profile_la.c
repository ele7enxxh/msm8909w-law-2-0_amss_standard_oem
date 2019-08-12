/*====================================================================
 *
 * FILE: flash_profile_la.c
 *
 * SERVICES: Functions for LA based profiling
 *
 * DESCRIPTION: This file contain initialization and interface APIs
 *              of flash config data, which is used across all NOR
 *              devices
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_profile_la.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/03/12     sv     Update start, end and init API's
 * 02/02/11     bb     Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_profile_la.h"


/* This is the global pointer to the desired port */
volatile uint16 *flash_la_port_ptr;

uint32 flash_la_flag = 0;
uint32 flash_la_npages = 0;

static void flash_la_port_init(void)
{
#ifdef FEATURE_FPGA_DAL
  /* Get port address from FPGA DAL */
  DalDeviceHandle * fpga_device;
  uint32 la_port_addr = 0;

  if (DAL_DeviceAttach(DALDEVICEID_FPGA, &fpga_device) == DAL_SUCCESS)
  {
    if (fpga_device != NULL)
    {
      (void)DalFPGA_ClearLEDMask(fpga_device, (uint32)0xFF);
    }

    DalFPGA_GetAddress(fpga_device, "HWIO_HAPPY_LED_ADDR", &la_port_addr);
    flash_la_port_ptr = (volatile uint16 *)la_port_addr;
  }
#else
  flash_la_port_ptr = (volatile uint16 *)FLASH_LA_PORT;
#endif
}

void flash_la_init(void)
{
  flash_la_port_init();

  /* Initialize all the markers to low(0) */
  FLASH_LA_MARKER_INIT(0);
}	


/* Marks the start of a marker in driver */
void flash_la_start(uint8 id , uint32 npages)
{
  FLASH_LA_MARKER_HIGH(id);
  flash_la_npages = npages;
}

/* Marks the end of a marker in driver */
void flash_la_end(uint8 id)
{
  FLASH_LA_MARKER_LOW(id);
}

/* Sets the page size and other info required in post processing */
void flash_la_set_info(flash_client_ctxt *client_ctxt )
{
}

/* Sync lock  for multipage support */
void flash_la_lock()
{
}

/* Sync lock  for multipage support */
void flash_la_unlock()
{
}
