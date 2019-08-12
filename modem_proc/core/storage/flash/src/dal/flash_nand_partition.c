/*=============================================================================
 *
 * FILE:      flash_nand_partition.c
 *
 * DESCRIPTION: Function and data structure declarations specific to the NAND
 *             partition layer
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_nand_partition.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 03/24/14     sb      Fix KW warnings
 * 10/01/13     sb      Store age of new mibib for later usage
 * 06/21/13     sb      Remove user partition table from MIBIB 
 * 04/04/13     eo      Fix Klocwork errors
 * 03/25/13     eo      Set minimum block limit to search for MIBIB 
 * 03/13/13     eo      Disable shared memory dependency
 * 09/12/12     sv      BAM based NAND driver support / Clean up
 * 09/11/12     eo      Clean up
 * 07/27/12     sv      Remove 2x mode and tools specific code
 * 02/28/12     sv      Fix Klocwork error
 * 01/18/12     bb      Adding support for opening up 0:ALL with ECC on M&S
 * 07/25/11     eo      Initialize nand_partition_table for later retrieval
 * 05/20/11     bb      Flash driver Optimization
 * 10/14/10     bb      Fix for QPST failure with DSP1 image in 8K WM7
 * 07/20/10     jz      Check 2x support enablement on each partition
 * 07/16/10     jz      Handle interleaved mode with demand paging support
 * 07/08/10     jz      Cleanup dual nand controller mode
 * 04/09/10     jz      Fixed to set physical addr in init_buffer_descriptor
 * 02/08/10     bb      Cleanup
 * 11/24/09     jz      Fixed compiler warnings
 * 10/12/09     mh      Rename nand_partition_open 
 * 10/01/09     eo      Add DM support to MDM9k target
 * 07/21/09     sv      Modified to use DALSYS api's for memory operations
 * 07/20/09     eo      Moved kick watchdog logic to dal config
 * 06/19/09     mm      Fix Warning related to unused variables
 * 04/28/09     sv      kick watchdog in MIBIB block scan, fix alignments
 * 03/12/09     bb      Add updates for IOVEC changes
 * 02/19/09     mh      Clean-up, fix prefast warning
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nand_partition.h"
#include <string.h>
#include "flash_mibib.h"
#include "flash_miparti.h"
#include "flash_micrc.h"
#include "flash_dal_util.h"
#include "crc.h"

#include "flash_nand.h"
#include "flash_nand_entry.h"

#define FLASH_DEV_TYPE(client_ctxt)  \
  client_ctxt->flash_dev_ctxt->dev_info.common.type

#define GET_RAW_PBUF(client_ctxt) GET_PDATA(client_ctxt)->raw_page_buffer

#define GET_FLASH_DRV_MEM(client_ctxt)                          \
     GET_PDATA(client_ctxt)->flash_mem_info->handle_drv_mem

/* Number of bytes required for storing partition table */
#define FLASH_PARTITION_TABLE_SIZE 1024   

/* DAL Memory descriptor for page operations */
static DALSYS_MEM_DESC_LIST_OBJECT(parti_buff_desc, 2);
static struct flash_page_iovec page_data_iovec;

/*------------------------------------------------------------------------------
  Local declarations.
 -----------------------------------------------------------------------------*/

/* Nand Partition table */
struct nand_parti_table nand_partition_table = {0};

static __inline void init_page_param(flash_client_ctxt *client_ctxt,
  uint32 page, uint32 page_count, struct flash_page_iovec *page_data)
{
  /* This function assumes that we will do on FLASH_READ_MAIN
     in this file */
  page_data->main_size_bytes =
    client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes;
  page_data->spare_size_bytes = 0;
  page_data->start_page = page;
  page_data->total_page_count = page_count;
}

static __inline void init_buffer_descriptor(flash_client_ctxt *client_ctxt,
  uint32 buff_addr, uint32 page_count, dalsys_mem_desc_list *buffer_desc)
{
  DALSysMemDescBuf *desc_buff;

  DALFW_MemDescInit((void*)GET_FLASH_DRV_MEM(client_ctxt),
    (DALSysMemDescList *) buffer_desc, 2);

  desc_buff = DALFW_MemDescBufPtr(buffer_desc, 0);
  if (desc_buff)
  {
    /* set the size and address for main */
    desc_buff->VirtualAddr = buff_addr;
    desc_buff->PhysicalAddr = flash_vtop_get_physical(buff_addr);
    desc_buff->size = 
      client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes;
    desc_buff->user = page_count;

    /* set the size for spare to 0 */
    desc_buff++;
    desc_buff->size = 0;
    desc_buff->user = page_count;
  }
}

static int flash_parti_read_pages(flash_client_ctxt *client_ctxt, uint32 page,
  uint32 read_type, void *buffer)
{
  int result = FLASH_DEVICE_DONE;

  /* Initialize buffer descriptors */
  init_buffer_descriptor(client_ctxt, (uint32) buffer, 1,
    (DALSysMemDescList *) &parti_buff_desc);

  /* Initialize the page data IOVEC structure */
  init_page_param(client_ctxt, page, 1, &page_data_iovec);

  result = nand_entry_read_pages(client_ctxt, (enum page_read_opcode) read_type,
    (DALSysMemDescList *) &parti_buff_desc, &page_data_iovec);

  return result;
}

/*
 * This function compares a partition enty name to a string to find a match.
 */

static int nand_partition_name_matches(flash_partentry_t partentry,
  const unsigned char *name)
{
  if (strncmp(partentry->name, (const char *)name, 
    FLASH_PART_NAME_LENGTH) == 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*  This function will attempt to find the named partition in the partition
 *   table
 */

static flash_partentry_t nand_partition_find_entry(flash_partable_t parti_ptr,
  const unsigned char *parti_name)
{
  int entries = parti_ptr->numparts;
  flash_partentry_t entry;
  int i;

  for (i=0; i<entries; i++)
  {
    entry = &parti_ptr->part_entry[i];
    FLASHLOG(5,("\nPartition 0x%x ::  \n", i));
    FLASHLOG(5,("\tName is %s\n", entry->name));
    FLASHLOG(5,("\tStart is 0x%x\n", entry->offset));
    FLASHLOG(5,("\tLength is 0x%x\n", entry->length));
    FLASHLOG(5,("\tFlash is 0x%x\n", entry->which_flash));

    if (nand_partition_name_matches(entry, parti_name))
    {
      return entry;
    }
  }

  return(flash_partentry_t)0;
}

/*  This function uses a pointer to a partition table and
 *  partition name, and if it finds the partition by name
 *  it updates the block limits in the device structure
 *  with the values from the partition table entry.
 */

static int nand_update_limits(flash_client_ctxt *client_ctxt,
  const unsigned char *parti_name)
{
  flash_partentry_t entry_ptr;
  int orig_start, orig_limit;
  int result = FLASH_DEVICE_DONE;
  flash_partable_t parti_ptr =
    ( flash_partable_t )client_ctxt->flash_dev_ctxt->parti_ptr;

  /* We are going to mess with the internal data of the device structure,
   * so store away the previous values and modify them so that we can
   * see the entire device.  If we fail, we will restore the original
   * values.  If we find the partition, we will modify the original
   * values to reflect the new partiton limits.
   */
  orig_start = client_ctxt->client_data.partition_block_start;
  orig_limit = client_ctxt->client_data.partition_block_limit;

  FLASHLOG(3, ("fnul:  Updating device limits to partition %s\n", parti_name));

  entry_ptr = nand_partition_find_entry(parti_ptr, parti_name);

  if (entry_ptr != (flash_partentry_t)0)
  {
    uint32 parti_attrib = 0;

    strlcpy(client_ctxt->client_data.partition_name,
            (const char*)parti_name, sizeof(client_ctxt->client_data.partition_name));

    /* We have found the partition, update the device limits in the
     * caller's data structure.
     * Ensure that we did not go beyond the physical limits of the
     * the device.  This is because we allow the last partition
     * defined to grow to the device size by giving it a
     * special value.  This code ensures that we remain
     * within the actual device limits.
     */
    client_ctxt->client_data.partition_block_start = entry_ptr->offset;
    FLASHLOG(3, ("fnul:  Set partition start to requested block 0x%x\n",
      client_ctxt->client_data.partition_block_start));
    if (entry_ptr->length == FLASH_PARTITION_GROW)
    {
      client_ctxt->client_data.partition_block_limit =
        client_ctxt->flash_dev_ctxt->dev_info.u.nand.block_count;
      FLASHLOG(3, ("fnul:  Grew partition limit to 0x%x\n",
        client_ctxt->client_data.partition_block_limit));
      entry_ptr->length = 
        client_ctxt->flash_dev_ctxt->dev_info.u.nand.block_count - 
        entry_ptr->offset;
    }
    else
    {
      client_ctxt->client_data.partition_block_limit = entry_ptr->offset
                                                       + entry_ptr->length;
      FLASHLOG(3, ("fnul:  Set partition limit to requested block 0x%x\n",
        client_ctxt->client_data.partition_block_limit));

      /* Check to see that someone has not overlooked device size and made
       * the partition go past the end of the device.
       */
      if (client_ctxt->client_data.partition_block_limit >
        (uint32)client_ctxt->flash_dev_ctxt->dev_info.u.nand.block_count)
      {
        FLASHLOG(3, ("fnul:  Partition is beyond device limits\n"));
        result = FLASH_DEVICE_FAIL;
      }
    }
    client_ctxt->client_data.block_count = entry_ptr->length;

    /* If partition was found and limits were set accordingly, check to see
     * if controller needs to be reconfigured.
     */
    if (FLASH_DEVICE_DONE == result)
    {
      /* Concatenate all the partition attributes */
      parti_attrib |= (uint32)entry_ptr->attrib1;
      parti_attrib |= 
        ((uint32)entry_ptr->attrib2 << FLASH_PART_ATTRIBUTE2_SHFT);
      parti_attrib |= 
        ((uint32)entry_ptr->attrib3 << FLASH_PART_ATTRIBUTE3_SHFT);

      /* Populate controller private field with attrib3 */
      client_ctxt->client_data.partition_attributes = 
        parti_attrib & FLASH_PART_ATTRIBUTE3_BMSK;

      if ((entry_ptr->attrib1 != (uint32)FLASH_PARTITION_DEFAULT_ATTRB) ||
          (entry_ptr->attrib2 != (uint32)FLASH_PARTITION_DEFAULT_ATTRB2))
      {        
        /* HW ECC is enabled per partition basis. Checks if Spare ECC is 
         * enabled. If Spare ECC is enabled then recalculate the hardware 
         * register values, and spare layout.
         */
        if ((FLASH_PART_ATTRIB(parti_attrib, FLASH_PART_ATTRIBUTE2)) ==
          (uint32)FLASH_PARTITION_MAIN_AND_SPARE_ECC)
        {
          client_ctxt->client_data.ecc_state = FLASH_ECC_MAIN_SPARE_ENABLED;
          FLASHLOG (3, (" nand_update_limits called with ECC Main & \
            Spare\n"));
        }
        else if ((FLASH_PART_ATTRIB(parti_attrib, FLASH_PART_ATTRIBUTE2)) ==
         (uint32)FLASH_PARTITION_MAIN_AREA_ONLY)
        {
          client_ctxt->client_data.ecc_state = FLASH_ECC_MAIN_ENABLED;
          FLASHLOG (3, (" nand_update_limits called with ECC Main only\n"));
        }
      }

      /* Initialize the partition attributes. Driver only uses attrib1 and
       * attrib2. Attrib3 is used by external clients only */
      client_ctxt->client_data.partition_attributes = parti_attrib;
    }
  }
  else
  {
    /* Partition table did not have an entry for the partition name */
    FLASHLOG(3, ("fnul:  Did not get a partition entry match for %s\n",
      parti_name));
    result = FLASH_DEVICE_FAIL;
  }

  if (FLASH_DEVICE_FAIL == result)
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
 * Determines if the contents of the block are a valid MIBIB.
 * If a valid MIBIB, return TRUE * update age field passed by caller
 * If not valid, return FALSE
 */

static int nand_is_block_mibib ( flash_client_ctxt *client_ctxt, int block, 
  uint32 *age)
{
  enum flash_block_state blk_state;
  mi_boot_info_t mibib_magic;
  flash_partable_t parti_sys;
  flash_micrc_data_t mibib_crc;
  uint32 mibib_age;
  uint8 fill_char = 0xFF;
  uint32 page_size_in_bits;
  uint32 crc32 = 0x0UL;
  uint32 i;
  uint32 page;
  unsigned char *page_buf  = NULL;
  int result = FLASH_DEVICE_DONE;

  page_buf = (unsigned char *)(GET_RAW_PBUF(client_ctxt));

  if (DAL_SUCCESS != nand_entry_block_get_state(client_ctxt,
    block, &blk_state ))
  {
    FLASHLOG (4, ("fnibm: Is Block MIBIB - Get Block Bad Failure.\n"));
    return FALSE;
  }

  if (blk_state == FLASH_BLOCK_BAD)
  {
    FLASHLOG (4, ("fnibm: Is Block MIBIB - Block Bad.\n"));
    return FALSE;
  }
  /* Calculate page numbers of MIBIB ID page and partition page */
  page_size_in_bits =
    client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes * 8;

  page = block * client_ctxt->flash_dev_ctxt->dev_info.u.nand.pages_per_block;

  result = flash_parti_read_pages(client_ctxt, page, FLASH_READ_MAIN, page_buf);

  if (DAL_SUCCESS != result)
  {
    return FALSE;
  }

  mibib_magic = (mi_boot_info_t)page_buf;
  if ( (mibib_magic->magic1 != MIBIB_MAGIC1) ||
    (mibib_magic->magic2 != MIBIB_MAGIC2) ||
    (mibib_magic->version != MIBIB_VERSION) )
  {
    FLASHLOG (4, ("fnibm: MIBIB magic or version number mismatch.\n"));
    return FALSE;
  }

  /* Stash away the age number to return, if everything else checks out to be
   * alright.
   */
  mibib_age = mibib_magic->age;

  crc32 = crc_32_calc((uint8 *)page_buf, (uint16)page_size_in_bits, crc32);

  /* If the sys and user partition pages are erased, it cannot be a valid
   * MIBIB */

  page++;

  result = flash_parti_read_pages(client_ctxt, page,
    FLASH_READ_MAIN, page_buf);

  if (DAL_SUCCESS != result)
  {
    return FALSE;
  }

  /* If the sys partition page does not contain valid ID, it cannot be a valid
   * MIBIB
   */
  parti_sys = (flash_partable_t)page_buf;
  if ( (parti_sys->magic1 != FLASH_PART_MAGIC1) ||
    (parti_sys->magic2 != FLASH_PART_MAGIC2) ||
    (parti_sys->version != FLASH_PARTITION_VERSION) )
  {
    FLASHLOG (4, ("fnibm: System parti magic or version number mismatch.\n"));
    return FALSE;
  }
  crc32 = crc_32_calc((uint8 *)page_buf, (uint16)page_size_in_bits, crc32);

  /* Note that do incremental computation by passing the result of previous
   * operation as seed for next operation. This makes the sequence of
   * invocation with respect to the page buffers rigid.
   */

  /* Compute crc32 checksum on rest of the MIBIB pages. First read pages out
   * in a page size buffer excluding the CRC page. Then call the CRC routine
   * to calculate checksum in lock-step fashion page-by-page.
   *
   * There may be empty pages in MIBIB. For CRC calculation we assume the data
   * to be all 'F's. To accomplish this we will fill the buffer with 'F's.
   */

  /* We already know that MIBIB and partition table pages are there and are
   * good. We also have crc checksum for those pages. We start with subsequent
   * page here.
   */
  for (i = (MIBIB_PAGE_PARTITION_TABLE + 1);
    i < (MIBIB_PAGE_LAST_PAGE - 2); i++)
  {
    page++;

    result = flash_parti_read_pages(client_ctxt, page,
      FLASH_READ_MAIN, page_buf);

    if (DAL_SUCCESS != result)
    {
      if (FLASH_DEVICE_FAIL_PAGE_ERASED == result)
      {
        FLASHLOG (4, ("fnibm: MIBIB page 0x%x is erased. Filling buf w/ 0xFF\n",
          page));
        DALSYS_memset((void *)page_buf, fill_char,
          client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes);
      }
      else
      {
        return FALSE;
      }
    }

    FLASHLOG(4, ("fmmv: Calculating checksum on page 0x%x\n", page));
    crc32 = crc_32_calc((uint8 *)page_buf, (uint16)page_size_in_bits, crc32);
  }

  /* Read the crc page parse the data */
  page = page + 2;

  FLASHLOG (4, ("fmmv: Reading CRC checksum from page 0x%x\n", page));

  result = flash_parti_read_pages(client_ctxt, page,
    FLASH_READ_MAIN, page_buf);

  if (DAL_SUCCESS != result)
  {
    FLASHLOG (1, ("fnibm: Read of MIBIB crc page failed at 0x%x.\n", page));
    return FALSE;
  }

  /* If the CRC page does not contain valid ID and version number this MIBIB
   * block is corrupted.
   */
  mibib_crc = (flash_micrc_data_t)page_buf;

  if (mibib_crc->magic1 != FLASH_MIBIB_CRC_MAGIC1)
  {
    FLASHLOG (4, ("fnibm: CRC page magic1 number mismatch.\n"));
    return FALSE;
  }
  else if (mibib_crc->magic2 != FLASH_MIBIB_CRC_MAGIC2)
  {
    FLASHLOG (4, ("fnibm: CRC page magic2 number mismatch.\n"));
    return FALSE;
  }
  else if (mibib_crc->version != FLASH_MIBIB_CRC_VERSION)
  {
    FLASHLOG (4, ("fnibm: CRC page version number mismatch.\n"));
    return FALSE;
  }
  else if (mibib_crc->crc != crc32)
  {
    FLASHLOG (4, ("fnibm: CRC page checksum mismatch.\n"));
    return FALSE;
  }
  else
  {
    FLASHLOG (4, ("fnibm: CRC page magic and version number match.\n"));
  }
  /* All the tests have passed successfully.  We have a valid MIBIB
   * with a valid partition table in it.  Update the age field
   * passed in by caller and return.
   */
  *age = mibib_age;

  return TRUE;
}

#if ((defined(IMAGE_APPS_PROC) || defined(IMAGE_QDSP6_PROC)) && \
     !defined(FLASH_DISABLE_SMEM))

#include "smem.h"

int smem_retrieve_mibib(flash_client_ctxt *client_ctxt)
{
  flash_partable_t parti_ptr;
  flash_partable_t smem_aarm_partition_table;
  unsigned char *page_buf  = NULL;

  page_buf = (unsigned char *) (GET_RAW_PBUF(client_ctxt));

  if (client_ctxt->flash_dev_ctxt->parti_ptr == NULL)
  {
    if (DAL_SUCCESS != DALSYS_Malloc
      (client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes,
      &(client_ctxt->flash_dev_ctxt->parti_ptr)))
    {
      FLASHLOG(3, ("\nMalloc failed\n"));
      return FLASH_DEVICE_FAIL;
    }
  }

  /* Initialize the shared memory pointer first */
  smem_aarm_partition_table = smem_alloc( SMEM_AARM_PARTITION_TABLE,
                                         sizeof(struct flash_partition_table) );
  if (smem_aarm_partition_table == NULL)
  {
    return FLASH_DEVICE_FAIL;
  }

  DALSYS_memcpy((void *)page_buf, (void *)smem_aarm_partition_table,
    sizeof(struct flash_partition_table));
  parti_ptr = (flash_partable_t)page_buf;

  if ((parti_ptr->magic1 == FLASH_PART_MAGIC1) &&
      (parti_ptr->magic2 == FLASH_PART_MAGIC2) &&
      (parti_ptr->version == FLASH_PARTITION_VERSION) )
  {
        /* We have a valid partition table */
    FLASHLOG(5,("fnop:  found partition table\n"));
    FLASHLOG(5,("\tMagic1 is 0x%x\n", parti_ptr->magic1));
    FLASHLOG(5,("\tMagic2 is 0x%x\n", parti_ptr->magic2));
    FLASHLOG(5,("\tTable version is 0x%x\n", parti_ptr->version));
    FLASHLOG(5,("\tNum of partitions is 0x%x\n", parti_ptr->numparts));

    DALSYS_memcpy(client_ctxt->flash_dev_ctxt->parti_ptr,
      (const void *)page_buf,
      client_ctxt->flash_dev_ctxt->dev_info.u.nand.page_size_bytes);

  }
  return FLASH_DEVICE_DONE;
}
#else
int smem_retrieve_mibib(flash_client_ctxt *client_ctxt)
{
  return FLASH_DEVICE_FAIL;
}
#endif

/*
 * Reads the mibib from the flash
 */
int nand_retrieve_mibib(flash_client_ctxt *client_ctxt)
{
  int copy1_blockno=0, copy2_blockno=0; /* init to suppress warning only */
  int cur_block, age = 0;
  int result = FLASH_DEVICE_DONE;
  uint32 copy1_age=0, copy2_age=0, page = 0;
  int copy1_valid = FALSE;
  int copy2_valid = FALSE;
  flash_partable_t parti_ptr;
  unsigned char *page_buf  = NULL;

  page_buf = (unsigned char *) (GET_RAW_PBUF(client_ctxt));

  if (client_ctxt->flash_dev_ctxt->parti_ptr == NULL)
  {
    if (DAL_SUCCESS != DALSYS_Malloc(FLASH_PARTITION_TABLE_SIZE,
      &(client_ctxt->flash_dev_ctxt->parti_ptr)))
    {
      FLASHLOG(3, ("\nMalloc failed\n"));
      return FLASH_DEVICE_FAIL;
    }
  }

  client_ctxt->client_data.partition_block_start   = 0;
    client_ctxt->client_data.partition_block_limit = MIBIB_BLOCK_SEARCH_MAX;
  client_ctxt->client_data.block_count      =
    client_ctxt->client_data.partition_block_limit -
    client_ctxt->client_data.partition_block_start;

  /* Start looking from first MIBIB block */
  cur_block = MIBIB_BLOCK_SEARCH_MIN;

  while ((cur_block <= MIBIB_BLOCK_SEARCH_MAX))
  {
    //FLASHLOG (4, ("fnfm: Scanning block 0x%x for first MIBIB\n", cur_block));
    if ((nand_is_block_mibib (client_ctxt, cur_block, &copy1_age)) == TRUE)
    {
      copy1_valid = TRUE;
      copy1_blockno = cur_block;
      break;
    }

    cur_block++;
  }

  /* If we searched to the limit without finding, we are done */
  if (cur_block > MIBIB_BLOCK_SEARCH_MAX)
  {
    client_ctxt->flash_dev_ctxt->mibib_info.old_mibib_block = -1;
    client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block = -1;
    return FLASH_DEVICE_DONE;
  }


  /* Start looking for the second MIBIB at next block */
  cur_block++;

  while ((cur_block <= MIBIB_BLOCK_SEARCH_MAX))
  {
    FLASHLOG (4, ("fnfm: Scanning block 0x%x for second MIBIB\n", cur_block));
    /* Check for MIBIB block */
    if ((nand_is_block_mibib (client_ctxt, cur_block, &copy2_age)) == TRUE)
    {
      copy2_valid = TRUE;
      copy2_blockno = cur_block;
      break;
    }

    cur_block++;
  }

  /* Now, finally determine which is newer */
  if (!copy1_valid && !copy2_valid)
  {
    FLASHLOG(5,("fnfm:  Neither is valid\n"));
    client_ctxt->flash_dev_ctxt->mibib_info.old_mibib_block = -1;
    client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block = -1;
  }
  else if (copy1_valid && !copy2_valid)
  {
    FLASHLOG(5,("fnfm:  Only copy1 is valid\n"));
    client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block = copy1_blockno;
    client_ctxt->flash_dev_ctxt->mibib_info.old_mibib_block = -1;
  }
  else if (!copy1_valid && copy2_valid)
  {
    FLASHLOG(5,("fnfm:  Only copy2 is valid\n"));
    client_ctxt->flash_dev_ctxt->mibib_info.old_mibib_block = copy2_blockno;
    client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block = -1;
  }
  else
  {
    /* Both are valid, check the age */
    FLASHLOG(5,("fnfm:  copy1 age 0x%x copy2 age 0x%x\n", copy1_age, \
      copy2_age));
    if (copy1_age == copy2_age)
    {
      FLASHLOG(5,("fnfm:  ERROR:  ages are equal\n"));
      return FLASH_DEVICE_FAIL;
    }
    if (copy1_age > copy2_age)
    {
      client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block = copy1_blockno;
      client_ctxt->flash_dev_ctxt->mibib_info.old_mibib_block = copy2_blockno;
      age = copy1_age;
    }
    else
    {
      client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block = copy2_blockno;
      client_ctxt->flash_dev_ctxt->mibib_info.old_mibib_block = copy1_blockno;
      age = copy2_age;
    }
  }

  /* Find new_mibib_block and old_mibib_block MIBIB.  This function call will 
   * only return an error if it cannot find a good block in the flash.  Not
   * finding any valid MIBIB instances is not a failure.
   */
  if (-1 != client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block)
  {
    if (age != -1)
    {
      FLASHLOG(5,("fnop: new_mibib_block MIBIB age is 0x%x\n", age));
	  client_ctxt->flash_dev_ctxt->mibib_age = age;

      /* Reading page 1 for Partition table */
      page = (client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block * \
          client_ctxt->flash_dev_ctxt->dev_info.u.nand.pages_per_block) +
          MIBIB_PAGE_PARTITION_TABLE;

      result = flash_parti_read_pages(client_ctxt, page,
        FLASH_READ_MAIN, page_buf);

      if (DAL_SUCCESS == result)
      {
        parti_ptr = (flash_partable_t)page_buf;
        if ((parti_ptr->magic1 == FLASH_PART_MAGIC1) &&
          (parti_ptr->magic2 == FLASH_PART_MAGIC2) &&
          (parti_ptr->version == FLASH_PARTITION_VERSION) )
        {
          /* We have a valid partition table */
          FLASHLOG(5,("fnop:  found partition table\n"));
          FLASHLOG(5,("\tMagic1 is 0x%x\n", parti_ptr->magic1));
          FLASHLOG(5,("\tMagic2 is 0x%x\n", parti_ptr->magic2));
          FLASHLOG(5,("\tTable version is 0x%x\n", parti_ptr->version));
          FLASHLOG(5,("\tNum of partitions is 0x%x\n", parti_ptr->numparts));

#ifdef FLASH_PARTITION_SMEM
          flash_smem_parti_init(client_ctxt, page_buf);
#endif
          DALSYS_memcpy(client_ctxt->flash_dev_ctxt->parti_ptr, 
            (const void *)page_buf, FLASH_PARTITION_TABLE_SIZE);

          /* Save the partition table size and table data for later retrieval */
          nand_partition_table.parti_table_size_bytes =
            sizeof(struct flash_partition_table);
          nand_partition_table.parti_table = 
            (uint32 *)client_ctxt->flash_dev_ctxt->parti_ptr;
        }
      }
      return FLASH_DEVICE_DONE;
    }
    else /* if (age == -1) */
    {
      FLASHLOG(5, ("fnop:  Cannot find MIBIB block\n"));
      return FLASH_DEVICE_FAIL;
    } /* if (age == -1) */
  } /* if a good MIBIB block was found */

  return FLASH_DEVICE_DONE;
}

/*  Opens a partition found by a name string passed in.
 *  Modifies the private data of the device structure to
 *  limit the "view" of the device by the device driver
 *  for all subsequent calls to device functions.
 */
int nand_partition_update_limits (flash_client_ctxt *client_ctxt,
  const unsigned char *parti_name)
{
  int result = FLASH_DEVICE_DONE;

  /* Set the partition partition_block_start and partition_block_limit
   * according to the parti_name
   */
  FLASHLOG (3, (" nand_partition_open called with %s\n", parti_name));

  client_ctxt->client_data.ecc_state = FLASH_ECC_MAIN_SPARE_ENABLED;

  if (strncmp((const char *)parti_name, "0:ALL", 6) == 0)
  {
    client_ctxt->client_data.partition_block_start   = 0;
    client_ctxt->client_data.partition_block_limit   =
      client_ctxt->flash_dev_ctxt->dev_info.u.nand.block_count;
    client_ctxt->client_data.block_count      =
      client_ctxt->client_data.partition_block_limit -
      client_ctxt->client_data.partition_block_start;
  }
  else
  {
    if ((nand_update_limits(client_ctxt, parti_name)) != FLASH_DEVICE_DONE)
    {
      FLASHLOG(5, ("fnop:  Could not update partition limits\n"));
      result = FLASH_DEVICE_FAIL;
    }
  } /* If partition being opened is not 0:ALL */

  return result;
}
