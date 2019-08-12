#ifndef IXSFSFILE_H
#define IXSFSFILE_H
/*============================================================================
IxSFSFile

ABSTRACT
   The IxSFSFile interface definitions.

DESCRIPTION
   This service provides basic file operations to SFS.


Copyright (c) 2005 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixfile/environment/src/IxSFSFile.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/10    nk     Removed references to IxFileClient, IXFILE IPC cleanup.
=========================================================================== */
#include "comdef.h"
#include "IxErrno.h"
#include "IxFileServer.h"

class IxSFSFile : public IxFileServer {
 
   friend class IxFileMgr;

public:

   IxSFSFile ();
   virtual ~IxSFSFile ();

   virtual unsigned long AddRef (void);
   virtual unsigned long Release (void);

   /***********************************************************************
   * Read
   *
   * Attempt to read 'nWant' of data from the file associated with this
   * object.  Returns the number of bytes successfully read, or -1 for an
   * error. Zero is a valid result, and generally indicates that the end of
   * file has been reached.
   *
   * It is permitted for Read to return fewer bytes than were requested,
   * even if the data is available in the file.
   **********************************************************************/
   virtual int32 Read (void * pDest, uint32 nWant);

  /***********************************************************************
   * Write
   *
   * Attempt to write 'dwCount' of data to the file associated with this
   * object. Returns the number of bytes successfully  written or -1 for 
   * an error.  Zero generally indicates that the filesystem is full.
   *
   * It is permitted for Write to write fewer bytes than were requested,
   * even if space is available.
   **********************************************************************/
   virtual uint32 Write (const void * pBuffer, uint32 dwCount); 

  /***********************************************************************
   * Seek
   * 
   * Moves the file pointer associated with this object by a given distance
   * from a specified origin.
   *
   * The specified origin (seek) can take three values:
   *    IxFile_SEEK_START
   *    IxFile_SEEK_END,
   *    IxFile_SEEK_CURRENT
   *
   * The move distance can be specified in positive or negative directions
   * (values). For example, negative direction is used when seeking from the
   * end of the file (_SEEK_END). If the seek type is _SEEK_CURRENT and move
   * distance is 0 (zero),this acts as a tell operation and returns the
   * current position of the file pointer. Otherwise, the file pointer is
   * moved by the given distance from the specified origin. If the file was
   * opened in the _OFM_READ mode, this operation succeeds if the move
   * distance from the specified origin is within the boundaries of the file.
   * If the file was opened in the _OFM_READWRITE mode and the move distance
   * points to the location before the beginning of the file, this operation
   * fails. If the move distance from the specified origin is past the end
   * of the file, the file size is extended to the point of the move distance
   * (from the seek origin).
   **********************************************************************/
   virtual IxErrnoType Seek (IxFileSeekType seek, int32 position); 

  /***********************************************************************
   * Truncate 
   *
   * Truncates the file associated with this object to a position specified by
   * truncate_pos parameter. 
   *
   * To truncate a file, the file needs to be open for writing.
   **********************************************************************/
   virtual IxErrnoType Truncate (uint32 truncate_pos); 

  /***********************************************************************
   * GetInfoEx
   *
   * Gets the extended information of the file associated with this object.
   * The extended file information is contained in IxFileInfoEx.
   **********************************************************************/
   virtual IxErrnoType GetInfoEx (IxFileInfoEx * pi); 

  /***********************************************************************
   * Tell
   *
   * Retruns the current position of the file associated with this object.
   **********************************************************************/
   virtual int32       Tell (void);

  /***********************************************************************
   * GetInfo
   *
   * Unsupported
   **********************************************************************/
   virtual IxErrnoType GetInfo (IxFileInfo * pInfo); 

  /***********************************************************************
   * SetCacheSize
   *
   * UnSupported
   **********************************************************************/
   virtual IxErrnoType SetCacheSize (int nSize);

  /***********************************************************************
   * Map
   *
   * UnSupported
   **********************************************************************/
   virtual void * Map (void * pStart, uint32 dwSize, int protections,
      int flags, uint32 dwOffset);


private:
   virtual void SetFileHandle (int fileHandle);

   uint32 m_nRefs;
   int    m_file_handle;
};

#endif /* IXSFSFILE_H */
