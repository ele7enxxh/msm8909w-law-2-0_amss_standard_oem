#ifndef DS_SOCK_ISOCKETLOCALPRIV_H
#define DS_SOCK_ISOCKETLOCALPRIV_H

/*============================================================================
  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#if !defined(AEEINTERFACE_CPLUSPLUS)
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_IPort1.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Sock_Def.h"
#include "ds_Sock_IAncDataPriv.h"

/**
  * This is an internal interface for ds_Socket.
  *
  * This interface exposes extra socket functionality for use 
  * by the implementation of QCM API and the DSS API Compatibility Layer.
  *
  * This interface is local (not remoted) and as such is not usable outside DS.
  */

/**
  * Private Socket Protocol Values.
  */
#define ds_Sock_Protocol_QDS_ICMP 1
#define ds_Sock_OptLevel_QDS_LEVEL_ICMP 5
typedef int ds_Sock_OptNameTypePriv;

/** @memberof ds_Sock_OptNamePriv
  * 
  * Option name: QDS_ICMP_ECHO_ID
  * Level: ICMP
  * Value type: int
  * Description: ICMP ECHO_REQ message, identifier
  */
#define ds_Sock_OptNamePriv_QDS_ICMP_ECHO_ID 0

/** @memberof ds_Sock_OptNamePriv
  * 
  * Option name: QDS_ICMP_ECHO_SEQ_NUM
  * Level: ICMP
  * Value type: int
  * Description: ICMP ECHO_REQ message, sequence num
  */
#define ds_Sock_OptNamePriv_QDS_ICMP_ECHO_SEQ_NUM 1

/** @memberof ds_Sock_OptNamePriv
  * 
  * Option name: QDS_ICMP_TYPE
  * Level: ICMP
  * Value type: long
  * Description: ICMP message type
  */
#define ds_Sock_OptNamePriv_QDS_ICMP_TYPE 2

/** @memberof ds_Sock_OptNamePriv
  * 
  * Option name: QDS_ICMP_CODE
  * Level: ICMP
  * Value type: long
  * Description: ICMP message code
  */
#define ds_Sock_OptNamePriv_QDS_ICMP_CODE 3
struct ds_Sock__IOBlocksType__seq__seq_byte_IPort1 {
   IPort1_SeqBytes* data;
   int dataLen;
   int dataLenReq;
};
typedef struct ds_Sock__IOBlocksType__seq__seq_byte_IPort1 ds_Sock__IOBlocksType__seq__seq_byte_IPort1;
typedef ds_Sock__IOBlocksType__seq__seq_byte_IPort1 ds_Sock_IOBlocksType;
#define ds_Sock_AEEIID_ISocketLocalPriv 0x106d9e1
#define INHERIT_ds_Sock_ISocketLocalPriv(iname) \
   INHERIT_IQI(iname); \
   AEEResult (*GetSystemOption)(iname* _pif, boolean* value); \
   AEEResult (*SetSystemOption)(iname* _pif, boolean value); \
   AEEResult (*RecvMsg)(iname* _pif, ds_SockAddrStorageType name, IPort1_SeqBytes* bufs, int bufsLen, int* bufsLenReq, struct ds_Sock_IAncDataPriv** outAncillaryData, int outAncillaryDataLen, int* outAncillaryDataLenReq, unsigned int* outFlags, unsigned int flags); \
   AEEResult (*SendMsg)(iname* _pif, const ds_SockAddrStorageType name, const IPort1_SeqBytes* bufs, int bufsLen, int* numWritten, struct ds_Sock_IAncDataPriv** inAncillaryData, int inAncillaryDataLen, unsigned int flags); \
   AEEResult (*WriteV)(iname* _pif, const IPort1_SeqBytes* bufs, int bufsLen, int* numWritten); \
   AEEResult (*ReadV)(iname* _pif, IPort1_SeqBytes* bufs, int bufsLen, int* bufsLenReq); \
   AEEResult (*GetOptPriv)(iname* _pif, ds_Sock_OptLevelType level, ds_Sock_OptNameTypePriv name, int* value); \
   AEEResult (*SetOptPriv)(iname* _pif, ds_Sock_OptLevelType level, ds_Sock_OptNameTypePriv name, int value)
AEEINTERFACE_DEFINE(ds_Sock_ISocketLocalPriv);

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * Increment the object's reference count.
  *
  * @param _pif Pointer to interface
  **/
static __inline uint32 ds_Sock_ISocketLocalPriv_AddRef(ds_Sock_ISocketLocalPriv* _pif)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->AddRef(_pif);
}

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * Decrement the object's reference count.  If zero, free the object.
  *
  * @param _pif Pointer to interface
  **/
static __inline uint32 ds_Sock_ISocketLocalPriv_Release(ds_Sock_ISocketLocalPriv* _pif)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->Release(_pif);
}

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * Detect if an object implements the requested interface.
  *
  * @param _pif Pointer to interface
  * @param iqi_iid Interface ID of requested interface
  * @param iqi Requested interface.
  */
static __inline int ds_Sock_ISocketLocalPriv_QueryInterface(ds_Sock_ISocketLocalPriv* _pif, AEEIID iqi_iid, void** iqi)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->QueryInterface(_pif, iqi_iid, iqi);
}

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * This function sets the socket as system socket.
  * @param _pif Pointer to interface
  * @param value Attribute value
  */
static __inline AEEResult ds_Sock_ISocketLocalPriv_GetSystemOption(ds_Sock_ISocketLocalPriv* _pif, boolean* value)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->GetSystemOption(_pif, value);
}

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * This function sets the socket as system socket.
  * @param _pif Pointer to interface
  * @param value Attribute value
  */
static __inline AEEResult ds_Sock_ISocketLocalPriv_SetSystemOption(ds_Sock_ISocketLocalPriv* _pif, boolean value)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->SetSystemOption(_pif, value);
}

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * This function reads data and ancillary data IO blocks buffers and ancillaryData structs over
  * the transport specified by the socket. The msghdr parameter as defined in RFC 3542 is split here
  * to several parameters, each for every individual member of the struct. This is done to allow some
  * of the members to be both input and output parameters.
  * @param _pif Pointer to interface
  * @param name Output Address of the source.
  * @param bufs Output Message payload.
  * @param bufsLen Length of sequence
  * @param bufsLenReq Required length of sequence
  * @param outAncillaryData Output Ancillary data. Note: This sequence does *not* follow
  *                                the regular "output sequence" semantics. The LenReq parameter
  *                                (implicitly included here) shall indicate the actual size of
  *                                AncillaryData returned, which may be smaller or equal to the buffer 
  *                                size provided by the application. Information on additional Ancillary
  *                                Data that is available but does not fit into the buffer provided
  *                                by the application is not supported.
  * @param outAncillaryDataLen Length of sequence
  * @param outAncillaryDataLenReq Required length of sequence
  * @param outFlags Output Output flags.
  * @param flags Input flags. Currently only MSG_ERRQUEUE is supported - used for retrieving ICMP errors.
  * @retval AEE_SUCCESS The operation completed successfully.
  * @retval Other ds designated error codes might be returned.
  * @see ds_Errors_Def.idl.
  */
static __inline AEEResult ds_Sock_ISocketLocalPriv_RecvMsg(ds_Sock_ISocketLocalPriv* _pif, ds_SockAddrStorageType name, IPort1_SeqBytes* bufs, int bufsLen, int* bufsLenReq, struct ds_Sock_IAncDataPriv** outAncillaryData, int outAncillaryDataLen, int* outAncillaryDataLenReq, unsigned int* outFlags, unsigned int flags)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->RecvMsg(_pif, name, bufs, bufsLen, bufsLenReq, outAncillaryData, outAncillaryDataLen, outAncillaryDataLenReq, outFlags, flags);
}

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * This function sends data from the provided IO blocks over the 
  * transport specified by the socket.
  * @param _pif Pointer to interface
  * @param name Address of the destination.
  * @param bufs Message payload.
  * @param bufsLen Length of sequence
  * @param numWritten Output Number of bytes actually sent.
  * @param inAncillaryData Ancillary data, if applicable. 
  *        For SendMsg AncillaryData is an input only parameter
  * @param inAncillaryDataLen Length of sequence
  * @param flags Input flags. Currently none are supported.
  * @retval AEE_SUCCESS The operation completed successfully.
  * @retval Other ds designated error codes might be returned.
  * @see ds_Errors_Def.idl.
  */
static __inline AEEResult ds_Sock_ISocketLocalPriv_SendMsg(ds_Sock_ISocketLocalPriv* _pif, const ds_SockAddrStorageType name, const IPort1_SeqBytes* bufs, int bufsLen, int* numWritten, struct ds_Sock_IAncDataPriv** inAncillaryData, int inAncillaryDataLen, unsigned int flags)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->SendMsg(_pif, name, bufs, bufsLen, numWritten, inAncillaryData, inAncillaryDataLen, flags);
}

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * This function sends specified number of bytes from one or more buffers provided by the caller
  * across the TCP/UDP transport.
  * @param _pif Pointer to interface
  * @param bufs Byte arrays that contain the data to be sent.
  * @param bufsLen Length of sequence
  * @param numWritten If AEE_SUCCESS is returned, indicates the total number of bytes successfully written.
  * @retval AEE_SUCCESS The operation completed successfully.
  * @retval Other ds designated error codes might be returned.
  * @see ds_Errors_Def.idl.
  */
static __inline AEEResult ds_Sock_ISocketLocalPriv_WriteV(ds_Sock_ISocketLocalPriv* _pif, const IPort1_SeqBytes* bufs, int bufsLen, int* numWritten)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->WriteV(_pif, bufs, bufsLen, numWritten);
}

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * This function reads specified number of bytes from the TCP/UDP transport into
  * one or more buffers provided by the caller.
  * @param _pif Pointer to interface
  * @param bufs Input/Output Bytes arrays into which data can be read. On output contains the
  *                          read data. This parameter implicitly includes the buffer input
  *                          and output size parameter.
  * @param bufsLen Length of sequence
  * @param bufsLenReq Required length of sequence
  * @retval AEE_SUCCESS The operation completed successfully.
  * @retval Other ds designated error codes might be returned.
  * @see ds_Errors_Def.idl.
  */
static __inline AEEResult ds_Sock_ISocketLocalPriv_ReadV(ds_Sock_ISocketLocalPriv* _pif, IPort1_SeqBytes* bufs, int bufsLen, int* bufsLenReq)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->ReadV(_pif, bufs, bufsLen, bufsLenReq);
}

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * Gets non-public options for the socket. (compatible to socket options 
  * that use a 32 bit integer as the socket option data type).
  * @param _pif Pointer to interface
  * @param level option level (see ds_Sock_OptLevelType)
  * @param name option name (see ds_Sock_OptNameTypePriv)
  * @param value option value
  * @retval AEE_SUCCESS The operation completed successfully.
  * @retval Other ds designated error codes might be returned.
  * @see ds_Errors_Def.idl.
  */
static __inline AEEResult ds_Sock_ISocketLocalPriv_GetOptPriv(ds_Sock_ISocketLocalPriv* _pif, ds_Sock_OptLevelType level, ds_Sock_OptNameTypePriv name, int* value)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->GetOptPriv(_pif, level, name, value);
}

/** @memberof ds_Sock_ISocketLocalPriv
  * 
  * Sets non-public options for the socket. (compatible to socket options 
  * that use a 32 bit integer as the socket option data type).
  * @param _pif Pointer to interface
  * @param level option level (see ds_Sock_OptLevelType)
  * @param name option name (see ds_Sock_OptNameTypePriv)
  * @param value option value
  * @retval AEE_SUCCESS The operation completed successfully.
  * @retval Other ds designated error codes might be returned.
  * @see ds_Errors_Def.idl.
  */
static __inline AEEResult ds_Sock_ISocketLocalPriv_SetOptPriv(ds_Sock_ISocketLocalPriv* _pif, ds_Sock_OptLevelType level, ds_Sock_OptNameTypePriv name, int value)
{
   return AEEGETPVTBL(_pif, ds_Sock_ISocketLocalPriv)->SetOptPriv(_pif, level, name, value);
}
#else /* C++ */
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_IPort1.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Sock_Def.h"
#include "ds_Sock_IAncDataPriv.h"

/**
  * This is an internal interface for ds::Socket.
  *
  * This interface exposes extra socket functionality for use 
  * by the implementation of QCM API and the DSS API Compatibility Layer.
  *
  * This interface is local (not remoted) and as such is not usable outside DS.
  */
namespace ds
{
   namespace Sock
   {
      
      /**
        * Private Socket Protocol Values.
        */
      namespace Protocol
      {
         const ::ds::Sock::ProtocolType QDS_ICMP = 1;
      };
      namespace OptLevel
      {
         const ::ds::Sock::OptLevelType QDS_LEVEL_ICMP = 5;
      };
      typedef int OptNameTypePriv;
      namespace OptNamePriv
      {
         
         /**
           * Option name: QDS_ICMP_ECHO_ID
           * Level: ICMP
           * Value type: int
           * Description: ICMP ECHO_REQ message, identifier
           */
         const ::ds::Sock::OptNameTypePriv QDS_ICMP_ECHO_ID = 0;
         
         /**
           * Option name: QDS_ICMP_ECHO_SEQ_NUM
           * Level: ICMP
           * Value type: int
           * Description: ICMP ECHO_REQ message, sequence num
           */
         const ::ds::Sock::OptNameTypePriv QDS_ICMP_ECHO_SEQ_NUM = 1;
         
         /**
           * Option name: QDS_ICMP_TYPE
           * Level: ICMP
           * Value type: long
           * Description: ICMP message type
           */
         const ::ds::Sock::OptNameTypePriv QDS_ICMP_TYPE = 2;
         
         /**
           * Option name: QDS_ICMP_CODE
           * Level: ICMP
           * Value type: long
           * Description: ICMP message code
           */
         const ::ds::Sock::OptNameTypePriv QDS_ICMP_CODE = 3;
      };
      struct _IOBlocksType__seq__seq_byte_IPort1 {
         ::IPort1::SeqBytes* data;
         int dataLen;
         int dataLenReq;
      };
      typedef _IOBlocksType__seq__seq_byte_IPort1 IOBlocksType;
      const ::AEEIID AEEIID_ISocketLocalPriv = 0x106d9e1;
      struct ISocketLocalPriv : public ::IQI
      {
         
         /**
           * This function sets the socket as system socket.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSystemOption(boolean* value) = 0;
         
         /**
           * This function sets the socket as system socket.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetSystemOption(boolean value) = 0;
         
         /**
           * This function reads data and ancillary data IO blocks buffers and ancillaryData structs over
           * the transport specified by the socket. The msghdr parameter as defined in RFC 3542 is split here
           * to several parameters, each for every individual member of the struct. This is done to allow some
           * of the members to be both input and output parameters.
           * @param name Output Address of the source.
           * @param bufs Output Message payload.
           * @param outAncillaryData Output Ancillary data. Note: This sequence does *not* follow
           *                                the regular "output sequence" semantics. The LenReq parameter
           *                                (implicitly included here) shall indicate the actual size of
           *                                AncillaryData returned, which may be smaller or equal to the buffer 
           *                                size provided by the application. Information on additional Ancillary
           *                                Data that is available but does not fit into the buffer provided
           *                                by the application is not supported.
           * @param outFlags Output Output flags.
           * @param flags Input flags. Currently only MSG_ERRQUEUE is supported - used for retrieving ICMP errors.
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RecvMsg(::ds::SockAddrStorageType name, ::IPort1::SeqBytes* bufs, int bufsLen, int* bufsLenReq, ::ds::Sock::IAncDataPriv** outAncillaryData, int outAncillaryDataLen, int* outAncillaryDataLenReq, unsigned int* outFlags, unsigned int flags) = 0;
         
         /**
           * This function sends data from the provided IO blocks over the 
           * transport specified by the socket.
           * @param name Address of the destination.
           * @param bufs Message payload.
           * @param numWritten Output Number of bytes actually sent.
           * @param inAncillaryData Ancillary data, if applicable. 
           *        For SendMsg AncillaryData is an input only parameter
           * @param flags Input flags. Currently none are supported.
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SendMsg(const ::ds::SockAddrStorageType name, const ::IPort1::SeqBytes* bufs, int bufsLen, int* numWritten, ::ds::Sock::IAncDataPriv** inAncillaryData, int inAncillaryDataLen, unsigned int flags) = 0;
         
         /**
           * This function sends specified number of bytes from one or more buffers provided by the caller
           * across the TCP/UDP transport.
           * @param bufs Byte arrays that contain the data to be sent.
           * @param numWritten If AEE_SUCCESS is returned, indicates the total number of bytes successfully written.
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL WriteV(const ::IPort1::SeqBytes* bufs, int bufsLen, int* numWritten) = 0;
         
         /**
           * This function reads specified number of bytes from the TCP/UDP transport into
           * one or more buffers provided by the caller.
           * @param bufs Input/Output Bytes arrays into which data can be read. On output contains the
           *                          read data. This parameter implicitly includes the buffer input
           *                          and output size parameter.
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL ReadV(::IPort1::SeqBytes* bufs, int bufsLen, int* bufsLenReq) = 0;
         
         /**
           * Gets non-public options for the socket. (compatible to socket options 
           * that use a 32 bit integer as the socket option data type).
           * @param level option level (see ds::Sock::OptLevelType)
           * @param name option name (see ds::Sock::OptNameTypePriv)
           * @param value option value
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetOptPriv(::ds::Sock::OptLevelType level, ::ds::Sock::OptNameTypePriv name, int* value) = 0;
         
         /**
           * Sets non-public options for the socket. (compatible to socket options 
           * that use a 32 bit integer as the socket option data type).
           * @param level option level (see ds::Sock::OptLevelType)
           * @param name option name (see ds::Sock::OptNameTypePriv)
           * @param value option value
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetOptPriv(::ds::Sock::OptLevelType level, ::ds::Sock::OptNameTypePriv name, int value) = 0;
      };
   };
};
#endif /* !defined(AEEINTERFACE_CPLUSPLUS) */
#endif //DS_SOCK_ISOCKETLOCALPRIV_H
