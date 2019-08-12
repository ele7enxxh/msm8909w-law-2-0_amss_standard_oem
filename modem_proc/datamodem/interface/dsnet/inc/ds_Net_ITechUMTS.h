#ifndef DS_NET_ITECHUMTS_H
#define DS_NET_ITECHUMTS_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
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
#include "ds_Net_IPolicy.h"

/**
  * ds Technology UMTS interface.
  * Instantiation of this interface shall create an object that
  * can be used to register to MTPD events.
  */
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_ITechUMTS = 0x107d26e;
      
      /** @interface ITechUMTS
        * 
        * ds ITechUMTS interface.
        * This interface is used to register for Mobile Terminated Packet Data
        * events. Instantiation of this interface is done via INetworkFactory.
        * @See INetworkFactory::CreateTechUMTS
        *
        * To Register for MTPD events:
        * Use RegMTPD() method of this interface. Via RegMTPD() the client
        * also provides a signal object that would be set to notify the 
        * application on a requested MTPD call.
        *
        * Once the application is notified of an MTPD call it should create an
        * ACTIVE mode INetwork object specifying the same policy used in RegMTPD
        * for registering to the MTPD events (see note below on address family
        * of the policy). If this step is not carried out as soon as the
        * application is notified of the MTPD call, the call shall not be
        * established.
        * @See INetworkFactory::CreateNetwork
        * The application should use different signals for different MTPD
        * registrations. This shall allow the application to identify the
        * applicable policy for creating the INetwork object when it receives
        * an MTPD call notification.
        *
        * To transfer data over the MTPD call, the application should create
        * socket based on the ACTIVE INetwork object or on the same policy used
        * for registering to the MTPD events.
        * @See ISocketFactory::CreateSocket
        * @See ISocketFactory::CreateSocketByNetwork
        *
        * To cancel the registration the application should release the signal.
        * 
        */
      struct ITechUMTS : public ::IQI
      {
         
         /**
           * Register for MTPD notifications.
           * @param Policy Requested Network Policy. MTPD events are address
           *               family agnostic, meaning the Network cannot specify
           *               that the MTPD event is IPv4 or IPv6. Therefor, the
           *               address family of the policy shall not effect the
           *               type of MTPD events the application shall receive
           *               (in respect to address family).
           * @param signal Signal that shall be set when MTPD notification occurs.
           *               To cancel the registration the application should
           *               release this signal.
           * @param mtpdReg The application must hold this output registration
           *                object to ensure its Signal stays registered. The
           *                application can release this object once it has
           *                released the signal object.
           * @retval AEE_SUCCESS NetType successfully set in the policy.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RegMTPD(::ds::Net::IPolicy* Policy, ::ISignal* signal, ::IQI** mtpdReg) = 0;
      };
   };
};
#endif //DS_NET_ITECHUMTS_H
