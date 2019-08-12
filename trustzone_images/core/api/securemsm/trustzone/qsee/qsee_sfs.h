#ifndef _QSEE_SFS_H_
#define _QSEE_SFS_H_

/**
@file qsee_sfs.h
@brief Secure File System with Encryption and Integrity Protection.
This file contains the definitions of the constants, data 
structures, and interfaces that provide methods to 
encrypt/integrity-protect and decrypt/verify content to the EFS.
*/

/*=============================================================================
              Copyright (c) 2000-2014 QUALCOMM Technologies Incorporated.
                      All Rights Reserved.
              Qualcomm Confidential and Proprietary

=============================================================================*/

/*=============================================================================

                       EDIT HISTORY FOR MODULE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_sfs.h#1 $ 
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/14   yh      Add qsee_sfs_error()
02/12/12   chm     Added support for SFS remove directory.
10/06/11    cz     corrected the checking of theqsee_sfs_open return
08/06/10   sd      {TechPubs] Edited Doxygen markup and comments.
08/28/09   ejt     removed #include "customer.h" and FEATURE flags for CMI compliance
12/17/08   ejt     removedqsee_sfs_SYSTEM_INTERFACE from SFS files
04/04/07   df      adding MINIMAL_DUAL_PD featurization
03/03/07   df      new functionqsee_sfs_ixfile_rm()
03/01/07   df      support for seek
01/09/06   df      local support for GetInfo
12/22/06   df      support for usfs
12/08/06   df      added cpp support
07/25/05   df      changedqsee_sfs_init prototype
07/22/05   df      added FEATURE_SEC_SFS
=============================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "comdef.h"

/**
   @addtogroupqsee_sfs_apis 
   @{
*/

typedef enum
{
  SFS_NO_ERROR = 0,
  SFS_INIT_FAIL = 1,              /* Failed to set OEM defined segment size, or failed to created OEM defined root folder*/
  SFS_INVALID_FILENAME = 2,       /* file name too long */
  SFS_NO_MEMORY = 3,              /* Malloc failed            */
  SFS_FS_ERROR = 4,               /* HLOS file system operation failed */
  SFS_NO_FILE = 5,                /* No such file             */
  SFS_RPMB_FAIL = 6,              /* RPMB is not ready, check if RPMB is provisioned or not     */
  SFS_CRYPTO_ERROR = 7,           /* Random number, hash, encrypt error caused by crypto engine */
  SFS_VERI_VER_ERROR = 8,         /* Version anti-rollback hack is detected */
  SFS_VERI_HMAC_ERROR = 9,        /* SFS segment sanity check failed, data is tampered */
  SFS_VERI_SEG_NAME_ERROR = 10,   /* SFS segment file name check failed */
  SFS_INDEX_RECOVER_ERROR = 11,   /* SFS can't find sanity index files for a TZ application, SFS can't do any operation for this TZ applications*/
  SFS_DATA_RECOVER_ERROR = 12,    /* SFS can't find sanity data segments, SFS can't do any operation for this file*/
  SFS_DATA_SEG_MISSING_ERROR = 13,/* SFS data segment was deleted maliciously */
  SFS_INDEX_SEG_MISSING_ERROR= 14,/* SFS index segment was deleted maliciously */
  SFS_EOF = 15,                   /* End of file */
}sfs_errno_enum_type;

/**
Retrieves the size of an open SFS file.

retrieves the file size of an open sfs file 

@param[in] fd    File descriptor.
@param[out] size Open file size.
  
@return
E_SUCCESS - File size is stored in the size parameter. \n 
E_FAILURE - Error occurred while getting the file size. 

*/
int qsee_sfs_getSize(int fd, uint32* size);


/**
Deprecated function, SFS doesn't support folder hierachy 

always returns E_SUCCESS \n

*/
int qsee_sfs_mkdir(const char* path);

/**
Deprecated function, SFS doesn't support folder hierachy 

always returns E_SUCCESS \n

*/
int qsee_sfs_rmdir(const char* path);

/**
Opens an SFS file.\ The options are specified by the File mode in the flag 
parameter.

In the SFS, opening a file does not actually do anything in the file 
system. However, if the file (along with the associated file segments) already 
exists and the file is created with the O_TRUNC mode, the associated 
subfiles are deleted. The first segment is created only when new bytes to be 
written begin arriving.
Note:The base directory must exist; otherwise, a NULL is returned.

@param[in] path      Fully qualified path of the filename to be
                     opened.
@param[in] flags     Bitmask field that is used to specify file 
                     modes.
                     - O_RDONLY - Open for read-only access.
                     - O_READWRITE - Open for read-write access.
                     - O_CREAT - Create the file if it does not
                       exist.
                     - O_TRUNC - Truncate the file to zero size
                       after opening.
                     - O_APPEND - Write operations occur at
                       the end of the file.

@return
Non Zero - Opened the file successfully. 
Zero - Error occurred while opening the file.

*/
int qsee_sfs_open(const char* path, int flags);

/**
Closes an open SFS file.\
All the resources used by the file are released.

@param[in] fd File descriptor.

@return
E_SUCCESS - Closed the file successfully. \n 
E_FAILURE - Error occurred while closing the file. 

*/
int qsee_sfs_close(int fd);

/**
Reads bytes from an encrypted SFS file that was previously opened via 
a call to IxFileMgr::OpenFile. 

The bytes are read from the current file position, and the file position 
advances by the number of read bytes. The SFS performs the necessary cipher and 
verification operations as bytes are being read from the file.  

@param[in] fd     File descriptor.
@param[in] buf    Pointer to the buffer to hold the bytes read.
@param[in] nbytes Number of bytes to read from the file.

@return 
Returns the number of bytes read from an SFS file. 
Returns -1 for the error case

*/
int qsee_sfs_read(int fd, char *buf, int nbytes);

/**
Writes bytes to an encrypted SFS file that was previously opened 
via a call to qsee_sfs_open. 

The bytes are written to the current file position. If the APPEND flag was set 
in qsee_sfs_open(), the bytes are written at the end of the file unless 
qsee_sfs_seek() was issued before qsee_sfs_write(). In this case, the data is written to 
the file position that qsee_sfs_seek() set. The file position advances by the number 
of bytes written. 

@param[in] fd     File descriptor.
@param[in] buf    Pointer to the buffer to be written.
@param[in] nbytes Number of bytes to write to the file.

@return
Returns the number of bytes written to an SFS file. 
Returns -1 for the error case

*/
int qsee_sfs_write(int fd, const char *buf, int nbytes);

/**
Removes an SFS file that was previously created through qsee_sfs_open().

@param[in] path Fully qualified path of the file to be deleted.

@return
E_SUCCESS - File removal is successful. \n 
E_FAILURE - Error occurred while removing the file. 

*/
int qsee_sfs_rm(const char *path);

/**
Moves the read location to point to n bytes from the start, current,
or end of the file. 

The subfile corresponding to the
current position is opened, verified, and decrypted. The segment 
position is advanced to the current position. 

@param[in] fd     File descriptor.
@param[in] offset File offset to seek in bytes.
@param[in] whence Indicates start, end, or current position.
                  - SEEK_SET - Start of the file.
                  - SEEK_CUR - Current file point.
                  - SEEK_END - End of the file.

@return
Returns the current location if successful, or -1 if an error occurred. 

*/
int qsee_sfs_seek(int fd, int32 offset, int whence);

/**
Check error indicator
Checks if the error indicator associated with fd is set, returning a value different from zero if it is.

This indicator is generally set by a previous operation on the SFS fd that failed

@param[in] fd     File descriptor. When 0 is returned by qsee_sfs_open(), 
                  will try to return the failure reason of last file open.


@return
A non-zero value is returned in the case that the error indicator associated with fd is set.
Otherwise, zero is returned.

*/
int qsee_sfs_error(int fd);


/** @} */   /* end_addtogroup qsee_sfs_apis */

#ifdef __cplusplus
}
#endif

#endif //_qsee_sfs_API_H_
