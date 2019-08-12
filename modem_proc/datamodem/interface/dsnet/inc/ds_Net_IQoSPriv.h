#ifndef DS_NET_IQOSPRIV_H
#define DS_NET_IQOSPRIV_H

/*============================================================================
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Net_IQoSFlowPriv.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IQoSPriv = 0x10a32b0;
      
      /** @interface IQoSPriv
        * 
        * ds Net QoS Private interface.
        */
      struct IQoSPriv : public ::IQI
      {
         
         /**
           * This function returns the current Rx and Tx flow specification.
           * @param rxFlow the current Rx flow specification.
           * @param txFlow the current Tx flow specification.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval ds::EBADOPTS Bad options.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetGrantedFlowSpecPriv(::ds::Net::IQoSFlowPriv** rxFlow, ::ds::Net::IQoSFlowPriv** txFlow) = 0;

        /**
          * This function returns flow handle.
          * @retval flow handle.
          */
	virtual int32 AEEINTERFACE_CDECL GetFlowPriv() = 0;
      };
   };
};
#endif //DS_NET_IQOSPRIV_H
