#ifndef NOR_PARAM_H
#define NOR_PARAM_H
/*===========================================================================

         Flash Programming Plug-In for Lauterbach JTAG/ICD TRACE32

DESCRIPTION
  This header file contains the struct that is used to communicate 
  between our plug-in and Lauterbach TRACE32 Debugger. Also contains
  generic types.

  Copyright (c) 2010-2011 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/06/11   jz      Added FLASH_PROG_READ_PLATFORM_ID, reduced FLASH_PROG_BUFFER_SIZE
09/03/10   sv      Initial version 
===========================================================================*/

#include "DALStdDef.h"
extern struct flash_prog_param prog_param;

/* The following value is based on max 32KB block size
 * for currently supported NOR devices (SPI NOR, EEPROM)
 */
#define FLASH_PROG_BUFFER_SIZE          0x8000

/* 512 bytes each for System and User partition tables */
#define FLASH_PARTI_BSIZE               (512 * 2) 
#define FLASH_MAX_FNAME_BSIZE           40
#define FLASH_MAX_PATH_BSIZE            1024

/* This structure is used to communcate between TRACE32 Debugger and
 * our FLASH plug-in.
 */
struct flash_prog_param {
   uint32 flashaddr;
   uint32 flashwidth;
   uint32 width;
   uint32 offset;      
   uint32 addr;        // in: address of the flash to write/erase
   uint32 size;        // in: size of data to write/erase
   uint32 reserved;
   uint32 status;      // in: 1=prog., 2=erase, 5=erase, out: 0=o.k., 0x100=fail
   byte   data[FLASH_PROG_BUFFER_SIZE];   // give 128K worth of buffer
                                          // this is where T32 will store data
                                          // that we'll be writing to the FLASH
};

/* This structure is used to communcate from our program to T32
 * in an extended fashion for post processing statistics.  It
 * overlays the data area of jtag_flash_param and presents
 * a different interface */
struct type_jtag_report {
   uint32 input_gap_size;
   uint32 bad_block_gap_size;
   uint32 image_size;
   uint32 reserved1;      
   uint32 reserved2;
   uint32 reserved3;
   uint32 reserved4;
   uint32 status;           // out: 0=o.k., 0x100=fail
};



 /*===========================================================================

                     MACRO DEFINITIONS FOR MODULE

===========================================================================*/

#define SET_ERR(x) prog_param.reserved=((uint32)(x))

/* command code from type_jtag_flash_param.status */

/* General command code - Used always */
#define FLASH_PROG_PROGRAM            0x00000001
#define FLASH_PROG_INITIALIZE         0x00000022
#define FLASH_PROG_FINALIZE           0x00000023
#define FLASH_PROG_ERASE_CHIP         0x00000002

/* Multi-Image only */
#define FLASH_PROG_INIT_USR_PARTI     0x00000020
#define FLASH_PROG_INIT_SYS_PARTI     0x00000020


#define FLASH_PROG_STRESS_TEST        0x00000046
#define FLASH_PROG_ERASE_BLOCK        0x00000047
#define FLASH_PROG_READ_PAGE          0x00000048
#define FLASH_PROG_FILL_PAGE          0x00000049
#define FLASH_PROG_DUMP_INIT          0x0000004A
#define FLASH_PROG_PARTI_DUMP         0x0000004B
#define FLASH_PROG_READ_PLATFORM_ID   0x0000004C

#endif  /* NOR_PARAM_H */
