#ifndef IXFILEMGR_H
#define IXFILEMGR_H

/**
   @file IxFileMgr.h

   @brief This header file defines the interface to IxFileMgr

   @sa IxFile.h
*/

/*===========================================================================
                          I X  F I L E M A N A G E R
                             H E A D E R  F I L E

Copyright (c) 2006-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/IxFileMgr.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $


when       who      what, where, why
--------   ---      -----------------------------------------------------
01/12/11   leo      (Tech Pubs) Edited Doxygen comments and markup.
12/07/10   nk       Doxygenated.

===========================================================================*/

#include "IxFileSystem.h"
#ifdef __cplusplus
#include "IxBase.h"
#endif

#include "rtedef.h"
#include "comdef.h"
#include "IxErrno.h"
#include "IxFileServer.h"


#ifdef __cplusplus

/** @addtogroup SMECOMFILE 
    @{
*/

/**
   @brief Class to provide platform-independent file system manipulation.

   IxFileMgr provides a platform-independent interface to an underlying file
   system. Currently, IxFileMgr provides support for EFS2 and SFS file systems.
   IxFileMgr is closely tied to IxFile.
*/
class IxFileMgr : public IxBase 
{
public:

/** Virtual destructor.
*/
virtual ~IxFileMgr( );

/** Default constructor.
*/
IxFileMgr( );

/**
   Initializes IxFileMgr to use a filesystem.

   @param[in] pFS Filesystem with which to initialize IxFileMgr.

   @return
   None.

   @dependencies
   None.
*/
IxFileMgr( IxFileSystem *pFS );

/**
   Adds a reference count so the destructor does not delete shallow copied
   objects.

   This is used if the client wants to retain multiple pointers to this object
   that are shared, but does not want to remove the underlying references this
   object is holding. This prevents deletion of shallow copied objects.

   @return
   Returns the current reference count + 1.

   @dependencies
   None.

   @sa
   Release()
*/
virtual unsigned long AddRef(void);

/**
   Releases the reference count. This is automatically called by the destructor,
   so it must be used with caution.

   @note1hang This operation is not normally called by the client.

   @return
   Returns the current reference count - 1.

   @dependencies
   None.

   @sa
   AddRef()
*/
virtual unsigned long Release(void);

/**
   Gets the internal serialized state of the object through passed pointers.

   @param[out] stateDataPtr    Pointer to the serialized state.
   @param[out] stateDataLenPtr Pointer to the length of the serialized state.

   @return
   None.

   @dependencies
   None.

   @sa
   SetState()
*/
virtual void GetState(uint8** stateDataPtr, uint32* stateDataLenPtr) {;};

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
virtual void SetState(uint8* stateDataPtr, uint32 stateDataLen) {;};

/**
   Opens a file in a specified mode.

   @param[in] pszFile Full path and name of the file to open. The file may
                      reside on an EFS or SFS file system, and the path is used
                      to determine the correct filesystem.
   @param[in] mode Mode in which to open the file.

   @return
   Returns the IxFile pointer.

   @dependencies
   None.
*/
virtual IxFile *OpenFile(const char *pszFile, IxFileOpenFileMode mode );


/**
   This operation is currently unsupported and is a placeholder for future
   extensibility.

   @deprecated To be removed.

   @param[in] pszName Full path and name of the file.
   @param[in] pInfo   Pointer to where to obtain file information. 

   @return
   E_NOT_SUPPORTED -- Unsupported operation on the file.

   @dependencies
   None.
*/
virtual IxErrnoType GetInfo(const char *pszName, IxFileInfo * pInfo);

/**
   Deletes a file in the filesystem. This cannot be used on directories.

   @param[in] pszName Full path and name of the file to be deleted.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.

   @sa
   RmDir() for directory deletion.
*/
virtual IxErrnoType Remove(const char *pszName);

/**
   Creates a directory specified by a directory name. The directory is set with
   all permissions (ALLPERMS: read, write, and execute).

   @param[in] pszDir Full path and name of the directory to be created.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType MkDir(const char *pszDir);

/**
   Removes a directory identified by a specified directory name.

   @param[in] pszDir Full path and name of the directory to be deleted.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType RmDir(const char *pszDir);

/**
   Tests if a file or directory exists.

   @param[in] pszName Full path and name of the file/directory to check.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType Test( const char *pszName );

/**
   Retrieves the last error for the task that was saved on a file operation.

   @return
   This operation is currently unsupported and will always return E_FAILURE.

   @dependencies
   None.
*/
virtual IxErrnoType GetLastError(void);

/**
   Renames the source file to the destination file name. If the destination file
   name already exists, it will be replaced. If the destination file is open, an
   error will be returned.

   @param[in] pszSrc  Full path and filename of the source.
   @param[in] pszDest Full path and filename of the destination.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType Rename(const char * pszSrc, const char * pszDest);

/**
   Retrieves the extended information for a file.

   @param[in] pszName Full path and name of the file or directory for which
                      to get extended information.
   @param[out] pi Extended file information.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType GetInfoEx(const char * pszName, IxFileInfoEx * pi);

/**
   Opens a directory corresponding to the directory name. The pDirHandle is
   positioned at the first entry in the directory.

   @param[in] pszName     Full path and name of the directory to open.
   @param[out] pDirHandle Handle to the open directory.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
   virtual IxErrnoType OpenDir(const char * pszName,
                               IxFileDirHandleT * pDirHandle);

/**
   Reads a directory.

   This function reads IxFileDirEntT information for every file and subdirectory
   contained in the specified directory. The postion of the dirHandle is set to
   the next entry for every call to ReadDir(). This allows the caller to iterate
   through all files and directories.

   @param[in] dirHandle Directory handle.
   @param[in] pDirEnt   Information about the directory entry.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
   virtual IxErrnoType ReadDir(IxFileDirHandleT dirHandle,
                               IxFileDirEntT *pDirEnt );

/**
   Closes an open directory.

   @param[in] dirHandle Directory handle. 

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType CloseDir( IxFileDirHandleT dirHandle );

/**
   Retrieves the total and free bytes that are available for a specfic
   directory.

   This function can also be used to determine the amount of free space on the
   file system if the path is set to a file system base.

   @param[in] pPath   Path to where to determine the free space.
   @param[out] pTotal Total bytes used.
   @param[out] pFree  Free bytes available.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
   virtual IxErrnoType GetFreeSpaceEx(const char *pPath,
                                      uint32 * pTotal, uint32 *pFree);

/**
   This operation is currently unsupported and is a placeholder for future
   extensibility.

   @return
   Returns zero.

   @dependencies
   None.
*/
virtual uint32 GetFreeSpace(uint32 * pdwTotal);

/**
   This operation is currently unsupported and is a placeholder for future
   extensibility.

   @return
   E_NOT_SUPPORTED -- Unsupported operation on the file.

   @dependencies
   None.
*/
virtual IxErrnoType SetDescription(const char * pszName, wchar * pszDesc);

/**
   This operation is currently unsupported and is a placeholder for future
   extensibility.

   @return
   E_NOT_SUPPORTED -- Unsupported operation on the file.

   @dependencies
   None.
*/
virtual IxErrnoType Use( const char * pszName,boolean bUse );

/**
   This operation is currently unsupported and is a placeholder for future
   extensibility.

   @return
   E_NOT_SUPPORTED -- Unsupported operation on the file.

   @dependencies
   None.
*/
virtual IxErrnoType GetFileUseInfo( IxFileUseInfo *pfu );

/**
   This operation is currently unsupported and is a placeholder for future
   extensibility.

   @return
   E_NOT_SUPPORTED -- Unsupported operation on the file.

   @dependencies
   None.
*/
   virtual IxErrnoType ResolvePath(const char *cpszIn, char *pszOut,
                                   int *pnOutLen);

/**
   This operation is currently unsupported and is a placeholder for future
   extensibility.

   @return
   E_NOT_SUPPORTED -- Unsupported operation on the file.

   @dependencies
   None.
*/
   virtual IxErrnoType CheckPathAccess(const char *cpszIn,
                                       uint32 dwDesiredRights,
                                       uint32 *pdwActualRights);
/** @cond
*/
private:
    /*
   @brief 
   Placeholder for directory information for internal data.
*/
typedef struct
{
IxFileDirHandleT handle;
IxFileSystem    *pFS;
} DirInfoInternalT;

   uint32 m_nRefs;  /* Reference variable. */

/*
   Opens an SFS file in a specified mode.

   @param[in] pszFile Full path and name of the file to open. The file may
                      reside on an EFS or SFS file system, and the path is
                      used to determine the correct filesystem.
   @param[in] mode Mode in which to open the file.

   @return
   Returns the IxFile pointer.

   @dependencies
   None.
*/
virtual IxFileServer *OpenSFSFile(const char *pszFile,
IxFileOpenFileMode mode);

   IxFileSystem *m_fs;  /* Pointer to a file system object. */
   /** @endcond */
};


/** @} */ /* end_group SMECOMFILE */

#endif /* __cplusplus */

/*------------------------------------------------------------------------
C WRAPPERS
------------------------------------------------------------------------*/

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif

/**
   @addtogroup CAPI C Language API functions
@{
*/

/**
   @name IXFILEMGR_C_API

   These functions are C wrappers for a subset of the IxFileMgr API. The
   name of each C wrapper function makes it clear as to which C++ method
   it corresponds.
@{ */

/** See IxFileMgr::IxFileMgr. */
void * ixfilemgr_new(void);

/** See IxFileMgr::~IxFileMgr. */
void ixfilemgr_delete(void *pFileMgr);

/** See IxFileMgr::AddRef. */
unsigned long ixfilemgr_add_ref(void *pFileMgr);

/** See IxFileMgr::Release. */
unsigned long ixfilemgr_release(void *pFileMgr);

/** See IxFileMgr::OpenFile. */
void * ixfilemgr_open_file(void *pFileMgr, const char *pszFile,
                           IxFileOpenFileMode mode);

/** See IxFileMgr::GetInfo. */
IxErrnoType ixfilemgr_get_info(void *pFileMgr, const char *pszName,
                              IxFileInfo * pInfo);

/** See IxFileMgr::Remove. */
IxErrnoType ixfilemgr_remove( void *pFileMgr, const char *pszName );

/** See IxFileMgr::MkDir. */
IxErrnoType ixfilemgr_mk_dir(void *pFileMgr, const char *pszDir);

/** See IxFileMgr::RmDir. */
IxErrnoType ixfilemgr_rm_dir(void *pFileMgr, const char *pszDir);

/** See IxFileMgr::Test. */
IxErrnoType ixfilemgr_test(void *pFileMgr, const char *pszName);

/** See IxFileMgr::GetLastError. */
IxErrnoType ixfilemgr_get_last_error(void *pFileMgr);

/** See IxFileMgr::GetInfoEx. */
IxErrnoType ixfilemgr_get_info_ex(void *pFileMgr, const char * pszName,
                                  IxFileInfoEx * pi);

/** See IxFileMgr::OpenDir. */
IxErrnoType ixfilemgr_open_dir(void *pFileMgr, const char * pszName,
                              IxFileDirHandleT * pDirHandle);

/** See IxFileMgr::CloseDir. */
IxErrnoType ixfilemgr_close_dir(void *pFileMgr,IxFileDirHandleT dirHandle);

/** See IxFileMgr::ReadDir. */
IxErrnoType ixfilemgr_read_dir(void *pFileMgr, IxFileDirHandleT dirHandle,
IxFileDirEntT *pDirEnt);

/** @} */ /* end_namegroup */
/** @} */ /* end_addtogroup CAPI */

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif

#endif /* IXFILEMGR_H */
