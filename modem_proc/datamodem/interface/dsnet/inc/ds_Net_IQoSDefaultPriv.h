#ifndef DS_NET_IQOSDEFAULTPRIV_H
#define DS_NET_IQOSDEFAULTPRIV_H

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
#include "ds_Net_IQoSPriv.h"
#include "ds_Net_Def.h"
#include "ds_Net_QoS_Def.h"
#include "ds_Addr_Def.h"
#include "ds_Net_IIPFilterPriv.h"
#include "ds_Net_QoS_DefPriv.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IQoSDefaultPriv = 0x10a32b2;
      
      /** @interface IQoSDefaultPriv
        * 
        * ds Net QoS Default Private interface.
        */
      struct IQoSDefaultPriv : public ::ds::Net::IQoSPriv
      {
         
         /**
           * This function issues a request to modify the default QoS Flow specification 
           * in either Rx direction, Tx direction or both directions.
           * Note that this is a privileged operation.
           * @param requestedSpec the QoS specification used for modifying the default QoS instance.
           * @param mask The modification direction, can be RX_FLOW, TX_FLOW or RX_FLOW|TX_FLOW.
           * @retval AEE_SUCCESS Modification request received successfully.
           * @retval ds::EBADOPTS Bad options.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL ModifyDefaultPriv(const ::ds::Net::QoSSpecPrimaryType* requestedSpec, ::ds::Net::QoSModifyMaskType mask) = 0;
      };
   };
};
#endif //DS_NET_IQOSDEFAULTPRIV_H
