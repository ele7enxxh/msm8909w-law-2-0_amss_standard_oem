#ifndef DS_NET_IIPFILTERREGPRIV_H
#define DS_NET_IIPFILTERREGPRIV_H

/*============================================================================
  Copyright (c) 20088 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_Def.h"
#include "ds_Net_IEventManager.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IIPFilterRegPriv = 0x107d26f;
      
      /** @interface IIPFilterRegPriv
        * 
        * ds Filter Registration interface.
        * This interface can be used to register for IP FILTER STATE_CHANGED Event.
        * The release of this interface deregister the Filters installed.
        */
      struct IIPFilterRegPriv : public ::IQI
      {
      };
   };
};
#endif //DS_NET_IIPFILTERREGPRIV_H
