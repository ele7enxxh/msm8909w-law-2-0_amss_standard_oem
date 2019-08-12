/**********************************************************************
 * boot_flash_dev_dal.c
 *
 * NAND Flash device level operations
 *
 * This file implements all the functions common to all the NAND devices
 *
 * Copyright 2009-2010, 2012, 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_flash_dev_dal.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
02/03/15     sc      Added error handling in flash_nand_bad_block_check
01/29/15     sc      Added checks for potential integer overflow in flash_init_buffer_descriptor()
02/07/12     sy      Fixing klocwork error
06/05/12     kedara  removed customer.h. moved assignment stmt outside 
                     condition check. 
03/15/12     aus     Fixed release of flash device structure when flash open fails
                     and reduce number of clients
09/09/11     dh      Map all dal and flash driver calls to boot extern interface.
                     Change DALSYS_memset to memset it's a macro maps to memset in DALSys.h
                     Change flash_device_close to DalDevice_Close,
                            flash_device_attach to DAL_DeviceAttach,
                            flash_device_detach to DAL_DeviceDetach,
                     since those are mapped in macros from flash.h
09/21/10     plc     Initial port into SB3.0   
===========================================================================*/
#include "boot_extern_dal_interface.h"
#include "boot_extern_flash_interface.h"
#include "boot_flash_dev_dal.h"
#include "DALDeviceId.h"
#include "boot_error_if.h"


#define MAIN_BUFF_DESC  0
#define SPARE_BUFF_DESC 1
#define FLASH_MAX_CLIENTS 3

/*===========================================================================

  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/****************************************************************
 * The client device structure, the client structure array and
 * current client count
 ***************************************************************/
struct flash_client_device_data
{
  struct fs_device_data flash_device_structure;
  uint32               available;
};

static struct flash_client_device_data flash_device_clients[FLASH_MAX_CLIENTS];
static uint32 flash_client_count = 0;

/* Indicates whether the flash client structure array is initialized */
static uint32 flash_device_clients_initialized = FALSE;

/* DAL Memory descriptor for page operations */
static DALSYS_MEM_DESC_LIST_OBJECT (page_buff_desc, 64*2);

static DALBOOL buff_desc_initialized = FALSE;
static fs_device_t fs_device;

/*===========================================================================

**  Function :  flash_init_buffer_descriptor

** ==========================================================================
*/
/*!
* 
* @brief
*   This function will initialize the buffer for desriptors.
* 
* @param[in] op_type Buffer-type 
*            page_buff_addr      Main buffer address 
*            spare_buff_addr     Spare buffer address
*            *buffer_desc        Pointer to buffer descriptor
*            *page_data_iovec    Pointer to page-data info structure
*     
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static __inline void
flash_init_buffer_descriptor (uint32 op_type, uint32 page_buff_addr,
                              int32 spare_buff_addr,
                              dalsys_mem_desc_list *buffer_desc,
                              struct flash_page_iovec  *page_data_iovec)
{
  DALSysMemDescBuf *main_desc_buff;
  DALSysMemDescBuf *spare_desc_buff;

  boot_DALFW_MemDescInit (NULL, (DALSysMemDescList *) &page_buff_desc, 2);

  main_desc_buff = boot_DALFW_MemDescBufPtr(buffer_desc, MAIN_BUFF_DESC);
  BL_VERIFY(main_desc_buff != NULL, BL_ERR_NULL_PTR); 
  
  main_desc_buff->VirtualAddr = page_buff_addr;
  main_desc_buff->user = page_data_iovec->total_page_count;

  spare_desc_buff = boot_DALFW_MemDescBufPtr(buffer_desc, SPARE_BUFF_DESC);
  BL_VERIFY(spare_desc_buff != NULL, BL_ERR_NULL_PTR);
  
  spare_desc_buff->VirtualAddr = spare_buff_addr;
  spare_desc_buff->user = page_data_iovec->total_page_count;

  buffer_desc->dwNumDescBufs = 2;

  switch (op_type)
  {
    case FLASH_READ_SPARE:
      spare_desc_buff->size = page_data_iovec->total_page_count *
        page_data_iovec->spare_size_bytes;
      if (page_data_iovec->total_page_count > 1)
      {
        BL_VERIFY(spare_desc_buff->size / page_data_iovec->total_page_count ==
        page_data_iovec->spare_size_bytes, BL_ERR_SBL);
      }
      main_desc_buff->VirtualAddr = DALSYS_MEM_ADDR_NOT_SPECIFIED;
      main_desc_buff->size = 0;

      break;

    case FLASH_READ_MAIN:
      main_desc_buff->size = page_data_iovec->total_page_count *
        page_data_iovec->main_size_bytes;
      if (page_data_iovec->total_page_count > 1)
      {
        BL_VERIFY(main_desc_buff->size / page_data_iovec->total_page_count ==
        page_data_iovec->main_size_bytes, BL_ERR_SBL);
      }
      spare_desc_buff->VirtualAddr = DALSYS_MEM_ADDR_NOT_SPECIFIED;
      spare_desc_buff->size = 0;
    break;

    case FLASH_READ_MAIN_SPARE:
      main_desc_buff->size = page_data_iovec->total_page_count *
        page_data_iovec->main_size_bytes;
      spare_desc_buff->size = page_data_iovec->total_page_count *
        page_data_iovec->spare_size_bytes;
      if (page_data_iovec->total_page_count > 1 )
      {
        if (page_data_iovec->main_size_bytes >= page_data_iovec->spare_size_bytes)
          BL_VERIFY(main_desc_buff->size / page_data_iovec->total_page_count ==
        page_data_iovec->main_size_bytes, BL_ERR_SBL);
        else
          BL_VERIFY(spare_desc_buff->size / page_data_iovec->total_page_count ==
        page_data_iovec->spare_size_bytes, BL_ERR_SBL);
      }
      break;
  }
}

/*===========================================================================
**  Function : flash_initialize_device_structure

** ==========================================================================
*/
/*!
* 
* @brief
*   Initialize the flash device structure.
* 
* @par Dependencies
*   None.
* 
* @retval
*   None.
* 
* @par Side Effects
*   None
*/
void
flash_initialize_device_structure (void)
{
  int i=0;

  /* Initialization is only done on the first call */
  if (flash_device_clients_initialized == FALSE)
  {
    for (i=0; i<FLASH_MAX_CLIENTS; i++)
    {
      flash_device_clients[i].available = TRUE;
    }
    flash_device_clients_initialized = TRUE;
  }
}

/*===========================================================================
**  Function : flash_allocate_device_structure

** ==========================================================================
*/
/*!
* 
* @brief
*   Allocate the flash device structure.
* 
* @par Dependencies
*   None.
* 
* @retval
*   Available device structure. NULL if none is available.
* 
* @par Side Effects
*   None
*/
fs_device_t
flash_allocate_device_structure (void)
{
  int i = 0;
  fs_device_t available_client = NULL;

  /* Check to see if a free slot is available */
  if (flash_client_count <= FLASH_MAX_CLIENTS)
  {
    for (i=0; i<FLASH_MAX_CLIENTS; i++)
    {
      if (flash_device_clients[i].available == TRUE)
      {
        flash_device_clients[i].available = FALSE;
        available_client = &flash_device_clients[i].flash_device_structure;
        flash_client_count++;
        break;
      }
    }
  }
  return available_client;
}

/*===========================================================================
**  Function : flash_release_device_structure

** ==========================================================================
*/
/*!
* 
* @brief
*   Release the device structure.
* 
* @par Dependencies
*   None.
* 
* @retval
*   Frees if the device structure exists.
* 
* @par Side Effects
*   None
*/
void
flash_release_device_structure (fs_device_t device_desc)
{
  int i = 0;

  /* Check to prevent from getting overflow */
  if (flash_client_count != 0)
  {
    for (i=0; i<FLASH_MAX_CLIENTS; i++)
    {
      if (&flash_device_clients[i].flash_device_structure == device_desc &&
          flash_device_clients[i].available == FALSE)
      {
        flash_device_clients[i].available = TRUE;
        flash_client_count--;
        break;
      }
    }
  }
}


/*===========================================================================
**  Function : flash_close

** ==========================================================================
*/
/*!
* 
* @brief
*   Close the flash and free the associated device structure
* 
* @par Dependencies
*    Should always follow a flash_open
* 
* @retval
*   FS_DEVICE_OK             - Success
*   FS_DEVICE_FAILED         - An error occurred.
* 
* @par Side Effects
*   None
*/
int
flash_close (fs_device_t flash_device)
{
  boot_DalDevice_Close((DalDeviceHandle*)flash_device->handle_flash);
  boot_DAL_DeviceDetach((DalDeviceHandle*)flash_device->handle_flash);
  flash_release_device_structure((fs_device_t)flash_device);

  return (int)FS_DEVICE_OK;
}

/***********************************************************************
**  Function :      flash_nand_write_page

** ==========================================================================
*/
/*!
* 
* @brief   This functions writes a page data into the given page
*               on the device by using the Nand Controller
* 
* @par Dependencies
*   None
*
* @retval  FS_DEVICE_DONE if write was successful else FS_DEVICE_FAIL
* 
* @par Side Effects
*   None
*/
int
flash_nand_write_page (fs_device_t self, page_id page, void *data)
{
  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 write_type = FLASH_WRITE_MAIN;

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = 0;
  page_data_iovec.start_page = page;
  page_data_iovec.total_page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(write_type, (uint32) data, NULL,
    (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  ret_status = boot_flash_write_pages(self->handle_flash,
                                      (enum page_write_opcode) write_type,
                                      (DALSysMemDescList *) &page_buff_desc,
                                      &page_data_iovec);

  return ret_status;
} /* End of flash_nand_write_page */

/***********************************************************************
**  Function :      flash_nand_write_page_and_udata

** ==========================================================================
*/
/*!
* 
* @brief   Write page data and spare user data bytes to given page with
*               any hardware/software ECC for spare user data bytes.
* 
* @par Dependencies
*   None
*
* @retval  FS_DEVICE_DONE if write was successful else FS_DEVICE_FAIL
* 
* @par Side Effects
*   None
*/
int
flash_nand_write_page_and_udata (fs_device_t self, page_id page,
  void *page_data, void *spare_udata, uint32 spare_udata_blen)
{

  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 write_type = FLASH_WRITE_MAIN_SPARE;

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = spare_udata_blen;
  page_data_iovec.start_page = page;
  page_data_iovec.total_page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(write_type, (uint32) page_data,
                               (uint32) spare_udata,
                               (DALSysMemDescList *) &page_buff_desc,
                               &page_data_iovec);

  ret_status = boot_flash_write_pages(self->handle_flash,
                                      (enum page_write_opcode) write_type,
                                      (DALSysMemDescList *) &page_buff_desc,
                                      &page_data_iovec);

  if (ret_status != FLASH_DEVICE_DONE)
  {
    ret_status = FS_DEVICE_FAIL;
  }
  return ret_status;
} /* End of flash_nand_write_page_and_udata */

/***********************************************************************
**  Function :      flash_nand_erase_block

** ==========================================================================
*/
/*!
* 
* @brief   This function erases the contents of the given block using
*               the Nand Controller.
*
* @par Dependencies
*   None
*
* @retval  FS_DEVICE_DONE if erase was successful else FS_DEVICE_FAIL
* 
* @par Side Effects
*   None
*/
int
flash_nand_erase_block (fs_device_t self, block_id block)
{
  struct flash_block_vector block_vector;
  int result_vector[1];

  block_vector.start_block = block;
  block_vector.block_count = 1;
  block_vector.result_vector =  (int *)&result_vector[0];

  return (boot_flash_erase_blocks(self->handle_flash, &block_vector, 1));

} /* End of flash_nand_erase_block */

/***********************************************************************
**  Function :      flash_nand_is_page_erased

** ==========================================================================
*/
/*!
* 
* @brief   This function checks if every byte of a given page is
*               erased, using the Nand Controller.
* 
* @par Dependencies
*   None
*
* @retval  TRUE if page is erased
*               FALSE if page is not erased, or any error occured during
*			      checking
* 
* @par Side Effects
*   None
*/
int
flash_nand_is_page_erased (fs_device_t self, page_id page)
{

  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 read_type = FLASH_READ_MAIN;
  DALSysMemDescBuf *desc_buff;
  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = 0;
  page_data_iovec.start_page = page;
  page_data_iovec.total_page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(read_type,
                               (uint32)(self->page_buf), NULL,
                               (DALSysMemDescList *) &page_buff_desc,
                               &page_data_iovec);

  /* For this buffer which the driver has allocated we are responsible for
     setting the Physical Address and not rely of DALFW */
  desc_buff = boot_DALFW_MemDescBufPtr((DALSysMemDescList *) &page_buff_desc,
                                  MAIN_BUFF_DESC);
  BL_VERIFY(desc_buff != NULL, BL_ERR_NULL_PTR);                                  

  desc_buff->PhysicalAddr = desc_buff->VirtualAddr;

  ret_status = (boot_flash_read_pages(self->handle_flash,
                                      (enum page_read_opcode) read_type,
                                      (DALSysMemDescList *) &page_buff_desc,
                                      &page_data_iovec)  ==
                                      FLASH_DEVICE_FAIL_PAGE_ERASED);

  return ret_status;
} /* END flash_nand_is_page_erased */

/***********************************************************************
**  Function :      flash_nand_is_block_erased

** ==========================================================================
*/
/*!
* 
* @brief   This function checks if every byte of every page in a given
*               block is erased, using the Nand Controller.
* 
* @par Dependencies
*   None
*
* @retval  TRUE if block is erased
*               FALSE if block is not erased, or any error occurs during
*                     checking
* 
* @par Side Effects
*   None
*/
int
flash_nand_is_block_erased (fs_device_t self, block_id block)
{
  page_id      start_page_no;
  uint32       index, limit, start;
  int          status = TRUE;
  uint32       pages_per_block;

  start = self->info.partition.physical_start_block;
  limit = self->info.partition.physical_start_block +
              self->info.partition.block_count;
  pages_per_block = self->info.partition.pages_per_block;

  /* If the block number passed in is out of range, return an error */
  if (block >= (limit - start))
  {
    return FALSE;
  }
  start_page_no = (page_id) (block * pages_per_block);

  for (index = 0; index < pages_per_block; index++)
  {
    if (!flash_nand_is_page_erased (self, start_page_no + index))
    {
      status = FALSE;
      break;
    }
  }

  return status;

} /* END flash_nand_is_block_erased */

/***********************************************************************
**  Function :      flash_nand_read_page

** ==========================================================================
*/
/*!
* 
* @brief   This function read page data of the given page into the
*               given buffer using the Nand Controller.
* 
* @par Dependencies
*   None
*
* @retval  FS_DEVICE_DONE if page read was successful or if it was an
*               erased page in a onenand device else
*               FS_DEVICE_FAIL_PAGE_ERASED if page read is erased in a 
*             non-onenand device else
*               FS_DEVICE_FAIL
* 
* @par Side Effects
*   None
*/
int
flash_nand_read_page (fs_device_t self, page_id page, void *data)
{
  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 read_type = FLASH_READ_MAIN;

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = 0;
  page_data_iovec.start_page = page;
  page_data_iovec.total_page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(read_type, (uint32) data, 0,
    (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  ret_status = boot_flash_read_pages(self->handle_flash,
                                     (enum page_read_opcode) read_type,
                                     (DALSysMemDescList *) &page_buff_desc,
                                     &page_data_iovec);

  if (ret_status != FLASH_DEVICE_DONE)
  {
    if ((FLASH_ONENAND == self->info.type) &&
        (FLASH_DEVICE_FAIL_PAGE_ERASED == ret_status))
    {
      ret_status = FLASH_DEVICE_DONE;
    }
	else if (FLASH_DEVICE_FAIL_PAGE_ERASED == ret_status)
    {
      ret_status = FS_DEVICE_FAIL_PAGE_ERASED;
    }
    else
    {
      ret_status = FS_DEVICE_FAIL;
    }
  }
  return ret_status;
} /* End of flash_nand_read_page */

/***********************************************************************
**  Function :      flash_nand_read_spare_udata

** ==========================================================================
*/
/*!
* 
* @brief   Read only the user data in spare location of given page.
*               Data is ECC verified/corrected.
* 
* @par Dependencies
*   None
*
* @retval  FS_DEVICE_DONE if page read was successful or if it was an
*               erased page in a onenand device else
*               FS_DEVICE_FAIL
* 
* @par Side Effects
*   None
*/
int
flash_nand_read_spare_udata (fs_device_t self, page_id page, void *spare_udata,
  uint32 spare_udata_blen)
{
  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 read_type = FLASH_READ_SPARE;
  DALSysMemDescBuf *spare_desc_buff;

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = 0;
  page_data_iovec.spare_size_bytes = spare_udata_blen;
  page_data_iovec.start_page = page;
  page_data_iovec.total_page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(read_type, NULL,
                               (uint32) spare_udata,
                               (DALSysMemDescList *) &page_buff_desc,
                               &page_data_iovec);

  spare_desc_buff = boot_DALFW_MemDescBufPtr((DALSysMemDescList *) &page_buff_desc,
                                        SPARE_BUFF_DESC);
  BL_VERIFY(spare_desc_buff != NULL, BL_ERR_NULL_PTR);
  
  spare_desc_buff->size  = spare_udata_blen;

  ret_status = boot_flash_read_pages(self->handle_flash,
                                     (enum page_read_opcode) read_type,
                                     (DALSysMemDescList *) &page_buff_desc,
                                     &page_data_iovec);

  if (ret_status != FLASH_DEVICE_DONE)
  {
    if ((FLASH_ONENAND == self->info.type) &&
        (FLASH_DEVICE_FAIL_PAGE_ERASED == ret_status))
    {
      ret_status = FLASH_DEVICE_DONE;
    }
    else
    {
      ret_status = FS_DEVICE_FAIL;
    }
  }
  return ret_status;
} /* End of flash_nand_read_spare_udata */

/***********************************************************************
**  Function :      flash_nand_read_page_and_udata

** ==========================================================================
*/
/*!
* 
* @brief   This function read page data and spare bytes of the given
*               page into the given buffer using the Nand Controller.
* 
* @par Dependencies
*   None
*
* @retval  FS_DEVICE_DONE if page read was successful or if it was an
*               erased page in a onenand device else
*               FS_DEVICE_FAIL
* 
* @par Side Effects
*   None
*/
int
flash_nand_read_page_and_udata (fs_device_t self, page_id page, void *data,
  void *spare, uint32 spare_udata_blen)
{
  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  uint32 read_type = FLASH_READ_MAIN_SPARE;
  struct flash_page_iovec page_data_iovec;

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;;
  page_data_iovec.spare_size_bytes = spare_udata_blen;
  page_data_iovec.start_page = page;
  page_data_iovec.total_page_count = 1;

    /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(read_type,  (uint32) data,
       (uint32)spare, (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  ret_status = boot_flash_read_pages(self->handle_flash,
                                     (enum page_read_opcode) read_type,
                                     (DALSysMemDescList *) &page_buff_desc,
                                     &page_data_iovec);

  if (ret_status != FLASH_DEVICE_DONE)
  {
    if ((FLASH_ONENAND == self->info.type) &&
        (FLASH_DEVICE_FAIL_PAGE_ERASED == ret_status))
    {
      ret_status = FLASH_DEVICE_DONE;
    }
    else
    {
      ret_status = FS_DEVICE_FAIL;
    }
  }
  return ret_status;
} /* End of flash_nand_read_page_and_udata */

/***********************************************************************
**  Function :      flash_nand_read_spare_bytes

** ==========================================================================
*/
/*!
* 
* @brief   Read len bytes from spare area starting from spare_offset
*               of given page. No ECC is checked on the data read.
* 
* @par Dependencies
*   None
*
* @retval  FS_DEVICE_OK if successful else
*               FS_DEVICE_FAIL
* 
* @par Side Effects
*   None
*/
int
flash_nand_read_spare_bytes (fs_device_t self, page_id page,
  uint32 spare_offset, uint32 len, void *data)
{


  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  uint32 read_type = FLASH_READ_SPARE;
  struct flash_page_iovec page_data_iovec;

  (void) spare_offset;
  (void) self;

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = 0;
  page_data_iovec.spare_size_bytes = len;
  page_data_iovec.start_page = page;
  page_data_iovec.total_page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(read_type, NULL,  (uint32)data,
     (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  ret_status = boot_flash_read_pages(self->handle_flash,
                                     (enum page_read_opcode) read_type,
                                     (DALSysMemDescList *) &page_buff_desc,
                                     &page_data_iovec);

  if (ret_status != FLASH_DEVICE_DONE)
  {
    if ((FLASH_ONENAND == self->info.type) &&
        (FLASH_DEVICE_FAIL_PAGE_ERASED == ret_status))
    {
      ret_status = FLASH_DEVICE_DONE;
    }
    else
    {
      ret_status = FS_DEVICE_FAIL;
    }
  }
  return ret_status;
} /* End of flash_nand_read_spare_bytes */

/***********************************************************************
**  Function :      flash_nand_lock_ops

** ==========================================================================
*/
/*!
* 
* @brief   This function determines the offset of bad block flag, in the
*               Code Word correctly based on flash width and page_size.
* 
* @par Dependencies
*   None
*
* @retval   
*               Returns FS_DEVICE_FAIL if operation fails or inputs out of range
*               Returns FS_DEVICE_OK otherwise
* 
* @par Side Effects
*   None
*/
int
flash_nand_lock_ops (fs_device_t self, block_id block_start,
  block_id block_end, uint32 opcode)
{
  (void) self;
  (void) block_start;
  (void) block_end;
  (void) opcode;

  return FS_DEVICE_DONE;
} /* End of flash_nand_lock_ops */

/***********************************************************************
**  Function :      flash_nand_read_mpages

** ==========================================================================
*/
/*!
* 
* @brief   This function reads num_pages from start_page into location
*               pointed to by data. All the pages read at a time SHOULD fall
*               within a single good block
* 
* @par Dependencies  It calls the wdog_kick_fn_ptr, if not set to NULL, after
*               every individual page read. This function also does not handle
*               any bad block check and assumes all pages from start_page to
*               (start_page + num_pages) are within a single good block.
*
* @retval  
*               Returns FS_DEVICE_FAIL if operation fails or inputs out of range
*               Returns FS_DEVICE_FAIL_PAGE_ERASED if page read is erased
*               Returns FS_DEVICE_OK otherwise
* 
* @par Side Effects
*   None
*/
int
flash_nand_read_mpages (fs_device_t self, page_id start_page,
                        page_id num_pages,
                        void *data, void (*wdog_kick_fn_ptr)(void))
{

  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 read_type = FLASH_READ_MAIN;

  (void) wdog_kick_fn_ptr;

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = 0;
  page_data_iovec.start_page = start_page;
  page_data_iovec.total_page_count = num_pages;


  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(read_type, (uint32) data, 0,
     (DALSysMemDescList *) &page_buff_desc,  &page_data_iovec);

  ret_status = boot_flash_read_pages(self->handle_flash,
                                     (enum page_read_opcode) read_type,
                                     (DALSysMemDescList *) &page_buff_desc,
                                     &page_data_iovec);

  if (ret_status != FLASH_DEVICE_DONE)
  {
    if ((FLASH_ONENAND == self->info.type) &&
        (FLASH_DEVICE_FAIL_PAGE_ERASED == ret_status))
    {
      ret_status = FLASH_DEVICE_DONE;
    }
	else if (FLASH_DEVICE_FAIL_PAGE_ERASED == ret_status)
    {
      ret_status = FS_DEVICE_FAIL_PAGE_ERASED;
    }   
    else
    {
      ret_status = FS_DEVICE_FAIL;
    }
  }
  return ret_status;
} /* End of flash_nand_read_mpages */

/***********************************************************************
**  Function :      flash_nand_bad_block_check

** ==========================================================================
*/
/*!
* 
* @brief   This function finds the state of a block
* 
* @par Dependencies  None
*
* @retval  
*               Returns FS_DEVICE_BAD_BLOCK if block state is bad              
*               Returns FS_DEVICE_OK otherwise
* 
* @par Side Effects
*   None
*/
static int
flash_nand_bad_block_check (fs_device_t self, block_id block)
{
  int          result = FS_DEVICE_OK;    /* return status of block check */
  enum flash_block_state block_state;

  BL_VERIFY(DAL_SUCCESS == boot_flash_block_get_state(self->handle_flash,
                                                      block, &block_state),
            BL_ERR_BOOT_FLASH_ACCESS);
  if (FLASH_BLOCK_BAD == block_state)
  {
      result = FS_DEVICE_BAD_BLOCK;
  }
  return result;
} /* End of flash_nand_bad_block_check */

/***********************************************************************
**  Function :      flash_nand_copy_back_write_page

** ==========================================================================
*/
/*!
* 
* @brief   This function supports the copy back write from src_page to
*               dest_page without doing any copy from NAND buffer to RAM.
* 
* @par Dependencies
*   None
*
* @retval  
*               Returns FS_DEVICE_FAIL if opeartion fails or inputs out of range
*               Returns FS_DEVICE_OK otherwise
* 
* @par Side Effects
*   None
*/
int
flash_nand_copy_page (fs_device_t self, page_id src_page, page_id dest_page)
{
  struct flash_copy_iovec copy_page_iovec;

  /* If the block number passed in is out of range, return an error */
  if ((src_page >= (self->info.partition.block_count *
                    self->info.partition.pages_per_block)) ||
      (dest_page >= (self->info.partition.block_count *
                     self->info.partition.pages_per_block)))
  {
    return FS_DEVICE_FAIL;
  }

  /* if src and dest page are the same, just return operation done */
  if (src_page == dest_page)
  {
    return FS_DEVICE_OK;
  }

  copy_page_iovec.src_page = src_page;
  copy_page_iovec.dest_page = dest_page;
  copy_page_iovec.copy_count = 1  ;  /* MONA TO CHECK ON THIS */

  boot_flash_copy_pages(self->handle_flash, &copy_page_iovec, 1);

  return FS_DEVICE_OK;
} /* End of flash_nand_copy_page */

/***********************************************************************
**  Function :      flash_nand_get_partition_phy_limits

** ==========================================================================
*/
/*!
* 
* @brief   Return Physical boundaries of a partition.
* 
* @par Dependencies
*   None
*
* @retval  Return FS_DEVICE_FAIL if the fs_device is NULL
*               Return FS_DEVICE_OK if the fs_device is not NULL and
*               partition boundaries start and limit have been populated
*               rsvd* is an unused pointer
* 
* @par Side Effects
*   None
*/
int
flash_nand_get_partition_phy_limits (fs_device_t self, uint32 *start,
  uint32 *limit, void *rsrvd)
{
   int status = FS_DEVICE_OK;

   (void) rsrvd;
   if (self->handle_flash != NULL)
   {
     *start = self->info.partition.physical_start_block;
     *limit = self->info.partition.physical_start_block +
              self->info.partition.block_count;
     status = FS_DEVICE_OK;
   }
   else
   {
     status = FS_DEVICE_FAIL;
   }
   return status;
}

/***********************************************************************
**  Function :      flash_nand_get_partition_attribs

** ==========================================================================
*/
/*!
* 
* @brief   This function returns partition attrubute stored
*               in private data structure.
* 
* @par Dependencies
*   None
*
* @retval  Return FS_DEVICE_FAIL if the fs_device is NULL
*               Return FS_DEVICE_OK if the fs_device is not NULL and
*               varuiable attribute has been successfully populated. 
* 
* @par Side Effects
*   None
*/
int
flash_nand_get_partition_attribs(fs_device_t self, uint32 *attribute)
{
  int status = FS_DEVICE_OK;

   if (self->handle_flash != NULL)
   {
     *attribute =   self->info.partition.attributes;
   }
   else
   {
     status = FS_DEVICE_FAIL;
   }
   return status;
}

/***********************************************************************
**  Function :      various generic functions

** ==========================================================================
*/
/*!
* 
* @brief   Each function returns a member of the private data
*               structure to the caller.  Type are various and are the
*               return type of the function.  They cannot fail.
* 
* @par Dependencies
*   None
*
* @retval  char * for device name
*          int/uint32 for id/count/type functions
* 
* @par Side Effects
*   None
*/
char *
flash_nand_device_name (fs_device_t self)
{
  return  (char *)self->info.device_name;
} /* End of flash_nand_device_name */

int
flash_nand_maker_id (fs_device_t self)
{
  return  self->info.maker_id;
} /* End of flash_nand_maker_id */

int
flash_nand_device_id (fs_device_t self)
{
  return  self->info.device_id;
} /* End of flash_nand_device_id */

uint32
flash_nand_block_count (fs_device_t self)
{
  return  self->info.partition.block_count;
} /* End of flash_nand_block_count */

uint32
flash_nand_block_size (fs_device_t self)
{
  return  self->info.partition.pages_per_block;
} /* End of flash_nand_block_size */

uint32
flash_nand_page_size (fs_device_t self)
{
  return  self->info.partition.page_size_bytes;
} /* End of flash_nand_page_size */

uint32
flash_nand_total_page_size (fs_device_t self)
{
  return  self->info.partition.total_page_size_bytes;
} /* End of flash_nand_total_page_size */

uint32
flash_nand_device_type (fs_device_t self)
{

  return  self->info.type;
} /* End of flash_nand_device_type */

uint32
flash_nand_device_width (fs_device_t self)
{
  return self->info.width;
} /* End of flash_nand_device_width*/

/***********************************************************************
**  Function :      flash_nand_get_write_style

** ==========================================================================
*/
/*!
* 
* @brief
*               Always return device write style as PAGED for NAND
*
* @par Dependencies
*   None
*
* @retval  FS_DEVICE_WRITES_PAGED
* 
* @par Side Effects
*   None
*/
fs_device_write_style_t
flash_nand_get_write_style (fs_device_t self)
{
  (void) self;
  return FS_DEVICE_WRITES_PAGED;
}

/***********************************************************************
**  Function :      flash_nand_get_spare_corrected_bcount

** ==========================================================================
*/
/*!
* 
* @brief   This function returns total number of bytes available in the
*               NAND spare area that can be corrected by the controller via
*               some ECC algorithm, if any.
* 
* @par Dependencies
*   None
*
* @retval  Number of bytes of spare area that can be corrected via ECC
* 
* @par Side Effects
*   None
*/
int
flash_nand_get_spare_corrected_bcount (fs_device_t self)
{
  return  self->info.partition.max_spare_udata_bytes;
}
/*
**  Function : flash_open

** ==========================================================================
*/
/*!
* 
* @brief
*   Probe for the flash and open the requested partition if a partition table
*   exists.
* 
* @par Dependencies
*   Must be done before any operations are carried out on the flash hardware.
* 
* @retval
*   Device structure is returned. In case of error, the return value will be
*   NULL.
* 
* @par Side Effects
*   None
*/
fs_device_t flash_open(char *partition_name, int whichflash)
{
  uint32 nand_page_buf = 0;
  DALSYSMemHandle  handle_mem;   /* HAL Workspace mem handle */
  DALSYSMemInfo    mem_info;     /* HAL Workspace mem info */

  (void) whichflash;
  /* Initialize the client device structure array */
  flash_initialize_device_structure();

  /* Find the free slot availble */
  fs_device = flash_allocate_device_structure();
  if (!(fs_device))
  {
    return NULL;
  }
  
  /* Initialization is only done on the first call */
  boot_DALSYS_InitMod(NULL);
  if(DAL_SUCCESS != boot_DAL_DeviceAttach(DALDEVICEID_FLASH_DEVICE_1, //Device Id
                                          (DalDeviceHandle **)&(fs_device->handle_flash))) //Handle
  {
    /* Release the allocated structure when open fails */  
    flash_release_device_structure(fs_device);
    return NULL;
  }
  //Open device...
  if(DAL_SUCCESS == boot_flash_open_partition(fs_device->handle_flash,
                                       (const unsigned char *)partition_name))
  {
    /* Device ops table common to any NAND like device */
    /* Device Name                    */
    fs_device->device_name               = flash_nand_device_name;

    /* Maker Identification           */
    fs_device->device_maker_id           = flash_nand_maker_id;

    /* Device Identification          */
    fs_device->device_id                 = flash_nand_device_id;

    /* Block Count                    */
    fs_device->block_count               = flash_nand_block_count;

    /* Pages in block                 */
    fs_device->block_size                = flash_nand_block_size;

    /* Page Size                      */
    fs_device->page_size                 = flash_nand_page_size;

    /* Total Page Size                */
    fs_device->total_page_size           = flash_nand_total_page_size;

    /* Device Type                    */
    fs_device->device_type               = flash_nand_device_type;

    /* Device Width                    */
    fs_device->device_width              = flash_nand_device_width;

    /* Bad Block Check                */
    fs_device->bad_block_check           = flash_nand_bad_block_check;

    /* Mark Bad Block                 */
    fs_device->mark_block_bad            = 0;

    /* Write Page                     */
    fs_device->write_page                = flash_nand_write_page;

    /* Erase Block                    */
    fs_device->erase_block               = flash_nand_erase_block;

    /* Read Page                      */
    fs_device->read_page                 = flash_nand_read_page;

    /* Is Page Erased                 */
    fs_device->is_erased                 = flash_nand_is_page_erased;

    /* Partial Write                  */
    fs_device->partial_write             = 0;

    /* Begin Erase Block              */
    fs_device->begin_erase_block         = 0;

    /* Suspend Erase                  */
    fs_device->suspend_erase             = 0;

    /* Resume Erase                   */
    fs_device->resume_erase              = 0;

    /* Erase Status                   */
    fs_device->erase_status              = 0;

    /* Is Block erased                */
    fs_device->is_block_erased           = flash_nand_is_block_erased;

    /* Read spare bytes               */
    fs_device->read_spare_bytes          = flash_nand_read_spare_bytes;

    /* Read multiple pages            */
    fs_device->read_mpages               = flash_nand_read_mpages;

    /* Copy page                      */
    fs_device->copy_page                 = flash_nand_copy_page;

    /* Lock Unlock LockTight          */
    fs_device->lock_ops                  = flash_nand_lock_ops;

    /* Write Page & Spare udata       */
    fs_device->write_page_and_udata      = flash_nand_write_page_and_udata;

    /* Read spare user data           */
    fs_device->read_spare_udata          = flash_nand_read_spare_udata;

    /* Read Page and spare user data  */
    fs_device->read_page_and_udata       = flash_nand_read_page_and_udata;

    /* Open Partition   */
    fs_device->open_partition            = NULL;

    /* Get partition physical limits  */
    fs_device->get_partition_phy_limits  = flash_nand_get_partition_phy_limits;

    /* Get partition attributes       */
    fs_device->get_partition_attribs     = flash_nand_get_partition_attribs;

    /* Set ECC state */
    fs_device->set_ecc_state             =  0;

    fs_device->get_spare_corrected_bytes =
      flash_nand_get_spare_corrected_bcount;

    (void)boot_flash_get_info(fs_device->handle_flash, FLASH_DEVICE_INFO,
      &(fs_device->info));

    if (DAL_SUCCESS == boot_DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED |
        DALSYS_MEM_PROPS_PHYS_CONT, DALSYS_MEM_ADDR_NOT_SPECIFIED,
        DALSYS_MEM_ADDR_NOT_SPECIFIED, fs_device->info.partition.total_page_size_bytes,
        &handle_mem, NULL))
      {
        /* If the allocation is success, get the memory address
           information */
        boot_DALSYS_MemInfo(handle_mem, &mem_info);

        nand_page_buf = (uint32) mem_info.VirtualAddr;
        fs_device->page_buf = (uint8 *) nand_page_buf;
        /*  Need to touch the memory  for it  to be configured in
            MMU table for ARM11 */
        memset((void *)nand_page_buf, 0xFF, 10);
      }

      /* need to have at least two entries in buff desc: one for main and the
       * other for spare */
      if (FALSE == buff_desc_initialized)
      {
        boot_DALFW_MemDescInit(NULL, (DALSysMemDescList *) &page_buff_desc, 2);
        buff_desc_initialized = TRUE;
      }

      return (fs_device_t) fs_device;
  }
  else
  {
    /* Release the allocated structure when open fails */
    flash_release_device_structure(fs_device);
    return NULL;
  }

}
