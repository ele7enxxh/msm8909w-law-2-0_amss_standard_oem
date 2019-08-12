#ifndef DS_NET_INETWORKFACTORY_H
#define DS_NET_INETWORKFACTORY_H

/*============================================================================
  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Net_DownReasons_Def.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_INetwork.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_ITechUMTS.h"

#include "ds_Net_IPolicyPriv.h"
#include "ds_Net_QoS_Def.h"
#include "ds_Net_IBearerInfo.h"
#include "ds_Net_INetworkPriv.h"
#include "ds_Net_IIPFilterPriv.h"
#include "ds_Net_IQoSFlowPriv.h"
namespace ds
{
   namespace Net
   {

      /**
        * INetwork mode that allows Factory API to know which mode to use in INetwork creation
        */
      typedef int NetworkModeType;
      namespace NetworkMode
      {

         /**
           * Active INetwork. This mode should be used unless special needs are required.
           * Data Network is brought up upon instantiation of this mode of INetwork.
           */
         const ::ds::Net::NetworkModeType QDS_ACTIVE = 0x10762b4;

         /**
           * Monitored mode INetwork. This mode is intended for listening to network 
           * events without bringing up the Data Network. 
           * Some API of INetwork interface are unsupported in MONITORED MODE when
           * the underlying network interface is down.
           */
         const ::ds::Net::NetworkModeType QDS_MONITORED = 0x10762b5;
      };
      const ::AEEIID AEEIID_INetworkFactory = 0x106d851;
      struct INetworkFactory : public ::IQI
      {

         /**
           * This function creates an instance of INetwork. INetwork creation is
           * supported only via INetworkFactory. In this API the Application
           * specifies the network policy to be used for the creation of the
           * network object.
           * 
           * @param networkMode Specification of which INetwork type to create.
           * @param policy Network policy. If NULL default policy shall be used.
           * @param newNetwork Output The newly created INetwork instance.
           * @retval AEE_SUCCESS INetwork created successfully. In the case of
           *                     ACTIVE Network Mode, the application should
           *                     register to ds::Net::NetworkEvent::STATE_CHANGED
           *                     events on the new Network object and wait for
           *                     the Network state to become OPEN.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateNetwork(::ds::Net::NetworkModeType networkMode, ::ds::Net::IPolicy* policy, ::ds::Net::INetwork** newNetwork) = 0;

         /**
           * This function creates an instance of Policy. Policy creation is supported only
           * via INetworkFactory. 
           * @param newPolicy Output The newly created Policy instance.
           * @retval AEE_SUCCESS Policy created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreatePolicy(::ds::Net::IPolicy** newPolicy) = 0;

         /**
           * This function creates an instance of ITechUMTS. ITechUMTS creation is supported only
           * via INetworkFactory. 
           * @param newTechUMTS Output The newly created ITechUMTS instance.
           * @retval AEE_SUCCESS ITechUMTS created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateTechUMTS(::ds::Net::ITechUMTS** newTechUMTS) = 0;

         /**
           * This function creates an instance of PolicyPriv. PolicyPriv creation is supported only
           * via INetworkFactory. 
           * @param newPolicyPriv Output The newly created PolicyPriv instance.
           * @retval AEE_SUCCESS PolicyPriv created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreatePolicyPriv(::ds::Net::IPolicyPriv** newPolicyPriv) = 0;

         /**
           * This function creates an instance of IPFilterSpec. IPFilterSpec creation is supported only
           * via INetworkFactory. 
           * @param newIPFilterSpec Output The newly created IPFilterSpec instance.
           * @retval ds::SUCCESS IPFilterSpec created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateIPFilterSpec(::ds::Net::IIPFilterPriv** newIPFilterSpec) = 0;

         /**
           * This function creates an instance of QoSFlowSpec. QoSFlowSpec creation is supported only
           * via INetworkFactory. 
           * @param newQoSFlowSpec Output The newly created QoSFlowSpec instance.
           * @retval AEE_SUCCESS QoSFlowSpec created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateQoSFlowSpec(::ds::Net::IQoSFlowPriv** newQoSFlowSpec) = 0;
      };
   };
};
#endif //DS_NET_INETWORKFACTORY_H
