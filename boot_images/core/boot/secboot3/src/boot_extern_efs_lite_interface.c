/*===========================================================================

                    BOOT EXTERN EFS LITE DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external efs Lite /readonly drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_efs_lite_interface.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/13   kedara  Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_extern_efs_interface.h"
#include "fs_cookie.h"
#include <stdarg.h>

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_efs_read_only_initialize

** ==========================================================================
*/
/**
 * Initialize the filesystem and mount romfs as the root filesystem.
 * This should only be called from boot code.
 *
 * Returns 0 on success, otherwise returns an error code (does not set
 * errno).
 **********************************************************************/
int boot_efs_read_only_initialize (void)
{
  return efs_read_only_initialize();
}

/*===========================================================================

**  Function :  boot_efs_stat

** ==========================================================================
*/
/**
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int boot_efs_stat (const char *path, struct fs_stat *buf);
 *  @endcode
 *
 * @par Description:
 *   boot_efs_stat returns the information about the file given its path. 
 *   boot_efs_stat and efs_lstat() are very similar, in that they both return the
 *   information about a given pathname. They differ in how they treat the
 *   last component of path. If it is a symlink, boot_efs_stat will "dereference"
 *   this symlink, returning the information about what the symlink points to.
 *   It gives the same perspective that a call to 'efs_open()' or
 *   'efs_opendir()' would give of the symlink. These are the correct semantics
 *   for most applications wanting to find out about files. The efs_lstat()
 *   call on the other hand gives the information about the symlink itself.
 *
 *   The boot_efs_stat function returns the following information in fs_stat:\n
 * @code
 *   struct fs_stat {
 *      fs_dev_t         st_dev;
 *      fs_inode_t       st_ino;
 *      fs_mode_t        st_mode;
 *      fs_nlink_t       st_nlink;
 *      fs_size_t        st_size;
 *      unsigned long    st_blksize;
 *      unsigned long    st_blocks;
 *      fs_time_t        st_atime;
 *      fs_time_t        st_mtime;
 *      fs_time_t        st_ctime;
 *      fs_devspecial_t  st_rdev;
 *      uint16           st_uid;
 *      uint16           st_gid;
 *    };
 * @endcode
 *
 * @par
 * - st_dev     : Unique Device ID among the mounted file systems.\n
 * - st_ino     : INode number associated with the file.\n
 * - st_mode    : Mode associated with the file.\n
 * - st_nlink   : Number of active links that are referencing this file. The
 *                space occupied by the file is released after its references
 *                reduces to 0.\n
 * - st_size    : File size in bytes.\n
 * - st_blksize : Block size, smallest allocation unit of the file system. Unit
 *                in which block count is represented.\n
 * - st_blocks  : Number of blocks allocated for this file in st_blksize
 *                units.\n
 * - st_atime   : Last access time, this is not updated, it might not have a
 *                proper value.\n
 * - st_mtime   : Last modification time, currently this would indicate the
 *                time when file was created.\n
 * - st_ctime   : Last status change time, currently this would indicate the
 *                time when file was created.\n
 * - st_rdev    : Major & Minor device number for special device files.\n
 * - st_uid     : Owner / User ID the file.\n
 * - st_gid     : Group ID of the file. The stored file data blocks are charged
 *                to the quota of this group ID.\n
 *
 *   These following macros can be used to test a given mode to determine its
 *   type, given the st_mode returned in the fs_stat structure.\n
 *
 * @par
 *   ::S_ISFIFO(m)  : Entry is a FIFO file type.\n
 *   ::S_ISCHR(m)   : Entry is a character device file.\n
 *   ::S_ISDIR(m)   : Entry is a directory.\n
 *   ::S_ISBLK(m)   : Entry is a block device file.\n
 *   ::S_ISREG(m)   : Entry is a regular file.\n
 *   ::S_ISLNK(m)   : Entry is a symbolic link.\n
 *   ::S_ISSOCK(m)  : Entry is a socket.\n
 *   ::S_ISITM(m)   : Entry is an item file.\n
 *
 *   ::S_ISSPECIAL(m) indicates if the file is one of special type of files, is
 *   ORd combination of S_ISFIFO, S_ISCHR, S_ISBLK, S_ISSOCK.
 *
 *
 * @param path
 *   The path is NULL terminated string indicating the path for which
 *   the stat information is needed.
 *
 * @param buf
 *   Pointer to the fs_stat structure where the return values are expected.
 *
 * @return
 *   Returns 0 on success and -1 on failure. On failure efs_errno is set to
 *   appropriate value indicating the reason of failure.
 *
 * @sa
 *   efs_lstat, efs_fstat, fs_dirent, efs_readdir
 *
 **********************************************************************/
int boot_efs_stat (const char *path, struct fs_stat *buf)
{
   return efs_stat (path,buf);
}

/*===========================================================================

**  Function :  boot_efs_get

** ==========================================================================
*/
/**
 * @par Synopsis:
 *  @code
 *    #include "fs_lib.h"
 *    #include "fs_sys_types.h"
 *    int boot_efs_get (const char *path, void *data, fs_size_t length);
 *  @endcode
 *
 * @par Description:
 *  Retrieve an EFS2 'special item' file. 'data' should be a pointer to a
 *  buffer, and 'length' should indicate the size of this buffer in number of
 *  bytes. This function will retrieve the item-file, and return its
 *  contents in the supplied buffer 'data'.
 *
 * @param path
 *  The path of the item-file.
 *
 * @param data
 *  The start-address of the data.
 *
 * @param length
 *  The length indicates the size of the buffer 'data' in bytes.
 *
 * @return
 *  Returns the number of bytes copied in the buffer, or -1 for an error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - The item or a component of the path does not exist.\n
 *    ::FS_ERANGE - The item file is larger than the specified buffer.\n
 *    ::EINVAL - Invalid parameter.\n
 *
 * @sa
 *    efs_put
 *
 **********************************************************************/
int boot_efs_get (const char *path, void *data, fs_size_t length)
{
  return efs_get (path,data,length);
}

