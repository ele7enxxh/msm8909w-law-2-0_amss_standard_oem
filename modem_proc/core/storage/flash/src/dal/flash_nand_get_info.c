/**********************************************************************
 * flash_nand_get_info.c
 *
 * SERVICES:    Provides flash operations API's 
 *
 * DESCRIPTION: Provides flash operations API's 
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===================================================================
 *
 *                   EDIT HISTORY FOR MODULE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_nand_get_info.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 * when         who     what, where, why
 * --------     ---     ----------------------------------------------
 * 10/15/12     sb      Initial Version
 *===================================================================*/

 /*===================================================================

===========================================================================*/

#include "flash_nand_get_info.h"
#include "flash_nand_partition.h"

/* Flash operation to get MIBIB partition table info */
void flash_ops_get_parti_tbl (uint32 *buffer, uint32 *size_in_bytes)
{
  struct nand_parti_table *nand_parti_table_ptr = &nand_partition_table;
  
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
}
