#ifndef IXFILESERVER_H
#define IXFILESERVER_H

/**
   @file IxFileServer.h

   @brief 
   The IxFileServer interface definitions.

   Provides basic file operations to EFS.

   Externalized Functions:
   AddRef,
   Release,
   GetState,
   SetState,
   Read,
   Write,
   GetInfo,
   Seek,
   Truncate,
   GetInfoEx,
   SetCacheSize,
   Map,
   Tell.


   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                             I X  F I L E S E R V E R
                              H E A D E R  F I L E

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                              EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/IxFileServer.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $


when       who      what, where, why
--------   ---      ------------------------------------------------------
01/13/11   leo     (Tech Pubs) Edited Doxygen comments and markup.
12/07/10   nk      Doxygenated.

===========================================================================*/

#include "comdef.h"
#include "IxErrno.h"
#include "IxFile.h"

#ifdef __cplusplus

/**
   @addtogroup SMECOMFILE 
   @{
*/

/**
   @brief Provides basic support for file operations to EFS or SFS. IxFileServer
   is derived from the IxFile base class.
*/
class IxFileServer : public IxFile {

friend class IxFileMgr;

public:

IxFileServer( );  /**< Default contructor. */

virtual ~IxFileServer( );  /**< Virtual destructor. */

/**
   Adds a reference count so the destructor does not delete shallow copied
   objects.

   This is used if the client wants to retain multiple pointers to this
   object that are shared but does not want to remove the underlying
   references this object is holding. This prevents deletion of shallow
   copied objects.

   @return
   Returns the current reference count + 1.

   @dependencies
   None.

   @sa
   IxFileServer::Release
*/
virtual unsigned long AddRef( void );

/**
   Releases the reference count. This is automatically called by the
   destructor, so it must be used with caution.

   @note1hang This operation is not normally called by the client.

   @return
   Returns the current reference count - 1.

   @dependencies
   None.

   @sa
   IxFileServer::AddRef
*/
virtual unsigned long Release( void );

/**
   Gets the internal serialized state of the object through passed input
   parameter pointers.

   @param[out] stateDataPtr    Pointer to the serialized state.
   @param[out] stateDataLenPtr Pointer to the length of the serialized state.

   @return
   None.

   @dependencies
   None.

   @sa
   IxFileServer::SetState
*/
virtual void GetState( uint8** stateDataPtr, uint32* stateDataLenPtr )
{ (void) stateDataPtr; (void) stateDataLenPtr; };

/**
   Sets the internal serialized state of the object.

   @param[in] stateDataPtr Pointer to the serialized state.
   @param[in] stateDataLen Length of the serialized state.

   @return
   None.

   @dependencies
   None.

   @sa
   IxFileServer::GetState
*/
virtual void SetState( uint8* stateDataPtr, uint32 stateDataLen )
{ (void) stateDataPtr; (void) stateDataLen; };

/**
   Reads nWant bytes of data from the file.

   @param[in] pDest Pointer to the destination buffer in which to read.
   @param[in] nWant Length of the destination buffer.

   @return
   Returns the number of bytes successfully read, or -1 on error. \n
   Zero is a valid result and generally indicates that the end of file
   has been reached.

   @dependencies
   None.
*/
virtual int32 Read( void * pDest, uint32 nWant);

/**
   Writes a buffer to a file.

   @param[in] pBuffer Pointer to the source buffer.
   @param[in] dwCount Number of bytes to write.

   @return Returns the number of bytes written.

   @dependencies
   None.
*/
virtual uint32 Write( const void * pBuffer, uint32 dwCount );

/**
   Gets the file creation date, file size, filename, and file attributes of
   the file pointed to by this object.

   @param[out] pInfo Pointer to where to obtain the file information.

   @return
   E_SUCCESS -- Success.\n
   E_*       -- Error (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType GetInfo( IxFileInfo * pInfo );

/**
   Moves the file pointer associated with this object by a given distance from
   a specified origin.

   The specified origin (seek) can take three values:
   - IxFile_SEEK_START
   - IxFile_SEEK_END
   - IxFile_SEEK_CURRENT

   The move distance can be specified in positive or negative directions
   (values). For example, a negative direction is used when seeking from the
   end of the file (_SEEK_END). If the seek type is _SEEK_CURRENT and the move
   distance is 0 (zero), this acts as a tell operation and returns the current
   position of the file pointer. Otherwise, the file pointer is moved by the
   specified distance from the specified origin.

   If the file was opened in _OFM_READ mode, this operation succeeds if
   the move distance from the specified origin is within the boundaries of
   the file. If the file was opened in _OFM_READWRITE mode and the move
   distance points to a location before the beginning of the file, this
   operation fails. If the move distance from the specified origin is past
   the end of the file, the file size is extended to the point of the move
   distance (from the seek origin).

   @param[in] seek     Origin from which to begin to seek.
   @param[in] position Position from seek origin to seek (can be + or -).

   @return
   E_SUCCESS      -- Success.\n
   E_OUT_OF_RANGE -- Position is out of range.\n
   E_*            -- All other errors (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType Seek( IxFileSeekType seek, int32 position );

/**
   Truncates the file associated with this object to a position specified by
   the truncate_pos parameter.

   If the file is larger than the specified length, the extra data is
   discarded. If the file is shorter than the specified length, its
   size is increased and the extended area is zero-filled.

   @param[in] truncate_pos Length of the file to truncate.

   @return
   E_SUCCESS       -- Success.\n
   E_NOT_SUPPORTED -- Unsupported operation.\n
   E_*             -- All other errors (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   To truncate a file, the file must be open for writing.
*/
virtual IxErrnoType Truncate( uint32 truncate_pos );


/**
   Gets the extended information of the file associated with this object.
   The extended file information is contained in IxFileInfoEx.

   @param[out] pi File information to read.

   @return
   E_SUCCESS       -- Success.\n
   E_NOT_SUPPORTED -- Unsupported operation.\n
   E_*             -- All other errors (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
virtual IxErrnoType GetInfoEx( IxFileInfoEx * pi );


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
virtual IxErrnoType SetCacheSize( int nSize );

/**
   This operation is currently unsupported and is a placeholder for future
   extensibility.

   @deprecated To be removed.

   @param[in] pStart      Pointer to the start position of the file.
   @param[in] dwSize     Buffer size.
   @param[in] protections File protection information.
   @param[in] flags       File flag information.
   @param[in] dwOffset    File offset information.

   @return
   E_NOT_SUPPORTED -- Unsupported operation.

   @dependencies
   None.
*/
   virtual void *  Map( void * pStart, uint32 dwSize, int protections,
                        int flags, uint32 dwOffset );

/**
   Obtains the current value of the file position indicator.

   @return
   The position in the file relative to IxFileSeekType. 
   The position can be a negative value.

   @dependencies
   None.

   @sa
   IxFileServer::Seek
*/
virtual int32  Tell( void );


private:
virtual void SetFileHandle( int fileHandle );
virtual void SetBuffering( bool buffering_enabled );
int FileSystemSeek( IxFileSeekType seek_type,
int32 position );

uint32        m_nRefs;
int           file_handle;
bool          is_buffered;
int32         m_curPos;
int32         m_fsCurPos;
IxFileSystem *m_fs;
};

/** @} */ /* end_group SMECOMFILE */

#endif /* __cplusplus */

#endif /* IXFILESERVER_H */

