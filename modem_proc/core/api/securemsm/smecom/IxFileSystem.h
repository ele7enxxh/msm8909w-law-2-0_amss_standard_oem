#ifndef __IXFILESYSTEM_H__
#define __IXFILESYSTEM_H__

/**
   @file  IxFileSystem.h

   @brief 
   Public API for IxFile file systems.

   This header file defines the interface for file system API's

   Externalized Functions:
   None.

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                    I X F I L E S Y S T E M
                     H E A D E R  F I L E

Copyright (c) 2007-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/IxFileSystem.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------     ---     ------------------------------------------------------
01/13/11   leo     (Tech Pubs) Edited Doxygen comments and markup.
12/11/10   nk      Doxygenated.
11/01/07   jee     Initial release

===========================================================================*/

#include "IxFileSystemTypes.h"

#ifdef __cplusplus

/** @addtogroup SMECOMFILE 
@{
*/

/**
  @brief Defines the file system interface.
*/
class IxFileSystem 
{
public:

/** Virtual destructor.
*/
virtual ~IxFileSystem() {}

/**
   Opens a file in the filesystem.

   @param[in] path  Pointer to the file path.
   @param[in] oflag Open file type flag.

   @return
   Returns >= 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int Open( const char *path, int oflag, ... ) = 0;

/**
   Closes the file descriptor.

   The descriptor will no longer refer to a file and may be returned by
   subsequent calls to open.

   @param[in] filedes File descriptor obtained by the Open() function.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int Close( int filedes ) = 0;

/**
   Reads nbyte of data from the file associated with the specified file
   descriptor.

   @param[in] filedes File descriptor obtained by the Open() function.
   @param[in] buf     Pointer to the buffer where the read bytes from the
                      file are to be stored.
   @param[in] nbyte   Number of bytes to read from the file.

   @return
   Returns the number of bytes successfully read or -1 for an error.\n
   Zero is a valid result and generally indicates that the end of the file
   has been reached.\n
   A read may return fewer bytes than were requested, even if the data is
   available in the file.

   @dependencies
   None.
*/
virtual signed long int Read( int filedes, void *buf, unsigned long int nbyte ) = 0;

/**
   Writes nbyte of data to the file associated with the specified file
   descriptor.

   @param[in] filedes File descriptor obtained by the Open() function.
   @param[in] buf     Pointer to buffer that is to be written to the file.
   @param[in] nbyte   Number of bytes to write to the file.

   @return
   Returns the number of bytes successfully written or -1 for an error.\n
   Zero generally indicates that the filesystem is full( writing again
   results in a POSIX ENOSPC error).\n
   The write operation may write fewer bytes than were requested, even if
   space is available.

   @dependencies
   None.
*/
virtual signed long int Write( int filedes, const void *buf,
unsigned long int nbyte ) = 0;

/**
   Checks a user's permissions to access a file. Also checks if it is
   possible for the file to be accessed with the specified mode.

   @param[in] path  Pointer to the file path.
   @param[in] amode Open file type mode.

   @return
   Returns >= 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int Access( const char *path, int amode ) = 0;

/**
   Creates a new directory.

   @param[in] path Pointer to the file path.
   @param[in] mode Specifies the permission bits of the new directory.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int MkDir( const char *path, unsigned short mode ) = 0;

/**
   Returns additional information about the named file/directory.

   FStat() is used to get information about an opened file handle. LStat()
   and Stat() are very similar in that they both return information about a
   specified path name. They differ in how they treat the last component if
   it is a symlink. Stat() will dereference the symlink, returning the
   information on what the symlink points to. It gives the same perspective
   that a call to Open() or Opendir() would give of the symlink. These are
   the correct semantics for most applications wanting to find out about
   files.

   The LStat() call, on the other hand, provides information about any
   symlink. This call should be used by a type of application that must
   be aware of symlinks. The most common is a type of file browser that
   provides visibility into the real file types, including the presence of
   symlinks.

   @note1hang Stat() follows symlinks. See LStat() if you do not want to
   follow the symlinks.

   @param[in] path Pointer to the file path.
   @param[in,out] buf Pointer to the file status type. On success, the
                      fields of this buffer are filled in.

   @return
   Returns >= 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int Stat( const char *path, IxFileStatType *buf ) = 0;

/**
   Returns additional information about an open file. Otherwise, this
   function is similar to Stat().

   @note1hang FStat() follows symlinks.

   @param[in] fildes File descriptor obtained by the Open() function.
   @param[in,out] buf Pointer to the file stat type. On success, the
                      fields of this buffer are filled in.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int FStat( int fildes, IxFileStatType *buf ) = 0;

/**
   Returns information on the sybmolic link.

   If the final component of the path is not a symlink, this is identical to
   Stat(). If the final component is a symlink, FStat() returns information
   about the file pointed to by the symlink. LStat() returns information
   about the symlink itself.

   @note1hang LStat() does NOT follow symlinks. See Stat() if you want the
   symlinks as well.

   @param[in] path Pointer to the file path.
   @param[in,out] buf Pointer to the file stat type. On success, the fields
                      of this buffer are filled in.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int LStat( const char *path, IxFileStatType *buf ) = 0;

/**
   Obtains information about an entire filesystem.

   @param[in] path  Pointer to the file path name. The path name must be
                    a valid path in the file system (the root directory
                    is valid).
   @param[in,out] buf Pointer to the file stat type. On success, the fields
                      of this buffer are filled in.

   @return
   Returns 0 and fills in buf on success, otherwise returns -1.

   @dependencies
   None.
*/
virtual int StatVfs( const char *path, IxFileStatFSType *buf ) = 0;

/**
   Obtatins status information about an entire filesystem using a file
   descriptor for a handle. Otherwise, it is similar to StatVfs().

   @param[in] fildes File descriptor obtained by the Open() function.
      @param[in] buf Pointer to the file stat type. On success, the fields
                     of this buffer are filled in.

   @return
   Returns 0 and fills in buf on success, otherwise returns -1.

   @dependencies
   None.
*/
virtual int FStatVfs( int fildes, IxFileStatFSType *buf ) = 0;


/**
   Changes the file offset for the opened file descriptor.

   @param[in] filedes File descriptor obtained by the Open() function.
   @param[in] offset  New offset of the file.
   @param[in] whence  Indicates how the new offset is computed:
                       - IxFile_SEEK_SET -- Set to offset.
                       - IxFile_SEEK_CUR -- Set to offset + current position.
                       - IxFile_SEEK_END -- Set to offset + file size.

   @return
   On success, LSeek() returns the resulting offset as bytes from the
   beginning of the file. Otherwise, (off_t ) -1 is returned.

   @dependencies
   None.
*/
    virtual signed long int LSeek( int filedes, signed long int offset,
      int whence ) = 0;


/**
   Truncates a file to a specified length. The file will be extended or
   contracted to the newly specified size.

   @param[in] path   Path of the file whose length is to be changed.
   @param[in] length New size of the file.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual signed long int Truncate( const char *path,
signed long int length ) = 0;

/**
   Truncates an open file. Similar to Truncate(), but the file must be
   open for writing.

   @param[in] fildes File descriptor of the open file.
   @param[in] length New size of the file.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual signed long int FTruncate( int fildes, signed long int length ) = 0;

/**
   Unlinks a file and deletes the name in the filesystem. If this is the
   only reference to this file, the file itself will also be deleted.
   Unlink() can be used on all file system objects except for directories.
   Use RmDir() to remove directories.

   @param[in] path File that is to be unlinked.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int Unlink( const char *path ) = 0;

/**
   Opens a directory for iteration. The returned pointer is an opaque handle
   that can be used to iterate through the entries of the directory. The
   entries must be read with ReadDir() and closed with CloseDir().

   @param[in] dirname Base path under which to iterate.

   @return
   Returns an opaque pointer on success, NULL on error.

   @dependencies
   None.
*/
virtual unsigned long int OpenDir( const char *dirname ) = 0;

/**
   Reads the next entry from the opened directory.

   @param[in] dirHandle Iterator handle obtained by the OpenDir() function.
   @param[in] dirEnt    Pointer to the IxFileDirEntT structure to fill in.

   @return
   On success, returns a pointer to a struct IxFileDirEntT, which contains
   the field d_name, which is the name of this directory component.\n
   Returns a NULL pointer on error or end of file.

   @dependencies
   None.
*/
virtual int ReadDir( unsigned long int dirHandle, IxFileDirEntT *dirEnt ) = 0;

/**
   Closes a directory iterator.

   @param[in] dirhandle Iterator handle obtained by the OpenDir() function.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int CloseDir( unsigned long int dirhandle ) = 0;

/**
   Renames a file. Currently, only regular files can be renamed. oldpath and
   newpath do not have to be in the same directory, but they must be on the
   same filesystem device.

   @param[in] oldpath Old path name.
   @param[in] newpath New path name.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int Rename( const char *oldpath, const char *newpath ) = 0;

/**
   Removes a directory.

   @param[in] path Directory that is to be removed.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
        Only empty directories can be removed.
*/
virtual int RmDir( const char *path ) = 0;

/**
   Changes the mode.

   @param[in] path File path in which to change the mode.
   @param[in] mode Changes the directory mode.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int ChMod( const char *path, unsigned short mode ) = 0;

/**
   Creates a symbolic link containing the string oldpath. Future
   references to newpath act as if oldpath were substituted for the name
   component of the path. If oldpath is a relative path, it is interpreted
   relative to the directory in which newpath resides.

   @param[in] oldpath Old path name.
   @param[in] newpath New path name.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int SymLink( const char *oldpath, const char *newpath ) = 0;

/**
   Reads the value of a symbolic link. Up to bufsiz bytes of data is copied
   into buf.

   @note1hang The result is not NULL-terminated.

   @param[in] path   Pointer to the file path.
   @param[in] buf    Pointer to the buffer.
   @param[in] bufsiz Buffer size.

   @return
   Returns the number of bytes placed into buf on success, -1 on error.

   @dependencies
   None.
*/
virtual int ReadLink( const char *path, char *buf, unsigned long int bufsiz ) = 0;

/**
   Initializes the filesystem and mounts the root filesystem.

   @return
   Returns 0 on success, otherwise returns an error 
   code( does not set errno ).

   @dependencies
   None.
*/
virtual int Initialize( void ) = 0;

/**
   Places the filesystem in a state where it is safe to remove power.

   @return
   Returns 0 on success, otherwise returns an error 
   code( does not set errno ).

   @dependencies
   None.
*/
virtual int PowerDown( void ) = 0;

/**
   Mounts a file system at a specified directory. Unlike in POSIX, the
   specified name is a parsed string describing the source of the file
   system.

   The special argument must be an integer index for the specified file
   system. The fstype argument must be the name of the file system type
   (such as romfs). At this time, flags and data are zero.

   @param[in] special Index integer for the filesystem.
   @param[in] dir     Pointer to the directory name.
   @param[in] fstype  File system type.
   @param[in] flags   Flag for file mounting.
   @param[in] data    Data buffer.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int Mount( const char *special, const char *dir,
const char *fstype, int flags, const void *data ) = 0;

/**
   Unmounts a previously mounted filesystem. 

   @param[in] target Pointer to the target.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int Umount( const char *target ) = 0;

/**
   Remounts an unmounted file. Relocates where an existing
   mountpoint is in the filesystem.

   @param[in] oldtarget Pointer to the old target.
   @param[in] newtarget Pointer to the new target.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int Remount( const char *oldtarget, const char *newtarget ) = 0;

/**
   Resets the filesystem and stops the system.

   @return
   None.

   @dependencies
   None.
*/
virtual void Reset( void ) = 0;

/**
   Changes the owner ID or group ID of the specified file or directory.

   @param[in] path    Pointer to the file path.
   @param[in] uid_val User (owner) ID value.
   @param[in] gid_val Group ID value.

   @return
   Returns 0 on success, -1 on error.

   @dependencies
   None.
*/
virtual int ChOwn( const char *path, int uid_val, int gid_val ) = 0;

/**
   Gets the last error.

   @return
   Returns the value of the last operational error condition.

   @dependencies
   None.
*/
virtual int GetLastError( void ) = 0;
};

/** @} */ /* end_group SMECOMFILE */

#endif /* __cplusplus */

#endif /* End of __IXFILESYSTEM_H__ */
