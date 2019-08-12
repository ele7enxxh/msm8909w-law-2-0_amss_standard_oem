#ifndef DS_NET_IEVENTMANAGER_H
#define DS_NET_IEVENTMANAGER_H

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
#include "ds_Net_Def.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IEventManager = 0x106d860;
      
      /** @interface IEventManager
        * 
        * ds Net Handle interface.
        */
      struct IEventManager : public ::IQI
      {
         
         /**
           * This function registers a signal to be set when event eventID occurs.
           * Cannot use the signal for more than one eventID. Can use more than one
           * signal for the same event.
           * @param signal The signal to Set() when the state associated with the eventID changes.
           *               To cancel the registration the application should release this signal. 
           * @param eventID The event for which the signal shall be fired.
           * @param regObj Output The application must hold this output registration object to ensure its
           *                      Signal stays registered. The application can release this object once
           *                      it has released the signal object.
           * @retval AEE_SUCCESS Signal set completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL OnStateChange(::ISignal* signal, ::ds::Net::EventType eventID, ::IQI** regObj) = 0;
      };
   };
};
#endif //DS_NET_IEVENTMANAGER_H
