#ifndef NOR_CORE_H
#define NOR_CORE_H

/*=======================================================================
 * FILE:        nor_core.h
 *
 * SERVICES:    Flash Programming Plug-In for Lauterbach FLASH_PROG/ICD TRACE32
 *
 * DESCRIPTION
 *   This file has the main functions used for programming the flash
 *
 * Copyright(c) 2010 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/nor_core.h#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ 
 *   $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/02/10     sv      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_prog.h"
#include "flash.h"
#include "DALDeviceId.h"
#include "nor_param.h"
#include "flash_log.h"
#include "nor_config.h"
#include "flash_nor_partition_tbl.h"

#define ALIGN_VAL 32
#define MAX_NOR_BLOCKS 512

/* Structure for organizing a table of block boundaries within the
 * Flash device.  */
typedef struct
{
  uint32 start;                  /* First address of the block */
  uint32 len;                    /* Length of the block in bytes */
  uint16  is_erased;             /* Flag: TRUE if block is erased */
} parti_block_descriptor_type;


/* Structure to store information about the flash device. */
struct nor_program_info
{
  flash_handle_t  handle;
  struct flash_info  nor_info;
  struct flash_nor_block_erase_cfg_info erase_cfg_info;
  struct flash_nor_erase_region_info erase_region_info;
  uint32 device_size;
  uint32 partition_size;
  parti_block_descriptor_type parti_block_info[MAX_NOR_BLOCKS];
};

uint32 flash_prog_init_for_partition_image (unsigned char *pname);
uint32 flash_prog_program_img (void);
uint32 flash_prog_init_image (void);
uint32 flash_prog_erase_chip (void);
uint32 flash_prog_get_flash_info (void);
uint32 flash_prog_update_device_info (void);
uint32 flash_prog_write_data(uint32 addr, uint32 size, uint8 *data_buf);
uint32 flash_prog_dump_parti(void);
uint32 flash_prog_init_parti_dump(void);

#endif /* #ifndef NOR_CORE_H */
