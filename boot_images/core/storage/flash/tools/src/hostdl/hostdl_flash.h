#ifndef HOSTDL_FLASH_H
#define HOSTDL_FLASH_H

/*==================================================================
 *
 * FILE:        hostdl_flash.h
 *
 * SERVICES:    Flash programmer public interface header file
 *
 * DESCRIPTION:
 *    This file contains the interface to all flash devices and is not
 *    flash type specific (NOR or NAND).  It is the high level "public"
 *    interface to the flash device driver layer.
 *
 * Copyright (c) 2008-2010, 2013 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl_flash.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 02/15/13     sv      Adding Erase NAND capability in EDL/QPST download
 * 10/14/10     sv      Modify flash_device_sector to get buf size parameter  
 * 10/29/08     mm      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "comdef.h"

//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------
/* For Probe table */
#define FLASH_PROBE_END 0xDEADBEEF


//--------------------------------------------------------------------------
// Type Declarations
//--------------------------------------------------------------------------

/* Enumerated type to see if device probed OK so we only probe once */
typedef enum
{
  FLASH_DEVICE_FOUND,         /* Successful probe             */
  FLASH_DEVICE_UNKNOWN,       /* Failed to probe successfully */
  FLASH_DEVICE_NOT_PROBED     /* Not yet probed               */
}
probe_code_type;


/* Public definition for dispatch table structure containing
 * all pertinent information about a particular flash device.
 * Detailed definition is in flash private header file.
 */

struct flash_device_data;
typedef struct flash_device_data *flash_device_t;


/* The Flash device is, of course, memory mapped, so the compiler might
   find the various device manipulations to be nonsense, and optimize
   it all away.  To prevent this, we use pointers to volatile to let
   the compiler know that we're dealing with a special device and that
   it is to leave the operations as written. */

typedef volatile word *flash_ptr_type;


/* Definition of probe table structure and pointer type.  Defined in
 * MSM specific code and initialized to tell the flash layer where
 * to probe with what function.
 */

typedef struct{
  volatile word *probe_addr;
  flash_device_t (*probe_fcn) (flash_ptr_type baseaddr);
}probe_tbl;

/* pointer type to Probe Table Structure */
typedef probe_tbl *probe_tbl_t;


//--------------------------------------------------------------------------
// Extern Definitions
//--------------------------------------------------------------------------
/* enum to indicate status of probe */
extern probe_code_type flash_dev_type;


//--------------------------------------------------------------------------
// Function Definitions
//--------------------------------------------------------------------------

/*===========================================================================

  This function writes a provided string of bytes into a specified
  block of memory.

  If the block is in RAM space, the only restriction is that the
  write is not permitted to overlay this program.

  If the block is in Flash space, the write will be attempted.  The
  updated memory will then be verified to confirm that the write
  operation succeeded.  It might fail because the area had not been
  erased, or because it has worn out, or because it is in the
  hardware-lockable boot block.

===========================================================================*/

response_code_type do_simple_write (byte * buf, dword addr, word len);


/*===========================================================================

  This function writes a provided string of bytes into a specified
  block of memory and handles erase of any blocks as necessary
  before writing.


  If the block is in Flash space, the write will be attempted.  The
  updated memory will then be verified to confirm that the write
  operation succeeded.  It might fail because the area had not been
  erased, or because it has worn out, or because it is in the
  hardware-lockable boot block.

===========================================================================*/

response_code_type do_stream_write (byte * buf, dword addr, word len);


/*===========================================================================

  This function reads string of bytes into a specified
  block of memory.

===========================================================================*/

response_code_type do_read (byte * buf, dword addr, word len);

#ifdef ADD_LATER_FOR_COMMON_WITH_JFLASH
/*===========================================================================

  This function erases a specified block of flash memory.

  If the block is in RAM space, the only restriction is that the
  erased block is not permitted to overlay this program.  The RAM
  is cleared to 0.

  The memory address and block size must exactly match the address
  and size of one of the blocks in the Flash memory device.  The Flash
  memory is erased to a device-specific value, in this case 0xFFFF.

===========================================================================*/

response_code_type do_erase (dword addr, dword len);
#endif


/*===========================================================================

  This function returns a code indicating how large the Flash device is.

===========================================================================*/

byte flash_device_size (void);


/*===========================================================================

  This function returns a pointer to a string containing the flash
  device name

===========================================================================*/

char * flash_device_name (void);


/*===========================================================================

  This function fills the indicated handle_hello response buffer with the
  number of sectors, and a length for each sector.

===========================================================================*/

int flash_device_sectors (uint8 *buf, uint32 buf_size);



/*===========================================================================

  This function does one time initialization of data structures used in
  flash device access

===========================================================================*/

void flash_device_init_data (void);


/*===========================================================================

  This function calls the flash device specific function to probe the
  flash

===========================================================================*/

void flash_device_probe (void);


/*===========================================================================

  This function is a stub for a function needed in the next revision of
  the protocol.

===========================================================================*/

response_code_type do_open (byte mode);


/*===========================================================================

    This function handles the NAND erase.

===========================================================================*/

response_code_type do_erase (void);

/*===========================================================================

  This function is a stub for a function needed in the next revision of
  the protocol.

===========================================================================*/

response_code_type do_close (void);

/*===========================================================================

  This function Sends the security mode to the layer below

===========================================================================*/
response_code_type do_security_mode (byte mode);


/*===========================================================================

  This function Sends the partition table to the layer below

===========================================================================*/
response_code_type do_partition_table (byte *data, byte override, word len);


/*===========================================================================

  This function handles the open for all modes.

===========================================================================*/
response_code_type do_open_multi (byte *data, byte mode, uint32 length);

flash_device_t spansion_probe(flash_ptr_type baseaddr);


#endif /* HOSTDL_FLASH_H */
