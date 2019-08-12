/***********************************************************************
 * fs_rmts_bootup_action.h
 *
 * Remote Storage Bootup Action module.
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * This module allows OEMs to check on the 3 EFS-image (FS1, FS2 and FSG)
 * decryption results and OEM-data stored in the SuperBlock.
 * The OEMs after inspecting the EFS-image decryption results and the OEM-data
 * can tell the EFS-module to either go ahead with the current active EFS-image
 * or create a brand new FileSystem.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_rmts_bootup_action.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-08-21   dks   Featurize compilation of file for rmts
2013-07-22   rp    Create

===========================================================================*/

#ifndef __FS_RMTS_BOOTUP_ACTION_H__
#define __FS_RMTS_BOOTUP_ACTION_H__

#include "fs_rmts_config.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

#include "comdef.h"

enum fs_rmts_bootup_action_image_id
{
  FS_RMTS_BOOTUP_ACTION_IMAGE_ID_NONE,
  FS_RMTS_BOOTUP_ACTION_IMAGE_ID_FS1,
  FS_RMTS_BOOTUP_ACTION_IMAGE_ID_FS2,
  FS_RMTS_BOOTUP_ACTION_IMAGE_ID_FSG,
};

/** Struct to specify the image information after decryption. */
struct fs_rmts_bootup_action_image_info
{
  uint32  age; /* Age of the image [0 to 0xFFFFFFFF] */
  int     was_sb_decrypt_successful;
  uint8   oem_data[FS_RMTS_SUPERBLOCK_OEM_DATA_SIZE_IN_BYTES];
};

/**
 * This function will be called by the Remote-Storage Page-Manager to inform
 * of the decryption results of the EFS-images and which exact EFS-image is
 * chosen as the active EFS copy.
 *
 * The return value of this function can be used to tell EFS-module to either
 * continue using the currently selected active EFS-image or to discard all
 * the EFS-images and create a brand new FileSystem image.
 *
 * @param image_id
 *   The currently selected active EFS image id.
 *
 * @param fs1, fs2, fsg
 *   Information about the three EFS-images FS1, FS2 and FSG.
 *
 * @return
 *   The return value is used to tell the Remote-Storage Page-Manager how to
 *   proceed further, which is to either go ahead with the currently selected
 *   EFS-image or discard all and create a brand new FileSystem image.
 *    0 -> Go ahead with the current selected EFS-image.
 *   -1 -> Discard all images and create a brand new EFS-image.
 *
 **********************************************************************/
int fs_rmts_bootup_action_notify (enum fs_rmts_bootup_action_image_id image_id,
                                 struct fs_rmts_bootup_action_image_info *fs1,
                                 struct fs_rmts_bootup_action_image_info *fs2,
                                 struct fs_rmts_bootup_action_image_info *fsg);


#ifdef FS_UNIT_TEST
  /* Callback function used by tests */
  int (*sim_test_rmts_boot_action_notify_callback) (
            enum fs_rmts_bootup_action_image_id image_id,
            struct fs_rmts_bootup_action_image_info *fs1,
            struct fs_rmts_bootup_action_image_info *fs2,
            struct fs_rmts_bootup_action_image_info *fsg);
#endif

#endif /* FEATURE_EFS_EFS2_ON_RMTS */

#endif /* not __FS_RMTS_BOOTUP_ACTION_H__ */
