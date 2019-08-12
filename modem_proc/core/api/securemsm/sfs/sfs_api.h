#ifndef _SFS_API_H_
#define _SFS_API_H_

/**
@file sfs_api.h
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

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/sfs/sfs_api.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/10   sd      {TechPubs] Edited Doxygen markup and comments.
08/28/09   ejt     removed #include "customer.h" and FEATURE flags for CMI compliance
12/17/08   ejt     removed SFS_SYSTEM_INTERFACE from SFS files
04/04/07   df      adding MINIMAL_DUAL_PD featurization
03/03/07   df      new function sfs_ixfile_rm()
03/01/07   df      support for seek
01/09/06   df      local support for GetInfo
12/22/06   df      support for usfs
12/08/06   df      added cpp support
07/25/05   df      changed sfs_init prototype
07/22/05   df      added FEATURE_SEC_SFS
=============================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "secerrno.h"
#define SFS_MAX_FILENAME_SIZE     128

typedef struct
{
  char    file_name[SFS_MAX_FILENAME_SIZE];  /* folder path is not included, file name only */
  uint32  file_size;
}sfs_file_info;

typedef struct
{
  uint32         file_num;
  sfs_file_info *file_info;
}sfs_file_info_list;

/**
   @addtogroup sfs_apis 
   @{
*/

/**
Initializes the seg_size parameter.

The seg_size parameter must be called before using SFS; 
otherwise, the default SFS segment size (1024 bytes) is 
used. 

@param[in] seg_size Segment size.
  
@return
None.

@dependencies
None.
*/
void sfs_init(uint32 seg_size);

/**
Retrieves the size of an open SFS file.

This non-published function is available only if the SEC_PD() retrieved the 
file size of an open SFS file.   

@param[in] fd    File descriptor.
@param[out] size Open file size.
  
@return
E_SUCCESS - File size is stored in the size parameter. \n 
E_FAILURE - Error occurred while getting the file size. 

@dependencies
None.
*/
secerrno_enum_type sfs_getSize(int fd, uint32* size);

/**
Opens an SFS file with ixfile interface, and creates a subdirectory under the 
base path, if necessary. If the file does not exist, it is created. If the file 
already exists, the file may be appended to or truncated. The options are 
specified by the File mode in the flag parameter. 

The subdirectory name is constructed based on the current base path. If the 
directory already exists in the underlying file system, a subdirectory with a 
mangled name is created under the parent directory, and the SFS file is stored 
in this new subdirectory. If this subdirectory already exists, it is reused so 
there is only one subdirectory per parent directory. 
  
An SFS file cannot be created directly under the root without a parent 
directory. The name of the file being created is also mangled in the 
underlying file system. The name-mangling conversion consists of a hash function 
followed by base64 encoding, with the resulting output from this process used as 
the created filename that is written to the file system. 

In the SFS, opening a file does not actually do anything in the file 
system. However, if the file (along with the associated file segments) already 
exists and the file is created with the IxFILE_OFM_TRUNCATE mode, the associated 
subfiles are deleted. The first segment is created only when new bytes to be 
written begin arriving.

@param[in] path      Fully qualified path of the filename to be opened.
@param[in] flags     Bitmask field that is used to specify file modes.
                     - O_RDONLY - Open for read-only access.
                     - O_READWRITE - Open for read-write access.
                     - O_CREAT - Create the file if it does not exist.
                     - O_TRUNC - Truncate the file to zero size
                       after opening.
                     - O_APPEND - Write operations occur
                       at the end of the file.

@return
Returns a valid file descriptor, or -1 if an error occurred. 

@dependencies
The base directory must exist; otherwise, a NULL is returned.
*/
int sfs_ixfile_open(const char* path, int flags);

/**
Removes an SFS file that originated from the IxFile interface.

The file must be created through sfs_ixfile_open() before it can be removed. 
Otherwise, an error is returned. See sfs_ixfile_open() for more 
information. 

@param[in] path Fully qualified path of the file to be deleted.

@return
E_SUCCESS - SFS file removal is successful. \n 
E_FAILURE - Error occurred while removing the file. 

@dependencies
  None.
*/
int sfs_ixfile_rm(const char *path);

/**
Creates a parent directory in the underlying file system in which to place the 
SFS files. 

The directory name created by this call is 
mangled. The name-mangling conversion consists of a hash 
function followed by base64 encoding, with the resulting output 
from this process used as the created pathname that is written 
to the file system. 

@param[in] path    Fully qualified path of the directory to be created.

@return
E_SUCCESS - Directory creation is sucessful. \n 
E_FAILURE - Error occurred while creating the directory. 

@dependencies
  None.
*/
int sfs_mkdir(const char* path);

/**
Opens an SFS file.\ The options are specified by the File mode in the flag 
parameter.

In the SFS, opening a file does not actually do anything in the file 
system. However, if the file (along with the associated file segments) already 
exists and the file is created with the IxFILE_OFM_TRUNCATE mode, the associated 
subfiles are deleted. The first segment is created only when new bytes to be 
written begin arriving.

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
E_SUCCESS - Opened the file successfully. \n 
E_FAILURE - Error occurred while opening the file. 

@dependencies
The base directory must exist; otherwise, a NULL is returned.
*/
int sfs_open(const char* path, int flags);

/**
Closes an open SFS file.\ 
All the resources used by the file are released.

@param[in] fd File descriptor.

@return
E_SUCCESS - Closed the file successfully. \n 
E_FAILURE - Error occurred while closing the file. 

@dependencies
None.
*/
int sfs_close(int fd);

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

@dependencies
None.
*/
int sfs_read(int fd, char *buf, int nbytes);

/**
Writes bytes to an encrypted SFS file that was previously opened 
via a call to sfs_open. 

The bytes are written to the current file position. If the APPEND flag was set 
in sfs_open(), the bytes are written at the end of the file unless 
sfs_seek() was issued before sfs_write(). In this case, the data is written to 
the file position that sfs_seek() set. The file position advances by the number 
of bytes written. 

@param[in] fd     File descriptor.
@param[in] buf    Pointer to the buffer to be written.
@param[in] nbytes Number of bytes to write to the file.

@return
Returns the number of bytes written to an SFS file. 

@dependencies
None.
*/
int sfs_write(int fd, const char *buf, int nbytes);

/**
Removes an SFS file that was previously created through sfs_open().

@param[in] path Fully qualified path of the file to be deleted.

@return
E_SUCCESS - File removal is successful. \n 
E_FAILURE - Error occurred while removing the file. 

@dependencies
None.
*/
int sfs_rm(const char *path);

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

@dependencies
None.
*/
int sfs_seek(int fd, int32 offset, int whence);

/**
Returns all file names under a SFS directory.

@param[in] path               Fully qualified path of the directory, should be same
                              path be used for sfs_mkdir()
@param[in/out] file_info_list Data container to hold the file names under a SFS directly
                              see sfs_file_info_list and sfs_file_info data struct

@return
  E_SUCCESS     -- File name list is successfully returned.
  E_INVALID_ARG -- NULL pointer or path length exceeds SFS_MAX_FILENAME_SIZE.
  E_NO_MEMORY   -- Internal Malloc failed.
  E_BAD_DATA    -- SFS folder metadata is corrupted.
  E_FAILURE     -- Other error. 

@dependencies
None.
*/
int sfs_get_filelist(const char *path, sfs_file_info_list *file_info_list);

/**
Free up a file_info_list file name list container.

@param[in] file_info_list  Pointer to a sfs_file_info_list which is returned 
                           after sfs_get_filelist

@return
None

@dependencies
None.
*/
void sfs_clean_filelist(sfs_file_info_list *file_info_list);

/** @} */   /* end_addtogroup sfs_apis */

#ifdef __cplusplus
}
#endif

#endif //_SFS_API_H_
