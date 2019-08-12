#ifndef IXBUFFEREDFILE_H
#define IXBUFFEREDFILE_H
/*============================================================================
IxBufferedFile

ABSTRACT
   The IxBufferedFile interface definitions. This class derives from IxFile.

DESCRIPTION
   Provide basic buffered file operations.

Copyright (c) 2005 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixfile/shared/inc/IxBufferedFile.h#1 $

=========================================================================== */
#include "IxFileTypes.h"
#include "comdef.h"
#include "IxFile.h"


#ifdef __cplusplus

class IxBufferedFile : public IxFile
{
    friend class IxFileMgr;

public:

   IxBufferedFile ();
   virtual ~IxBufferedFile ();

   /***********************************************************************
   * Init
   *
   * Copies all the contents of the file associated with this BufferedFile
   * object into memory.
   **********************************************************************/
   virtual IxErrnoType Init ();

   virtual unsigned long AddRef ();
   virtual unsigned long Release ();
   virtual void GetState (uint8** stateDataPtr, uint32* stateDataLenPtr)
   { (void) stateDataPtr; (void) stateDataLenPtr; };
   virtual void SetState (uint8* stateDataPtr, uint32 stateDataLen)
   { (void) stateDataPtr; (void) stateDataLen; };

   /***********************************************************************
   * Read
   *
   * Attempt to read 'nWant' of data from the file associated with this
   * object.  Returns the number of bytes successfully read, or -1 for an
   * error. Zero is a valid result, and generally indicates that the
   * end of file has been reached.
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
   * distance is 0 (zero), this acts as a tell operation and returns the
   * current position of the file pointer. Otherwise, the file pointer is
   * moved by the given distance from the specified origin. If the file was
   * opened in the _OFM_READ mode, this operation succeeds if the move
   * distance from the specified origin is within the boundaries of the file.
   * If the file was opened in the _OFM_READWRITE mode and the move distance
   * points to the location before the beginning of the file, this operation
   * fails. If the move distance from the specified origin is past the end of
   * the file, the file size is extended to the point of the move distance
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
   * GetInfo
   *
   * Unsupported (use GetInfoEx instead)
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
   virtual void *Map (void * pStart, uint32 dwSize, int protections,int flags,
     uint32 dwOffset);


  /***********************************************************************
   * Tell
   *
   * Retruns the current position of the file associated with this object.
   **********************************************************************/
   virtual int32 Tell ();

private:
   virtual IxErrnoType WriteFromUxBuf (UxBuf *pBuf, uint32 bufOffset,
     IxFile *pFile, uint32 *pNBytes);
   virtual IxErrnoType ReadEntireFile (UxBuf *pBuf, IxFile *pFile);

   uint32  m_nRefs;
   IxFile *m_pFile;
   UxBuf  *m_buf;
};

#endif /* __cplusplus */

#endif /* IXBUFFEREDFILE_H */
