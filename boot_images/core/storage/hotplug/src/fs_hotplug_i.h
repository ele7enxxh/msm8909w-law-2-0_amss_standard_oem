/***********************************************************************
 * fs_hotplug_i.h
 *
 * Hotplug definitions Internal to the Hotplug components themselves.
 *
 * Copyright (C) 2009-2013 QUALCOMM Technologies, Inc.
 *
 * This is the PRIVATE header file for Hotplug.
 * Any code outside EFS should include only "fs_hotplug.h" instead.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_i.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-10   rp    Hotplug to poll only devices of interest.
2012-04-16   wek   Change legacy hdev stub return to discover successfully.
2012-03-18   wek   Reduce hotplug size for bootloaders.
2011-11-03   dks   Make Hotplug independent of EFS.
2011-04-29   wek   Fix documentation errors.
2011-03-31   wek   Implement GPT/MBR partition table parser.
2010-10-21   dks   Fix hotplug APIs to work correctly for legacy hdevs.
2010-10-06   dks   Discover device when Hotplug polling is disabled.
2010-05-21   rp    Fix compiler warnings seen on target.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_HOTPLUG_I_H__
#define __FS_HOTPLUG_I_H__

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

/* Pick up the public definitions as well */
#include "comdef.h"
#include "fs_hotplug.h"
#include "fs_blockdev.h"
#include "fs_osal.h"
#include "fs_hotplug_priv_api.h"



/*************************************************************
 * INTERNAL (PRIVATE) INTERFACE
 *
 * These definitions are for use only within Hotplug itself
 *************************************************************/

extern fs_os_tcb_type hotplug_tcb; /* Task TCB for Hotplug */

#define HOTPLUG_POLL_SIG                0x0001 /* Timer signal. */
#define HOTPLUG_STATE_SIG               0x0002 /* Polling state change. */
#define HOTPLUG_USBHOST_OPEN_SIG        0x0004
#define HOTPLUG_USBHOST_CLOSE_SIG       0X0008
#define HOTPLUG_USBHOST_SEM_WAIT_SIG    0x0010
#define HOTPLUG_IDLE_DEVICE_SIG         0X0020 /* Timeout event signal. */


/* Hotplug physical device states. */
enum hotplug_physical_dev_state
{
  HPDEV_UNINITIALIZED     = 0,
  HPDEV_UNUSABLE          = 1,
  HPDEV_UNDISCOVERED      = 2,
  HPDEV_DISCOVERED        = 3,
  HPDEV_MOUNTED           = 4,
  HPDEV_UNMOUNTED         = 5,
  HPDEV_LOCKED            = 6,
  HPDEV_FORMATTING        = 7
};

/* Hotplug physical device structure. */
struct hotplug_physical_device
{
  enum hotplug_physical_dev_state     dev_state;
  hotplug_dev_type                    dev_type;
  int                                 is_hidden;

  char canonical_name[FS_HOTPLUG_NAME_MAX];

  struct block_device         *bdev;
  void                        *bdev_handle;
  int                         is_embedded_device;
  uint32                      idle_counter;

  struct hotplug_partition    *parti_list;
  struct hotplug_device       *hdev_list;

  struct hotplug_device       *legacy_hdev;
};

/* Hotplug device (hdev) */
struct hotplug_device
{
  /* Device info */
  int                         is_in_use;
  int                         is_invalid;

  /* Partition info */
  struct hotplug_partition    *parti;
  uint32                      size_in_sectors;
  uint16                      one_sector_size;

  struct hotplug_physical_device    *phy_hdev;
  void                              *bdev_handle;

  /* Pointer to the next hotplug_device */
  struct hotplug_device     *next_hdev;
};

/* We have to handle two types of boot records, MBR and GPT */
enum hotplug_partition_type
{
  HOTPLUG_ENTIRE_DEVICE      = 1,
  HOTPLUG_HARD_PARTITION     = 2,
  HOTPLUG_SOFT_GPT_PARTITION = 3,
  HOTPLUG_SOFT_MBR_PARTITION = 4,
  HOTPLUG_SOFT_FAT_PARTITION = 5 /* No MBR, just FAT boot record. */
};

/* Hotplug partition structure. One instance of this struct is created for
 * every partition that we discover, and the instance is reference counted. */
struct hotplug_partition
{
  /* Mantains how many times this partition is referenced, could be by the
   * partition list or by an open hdev. */
  int         ref_cnt;

  /* The following 3 values change after the partition is discovered */
  int         is_mounted;
  int         is_formatting;
  int         is_locked;

  /* The rest of the members of this structure are populated when the
   * device is discovered and a partition is found. */
  const char  *friendly_name;    /* Points to friendly name in map */
  char canonical_name[FS_HOTPLUG_NAME_MAX]; /* ie: /hdev/sdc1h0p1 */

  uint8 hard_partition_number; /* used to open the block device. */
  uint8 bootable; /* Is it marked bootable in the partition table */

  /* Indicates if it is entire device/hard/soft and which type of soft
   * partition, gpt or MBR */
  enum hotplug_partition_type partition_type;
  union
  {
    struct hotplug_guid guid;
    uint32 mbr;
  } boot_record_partition_type;
  uint32 start_sector;
  uint32 size_in_sectors;

  struct hotplug_physical_device *phy_hdev;

  /* Chains a list of partitions together, creating a list of partitions
   * for a given physical device. */
  struct hotplug_partition  *next_parti;
};

struct hotplug_iter
{
  int                                 is_in_use;

  /* user supplied values. */
  hotplug_dev_type                    dev_type;
  enum hotplug_iter_type              iter_type;
  enum hotplug_iter_device_location   dev_location;

  /* The latest physical-device and partition iterated so far. */
  struct hotplug_physical_device      *phy_hdev;
  struct hotplug_partition            *parti;

  /* Place holder to return the result to the user. */
  struct hotplug_iter_record          iter_rec;
};


/* We need a Hotplug lock to make sure the hotplug data structures are
 * only accessed by one thread at a time.   Any access to the table
 * should only take place with the lock held.  */
#ifdef FS_STANDALONE
  #define HOTPLUG_LOCK_INIT()
  #define HOTPLUG_LOCK()
  #define HOTPLUG_UNLOCK()
#else
  extern fs_os_crit_sect_type hotplug_crit_sect;
  #define HOTPLUG_LOCK_INIT() fs_os_init_crit_sect  (&hotplug_crit_sect)
  #define HOTPLUG_LOCK()      fs_os_enter_crit_sect (&hotplug_crit_sect)
  #define HOTPLUG_UNLOCK()    fs_os_leave_crit_sect (&hotplug_crit_sect)
#endif


extern struct hotplug_device hotplug_devices[];
extern struct hotplug_physical_device hotplug_physical_devices[];
extern int hotplug_total_devices;
extern int hotplug_total_physical_devices;
extern struct hotplug_partition hotplug_partitions[];
extern int hotplug_total_partitions;


/* Hdev functions. */
struct hotplug_device* hotplug_hdev_alloc (void);
void hotplug_hdev_free (struct hotplug_device *hdev);
void hotplug_add_hdev_to_phy_hdev (struct hotplug_physical_device *phy_hdev,
                                   struct hotplug_device *new_hdev);
void hotplug_remove_hdev_from_phy_hdev(struct hotplug_physical_device *,
                                       struct hotplug_device *freeing_hdev);

/* Partition functions. */
void hotplug_init_partitions (void);
struct hotplug_partition* hotplug_partition_alloc (void);
void hotplug_partition_ref (struct hotplug_partition *parti);
void hotplug_partition_unref (struct hotplug_partition *parti);
void hotplug_free_all_partitions (struct hotplug_physical_device *phy_hdev);
void hotplug_mount_all_partitions (struct hotplug_physical_device *phy_hdev,
                                   const char *hdev_name);
void hotplug_unmount_all_partitions (struct hotplug_physical_device *phy_hdev,
                                     const char *hdev_name);
int hotplug_is_partition_ok_to_lock (struct hotplug_physical_device *phy_hdev,
                                 const char *hdev_name);
void hotplug_mark_all_partitions_as_formatting (
  struct hotplug_physical_device *phy_hdev, const char *hdev_name);
void hotplug_unmark_all_partitions_as_formatting (
  struct hotplug_physical_device *phy_hdev, const char *hdev_name);
void hotplug_lock_all_partitions (struct hotplug_physical_device *phy_hdev,
                             const char *hdev_name);
void hotplug_unlock_all_partitions (struct hotplug_physical_device *phy_hdev,
                               const char *hdev_name);
void hotplug_add_partition (struct hotplug_physical_device *phy_hdev,
                            struct hotplug_partition *new_parti);
struct hotplug_partition* hotplug_find_partition (const char *hdev_name);
struct hotplug_physical_device* hotplug_find_physical_device(const char *name);


/* Partition discovery functions. */
int hotplug_discover_device (struct hotplug_physical_device *phy_hdev);
void hotplug_task_discover_device (const char *hdev_name);

/* Hotplug event functions. */
#ifdef FEATURE_FS_HOTPLUG_DISABLE_TASK
static inline void hotplug_events_init (void)
{
}
static inline void hotplug_notify_events (const char *hdev_name,
     struct hotplug_physical_device *phy_hdev, hotplug_notify_event event_id)
{
  (void) hdev_name;
  (void) phy_hdev;
  (void) event_id;
}
#else /* if FEATURE_FS_HOTPLUG_DISABLE_TASK is not defined. */
void hotplug_events_init (void);
void hotplug_notify_events (const char *hdev_name,
     struct hotplug_physical_device *phy_hdev, hotplug_notify_event event_id);
#endif


/* Hotplug Idle device functions. */
void hotplug_idle_init (void);
void hotplug_idle_device_dispatch (void);
#ifdef FEATURE_FS_HOTPLUG_DISABLE_TASK
static inline void
hotplug_reset_device_idle_counter (struct hotplug_physical_device *hdev)
{
  (void) hdev;
}
#else
void hotplug_reset_device_idle_counter (struct hotplug_physical_device *);
#endif
void hotplug_idle_device_timer_signal_received (void);
int hotplug_set_device_idle (struct hotplug_physical_device *phy_hdev);

/* Hotplug iter functions. */
void hotplug_iter_init (void);

/* Hotplug mount functions. */
void hotplug_mount_init (void);
void hotplug_mount_nodev (const char *mname);
void hotplug_umount_nodev (const char *mname);
int hotplug_is_it_a_fat_partition (struct hotplug_partition *parti);
int hotplug_partition_mount_helper (struct hotplug_partition *parti);
int hotplug_partition_unmount_helper (struct hotplug_partition *parti);


/* Hotplug poll functions. */
void hotplug_poll_init (void);
void hotplug_poll_before_mounting_root (void);
void hotplug_poll (void);
void hotplug_poll_before_mounting_root_helper (hotplug_dev_type dev_type,
                          enum hotplug_iter_device_location dev_location);
void hotplug_force_poll_now (void);
void hotplug_poll_signal_received (void);

/* Hotplug legacy-names functions. */
#ifndef FEATURE_HOTPLUG_COMPILE_WITHOUT_EFS
void hotplug_legacy_names_init (void);
void hotplug_legacy_names_create (struct hotplug_physical_device *phy_hdev);
void hotplug_legacy_names_remove (struct hotplug_physical_device *phy_hdev);
const char* hotplug_legacy_names_get_path (const char *canonical_name);
#else /* FEATURE_HOTPLUG_COMPILE_WITHOUT_EFS */

static inline void
hotplug_legacy_names_init (void)
{
}
static inline void
hotplug_legacy_names_create (struct hotplug_physical_device *phy_hdev)
{
  (void) phy_hdev;
}
static inline void
hotplug_legacy_names_remove (struct hotplug_physical_device *phy_hdev)
{
  (void) phy_hdev;
}
static inline const char*
hotplug_legacy_names_get_path (const char *canonical_name)
{
  (void) canonical_name;
  return NULL;
}
#endif /* !FEATURE_HOTPLUG_COMPILE_WITHOUT_EFS */

/* Hotplug legacy hdev functions. */
#ifdef FEATURE_FS_HOTPLUG_LEGACY_HDEV_SUPPORT
void hotplug_legacy_hdev_create (struct hotplug_physical_device *phy_hdev);
int hotplug_legacy_hdev_open (struct hotplug_physical_device *phy_hdev);
void hotplug_legacy_hdev_close (struct hotplug_physical_device *phy_hdev);
const char* hotplug_legacy_hdev_get_path (struct hotplug_device *hdev);
int hotplug_legacy_hdev_lock (struct hotplug_device *hdev);
int hotplug_legacy_hdev_unlock (struct hotplug_device *hdev);
int hotplug_legacy_hdev_is_mounted (struct hotplug_device *hdev);
int hotplug_legacy_hdev_get_state (struct hotplug_device *hdev,
                                   struct hdev_state *state);
#else /* FEATURE_FS_HOTPLUG_LEGACY_HDEV_SUPPORT */

static inline void
hotplug_legacy_hdev_create (struct hotplug_physical_device *phy_hdev)
{
  (void) phy_hdev;
}
static inline int
hotplug_legacy_hdev_open (struct hotplug_physical_device *phy_hdev)
{
  (void) phy_hdev;
  return 0;
}
static inline void
hotplug_legacy_hdev_close (struct hotplug_physical_device *phy_hdev)
{
  (void) phy_hdev;
}
static inline const char*
hotplug_legacy_hdev_get_path (struct hotplug_device *hdev)
{
  (void) hdev;
  return NULL;
}
static inline int hotplug_legacy_hdev_lock (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}
static inline int hotplug_legacy_hdev_unlock (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}
static inline int hotplug_legacy_hdev_is_mounted (struct hotplug_device *hdev)
{
  (void) hdev;
  return 0;
}
static inline int hotplug_legacy_hdev_get_state (struct hotplug_device *hdev,
                                   struct hdev_state *state)
{
  (void) hdev;
  (void) state;
  return -1;
}
#endif /* FEATURE_FS_HOTPLUG_LEGACY_HDEV_SUPPORT */

/* Hotplug physical device state function. */
void hotplug_change_physical_dev_state (struct hotplug_physical_device *,
                                        enum hotplug_physical_dev_state);

/* Hotplug validation functions to validate API args. */
int hotplug_is_valid_physical_hdev (struct hotplug_physical_device *);
int hotplug_is_valid_hdev (struct hotplug_device *hdev);
int hotplug_is_valid_hdev_for_read_write (struct hotplug_device*);
int hotplug_is_valid_parti (struct hotplug_partition *parti);
int hotplug_validate_physical_hdev (struct hotplug_physical_device *);

/* Hotplug API helper functions. */
int hotplug_format_hdev (struct hotplug_device *hdev);
int hotplug_lock_hdev (struct hotplug_device *hdev);
int hotplug_unlock_hdev (struct hotplug_device *hdev);
int hotplug_unlock_entire_device (struct hotplug_device *hdev);


#endif /* FEATURE_FS_HOTPLUG */

#endif /* not __FS_HOTPLUG_I_H__ */
