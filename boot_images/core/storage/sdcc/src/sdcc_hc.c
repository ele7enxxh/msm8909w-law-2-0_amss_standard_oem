/**********************************************************************
 * sdcc_hc.c
 *
 * Implementation of SD Host Controller Standard interface
 *
 * Copyright (c) 2013-2014
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header:
$DateTime:

when        who     what, where, why
----------  ---     ---------------------------------------------------
2014-12-15  sb      Enable ADMA support for device programmer
2014-11-11  sb      Add null pointer check for sdcc_get_slot_handle
2014-05-16  sb      Add SDHCi SetCapabilities. Code Clean up
2014-03-11  bn      Code Clean up
2014-02-12  bn      Fixed Klocwork warnings 
2013-09-14  bn      Fixed 8084 MTP Card Detection issue
2013-09-03  bn      Initial release

======================================================================*/

#include "sdcc_hc.h"
#include "HALhwio.h"
#include "msmhwiobase.h"
#include "HALsdhci.h"
#include "sdcc_util.h"

#pragma pack(push)
#pragma pack(1)
typedef struct sSDHCADMADescriptor32bit_tag
{
   uint16      nADMAFlags;
   uint16      nDataSize;
   uint32      nDataAddress;
} sSDHCADMADescriptor32bit;
#pragma pack(pop)

/* SDHCi configuration flags */
const boolean SDCCSdhciEnable = TRUE;
const boolean DMASupportEnable = TRUE;

/******************************************************************************
* Name: sdcc_init_sdhci_mode
*
* Description:
*    This function initializes the SDCC controller to using the Host Controller 
*    interface for communication with the memory devices.
*    It first turns on the clock, configures the GPIOs and then turns on the 
*    power supply to the device.  It also initializes the related software data
*    structures.
*
* Arguments:
*    driveno   [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_init_sdhci_mode (uint32 driveno)
{
   sdcc_slot_type   *sdcc_pdata = NULL;
   sdcc_dev_type    *sdcc_pdev  = NULL;

   /* SDCC HAL version string Provided by HAL init */
   char *sdcc_hal_ver;
   uint32 apps_max_clk = 0;

   sdcc_pdata = sdcc_get_slot_handle (driveno);
   if (NULL == sdcc_pdata)
   {
      DPRINTF(("sdcc_init_sdhci_mode: can't get slot handle\n"));
      return FALSE;
   }
   
   sdcc_pdev = &(sdcc_pdata->dev);
   sdcc_pdev->pslot = sdcc_pdata;

   /* The controller already turned on */
   if ((sdcc_pdev->host_state  == SDCC_HOST_IDENT) ||
       (sdcc_pdev->host_state  == SDCC_HOST_TRAN))
   {
      return TRUE;
   }

   if (FALSE == sdcc_pdata->drive_in_use)
   {
      /* intialize the dev data structs */
      memset(&sdcc_pdata->dev, 0, sizeof(sdcc_pdata->dev));
      sdcc_pdev->pslot = sdcc_pdata;
      sdcc_pdev->driveno = driveno;
      sdcc_pdata->driveno = driveno;
      sdcc_pdata->drive_in_use = TRUE;
      sdcc_pdev->sdhci_mode = TRUE;
   }

   /* Configure GPIOs */
   sdcc_bsp_gpio_control(driveno, SDCC_BSP_GPIO_ON);
   
   /* Initialize MPM timer and busywait */
   sdcc_bsp_hw_busy_wait_init();
   
   /* Set the gcc_sdc_apps_clock to max supported clock */
   apps_max_clk = sdcc_bsp_get_max_supported_clk (driveno);
   sdcc_bsp_config_clock (driveno, &apps_max_clk, SDCC_BSP_CLK_CONFIG_MCLK);

   /* Turn on the SDCC clock */
   sdcc_config_clk(sdcc_pdev, SDCC_INIT_MODE);

   /* Switching clock takes a while for the clock source to settle */
   sdcc_osal_sleep(SDCC_CLK_RAMP_TIME_MS);
   /* initialize the controller */
   HAL_sdcc_InitializeRegisters();
   HAL_sdcc_Init (driveno, &sdcc_hal_ver);

   /* update device info */
   sdcc_pdev->driveno        = driveno;
   sdcc_pdev->host_state     = SDCC_HOST_IDENT;
   sdcc_pdev->cache_align_sz = sdcc_bsp_get_cache_align_size();
   sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;

   /* set the MCI power mode to 'power on' */
   HAL_sdcc_SetPowerState(driveno, HAL_SDCC_POWER_ON);
   /* Enable SDHC mode */
   HAL_sdcc_HCModeEnable (driveno, HAL_SDHCI_SDHC_MODE);
   /* Reset SDHCi registers */
   HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_ALL);
   /* Set the controller's capabilities */
   HAL_sdhci_SetCapabilities(driveno);
   /* Get controller's capabilities to see what features it supports */
   HAL_sdhci_GetCapabilities (driveno, &(sdcc_pdata->capabilities));
   /* Turn on power to the memory device */
   (void)sdcc_hc_set_power_mode (driveno, TRUE);
   /* Disable interrupts */
   HAL_sdhci_InterruptSignalEnable(driveno, HAL_SDHCI_INTERRUPT_MSK_ALL, FALSE);
   /* Enable Statuses */
   HAL_sdhci_InterruptStatusEnable (driveno, HAL_SDHCI_INTERRUPT_MSK_ALL, TRUE);
   /* Clear Statuses */
   HAL_sdhci_ResetInterruptStatus(driveno, HAL_SDHCI_INTERRUPT_MSK_ALL);
   /* Is DDR supported? This ddr_support flag will be used later in the 
    * card initialization to determine if we should turn on DDR mode for 
    * data transfer */
   sdcc_pdev->sdhci_ddr_support = ((sdcc_pdata->capabilities.HAL_sdhci_Reg_44_46) & 
               HAL_SDHCI_CAPS_DDR_50_SUPPORT_BMSK) ? TRUE : FALSE;
   
   /* Is ADMA supported? */
   if ((TRUE == DMASupportEnable) && (sdcc_pdata->capabilities.HAL_sdhci_Reg_40_42 & 
       HAL_SDHCI_CAPS_ADMA2_SUPPORT_BMSK))
   {
      sdcc_pdev->sdhci_adma_enable = TRUE;
      HAL_sdhci_ADMAEnable (driveno, TRUE);
   }
   else
   {
      sdcc_pdev->sdhci_adma_enable = FALSE;
      HAL_sdhci_ADMAEnable (driveno, FALSE);
   }
   
   return(TRUE);
}

/******************************************************************************
* Name: sdcc_hc_set_power_mode
*
* Description:
*    This function sets the voltage level supported in the Capabilities registers. 
*    It then turns the bus power on/off as indicated in the bOn passed in parameter

* Arguments:
*    driveno     [IN]: drive number
*    bOn         [IN]: TRUE to turn bus pwer ON. OFF otherwise
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS sdcc_hc_set_power_mode (uint32 driveno, boolean bOn)
{
   uint32 nCapabilitiesRegVal = 0;
   HAL_sdhci_BusVolSelect voltage = HAL_SDHCI_SD_BUS_VOLTAGE_18_BMSK;
   uint8 CurrentState = 0, NewState = 0;
   struct sdcc_slot_data *pslot = NULL;

   pslot = sdcc_get_slot_handle (driveno);
   if (NULL == pslot)
   {
      DPRINTF(("sdcc_hc_set_power_mode: can't get slot handle\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   nCapabilitiesRegVal = pslot->capabilities.HAL_sdhci_Reg_40_42;
   if (nCapabilitiesRegVal & HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_3V_BMSK)
   {
      voltage = HAL_SDHCI_SD_BUS_VOLTAGE_33_BMSK;
   }
   else if (nCapabilitiesRegVal & HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_BMSK)
   {
      voltage = HAL_SDHCI_SD_BUS_VOLTAGE_30_BMSK;
   }
   else if (nCapabilitiesRegVal & HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_BMSK)
   {
      voltage = HAL_SDHCI_SD_BUS_VOLTAGE_18_BMSK;
   }  
   else
   {
      return SDCC_ERR_UNKNOWN;
   }

   /* change voltage and power state if it's different with the existing setting */
   CurrentState = HAL_sdhci_GetPowerState (driveno);
   NewState = voltage | (uint8)bOn;
   if (NewState == CurrentState)
   {
      return SDCC_NO_ERROR;;
   }
   
   HAL_sdhci_SetPower (driveno, FALSE);
   
   if (TRUE == bOn)
   {
      /* set the voltage level first, then set the power on bit */
      HAL_sdhci_VoltageSelect (driveno, voltage);   
      /* turn the bus power on */
      HAL_sdhci_SetPower (driveno, TRUE);
      /* handle all the power interrupt/status sequence */
      HAL_sdhci_HandlePowerInterrupts (driveno);
   }

   return SDCC_NO_ERROR;
}

/******************************************************************************
 * Name: sdcc_hc_set_clock
 *
 * Description:
 *    This function sets the SCLK frequency that goes to the memory device
 *
 * Arguments:
 *    driveno     [IN]: drive number
 *    freq_hz     [IN]: desired SCLK frequency in unit of Hertz.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_hc_set_clock (uint32 driveno, uint32 freq_hz)
{
   uint32 nNewClkHz, nBaseClkMHz = 0;
   uint32 nDivider = 0, count = 0; 

   struct sdcc_slot_data *pslot = NULL;
   pslot = sdcc_get_slot_handle (driveno);
   if (NULL == pslot)
   {
      DPRINTF(("sdcc_hc_set_clock: can't get slot handle\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   nBaseClkMHz = ((pslot->capabilities.HAL_sdhci_Reg_40_42) & 
                   HAL_SDHCI_CAPS_BASE_SDCLK_FREQ_BMSK) >> 8;

   /* disable DLL clock */
   HAL_sdhci_DisableDLLClock (driveno);

   if (nBaseClkMHz == 0)
   {
      return SDCC_ERR_UNKNOWN; 
   }
   
   /* Base Clock is in unit of MHz */
   nBaseClkMHz *= 1000000;

   /* Disable SD clock before making any change */
   HAL_sdhci_EnableSDClk(driveno, FALSE);

   /* Our core support SD 3.0, use 10-bit divider by default */
   nDivider = nBaseClkMHz / (2*freq_hz);

   if (nDivider > 0)
   {
      nNewClkHz = nBaseClkMHz / (2*nDivider);
   }
   else
   {
      nNewClkHz = nBaseClkMHz;
   }			  
     
   while (nNewClkHz > freq_hz)
   {
      nDivider++;
      nNewClkHz = nBaseClkMHz / (2*nDivider);
   }

   /* Use 10-bit clock mode. See HC specs for details */   
   HAL_sdhci_SetClock_10Bits_Mode (driveno, nDivider);

   count = SDHCI_STATUS_POLL_MAX;

   /* Wait for internal CLK to stable */
   do
   {
      if (TRUE == HAL_sdhci_IsClkStable(driveno))
      {
        /* Enable the SCLK */
        HAL_sdhci_EnableSDClk(driveno, TRUE);   
        return SDCC_NO_ERROR;
      }

      sdcc_udelay(1);
   } while (count--);

   /* count timeout expired */
   return SDCC_ERR_TIMEOUT;
}

/******************************************************************************
* Name: sdcc_hc_wait_cmd_ready
*
* Description:
*    This function checks the CMD and DATA Inhibit states. 
*    It returns error if the Inhibit conditions continue after the 
*    timeout period.
*
* Arguments:
*    driveno        [IN]: drive number
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS sdcc_hc_wait_cmd_ready (uint32 driveno)
{
   uint32 count = 0;

   /* Waiting for Command Inhibit clear */
   while (count < SDHCI_CMD_POLL_MAX)
   {
      if ((HAL_sdhci_GetPresentState(driveno) & 
          (HAL_SDHCI_PRESENT_STATE_CMD_INHIBIT_CMD_BMSK | 
           HAL_SDHCI_PRESENT_STATE_CMD_INHIBIT_DAT_BMSK)) == 0)
      {
         return SDCC_NO_ERROR;
      }
	  
      sdcc_udelay (100);
      count += 100;
   }

   /* Timeout error if command inhibit state is stuck */
   return SDCC_ERR_TIMEOUT;
}

/******************************************************************************
* Name: sdcc_hc_prepare_dma_descriptors
*
* Description:
*    This function prepares the 32-bit ADMA2 descriptor per the HC specs.
*
* Arguments:
*    driveno              [IN]: drive number
*    buffer               [IN]: pointer to the data buffer used in the DMA transfer
*    nTransferSizeInByte  [IN]: size of the transfer in unit of byte
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS
sdcc_hc_prepare_dma_descriptors (uint32 driveno, byte *buffer,
                                 uint32 nTransferSizeInByte)
{
   sSDHCADMADescriptor32bit *pDescTable;
   uint32 nCurrSize = 0;
   uint32 nDescCount = 0, nDescTableSize = 0;  
   uint32 pData;
   uint32 nDataLeft = nTransferSizeInByte;

   sdcc_bsp_cache_operation(SDCC_BSP_MEM_BARRIER, NULL, 0);   
   sdcc_bsp_cache_operation(SDCC_BSP_CACHE_CLEAN, buffer, nTransferSizeInByte);
   sdcc_bsp_cache_operation(SDCC_BSP_MEM_BARRIER, NULL, 0);
	 
   pDescTable = (sSDHCADMADescriptor32bit*)pDescTableAddr;
   nDescCount = (nDataLeft/MAX_DESCRIPTOR_SIZE);
   
   if (nDataLeft % MAX_DESCRIPTOR_SIZE)
   {
      nDescCount++;
   }

   nDescTableSize = nDescCount * sizeof(sSDHCADMADescriptor32bit);
   memset(pDescTableAddr, 0, nDescTableSize);

   /* First descriptor table entry points to the buffer */
   pData = (uint32) buffer;

   while (nDataLeft > 0)
   {
      nCurrSize = MAX_DESCRIPTOR_SIZE;

      if (nCurrSize > nDataLeft) 
      {   
         nCurrSize = nDataLeft;
      }

      pDescTable->nDataAddress = pData;
      pDescTable->nDataSize = (uint16)(nCurrSize & 0xFFFF);
      pDescTable->nADMAFlags = ADMA_FLAGS_DESC_VALID;

      nDataLeft -= nCurrSize;

      if (nCurrSize == 0)
      {
         pDescTable->nADMAFlags |= ADMA_FLAGS_OPERATION_NOP;
      }
      else
      {
         pDescTable->nADMAFlags |= ADMA_FLAGS_OPERATION_TRAN;
      }

      pData += nCurrSize;

      /* the last descriptor entry has a DESC_END flag set */
      if (nDataLeft == 0)
      {
         pDescTable->nADMAFlags |= ADMA_FLAGS_DESC_END;
         break;
      }
	  
      pDescTable++;
   }

   sdcc_bsp_cache_operation(SDCC_BSP_MEM_BARRIER, NULL, 0);
   sdcc_bsp_cache_operation(SDCC_BSP_CACHE_CLEAN, pDescTableAddr, nDescTableSize);
   sdcc_bsp_cache_operation(SDCC_BSP_MEM_BARRIER, NULL, 0);

   /* write the descriptor addr to the 32-bit ADMA */
   HAL_sdhci_SetADMASystemAddress64bit(driveno, 0);
   HAL_sdhci_SetADMASystemAddress (driveno, (uint32)pDescTableAddr);

   return SDCC_NO_ERROR;
}
/******************************************************************************
* Name: sdcc_hc_fifo_mode_rw
*
* Description:
*    This function is to read/write data from the memory device using
*    non-dma in Host Controller interface mode.
*
* Arguments:
*    p_device               [IN]: pointer to device structure
*    p_command              [IN]: pointer to command structure
*    buffer                 [IN/OUT]: pointer of the data buffer to write to or read from
*    transfer_size_in_byte  [IN]: transfer size in unit of bytes
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS sdcc_hc_fifo_mode_rw (sdcc_dev_type *p_device,
                                         sdcc_cmd_type *p_command,
                                         byte *buffer,
                                         uint32 transfer_size_in_byte)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   uint32 nWaitMask = 0;
   uint32 nIntStatus = 0, i = 0;
   uint32 nBlockSize = 0, nBlockSizeDWords = 0;
   uint32 nBlockCount = 0, nLeftBlockCount = 0;
   uint32 driveno = p_device->driveno;
   uint32 *pCurrData = (uint32 *)(buffer);

   if (SDCC_MEM_BLK_MODE == p_device->mem.block_mode)
   {
      nBlockSize = p_device->mem.mem_info.block_len;
      nBlockCount = (transfer_size_in_byte/nBlockSize) ;
   }
   else
   {
      /* Byte mode, transfer less than 512 bytes */
      nBlockSize = transfer_size_in_byte;
      nBlockCount = 1;
   }

   nBlockSizeDWords = (nBlockSize + 3) >> 2;
   nLeftBlockCount = nBlockCount;

   if (SDCC_IS_READ_TRANSFER(p_command->flags))
   {
      nWaitMask = HAL_SDHCI_NORMAL_INT_STS_BUF_RD_READY_BMSK;
   }
   else if (SDCC_IS_WRITE_TRANSFER(p_command->flags))
   {
      nWaitMask = HAL_SDHCI_NORMAL_INT_STS_BUF_WR_READY_BMSK;
   }
   else
   {
      return SDCC_ERR_UNKNOWN;
   }

   do
   {
      rc = sdcc_hc_wait_interrupt_status (driveno, nWaitMask, &nIntStatus);
      if (rc != SDCC_NO_ERROR)
      {
         p_command->status = nIntStatus;
         return rc;
      }

      HAL_sdhci_ResetInterruptStatus(driveno, nWaitMask);

      i = 0;
      if (SDCC_IS_READ_TRANSFER(p_command->flags))
      {
         while (i < nBlockSizeDWords)
         {
            *pCurrData = HAL_sdhci_ReadDataBufPort(driveno);
            pCurrData++;
            i++;
         }
      }
      else if (SDCC_IS_WRITE_TRANSFER(p_command->flags))
      {
         while (i < nBlockSizeDWords)
         {
            HAL_sdhci_WriteDataBufPort(driveno, *pCurrData);
            pCurrData++;
            i++;
         }
      }

      nLeftBlockCount--;
   }
   while (nLeftBlockCount > 0);

   return rc;
}

/******************************************************************************
 * Name: sdcc_do_transfer_sdhci_mode
 *
 * Description:
 *    This is a generic function to perform a read or write data transfer on
 *    the SD-card using the Host Controller standard interface.
 *
 * Arguments: 
 *    p_device             [IN] : SDCC context data for this SD-card
 *    p_command            [IN] : command to send to the SD-card to perform
 *                                the read or write
 *    buffer           [IN/OUT] : buffer of data to read or write
 *    transfer_size        [IN] : When in block mode, this is the number of
 *                                blocks to transfer.  When in byte mode, it
 *                                is the number of bytes to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_do_transfer_sdhci_mode (sdcc_dev_type *p_device, 
                                         sdcc_cmd_type *p_command,
                                         byte *buffer,
                                         uint16 transfer_size)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   uint32 nIntStatus = 0, nWaitMask = 0;
   uint32 nTransferSize = 0, nBlkSize = 0, nBlkCount = 0;
   uint32 driveno = p_device->driveno;

   nBlkSize = (SDCC_MEM_BYTE_MODE == p_device->mem.block_mode) ?
                transfer_size : p_device->mem.mem_info.block_len;

   nBlkCount = (SDCC_MEM_BYTE_MODE == p_device->mem.block_mode) ?
                1 : transfer_size;

   /* transfer size in unit of bytes */
   nTransferSize = nBlkSize*nBlkCount;

   nIntStatus = HAL_sdhci_GetInterruptStatus(driveno);
   
   /* clear all previous interrupts */
   if (HAL_SDHCI_INTERRUPT_MSK_ALL & nIntStatus)
   {
      HAL_sdhci_ResetInterruptStatus(driveno, HAL_SDHCI_INTERRUPT_MSK_ALL);
      /* reset CPSM & DPSM on errors, abort transaction if not finished */
      if (HAL_SDHCI_INTERRUPT_ERROR_MSK_ALL & nIntStatus)
      {
         HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_CMD_DAT);
      }
   }

   if (TRUE == p_device->sdhci_adma_enable)
   {
      (void) sdcc_hc_prepare_dma_descriptors(driveno, buffer, nTransferSize);	
   }

   /* configure the number of bytes in a data block */
   HAL_sdhci_SetBlockSize(driveno, nBlkSize);

   /* configure the numbe of blocks in a data transfer */
   HAL_sdhci_SetBlockCount(driveno, nBlkCount);

   /* set up the Transfer Mode register. This is done before sending CMD */
   sdcc_hc_set_transfer_mode(p_device, p_command, nBlkCount);

   /* send command now */
   if (SDCC_IS_APP_COMMAND(p_command->flags))
   {
      rc = sdcc_send_app_command(p_device, p_command);
   }
   else
   {
      rc = sdcc_command(p_device, p_command);
   }

   if (SDCC_NO_ERROR != rc)
   {
      goto ErrorExit;
   }

   /* CMD sent. Response received. Now read or write data */
   nWaitMask = SDHCI_STATUS_TRANS_COMPLETE_BMSK;

   if (FALSE == p_device->sdhci_adma_enable)
   {
      rc = sdcc_hc_fifo_mode_rw(p_device, p_command, buffer, nTransferSize);
      if (SDCC_NO_ERROR != rc)
      {
         goto ErrorExit;
      }
   }
   else
   {
      nWaitMask |= SDHCI_STATUS_ADMA_ERR_BMSK;
   }

   /* poll the transfer complete or any error status bits */
   rc = sdcc_hc_wait_interrupt_status(driveno, nWaitMask, &nIntStatus);
   if (SDCC_NO_ERROR != rc)
   {
      p_command->status = nIntStatus;
      goto ErrorExit;
   }

   if (nIntStatus & SDHCI_STATUS_TRANS_COMPLETE_BMSK)
   {
      /* Data transfer succeeded. Clear Transfer Complete status */
      HAL_sdhci_ResetInterruptStatus (driveno, SDHCI_STATUS_TRANS_COMPLETE_BMSK);

      if (SDCC_IS_APP_COMMAND(p_command->flags) != 0)
      {
         goto TransferDone;
      }

      /* Multi block transfer, send CMD12 to stop transfer */
      if (nBlkCount > 1)
      {
         sdcc_cmd_type sdcc_cmd;
	  
         sdcc_cmd.cmd       = SD_CMD12_STOP_TRANSMISSION;
         sdcc_cmd.resp_type = SDCC_RESP_SHORT;
         sdcc_cmd.prog_scan = (SDCC_IS_WRITE_TRANSFER(p_command->flags)) ? 1 : 0;
         sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
         sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;
         rc = sdcc_command(p_device, &sdcc_cmd);
         
         goto TransferDone;
      }
   }

TransferDone:
   sdcc_bsp_cache_operation(SDCC_BSP_MEM_BARRIER, NULL, 0);
   if (SDCC_IS_READ_TRANSFER(p_command->flags) && (TRUE == p_device->sdhci_adma_enable))
   {
     sdcc_bsp_cache_operation(SDCC_BSP_CACHE_INVAL, buffer, nTransferSize);
     sdcc_bsp_cache_operation(SDCC_BSP_MEM_BARRIER, NULL, 0);
   }

   HAL_sdhci_SoftwareReset(driveno, HAL_SDHCI_RESET_CMD_DAT);
   return rc;

ErrorExit:
   sdcc_sdhci_print_error(driveno);
   return rc;
}

/******************************************************************************
* Name: sdcc_hc_set_transfer_mode
*
* Description:
*    This function sets up the data transfer. The Host Driver must set the 
*    Transfer Mode Register before issuing a command which transfers data. 
*    Writes to this reigster are ignored when the DAT Command Inhibit bit 
*    in the Present State register is set.
*
* Arguments:
*    p_device       [IN]: pointer to device structure
*    p_command      [IN]: pointer to command structure
*    block_count    [IN]: transfer size in unit of 512-byte blocks
*
* Returns:
*    Error Code
*
******************************************************************************/
static void sdcc_hc_set_transfer_mode (sdcc_dev_type *p_device, 
                                       sdcc_cmd_type *p_command, 
                                       uint16 block_count)
{
   struct HAL_sdhci_TransMode transfer_mode;
   memset(&transfer_mode, 0, sizeof(transfer_mode));

   if (SDCC_IS_READ_TRANSFER(p_command->flags))
   {
      transfer_mode.direction = 1;
   }

   if (block_count > 1)
   {
      transfer_mode.multiblock = 1;
   }
   /* not doing any auto cmd stuff now. */
   transfer_mode.autocmd_en = 0;

   transfer_mode.blk_count_en = 1;

   transfer_mode.dma_en = p_device->sdhci_adma_enable;
   
   HAL_sdhci_SetTransferMode (p_device->driveno, &transfer_mode); 
}

/******************************************************************************
* Name: sdcc_hc_config_buswidth
*
* Description:
*    This function configures the MMC bus width in Host Controller interface mode.
*
* Arguments:
*    driveno          [IN] : drive number
*    bus_width        [IN] : MMC bus width
*
* Returns:
*    Error Code
*
******************************************************************************/
SDCC_STATUS sdcc_hc_config_buswidth (uint32 driveno, SDCC_MMC_BUSWIDTH buswidth)
{
   HAL_sdhci_InterruptStatusEnable(driveno, HAL_SDHCI_INTERRUPT_MSK_ALL, FALSE);

   switch (buswidth)
   {
   case SDCC_MMC_BUSWIDTH_1BIT:
      HAL_sdhci_SetBusWidth (driveno, HAL_SDCC_BUS_WIDTH_1_BIT);
      break;

   case SDCC_MMC_BUSWIDTH_4BIT:
   case SDCC_MMC_BUSWIDTH_DDR_4BIT:
      HAL_sdhci_SetBusWidth (driveno, HAL_SDCC_BUS_WIDTH_4_BIT);
      break;

   case SDCC_MMC_BUSWIDTH_8BIT:
   case SDCC_MMC_BUSWIDTH_DDR_8BIT:
      HAL_sdhci_SetBusWidth (driveno, HAL_SDCC_BUS_WIDTH_8_BIT);
      break;

   default:
      return SDCC_ERR_INVALID_PARAM;
   }

   HAL_sdhci_InterruptStatusEnable(driveno, HAL_SDHCI_INTERRUPT_MSK_ALL, TRUE);
   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_hc_wait_interrupt_status
*
* Description:
*    This function reads the Normal and Error Interrupt Status registers and 
*    waits until the first specified Status bit is set. It returns error if the 
*    expected Status bits are not set within a timeout period
*
* Arguments:
*    driveno        [IN]: drive number
*    mask           [IN]: Status bits that we are waiting for
*    pnIntStatus    [OUT]: returned copy from reading the Status Regs
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS 
sdcc_hc_wait_interrupt_status(uint32 driveno, uint32 mask, 
                              uint32 *pnIntStatus)
{
   uint32 status = 0, count = 0;
   
   /* Check if the mask contains an interrupt status bit */
   if ((mask & HAL_SDHCI_INTERRUPT_MSK_ALL) == FALSE)
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   while (count < SDHCI_STATUS_POLL_MAX)
   {
      /* Wait for first Status bit that is set, either error or normal status bit */
      status = HAL_sdhci_GetInterruptStatus (driveno);

      if ((status & mask) || (status & HAL_SDHCI_NORMAL_INT_STS_ERR_INT_BMSK))
      {
         /* save a copy of the Status and pass it back to caller */
         *pnIntStatus = status;
		 
         if (status & HAL_SDHCI_INTERRUPT_ERROR_MSK_ALL)
         {
            return SDCC_ERR_UNKNOWN;
         }
         else
         {
            return SDCC_NO_ERROR;
         }
      }

      sdcc_udelay(100);
      count += 100;
   }

   return SDCC_ERR_TIMEOUT;
}

/******************************************************************************
* Name: sdcc_command_sdhci_mode
*
* Description:
*    This function is to send the command and poll the status using the standard 
*    Host Controller interface to communicate with the memory devices
*
* Arguments:
*    sdcc_pdev  [IN] : pointer to device structure
*    sdcc_cmd   [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_command_sdhci_mode (sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *cmd)
{
   uint32 resp_type = 0;
   SDCC_STATUS rc = SDCC_NO_ERROR;
   boolean data_present = 0;
   uint32 nWaitMask = 0, nIntStatus = 0;
   uint32 driveno = sdcc_pdev->driveno;

   struct HAL_sdhci_Cmd hal_cmd;
   memset(&hal_cmd, 0, sizeof(hal_cmd));

   rc = sdcc_hc_wait_cmd_ready (driveno);
   if (rc != SDCC_NO_ERROR)
   {
      return SDCC_ERR_TIMEOUT;
   }

   /* set the timeout counter for the DAT line timeout */
   HAL_sdhci_SetTimeout(driveno, 0xF);
   
   HAL_sdhci_SetCmdArg(driveno, cmd->cmd_arg);

   if (SDCC_IS_READ_TRANSFER(cmd->flags) || SDCC_IS_WRITE_TRANSFER(cmd->flags))
   {
      data_present = TRUE;
   }

   hal_cmd.cmd_idx = cmd->cmd;
   hal_cmd.cmd_type = 0;
   hal_cmd.data_present = data_present;
   
   if (cmd->resp_type == SDCC_RESP_NONE)
   {
      resp_type = 0;
   }
   else if (cmd->resp_type == SDCC_RESP_LONG)
   {
      resp_type = 1;
   }
   else if (cmd->resp_type == SDCC_RESP_SHORT)
   {
      resp_type = (cmd->prog_scan == 0) ? 2 : 3;
   }

   hal_cmd.resp_type = resp_type;

   /* issue the CMD */
   HAL_sdhci_SetCommand (driveno, &hal_cmd);

   /* polling the command complete status */
   nWaitMask = SDHCI_STATUS_CMD_COMPLETE_BMSK;   
   rc = sdcc_hc_wait_interrupt_status(driveno, nWaitMask, &nIntStatus);
   if (rc != SDCC_NO_ERROR)
   {
      cmd->status = nIntStatus;
      return rc;
   }

   /* CMD sent successfully. Clear the CMD Complete status */
   nWaitMask = SDHCI_STATUS_CMD_COMPLETE_BMSK;
   HAL_sdhci_ResetInterruptStatus(driveno, nWaitMask);

   /* Get the response if expect any */   
   if (resp_type)
   {
      memset(cmd->resp, 0, sizeof(cmd->resp));
      HAL_sdhci_GetResponse (driveno, cmd->resp, (cmd->resp_type == SDCC_RESP_LONG));
   }

   /* For CMD6, CMD38 we poll the Transfer Complete status here.
    * In case of CMD25, the transfer complete status is polled later 
    * once the data has been written */
   if ((cmd->prog_scan) && (data_present == FALSE))
   {
      nWaitMask = SDHCI_STATUS_TRANS_COMPLETE_BMSK;  
      rc = sdcc_hc_wait_interrupt_status(driveno, nWaitMask, &nIntStatus);
      if (rc != SDCC_NO_ERROR)
      {
         cmd->status = nIntStatus;
         return SDCC_ERR_PROG_DONE;
      }
	  
      /* clear the Transfer Complete status */
      nWaitMask = SDHCI_STATUS_TRANS_COMPLETE_BMSK;
      HAL_sdhci_ResetInterruptStatus(driveno, nWaitMask);
   }
 
   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_hc_enable_ddr50
*
* Description:
*    This function enables the DDR50 mode
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    Error Code
*
******************************************************************************/
SDCC_STATUS sdcc_hc_enable_ddr50 (uint32 driveno)
{
   /* Disable the clock before making any change */ 
   HAL_sdhci_EnableSDClk(driveno, FALSE);
   /* Enable DDR50 mode */         
   HAL_sdhci_EnableDDR50(driveno);
   
   return SDCC_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_sdhci_print_error
*
* Description:
*    This function prints the error from the Error Interrupt Status register
*
* Arguments:
*    driveno        [IN]: drive number
*
* Returns: None
*
******************************************************************************/
static void sdcc_sdhci_print_error (uint32 driveno)
{
   uint32 status = 0;

   /* Read the Error Interrupt Status register */
   status = HAL_sdhci_GetInterruptStatus (driveno);

   if (status & SDHCI_STATUS_ERR_INT_BMSK)
      DPRINTF (("Storage SDHCI Error. Specific error below: \n"));
   else if (status & SDHCI_STATUS_CMD_TIMEOUT_ERR_BMSK)
      DPRINTF (("Storage SDHCI driver CMD_TIMEOUT_ERR \n"));
   else if (status & SDHCI_STATUS_CMD_CRC_ERR_BMSK)
      DPRINTF (("Storage SDHCI driver CMD_CRC_ERR \n"));     
   else if (status & SDHCI_STATUS_CMD_END_BIT_ERR_BMSK)   
      DPRINTF (("Storage SDHCI driver CMD_END_BIT_ERR \n")); 
   else if (status & SDHCI_STATUS_CMD_INDX_ERR_BMSK)
      DPRINTF (("Storage SDHCI driver CMD_INDEX_ERR \n")); 
   else if (status & SDHCI_STATUS_DATA_TIMEOUT_ERR_BMSK)
      DPRINTF (("Storage SDHCI driver CMD_DATA_TIMEOUT_ERR \n")); 
   else if (status & SDHCI_STATUS_DATA_CRC_ERR_BMSK)
      DPRINTF (("Storage SDHCI driver CMD_DATA_CRC_ERR \n")); 
   else if (status & SDHCI_STATUS_DATA_END_BIT_ERR_BMSK)
      DPRINTF (("Storage SDHCI driver CMD_DATA_END_BIT_ERR \n")); 
   else if (status & SDHCI_STATUS_CURRENT_LIMIT_ERR_BMSK)
      DPRINTF (("Storage SDHCI driver CURRENT_LIMIT_ERR \n")); 
   else if (status & SDHCI_STATUS_AUTO_CMD_ERR_BMSK)
      DPRINTF (("Storage SDHCI driver AUTO_CMD_ERR \n")); 
   else if (status & SDHCI_STATUS_ADMA_ERR_BMSK)
      DPRINTF (("Storage SDHCI driver ADMA_ERR \n"));
   else if (status & SDHCI_STATUS_TUNING_ERR_BMSK)
      DPRINTF (("Storage SDHCI driver TUNING_ERR \n"));
   else
      DPRINTF (("Storage SDHCI driver UNKNOWN ERROR \n"));
   
   return;
}
