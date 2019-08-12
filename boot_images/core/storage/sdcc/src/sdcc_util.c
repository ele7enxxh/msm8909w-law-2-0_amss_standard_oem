/**********************************************************************
 * sdcc_util.c
 *
 * SDCC(Secure Digital Card Controller) driver utility functions.
 *
 * This file implements the utility functions for SDCC driver.
 *
 * Copyright (c) 2007-2014
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_util.c#1 $
$DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when       who     what, where, why
---------------------------------------------------------------------- 
2014-05-16 sb      Initialize busywait
2014-03-11 bn      Moved the gcc_sdcc_apps_clk setting to sdhci_init()
2013-09-19 sb      Code clean up
2013-09-03 bn      Add SDHCi, DDR support
2013-08-12 bn      Code clean up
2013-06-13 bn      Reset CPSM after command sent according to HPG
2013-04-09 bn      Workaround for 8x10 SD Ram Dump feature
2010-06-16 vj      Fixed a bug in sdcc_config_clk
2010-06-11 vin     Added sdcc_bsp_get_max_supported_clk   
2010-05-06 rh      Disable DMA transfer when init_dma is not successful
2010-05-05 vj      Enable PROG_DONE interrupt when required
2010-04-29 rh      Make DPRINTF compatable with RETAILMSG
2010-04-22 rh      Add a delay after switching to INIT clock frequency
2010-04-20 vj      Removed turning off of HCLK/MCLK for DRIVE_1 in de-init
2010-03-30 vj      Removed featurization for sdcc_reset in sdcc_init
2010-03-11 vin     PROG_DONE enhancements
2010-03-02 yg      Added SPS support. Wrapped some previous functionality 
                   with FEATURE_SDCC_BUILD_WITH_SPS. also added som debug printouts
2010-02-19 yg      Removed #define HAL_DRIVENO. (defined once in sdcc_priv.h)
2010-02-09 vj      Featurized slot 4 usage based on SDCC_MAX_NUM_CONTROLLER
2009-11-23 rh      Modify sdcc_wait_prog_done to take sdcc_pdev as input
2009-11-18 rh      Ensure the SD code can compile with DM support
                   Restore the clock correctly after reset
2009-11-17 vj      Fixed interrupt issue caused by sdcc_reset_reinit()
2009-11-12 vj      Added API sdcc_deinit_pal
2009-11-10 rh      Adding reinitialize routine for resetting FIFO error
2009-10-22 vj      Re-ordered the reset of the controller
2009-09-22 vj      Added macro sdcc_busy_check
2009-08-07 sc      Initialized timer in sdcc_init
2009-08-04 sc      Fixed sdcc_command to not to process signals if failure occurred
2009-07-28 vj      Vote against sleep during busy check
2009-07-20 ah      removed "static" from sdcc_clk_regime_reset function decl
2009-07-15 vin     Removed a redundant check in sdcc_init
2009-06-05 vin     Exported sdcc_clk_regime_sel_clk
2009-06-05 vj      Initialized cache_align_sz
2009-06-01 sp      PAL support added
2009-05-29 vin     Removed unused SDCC_ERASE_TRANSFER_MODE
2009-05-20 vin     Set clk to 144kHz at init
2009-05-13 sc      Fixed issue taking up 25% CPU usage when Hotplug polls SDCC
2009-05-07 vin     Change data timeout for SDIO to 1 second per spec
2009-05-06 vj      Bug in synchronising of enable/disable datamover
2009-05-07 vin     Conditional WM compilation around signal and timer init
2009-04-15 vin     Support for HS SDIO
2009-03-09 vin     Bug-fix in transfer with DM disabled
2009-03-09 vj      Boot Support for the driver
2009-04-27 sc      Fixed task being held issue if DMOV failed the data transfer
2009-04-20 vj      Fixed a bug in sdcc_find_mmc_device
2009-04-15 sc      Moved sdcc_find_sd_device, sdcc_find_mmc_device and
                   sdcc_handle_find_sdio_device to sdcc_sd_util, sdcc_mmc_util
                   and sdcc_sdio_util, respectively
2009-04-14 sc      Removed feedback clock config from sdcc_handle_find_card
2009-04-14 rh      Fix a bug introduced by minimal transfer length change
2009-04-10 rcc     Specify a minimum transfer length for DM
2009-04-09 vin     Updated sdcc_handle_find_sdio_device
2009-04-07 vin     Removed Intlocks
2009-03-20 sc      Added handle support
2009-03-17 sr      Added Critical Section for 
                   VREG_GP6 dynamic voting mechanism.
2009-03-16 vj      Bump up CPU speed during data transfers using DMOV
                   for 6290/6246 targets
2009-03-11 sc      Fixed type casting issue
2009-02-23 sc      Updated sdcc_get_memory_info to check the MMC TRAN_SPEED info
2009-02-18 rh      Remove Alternate-GPIO usage
2009-02-17 vin     Added powerup & supply ramp up time for spec compliance
2009-02-09 vj      OS Abstraction Layer
2009-01-28 rh      Interface cleanup
2009-01-22 rh      Move GPIO interface control into BSP
2009-01-20 rcc     Removed featurized 6K GPIO configuration
2009-01-09 rh      Merge with WM7 changes/Move VREG control to BSP
2008-12-05 sr      HAL Adaptation for WM6.
2008-11-21 vj      Fixed High Speed Specification Violations for SD/MMC
2008-11-14 sc      Replaced reliable_write_support with
                   reliable_write_sector_count
2008-10-30 sc      Cleaned up file sdcc_api.h
2008-10-28 vin     Use one signal (FS_OP_COMPLETE_SIG) for all rex_set/rex_wait
2008-10-23 vin     Support for MMC power collapse
2008-10-29 sc      Corrected return value overwritten issue in sdcc_do_transfer
2008-10-23 vj      Added timeout to break from looping forever in read fifo
2008-10-17 vin     HAL implementation
2008-10-13 vin     Reset I/O in sdcc_find_card
2008-10-10 sc      Fixed Lint warnings
2008-09-29 rcc     Added support for VREG_GP6, which is currently used for 8K FFAs
2008-09-22 sc      Separated out read/write function for DMOV non-compliant memory
2008-08-13 rh      Add timeout when clearing status register
2008-06-23 sc      Added dmov_enable and dmov_disable
2008-06-18 rcc     Add 8K SURF-specific VREG switch
2008-06-18 rcc     Included tramp.h because clkregim is removing it from clkrgm_msm.h
2008-05-16 sc      Extracted the temp data buffer codes into a new function
2008-05-15 sc      Changed IS_ALIGNED macro to SDCC_ADDR_ALIGNED_FOR_DM
2008-05-07 vin     Added support for SD & SDIO concurrency
2008-05-01 rcc     Fixed a number of bugs with concurrent slot access
===========================================================================*/
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_sd_util.h"
#include "sdcc_mmc_util.h"
#include "sdcc_bsp.h"
#include "sdcc_handle_util.h"
#include "sdcc_hc.h"
#include "HALsdhci.h"

/* SDCC slot info structure is internal, it is not expected to be externally used */
static struct sdcc_slot_data sdcc_info[SDCC_MAX_NUM_CONTROLLER];

/*********************************************************************/
/*             PROTOTYPES FOR LOCAL FUNCTIONS                        */
/*********************************************************************/
static SDCC_STATUS sdcc_cmd_send_verify(sdcc_dev_type *);
static SDCC_STATUS sdcc_get_resp(sdcc_dev_type *, sdcc_cmd_type *);
SDCC_STATUS sdcc_poll_status(sdcc_dev_type *, sdcc_cmd_type *);
static void sdcc_decode_csd(sdcc_dev_type *, uint32 *, sdcc_csd_type *);
static void sdcc_decode_std_csd(sdcc_dev_type *, uint32 *, sdcc_csd_type *);
static void sdcc_decode_hc_csd(uint32 *, sdcc_csd_type *);
static void sdcc_get_std_memory_info(sdcc_dev_type *, uint32, uint8, uint32);
static void sdcc_get_hc_memory_info(sdcc_dev_type *, uint32, uint32);
#define SDCC_MIN(a, b) ((a)<(b)?(a):(b)) 

/******************************************************************************
* Name: sdcc_get_slot_handle
*
* Description:
*    This function return the handle to the sdcc data structure
*
* Arguments:
*    driveno          [IN] : sdcc drive number
*
* Returns:
*    None
*
******************************************************************************/
struct sdcc_slot_data *sdcc_get_slot_handle (uint32 driveno)
{
   if (!SDCC_DRIVENO_IS_VALID(driveno))
   {
      DPRINTF(("Invalid Drive Number:%d", driveno));
      return NULL;
   }
   
   return &(sdcc_info[driveno]);
}

/******************************************************************************
* Name: sdcc_config_clk
*
* Description:
*    This function is to configure the SDCC controller clock depending
*    on current mode.
*
* Arguments:
*    sdcc_pdev        [IN] : pointer to device type 
*    mode             [IN] : card mode
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_config_clk
(
   sdcc_dev_type *sdcc_pdev,
   SDCC_CARD_MODE mode
)
{
   uint32 min_clk;
   uint32 clk_khz;
   uint32 driveno = sdcc_pdev->driveno;
   SDCC_CARD_TYPE card_type = sdcc_pdev->card_type;
   uint32 bsp_max_clk = sdcc_bsp_get_max_supported_clk (driveno);

   switch (mode)
   {
   case SDCC_INIT_MODE:
      /* MCLK to be set for doing controller reset */
      /* controller reset is not happening at 144KHz hence increasing it to 20MHz */
      clk_khz = 20000;
      break;
   case SDCC_IDENTIFICATION_MODE:
      /* lower the clock to < 400KHz for card identification */
      clk_khz = 400;
      sdcc_pdev->mem.clk_freq = SDCC_MMC_DEFAULT_FREQUENCY;
      break;
   case SDCC_HS_TRANSFER_MODE:
      /* High clock 49MHz for High Speed cards */
      DPRINTF((TEXT("sdcc_config_clk: High Speed card type.\n")));
      if (card_type == SDCC_CARD_MMC || card_type == SDCC_CARD_MMCHC)
      {
          sdcc_pdev->mem.clk_freq = SDCC_MMC_HIGH_FREQUENCY_52MHZ;
      }
      else if ((card_type == SDCC_CARD_SD) || (card_type == SDCC_CARD_SDHC))
      {
          sdcc_pdev->mem.clk_freq = SDCC_SD_HIGH_FREQUENCY;
      }
      /* Fall Through */

   case SDCC_DATA_TRANSFER_MODE:
   case SDCC_READ_TRANSFER_MODE:
   case SDCC_WRITE_TRANSFER_MODE:
      if (card_type == SDCC_CARD_MMC || card_type == SDCC_CARD_MMCHC)
      {
         /* check the MMC clock freq */
         if (sdcc_pdev->mem.clk_freq == SDCC_MMC_HIGH_FREQUENCY_52MHZ)
         {
            clk_khz = 52000;
         }
         else if (sdcc_pdev->mem.clk_freq == SDCC_MMC_HIGH_FREQUENCY_26MHZ)
         {
            clk_khz = 26000;
         }
         else
         {
            /* MMC cards at Normal Speed */
            /* MMC 3.x compliant cards support only upto 20MHz */
            /* MMC 4.x compliant cards can be used at normal speed */
            /* without enabling HS_TIMING in EXT_CSD or by enumerating */
            /* as a MMC 3.x card */
            clk_khz = 20000;
         }
      }
      else if ((card_type == SDCC_CARD_SD) || (card_type == SDCC_CARD_SDHC))
      {
         /* check the SD clock freq */
         if (sdcc_pdev->mem.clk_freq == SDCC_SD_HIGH_FREQUENCY)
         {
            clk_khz = 50000;
         }
         else
         {
            clk_khz = 25000;
         }
      }
      else
      {
         clk_khz = 20000;
         DPRINTF(("sdcc_config_clk: Unknown card type.\n"));
      }
      break;

   case SDCC_DATA_TRANSFER_DDR_MODE:
      if ((SDCC_CARD_MMC == card_type) || (SDCC_CARD_MMCHC == card_type))
      {
         if (sdcc_pdev->mem.clk_freq == SDCC_MMC_HIGH_FREQUENCY_52MHZ)
         {
           /* DDR mode, double the mclk */
            clk_khz = 100000;
         }
         else
         {
            clk_khz = 20000;
         }
      }
      else
      {
         clk_khz = 20000;
         DPRINTF(("sdcc_config_clk: DDR is only supported on MMC card\n"));
      }
      break;

   default:
      clk_khz = 20000;
      DPRINTF(("sdcc_config_clk: mode unknown.\n"));
      break;
   }

   if (TRUE == sdcc_pdev->sdhci_mode) 
   {
      /* In SDHCi mode, the gcc_sdcc_apps_clk should be set to max supported freq */
      /* for best performance. The SCLK frequency going to the card is controlled */
      /* via the HC's register */
      min_clk = SDCC_MIN(clk_khz, bsp_max_clk);
      (void) sdcc_hc_set_clock(driveno, min_clk*1000);
      sdcc_pdev->clk_freq_in_khz = min_clk;
   }
   else
   {
      min_clk = SDCC_MIN(clk_khz, bsp_max_clk);
      sdcc_bsp_config_clock (driveno, &min_clk, SDCC_BSP_CLK_CONFIG_MCLK);
      sdcc_pdev->clk_freq_in_khz = min_clk;
   } 
} /* sdcc_config_clk */

/******************************************************************************
* Name: sdcc_cmd_send_verify
*
* Description:
*    This function is to make sure the command is sent out and then
*    clear the corresponding bit in the status.
*
* Arguments:
*    sdcc_pdev         [IN] : pointer to device type 
*
* Returns:
*    Returns error code
*
* Note:
*    For slow clock(400KHz), this is needed.
*
******************************************************************************/
static SDCC_STATUS
sdcc_cmd_send_verify( sdcc_dev_type *sdcc_pdev )
{
   uint32 t = 0;
   uint32 driveno = sdcc_pdev->driveno;

   while (t++ < 800)
   {
      if (HAL_sdcc_StatusCheckFlag (driveno,
                                    HAL_sdcc_StatusNow (driveno),
                                    HAL_SDCC_STATUS_CMD_SENT))
         break;
      sdcc_udelay (10);
   }

   if (t < 800)
   {
      HAL_sdcc_StatusClear (driveno, HAL_SDCC_STATUS_CMD_SENT);
      while (t++ < 800)
      {
         /* make sure cmd_sent is cleared */
         if (! HAL_sdcc_StatusCheckFlag (driveno,
                                         HAL_sdcc_StatusNow (driveno),
                                         HAL_SDCC_STATUS_CMD_SENT))
            return SDCC_NO_ERROR;
         sdcc_udelay (10);
      }
   }
   return SDCC_ERR_CMD_SENT;
} /* sdcc_cmd_send_verify */

/******************************************************************************
* Name: sdcc_get_resp
*
* Description:
*    This function reads out the command response.
*
* Arguments:
*    sdcc_pdev         [IN] : pointer to device type 
*    sdcc_cmd          [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
static SDCC_STATUS
sdcc_get_resp( sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd )
{
   uint32        count  = 0;
   uint32        i;
   uint32       *resp = NULL;
   SDCC_STATUS   rc   = SDCC_NO_ERROR;

   /* loading command responses */
   if (sdcc_cmd->resp_type)
   {
      count = ( SDCC_RESP_LONG == sdcc_cmd->resp_type ) ? 4 : 1;
      resp  = sdcc_cmd->resp;

      for (i = 0; i < count ; i++ )
      {
         HAL_sdcc_GetResponse (sdcc_pdev->driveno, resp, i);
         resp++;
      }

      /* check the R5 response flags */
      if (( SD_CMD52_IO_RW_DIRECT   == sdcc_cmd->cmd ) ||
          ( SD_CMD53_IO_RW_EXTENDED == sdcc_cmd->cmd ))
      {
         if (sdcc_cmd->resp[0] & 0xCF00)
         {
            rc = SDCC_ERR_SDIO_R5_RESP;
            DPRINTF((TEXT("R5 flag(0x%x) error.\n"), sdcc_cmd->resp[0]));
         }
      }
   }

   return rc;
} /* sdcc_get_resp */

/******************************************************************************
* Name: sdcc_poll_status
*
* Description:
*    This function is to poll the SDCC status.
*
* Arguments:
*    sdcc_pdev         [IN] : pointer to device type 
*    sdcc_cmd          [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_poll_status( sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd )
{
   uint32 i = 0;
   SDCC_STATUS rc = SDCC_ERR_UNKNOWN;
   uint32 driveno = sdcc_pdev->driveno;
   uint32 status = 0;

   /* polling for status */
   while (i++ < SDCC_STATUS_POLL_MAX)
   {
      /* Get current status */
      status = HAL_sdcc_StatusNow (driveno);
      if (HAL_sdcc_StatusCheckFlag (driveno, status, HAL_SDCC_STATUS_CMD_RESPONSE_END))
      {
         HAL_sdcc_StatusClear (driveno, HAL_SDCC_STATUS_CMD_RESPONSE_END);
         rc = SDCC_NO_ERROR;
         //if prog done is already asserted, mark it
         if (HAL_sdcc_StatusCheckFlag (driveno, status, HAL_SDCC_STATUS_PROG_DONE))
         {
            HAL_sdcc_StatusClear (driveno, HAL_SDCC_STATUS_PROG_DONE);
            sdcc_cmd->prog_scan = FALSE;
         }
         break;
      }
      /* timed out on response */
      if (HAL_sdcc_StatusCheckFlag (driveno, status, HAL_SDCC_STATUS_CMD_TIMEOUT))
      {
         HAL_sdcc_StatusClear (driveno, HAL_SDCC_STATUS_CMD_TIMEOUT);
         rc = SDCC_ERR_CMD_TIMEOUT;
         break;
      }
      /* check CRC error */
      if (HAL_sdcc_StatusCheckFlag (driveno, status, HAL_SDCC_STATUS_CMD_CRC_FAIL))
      {
         /* the following cmd response doesn't have CRC. */
         if (( SD_ACMD41_SD_APP_OP_COND == sdcc_cmd->cmd ) ||
             ( SD_CMD1_SEND_OP_COND     == sdcc_cmd->cmd ) ||
             ( SD_CMD5_IO_SEND_OP_COND  == sdcc_cmd->cmd ))
         {
            rc = SDCC_NO_ERROR;
         }
         else
         {
            rc = SDCC_ERR_CMD_CRC_FAIL;
         }
         HAL_sdcc_StatusClear (driveno, HAL_SDCC_STATUS_CMD_CRC_FAIL);
         break;
      }
   }
   sdcc_cmd->status = status;

   if (i == (SDCC_STATUS_POLL_MAX + 1))
   {
      DPRINTF((TEXT("polling status timed out, driveno = %d\n"), driveno));
   }
   return rc;
} /* sdcc_poll_status */

/******************************************************************************
* Name: sdcc_command
*
* Description:
*    This is a wrapper function for sending the command and poll 
*    the status. It decides whether to use the Qualcomm or standard 
*    Host Controller interface to communicate with the memory device.
*
* Arguments:
*    sdcc_pdev         [IN] : pointer to device type 
*    sdcc_cmd          [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_command(sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd)
{
   if ((NULL == sdcc_pdev) || (NULL == sdcc_cmd))
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   if (TRUE == sdcc_pdev->sdhci_mode)
   {
      return sdcc_command_sdhci_mode(sdcc_pdev, sdcc_cmd);
   }
   else
   {
      return sdcc_command_qc_mode(sdcc_pdev, sdcc_cmd);
   }
} /* sdcc_command */

/******************************************************************************
* Name: sdcc_command_qc_mode
*
* Description:
*    This function is to send the command and poll the status using the
*    Qualcomm interface to communicate with the memory devices
*
* Arguments:
*    sdcc_pdev  [IN] : pointer to device structure
*    sdcc_cmd   [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_command_qc_mode(sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   struct HAL_sdcc_SdCmd HAL_cmd;

   sdcc_send_cmd(sdcc_pdev, sdcc_cmd);

   /* CMD0 takes 74 clock clcles, need to verify that we have
      finished sending command */
   if ( SD_CMD0_GO_IDLE_STATE == sdcc_cmd->cmd )
   {
      rc = sdcc_cmd_send_verify(sdcc_pdev);
      if (rc != SDCC_NO_ERROR)
      {
         DPRINTF((TEXT("sdcc_command() : sdcc_cmd_send_verify() returned with ERROR")));
      }
      return rc;
   }

   /* if response specified, poll status */
   if (sdcc_cmd->resp_type)
   {
      rc = sdcc_poll_status(sdcc_pdev, sdcc_cmd);
      if (rc != SDCC_NO_ERROR)
      {
         return rc;
      }
      rc = sdcc_get_resp(sdcc_pdev, sdcc_cmd);
      if (rc != SDCC_NO_ERROR)
      { 
         DPRINTF((TEXT("sdcc_command() : sdcc_get_resp() returned with ERROR")));
      }
   }

   /* wait for finishing program the device */
   if (sdcc_cmd->prog_scan && rc == SDCC_NO_ERROR)
   {
      rc = sdcc_busy_check(sdcc_pdev);
      if (rc != SDCC_NO_ERROR)
      {
         DPRINTF((TEXT("sdcc_command(), sdcc_busy_check() returned with ERROR")));
      }
   }

   /* Reset CPSM */
   memset ((void *) &HAL_cmd, 0, sizeof (HAL_cmd));
   HAL_sdcc_SetSdCmd (sdcc_pdev->driveno, &HAL_cmd);
   
   return rc;
} /* sdcc_command_qc_mode */

/******************************************************************************
* Name: sdcc_send_cmd
*
* Description:
*    This function is to send out the command.
*
* Arguments:
*    sdcc_pdev         [IN] : pointer to device type 
*    sdcc_cmd          [IN] : pointer to command data structure
*
* Returns:
*    None
*
******************************************************************************/
void
sdcc_send_cmd (sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd)
{
   uint32 driveno = sdcc_pdev->driveno;
   struct HAL_sdcc_SdCmd HAL_cmd;
   uint32 status = TRUE;
   uint32   timeout = 0;

   memset ((void *) &HAL_cmd, 0, sizeof (HAL_cmd));

   /* clear any status flags
      Note: For slower clock, need to wait to make sure the flags
            are cleared. */
   do
   {
      if (timeout >= SDCC_CLR_STATUS_RETRIES)
      {
         DPRINTF((TEXT("Timeout while clearing status register.")));
         /* Card can be removed, causing status bit to not resetting - 
          * Stop the data read process in DPSM */
         HAL_sdcc_DataControlStopDPSM (driveno);
         break;
      }
      HAL_sdcc_StatusClearStaticFlags (driveno);
      timeout++;

      status = HAL_sdcc_StatusCheckFlag (driveno,
                                         HAL_sdcc_StatusNow (driveno),
                                         HAL_SDCC_STATUS_ANY_STATIC);
   }
   while (status);

   /* set the command */
   HAL_cmd.cmd_index = (uint16) sdcc_cmd->cmd;
   HAL_cmd.enable = TRUE;
   if (sdcc_cmd->resp_type)
   {
      HAL_cmd.response = TRUE;
      if (SDCC_RESP_LONG == sdcc_cmd->resp_type)
      {
         HAL_cmd.longrsp = TRUE;
      }
   }
   if (sdcc_cmd->prog_scan)
   {
      HAL_cmd.prog_ena = TRUE;
   }

   if ((SD_CMD53_IO_RW_EXTENDED       == sdcc_cmd->cmd) ||
       (SD_CMD17_READ_BLOCK           == sdcc_cmd->cmd) ||
       (SD_CMD18_READ_MULTIPLE_BLOCK  == sdcc_cmd->cmd) ||
       (SD_CMD24_WRITE_BLOCK          == sdcc_cmd->cmd) ||
       (SD_CMD25_WRITE_MULTIPLE_BLOCK == sdcc_cmd->cmd))
   {
      HAL_cmd.dat_cmd = TRUE;
   }
   HAL_cmd.cmd_arg = sdcc_cmd->cmd_arg;
   HAL_sdcc_SetSdCmd (driveno, &HAL_cmd);
} /* sdcc_send_cmd */

/******************************************************************************
* Name: sdcc_send_status
*
* Description:
*    This function is to let the currently selected device send
*    the device status.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns the current device status
*
******************************************************************************/
SDCC_CARD_STATUS
sdcc_send_status( sdcc_dev_type *sdcc_pdev )
{
   sdcc_cmd_type        sdcc_cmd;
   SDCC_CARD_STATUS     card_status = SDCC_CARD_IGNORE;
   uint32               rc          = SDCC_ERR_UNKNOWN;

   /*-----------------------------------------------------------------------*/

   sdcc_cmd.cmd       = SD_CMD13_SEND_STATUS;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg   = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (SDCC_NO_ERROR == rc)
   {
      card_status = (SDCC_CARD_STATUS)(
                     ( sdcc_cmd.resp[0] >> SDCC_CARD_STATUS_SHFT )
                     & (SDCC_CARD_STATUS_BMSK));
   }

   return card_status;
} /* sdcc_send_status */

/******************************************************************************
* Name: sdcc_wait_prog_done
*
* Description:
*    This function is to poll the status to make sure the device is not
*    busy.  CMD12(stop) and CMD38(erase) need to call this to make sure
*    the device programming has finished.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*    
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_wait_prog_done( sdcc_dev_type *sdcc_pdev )
{
   uint32            t = 0;
   SDCC_STATUS       rc = SDCC_ERR_PROG_DONE;
   SDCC_CARD_STATUS  card_status = SDCC_CARD_IGNORE;
   uint32            driveno = sdcc_pdev->driveno;

   while (t++ < SDCC_PROG_DONE_MAX)
   {
      if (HAL_sdcc_StatusCheckFlag (driveno,
                                    HAL_sdcc_StatusNow (driveno),
                                    HAL_SDCC_STATUS_PROG_DONE))
      {
         HAL_sdcc_StatusClear (driveno, HAL_SDCC_STATUS_PROG_DONE);
         rc = SDCC_NO_ERROR;
         break;
      }
      sdcc_udelay (10);
   }

   /* CMD13: if no prog_done indication, give it one more
      chance by polling the card directly */
   if (SDCC_NO_ERROR != rc)
   {
      DPRINTF((TEXT("timeout waiting for prog done, driveno = %d\n"), driveno));

      card_status = sdcc_send_status(sdcc_pdev);
      DPRINTF((TEXT("card status = 0x%x.\n"), card_status));

      if ( SDCC_CARD_TRAN == card_status )
      {
         rc = SDCC_NO_ERROR;
      }
   }

   return rc;
} /* sdcc_wait_prog_done */

/******************************************************************************
* Name: sdcc_init_qc_mode
*
* Description:
*    This function initializes the SDCC controller to using Qualcomm interface 
*    for communication with the memory device. It first turns on
*    the clock, configures the GPIOs and then turns on the power supply
*    to the device.  It also initializes the related software data
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
boolean sdcc_init_qc_mode (uint32 driveno)
{
   sdcc_slot_type   *sdcc_pdata = NULL;
   sdcc_dev_type    *sdcc_pdev  = NULL;

   /* SDCC HAL version string Provided by HAL init */
   char *sdcc_hal_ver;

   sdcc_pdata = sdcc_get_slot_handle (driveno);

   if (sdcc_pdata == NULL)
   {
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

   sdcc_pdev->pslot->dma_data.hdma = NULL;

   if (FALSE == sdcc_pdata->drive_in_use)
   {
      /* intialize the dev data structs */
      memset(&sdcc_pdata->dev, 0, sizeof(sdcc_pdata->dev));
      sdcc_pdev->pslot = sdcc_pdata;
      sdcc_pdev->driveno = driveno;
      sdcc_pdata->driveno = driveno;
      sdcc_pdata->drive_in_use = TRUE;
      sdcc_pdev->sdhci_mode = FALSE;
   }

   /* Configure GPIOs */
   sdcc_bsp_gpio_control(driveno, SDCC_BSP_GPIO_ON);
   /* Initialize MPM timer and busywait */
   sdcc_bsp_hw_busy_wait_init();
   /* Turn on the SDCC clock */
   sdcc_config_clk(sdcc_pdev, SDCC_INIT_MODE);
   /* Switching clock takes a while for the clock source to settle */
   sdcc_osal_sleep(SDCC_CLK_RAMP_TIME_MS);
   /* initialize the controller */
   HAL_sdcc_InitializeRegisters();
   /* Disable SDHCi mode */
   HAL_sdcc_HCModeEnable (driveno, HAL_SDHCI_QCT_MODE);

   HAL_sdcc_Init (driveno, &sdcc_hal_ver);

   /* update device info */
   sdcc_pdev->driveno        = driveno;
   sdcc_pdev->host_state     = SDCC_HOST_IDENT;
   sdcc_pdev->cache_align_sz = sdcc_bsp_get_cache_align_size();
   sdcc_pdev->mem.block_mode = SDCC_MEM_BLK_MODE;

   /* reset the controller
    * reset is to be done after enabling HCLK, MCLK
    * and Power On to the controller only
    * Note: Reset was failing at 144KHz MCLK (requires 1ms delay
    *       in clk_regime_msm_reset between reset bit set and clear)
    *       and hence to avoid dependency with clkrgm we are going 
    *       with setting mclk to 19.2MHz as reset was successful.
    */
   sdcc_bsp_config_clock(driveno, NULL, SDCC_BSP_CLK_CONFIG_RESET);
   return(TRUE);
} /* sdcc_init_qc_mode */

/******************************************************************************
 * Name: sdcc_deinit
 *
 * Description:
 *    This function de-selects the device, turns off the power supply to the
 *    device and switches off the controller clock.
 *
 * Arguments:
 *    driveno     [IN] drive number
 *
 * Returns:
 *    SDCC_NO_ERROR if successful.  Error code otherwise.
 *
 ******************************************************************************/
SDCC_STATUS
sdcc_deinit( sdcc_dev_type  *sdcc_pdev )
{
   sdcc_slot_type   *sdcc_pdata = NULL;
   uint32 driveno;

   sdcc_pdata = sdcc_pdev->pslot;
   driveno = sdcc_pdata->driveno;

   do
   {
      if (sdcc_pdev->host_state == SDCC_HOST_IDLE)
      {
         break;
      }
  
      if (sdcc_pdata->drive_in_use)
      {    
         /* set the power mode to 'power OFF' */
         HAL_sdcc_SetPowerState (driveno, HAL_SDCC_POWER_OFF);

         /* close the GPIO interfaces and turn off the clocks */
         sdcc_bsp_gpio_control(driveno, SDCC_BSP_GPIO_OFF);
		 
         /* meaning the controller is not initialized any more*/
         sdcc_pdata->drive_in_use = FALSE;
      }

      sdcc_pdev->card_type = SDCC_CARD_UNKNOWN;
      sdcc_pdev->host_state = SDCC_HOST_IDLE;
      sdcc_pdev->clk_freq_in_khz = 0;
   }
   while(0); 

   return SDCC_NO_ERROR;
} /* sdcc_deinit */

/******************************************************************************
* Name: sdcc_reset_reinit
*
* Description:
*    This function resets and reinitializes the SDCC controller after
*    the controller has encountered an transfer error causing the FIFO
*    to be corrupted.
*    Note: This function should only be called when the card is in 
*    transfer state.
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
sdcc_reset_reinit( sdcc_slot_type  *pslot )
{
   sdcc_dev_type  *sdcc_pdev = &(pslot->dev);
   uint32 driveno = pslot->driveno;
   uint32 storage[SDCC_SAVE_RESTORE_ARRAY_SIZE];

   /* The controller already turned on - switch to init mode clock speed */
   sdcc_config_clk(sdcc_pdev, SDCC_INIT_MODE);

   /* Save the state of the controller */
   HAL_sdcc_Save(driveno, storage);
   
   /* Resetting the controller */
   sdcc_bsp_config_clock(driveno, NULL, SDCC_BSP_CLK_CONFIG_RESET);

   /* Restore the state of the controller */
   HAL_sdcc_Restore(driveno, storage);

   /* Restore the clock configuration */
   if(sdcc_pdev->host_state == SDCC_HOST_TRAN)
   {
      sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_MODE);
   }
   else
   {
      sdcc_config_clk(sdcc_pdev, SDCC_IDENTIFICATION_MODE);
   }

   return(TRUE);
} /* sdcc_reset_reinit */


/******************************************************************************
* Name: sdcc_decode_csd
*
* Description:
*    This function is to parse out the CSD data.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device type
*    data                   [IN] : pointer to data read in from response
*                                  registers
*    csd                    [IN] : pointer to the stucture to save the
*                                  csd data to
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_decode_csd
(
   sdcc_dev_type  *sdcc_pdev,
   uint32         *data,
   sdcc_csd_type  *csd
)
{
   uint32   value;

   /*----------------------------------------------------------------------*/

   if (NULL == csd)
   {
      return;
   }

   /* In resp[0] */
   value = *data;
   csd->csd_structure = (uint8)(value >> 30);

   switch (sdcc_pdev->card_type)
   {
      case SDCC_CARD_SD:
      case SDCC_CARD_SDHC:
      {
         if (csd->csd_structure == 1)
         {
            /* CSD Version 2.0: CSD_STRUCTURE = 1 */
            sdcc_decode_hc_csd(data, csd);
         }
         else
         {
            /* CSD Version 1.0: CSD_STRUCTURE = 0 */
            sdcc_decode_std_csd(sdcc_pdev, data, csd);
         }
      }
      break;

      case SDCC_CARD_MMC:
      case SDCC_CARD_MMCHC:
      default:
      {
         sdcc_decode_std_csd(sdcc_pdev, data, csd);
         /* System Specification version is for MMC only. */
         sdcc_pdev->mem.spec_vers = csd->spec_vers;
         /* check if write protection is supported */
         if (csd->wp_grp_enable && csd->spec_vers >= SDCC_MMC_SPEC_VERS_3X)
         {
            sdcc_pdev->mem.write_prot_support = TRUE;

            /* calculate the default write protect group size from CSD */
            /* refer to section 8.3 in JEDEC Standard No. 84-A44 */
            sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors =
               512 * (csd->wp_grp_size + 1) *
               (csd->erase.v31.erase_grp_size + 1) *
               (csd->erase.v31.erase_grp_mult + 1) /
               SDCC_DEFAULT_BLK_LENGTH_SIZE;
         }
         else
         {
            sdcc_pdev->mem.write_prot_support = FALSE;
         }
      }
      break;
   }
} /* sdcc_decode_csd */

/******************************************************************************
* Name: sdcc_decode_std_csd
*
* Description:
*    This function is to read the passed in data of the Standard Capacity
*    SD/MMC memory card and assign the proper bit values to the fields in
*    csd structure.
*
* Arguments:
*    sdcc_pdev     [IN] : pointer to device type
*    data          [IN] : pointer to data read in from response registers
*    csd          [OUT] : pointer to the stucture to save the csd data to
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_decode_std_csd
(
   sdcc_dev_type  *sdcc_pdev,
   uint32         *data,
   sdcc_csd_type  *csd
)
{
   uint32    value;
   uint32    tmp;

   if (csd == NULL)
   {
      return;
   }

   /* In resp[0] */
   value                   = *data;

   if (sdcc_pdev->card_type == SDCC_CARD_MMC ||
       sdcc_pdev->card_type == SDCC_CARD_MMCHC)
   {
      /* For MMC only: spec_vers: system specification version --> csd[125:122] */
      csd->spec_vers          = (uint8)((value & 0x3C000000) >> 26);
   }
   /* taac: data read access-time-1 --> csd[119:112] */
   csd->taac               = (uint8)((value & 0x00FF0000) >> 16);
   /* nsac: data read access-time-2 in CLK cycles --> csd[111:104] */
   csd->nsac               = (uint8)((value & 0x0000FF00) >> 8);
   /* tran_speed: max. data transfer rate --> csd[103:96] */
   csd->tran_speed         = (uint8)(value & 0x000000FF);

   /* read_bl_len: max. read data block length --> csd[83:80] */
   value = *(++data);
   csd->read_bl_len = (uint8)((value & 0x000F0000) >> 16);

   /* c_size: device size --> csd[73:62] */
   tmp         = value & 0x000003FF;
   value       = *(++data);
   csd->c_size = (uint32)((tmp << 2) | ((value & 0xC0000000) >> 30));

   /* c_size_mult: device size multiplier --> csd[49:47] */
   csd->c_size_mult = (uint16)((value & 0x00038000) >> 15);

   if ((sdcc_pdev->card_type == SDCC_CARD_MMC ||
       sdcc_pdev->card_type == SDCC_CARD_MMCHC) &&
       csd->spec_vers >= SDCC_MMC_SPEC_VERS_3X)
   {
      /* erase_grp_size: erase group size --> csd[46:42] */
      csd->erase.v31.erase_grp_size = (uint8)((value & 0x00007C00) >> 10);

      /* erase_grp_mult: erase group size multiplier --> csd[41:37] */
      csd->erase.v31.erase_grp_mult = (uint8)((value & 0x000003E0) >> 5);

      /* wp_grp_size: write protect group size --> csd[36:32] */
      csd->wp_grp_size = (uint8)(value & 0x0000001F);
   }

   /* wp_grp_enable: write protect group enable --> csd[31:31] */
   value = *(++data);
   csd->wp_grp_enable = (uint8)((value & 0x80000000) >> 31);
} /* sdcc_decode_std_csd */

/******************************************************************************
* Name: sdcc_decode_hc_csd
*
* Description:
*    This function is to read the passed in data of the High Capacity
*    SD memory card and assign the proper bit values to the fields in
*    csd structure.
*
* Arguments:
*    data          [IN] : pointer to data read in from response registers
*    csd          [OUT] : pointer to the stucture to save the csd data to
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_decode_hc_csd
(
   uint32         *data,
   sdcc_csd_type  *csd
)
{
   uint32    value;
   uint32    tmp;

   /*----------------------------------------------------------------------*/

   if (NULL == csd)
   {
      return;
   }

   /* In resp[0] */
   value                   = *data;
   /* taac: data read access-time is fixed to 0Eh (means 1ms) --> csd[119:112]*/
   csd->taac               = SDCC_CSD_TAAC;
   /* nsac: data read access-time in CLK cycles is fixed to 00h --> csd[111:104]*/
   csd->nsac               = SDCC_CSD_NSAC;
   /* tran_speed: max. data transfer rate --> csd[103:96] */
   csd->tran_speed         = (uint8)(value & 0x000000FF);

   /* read_bl_len: max. read data block length is fixed to */
   /* 9h (means 512 Bytes) --> csd[83:80] */
   value                   = *(++data);
   csd->read_bl_len        = SDCC_CSD_READ_BL_LEN;

   /* c_size: device size --> csd[69:48] */
   /* This field is expanded to 22 bits and can indicate up to 2TBytes. */
   tmp         = value & 0x0000003F;
   value       = *(++data);
   csd->c_size = (uint32)((tmp << 16) | ((value & 0xFFFF0000) >> 16));
} /* sdcc_decode_hc_csd */

/******************************************************************************
* Name: sdcc_get_std_memory_info
*
* Description:
*    This function is to calculate the block length and memory capacity
*    of the Standard Capacity SD/MMC memory card.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device type
*    block_len              [IN] : data block length
*    c_size_mult            [IN] : device size multiplier
*    c_size                 [IN] : device size
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_get_std_memory_info
(
   sdcc_dev_type   *sdcc_pdev,
   uint32           block_len,
   uint8            c_size_mult,
   uint32           c_size
)
{
   uint32 mult = 0;
   uint32 card_size_in_sectors = 0;

   /*----------------------------------------------------------------------*/

   mult = 1UL << (c_size_mult + 2);
   card_size_in_sectors = mult * (c_size + 1);

   sdcc_pdev->mem.mem_info.block_len = 1UL << block_len; /* in Bytes */

   /* Since the block length can be in diffrent size
   * 512, 1024, 2048 etc.  Calculate the card size in the absolute
   * block length of multiple 512 (i.e., SDCC_DEFAULT_BLK_LENGTH_SIZE)
   */
   sdcc_pdev->mem.mem_info.card_size_in_sectors = card_size_in_sectors *
      (sdcc_pdev->mem.mem_info.block_len/SDCC_DEFAULT_BLK_LENGTH_SIZE);
} /* sdcc_get_std_memory_info */

/******************************************************************************
* Name: sdcc_get_hc_memory_info
*
* Description:
*    This function is to calculate the block length and memory capacity
*    of the High Capacity SD/MMC memory card.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device type
*    block_len              [IN] : data block length
*    c_size                 [IN] : device size
*
* Returns:
*    None
*
******************************************************************************/
static void
sdcc_get_hc_memory_info
(
   sdcc_dev_type   *sdcc_pdev,
   uint32           block_len,
   uint32           c_size
)
{
   sdcc_pdev->mem.mem_info.block_len = 1UL << block_len; /* in Bytes */

   /* The user data area capacity is calculated from the c_size as follows: */
   /*    memory capacity = (c_size+1)*512K byte    */
   /* where 512 is the block len of the High Capacity SD memory card and */
   /*       1K byte = 1024 bytes */
   /* Calculate the card size in the absolute block length of muliple */
   /* 512 (i.e., SDCC_DEFAULT_BLK_LENGTH_SIZE) */
   sdcc_pdev->mem.mem_info.card_size_in_sectors = (c_size + 1) *
      ((sdcc_pdev->mem.mem_info.block_len*1024) / SDCC_DEFAULT_BLK_LENGTH_SIZE);
} /* sdcc_get_hc_memory_info */


/******************************************************************************
* Name: sdcc_get_memory_info
*
* Description:
*    This function is to get the specific information related to the
*    memory device via CMD9 and/or CMD10 before it goes into data
*    transfer mode.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in stand-by mode.
*
******************************************************************************/
SDCC_STATUS
sdcc_get_memory_info
(
   sdcc_dev_type *sdcc_pdev
)
{
   SDCC_STATUS      rc = SDCC_NO_ERROR;
   sdcc_cmd_type    sdcc_cmd;
   sdcc_csd_type    csd;
   SDCC_CARD_STATUS card_status = SDCC_CARD_IGNORE;

   /*------------------------------------------------------------------------*/

   /* CMD13: make sure in STBY state */
   card_status = sdcc_send_status(sdcc_pdev);
   if ( SDCC_CARD_STBY != card_status )
   {
      DPRINTF((TEXT("In invalid data TX state 0x%x\n"),card_status));
      rc = SDCC_ERR_INVALID_TX_STATE;

      return rc;
   }

   /* CMD9: get card specific data */
   sdcc_cmd.cmd       = SD_CMD9_SEND_CSD;
   sdcc_cmd.resp_type = SDCC_RESP_LONG;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg   = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);

   if (SDCC_NO_ERROR == rc)
   {
      memset(&csd, 0, sizeof(csd));
      sdcc_decode_csd(sdcc_pdev, sdcc_cmd.resp, &csd);

      switch (sdcc_pdev->card_type)
      {
         case SDCC_CARD_SD:
         case SDCC_CARD_SDHC:
         {
            if (csd.csd_structure == 1)
            {
               /* csd_structure = 1 means CSD Version 2.0 for */
               /* High Capacity SD memory card */
               sdcc_get_hc_memory_info(sdcc_pdev, csd.read_bl_len, csd.c_size);
            }
            else
            {
               /* csd_structure == 0 means CSD Version 1.0 for */
               /* Standard Capacity SD memory card */
               sdcc_get_std_memory_info(sdcc_pdev, csd.read_bl_len,
                                        csd.c_size_mult, csd.c_size);
            }

            /* set the mandatory maximum operating frequency */
            /* refer to section 5.3.2 in SD 2.0 Spec. */
            sdcc_pdev->mem.clk_freq = SDCC_SD_DEFAULT_FREQUENCY;
         }
         break;

         case SDCC_CARD_MMCHC:
         {
            /* defer to sdcc_config_mmc_modes_segment as EXT_CSD register */
            /* is read there.  SEC_COUNT must be used to calculate the high */
            /* density card size. */

            /* set the default clock frequency per TRAN_SPEED field in CSD */
            /* register */
            /* refer to section 8.3 in JEDEC Standard No. 84-A43 */
            if ( SDCC_MMC_TRAN_SPEED_26MHZ == csd.tran_speed )
            {
               sdcc_pdev->mem.clk_freq = SDCC_MMC_HIGH_FREQUENCY_26MHZ;
            }
            else
            {
               sdcc_pdev->mem.clk_freq = SDCC_MMC_DEFAULT_FREQUENCY;
            }
         }
         break;

         case SDCC_CARD_MMC:
         default:
         {
            /* get the standard memory info anyway */
            sdcc_get_std_memory_info(sdcc_pdev, csd.read_bl_len,
                                     csd.c_size_mult, csd.c_size);

            /* set the default clock frequency per TRAN_SPEED field in CSD */
            /* register */
            /* refer to section 8.3 in JEDEC Standard No. 84-A43 */
            if ( SDCC_MMC_TRAN_SPEED_26MHZ == csd.tran_speed )
            {
               sdcc_pdev->mem.clk_freq = SDCC_MMC_HIGH_FREQUENCY_26MHZ;
            }
            else
            {
               sdcc_pdev->mem.clk_freq = SDCC_MMC_DEFAULT_FREQUENCY;
            }
         }
         break;
      }
   }

   return rc;
} /* sdcc_get_memory_info */

/******************************************************************************
*
* Description:
*    This function is to select/deselect the specified card.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*    select             [IN] : select if TRUE, deselect if FALSE
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_select_card
(
   sdcc_dev_type  *sdcc_pdev,
   boolean         select
)
{
   sdcc_cmd_type    sdcc_cmd;
   SDCC_STATUS      rc = SDCC_ERR_UNKNOWN;

   /* CMD7: select the card */
   sdcc_cmd.cmd_arg   = (select) ? SDCC_ARG_RCA(sdcc_pdev->rca) : 0;
   sdcc_cmd.cmd       = SD_CMD7_SELECT_CARD;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command (sdcc_pdev, &sdcc_cmd);
   if (select == FALSE)
   {
      /* Ignore the error as it is expected behavior of the card to return 
       * CMD_TMOUT when CMD7 deselect is sent with 0x0 argument */
      rc = SDCC_NO_ERROR;
   }

   return rc;
} /* sdcc_select_card */


/*lint -restore */

