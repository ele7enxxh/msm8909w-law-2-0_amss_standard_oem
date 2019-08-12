#ifndef IXFILE_H
#define IXFILE_H
/**
   @file IxFile.h

   @brief This header file defines the interface to IxFile.

   @sa IxFileMgr.h
*/

/*===========================================================================
                             I X F I L E
                        H E A D E R  F I L E

Copyright (c) 2005 - 2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                       EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/IxFile.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/11   nk     Removed wstring inclusion and included standard 
                  kernel stringl.h
01/11/11   leo    (Tech Pubs) Edited Doxygen comments and markup.
12/10/10   nk     Doxygenated.

===========================================================================*/

#ifdef __cplusplus
#include "IxBase.h"
#endif
#include "rtedef.h"
#include "comdef.h"
#include "IxErrno.h"
#include "stringl.h"

#include "IxFileSystem.h"

/**
   @addtogroup SMECOMFILE 
   @{
*/

/**
@name SFS Drive Prefixes and Replacement Strings
@{
*/

/**
   Defines the system SFS drive.
*/
#define SYS_SFS_DRIVE  "sys_sfs:"

/**
   Defines the user SFS drive.
*/
#define USR_SFS_DRIVE  "usr_sfs:"

/**
   Defines the user system SFS drive map.
*/
#define USR_SFS_DRIVE_MAP ""

/** @} */ /* end_name SFS Drive Strings */

/**
   @brief Placeholder for file information extension data.
*/
typedef struct
{
int        nStructSize;      /**< Size of the structure.             */
char       attrib;           /**< Bit flags of file attribute types. */
uint32     dwCreationDate;   /**< File creation date in ms since the
                                  epoch.                             */
uint32     dwSize;           /**< Size of the file.                  */
char  *    pszFile;          /**< ASCII name of the file.            */
int        nMaxFile;         /**< Length of the ASCII name.          */
wchar *    pszDescription;   /**< File description.                  */
int        nDescriptionSize; /**< Length of the file description.    */
uint32 *   pClasses;         /**< AEECLSIDs that own/use this file.  */
int        nClassesSize;     /**< Size of the pClasses list.         */
int        fileType;         /**< File type.                         */
} IxFileInfoEx;

/**
   @brief Placeholder for file information data.
*/
typedef struct
{
char     attrib;                /**< Bit flags of file attribute types. */
uint32   dwCreationDate;        /**< File creation date, in seconds
                                     since Jan. 6, 1980 GMT; may be
                                     NULL.                              */
uint32   dwSize;                /**< File size in bytes.                */
char  szName[IXFILE_NAME_MAX];  /**< File name.                         */
} IxFileInfo;

/* todo To be removed. */
/**
   @brief Placeholder for file information use data.
*/
typedef struct
{
uint16            wMaxFiles;     /**< Maximum number of allowed files to
                                      create.                             */
uint16            wFilesUsed;    /**< Number of files currenty used.      */
uint32            dwMaxSpace;    /**< Maximum space allowed to consume.   */
uint32            dwSpaceUsed;   /**< Total space currently used.         */
} IxFileUseInfo;

/* todo To be removed */
/** 
Placeholder for the file type.
*/
typedef enum {
IxFile_SEEK_START,   /**< Seek to start of file.         */
IxFile_SEEK_END,     /**< Seek to end of file.           */
IxFile_SEEK_CURRENT  /**< Use current file position.     */
} IxFileSeekType;

/**
  Handle for the directory. 
*/
typedef unsigned int IxFileDirHandleT;

#ifdef __cplusplus


/**
   @brief Class to provide platform-independent file manipulation.

   IxFile provides the base class that is used as an interface to manipulate
   files. The interface provides generic functionality to read, seek, and write
   to a file. This file interface is returned from the IxFileMgr class,
   providing a separation from file access and from the underlying file system.
*/
class IxFile : public IxBase {

friend class IxFileMgr;

public:

/** Destructor.
*/
virtual ~IxFile() {}

/**
   Adds a reference count so the destructor does not delete shallow copied
   objects.

   This is used if the client wants to retain multiple pointers to this object
   that are shared, but does not want to remove the underlying references this
   object is holding. This is used to prevent deletion of shallow copied objects.

   @return 
   Returns the current reference count + 1.

   @dependencies
   None.

   @sa
   Release()
*/
virtual unsigned long AddRef(void) = 0;

/**
   Releases the reference count. 

   This is automatically called by the destructor, so it must be used with
   caution.

   @note1hang This operation is not normally called by the client.

   @return 
   Returns the current reference count - 1.

   @dependencies
   None.

   @sa
   AddRef()
*/
virtual unsigned long Release(void) = 0;

/**
   Gets the internal serialized state of the object.

   @param[out] stateDataPtr    Pointer to the serialized state.
   @param[out] stateDataLenPtr Pointer to the length of the serialized state.

   @return
   None.

   @dependencies
   None.

   @sa
   SetState()
*/
virtual void GetState(uint8** stateDataPtr, uint32* stateDataLenPtr) = 0;

/**
   Sets the internal serialized state of the object.

   @param[in] stateDataPtr Pointer to the serialized state.
   @param[in] stateDataLen Length of the serialized state.

   @return
   None.

   @dependencies
   None.

   @sa
   GetState()
*/
virtual void SetState(uint8* stateDataPtr, uint32 stateDataLen) = 0;

/**
   Reads nWant bytes of data from the file.

   @param[in] pDest Pointer to the destination buffer to which to read.
   @param[in] nWant Length of destination buffer.

   @return
   Returns the number of bytes successfully read, or -1 for an error.\n
   Zero is a valid result and generally indicates that the end of file has been
   reached.
*/
virtual int32 Read(void * pDest, uint32 nWant) = 0;

/**
   Writes dwCount of data to the file.

   @param[in] pBuffer Pointer to the buffer from which to write.
   @param[in] dwCount Length of the pBuffer in bytes.

   @return
   Returns the number of bytes successfully written, or -1 for an error.\n
   Zero indicates the filesystem is full.

   @dependencies
   None.
*/
virtual uint32 Write(const void * pBuffer, uint32 dwCount) = 0;

/**
   Gets the file creation date, file size, filename, and file attributes of the
   file pointed to by this object.

   @param[out] pInfo Pointer to where to obtain file information.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType GetInfo(IxFileInfo * pInfo) = 0;


/**
   Moves the file pointer associated with this object by a given distance from a
   specified origin.

   The specified origin (seek) can take three values:
   - IxFile_SEEK_START
   - IxFile_SEEK_END
   - IxFile_SEEK_CURRENT

   The move distance can be specified in positive or negative directions
   (values). For example, a negative direction is used when seeking from the
   end of the file (_SEEK_END). If the seek type is _SEEK_CURRENT and the move
   distance is 0 (zero), this acts as a tell operation and returns the current
   position of the file pointer. Otherwise, the file pointer is moved by the
   given distance from the specified origin.

   If the file was opened in the _OFM_READ mode, this operation succeeds if the
   move distance from the specified origin is within the boundaries of the file.
   If the file was opened in the _OFM_READWRITE mode and the move distance
   points to a location before the beginning of the file, this operation fails.
   If the move distance from the specified origin is past the end of the file,
   the file size is extended to the point of the move distance (from the seek
   origin).

   @param[in] seek     Origin from which to seek.
   @param[in] position Position from seek origin to seek (can be + or -).

   @return
   E_SUCCESS      -- Success.\n
    E_OUT_OF_RANGE -- Position is out of range.\n
    E_*            -- All other errors (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType Seek(IxFileSeekType seek, int32 position) = 0;

/**
   Truncates the file associated with this object to a position specified by
   the truncate_pos parameter.

   If the file is larger than the specified length, the extra data is discarded.
   If the file is shorter than the specified length, its size is increased and the
   extended area is zero-filled.

   @param[in] truncate_pos Length of the file to truncate.

   @return
   E_SUCCESS       -- Success.\n
   E_NOT_SUPPORTED -- Unsupported operation.\n
   E_*             -- All other errors (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   To truncate a file, the file must be open for writing.
*/
virtual IxErrnoType Truncate(uint32 truncate_pos) = 0;


/**
   Gets the extended information of the file associated with this object. The
   extended file information is contained in IxFileInfoEx.

   @param[out] pi File information to read.

   @return
   E_SUCCESS       -- Success.\n
   E_NOT_SUPPORTED -- Unsupported operation.\n
   E_*             -- All other errors (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType GetInfoEx(IxFileInfoEx * pi) = 0;


/**
   This operation is currently unsupported and is a placeholder for future
   extensibility.

   @deprecated To be removed.

   @param[in] nSize File cache size.

   @return
   E_NOT_SUPPORTED -- Unsupported operation.

   @dependencies
   None.
*/
virtual IxErrnoType SetCacheSize(int nSize) = 0;

/**
   This operation is currently unsupported and is a placeholder for future
   extensibility.

   @deprecated To be removed.

   @param[in] pStart      Pointer to file start position.
   @param[in] dwSize      Buffer size.
   @param[in] protections File protection information.
   @param[in] flags       File flag information.
   @param[in] dwOffset    File offset information.

   @return
   E_NOT_SUPPORTED -- Unsupported operation.

   @dependencies
   None.
*/
virtual void * Map(void * pStart, uint32 dwSize, int protections,
int flags, uint32 dwOffset) = 0;

/**
   Obtains the current value of the file position indicator.

   @return
   Returns the position in the file relative to IxFileSeekType. The position
   can be a negative value.

   @dependencies
   None.

   @sa
   Seek()
*/
virtual int32 Tell() = 0;

};


#endif /* __cplusplus */

/** @} */ /* end_group SMECOMFILE */

/*------------------------------------------------------------------------
C WRAPPERS
------------------------------------------------------------------------*/

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif

/**
   @addtogroup CAPI 
@{
*/

/**
   @name IXFILE_C_API
   These functions are C wrappers for a subset of the IxFile API. The
   name of each C wrapper function makes it clear as to which C++ method
   it corresponds.
@{
*/

/** See IxFile::~IxFile. */
void ixfile_delete(void *pFile);

/** See IxFile::AddRef. */
unsigned long ixfile_add_ref(void *pFile);

/** See IxFile::Release. */
unsigned long ixfile_release(void *pFile);

/** See IxFile::Read. */
int32 ixfile_read(void *pFile, void *pDest, uint32 nWant);

/** See IxFile::Write. */
uint32 ixfile_write(void *pFile, const void *pBuffer,uint32 dwCount);

/** See IxFile::Seek. */
IxErrnoType ixfile_seek(void * pFile, IxFileSeekType seek, int32 position);

/** See IxFile::Truncate. */
IxErrnoType ixfile_truncate(void * pFile, uint32 truncate_pos);

/** See IxFile::GetInfoEx. */
IxErrnoType   ixfile_get_info_ex(void * pFile, IxFileInfoEx * pi);

/** @} */  /* end_name IXFILE_C_API */
/** @} */  /* end_group CAPI */

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif

/** @} */  /* end_name IXFILE_C_API */
/** @} */  /* end_group CAPI */

#endif
