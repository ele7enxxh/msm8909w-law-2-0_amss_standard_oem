#ifndef DS_NET_IMCASTMANAGERPRIV_H
#define DS_NET_IMCASTMANAGERPRIV_H

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
#include "ds_Net_IMCastSessionPriv.h"
#include "ds_Net_IMBMSSpecPriv.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IMCastManagerPriv = 0x109b2e5;
      
      /** @interface IMCastManagerPriv
        * 
        * ds Net MCast Manager Private interface.
        * This interface supports both BCMCS 1.0 and BCMCS 2.0 MCast semantics.
        */
      struct IMCastManagerPriv : public ::IQI
      {
         struct _SeqSockAddrStorageType__seq_octet_ds {
            ::ds::SockAddrStorageType* data;
            int dataLen;
            int dataLenReq;
         };
         typedef _SeqSockAddrStorageType__seq_octet_ds SeqSockAddrStorageType;
         struct _SeqIQI__seq_IQI {
            ::IQI** data;
            int dataLen;
            int dataLenReq;
         };
         typedef _SeqIQI__seq_IQI SeqIQI;
         struct _SeqIMCastSessionPrivType__seq_IMCastSessionPriv_Net_ds {
            ::ds::Net::IMCastSessionPriv** data;
            int dataLen;
            int dataLenReq;
         };
         typedef _SeqIMCastSessionPrivType__seq_IMCastSessionPriv_Net_ds SeqIMCastSessionPrivType;
         struct _SeqMCastJoinFlagsType__seq_long {
            ::ds::Net::MCastJoinFlagsType* data;
            int dataLen;
            int dataLenReq;
         };
         typedef _SeqMCastJoinFlagsType__seq_long SeqMCastJoinFlagsType;
         
         /**    
           * This function issues a request for joining a multicast group and
           * registering to it.
           * @param addr Multicast group address.
           * @param mcastSpecInfo Multicast extra join information. May be NULL
           *                      if no extra information is needed.          
           * @param session The IMCastSession opened for handling the request.
           * @see IMCastSession
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Join(const ::ds::SockAddrStorageType addr, ::IQI* mcastSpecInfo, ::ds::Net::IMCastSessionPriv** session) = 0;
         
         /**    
           * This function issues a request for joining multiple multicast
           * groups.
           * In order to receive the MCast data, the application must also 
           * register to the MCast groups using the RegisterBundle API.
           * Applying JoinBundle allows applications to commit the "Join" part
           * of the MCast setup at an early stage (e.g. by Joining all available
           * MCast groups) and apply only the RegisterBundle API once the User
           * is requesting service, thus minimizing the setup time required from
           * the moment the User requests the service.          
           * @See RegisterBundle.
           * @param addrSeq Multicast group addresses.
           * @param mcastSpecInfoSeq Multicast extra join information sequence.
           * @param mcastJoinFlagsSeq flags for the Join operation
           * @param sessions Output IMCastSessionsOutput containing IMCastSession 
           *                        objects opened for handling the requests.
           *                        The order of sessions shall correspond to the
           *                        order of addresses in addrSeq.
           *                        @see IMCastSessionsOutput, IMcastSession
           * @param errSpec Output specification of errors in mcastSpecs.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.          
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL JoinBundle(const ::ds::SockAddrStorageType* addrSeq, int addrSeqLen, ::IQI** mcastSpecInfoSeq, int mcastSpecInfoSeqLen, const ::ds::Net::MCastJoinFlagsType* mcastJoinFlagsSeq, int mcastJoinFlagsSeqLen, ::ds::Net::IMCastSessionPriv** sessions, int sessionsLen, int* sessionsLenReq) = 0;
         
         /**    
           * This function issues a request for registering multiple multicast
           * groups.
           * @param sessions A MCastSessions group with which all
           *                 IMcastSessions.
           *                 objects to register are associated.
           *                 @see IMCastSessionsInput, IMcastSession.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RegisterBundle(::ds::Net::IMCastSessionPriv** sessions, int sessionsLen) = 0;
         
         /**
           * This function issues a request for leaving multiple multicast
           * groups.
           * @param sessions A MCastSessions group with which all
           IMcastSessions objects to leave are associated.
           *                 @see IMCastSessionsInput, IMcastSession.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL LeaveBundle(::ds::Net::IMCastSessionPriv** sessions, int sessionsLen) = 0;
         
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
           * @retval ds::SUCCESS Interface retrieved successfully.
           * @retval AEE_ECLASSNOTSUPPORT Specified interface unsupported.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTechObject(AEEIID techObj_iid, void** techObj) = 0;
         
         /**    
           * This function creates an instance of MBMSSpec. MBMSSpec creation is
           * supported only via IMCastManagerPriv. 
           * IMBMSSpec instance can be used to specify MBMS MCast Join information
           * in IMCastManager::Join and IMCastManager::JoinBundle.
           * @param newMBMSSpec Output The newly created MBMSSpec instance.
           * @retval AEE_SUCCESS MBMSSpec created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see IMCastManager::Join
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateMBMSSpecPriv(::ds::Net::IMBMSSpecPriv** newMBMSSpecPriv) = 0;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_IMCASTMANAGERPRIV_H
