/***********************************************************************
 * fs_zip_tar_i.h
 *
 * FS ZIPPED TAR MODULE
 * Copyright (C) 2011 QUALCOMM Technologies, Inc.
 *
 * This provides api to create zipped tar files.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_tar/inc/fs_zip_tar_i.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-09-21   nr    Create

===========================================================================*/

#ifndef __FS_ZIP_TAR_I_H__
#define __FS_ZIP_TAR_I_H__

#include "fs_config_i.h"

#ifdef FEATURE_FS_ZIP_TAR
#include "fs_util.h"

/* Total heap required for zipped tar creation */
#define FS_ZIP_TAR_HEAP_SIZE (FS_ZIP_COMPRESSION_MEMORY + FS_TAR_HDR_SIZE)

/**************************************************************************//**
 * @par Name:
 *   fs_zip_tar_init - Initialize the fs_zip_tar module.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip_tar_i.h"
 *    void fs_zip_tar_init (void);
 *  @endcode
 *
 * @par Description:
 *  Initalize the fs_zio_tar module, called from efs_initialize()
 *
******************************************************************************/
void fs_zip_tar_init (void);

/***********************************************************************//**
 * @par Name:
 *   fs_zip_tar_open - Opens a directory to be returned in ZIPPED_TAR format.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip_tar_i.h"
 *    void* fs_zip_tar_open (const char *path);
 *  @endcode
 *
 * @par Description:
 *  Opens a directory so the directory and all its contents can be returned
 *  in zip tar format.
 *
 * @param path[in]   Path to archive.
 *
 * @return
 *  A valid pointer if open succeded, NULL on error with efs_errno set
 *  accordingly.
 *
 **************************************************************************/
void*
fs_zip_tar_open (const char *path);

/***********************************************************************//**
 * @par Name:
 *   fs_zip_tar_create_read - Outputs the Zip_TAR archive.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip_tar_i.h"
 *    uint32 fs_zip_tar_create_read (void *img_handle, uint8 *output,
 *                                   uint32 out_buff_size);
 *  @endcode
 *
 * @par Description:
 *  Reads the directory contents recursively populates the input buffer
 *  with the data corresponding to a zip_tar of the directory.
 *
 * @param img_handle[in]   The handle returned by fs_zip_tar_open().
 * @param output[out]  Buffer that holds the read zipped TAR.
 * @param out_buff_size[in] Maximum number of bytes to read.
 *
 * @return
 *  Number of bytes bytes successfully read and placed into the output
 *  buffer. On error returns -1 and efs_errno set accordingly.
 *
 **************************************************************************/
int32
fs_zip_tar_create_read (void *img_handle, uint8 *output,
                                uint32 out_buff_size);

/***********************************************************************//**
 * @par Name:
 *   fs_zip_tar_close - Close the zip_tar creation handle.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_zip_tar_i.h"
 *    int fs_zip_tar_close (void *img_handle);
 *  @endcode
 *
 * @par Description:
 *  Close a zip_tar creation handle given by fs_zip_tar_open().
 *
 * @param img_handle[in]   Zip_tar handle returned by fs_zip_tar_open().
 *
 * @return
 *  Zero (0) on success, -1 on error with efs_errno set accordingly.
 *
 **************************************************************************/
int
fs_zip_tar_close (void *img_handle);
#endif /* FEATURE_FS_ZIP_TAR */
#endif /* not __FS_ZIP_TAR_I_H__ */
