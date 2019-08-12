#ifndef DS_NET_IQOSSECONDARYPRIV_H
#define DS_NET_IQOSSECONDARYPRIV_H

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
      const ::AEEIID AEEIID_IQoSSecondaryPriv = 0x10a32b3;
      
      /** @interface IQoSSecondaryPriv
        * 
        * ds QoS Secondary Private interface.
        */
      struct IQoSSecondaryPriv : public ::ds::Net::IQoSPriv
      {
         
         /**
           * This function issues the following modification requests:
           *  - Modify an existing QoS Session either in Rx, Tx or both directions.
           *  - Add a new QoS in a specific direction.
           *  - Remove a granted QoS in a specific direction.
           * The modification requests can be performed on Rx blocks, Tx blocks or both.
           * @param requestedSpec the QoS specification used for modifying the QoS instance
           * @param modifyMask bit mask which indicates the QoS specification blocks to be modified 
           * @retval AEE_SUCCESS Request received successfully.
           * @retval ds::EBADOPTS Bad options.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL ModifySecondaryPriv(const ::ds::Net::QoSSpecType* requestedSpec, ::ds::Net::QoSModifyMaskType modifyMask) = 0;
      };
   };
};
#endif //DS_NET_IQOSSECONDARYPRIV_H
