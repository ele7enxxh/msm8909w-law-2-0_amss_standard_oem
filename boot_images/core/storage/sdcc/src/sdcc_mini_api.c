 /*****************************************************************************
 * sdcc_mini_api.c
 *
 * SDCC(Secure Digital Card Controller) driver minimized API for memory device.
 *
 * This file implements the SDCC driver minimized APIs for the memory device.
 *
 * Copyright (c) 2009-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================
                        Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_mini_api.c#1 $
$DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

YYYY-MM-DD   who     what, where, why
-------------------------------------------------------------------------------
2014-11-11   sb      Add null pointer check for sdcc_get_slot_handle
2014-05-16   sb      Fixed sizeof(sdcc_mem_info_type)
2014-03-11   bn      Added API to querry slot type 
2012-09-03   bn      Add SDHCi support
2012-08-14   bn      Reverted optimized init support
2012-08-12   bn      Code clean up
2012-05-31   vk      Avoid powering off during close in bootloaders
2012-01-20   rh      Improve of the slot type definition in BSP
2011-09-30   rh      Correct the missing WP group size info
2011-08-30   rh      Added GPP partitioning support for eMMC
2011-06-16   pa      Changes to support release of thread local errno
2010-12-08   vj      Resolve compilation warnings
2010-12-07   rh      Adding support to erase low capacity MMC cards
2010-11-29   rh      Add API to enable the hardware reset function
2010-10-27   rh      Port over the optimized open APIs
2010-06-08   rh      Increase the wait time when erasing the memory card
2010-04-16   rcc     Use the BSP to determine whether a slot is embedded
2010-02-08   rh      Port over the change for detecting embedded memory card
2010-02-04   rh      Port over sdcc_slot_has_support
2010-01-20   rh      Port over sdcc_handle_set_active_bootable_partition
2009-12-15   rh      Adding stub for write protection API
2009-04-13   rh      Created from sdcc_api.c as a mini-size replacement

=============================================================================*/
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_handle_util.h"
#include "sdcc_mmc_util.h"
#include "HALsdcc.h"
#include "sdcc_bsp.h"
#include "sdcc_hc.h"

extern boolean SDCCSdhciEnable;

sdcc_mini_boot_device_info_type sdcc_mini_boot_device_info = {0};

/*lint -save -e641 Suppress 'Converting enum to int' warning */
/******************************************************************************
* Name: sdcc_handle_open
*
* Description:
*    This function first initializes the SDCC controller such as turning on the
*    clock, configuring the GPIOs, and turning on the power supply to the
*    device.  It returns NULL if there is no memory device attached.  If there
*    is a memory device, it will initialize the device and take it out of the
*    identification state.  It then sets up the device for data transfer.
*    A non-null pointer to the SDCC device is returned.
*
* Arguments:
*    driveno           [IN] : a valid drive number between 0 and the total
*                            number of card slots - 1
*    phy_partition_num [IN] : physical partition number.  Use
*                             \ref SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE
*                             to get a handle representing the entire memory
*                             device.  For eMMC v4.3, the valid boot partition
*                             numbers are 1 and 2.  User area partition number
*                             is 0.
*
* Returns:
*    A pointer to the SDCC device if successful.  NULL if failed.
*
* Note:
*    sdcc_init() is called in sdcc_handle_open().
*
******************************************************************************/
struct sdcc_device *
sdcc_handle_open( int16 driveno, uint32 phy_partition_num )
{
   SDCC_STATUS         rc         = SDCC_NO_ERROR;
   sdcc_slot_type     *sdcc_pdata = NULL;
   sdcc_dev_type      *sdcc_pdev  = NULL;
   boolean             ret        = TRUE;
   struct sdcc_device *ret_handle = NULL;
   boolean             still_in_use = TRUE;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid Drive Number: %d", driveno));
      return NULL;
   }

   /* PBL uses legacy Qualcomm mode, so partial enumeration is supported in */
   /* Qualcomm legacy mode only. However, if we switch to SDHCi mode now, */
   /* we need to do a full re-init */
   if ((FALSE == SDCCSdhciEnable) && 
       (TRUE == sdcc_is_partial_enumeration (driveno)))
   {
      ret_handle = sdcc_handle_optimized_open (phy_partition_num);
      return ret_handle;
   }
   
   /* This is the only place we get config flag from a UEFI variable 
    * to determine if the driver supports SDHCi or Qualcomm interface.
    * After this, we'll use the p_device->sdhci_mode as flag */
   if (TRUE == SDCCSdhciEnable)
   {
      ret = sdcc_init_sdhci_mode(driveno);
   }
   else
   {
      ret = sdcc_init_qc_mode(driveno); 
   }
   
   if ( FALSE == ret )
   {
      DPRINTF(("Failed to initialize drive number %d", driveno));
      return NULL;
   }

   sdcc_pdata = sdcc_get_slot_handle (driveno);
   if (NULL == sdcc_pdata)
   {  
      DPRINTF(("sdcc_handle_open: can't get slot handle\n"));
      return NULL;
   }
   
   sdcc_pdev = &(sdcc_pdata->dev);

   do
   {
      /* register the client handle first */
      ret_handle = sdcc_register_client_handle( (uint32)driveno,
                                                phy_partition_num );
      if ( NULL == ret_handle )
      {
         /* check if memory device in driveno is still in use */
         still_in_use = sdcc_is_device_still_in_use( sdcc_pdev );
         if ( !still_in_use )
         {
            /* client handle list is full */
            (void) sdcc_deinit( sdcc_pdev );
         }
         break;
      }

      /* return if this is a SDIO device */
      if ( SDCC_CARD_SDIO == sdcc_pdev->card_type )
      {
         /* client handle registration will be initiated in SDIO API */
         /* deregister the client handle */
         sdcc_deregister_client_handle( &ret_handle );
         break;
      }

      /* return if it has been initialized */
      if (( SDCC_CARD_SD  == sdcc_pdev->card_type ) ||
          ( SDCC_CARD_SDHC == sdcc_pdev->card_type ) ||
          ( SDCC_CARD_MMC == sdcc_pdev->card_type ) ||
          ( SDCC_CARD_MMCHC == sdcc_pdev->card_type ))
      {
         /* no need to select the partition region on the device as it is */
         /* already in use */
         /* will return a new handle to client */
         break;
      }

      /* find the attached device */
      sdcc_pdev->card_type = sdcc_handle_find_card( ret_handle );
      /* update card_type field in sdcc_mem_info struct */
      sdcc_pdev->mem.mem_info.card_type = sdcc_pdev->card_type;

      /* return and close controller if none attached */
      if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
      {
         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      /* return if SDIO device attached */
      if ( SDCC_CARD_SDIO == sdcc_pdev->card_type )
      {
         /* client handle registration will be initiated in SDIO API */
         /* deregister the client handle */
         sdcc_deregister_client_handle( &ret_handle );
         DPRINTF(("Found SDIO device.\n"));
         break;
      }

      /* proceed if this is SD/MMC device */
      /* finish identification mode */
      rc = sdcc_init_memory_device(sdcc_pdev);
      if (SDCC_NO_ERROR != rc)
      {
         DPRINTF(("Failed to initialize the card.\n"));
         sdcc_pdev->errno = rc;

         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      if (FALSE == sdcc_pdev->sdhci_mode)
      {
         HAL_sdcc_FlowControl (driveno, TRUE);
      }

      /* get memory device info  */
      rc = sdcc_get_memory_info(sdcc_pdev);
      if (SDCC_NO_ERROR != rc)
      {
         DPRINTF(("Failed to get memory card info.\n"));
         sdcc_pdev->errno = rc;

         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      /* re-program the clock for data transfer */
      sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_MODE);

      /* config device for data transfer */
      rc = sdcc_config_memory_device(sdcc_pdev);
      if (SDCC_NO_ERROR == rc)
      {
         sdcc_pdev->host_state = SDCC_HOST_TRAN;
         // set to true if card is removed / reinserted
         sdcc_pdata->slot_state_changed = FALSE;
      }
      else
      {
         sdcc_pdev->errno     = rc;
         sdcc_pdev->card_type = SDCC_CARD_UNKNOWN;
         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
      }
   }
   while (0);

   return ret_handle;
} /* sdcc_handle_open */

/******************************************************************************
* Name: sdcc_handle_mem_get_device_info
*
* Description:
*    This function gets the memory card specific information.  If there is no
*    memory device attached or device is not initialized, all fields in the
*    card specific information are zero.
*
* Dependencies:
*    sdcc_handle_open() must be called before getting card info for the
*    specified SDCC handle.
*
* Parameters:
*    handle          [IN] : a pointer to the SDCC device that was returned from
*                           sdcc_handle_open()
*    sdcc_pmem_info [OUT] : pointer to the structure to save the memory card
*                           specific information
*
* Returns:
*    SDCC_NO_ERROR on success
*    error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_mem_get_device_info
(
   struct sdcc_device *handle,
   sdcc_mem_info_type *sdcc_pmem_info
)
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   sdcc_dev_type   *sdcc_pdev  = NULL;
   uint32           driveno    = 0;
   uint32           phy_partition_num = 0;
   uint32           partition_size_in_sectors = 0;
   uint32           phy_partition_type = 0;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DPRINTF(("Invalid SDCC handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   if ( NULL == sdcc_pmem_info )
   {
      DPRINTF(("Passed in NULL sdcc_pmem_info pointer"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   /* zero out all the memory info fields */
   memset( sdcc_pmem_info, 0, sizeof(sdcc_mem_info_type) );

   /* translate physical partition number of the client handle */
   /* SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE is translated as */
   /* partition number 0 which is the user area. */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   do
   {
      if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
      {
         rc = SDCC_ERR_CARD_UNDETECTED;
         break;
      }

      /* populate the sdcc_pmem_info out pointer */
      sdcc_pmem_info->card_type = sdcc_pdev->card_type;
      sdcc_pmem_info->block_len = sdcc_pdev->mem.mem_info.block_len;
      /* Query the BSP to see if the slot is embedded */
      if (sdcc_bsp_get_slot_type(handle->hdev->driveno) & SDCC_BSP_EMBEDDED_MEM_SLOT)
         sdcc_pmem_info->is_embedded_device = TRUE;
      else
         sdcc_pmem_info->is_embedded_device = FALSE;

      if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
           SDCC_CARD_MMCHC == sdcc_pdev->card_type )
      {
         rc = sdcc_mmc_get_phy_partition_info( handle,
                                               &partition_size_in_sectors,
                                               &phy_partition_type );
         /* regardless rc, populate the partition size and type */
         sdcc_pmem_info->card_size_in_sectors = partition_size_in_sectors;
         sdcc_pmem_info->speed_class.mmc_speed_class =
            (sdcc_mmc_speed_class_type)sdcc_pdev->mem.speed_class;
         sdcc_pmem_info->reliable_write_sector_count =
            sdcc_pdev->mem.mem_info.reliable_write_sector_count;
         sdcc_pmem_info->is_phy_partition_bootable =
            sdcc_mmc_is_phy_partition_bootable(handle);
         sdcc_pmem_info->write_protect_group_size_in_sectors =
            sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors;
      }
      else
      {
         if ( 0 == phy_partition_num )
         {
            sdcc_pmem_info->card_size_in_sectors =
               sdcc_pdev->mem.mem_info.card_size_in_sectors;
         }
         else
         {
            /* regardless rc, populate the partition size and type */
            rc = SDCC_ERR_FEATURE_UNSUPPORTED;
            sdcc_pmem_info->card_size_in_sectors = 0;
         }
         /* Card type is SD or SDHC. */
         sdcc_pmem_info->speed_class.sd_speed_class =
            (sdcc_sd_speed_class_type)sdcc_pdev->mem.speed_class;
         sdcc_pmem_info->reliable_write_sector_count = 0;
         sdcc_pmem_info->is_phy_partition_bootable = FALSE;
      }

      sdcc_pmem_info->phy_partition_type = phy_partition_type;
      sdcc_pmem_info->num_phy_partition_created =
         sdcc_pdev->mem.mem_info.num_phy_partition_created;

      /* card identification info */
      sdcc_pmem_info->mfr_id = sdcc_pdev->mem.mem_info.mfr_id;
      sdcc_pmem_info->oem_id = sdcc_pdev->mem.mem_info.oem_id;
      memcpy( sdcc_pmem_info->prod_name, sdcc_pdev->mem.mem_info.prod_name,
         sizeof(sdcc_pdev->mem.mem_info.prod_name) );
      sdcc_pmem_info->prod_rev = sdcc_pdev->mem.mem_info.prod_rev;
      sdcc_pmem_info->prod_serial_num = sdcc_pdev->mem.mem_info.prod_serial_num;
      memcpy( sdcc_pmem_info->mfr_date, sdcc_pdev->mem.mem_info.mfr_date,
         sizeof(sdcc_pdev->mem.mem_info.mfr_date) );
   } while (0);

   /* display the memory device info on QXDM */
   sdcc_display_mem_device_info( handle, sdcc_pmem_info );

   return rc;
} /* sdcc_handle_mem_get_device_info */

/******************************************************************************
* Name: sdcc_handle_is_mem_present
*
* Description:
*    This function gets the card status of the specified SDCC handle.
*
* Parameters:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open
*
* Returns:
*    TRUE if memory device is present.  FALSE otherwise.
*
******************************************************************************/
boolean sdcc_handle_is_mem_present( struct sdcc_device *handle )
{
   sdcc_dev_type  *sdcc_pdev  = NULL;
   SDCC_CARD_STATUS card_status = SDCC_CARD_IDLE;
   uint32 driveno = 0;

   if ( NULL == handle || NULL== handle->hdev )
   {
      DPRINTF(("Invalid SDCC handle 0x%x", handle));
      return FALSE;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return FALSE;
   }

   sdcc_pdev = handle->hdev;
   if ( sdcc_pdev->pslot->slot_state_changed ||
        SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
   {
      return FALSE;
   }

   card_status = sdcc_send_status( sdcc_pdev );
   if ( SDCC_CARD_IGNORE == card_status )
   {
      sdcc_pdev->card_type = SDCC_CARD_UNKNOWN;
      return FALSE;
   }
   return TRUE;
} /* sdcc_handle_is_mem_present */

/******************************************************************************
* Name: sdcc_handle_set_sps_end_point
*
* Description:
*    This function sets the SPS end-point for data transfer.  SDCC client that
*    uses SPS for data transfer is required to call this function before
*    invoking the read/write function.
*
* Parameters:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open
*    end_point  [IN] : the SPS end-point to be configured for data transfer
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_handle_set_sps_end_point
(
   struct sdcc_device *handle,
   void *end_point
)
{
   /* a placeholder */
   (void)handle;
   (void)end_point;
   return SDCC_NO_ERROR;
} /* sdcc_handle_set_sps_end_point */

/******************************************************************************
* Name: sdcc_handle_set_sps_end_point
*
* Description:
*    This function gets the SPS end-point used for data transfer for the
*    specified client handle.
*
* Parameters:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open
*    end_point  [IN] : the SPS end-point this is used for data transfer
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_handle_get_sps_end_point
(
   struct sdcc_device *handle,
   void *end_point
)
{
   /* a placeholder */
   (void)handle;
   (void)end_point;
   return SDCC_NO_ERROR;
} /* sdcc_handle_get_sps_end_point */

/******************************************************************************
* Name: sdcc_handle_read
*
* Description:
*    This function reads num_blocks blocks of data from the data_address in
*    the device, storing them at the memory location given by buff.
*
* Arguments:
*    handle           [IN] : a pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    data_address     [IN] : start of the sector.  This is the sector unit
*                            address (512 bytes per block).
*    buff            [OUT] : pointer to buffer for the incoming data
*    num_blocks       [IN] : number of blocks to be read (512 bytes per
*                            block)
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
* Note:
*    Multiblock transfer is used when more than 1 block data is transfered.
*
******************************************************************************/
SDCC_STATUS
sdcc_handle_read
(
   struct sdcc_device *handle,
   uint32              data_address,
   byte               *buff,
   uint16              num_blocks
)
{
   uint32           driveno = 0;
   boolean          multi_block = FALSE;
   sdcc_dev_type   *sdcc_pdev   = NULL;
   sdcc_cmd_type    sdcc_cmd;
   SDCC_STATUS rc = SDCC_NO_ERROR;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DPRINTF(("Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   // fail the call if card is removed or reinserted
   if ( sdcc_pdev->pslot->slot_state_changed )
   {
      DPRINTF(("Slot %d state changed", driveno));
      return SDCC_ERR_CARD_REMOVED;
   }

   if ( SDCC_HOST_TRAN != sdcc_pdev->host_state )
   {
      if ( FALSE == sdcc_handle_silent_reinit(handle) )
      {
         rc = SDCC_ERR_CARD_INIT;
         return rc;
      }
   }

   if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
   {
      rc = SDCC_ERR_CARD_UNDETECTED;
      return rc;
   }
   else if ( SDCC_CARD_SD == sdcc_pdev->card_type ||
             SDCC_CARD_SDHC == sdcc_pdev->card_type ||
             SDCC_CARD_MMC == sdcc_pdev->card_type ||
             SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      /* set the client handle partition number before data transfer */
      rc = sdcc_set_partition( handle );
      if ( SDCC_NO_ERROR != rc )
      {
         /* failed to select the requested partition number */
         return rc;
      }

      multi_block = (1 == num_blocks) ? FALSE : TRUE;
        
      /* CMD17/CMD18 */
      sdcc_cmd.cmd = ( multi_block ) ? SD_CMD18_READ_MULTIPLE_BLOCK :
                                      SD_CMD17_READ_BLOCK;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
  
      if ( SDCC_CARD_SDHC == sdcc_pdev->card_type ||
           SDCC_CARD_MMCHC == sdcc_pdev->card_type )
      {
         /* in high capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in block address format */
         sdcc_cmd.cmd_arg   = data_address;
      }
      else
      {
         /* in standard capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in byte address format */
         sdcc_cmd.cmd_arg   = data_address * (sdcc_pdev->mem.mem_info.block_len);   
      }

      sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;
      rc = sdcc_do_transfer(sdcc_pdev, &sdcc_cmd, buff, num_blocks);
   }
   else
   {
      /* card type is SDIO */
      DPRINTF(("Operation not supported on SDIO card"));
      rc = SDCC_ERR_SDIO;
      return rc;
   }

   return rc;
} /* sdcc_handle_read */

/******************************************************************************
* Name: sdcc_handle_write
*
* Description:
*    This function writes num_blocks blocks of data, obtained from the
*    memory location buff, to the data_address on the device.
*
* Arguments:
*    handle           [IN] : a pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    data_address     [IN] : start of the sector.  This is the sector unit
*                            address (512 bytes per block).
*    buff             [IN] : pointer to buffer for the outgoing data
*    num_blocks       [IN] : number of blocks to be written (512 bytes per
*                            block)
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
* Note:
*    Multiblock transfer is used when more than 1 block data is transfered.
*
******************************************************************************/
SDCC_STATUS
sdcc_handle_write
(
   struct sdcc_device *handle,
   uint32              data_address,
   byte               *buff,
   uint16              num_blocks
)
{
   uint32 driveno = 0;
   boolean          multi_block = FALSE;
   sdcc_dev_type   *sdcc_pdev   = NULL;
   sdcc_cmd_type    sdcc_cmd;
   SDCC_STATUS rc = SDCC_NO_ERROR;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DPRINTF(("Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   // fail the call if card is removed or reinserted
   if ( sdcc_pdev->pslot->slot_state_changed )
   {
      DPRINTF(("Slot %d state changed", driveno));
      return SDCC_ERR_CARD_REMOVED;
   }

   if ( SDCC_HOST_TRAN != sdcc_pdev->host_state )
   {
      if ( FALSE == sdcc_handle_silent_reinit(handle) )
      {
         rc = SDCC_ERR_CARD_INIT;
         return rc;
      }
   }

   if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
   {
      rc = SDCC_ERR_CARD_UNDETECTED;
      return rc;
   }
   else if ( SDCC_CARD_SD == sdcc_pdev->card_type ||
             SDCC_CARD_SDHC == sdcc_pdev->card_type ||
             SDCC_CARD_MMC == sdcc_pdev->card_type ||
             SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      /* set the client handle partition number before data transfer */
      rc = sdcc_set_partition( handle );
      if ( SDCC_NO_ERROR != rc )
      {
         /* failed to select the requested partition number */
         return rc;
      }

      multi_block = (1 == num_blocks) ? FALSE : TRUE;
        
      /* CMD24/CMD25 */
      sdcc_cmd.cmd = (multi_block) ? SD_CMD25_WRITE_MULTIPLE_BLOCK :
                                     SD_CMD24_WRITE_BLOCK;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
  
      if ( SDCC_CARD_SDHC == sdcc_pdev->card_type ||
           SDCC_CARD_MMCHC == sdcc_pdev->card_type )
      {
         /* in high capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in block address format */
         sdcc_cmd.cmd_arg   = data_address;
      }
      else
      {
         /* in standard capacity cards, the 32-bit argument of memory access */
         /* commands uses the memory address in byte address format */
         sdcc_cmd.cmd_arg   = data_address * (sdcc_pdev->mem.mem_info.block_len);   
      }

      sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER;
      rc = sdcc_do_transfer(sdcc_pdev, &sdcc_cmd, buff, num_blocks);
   }
   else
   {
      /* card type is SDIO */
      DPRINTF(("Operation not supported on SDIO card"));
      rc = SDCC_ERR_SDIO;
      return rc;
   }

   return rc;
} /* sdcc_handle_write */

/******************************************************************************
* Name: sdcc_handle_write_iovec
*
* Description:
*    This function is not supported in minimalist SDCC API
*
* Parameters:
*    handle         [IN] : a pointer to the SDCC device that was returned from
*                          sdcc_handle_open()
*    iovec          [IN] : array of I/O vector data descriptors that contain
*                          the data to be written to the memory device
*    iovcnt         [IN] : number of elements in the iovec array
*    iovec_xfer     [IN] : transfer type for the scatter/gather data request
*
* Returns:
*    SDCC_ERR_UNKNOWN
*
******************************************************************************/
SDCC_STATUS sdcc_handle_write_iovec
(
   struct sdcc_device *handle,
   const sdcc_memory_iovec_data_desc_type *iovec,
   uint32 iovcnt,
   sdcc_memory_iovec_transfer_type iovec_xfer
)
{
   (void) handle;
   (void) iovec;
   (void) iovcnt;
   (void) iovec_xfer;
   return SDCC_ERR_UNKNOWN;
} /* sdcc_handle_write_iovec */

/******************************************************************************
* Name: sdcc_handle_close
*
* Description:
*    This function de-select the device, turn off the power supply to
*    the device and switch off the controller clock.
*
* Arguments:
*    handle     [IN] : a pointer to a SDCC device
*
* Returns:
*    SDCC_NO_ERROR if successful.  Erro code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_close( struct sdcc_device *handle )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;

   /* sanity check the handle */
   if ( NULL == handle || NULL == handle->hdev )
   {
      DPRINTF(("Invalid SDCC handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* de-register the client handle first */
   sdcc_deregister_client_handle( &handle );
   /* Normally here we'd power off here, but in boot, do nothing */
   return rc;
} /* sdcc_handle_close */

/*=============================================================================
 * FUNCTION      sdcc_get_config_info
 *
 * DESCRIPTION   This function gets the hardware/target specific information
 *               such as total number of card slots, block size, and maximum
 *               number of blocks that the target and SDCC controller support.
 *
 * DEPENDENCIES  None
 *
 * PARAMETERS    sdcc_pconfig_info [OUT]: pointer to the structure to save the
 *                                        hardware/target specific information
 *
 * RETURN VALUE  SDCC_STATUS: SDCC_NO_ERROR or SDCC_ERR_UNKNOWN
 *
 * SIDE EFFECTS  None
 *
 *===========================================================================*/
SDCC_STATUS
sdcc_get_config_info( sdcc_config_info_type *sdcc_pconfig_info )
{
   if ( NULL == sdcc_pconfig_info )
   {
      DPRINTF(("Invalid config info pointer\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* total number of card slots supported */
   sdcc_pconfig_info->num_slots = SDCC_MAX_NUM_CONTROLLER;
   /* Block size supported by the SDCC controller */
   sdcc_pconfig_info->blk_size  = SDCC_DEFAULT_BLK_LENGTH_SIZE;
   /*
    * Maximum number of blocks can be requested at one time
    * If the value is 0, EFS takes the default hardcoded values.
    */
   sdcc_pconfig_info->max_blks  = (uint32)(SDCC_TEMP_DATA_BUFF_LENGTH /
                                       SDCC_DEFAULT_BLK_LENGTH_SIZE);

   return SDCC_NO_ERROR;
} /* sdcc_get_config_info */

/*=============================================================================
 * FUNCTION      sdcc_slot_has_support
 *
 * DESCRIPTION   This function checks if a slot can be used
 *               for a certain type of device or not
 *
 * DEPENDENCIES  Slot configuration should exist in BSP
 *
 * PARAMETERS    driveno [IN]: drive number
 *               stype [IN] : type of slot (memory or IO)
 *
 * RETURN VALUE  FALSE
 *
 * SIDE EFFECTS  None
 *
 *===========================================================================*/
boolean
sdcc_slot_has_support (int16 driveno, enum sdcc_slot_supports stype)
{
   sdcc_bsp_slot_type bsp_slot_type;
   boolean status = FALSE;

   if (!SDCC_DRIVENO_IS_VALID(driveno))
      return FALSE;

   /* Get Slot configuration from BSP */
   bsp_slot_type = sdcc_bsp_get_slot_type (driveno);

   if (bsp_slot_type & SDCC_BSP_EMBEDDED_MEM_SLOT)
   {
      /* Slot type is exclusive memory slot */
      if (SDCC_SLOT_MEMORY == stype)
         status = TRUE;
   }

   if (bsp_slot_type & SDCC_BSP_OPEN_SLOT)
   {
      /* Slot type is a open slot */
      status = TRUE;
   }

   return status;
}

/*=============================================================================
 * FUNCTION      sdcc_activity_timeout_event
 *
 * DESCRIPTION   This function is not supported in minimalist SDCC API
 *
 * DEPENDENCIES  None
 *
 * PARAMETERS    driveno [IN]: drive number
 *
 * RETURN VALUE  SDCC_ERR_UNKNOWN
 *
 *===========================================================================*/
SDCC_STATUS sdcc_activity_timeout_event( int16 driveno )
{
   (void)driveno;
   return SDCC_ERR_UNKNOWN;
}

/******************************************************************************
* Name: sdcc_poll_card_status
*
* Description:
*    This function polls the card state till card is READY_FOR_DATA and in the
*    state indicated in parameter card_state.
*
* Parameters:
*    driveno    [IN] : a valid drive number between 0 and the total number of
*                      card slots - 1
*    card_state [IN] : card state such as transfer state, receive state, data
*                      state
*
* Returns:
*    SDCC_NO_ERROR on success
*    error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_poll_card_status
(
   int16 driveno,
   SDCC_CARD_STATUS card_state
)
{
   SDCC_STATUS rc_status = SDCC_ERR_CARD_READY;
   sdcc_dev_type   *sdcc_pdev  = NULL;
   uint32 t = 0;
   sdcc_cmd_type sdcc_cmd;
   SDCC_CARD_STATUS current_card_state;
   uint32 ready_for_data;
   sdcc_slot_type   *pslot = NULL;
   
   if (!SDCC_DRIVENO_IS_VALID(driveno))
      return SDCC_ERR_INVALID_PARAM;

   pslot = sdcc_get_slot_handle (driveno);
   if (NULL == pslot)
   {
      DPRINTF(("sdcc_poll_card_status: can't get slot handle\n"));
      return SDCC_ERR_INVALID_PARAM;
   }   
   
   sdcc_pdev = &(pslot->dev);

   while ( t++ < SDCC_POLL_CARD_MAX )
   {
      memset(&sdcc_cmd, 0, sizeof(sdcc_cmd));

      sdcc_cmd.cmd = SD_CMD13_SEND_STATUS;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.cmd_arg =  SDCC_ARG_RCA(sdcc_pdev->rca);
      sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

      (void) sdcc_command( sdcc_pdev, &sdcc_cmd );

      current_card_state = (SDCC_CARD_STATUS) SDCC_XLATE_CARD_CURRENT_STATE(
         sdcc_cmd.resp[0]);
      ready_for_data = SDCC_XLATE_READY_FOR_DATA(
         sdcc_cmd.resp[0]);
      if ( card_state == current_card_state &&
           SDCC_MEM_READY_FOR_DATA == ready_for_data )
      {
         rc_status = SDCC_NO_ERROR;
         break;
      }
   }

   return rc_status;
} /* sdcc_poll_card_status */

/******************************************************************************
* Name: sdcc_handle_erase
*
* Description:
*    This function is to erase data on the device, the commands used
*    to perform the erase operation should only be used on eMMC cards
*
* Arguments:
*    handle     [IN] : a pointer to a SDCC device
*    start_addr [IN] : the start of the sector
*    num_blocks [IN] : number of sectors to be erased
*
* Returns:
*    SDCC_NO_ERROR if successful.  Erro code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_erase
(
   struct sdcc_device *handle,
   uint32 start_addr,
   uint32 num_blocks
)
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   sdcc_cmd_type    sdcc_cmd;
   uint32           driveno = 0;
   sdcc_dev_type   *sdcc_pdev   = NULL;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DPRINTF(("Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   /* get the device info specified by the current interface */

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   if ( (sdcc_pdev->card_type != SDCC_CARD_MMCHC ) &&
        (sdcc_pdev->card_type != SDCC_CARD_MMC ) )
   {
      DPRINTF(("Function not supported by inserted card", driveno));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   /* set the client handle partition number before data transfer */
   rc = sdcc_set_partition( handle );
   if ( SDCC_NO_ERROR != rc )
   {
      /* failed to select the requested partition number */
      return rc;
   }

   /* CMD35: set erase start */
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg   = start_addr;
   sdcc_cmd.cmd       = SD_CMD35_TAG_ERASE_GROUP_START;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }

   /* CMD36: set erase end */
   if (sdcc_pdev->card_type == SDCC_CARD_MMCHC )
   {
      sdcc_cmd.cmd_arg   = start_addr + num_blocks - 1;
   }
   else 
   {
      sdcc_cmd.cmd_arg   = start_addr + (num_blocks - 1) * SDCC_DEFAULT_BLK_LENGTH_SIZE;
   }

   sdcc_cmd.cmd       = SD_CMD36_TAG_ERASE_GROUP_END;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }
  
   /* CMD38: erase those blocks */
   sdcc_cmd.prog_scan = 1;
   sdcc_cmd.cmd_arg   = 0;
   sdcc_cmd.cmd       = SD_CMD38_ERASE;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR == rc)     
   {
      if (sdcc_cmd.resp[0] & SDCC_CARD_ERASE_FAIL)
      {
         rc = SDCC_ERR_ERASE;
      }
   }

   /* Erase command can take multiple of seconds to complete, if the sdcc_command 
    * indicates program done error, continue to wait */
   if (rc == SDCC_ERR_PROG_DONE)     
   {
      if (sdcc_cmd.resp[0] & SDCC_CARD_ERASE_FAIL)
      {
         rc = SDCC_ERR_ERASE;
      }
      else
      {
         while ((rc = sdcc_wait_prog_done (sdcc_pdev)) == SDCC_ERR_PROG_DONE);
      }
   }
    
   if (SDCC_NO_ERROR != rc)
   {
      DPRINTF(("sdcc_erase failed\n"));
   }
   return rc;
} /* sdcc_handle_erase */


/******************************************************************************
* Name: sdcc_enable_hw_reset
*
* Description:
*    This function enable the hardware reset function provided by the MMC
*    hardware.  Note, this function only work once.  Once reset is enabled
*    on a MMC device, it can not be disabled in the future.  This is a 
*    non-volatile update.
*
* Arguments:
*    driveno          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_enable_hw_reset (struct sdcc_device *handle)
{
   SDCC_STATUS    rc = SDCC_NO_ERROR;
   uint32         driveno = 0;
   sdcc_dev_type *sdcc_pdev   = NULL;

   if ( NULL == handle || NULL == handle->hdev )
   {
      DPRINTF(("Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   sdcc_pdev = handle->hdev;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   if ( SDCC_CARD_MMCHC != sdcc_pdev->card_type )
   {
      DPRINTF(("Function not supported by inserted card", driveno));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }
   
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_EMMC_ENABLE_HW_RESET);
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed in changing bus width %d", rc));
      return rc;
   }

   return rc;
}


/******************************************************************************
* Name: sdcc_handle_set_active_bootable_partition
*
* Description:
*    This function sets a bootable partition on the embedded device specified
*    in the SDCC \c handle.  Only one partition can be set as active bootable
*    partition -- boot partition 1, boot partition 2, or user area.
*
* Parameters:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open
*
* Returns:
*    TRUE if memory device is present.  FALSE otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_handle_set_active_bootable_partition( struct sdcc_device *handle )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   uint32 driveno = 0;

   if (handle == NULL || handle->hdev == NULL)
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   do
   {
      sdcc_pdev = handle->hdev;

      if (sdcc_pdev->host_state != SDCC_HOST_TRAN &&
          sdcc_handle_silent_reinit(handle) == FALSE)
      {
         rc = SDCC_ERR_CARD_INIT;
         break;
      }

      /* only handle setting active boot partition for eMMC device */
      if (sdcc_pdev->card_type == SDCC_CARD_MMC ||
          sdcc_pdev->card_type == SDCC_CARD_MMCHC)
      {
         rc = sdcc_mmc_set_active_bootable_partition(handle);
      }
      else
      {
         rc = SDCC_ERR_FEATURE_UNSUPPORTED;
      }
   } while (0);

   return rc;
} /* sdcc_handle_set_active_bootable_partition */

/******************************************************************************
* Name: sdcc_handle_set_power_on_write_protection
*
* Description:
*    This function sets the power-on write protection bit of the addressed
*    group provided the memory device specified in the SDCC handle.  This 
*    feature is not supported in the minimized API file.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open
*    data_address  [IN] : start of the sector to get power-on write protected
*                         This is the sector unit address.  This is ignored for
*                         boot partition.
*    num_of_blocks [IN] : number of blocks to be power-on write protected
*                         (512 bytes per block).  For user area partition, this
*                         is based on the write protect group size.  If
*                         num_of_blocks < write protect group size, the entire
*                         write protect group is write protected.  This is
*                         ignored for boot partition.
*
* Returns:
*    SDCC_NO_ERROR on success.  Error code otherwise
*
* Note:
*    After a power failure or hardware reset, the power-on write protection
*    must be reapplied.
*
******************************************************************************/
SDCC_STATUS sdcc_handle_set_power_on_write_protection
(
   struct sdcc_device *handle,
   uint32 data_address,
   uint32 num_of_blocks
)
{
   (void) handle;
   (void) data_address;
   (void) num_of_blocks;

   return SDCC_ERR_FEATURE_UNSUPPORTED;
} /* sdcc_handle_set_power_on_write_protection */

/******************************************************************************
* Name: sdcc_boot_set_device_info
*
* Description:
*    This function sets/sends the device information from boot to
*    the driver to perform partial initialization.
*    This function will be called at DBL/QCSBL to set the pass
*    the device info obtained from PBL
*
* Arguments:
*    device_info       [IN] : pointer to device info
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
void sdcc_boot_set_device_info( sdcc_boot_device_info_type *device_info )
{
   if (device_info != NULL)
   {
      /* currently we support this feature only for MMC */
      if ((device_info->card_type == SDCC_CARD_MMC) ||
          (device_info->card_type == SDCC_CARD_MMCHC) ||
          (device_info->card_type == SDCC_CARD_SD) ||
          (device_info->card_type == SDCC_CARD_SDHC))
      {
         sdcc_mini_boot_device_info.driveno = device_info->driveno;
         sdcc_mini_boot_device_info.card_type = device_info->card_type;
         sdcc_mini_boot_device_info.rca = device_info->rca;
      }
   }
} /* sdcc_boot_set_device_info */

/******************************************************************************
* Name: sdcc_handle_force_erase
*
* Description:
*    This function is to force erase data on the device, the commands used
*    to perform the erase operation should only be used on Locked cards
*    - Feature removed
*
* Arguments:
*    handle     [IN] : a pointer to a SDCC device
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_force_erase(struct sdcc_device *handle)
{
   (void) handle;
   return SDCC_ERR_FEATURE_UNSUPPORTED;
} /* sdcc_handle_force_erase */


/******************************************************************************
* Name: sdcc_config_emmc_gpp_enh
*
* Description:
*    This function configures the General Purpose Partition and 
*    enhanced user data area information
*    Note: This routine can only be used once for the life of each
*    emmc chip!
* 
*
* Arguments:
*    handle     [IN] : a pointer to a SDCC device
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_config_emmc_gpp_enh (struct sdcc_device *handle,
                                      sdcc_emmc_gpp_enh_desc_type *gpp_enh_desc)
{
   if ( NULL == handle || NULL == handle->hdev )
   {
      DPRINTF(("Invalid SDCC handle 0x%x", handle));
      return SDCC_ERR_INVALID_PARAM;
   }
   
   return sdcc_mmc_config_gpp_enh (handle, gpp_enh_desc);
}

/*=============================================================================
 * FUNCTION      sdcc_get_slot_type
 *
 * DESCRIPTION   This function queries the type of sdcc slot
 *
 * PARAMETERS    driveno [IN]: drive number
 *               sdcc_slot_mem_type [OUT]: pointer to memory slot type. Slot is 
 *                                         emmc or sd memory device
 *               sdcc_slot_access_type [OUT]: pointer to slot access type. Slot is 
 *                                            internal or external memory device
 *
 * Returns:
 *    SDCC_NO_ERROR if successful.  Error code otherwise
 *
 * SIDE EFFECTS  None
 *
 *===========================================================================*/
SDCC_STATUS sdcc_get_slot_type( int16 driveno, sdcc_slot_mem_type* slot_mem_type,
                                sdcc_slot_access_type* slot_access_type)
{
   sdcc_bsp_slot_type bsp_slot_mem_type = SDCC_BSP_NO_SLOT;
   sdcc_bsp_slot_access_type bsp_slot_access_type = SDCC_BSP_NO_SLOT;
   
   /* Get Slot configuration from BSP */
   bsp_slot_mem_type = sdcc_bsp_get_slot_type (driveno);

   if (bsp_slot_mem_type & SDCC_BSP_EMBEDDED_MEM_SLOT)
   {
      *slot_mem_type = SDCC_SLOT_EMMC;
   }
   else if (bsp_slot_mem_type & SDCC_BSP_OPEN_SLOT)
   {
      /* Slot type is a SD slot */
      *slot_mem_type = SDCC_SLOT_SD;
   }
   else
   {
      *slot_mem_type = SDCC_SLOT_UNKNOWN;   
   }

   /* Get External or Internal Slot configuration from BSP */
   bsp_slot_access_type = sdcc_bsp_get_slot_access_type (driveno);
   
   if (bsp_slot_access_type & SDCC_BSP_SLOT_INTERNAL)
   {
      /* Slot type is a internal slot */
      *slot_access_type = SDCC_SLOT_INTERNAL;
   }
   else if (bsp_slot_access_type & SDCC_BSP_SLOT_EXTERNAL)
   {
      /* Slot type is a external slot */
      *slot_access_type = SDCC_SLOT_EXTERNAL;
   }
   else
   {
      *slot_access_type = SDCC_SLOT_INVALID;   
   }
   
   return SDCC_NO_ERROR;
}

/*lint -restore */
