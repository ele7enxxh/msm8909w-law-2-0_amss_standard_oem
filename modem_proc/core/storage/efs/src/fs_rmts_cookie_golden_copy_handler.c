/***********************************************************************
 * fs_rmts_cookie_golden_copy_handler.c
 *
 * Copyright (C) 2014 QUALCOMM Technologies, Inc.
 *
 * This file is defined to support for customer's requirements to allow
 * creation of golden copy multiple times in boot. The function
 * fs_rmts_cookie_is_golden_copy_present () can be re-defined as below to
 * achieve this objective:
 *
 *
 * int fs_rmts_cookie_is_golden_copy_present (struct hotplug_device *hdev,
 *                                            uint8* buffer, uint32 buf_size)
 * {
 *   (void) hdev;
 *   (void) buffer;
 *   (void) buf_size;
 *
 *   return 0;
 * }
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_cookie_golden_copy_handler.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-05-12   vm    Create

===========================================================================*/
#include "fs_hotplug.h"
#include "fs_err.h"
#include "fs_rmts_image_header.h"
#include "fs_rmts_cookie_golden_copy_handler.h"

#ifdef FS_UNIT_TEST
  int (*sim_test_boot_create_golden_copy_callback) () = NULL;
#endif


/* Check if FSG has already been created. Returns 1 if created and 0 if not
   and -1 on error to read from device. */
int
fs_rmts_cookie_is_golden_copy_present (struct hotplug_device *hdev,
                                       uint8 *cookie_copy_buffer,
                                       uint32 copy_buf_size)
{
  int result;
  struct fs_rmts_image_header *image_header;

  (void) copy_buf_size;

#ifdef FS_UNIT_TEST
  if (sim_test_boot_create_golden_copy_callback != NULL)
  {
    return sim_test_boot_create_golden_copy_callback ();
  }
#endif

  result = hotplug_dev_read (hdev, 0, cookie_copy_buffer, 1);
  if (result != 0)
  {
    FS_ERR_FATAL ("%d,Golden copy failed: read failed", result, 0, 0);
  }

  image_header = (struct fs_rmts_image_header *) cookie_copy_buffer;

  /* Check if the meta header is equal to the string "IMGEFSG" */
  if ((image_header->meta_hdr.magic_string[0]  == 'I') &&
      (image_header->meta_hdr.magic_string[1]  == 'M') &&
      (image_header->meta_hdr.magic_string[2]  == 'G') &&
      (image_header->meta_hdr.magic_string[3]  == 'E') &&
      (image_header->meta_hdr.magic_string[4]  == 'F') &&
      (image_header->meta_hdr.magic_string[5]  == 'S') &&
      (image_header->meta_hdr.parti_magic_char == 'G'))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
