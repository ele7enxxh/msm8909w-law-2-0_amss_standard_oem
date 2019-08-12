/***********************************************************************
 * fs_rmts_bootup_action.c
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

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_bootup_action.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-09-13   dks   Fix warning due to unused oem_data variable.
2013-08-21   dks   Featurize compilation of file for rmts
2013-07-22   rp    Create

===========================================================================*/

#include "fs_rmts_config.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

#include "fs_rmts_bootup_action.h"
#include "fs_rmts.h"
#include "assert.h"

#ifdef FS_UNIT_TEST
  int (*sim_test_rmts_boot_action_notify_callback) (
            enum fs_rmts_bootup_action_image_id image_id,
            struct fs_rmts_bootup_action_image_info *fs1,
            struct fs_rmts_bootup_action_image_info *fs2,
            struct fs_rmts_bootup_action_image_info *fsg) = NULL;
#endif

int
fs_rmts_bootup_action_notify (enum fs_rmts_bootup_action_image_id image_id,
                              struct fs_rmts_bootup_action_image_info *fs1,
                              struct fs_rmts_bootup_action_image_info *fs2,
                              struct fs_rmts_bootup_action_image_info *fsg)
{
  struct fs_rmts_bootup_action_image_info *active_fs = NULL;
  uint8 *oem_data = NULL;
  int result = 0;

  (void) oem_data; /* To supress unused variable warning */

  ASSERT (fs1 != NULL && fs2 != NULL && fsg != NULL);

  switch (image_id)
  {
    case FS_RMTS_BOOTUP_ACTION_IMAGE_ID_FS1:
      active_fs = fs1;
      break;

    case FS_RMTS_BOOTUP_ACTION_IMAGE_ID_FS2:
      active_fs = fs2;
      break;

    case FS_RMTS_BOOTUP_ACTION_IMAGE_ID_FSG:
      active_fs = fsg;
      break;

    case FS_RMTS_BOOTUP_ACTION_IMAGE_ID_NONE:
    default:
      /* All EFS-images are bad, none of them decrypted fine. */
      active_fs = NULL;
      break;
  }

  if (active_fs != NULL)
  {
    ASSERT (image_id != FS_RMTS_BOOTUP_ACTION_IMAGE_ID_NONE);
    ASSERT (active_fs->was_sb_decrypt_successful == 1);
  }

#ifdef FS_UNIT_TEST
  if (sim_test_rmts_boot_action_notify_callback != NULL)
  {
    result=sim_test_rmts_boot_action_notify_callback (image_id, fs1, fs2, fsg);
    return result;
  }
#endif

  if (image_id == FS_RMTS_BOOTUP_ACTION_IMAGE_ID_NONE)
  {
    return 0; /* return 0 or -1 to make EFS create a new FileSystem image. */
  }
  ASSERT (active_fs != NULL);

  /* Do something with OEM-data */
  oem_data = (uint8 *)active_fs->oem_data;

  /*---------------------------------------------------------------------------

  if (oem_data[1] == 0)
  {
    // BAD oem data, so tell EFS-module to do fresh start.
    // return -1 from this function will tell the EFS-module to do fresh start.
    result = -1;
  }

  ---------------------------------------------------------------------------*/

  return result;
}

#endif /* FEATURE_EFS_EFS2_ON_RMTS */

