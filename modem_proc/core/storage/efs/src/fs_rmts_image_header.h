/***********************************************************************
 * fs_rmts_image_header.h
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

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_image_header.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-03-24   dks   Add RAM variable to indicate full sync vs not.
2013-02-20   dks   Add additional debugging information on write-cache.
2013-02-04   rp    Add RAM debug-variable to RMTS-PM for debug purpose.
2013-02-01   nr    Increase probability of correctly detecting golden copy.
2013-01-17   dks   Fix image source offset calculation for rmts image header.
2013-01-17   dks   Featurize compilation of RMTS files.
2012-12-18   dks   Update golden copy strategy for new rmts layout.
2012-12-18   dks   Add APIs to create and update headers.
2012-11-16   dks   Create

===========================================================================*/

#ifndef __FS_RMTS_IMAGE_HEADER_H__
#define __FS_RMTS_IMAGE_HEADER_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

#include "comdef.h"
#include "fs_rmts.h"


/* Fusion based targets do not have support for reading EFS Images over
   remotefs server during boot-up. Because of this the EFS Images need to be
   loaded by SBL. These images which are on the eMMC card are sent to
   bootloaders over Sahara by Kickstart. Once images are received, SBL will
   require all the information to load images successfully into correct RAM
   address to be specified in the image itself. For images generated at build
   time, the headers are generated with the image as a part of the build. Since
   new EFS Images are generated at run-time on each sync, the headers will need
   to attached to the images at run-time. Follwing are APIs to create the
   image headers required by Boot. */

/* MBN header structure as expected by Boot containing the Image ID to identify
   the image, the RAM address to load the image into, the offset of the image
   itself from the header, image size etc. The EFS image headers do not undergo
   authentication unlike other images; so the signature and certificate fields
   are not used. */
PACKED struct fs_rmts_image_header_boot_header
{
  uint32 image_id;           /**< Identifies the type of image this header
                                  represents (OEM SBL, AMSS, Apps boot loader,
                                  etc.). */
  uint32 header_vsn_num;     /**< Header version number. */
  uint32 image_src;          /**< Offset from end of the Boot header where the
                                  image starts. */
  uint8* image_dest_ptr;     /**< Pointer to location to store image in RAM.
                                  Also, entry point at which image execution
                                  begins. */
  uint32 image_size;         /**< Size of complete image in bytes */
  uint32 code_size;          /**< Size of code region of image in bytes */
  uint8* signature_ptr;      /**< Pointer to images attestation signature */
  uint32 signature_size;     /**< Size of the attestation signature in
                                 bytes */
  uint8* cert_chain_ptr;     /**< Pointer to the chain of attestation
                                 certificates associated with the image. */
  uint32 cert_chain_size;    /**< Size of the attestation chain in bytes */
} PACKED_POST;


/* Additional meta data stored by EFS in the image header sector */
PACKED struct fs_rmts_image_header_meta_header
{
  uint8 magic_string[6]; /* Should contain the string "IMGEFS" */
  uint8 parti_magic_char; /* Contains special chars ie 1, 2, or G */
  uint32 efs_image_age;
  uint64 current_phone_time_in_ms;
  uint8 is_write_cache_present;
  uint8 data_sync_counter;
  uint8 is_full_sync;
} PACKED_POST;


/* Structure used in EFS for easy access of all header data. */
PACKED struct fs_rmts_image_header
{
  struct fs_rmts_image_header_boot_header boot_hdr;
  struct fs_rmts_image_header_meta_header meta_hdr;
} PACKED_POST;


#define FS_RMTS_IMG_HEADER_IMAGE_VERSION      (3)
#define FS_RMTS_IMG_HEADER_IMAGE_ID_FS1       (16)
#define FS_RMTS_IMG_HEADER_IMAGE_ID_FS2       (17)
#define FS_RMTS_IMG_HEADER_IMAGE_ID_FSG       (20)
#define FS_RMTS_IMG_HEADER_IMAGE_ID_INVALID   (0x7FFFFFFF)

/* Create an image header with generic fields filled out. */
struct fs_rmts_image_header* fs_rmts_image_header_create (uint8 *img_hdr_ram,
                                   uint32 img_hdr_ram_size, uint32 image_size);

/* Update the image (partition) specific fields of the header. */
void fs_rmts_image_header_update_for_partition
                                       (
                                        struct fs_rmts_image_header *img_hdr,
                                        enum fs_rmts_partition_id parti_id,
                                        uint8 *image_dest_ptr,
                                        uint32 efs_image_age,
                                        uint64 current_phone_time_in_ms,
                                        uint8 is_write_cache_present,
                                        uint8 data_sync_counter,
                                        uint8 is_full_sync
                                       );


#endif /* FEATURE_EFS_EFS2_ON_RMTS */

#endif /* not __FS_RMTS_IMAGE_HEADER_H__ */
