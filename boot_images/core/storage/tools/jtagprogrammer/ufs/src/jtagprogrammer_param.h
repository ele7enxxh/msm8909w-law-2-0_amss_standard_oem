#ifndef _JSDCC_PARAM_H
#define _JSDCC_PARAM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


             SDCC Programming Support via Lauterbach JTAG ICD

GENERAL DESCRIPTION
  This file exist so that we can explicitly place our jtag_prog_param
  data in memory since ARM doesn't allow this within a file but rather by
  file basis.

 Copyright (c) 2007-2010 Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/jtagprogrammer/ufs/src/jtagprogrammer_param.h#1 $ 
  $DateTime: 2015/09/01 00:30:35 $ 
  $Author: pwbldsvc $


when         who     what, where, why
----------   ---     ----------------------------------------------------------
2012-04-12   ah      Added enable_hw_reset() feature
2012-02-06   ah      Maximum buffer is now 64KB to avoid going over IMEM
2011-02-03   ab      Added sector erase and CRC32 commands
2010-02-18   rh      Adding support for RX signal quality test
2010-01-20   rh      Adding function to set active boot partition
2009-04-21   rh      Clean up and ported for JTAG SDCC programming
2008-06-18   vtw     Initial version, ported from mjnand.

===========================================================================*/


#include "comdef.h"

// This structure is used to communcate between TRACE32 Debugger and our FLASH plug-in.

struct struct_jtag_prog_param {
   uint32 mod_addr;           // in: Flash module address
   uint32 reserved;           // in: Not used
   uint32 width;              // in: Data width - Not used
   uint32 offset;             // in: Not used
   uint32 addr;               // in: address of the datacard to write/erase
   uint32 data_length;        // in: size of data to write/erase
   uint32 timing;             // in: Clock frequency - Not used
   uint32 cmd_status;         // in: 1=program, 2=erase, 5=erase, out: 0=o.k., 0x100=fail
   byte   data[PARAM_BUFFER_SIZE];  // give 32K worth of buffer - defined in sconscript
                              // this is where TRACE32 will store data that
                              // we'll be writing to the datacard
};

// command code from struct_jtag_prog_param.cmd
#define JTAG_CMD_PROGRAM               0x00000001
#define JTAG_CMD_ERASE                 0x00000002
#define JTAG_CMD_CHIP_ERASE            0x00000005
#define JTAG_CMD_READ_SECTOR           0x00000009
#define JTAG_CMD_WRITE_SECTOR          0x0000000A
#define JTAG_CMD_ENABLE_HW_RESET       0x0000000E
#define JTAG_CMD_PRINT_INFO            0x00000020
#define JTAG_CMD_SET_HW_PARTITION      0x00000031
#define JTAG_CMD_SET_ACTIVE_BOOT       0x00000035
#define JTAG_CMD_PROG_EXTRAS           0x00000036
#define JTAG_CMD_ERASE_GENERIC         0x00000040
#define JTAG_CMD_COMPUTE_CRC           0x00000041

// return code for type_jtag_flash_param.status
#define STATUS_SUCCESS                 0
#define STATUS_FAIL                    0x00000100
#define STATUS_NOT_IMPLEMENTED         0x00000100

#endif  /* _JSDCC_PARAM_H */

