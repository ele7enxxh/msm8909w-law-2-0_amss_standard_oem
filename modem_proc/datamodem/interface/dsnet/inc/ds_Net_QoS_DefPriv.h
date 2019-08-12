#ifndef DS_NET_QOS_DEFPRIV_H
#define DS_NET_QOS_DEFPRIV_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Net_IQoSFlowPriv.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Net_IIPFilterPriv.h"

/**
  * ds Net QoS Def Private module.
  * This module groups all the Private QoS types and constants.
  */
namespace ds
{
   namespace Net
   {
      struct _SeqIQoSFlowPrivType__seq_IQoSFlowPriv_Net_ds {
         IQoSFlowPriv** data;
         int dataLen;
         int dataLenReq;
      };
      typedef _SeqIQoSFlowPrivType__seq_IQoSFlowPriv_Net_ds SeqIQoSFlowPrivType;
      struct QoSSpecType {
         IQoSFlowPriv** rxFlows;
         int rxFlowsLen;
         int rxFlowsLenReq;
         boolean rxMinFlow;
         AEEINTERFACE_PADMEMBERS(txFlows, 3)
         IQoSFlowPriv** txFlows;
         int txFlowsLen;
         int txFlowsLenReq;
         boolean txMinFlow;
         AEEINTERFACE_PADMEMBERS(rxFilter, 3)
         IIPFilterPriv** rxFilter;
         int rxFilterLen;
         int rxFilterLenReq;
         IIPFilterPriv** txFilter;
         int txFilterLen;
         int txFilterLenReq;
      };
      struct QoSSpecPrimaryType {
         IQoSFlowPriv** rxFlows;
         int rxFlowsLen;
         int rxFlowsLenReq;
         boolean rxMinFlow;
         AEEINTERFACE_PADMEMBERS(txFlows, 3)
         IQoSFlowPriv** txFlows;
         int txFlowsLen;
         int txFlowsLenReq;
         boolean txMinFlow;
         AEEINTERFACE_PADMEMBERS(__pad, 3)
      };
      struct QoSSpecNetInitiatedType {
         IIPFilterPriv** rxFilters;
         int rxFiltersLen;
         int rxFiltersLenReq;
         IIPFilterPriv** txFilters;
         int txFiltersLen;
         int txFiltersLenReq;
      };
      struct _SeqQoSSpecType__seq_QoSSpecType_Net_ds {
         QoSSpecType* data;
         int dataLen;
         int dataLenReq;
      };
      typedef _SeqQoSSpecType__seq_QoSSpecType_Net_ds SeqQoSSpecType;
   };
};
#endif //DS_NET_QOS_DEFPRIV_H
