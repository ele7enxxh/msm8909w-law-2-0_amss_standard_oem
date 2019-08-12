/***********************************************************************
 * fs_hotplug.h
 *
 * Hotplug module.
 * Copyright (C) 2009-2013 QUALCOMM Technologies, Inc.
 *
 * The Hotplug module responds to events indicating insertion and removal
 * of removable media.  It is also responsible for periodic polling of
 * devices that do not self detect.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/storage/fs_hotplug.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-10   rp    Hotplug to poll only devices of interest.
2013-06-21   rp    Adapt to the new UFS write-protection API.
2013-05-24   rp    Add UFS support to Hotplug.
2013-05-24   wek   Make GUID parameter as constant.
2012-07-30   nr    Fix GPT based FAT mount during boot.
2012-04-09   dks   Support storing/reading cookies to/from emmc partitions.
2012-03-20   wek   Cleanup customer header file inclusion.
2011-04-29   wek   Update minor header version, due introduciton of gpt api.
2011-03-31   wek   Implement GPT/MBR partition table parser.
2010-11-16   dks   Added HOTPLUG_PARTITION_ID_SSD.
2010-11-16   dks   Added additional Hoptlug partition IDs for 9k on fusion.
2010-09-29   dks   Moved hotplug configurable variables to fs_hotplug_config*.h
2010-07-12   rp    Added defines for major and minor number for API files.
2010-05-07   wek   Add Partitoin ID for File system Backup.
2010-03-03   yog   Unified IDs for both OEMSBL and OSBL Boot partitions.
2010-02-24   rp    Added hotplug_open_device_by_partition_type API.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-12-15   wek   Added partition ID for ADSP.
2009-12-10   rp    Make 0 to specify whole partition on write-protect.
2009-10-28   vr    Added get_driveno and get_handle apis.
2009-08-10   wek   Fix comments for doxygen.
2009-05-15   ebb   Updated #ifdefs to avoid mismatched signatures in stubs.
2009-05-08   ebb   Added inline stubs for unavailable hotplug functions.
2008-12-16   yg    Modifed device info structure.
2008-12-12   rp    Introducing i/o-vec interface.
2008-11-24   yog   Added hotplug_set_idle_device_event_duration().
2008-10-17   ebb   Updated mode flags for hotplug_checkfat.
2008-09-16   yog   Remove trailing commas from the enum declaration.
2008-09-15   yg    Updated comments for CheckFAT about task.
2008-05-30   ebb   Added callback to hotplug_checkfat.
2008-05-29   ebb   Added checkfat flags.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2008-05-05   ebb   Added hotplug_checkfat interface.
2008-02-19   rp    Documentation: changed from @retval to @returns.
2008-02-04   sch   Corrected documentation mistake in dev read/write calls
2008-01-10   sch   Improved hotplug format and device API comments
2008-01-09   yg    Update documentation for hotplug_init, find_dev_by_path.
2007-12-18   rp    Improved documentation of the registration APIs.
2007-06-09   rp    Doxygen comments markup.
2007-04-08   s h   Clarify that dev_get_path needs a valid hdev.
2007-04-05   s h   Comment and cleanup.
2007-04-02   rp    Added hotplug event notification.
2007-03-14   s h   Improve hotplug device API comments.
2006-11-09   sch   Added CPRM specific definitions
2006-10-16   s h   hotplug_set_polling_interval returns prior value.
2006-10-16   s h   Change lock/unlock functions to return int status.
2006-10-10   s h   Export hotplug_find_dev_by_type() as public.
2006-09-20   s h   Export hotplug_find_dev_by_path() as public.
2006-05-16   s h   Added hotplug_dev_is_mounted()
2006-05-11   s h   Renamed efs_hotplug_... to hotplug_...
2006-05-10   s h   Removed mount/unmount access functions
2006-05-10   s h   Replaced DRV_GEOMETRY_DESC with block size & count
2006-05-10   s h   Added device access functions
2006-05-05   s h   Brought in DEFAULT_POLL_INTERVAL_MS
2006-05-03   s h   Made lock_card device-specific
2006-04-22   s h   Moved all private definitions into fs_hotplug_i.h
2006-04-17   s h   Improved comments
2006-04-17   s h   Added external prototypes for lock/unlock card.
2006-04-13   s h   Code review feedback implemented.
2006-04-13   s h   Allow compilation without USB features
2006-03-10   s h   Added USB CLOSED state
2006-03-10   sch   Added USB specific functionality
2005-09-20   dlb   Create

===========================================================================*/

#ifndef __FS_HOTPLUG_H__
#define __FS_HOTPLUG_H__

#include "comdef.h"
#include "fs_hotplug_config.h"

/** Identifies the major version number of this API file. */
#define VER_FS_HOTPLUG_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_HOTPLUG_H_MINOR     2

/*************************************************************************//**
 * @defgroup hotplug_module Hotplug Module
 *
 * Hotplug supports removable media (MMC/SD, USB Mass storage) through the
 * notion of a hotplug moudle.  This module keeps track of devices in the
 * system, and performs auto mounting and unmounting of the appropriate
 * mountpoints, so that these devices appear in the filesystem namespace.
 *
 * Some devices, such as USB mass storage, are able to notify us on
 * significant events, such as enumeration and device removal.
 * Automounting these devices is merely a matter of responding to the
 * appropriate events, and mounting or unmounting the device at the
 * appropriate time.
 *
 * Other devices, such as MMC/SD do not have a consistent way of detecting
 * card insertion.  A design may implement card detection with DAT3, power
 * drain detection, or a physical insertion switch.  The design may also
 * opt to do only software polling.  This module needs to be flexible to
 * these differing needs.
 *
 * Because power consumption is a significant concern in a mobile device,
 * the hotplug module provides calls to enable and disable this periodic
 * polling.  Some suggestions are to enable and disable polling in a
 * similar manner to that used for the backlight.  When the user isn't
 * pressing keys, polling can be disabled.
 *
 * The entire hotplug module is featurized on @b FEATURE_FS_HOTPLUG.
 *
 *
*****************************************************************************/


/*************************************************************************//**
 * @defgroup hotplug_config Hotplug Configuration Options
 *  @ingroup hotplug_module
 * \n \n
 * Targets can override these values in the cust files as needed.
 *
 * \link ::FS_HOTPLUG_DEFAULT_POLL_INTERVAL_MS Poll Interval \endlink \n
 * \link ::HOTPLUG_NOTIFY_MAX_CLIENTS Max Registration clients
 * \endlink \n
 * \link ::hotplug_set_polling_interval Adjust Poll Interval at runtime
 * \endlink \n
 *
*****************************************************************************/

/*************************************************************************//**
 * @defgroup hotplug_iterate Hotplug Device Iterate functions
 *  @ingroup hotplug_module
 * \n \n
 * These functions can be used to iterate through all the devices managed
 * by the hotplug module. Hotplug now supports the following types of
 * iteration:
 * 1. Iterate through all the devices.
 * 2. Iterate through all the Hardware-partitions.
 * 3. Iterate through all the Soft-partitions.
 * 4. Iterate through only the FAT partitions.
 *
 * And also while iterating, we can also specify to iterate only the
 * embedded-media or external-media or both.
 *
 * \link ::hotplug_iter_open hotplug_iter_open \endlink \n
 * \link ::hotplug_iter_get_next hotplug_iter_get_next \endlink \n
 * \link ::hotplug_iter_close hotplug_iter_close \endlink \n
 *
*****************************************************************************/

/*************************************************************************//**
 * @defgroup hotplug_device_access Hotplug Device Access functions
 *  @ingroup hotplug_module
 * \n \n
 * These functions can be used to obtain an hotplug device (hdev)
 * from the hotplug module.
 *
 * \link ::hotplug_open_device hotplug_open_device \endlink \n
 * \link ::hotplug_close_device hotplug_close_device \endlink \n
 * \link ::hotplug_open_device_by_partition_type
 *            hotplug_open_device_by_partition_type \endlink \n
 *
*****************************************************************************/


/*************************************************************************//**
 * @defgroup hotplug_device Hotplug Device functions
 *  @ingroup hotplug_module
 * \n \n
 *
 * Once a hotplug 'hdev' handle is retrieved, it can be passed
 * to these functions to perform direct device access.
 *
 * In most cases, these calls (writes in particular) should be avoided
 * or used only on 'locked' devices so that direct device access does
 * not interfere with use by the FAT filesystem.  Additional
 * coordination may be required if there are multiple device users.
 * These "raw" device calls are to be used carefully.
 *
 * Each of these calls is passed to the appropriate Hotplug Device
 * layer (SD, USB, etc) for handling specific to that device.
 *
 * The common parameters are as follows:
 *
 * @param hdev
 *    This is a pointer to a Hotplug device returned from the
 *    hotplug_open_device function above. This represents the persistent
 *    device handle.
 *
 * @param lba
 *    the Linear Block Address, starting from zero.  This is the number of
 *    the first sector that will be read or written.
 *
 * @param buf
 *    The data buffer to read or write.  In most cases, this buffer should be
 *    located in "DMA-aligned" memory for maximum performance. (Some devices
 *    employ Data Mover hardware that benefits from specific address alignment
 *    in physical memory.)
 *
 * @param block_count
 *    The number of blocks (sectors) to read or write. (A nonsensical 0 value
 *    is not permitted, and may fail or succeed, depending on the device.)
 *
 * @return
 * These functions all return 0 for SUCCESS.
 * Any non-zero value indicates FAILURE, and the specific failure
 * values are device-specific.

 * These are the device access functions provided by hotplug module:
 * @par
 * \link ::hotplug_open_device hotplug_open_device \endlink \n
 * \link ::hotplug_close_device hotplug_close_device \endlink \n
 * \link ::hotplug_open_device_by_partition_type
 *                hotplug_open_device_by_partition_type \endlink \n
 * \link ::hotplug_dev_read hotplug_dev_read \endlink \n
 * \link ::hotplug_dev_write hotplug_dev_write \endlink \n
 * \link ::hotplug_dev_write_udata hotplug_dev_write_udata \endlink \n
 * \link ::hotplug_dev_erase hotplug_dev_erase \endlink \n
 * \link ::hotplug_dev_get_size hotplug_dev_get_size \endlink \n
 * \link ::hotplug_dev_is_present hotplug_dev_is_present \endlink \n
 * \link ::hotplug_dev_is_mounted hotplug_dev_is_mounted \endlink \n
 * \link ::hotplug_dev_get_state hotplug_dev_get_state \endlink \n
 *
*****************************************************************************/


/*************************************************************************//**
 * @defgroup hotplug_notification Hotplug Notification
 *  @ingroup hotplug_module
 * \n \n
 * @b Detailed @b Description \n \n
 * The hotplug module can optionally provide system events like "Card-Inserted"
 * or "Card-Removed" etc. to another AMSS task. In order to receive the events,
 * the user will register with the hotplug module indicating what types of
 * events they would like to receive about a device managed by hotplug. The
 * APIs to register and unregister are declared in the public header file
 * fs_hotplug.h
 *
 * \link ::hotplug_dev_type Hotplug Device Types \endlink \n
 * Hotplug manages several different types of media, both removable and
 * permanent. Tasks can nominate interest in one or all of these devices by
 * registering to receive events from any of these device types.  Each type
 * has an enumerated name declared, and the special type "HOTPLUG_TYPE_ALL"
 * represents a wildcard type, including all of the devices managed by Hotplug.
 *
 *
 * \link ::HOTPLUG_NOTIFY_EVENT_CARD_INSERTED Hotplug Events \endlink \n
 * The users can register for any number or combination of the these events by
 * OR'ing them together and forming an event_mask to supply to the registration
 * API.

 * \link ::hotplug_notify_event_cb Registration Callback function \endlink \n
 * The client's callback function should conform to the above prototype. The
 * hotplug module will call this function whenever it has any event for a
 * device that the client has registered for.
 * @Note The callback functions must perform the minimum amount of processing,
 * and operate as if they are in interrupt context.  Each callback should do no
 * more than record simple state information and set signals, returning as soon
 * as possible. Specifically, calls to other libraries like clock timers and
 * queue functions are prohibited.  No blocking functions are allowed.
 * Extensive computation is to be avoided.
 * The callback functions could be invoked from any task with unknown stack
 * size or priority so callback functions should make every effort to be quick
 * and small.
 *
 *
 * \link ::hotplug_notify_register Registration \endlink \n
 * This registration API allows clients to register with the hotplug module to
 * receive notifications for any set of events for any type of device. After
 * successful registration the clients will receive event notifications from
 * the hotplug module through their callback function.
 *
 *
 * \link ::hotplug_notify_unregister Unregistration \endlink \n
 * The above API would allow the clients to unregister from the hotplug module.
 * After successful unregistration the clients will no longer receive event
 * notifications from the hotplug module.
*****************************************************************************/

/** The hotplug device is an internal (private) data structure.
 * Here is an anonymous declaration so we can talk about pointers to
 * this structure.  */
struct hotplug_device;


/*************************************************************************//**
 * Set the frequency at which Hotplug task will poll ALL devices for changes.
 *
 * @param milliseconds
 *    Amount of time (excluding the detection time) between consecutive hotplug
 *    polls\n
 *
 *    If the value is too small then the task will run frequently, polling the
 *    interface for the presence of media too often resulting in more CPU
 *    usage.\n
 *    If the value is too big then the phone will be sluggish in detecting
 *    new media.\n
 *    This value should be set to 0 to inhibit polling entirely, perhaps
 *    for sleep or low-current scenarios.\n
 *    A poll interval of two to five seconds is reasonable.
 *
 * @return
 *    Returns the previous value of the polling interval.
 */
unsigned int hotplug_set_polling_interval (unsigned int milliseconds);

/**
 * @deprecated
 * Since we did release one VU that called it by this unfortunate
 * name, add a define to make migration easy.
 * This name is DEPRECATED. */
#define efs_hotplug_set_polling_interval(x) hotplug_set_polling_interval(x)


/****************************************************************************
 *  Reserved list of partition types.
 *
 *  When adding new partiton IDs consider growing the list below and using
 *  unpopular partition IDs. Some unpopular ID ranges are (both inclusive):
 *  [0x2C, 0x30], [0x58, 0x5B], [0x5D, 0x60], [0x6A, 0x6F], [0x70, 0x7D].
 ****************************************************************************/
#define HOTPLUG_PARTITION_ID_BOOT_QCSBL               0x45
/* As both OSBL and OEMSBL are mutually exclusive, thus have the unified
 * IDs 0x46 for both OSBL and OEMSBL */
#define HOTPLUG_PARTITION_ID_BOOT_OEMSBL              0x46
#define HOTPLUG_PARTITION_ID_BOOT_OSBL                0x46
#define HOTPLUG_PARTITION_ID_BOOT_APPSSBL             0x47
#define HOTPLUG_PARTITION_ID_BOOT_APPS                0x48
#define HOTPLUG_PARTITION_ID_BOOT_MODEM               0x49
#define HOTPLUG_PARTITION_ID_BOOT_MODEM_FS1           0x4A
#define HOTPLUG_PARTITION_ID_BOOT_MODEM_FS2           0x4B
#define HOTPLUG_PARTITION_ID_BOOT_FOTA                0x4C
#define HOTPLUG_PARTITION_ID_BOOT_QCSBLHD_CFG_DATA    0x4D
#define HOTPLUG_PARTITION_ID_MNAND                    0x4E
#define HOTPLUG_PARTITION_ID_ADSP                     0x50
#define HOTPLUG_PARTITION_ID_BOOT_MODEM_FS_BKP        0x58
#define HOTPLUG_PARTITION_ID_Q6_FS1                   0x59
#define HOTPLUG_PARTITION_ID_Q6_FS2                   0x5A
#define HOTPLUG_PARTITION_ID_Q6_FSG                   0x5B
#define HOTPLUG_PARTITION_ID_Q6_FSC                   0x5C
#define HOTPLUG_PARTITION_ID_SSD                      0x5D
#define HOTPLUG_PARTITION_ID_BOOT_MODEM_FSC           0x5E
/*************************************************************************//**
 * @par Name:
 *   hotplug_init - Initialize the hotplug module.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    void hotplug_init (void);
 *  @endcode
 *
 * @par Description:
 *    Initialize the hotplug module, and start its task. This must be called
 *    once during startup. EFS2 must be initialized first. EFS currently calls
 *    this after its own startup.
*****************************************************************************/
void hotplug_init (void);

/*************************************************************************//**
 * @par Name:
 *   hotplug_init_no_poll - Initialize the hotplug module.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    void hotplug_init_no_poll (void);
 *  @endcode
 *
 * @par Description:
 *    Initialize the hotplug module, and does not poll any hotplug-device.
*****************************************************************************/
void hotplug_init_no_poll (void);


/*************************************************************************//**
 * Formats the media referred by mount point with a fresh filesystem,
 * completely DESTROYING the previous contents.
 *
 * This function also determines which file system is appropriate for
 * the size of the device (FAT16 or FAT32) and formats accordingly.
 * The device is locked for file system operations during formatting. It
 * can still be accessed using direct device calls. It is advisable to
 * lock the device before calling this function to avoid file system
 * inconsistencies.
 *
 * @param dirname
 * The provided pathname is the directory under which the media would
 * normally be mounted by Hotplug (eg: /mmc1 )
 *
 * @return
 *    Return 0 on success, -1 on failure.
 *
 * see also: ::hotplug_format_dev
*****************************************************************************/
int hotplug_format (const char *dirname);

/*************************************************************************//**
 * Formats the media referred by hotplug device instance with a fresh
 * filesystem, completely DESTROYING the previous contents.
 *
 * This function also determines which file system is appropriate for
 * the size of the device (FAT16 or FAT32) and formats accordingly.
 * The device is locked for file system operations during formatting. It
 * can still be accessed using direct device calls. It is advisable to
 * lock the device before calling this function to avoid file system
 * inconsistencies.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    Return 0 on success, -1 on failure.
 *
 * see also: ::hotplug_format
*****************************************************************************/
int hotplug_format_dev (struct hotplug_device *hdev);


/*************************************************************************//**
 * Lock all media access on one device/partition.  While media access is
 * "locked", hotplug will stay off the device/partition and ignore any media.
 * Hotplug will forcibly unmount the device and disable polling, so
 * the device will not be auto-mounted.
 *
 * The device/partition remains useable through direct read/write calls, but
 * Hotplug itself will not attempt to (FAT) mount it.
 *
 * Usage:
 * This is used during Cardreader mode, CPRM, and other
 * exclusive-access applications.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device/partition, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    Return 0 on success, -1 on failure.
 *
 * This function can fail if the device is already locked or if hotplug
 * is unable to unmount or close the media
*****************************************************************************/
int hotplug_lock_dev (struct hotplug_device *hdev);

/*************************************************************************//**
 * Unlock a previously locked media. After unlock, hotplug will immediately
 * force a poll, and in this poll the hotplug will try to mount the device.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    Return 0 on success, -1 on failure.
 *
 * This function can fail if the device is NOT locked
*****************************************************************************/
int hotplug_unlock_dev (struct hotplug_device *hdev);


/**
 * @deprecated
 *    Old names for the above hotplug_lock_dev (DEPRECATED, Do not use!) */
void fs_sfat_lock_card (void);

/**
 * @deprecated
 * Old names for the above hotplug_unlock_dev (DEPRECATED, Do not use!) */
void fs_sfat_unlock_card (void);

/**
 * DEPRECATED : PLEASE DO NOT USE THIS FUNCTION.
 *
 * This function returns a device entry by index/"drive ID".
 *
 * @param drive_id
 *    The hotplug drive ID is an arbitrary number associated with a
 *    particular hotplug device. It's impossible for an application to
 *    predict which device is associated with a particular hotplug drive ID,
 *    so it's best to use this only when a drive ID is derived from
 *    an hdev handle during runtime.
 *
 * @return
 *    Returns NULL if no device was found with that drive ID
 *            or hotplug device entry referred by drive ID
 *
 * @deprecated
 *    This API is deprecated. Please do no use it for new designs.
 */
struct hotplug_device *hotplug_hdev (int16 drive_id);

/*-----------------------------------------------------------------------------
          H O T P L U G   I T E R A T O R S
-----------------------------------------------------------------------------*/

/**
 * Each Hotplug device has a "type" that identifies the specific
 * device handler for this particular device.
 *
 * The ALL type is a wildcard that matches any type of media.
 */
typedef enum
{
  HOTPLUG_TYPE_INVALID = 0, /**< Invalid device */
  HOTPLUG_TYPE_MMC     = 1, /**< SD or MMC */
  HOTPLUG_TYPE_REMOTE  = 2, /**< Device hosted on other processor */
  HOTPLUG_TYPE_USB_MS  = 3, /**< USB Host Mass Storage (thumdrive) */
  HOTPLUG_TYPE_DEVNULL = 4, /**< NULL (nop) device */
  HOTPLUG_TYPE_FTL     = 5, /**< Flash Translation Layer (FAT-on-NAND) */
  HOTPLUG_TYPE_SD_CPRM = 6, /**< Protected area of SD card (for CPRM) */
  HOTPLUG_TYPE_UFS     = 7, /**< UFS */
  HOTPLUG_TYPE_ALL     = 8  /**< All the above devices */
}hotplug_dev_type;

/**
 * DEPRECATED : PLEASE DO NOT USE THIS FUNCTION.
 * Please use the ::hotplug_iter_open() instead.
 *
 * This function returns a handle to the next device of this "type", or the
 * first one of this type if the value of last == NULL.
 * It returns 'NULL' when there are no more devices of that type.
 *
 * Usage scenario:
 * This could be used to iterate through all devices of TYPE_MMC, for
 * example.  It's generally called with 'NULL' the first time, and
 * then each returned hdev pointer is passed back in to find the next
 * one.
 *
 * @b Example:
 * @code
 *    first = hotplug_find_dev_by_type (HOTPLUG_TYPE_MMC, NULL);
 *    hdev = hotplug_find_dev_by_type (HOTPLUG_TYPE_MMC, first);
 * @endcode
 *
 * @return
 *    Returns the next device of type "type" or NULL if no such device
 *    is found
 *
 * @deprecated
 *    This API is deprecated. Please do no use it for new designs.
 */
struct hotplug_device *
hotplug_find_dev_by_type (hotplug_dev_type type,
                          struct hotplug_device *last);

/**
 * DEPRECATED : PLEASE DO NOT USE THIS FUNCTION.
 * Please use the ::hotplug_iter_open() instead.
 *
 * @par Name:
 *   hotplug_find_dev_by_path - Get the device handle by path name.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    struct hotplug_device * hotplug_find_dev_by_path (const char *path);
 *  @endcode
 *
 * @par Description:
 *   Find a device and get its handle, that would be mounted
 *   under this pathname.
 * @b Example:
 * @code
 *    hotplug_find_dev_by_path ("/mmc1");
 * @endcode
 *
 * @param path
 *   The path is required, NULL terminated string and the leading slash is
 *   optional.
 *
 * @return
 *   Returns the hdev handle, or NULL if no device exists with this name.
 *
 * @deprecated
 *    This API is deprecated. Please do no use it for new designs.
 */
struct hotplug_device *
hotplug_find_dev_by_path (const char *path);



/**
 * Iterate devices, hard-partitons, soft-partitions or potential FAT-partitons.
 */
enum hotplug_iter_type
{
  HOTPLUG_ITER_DEVICES          = 0,
  HOTPLUG_ITER_HARD_PARTITIONS  = 1,
  HOTPLUG_ITER_SOFT_PARTITIONS  = 2,
  HOTPLUG_ITER_FAT_PARTITIONS   = 3
};

/**
 * Removable media, or media permenantly fixed to the phone.
 * The ALL type can be used to match both.
 */
enum hotplug_iter_device_location
{
  HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY      = 0,
  HOTPLUG_ITER_EXTERNAL_DEVICES_ONLY      = 1,
  HOTPLUG_ITER_ALL_DEVICES                = 2
};

/**
 * Iterator record returned once for every hotplug_iter_get_next() cal.
 */
struct hotplug_iter_record
{
  char                hdev_name[FS_HOTPLUG_NAME_MAX]; /**< Name */
  hotplug_dev_type    device_type; /**<  Type of device, MMC, FTL etc. */
  int                 is_fat_partition; /**< 1 -> FAT partition, 0 -> No */
  uint32              boot_record_partition_type; /**< from boot record,
                      0 if 32 bits isn't enough to hold the partition type. */
  uint8               bootable; /**< is this a bootable type partition. */
  uint8               is_hard_partition; /**< 1 -> Hard-partition, 0 -> No */
  int                 is_mounted; /**< 1 -> Mounted with EFS2 file system*/
  int                 is_formatting; /**< is currently being formatted */
  int                 is_locked;  /**< Is is locked? */
  int                 is_embedded_device; /**< 1 -> Embedded, 0 -> External */
};

/*************************************************************************//**
 * This function opens an iterator of this "device type", this type of
 * partitions and internal or external device.
 *
 * Usage scenario:
 * This could be used to iterate through all hard-partitions of TYPE_MMC on
 * internal-cards. Using the returned iterator handle, calling
 * hotplug_iter_get_next() will return the next hdev_name that is matching
 * the supplied iterator criteria.
 *
 * @b Example:
 * @code
 *    struct hotplug_iter *hiter;
 *    struct hotplug_iter_record *hiter_rec;
 *    struct hotplug_device *hdev;
 *
 *    hiter = hotplug_iter_open (HOTPLUG_TYPE_MMC, HOTPLUG_ITER_DEVICES,
 *                               HOTPLUG_ITER_ALL_DEVICES);
 *    hiter_rec = hotplug_iter_get_next (hiter);
 *    while (hiter_rec != NULL)
 *    {
 *      hdev = hotplug_open_device (hiter_rec->hdev_name);
 *      ASSERT (hdev != NULL);
 *      hotplug_close_device (hdev);
 *
 *      hiter_rec = hotplug_get_next (hiter);
 *    }
 * @endcode
 *
 * @return
 *    Returns the next device of type "type" or NULL if no such device
 *    is found
 *
 * @sa
 *   hotplug_iter_get_next, hotplug_iter_close
*****************************************************************************/
struct hotplug_iter* hotplug_iter_open (hotplug_dev_type dev_type,
                                    enum hotplug_iter_type iter_type,
                                    enum hotplug_iter_device_location dev_loc);

/*************************************************************************//**
 * This function is used to loop through all the devices/partitions that
 * satisfy the iterating criteria.

 * @b Example:
 * @code
 *    struct hotplug_iter *hiter;
 *    struct hotplug_iter_record *hiter_rec;
 *    struct hotplug_device *hdev;
 *
 *    hiter = hotplug_iter_open (HOTPLUG_TYPE_MMC, HOTPLUG_ITER_DEVICES,
 *                               HOTPLUG_ITER_ALL_DEVICES);
 *    hiter_rec = hotplug_iter_get_next (hiter);
 *    while (hiter_rec != NULL)
 *    {
 *      hdev = hotplug_open_device (hiter_rec->hdev_name);
 *      ASSERT (hdev != NULL);
 *      hotplug_close_device (hdev);
 *
 *      hiter_rec = hotplug_get_next (hiter);
 *    }
 * @endcode
 *
 * @return
 *    Returns the next device of type "type" or NULL if no such device
 *    is found
 *
 * @sa
 *   hotplug_iter_open, hotplug_iter_close
*****************************************************************************/
struct hotplug_iter_record* hotplug_iter_get_next (struct hotplug_iter *hiter);

/*************************************************************************//**
 * This function is used to close an opened iterator.

 * @b Example:
 * @code
 *    struct hotplug_iter *hiter;
 *    struct hotplug_iter_record *hiter_rec;
 *    struct hotplug_device *hdev;
 *
 *    hiter = hotplug_iter_open (HOTPLUG_TYPE_MMC, HOTPLUG_ITER_DEVICES,
 *                               HOTPLUG_ITER_ALL_DEVICES);
 *    hiter_rec = hotplug_iter_get_next (hiter);
 *    while (hiter_rec != NULL)
 *    {
 *      hdev = hotplug_open_device (hiter_rec->hdev_name);
 *      ASSERT (hdev != NULL);
 *      hotplug_close_device (hdev);
 *
 *      hiter_rec = hotplug_get_next (hiter);
 *    }
 * @endcode
 *
 * @return
 *    Returns the next device of type "type" or NULL if no such device
 *    is found
 *
 * @sa
 *   hotplug_iter_open, hotplug_iter_get_next
*****************************************************************************/
int hotplug_iter_close (struct hotplug_iter *hiter);


/** --------------- Device Access Functions --------------- */

/**
 * DEPRECATED : PLEASE DO NOT USE THIS FUNCTION.
 * Please use the ::hotplug_open_device() instead.
 *
 * Open the device for use.
 *
 * This must be called before read or write access, and gives the
 * device layer an opportunity to power up the interface or otherwise
 * prepare for media use.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the handle to the
 *    persistent device.
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * @deprecated
 *    This function is just maintained for backwards compatibility. Please
 *    stop using these APIs and move to the newer ::hotplug_open_device APIs.
 */
 int hotplug_dev_open (struct hotplug_device *hdev);

/**
 * DEPRECATED : PLEASE DO NOT USE THIS FUNCTION.
 * Please use the ::hotplug_close_device() instead.
 *
 * Close the device.
 *
 * This indicates that no further device access is expected, and
 * permits the device layer to shut down as appropriate.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the handle to the
 *    persistent device.
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * @deprecated
 * This function is just maintained for backwards compatibility. Please
 * stop using these APIs and move to the newer ::hotplug_close_device APIs
 */
 int hotplug_dev_close (struct hotplug_device *hdev);


/*************************************************************************//**
 * Open the hotplug device for use.
 *
 * This must be called before read or write access, and gives the
 * device layer an opportunity to power up the interface or otherwise
 * prepare for media use.
 *
 * @param hdev_name
 *    Name of the hotplug device or the partition to be opened.
 *
 * @return
 *    NULL on failure and a valid hdev pointer on success.
*****************************************************************************/
struct hotplug_device* hotplug_open_device (const char *hdev_name);


/*************************************************************************//**
 * Close the device.
 *
 * This indicates that no further device access is expected, and
 * permits the device layer to shut down as appropriate.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, returned from the
 *    ::hotplug_open_device() function. Please don't use this function to
 *    close the hdev obtained via hotplug_find_dev_by_path or type functions.
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
*****************************************************************************/
int hotplug_close_device (struct hotplug_device *hdev);


/*************************************************************************//**
 * Open the hotplug device for use.
 *
 * This must be called before read or write access, and gives the
 * device layer an opportunity to power up the interface or otherwise
 * prepare for media use.
 *
 * @param dev_type
 *    The hotplug device type, refer to the enum hotplug_dev_type for the
 *    list of Hotplug devices.
 *
 * @param dev_loc
 *    The location of the hotplug device say external, internal or both.
 *
 * @param partition_type
 *    The partition type from MBR/EBR.
 *
 * @return
 *    NULL on failure and a valid hdev pointer on success.
 *
 * @sa
 *   hotplug_open_device, hotplug_open_device_by_gpt_partition_type
*****************************************************************************/
struct hotplug_device* hotplug_open_device_by_partition_type (
    hotplug_dev_type dev_type,
    enum hotplug_iter_device_location dev_loc,
    uint32 partition_type);



/*************************************************************************//**
 * Definition of a Globally Unique Identifier (GUID)
 ****************************************************************************/
PACKED struct hotplug_guid
{
  uint32 data1;
  uint16 data2;
  uint16 data3;
  uint8  data4[8];
} PACKED_POST;


/*************************************************************************//**
 * Open the hotplug device using the partition GUID type.
 *
 * @param dev_type
 *    The hotplug device type, refer to the enum hotplug_dev_type for the
 *    list of Hotplug devices.
 *
 * @param dev_loc
 *    The location of the hotplug device say external, internal or both.
 *
 * @param guid_type
 *    GUID type of the partition to open.
 *
 * @return
 *    NULL on failure and a valid hdev pointer on success.
 *
 * @sa
 *   hotplug_open_device, hotplug_open_device_by_partition_type
*****************************************************************************/
struct hotplug_device* hotplug_open_device_by_gpt_partition_type (
    hotplug_dev_type dev_type,
    enum hotplug_iter_device_location dev_loc,
    const struct hotplug_guid *guid_type);


/*************************************************************************//**
 * Polls the device that matches the supplied device-type and location.
 *
 * @param dev_type
 *    The hotplug device type, refer to the enum hotplug_dev_type for the
 *    list of Hotplug devices.
 *
 * @param dev_loc
 *    The location of the hotplug device say external, internal or both.
 *
 * @return
 *    0 on success, or negative error value on failure.
*****************************************************************************/
int hotplug_poll_device (hotplug_dev_type dev_type,
          enum hotplug_iter_device_location dev_loc);

/*************************************************************************//**
 * This API does the following:
 *   1. Polls the device that matches the supplied device-type and location.
 *   2. Inits EFS and mounts ROM-FS as the root file system.
 *   3. Mounts the first FAT-partition as "/mmc1" that matches the supplied
 *      device-type and location.
 *
 * @param dev_type
 *    The hotplug device type, refer to the enum hotplug_dev_type for the
 *    list of Hotplug devices.
 *
 * @param dev_loc
 *    The location of the hotplug device say external, internal or both.
 *
 * @return
 *    0 on success, or negative error value on failure.
*****************************************************************************/
int hotplug_poll_and_mount_first_fat (hotplug_dev_type dev_type,
                     enum hotplug_iter_device_location dev_loc);


/*************************************************************************//**
 * Read attempts to read upto 'block_count' number of blocks starting
 * from 'lba' from the open device referred by 'hdev' into the buffer
 * starting at 'buf'
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, obtained from the
 *    hotplug_open_device function. This represents the persistent
 *    device handle.
 * @param lba
 *    Start reading from this block
 * @param buf
 *    Buffer that holds the read data
 * @param block_count
 *    Number of blocks to read
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * Note: This call fails if the device is not open
 *
 * Please click @ref hotplug_device "here" for information about the
 * parameters and return values.
*****************************************************************************/
int hotplug_dev_read  (struct hotplug_device *hdev,
                       uint32 lba,
                       unsigned char *buf,
                       uint16 block_count);

/*************************************************************************//**
 * Write attempts to write upto 'block_count' number of blocks starting
 * from 'lba' to the open device referred by 'hdev' from the buffer starting
 * at 'buf'
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 * @param lba
 *    Start writing from this block
 * @param buf
 *    Buffer that holds the data to be written
 * @param block_count
 *    Number of blocks to write
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * Note: This call fails if the device is not open
 *
 * Please click @ref hotplug_device "here" for information about the
 * parameters and return values.
*****************************************************************************/
int hotplug_dev_write (struct hotplug_device *hdev,
                       uint32 lba,
                       unsigned char *buf,
                       uint16 block_count);

/*************************************************************************//**
 * Write_udata attempts to write upto 'block_count' number of blocks, starting
 * at 'lba', of 'User Data' to the open device referred by 'hdev' from the
 * buffer starting at 'buf'
 *
 * Some devices (eg: CPRM) must distinguish between filesystem metadata
 * (directory entries, accounting structures, etc) and "User Data" or
 * stored payload that is subject to encryption or special processing.
 * This function is used to write "user data" that is determined to
 * have originated from the application, and not the filesystem
 * itself.  For most devices, this is handled identical to
 * hotplug_dev_write().
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 * @param lba
 *    Start writing from this block
 * @param buf
 *    Buffer that holds the user data to be written
 * @param block_count
 *    Number of blocks to write
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * Please click @ref hotplug_device "here" for information about the
 * parameters and return values.
 *
 * see also: ::hotplug_dev_write
*****************************************************************************/
int hotplug_dev_write_udata (struct hotplug_device *hdev,
                             uint32 lba,
                             unsigned char *buf,
                             uint16 block_count);

/*************************************************************************//**
 * Erase attempts to erase upto 'block_count' number of blocks, starting at
 * 'lba' from an open device referred by 'hdev'
 *
 * Some devices may support a "block erase" function.  This erases a
 * span of blocks more quickly than writing 0s to them.  For most
 * devices, this is unsupported and will always return a failure.

 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 * @param lba
 *    Start erasing from this block
 * @param block_count
 *    Number of blocks to erase
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * Please click @ref hotplug_device "here" for information about the
 * parameters and return values.
*****************************************************************************/
int hotplug_dev_erase (struct hotplug_device *hdev,
                       uint32 lba,
                       uint16 block_count);

/*************************************************************************//**
 * Gets the size (number of blocks and size of each block) for a given device.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
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
*****************************************************************************/
int hotplug_dev_get_size (struct hotplug_device *hdev,
                          uint32 *block_count,
                          uint16 *bytes_per_block);

/*************************************************************************//**
 * This function checks for media presence.
 *
 * If a card is present in the slot, it means that sector read
 * or write calls are available.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the hotplug 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    Returns 1 if media in this device is present
 *    0 otherwise
 *
 * @Note
 *    This information is dynamic, so applications should allow for the
 *    possibility that media was removed (or inserted) immediately after
 *    this call returned.
*****************************************************************************/
int hotplug_dev_is_present (struct hotplug_device *hdev);

/*************************************************************************//**
 * If this function returns success, then it means that media is present, and
 * that a valid FAT filesystem was found on it, and the filesystem on this
 * device is mounted and ready for file access.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @return
 *    Returns 1 if Hotplug currently has this device mounted.
 *    (eg: the card is present and mounted as /mmc1).
 *    Returns 0 if no media is present, or if media is present, but not
 *    useable as a FAT filesystem (ie, unformatted or corrupt).
*****************************************************************************/
int hotplug_dev_is_mounted (struct hotplug_device *hdev);

/*************************************************************************//**
 * Return the mountpoint pathname of a specific device referred by 'hdev'.
 *
 * This path is where the media is (or will be) mounted when present
 * and formatted.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the hotplug 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    The return value is a pointer to a fixed (unwritable)
 *    null-terminated string, or NULL if hdev is not valid.
*****************************************************************************/
const char * hotplug_dev_get_path (struct hotplug_device *hdev);



/*************************************************************************//**
 * The last-known state of a device is represented by these flags.
 * This structure is populated by hotplug_dev_get_state().
 */
struct hdev_state
{
  boolean is_present;   /**< Media is present in the slot or device */
  boolean is_mounted;    /**< A FAT filesystem is mounted and ready */
  boolean is_formatting;     /**< A format operation is in progress */
  boolean is_locked; /**< The device is locked for exclusive access */
};

/*************************************************************************//**
 * Fetch the last known state of this hotplug device.
 *
 * An application can query the state at any time.
 * This is an inexpensive operation and performs NO media access.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @param state
 *    A pointer to the ::hdev_state structure. Upon success this structure
 *    will be populated with the last known information about this device.
 *
 * @return
 *    Returns 1 on success, 0 on failure (invalid device).
 *
 * @Note that the 'is_present' flag represents the last-known media
 * presence (at the time of the last hotplug poll), and may be stale
 * information.
*****************************************************************************/
int hotplug_dev_get_state (struct hotplug_device *hdev,
                           struct hdev_state *state);

/**
 * The current version of the hotplug_device_info structure.
 */
#define HOTPLUG_DEVICE_INFO_STRUCT_VERSION        (1)

/**
 * The device specific information of a hotplug device.
 * This structure is populated by hotplug_dev_get_device_info().
 *
 * @Note that the device specific information may not be available for
 * all device types.
 */
struct hotplug_device_info
{
  uint32   manufacturer_id;
  uint32   oem_id;
  uint32   prod_rev;
  uint32   prod_serial_num;
  byte     product_name[16];
  byte     manufactured_date[16];
};


/*************************************************************************//**
 * Fetch device specific information of a hotplug device.
 *
 * An application can query the device info at any time.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @param dev_info
 *    A pointer to the ::hotplug_device_info structure. Upon success this
 *    structure will be populated with the device specific information about
 *    this device.
 *
 * @Note that the device specific information may not be available for
 * all device types. -1 is returned if the info is not available.
 *
 * @return
 *    Returns 0 on success, -1 on failure.
*****************************************************************************/
int hotplug_dev_get_device_info (struct hotplug_device *hdev,
                                 struct hotplug_device_info *dev_info);

/**
 * DEPRECATED : PLEASE DO NOT USE THIS FUNCTION.
 *
 * Get drive number.
 *
 * This allows us to obtain the drive number associated with hdev for
 * devices at the hotplug device layer.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the handle to the
 *    persistent device.
 *
 * @return
 *    Drive number if supported by device and -1 for unsupported device
 *
 * @deprecated
 *    This API is deprecated. Please do no use it for new designs.
 */

int hotplug_dev_get_driveno (struct hotplug_device *hdev);

/**
 * DEPRECATED : PLEASE DO NOT USE THIS FUNCTION.
 *
 * Get device handle.
 *
 * This allows us to obtain the handle associated with hdev for devices
 * at the hotplug device layer.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the handle to the
 *    persistent device.
 *
 * @return
 *    Handle to the device which can be directly used with appropriate driver
 *    that is handle based. If handle does not exist NULL is returned.
 */
void * hotplug_dev_get_handle (struct hotplug_device *hdev);

/* This is the set of events that hotplug can provide notification. */
/**
 * This event signals that new media was detected in this device.  The media
 * is known to be present at the time of this event, but may be removed at
 * any time after.  (Applications should be prepared for rapid transitions.)
 * While this event signifies that media is present, that media may not contain
 * a formatted or useable filesystem.
 * (See also: ::HOTPLUG_NOTIFY_EVENT_MOUNTED)
*/
#define HOTPLUG_NOTIFY_EVENT_CARD_INSERTED      0x001

/**
 * Media has been removed from the device, and the device is no longer ready
 * for read/write access.
 * When media is rapidly changed, even with the same physical media being
 * removed and inserted, these events will always be generated for each
 * detection cycle.
 */
#define HOTPLUG_NOTIFY_EVENT_CARD_REMOVED       0x002

/**
 * When new media is detected as present, an attempt will be made to mount
 * the filesystem as a Windows-compatible FAT filesystem.  If this mount
 * succeeds, then the device is now available for filesystem access (efs_open,
 * efs_read, etc) and this event will be generated.
 */
#define HOTPLUG_NOTIFY_EVENT_MOUNTED            0x004

/**
 * When media is locked, removed, or otherwise rendered unavailable to the
 * filesystem, this event signifies that the filesystem has been unmounted.
 * Files on the device are no longer accessible through the EFS API.
 * Further, any files that were still open are immediately forced into an
 * error state.  No further read/write access will be permitted on those
 * existing file handles.
 */
#define HOTPLUG_NOTIFY_EVENT_UNMOUNTED          0x008

/**
 * An attempt was made to mount the media as a FAT filesystem, and this mount
 * failed.  This may indicate that the media does not contain a formatted FAT
 * filesystem, or that the filesystem is corrupt.  The media may be
 * temporarily unavailable (spinning up), unreadable or incompatible.  In most
 * cases, Hotplug will periodically attempt to remount the filesystem.
 */
#define HOTPLUG_NOTIFY_EVENT_MOUNT_FAILED       0x010

/**
 * This event is sent at the beginning of a Format operation.  Formatting large
 * devices can take significant time, and this even indicates the operation has
 * started.
 */
#define HOTPLUG_NOTIFY_EVENT_FORMAT_START       0x020

/**
 * This event is sent when the format operation terminates (either
 * successfully, or with error).  After this event, the format is no longer
 * in progress.
 */
#define HOTPLUG_NOTIFY_EVENT_FORMAT_COMPLETE    0x040

/**
 * The device has been locked for exclusive access by a Hotplug client.  This
 * is used to prohibit the use of media by the FAT filesystem.  Locking a card
 * will forcefully unmount the device and prevent periodic polling so that an
 * application (WLAN, Card Reader Mode, etc) can have exclusive use of the
 * device.
 */
#define HOTPLUG_NOTIFY_EVENT_CARD_LOCKED        0x080

/**
 * The Hotplug device has been released from exclusive access and is now
 * available for Hotplug to poll and mount.
 */
#define HOTPLUG_NOTIFY_EVENT_CARD_UNLOCKED      0x100

/**
 * The Hotplug device has been gone to idle state as there is no activity from
 * some predetermined time.
 */
#define HOTPLUG_NOTIFY_EVENT_DEVICE_IDLE        0x200

/** For convenience this macro is available to indicate all the above events.
 *  It is used only for registration, and never passed into the callback at
 *  event time.
 */
#define HOTPLUG_NOTIFY_ALL_EVENTS \
  ( \
    HOTPLUG_NOTIFY_EVENT_CARD_INSERTED      | \
    HOTPLUG_NOTIFY_EVENT_CARD_REMOVED       | \
    HOTPLUG_NOTIFY_EVENT_MOUNTED            | \
    HOTPLUG_NOTIFY_EVENT_UNMOUNTED          | \
    HOTPLUG_NOTIFY_EVENT_MOUNT_FAILED       | \
    HOTPLUG_NOTIFY_EVENT_FORMAT_START       | \
    HOTPLUG_NOTIFY_EVENT_FORMAT_COMPLETE    | \
    HOTPLUG_NOTIFY_EVENT_CARD_LOCKED        | \
    HOTPLUG_NOTIFY_EVENT_CARD_UNLOCKED      | \
    HOTPLUG_NOTIFY_EVENT_DEVICE_IDLE          \
  )

/** This bitmask represents one or more events */
typedef unsigned int hotplug_notify_event;

/** Each hotplug subscription provides a handle upon registration.
 * This handle must be retained by the application to deregister
 * at a later time. */
typedef unsigned int hotplug_notify_handle;

/** This handle is not valid, and is returned in the event of error. */
#define HOTPLUG_NOTIFY_INVALID_HANDLE       0


/*************************************************************************//**
 * DEPRECATED: PLEASE DO NOT USE THIS.
 * Please refer the ::hotplug_notify_register2() function instead.
 *
 * Callback functions for notification must be of this type.
 *
 * On event, these arguments will be provided during the callback:
 *
 * hdev: The specific device that triggered the event.
 * event_mask: The event that has taken place.
 * reg_hdl: Same registration handle, returned while registering this callback.
 * user_data: Convenience context data, unused by hotplug.
 *
 * Callback functions have no return value.
 *
 * @deprecated
 *   Please use the ::hotplug_notify_register_new() functions.
*****************************************************************************/
typedef void (*hotplug_notify_event_cb)
(
  struct hotplug_device     *hdev,
  hotplug_notify_event      event_mask,
  hotplug_notify_handle     reg_hdl,
  void                      *user_data
);

/*************************************************************************//**
 * DEPRECATED: PLEASE DO NOT USE THIS.
 * Please refer the ::hotplug_notify_register_new() function instead.
 *
 * @par Name:
 *    hotplug_notify_register - Register a callback function with hotplug to
 *    receive hotplug event notifications.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    hotplug_notify_handle hotplug_notify_register (
 *                hotplug_dev_type device_type,
 *                hotplug_notify_event event_mask,
 *                hotplug_notify_event_cb user_cb,
 *                void *user_data);
 *    @endcode
 *
 * @par Description:
 *  To initiate hotplug notification events, the application registers for the
 *  devices and events of interest.
 *
 * @param device_type
 *  One specific Hotplug device type,or HOTPLUG_TYPE_ALL as a wildcard
 *  for any device.
 *
 * @param event_mask
 *  An 'or'ed combination of one or more events of interest.
 *  Click \link ::HOTPLUG_NOTIFY_EVENT_CARD_INSERTED here \endlink to see the
 *  various types of events.
 *
 * @param user_cb
 *  The callback function to be used on event.
 *
 * @param user_data
 *  Arbitrary data pointer for context.  Hotplug will store this pointer and
 *  provide it to your callback, but does not use it.
 *
 * @return
 *  A valid hotplug_notify_handle on success, or HOTPLUG_NOTIFY_INVALID_HANDLE
 *  on error.
 *
 * @sa
 *  hotplug_notify_register_new hotplug_notify_unregister
 *
 * @deprecated
 *   Please use the ::hotplug_notify_register_new() functions.
 *
 **********************************************************************/
hotplug_notify_handle hotplug_notify_register (hotplug_dev_type device_type,
                                               hotplug_notify_event event_mask,
                                               hotplug_notify_event_cb user_cb,
                                               void *user_data);


/*************************************************************************//**
 * The callback information of an hotplug device.
 * hdev_name: will have the name of the device.
 * event_mask: The event that has taken place.
 * dev_type : Hotplug device type.
*****************************************************************************/
struct hotplug_notify_event_callback_info
{
  const char                *hdev_name;
  hotplug_notify_event      event_mask;
  hotplug_dev_type          dev_type;
};

/*************************************************************************//**
 * Callback functions for notification must be of this type.
 *
 * On event, these arguments will be provided during the callback:
 *
 * event_info: Information about the event like which device, what event etc.
 * reg_hdl: Same registration handle, returned while registering this callback.
 * user_data: Convenience context data, unused by hotplug.
 *
 * Callback functions have no return value.
*****************************************************************************/
typedef void (*hotplug_notify_event_cb_new)
(
  struct hotplug_notify_event_callback_info *event_info,
  hotplug_notify_handle     reg_hdl,
  void                      *user_data
);

/*************************************************************************//**
 * @par Name:
 *    hotplug_notify_register_new - Register a callback function with hotplug
 *    to receive hotplug event notifications.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    hotplug_notify_handle hotplug_notify_register (
 *                hotplug_dev_type device_type,
 *                hotplug_notify_event event_mask,
 *                hotplug_notify_event_cb user_cb,
 *                void *user_data);
 *    @endcode
 *
 * @par Description:
 *  To initiate hotplug notification events, the application registers for the
 *  devices and events of interest.
 *
 * @param device_type
 *  One specific Hotplug device type,or HOTPLUG_TYPE_ALL as a wildcard
 *  for any device.
 *
 * @param event_mask
 *  An 'or'ed combination of one or more events of interest.
 *  Click \link ::HOTPLUG_NOTIFY_EVENT_CARD_INSERTED here \endlink to see the
 *  various types of events.
 *
 * @param user_cb
 *  The callback function to be used on event.
 *
 * @param user_data
 *  Arbitrary data pointer for context.  Hotplug will store this pointer and
 *  provide it to your callback, but does not use it.
 *
 * @return
 *  A valid hotplug_notify_handle on success, or HOTPLUG_NOTIFY_INVALID_HANDLE
 *  on error.
 *
 * @sa
 *  hotplug_notify_unregister
 *
*****************************************************************************/
hotplug_notify_handle hotplug_notify_register_new (
      hotplug_dev_type device_type,
      hotplug_notify_event event_mask,
      hotplug_notify_event_cb_new user_cb_new,
      void *user_data);


/*************************************************************************//**
 * @par Name:
 *  hotplug_notify_unregister - Terminate notification events for a previous
 *  registration.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    int hotplug_notify_unregister (hotplug_notify_handle reg_hdl);
 *  @endcode
 *
 * @par Description:
 *  This API can be used to terminate notification events for a previous
 *  registration.
 *
 * @param reg_hdl
 *  The registration handle is the same one returned while registration.
 *
 * @return
 *  0 on success, -1 on failure
 *
 * @sa
 *  hotplug_notify_register
 *
*****************************************************************************/
int hotplug_notify_unregister (hotplug_notify_handle reg_hdl);




/*************************************************************************//**
 * @par Name:
 *   hotplug_checkfat - Check for corruption within FAT filesystem.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    int hotplug_checkfat (const char *path, int mode, int *errors,
 *                          void (*callback) (void *), void *param);
 *
 *  @endcode
 *
 * @par Description:
 *   hotplug_checkfat is a utility that validates the integrity of inner
 *   FAT filesystem data structures. NOTE: This feature is only available
 *   when FEATURE_HFAT_CHKFAT is defined. There are 3 classes of errors
 *   within the FAT filesystem that it can detect and fix.
 *
 *   1. Errors within the file allocation table.\n
 *   2. Errors within the directory entries.\n
 *   3. Recovery of lost file chains.\n
 *
 *   Depending on the parameters passed efs_checkfat can either check
 *   for errors and return a bit mask of the error type in the
 *   errors parameter, fix cluster and directory entry errors
 *   and/or recover lost file chains.
 *
 *
 * @param path
 *   pathname of the drive to be checked. ie: /mmc1, /ftl1, etc...
 *
 * @param mode
 *   The mode in which to run hotplug_checkfat. The options are:\n
 *   1. CHKFAT_READONLY - Do NOT make changes, return error mask.\n
 *   2. CHKFAT_FIX - Fix errors and delete lost file chains.\n
 *   3. CHKFAT_FIX_RECOVER - Fix errors and recover lost file chains.\n
 *   4. CHKFAT_ERASE_BAD_CHAIN - Erase bad chains rather than truncate.
 *      Can be OR'd with previous 2 flags.
 *
 * @param callback
 *   This function will get called periodically from checkfat to prevent
 *   from the watch dog timing out. There should not be additional work
 *   performed other than state updates and kicking the watch dog.
 *
 * @param errors
 *   On failure, it returns the errors on this parameter. The possible errors
 *   are ::CHKFAT_BAD_ELEMENT ::CHKFAT_CROSS_CHAINED ::CHKFAT_LOST_CHAIN_FOUND
 *   ::CHKFAT_FATS_INCONSISTENT ::CHKFAT_SHORT_ENTRY_INVALID
 *   ::CHKFAT_BAD_DIR_REFERENCE ::CHKFAT_LONG_ENTRY_INVALID
 *   ::CHKFAT_DIR_DEPTH_EXCEEDED
 *
 * @param param
 *   Parameter passed to the callback function.
 *
 * @return
 *   Returns 0 on success and -1 on failure.
 *
 * @note Please adjust stack size of the task based on the desired directory
 * depth CHKFAT_MAX_DIR_DEPTH. If this is improperly set, a stack overrun is
 * likely. Also, this function inherently takes time to complete. The time
 * taken corresponds to the size of disk to be checked and the number of
 * directories and files on the disk.
 *
 * @par WARNING
 * Because of the above reason this function should not be called
 * from UI task, since it might lose the events like incoming call and SMS
 * which need to be processed immediately. Instead create a lower priority
 * task that executes this function and UI task can monitor and update the
 * UI as the checkFAT progresses.
 *
 * @par WARNING
 * This revision has a known issue with multiple active HFAT
 * devices. If any other HFAT devices are present but not mounted a
 * potential deadlock condition may arise. The workaround is to ensure all
 * HFAT devices are mounted before hotplug_checkfat is run. To accomplish
 * this all mountable devices must be formatted, after which hotplug_checkfat
 * can be run after all devices have been polled after boot (~30 seconds).
 */
int hotplug_checkfat (const char *path, int mode, int *errors,
                      void (*callback) (void *), void *param);

/* Flags for checkfat
 * These can be OR'd together.
 */
#define CHKFAT_READONLY            0x1     /**< Do not change on disk */
#define CHKFAT_FIX_ERRORS          0x2     /**< Fix errors */
#define CHKFAT_RECOVER_FILES       0x4     /**< Recover files */

/* Old flags (Deprecated) */
/** @deprecated */
#define CHKFAT_FIX                 CHKFAT_FIX_ERRORS
/** @deprecated */
#define CHKFAT_FIX_RECOVER         (CHKFAT_FIX_ERRORS | CHKFAT_RECOVER_FILES)
/** @deprecated */
#define CHKFAT_ERASE_BAD_CHAIN     0       /* Default is erasing bad chain */

/* Bit mask of checkfat failure conditions */
#define CHKFAT_BAD_ELEMENT         0x1     /**< Cluster chain errors */
#define CHKFAT_CROSS_CHAINED       0x2     /**<2 chains pointing to 1 cluster*/
#define CHKFAT_LOST_CHAIN_FOUND    0x4     /**< Lost chain present */
#define CHKFAT_FATS_INCONSISTENT   0x8     /**< FAT2 doesn't match FAT1 */
#define CHKFAT_SHORT_ENTRY_INVALID 0x10    /**< Bad shortname entry */
#define CHKFAT_BAD_DIR_REFERENCE   0x20    /**< Bad . or .. reference */
#define CHKFAT_LONG_ENTRY_INVALID  0x40    /**< Bad LFN entry       */

/* Error conditions */
#define CHKFAT_DIR_DEPTH_EXCEEDED  0x1000  /**< Exceeded dir depth limit */

/*************************************************************************//**
 * Set the frequency at which Hotplug task will check for any idle device.
 *
 * @param minutes
 *    This would take amount of time (excluding the detection time) between
 *    consecutive idle_device_event_generation polls in minutes.
 *    The maximum value this can take is 254 minutes.
 *    This value should be set to 0 to inhibit polling entirely.
 *
 * @return
 *    Returns nothing.
*****************************************************************************/
void hotplug_set_idle_device_event_duration (uint8 minutes);

/*************************************************************************//**
 * This function sets the SPS end-point for data transfer. This function would
 * call the SDCC's sdcc_handle_set_sps_end_point() function.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @param param
 *    see documentation of sdcc_handle_set_sps_end_point() in sdcc_api.h
 *
 * @return
 *    Returns 0 on success and -1 on failure
 *
 * @sa
 *  sdcc_handle_set_sps_end_point in sdcc_api.h
 *
*****************************************************************************/
int hotplug_set_sps_end_point (struct hotplug_device *hdev, void *param);

/*************************************************************************//**
 * This function gets the SPS end-point for data transfer. This function would
 * call the SDCC's sdcc_handle_get_sps_end_point() function.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @param param
 *    see documentation of sdcc_handle_get_sps_end_point() in sdcc_api.h
 *
 * @return
 *    Returns 0 on success and -1 on failure
 *
 * @sa
 *  sdcc_handle_get_sps_end_point in sdcc_api.h
 *
*****************************************************************************/
int hotplug_get_sps_end_point (struct hotplug_device *hdev, void *param);

/*************************************************************************//**
 * This function write-protects the specified region on the device.
 * This function would call the driver's xxx_set_power_on_write_protection()
 * function.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @param lba
 *    Start of the sector to get write-protected. This would be relative
 *    to the partition rather than the physical sector location in the card.
 *    Since for UFS device we can only write-protect LUNs the lba and
 *    block_cnt parameters are not used for UFS.
 *
 * @param block_cnt
 *    Total number of blocks from starting sector to be write-protected.
 *    If 0 is specifed, whole partition will be protected.
 *    Since for UFS device we can only write-protect LUNs the lba and
 *    block_cnt parameters are not used for UFS.
 *
 * @return
 *    Returns 0 on success and -1 on failure
 *
 * @sa
 *   set_power_on_write_protection in sdcc_api.h
 *
*****************************************************************************/
int hotplug_set_power_on_write_protection (struct hotplug_device *hdev,
                                           uint32 lba, uint32 block_cnt);

#endif /* !__FS_HOTPLUG_H__ */
