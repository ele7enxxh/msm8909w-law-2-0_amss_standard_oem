/**********************************************************************
 * flash_boot_ops_setup.c
 *
 * SERVICES:    Provides flash operations API's used by Bootloader
 *
 * DESCRIPTION: Provides flash operations API's used by Bootloader
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2011-2012 by QUALCOMM Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===================================================================
 *
 *                   EDIT HISTORY FOR MODULE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_boot_ops_setup.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * --------     ---     ----------------------------------------------
 * 10/25/12     sb      Add support to share NAND info via SMEM
 * 07/01/11     eo      Initial Version
 *===================================================================*/

 /*===================================================================

===========================================================================*/

#include "flash_boot_ops_setup.h"
#include "flash_nand_partition.h"
#include "flash_nand.h"
#include "smem.h"
#include "smem_v.h"


/* NAND info to share via SMEM */
static struct nand_dev_smem_info *smem_flash_dev_info;

/*
 * Request for smem allocation for flash usage
 */
void *flash_smem_alloc(smem_mem_type smem_type,uint32 buf_size)
{
  return smem_alloc(smem_type, buf_size);
}

/* Flash boot operations update SMEM with MIBIB partition table */
void flash_boot_ops_get_parti_tbl(uint32 *buffer, uint32 *size_in_bytes)
{
  struct nand_parti_table *nand_parti_table_ptr = &nand_partition_table;
  uint32 smem_flash_dev_info_size = sizeof(*smem_flash_dev_info);

  if ((buffer == NULL) || (nand_parti_table_ptr->parti_table == NULL) ||
    (nand_parti_table_ptr->parti_table_size_bytes > *size_in_bytes))
  {
    *size_in_bytes = 0;
  }
  else 
  {
    *size_in_bytes = nand_parti_table_ptr->parti_table_size_bytes;
    DALSYS_memcpy(buffer, (const void *)nand_parti_table_ptr->parti_table, 
      *size_in_bytes);
  } 

  /* Allocate SMEM for flash usage */
  smem_flash_dev_info = flash_smem_alloc( SMEM_FLASH_DEVICE_INFO,
    smem_flash_dev_info_size );

  if (NULL != smem_flash_dev_info)
  {
    DALSYS_memcpy((uint32 *)smem_flash_dev_info, (const void *) &nand_dev_info, 
      smem_flash_dev_info_size);
  }
}
