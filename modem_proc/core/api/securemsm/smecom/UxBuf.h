
#ifndef __UXBUF_H__
#define __UXBUF_H__

/**
   @file UxBuf.h

   @brief
   This header file defines the interface to UxBuf.

   UxBuf defines an interface to allocate and manage storage that can be 
   shared across process domains without copying data. Storage is allocated 
   from a pool of memory with a specified access control.

   UxBuf implments a buffer aggregate that encapsulates a sequence of bytes. 
   The data may be stored in a number of discontinuous segments (where a segment 
   is a contiguous memory region).

   Update:

   Under the LTK, UxBuf will allocate and maintain memory in contiguous blocks
   using standard malloc/free calls.  This is to bypass a limitation of 
   resources when using the dsm blocks on the LTK.  Future updates will likely
   include the conversion from malloc/free to KxMem.


   Externalized Functions:
   UxBuf()       - Constructor for UxBuf
   Prepend()     - Appends another UxBuf at the beginning.
   Append()      - Appends another UxBuf to the end.
   Alloc()       - Allocates memory at a specific offset.
   Dup()         - Duplicate a subrange of the contents of a UxBuf.
   Copy()        - Do deep copy of a subrange of a UxBuf.
   Trim()        - Trims some bytes from a UxBuf.
   Truncate()    - Truncates a UxBuf to 'Length' bytes starting 
                   from the beginning.
   Length()      - Returns the length of a UxBuf.
   Seek()        - Seeks to a position in a UxBuf.
   Tell()        - Tells the position of Read/Write pointer.
   NextSegment() - Returns a pointer to the next contiguous segment.
   Get()         - Gets the subrange of data from the UxBuf into a byte array.
   Set()         - Sets the data into a UxBuf from a byte array.
   GetNext()     - Gets the next subrange of data stored in a UxBuf into a
                   byte array 
   SetNext()     - Sets the data stored in a byte array into a UxBuf starting
                   from next location pointed to by curPos.
   Grant()       - Grants, to a specified process, access to a UxBuf. The
                   current process loses access.
   Revoke()      - Revokes, from a specified process, access to a UxBuf.
                   The current process gains access.
   Relinquish()  - Relinquish access to a UxBuf. The process that previously
                   had access gains access.

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                                U X  B U F
                          H E A D E R  F I L E

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/UxBuf.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $


when      who      what, where, why
--------  ---      ----------------------------------------------------
01/18/11  leo      (Tech Pubs) Edited Doxygen comments and markup.
12/13/10  nk       Doxygenated.
11/22/06  gr       Added GetState and GetState
05/15/06  rv       Fixed compiler error on ARM
03/20/06  gr       Changes to support remoting of new UxBuf structure
03/20/06  rv       Fixed target Compiler error
02/21/06  gr       Fixed featurization problem
10/21/05  gr       Clean-up, API update, bug fixes
10/11/05  Sur      Initial Version


===========================================================================*/
/*===========================================================================
                            INCLUDE FILES
============================================================================*/
#include "IxErrno.h"
#include "msg.h"
#include "iovec.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SMECOMUTIL_MEMORY 
    @{ 
*/

/**
   Ux buffer maximum allowed invalid objects.
*/
#define UXBUF_MAX_INVALID_OBJS    10

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

/*
define a minimum size packet in the case of dynamic memory allocation
to help keep from fragmenting memory.
*/


/*------------------------------------------------------------------------
                            PUBLIC TYPES
------------------------------------------------------------------------*/

/**
   @brief Placeholder for buffer meta data.
*/
typedef struct UxBufMetaDataT_
{
UxIOVEC_T *iovt;      /**< Pointer to the IOVEC table. */ 
UxBufPool m_pool;     /**< Pointer to the buffer memory pool. */ 
} UxBufMetaDataT;

/**
   @brief Placeholder for buffer state data.
*/
typedef struct
{
UxIOVEC_T *iovt;      /**< Pointer to the IOVEC table. */ 
UxBufPool pool;       /**< Pointer to the buffer memory pool. */ 
} UxBufStateDataT;  

/**
   @brief
   Class that contains the interface for buffer operations on a specified
   object.
*/
class UxBuf
{

/** Friend function that gets the metadata.

   @param[in] pBuf      Pointer to the UxBuf.
   @param[in] pMetadata Pointer to a placeholder for buffer metadata.

   @return
   E_SUCCESS is returned on success.\n
   Error code is returned on failure (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
  */
friend IxErrnoType UxBufGetMetaData (UxBuf *pBuf, UxBufMetaDataT *pMetadata);

/**
   Friend function that creates a buffer from the metadata.

   @param[in] ppBuf     Pointer to a pointer to the UxBuf.
   @param[in] pMetadata Pointer to a placeholder for buffer metadata.

   @return
   E_SUCCESS is returned on success.\n
   Error code is returned on failure (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
  */
friend IxErrnoType UxBufCreateBufFromMetaData (UxBuf **ppBuf,
UxBufMetaDataT *pMetadata);

public:

/*------------------------------------------------------------------------
                          PUBLIC METHODS
------------------------------------------------------------------------*/

/**
   Constructor for UxBuf.

   UxBuf is an aggregator object that encapsulates an ordered sequence of bytes.
   Data may be stored in a number of discontiguous segments.

   @param[in] pool Buffer pool used to create the buffers.

   @return
   None.

   @dependencies
   None.
*/
UxBuf (UxBufPool pool=BUF_POOL_LOCAL);    // enum in iovec.h

/**
   Creates a UxBuf object. This constructor is a copy constructor.

   @param[in] u Reference to copy UxBuf object.

   @return
   None.

   @dependencies
   None.
*/
UxBuf(const UxBuf &u);

/**
   Creates a UxBuf object. This constructor is a pointer to character and pool
   type as parameters.

   @param[in] p    Pointer to the character type.
   @param[in] pool Buffer pool used to create the buffers.

   @return
   None.

   @dependencies
   None.
*/
UxBuf(const char *p, UxBufPool pool=BUF_POOL_LOCAL);

/**
   An assignment operator override method (UxBuf variant). 

   @param[in] u Reference to the UxBuf object to be assigned.

   @return
   None.

   @dependencies
   None.
*/
UxBuf& operator=(const UxBuf &u);

/**
   An assignment operator override method (character variant).

   @param[in] p Reference to the character pointer to be assigned.

   @return
   None.

   @dependencies
   None.
*/
UxBuf& operator=(const char *p);

/**
   An addition operator overide method. Adds UxBuf and Left-Hand Side (LHS)
   items together to form a new UxBuf.

   @param[in] b Reference to the UxBuf object to be added.

   @return
   None.

   @dependencies
   None.
*/
UxBuf& operator+(const UxBuf &b);

/**
   An addition assignment operator overide method. Appends the LHS UxBuf to the
   Right-Hand Side (RHS) UxBuf to get a resultant UxBuf.

   @param[in] u Reference to the UxBuf object to be appended.

   @return
   None.

   @dependencies
   None.
*/
UxBuf& operator+= (const UxBuf &u);

/**
   Destructor for UxBuf. Reclaims the underlying buffers of the UxBuf and
   deletes the UxBuf object.

   @return
   None.

   @dependencies
   None.
*/
~UxBuf ();

/**
   Prepends a UxBuf.

   The buffers pointed to by the input UxBuf are inserted at the beginning.
   Buffer data is not copied. Only references to the buffers are added.

   @param[in] Buf UxBuf to append.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.

   @sideeffects
   This function does not monitor whether there is duplication of buffer
   packets in the UxBuf passed in.
*/
IxErrnoType  Prepend (UxBuf **Buf);

/**
   Appends a UxBuf (input buffer variant).

   The buffers pointed to by the input UxBuf are inserted at the end.
   Buffer data is not copied. Only references to the buffers are added.

   @param[in] Buf UxBuf to append.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.

   @sideeffects
   This function does not monitor whether there is duplication of buffer
   packets	in the UxBuf passed in.
*/
IxErrnoType  Append (UxBuf **Buf);

/**
  Appends a UxBuf (byte input variant).

   The buffers pointed to by the input UxBuf are inserted at the end.
  Buffer data is not copied. Only references to the buffers are added.

   @param[in] src       Pointer to the source bytes.
   @param[in] num_bytes Number of bytes to be appended.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.

   @sideeffects
   This function does not monitor whether there is duplication of buffer
   packets in the UxBuf passed in.
*/
IxErrnoType  Append (byte* src, uint32 num_bytes);

/**
   Allocates a Size amount of storage at a specific offset. Allocated
   storage is not guaranteed to be contiguous.

   @param[in] Offset Offset within UxBuf where storage is to be allocated.
   A value of -1 (0xffffffff) allocates storage at the end.
   @param[in] Size   Size of the aggregator object to create.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Alloc (uint32 Offset, uint32 Size);

/**
   Duplicates a subrange of the contents of a UxBuf.

   This function returns a new UxBuf that contains the subrange of the contents
   of the UxBuf. The subrange starts at the byte offset specified by Offset and
   contains Length bytes. The value of Offset must be within range or an error
   is returned.

   This is a shallow copy, where the top level objects are duplicated and the
   lower levels contain references.

   @param[in] Offset Offset that specifies the start of the new UxBuf.
   @param[in] Length Length in bytes of the new UxBuf.
   @param[in] Buf    Pointer to the new UxBuf.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Dup (uint32 Offset, uint32 Length, UxBuf** Buf);

/**
   Copies a subrange of the contents of a UxBuf.

   This function returns a new UxBuf that is a deep copy of a subrange of the
   UxBuf. The subrange starts at the byte offset specified by Offset and
   contains Length bytes. The value of Offset must be within range or an error
   is returned.

   This is a deep copy operation, where UxBuf structure and segment data are
   duplicated.

   @param[in] Offset Offset that specifies the start of the new UxBuf.
   @param[in] Length Length in bytes of the new UxBuf.
   @param[in] Buf    Pointer to the new UxBuf.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Copy (uint32 Offset, uint32 Length, UxBuf** Buf);

/**
   Trims some bytes from a UxBuf.

   The data that is trimmed starts at the Offset byte position and
   includes Length bytes.

   @param[in] Offset Offset where UxBuf is trimmed.
   @param[in] Length Length of bytes to trim.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Trim (uint32 Offset, uint32 Length);

/**
   Truncates the UxBuf to Length bytes starting from the beginning of
   the buffer.

   @param[in] Length Length of the new UxBuf.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Truncate (uint32 Length);

/**
   Calculates and returns the length of the UxBuf.

   @param[in,out] Length Pointer to the Length field filled in by the callee.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Length (uint32* Length);

/**
   Calculates and returns the size of the UxBuf.

   @param[in,out] Size Pointer to the Size field filled in by the callee.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Size (uint32* Size);

/**
   Seeks to a position in UxBuf.

   This function is typically used before accessing the contents of a buffer
   at a specified position. The position is relative to the start of the UxBuf.
   If the position value is not within the range of the buffer, an error is
   returned.

   @param[in,out] Position Position to which to seek.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Seek (uint32  Position);

/**
   Tells the position of the Read/Write pointer.

   This function is typically used when accessing the contents of the UxBuf
   using SetNext() and GetNext() calls. The position is relative to the start
   of the UxBuf.

   @param[in,out] Position Pointer to the position filled in by the callee.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Tell (uint32*  Position);

/**
   Returns the [pointer, length] pair of the next contiguous segment relative to
   the position. The position is set using the Seek() method.

   @param[in,out] SegAddr Pointer to the buffer segment. This is filled in by
                          the callee.
   @param[in,out] Length  Length of bytes that are valid at SegAddr. This is
                         filled in by the callee.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  NextSegment (uint8**  SegAddr, uint32*  Length);

/**
   Returns the [pointer, length] pair of the next contiguous segment relative to
   the position. The position is set using the Seek() method.

   @param[in,out] SegAddr Pointer to the buffer segment. This is filled in by
                         the callee.
   @param[in,out] Length  Length of bytes that are valid at SegAddr. This is
                         filled in by the callee.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  GetFirstSegment (uint8** SegAddr, uint32* Length);

/**
   Returns the [pointer, length] pair of the next contiguous segment relative to
   the position. The position is set using the Seek() method.

   @param[in,out] SegAddr Pointer to the buffer segment. This is filled in by
                         the callee.
   @param[in,out] Length  Length of bytes that are valid at SegAddr. This is
                         filled in by the callee.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  GetSegment (uint8**  SegAddr, uint32*  Length);

/**
   Gets the subrange of data from the UxBuf into a byte array.

   This function copies the data of size Length from the UxBuf starting at the
   Offset location inside the UxBuf into the byte array provided. 

   @param[in] Offset    Byte at which the data to be copied starts.
   @param[in] Length    Number of bytes to be copied.
   @param[in] Bytes     Pointer to the destination byte array into which  
                        the data is copied.
   @param[in] BytesRead Pointer to the bytes read.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Get(uint32 Offset, uint32 Length, byte** Bytes, uint32 *BytesRead);

/**
  Sets the data of size Length into the UxBuf from the byte array provided. The
   copying starts from the location Offset inside UxBuf.

   @param[in] Offset       Specifies the first location in the UxBufdata from
                           where to start copying the data.
   @param[in] Length       Number of bytes to be copied.
   @param[in] Bytes        Pointer to the source byte array from which the data
                           is copied.
   @param[in] BytesWritten Pointer to the bytes written.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Set( uint32 Offset, uint32 Length , byte* Bytes, uint32 *BytesWritten);

/**
  Gets the next subrange of data stored in the UxBuf into a byte array.

   Gets the data from the location set by the Seek() operation or a previous
   GetNext() operation. After reading the data, it sets the curPos to the next
   data byte to the byte last read.

   @param[in] Length Number of bytes to be copied.
   @param[in] Bytes  Pointer to the destination byte array into which the 
                     data is copied.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  GetNext( uint32 Length , byte** Bytes );

/**
   Sets the data stored in a byte array into the UxBuf starting from the next 
   location pointed to by current position. After reading the data, the
   current position is set to the data byte after the byte last written into
   the UxBuf.

   @param[in] Length Number of bytes to be copied.
   @param[in] pBytes Pointer to the source byte array from which the data is
                     copied.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  SetNext( uint32 Length , byte* pBytes );

/**
   Grants privileges to a specified process to access the UxBuf.

   This function returns an error unless called from the process that
   currently has access to the the UxBuf. The process that currently has
   access to the the UxBuf loses access if the call succeeds.

   @param[in] pId  Process ID.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Grant( uint32 pId );

/**
   Revokes access privileges to the UxBuf from a specified process.
   The process that currently has access to the UxBuf loses access.

   @param[in] pId Process ID.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Revoke( uint32 pId );

/**
   Relinquishes access privileges to the UxBuf. Access privileges revert to
   the process that previously had access.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
IxErrnoType  Relinquish( void );

/**
   Gets the buffer state data for the UxBuf. This data can be used to recreate
   the UxBuf.

   @param[in] ppStateData   Pointer to the state data.
   @param[in] pStateDataLen Pointer to the state data length.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
void GetState( uint8** ppStateData, uint32* pStateDataLen );

/**
   Sets the buffer state data. This function recreates the UxBuf using specified
   state data.

   @param[in] pStateData   Pointer to the state data.
   @param[in] stateDataLen State data length.

   @return
   Success or error number (see Chapter @latexonly \ref{ErrorCodes}@endlatexonly).

   @dependencies
   None.
*/
void SetState( uint8* pStateData, uint32 stateDataLen );

/**
   Checks whether this pointer is a valid Player Service object.

   @param[in] ptr Pointer to the buffer.

   @return
   TRUE  -- Object is a valid Player Service object.\n
   FALSE -- Obejct is not a valid Player Service object.

   @dependencies
   None.
*/
static boolean IsObjectValid( UxBuf *ptr );

private:

/*------------------------------------------------------------------------
                               PRIVATE METHODS
------------------------------------------------------------------------*/

/** @cond
*/
/* @name Invalidate/Validate this UxBuf Object

   Static that stores recently deallocated objects.
   This is to handle the case where the object is deallocated, but before
   the deallocation took place, another IPC message is placed on the queue.
   Those objects must be discarded.
@{
*/

static boolean InvalidObjects_Init;
static uint32 InvalidObjects[ UXBUF_MAX_INVALID_OBJS ];
static uint32 InvalidObjects_CurIndex;

void InvalidateThisObject();
void ValidateThisObject();

/* @} */ /* end_name invalidate objects */

/** @endcond */

/*------------------------------------------------------------------------
                               PRIVATE MEMBERS
------------------------------------------------------------------------*/
UxIOVEC_T *m_iovt;  // beginning of our iovec table
uint32 m_offset;    // current offset of the UxBuf
UxBufPool m_pool;   // the allocation pool type
};

/** @} */  /* end_addtogroup SMECOMUTIL_MEMORY */

#ifdef __cplusplus
}
#endif

#endif /* __UXBUF_H__ */
