/***********************************************************************
 * fs_tar_i.h
 *
 * FS TAR Module.
 * Copyright (C) 2010-2011, 2013 QUALCOMM Technologies, Inc.
 *
 * This module can parse an TAR stream and store the TAR contents in
 * the native EFS in the phone.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_tar/inc/fs_tar_i.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-01-29   nr    Migrate to new signed image authentication apis.
2011-10-11   nr    Moved FS_TAR_HDR_SIZE to this file from fs_tar.c.
2011-07-01   wek   Create a TAR file from a given directory.
2011-06-18   wek   Add secure TAR signing/verification.
2010-12-03   rp    Featurize FS-TAR module.
2010-11-18   rp    Create

===========================================================================*/

#ifndef __FS_TAR_I_H__
#define __FS_TAR_I_H__

#include "comdef.h"
#include "fs_config_i.h"

#ifdef FEATURE_FS_TAR

#define FS_TAR_HDR_SIZE        (512) /* 1 Sector : 512 bytes. */
struct fs_tar_info;


/**************************************************************************//**
 * @par Name:
 *   fs_tar_init - Initialize the fs_tar module.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_tar_i.h"
 *    void fs_tar_init (void);
 *  @endcode
 *
 * @par Description:
 *  Initalize the fs_tar module, called from efs_initialize()
 *
******************************************************************************/
void fs_tar_init (void);


/**************************************************************************//**
 * @par Name:
 *   fs_tar_open - Opens a fs_tar handle
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_tar_i.h"
 *    struct fs_tar_info* fs_tar_open (void);
 *  @endcode
 *
 * @par Description:
 *  Opens the tar handle
 *
******************************************************************************/
struct fs_tar_info* fs_tar_open (void);


/**************************************************************************//**
 * @par Name:
 *   fs_tar_process_data - Parse a given buffer of TAR data.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_tar_i.h"
 *    int fs_tar_process_data (struct fs_tar_info *tar_info,
 *                             uint8 *buffer, uint32 buffer_size);
 *  @endcode
 *
 * @par Description:
 *  Parse the given TAR data and create appropriate dirs/files in EFS.
 *
 * @param tar_info
 *  Tar handle obtained from the fs_tar_open() API.
 *
 * @param buffer
 *  TAR data.
 *
 * @param buffer_size
 *  Size of the previous argument.
 *
 * @return
 *  Returns 0 if end of tar image is reached, or 1 if still some more tar data
 *  is pending.
 *
******************************************************************************/
int fs_tar_process_data (struct fs_tar_info *tar_info,
                         uint8 *buffer, uint32 buffer_size);


/**************************************************************************//**
 * @par Name:
 *   fs_tar_open - Close a fs_tar handle
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_tar_i.h"
 *    int fs_tar_close (struct fs_tar_info *tar_info);
 *  @endcode
 *
 * @par Description:
 *  Close the tar handle
 *
******************************************************************************/
int fs_tar_close (struct fs_tar_info *tar_info);


/**************************************************************************//**
 * @par Name:
 *   fs_tar_is_this_a_tar_file - Checks if the supplied buffer for TAR image.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_tar_i.h"
 *    int fs_tar_is_this_a_tar_file (uint8 *buffer, uint32 buffer_size);
 *  @endcode
 *
 * @par Description:
 *  Checks if the supplied buffer looks like a TAR file header.
 *
 * @param buffer
 *  Tar data.
 *
 * @param buffer_size
 *  Size of the previous argument.
 *
 * @return
 *  Returns 1 if the supplied buffer looks like a TAR image or else 0.
 *
******************************************************************************/
int fs_tar_is_this_a_tar_file (uint8 *buffer, uint32 buffer_size);

/***********************************************************************//**
 * @par Name:
 *   fs_tar_verify_signature - Checks if the buffer is signed.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_tar_i.h"
 *    int fs_tar_verify_signature (uint8 *buf, uint32 buf_size,
 *                                 uint32 *img_offset, uint32 *img_size);
 *  @endcode
 *
 * @par Description:
 *  Checks the given buffer for a signed file system image.
 *
 * @param buf[in]          buffer that holds the signed image.
 * @param buffer_size[in]  Size of the above buffer
 * @param img_offset[out]  In what offset of the buffer is the signed image.
 * @param img_size[out]          Size of the File System Image.
 * @param handle_buffer[in]      The buffer for internal handle.
 * @param handle_buffer_size[in] The size of the above buffer.
 *
 * @return
 *  If the image signature is valid returns 1. If it is invalid returns 0.
 *
 **************************************************************************/
int fs_tar_verify_signature (uint8 *buf, uint32 buf_size,
                             uint32 *img_offset, uint32 *img_size,
                             uint8 *handle_buffer, uint32 handle_buffer_size);

/* Basic handle used for TAR creation. */
struct fs_tar_create_info;

/***********************************************************************//**
 * @par Name:
 *   fs_tar_create_open - Opens a directory to be returned in TAR format.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_tar_i.h"
 *    struct fs_tar_create_info *fs_tar_create_open (const char *dir_path);
 *  @endcode
 *
 * @par Description:
 *  Opens a directory so the directory and all its contents can be returned
 *  in Tape Archive (TAR) format.
 *
 * @param dir_path[in]   Path to archive.
 *
 * @return
 *  A valid pointer if open succeded, NULL on error with efs_errno set
 *  accordingly.
 *
 **************************************************************************/
struct fs_tar_create_info *fs_tar_create_open (const char *dir_path);

/***********************************************************************//**
 * @par Name:
 *   fs_tar_create_read - Outputs the TAR archive.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_tar_i.h"
 *    uint32 fs_tar_create_read (struct fs_tar_create_info *t_info,
 *                               uint8 *buffer, uint32 buf_size);
 *  @endcode
 *
 * @par Description:
 *  Reads the directory contents recursively populates the input buffer
 *  with the data corresponding to a Tape Archive (TAR) of the directory.
 *
 * @param t_info[in]   Tar handle returned by fs_tar_create_open().
 * @param buffer[out]  Buffer that holds the read TAR.
 * @param buf_size[in] Maximum number of bytes to read.
 *
 * @return
 *  Number of bytes bytes successfully read and placed into the output
 *  buffer. On error returns -1 and efs_errno set accordingly.
 *
 **************************************************************************/
int fs_tar_create_read (struct fs_tar_create_info *t_info,
                        uint8 *buffer, uint32 buf_size);

/***********************************************************************//**
 * @par Name:
 *   fs_tar_create_close - Close the tar creation handle.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_tar_i.h"
 *    int fs_tar_create_close (struct fs_tar_create_info *t_info);
 *  @endcode
 *
 * @par Description:
 *  Close a tar creation handle given by fs_tar_create_open().
 *
 * @param t_info[in]   Tar handle returned by fs_tar_create_open().
 *
 * @return
 *  Zero (0) on success, -1 on error with efs_errno set accordingly.
 *
 **************************************************************************/
int fs_tar_create_close (struct fs_tar_create_info *t_info);

#endif /* FEATURE_FS_TAR */

#endif /* not __FS_TAR_I_H__ */
