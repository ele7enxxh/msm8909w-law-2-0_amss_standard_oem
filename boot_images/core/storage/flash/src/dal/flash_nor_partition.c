/*=============================================================================
 *
 * FILE:      flash_nor_partition.c
 *
 * DESCRIPTION: Function and data structure declarations specific to the NOR
 *             partition layer
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010, 2012, 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_nor_partition.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/28/15     sb      Remove KW Banned Functions
 * 03/01/12     sv      Fix potential invalid memory access issue
 * 10/14/10     eo      Fix compiler reported warning
 * 09/10/10     eo      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nor_partition.h"
#include "stringl/stringl.h"

/*------------------------------------------------------------------------------
  Defined Macros.
 -----------------------------------------------------------------------------*/

/* Memory allocation for partition table in bytes. */
#define FLASH_NOR_PARTI_TABLE_SIZE_BYTES  512

#define GET_NOR_INFO(client_ctxt)   ((struct nor_dev_data *) \
   &(client_ctxt->flash_dev_ctxt->dev_info.u.nor))

/*------------------------------------------------------------------------------
  Local functions declarations.
 -----------------------------------------------------------------------------*/

/*
 * This function compares a partition enty name to a string to find a match.
 */
static int nor_partition_name_matches(flash_nor_partentry_t partentry,
  const unsigned char *name)
{
  if (strncmp(partentry->name, (const char *)name, 
    FLASH_NOR_PARTI_NAME_LENGTH) == 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*  
 * This function will attempt to find the named partition in the partition
 * table.
 */
static flash_nor_partentry_t nor_partition_find_entry(
  flash_nor_partable_t parti_ptr, const unsigned char *parti_name)
{
  int entries = parti_ptr->numparts;
  flash_nor_partentry_t entry;
  int i;

  for (i = 0; i < entries; i++)
  {
    entry = &parti_ptr->part_entry[i];
    FLASHLOG(5,("\nPartition 0x%x ::  \n", i));
    FLASHLOG(5,("\tName is %s\n", entry->name));
    FLASHLOG(5,("\tStart is 0x%x\n", entry->offset));
    FLASHLOG(5,("\tLength is 0x%x\n", entry->length));

    if (nor_partition_name_matches(entry, parti_name))
    {
      return entry;
    }
  }

  return(flash_nor_partentry_t)0;
}

/* 
 * This function accepts user defined partition table as an input and generates
 * a system partition table.
 */
static int nor_convert_usr_parti_tbl_to_sys (flash_client_ctxt *client_ctxt,
  flash_nor_usr_partable_t parti1, flash_nor_partable_t parti2)
{
  uint32 i, user_parti_size = 0;
  uint32 sys_parti_start = 0;
  uint32 sys_parti_size = 0;
  uint32 block_size = 0;
  uint8 *parti_buffer = (uint8 *)parti2;
  struct nor_dev_data *nor_info = GET_NOR_INFO(client_ctxt);

  /* Check device parameters information is non-zero. */
  if ((nor_info->pages_per_block == 0) || (nor_info->block_count == 0))
  {
    FLASHLOG(3,("Invalid device geometry infomation\n"));
    return FLASH_DEVICE_FAIL;
  }

  switch (nor_info->page_size_bytes)
  {
    /* Since block_size in partition table is specified in KB, we may use a
     * simple calculation */
    case (256):
      block_size = nor_info->pages_per_block / 4;
      break;

    default:
      FLASHLOG(1,("Unsupported page size encountered\n"));
      return FLASH_DEVICE_FAIL;
  }

  /* Start partition buffer with all 0xFFs. */
  memset(parti_buffer, 0xFF, FLASH_NOR_PARTI_TABLE_SIZE_BYTES);

  /* Set number of partition parts even if not used. */
  parti2->numparts = parti1->numparts;

  /* Use number of partitions from user partition table*/
  for (i = 0; i < parti1->numparts; i++)
  {
    /* check for 0:All partitions */
    if (strncmp(parti1->part_entry[i].name, FLASH_DEVICE_ALL_PARTI_NAME,
        sizeof(FLASH_DEVICE_ALL_PARTI_NAME)) != 0)
    {
      (void)memscpy(parti2->part_entry[i].name, FLASH_NOR_PARTI_NAME_LENGTH, 
                parti1->part_entry[i].name, FLASH_NOR_PARTI_NAME_LENGTH);

      parti2->part_entry[i].attrib1 = parti1->part_entry[i].reserved_flag1;
      parti2->part_entry[i].attrib2 = parti1->part_entry[i].reserved_flag2;
    } // if strcmp partition entry with :All partition
    /* can not find 0:All partition */
    else
    {
      FLASHLOG(1,("Reserved partition name found in user partition table!\n"));
      return FLASH_DEVICE_FAIL;
    } // else can not find :All parition

    FLASHLOG(3,("Finding bounds for partition %s\n",
      (char *)&parti2->part_entry[i].name));

    user_parti_size = parti1->part_entry[i].img_size;

    if (user_parti_size == 0)
    {
      FLASHLOG(1,("Partition %s is defined with zero size\n",
        parti1->part_entry[i].name));
      return FLASH_DEVICE_FAIL;
    }

    FLASHLOG(3, ("Image Size is %ld KB\n", user_parti_size));

    if (user_parti_size == FLASH_NOR_PARTI_GROW)
    {
      if (i != (parti1->numparts - 1))
      {
        FLASHLOG(1,("Non-terminal partition defined with GROW \n"));
        return FLASH_DEVICE_FAIL;
      }
      parti2->part_entry[i].offset = sys_parti_start;
      parti2->part_entry[i].length = FLASH_NOR_PARTI_GROW;

      FLASHLOG(3,("Partition start at block %ld\n" ,
        parti2->part_entry[i].offset));
      FLASHLOG(3,("Partition length in blocks %ld\n" ,
        parti2->part_entry[i].length));

      return FLASH_DEVICE_DONE;
    }

    sys_parti_size = 0;

    if (user_parti_size != 0)
    {
      sys_parti_size = user_parti_size / block_size;
      if (user_parti_size % block_size)
      {
        sys_parti_size++;
      }
    }

    FLASHLOG(4,("Partition start is %ld blocks\n", sys_parti_start));
    FLASHLOG(4,("Partition length is %ld blocks\n", sys_parti_size));

    parti2->part_entry[i].offset = sys_parti_start;
    parti2->part_entry[i].length = sys_parti_size;
    sys_parti_start += sys_parti_size;


    FLASHLOG(3,("Partition start at block %ld\n" ,
      parti2->part_entry[i].offset));
    FLASHLOG(3,("Partition length in blocks %ld\n" ,
      parti2->part_entry[i].length));

  } //for

  return FLASH_DEVICE_DONE;
}

/*  
 * This function initializes the flash driver's system partition table.
 */
static int nor_partition_init_info(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;
  flash_nor_partable_t sys_parti = NULL; 
  flash_nor_usr_partable_t usr_parti = (flash_nor_usr_partable_t)&nor_ram_table;

  if (client_ctxt->flash_dev_ctxt->parti_ptr == NULL)
  {
    if (DALSYS_Malloc(FLASH_NOR_PARTI_TABLE_SIZE_BYTES, (void *)&sys_parti) != 
      DAL_SUCCESS)
    {
      FLASHLOG(3, ("\nMalloc failed\n"));
      return FLASH_DEVICE_FAIL;
    }
    client_ctxt->flash_dev_ctxt->parti_ptr = (void *)sys_parti;
  }
  else
  {
    sys_parti = (flash_nor_partable_t)client_ctxt->flash_dev_ctxt->parti_ptr;
  }
  
  if (nor_convert_usr_parti_tbl_to_sys(client_ctxt, usr_parti, sys_parti) != 
    FLASH_DEVICE_DONE)
  {
    FLASHLOG(3, ("\nConvert User to system partition failed\n"));
    return FLASH_DEVICE_FAIL;
  }
  
  return result;
}

/*  
 * This function uses a pointer to a partition table and
 * partition name, and if it finds the partition by name
 * it updates the block limits in the device structure
 * with the values from the partition table entry.
 */
static int nor_update_limits(flash_client_ctxt *client_ctxt,
  const unsigned char *parti_name)
{
  int result = TRUE;
  int orig_start, orig_limit;
  flash_nor_partentry_t entry_ptr;
  flash_nor_partable_t parti_ptr =
    (flash_nor_partable_t)client_ctxt->flash_dev_ctxt->parti_ptr;
  struct nor_dev_data *nor_info = GET_NOR_INFO(client_ctxt);

  /* We are going to mess with the internal data of the device structure,
   * so store away the previous values and modify them so that we can
   * see the entire device.  If we fail, we will restore the original
   * values.  If we find the partition, we will modify the original
   * values to reflect the new partiton limits.
   */
  orig_start = client_ctxt->client_data.partition_block_start;
  orig_limit = client_ctxt->client_data.partition_block_limit;

  FLASHLOG(3, ("Updating device limits to partition %s\n", parti_name));

  entry_ptr = nor_partition_find_entry(parti_ptr, parti_name);

  if (entry_ptr != (flash_nor_partentry_t)0)
  {
    uint32 parti_attrib = 0;

    /* We have found the partition, update the device limits in the
     * caller's data structure.
     * Ensure that we did not go beyond the physical limits of the
     * the device.  This is because we allow the last partition
     * defined to grow to the device size by giving it a
     * special value.  This code ensures that we remain
     * within the actual device limits.
     */
    client_ctxt->client_data.partition_block_start = entry_ptr->offset;
    FLASHLOG(3, ("Set partition start to requested block 0x%x\n",
      client_ctxt->client_data.partition_block_start));

    if (entry_ptr->length == FLASH_NOR_PARTI_GROW)
    {
      client_ctxt->client_data.partition_block_limit = nor_info->block_count;
      FLASHLOG(3, ("Grew partition limit to 0x%x\n",
        client_ctxt->client_data.partition_block_limit));
      entry_ptr->length = nor_info->block_count - entry_ptr->offset;
    }
    else
    {
      client_ctxt->client_data.partition_block_limit = entry_ptr->offset +
        entry_ptr->length;
      FLASHLOG(3, ("Set partition limit to requested block 0x%x\n",
        client_ctxt->client_data.partition_block_limit));

      /* Check to see that someone has not overlooked device size and made
       * the partition go past the end of the device.
       */
      if (client_ctxt->client_data.partition_block_limit >
        (uint32)nor_info->block_count)
      {
        FLASHLOG(3, ("Partition is beyond device limits\n"));
        result = FALSE;
      }
    }
    client_ctxt->client_data.block_count = entry_ptr->length;

    /* If partition was found and limits were set accordingly, check to see
     * if controller needs to be reconfigured.
     */
    if (result == TRUE)
    {
      /* Concatenate all the partition attributes */
      parti_attrib |= (uint32)entry_ptr->attrib1;
      parti_attrib |= ((uint32)entry_ptr->attrib2 << 
        FLASH_NOR_PARTI_ATTRIB2_SHFT);
       
      /* Initialize the partition attributes. Driver only uses attrib1 and
       * attrib2. */
      client_ctxt->client_data.partition_attributes = parti_attrib;
    }
  }
  else
  {
    /* Partition table did not have an entry for the partition name */
    FLASHLOG(3, ("Did not get a partition entry match for %s\n",
      parti_name));
    result = FALSE;
  }

  if (result == FALSE)
  {
    /* If we get here, we did not find the partition and we should restore
     * the original start and limit values to the device structure
     */
    client_ctxt->client_data.partition_block_start = orig_start;
    client_ctxt->client_data.partition_block_limit = orig_limit;
  }

  return result;
}

/*  
 * Opens a partition found by a name string passed in.
 * Modifies the private data of the device structure to
 * limit the "view" of the device by the device driver
 * for all subsequent calls to device functions.
 */
int nor_partition_update_limits(flash_client_ctxt *client_ctxt,
  const unsigned char *parti_name)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_dev_data *nor_info = GET_NOR_INFO(client_ctxt);

  if (client_ctxt->flash_dev_ctxt->parti_ptr == NULL)
  {
    result = nor_partition_init_info(client_ctxt);
    if (result != FLASH_DEVICE_DONE)
    {
      FLASHLOG(5, ("NOR partition initialization failed.\n"));
      result = FLASH_DEVICE_FAIL;
    }
  }

  /* Set the partition partition_block_start and partition_block_limit
   * according to the parti_name
   */
  FLASHLOG (3, ("nor_partition_open called with %s\n", parti_name));

  if (strncmp((const char *)parti_name, FLASH_DEVICE_ALL_PARTI_NAME, 
    sizeof(FLASH_DEVICE_ALL_PARTI_NAME)) == 0)
  {
    client_ctxt->client_data.partition_block_start = 0;
    client_ctxt->client_data.partition_block_limit = nor_info->block_count;
    client_ctxt->client_data.block_count =
      client_ctxt->client_data.partition_block_limit -
      client_ctxt->client_data.partition_block_start;
    client_ctxt->client_data.partition_block_size_bytes = 
      nor_info->page_size_bytes * nor_info->pages_per_block;
    client_ctxt->client_data.ecc_state = FLASH_ECC_NONE;

  }
  else
  {
    if ((nor_update_limits(client_ctxt, parti_name)) != TRUE)
    {
      FLASHLOG(5, ("Could not update partition limits\n"));
      result = FLASH_DEVICE_FAIL;
    }
  } /* If partition being opened is not 0:ALL */

  return result;
}
