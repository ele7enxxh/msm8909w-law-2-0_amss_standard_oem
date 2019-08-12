/*==================================================================
 *
 * FILE:        deviceprogrammer_storage_device.c
 *
 * DESCRIPTION:
 *   
 *
 *        Copyright © 2008-2013 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/emmc/deviceprogrammer_storage_device.c#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2014-03-03   dks     Added MMC FFU Support
 * 2013-07-15   ah      Removed warning
 * 2013-06-03   ah      Added legal header
 * 2013-05-31   ab      Initial checkin
 */

#include "deviceprogrammer_storage_device.h"
#include "deviceprogrammer_firehose.h"

static const int BLOCK_SIZE = 512;

void init_storage_device_struct(storage_device_t *storedev) {
    int i;

    for (i = PARTITION_EMMC_USER; i < PARTITION_EMMC_NONE; i++) {
        storedev->hsdev_partition_handles[i] = NULL;
    }
    storedev->sector_addresses_enabled = FALSE;
    storedev->drivenum = DEVICEPROGRAMMER_USE_DRIVENO;
    storedev->current_open_partition = PARTITION_EMMC_NONE;
    storedev->sector_size = 512;
}

char * storage_device_name(storage_device_t *storedev)
{
   char *storage_name={"eMMC"};
   return storage_name;
}

boolean init_storage_device_hw(storage_device_t *storedev) {
    SDCC_STATUS rc;
    int i;
    sdcc_mem_info_type mem_info;

    for (i = PARTITION_EMMC_USER; i < PARTITION_EMMC_NONE; i++) {
        if (i != PARTITION_EMMC_RPMB) {
            //FREEZE_WATCHDOG();
            storedev->hsdev_partition_handles[i] = sdcc_handle_open (
                                                       storedev->drivenum,
                                                       i);
            if (i == PARTITION_EMMC_USER) {
                if (storedev->hsdev_partition_handles[i] == NULL) {
                    storedev->hsdev_partition_handles[i] = sdcc_handle_open (
                                                                storedev->drivenum,
                                                                i);
                    if (storedev->hsdev_partition_handles[i] == NULL)
                        return FALSE;
                }

                rc = sdcc_handle_mem_get_device_info(storedev->hsdev_partition_handles[i], &mem_info);
                if (rc != SDCC_NO_ERROR)
                    return FALSE;
                if (mem_info.card_size_in_sectors > 0)
                    storedev->sector_size = mem_info.block_len;
            }

        }
    }

    //FREEZE_WATCHDOG();
    // TODO: Can we ignore the return value of this call ?
    rc = sdcc_handle_set_active_bootable_partition(
            storedev->hsdev_partition_handles[PARTITION_EMMC_USER]);
    if (rc != SDCC_NO_ERROR) {

    }
    return TRUE;
}

static boolean
deinit_storage_device_hw(storage_device_t *storedev, int *errno) {
    SDCC_STATUS rc;
    uint32 i;

    for (i = PARTITION_EMMC_USER; i < PARTITION_EMMC_NONE; i++) {
        if (storedev->hsdev_partition_handles[i] != NULL)   {
            rc = sdcc_handle_close (storedev->hsdev_partition_handles[i]);
            if (rc != SDCC_NO_ERROR)  {
                if (*errno != NULL)
                    *errno = rc;
                return FALSE;
            }
        }
        storedev->hsdev_partition_handles[i] = NULL;
    }

    rc = SDCC_NO_ERROR; // HACK - until SD code is updated
    //rc = sdcc_deinit_device (0);
    if (rc != SDCC_NO_ERROR)  {
        if (*errno != NULL)
            *errno = rc;
        return FALSE;
    }

    return TRUE;
}


boolean storage_device_open_partition(storage_device_t *storedev, byte partition) {
    sdcc_mem_info_type mem_info;
    SDCC_STATUS rc;

    if (/* partition < (byte) PARTITION_EMMC_USER || */
        partition >= (byte) PARTITION_EMMC_NONE ||
        partition == (byte) PARTITION_EMMC_RPMB)
        return FALSE;

    rc = sdcc_handle_mem_get_device_info(storedev->hsdev_partition_handles[0], &mem_info);
    if (rc != SDCC_NO_ERROR)
        return FALSE;

    if (partition > mem_info.num_phy_partition_created - 1)
        return FALSE;

    storedev->current_open_partition = (open_partition_type)partition;
    return TRUE;
}

boolean storageDeviceClosePartition(storage_device_t *storedev) {
    if (storedev->current_open_partition == PARTITION_EMMC_NONE)
        return FALSE;
    storedev->current_open_partition = PARTITION_EMMC_NONE;
    return TRUE;
}

boolean storageDeviceSetBootableStorageDrive(storage_device_t *storedev) {
    SDCC_STATUS rc;
    struct sdcc_device *hsdev_handle =
        storedev->hsdev_partition_handles[storedev->current_open_partition];

    if (hsdev_handle == NULL)
        return FALSE;

    rc = sdcc_handle_set_active_bootable_partition(hsdev_handle);
    if (rc != SDCC_NO_ERROR) {
        return FALSE;
    }
    return TRUE;
}

boolean storageDeviceErase(storage_device_t *storedev) {
    SDCC_STATUS rc;
    sdcc_mem_info_type mem_info;

    struct sdcc_device *hsdev_handle =
        storedev->hsdev_partition_handles[storedev->current_open_partition];
    if (hsdev_handle == NULL)
        return FALSE;

    rc = sdcc_handle_mem_get_device_info(hsdev_handle, &mem_info);
    if (rc != SDCC_NO_ERROR)
        return FALSE;

    rc = sdcc_handle_erase(hsdev_handle, 0, mem_info.card_size_in_sectors - 1);
    if (rc != SDCC_NO_ERROR)
        return FALSE;

    return TRUE;
}

boolean storageDeviceCreateStorageDrives(storage_device_t *storedev,
                               SIZE_T *drive_sector_sizes,
                               SIZE_T array_length) {
    SDCC_STATUS rc = SDCC_NO_ERROR;
    sdcc_emmc_gpp_enh_desc_type desc;
    SIZE_T i;
    struct sdcc_device *hsdev_handle =
        storedev->hsdev_partition_handles[storedev->current_open_partition];

    if (hsdev_handle == NULL)
        return FALSE;

    // Fill out as many values as we can
    for (i = 0; i < MIN(array_length, 4); i++) {
        desc.GPP_size[i] = drive_sector_sizes[i];
    }

    // Fill the rest with zeros
    for (; i < 4; i++) {
        desc.GPP_size[i] = drive_sector_sizes[i];
    }

    desc.ENH_size = 0;
    desc.ENH_start_addr = 0;
    desc.GPP_enh_flag = 0;
    logMessage("gpp1 %d gpp2 %d gpp3 %d gpp4 %d.",
               desc.GPP_size[0],
               desc.GPP_size[1],
               desc.GPP_size[2],
               desc.GPP_size[3]);

    rc = sdcc_config_emmc_gpp_enh (hsdev_handle, &desc);
    if (rc != SDCC_NO_ERROR) {
        return FALSE;
    }
    return TRUE;
}

boolean storageDeviceGetStorageInfo(storage_device_t *storedev) {
  SDCC_STATUS rc;
  sdcc_mem_info_type mem_info;
  struct sdcc_device *hsdev_handle =
      storedev->hsdev_partition_handles[storedev->current_open_partition];

  if (hsdev_handle == NULL)
      return FALSE;

  rc = sdcc_handle_mem_get_device_info(hsdev_handle, &mem_info);
  if (rc != SDCC_NO_ERROR) {
      return FALSE;
  }
  logMessage("num_partition_sectors=%d", mem_info.card_size_in_sectors);
  logMessage("SECTOR_SIZE_IN_BYTES=%d", mem_info.block_len);
  logMessage("num_physical_partitions=%d", mem_info.num_phy_partition_created);
  return TRUE;
}

SIZE_T storageDeviceGetPartitionSizeSectors(storage_device_t *storedev) {
    SDCC_STATUS rc;
    sdcc_mem_info_type mem_info;
    struct sdcc_device *hsdev_handle =
        storedev->hsdev_partition_handles[storedev->current_open_partition];

    if (hsdev_handle == NULL)
        return FALSE;

    rc = sdcc_handle_mem_get_device_info(hsdev_handle, &mem_info);
    if (rc != SDCC_NO_ERROR) {
        return FALSE;
    }
    return mem_info.card_size_in_sectors;
}

SIZE_T storageDeviceGetLowerBoundSector(storage_device_t *storedev) {
    if (0 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_0
            return ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_0;
        #endif
    }
    else if (1 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_1
            return ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_1;
        #endif
    }
    else if (2 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_2
            return ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_2;
        #endif
    }
    else if (3 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_3
            return ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_3;
        #endif
    }
    else if (4 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_4
            return ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_4;
        #endif
    }
    else if (5 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_5
            return ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_5;
        #endif
    }
    else if (6 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_6
            return ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_6;
        #endif
    }
    else if (7 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_7
            return ALLOW_WRITING_BEGINNING_AT_SECTOR_ADDR_7;
        #endif
    }
    return 0;
}

SIZE_T storageDeviceGetUpperBoundSector(storage_device_t *storedev) {
    if (0 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_UP_TO_SECTOR_ADDR_0
            return ALLOW_WRITING_UP_TO_SECTOR_ADDR_0;
        #endif
    }
    else if (1 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_UP_TO_SECTOR_ADDR_1
            return ALLOW_WRITING_UP_TO_SECTOR_ADDR_1;
        #endif
    }
    else if (2 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_UP_TO_SECTOR_ADDR_2
            return ALLOW_WRITING_UP_TO_SECTOR_ADDR_2;
        #endif
    }
    else if (3 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_UP_TO_SECTOR_ADDR_3
            return ALLOW_WRITING_UP_TO_SECTOR_ADDR_3;
        #endif
    }
    else if (4 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_UP_TO_SECTOR_ADDR_4
            return ALLOW_WRITING_UP_TO_SECTOR_ADDR_4;
        #endif
    }
    else if (5 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_UP_TO_SECTOR_ADDR_5
            return ALLOW_WRITING_UP_TO_SECTOR_ADDR_5;
        #endif
    }
    else if (6 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_UP_TO_SECTOR_ADDR_6
            return ALLOW_WRITING_UP_TO_SECTOR_ADDR_6;
        #endif
    }
    else if (7 == storedev->current_open_partition) {
        #ifdef ALLOW_WRITING_UP_TO_SECTOR_ADDR_7
            return ALLOW_WRITING_UP_TO_SECTOR_ADDR_7;
        #endif
    }
    return storageDeviceGetPartitionSizeSectors(storedev);
}

boolean storageDeviceRead(storage_device_t *storedev,
                          byte* buffer,
                          SIZE_T sector_address,
                          SIZE_T sector_length) {
    SDCC_STATUS rc;
    struct sdcc_device *hsdev_handle =
        storedev->hsdev_partition_handles[storedev->current_open_partition];

    if (hsdev_handle == NULL)
        return FALSE;

    //FREEZE_WATCHDOG();
    rc = sdcc_handle_read (
            hsdev_handle,
            sector_address,
            buffer,
            sector_length);

    if (rc != SDCC_NO_ERROR)
        return FALSE;

    return TRUE;
}

boolean storageDeviceWrite(storage_device_t *storedev,
                           byte* buffer,
                           SIZE_T sector_address,
                           SIZE_T sector_length,
                           int *errno) {
    SDCC_STATUS rc;
    struct sdcc_device *hsdev_handle =
        storedev->hsdev_partition_handles[storedev->current_open_partition];

    if (hsdev_handle == NULL)
        return FALSE;

    //FREEZE_WATCHDOG();
    rc = sdcc_handle_write (
            hsdev_handle,
            sector_address,
            buffer,
            sector_length);

    if (rc != SDCC_NO_ERROR) {
        if (errno != NULL)
            *errno = (int) rc;
        return FALSE;
    }

    return TRUE;
}

boolean storageDeviceFWUpdate(storage_device_t *storedev,
                              byte* fw_bin,
                              SIZE_T num_sectors,
                              int *errno) {
    SDCC_STATUS rc;
    int ffu_status = 0;
    struct sdcc_device *hsdev_handle;
    uint64 fw_ver_before = 0;
    uint64 fw_ver_after = 0;
    boolean fw_download_failed = FALSE;
    boolean result;

    hsdev_handle = sdcc_handle_open (0, PARTITION_EMMC_USER);
    if (hsdev_handle == NULL)  {
        logMessage("Could not open User partition. Check card.");
        if (errno != NULL)
            *errno = SDCC_ERR_UNKNOWN;
        return FALSE;
    }

    rc = SDCC_NO_ERROR; // HACK - until SD code is updated
    //rc = sdcc_handle_get_fw_version (hsdev_handle, &fw_ver_before);
    if (rc != SDCC_NO_ERROR)  {
        logMessage("Failed to get current FW Version before update %d", rc);
        if (errno != NULL)
            *errno = (int) rc;
        result = FALSE;
        goto cleanup;
    }

    rc = SDCC_NO_ERROR; // HACK - until SD code is updated
    //rc = sdcc_handle_fw_update (hsdev_handle, fw_bin, num_sectors);
    if (rc != SDCC_NO_ERROR) {
        logMessage("Failed to download FW Binary %d", rc);
        if (errno != NULL)
            *errno = (int) rc;
        fw_download_failed = TRUE; /* Note that FW Download failed, so that we
                              can return after trying card re-initialization */
    }

    /* Done with handle. Close it */
    (void) sdcc_handle_close (hsdev_handle);

    /* Bring device down for reset */
    if (FALSE == deinit_storage_device_hw (storedev, errno))  {
        logMessage("Failed to close all handles and deinit device");
        if (errno != NULL)
            logMessage("Deinit error %d", *errno);
        return FALSE;
    }

    if (FALSE == init_storage_device_hw (storedev))   {
        logMessage("Failed to Re-Initialize HW. Please Powercycle device and try");
        if (errno != NULL)
            *errno = (int) SDCC_ERR_UNKNOWN;
        return FALSE;
    }

    /* If FW download had failed, just return after attempting to restore the
       card status */
    if (fw_download_failed == TRUE)
    {
        return FALSE;
    }

    hsdev_handle = sdcc_handle_open (0, PARTITION_EMMC_USER);
    if (hsdev_handle == NULL)  {
        logMessage("Could not open User partition after reset. Check card.");
        if (errno != NULL)
            *errno = SDCC_ERR_UNKNOWN;
        return FALSE;
    }

    rc = SDCC_NO_ERROR; // HACK - until SD code is updated
    //rc = sdcc_handle_get_ffu_status (hsdev_handle, &ffu_status);
    if (rc != SDCC_NO_ERROR) {
        logMessage("Failed to get FFU status %d", rc);
        if (errno != NULL)
            *errno = (int) rc;
        result = FALSE;
        goto cleanup;
    }

    ffu_status = SDCC_NO_ERROR;  // HACK - until SD code is updated
    if (ffu_status != SDCC_NO_ERROR) {
        logMessage("FW Update failed with status %d", rc);
        if (errno != NULL)
            *errno = (int) rc;
        result = FALSE;
        goto cleanup;
    }

    rc = SDCC_NO_ERROR; // HACK - until SD code is updated
    //rc = sdcc_handle_get_fw_version (hsdev_handle, &fw_ver_after);
    if (rc != SDCC_NO_ERROR)  {
        logMessage("Failed to get current FW Version after update %d. Check card", rc);
        if (errno != NULL)
            *errno = (int) rc;
        result = FALSE;
        goto cleanup;
    }

    logMessage("FW Update Successful. FW Version updated from '0x%X' to '0x%X'",
               (int) fw_ver_before, (int) fw_ver_after);
    result = TRUE;

cleanup:
    if (hsdev_handle != NULL)  {
        (void) sdcc_handle_close (hsdev_handle);
    }

    return result;
}


boolean storage_device_disk_write(storage_device_t *storedev, byte* buffer, uint32 address, uint32 length) {
    uint32 start_sector = 0;
    SDCC_STATUS rc;
    struct sdcc_device *hsdev_handle =
        storedev->hsdev_partition_handles[storedev->current_open_partition];

    if (hsdev_handle == NULL)
        return FALSE;

    /*if (address & 0x1FF)
        return FALSE;
    if ((uint32)buffer & 0x3) {
        memcpy(aligned_buffer, buffer, length);
    }*/

    if (TRUE == storedev->sector_addresses_enabled)
        start_sector += (address);
    else
        start_sector += (address / BLOCK_SIZE);

    //FREEZE_WATCHDOG();
    rc = sdcc_handle_write (hsdev_handle, start_sector, buffer,
                            (length / BLOCK_SIZE) + (((length % BLOCK_SIZE) > 0) ? 1 : 0));
    if (rc != SDCC_NO_ERROR)
        return FALSE;

    return TRUE;
}

boolean storageDeviceSetExtras(storage_device_t *storedev,
                            const char* attribute_name,
                            const char* attribute_value) {
    boolean num_conversion;

    if (strcasecmp(attribute_name, "DRIVE4_SIZE_IN_KB") == 0) {
        storedev->extras.GPP_size[0] = stringToNumber(attribute_value, &num_conversion) * 2;
        if (FALSE == num_conversion) {
            storedev->extras.GPP_size[0] = 0;
            return FALSE;
        }
    }
    else if (strcasecmp(attribute_name, "DRIVE5_SIZE_IN_KB") == 0) {
        storedev->extras.GPP_size[1] = stringToNumber(attribute_value, &num_conversion) * 2;
        if (FALSE == num_conversion) {
            storedev->extras.GPP_size[1] = 0;
            return FALSE;
        }
    }
    else if (strcasecmp(attribute_name, "DRIVE6_SIZE_IN_KB") == 0) {
        storedev->extras.GPP_size[2] = stringToNumber(attribute_value, &num_conversion) * 2;
        if (FALSE == num_conversion) {
            storedev->extras.GPP_size[2] = 0;
            return FALSE;
        }
    }
    else if (strcasecmp(attribute_name, "DRIVE7_SIZE_IN_KB") == 0) {
        storedev->extras.GPP_size[3] = stringToNumber(attribute_value, &num_conversion) * 2;
        if (FALSE == num_conversion) {
            storedev->extras.GPP_size[3] = 0;
            return FALSE;
        }
    }
    else if (strcasecmp(attribute_name, "ENH_SIZE") == 0) {
        storedev->extras.ENH_size = stringToNumber(attribute_value, &num_conversion);
        if (FALSE == num_conversion) {
            storedev->extras.ENH_size = 0;
            return FALSE;
        }
    }
    else if (strcasecmp(attribute_name, "ENH_START_ADDR") == 0) {
        storedev->extras.ENH_start_addr = stringToNumber(attribute_value, &num_conversion);
        if (FALSE == num_conversion) {
            storedev->extras.ENH_start_addr = 0;
            return FALSE;
        }
    }
    else if (strcasecmp(attribute_name, "GPP_ENH_FLAG") == 0) {
        storedev->extras.GPP_enh_flag = stringToNumber(attribute_value, &num_conversion);
        if (FALSE == num_conversion) {
            storedev->extras.GPP_enh_flag = 0;
            return FALSE;
        }
    }
    else {
        return FALSE;
    }
    return TRUE;
}

void storageDeviceGetExtras(storage_device_t *storedev) {
    logMessage("DRIVE4_SIZE_IN_KB %d", storedev->extras.GPP_size[0] / 2);
    logMessage("DRIVE5_SIZE_IN_KB %d", storedev->extras.GPP_size[1] / 2);
    logMessage("DRIVE6_SIZE_IN_KB %d", storedev->extras.GPP_size[2] / 2);
    logMessage("DRIVE7_SIZE_IN_KB %d", storedev->extras.GPP_size[3] / 2);
    logMessage("ENH_SIZE %d", storedev->extras.ENH_size);
    logMessage("ENH_START_ADDR %d", storedev->extras.ENH_start_addr);
    logMessage("GPP_ENH_FLAG %d", storedev->extras.GPP_enh_flag);
}

boolean storageDeviceExtrasCommit(storage_device_t *storedev,
                                  int *errno) {
    SDCC_STATUS rc;
    //struct sdcc_device *hsdev_handle =
    //    storedev->hsdev_partition_handles[storedev->current_open_partition];

    rc = SDCC_NO_ERROR;
    logMessage("would have committed here");
    //rc = sdcc_config_emmc_gpp_enh (hsdev_handle, &(storedev->extras));
    if (rc != SDCC_NO_ERROR) {
        return FALSE;
    }
    return TRUE;
}

