/***********************************************************************
 * fs_blockdev.h
 *
 * Header file containing all the public interfaces for the blockdev API
 * Copyright (C) 2009-2010,2013,2014 QUALCOMM Technologies, Inc.
 *
 * The blockdev API contains all the necessary interfaces to communicate
 * with a block device at the lowest abstraction layer. This includes
 * the common functions such as opening, reading and writing to a block
 * device as well as specific management functions such as hiding a block
 * device from discovery and returning device data from a block device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2013-05-24   rp    Add UFS support to Hotplug.
2010-09-30   wek   Remove device iovec interface.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-12-23   vr    Return NULL if device is invalid in blockdev_get_name
2009-12-10   rp    Do sector translation in write protect function.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_BLOCKDEV_H__
#define __FS_BLOCKDEV_H__

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "comdef.h"

/**
 * @defgroup blockdev_module Block Device Driver
 * @ingroup efs
 *
 * The block device driver maintains a list of block devices which
 * abstracts the use of any block device through a common API.
 */

/**
 * @defgroup blockdev_api Block Device APIs
 * @ingroup blockdev_module
 *
 * The following API's provide uniform access to all block devices:
 *
 * Block Device I/O API
 * @par
 * \link ::blockdev_open \endlink \n
 * \link ::blockdev_close \endlink \n
 * \link ::blockdev_read \endlink \n
 * \link ::blockdev_write \endlink \n
 * \link ::blockdev_reliable_write \endlink \n
 * \link ::blockdev_uwrite \endlink \n
 * \link ::blockdev_erase \endlink \n
 * \link ::blockdev_reset \endlink \n
 * \link ::blockdev_is_present \endlink \n
 * \link ::blockdev_get_device_info \endlink \n
 * \link ::blockdev_device_format \endlink \n
 * \link ::blockdev_get_device_size \endlink \n
 * \link ::blockdev_set_device_idle \endlink \n
 *
 *
 * Block Device Management
 * @par
 * \link ::blockdev_find_by_type \endlink \n
 * \link ::blockdev_hide_device \endlink \n
 * \link ::blockdev_unhide_device \endlink \n
 * \link ::blockdev_get_name \endlink \n
 * \link ::blockdev_get_type \endlink \n
 */


/**
 * Anonymous (internal) structure defining a block device */
struct block_device;

/**
 * This is an exhaustive list of all recognized block device types.
 * These types shall be referenced when locating a block device using
 * blockdev_find_by_type. This must match hotplug_dev_types to maintain
 * interface compatibility */
typedef enum
  {
    BLOCKDEV_TYPE_INVALID = 0, /**< Invalid device */
    BLOCKDEV_TYPE_MMC     = 1, /**< SD or MMC */
    BLOCKDEV_TYPE_REMOTE  = 2, /**< Device hosted on other processor */
    BLOCKDEV_TYPE_USB_MS  = 3, /**< USB Host Mass Storage (thumbdrive) */
    BLOCKDEV_TYPE_DEVNULL = 4, /**< NULL (nop) device */
    BLOCKDEV_TYPE_FTL     = 5, /**< Flash Translation Layer (FAT-on-NAND) */
    BLOCKDEV_TYPE_SD_CPRM = 6, /**< Protected area of SD card (for CPRM) */
    BLOCKDEV_TYPE_UFS     = 7, /**< UFS */
    BLOCKDEV_TYPE_ALL     = 8, /**< All the above devices */
  } block_device_type;

/**
 * The device specific information of a block device.
 * This structure is populated by blockdev_get_device_info().
 *
 * @Note that the device specific information may not be available for
 * all device types.
 */
struct block_device_info
{
  uint32   manufacturer_id;
  uint32   oem_id;
  uint32   prod_rev;
  uint32   prod_serial_num;
  uint32   card_size_in_sectors;
  uint32   bytes_per_sector;
  uint32   phy_partition_type;
  char     product_name[16];
  char     manufactured_date[16];
};

/**
 * Initializes the block device module. This MUST be called before
 * any block device API is used. This should only be called once
 * on initialization.
 */
void blockdev_init (void);



/*****************************************************
 *    B L O C K   D E V I C E   M A N A G E M E N T
 ****************************************************/

/**
 * Locate a block device given a mount path. This function searches
 * through the device list and returns the device existing at
 * the specified mount point. This function will ignore any
 * hidden flag set in the block device.
 *
 * @param path
 *   Mount point where the block device exists
 *
 * @return
 *   Returns persistent handle to the underlying block device to
 *   be used for further blockdev calls.
 */
struct block_device * blockdev_find_by_path (char *path);

/**
 * Locate a block device of the specified type. Upon first calling 'last'
 * should be given as NULL and the first block device of type 'type' will
 * be returned. On further calls 'last' should be specified as the last
 * block device returned. In doing this one can iterate through all block
 * devices of a specified type.
 * Note:
 * 1. Block devices are not ensured to be returned in a specific order.
 * 2. By specifying BLOCKDEV_TYPE_ALL all devices can be enumerated.
 *
 * @param type
 *   The type of block device requested or BLOCKDEV_TYPE_ALL for any.
 *
 * @param last
 *   The previous block device returned or NULL on the first invocation.
 *
 * @return
 *   Returns persistent handle to the underlying block device to
 *   be used for further blockdev calls.
 */
struct block_device * blockdev_find_by_type (block_device_type type,
                                             struct block_device *last);
/**
 * Sets the hidden property for the given block device.
 * When this field is set any of the blockdev enumeration functions
 * will skip over the device. blockdev_find_by_path is the only function
 * that may be used to locate a block device that is hidden.
 *
 * @param bdev
 *   This is a pointer to a block device, usually returned from
 *   one of the 'find' functions above.
 *
 * @return
 *   Returns the previous hidden state.
 *   1 - Hidden  0 - Not hidden
 */
int blockdev_hide_device (struct block_device *bdev);

/**
 * Clears the hidden property for the given block device. Once cleared this
 * device will be enumerable by both blockdev_get_next_device and
 * blockdev_find_by_type.
 *
 * @param bdev
 *   This is a pointer to a block device, usually returned from
 *   one of the 'find' functions above.
 *
 * @return
 *   Returns the previous hidden state.
 *   1 - Hidden  0 - Not hidden
 */
int blockdev_unhide_device (struct block_device *bdev);

/**
 * Return the name of the supplied block device. Currently this
 * is the path in the filesystem where that block device resides.
 *
 * @param bdev
 *   This is a pointer to a block device, usually returned from
 *   one of the 'find' functions above.
 *
 * @return
 *   A pointer to the name of the given device. If bdev pointer
 *   is invalid, returns NULL.
 */
const char * blockdev_get_name (struct block_device *bdev);

/**
 * Return the type (block_device_type) of the supplied block device.
 *
 * @param bdev
 *   This is a pointer to a block device, usually returned from
 *   one of the 'find' functions above.
 *
 * @return
 *   Enum element that corresponds to the given devices type.
 */
block_device_type blockdev_get_type (struct block_device *bdev);

/**
 * Return the unique device id of the specified block device.
 *
 * @param bdev
 *   This is a pointer to a block device, usually returned from
 *   one of the 'find' functions above.
 *
 * @return
 *   Unique device id for the given block device.
 */
uint16 blockdev_get_device_id (struct block_device *bdev);



/*************************************
 *     B L O C K  D E V I C E  I / O
 *************************************/

/**
 * Open a block device for use. This should be called on a block device
 * before any other APIs are used.
 *
 * @param bdev
 *   Block device to be opened. Can be located using one of the
 *   blockdev_find_xxx functions.
 *
 * @param data
 *   Optional data to be passed to device driver on open. Data is
 *   specific to device driver implementation.
 *
 * @return
 *   Return 0 on success, -1 on failure.
 */
int blockdev_open (struct block_device *bdev, void **handle, uint32 data);

/**
 * Close a block device when finished using it. To be called to cleanup
 * after all device I/O is performed. Behavior is dependent on the underlying
 * block device.
 *
 * @param bdev
 *   Block device to be closed. Can be located using one of the
 *   blockdev_find_xxx functions.
 *
 * @return
 *   Returns 0 on success, -1 on failure.
 */
int blockdev_close (struct block_device *bdev, void *handle);

/**
 * Read attempts to read up to 'block_count' number of blocks starting
 * from 'lba' from the open device referred by 'bdev' into the buffer
 * starting at 'buf'
 *
 * @param bdev
 *   Block device to be read from. Can be located using one of the
 *   blockdev_find_xxx functions.
 *
 * @param lba
 *   Start reading from this block
 *
 * @param buf
 *   Buffer to store read data
 *
 * @param block_cnt
 *   Number of blocks to read
 *
 * @return
 *  Returns 0 for success, non-zero indicates a failure.
 */
int blockdev_read (struct block_device *bdev, void *handle, uint32 lba,
                   unsigned char *buf, uint16 block_cnt);

/**
 * Write attempts to write up to 'block_count' number of blocks starting
 * from 'lba' to the open device referred by 'hdev' from the buffer starting
 * at 'buf'
 *
 * @param bdev
 *   Block device to be written to. Can be located using one of the
 *   blockdev_find_xxx functions.
 *
 * @param lba
 *    Start writing from this block
 *
 * @param buf
 *    Buffer that holds the data to be written
 *
 * @param block_cnt
 *    Number of blocks to write
 *
 * @return
 *  Returns 0 for success, non-zero indicates a failure.
 */
int blockdev_write (struct block_device *bdev, void *handle, uint32 lba,
                    unsigned char *buf, uint16 block_cnt);


/**
 * blockdev_uwrite attempts to write up to 'block_count' number of blocks,
 * starting at 'lba', of 'User Data' to the open device referred by 'bdev'
 * from the buffer starting at 'buf'.
 *
 * Some devices (eg: CPRM) must distinguish between filesystem metadata
 * (directory entries, accounting structures, etc) and "User Data" or
 * stored payload that is subject to encryption or special processing.
 * This function is used to write "user data" that is determined to
 * have originated from the application, and not the filesystem
 * itself.  For most devices, this is handled identical to
 * blockdev_write().
 *
 * @param bdev
 *    This is a pointer to a block device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @param lba
 *    Start writing from this block
 *
 * @param buf
 *    Buffer that holds the user data to be written
 *
 * @param block_cnt
 *    Number of blocks to write
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * @see ::blockdev_write
 */
int blockdev_uwrite (struct block_device *bdev, void *handle, uint32 lba,
                     unsigned char *buf, uint16 block_cnt);

/**
 * This is very similar to write except when available it ensures that
 * the given write will either be completely committed into memory or
 * none of the data will be committed into memory.
 *
 * This API is only available on some MMC/SD media.
 *
 * @param bdev
 *    This is a pointer to a block device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @param lba
 *    Start writing from this block
 *
 * @param buf
 *    Buffer that holds the user data to be written
 *
 * @param block_cnt
 *    Number of blocks to write
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * @see ::blockdev_write
*/
int blockdev_reliable_write (struct block_device *bdev, void *handle,
                             uint32 lba, uint32 *buf, uint16 block_cnt);

/**
 * Erase attempts to erase up to 'block_count' number of blocks, starting at
 * 'lba' from an open device referred by 'bdev'
 *
 * Some devices may support a "block erase" function.  This erases a
 * span of blocks more quickly than writing 0s to them.  For most
 * devices, this is unsupported and will always return a failure.

 * @param bdev
 *   Block device to be erased. Can be located using one of the
 *   blockdev_find_xxx functions.
 *
 * @param lba
 *   Start erasing from this block
 *
 * @param block_cnt
 *   Number of blocks to erase
 *
 * @return
 *   Returns 0 for success, -1 for failure.
 */
int blockdev_erase (struct block_device *bdev, void *handle,
                    uint32 lba, uint16 block_cnt);


/**
 * This function checks if the block device is present.
 *
 * If the media is present, it means that block read or
 * write calls are available.
 *
 * @param bdev
 *   Block device to query. Can be located using one of the
 *   blockdev_find_xxx functions.
 *
 * @return
 *    Returns 1 if media in this device is present
 *    0 otherwise
 *
 * @Note
 *    This information is dynamic, so applications should allow for the
 *    possibility that media was removed (or inserted) immediately after
 *    this call returned.
 */
int blockdev_is_present (struct block_device *bdev, void *handle);


/**
 * Gets the size (number of blocks and size of each block) for a given device.
 *
 * @param bdev
 *    This is a pointer to a block device, usually returned from
 *    one of the 'find' functions above.
 *
 * @param block_count
 *    How many blocks (sectors) are available on this media. This function will
 *    write to this variable.
 *
 * @param bytes_per_block
 *    All blocks are a uniform size of this many bytes. This function will
 *    write to this variable.
 *
 * @return
 *    Returns 0 on success and -1 on failure
 *    'block_count' and 'bytes_per_block' are populated with device specific
 *    numbers on success
 */
int blockdev_get_device_size (struct block_device *bdev, void *handle,
                              uint32 *block_count, uint16 *bytes_per_block);


/**
 * Gets extended info from a device. If available this function will
 * populate 'block_device_info *' with information specific
 * to 'bdev'. See structure definition above for specifics of
 * information available.
 *
 * @param bdev
 *   This is a pointer to a block device, usually returned from
 *   one of the 'find' functions above.
 *
 * @param info
 *   This structure will be populated with device specifics if
 *   the function succeeds.
 *
 * @return
 *    Returns 0 on success and -1 on failure
 */
int blockdev_get_device_info (struct block_device *bdev, void *handle,
                              struct block_device_info *info);

/**
 * Performs a low level format on the block device. This is completely
 * independent from higher level formatting such as FAT or NTFS. This
 * low level format ensures consistency on the device.
 *
 * @param bdev
 *   This is a pointer to a block device, usually returned from
 *   one of the 'find' functions above.
 *
 * @return
 *   Returns 0 on success and -1 on failure.
 */
int blockdev_device_format (struct block_device *bdev, void *handle);

/**
 * Set a device as idle to be put into a low-power mode.
 * This function signals down to the lower device driver that
 * the application will not be using the device which will result
 * in power savings if implemented. Not available on all block devices,
 * currently only implemented for SD.
 *
 * @param bdev
 *   This is a pointer to a block device, usually returned from
 *   one of the 'find' functions above.
 *
 * @return
 *   Returns 0 on success and -1 on failure
 */
int blockdev_set_device_idle (struct block_device *bdev, void *handle);



/**
 * Resets the block device. Behavior is device dependent.
 *
 * @param bdev
 *   This is a pointer to a block device, usually returned from
 *   one of the 'find' functions above.
 *
 * @return
 *   Returns 0 on success and -1 on failure.
 */
int blockdev_reset (struct block_device *bdev, void *handle);

uint32 blockdev_get_entire_device_id (struct block_device *bdev, void *handle);

int blockdev_is_embedded_device (struct block_device *bdev, void *handle);

int blockdev_set_sps_end_point (struct block_device *bdev, void *handle,
                               void *param);
int blockdev_get_sps_end_point (struct block_device *bdev, void *handle,
                               void *param);

int blockdev_set_power_on_write_protection (struct block_device *bdev,
                          void *handle, uint32 lba, uint32 block_cnt);


#else

static inline void
blockdev_init (void)
{
}


#endif /* !FEATURE_FS_HOTPLUG */

#endif /* not __FS_BLOCKDEV_H__ */
