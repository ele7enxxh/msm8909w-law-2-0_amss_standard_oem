#ifndef DS_SOCK_ISOCKET_H
#define DS_SOCK_ISOCKET_H

/*============================================================================
  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Sock_Def.h"

namespace ds
{
   namespace Sock
   {
      const ::AEEIID AEEIID_ISocket = 0x106c549;
      
      /** @interface ISocket
        * 
        */
      struct ISocket : public IQI
      {
        /**
          * This function closes the socket.  Any further Read() and Write() on the
          * socket will fail.
          *
          * @return AEE_SUCCESS if successful, or another appropriate error code if
          *         operation was not successful.
            
          */
         virtual ::AEEResult AEEINTERFACE_CDECL Close (void) = 0;

         /**
          * Copy bytes from the specified buffer to the source stream.  The write
          * pointer is advanced.
          *
          * @param buf the buffer from which bytes are taken to write to the socket
          * @param written size of data written to the socket
          *
          * @retval AEE_SUCCESS successfully wrote 'written' bytes to the socket
          * @retval AEE_ENOTALLOWED caller does not have the necessary capability to
          *                         perform the operation
          * @retval AEE_EWOULDBLOCK no data available
          *
          * @return Another appropriate error core may be returned if the operation
          *         is not successful.
    
          */
         virtual ::AEEResult AEEINTERFACE_CDECL Write (const ::byte* buf, int bufLen, int* written) = 0;
         
         /**
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
         virtual ::AEEResult AEEINTERFACE_CDECL Read (::byte* buf, int bufLen, int* bufLenReq) = 0;

         /**
           * This function associates a local address and port value to the socket.
           * @param localAddr A specification of the address and port to attach.
           *                  @See SockAddrStorageType
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval AEE_EWOULDBLOCK Request is processed asynchronously.
           *         Application should register to be notified when progress can
           *         be made and call Bind again when the notification has been
           *         received. Application should repeat this process until
           *         AEE_SUCCESS is returned from the call.
           *         Registration for the notification can be done via 
           *         ISocketExt::RegEvent (READ event).
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Bind(const ::ds::SockAddrStorageType localAddr) = 0;
         
         /**
           * This function initiates passive open for TCP connections.
           * @param backLog Maximum number of half-open TCP connections to track at one time.
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Listen(int backLog) = 0;
         
         /**
           * This function returns a newly created socket for a new passively opened connection.
           * @param remoteAddrOut Output Address of the remote end of the new connection.
           *                             @See SockAddrStorageType
           * @param newConnSock Output The newly created socket.
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval AEE_EWOULDBLOCK Request is processed asynchronously.
           *         Application should register to be notified when progress can
           *         be made and call Accept again when the notification has been
           *         received. Application should repeat this process until
           *         AEE_SUCCESS is returned from the call.
           *         Registration for the notification can be done via 
           *         ISocketExt::RegEvent (ACCEPT event).
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Accept(::ds::SockAddrStorageType remoteAddrOut, ISocket** newConnSock) = 0;
         
         /**
           * This function initiates an active open for TCP connection;
           * For UDP sockets this function sets the peer∆s IP address and port value of the socket
           * @param remoteAddr Address to connect.
           *                   @See SockAddrStorageType
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval AEE_EWOULDBLOCK Request is processed asynchronously.
           *         Application should register to be notified when progress can
           *         be made and call Connect again when the notification has been
           *         received. Application should repeat this process until
           *         AEE_SUCCESS is returned from the call.
           *         Registration for the notification can be done via 
           *         ISocketExt::RegEvent (WRITE event).
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Connect(const ::ds::SockAddrStorageType remoteAddr) = 0;
         
         /**
           * This function sends specified number of bytes across the UDP transport
           * @param buf Byte array that contains the data to be sent.
           * @param bufLen size in bytes of data to be sent.
           * @param remoteAddr A specification of the address and port to send to.
           *                   @See SockAddrStorageType 
           * @param flags Supported values are:
           *    <ul>
           *       <li> 0: No special handling.
           *       <li> MSG_EXPEDITE: Indicates that the packet should be sent on
           *                          ACH or REACH if traffic channel is not UP
           *       <li> MSG_FASTEXPEDITE: Indicates that the packet should be sent on REACH if
           *                              traffic channel is not UP
           *    </ul>
           * @param numSent If AEE_SUCCESS is returned, indicates the number of bytes actually sent.
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval AEE_EWOULDBLOCK Request is processed asynchronously.
           *         Application should register to be notified when progress can
           *         be made and call SendTo again when the notification has been
           *         received. Application should repeat this process until
           *         AEE_SUCCESS is returned from the call.
           *         Registration for the notification can be done via 
           *         ISocketExt::RegEvent (WRITE event).
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SendTo(const ::byte* buf, int bufLen, const ::ds::SockAddrStorageType remoteAddr, unsigned int flags, int* numSent) = 0;
         
         /**
           * This function reads specified number of bytes from the UDP transport
           * @param buf Input/Output Bytes array into which data can be read.
           *                         On output contains the received data.
           * @param flags Not supported - must be set to zero.
           * @param remoteAddr Output A specification of the address and port from which data was received.
           *                          @See SockAddrStorageType 
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval AEE_EWOULDBLOCK Request is processed asynchronously.
           *         Application should register to be notified when progress can
           *         be made and call RecvFrom again when the notification has been
           *         received. Application should repeat this process until
           *         AEE_SUCCESS is returned from the call.
           *         Registration for the notification can be done via 
           *         ISocketExt::RegEvent (READ event).
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RecvFrom(::byte* buf, int bufLen, int* bufLenReq, unsigned int flags, ::ds::SockAddrStorageType remoteAddr) = 0;
         
         /**
           * Use this function to get the local address assigned to the socket
           * @param localAddr Output The address assigned to the socket.
           *                         @See SockAddrStorageType
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSockName(::ds::SockAddrStorageType localAddr) = 0;
         
         /**
           * Use this function to get the remote address of the connected socket.
           * @param remoteAddr Output The remote address of the connected socket.
           *                          @See SockAddrStorageType
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetPeerName(::ds::SockAddrStorageType remoteAddr) = 0;
         
         /**
           * This function causes all or part of a full-duplex connection to be terminated gracefully.
           * @param how specifies which direction(s) of the connection to shutdown.
           * @see ShutdownDirType
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Shutdown(::ds::Sock::ShutdownDirType how) = 0;
         
         /**
           * Gets options for the socket. (compatible to socket options that use
           * a 32 bit integer or boolean as the socket option data type).
           * @param level option level (see ds::Sock::OptLevelType)
           * @param name option name (see ds::Sock::OptNameType)
           * @param value option value
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetOpt(::ds::Sock::OptLevelType level, ::ds::Sock::OptNameType name, int* value) = 0;
         
         /**
           * Sets options for the socket. (compatible to socket options that use
           * a 32 bit integer or boolean as the socket option data type).
           * @param level option level (see ds::Sock::OptLevelType)
           * @param name option name (see ds::Sock::OptNameType)
           * @param value option value
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetOpt(::ds::Sock::OptLevelType level, ::ds::Sock::OptNameType name, int value) = 0;
         
         /**
           * This function reads data and control (ancillary) data buffers over
           * the transport specified by the socket. 
           * The parameters with msg_ prefix follow the convention of the msghdr 
           * defined in RFC 3542. 
           * @param msg_name Output Address of the source. @See SockAddrStorageType
           * @param msg_buf Output Message payload.
           * @param msg_control Output Ancillary data, if applicable.
           *
           *        Note1: This sequence does *not* follow the regular 
           *        "output sequence" semantics. The LenReq parameter
           *        (implicitly included here) shall indicate the actual size of
           *        AncillaryData returned, which may be smaller or equal to the 
           *        buffer size provided by the application. Information on
           *        additional Ancillary Data that is available but does not fit
           *        into the buffer provided by the application is not supported.
           *        @See socket options QDS_IP_RECVIF and QDS_IP_RECVERR.
           *
           *        Note2: Language specific headers may be available to
           *        facilitate API usage of Ancillary Data. For C++/C see 
           *        ds_Sock_CMsg_Helpers.h
           *
           *        Note3: The ancillary header (len, level, type) inside the
           *        msg_control buffer is on little-endian byte order. For the
           *        byte ordering of the specific ancillary data part (currently
           *        ExtendedErrInfoType or RecvIfaceInfoType) please refer to
           *        its definition.
           *  
           *        Note4: Ancillary Data types:
           *
           *               cmsg_level: QDS_LEVEL_IP
           *               cmsg_type: QDS_IP_RECVIF
           *               structure of data: ds::Sock::RecvIfaceInfoType
           *
           *               cmsg_level: QDS_LEVEL_IP
           *               cmsg_type: QDS_IP_RECVERR
           *               structure of data: ds::Sock::ExtendedErrInfoType
           *
           * @See ds::Sock::RecvIfaceInfoType, ds::Sock::ExtendedErrInfoType
           *
           * @param msg_flags Output Output flags.
           *        <ul>
           *           <li> MSG_CTRUNC: Indicates amount of incoming ancillary
           *                            data that is larger than buffer supplied
           *                            by the application. The part of 
           *                            ancillary data that fits into the buffer
           *                            is provided to the application. The rest
           *                            of the ancillary data cannot be fetched.
           *           <li> MSG_TRUNC:  Indicates amount of incoming data that is
           *                            larger than buffer supplied by the
           *                            application. The part of data that fits 
           *                            into the buffer is provided to the 
           *                            application. The rest of the data cannot be fetched.
           *        </ul>
           * @param flags Input flags.
           *        <ul>
           *           <li> 0: No special handling.
           *           <li> MSG_ERRQUEUE: Retrieve ICMP errors.
           *                              @See QDS_IP_RECVERR socket option
           *        </ul>
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval AEE_EWOULDBLOCK Request is processed asynchronously.
           *         Application should register to be notified when progress can
           *         be made and call RecvMsg again when the notification has been
           *         received. Application should repeat this process until
           *         AEE_SUCCESS is returned from the call.
           *         Registration for the notification can be done via 
           *         ISocketExt::RegEvent (READ event).
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RecvMsg(::ds::SockAddrStorageType msg_name, ::byte* msg_buf, int msg_bufLen, int* msg_bufLenReq, ::byte* msg_control, int msg_controlLen, int* msg_controlLenReq, unsigned int* msg_flags, unsigned int flags) = 0;
         
         /**
           * This function sends data from the provided data and ancillary buffers
           * over the transport specified by the socket.
           * The parameters with msg_ prefix follow the convention of the msdhdr 
           * defined in RFC 3542. 
           * @param msg_name Address of the destination. @See SockAddrStorageType
           * @param msg_buf Message payload.
           * @param numWritten Output Number of bytes actually sent.
           * @param msg_control Ancillary data, if applicable. 
           *        For SendMsg AncillaryData is an input only parameter.
           *
           * @param flags Input flags. Currently none are supported.
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval AEE_EWOULDBLOCK Request is processed asynchronously.
           *         Application should register to be notified when progress can
           *         be made and call SendMsg again when the notification has been
           *         received. Application should repeat this process until
           *         AEE_SUCCESS is returned from the call.
           *         Registration for the notification can be done via 
           *         ISocketExt::RegEvent (WRITE event).
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SendMsg(const ::ds::SockAddrStorageType msg_name, const ::byte* msg_buf, int msg_bufLen, int* numWritten, const ::byte* msg_control, int msg_controlLen, unsigned int flags) = 0;
         
         /**
           * This attribute represents the SO_LINGER_RESET socket option.
           * Level: Socket
           * Value type: LingerType
           * Description: Linger and reset on timeout.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSOLingerReset(::ds::Sock::LingerType* value) = 0;
         
         /**
           * This attribute represents the SO_LINGER_RESET socket option.
           * Level: Socket
           * Value type: LingerType
           * Description: Linger and reset on timeout.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetSOLingerReset(const ::ds::Sock::LingerType* value) = 0;
      };
   };
};
#endif //DS_SOCK_ISOCKET_H
