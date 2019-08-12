/*=============================================================================

                     Boot eMMC Hotplug Flash Device Module

GENERAL DESCRIPTION
  Contains the implementation for the boot MMC flash device module.

Copyright 2014,2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/10/15   sk      Added verification condition check to verify file_handle is valid or not.
05/02/15   sc      Added integer overflow checks in dev_sdcc_read_bytes
02/05/15   sc      Added overflow check in dev_sdcc_write_bytes
01/27/15   lm      Added error condition check for poll hotplug device API
11/01/13   plc     Update for Arm6 syntax ("attribute")
10/18/13   jz      Added call to poll hotplug device in the system
08/23/13   jz      Updated for UFS boot support, fixed hard-coded sector size
08/23/13   jz      Fixed get_hotplug_dev_handle to support boot from SD card
06/24/13   dh      Update to use run time sector size detection. Expand sector buffer
                   to 4K for UFS devices
06/18/13   wg      Fixed P1 KW warning array bounding
04/23/13   dh      Fix a bug in dev_sdcc_write_bytes that may write 
                   one extra tailing sector
03/18/13   dh      Support unaligned write on dev_sdcc_write_bytes
07/20/12   dh      Add dev_sdcc_write_bytes(doesn't support unaligned write)
09/08/11   dh      Switch all calls to sdcc/hotplug/efs driver to boot extern interface
09/01/11   dh      Remove boot_api.h from include headers
03/21/11   dh      Support loading use GPT partition
03/04/11   dxiang  Support loading of general images with custom partition types
02/22/11   dxiang  Add additional entries for SSD partition loading 
10/15/10   plc     Fix bug in dev_sdcc_read_bytes where breaking out of nested 
                   while loop on failure, results in trailing bytes still being
                   loaded from SDCC, and failure being masked as false success
08/13/10   aus     Created.

=============================================================================*/


/*=============================================================================
                            INCLUDE FILES FOR MODULE
=============================================================================*/
#include "boot_error_if.h"
#include "boot_sdcc.h"
#include "boot_util.h"
#include "boot_extern_hotplug_interface.h"
#include "boot_sbl_shared.h"
#include "boot_flash_dev_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/* max sector size we can have, which is 4k for UFS device */
#define MAX_SECTOR_SIZE 4096

/* To use aligned buffer for Hotplug reads */
static ALIGN(32) uint8 dev_sdcc_buf_unalign[MAX_SECTOR_SIZE];

static boolean poll_done = FALSE;

#define UINT16_MAX_VALUE 0xFFFF

/*---------------------------------------------------------------------------
  Image path on the MMC device.
---------------------------------------------------------------------------*/
/* The image_partition_id[] array holds a list of image specific partition IDs
 * which are used by the hotplug API to open partitions and get a handle of
 * the partition to perform various operations. */
flash_partition_id image_partition_id[] = 
{
  { {HOTPLUG_PARTITION_ID_BOOT_QCSBLHD_CFG_DATA} },   /* DBL */
  { {HOTPLUG_PARTITION_ID_BOOT_QCSBL} },              /* QCSBL */
  { {HOTPLUG_PARTITION_ID_BOOT_OEMSBL} },             /* OSBL */
  { {HOTPLUG_PARTITION_ID_BOOT_APPSSBL} },            /* APPSBL */
  { {HOTPLUG_PARTITION_ID_BOOT_MODEM} },              /* AMSS */
  { {HOTPLUG_PARTITION_ID_BOOT_APPS} },               /* APPS */
  { {HOTPLUG_PARTITION_ID_BOOT_MODEM_FS1} },          /* MODEM_ST1 */
  { {HOTPLUG_PARTITION_ID_BOOT_MODEM_FS2} },          /* MODEM_ST2 */
  { {HOTPLUG_PARTITION_ID_ADSP} },                    /* ADSP */
  { {HOTPLUG_PARTITION_ID_BOOT_FOTA} },               /* FOTA */
  { {0x51} },                                         /* SBL2 */
  { {0x5d} },                                         /* SSD_KEYS */
  { {0x0} },                                          /* GEN_IMG */
  { NULL },                                           /* LAST_ENTRY */
};

/* The path_id[] is an array which is one to one mapped with the image_type 
 * enumeration. This is used to index into image_partition_id[] in order 
 * to associate specific partition ID information with each image */
const int path_id[] = 
  { /* NONE_IMG */    -1,
    /* OEM_SBL_IMG */ -1, 
    /* AMSS_IMG */     4, 
    /* QCSBL_IMG */   -1, 
    /* HASH_IMG */    -1, 
    /* APPSBL_IMG */   9, 
    /* APPS_IMG */     5, 
    /* HOSTDL_IMG */  -1, 
    /* DSP1_IMG */    -1, 
    /* FSBL_IMG */    -1,
    /* DBL_IMG */      0, 
    /* OSBL_IMG */     2, 
    /* DSP2_IMG */    -1, 
    /* EHOSTDL_IMG */ -1, 
    /* NANDPRG_IMG */ -1, 
    /* NORPRG_IMG */  -1, 
    /* RAMFS1_IMG */   6, 
    /* RAMFS2_IMG */   7,
    /* ADSP_Q5_IMG */  8,
    /* APPS_KERNEL_IMG */ -1,
    /* BACKUP_RAMFS_IMG */ -1,
    /* SBL1_IMG */    -1,  
    /* SBL2_IMG */    10,
    /* RPM_IMG  */     3,
    /* SBL3_IMG */     1,
    /* TZ_IMG   */     2,
    /* SSD_KEYS_IMG */ 11,
    /* GEN_IMG */      12,
  };

/* IMAGE_NUMBERS represents the total number of valid partition IDs in image_partition_id[] */
static const uint32 IMAGE_NUMBERS = sizeof(image_partition_id)/sizeof(flash_partition_id) - 1; 

/* PATH_ID_INDEX_MAX represents the maximum value we can use to index into path_id[] */
static const uint32 PATH_ID_INDEX_MAX = sizeof(path_id)/sizeof(int);

/* The structure holds images partition info */
static hotplug_partition_t sbl_partitions[IMAGE_NUMBERS];

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*=========================================================================
**  Function :  hotplug_set_general_partition_id
** ==========================================================================
*/
/*!
* 
* @brief
*   This function takes in a pointer to the partition ID information and makes a deep
*   copy of the information into the image_partition_id[] array corresponding
*   to a general image ID (GEN_IMG). 
* 
* @param[in] partition_id - Byte pointer to the partition id
*
* @par Dependencies
*   None 
* 
* @retval
*   None 
* 
*/
void hotplug_set_general_partition_id(uint8 * partition_id)
{
  BL_VERIFY(partition_id != NULL, BL_ERR_NULL_PTR );
 
  /* Copy data from the byte pointer into the image_partition_id[] array corresponding to GEN_IMG */
  qmemcpy( image_partition_id[path_id[(uint16)GEN_IMG]].partition_id, 
           partition_id, 
           sizeof(flash_partition_id)); 
}

/*=========================================================================
**  Function : hotplug_check_partition_id_is_mbr
** ==========================================================================
*/
/*!
* 
* @brief
*   This function takes in a pointer to the partition ID information and determines
*   whether or not it is of MBR type. With MBR partition tables, partition types are
*   limited to 1 byte. Other formats such as GUID Partition Table (GPT) may use up to
*   16 bytes for partition ID information. 
* 
* @param[in] partition_id - Byte pointer to the partition id
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE - Partition ID is of MBR type (Only holds 1 byte of information)
*   FALSE - Otherwise 
* 
*/
static boolean hotplug_check_partition_id_is_mbr(uint8 * partition_id)
{
  boolean ret_val = TRUE;
  uint32 i = 0;
 
  BL_VERIFY(partition_id != NULL, BL_ERR_NULL_PTR );

  /* We check for MBR type by ensuring only the first byte contains any data */
  for(i = 1; i < PARTITION_ID_MAX_SIZE; i++)
  {
    /* Starting from the second byte, check if byte is not 0x0 */
    if(partition_id[i] != 0x0){
      ret_val = FALSE;
      break;
    } 
  } 

  return ret_val;
}

/*=========================================================================

**  Function :  get_hotplug_dev_handle

** ==========================================================================
*/
/*!
* 
* @brief
*   This function performs the following:
*   1. Get the hotplug device handle of partition specific to the image id.
*   2. Verify the device size and block size.
*   3. Setup an instance of the boot translation module, pass in the start sector
*      of image partition.
* 
* @param[in] image_id Image type to open
*
* @par Dependencies
*   Hotplug APIs
* 
* @retval
*   Hotplug ID on success otherwise -1.
* 
*/
int16 get_hotplug_dev_handle
(
  image_type image_id                  /* Image type to open */
)
{
  int16 hotplug_id = -1;  
  boot_flash_type fastboot_option = NO_FLASH;
  int16 status = -1;
  uint32 mbr_partition_id = 0;
  uint8 * partition_id_ptr = NULL;
  boolean partition_type_is_mbr = FALSE;
  enum hotplug_iter_device_location dev_loc = HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY;
  hotplug_dev_type dev_type = HOTPLUG_TYPE_INVALID;  

  /* First verify the input parameters */
  BL_VERIFY( image_id != NONE_IMG, BL_ERR_WRG_IMGHD_REQUESTED );

  /* Get the flash type and appropriately assign the device location and device type */
  fastboot_option = boot_pbl_get_flash_type();

  if (fastboot_option == MMC_FLASH)
  {
    dev_loc = HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY;
    dev_type = HOTPLUG_TYPE_MMC;
  }
  else if (fastboot_option == SDC_FLASH)
  {
    dev_loc = HOTPLUG_ITER_EXTERNAL_DEVICES_ONLY;
    dev_type = HOTPLUG_TYPE_MMC;
  }
  else if (fastboot_option == UFS_FLASH)
  {
    dev_loc = HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY;
    dev_type = HOTPLUG_TYPE_UFS;
  }

   /* Poll the hotplug device if it is not already polled*/
  if (poll_done == FALSE)
  {
     status = boot_hotplug_poll_device(dev_type, dev_loc);
	 BL_VERIFY( (status == HOTPLUG_SUCCESS), BL_ERR_HOTPLUG );
     poll_done = TRUE;
  }

  /* Get the Hotplug ID, used to index into image_partition_id[] */
  hotplug_id = path_id[(uint16)image_id];
  BL_VERIFY( hotplug_id != INVALID_FILE_HANDLE, BL_ERR_IMG_NOT_FOUND );
  BL_VERIFY( hotplug_id < IMAGE_NUMBERS, BL_ERR_IMG_NOT_FOUND );

  /* Assign pointer to partition ID information */ 
  partition_id_ptr = image_partition_id[hotplug_id].partition_id;
  
  /* Determine if partition ID information is of MBR type */
  partition_type_is_mbr = hotplug_check_partition_id_is_mbr(partition_id_ptr);
  
  /* If partition ID is MBR, we assign the partition ID information to an integer 
   * to be passed into the hotplug API. */
  if (partition_type_is_mbr == TRUE)
  {
    mbr_partition_id = *((uint32*)partition_id_ptr);
    sbl_partitions[hotplug_id].hdev_ptr =
        boot_hotplug_open_device_by_partition_type (dev_type, dev_loc, mbr_partition_id);
  }
  else
  {
    /* since we are not using MBR parition, we know partition_id_ptr points 
       to a GUID struct, so call into GPT hotplug api instead */
    sbl_partitions[hotplug_id].hdev_ptr =
        boot_hotplug_open_device_by_gpt_partition_type (dev_type, 
                                                   dev_loc, 
                                                   (struct hotplug_guid *)partition_id_ptr);
  }

  /* After successfully opening the partition and getting the device handle */ 
  if (sbl_partitions[hotplug_id].hdev_ptr != NULL)
  {
    /* Partition block infomation in the hotplug device API. */
    status = hotplug_dev_get_size(sbl_partitions[hotplug_id].hdev_ptr, 
        &sbl_partitions[hotplug_id].blkcnt,
        (uint16*)&sbl_partitions[hotplug_id].blksz );

    BL_VERIFY( (status == HOTPLUG_SUCCESS), BL_ERR_HOTPLUG );

    /* Check for the device block count & block size */
    BL_VERIFY( (sbl_partitions[hotplug_id].blkcnt || 
          sbl_partitions[hotplug_id].blksz), BL_ERR_SDC_INVALID_SIZE);
  }  
  else
  {
    hotplug_id = INVALID_FILE_HANDLE;
  }

  return hotplug_id;
} /* get_hotplug_dev_handle */

/*=============================================================================

**  Function :  dev_sdcc_read_bytes

** ==========================================================================
*/
/*!
* 
* @brief
*   This function reads the specified number of bytes from eMMC device to the 
*   buffer provided. Takes care not to overflow the buffer
* 
* @param[in] ram_addr Destination address in RAM
* @param[in] src_offset Source address in bytes from partition start
* @param[in] bytes_to_read Number of bytes to read
* @param[in] parti_id Hotplug ID or file handle depending on parti type
*
* @par Dependencies
*   None
* 
* @retval
*   TRUE on success FALSE otherwise.
* 
* @par Side Effects
*   None
* 
*/
boolean dev_sdcc_read_bytes
(
  void   *ram_addr,          /* Destination address in RAM */
  uint32 src_offset,         /* source address in bytes from partition start */
  uint32 bytes_to_read,      /* number of bytes to read */
  uint32 parti_id           /* hotplug ID or file handle depending on parti type */
)
{

  uint32 sector_size, start_sector, end_sector, num_sectors, leading_pad;
  uint32 trailing_length, end_sector_bytes;
  uint16 block_count = 0;
  
  boolean return_status = FALSE;
  
  hotplug_partition_t* hotplug_dev = 0;
  uint8* dest_addr = (uint8*)ram_addr;
  
  hotplug_dev = &sbl_partitions[parti_id];

 do
 {
    if (hotplug_dev->hdev_ptr == NULL )
    {
      return_status = FALSE;
      break;
    }
  
    sector_size = hotplug_dev->blksz;
  
    if (sector_size == 0 )
    {
      return_status = FALSE;
      break;
    }
  
    /*Make sure device sector size doesn't exceed the sector buffer size */
    BL_VERIFY(sector_size <= MAX_SECTOR_SIZE, BL_ERR_SBL);
    
    start_sector = src_offset / sector_size;
    leading_pad = src_offset % sector_size;
  
    BL_VERIFY(src_offset + bytes_to_read >= bytes_to_read, BL_ERR_SBL);
    end_sector_bytes = src_offset + bytes_to_read;
    end_sector = end_sector_bytes  / sector_size;
    trailing_length = end_sector_bytes % sector_size;
   
    num_sectors = (trailing_length != 0) ? (end_sector - start_sector + 1) : 
                                                              (end_sector - start_sector); 

    /* This handles the case of reading a partial sector. In this partial sector 
       read we need to skip the leangth of leading bytes */
    if (leading_pad > 0 )
    {      
      if (HOTPLUG_SUCCESS != boot_hotplug_dev_read(hotplug_dev->hdev_ptr, start_sector, 
        dev_sdcc_buf_unalign, 1))
      {
        return_status = FALSE;
        break;
      }
      
      /* If the requested read lies within this sector, that is the read is less than
         a sector */
      if ( (end_sector == start_sector) && (trailing_length != 0) )
      {        
        qmemcpy(dest_addr, &dev_sdcc_buf_unalign[leading_pad], bytes_to_read);
        return_status = TRUE;
        break;
      }
      else
      {
        qmemcpy(dest_addr, &dev_sdcc_buf_unalign[leading_pad], (sector_size - leading_pad));
        dest_addr += (sector_size - leading_pad);
      }
      
       /* As we have read a partial sector decrement the total number 
          of sectors to be read and increment the start sector */       
      if (num_sectors > 0)
      {
        num_sectors --;
      }
   
      start_sector++;      
    }/* leading_pad */

    /* If trailing bytes exist, that is a partial read of the last sector, then we
       decrement the number of sectors and do the partial read of the last sector separately */
    if ( (trailing_length != 0) && (num_sectors > 0))
    {
      num_sectors--;
    }

    /* Read all the remaining sectors by looping. */
    while (num_sectors > 0) 
    {
      /* If the number of sectors to be read exceeds max value of block_count,
         do it in a loop */
      block_count = (num_sectors > UINT16_MAX_VALUE)? UINT16_MAX_VALUE : num_sectors;
   
      if (HOTPLUG_SUCCESS != boot_hotplug_dev_read(hotplug_dev->hdev_ptr, 
         start_sector, dest_addr, block_count))
      {
        return_status = FALSE;
        break;
      }
   
      num_sectors  -= block_count;
      start_sector += block_count;
      dest_addr    += block_count*sector_size;
    } /* num_sectors */

    /* If num_sectors is still > 0, that means we broke out prematurely, and
       need to break out of the overall loop, so we catch the FALSE return_status
       and don't continue loading the final trailing bytes, if there are any     */
    if (num_sectors > 0)
    {
       break;
    }

    /* Read the traling bytes - the partial sector at the end */
    if ( trailing_length != 0 )
    {
      if (HOTPLUG_SUCCESS != boot_hotplug_dev_read(hotplug_dev->hdev_ptr, start_sector, 
        dev_sdcc_buf_unalign, 1))
      {
        return_status = FALSE;
        break;
      }
   
      qmemcpy(dest_addr, dev_sdcc_buf_unalign, trailing_length);
    }
 
    return_status = TRUE;    
  }while(0);

  return return_status;
  
} /* dev_sdcc_read_bytes */

/*===========================================================================

**  Function :  hotplug_close_device_by_image_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This function closes the file using hotplug_close_device.
* 
* @param[in] image_id Image type
*
* @par Dependencies
*   EFS APIs 
* 
* @retval
*   TRUE on success FALSE otherwise.
* 
* @par Side Effects
*   None
* 
*/
int hotplug_close_device_by_image_type
(
  image_type image_id       /* image type */
)
{
  int status = EFS_ERROR;  
  int16 hotplug_id = -1;  

  BL_VERIFY( image_id < PATH_ID_INDEX_MAX, BL_ERR_IMG_NOT_FOUND ); 

  /* Get the Hotplug ID, used to index into image_partition_id[] */
  hotplug_id = path_id[(uint16)image_id];
  BL_VERIFY( hotplug_id != INVALID_FILE_HANDLE, BL_ERR_IMG_NOT_FOUND );
  BL_VERIFY( hotplug_id < IMAGE_NUMBERS, BL_ERR_IMG_NOT_FOUND );

  /* First check if image is in RAW partition */
  if (sbl_partitions[hotplug_id].hdev_ptr != NULL)
  {
     /* Close hotplug */
     status = boot_hotplug_close_device (sbl_partitions[hotplug_id].hdev_ptr);
   BL_VERIFY( status == EFS_SUCCESS, BL_ERR_EFS_CLOSE_FAIL );
  }
  
  return status;
   
} /* hotplug_close_device_by_image_type */

/*===========================================================================

**  Function :  hotplug_close_device_by_handle

** ==========================================================================
*/
/*!
* 
* @brief
*   This function closes the file using hotplug_close_device.
* 
* @param[in] file_handle file handle
*
* @par Dependencies
*   EFS APIs 
* 
* @retval
*   TRUE on success FALSE otherwise.
* 
* @par Side Effects
*   None
* 
*/
int hotplug_close_device_by_handle
(
    int16 file_handle     /* file handle */
)
{
 
  int status = EFS_ERROR;
  
  /* First verify the file handle is valid. */
  BL_VERIFY( file_handle != INVALID_FILE_HANDLE, BL_ERR_EFS_INVALID_HANDLE ); 
  
  BL_VERIFY( file_handle < IMAGE_NUMBERS, BL_ERR_EFS_INVALID_HANDLE);
  
  /* First check if image is in RAW partition */
  if (sbl_partitions[file_handle].hdev_ptr != NULL)
  {
    /* Close hotplug */
    status = boot_hotplug_close_device (sbl_partitions[file_handle].hdev_ptr);
    BL_VERIFY( status == EFS_SUCCESS, BL_ERR_EFS_CLOSE_FAIL );
  }
  
  return status;
   
} /* hotplug_close_device_by_handle */


/*===========================================================================

**  Function :  do_hotplug_init

** ==========================================================================
*/
/*!
* 
* @brief   
*     This function initializes hotplug  
* 
* @par Dependencies
*     None
* 
* @retval  
*     None
* 
* @par Side Effects
*     None
*/
void do_hotplug_init 
(
  void
)
{
  boot_hotplug_init();
} /* do_hotplug_init */


/*==============================================================================

**  Function :  dev_sdcc_write_bytes

** =============================================================================
*/
/*!
*
* @brief
*   This function writes the specified number of bytes to eMMC device from the
*   buffer provided starting at specified offset of the partition
*   We currently don't support unaligned write
*   (destination offset that doesn't start at sector boundary)
*
* @param[in] ram_addr Source address in RAM
* @param[in] dst_offset Destination address in bytes from partition start
* @param[in] bytes_to_write Number of bytes to write
* @param[in] image_id Image type
*
* @par Dependencies
*   None
*
* @retval
*   TRUE on success FALSE otherwise.
*
* @par Side Effects
*   None
*
*/
boolean dev_sdcc_write_bytes
(
  void   *ram_addr,          /* Source address in RAM */
  uint64 dst_offset,         /* Destination address in bytes from partition start */
  uint32 bytes_to_write,     /* Number of bytes to write */
  image_type image_id        /* Image type */
)
{
  boolean return_status = FALSE;
  int16 hotplug_id;
  hotplug_partition_t *hotplug_dev;
  uint32 start_sector, leading_length;
  uint32 end_sector, trailing_length;
  uint16 block_count = 0;
  uint8 *buf_addr = (uint8*)ram_addr;
  uint32 sector_size;
  
  hotplug_id = path_id[(uint16)image_id];
  hotplug_dev = &sbl_partitions[hotplug_id];
  sector_size = hotplug_dev->blksz;
    
  /*Make sure device sector size doesn't exceed the sector buffer size */
  BL_VERIFY( (sector_size != 0) && (sector_size <= MAX_SECTOR_SIZE), BL_ERR_SBL);

  start_sector = dst_offset/sector_size;
  leading_length = dst_offset % sector_size;
  BL_VERIFY(leading_length < sector_size, BL_ERR_SBL);
  
  BL_VERIFY(dst_offset + bytes_to_write >= dst_offset, BL_ERR_SBL);
  end_sector = (dst_offset + bytes_to_write)/sector_size;
  trailing_length = (dst_offset + bytes_to_write) % sector_size;
  
  do 
  {
    /* Nothing to write */
    if(bytes_to_write == 0)
    {
      return_status = TRUE;
      break;
    }
    
    if (hotplug_dev->hdev_ptr == NULL )
    {
      break;
    }

    if (leading_length > 0)
    {
      /* Read the orginal data from start sector */
      if (HOTPLUG_SUCCESS != boot_hotplug_dev_read(hotplug_dev->hdev_ptr, start_sector, 
          dev_sdcc_buf_unalign, 1))
      {
        break;
      }
      
      /* When end_sector equals start_sector, the whole write won't exceed one sector */
      if (end_sector == start_sector)
      {
        /* Check the write size, make sure we don't overflow the buffer. 
           Not necessary but added for klocwork*/ 
        if((bytes_to_write + leading_length) <= sector_size)
        {
          qmemcpy(&dev_sdcc_buf_unalign[leading_length], buf_addr, bytes_to_write);
        }
        else
        {
          /* Should never happen */
          return_status = FALSE;
          break;
        }
        
        /* Now write back this one leading sector and we are done */
        if (HOTPLUG_SUCCESS == boot_hotplug_dev_write(hotplug_dev->hdev_ptr, 
                                                      start_sector, 
                                                      dev_sdcc_buf_unalign, 
                                                      1))
        {
          return_status = TRUE;
        }
        break;
      }

      /* The write size is more than this sector, need to copy data till this sector's end */
      qmemcpy(&dev_sdcc_buf_unalign[leading_length], buf_addr, sector_size - leading_length);
      buf_addr += sector_size - leading_length; 
      /* Now write back this one leading sector */
      if (HOTPLUG_SUCCESS != boot_hotplug_dev_write(hotplug_dev->hdev_ptr, 
                                                    start_sector, 
                                                    dev_sdcc_buf_unalign, 
                                                    1))
      {
        break;
      }         
      
      /* As we have write a partial sector,increment the start sector */
      start_sector++;
    }

    /* write all the aligned sectors by looping. */
    while (start_sector < end_sector) 
    {
      /* If the number of sectors to be write exceeds max value of block_count,
         do it in a loop */
      block_count = ((end_sector - start_sector) > UINT16_MAX_VALUE)? UINT16_MAX_VALUE : (end_sector - start_sector);
   
      if (HOTPLUG_SUCCESS != boot_hotplug_dev_write(hotplug_dev->hdev_ptr, 
                                                    start_sector, 
                                                    buf_addr, 
                                                    block_count))
      {
        return_status = FALSE;
        break;
      }
      start_sector += block_count;
      buf_addr    += block_count*sector_size;
    }

    /* If start_sector does not equals end_sector, that means we broke out prematurely, and
       need to break out of the overall loop, so we catch the FALSE return_status
       and don't continue writing the final trailing bytes, if there are any */
    if (start_sector != end_sector)
    {
       break;
    }
    
    /* Write trailing sector */
    if (trailing_length > 0)
    {
      /* Read sector for partial write */
      if (boot_hotplug_dev_read(hotplug_dev->hdev_ptr, 
                                end_sector,
                                dev_sdcc_buf_unalign, 1) 
          != HOTPLUG_SUCCESS)
      {
        break;
      }

      /* Copy trailing data to write buffer */
      qmemcpy(dev_sdcc_buf_unalign, buf_addr, trailing_length);

      /* Write back updated sector */
      if (boot_hotplug_dev_write(hotplug_dev->hdev_ptr, 
                                 end_sector,
                                 dev_sdcc_buf_unalign, 
                                 1) 
         != HOTPLUG_SUCCESS)
      {
        break;
      }
    }
    
    return_status = TRUE;

  } while (0);

  return return_status;

} /* dev_sdcc_write_bytes */


/*==============================================================================

**  Function :  hotplug_get_partition_size_by_image_id

** =============================================================================
*/
/*!
*
* @brief
*   This function return the parition size in byte.
*
* @par Dependencies
*   None
*
* @retval
*   returns Parition size, 0 if error or parition doesn't exit
*
* @par Side Effects
*   None
*
*/
uint64 hotplug_get_partition_size_by_image_id
(
  image_type image_id        /* Image type */
)
{
  int16 hotplug_id = -1;  
  hotplug_partition_t *hotplug_dev;
  uint64 parition_size = 0;
  uint32 block_count = 0;
  uint16 bytes_per_block = 0;
  
  BL_VERIFY( image_id < PATH_ID_INDEX_MAX, BL_ERR_IMG_NOT_FOUND ); 

  /* Get the Hotplug ID, used to index into image_partition_id[] */
  hotplug_id = path_id[(uint16)image_id];
  BL_VERIFY( hotplug_id != INVALID_FILE_HANDLE, BL_ERR_IMG_NOT_FOUND );
  BL_VERIFY( hotplug_id < IMAGE_NUMBERS, BL_ERR_IMG_NOT_FOUND );

  hotplug_dev = &sbl_partitions[hotplug_id];
  
  if (hotplug_dev->hdev_ptr != NULL )
  {
    if ( HOTPLUG_SUCCESS == 
         boot_hotplug_dev_get_size(hotplug_dev->hdev_ptr, &block_count, &bytes_per_block))
    {
      parition_size = block_count * bytes_per_block;
    }  
  }
  
  return parition_size;
}

