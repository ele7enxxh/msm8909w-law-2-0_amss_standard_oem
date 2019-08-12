#ifndef DS_NET_IMCASTMANAGER_H
#define DS_NET_IMCASTMANAGER_H

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
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_Def.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_MCast_Def.h"
#include "ds_Net_IMCastSession.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IMCastManager = 0x106cf94;
      
      /** @interface IMCastManager
        * 
        * ds Net MCast Manager interface.
        * This interface supports basic MultiCast management operations for
        * various MCast technologies inclusing MFLO, DVB-H and BCMCS 1.0 
        */
      struct IMCastManager : public ::IQI
      {
         
         /**    
           * This function issues a request for joining a multicast group and
           * registering to it.
           * @param addr Multicast group address.
           * @param mcastSpec A JSON string specifying extra Multicast join
           *                  information. The extra Multicast join information
           *                  is supported only for certain MCast technologies.
           *                  When using this API to join other MCast
           *                  technologies, mcastSpec can be empty.          
           *                  Currently extra Join information is not applicable
           *                  for supported technologies hence no JSON schema
           *                  is currently specified for this information.
           * @param session Output The IMCastSession opened for handling the
           *                       request.
           * @param errSpec Output specification of errors in mcastSpec.
           * @see IMCastSession
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Join(const ::ds::SockAddrStorageType addr, const char* mcastSpec, ::ds::Net::IMCastSession** session, char* errSpec, int errSpecLen, int* errSpecLenReq) = 0;
         
         /**
           * This function allows object extensibility.
           * For supported interfaces, objects implementing those interfaces may be 
           * fetched via this function. The supported interfaces are documented in
           * the DS_NET_NetworkFactory.bid file. GetTechObject-supported interface
           * does not imply any similar contract in regard to QueryInterface for the
           * respective interface.
           * Unlike IQI, the availability of a specific interface depends on some 
           * factors, e.g. current network type. Moreover, there is no guaranty 
           * for transitivity or symmetry. 
           * Note: 'interface' parameter will map to iid and techObj.
           * @param iid The interface that should be retrieved.
           * @param techObj On success, will contain the requested interface instance.
           * @retval ds::SUCCESS Interface retrieved successfully.
           * @retval AEE_ECLASSNOTSUPPORT Specified interface unsupported.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTechObject(AEEIID techObj_iid, void** techObj) = 0;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_IMCASTMANAGER_H
