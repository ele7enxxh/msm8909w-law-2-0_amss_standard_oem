#ifndef AEEIPORT1_H
#define AEEIPORT1_H
#include "datamodem_variation.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_Interface.h"
/*
============================================================================
// Copyright (c) 2007 Qualcomm Technologies Incorporated.

// All Rights Reserved.

// Qualcomm Confidential and Proprietary

// Copyright (c) 2006, 2007 Qualcomm Technologies Incorporated.
============================================================================
*/

#if !defined(AEEINTERFACE_CPLUSPLUS)
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"

#define AEEIID_IPort1 0x1056898
/*
 * Generic interface for bidirectional data stream providing Read/Readable,
 * Write/Writeable, Control, and Close operations.
 *
 * Instances of IPort1 usually come from a factory or some other object that
 * owns or provides access to the data to be read or written.
 *
 * For example, if there is an IPort1-based implementation of file, you may
 * get an instances of the file from the filesystem when the file is opened.
 * Similarly, given an IPort1-based serial I/O implementation, an instance
 * could be created to access serial I/O.
*/
struct IPort1__SeqBytes__seq_octet {
   byte* data;
   int dataLen;
   int dataLenReq;
};
typedef struct IPort1__SeqBytes__seq_octet IPort1__SeqBytes__seq_octet;
typedef IPort1__SeqBytes__seq_octet IPort1_SeqBytes;
#define INHERIT_IPort1(iname) \
   INHERIT_IQI(iname); \
   AEEResult (*Read)(iname* _pif, byte* buf, int bufLen, int* bufLenReq); \
   AEEResult (*Readable)(iname* _pif, ISignal* ps); \
   AEEResult (*Write)(iname* _pif, const byte* buf, int bufLen, int* written); \
   AEEResult (*Writeable)(iname* _pif, ISignal* ps); \
   AEEResult (*Control)(iname* _pif, AEEUID uid, const byte* inbuf, int inbufLen, byte* outbuf, int outbufLen, int* outbufLenReq); \
   AEEResult (*Close)(iname* _pif)
AEEINTERFACE_DEFINE(IPort1);
static __inline uint32 IPort1_AddRef(IPort1* _pif)
{
   return AEEGETPVTBL(_pif, IPort1)->AddRef(_pif);
}
static __inline uint32 IPort1_Release(IPort1* _pif)
{
   return AEEGETPVTBL(_pif, IPort1)->Release(_pif);
}
static __inline int IPort1_QueryInterface(IPort1* _pif, AEEIID iqi_iid, void** iqi_ppif)
{
   return AEEGETPVTBL(_pif, IPort1)->QueryInterface(_pif, iqi_iid, iqi_ppif);
}

/*
    * Copy bytes from the source stream to the specified buffer.  The read
    * pointer is advanced.
    *
    * @param buf the buffer to receive the read bytes
    * 
    * @retval AEE_SUCCESS bytes were successfully read into the buffer.  If
    *                     the buffer was of non-zero size but is returned
    *                     empty, the end of the stream has been reached.
    * @retval AEE_ENOTALLOWED caller does not have the necessary capability to
    *                         perform the operation
    * @retval AEE_EWOULDBLOCK no data available; call Readable() to wait
    *
    * @return Another appropriate error core may be returned if the operation
    *         is not successful.
    
*/
static __inline AEEResult IPort1_Read(IPort1* _pif, byte* buf, int bufLen, int* bufLenReq)
{
   return AEEGETPVTBL(_pif, IPort1)->Read(_pif, buf, bufLen, bufLenReq);
}

/*
    * Register a signal to be set when Read() would return something other
    * than AEE_EWOULDBLOCK.
    * 
    * @param ps the signal to register
    *
    * @return AEE_SUCCESS if successful, or another appropriate error code if
    *         operation was not successful.
    
*/
static __inline AEEResult IPort1_Readable(IPort1* _pif, ISignal* ps)
{
   return AEEGETPVTBL(_pif, IPort1)->Readable(_pif, ps);
}

/*
    * Copy bytes from the specified buffer to the source stream.  The write
    * pointer is advanced.
    *
    * @param buf the buffer from which bytes are taken to write to the port
    * @param written size of data written to the port
    *
    * @retval AEE_SUCCESS successfully wrote 'written' bytes to the port
    * @retval AEE_ENOTALLOWED caller does not have the necessary capability to
    *                         perform the operation
    * @retval AEE_EWOULDBLOCK no data available; call Writeable() to wait
    *
    * @return Another appropriate error core may be returned if the operation
    *         is not successful.
    
*/
static __inline AEEResult IPort1_Write(IPort1* _pif, const byte* buf, int bufLen, int* written)
{
   return AEEGETPVTBL(_pif, IPort1)->Write(_pif, buf, bufLen, written);
}

/*
    * Register a signal to be set when Write() would return something other
    * than AEE_EWOULDBLOCK. 
    * 
    * @param ps the signal to register
    *
    * @return AEE_SUCCESS if successful, or another appropriate error code if
    *         operation was not successful.
    
*/
static __inline AEEResult IPort1_Writeable(IPort1* _pif, ISignal* ps)
{
   return AEEGETPVTBL(_pif, IPort1)->Writeable(_pif, ps);
}

/*
    * This function allows some implementation-specific control over the
    * behavior of an IPort1, e.g. setting/getting configuration.
    * 
    * Control() here is analogous to SYSV's Control.  Its intended use is for
    * special, out-of-object-signature behavior.  For that reason, it is
    * super-generic.
    *
    * This method could also be used to selectively close the capabilities
    * of the port.
    *
    * @par Comments:
    *    Data passed in the buffer arguments must always be arranged as if
    *    they've already been marshalled, i.e. the precise byte ordering must
    *    be specified.  The data must not include pointers or objects. 
    *
    * @param uid uid of the control operation, defined by the class
    *            implementing IPort1
    * @param inbuf input buffer for the operation
    * @param outbuf output buffer for the operation
    *
    * @return AEE_SUCCESS if successful, or another appropriate error code if
    *         operation was not successful.
    
*/
static __inline AEEResult IPort1_Control(IPort1* _pif, AEEUID uid, const byte* inbuf, int inbufLen, byte* outbuf, int outbufLen, int* outbufLenReq)
{
   return AEEGETPVTBL(_pif, IPort1)->Control(_pif, uid, inbuf, inbufLen, outbuf, outbufLen, outbufLenReq);
}

/*
    * This function closes the port.  Any further Read() and Write() on the
    * port will fail, and the IPort1 will become readable and writeable.
    *
    * @return AEE_SUCCESS if successful, or another appropriate error code if
    *         operation was not successful.
    
*/
static __inline AEEResult IPort1_Close(IPort1* _pif)
{
   return AEEGETPVTBL(_pif, IPort1)->Close(_pif);
}

#else /* C++ */

#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
const AEEIID AEEIID_IPort1 = 0x1056898;

class IPort1 : public IQI
{
public:
   struct _SeqBytes__seq_octet
   {
      ::byte* data;
      int dataLen;
      int dataLenReq;
   };

   typedef _SeqBytes__seq_octet SeqBytes;

   virtual ::AEEResult CDECL Read (/*rout*/ ::byte* buf, /*in*/ int bufLen, /*rout*/ int* bufLenReq) = 0;
   virtual ::AEEResult CDECL Readable (/*in*/ ::ISignal* ps) = 0;
   virtual ::AEEResult CDECL Write (/*in*/ const ::byte* buf, /*in*/ int bufLen, /*rout*/ int* written) = 0;
   virtual ::AEEResult CDECL Writeable (/*in*/ ::ISignal* ps) = 0;
   virtual ::AEEResult CDECL Control (/*in*/ ::AEEUID uid, /*in*/ const ::byte* inbuf, /*in*/ int inbufLen, /*rout*/ ::byte* outbuf, /*in*/ int outbufLen, /*rout*/ int* outbufLenReq) = 0;
   virtual ::AEEResult CDECL Close (void) = 0;
};

#endif /* !defined(AEEINTERFACE_CPLUSPLUS) */

/*=====================================================================
  INTERFACES DOCUMENTATION
=======================================================================

IPort1 Interface

Description:

   Generic interface for bidirectional data stream providing
   Read/Readable, Write/Writeable, Control, and Close operations.

   Instances of IPort1 usually come from a factory or some other 
   interface that owns or provides access to the data to be read or written.

   For example, if there is an IPort1-based implementation of file,
   you may get an instances of the file from the filesystem when the 
   file is opened.  Similarly, given an IPort1-based serial I/O
   implementation, an instance could be created to access serial I/O.
 
Interface ID: 
   AEEIID_IPort1
 
=======================================================================

IPort1_AddRef()

Description:
    This function is inherited from IQI_AddRef().
 
Prototype:
   unsigned int IPort1_AddRef(IPort1* _pif)
 
Parameters:
   _pif      : Pointer to the IPort1 interface
 
Return Value: 
   Updated reference count 

See Also:
    IPort1_Release()

=======================================================================

IPort1_Release()

Description:
    This function is inherited from IQI_Release()..

Prototype:
   unsigned int IPort1_Release(IPort1* _pif)
 
Parameters:
   _pif      : Pointer to the IPort1 interface
 
Return Value:
   updated reference count
 
See Also:
    IPort1_AddRef()

=======================================================================

IPort1_QueryInterface()

Prototype:
    int IPort1_QueryInterface(IPort1* _pif, AEEIID iid, void** ppo)
 
Parameters:
   _pif      : Pointer to the IPort1 interface
   iid       : Interface ID
   ppo       : interface pointer
 
Return Value:
   Returns AEE_SUCCESS if the operation was successful, otherwise an 
   appropriate error code.
 
Comments:
   Common error codes:
     AEE_ECLASSNOTSUPPORT: The interface type is not supported
 
Description:
    This function is inherited from IQI_QueryInterface().

=======================================================================

IPort1_Read()

Description:
   Copy bytes from the source stream to the specified buffer.  All copied
   bytes are "consumed" (i.e. the "read pointer" is advanced).

Prototype:
   int IPort1_Read
   (
      IPort1 *pif,
      byte *pBuf,
      int nBufSize,
      int *pnRead
   )

Parameters:
   pif     : [in]  : Pointer IPort1 interface
   pBuf    : [out] : pointer to buffer to receive bytes read
   nBufSize: [in]  : size of pBuf buffer
   pnRead  : [out] : the number of bytes read successfully

Return Value:

   AEE_SUCCESS      : if *pnRead = 0 and nBufSize != 0, end of read stream.
                      If not, *pnRead indicates the amount of data read.
   AEE_EWOULDBLOCK  : no data available at the moment call Readable() to wait
   AEE_ENOTALLOWED  : Does not have the capabilities to perform the operation.

   Another appropriate error code if operation was not successful

Comments:
   Note that the AEE_EWOULDBLOCK return value is used so that the Readable
   can be called. pnRead > 0 indicates that data has been successfully read
   from the port.

   As already indicated above, when the end is reached for reading from
   an IPort1, *pnRead returned is 0 (for nBufSize > 0) and error code 
   returned is AEE_SUCCESS.

Side Effects:
   None

See Also:
   IPort1_Readable()

=======================================================================

IPort1_Readable()

Description:
   Register a signal to be set when IPort1_Read() would
   return something other than AEE_EWOULDBLOCK.

Prototype:
   int IPort1_Readable
   (
      IPort1 *pif,
      ISignal *piSignal
   )

Parameters:
   pif      :  [in] : Pointer IPort1 interface
   piSignal :  [in] : Pointer to a signal to be set when the port
                      may be read with IPort1_Read().

Return Value:
   AEE_SUCCESS  : Successfully registered the signal.

   Another appropriate error code if operation was not successful.

Comments:
   The signal pointed by the piSignal must be created before calling 
   this function.

Side Effects:
   None

See Also:
   IPort1_Read()


===========================================================================

IPort1_Write()

Description:
   Copy bytes from the specified buffer to the source stream.  All copied
   bytes are "consumed" (i.e. the "write pointer" is advanced).

Prototype:
   int IPort1_Write
   (
      IPort1 *pif,
      const byte *pBuf,
      int nBufSize,
      int *pnWritten
   )

Parameters:
   pif      : [in]  : pointer to the IPort1 interface
   pBuf     : [in]  : pointer to buffer from which bytes are taken to write
                      to the port
   nBufSize : [in]  : size of data in pBuf
   pnWritten: [out] : size of data written to the Port

Return Value:

   AEE_SUCCESS      : Successfully wrote pnWritten bytes to the port
   AEE_ENOTALLOWED  : Does not have the capabilities to perform the operation.
   AEE_EWOULDBLOCK  : no data available at the moment call Writeable() to wait

   Another appropriate error code if operation was not successful

Comments:
   Note that the AEE_EWOULDBLOCK return value is used so that the writeable
   can be called. pnWritten > 0 indicates that data has been successfully
   written to the port.

Side Effects:
   None

See Also:
   IPort1_Writeable()

=======================================================================

IPort1_Writeable()

Description:
   Register a signal to be set when IPort1_Write() would
   return something other than AEE_EWOULDBLOCK.

Prototype:
   void IPort1_Writeable
   (
      IPort1 *pif,
      ISignal *piSignal
   )

Parameters:
   pif      : [in] : Pointer IPort1 interface
   piSignal : [in] : Pointer to a signal that is associated with a
                     callback function that can be invoked when the port
                     may be written with IPort1_Write().

Return Value:
   AEE_SUCCESS  : Successfully registered the signal.
   Another appropriate error code if operation was not successful.

Comments:
   The signal pointed by the piSignal must be created before calling 
   this function.

Side Effects:
   None

See Also:
   IPort1_Write()

=======================================================================

IPort1_Control()

Description:
   This function allows some control over the behavior of an IPort1,
   e.g. setting/getting configuration.

   Its form is analogous to SYSV's Control, with encoded arguments that
   should be specified with the *_Control_* macros listed here.  Its
   intended use is for special, out-of-interface-signature behavior.  For
   that reason, it is super-generic.

   This method could also be used to selectively close the capabilities
   of the port.

Prototype:
   int IPort1_Control
   (
     IPort1 *pif,
     AEEUID  uidControl,
     const byte *pIn, 
     int nInLen,
     byte *pOut, 
     int nOutLen, 
     int* pnOutLenReq
   )


Parameters:
   pif        : [in]  : pointer to the IPort interface
   uidControl : [in]  : uid of the control operation, defined by the class
                        implementing IPort1
   pIn        : [in]  : input buffer argument for the operation
   nInLen     : [in]  : length of pIn
   pOut       : [out] : output buffer argument for the operation, filled
                        up to nOutLen bytes of the result of the operation,
                        if any.
   nOutLen    : [in]  : length of pOut
   pnOutLen   : [out] : set to length of pOut required to complete 
                        the operation

Return Value :
  AEE_SUCCESS  : Successfully completed the control.

  Another appropriate error code if operation was not successful.

Comments:

  Data passed in the buffer arguments must always be arranged as 
  if they've already been marshalled, i.e. the precise byte ordering must
  be specified.  The data must not include pointers or interfaces. 


Side Effects:
   None

See Also:
   None

=======================================================================

IPort1_Close()

Description:
   This function closes the port.  Any further Read() and Write() on 
   the port will fail, and the IPort1 will become readable and writeable.

Prototype:
   int IPort1_Close
   (
      IPort1 *pif
   )

Parameters:
   pif: [in] : Pointer to the IPort interface

Return Value:

  AEE_SUCCESS      : Successfully closed the port

  Another appropriate error code if operation was not successful.

Comments:

Side Effects:
   None

See Also:
   None


=============================================================================*/

#endif //AEEIPORT1_H
