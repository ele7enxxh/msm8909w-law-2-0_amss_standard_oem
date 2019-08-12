/**********************************************************************
* sdcc_debug.c
*
* SDCC(Secure Digital Card Controller) driver debug function
*
* This file implements the logging facility for the SDCC driver
*
* Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
**********************************************************************/

/*=======================================================================
Edit History
            
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_debug.c#1 $
$DateTime: 2015/09/01 00:30:35 $

when       who     what, where, why
---------- --------------------------------------------------------------
2014-11-11 sb      Add null pointer check for sdcc_get_slot_handle
2013-08-12 bn      Code clean up
2013-07-18 rh      Removed code for non-boot build (not used anymore)
2010-02-19 yg      Removed #define HAL_DRIVENO. (defined once in sdcc_priv.h)
                   sdcc_init_pal() returns a value
2010-02-18 rh      Adjust the packet size for RX test to prevent flow control
2009-10-30 sc      Added feature to set active boot partition
2009-10-12 sc      Added write_protect_group_size_in_sectors display info
2009-09-29 rh      Adding raw data debugging routines
2009-08-13 sc      Added is_embedded_device in sdcc_display_mem_device_info
2009-06-05 sp      Fix compiler Warnings at boot time
2009-05-20 sc      Consolidated physical partition info support to get device info
2009-04-22 sc      Changed "Number of physical partitions created" to upper case
2009-03-09 sc      Added num_phy_partition_created in
                   sdcc_display_mem_device_info
2009-02-12 vj      OS Abstraction Layer
2009-01-28 rh      Moving the display mem info function here
2009-01-26 sr      Fix WM versions 6 and 7 compatibility. 
2009-01-19 rh      Changed so sdcc_debug.c is not included from sdcc_util.c
2009-01-19 rh      Merge the change from 02 branch 
2009-01-05 rcc     Removed copyright encumbered printf code & fix lint errors

**********************************************************************/

#include "sdcc_priv.h"
#include "sdcc_handle_util.h"
#include "sdcc_util.h"
#include "sdcc_bsp.h"

#if !defined(FEATURE_SDCC_FOR_BOOT)
#include "msg.h"
#include "AEEstd.h"
#endif /* !FEATURE_SDCC_FOR_BOOT */

/******************************************************************************
* Name: sdcc_display_mem_device_info
*
* Description:
*    This function simply displays the memory device information on QXDM.
*
* Arguments:
*    handle         [IN] : a pointer to the SDCC device that was returned from
*                          sdcc_handle_open()
*    sdcc_pmem_info [IN] : pointer to the structure that contains the memory
*                          card specific information
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_display_mem_device_info
(
   struct sdcc_device *handle,
   const sdcc_mem_info_type *sdcc_pmem_info
)
{
   int16 driveno = 0;
   uint32 phy_partition_num = 0;

   driveno = SDCC_HANDLE_GET_DRIVENO(handle);

   /* translate physical partition number of the client handle */
   /* SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE is translated as */
   /* partition number 0 which is the user area. */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   switch ( sdcc_pmem_info->card_type )
   {
   case SDCC_CARD_SD:
      DPRINTF(("Slot %d, CARD TYPE: SD", driveno));
      break;

   case SDCC_CARD_SDHC:
      DPRINTF(("Slot %d, CARD TYPE: SDHC", driveno));
      break;

   case SDCC_CARD_MMC:
      DPRINTF(("Slot %d, CARD TYPE: MMC", driveno));
      break;

   case SDCC_CARD_MMCHC:
      DPRINTF(("Slot %d, CARD TYPE: MMCHC", driveno));
      break;

   default:
      DPRINTF(("Slot %d, CARD TYPE: Not detected", driveno));
      return;
   }

   DPRINTF(("Slot %d, PHYSICAL PARTITION NUMBER: %u", driveno,
      phy_partition_num));
   DPRINTF(("Slot %d, PHYSICAL PARTITION TYPE: 0x%x", driveno,
      sdcc_pmem_info->phy_partition_type));
   
   if (sdcc_pmem_info->is_phy_partition_bootable)
   {
      DPRINTF(("Slot %d, PHYSICAL PARTITION IS BOOTABLE", driveno));
   }
   else
   {
      DPRINTF(("Slot %d, PHYSICAL PARTITION IS NON-BOOTABLE", driveno));
   }

   if ( phy_partition_num )
   {
      /* simply display the physical partition size in Kilobytes */
      DPRINTF(("Slot %d, PHYSICAL PARTITION SIZE: %u Kilobytes", driveno,
         (sdcc_pmem_info->card_size_in_sectors * sdcc_pmem_info->block_len) /
         1024));
   }
   else
   {
      DPRINTF(("Slot %d, CARD SIZE: %u Megabytes", driveno,
         ((sdcc_pmem_info->card_size_in_sectors / 1024) *
         sdcc_pmem_info->block_len) / 1024));
   }

   DPRINTF(("Slot %d, WRITE PROTECT GROUP SIZE: %u sectors", driveno,
      sdcc_pmem_info->write_protect_group_size_in_sectors));
   DPRINTF(("Slot %d, BLOCK LENGTH: %u bytes", driveno,
      sdcc_pmem_info->block_len));

   if ( SDCC_CARD_SD == sdcc_pmem_info->card_type ||
        SDCC_CARD_SDHC == sdcc_pmem_info->card_type )
   {
      switch ( sdcc_pmem_info->speed_class.sd_speed_class )
      {
      case SDCC_SD_SPEED_CLASS_0:
         DPRINTF(("Slot %d, SD SPEED CLASS: 0", driveno));
         break;

      case SDCC_SD_SPEED_CLASS_2:
         DPRINTF(("Slot %d, SD SPEED CLASS: 2", driveno));
         break;

      case SDCC_SD_SPEED_CLASS_4:
         DPRINTF(("Slot %d, SD SPEED CLASS: 4", driveno));
         break;

      case SDCC_SD_SPEED_CLASS_6:
         DPRINTF(("Slot %d, SD SPEED CLASS: 6", driveno));
         break;

      default:
         DPRINTF(("Slot %d, SD SPEED CLASS: Not detected", driveno));
         break;
      }
   }
   else if ( SDCC_CARD_MMC == sdcc_pmem_info->card_type ||
             SDCC_CARD_MMCHC == sdcc_pmem_info->card_type )
   {
      switch ( sdcc_pmem_info->speed_class.mmc_speed_class )
      {
      case SDCC_MMC_SPEED_CLASS_LOWEST:
         DPRINTF(("Slot %d, MMC SPEED CLASS: 0", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_A:
         DPRINTF(("Slot %d, MMC SPEED CLASS: A", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_B:
         DPRINTF(("Slot %d, MMC SPEED CLASS: B", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_C:
         DPRINTF(("Slot %d, MMC SPEED CLASS: C", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_D:
         DPRINTF(("Slot %d, MMC SPEED CLASS: D", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_E:
         DPRINTF(("Slot %d, MMC SPEED CLASS: E", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_F:
         DPRINTF(("Slot %d, MMC SPEED CLASS: F", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_G:
         DPRINTF(("Slot %d, MMC SPEED CLASS: G", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_H:
         DPRINTF(("Slot %d, MMC SPEED CLASS: H", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_J:
         DPRINTF(("Slot %d, MMC SPEED CLASS: J", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_K:
         DPRINTF(("Slot %d, MMC SPEED CLASS: K", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_M:
         DPRINTF(("Slot %d, MMC SPEED CLASS: M", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_O:
         DPRINTF(("Slot %d, MMC SPEED CLASS: O", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_R:
         DPRINTF(("Slot %d, MMC SPEED CLASS: R", driveno));
         break;

      case SDCC_MMC_SPEED_CLASS_T:
         DPRINTF(("Slot %d, MMC SPEED CLASS: T", driveno));
         break;

      default:
         DPRINTF(("Slot %d, MMC SPEED CLASS: Not detected", driveno));
         break;
      }
   }
   else
   {
      DPRINTF(("Slot %d, SPEED CLASS: Not detected", driveno));
   }

   if ( sdcc_pmem_info->reliable_write_sector_count )
   {
      DPRINTF(("Slot %d, SUPPORT RELIABLE WRITE: TRUE", driveno));
      DPRINTF(("Slot %d, RELIABLE WRITE SECTOR COUNT: %d", driveno,
         sdcc_pmem_info->reliable_write_sector_count));
   }
   else
   {
      DPRINTF(("Slot %d, SUPPORT RELIABLE WRITE: FALSE", driveno));
   }

   if (sdcc_pmem_info->is_embedded_device == TRUE)
   {
      DPRINTF(("Slot %d, IS EMBEDDED DEVICE", driveno));
   }
   else
   {
      DPRINTF(("Slot %d, IS REMOVABLE DEVICE", driveno));
   }

   DPRINTF(("Slot %d, NUMBER OF PHYSICAL PARTITIONS CREATED: %d",
      driveno, sdcc_pmem_info->num_phy_partition_created));
   DPRINTF(("Slot %d, MANUFACTURER ID: 0x%x", driveno,
      sdcc_pmem_info->mfr_id));
   DPRINTF(("Slot %d, OEM/APPLICATION ID: 0x%x", driveno,
      sdcc_pmem_info->oem_id));
   DPRINTF(("Slot %d, PRODUCT NAME: %s", driveno,
      sdcc_pmem_info->prod_name));
   DPRINTF(("Slot %d, PRODUCT REVISION: 0x%x", driveno,
      sdcc_pmem_info->prod_rev));
   DPRINTF(("Slot %d, PRODUCT SERIAL NUMBER: 0x%x", driveno,
      sdcc_pmem_info->prod_serial_num));
   DPRINTF(("Slot %d, MANUFACTURING DATE %s", driveno,
      sdcc_pmem_info->mfr_date));
   /* To avoid compiler Warning at Boot time (qcsbl/oemsbl) */
   (void)driveno;
} /* sdcc_display_mem_device_info */


/******************************************************************************
* Name: sdcc_tx_test
*
* Description:
*    This function test the SDCC controller by transmit a fix sequence of data.
*    The function assume the SDCC controller is not initialized and will go 
*    through the entire initialization sequence. 
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
sdcc_tx_test( uint32 driveno )
{
   /* A buffer filled predefined data string */
   byte data_buffer[1024];

   /* SDCC HAL version string Provided by HAL init */
   char *sdcc_hal_ver;
   int i;

   sdcc_slot_type   *pslot = NULL;
   sdcc_dev_type    *pdev  = NULL;

   /*----------------------------------------------------------------------*/
   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      DPRINTF(("Invalid Drive Number:%d", driveno));
      return FALSE;
   }

   pslot = sdcc_get_slot_handle (driveno);
   if (NULL == pslot)
   {
      DPRINTF(("sdcc_tx_test: can't get slot handle\n"));
      return FALSE;
   }
   
   pdev = &(pslot->dev);
   pdev->pslot = pslot;
   pdev->card_type = SDCC_CARD_MMCHC;

   /* reset the controller */
   TPRINTF(SDCC_MSG_LEVEL, ("Reset SDCC controller \n"));
   sdcc_bsp_config_clock(driveno, NULL, SDCC_BSP_CLK_CONFIG_RESET);

   /* Configure GPIOs */
   sdcc_bsp_gpio_control(driveno, SDCC_BSP_GPIO_ON);
   /* Turn on the SDCC clock */
   sdcc_bsp_config_clock(driveno, NULL, SDCC_BSP_CLK_CONFIG_HCLK);
   sdcc_config_clk(pdev, SDCC_HS_TRANSFER_MODE);
   
   /* initialize the controller */
   HAL_sdcc_InitializeRegisters();
   HAL_sdcc_Init(driveno, &sdcc_hal_ver);

   /* set the power mode to 'power ON' */
   HAL_sdcc_SetPowerState (driveno, HAL_SDCC_POWER_ON);

   HAL_sdcc_BusClockEnable (driveno, TRUE);

   /* Set the controller for 4 bit access */
   HAL_sdcc_SetBusWidth(driveno, HAL_SDCC_BUS_WIDTH_4_BIT);

   /* Setup the flow control */
   HAL_sdcc_FlowControl (driveno, TRUE);

   /* Fill in the data_buffer with predefined data string */
   for(i = 0; i < 1024; i++)
   {
      data_buffer[i] = (i & 0xff);
   }
 
   /* Set up the data control register to write */
   {
      struct HAL_sdcc_DataControl data_ctrl;
      memset ((void *) &data_ctrl, 0, sizeof (data_ctrl));
      data_ctrl.block_size = 1024;
      data_ctrl.data_xfer_enable = TRUE;
      data_ctrl.direction = HAL_SDCC_DIR_WRITE;
      HAL_sdcc_DataLength (driveno, 1024);
      HAL_sdcc_DataControl (driveno, &data_ctrl);
   }

   /* Transmit the data out using FIFO transfer */
   sdcc_write_fifo(pslot, data_buffer, 1024);

   return(TRUE);
} /* sdcc_tx_test */


/******************************************************************************
* Name: sdcc_rx_test
*
* Description:
*    This function test the SDCC controller by receiving a fix sequence of data.
*    The function assume the SDCC controller is not initialized and will go 
*    through the entire initialization sequence. 
*
* Arguments:
*    driveno       [IN] : drive number
*    buffer        [OUT]: A small buffer to store received test patterns
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_rx_test( uint32 driveno, unsigned char *buffer )
{
   /* SDCC HAL version string Provided by HAL init */
   char *sdcc_hal_ver;
   struct HAL_sdcc_DataControl data_ctrl;
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_slot_type   *pslot = NULL;
   sdcc_dev_type    *pdev  = NULL;

   /*----------------------------------------------------------------------*/
   memset ((void *) &data_ctrl, 0, sizeof (data_ctrl));

   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      DPRINTF(("Invalid Drive Number:%d", driveno));
      return FALSE;
   }

   pslot = sdcc_get_slot_handle (driveno);
   if (NULL == pslot)
   {
      DPRINTF(("sdcc_rx_test: can't get slot handle\n"));
      return FALSE;
   }
   
   pdev = &(pslot->dev);
   pdev->pslot = pslot;
   pdev->card_type = SDCC_CARD_MMCHC;

   /* reset the controller */
   TPRINTF(SDCC_MSG_LEVEL, ("Reset SDCC controller \n"));
   sdcc_bsp_config_clock(driveno, NULL, SDCC_BSP_CLK_CONFIG_RESET);
   /* Configure GPIOs */
   sdcc_bsp_gpio_control(driveno, SDCC_BSP_GPIO_ON);
   /* Turn on the SDCC clock */
   sdcc_bsp_config_clock(driveno, NULL, SDCC_BSP_CLK_CONFIG_HCLK);
   sdcc_config_clk(pdev, SDCC_HS_TRANSFER_MODE);
   
   /* initialize the controller */
   HAL_sdcc_InitializeRegisters();
   HAL_sdcc_Init(driveno, &sdcc_hal_ver);

   /* set the power mode to 'power ON' */
   HAL_sdcc_SetPowerState (driveno, HAL_SDCC_POWER_ON);

   HAL_sdcc_BusClockEnable (driveno, TRUE);

   /* Set the controller for 4 bit access */
   HAL_sdcc_SetBusWidth(driveno, HAL_SDCC_BUS_WIDTH_4_BIT);

   /* Setup the flow control */
   HAL_sdcc_FlowControl (driveno, TRUE);
   /* Setup the timeout period to be long */
   HAL_sdcc_DataTimeout (driveno, 0xffffffff);
 
   /* Set up the data control register to read */
   
   data_ctrl.block_size = 64;
   data_ctrl.data_xfer_enable = TRUE;
   data_ctrl.direction = HAL_SDCC_DIR_READ;
   HAL_sdcc_DataLength (driveno, 64);
   HAL_sdcc_DataControl (driveno, &data_ctrl);

   /* Receive the data out using FIFO transfer */
   rc = sdcc_read_fifo(pslot, buffer, 64);

   /* Turn off the data path when finished */
   data_ctrl.data_xfer_enable = FALSE;
   HAL_sdcc_DataControl (driveno, &data_ctrl);

   return(rc == SDCC_NO_ERROR ? TRUE : FALSE);
} /* sdcc_rx_test */



