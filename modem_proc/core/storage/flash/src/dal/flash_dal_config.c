/*=============================================================================
 *
 * FILE:      flash_dal_config.c
 *
 * DESCRIPTION: Functions supporting flash DAL driver configuration
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_dal_config.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/18/13     eo      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "comdef.h"
#include "smem.h"
#include "DALStdErr.h"
#include "DALFramework.h"

#include "flash_dal.h"
#include "flash_mibib.h"
#include "flash_miparti.h"
#include "flash_dal_config.h"

#include <string.h>

/***********************************************************************
 *   Data structures
 ***********************************************************************/
 
/* Holds the flash driver configuration */
struct flash_dal_drv_config flash_dal_config =
{
  FLASH_DAL_CONFIG_UNKNOWN,
  NULL
};

/***********************************************************************
 *    Functions
 ***********************************************************************/
/*
 * This API checks in shared memory for a valid partition table.
*/
static int flash_dal_check_smem_parti(void)
{
  uint32 smem_parti_buf_size = 0;
  flash_partable_t parti_ptr;
  flash_partable_t smem_aarm_partition_table;
    
  /* Check shared memory partition buffer has been initialized */
  smem_aarm_partition_table = smem_get_addr(SMEM_AARM_PARTITION_TABLE,
                                &smem_parti_buf_size);
  if ((smem_aarm_partition_table == NULL) ||
      (smem_parti_buf_size == 0))
  {
    flash_dal_config.config = FLASH_DAL_CONFIG_FLASHLESS_BOOT;
    return FLASH_DEVICE_DONE;
  }
  
  parti_ptr = (flash_partable_t)smem_aarm_partition_table;
  
  if ((parti_ptr->magic1 == FLASH_PART_MAGIC1) &&
      (parti_ptr->magic2 == FLASH_PART_MAGIC2) &&
      (parti_ptr->version == FLASH_PARTITION_VERSION) )
  {
    /* We have a valid partition table */
    FLASHLOG(5,("fdcsp:  found partition table\n"));
    FLASHLOG(5,("\tMagic1 is 0x%x\n", parti_ptr->magic1));
    FLASHLOG(5,("\tMagic2 is 0x%x\n", parti_ptr->magic2));
    FLASHLOG(5,("\tTable version is 0x%x\n", parti_ptr->version));
    FLASHLOG(5,("\tNum of partitions is 0x%x\n", parti_ptr->numparts));

    flash_dal_config.config = FLASH_DAL_CONFIG_NAND_BOOT;
    flash_dal_config.parti_tbl = parti_ptr;
    
    return FLASH_DEVICE_DONE;
  }
  
  return FLASH_DEVICE_FAIL;
}

/*
 * This API checks a valid partition table for 0:EFS2 partition.
*/
static int flash_dal_check_efs2(void)
{
  flash_partable_t parti_ptr = flash_dal_config.parti_tbl;
  flash_partentry_t entry;
  int entries;
  int i;

  if (parti_ptr)
  {
    entries = parti_ptr->numparts;
    
    for (i = 0; i < entries; i++)
    {
      entry = &parti_ptr->part_entry[i];
      FLASHLOG(5,("\nPartition 0x%x ::  \n", i));
      FLASHLOG(5,("\tName is %s\n", entry->name));
      FLASHLOG(5,("\tStart is 0x%x\n", entry->offset));
      FLASHLOG(5,("\tLength is 0x%x\n", entry->length));
      FLASHLOG(5,("\tFlash is 0x%x\n", entry->which_flash));
  
      if (strncmp(entry->name, (const char *)MIBIB_EFS2_MODEM_PARTI_NAME, 
          FLASH_PART_NAME_LENGTH) == 0)
      {
        return TRUE;
      }
    }
  }
  else
  {
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                        "Flash DAL: Error! partition tbl is NULL!");
  }
  return FALSE;
}

/*
 * This API checks smem and nand partition table to determine flash driver 
 * configuration.
*/
enum flash_dal_config_state flash_dal_drv_get_config(void)
{
  enum flash_dal_config_state probe_result = FLASH_DAL_CONFIG_UNKNOWN;

  if (flash_dal_check_smem_parti() == FLASH_DEVICE_DONE)
  {
    switch (flash_dal_config.config)
    {
      case FLASH_DAL_CONFIG_NAND_BOOT:
      {
        if (flash_dal_check_efs2())
        {
          flash_dal_config.config = FLASH_DAL_CONFIG_NAND;
        }
        else
        {
          flash_dal_config.config = FLASH_DAL_CONFIG_NOR;
        }
        break;
      }
      case FLASH_DAL_CONFIG_FLASHLESS_BOOT:
      {
        flash_dal_config.config = FLASH_DAL_CONFIG_NOR;
        break;
      }
      default:
      {
        DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                            "Flash DAL: Error! unknown flash dal config state!");
      }
    }
    probe_result = flash_dal_config.config;
  }

  return probe_result;
}
