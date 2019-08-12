#ifndef DS_NET_IMCASTMANAGEREXT_H
#define DS_NET_IMCASTMANAGEREXT_H

#ifdef FEATUTE_DATA_PS_MCAST
/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Addr_Def.h"
#include "ds_Net_MCast_Def.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_Def.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IMCastSession.h"
#include "ds_Net_IMCastSessionsInput.h"
#include "ds_Net_IMCastSessionsOutput.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IMCastManagerExt = 0x10a380d;
      
      /** @interface IMCastManagerExt
        * 
        * ds Net MCast Manager Extended interface.
        * This interface supports MultiCast management operations according to
        * BCMCS 2.0 specifications
        */
      struct IMCastManagerExt : public ::IQI
      {
         struct _SeqSockAddrStorageType__seq_octet_ds {
            ::ds::SockAddrStorageType* data;
            int dataLen;
            int dataLenReq;
         };
         typedef _SeqSockAddrStorageType__seq_octet_ds SeqSockAddrStorageType;
         struct _SeqMCastJoinFlagsType__seq_long {
            ::ds::Net::MCastJoinFlagsType* data;
            int dataLen;
            int dataLenReq;
         };
         typedef _SeqMCastJoinFlagsType__seq_long SeqMCastJoinFlagsType;
         
         /**    
           * This function issues a request for joining multiple multicast
           * groups.
           * In order to receive the MCast data, the application must
           * register to the MCast groups. If the Join Flags in parameter
           * mcastJoinFlagsSeq specifies that registration setup is allowed, 
           * this API shall take care to carry out the registration part of the
           * MCast session setup. Otherwise, the application has to use the
           * RegisterBundle API after applying JoinBundle to complete the setup.
           *
           * Using the REG_SETUP_NOT_ALLOWED flag allows applications to commit
           * the "Join" part of the MCast setup at an early stage (e.g. by
           * Joining all available MCast groups) and complete the MCast session
           * setup later only once the User requests service, thus minimizing
           * the setup time required from the moment the User requests the
           * service.
           * 
           * @See RegisterBundle.
           * @param addrSeq Multicast group addresses.
           * @param mcastSpecs A JSON string specifying extra Multicast join
           *                   information. The extra Multicast join information
           *                   is supported only for certain MCast technologies.
           *                   When using this API to join other MCast
           *                   technologies, mcastSpecs can be empty.          
           *                   The application should add the Mcast Specs info
           *                   to the JSON string in the same order of the 
           *                   addresses in addrSeq.          
           *                   Currently extra Join information is not applicable
           *                   for supported technologies hence no JSON schema
           *                   is currently specified for this information.
           * @param sessions Output IMCastSessionsOutput containing IMCastSession 
           *                        objects opened for handling the requests.
           *                        The order of sessions shall correspond to the
           *                        order of addresses in addrSeq.
           *                        @see IMCastSessionsOutput, IMcastSession
           * @param mcastJoinFlagsSeq flags for the Join operation. The order of
           *                          of flags specifications must correspond to
           *                          the order of MCast addresses in addrSeq.
           *                          Different requests in the sequence may
           *                          have different flags.
           *                          @See MCastJoinFlagsType
           * @param errSpec Output specification of errors in mcastSpecs.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.          
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL JoinBundle(const ::ds::SockAddrStorageType* addrSeq, int addrSeqLen, const char* mcastSpecs, ::ds::Net::IMCastSessionsOutput** sessions, const ::ds::Net::MCastJoinFlagsType* mcastJoinFlagsSeq, int mcastJoinFlagsSeqLen, char* errSpec, int errSpecLen, int* errSpecLenReq) = 0;
         
         /**    
           * This function issues a request for registering multiple multicast
           * groups.
           * @param sessions A MCastSessions group with which all
           *                 IMcastSessions.
           *                 objects to register are associated.
           *                 @see IMCastSessionsInput, IMcastSession.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RegisterBundle(::ds::Net::IMCastSessionsInput* sessions) = 0;
         
         /**
           * This function issues a request for leaving multiple multicast
           * groups.
           * @param sessions A MCastSessions group with which all
           IMcastSessions objects to leave are associated.
           *                 @see IMCastSessionsInput, IMcastSession.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL LeaveBundle(::ds::Net::IMCastSessionsInput* sessions) = 0;
         
         /**    
           * This function creates an instance of IMCastSessionsInput.
           * IMCastSessionsInput creation is supported only via IMCastManagerExt.
           * @param new newMCastSessionsInput Output The newly created 
           *                                         IMCastSessionsInput
           *                                         instance.
           * @retval AEE_SUCCESS IMCastSessionsInput created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateMCastSessionsInput(::ds::Net::IMCastSessionsInput** newMCastSessionsInput) = 0;
         
         /**
           * This function allows object extensibility.
           * For supported interfaces, objects implementing those interfaces may
           * be fetched via this function. The supported interfaces are
           * documented in class documentation (@See ds_Net_CNetworkFactory.idl).
           * GetTechObject-supported interface does not imply any similar
           * contract in regard to QueryInterface for the respective interface.
           * Unlike IQI, the availability of a specific interface depends on
           * some factors, e.g. current network type. Moreover, there is no
           * guaranty for transitivity or symmetry. 
           * Note: 'interface' parameter will map to iid and techObj.
           * @param iid The interface that should be retrieved.
           * @param techObj On success, will contain the requested interface
           * instance.
           * @retval AEE_SUCCESS Interface retrieved successfully.
           * @retval AEE_ECLASSNOTSUPPORT Specified interface unsupported.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTechObject(AEEIID techObj_iid, void** techObj) = 0;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_IMCASTMANAGEREXT_H
