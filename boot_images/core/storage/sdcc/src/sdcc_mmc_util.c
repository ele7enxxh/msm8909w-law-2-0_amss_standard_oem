/**********************************************************************
 * File: sdcc_mmc_util.c
 *
 * Services: 
 *    This file provides SDCC services for MMC device.
 *
 * Description:
 *    This file contains the MMC specific utility function.
 *
 * Copyright (c) 2007-2014
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_mmc_util.c#1 $
$DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when       who     what, where, why
---------------------------------------------------------------------- 
----------------------------------------------------------------------
2014-12-15 sb      Enable ADMA support for device programmer
2014-01-13 bn      Fixed CMD6 PowerOn Write Protection Failure.
2013-09-25 bn      Corrected the data transfer DDR mode frequency
2013-09-04 pa      Fixes for enhanced user area configuration  
2013-09-03 bn      Add SDHCi, DDR Support
2013-08-12 bn      Code clean up. Added flags to cmd structure
2013-07-18 rh      Removed code for non-boot build (not used anymore)
2012-01-24 rh      Use a bit field to determine if the slot is 8bit
2011-09-19 rh      Record the partition setting complete value
2011-09-09 rh      Correctly report physical partition layout
2011-08-30 rh      Added GPP partitioning support for eMMC
2011-01-06 rh      Fix some mismerged change
2011-01-03 rh      Correct frequency switch for eMMC card supporting DDR
2010-08-03 rh      Use WP_GRP size from CSD if ERASE_GROUP_DEF switch failed
2010-02-25 rh      Eliminate timeout occurred during 8-bit detection
2009-11-12 vj      Eliminate multiple enumerations of the eMMC card
2009-11-10 rh      Resetting the FIFO when 8-bit bus detection failed
2009-10-30 sc      Added feature to set active boot partition
2009-10-23 sc      Renamed boot_partition_access to partition_access
2009-10-16 sc      Added power-on write protection feature
2009-10-12 sc      Added write protection support
2009-10-07 rh      When detecting 8bit MMC, disable power save mode
2009-09-30 rh      Remove code that detect card type base on card size
2009-09-21 vin     Updated to use HAL_SDCC_STATUS_TXACTIVE
2009-09-16 vin     Support for 8bit MMC 
2009-08-20 sc      Fixed the logic for sending CMD6 SWITCH
2009-08-17 sc      Replaced ext csd data buffer with ext_csd_raw_data buffer
2009-08-13 sc      Added temp workaround for eMMC v4.3/4.3+ device type
2009-08-06 sc      Fixed the decode for mmc_device_type
2009-06-13 rh      Add wait after switching partition
2009-05-20 sc      Consolidated physical partition info support to get device info
2009-05-13 sc      Fixed issue taking up 25% CPU usage when Hotplug polls SDCC
2009-04-27 sc      Added support to get physical partition info
2009-04-15 sc      Added sdcc_find_mmc_device
2009-03-20 sc      Added handle support
2009-03-11 sc      Fixed type casting issue
2009-02-23 sc      Fixed the card type checking with the 'else' in
                   sdcc_config_mmc_modes_segment
2009-02-12 vj      OS Abstraction Layer
2008-11-21 vj      Fixed High Speed Specification Violations for SD/MMC
2008-11-14 sc      Updated reliable_write_sector_count
2008-09-22 sc      Added reliable write support and decode function for CID reg
02/20/08   umr     Add HAL calls to set bit width mode.
12/14/07   sc      Handled MMC card of card size <= 2GB with invalid
                   SEC_COUNT value
11/26/07   sc      Replaced the T_MSM7600 flag with 
                   FEATURE_SDCC_MMC_8BIT_BUS_WIDTH_SUPPORT
06/11/07   sc      Updated local data buff to be static
06/06/07   sc      Updated bus width setting for M7600 and fixed the precedence
                   for the MMC card size calculation
03/01/07   sc      Fixed lint warnings
02/15/07   sc      Initial version

===========================================================================*/
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_mmc_util.h"
#include "sdcc_handle_util.h"
#include "sdcc_partition_types.h"
#include "sdcc_hc.h"
#if !defined(FEATURE_SDCC_FOR_BOOT)
#include "AEEstd.h"
#endif /* !FEATURE_SDCC_FOR_BOOT */

/*lint -save -e641 Suppress 'Converting enum to int' warning */
/* buffer to retrieve the extended CSD data */
#define SDCC_MMC_EXT_CSD_SIZE 512
#ifdef SDHCI_AMDA_MODE_ENABLED
#pragma arm section zidata = ".bss.SDCC_ADMA_BUFF_ZI_ZONE"
static byte ext_csd_raw_data[SDCC_MMC_EXT_CSD_SIZE];
#pragma arm section zidata
#else
static byte ext_csd_raw_data[SDCC_MMC_EXT_CSD_SIZE];
#endif

static SDCC_STATUS sdcc_mmc_check_switch_error(sdcc_dev_type *sdcc_pdev);
static SDCC_STATUS sdcc_emmc_get_boot_partition_enable_val(
   uint8 part_config_reg, uint8 *boot_enabled_val);
static SDCC_STATUS sdcc_emmc_calc_write_prot_grp_size(
   sdcc_dev_type *sdcc_pdev, sdcc_ext_csd_type *ext_csd);
static SDCC_STATUS sdcc_emmc_check_addr_out_of_range(sdcc_dev_type *sdcc_pdev);
static SDCC_STATUS sdcc_emmc_set_user_pwr_on_wp(
   struct sdcc_device *handle, uint32 data_address, uint32 num_of_blocks);
static SDCC_STATUS sdcc_emmc_set_boot_pwr_on_wp(struct sdcc_device *handle);

/******************************************************************************
* Name: sdcc_mmc_check_switch_error
*
* Description:
*    This function sends CMD13 SEND_STATUS to the memory device to get the
*    card status.  It then checks the card state and switch error status.
*
* Arguments:
*    sdcc_pdev [IN] : pointer to device type
*
* Returns:
*    SDCC_NO_ERROR if card state is transfer state and no switch error occurs
*    Otherwise, returns error code
*
******************************************************************************/
static
SDCC_STATUS sdcc_mmc_check_switch_error
(
   sdcc_dev_type *sdcc_pdev
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_cmd_type sdcc_cmd;

   sdcc_cmd.cmd = SD_CMD13_SEND_STATUS;
   sdcc_cmd.cmd_arg = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed to get card status"));
      return rc;
   }

   /* check if current card state is in TRAN state */
   if ((SDCC_CARD_STATUS)SDCC_XLATE_CARD_CURRENT_STATE(sdcc_cmd.resp[0]) !=
       SDCC_CARD_TRAN)
   {
      DPRINTF(("eMMC is not ready"));
      return SDCC_ERR_CARD_READY;
   }

   /* check if switch error occurs */
   if (SDCC_XLATE_SWITCH_ERROR(sdcc_cmd.resp[0]) == SDCC_MMC_SWITCH_ERROR)
   {
      DPRINTF(("Switch error occurs"));
      return SDCC_ERR_SWITCH;
   }

   return SDCC_NO_ERROR;
} /* sdcc_mmc_check_switch_error */

/******************************************************************************
* Name: sdcc_mmc_switch
*
* Description:
*    This function sends CMD6 SWITCH to the memory device to modify the EXT_CSD
*    registers.  It then checks if switch error occurs.
*
* Arguments:
*    sdcc_pdev  [IN] : pointer to device type
*    switch_arg [IN] : command argument for CMD6 SWITCH
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_switch
(
   sdcc_dev_type *sdcc_pdev,
   uint32 switch_arg
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_cmd_type sdcc_cmd;

   sdcc_cmd.cmd = SD_CMD6_SWITCH;
   sdcc_cmd.cmd_arg = switch_arg;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 1;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   rc = sdcc_mmc_check_switch_error(sdcc_pdev);
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed %d in switching to 0x%x", rc, switch_arg));
      return rc;
   }

   return rc;
} /* sdcc_mmc_switch */

/******************************************************************************
* Name: sdcc_emmc_get_boot_partition_enable_val
*
* Description: 
*    This function extracts the BOOT_PARTITION_ENABLE bits from the
*    PARTITION_CONFIG slice in EXT_CSD register.
*
* Parameters:
*    part_config_reg   [IN] : PARTITION_CONFIG slice in EXT_CSD register
*    boot_enabled_val [OUT] : pointer to the value of BOOT_PARTITION_ENABLE bits
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_get_boot_partition_enable_val
(
   uint8 part_config_reg,
   uint8 *boot_enabled_val
)
{
   uint8 boot_val = 0;

   boot_val = SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(part_config_reg,
                 SDCC_EMMC_BIT_BOOT_PARTITION_ENABLE,
                 SDCC_EMMC_NUM_BITS_BOOT_PARTITION_ENABLE);
   /* validate the boot value */
   /* Per PARTITION_CONFIG definition in JEDEC Standard No. 84-A44, */
   /* 0x3 to 0x6 is reserved. */
   if (!SDCC_EMMC_IS_BOOT_PARTITION_VAL_VALID(boot_val))
   {
      DPRINTF(("Invalid boot partition enable value 0x%x", boot_val));
      return SDCC_ERR_INVALID_PARAM;
   }

   *boot_enabled_val = boot_val;
   return SDCC_NO_ERROR;
} /* sdcc_emmc_get_boot_partition_enable_val */

/******************************************************************************
* Name: sdcc_emmc_calc_write_prot_grp_size
*
* Description: 
*    This function calculates the write protection group size, in sectors.  It
*    examines if the memory device is embedded MMC, high-capacity erase unit
*    size and high-capacity write protect group size are non-zero.  If so,
*    ERASE_GROUP_DEF bit in extended CSD register is enabled to use the
*    high-capacity write protect group size definition.  If the memory device
*    is embedded MMC and either high-capacity erase unit size or high-capacity
*    write protect group size is zero, default write group size definition
*    from the CSD register is used.  If the memory device is a removable MMC
*    card, write protection should not be supported and the write protect group
*    size would be zero.
*
* Parameters:
*    sdcc_pdev [IN] : pointer to the device type
*    ext_csd   [IN] : pointer to the extended CSD structure that data is read from
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_calc_write_prot_grp_size
(
   sdcc_dev_type *sdcc_pdev,
   sdcc_ext_csd_type *ext_csd
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;

   /* examine if MMC device is embedded */
   if (sdcc_pdev->mem.mmc_device_type == SDCC_MMC_DEVICE_TYPE_REMOVABLE_CARD)
   {
      sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors = 0;
      /* only support write protect feature on embedded MMC */
      sdcc_pdev->mem.write_prot_support = FALSE;
      return SDCC_NO_ERROR;
   }

   /* make sure high-capacity erase-unit size or high-capacity */
   /* write protect group size are non-zero */
   if (ext_csd->hc_erase_grp_size == 0 || ext_csd->hc_wp_grp_size == 0)
   {
      /* Per section 8.4 in JEDEC Standard No. 84-A44, write protect group */
      /* size definition would be from CSD register.  As such, enabling */
      /* ERASE_GROUP_DEF is not needed. */
      /* The non high-capacity write protect group size has been calculated */
      /* when decoding CSD register. */
      return SDCC_NO_ERROR;
   }

   /* MMC device is embedded, either BGA or POP */
   /* enable ERASE_GROUP_DEF to use high-capacity write protect definition */
   rc = sdcc_mmc_switch(sdcc_pdev,
           SDCC_EMMC_ENABLE_ERASE_GROUP_DEF | ((uint32) 1 << 8));
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed in enabling ERASE_GROUP_DEF %d - Use default erase group size", rc));
      /* If the switch failed, use the write protect group size calculated 
       * from CSD register */
      return SDCC_NO_ERROR;
   }

   /* calculate the write protect group size per */
   /* section 8.4 in JEDEC Standard No. 84-A44 */
   sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors =
      (512 * 1024 / SDCC_DEFAULT_BLK_LENGTH_SIZE) *
      ext_csd->hc_erase_grp_size * ext_csd->hc_wp_grp_size;

   return SDCC_NO_ERROR;
} /* sdcc_emmc_calc_write_prot_grp_size */

/******************************************************************************
* Name: sdcc_emmc_check_addr_out_of_range
*
* Description:
*    This function sends CMD13 SEND_STATUS to the memory device to get the
*    card status.  It then checks if address out of range error bit is set.
*
* Arguments:
*    sdcc_pdev [IN] : pointer to device type
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_check_addr_out_of_range
(
   sdcc_dev_type *sdcc_pdev
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_cmd_type sdcc_cmd;

   sdcc_cmd.cmd       = SD_CMD13_SEND_STATUS;
   sdcc_cmd.cmd_arg   = SDCC_ARG_RCA(sdcc_pdev->rca);
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* check if address out of range error occurs */
   if (SDCC_MMC_ADDR_OUT_OF_RANGE_ERROR ==
       SDCC_XLATE_ADDR_OUT_OF_RANGE(sdcc_cmd.resp[0]))
   {
      DPRINTF(("Address out of range"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }

   return SDCC_NO_ERROR;
} /* sdcc_emmc_check_addr_out_of_range */

/******************************************************************************
* Name: sdcc_emmc_set_user_pwr_on_wp
*
* Description:
*    This function enables the power-on period write protection to the user
*    area.  The write protection is applied to the specified parameter
*    data_address.  Parameter num_of_blocks should be aligned with the write
*    protect group size specified in write_protect_group_size_in_sectors in
*    structure sdcc_mem_info_type.  CMD28 SET_WRITE_PROT is sent to the eMMC
*    device repeatedly until the requested regions are write protected.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    data_address  [IN] : start of the sector to get power-on write protected
*    num_of_blocks [IN] : number of blocks to be power-on write protected
*                         (512 bytes per block)
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_set_user_pwr_on_wp
(
   struct sdcc_device *handle,
   uint32 data_address,
   uint32 num_of_blocks
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   sdcc_cmd_type sdcc_cmd;
   uint32 loop_cnt = 0;
   uint32 i = 0;

   if (num_of_blocks == 0)
   {
      DPRINTF(("Invalid number of blocks for enabling power-on WP"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   memset(ext_csd_raw_data, 0, sizeof(ext_csd_raw_data));
   /* read the extended CSD register */
   rc = sdcc_get_ext_csd(sdcc_pdev, ext_csd_raw_data);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* refer to annex A.5 in JEDEC Standard No. 84-A44 for setting */
   /* the US_PWR_WP_EN */
   /* make sure power-on write protect is not disabled */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_USER_WP],
       SDCC_EMMC_BIT_US_PWR_WP_DIS, 1))
   {
      /* power-on period write protection for user area is disabled */
      DPRINTF(("Use of user power-on WP has been disabled"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }
   /* make sure permanent write protect is not enabled */
   /* issuing CMD28 when both US_PERM_WP_EN and US_PWR_WP_EN are set will */
   /* result in write protection group permanently protected */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_USER_WP],
       SDCC_EMMC_BIT_US_PERM_WP_EN, 1))
   {
      /* permanent write protection for user area is enabled */
      DPRINTF(("User permanent WP has been enabled"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }

   /* enable power-on period write protection for the user area */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_EMMC_ENABLE_USER_PWR_WP);
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed in enabling user power-on write protect %d", rc));
      return rc;
   }

   /* sanity check to make sure write protect group size is non-zero */
   if (sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors == 0)
   {
      DPRINTF(("Write protect group size is 0"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }

   /* calculate the number of loops that SET_WRITE_PROT needs to be sent */
   if (num_of_blocks % sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors)
   {
      loop_cnt = (num_of_blocks /
                 sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors) + 1;
   }
   else
   {
      loop_cnt = num_of_blocks /
                 sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors;
   }

   /* send CMD28 to set the write protection bit of the addressed group */
   sdcc_cmd.cmd       = SD_CMD28_SET_WRITE_PROT;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 1;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   for (i = 0; i < loop_cnt; i++)
   {
      sdcc_cmd.cmd_arg = SDCC_MEM_ACCESS_CMD_ARG(sdcc_pdev->card_type,
         data_address +
         i * sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors,
         sdcc_pdev->mem.mem_info.block_len);
      rc = sdcc_command(sdcc_pdev, &sdcc_cmd);
      /* check if error occurs in sending CMD28 */
      if (rc != SDCC_NO_ERROR)
      {
         DPRINTF(("CMD28 failed %d", rc));
         return rc;
      }

      /* As recommended by one of the eMMC device manufacturers, */
      /* check for ADDRESS_OUT_OF_RANGE in both responses as below: */
      /* 1) response to CMD28 per JEDEC Standard No. 84-A44 */
      /* 2) response to CMD13 */

      /* check if address is out of range in the CMD28 response */
      if (SDCC_MMC_ADDR_OUT_OF_RANGE_ERROR ==
          SDCC_XLATE_ADDR_OUT_OF_RANGE(sdcc_cmd.resp[0]))
      {
         DPRINTF(("CMD28 response with address out of range"));
         return SDCC_ERR_PWR_ON_WRITE_PROT;
      }

      /* send CMD13 to check if address is out of range */
      rc = sdcc_emmc_check_addr_out_of_range(sdcc_pdev);
      if (rc != SDCC_NO_ERROR)
      {
         DPRINTF(("CMD28 failed with address 0x%x", sdcc_cmd.cmd_arg));
         return rc;
      }
   }

   return SDCC_NO_ERROR;
} /* sdcc_emmc_set_user_pwr_on_wp */

/******************************************************************************
* Name: sdcc_emmc_set_boot_pwr_on_wp
*
* Description:
*    This function enables the power-on period write protection to the boot
*    areas.  The write protection is applied to the entire boot partition and
*    both boot areas would get write protected.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
static
SDCC_STATUS sdcc_emmc_set_boot_pwr_on_wp
(
   struct sdcc_device *handle
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   sdcc_pdev = handle->hdev;

   memset(ext_csd_raw_data, 0, sizeof(ext_csd_raw_data));
   /* read the extended CSD register */
   rc = sdcc_get_ext_csd(sdcc_pdev, ext_csd_raw_data);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* make sure power-on write protect is not disabled */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_BOOT_WP],
       SDCC_EMMC_BIT_BOOT_PWR_WP_DIS, 1))
   {
      /* power-on period write protection for boot area is disabled */
      DPRINTF(("Use of boot power-on WP has been disabled"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }
   /* make sure permanent write protect is not enabled */
   /* setting both B_PERM_WP_EN and B_PWR_WP_EN will result in the boot area */
   /* being permanently protected */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_BOOT_WP],
       SDCC_EMMC_BIT_BOOT_PERM_WP_EN, 1))
   {
      /* permanent write protection for boot area is enabled */
      DPRINTF(("Boot permanent WP has been enabled"));
      return SDCC_ERR_PWR_ON_WRITE_PROT;
   }

   /* check if power-on write protection has already been enabled */
   if (SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(
       ext_csd_raw_data[SDCC_EMMC_EXT_CSD_SLICE_BOOT_WP],
       SDCC_EMMC_BIT_BOOT_PWR_WP_EN, 1))
   {
      /* power-on write protection has already been enabled */
      /* sending CMD6 is not needed */
      return SDCC_NO_ERROR;
   }

   /* enable power-on period write protection for the boot area */
   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_EMMC_ENABLE_BOOT_PWR_WP);
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed in enabling boot power-on write protect %d", rc));
   }

   return rc;
} /* sdcc_emmc_set_boot_pwr_on_wp */

/******************************************************************************
* Name: sdcc_find_mmc_device
*
* Description: 
*    This function looks for the MMC device of the specified driveno.
*
* Parameters:
*    handle [IN] : a pointer to the SDCC device 
* 
* Returns:
*    card type that indicates if device is MMC or unknown
*
******************************************************************************/
SDCC_CARD_TYPE
sdcc_find_mmc_device ( struct sdcc_device *handle )
{
   SDCC_CARD_TYPE card_type = SDCC_CARD_UNKNOWN;
   sdcc_dev_type *sdcc_pdev = handle->hdev;
   SDCC_STATUS rc = SDCC_ERR_UNKNOWN;
   sdcc_cmd_type sdcc_cmd;
   uint32 i;

   /* CMD0: reset card first */
   sdcc_cmd.cmd       = SD_CMD0_GO_IDLE_STATE;
   sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_NONE;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

   (void) sdcc_command (sdcc_pdev, &sdcc_cmd);

   /* continue on to look for MMC card */
   for ( i = 0; i < SDCC_MMC_DETECT_MAX_RETRIES; i++ )
   {
      /* CMD1: sent voltage to be used */
      sdcc_cmd.cmd       = SD_CMD1_SEND_OP_COND;
      sdcc_cmd.cmd_arg   = SDCC_MMC_HCS_NEGOTIATE_OCR;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_CMD_ONLY;

      rc = sdcc_command( sdcc_pdev, &sdcc_cmd );

      if ( SDCC_NO_ERROR != rc )
      {
         /* exit the for-loop for MMC card detection if there is issue in */
         /* sending the command to the card */
         break;
      }
      else if ( sdcc_cmd.resp[0] & SDCC_DEVICE_RDY )
      {
         /* access mode validation: */
         /* byte access mode represents <= 2GB card size */
         /* sector access mode represents > 2GB card size, however */
         /* this needs to be reconfirmed by reading SEC_COUNT */
         /* in EXT_CSD */
         /* >2GB: [30:29] = 1,0 */
         if ( ((sdcc_cmd.resp[0] >> 30) & 1) &&
               !((sdcc_cmd.resp[0] >> 29) & 1) )
         {
            card_type = SDCC_CARD_MMCHC;
            DPRINTF(("Found MMCHC card after %dth retry\n", i));
         }
         else
         {
            card_type = SDCC_CARD_MMC;
            DPRINTF(("Found MMC card after %dth retry\n", i));
         }
         return card_type;
      }

      sdcc_osal_sleep( SDCC_MMC_DETECT_BUSY_RETRY_INTERVAL );
   }

   return card_type;
} /* sdcc_find_mmc_device */

/******************************************************************************
* Name: sdcc_config_mmc_bus_width
*
* Description:
*    This function configures the MMC bus width.
*
* Arguments:
*    driveno          [IN] : drive number
*    bus_width        [IN] : MMC bus width
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_config_mmc_bus_width
(
   uint32              driveno,
   SDCC_MMC_BUSWIDTH   bus_width
)
{
   uint32 status = FALSE;

   /* Configure the bus width to the controller */
   switch (bus_width)
   {
   case SDCC_MMC_BUSWIDTH_1BIT:
      HAL_sdcc_SetBusWidth (driveno, HAL_SDCC_BUS_WIDTH_1_BIT);
      status = TRUE;
      break;

   case SDCC_MMC_BUSWIDTH_4BIT:
   case SDCC_MMC_BUSWIDTH_DDR_4BIT:
      HAL_sdcc_SetBusWidth (driveno, HAL_SDCC_BUS_WIDTH_4_BIT);
      status = TRUE;
      break;

   case SDCC_MMC_BUSWIDTH_8BIT:
   case SDCC_MMC_BUSWIDTH_DDR_8BIT:
      /* Check to see if SDCC controller supports 8 bit bus mode */
      status = HAL_sdcc_Supports8BitBus (driveno);
      if (status == TRUE)
      {
         HAL_sdcc_SetBusWidth (driveno, HAL_SDCC_BUS_WIDTH_8_BIT);
      }
      else
      {
         DPRINTF (("8BIT BUSWIDTH Not Supported\n"));
      }
      break;

   default:
      DPRINTF (("bus_width Not Supported\n"));
      status = FALSE;
   }
   return(status ? TRUE : FALSE);
} /* sdcc_config_mmc_bus_width */

/******************************************************************************
* Name: sdcc_config_mmc_modes_segment
*
* Description:
*    This function programs the modes segment in EXT_CSD register of the
*    MMC/eMMC device.  It gets the EXT_CSD information and sends CMD6 SWITCH
*    command to configure the HS_TIMING and BUS_WIDTH.  It also calculates the
*    card size for the device that is higher than 2GB, boot partition size and
*    write protect group size.
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS 
sdcc_config_mmc_modes_segment( sdcc_dev_type *sdcc_pdev )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   uint32 driveno = sdcc_pdev->driveno;
   SDCC_MMC_BUSWIDTH buswidth;
   sdcc_ext_csd_type ext_csd;
   uint8 boot_enabled_val = 0;
   boolean DDRSupport = FALSE;

   int i;

   /* get the MMC EXT_CSD information */
   memset(&ext_csd, 0, sizeof(ext_csd));
   rc = sdcc_get_ext_csd(sdcc_pdev, ext_csd_raw_data);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* decode ext_csd_raw_data */
   (void) sdcc_decode_extCSD(ext_csd_raw_data, &ext_csd);
#ifdef FEATURE_RUMI_BOOT
   ext_csd.card_type = SDCC_HIGHSPEED_MMC_26MHZ;
#endif
   /* sanity check for high density MMC card that the sec_count */
   /* should not be 0 */
   if (sdcc_pdev->card_type == SDCC_CARD_MMCHC && ext_csd.sec_count == 0)
   {
      return SDCC_ERR_UNKNOWN;
   }

   /* Some MMC cards with card size <= 2GB may have invalid non-zero */
   /* SEC_COUNT value.  Ignore the SEC_COUNT if card size <= 2GB. */
   /* Since some MMC cards with card size > 4GB may not comply with */
   /* MMC v4.2, the sdcc_pdev->mem.mem_info.card_size_in_sectors may have */
   /* been populated. */
   /* That is only reconfirm device is MMCHC if */
   /* sdcc_pdev->mem.mem_info.card_size_in_sectors is 0 or > 2GB in */
   /* absolute block length. */
   if (ext_csd.sec_count != 0 &&
       (sdcc_pdev->mem.mem_info.card_size_in_sectors == 0 ||
        sdcc_pdev->mem.mem_info.card_size_in_sectors >
        (uint32)(SDCC_MMC_2GB_CARD_SIZE / SDCC_DEFAULT_BLK_LENGTH_SIZE)) )
   {
      /* calculate the card size for high density MMC card */
      /* device density is calculated from the EXT_CSD by */
      /* multiplying the value of the sector count by 512B/sector */
      /* for example, 2Tera bytes is (4294967296 x 512B) */
      sdcc_pdev->mem.mem_info.card_size_in_sectors = 512 *
         (ext_csd.sec_count / SDCC_DEFAULT_BLK_LENGTH_SIZE);
   }

   /* save the number of reliable write sector count supported */
   if (ext_csd.rel_wr_sec_c != 0)
   {
      DPRINTF(("Reliable write is supported with sector count %d",
              ext_csd.rel_wr_sec_c));
      sdcc_pdev->mem.mem_info.reliable_write_sector_count =
         (uint32)ext_csd.rel_wr_sec_c;
   }

   /* determine which physical partition is bootable */
   rc = sdcc_emmc_get_boot_partition_enable_val(ext_csd.partition_config,
                                                &boot_enabled_val);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }
   sdcc_pdev->mem.boot_partition_enable_val = boot_enabled_val;

   /* check the freq that the card can support */
   if ((ext_csd.card_type & SDCC_HIGHSPEED_MMC_26MHZ) ||
       (ext_csd.card_type & SDCC_HIGHSPEED_MMC_52MHZ))
   {
      /* enable HS_TIMING */
      rc = sdcc_mmc_switch(sdcc_pdev,
                           SDCC_MMCPLUS_ENABLE_HS_TIMING | ((uint32)(1 << 8)));
      if (rc != SDCC_NO_ERROR)
      {
         DPRINTF(("Failed in enabling HS_TIMING %d", rc));
         return rc;
      }

      /* set clock freq to high speed */
      /* high speed clk freq is set based on card type */
      if (ext_csd.card_type & SDCC_HIGHSPEED_MMC_52MHZ)
      {
         /* Cards support 52MHz */
         sdcc_pdev->mem.clk_freq = SDCC_MMC_HIGH_FREQUENCY_52MHZ;
      }
      else
      {
         /* Cards support 26MHz */
         sdcc_pdev->mem.clk_freq = SDCC_MMC_HIGH_FREQUENCY_26MHZ;
      }

      if (sdcc_pdev->card_type == SDCC_CARD_MMCHC)
      {
         /* confirmed device is MMCPlus as well as MMCHC */
         DPRINTF(("MMCPlus High Density card found\n"));
      }
      else
      {
         /* confirmed device is MMCPlus */
         DPRINTF(("MMCPlus card found\n"));
      }
   }

   /* re-program the clock according to the MMC card type */
   sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_MODE);

   /* Check if DDR50 is supported */
   if (0 == (ext_csd.card_type & SDCC_HIGHSPEED_MMC_DDR_52MHZ))
   {
      DDRSupport = FALSE;
   }
   /* In SDHCi mode, check host's capability if feature is supported */
   else if ((TRUE == sdcc_pdev->sdhci_mode) && (FALSE == sdcc_pdev->sdhci_ddr_support))
   {
      DDRSupport = FALSE;
   }
   else
   {
      DDRSupport = TRUE;
   }

   /* configure MMC BUS_WIDTH */
   /* If the slot is embedded type, assume 8 bit to be available */
   if (sdcc_bsp_get_slot_type(driveno) & SDCC_BSP_8BIT_SLOT)
   {
      buswidth = (TRUE == DDRSupport) ? SDCC_MMC_BUSWIDTH_DDR_8BIT : SDCC_MMC_BUSWIDTH_8BIT;
      sdcc_pdev->mem.speed_class = ext_csd.min_perf_w_8_52;
   }
   else
   {
      buswidth = (TRUE == DDRSupport) ? SDCC_MMC_BUSWIDTH_DDR_4BIT : SDCC_MMC_BUSWIDTH_4BIT;
      sdcc_pdev->mem.speed_class = ext_csd.min_perf_w_8_26_4_52;
   }

   rc = sdcc_mmc_switch(sdcc_pdev, SDCC_MMCPLUS_CHANGE_BUSWIDTH | 
                        ((uint32)(buswidth << 8)));
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed in changing bus width %d", rc));
      return rc;
   }

   /* configure MMC bus width */
   if (TRUE == sdcc_pdev->sdhci_mode)
   {
      rc = sdcc_hc_config_buswidth (driveno, buswidth);
      if (SDCC_NO_ERROR != rc)
      {
         DPRINTF(("Failed in config bus width %d", rc));
         return rc;
      }

      if (TRUE == DDRSupport)
      {
         /* In Qualcomm interface mode, DDR50 mode doubles the mCLK frequency
          * (the SCLK seen by the memory device is still 50MHz). 
          * In SDHCi mode, the below functions set the SCLK frequency directly.
          * Set the clock to 50MHz */         
          
         /* switch to DDR50 mode */
         (void) sdcc_hc_enable_ddr50 (driveno);
         /* Enable the clock with new frequency */
         sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_MODE);   
      }
   }
   else /* Qualcomm mode */
   {
      (void) sdcc_config_mmc_bus_width(driveno, buswidth);

      if (TRUE == DDRSupport)
      {
         /* Enable DDR for Qualcomm interface mode */
         HAL_sdcc_SetClockEdge(driveno, HAL_SDCC_CLOCK_EDGE_DDR);
         /* re-program the mclk clock to support DDR50 */
         sdcc_config_clk(sdcc_pdev, SDCC_DATA_TRANSFER_DDR_MODE);       
      }
   }

   if (sdcc_pdev->mem.write_prot_support == TRUE)
   {
      /* calculate the write protect group size (in bytes) */
      rc = sdcc_emmc_calc_write_prot_grp_size(sdcc_pdev, &ext_csd);
   }

   /* Decode and parse which physical parition is in use */
   /* First, user partition is always in use */
   sdcc_pdev->mem.partition_inuse_flag = SDCC_BMSK_USER_PARTITION;

   /* update num_phy_partition_created field */
   if (ext_csd.boot_size_multi == 0)
   {
      sdcc_pdev->mem.partition_support = FALSE;
      /* per section 8.4 in JEDEC Standard No. 84-A43 */
      /* no boot partition available or boot mode not supported */
      /* User area is a partition. */
      sdcc_pdev->mem.mem_info.num_phy_partition_created = 1;
   }
   else
   {
      /* boot partition or boot mode is supported */
      sdcc_pdev->mem.partition_support = TRUE;
      sdcc_pdev->mem.mem_info.num_phy_partition_created = 3;
      /* per section 8.4 in JEDEC Standard No. 84-A43 */
      /* boot partition size is calculated as */
      /* boot partition size = 128Kbytes x BOOT_SIZE_MULT */
      sdcc_pdev->mem.partition_boot_size = 128 * ext_csd.boot_size_multi;
      /* Mark the boot partition inuse flag */
      sdcc_pdev->mem.partition_inuse_flag |= SDCC_BMSK_BOOT1_PARTITION |
                                             SDCC_BMSK_BOOT2_PARTITION;
   }

   /* Check to see if any of the GPP partition is in use */
   /* per section 7.4.64 in JEDEC Standard No. 84-B45 */
   /* GPP partition size is calculated as */
   /* GPP partition size = WP_GRP_SIZE x ERASE_GRP_SIZE x GP_SIZE_MULT */

   for (i = 0; i < 4; i++)
   {
      if (ext_csd.gp_size_mult[i] != 0)
      {
         sdcc_pdev->mem.mem_info.num_phy_partition_created++;
         sdcc_pdev->mem.partition_inuse_flag |= (SDCC_BMSK_GP1_PARTITION << i);
         sdcc_pdev->mem.partition_gp_size[i] = ext_csd.gp_size_mult[i] * 
                    sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors;
      }
   }

   sdcc_pdev->mem.partition_conf_done = ext_csd.partition_setting_completed;
   return rc;
} /* sdcc_config_mmc_modes_segment */

/******************************************************************************
* Name: sdcc_get_ext_csd
*
* Description:
*    This function gets the EXT_CSD register as a block of data from the
*    MMCPlus card.  It performs the adtc command with the read operation.
*
* Arguments:
*    sdcc_pdev              [IN] : pointer to device type
*    buff                  [OUT] : pointer to buffer for the incoming data
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_get_ext_csd
(
   sdcc_dev_type   *sdcc_pdev,
   byte                  *buff
)
{
   sdcc_cmd_type   sdcc_cmd;
   SDCC_STATUS     rc          = SDCC_ERR_READ_FIFO;

   sdcc_cmd.cmd       = SD_CMD8_SEND_EXT_CSD;
   sdcc_cmd.cmd_arg   = SDCC_CMD_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;

   rc = sdcc_do_transfer(sdcc_pdev, &sdcc_cmd, buff, 1/*block*/);

   return rc;
} /* sdcc_get_ext_csd */

/******************************************************************************
* Name: sdcc_decode_extCSD
*
* Description:
*    This function is to parse out the Extended CSD data.
*
* Arguments:
*    data          [IN] : pointer to buffer that contains ext_csd data
*    ext_csd      [OUT] : pointer to the structure to save the ext csd data to
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_decode_extCSD
(
   const byte          *data,
   sdcc_ext_csd_type   *ext_csd
)
{
   uint8 value;
   uint32 lvalue;

   if ( NULL == ext_csd )
   {
      return FALSE;
   }

   /* in data[143:145] */
   /* GP_SIZE_MULT_1: general purpose partition support --> ext_csd[143:145] in wp_grp_size */
   lvalue = *(data + 143);
   lvalue |= *(data + 144) << 8;
   lvalue |= *(data + 145) << 16;
   ext_csd->gp_size_mult[0] = lvalue;

   /* in data[146:148] */
   /* GP_SIZE_MULT_2: general purpose partition support --> ext_csd[146:148] in wp_grp_size */
   lvalue = *(data + 146);
   lvalue |= *(data + 147) << 8;
   lvalue |= *(data + 148) << 16;
   ext_csd->gp_size_mult[1] = lvalue;

   /* in data[149:151] */
   /* GP_SIZE_MULT_3: general purpose partition support --> ext_csd[149:151] in wp_grp_size */
   lvalue = *(data + 149);
   lvalue |= *(data + 150) << 8;
   lvalue |= *(data + 151) << 16;
   ext_csd->gp_size_mult[2] = lvalue;

   /* in data[152:154] */
   /* GP_SIZE_MULT_4: general purpose partition support --> ext_csd[152:154] in wp_grp_size */
   lvalue = *(data + 152);
   lvalue |= *(data + 153) << 8;
   lvalue |= *(data + 154) << 16;
   ext_csd->gp_size_mult[3] = lvalue;

   /* in data[155] */
   value = *(data + 155);
   /* partition_setting_completed: partition_setting_completed --> ext_csd[155] in bytes */
   ext_csd->partition_setting_completed = value;

   /* in data[160] */
   value = *(data + 160);
   /* partitioning_support: partitioning support --> ext_csd[160] in bytes */
   ext_csd->partitioning_support = value;

   /* in data[173] */
   value = *(data + 173);
   /* boot_wp: boot area write protection register --> ext_csd[173] in bytes */
   ext_csd->boot_wp = value;

   /* in data[174] */
   value = *(data + 174);
   /* user_wp: user area write protection register --> ext_csd[174] in bytes */
   ext_csd->user_wp = value;

   /* in data[175] */
   value = *(data + 175);
   /* erase_group_def: high-density erase group definition */
   /* --> ext_csd[175] in bytes */
   ext_csd->erase_group_def = value;

   /* in data[179] */
   value = *(data + 179);
   /* partition_config: partition configuration --> ext_csd[179] in bytes */
   ext_csd->partition_config = value;

   /* in data[189] */
   value = *(data + 189);
   /* cmd_set_rev: command set revision --> ext_csd[189] in bytes */
   ext_csd->cmd_set_rev = value;

   /* in data[191] */
   value = *(data + 191);
   /* cmd_set: command set --> ext_csd[191] in bytes */
   ext_csd->cmd_set = value;

   /* in data[192] */
   value = *(data + 192);
   /* ext_csd_rev: extended CSD revision --> ext_csd[192] in bytes */
   ext_csd->ext_csd_rev = value;

   /* in data[194] */
   value = *(data + 194);
   /* csd_structure: CSD structure version --> ext_csd[194] in bytes */
   ext_csd->csd_structure = value;

   /* in data[196] */
   value = *(data + 196);
   /* card_type: card type --> ext_csd[196] in bytes */
   ext_csd->card_type = value;

   /* in data[200] */
   value = *(data + 200);
   /* pwr_cl_52_195: power class for 52MHz @ 1.95V --> ext_csd[200] in bytes */
   ext_csd->pwr_cl_52_195 = value;

   /* in data[201] */
   value = *(data + 201);
   /* pwr_cl_26_195: power class for 26MHz @ 1.95V --> ext_csd[201] in bytes */
   ext_csd->pwr_cl_26_195 = value;

   /* in data[202] */
   value = *(data + 202);
   /* pwr_cl_52_360: power class for 52MHz @ 3.6V --> ext_csd[202] in bytes */
   ext_csd->pwr_cl_52_360 = value;

   /* in data[203] */
   value = *(data + 203);
   /* pwr_cl_26_360: power class for 26MHz @ 3.6V --> ext_csd[203] in bytes */
   ext_csd->pwr_cl_26_360 = value;

   /* in data[205] */
   value = *(data + 205);
   /* min_perf_r_4_26: minimum read performance for 4bit at 26MHz */
   /* --> ext_csd[205] in bytes */
   ext_csd->min_perf_r_4_26 = value;

   /* in data[206] */
   value = *(data + 206);
   /* min_perf_w_4_26: minimum write performance for 4bit at 26MHz */
   /* --> ext_csd[206] in bytes */
   ext_csd->min_perf_w_4_26 = value;

   /* in data[207] */
   value = *(data + 207);
   /* min_perf_r_8_26_4_52: minimum read performance for 8bit at 26MHz, */
   /* for 4bit at 52MHz --> ext_csd[207] in bytes */
   ext_csd->min_perf_r_8_26_4_52 = value;

   /* in data[208] */
   value = *(data + 208);
   /* min_perf_w_8_26_4_52: minimum write performance for 8bit at 26MHz, */
   /* for 4bit at 52MHz --> ext_csd[208] in bytes */
   ext_csd->min_perf_w_8_26_4_52 = value;

   /* in data[209] */
   value = *(data + 209);
   /* min_perf_r_8_52: minimum read performance for 8bit at 52MHz */
   /* --> ext_csd[209] in bytes */
   ext_csd->min_perf_r_8_52 = value;

   /* in data[210] */
   value = *(data + 210);
   /* min_perf_w_8_52: minimum write performance for 8bit at 52MHz */
   /* --> ext_csd[210] in bytes */
   ext_csd->min_perf_w_8_52 = value;

   /* in data[215:212] */
   value = *(data + 212);
   /* sec_count: sector count --> ext_csd[212] in bytes */
   ext_csd->sec_count = (uint32)value;
   value = *(data + 213);
   /* sec_count: sector count --> ext_csd[213:212] in bytes */
   ext_csd->sec_count |= (uint32)value << 8;
   value = *(data + 214);
   /* sec_count: sector count --> ext_csd[214:212] in bytes */
   ext_csd->sec_count |= (uint32)value << 16;
   value = *(data + 215);
   /* sec_count: sector count --> ext_csd[215:212] in bytes */
   ext_csd->sec_count |= (uint32)value << 24;

   /* in data[221] */
   value = *(data + 221);
   /* hc_wp_grp_size: high-capacity wrie protect group size */
   /* --> ext_csd[221] in bytes */
   ext_csd->hc_wp_grp_size = value;

   /* in data[222] */
   value = *(data + 222);
   /* rel_wr_sec_c: reliable write sector count --> ext_csd[222] in bytes */
   ext_csd->rel_wr_sec_c = value;

   /* in data[224] */
   value = *(data + 224);
   /* hc_erase_grp_size: high-capacity erase unit size --> ext_csd[224] in bytes */
   ext_csd->hc_erase_grp_size = value;

   /* in data[226] */
   value = *(data + 226);
   /* boot_size_multi: boot parition size --> ext_csd[226] in bytes */
   ext_csd->boot_size_multi = value;

   /* in data[504] */
   value = *(data + 504);
   /* s_cmd_set: supported command sets --> ext_csd[504] in bytes */
   ext_csd->s_cmd_set = value;

   return TRUE;
} /* sdcc_decode_extCSD */

/******************************************************************************
* Name: sdcc_decode_mmc_cid
*
* Description:
*    This function is to parse out the Card Identification (CID) register.
*
* Arguments:
*    raw_cid        [IN] : pointer to raw CID info
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean sdcc_decode_mmc_cid( const uint32   *raw_cid,
                             sdcc_dev_type  *sdcc_pdev )
{
   int i;
   uint32 prod_name;
   uint8 mfr_date;

   if ( NULL == raw_cid || NULL == sdcc_pdev )
   {
      return FALSE;
   }

   /* manufacturer ID: CID[127:120] */
   sdcc_pdev->mem.mem_info.mfr_id = (uint16)(raw_cid[0] >> 24);

   /* poke into CID[113:112] to determine the CBX device type */
   sdcc_pdev->mem.mmc_device_type = (uint8)((raw_cid[0] >> 16) & 0x00000003);

   /* OEM/application ID: CID[111:104] */
   sdcc_pdev->mem.mem_info.oem_id = (uint16)((raw_cid[0] >> 8) & 0x000000FF);

   /* product name: CID[103:56] */
   sdcc_pdev->mem.mem_info.prod_name[0] = (byte)(raw_cid[0]);
   prod_name = raw_cid[1];
   for ( i = 4; i > 0; i-- )
   {
      sdcc_pdev->mem.mem_info.prod_name[i] = (byte)(prod_name & 0xFF);
      prod_name >>= 8;
   }
   sdcc_pdev->mem.mem_info.prod_name[5] = (byte)(raw_cid[2] >> 24);
   sdcc_pdev->mem.mem_info.prod_name[6] = '\0';

   /* product revision: CID[55:48] */
   sdcc_pdev->mem.mem_info.prod_rev = (uint8)(raw_cid[2] >> 16);

   /* product serial number: CID[47:16] */
   sdcc_pdev->mem.mem_info.prod_serial_num =
      ((raw_cid[2] & 0x0000FFFF) << 16) | (raw_cid[3] >> 16);

   /* manufacturing date: CID[15:8] */
   mfr_date = (uint8)(raw_cid[3] >> 8);
   (void)sdcc_decode_mmc_mfr_date( mfr_date, sdcc_pdev );

   return TRUE;
} /* sdcc_decode_mmc_cid */

/******************************************************************************
* Name: sdcc_decode_mmc_mfr_date
*
* Description:
*    This function decodes the manufacturing date as mm/yyyy format.
*
* Arguments:
*    mfr_date       [IN] : raw manufacturing date
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_decode_mmc_mfr_date
(
   uint8          mfr_date,
   sdcc_dev_type *sdcc_pdev
)
{
   (void) mfr_date;
   (void) sdcc_pdev;
   return TRUE;
} /* sdcc_decode_mmc_mfr_date */

/******************************************************************************
* Name: sdcc_mmc_set_partition_access
*
* Description:
*    This function sets the partition for read and write operation by
*    sending CMD6 SWITCH to the eMMC device.
*
* Arguments:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_set_partition_access( struct sdcc_device *handle )
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   uint32 phy_partition_num = 0;

   sdcc_pdev = handle->hdev;
   /* get the device info specified by the current interface */

   /* translate physical partition number of the client handle */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   /* check if device supports partition */
   /* User area has partition number as 0. */
   if ( phy_partition_num && FALSE == sdcc_pdev->mem.partition_support )
   {
      DPRINTF(("Partition not supported"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   if ( !(SDCC_XLATE_PARTITION_ID_TO_MASK (phy_partition_num) & 
                         sdcc_pdev->mem.partition_inuse_flag) )
   {
      DPRINTF(("Invalid partition number %d", phy_partition_num));
      return SDCC_ERR_INVALID_PARAM;
   }

   rc = sdcc_mmc_switch(sdcc_pdev,
           SDCC_EMMC_SET_PARTITION_ACCESS |
           SDCC_EMMC_CONSTRUCT_PARTITION_CONFIG_VAL_FOR_SWITCH(
              sdcc_pdev->mem.boot_partition_enable_val, phy_partition_num));
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed to set EMMC partition %d access, rc %d",
              phy_partition_num, rc));
      return rc;
   }

   /* set the active partition number on the device list */
   sdcc_pdev->active_partition_num = phy_partition_num;
   DPRINTF(("Active partition is updated to %d", phy_partition_num));

   return rc;
} /* sdcc_mmc_set_partition_access */

/******************************************************************************
* Name: sdcc_mmc_get_phy_partition_info
*
* Description: 
*    This function gets the physical partition information on eMMC device.
*
* Parameters:
*    handle                     [IN] : a pointer to the SDCC device that was
*                                      returned from sdcc_handle_open()
*    partition_size_in_sectors [OUT] : physical partition size in sectors
*    phy_partition_type        [OUT] : type of physical partition to indicate
*                                      if partition is the user area, boot
*                                      partition, general purpose partition,
*                                      etc.
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_get_phy_partition_info
(
   struct sdcc_device *handle,
   uint32 *partition_size_in_sectors,
   uint32 *phy_partition_type
)
{
   sdcc_dev_type *sdcc_pdev = NULL;
   uint32 phy_partition_num = 0;

   sdcc_pdev = handle->hdev;

   /* translate physical partition number of the client handle */
   /* SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE is translated as */
   /* partition number 0 which is the user area. */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   /* check if device supports partition */
   /* User area has partition number as 0. */
   if ( phy_partition_num && FALSE == sdcc_pdev->mem.partition_support )
   {
      DPRINTF(("Partition is not supported"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   if ( !(SDCC_XLATE_PARTITION_ID_TO_MASK (phy_partition_num) & 
                         sdcc_pdev->mem.partition_inuse_flag) )
   {
      DPRINTF(("Invalid partition number %d", phy_partition_num));
      return SDCC_ERR_INVALID_PARAM;
   }

   if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_USER_AREA)
   {
      /* Physical partition number 0 is the user area. */
      /* As such, returns card size. */
      *partition_size_in_sectors = sdcc_pdev->mem.mem_info.card_size_in_sectors;
      *phy_partition_type = PARTITION_USER_AREA;
   }
   else if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_BOOT_PART_1)
   {
      /* Boot partitions 1 and 2 have the same boot partition size. */
      /* Refer to section 7.2.1 in JEDEC Standard No. 84-A43 */
      *partition_size_in_sectors =
         SDCC_CALC_PARTITION_SIZE_IN_SECTORS(sdcc_pdev->mem.partition_boot_size);
      *phy_partition_type = PARTITION_BOOT_1;
   }
   else if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_BOOT_PART_2)
   {
      *partition_size_in_sectors =
         SDCC_CALC_PARTITION_SIZE_IN_SECTORS(sdcc_pdev->mem.partition_boot_size);
      *phy_partition_type = PARTITION_BOOT_2;
   }
   else if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_GPP_1)
   {
      *partition_size_in_sectors = sdcc_pdev->mem.partition_gp_size[0];
      *phy_partition_type = PARTITION_GPP_1;
   }
   else if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_GPP_2)
   {
      *partition_size_in_sectors = sdcc_pdev->mem.partition_gp_size[1];
      *phy_partition_type = PARTITION_GPP_2;
   }
   else if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_GPP_3)
   {
      *partition_size_in_sectors = sdcc_pdev->mem.partition_gp_size[2];
      *phy_partition_type = PARTITION_GPP_3;
   }
   else if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_GPP_4)
   {
      *partition_size_in_sectors = sdcc_pdev->mem.partition_gp_size[3];
      *phy_partition_type = PARTITION_GPP_4;
   }
   else
   {
      return SDCC_ERR_INVALID_PARAM;
   }

   return SDCC_NO_ERROR;
} /* sdcc_mmc_get_phy_partition_info */

/******************************************************************************
* Name: sdcc_mmc_set_pwr_on_write_prot
*
* Description:
*    This function sets the power-on period write protection to the physical
*    partition specified in parameter handle.  If the physical partition is
*    the user area, the write protection is applied to the specified parameter
*    data_address.  Parameter num_of_blocks should be aligned with the write
*    protect group size specified in write_protect_group_size_in_sectors in
*    structure sdcc_mem_info_type.  If the physical partition is the boot area,
*    the write protection is enabled to the entire boot partition and both boot
*    areas would get write protected.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    data_address  [IN] : start of the sector to get power-on write protected
*    num_of_blocks [IN] : number of blocks to be power-on write protected
*                         (512 bytes per block)
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_set_pwr_on_write_prot
(
   struct sdcc_device *handle,
   uint32 data_address,
   uint32 num_of_blocks
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;

   sdcc_pdev = handle->hdev;

   /* select the requested partition */
   rc = sdcc_mmc_set_partition_access(handle);
   if (rc != SDCC_NO_ERROR)
   {
      return rc;
   }

   /* examine which physical partition would like to have the power-on */
   /* write protect enabled */
   if (SDCC_EMMC_IS_FROM_USER_AREA_PART(sdcc_pdev->active_partition_num))
   {
      rc = sdcc_emmc_set_user_pwr_on_wp(handle, data_address, num_of_blocks);
   }
   else if (SDCC_EMMC_IS_FROM_BOOT_AREA_PART(sdcc_pdev->active_partition_num))
   {
      rc = sdcc_emmc_set_boot_pwr_on_wp(handle);
   }
   else
   {
      /* active partition number is RPMB which is not supported */
      DPRINTF(("RPMB is not supported"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   return rc;
} /* sdcc_mmc_set_pwr_on_write_prot */

/******************************************************************************
* Name: sdcc_mmc_set_active_bootable_partition
*
* Description: 
*    This function sets the BOOT_PARTITION_ENABLE bits specified in parameter
*    handle.  It clears the PARTITION_ACCESS bits if the current active
*    partition is not the user area.  It then sends the SWITCH command to the
*    eMMC device to set the BOOT_PARTITION_ENABLE bits in the EXT_CSD register.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_set_active_bootable_partition
(
   struct sdcc_device *handle
)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   uint32 phy_partition_num = 0;
   uint32 boot_part_val = 0;

   sdcc_pdev = handle->hdev;

   /* translate physical partition number of the client handle */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);
   /* validate phy_partition_num */
   if (phy_partition_num >= sdcc_pdev->mem.mem_info.num_phy_partition_created)
   {
      DPRINTF(("Invalid partition number %d", phy_partition_num));
      return SDCC_ERR_INVALID_PARAM;
   }

   if (sdcc_mmc_is_phy_partition_bootable(handle) == TRUE)
   {
      /* BOOT_PARTITION_ENABLE already has the value set */
      return SDCC_NO_ERROR;
   }

   /* Clear the PARTITION_ACCESS bits if the active partition is not user */
   /* area.  Refer to section 7.3.5 in JEDEC Standard No. 84-A44 */
   if (sdcc_pdev->active_partition_num != SDCC_EMMC_PHY_PARTITION_USER_AREA)
   {
      rc = sdcc_mmc_switch(sdcc_pdev,
              SDCC_EMMC_SET_PARTITION_ACCESS |
              SDCC_EMMC_CONSTRUCT_PARTITION_CONFIG_VAL_FOR_SWITCH(
                 sdcc_pdev->mem.boot_partition_enable_val, 0));
      if (rc != SDCC_NO_ERROR)
      {
         DPRINTF(("Failed to clear the PARTITION_ACCESS bits, rc %d", rc));
         return rc;
      }
   }

   /* map phy_partition_num to the value for BOOT_PARTITION_ENABLE */
   /* refer to section 8.4 in JEDEC Standard No. 84-A44 */
   boot_part_val =
      SDCC_EMMC_MAP_PHY_PART_NUM_TO_BOOT_PART_VAL(phy_partition_num);

   /* set the active boot partition */
   rc = sdcc_mmc_switch(sdcc_pdev,
           SDCC_EMMC_SET_BOOT_PARTITION_ENABLE |
           SDCC_EMMC_CONSTRUCT_PARTITION_CONFIG_VAL_FOR_SWITCH(boot_part_val,
              sdcc_pdev->active_partition_num));
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed to set boot partition enable value %d, rc %d",
         boot_part_val, rc));
      return rc;
   }

   sdcc_pdev->mem.boot_partition_enable_val = boot_part_val;

   return rc;
} /* sdcc_mmc_set_active_bootable_partition */

/******************************************************************************
* Name: sdcc_mmc_is_phy_partition_bootable
*
* Description: 
*    This function examines if the partition specified in parameter handle is
*    the bootable partition. 
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    TRUE if partition is bootable.  FALSE otherwise.
*
******************************************************************************/
boolean
sdcc_mmc_is_phy_partition_bootable
(
   struct sdcc_device *handle
)
{
   sdcc_dev_type *sdcc_pdev = NULL;
   uint32 phy_partition_num = 0;

   sdcc_pdev = handle->hdev;

   /* translate physical partition number of the client handle */
   /* SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE is translated as */
   /* partition number 0 which is the user area. */
   phy_partition_num = SDCC_XLATE_PARTITION_NUM(handle->phy_partition_num);

   /* determine if the queried physical partition is bootable */
   switch (sdcc_pdev->mem.boot_partition_enable_val)
   {
   case SDCC_EMMC_BOOT_DATA_USER_AREA_ENABLED:
      if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_USER_AREA)
      {
         return TRUE;
      }
      break;

   case SDCC_EMMC_BOOT_DATA_BOOT_PART_1_ENABLED:
      if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_BOOT_PART_1)
      {
         return TRUE;
      }
      break;

   case SDCC_EMMC_BOOT_DATA_BOOT_PART_2_ENABLED:
      if (phy_partition_num == SDCC_EMMC_PHY_PARTITION_BOOT_PART_2)
      {
         return TRUE;
      }
      break;

   case SDCC_EMMC_BOOT_DATA_NOT_BOOT_ENABLED:
   default:
      break;
   }

   return FALSE;
} /* sdcc_mmc_is_phy_partition_bootable */


/******************************************************************************
* Name: sdcc_mmc_config_gpp_enh
*
* Description:
*    This function configures the General Purpose Partition and 
*    enhanced user data area information
*    Note: This routine can only be used once for the life of each
*    emmc chip!
*
* Arguments:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open()
*    desc       [IN] : a pointer to the structure that describes the layout
*                      of the GPP and ENH partition information
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_config_gpp_enh (struct sdcc_device *handle,
                         sdcc_emmc_gpp_enh_desc_type *desc)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   sdcc_dev_type *sdcc_pdev = NULL;
   uint32 wp_grp_size;               /* write protect group size in sectors */
   uint32 gpp_grp_size;
   uint32 pa_size = 0;
   uint32 pa_offset = 0;
   int i;

   sdcc_pdev = handle->hdev;
   wp_grp_size = sdcc_pdev->mem.mem_info.write_protect_group_size_in_sectors;

   /* check if device supports partition */
   if (sdcc_pdev->mem.partition_support == FALSE || wp_grp_size == 0)
   {
      DPRINTF(("Partition not supported"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   /* Check if the OTP area is alreay programmed */
   if (sdcc_pdev->mem.partition_conf_done & SDCC_MMC_PARTI_SET_COMPLETED_BMSK)
   {
      DPRINTF(("Device already programmed with partition information"));
      return SDCC_ERR_FEATURE_UNSUPPORTED;
   }

   /* Loop through the GPP configuration and update the extended CSD table */
   for (i = 0; i < 5; i++)
   {
      switch (i) {
         case 0:
            pa_offset = SDCC_EMMC_SET_GPP1_SIZE;
            pa_size = desc->GPP_size[0];
            break;
         case 1:
            pa_offset = SDCC_EMMC_SET_GPP2_SIZE;
            pa_size = desc->GPP_size[1];
            break;
         case 2:
            pa_offset = SDCC_EMMC_SET_GPP3_SIZE;
            pa_size = desc->GPP_size[2];
            break;
         case 3:
            pa_offset = SDCC_EMMC_SET_GPP4_SIZE;
            pa_size = desc->GPP_size[3];
            break;
         case 4:
            pa_offset = SDCC_EMMC_SET_ENH_SIZE;
            pa_size = desc->ENH_size;
            break;
      }

      if (pa_size != 0)
      {
         // A small logic to ensure we round up always during the division
         pa_size += (wp_grp_size - 1);
         gpp_grp_size = pa_size / wp_grp_size;
         rc = sdcc_mmc_switch(sdcc_pdev, pa_offset | 
                              SDCC_MMC_SET_DATA_VAL (gpp_grp_size & 0xff));
         if (rc != SDCC_NO_ERROR)
         {
            DPRINTF(("Failed to set GPP partition size, partition %d, rc %d", i, rc));
            return rc;
         }

         rc = sdcc_mmc_switch (sdcc_pdev, (pa_offset + SDCC_EMMC_CFG_BYTE1_OFFSET) | 
                              SDCC_MMC_SET_DATA_VAL ((gpp_grp_size >> 8) & 0xff));
         if (rc != SDCC_NO_ERROR)
         {
            DPRINTF(("Failed to set GPP partition size, partition %d, rc %d", i, rc));
            return rc;
         }

         rc = sdcc_mmc_switch (sdcc_pdev, (pa_offset + SDCC_EMMC_CFG_BYTE2_OFFSET) | 
                              SDCC_MMC_SET_DATA_VAL ((gpp_grp_size >> 16) & 0xff));
         if (rc != SDCC_NO_ERROR)
         {
            DPRINTF(("Failed to set GPP partition size, partition %d, rc %d", i, rc));
            return rc;
         }
      }
   }

   /* Set the enhance user area start address if the funcion is used */
   if (desc->ENH_size != 0)
   {
      uint32 sw_addr = SDCC_EMMC_SET_ENH_START_ADDR; 
      uint32 enh_start;
      enh_start = desc->ENH_start_addr * ((sdcc_pdev->mem.mem_info.card_type != SDCC_CARD_MMC) ?  
                                            1 : SDCC_DEFAULT_BLK_LENGTH_SIZE);
      for (i = 0; i < 4; i++)
      {
         rc = sdcc_mmc_switch (sdcc_pdev, sw_addr | 
                              SDCC_MMC_SET_DATA_VAL (enh_start & 0xff));
         if (rc != SDCC_NO_ERROR)
         {
            DPRINTF(("Failed to set ENH area start address, rc %d", rc));
            return rc;
         }
         enh_start = enh_start >> 8;
         sw_addr += SDCC_EMMC_CFG_BYTE1_OFFSET;
      }
   }

   // Set the Enhancement attribute flag
   rc = sdcc_mmc_switch (sdcc_pdev, SDCC_EMMC_SET_PARTITION_ATTR |
                        SDCC_MMC_SET_DATA_VAL (desc->GPP_enh_flag));
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed to set ENH area start address, rc %d", rc));
      return rc;
   }

   /* Finish by updateing the complete flag */
   // Disabled for debugging!
   rc = sdcc_mmc_switch (sdcc_pdev, SDCC_EMMC_PARTITION_SET_DONE);
   if (rc != SDCC_NO_ERROR)
   {
      DPRINTF(("Failed to set partition completition flag, rc %d", rc));
      return rc;
   }

   return rc;
} /* sdcc_mmc_config_gpp_enh */

/*lint -restore */

