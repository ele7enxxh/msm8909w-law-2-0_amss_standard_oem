/******************************************************************************
 * sdcc_pal_tool.c
 *
 * This file provides SDCC PAL definitions for TARGET TOOL platform
 *
 * Copyright (c) 2009-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*======================================================================
                        Edit History
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_pal_tool.c#1 $
$DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------
2014-11-11   sb      Add null pointer check for sdcc_get_slot_handle
2014-05-16   sb      Clean up. Removed MCI registers setting in SDHCi mode
2014-02-12   bn      Fixed Klocwork warning
2013-09-03   bn      Add SDHCi Support
2013-08-21   bn      Enable partial enumeration on external SD
2013-08-12   bn      Code clean up. Added flags to command structure
2012-06-22   vk      Clear data ctrl register after each read or write
2012-02-14   vk      Avoid re-writing to MCI_DATA_CTL register in FIFO mode
2012-01-24   rh      Use FIFO transfer if DAM transfer failed to setup
2011-08-08   pa      Clock regime DAL migration
2011-09-14   rh      Invalidate active partition when using optimized open
2011-07-14   rh      Add workaround for the SDCC3 PROG_DONE bug
2011-06-16   pa      Changes to support release of thread local errno
2011-02-28   mm      Removed delay for boot time optimization
2010-10-05   vj      Added force erase functionality
2010-11-08   rh      Optimized FIFO write
2010-10-27   rh      Port over the optimized open APIs
2010-10-19   rh      Port over the optimized FIFO read routine
2010-08-13   rh      Increase the timeout value for MMC cards
2010-08-03   rh      Unify the memory info decode function
2010-07-20   vj      Added driveno parameter for timetick API's
2010-05-27   vj      Enable feedback clock always
2010-03-23   rh      Correctly detect start bit error 
2010-03-20   yg      sdcc_init_pal() returns a boolean
2009-11-23   rh      Add a separate function to wait for program done
2009-11-14   vj      Declared sdcc_boot_device_info
2009-11-12   rh      Update the host state after sending the select signal
2009-10-28   rh      Fix issue caused by added DM support 
2009-10-23   sc      Renamed boot_partition_access to partition_access
2009-07-31   vj      Added loop in write_fifo to check for DATAEND
2009-07-24   vj      Use BSP API to set the VREG level
2009-07-17   rh      Poll program done status bit instead of polling CMD13
2009-06-05   rh      Initial Revision
=======================================================================*/

/*======================================================================
                            INCLUDE FILES FOR MODULE
=======================================================================*/
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_sd_util.h"
#include "sdcc_mmc_util.h"
#include "sdcc_handle_util.h"
#include "sdcc_bsp.h"
#include "sdcc_pal_tool.h"
#include "sdcc_hc.h"

/*********************************************************************/
/*             PROTOTYPES FOR LOCAL FUNCTIONS                        */
/*********************************************************************/
static SDCC_STATUS sdcc_set_blksz (sdcc_dev_type *sdcc_pdev);
SDCC_STATUS sdcc_wait_data_xfer_complete (sdcc_dev_type *sdcc_pdev);
extern sdcc_mini_boot_device_info_type sdcc_mini_boot_device_info;

/******************************************************************************
* Name: sdcc_init_pal
*
* Description:
*    This function is to perform PAL specific sdcc_init
*
* Arguments:
*    driveno                    [IN] : drive number
*    sdcc_osal_crit_sect_type   [IN] : Critical section pointer
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean sdcc_init_pal(sdcc_slot_type *pslot)
{   
   (void) pslot;
   return TRUE;
} /* sdcc_init_pal */

/******************************************************************************
* Name: sdcc_process_signals
*
* Description:
*     Process SDCC signals
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_process_signals( sdcc_dev_type *sdcc_pdev )
{
   uint32     status      = TRUE;

   do
   {
      status = HAL_sdcc_StatusCheckFlag (sdcc_pdev->driveno,
                                         HAL_sdcc_StatusNow (sdcc_pdev->driveno),
                                         HAL_SDCC_STATUS_PROG_DONE);
   }
   while (!status);

   return SDCC_NO_ERROR;
} /* sdcc_process_signals */

/******************************************************************************
* Name: sdcc_handle_find_card
*
* Description: 
*    This function finds out the card type specified in handle parameter. 
*
* Arguments:
*    handle [IN] : a pointer to the SDCC device that was returned from
*                  sdcc_handle_open()
*
* Returns:
*    card type that indicates if device is SD, MMC, SDIO, or unknown
*
******************************************************************************/
SDCC_CARD_TYPE sdcc_handle_find_card( struct sdcc_device *handle )
{
   SDCC_CARD_TYPE card_type = SDCC_CARD_UNKNOWN;
   uint32 driveno = 0;

   driveno = SDCC_HANDLE_GET_DRIVENO( handle );
   if (!SDCC_DRIVENO_IS_VALID( driveno ))
   {
      DPRINTF(("Invalid Drive Number:%d", driveno));
      return SDCC_CARD_UNKNOWN;
   }

   /* set the power mode to 'power on' */
   HAL_sdcc_SetPowerState( driveno, HAL_SDCC_POWER_ON );

   if (FALSE == handle->hdev->sdhci_mode)  
   {
     /* enable the clock */
     HAL_sdcc_BusClockEnable( driveno, TRUE );
     /* disable powersave on bus clock while supply voltage ramps up */
     HAL_sdcc_PowerSave( driveno, FALSE );
     sdcc_osal_sleep( SDCC_SUPPLY_RAMP_UP_TIME_MS );
     HAL_sdcc_PowerSave( driveno, TRUE );

     /* Enable Feedback Clock */      
     sdcc_osal_sleep( SDCC_SUPPLY_RAMP_UP_TIME_MS );
     HAL_sdcc_SetClockEdge( driveno, HAL_SDCC_CLOCK_EDGE_FEEDBACK );
   }
   else
   {
     HAL_sdhci_PowerSave( driveno, FALSE);
     sdcc_osal_sleep( SDCC_SUPPLY_RAMP_UP_TIME_MS );
     HAL_sdhci_PowerSave( driveno, TRUE);
   }
   
   /* lower the clock to < 400KHz for card identification */
   sdcc_config_clk( handle->hdev, SDCC_IDENTIFICATION_MODE );
   /* look for memory card */

   if (sdcc_bsp_get_slot_type(driveno) & SDCC_BSP_EMBEDDED_MEM_SLOT)
   {
      /* look for MMC device */
      card_type = sdcc_find_mmc_device( handle );
   }
   else
   {
      /* look for SD device */
      card_type = sdcc_find_sd_device( handle );
   }

   return card_type;
} /* sdcc_handle_find_card */

/******************************************************************************
* Name: sdcc_setup_data_xfer
*
* Description:
*    This function sets up the controller prior to data transfer.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*    direction          [IN] : data read or data write
*    units              [IN] : number of sectors to be read
*
* Returns:
*    data length
*
******************************************************************************/
uint32
sdcc_setup_data_xfer
(
   sdcc_dev_type  *sdcc_pdev,
   uint32          direction,
   uint16          units
)
{
   uint32 data_length = 0;
   uint16 blksz       = 1;
   int16  driveno     = sdcc_pdev->driveno;
   uint32 timeout     = 0;
   uint32 data_timer  = 0;

   if ( sdcc_pdev->card_type == SDCC_CARD_MMC ||
        sdcc_pdev->card_type == SDCC_CARD_MMCHC )
   {
      timeout = (SDCC_DATA_READ == direction) ? 
                SDCC_READ_TIMEOUT_MS : SDCC_MMC_WRITE_TIMEOUT_MS;
   }
   else
   {
      timeout = (SDCC_DATA_READ == direction) ? 
                SDCC_READ_TIMEOUT_MS : SDCC_WRITE_TIMEOUT_MS;
   }

   if(sdcc_pdev->clk_freq_in_khz)
   {
      data_timer = timeout * sdcc_pdev->clk_freq_in_khz;
   }
   else
   {
      /* sdcc_get_curr_clk_in_khz failed. Use no timeout */
      data_timer = timeout * SDCC_DEFAULT_CLK_KHZ;
   }
   HAL_sdcc_DataTimeout (driveno, data_timer);

   /* HW Note:
      1. data size in byte_mode should be the power of 2
      2. DMA doesn't work for small data size
      3. units: number of bytes in byte mode
                number of blocks in block mode
   */
   if (SDCC_MEM_BLK_MODE  == sdcc_pdev->mem.block_mode)
   {
      blksz = (uint16)sdcc_pdev->mem.mem_info.block_len;
   }
   /* set data length */
   data_length = units * blksz;
   HAL_sdcc_DataLength (driveno, data_length);
   return data_length;
} /* sdcc_setup_data_xfer */

/******************************************************************************
* Name: sdcc_config_rca
*
* Description:
*    This function is to set/get the RCA from the card.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to the device structure
*
* Returns:
*    rca value
*
******************************************************************************/
uint16
sdcc_config_rca( sdcc_dev_type *sdcc_pdev )
{
   sdcc_cmd_type    sdcc_cmd;
   uint16           rca = 0;

   /*-----------------------------------------------------------------------*/

   sdcc_cmd.cmd       = SD_CMD3_SET_RELATIVE_ADDR;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   /* CMD3: assign/get relative card address */
   if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      /* assign RCA to the card */
      rca               = 2;
      sdcc_cmd.cmd_arg  = SDCC_ARG_RCA(rca);

      (void)sdcc_command(sdcc_pdev, &sdcc_cmd);
   }
   else
   {
      sdcc_cmd.cmd_arg = SDCC_CMD_ARGU_NULL;
      (void)sdcc_command(sdcc_pdev, &sdcc_cmd);

      /* get RCA from the card */
      rca = (uint16)(sdcc_cmd.resp[0] >> SDCC_SEND_STATUS_RCA_SHFT);
   }

   return rca;
} /* sdcc_config_rca */

/******************************************************************************
* Name: sdcc_wait_card_ready
*
* Description:
*    This function is to check the card busy status by calling "send
*    status" command.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_wait_card_ready( sdcc_dev_type *sdcc_pdev )
{
   uint32               t           = 0;
   SDCC_STATUS          rc          = SDCC_ERR_CARD_READY;
   SDCC_CARD_STATUS     card_status = SDCC_CARD_IGNORE;

   while (t++ < SDCC_PROG_DONE_MAX)
   {
      card_status = sdcc_send_status(sdcc_pdev);
      if ( SDCC_CARD_TRAN == card_status )
      {
         rc = SDCC_NO_ERROR;
         break;
      }

      sdcc_udelay(100);
   }

   return rc;
} /* sdcc_wait_card_ready */

/******************************************************************************
* Name: sdcc_wait_data_complete
*
* Description:
*    This function wait until the DATA_END bit is set after a transfer
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_wait_data_xfer_complete( sdcc_dev_type *sdcc_pdev )
{
   uint32      t = 0;
   uint32      status;
   uint32      driveno = sdcc_pdev->driveno;
   SDCC_STATUS rc = SDCC_ERR_CARD_READY;

   while (t++ < SDCC_PROG_DONE_MAX)
   {
      status = HAL_sdcc_StatusNow (driveno);
      if (HAL_sdcc_StatusCheckFlag (driveno,
                                    status,
                                    HAL_SDCC_STATUS_WRITE_ERROR))
      {
         DPRINTF(("write error, SDCC status = 0x%x.\n", status));
         rc = SDCC_ERR_WRITE_FIFO;
         break;
      }
      if (HAL_sdcc_StatusCheckFlag (driveno,
                                    status,
                                    HAL_SDCC_STATUS_READ_ERROR))
      {
         DPRINTF(("read error, SDCC status = 0x%x.\n", status));
         rc = SDCC_ERR_READ_FIFO;
         break;
      }
      if (HAL_sdcc_StatusCheckFlag (driveno,
                                    status,
                                    HAL_SDCC_STATUS_DATA_END))
      {
         /* Finish data transfer, reset status bit and clear MCI register */
         HAL_sdcc_StatusClearStaticFlags (driveno);
         rc = SDCC_NO_ERROR;
         break;
      }
   }

   return rc;
} /* sdcc_wait_data_xfer_complete */

/******************************************************************************
* Name: sdcc_init_memory_device
*
* Description:
*    This function sends CMD2 to get the device ID and product related
*    information.  It then configures the device Relative Card Address and
*    completes the card identification process.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*   Card is in Identification mode.
*
*****************************************************************************/
SDCC_STATUS
sdcc_init_memory_device( sdcc_dev_type  *sdcc_pdev )
{
   sdcc_cmd_type   sdcc_cmd;
   SDCC_STATUS     rc = SDCC_ERR_UNKNOWN;
   boolean         decode_status = FALSE;

   /*------------------------------------------------------------------------*/

   /* CMD2: request card ID */
   sdcc_cmd.cmd       = (uint32)SD_CMD2_ALL_SEND_CID;
   sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_LONG;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }

   if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      decode_status = sdcc_decode_mmc_cid( &sdcc_cmd.resp[0], sdcc_pdev );
      if ( FALSE == decode_status )
      {
         return SDCC_ERR_INVALID_PARAM;
      }
   }
   else
   {
      /* Card type is SD or SDHC. */
      decode_status = sdcc_decode_sd_cid( &sdcc_cmd.resp[0], sdcc_pdev );
      if ( FALSE == decode_status )
      {
         return SDCC_ERR_INVALID_PARAM;
      }
   }

   /* CMD3: configure card RCA */
   sdcc_pdev->rca = sdcc_config_rca(sdcc_pdev);

   return (rc);
} /* sdcc_init_memory_device */


/******************************************************************************
* Name: sdcc_config_memory_device
*
* Description:
*    This function is to programm the card into data transfer mode.  It
*    selects the card, sets up the bus width and block data transfer mode.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS
sdcc_config_memory_device( sdcc_dev_type  *sdcc_pdev )
{
   SDCC_STATUS      rc          = SDCC_NO_ERROR;
   SDCC_CARD_STATUS card_status = SDCC_CARD_IGNORE;
   uint32           driveno     = sdcc_pdev->driveno;
   sdcc_scr_type    scr;
   SDCC_MMC_BUSWIDTH buswidth;

   /*------------------------------------------------------------------------*/

   /* CMD7: select the card */
   rc = sdcc_select_card(sdcc_pdev, TRUE);
   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }

   sdcc_pdev->host_state = SDCC_HOST_TRAN;

   /* CMD16: set the data block length */
   sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;
   rc = sdcc_set_blksz(sdcc_pdev);

   /* program the bus width for MMC */
   /* It has to be programmed after setting the block size */
   /* because adtc command will be sent to the MMC card */
   /* that has spec version 4.x. */
   if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      /* default MMC bus width to 1-bit mode */
      buswidth = SDCC_MMC_BUSWIDTH_1BIT;

      /* If MMC denotes a System Specification version 4.0 or higher, */
      /* this indicates the support of the SEND_EXT_CSD command (CMD8). */
      if ( sdcc_pdev->mem.spec_vers >= 4 )
      {
         rc = sdcc_config_mmc_modes_segment( sdcc_pdev );
         if ( SDCC_NO_ERROR != rc )
         {
            return rc;
         }
      }
      else
      {
         /* configure MMC bus width */
         (void) sdcc_config_mmc_bus_width(driveno, buswidth);
      }
   }

   else if ((SDCC_CARD_SD   == sdcc_pdev->card_type) ||
            (SDCC_CARD_SDHC == sdcc_pdev->card_type))
   {
      /* init num_phy_partition_created to 1 for now until new func to */
      /* create partition is in place */
      sdcc_pdev->mem.mem_info.num_phy_partition_created = 1;

      memset( &scr, 0, sizeof(sdcc_scr_type) );

      /* read the SD configuration register (SCR) -- ACMD51 */
      rc = sdcc_get_scr( sdcc_pdev, &scr );
      if ( SDCC_NO_ERROR != rc )
      {
         return rc;
      }

      if ( SDCC_SD_WIDE_BUS_WIDTH == scr.sd_bus_widths )
      {
         /* support 4-bit bus mode -- ACMD6 */
         rc = sdcc_set_sd_bus_width( sdcc_pdev );
         if ( SDCC_NO_ERROR != rc )
         {
            return rc;
         }
      }
      else
      {
         /* support 1-bit bus mode */
         /* by default, the SD card data bus width is 1-bit */
         HAL_sdcc_SetBusWidth (driveno, HAL_SDCC_BUS_WIDTH_1_BIT);
      }

      /* send ACMD13 to get the speed class from SD Status */
      /* if the SD card supports the high speed access mode */
      rc = sdcc_get_sd_speed_class( sdcc_pdev );
      if ( SDCC_NO_ERROR != rc )
      {
         /* re-init if getting SD Status failed */
         return rc;
      }

      /* only proceed to switch to high speed mode if */
      /* sd_spec >= 1 which means CMD6 is supported */
      /* If SD_SPEC field in the SCR register is of version 1.10 or higher */
      /* the SD card supports CMD6. */
      /* sd_spec = 1 means version 1.10 */
      /* sd_spec = 2 means version 2.00 */
      if ( scr.sd_spec >= 1 )
      {
         /* send CMD6 to query and set to high speed mode */
         (void) sdcc_sd_switch_to_hs( sdcc_pdev );
      }
   }

   /* CMD13: confirm now in TRAN state */
   card_status = sdcc_send_status(sdcc_pdev);
   if ( SDCC_CARD_TRAN != card_status )
   {
      rc = SDCC_ERR_INVALID_TX_STATE;
   }
   return rc;
} /* sdcc_config_memory_device */

/******************************************************************************
* Name: sdcc_set_blksz
*
* Description:
*    This function sets the block length to default block length 512Bytes.  It
*    sends CMD16 (SET_BLOCKLEN) to the memory device specified in parameter
*    sdcc_pdev.  The memory device sets the BLOCK_LEN_ERROR bit if the block
*    length is set larger than 512Bytes.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
static SDCC_STATUS
sdcc_set_blksz ( sdcc_dev_type *sdcc_pdev )
{
   SDCC_STATUS      rc          = SDCC_ERR_SET_BLKSZ;
   uint32           block_len   = 0;
   sdcc_cmd_type    sdcc_cmd;

   /*------------------------------------------------------------------------*/

   block_len   =  SDCC_DEFAULT_BLK_LENGTH_SIZE;

   sdcc_cmd.cmd       = (uint32)SD_CMD16_SET_BLOCKLEN;
   sdcc_cmd.cmd_arg   = block_len;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR == rc)
   {
      if (!(sdcc_cmd.resp[0] & SDCC_CARD_STATUS_BLKLEN_ERR))
      {
         rc = SDCC_NO_ERROR;

      }
   }

   /* re-adjust the block size */
   if (SDCC_NO_ERROR == rc)
   {
      sdcc_pdev->mem.mem_info.block_len = block_len;
      HAL_sdcc_BlockLength (sdcc_pdev->driveno, block_len);
   }

   return rc;
} /* sdcc_set_blksz */

/******************************************************************************
* Name: sdcc_complete_data_xfer
*
* Description:
*    This function is to finish up data transfer after the data has been
*    successfully transferred.  If this is a multi-block data transfer,
*    we need to send CMD12 to stop the data transfer.  If this is a
*    single block write, we need to send CMD13 to wait for card is ready.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*    direction              [IN] : read or write
*    multi_block            [IN] : TRUE or FALSE
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_complete_data_xfer
(
   sdcc_dev_type  *sdcc_pdev,
   uint32          direction,
   boolean         multi_block
)
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   sdcc_cmd_type    sdcc_cmd;

   if (multi_block)
   {
      sdcc_cmd.cmd       = SD_CMD12_STOP_TRANSMISSION;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = (SDCC_DATA_WRITE == direction) ? TRUE : 0;
      sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
      sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

      rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   }
   else
   {
      if (direction == SDCC_DATA_WRITE)
      {
         rc =  sdcc_wait_card_ready(sdcc_pdev);
      }
   }

   return rc;
} /* sdcc_complete_data_xfer */


/******************************************************************************
* Name: sdcc_read_fifo
*
* Description:
*    This function is to process "read" in non-dma mode.  It reads data
*    out of the FIFO.
*
* Arguments:
*    pslot          [IN] : Pointer to slot data structure
*    buff          [OUT] : pointer of the data buffer
*    length         [IN] : data length
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_read_fifo
(
   sdcc_slot_type *pslot,
   byte           *buff,
   uint32          length
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   uint32 status;
   int32 dsize = (int32) length;
   uint32 fifo_word = 0;
   uint32 i = 0;
   uint32 driveno = pslot->driveno;
   uint32 size = 0;
   boolean output_fifo_aligned;

   output_fifo_aligned = !(((uint32) buff) & 0x03);

   if (NULL == buff)
   {
      return SDCC_ERR_READ_FIFO;
   }

   /* normally DATAEND is set before length goes down to 0 */
   while (dsize > 0)
   {
      status = HAL_sdcc_StatusNow (driveno);

      if (HAL_sdcc_StatusCheckFlag (driveno,
                                    status,
                                    HAL_SDCC_STATUS_READ_ERROR))
      {
         DPRINTF(("read error, SDCC status = 0x%x.\n", status));
         rc = SDCC_ERR_READ_FIFO;
         break;
      }
      if (dsize &&
          HAL_sdcc_StatusCheckFlag (driveno,
                                    status,
                                    HAL_SDCC_STATUS_RXDATA_AVLBL))
      {
         // Fast read: FIFO must be half full 
         if( HAL_sdcc_StatusCheckFlag (driveno,
                                       status,
                                       HAL_SDCC_STATUS_RXFIFO_HALF_FULL) &&
             output_fifo_aligned)
         {
            size = dsize;
            HAL_sdcc_FifoBlockRead (driveno, (uint32 *)buff, &size);
            buff += dsize - size;
            dsize = size;
         }
         else
         {
            fifo_word = HAL_sdcc_FifoRead (driveno);
            if (output_fifo_aligned)
            {
               *(uint32 *)buff = fifo_word;
               buff += 4;
            }
            else
            {
               /* read in data byte by byte if buff addr not word aligned. */
               for (i = 0; i < 4; i++)
               {
                  *buff = (byte) (fifo_word & 0xFF);
                  fifo_word >>= 8;
                  buff++;
               }
            }
            dsize -= 4;
         }
      }
   }

   /* Need to reset the hardware when FIFO read error occurs to flush data out of FIFO */
   if (rc == SDCC_ERR_READ_FIFO)
   {
      sdcc_reset_reinit (pslot);
   }

   /* clear the static status bits */
   HAL_sdcc_StatusClearStaticFlags (driveno);
   return rc;
} /* sdcc_read_fifo */

/******************************************************************************
* Name: sdcc_write_fifo
*
* Description:
*    This function is to process "write" in non-dma mode.  It writes data
*    directly to FIFO.
*
* Arguments:
*    pslot          [IN] : Pointer to slot data structure
*    buff           [IN] : pointer of the data buffer
*    length         [IN] : data length
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_write_fifo
(
   sdcc_slot_type *pslot,
   byte     *buff,
   uint32    length
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   uint32 status = 0;
   int32 dsize = (int32) length;
   uint32 fifo_word = 0;
   uint32 i = 0;
   uint32 driveno = pslot->driveno;
   uint32 size = 0;
   boolean input_aligned;

   input_aligned = !(((uint32) buff) & 0x03);

   if (NULL == buff)
   {
      return SDCC_ERR_WRITE_FIFO;
   }
   HAL_sdcc_StatusClearStaticFlags (driveno);

   /* normally length becomes 0 before DATAEND is set */
   while (dsize > 0)
   {
      status = HAL_sdcc_StatusNow (driveno);
      /* check for write errors */
      if (HAL_sdcc_StatusCheckFlag (driveno,
                                    status,
                                    HAL_SDCC_STATUS_WRITE_ERROR))
      {
         DPRINTF(("write error, SDCC status = 0x%x.\n", status));
         rc = SDCC_ERR_WRITE_FIFO;
         break;
      }

      /* fill up the fifo if not full */
      if (dsize &&
          ! (HAL_sdcc_StatusCheckFlag (driveno,
                                       status,
                                       HAL_SDCC_STATUS_TXFIFO_FULL)))
      {

         // Fast write: FIFO must be half full 
         if( HAL_sdcc_StatusCheckFlag (driveno,
                                       status,
                                       HAL_SDCC_STATUS_TXFIFO_HALF_FULL) &&
             input_aligned &&
             (dsize >= 32))
         {

            size = dsize;
            HAL_sdcc_FifoBlockWrite (driveno, (uint32 *)buff, &size);
            buff += dsize - size;
            dsize = size;
            status = HAL_sdcc_StatusNow (driveno);
         }
         else if (input_aligned)
         {
            HAL_sdcc_FifoWrite (driveno, *(uint32 *) buff);
            buff += 4;
            dsize -= 4;
         }
         else
         {
            /* pack data byte by byte if buff addr is not word aligned */
            fifo_word = 0;
            for (i = 0; i < 32; i += 8)
            {
               fifo_word |= (*buff) << i;
               buff++;
            }
            HAL_sdcc_FifoWrite (driveno, fifo_word);
            dsize -= 4;
         }
      }
   }

   if (SDCC_NO_ERROR == rc)
   {
      /* Check for data end */
      while (!HAL_sdcc_StatusCheckFlag (driveno,
                                        status,
                                        HAL_SDCC_STATUS_DATA_END))
      {
         /* check for write errors */
         if (HAL_sdcc_StatusCheckFlag (driveno,
                                       status,
                                       HAL_SDCC_STATUS_WRITE_ERROR))
         {
            DPRINTF(("write error, SDCC status = 0x%x.\n", status));
            rc = SDCC_ERR_WRITE_FIFO;
            break;
         }
         status = HAL_sdcc_StatusNow (driveno);
      }
   }

   HAL_sdcc_StatusClearStaticFlags (driveno);
   return rc;
} /* sdcc_write_fifo */

/******************************************************************************
 * Name: sdcc_set_partition
 *
 * Description:
 *    This function sets the partition region on the memory device by sending
 *    a command to the device specified by the handle parameter.
 *
 * Arguments:
 *    handle       [IN] : a pointer to the SDCC device that was returned from
 *                        sdcc_handle_open()
 *
 * Returns:
 *    SDCC_NO_ERROR if successful, error code otherwise
 *
 ******************************************************************************/
SDCC_STATUS
sdcc_set_partition( struct sdcc_device *handle )
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   sdcc_dev_type   *sdcc_pdev  = NULL;
   uint32           phy_partition_num = 0;

   if ( NULL == handle )
   {
      DPRINTF(("Invalid NULL handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   /* translate phyiscal partition number of the client handle */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   /* check if the active partition number is same as the requested one */
   if ( phy_partition_num == sdcc_pdev->active_partition_num )
   {
      /* partition number is already set */
      return SDCC_NO_ERROR;
   }

   /* send CMD6 SWITCH to set the partition access */
   if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      rc = sdcc_mmc_set_partition_access( handle );
   }
   else
   {
      DPRINTF(("Set partition region not supported on card type %d",
         sdcc_pdev->card_type));
      rc = SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   return rc;
} /* sdcc_set_partition */

/******************************************************************************
 * Name: sdcc_send_app_command
 *
 * Description:
 *    This function is to send an App-command to the SD-card
 *
 * Arguments:
 *    p_app_command          [IN] : pointer to a structure describing the
 *                                  App-Command to send.  The command should
 *                                  be one of the SD_ACMDXXXX enums.
 *    p_device               [IN] : pointer to the device structure
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS
sdcc_send_app_command
(
   sdcc_dev_type   *p_device,
   sdcc_cmd_type   *p_app_command
)
{
   sdcc_cmd_type command;
   SDCC_STATUS   status;
   /*
   First, we must send 'CMD55_APP_CMD' to tell the SD-card to expect
   an app-command in the next command we send.  Set up the command
   structure and send the command.  The argument to this command is
   the card address (RCA) in the upper 16 bits.
   */
   command.cmd       = SD_CMD55_APP_CMD;
   command.cmd_arg   = SDCC_ARG_RCA(p_device->rca);
   command.resp_type = SDCC_RESP_SHORT;
   command.prog_scan = 0;
   command.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   status = sdcc_command(p_device, &command);

   /* Send the App command to the SD-card*/
   status = sdcc_command(p_device, p_app_command);

   return status;
} /* sdcc_send_app_command */

/******************************************************************************
 * Name: sdcc_do_transfer
 *
 * Description:
 *    This is a wrapper function that decides whether to use the Qualcomm
 *    or Host Controller standard interface for the data transfer.
 *
 * Arguments:
 *    p_device             [IN] : SDCC context data for this SD-card
 *    p_command            [IN] : command to send to the SD-card to perform
 *                                the read or write
 *    flags                [IN] : SDCC_TRANSFER_FLAG_IS_APP_COMMAND - Set this
 *                                bit to send the command as an app-command
 *                                SDCC_TRANSFER_FLAG_IS_READ_TRANSFER - Set
 *                                this bit for read transfers, clear it for
 *                                write transfers
 *    buffer           [IN/OUT] : buffer of data to read or write
 *    transfer_size        [IN] : When in block mode, this is the number of
 *                                blocks to transfer.  When in byte mode, it
 *                                is the number of bytes to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS
sdcc_do_transfer
(
   sdcc_dev_type     *p_device,
   sdcc_cmd_type     *p_command,
   byte              *buffer,
   uint16             transfer_size
)
{
   if(TRUE == p_device->sdhci_mode)    
   {
      return sdcc_do_transfer_sdhci_mode(p_device, p_command,
                                         buffer, transfer_size);
   }
   else
   {
      return sdcc_do_transfer_qc_mode(p_device, p_command,
                                      buffer, transfer_size);  
   }
} /* sdcc_do_transfer */

/******************************************************************************
 * Name: sdcc_do_transfer_qc_mode
 *
 * Description:
 *    This is a generic function to perform a read or write data transfer on
 *    the SD-card using the Qualcomm interface.
 *    The flags value contains a flag for whether we are
 *    sending a read or write transfer.
 *
 * Arguments: 
 *    p_device             [IN] : SDCC context data for this SD-card
 *    p_command            [IN] : command to send to the SD-card to perform
 *                                the read or write
 *    flags                [IN] : SDCC_TRANSFER_FLAG_IS_APP_COMMAND - Set this
 *                                bit to send the command as an app-command
 *                                SDCC_TRANSFER_FLAG_IS_READ_TRANSFER - Set
 *                                this bit for read transfers, clear it for
 *                                write transfers
 *    buffer           [IN/OUT] : buffer of data to read or write
 *    transfer_size        [IN] : When in block mode, this is the number of
 *                                blocks to transfer.  When in byte mode, it
 *                                is the number of bytes to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS
sdcc_do_transfer_qc_mode (sdcc_dev_type *p_device, sdcc_cmd_type *p_command,
                          byte *buffer, uint16 transfer_size)
{
   SDCC_STATUS      status             = SDCC_NO_ERROR;
   SDCC_STATUS      status_xfer        = SDCC_NO_ERROR;
   SDCC_STATUS      status_dma         = SDCC_NO_ERROR;
   uint32           length             = 0;
   boolean          send_stop_command  = FALSE;
   uint32           block_size;
   uint32           flags = p_command->flags;
   struct HAL_sdcc_DataControl data_ctrl;
   sdcc_slot_type  *p_slot = p_device->pslot;

   /*------------------------------------------------------------------*/

   /* setup the data transfer */
   /* This function sets a timeout and the data length for the transfer. */
   length = sdcc_setup_data_xfer(p_device,
                                 SDCC_DATA_DIRECTION(flags),
                                 transfer_size);

   if (!length)
   {
      p_device->errno = SDCC_ERR_INVALID_PARAM;
      return SDCC_ERR_INVALID_PARAM;
   }
   memset ((void *) &data_ctrl, 0, sizeof (data_ctrl));

   /* data length - Set a block size for the MCI_DATA_CTL register. */
   /* This is the block size in the case of block transfers or */
   /* the transfer size in the case of byte transfers. */
   block_size = (SDCC_MEM_BYTE_MODE == p_device->mem.block_mode) ?
                   transfer_size : p_device->mem.mem_info.block_len;

   data_ctrl.block_size = block_size;
   data_ctrl.data_xfer_enable = TRUE;

   if (p_slot->dma_data.hdma != NULL)
      data_ctrl.dm_enable = TRUE;

   /* This register needs to be done before sending the command */
   /* on reads and after on writes. */
   if (SDCC_IS_READ_TRANSFER(flags))
   {
      data_ctrl.direction = HAL_SDCC_DIR_READ;
      HAL_sdcc_DataControl (p_device->driveno, &data_ctrl);
   }

   /* Send the command to the SD card.  Note that app. commands use a */
   /* different function since they actually send two SD-card commands. */
   if (SDCC_IS_APP_COMMAND(flags))
   {
      status = sdcc_send_app_command(p_device, p_command);
   }
   else
   {
      status = sdcc_command(p_device, p_command);
   }

   if (SDCC_XLATE_WP_VIOLATION (p_command->resp[0]))
   {
      p_device->errno = SDCC_ERR_WP_VIOLATION;
      return SDCC_ERR_WP_VIOLATION;
   }
   
   /* Transfer the data to or from the SD-card, either using DMA or */
   /* by reading or writing the FIFO registers directly */
   if (status == SDCC_NO_ERROR)
   {
      if (p_slot->dma_data.hdma != NULL)
      {
         /* USE DMA transfer */
         status_dma = p_slot->dma_data.setup_dma (
                       p_slot, (uint32)buffer, length, 
                       SDCC_IS_READ_TRANSFER(flags) ? 
                       SDCC_DIR_IN : SDCC_DIR_OUT);

         /* This register needs to be set after DMA is up and running on write */
         if (status_dma == SDCC_NO_ERROR)
         {
            if (SDCC_IS_WRITE_TRANSFER(flags))
               HAL_sdcc_DataControl (p_device->driveno, &data_ctrl);
            /* Wait until transfer is completed */
            status_xfer =  sdcc_wait_data_xfer_complete (p_device);
         }
      }
      if (p_slot->dma_data.hdma == NULL || status_dma != SDCC_NO_ERROR)
      {
         /* USE FIFO transfer */
         if (SDCC_IS_READ_TRANSFER(flags))
         {
            status_xfer = sdcc_read_fifo(p_slot, buffer, length);
         }
         else
         {
            /* Disable DMA operation */
            data_ctrl.dm_enable = FALSE;
            HAL_sdcc_DataControl (p_device->driveno, &data_ctrl);         
            status_xfer = sdcc_write_fifo(p_slot, buffer, length);
         }

         //clear data_Ctl after each read or write
         memset ((void *) &data_ctrl, 0, sizeof (data_ctrl));
         HAL_sdcc_DataControl (p_device->driveno, &data_ctrl);
      }
   }

   /* Complete the data transfer by sending a stop command if required, */
   /* and clearing the status register */
   send_stop_command = ((SDCC_IS_APP_COMMAND(flags) == 0) &&
      ((p_command->cmd == SD_CMD18_READ_MULTIPLE_BLOCK) ||
      (p_command->cmd == SD_CMD25_WRITE_MULTIPLE_BLOCK)));

   status = (SDCC_STATUS)sdcc_complete_data_xfer(p_device,
               SDCC_DATA_DIRECTION(flags), send_stop_command);

   if (p_slot->dma_data.hdma != NULL && status_dma == SDCC_NO_ERROR)
   {
      status_xfer = p_slot->dma_data.done_dma ( p_slot,  
                    SDCC_IS_READ_TRANSFER(flags) ? 
                    SDCC_DIR_IN : SDCC_DIR_OUT);
   }

   p_device->errno = (SDCC_NO_ERROR == status_xfer) ? status : status_xfer;
   return (p_device->errno);
} /* sdcc_do_transfer */


/******************************************************************************
* Name: sdcc_get_cid
*
* Description:
*    This function sends CMD10 to get the device ID and product related
*    information.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
*****************************************************************************/
SDCC_STATUS
sdcc_get_cid( sdcc_dev_type  *sdcc_pdev )
{
   sdcc_cmd_type   sdcc_cmd;
   SDCC_STATUS     rc = SDCC_ERR_UNKNOWN;
   boolean         decode_status = FALSE;

   /*------------------------------------------------------------------------*/

   /* CMD2: request card ID */
   sdcc_cmd.cmd       = (uint32)SD_CMD10_SEND_CID;
   sdcc_cmd.cmd_arg   = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.resp_type = SDCC_RESP_LONG;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR != rc)
   {
      return rc;
   }
   
   if ( SDCC_CARD_MMC == sdcc_pdev->card_type ||
        SDCC_CARD_MMCHC == sdcc_pdev->card_type )
   {
      decode_status = sdcc_decode_mmc_cid( &sdcc_cmd.resp[0], sdcc_pdev );
      if ( FALSE == decode_status )
      {
         return SDCC_ERR_INVALID_PARAM;
      }
   }
   else
   {
      return SDCC_ERR_INVALID_PARAM;
   }
   return (rc);
} /* sdcc_get_cid */



/******************************************************************************
* Name: sdcc_partial_enumeration
*
* Description:
*    This function does partial enumeration of MMC card.
*    1. Switch to 1Bit Mode
*    2. Deselects the card to STBY state
*    3. Reduces the clock to fOD
*    4. Gets the CSD & CID information
*    5. Bumps up the clock
*    6. Selects the card & moves to TRANS state
*    7. Gets the EXT_CSD
*    8. Switches to HS mode
*    9. Switches the bus width
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
static SDCC_STATUS sdcc_partial_enumeration ( sdcc_dev_type *sdcc_pdev )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   int16 driveno = sdcc_pdev->driveno;
   SDCC_MMC_BUSWIDTH buswidth;
   
   do
   {
      /* Switch to 1Bit mode */
      buswidth = SDCC_MMC_BUSWIDTH_1BIT;
      rc = sdcc_mmc_switch(sdcc_pdev,
                           SDCC_MMCPLUS_CHANGE_BUSWIDTH |
                           ((uint32)(buswidth << 8)));
      if (rc != SDCC_NO_ERROR)
      {
         DPRINTF(("Failed in changing bus width %d", rc));
         return rc;
      }

      /* configure MMC bus width */
      (void) sdcc_config_mmc_bus_width(driveno, buswidth);

      rc = sdcc_select_card(sdcc_pdev, FALSE);
      if (rc != SDCC_NO_ERROR)
      {
         break;
      }

      rc = sdcc_get_memory_info(sdcc_pdev);
      if (rc != SDCC_NO_ERROR)
      {
         break;
      }
      
      rc = sdcc_get_cid(sdcc_pdev);
      if (rc != SDCC_NO_ERROR)
      {
         break;
      }

      HAL_sdcc_FlowControl (driveno, TRUE);
      
       /* re-program the clock for data transfer */
      sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_MODE );
    
      /* config device for data transfer */
      rc = sdcc_config_memory_device(sdcc_pdev);
      if (rc == SDCC_NO_ERROR)
      {
         sdcc_pdev->host_state = SDCC_HOST_TRAN;
         // set to true if card is removed / reinserted
         sdcc_pdev->pslot->slot_state_changed = FALSE;
      }
   } while(0);

   return rc;
}


/******************************************************************************
* Name: sdcc_partial_init
*
* Description:
*    This function partial initializes the SDCC controller.
*    It also initializes the related software data
*    structures.
*
* Arguments:
*    driveno       [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
static boolean sdcc_partial_init (uint32 driveno)
{
   sdcc_slot_type   *sdcc_pslot = NULL;
   sdcc_dev_type    *sdcc_pdev  = NULL;

   if (!SDCC_DRIVENO_IS_VALID (driveno))
   {
      return FALSE;
   }
   
   sdcc_pslot = sdcc_get_slot_handle (driveno);

   if (sdcc_pslot == NULL)
   {
      DPRINTF(("sdcc_partial_init: can't get slot handle\n"));
   	  return FALSE;
   }
   sdcc_pdev = &(sdcc_pslot->dev);

   if (sdcc_pdev == NULL)
   	  return FALSE;

   /* The controller already turned on */
   if ((sdcc_pdev->host_state  == SDCC_HOST_IDENT) ||
       (sdcc_pdev->host_state  == SDCC_HOST_TRAN))
   {
      return TRUE;
   }

   /* intialize the dev data structs */
   memset(sdcc_pdev, 0, sizeof(sdcc_pslot->dev));
   sdcc_pdev->pslot = sdcc_pslot;
   sdcc_pdev->driveno = driveno;
   sdcc_pslot->driveno      = driveno;
   sdcc_pslot->drive_in_use = TRUE;

   /* Initialize MPM timer and busywait */
   sdcc_bsp_hw_busy_wait_init();

   /* Turning on the clock to the SDCC controller */
   sdcc_bsp_config_clock(driveno, NULL, SDCC_BSP_CLK_CONFIG_HCLK);

   /* initialize the controller registers */
   HAL_sdcc_InitializeRegisters();

   /* update device info */
   sdcc_pdev->driveno        = driveno;
   sdcc_pdev->host_state     = SDCC_HOST_IDENT;
   sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;
   sdcc_pdev->pslot->dma_data.hdma = NULL;
   
   /* Configure GPIOs */
   sdcc_bsp_gpio_control(driveno, SDCC_BSP_GPIO_ON);

   return(TRUE);
} /* sdcc_partial_init */


/******************************************************************************
* Name: sdcc_handle_optimized_open
*
* Description:
*    This function does  optimized initialization of the MMC device.
*    It returns NULL if there is no memory device attached.
*    It then sets up the device for data transfer.
*    A non-null pointer to the SDCC device is returned.
*
* Arguments:
*    phy_partition_num [IN] : physical partition number.
*    device_info   [IN] : Partial initialized device information structure
*
* Returns:
*    A pointer to the SDCC device if successful.  NULL if failed.
*
******************************************************************************/
struct sdcc_device *
sdcc_handle_optimized_open (uint32 phy_partition_num)
{
   boolean ret;
   SDCC_STATUS          rc = SDCC_NO_ERROR;
   sdcc_mini_boot_device_info_type *device_info = &sdcc_mini_boot_device_info;
   uint32               driveno = device_info->driveno;
   struct sdcc_device   *ret_handle = NULL;
   boolean              still_in_use = TRUE;
   sdcc_slot_type       *sdcc_pslot = NULL;
   sdcc_dev_type        *sdcc_pdev  = NULL;

   sdcc_pslot = sdcc_get_slot_handle (driveno);
   if (NULL == sdcc_pslot)
   {
      DPRINTF(("sdcc_handle_optimized_open: can't get slot handle\n"));
      return NULL;
   }
   
   sdcc_pdev = &(sdcc_pslot->dev);

   do
   {
      if (!SDCC_DRIVENO_IS_VALID (driveno))
      {
         break;
      }
      
      ret = sdcc_partial_init (driveno);
      if ( FALSE == ret )
      {
         break;
      }

      /* register the client handle first */
      ret_handle = sdcc_register_client_handle (driveno,
                                                phy_partition_num);
      if ( ret_handle == NULL)
      {
         /* check if memory device in driveno is still in use */
         still_in_use = sdcc_is_device_still_in_use (sdcc_pdev);
         if ( !still_in_use )
         {
            /* client handle list is full */
            (void) sdcc_deinit( sdcc_pdev );
         }
         break;
      }
      
      /* return if it has been initialized */
      if (( sdcc_pdev->card_type == SDCC_CARD_SD ) ||
          ( sdcc_pdev->card_type == SDCC_CARD_SDHC ) ||
          ( sdcc_pdev->card_type == SDCC_CARD_MMC ) ||
          ( sdcc_pdev->card_type == SDCC_CARD_MMCHC ))
      {
         /* no need to select the partition region on the device as it is */
         /* already in use */
         /* will return a new handle to client */
         break;
      }

      sdcc_pdev->card_type = device_info->card_type;
      sdcc_pdev->rca = device_info->rca;
      sdcc_pdev->mem.mem_info.card_type = sdcc_pdev->card_type;

      /* return and close controller if none attached */
      if ( sdcc_pdev->card_type == SDCC_CARD_UNKNOWN )
      {
         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      rc = sdcc_partial_enumeration(sdcc_pdev);
      if (rc != SDCC_NO_ERROR)
      {
         sdcc_pdev->errno = rc;
         (void) sdcc_handle_close(ret_handle);
         ret_handle = NULL;
         break;
      }

      /* Invalidate the active partition number, this force setting of the 
       * active partition on the next access */
      sdcc_pdev->active_partition_num = SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE;
   } while(0);

   if ( ret_handle == NULL)
   {
      memset(device_info, 0, sizeof(sdcc_mini_boot_device_info_type));
   }

   return ret_handle;
}


/******************************************************************************
* Name: sdcc_is_partial_enumeration
*
* Description:
*    This function checks if partial Initialization is to be done.
*
* Arguments:
*    driveno       [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean sdcc_is_partial_enumeration ( int16 driveno )
{
   sdcc_mini_boot_device_info_type *device_info = &sdcc_mini_boot_device_info;

   if (device_info->driveno == (uint32)driveno)
   {
      /* Support this feature for MMC and external SD card */
      if ((device_info->card_type == SDCC_CARD_MMC) ||
          (device_info->card_type == SDCC_CARD_MMCHC) ||
          (device_info->card_type == SDCC_CARD_SD) ||
          (device_info->card_type == SDCC_CARD_SDHC))
      {
         return TRUE;
      }
   }

   return FALSE;
}

