/***********************************************************************
 * fs_hotplug_dev_state.c
 *
 * Hotplug Physical device state.
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 * Hotplug physical device state machine.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_dev_state.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_hotplug_i.h"
#include "fs_err.h"
#include "assert.h"


/******************************************************************************

                            HPDEV_UNINITIALIZED
                                    |
                                    |
                        [did blkdev_reset() succeed?]
                                    |
                                 no |
         HPDEV_UNUSABLE <-----------|
                                    |
                                    V yes
         |----------->------> HPDEV_UNDISCOVERED
         |                          |
         |                          |
         |    [did blkdev_open() && blkdev_is_present succeed?]
         |                          |
         |                     no   |
         <-----------<--------------|
         |                          |
         ^                          V yes
         |                  HPDEV_DISCOVERED
         |                          |
         |                          |
         |               [Mount all FAT partitions]
         |                          |
         |                          |
         |                          V
         ^                     HPDEV_MOUNTED <----<-----------|
         |                          |                         ^
         |                          |                         |
         |           [did blkdev_is_present succeed?]         |
         |                          |                         |
         |                          | yes                     |
         |                          |------------->-----------|
         |                          |
         |                       no V
         |<-------<---------- HPDEV_UNMOUNTED


******************************************************************************/
void
hotplug_change_physical_dev_state (struct hotplug_physical_device *phy_hdev,
                                   enum hotplug_physical_dev_state new_state)
{
  hotplug_validate_physical_hdev (phy_hdev);

  switch (new_state)
  {
    case HPDEV_UNINITIALIZED:
      FS_ERR_FATAL ("Default State is not done here",0,0,0);
      break;

    case HPDEV_UNUSABLE:
      ASSERT (phy_hdev->dev_state == HPDEV_UNINITIALIZED);
      phy_hdev->dev_state = HPDEV_UNUSABLE;
      break;

    case HPDEV_UNDISCOVERED:
      ASSERT (phy_hdev->dev_state == HPDEV_UNINITIALIZED ||
              phy_hdev->dev_state == HPDEV_LOCKED ||
              phy_hdev->dev_state == HPDEV_FORMATTING ||
              phy_hdev->dev_state == HPDEV_UNMOUNTED);
      phy_hdev->dev_state = HPDEV_UNDISCOVERED;
      break;

    case HPDEV_DISCOVERED:
      ASSERT (phy_hdev->dev_state == HPDEV_UNDISCOVERED);
      phy_hdev->dev_state = HPDEV_DISCOVERED;
      break;

    case HPDEV_MOUNTED:
      ASSERT (phy_hdev->dev_state == HPDEV_DISCOVERED);
      phy_hdev->dev_state = HPDEV_MOUNTED;
      break;

    case HPDEV_UNMOUNTED:
      ASSERT (phy_hdev->dev_state == HPDEV_MOUNTED);
      phy_hdev->dev_state = HPDEV_UNMOUNTED;
      break;

    case HPDEV_LOCKED:
      phy_hdev->dev_state = HPDEV_LOCKED;
      break;

    case HPDEV_FORMATTING:
      ASSERT (phy_hdev->dev_state == HPDEV_UNMOUNTED);
      phy_hdev->dev_state = HPDEV_FORMATTING;
      break;

    default:
      FS_ERR_FATAL ("hotplug physical dev-state %d",new_state,0,0);
      break;
  }

}

#endif /* FEATURE_FS_HOTPLUG  */
