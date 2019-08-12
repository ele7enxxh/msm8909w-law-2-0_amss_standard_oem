/*===========================================================================

                    BOOT CONFIG DATA TABLE UPDATE FUNCTION

DESCRIPTION
  Contains function to update config data table with data stored in EEPROM

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_config_eeprom.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/15   sk      Added verify condition check for boot_copy_config_data_block fails.
01/22/15   aus     Added integer overflow checks 
09/18/14   lm      Add a check for EEPROM read failure based on feature flag
09/16/14   jz      Added boot statistics for CDT read from EEPROM
08/28/14   sk      8909 SBL NAND changes
12/20/13   kpa     Support reading data from secondary EEPROM.
12/05/12   jz      Renamed and keep EEPROM specific code here
08/06/12   dh      Add a check for EEPROM read failure based on feature flag
05/02/12   dh      Change page_buf as static variable
10/24/11   dh      Add logic to only update default cdt array's platform id block
                   if eeprom cdt has no DDR data block. Move EEPROM_DEVICE_ID to
                   boot_target.h
09/09/11   dh      Map all dal and flash driver calls to boot extern interface.
                   Change flash_device_close to DalDevice_Close,
                          flash_device_attach to DAL_DeviceAttach,
                          flash_device_detach to DAL_DeviceDetach,
                   since those are mapped in macros from flash.h
08/04/11   aus     Fixed the warning due to wrong use of the escape character
07/29/11   aus     Updated Flash Device ID and added support for requirement to 
                   error fatal in case on unprogrammed EEPROM
01/21/10   dh      fixed klocwork warning
10/18/10   dh      Added boot logger
09/21/10   dh      switched to flash driver API instead of DALI2C
09/16/10   dh      Change read size to 1, eeprom offset to 0
09/10/10   dh      Initialize CDT to default value
09/08/10   dh      Add logic to read EEPROM
08/19/10   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_config_data.h"
#include "boot_util.h"
#include "boot_error_if.h"
#include "boot_sbl_if.h"
#include "boot_extern_dal_interface.h"
#include "boot_extern_flash_interface.h"
#include "DALDeviceId.h"
#include "boot_logger.h"
#include "boot_target.h"
#include "boot_statistics.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/**
*  max size of the buffer to hold one page
*/
#define EEPROM_MAX_PAGE_BUF_SIZE 32

/**
*  size of the cdt header in bytes
*/
#define CDT_HEADER_SIZE sizeof(struct cdt_header)


/**
*  Number of EEPROM pages we read in the first read operation
*  It is set to 2 which grantees that PlatformID data can be
*  retrieved into the buffer at the first read.
*/
#define INIT_READ_NUM_PAGE 2

/**
*eeprom device handler
*/
static flash_handle_t eeprom_dev_handle;

/**
*  boolean that indicates the device is initialized or not.
*  Target may have multiple EEPROM instances. Only one instance
*  can be initialized, accessed at a time.
*/
static boot_boolean eeprom_dev_initialized = FALSE;

/**
*size of a eeprom page in bytes
*/
static uint32 eeprom_page_size;

/**
* DAL buffer descriptor list for reading eeprom 
*/
static DALSysMemDescList eeprom_buff_desc_list;

/**
* I/O vector for reading eeprom page
*/
static struct flash_page_iovec eeprom_page_iovec;

/*buffer to hold 2 eeprom page, we need it to read the 
  magic number and platform ID data block
  this buffer must be 4 bytes aligned per flash driver requirement*/
__align(4) static uint8 page_buf[EEPROM_MAX_PAGE_BUF_SIZE*INIT_READ_NUM_PAGE];


#if (!defined(BOOT_PRE_SILICON)) && defined(BOOT_ERROR_ON_CDT_READ_FAIL)
static volatile boot_boolean eeprom_error_on_read_fail = TRUE;
#else
static volatile boot_boolean eeprom_error_on_read_fail = FALSE;
#endif

/*===========================================================================

**  Function :  boot_eeprom_dev_init

** ==========================================================================
*/
/*!
* 
* @brief
*    Initialize the eeprom device.
* 
* @param[in] DALDEVICEID  Device ID for eeprom to be initialized
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if initialization is successful, FALSE if it fails
* 
* @par Side Effects
*   None
*/
static boot_boolean boot_eeprom_dev_init(DALDEVICEID DeviceId)
{
  boot_boolean status = FALSE;
  struct flash_info eeprom_dev_info;
  
  do
  {
    if(TRUE == eeprom_dev_initialized)
    {
      /*An eeprom instance is already being used. Cannot open multiple
        instances simultaneously, hence return 
      */
      break;
    }
    /*first attach the device*/
    if(boot_DAL_DeviceAttach(DeviceId,(DalDeviceHandle **)&eeprom_dev_handle) 
       != DAL_SUCCESS)
    {
      break;
    }
    /*then open the partition*/
    if(boot_flash_open_partition(eeprom_dev_handle,"0:ALL") != DAL_SUCCESS)
    {
      boot_DAL_DeviceDetach((DalDeviceHandle*)eeprom_dev_handle);
      break;
    }
    /* get the info of eeprom*/
    if(boot_flash_get_info(eeprom_dev_handle, FLASH_DEVICE_INFO, &eeprom_dev_info) !=
       DAL_SUCCESS ||
       /*check to see if a eeprom page fits in our buffer*/
       eeprom_dev_info.partition.total_page_size_bytes > 
       EEPROM_MAX_PAGE_BUF_SIZE)
    {
      boot_DalDevice_Close((DalDeviceHandle*)eeprom_dev_handle);
      boot_DAL_DeviceDetach((DalDeviceHandle*)eeprom_dev_handle);
      break;
    }
    
    /*set the eeprom page size*/
    eeprom_page_size = eeprom_dev_info.partition.total_page_size_bytes;  
    /*init the buffer descriptor*/
    boot_DALFW_MemDescInit(NULL, &eeprom_buff_desc_list, 1);
    eeprom_dev_initialized = TRUE;
    status = TRUE;
  
  }while(0);
  
  return status;
}


/*===========================================================================

**  Function :  boot_eeprom_dev_deinit

** ==========================================================================
*/
/*!
* 
* @brief
*    Deinitialize the eeprom device.
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
static void boot_eeprom_dev_deinit(void)
{
  if(eeprom_dev_initialized == TRUE)
  {
    boot_DalDevice_Close((DalDeviceHandle*)eeprom_dev_handle);
    boot_DAL_DeviceDetach((DalDeviceHandle*)eeprom_dev_handle);
    eeprom_dev_initialized = FALSE;
  }
}


/*===========================================================================

**  Function :  boot_eeprom_set_read_info

** ==========================================================================
*/
/*!
* 
* @brief
*    Set all the required information before calling boot_eeprom_read.
*    This function must be called prior to call boot_eeprom_read
*
* @param[in] uint32 the first page to read
*
* @param[in] uint32 the last page to read 
*
* @param[in] uint32 the buffer address
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the read is successful, FALSE if the read fails
* 
* @par Side Effects
*   None
*/
static void boot_eeprom_set_read_info
(
  uint32 start_page, 
  uint32 page_count, 
  uint32 buff_addr
)
{
  DALSysMemDescBuf *desc_buff;
  
  /*set the io vector with page information*/
  eeprom_page_iovec.main_size_bytes = eeprom_page_size;
  eeprom_page_iovec.spare_size_bytes = 0;
  eeprom_page_iovec.start_page = start_page;
  eeprom_page_iovec.total_page_count = page_count;
  
  /*set the buffer descriptor with buffer information*/
  desc_buff = boot_DALFW_MemDescBufPtr(&eeprom_buff_desc_list, 0);
  if(desc_buff != NULL)
  {
    desc_buff->VirtualAddr = buff_addr;
    desc_buff->PhysicalAddr = buff_addr;
    desc_buff->size = eeprom_page_size * page_count;
    desc_buff->user = 1;
  }
}


/*===========================================================================

**  Function :  boot_eeprom_read

** ==========================================================================
*/
/*!
* 
* @brief
*    copy a number of pages from eeprom to buffer, it will inclusively copy 
*    all the data from start_page to end_page. If end_page is smaller than 
*    start_page it will return true and not copy any data.
*    boot_eeprom_set_read_info must be called first to set up the buffer descriptor
*
* @param[in] uint32 the first page to read
*
* @param[in] uint32 the last page to read 
*
* @param[in] uint8* the buffer to read the pages into  
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the read is successful or end page is smaller than start page 
*   so no data to read, FALSE if the read fails in
*   flash driver
* 
* @par Side Effects
*   None
*/
static boot_boolean boot_eeprom_read
(
  uint32 start_page, 
  uint32 end_page, 
  uint8 *buffer
)
{
  boot_boolean result = TRUE;
  /*only read if end page is bigger or equal than start page*/
  if(end_page >= start_page)
  {
    boot_eeprom_set_read_info(start_page, 
                              end_page - start_page + 1, 
                              (uint32)buffer);
  
    if(boot_flash_read_pages(eeprom_dev_handle,
     /*FLASH_READ_MAIN will set the flash io mode to read one page*/
                             FLASH_READ_MAIN,
                             &eeprom_buff_desc_list, 
                             &eeprom_page_iovec) 
      != FLASH_DEVICE_DONE)
    {
      result = FALSE;
    }
  }

  return result;
}


/*===========================================================================

**  Function :  get_config_data_table

** ==========================================================================
*/
/*!
* 
* @brief
*    Read the cdt from EEPROM and update the default config_data_table array
* 
* @param[in] cdt_info* pointer to cdt info
*   
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void boot_update_config_data_table(struct cdt_info *cdt_info_ptr)
{ 
  uint32 cdb_total_length = 0;
  uint32 cdb_start_offset = 0;
  uint32 cdb_first_page = 0;
  uint32 cdb_last_page = 0;
  uint8 *cdt_offset = 0;
  uint8 *platform_id_cdb = NULL;
  uint32 platform_id_cdb_size = 0;
  struct cdb_meta *cdb_meta_ptr = NULL;
  
  /*Counter for the next EEPROM page we haven't read yet*/
  uint32 next_page_to_read = 0;
  
  /* Start flash statistic timer. */
  boot_statistics_flash_read_start();

  do
  {
    /* init eeprom device.*/
    if(boot_eeprom_dev_init(EEPROM_DEVICE_ID) != TRUE)
    {
      break;
    }

    /*first we read the first 2 pages and check the magic number,
      if error happens at reading EEPROM ,
      we use the default CDT in config_data_table*/
    if(boot_eeprom_read(0, INIT_READ_NUM_PAGE - 1, page_buf) != TRUE)
    {
      /* Only error on read failure if flag is set */
      if(eeprom_error_on_read_fail == TRUE)
      {
        boot_log_message("Error: EEPROM read fail");
        CDT_BL_ERROR(BL_ERR_GET_CDT_FAIL);        
      }
      break;
    }

    /* we then check the magic number. if the EEPROM is not programmed,
      enter error handler */
    if(((struct cdt_header *)page_buf)->magic != CONFIG_DATA_MAGIC)
    {
		/* if the magic number is wrong and the eeprom_error_on_read_fail flag is set to false
		 then fall back to default CDT */
		if(eeprom_error_on_read_fail == TRUE)
		{
			boot_log_message("Error: Platform ID EEPROM is not programmed");	
			CDT_BL_ERROR(BL_ERR_GET_CDT_FAIL);
		}
		break;
    }
    
    /* now check if the first offset to cdb points to valid address. 
       if it does not, enter error handler */
    cdb_meta_ptr = (struct cdb_meta *)(page_buf + CDT_HEADER_SIZE);
    cdb_start_offset = cdb_meta_ptr->offset;
    if(cdb_start_offset == 0 || cdb_start_offset >= CONFIG_DATA_TABLE_MAX_SIZE)
    {
      boot_log_message("Error: Platform ID EEPROM is not programmed");
      CDT_BL_ERROR(BL_ERR_GET_CDT_FAIL);
    }
    
    /*
      There cases possible:
      1. EEPROM's DDR meta data doesn't exist.
      2. EEPROM's DDR meta data exists but size is 0.
      3. EEPROM'S DDR meta data exists and size is non-zero.
      
      For case 1 and 2 we copy over the platform id data from EEPROM CDT to default CDT
      and stop reading EEPROM CDT
      For case 3 we retrieve the entire EEPROM CDT
    */
    if(!((cdb_start_offset != (CDT_HEADER_SIZE + sizeof(struct cdb_meta))) &&
        (cdb_meta_ptr + 1)->size != 0) )
    {
      /* Case 1 and 2: 
         Only copy the first block data(Platform ID) to the default CDT and
         stop processing the rest of EERPOM, use the default CDT instead*/

      /* Retrieve the platform ID data from the buffer*/
      platform_id_cdb = boot_get_config_data_block(page_buf, 
                                                   CONFIG_DATA_BLOCK_INDEX_V1_PLATFORM_ID,
                                                   &platform_id_cdb_size);
                                                   
      /* Copy the platform ID data into default CDT*/                                             
      if(TRUE == boot_copy_config_data_block(cdt_info_ptr->cdt_ptr, 
                                             CONFIG_DATA_BLOCK_INDEX_V1_PLATFORM_ID, 
                                             platform_id_cdb,
                                             platform_id_cdb_size))
      { 
         break;
      }
	  else
	  {
	   BL_VERIFY(FALSE, BL_ERR_OTHERS);
	  }
    }
    
    /* Case3: copy the entire EEPROM CDT to default CDT */
    
    /*now copy the header and block's meta info, we assume 
     the first block will have nonzero offset which points to the beginning 
     of cdb data each block meta data is 32 bits with 16 bits to represent 
     the offset to the first byte of that block's data 
     and 16 bits to represent the total size of that block's data in bytes.
     first block's meta data's offset points to the beginning byte of
     block data section. The area in between the end of header and  
     the beginning byte of block data section holds the meta data of all blocks*/      
    qmemcpy(cdt_info_ptr->cdt_ptr, page_buf, eeprom_page_size * INIT_READ_NUM_PAGE);
    
     /*Update the next EEPROM page number we need to read*/
    next_page_to_read = INIT_READ_NUM_PAGE;
    cdt_offset = cdt_info_ptr->cdt_ptr + next_page_to_read * eeprom_page_size;
    
    /*get the first block's meta data*/
    cdb_meta_ptr = (struct cdb_meta *)(cdt_info_ptr->cdt_ptr + CDT_HEADER_SIZE);
    
    /*get the offset to the first byte of cdb, which is the offset value
      of the first block's meta data*/
    cdb_start_offset = cdb_meta_ptr->offset;
    
    /*this is the first page that cdb starts*/
    cdb_first_page = cdb_meta_ptr->offset/eeprom_page_size;
    
    /*Read up to cdb_fist_page if necessary*/
    if(cdb_first_page >= next_page_to_read)
    {
      /*copy all the meta data up to the first page of cdb to our cdt array,
        we've already copied page 0 and 1 so we start at page 2.*/
      if(boot_eeprom_read(next_page_to_read, cdb_first_page, cdt_offset) != TRUE)
      {
        break;
      }
      
      /*Update the next EEPROM page number we need to read*/
      next_page_to_read = cdb_first_page + 1;
      cdt_offset = cdt_info_ptr->cdt_ptr + next_page_to_read * eeprom_page_size;
    }
    
    /*sum up the sizes of all blocks to get the total size of cdb 
     keep looping until cdb_meta_ptr reaches the first byte of cdb*/
    while(((uint32)cdb_meta_ptr - (uint32)cdt_info_ptr->cdt_ptr) < cdb_start_offset)
    {
      /* Integer overflow check */
      BL_VERIFY((cdb_total_length <= (cdb_total_length + cdb_meta_ptr->size)), BL_ERR_IMG_SIZE_INVALID);

      cdb_total_length += cdb_meta_ptr->size;
      cdb_meta_ptr++;
    }
    
    /* Integer overflow check */
    BL_VERIFY((cdb_total_length <= (cdb_start_offset + cdb_total_length)), BL_ERR_IMG_SIZE_INVALID);

    /*this is the last page of cdb*/
    cdb_last_page = (cdb_start_offset + cdb_total_length)/eeprom_page_size;

    /*make sure the last page of cdb is within the range of CDT*/
    if(cdb_last_page >= CONFIG_DATA_TABLE_MAX_SIZE/eeprom_page_size)
    {
      break;
    }
    
    /* copy the rest of cdb pages to the CDT array, 
       start at cdb_first_page+1, end at cdb_last_page*/
    if(boot_eeprom_read(next_page_to_read, cdb_last_page, cdt_offset) != TRUE)
    {
      break;
    }
    
    /*update the total size of the cdt*/
    cdt_info_ptr->size = cdb_start_offset + cdb_total_length;
  
  }while(0);
  
  boot_eeprom_dev_deinit();

  boot_statistics_flash_read_stop(cdt_info_ptr->size); 
}

/*===========================================================================

**  Function :  boot_get_eepromII_info

** ==========================================================================
*/
/*!
* 
* @brief
*  Read the data from Secondary EEPROM and copy it to input buffer.
* 
* @param[in] uint8*  pointer to data buffer, to which info needs to be
*                    populated
*   
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void boot_get_eepromII_info(uint8* eepromII_data_buff_ptr, uint32 eepromII_data_size)
{ 
  uint32 start_page = 0;
  uint32 pages_to_read = 0;
  
  do
  {
    if(eepromII_data_size == 0 || eepromII_data_buff_ptr == NULL)
    {
      /*Proceed only for non-zero data size and buffer pointer */
      break;
    }
    
    /* init eeprom device.*/
    if(boot_eeprom_dev_init(EEPROM_DEVICE_ID_SECONDARY) != TRUE)
    {
      if(eeprom_error_on_read_fail == TRUE)
      {
        /*Do not error out. Only log the error */
        boot_log_message("Error: EEPROMII init fail");
      }
      break;
    }

    /* Calculate number of pages to be read */
    pages_to_read = ((eepromII_data_size - 1)/ eeprom_page_size) + 1;
    
    if(pages_to_read > 1)
    {
      /* Read upto second last page, directly to destinition buffer */
      if(boot_eeprom_read(0, pages_to_read - 2, eepromII_data_buff_ptr) != TRUE)
      {
        /* Only error on read failure if flag is set */
        if(eeprom_error_on_read_fail == TRUE)
        {
          /*Do not error out. Only log the error */
          boot_log_message("Error: EEPROMII read fail");
        }
        break;
      }
      eepromII_data_buff_ptr += (pages_to_read - 1)*eeprom_page_size;
      start_page = (pages_to_read - 1);
      eepromII_data_size = eepromII_data_size - ((pages_to_read - 1)*eeprom_page_size);
    }
      
    /* Read last page (or the only page if pages_to_read is 1) to temp buffer and 
       copy remaining data to destinition
    */
    if(boot_eeprom_read(start_page, start_page, page_buf) != TRUE)
    {
      /* Only error on read failure if flag is set */
      if(eeprom_error_on_read_fail == TRUE)
      {
        /*Do not error out. Only log the error */
        boot_log_message("Error: EEPROMII read fail");
      }
      break;
    }

    qmemcpy(eepromII_data_buff_ptr, page_buf, eepromII_data_size);
  
  }while(0);
  boot_eeprom_dev_deinit();  

}
