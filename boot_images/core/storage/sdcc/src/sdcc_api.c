 /*****************************************************************************
 * sdcc_api.c
 *
 * SDCC(Secure Digital Card Controller) driver API for memory device.
 *
 * This file implements the SDCC driver APIs for the memory device.
 *
 * Copyright (c) 2007-2010
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================
                        Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_api.c#1 $
$DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when       who     what, where, why
-------------------------------------------------------------------------------
2010-04-20 vj      Enable Prog_Done interrupt for FIFO only mode as well
2010-04-16 rcc     Use the BSP to determine whether a slot is embedded
2010-03-20 yg      Added SPS support
                   Removed #define HAL_DRIVENO. (defined once in sdcc_priv.h)
2009-11-05 vj      Added public API and structure for boot
2009-10-30 sc      Added feature to set active boot partition
2009-10-16 sc      Added power-on write protection feature
2009-10-12 sc      Added write_protect_group_size_in_sectors
2009-08-13 sc      Added support to determine the device type
2009-08-04 sc      Enabled interrupts before calling sdcc_config_memory_device
2009-07-28 vj      Vote against sleep during data transfers
2009-07-11 vin     Expanded scope of sdcc_static_handle_list 
2009-07-15 vin     Reset host state if mem open discovers SDIO card
2009-05-20 sc      Consolidated physical partition info support to get device info
2009-04-27 sc      Added support to get physical partition info
2009-03-09 vj      Added Boot support 
2009-04-22 sc      Populated num_phy_partition_created in
                   sdcc_handle_mem_get_device_info
2009-04-13 rh      Make read/write legacy function a simple wrapper
2009-04-09 vin     Handle-based find card is called from open
2009-03-09 sc      Added handle support
2009-02-23 sc      Moved clock configuration for transfer mode after reading
                   the CSD register
2009-02-09 vj      OS Abstraction Layer
2009-02-18 rh      Remove Alternate-GPIO usage
2008-12-05 sr      HAL Adaptation for WM6.
2008-11-14 sc      Replaced reliable_write_support with
                   reliable_write_sector_count
2008-11-03 vin     Break correctly if sdcc_silent_reinit fails
2008-10-30 sc      Cleaned up file sdcc_api.h and removed sdcc_sfat.c
2008-10-24 sc      Replaced legacy code in sdcc_read/write to invoke iovec
2008-10-23 vin     Support for MMC power collapse
2008-10-17 vj      Assigned values to block size and number of blocks in
                   function sdcc_get_config_info() to populate to EFS
2008-09-29 vin     HAL implementation
2008-09-22 sc      Added I/O vector APIs for scatter/gather support
2008-07-17 umr     Added BSP calls to support slot configuration.
2008-05-16 sc      Extracted the temp data buffer codes into a new function
2008-05-15 sc      Changed IS_ALIGNED macro to SDCC_ADDR_ALIGNED_FOR_DM
2008-04-28 vin     Using sdcc_process_signals for capturing DMA done & prog done
04/01/08   sc      Fixed power down issue on slot4 when no cards on other slots
04/04/08   vin     Fixed argument type in sdcc_read_serial
03/31/08   vin     Macro to validate driveno
02/27/08   vin     Added function sdcc_slot_has_support
02/25/08   vin     Use critical sections in SDCC API functions
02/13/08   umr     Removed feature FEATURE_SDCC_EXTERNAL_DM.
01/22/08   vin     Added error check after DM operation in sdcc_read & write
01/15/08   vin     Removed check for FEATURE_SDCC_HAVE_CONFIGURATION_API
                   before function sdcc_get_config_info()
12/11/07   sc      Added conditional check before including file KxMemory.h
12/05/07   vin     Fixed code to use "driveno" after validity check
11/29/07   vin     Added support for four SD controllers
11/26/07   sc      Added KxMemoryRegion_TranslateVirtualAddress function call
                   and removed the HFAT memory allocation workaround
11/20/07   umr     Call wrapper functions to control PMICs VREG_MMC and WLAN.
11/13/07   umr     Added support for 6k clkregim api.
10/10/07   umr     Added Placeholder for Bluefin3 to avoid control of VREG_MMC.
09/26/07   sc      Reset clk_freq_in_khz in sdcc_close
09/21/07   sc      Disabled MCI_INT_MASK_PROG_DONE interrupt in sdcc_close
07/27/07   sc      Removed irrelevant comments
06/26/07   dng     Fixed an issue with powering down the first controller during
                   wlan operation on second controller.
06/13/07   dng     Lowered the debug print messages in the device polling sequence.
06/11/07   sc      Moved sdcc_temp_data_buff_array to sdcc_api.c
06/08/07   sc      Fixed CR 120559 to move out the sdcc_complete_data_xfer()
05/29/07   dng     Added FEATURE_SDCC_DUAL_SDCC_POWER_SAVE to shut down
                   the second sd controller to save power.  This feature
                   only applies to 7K targets that have dual controllers.
04/20/07   dng     Moved sdcc_wait_for_dma to sdcc_util.c
04/20/07   vin     Added driveno check in sdcc_close to avoid race
                   condition
04/20/07   dng     Moved sdcc_init to sdcc_util.c
03/06/07   sc      Added conditional check for memcpy in sdcc_read
03/01/07   sc      Cleaned up the not referenced codes
02/20/07   sc      Minor code cleanup
02/17/17   dng     Added prog_done ISR support
02/16/07   sc      Fixed pm_vote_vreg_switch enumerated type mixed warnings
02/14/07   sc      Replaced the banned API and used compiler specifer
                   for HFAT workaround buffer
02/13/07   sc      Updated MMC card type with review comments
02/08/07   sc      Ported CE-ATA feature to 7K target and did major
                   code clean up
02/06/07   sc      Cleaned up lint errors
02/01/07   sc      Added MMC v4.2 support (i.e., MMC card size >2GB)
01/30/07   sc      Provided temporary workaround for HFAT memory
                   allocation issue filed in CRs 107363, 107572,
                   107944, 109177, 109843 and 110130
01/03/07   sc      Added MMCPlus support
10/11/06   sep     CPRM support
09/25/06   dng     Added sd2.0 support.
08/13/06   hwu     Merged in support for WINCE.
07/12/06   hwu     Vote to go in and out of sleep when voluntarily
                   giving up CPU in sdcc_open().
06/24/06   hwu     changed to call sdcc_flush_dma upon data xfer failure.
05/05/06   dng     Added 2gb sd card support.
03/28/06   hwu     Added muxing support for SDC2 port.
12/21/05   hwu     Added dual SDCC support.
11/11/05   hwu     Merged MSMSHARED_SDCC_WLAN.00.00.19 to MSM7500.
09/23/05   jhs     Ported to MSM7500.
08/28/05   hwu     General cleanups.
07/24/05   hwu     Added SDCC_GET_TYPE to pass up the card type.
06/07/05   hwu     Seperated out sdcc_complete_data_xfer().
                   Seperated out sdcc_process_dma().
05/07/05   hwu     Enabled DMA interrupt for DMA operation.
04/14/05   hwu     Changed to use sdcc_setup_data_xfer.
                   Moved block_mode and enable_dma to sdcc_data_type.
03/25/05   hwu     Featurized WLAN in sdcc_init().
02/15/05   hwu     added delay after switch on the power. PHG needs 5msec.
10/29/04   hwu     new release for MSM6550 v1.2 chipset. Turn on write DMA.
07/14/04   hwu     Added DMA support for dword aligned read.
                   Cleaned up some logic.
06/23/04   hwu     Changed to use TCXO for SD, TCXO/2 for MMC.(HW issue.)
                   Limited DMA usage to read only.(HW bug.)
                   Added support for single block only mode.
06/15/04   hwu     Initial version.
=============================================================================*/
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_mmc_util.h"
#include "sdcc_iovec_util.h"
#include "sdcc_handle_util.h"
#ifndef FEATURE_SDCC_FOR_BOOT_QCSBL
#include "pm.h"
#include "clkregim.h"
#endif /* FEATURE_SDCC_FOR_BOOT_QCSBL */
#include "HALsdcc.h"
#include "sdcc_bsp.h"
#ifdef FEATURE_SDCC_BUILD_WITH_SPS
#include "sdcc_sps.h"
#endif /* FEATURE_SDCC_BUILD_WITH_SPS */ 

/* Create local storage for handles used by non-handle based APIs */
struct sdcc_device *sdcc_static_handle_list[SDCC_MAX_NUM_CONTROLLER];

sdcc_boot_device_info_type sdcc_boot_device_info = {0};

/*lint -save -e641 Suppress 'Converting enum to int' warning */
/******************************************************************************
* Name: sdcc_open
*
* Description:
*    This function returns failure if there is no memory device
*    attached.  If there is a device, it then will initialize the device
*    and take it out of the identification state.  It then sets up the
*    device for data transfer.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_open( int16 driveno )
{
   boolean ret = TRUE;
   struct sdcc_device *handle;

   /*---------------------------------------------------------------------*/
   handle = sdcc_handle_open( driveno, 
                              SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE );
   if(handle != NULL)
   {
      sdcc_static_handle_list[driveno] = handle;
   }
   else
   {
      ret = FALSE;
      DPRINTF(("sdcc_open() : handle is NULL. drive number %d", driveno));
   }

   return(ret);
} /* sdcc_open */

/******************************************************************************
* Name: sdcc_mem_get_device_info
*
* Description:
*    This function gets the memory card specific information.  If there is no
*    memory device attached or device is not initialized, all fields in the
*    card specific information are zero.
*
* Dependencies:
*    sdcc_open() must be called before getting card info for the specified
*    drive number.
*
* Parameters:
*    driveno         [IN] : a valid drive number between 0 and the total
*                           number of card slots - 1
*    sdcc_pmem_info [OUT] : pointer to the structure to save the memory card
*                           specific information
*
* Returns:
*    SDCC_NO_ERROR on success
*    error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_mem_get_device_info( int16 driveno,
                                      sdcc_mem_info_type *sdcc_pmem_info )
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }
   rc = sdcc_handle_mem_get_device_info( sdcc_static_handle_list[driveno], 
                                         sdcc_pmem_info );
   return rc;
} /* sdcc_mem_get_device_info */

/******************************************************************************
* Name: sdcc_read
*
* Description:
*    This function reads num_blocks blocks of data from the data_address in
*    the device, storing them at the memory location given by buff.
*
* Arguments:
*    driveno          [IN] : drive number
*    data_address     [IN] : start of the sector.  This is the sector unit
*                            address (512 bytes per block).
*    buff            [OUT] : pointer to buffer for the incoming data
*    num_blocks       [IN] : number of blocks to be read (512 bytes per
*                            block)
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
* Note:
*    Multiblock transfer is used when more than 1 block data is transfered.
*
******************************************************************************/
boolean
sdcc_read
(
   int16      driveno,
   uint32     data_address,
   byte      *buff,
   uint16     num_blocks
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_memory_iovec_data_desc_type iovec;
   struct sdcc_device *handle;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return FALSE;
   }

   handle = sdcc_static_handle_list[driveno];

   /* populate iovec */
   iovec.sector_address = data_address;
   iovec.buffer = (uint32*)buff;
   iovec.num_sector = num_blocks;

   /* only 1 element in the iovec array */
   rc = sdcc_handle_read_iovec( handle, &iovec, 1,
                                SDCC_MEMORY_IOVEC_REGULAR_DATA_XFER );

   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("sdcc_read() : sdcc_handle_read_iovec() returned with ERROR\n"));
   }
   
   return (rc ? FALSE : TRUE);
} /* sdcc_read */

/******************************************************************************
* Name: sdcc_write
*
* Description:
*    This function writes num_blocks blocks of data, obtained from the
*    memory location buff, to the data_address on the device.
*
* Arguments:
*    driveno          [IN] : drive number
*    data_address     [IN] : start of the sector.  This is the sector unit
*                            address (512 bytes per block).
*    buff             [IN] : pointer to buffer for the outgoing data
*    num_blocks       [IN] : number of blocks to be written (512 bytes per
*                            block)
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
* Note:
*    Multiblock transfer is used when more than 1 block data is transfered.
*
******************************************************************************/
boolean
sdcc_write
(
   int16      driveno,
   uint32     data_address,
   byte      *buff,
   uint16     num_blocks
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_memory_iovec_data_desc_type iovec;
   struct sdcc_device *handle;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return FALSE;
   }

   handle = sdcc_static_handle_list[driveno];

   /* populate iovec */
   iovec.sector_address = data_address;
   iovec.buffer = (uint32*)buff;
   iovec.num_sector = num_blocks;

   /* only 1 element in the iovec array */
   rc = sdcc_handle_write_iovec( handle, &iovec, 1,
                                 SDCC_MEMORY_IOVEC_REGULAR_DATA_XFER );

   if (rc != SDCC_NO_ERROR)   
   {
      DPRINTF(("sdcc_write() : sdcc_handle_write_iovec returned with ERROR. drive number %d", driveno));
   }
   return (rc ? FALSE : TRUE);
} /* sdcc_write */

/******************************************************************************
* Name: sdcc_read_iovec
*
* Description:
*    This function reads data from the memory device indicated in the
*    driveno parameter.  Caller is required to populate the fields in structure
*    variable iovec which includes sector address and number of sectors.  Field
*    buffer must be big enough to hold the data read from the memory device.
*    It places the data read from memory device into the iovcnt buffers
*    specified by the members of the iovec array: iovec[0], iovec[1], ...,
*    iovec[iovcnt-1].  Parameter iovec_xfer must be specified by the caller.
*
* Parameters:
*    driveno        [IN] : a valid drive number between 0 and the total
*                          number of card slots - 1
*    iovec          [IN] : array of I/O vector data descriptors that contain
*                          pointer to buffer for incoming data from the memory
*                          device
*    iovcnt         [IN] : number of elements in the iovec array
*    iovec_xfer     [IN] : transfer type for the scatter /gather data request
*
* Returns:
*    SDCC_NO_ERROR on success
*    error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_read_iovec
(
   int16 driveno,
   const sdcc_memory_iovec_data_desc_type *iovec,
   uint32 iovcnt,
   sdcc_memory_iovec_transfer_type iovec_xfer
)
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   struct sdcc_device *handle = sdcc_static_handle_list[driveno];

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   rc = sdcc_handle_read_iovec( handle, iovec, iovcnt, iovec_xfer );
   return rc;
} /* sdcc_read_iovec */

/******************************************************************************
* Name: sdcc_write_iovec
*
* Description:
*    This function gathers data from the iovcnt buffers specified by the
*    members of the iovec array: iovec[0], iovec[1], ..., iovec[iovcnt-1].
*    It writes data to the memory device indicated in the driveno parameter.
*    Caller is required to populate the fields in structure variable iovec
*    which includes sector address, data buffer and number of sectors.
*    Parameter iovec_xfer must be specified by the caller.
*
* Parameters:
*    driveno        [IN] : a valid drive number between 0 and the total
*                          number of card slots - 1
*    iovec          [IN] : array of I/O vector data descriptors that contain
*                          the data to be written to the memory device
*    iovcnt         [IN] : number of elements in the iovec array
*    iovec_xfer     [IN] : transfer type for the scatter/gather data request
*
* Returns:
*    SDCC_NO_ERROR on success
*    error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_write_iovec
(
   int16 driveno,
   const sdcc_memory_iovec_data_desc_type *iovec,
   uint32 iovcnt,
   sdcc_memory_iovec_transfer_type iovec_xfer
)
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   rc = sdcc_handle_write_iovec( sdcc_static_handle_list[driveno], 
                                 iovec, iovcnt, iovec_xfer);
   return rc;
} /* sdcc_write_iovec */

/******************************************************************************
* Name: sdcc_read_serial
*
* Description:
*    This function is to read some drive information for SFAT.
*
*    !!! WARNING !!! WARNING !!! WARNING !!! WARNING !!! WARNING !!! WARNING !!
*    This function will be removed once BREW/Hotplug removed the invocation of
*    this function.
*
* Arguments:
*    driveno          [IN] : drive number
*    iDrvPtr         [OUT] : pointer to the drive geometry descriptor that is
*                            to be filled
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_read_serial
(
   int16             driveno,
   DRV_GEOMETRY_DESC *iDrvPtr
)
{
   sdcc_data_type *sdcc_pdata = NULL;
   sdcc_dev_type  *sdcc_pdev  = NULL;

   /*-----------------------------------------------------------------------*/
   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      DPRINTF(("Invalid Drive Number:%d", driveno));
      return FALSE;
   }

   sdcc_pdev = sdcc_get_slot_handle (driveno);
   sdcc_pdata = &(sdcc_pdev->pslot);

   // fail the call if card is removed or reinserted
   if ( sdcc_info[driveno].slot_state_changed )
   {
      return FALSE;
   }

   sdcc_osal_enter_crit_sect(sdcc_pdata->crit_sect);

   iDrvPtr->dfltCyl          = 0;
   iDrvPtr->dfltHd           = 0;
   iDrvPtr->dfltSct          = 0;

   iDrvPtr->totalLBA         = sdcc_pdev->mem.mem_info.card_size_in_sectors;
   iDrvPtr->dfltBytesPerSect = sdcc_pdev->mem.mem_info.block_len;

   iDrvPtr->serialNum[0]     = sdcc_pdev->mem.mem_info.prod_serial_num;

   memcpy(iDrvPtr->modelNum, sdcc_pdev->mem.mem_info.prod_name,
      sizeof(sdcc_pdev->mem.mem_info.prod_name));

   sdcc_osal_exit_crit_sect(sdcc_pdata->crit_sect);
   return TRUE;
} /* sdcc_read_serial */

/******************************************************************************
* Name: sdcc_close
*
* Description:
*    This function de-select the device, turn off the power supply to
*    the device and switch off the controller clock.
*
* Arguments:
*    driveno       [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_close( int16 driveno )
{
   boolean           ret = TRUE;
   SDCC_STATUS       status = SDCC_NO_ERROR;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return FALSE;
   }

   status = sdcc_handle_close( sdcc_static_handle_list[driveno] );

   if(status != SDCC_NO_ERROR)
   {
      DPRINTF(("sdcc_close() : sdcc_handle_close() returned with ERROR"));
      ret = FALSE;
   }

   return ret;
} /* sdcc_close */

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
          (device_info->card_type == SDCC_CARD_MMCHC))
      {
         memset(&sdcc_boot_device_info, 0, sizeof(sdcc_boot_device_info_type));
         memcpy(&sdcc_boot_device_info, device_info,
                sizeof(sdcc_boot_device_info_type));
      }
   }
} /* sdcc_boot_set_device_info */

/******************************************************************************
* Name: sdcc_boot_get_device_info
*
* Description:
*    This function gets the device information from driver to
*    boot to send it across to driver later.
*    This function will be called at DBL/QCSBL to get
*    the additional device info like CSD, CID etc...
*    after enumeration at DBL/QCSBL to be passed on to OSBL/OEMSBL
*    to avoid enumeration at OSBL/OEMSBL.
*
* Arguments:
*    device_info       [IN] : pointer to device info
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_boot_get_device_info( sdcc_boot_device_info_type *device_info )
{

   if (device_info != NULL)
   {
      /* currently we support this feature only for MMC */
      if ((sdcc_boot_device_info.card_type == SDCC_CARD_MMC) ||
          (sdcc_boot_device_info.card_type == SDCC_CARD_MMCHC))
      {
         memcpy(device_info, &sdcc_boot_device_info,
               sizeof(sdcc_boot_device_info_type));
      }
   }
} /* sdcc_boot_get_device_info */

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
   sdcc_data_type     *sdcc_pdata = NULL;
   sdcc_dev_type      *sdcc_pdev  = NULL;
   boolean             ret        = TRUE;
   struct sdcc_device *ret_handle = NULL;
   boolean             still_in_use = TRUE;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid Drive Number: %d", driveno));
      return NULL;
   }

   if (sdcc_is_partial_enumeration(driveno))
   {
      ret_handle = sdcc_handle_optimized_open(phy_partition_num);
      return ret_handle;
   }
   
   ret = sdcc_init( driveno );
   if ( FALSE == ret )
   {
      DPRINTF(("Failed to initialize drive number %d", driveno));
      return NULL;
   }

   sdcc_pdata = sdcc_get_slot_handle (driveno);
   sdcc_pdev = &(sdcc_pdata->dev);

   sdcc_osal_enter_crit_sect(sdcc_pdata->crit_sect);
   do
   {
      /* register the client handle first */
      ret_handle = sdcc_register_client_handle( (uint32)driveno,
                                                phy_partition_num );
      if ( NULL == ret_handle )
      {
         /* check if memory device in driveno is still in use */
         still_in_use = sdcc_is_device_in_use( sdcc_pdev );
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

      /* Allow app's power collapse for WINCE */
      /* not to got to sleep while initializing devices */
      sdcc_vote_against_sleep(sdcc_pdata->sleep_handle);

      /* find the attached device */
      sdcc_pdev->card_type = sdcc_handle_find_card( ret_handle );
      /* update card_type field in sdcc_mem_info struct */
      sdcc_pdev->mem.mem_info.card_type = sdcc_pdev->card_type;

      /* Allow app's power collapse for WINCE */
      /* vote to sleep now */
      sdcc_vote_for_sleep(sdcc_pdata->sleep_handle);

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
         /* allow SDIO routines to initialize the device */
         sdcc_pdev->host_state = SDCC_HOST_IDLE;
         DPRINTF(("Found SDIO device.\n"));
         break;
      }

      /* proceed if this is SD/MMC device */
      /* finish identification mode */
      rc = sdcc_init_memory_device (sdcc_pdev);
      if (SDCC_NO_ERROR != rc)
      {
         DPRINTF(("Failed to initialize the card.\n"));
         sdcc_pdev->errno = rc;

         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      HAL_sdcc_FlowControl (driveno, TRUE);

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

      /* Enable interrupts first.  Some commands for MMC require */
      /* interrupt support. */
      sdcc_enable_interrupts(sdcc_pdev->driveno, SDCC_PROG_DONE_INT_MASK);

      /* config device for data transfer */
      rc = sdcc_config_memory_device(sdcc_pdev);
      if (SDCC_NO_ERROR == rc)
      {
         // set to true if card is removed / reinserted
         sdcc_pdata->slot_state_changed = FALSE;
      }
      else
      {
         sdcc_pdev->errno     = rc;
         sdcc_pdev->card_type = SDCC_CARD_UNKNOWN;
         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         DPRINTF(("sdcc_handle_open() : sdcc_config_memory_device() returned with ERROR"));
      }
   }
   while (0);
   sdcc_osal_exit_crit_sect(sdcc_pdata->crit_sect);

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
   sdcc_data_type  *sdcc_pdata = NULL;
   sdcc_dev_type   *sdcc_pdev  = NULL;
   int16            driveno    = 0;
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
	sdcc_pdata = sdcc_pdev->pslot;

   /* zero out all the memory info fields */
   memset( sdcc_pmem_info, 0, sizeof(sdcc_pmem_info) );

   /* translate physical partition number of the client handle */
   /* SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE is translated as */
   /* partition number 0 which is the user area. */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   sdcc_osal_enter_crit_sect(sdcc_pdata->crit_sect);

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
      sdcc_pmem_info->write_protect_group_size_in_sectors =
         sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors;

      /* Query the BSP to see if the slot is embedded */
      if (sdcc_bsp_get_slot_type(handle->hdev->driveno) == SDCC_BSP_EMBEDDED_MEM_SLOT)
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

   sdcc_osal_exit_crit_sect(sdcc_pdata->crit_sect);

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
   boolean ret = TRUE;
   sdcc_data_type *sdcc_pdata = NULL;
   sdcc_dev_type  *sdcc_pdev  = NULL;
   SDCC_CARD_STATUS card_status = SDCC_CARD_IDLE;
   int16 driveno = 0;

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

	sdcc_pdata = handle->hdev;
	sdcc_pdev = &(sdcc_pdata->dev);

   sdcc_osal_enter_crit_sect(sdcc_pdata->crit_sect);

   do
   {
      if ( handle->hdev->slot_state_changed )
      {
         ret = FALSE;
         break;
      }

      if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
      {
         ret = FALSE;
         break;
      }

      card_status = sdcc_send_status( sdcc_pdev );
      if ( SDCC_CARD_IGNORE == card_status )
      {
         sdcc_pdev->card_type = SDCC_CARD_UNKNOWN;
         ret = FALSE;
         break;
      }

      ret = TRUE;
   } while (0);

   sdcc_osal_exit_crit_sect(&sdcc_crit_sect[driveno]);

   return ret;
} /* sdcc_handle_is_mem_present */

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
   sdcc_data_type *sdcc_pdata = NULL;
   sdcc_dev_type *sdcc_pdev = NULL;
   int16 driveno = 0;

   if (handle == NULL || handle->hdev == NULL)
   {
      DPRINTF(("Invalid SDCC handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_osal_enter_crit_sect(&sdcc_crit_sect[driveno]);

   do
   {
      sdcc_pdata = handle->hdev;
      /* get the device info specified by the current interface */
      sdcc_pdev = &(sdcc_pdata->dev_list[sdcc_pdata->curr_intf]);

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
         DPRINTF(("Active boot partition feature not supported in driveno %d",
            driveno));
         rc = SDCC_ERR_FEATURE_UNSUPPORTED;
      }
   } while (0);

   sdcc_osal_exit_crit_sect(&sdcc_crit_sect[driveno]);

   return rc;
} /* sdcc_handle_set_active_bootable_partition */

/******************************************************************************
* Name: sdcc_handle_set_power_on_write_protection
*
* Description:
*    This function sets the power-on write protection bit of the addressed
*    group provided the memory device specified in the SDCC handle is an
*    embedded device and write protect feature is supported on the device.
*    The power-on write protection is set based on the write protect group.
*    The write protect group size is memory device specific.  This group size
*    information write_protect_group_size_in_sectors can be queried via
*    sdcc_handle_mem_get_device_info.  For boot area partitions, power-on write
*    protection would be applied to the entire partition and both boot areas
*    would be power-on write protected.
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
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_data_type *sdcc_pdata = NULL;
   sdcc_dev_type *sdcc_pdev = NULL;
   int16 driveno = 0;

   if (handle == NULL || handle->hdev == NULL)
   {
      DPRINTF(("Invalid SDCC handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_osal_enter_crit_sect(&sdcc_crit_sect[driveno]);

   do
   {
      sdcc_pdata = handle->hdev;
      /* get the device info specified by the current interface */
      sdcc_pdev = &(sdcc_pdata->dev_list[sdcc_pdata->curr_intf]);

      if (sdcc_pdev->host_state != SDCC_HOST_TRAN &&
          sdcc_handle_silent_reinit(handle) == FALSE)
      {
         rc = SDCC_ERR_CARD_INIT;
         break;
      }

      /* only handle write protection for eMMC device */
      if ((sdcc_pdev->card_type == SDCC_CARD_MMC ||
          sdcc_pdev->card_type == SDCC_CARD_MMCHC) &&
          sdcc_pdev->mem.write_prot_support == TRUE)
      {
         rc = sdcc_mmc_set_pwr_on_write_prot(handle, data_address,
                                             num_of_blocks);
      }
      else
      {
         DPRINTF(("Write protect feature not supported in driveno %d",
            driveno));
         rc = SDCC_ERR_FEATURE_UNSUPPORTED;
      }
   } while (0);

   sdcc_osal_exit_crit_sect(&sdcc_crit_sect[driveno]);

   return rc;
} /* sdcc_handle_set_power_on_write_protection */

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
*                      if end_point is SPS_ENDPOINT_INVALID then the 
*                      the requested mode Legacy mode (no SPS).
*                      if end_point is not SPS_ENDPOINT_INVALID, then it 
*                      should be the SDCC SPS peer which is the SPS_ENDPOINT_USB.
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS 
sdcc_handle_set_sps_end_point
(
   struct sdcc_device *handle,
   void *end_point
)
{   
#ifndef FEATURE_SDCC_BUILD_WITH_SPS

   SDCC_USE_PARAM (handle);
   SDCC_USE_PARAM (end_point);
   return SDCC_ERR_SPS_MODE;
   
#else /* FEATURE_SDCC_BUILD_WITH_SPS */         

   SDCC_STATUS             rc          = SDCC_NO_ERROR;
   boolean                 status      = TRUE;
   sdcc_data_type          *sdcc_pdata = NULL;                                      
   sdcc_dev_type           *sdcc_pdev  = NULL;    
   sdcc_sps_connect_type   *sdcc_sps_con = NULL;     
   int16 driveno = 0;                                 

   /* sanity check */
   if (handle == NULL)
   {
      DPRINTF(("sdcc_handle_set_sps_end_point() : handle is NULL\n"));
      return SDCC_ERR_INVALID_PARAM;
   }
   
   driveno = SDCC_HANDLE_GET_DRIVENO(handle);
   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("sdcc_handle_set_sps_end_point() : Invalid drive number %d\n", driveno));
      return SDCC_ERR_INVALID_PARAM;
   }
      
   sdcc_osal_enter_crit_sect(&sdcc_crit_sect[driveno]);
   
   do
   {         
      sdcc_pdata = handle->hdev;                                         
      sdcc_pdev = &(sdcc_pdata->dev_list[sdcc_pdata->curr_intf]);                      
      sdcc_sps_con = &sdcc_pdev->sdcc_sps_connection; 
    
      /* LEGACY MODE REQUESTED */                                                          
      if (*(DAL_sps_EndPointType*)end_point == SPS_ENDPOINT_INVALID)
      {         
         /* already in LEGACY MODE */
         if (sdcc_pdev->sdcc_mode == SDCC_LEGACY_MODE)
         {                
            sdcc_sps_con->sdcc_sps_end_point_peer_Id = SPS_ENDPOINT_INVALID;                
            sdcc_pdev->sdcc_config_transfer_cb = sdcc_config_dma;                    
            DPRINTF(("sdcc_handle_set_sps_end_point() : SDCC is already configured to work in LEGACY MODE\n"));
            rc = SDCC_NO_ERROR;                
            break;
         }
         /* currently in SPS MODE and should move to LEGACY MODE */
         else
         {
            status = sdcc_sps_disable(&sdcc_pdev->sdcc_sps_connection);
            if (status != TRUE)
            {
               DPRINTF(("sdcc_handle_set_sps_end_point() : sdcc_sps_disable() returned with ERROR\n"));
               rc = SDCC_ERR_SPS_MODE;
               break;
            }
            sdcc_sps_con->sdcc_sps_end_point_peer_Id = SPS_ENDPOINT_INVALID;                    
            sdcc_pdev->sdcc_mode = SDCC_LEGACY_MODE;                                     
            sdcc_pdev->sdcc_config_transfer_cb = sdcc_config_dma;
            DPRINTF(("sdcc_handle_set_sps_end_point() - SDCC changed mode from SPS MODE into LEGACY MODE\n"));
            rc = SDCC_NO_ERROR;
            break;
         }
      }   
      
      /* SPS MODE REQUESTED */                                                                                                                                 
      else 
      {  
         /* current mode is LEGACY */     
         if (sdcc_pdev->sdcc_mode == SDCC_LEGACY_MODE)
         {
            sdcc_sps_con->sdcc_sps_end_point_peer_Id = *(DAL_sps_EndPointType*)end_point;                    
            sdcc_pdev->sdcc_mode = SDCC_SPS_MODE;                                     
            sdcc_pdev->sdcc_config_transfer_cb = sdcc_config_dml;
               
            status = sdcc_sps_enable( driveno, sdcc_sps_con );  
            if (status != TRUE)
            {
               DPRINTF(("sdcc_handle_set_sps_end_point() : sdcc_sps_enable() returned with ERROR\n"));
               rc = SDCC_ERR_SPS_MODE;
               break;
            }
            
            rc = sdcc_dml_init(driveno, sdcc_pdev->sdcc_sps_connection);
            if (rc != SDCC_NO_ERROR)
            {
               DPRINTF(("sdcc_handle_set_sps_end_point() : sdcc_dml_init() returned with ERROR\n"));
               rc = SDCC_ERR_SPS_MODE;
            }         
         }
         /* already working in SPS MODE */
         else
         {
            /* already working with the same requested end point */
            if (sdcc_sps_con->sdcc_sps_end_point_peer_Id == *(DAL_sps_EndPointType*)end_point)
            {
               DPRINTF(("sdcc_handle_set_sps_end_point() : SDCC is already configured to work in SPS MODE with the same peer_ID\n"));
               rc = SDCC_NO_ERROR;
               break;
            }
            /* working already in SPS MODE but with a different end point */
            else
            {
               DPRINTF(("sdcc_handle_set_sps_end_point() : SDCC is already configured to work in SPS MODE with a different peer_ID=%d\n",
                     *((uint32*)sdcc_sps_con->sdcc_sps_end_point_peer_Id)));
               rc = SDCC_ERR_SPS_MODE;            
               break;
            } 
         }
      }
   } while (0);
   
   sdcc_osal_exit_crit_sect(&sdcc_crit_sect[driveno]);
   
   return rc;
   
#endif /* FEATURE_SDCC_BUILD_WITH_SPS */   
                                          
} /* sdcc_handle_set_sps_end_point */

/******************************************************************************
* Name: sdcc_handle_get_sps_end_point
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
SDCC_STATUS 
sdcc_handle_get_sps_end_point
(
   struct sdcc_device *handle,
   void *end_point
)
{
#ifndef FEATURE_SDCC_BUILD_WITH_SPS

   SDCC_USE_PARAM (handle);
   SDCC_USE_PARAM (end_point);
   return SDCC_ERR_SPS_MODE;
   
#else /* FEATURE_SDCC_BUILD_WITH_SPS */ 
   
   SDCC_STATUS          rc       = SDCC_NO_ERROR;          
   boolean              res      = FALSE;
   int16                driveno  = 0;
   
   do
   {
      /* sanity check */
      if (handle == NULL)
      {
         DPRINTF(("sdcc_handle_get_sps_end_point() : handle is NULL\n"));
         rc = SDCC_ERR_INVALID_PARAM;
         break;         
      }
      
      driveno = SDCC_HANDLE_GET_DRIVENO(handle);
      
      /* sanity check */
      if ( !SDCC_DRIVENO_IS_VALID(driveno) )
      {
         DPRINTF(("sdcc_handle_get_sps_end_point() : Invalid drive number %d\n", driveno));
         rc = SDCC_ERR_INVALID_PARAM;
         break;         
      }
      
      res = sdcc_sps_get_sdcc_end_point ( driveno, (DAL_sps_EndPointType*)end_point );
      
      if (res != TRUE)
      {
         rc = SDCC_ERR_SPS_MODE;
         break;
      }                                                                      
   } while (0);
   
   return rc;
   
#endif /* FEATURE_SDCC_BUILD_WITH_SPS */      
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
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_memory_iovec_data_desc_type iovec;

   /* populate iovec */
   iovec.sector_address = data_address;
   iovec.buffer = (uint32*)buff;
   iovec.num_sector = num_blocks;

   /* only 1 element in the iovec array */
   rc = sdcc_handle_read_iovec( handle, &iovec, 1,
      SDCC_MEMORY_IOVEC_REGULAR_DATA_XFER );

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
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_memory_iovec_data_desc_type iovec;

   /* populate iovec */
   iovec.sector_address = data_address;
   iovec.buffer = (uint32*)buff;
   iovec.num_sector = num_blocks;

   /* only 1 element in the iovec array */
   rc = sdcc_handle_write_iovec( handle, &iovec, 1,
      SDCC_MEMORY_IOVEC_REGULAR_DATA_XFER );

   return rc;
} /* sdcc_handle_write */

/******************************************************************************
* Name: sdcc_handle_read_iovec
*
* Description:
*    This function reads data from the memory device indicated in the
*    handle parameter.  Caller is required to populate the fields in structure
*    variable iovec which includes sector address and number of sectors.  Field
*    buffer must be big enough to hold the data read from the memory device.
*    It places the data read from memory device into the iovcnt buffers
*    specified by the members of the iovec array: iovec[0], iovec[1], ...,
*    iovec[iovcnt-1].  Parameter iovec_xfer must be specified by the caller.
*
* Parameters:
*    handle         [IN] : a pointer to the SDCC device that was returned from
*                          sdcc_handle_open()
*    iovec          [IN] : array of I/O vector data descriptors that contain
*                          pointer to buffer for incoming data from the memory
*                          device
*    iovcnt         [IN] : number of elements in the iovec array
*    iovec_xfer     [IN] : transfer type for the scatter /gather data request
*
* Returns:
*    SDCC_NO_ERROR on success
*    error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_read_iovec
(
   struct sdcc_device *handle,
   const sdcc_memory_iovec_data_desc_type *iovec,
   uint32 iovcnt,
   sdcc_memory_iovec_transfer_type iovec_xfer
)
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   sdcc_data_type  *sdcc_pdata = NULL;
   sdcc_dev_type   *sdcc_pdev  = NULL;
   sdcc_iovec_xfer_info_type xfer_info;
   int16            driveno    = 0;

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

   // fail the call if card is removed or reinserted
   if ( handle->hdev->slot_state_changed )
   {
      DPRINTF(("Slot %d state changed", driveno));
      return SDCC_ERR_CARD_REMOVED;
   }

   if ( NULL == iovec )
   {
      DPRINTF(("Passed in NULL iovec pointer"));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* bail out if iovec_xfer request is reliable write while performing */
   /* read operation */
   if ( SDCC_IOVEC_IS_RELIABLE_WRITE(iovec_xfer) )
   {
      DPRINTF(("Invalid reliable write request requested in a read operation"));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* populate xfer_info struct */
   memset( &xfer_info, 0, sizeof(xfer_info) );
   xfer_info.driveno = driveno;
   xfer_info.iovec = iovec;
   xfer_info.xfer_dir = SDCC_ADTC_READ;
   xfer_info.b_reliable_write = FALSE;

   sdcc_osal_enter_crit_sect(&sdcc_crit_sect[driveno]);

   do
   {
      sdcc_pdata = handle->hdev;
      /* get the device info specified by the current interface */
      sdcc_pdev = &(sdcc_pdata->dev_list[sdcc_pdata->curr_intf]);

      if ( SDCC_HOST_TRAN != sdcc_pdev->host_state )
      {
         if ( FALSE == sdcc_handle_silent_reinit(handle) )
         {
            rc = SDCC_ERR_CARD_INIT;
            DPRINTF(("sdcc_handle_read_iovec() : sdcc_handle_silent_reinit() returned with ERROR\n"));
            break;
         }
      }

      if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
      {
         rc = SDCC_ERR_CARD_UNDETECTED;
         DPRINTF(("sdcc_handle_read_iovec() : card undetected\n"));
         break;
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
            DPRINTF(("sdcc_handle_read_iovec() : sdcc_set_partition() returned with ERROR\n"));
            /* failed to select the requested partition number */
            break;
         }
         sdcc_vote_against_sleep(sdcc_pdata->sleep_handle);
         rc = sdcc_handle_iovec_data_transfer(&xfer_info, iovcnt);
         sdcc_vote_for_sleep(sdcc_pdata->sleep_handle);
         if ( SDCC_NO_ERROR != rc )
         {
            DPRINTF(("I/O vector read failed %d", rc));
            break;
         }
      }
      else
      {
         /* card type is SDIO */
         DPRINTF(("Operation not supported on SDIO card"));
         rc = SDCC_ERR_SDIO;
         break;
      }
   } while (0);

   sdcc_osal_exit_crit_sect(&sdcc_crit_sect[driveno]);

   return rc;
} /* sdcc_handle_read_iovec */

/******************************************************************************
* Name: sdcc_handle_write_iovec
*
* Description:
*    This function gathers data from the iovcnt buffers specified by the
*    members of the iovec array: iovec[0], iovec[1], ..., iovec[iovcnt-1].
*    It writes data to the memory device indicated in the handle parameter.
*    Caller is required to populate the fields in structure variable iovec
*    which includes sector address, data buffer and number of sectors.
*    Parameter iovec_xfer must be specified by the caller.
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
*    SDCC_NO_ERROR on success
*    error code otherwise
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
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   sdcc_data_type  *sdcc_pdata = NULL;
   sdcc_dev_type   *sdcc_pdev  = NULL;
   sdcc_iovec_xfer_info_type xfer_info;
   int16            driveno    = 0;

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

   // fail the call if card is removed or reinserted
   if ( handle->hdev->slot_state_changed )
   {
      DPRINTF(("Slot %d state changed", driveno));
      return SDCC_ERR_CARD_REMOVED;
   }

   if ( NULL == iovec )
   {
      DPRINTF(("Passed in NULL iovec pointer"));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* populate xfer_info struct */
   memset( &xfer_info, 0, sizeof(xfer_info) );
   xfer_info.driveno = driveno;
   xfer_info.iovec = iovec;
   xfer_info.xfer_dir = SDCC_ADTC_WRITE;
   xfer_info.b_reliable_write = FALSE;

   sdcc_osal_enter_crit_sect(&sdcc_crit_sect[driveno]);

   do
   {
      sdcc_pdata = handle->hdev;
      /* get the device info specified by the current interface */
      sdcc_pdev = &(sdcc_pdata->dev_list[sdcc_pdata->curr_intf]);

      if ( SDCC_HOST_TRAN != sdcc_pdev->host_state )
      {
         if ( FALSE == sdcc_handle_silent_reinit(handle) )
         {
            rc = SDCC_ERR_CARD_INIT;
            DPRINTF(("sdcc_handle_write_iovec() : sdcc_handle_silent_reinit() returned with ERROR\n"));
            break;
         }
      }

      if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
      {
         rc = SDCC_ERR_CARD_UNDETECTED;
         DPRINTF(("sdcc_handle_write_iovec() : card is unknown\n"));
         break;
      }
      /* bail out if memory device does not support reliable write and */
      /* iovec_xfer is reliable write */
      else if ( SDCC_IOVEC_IS_RELIABLE_WRITE(iovec_xfer) &&
                0 == sdcc_pdev->mem.mem_info.reliable_write_sector_count )
      {
         DPRINTF(("Memory device does not support reliable write"));
         rc = SDCC_ERR_INVALID_PARAM;
         break;
      }

      /* populate b_reliable_write field */
      if ( SDCC_IOVEC_IS_RELIABLE_WRITE(iovec_xfer) )
      {
         xfer_info.b_reliable_write = TRUE;
      }

      if ( SDCC_CARD_SD == sdcc_pdev->card_type ||
           SDCC_CARD_SDHC == sdcc_pdev->card_type ||
           SDCC_CARD_MMC == sdcc_pdev->card_type ||
           SDCC_CARD_MMCHC == sdcc_pdev->card_type )
      {
         /* set the client handle partition number before data transfer */
         rc = sdcc_set_partition( handle );
         if ( SDCC_NO_ERROR != rc )
         {
            /* failed to select the requested partition number */
            DPRINTF(("sdcc_handle_write_iovec() : sdcc_set_partition() returned with ERROR\n"));
            break;
         }

         sdcc_vote_against_sleep(sdcc_pdata->sleep_handle);
         rc = sdcc_handle_iovec_data_transfer(&xfer_info, iovcnt);
         sdcc_vote_for_sleep(sdcc_pdata->sleep_handle);         
         if ( SDCC_NO_ERROR != rc )
         {
            DPRINTF(("I/O vector write failed %d", rc));
            break;
         }
      }
      else
      {
         /* card type is SDIO */
         DPRINTF(("Operation not supported on SDIO card"));
         rc = SDCC_ERR_SDIO;
         break;
      }
   } while(0);

   sdcc_osal_exit_crit_sect(&sdcc_crit_sect[driveno]);

   return rc;
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
   int16 driveno = 0;
   boolean still_in_use = TRUE;
   sdcc_data_type *sdcc_pdata = NULL;
   sdcc_dev_type  *sdcc_pdev  = NULL;
#ifdef FEATURE_SDCC_BUILD_WITH_SPS
   sdcc_sps_connect_type   *sdcc_sps_con = NULL;
#endif /* FEATURE_SDCC_BUILD_WITH_SPS */

   /* sanity check the handle */
   if ( NULL == handle || NULL == handle->hdev )
   {
      DPRINTF(("sdcc_handle_close() : Invalid SDCC handle\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

	sdcc_pdata = handle->hdev;
   sdcc_pdev = &(sdcc_pdata->dev);
#ifdef FEATURE_SDCC_BUILD_WITH_SPS
   sdcc_sps_con = &sdcc_pdev->sdcc_sps_connection;
#endif /* FEATURE_SDCC_BUILD_WITH_SPS */   

   if (sdcc_pdev->sdcc_mode == SDCC_SPS_MODE)
   {
       sdcc_pdev->sdcc_mode = SDCC_LEGACY_MODE;
       sdcc_pdev->sdcc_config_transfer_cb = sdcc_config_dma;
#ifdef FEATURE_SDCC_BUILD_WITH_SPS      
       sdcc_sps_disable(sdcc_sps_con);
       sdcc_sps_con->sdcc_sps_end_point_peer_Id = SPS_ENDPOINT_INVALID;               
#endif /* FEATURE_SDCC_BUILD_WITH_SPS */       
   }

   /* de-register the client handle first */
   sdcc_deregister_client_handle( &handle );
   /* check if memory device in driveno is still in use */
   still_in_use = sdcc_is_device_in_use( sdcc_pdev );
   if ( !still_in_use )
   {
      rc = sdcc_deinit( driveno );
   }

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
 * RETURN VALUE  boolean: TRUE or FALSE
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

   switch (bsp_slot_type)
   {
   case SDCC_BSP_EMBEDDED_MEM_SLOT:
      /* Slot type is exclusive memory slot */
      if (SDCC_SLOT_MEMORY == stype)
         status = TRUE;
      break;

   case SDCC_BSP_WLAN_SLOT:
   case SDCC_BSP_UBM_SLOT:
      /* Slot type is Exclusive SDIO slot */
      if (SDCC_SLOT_SDIO == stype)
         status = TRUE;
      break;

   case SDCC_BSP_OPEN_SLOT:
      /* Slot type is a open slot */
      status = TRUE;
      break;

   case SDCC_BSP_NO_SLOT:
      /* No functional slot available */
      status = FALSE;
      break;

   default:
      DPRINTF(("Invalid BSP Slot info\n"));
   }

   return status;
}

/*=============================================================================
 * FUNCTION      sdcc_activity_timeout_event
 *
 * DESCRIPTION   This function tells SDCC that the slot specified by driveno
 *               has not been accessed for a certain period of time. If we have
 *               the ability to detect card removal / re-insertion, we shutdown
 *               the slot, otherwise do nothing.
 *
 * DEPENDENCIES  None
 *
 * PARAMETERS    driveno [IN]: drive number
 *
 * RETURN VALUE  SDCC_NO_ERROR if successful, error code otherwise
 *
 * SIDE EFFECTS  None
 *
 *===========================================================================*/
SDCC_STATUS sdcc_activity_timeout_event( int16 driveno )
{
   if (!SDCC_DRIVENO_IS_VALID(driveno))
      return SDCC_ERR_INVALID_PARAM;


   if(FALSE == sdcc_bsp_slot_interrupt_exists(driveno))
      return SDCC_ERR_UNKNOWN;

   DPRINTF(("SDCC Activity timeout on slot %d. Closing the slot.\n", driveno));
   (void)sdcc_deinit(driveno);

   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: mmc_send_status
*
* Description:
*    Checks the status of the SD/MMC card.
*
*    !!! WARNING !!! WARNING !!! WARNING !!! WARNING !!! WARNING !!! WARNING !!
*    This function will be removed once Hotplug invokes
*    sdcc_handle_is_mem_present() instead of this function.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
mmc_send_status( int16 driveno )
{
   boolean ret = FALSE;
   sdcc_data_type *sdcc_pdata = NULL;
   sdcc_dev_type *sdcc_pdev = NULL;
   SDCC_CARD_STATUS card_status = SDCC_CARD_IGNORE;

   if ( !SDCC_DRIVENO_IS_VALID(driveno) )
   {
      DPRINTF(("Invalid drive number %d", driveno));
      return FALSE;
   }

   sdcc_pdata = sdcc_get_slot_handle (driveno);
   sdcc_pdev = &(sdcc_pdata->dev);

   sdcc_osal_enter_crit_sect(sdcc_pdata->crit_sect);

   do
   {
      if ( SDCC_CARD_UNKNOWN == sdcc_pdev->card_type )
      {
         ret = FALSE;
         break;
      }

      card_status = sdcc_send_status( sdcc_pdev );
      if ( SDCC_CARD_IGNORE == card_status )
      {
         sdcc_pdev->card_type = SDCC_CARD_UNKNOWN;
         ret = FALSE;
         break;
      }

      ret = TRUE;
   } while (0);

   sdcc_osal_exit_crit_sect(sdcc_pdata->crit_sect);
   return ret;
} /* mmc_send_status */

/*lint -restore */
