/***********************************************************************
 * fs_rmts_image_header.c
 *
 * The EFS Image header utility.
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * APIs to create and manage Boot header (MBN/ELF) for RMTS EFS Images.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_image_header.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-03-24   dks   Add RAM variable to indicate full sync vs not.
2013-02-20   dks   Add additional debugging information on write-cache.
2013-02-04   rp    Add RAM debug-variable to RMTS-PM for debug purpose.
2013-02-01   nr    Increase probability of correctly detecting golden copy.
2013-01-17   dks   Fix image source offset calculation for rmts image header.
2013-01-17   dks   Featurize compilation of RMTS files.
2012-12-18   dks   Update golden copy strategy for new rmts layout.
2012-12-18   dks   Create

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

#include "fs_rmts_image_header.h"
#include "fs_rmts_pm.h"
#include "fs_rmts.h"
#include "fs_err.h"
#include "assert.h"


struct fs_rmts_image_header*
fs_rmts_image_header_create (uint8 *img_hdr_buff, uint32 img_hdr_buff_size,
                             uint32 image_size)
{
  struct fs_rmts_image_header *image_header;
  uint32 boot_header_size, image_offset_from_boot_hdr;

  ASSERT (img_hdr_buff!= NULL);
  ASSERT (img_hdr_buff_size >= sizeof (struct fs_rmts_image_header));
  memset (img_hdr_buff, 0, img_hdr_buff_size);

  image_header = (struct fs_rmts_image_header *) img_hdr_buff;

  image_header->boot_hdr.header_vsn_num = FS_RMTS_IMG_HEADER_IMAGE_VERSION;

  boot_header_size = sizeof (struct fs_rmts_image_header_boot_header);
  image_offset_from_boot_hdr = img_hdr_buff_size - boot_header_size;
  image_header->boot_hdr.image_src = image_offset_from_boot_hdr;

  image_header->boot_hdr.image_size = image_size;
  image_header->boot_hdr.code_size = image_size;

  /* Populate the meta header. to "IMGEFS" */
  image_header->meta_hdr.magic_string[0] = 'I';
  image_header->meta_hdr.magic_string[1] = 'M';
  image_header->meta_hdr.magic_string[2] = 'G';
  image_header->meta_hdr.magic_string[3] = 'E';
  image_header->meta_hdr.magic_string[4] = 'F';
  image_header->meta_hdr.magic_string[5] = 'S';
  image_header->meta_hdr.parti_magic_char = '#'; /* Place holder. */

  image_header->meta_hdr.is_write_cache_present = 0xFF;
  image_header->meta_hdr.is_full_sync = 0xFF;
  image_header->meta_hdr.data_sync_counter = 0xFF;

  return image_header;
}


void
fs_rmts_image_header_update_for_partition(struct fs_rmts_image_header *img_hdr,
                                          enum fs_rmts_partition_id parti_id,
                                          uint8 *image_dest_ptr,
                                          uint32 efs_image_age,
                                          uint64 current_phone_time_in_ms,
                                          uint8 is_write_cache_present,
                                          uint8 data_sync_counter,
                                          uint8 is_full_sync)
{
  uint32 image_id = FS_RMTS_IMG_HEADER_IMAGE_ID_INVALID;

  ASSERT (img_hdr != NULL);

  switch (parti_id)
  {
    case FS_RMTS_PARTITION_ID_FS1:
      image_id = FS_RMTS_IMG_HEADER_IMAGE_ID_FS1;
      img_hdr->meta_hdr.parti_magic_char = '1';
      break;

    case FS_RMTS_PARTITION_ID_FS2:
      image_id = FS_RMTS_IMG_HEADER_IMAGE_ID_FS2;
      img_hdr->meta_hdr.parti_magic_char = '2';
      break;

    case FS_RMTS_PARTITION_ID_FSG:
      image_id = FS_RMTS_IMG_HEADER_IMAGE_ID_FSG;
      img_hdr->meta_hdr.parti_magic_char = 'G';
      break;

    case FS_RMTS_PARTITION_ID_FSC:
    default:
      FS_ERR_FATAL ("%d, Img Hdr update for invalid EFS partition", parti_id,
                    0, 0);
      break;
  }

  img_hdr->boot_hdr.image_id = image_id;
  img_hdr->boot_hdr.image_dest_ptr = image_dest_ptr;
  img_hdr->boot_hdr.signature_ptr = image_dest_ptr +
                                    img_hdr->boot_hdr.image_size;
  img_hdr->boot_hdr.signature_size = 0;
  img_hdr->boot_hdr.cert_chain_ptr = img_hdr->boot_hdr.signature_ptr;
  img_hdr->boot_hdr.cert_chain_size = 0;

  img_hdr->meta_hdr.efs_image_age = efs_image_age;
  img_hdr->meta_hdr.current_phone_time_in_ms = current_phone_time_in_ms;

  img_hdr->meta_hdr.is_write_cache_present = is_write_cache_present;
  img_hdr->meta_hdr.data_sync_counter = data_sync_counter;
  img_hdr->meta_hdr.is_full_sync = is_full_sync;
}

#endif /* FEATURE_EFS_EFS2_ON_RMTS */

