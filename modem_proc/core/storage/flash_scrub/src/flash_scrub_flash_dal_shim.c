/****************************************************************************
 * flash_scrub_flash_dal_shim.c
 *
 * Flash scrub module device shim layer.
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_flash_dal_shim.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-28   vm    Use correct MSG macros to save RAM.
2014-04-11   wek   Change type cast for read/write.
2014-01-07   wek   Add hooks/callback for read/write/erase operations.
2013-12-09   wek   Add macro to print F3 messages and log to efs file.
2013-11-16   dks   Add flash_ops functions.
2013-09-16   wek   Remove page size and block size arguments from read/write.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-14   rp    Include "scrub_config_i.h" before any other includes.
2013-08-08   rp    Avoid DAL-Attach and Detach on every flash-open-partition.
2013-08-08   rp    Memory-Align flash-buffers.
2013-08-04   rp    Add debug err-fatal and debug f3-messages.
2013-08-04   rp    Add Scrub-Log module.
2013-08-02   rp    Fix memory leak of DALSYS_MemRegionAlloc().
2013-07-12   wek   Fix lint warning.
2013-06-28   rp    Create

===========================================================================*/

#include "flash_scrub_device_callbacks.h"
#include "flash_scrub_config_i.h"
#include "flash_scrub_flash_dal_shim.h"
#include "flash_scrub_err.h"
#include "flash_scrub_log.h"

#include "flash.h"
#include "flash_ops_setup.h"
#include "flash_miparti.h"
#include "assert.h"
#include "DALFramework.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "stringl/stringl.h"



#include <string.h>

#define MAIN_BUFF_DESC  0
#define SPARE_BUFF_DESC 1

#define FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC1  0x12345678
#define FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC2  0xDEADBEEF
#define FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC3  0xCAFEABBA
#define FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC4  0xACCBBBAA

struct flash_scrub_shim_handle_type
{
  uint32 magic1;
  uint32 magic2;
  int is_in_use;

  flash_handle_t nand_handle;
  struct flash_info  info;
  uint8 *page_buf;
  char partition_name[32];

  /* HAL Workspace mem handle */
  DALSYSMemHandle dal_mem_handle;

  /* DAL Memory descriptor for page operations */
  DALSYS_MEM_DESC_LIST_OBJECT (page_buff_desc, 64 * 2);

  uint32 magic3;
  uint32 magic4;
};

#ifndef FLASH_SCRUB_SHIM_MAX_PARTITONS
  #define FLASH_SCRUB_SHIM_MAX_PARTITONS   5
#endif

static struct flash_scrub_shim_handle_type
              flash_scrub_shim_partitions[FLASH_SCRUB_SHIM_MAX_PARTITONS];

/* Static functions to be used for callbacks. */
static FLASH_SCRUB_TEST_CALLBACK_FUNC test_callback_scrub_erase;
static FLASH_SCRUB_TEST_CALLBACK_FUNC test_callback_scrub_write;
static FLASH_SCRUB_TEST_CALLBACK_FUNC test_callback_scrub_read;

void flash_scrub_register_write_callback (
  FLASH_SCRUB_TEST_CALLBACK_FUNC callback)
{
  test_callback_scrub_write = callback;
}

void flash_scrub_register_read_callback (
  FLASH_SCRUB_TEST_CALLBACK_FUNC callback)
{
  test_callback_scrub_read = callback;
}

void flash_scrub_register_erase_callback (
  FLASH_SCRUB_TEST_CALLBACK_FUNC callback)
{
  test_callback_scrub_erase = callback;
}


static void
flash_scrub_shim_validate_parti (
                     struct flash_scrub_shim_handle_type *shim_parti)
{
  ASSERT (shim_parti != NULL);
  ASSERT (shim_parti->is_in_use == 1);
  ASSERT (
      shim_parti->magic1 == FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC1 &&
      shim_parti->magic2 == FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC2 &&
      shim_parti->magic3 == FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC3 &&
      shim_parti->magic4 == FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC4);
}


static void
flash_scrub_shim_init_buffer_descriptor (uint32 op_type,
              void *page_buff_addr, void *spare_buff_addr,
              dalsys_mem_desc_list *buffer_desc,
              struct flash_page_iovec  *page_data_iovec)
{
  DALSysMemDescBuf *main_desc_buff;
  DALSysMemDescBuf *spare_desc_buff;

  DALFW_MemDescInit (NULL, (DALSysMemDescList *) buffer_desc, 2);

  main_desc_buff = DALFW_MemDescBufPtr(buffer_desc, MAIN_BUFF_DESC);
  ASSERT (main_desc_buff != NULL);
  main_desc_buff->VirtualAddr = (DALSYSMemAddr)page_buff_addr;
  main_desc_buff->user = page_data_iovec->total_page_count;

  spare_desc_buff = DALFW_MemDescBufPtr(buffer_desc, SPARE_BUFF_DESC);
  ASSERT (spare_desc_buff != NULL);
  spare_desc_buff->VirtualAddr = (DALSYSMemAddr)spare_buff_addr;
  spare_desc_buff->user = page_data_iovec->total_page_count;

  buffer_desc->dwNumDescBufs = 2;

  switch (op_type)
  {
    case FLASH_READ_SPARE:
      spare_desc_buff->size = (page_data_iovec->total_page_count *
                               page_data_iovec->spare_size_bytes);
      main_desc_buff->VirtualAddr = DALSYS_MEM_ADDR_NOT_SPECIFIED;
      main_desc_buff->size = 0;

      break;

    case FLASH_READ_MAIN:
      main_desc_buff->size = (page_data_iovec->total_page_count *
                              page_data_iovec->main_size_bytes);
      spare_desc_buff->VirtualAddr = DALSYS_MEM_ADDR_NOT_SPECIFIED;
      spare_desc_buff->size = 0;
    break;

    case FLASH_READ_MAIN_SPARE:
      main_desc_buff->size  = (page_data_iovec->total_page_count *
                               page_data_iovec->main_size_bytes);
      spare_desc_buff->size = (page_data_iovec->total_page_count *
                               page_data_iovec->spare_size_bytes);
      break;
  }
}


static uint32
flash_scrub_shim_detect_total_page_size (void)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  uint32 total_page_size;
  DALResult dal_result;
  int result;

  shim_parti = &flash_scrub_shim_partitions[0];

  dal_result = flash_device_attach (DALDEVICEID_FLASH_DEVICE_1,
                                    &shim_parti->nand_handle);
  if (dal_result != DAL_SUCCESS)
  {
    FLASH_SCRUB_ERR_FATAL ("%d, shim : attach failed", dal_result, 0, 0);
  }

  result = flash_open_partition (shim_parti->nand_handle,
                      (const unsigned char *)FLASH_SCRUB_MIBIB_PARTITION_NAME);
  if (result != FLASH_DEVICE_DONE)
  {
    FLASH_SCRUB_ERR_FATAL ("shim : flash-open failed", result, 0, 0);
  }

  result = flash_get_info (shim_parti->nand_handle, FLASH_DEVICE_INFO,
                           &shim_parti->info);
  ASSERT (result == FLASH_DEVICE_DONE);
  ASSERT (shim_parti->info.partition.total_page_size_bytes >= 512);
  total_page_size = shim_parti->info.partition.total_page_size_bytes;

  dal_result = flash_device_close (shim_parti->nand_handle);
  if (dal_result != DAL_SUCCESS)
  {
    FLASH_SCRUB_ERR_FATAL ("%d, close failed", dal_result, 0, 0);
  }

  return total_page_size;
}


static void
flash_scrub_shim_allocate_page_memory (uint32 total_page_size)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  void *nand_page_buf = NULL;
  DALSYSMemInfo mem_info;     /* HAL Workspace mem info */
  DALResult dal_result;
  uint32 i;

  for (i = 0; i < FLASH_SCRUB_SHIM_MAX_PARTITONS; ++i)
  {
    shim_parti = &flash_scrub_shim_partitions[i];

    dal_result = flash_device_attach (DALDEVICEID_FLASH_DEVICE_1,
                                      &shim_parti->nand_handle);
    if (dal_result != DAL_SUCCESS)
    {
      FLASH_SCRUB_ERR_FATAL ("%d, shim : attach failed", dal_result, 0, 0);
    }

    dal_result = DALSYS_MemRegionAlloc (
                        DALSYS_MEM_PROPS_UNCACHED | DALSYS_MEM_PROPS_PHYS_CONT,
                        DALSYS_MEM_ADDR_NOT_SPECIFIED,
                        DALSYS_MEM_ADDR_NOT_SPECIFIED,
                        total_page_size,
                        &shim_parti->dal_mem_handle, NULL);
    if (dal_result != DAL_SUCCESS)
    {
      FLASH_SCRUB_ERR_FATAL ("%d, DALSYS_MemRegionAlloc failed",
                             dal_result, 0, 0);
    }

    memset (&mem_info, 0, sizeof (mem_info));
    dal_result = DALSYS_MemInfo (shim_parti->dal_mem_handle, &mem_info);
    ASSERT (dal_result == DAL_SUCCESS);

    nand_page_buf = (void *)mem_info.VirtualAddr;
    shim_parti->page_buf = (uint8 *) nand_page_buf;
    /*  Need to touch the memory for it to be configured in MMU table */
    DALSYS_memset(nand_page_buf, 0xFF, 10);

    DALFW_MemDescInit(NULL,
                      (DALSysMemDescList *)&shim_parti->page_buff_desc,2);

    shim_parti->magic1 = FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC1;
    shim_parti->magic2 = FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC2;
    shim_parti->magic3 = FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC3;
    shim_parti->magic4 = FLASH_SCRUB_SHIM_HANDLE_TYPE_STRUCT_MAGIC4;
  }
}


flash_scrub_shim_handle
flash_scrub_shim_open_partition (const char *partition_name)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  int result;
  uint32 i, size;

  for (i = 0; i < FLASH_SCRUB_SHIM_MAX_PARTITONS; ++i)
  {
    shim_parti = &flash_scrub_shim_partitions[i];
    if (!shim_parti->is_in_use)
    {
      break;
    }
  }

  if (i >= FLASH_SCRUB_SHIM_MAX_PARTITONS)
  {
    FLASH_SCRUB_DEBUG_ERR_FATAL ("shim no free slot", 0, 0, 0);
    return NULL;
  }

  result = flash_open_partition (shim_parti->nand_handle,
                                 (const unsigned char *)partition_name);
  if (result != FLASH_DEVICE_DONE)
  {
    FLASH_SCRUB_DEBUG_ERR_FATAL ("shim : flash-open failed", result, 0, 0);
    return NULL;
  }

  result = flash_get_info (shim_parti->nand_handle, FLASH_DEVICE_INFO,
                           &shim_parti->info);
  ASSERT (result == FLASH_DEVICE_DONE);
  ASSERT (shim_parti->info.partition.total_page_size_bytes >= 512);

  shim_parti->is_in_use = 1;
  size = strlcpy (shim_parti->partition_name, partition_name,
                  sizeof (shim_parti->partition_name));
  if (size >= sizeof (shim_parti->partition_name))
  {
    FLASH_SCRUB_MSG_DEBUG_HIGH_3 ("[%d, %d, %d] Partition name too large",
                 partition_name[2],partition_name[3], size);
  }

  flash_scrub_shim_validate_parti (shim_parti);

  FLASH_SCRUB_MSG_DEBUG_HIGH_3 ("[%d, %d, %d] shim-open-part good",
        partition_name[2],partition_name[3], (uint32)shim_parti);

  return shim_parti;
}



int
flash_scrub_shim_close_partition (flash_scrub_shim_handle handle)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  DALResult dal_result;

  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  flash_scrub_shim_validate_parti (shim_parti);

  dal_result = flash_device_close (shim_parti->nand_handle);
  if (dal_result != DAL_SUCCESS)
  {
    FLASH_SCRUB_ERR_FATAL ("%d, close failed", dal_result, 0, 0);
  }

  shim_parti->is_in_use = 0;

  return dal_result;
}


uint32
flash_scrub_shim_get_page_size (flash_scrub_shim_handle handle)
{
  struct flash_scrub_shim_handle_type *shim_parti;

  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  flash_scrub_shim_validate_parti (shim_parti);

  return shim_parti->info.partition.page_size_bytes;
}

uint32
flash_scrub_shim_get_block_size (flash_scrub_shim_handle handle)
{
  struct flash_scrub_shim_handle_type *shim_parti;

  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  flash_scrub_shim_validate_parti (shim_parti);

  return shim_parti->info.partition.pages_per_block;
}

uint32
flash_scrub_shim_get_block_count (flash_scrub_shim_handle handle)
{
  struct flash_scrub_shim_handle_type *shim_parti;

  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  flash_scrub_shim_validate_parti (shim_parti);

  return shim_parti->info.partition.block_count;
}

int
flash_scrub_shim_erase_block (flash_scrub_shim_handle handle,
                              uint32 block_no)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  struct flash_block_vector block_vector;
  int result;

  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  flash_scrub_shim_validate_parti (shim_parti);

  memset (&block_vector, 0, sizeof (block_vector));
  block_vector.start_block = block_no;
  block_vector.block_count = 1;
  block_vector.result_vector = &result;

  result = flash_erase_blocks(shim_parti->nand_handle, &block_vector, 1);
  if (result != FLASH_DEVICE_DONE)
  {
    FLASH_SCRUB_LOG_ERR_2 ("shim: Erase-fail, res=%d,blk=%d",
                         result, block_no);
    FLASH_SCRUB_DEBUG_ERR_FATAL ("%d, erase failed", result, 0, 0);
  }

  if (test_callback_scrub_erase != NULL)
  {
    struct flash_scrub_callback_args args;
    int cresult;
    args.location = block_no;
    args.result = result;
    cresult = test_callback_scrub_erase (shim_parti->partition_name, &args);
    if (cresult != 0 && result == FLASH_DEVICE_DONE)
      result = cresult;
  }

  return result;
}

int
flash_scrub_shim_write_page (flash_scrub_shim_handle handle, uint8 *page_buff,
                             uint32 block_no, uint32 page_no)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  struct flash_page_iovec page_iovec;
  int result;
  uint32 page_size, block_size;

  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  flash_scrub_shim_validate_parti (shim_parti);

  /* Make sure that the address of the buffer is meeting the alignment
     expectations of the DMA. */
  ASSERT (FLASH_SCRUB_OKAY_FOR_DM (page_buff));

  page_size = shim_parti->info.partition.page_size_bytes;
  block_size = shim_parti->info.partition.pages_per_block;

  memset (&page_iovec, 0, sizeof (page_iovec));
  page_iovec.start_page = ((block_no * block_size) + page_no);
  page_iovec.total_page_count = 1;
  page_iovec.main_size_bytes = page_size;
  page_iovec.spare_size_bytes = 0;

  flash_scrub_shim_init_buffer_descriptor ((uint32)FLASH_WRITE_MAIN,
        page_buff, NULL, (DALSysMemDescList *) &shim_parti->page_buff_desc,
        &page_iovec);

  result = flash_write_pages (shim_parti->nand_handle, FLASH_WRITE_MAIN,
                (DALSysMemDescList *)&shim_parti->page_buff_desc,
                 &page_iovec);

  if (result != FLASH_DEVICE_DONE)
  {
    FLASH_SCRUB_LOG_ERR_3 ("shim: Write-fail, res=%d,blk=%d,pg=%d\n",
                         result, block_no, page_no);
    FLASH_SCRUB_DEBUG_ERR_FATAL ("%d, Write failed", result, 0, 0);
  }

  if (test_callback_scrub_write != NULL)
  {
    struct flash_scrub_callback_args args;
    int cresult;
    args.location = ((block_no * block_size) + page_no);
    args.result = result;
    cresult = test_callback_scrub_write (shim_parti->partition_name, &args);
    if (cresult != 0 && result == FLASH_DEVICE_DONE)
      result = cresult;
  }

  return result;
}

int
flash_scrub_shim_read_page (flash_scrub_shim_handle handle, uint8 *page_buff,
                            uint32 block_no, uint32 page_no)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  struct flash_page_iovec page_iovec;
  int result;
  uint32 page_size, block_size;

  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  flash_scrub_shim_validate_parti (shim_parti);

  /* Make sure that the address of the buffer is meeting the alignment
     expectations of the DMA. */
  ASSERT (FLASH_SCRUB_OKAY_FOR_DM (page_buff));

  page_size = shim_parti->info.partition.page_size_bytes;
  block_size = shim_parti->info.partition.pages_per_block;

  memset (&page_iovec, 0, sizeof (page_iovec));
  page_iovec.start_page = ((block_no * block_size) + page_no);
  page_iovec.total_page_count = 1;
  page_iovec.main_size_bytes = page_size;
  page_iovec.spare_size_bytes = 0;

  flash_scrub_shim_init_buffer_descriptor ((uint32)FLASH_READ_MAIN,
        page_buff, NULL, (DALSysMemDescList *) &shim_parti->page_buff_desc,
        &page_iovec);

  result = flash_read_pages (shim_parti->nand_handle, FLASH_READ_MAIN,
                     (DALSysMemDescList *)&shim_parti->page_buff_desc,
                     &page_iovec);

  if (test_callback_scrub_read != NULL)
  {
    struct flash_scrub_callback_args args;
    int cresult;
    args.location = ((block_no * block_size) + page_no);
    args.result = result;
    cresult = test_callback_scrub_read (shim_parti->partition_name, &args);
    if (cresult != 0 && result == FLASH_DEVICE_DONE)
      result = cresult;
  }

  return result;
}

int
flash_scrub_shim_mark_block_bad (flash_scrub_shim_handle handle,
                                     uint32 block_no)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  int result;

  flash_scrub_shim_validate_parti (shim_parti);

  FLASH_SCRUB_LOG_ERR_1 ("shim: mark-bad-block, blk=%d", block_no);

  result = flash_block_set_state (shim_parti->nand_handle, block_no,
                                  FLASH_BLOCK_BAD);

  FLASH_SCRUB_MSG_DEBUG_HIGH_3 ("[%d, %d, %d] shim-mark-block-bad",
                              result, block_no, (uint32)shim_parti);

  return result;
}

int
flash_scrub_shim_is_block_bad (flash_scrub_shim_handle handle,
                                   uint32 block_no)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  enum flash_block_state block_state;
  int result, is_block_bad;

  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  flash_scrub_shim_validate_parti (shim_parti);

  result = flash_block_get_state (shim_parti->nand_handle, block_no,
                                  &block_state);
  ASSERT (result == FLASH_DEVICE_DONE);

  is_block_bad = (block_state == FLASH_BLOCK_BAD) ? 1 : 0;
  if (is_block_bad)
  {
    FLASH_SCRUB_MSG_DEBUG_HIGH_2 ("[%d, %d] shim-block-is-bad",
                                block_no, (uint32)shim_parti);
  }

  return is_block_bad;
}

int
flash_scrub_shim_partition_table_get (flash_scrub_shim_handle handle,
                                          flash_partable_t parti_table)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  int result;

  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  flash_scrub_shim_validate_parti (shim_parti);

  result = flash_partition_table_get (shim_parti->nand_handle, parti_table);

  FLASH_SCRUB_MSG_DEBUG_HIGH_2 ("[%d, %d] shim-parti-tbl-get",
                              result, (uint32)shim_parti);

  return result;
}

int
flash_scrub_shim_partition_table_update (
      flash_scrub_shim_handle handle, flash_partable_t parti_table)
{
  struct flash_scrub_shim_handle_type *shim_parti;
  int result;

  shim_parti = (struct flash_scrub_shim_handle_type *)handle;
  flash_scrub_shim_validate_parti (shim_parti);

  result = flash_partition_table_update (shim_parti->nand_handle, parti_table);

  FLASH_SCRUB_MSG_DEBUG_HIGH_2 ("[%d, %d] shim-parti-tbl-update",
                              result, (uint32)shim_parti);

  return result;
}


volatile int flash_scrub_turn_off_ops_start = 0;

static void
flash_scrub_shim_ops_init (void)
{
    flash_ops_init ();
}

void
flash_scrub_shim_ops_start (void)
{
  if (!flash_scrub_turn_off_ops_start)
  {
    flash_ops_start ();
  }
}

void
flash_scrub_shim_ops_end (void)
{
  if (!flash_scrub_turn_off_ops_start)
  {
    flash_ops_end ();
  }
}

void
flash_scrub_shim_init (void)
{
  uint32 total_page_size;

  memset (flash_scrub_shim_partitions, 0, sizeof
          (flash_scrub_shim_partitions));

  total_page_size = flash_scrub_shim_detect_total_page_size ();

  flash_scrub_shim_allocate_page_memory (total_page_size);

  flash_scrub_shim_ops_init ();

  test_callback_scrub_erase = NULL;
  test_callback_scrub_write = NULL;
  test_callback_scrub_read = NULL;
}
