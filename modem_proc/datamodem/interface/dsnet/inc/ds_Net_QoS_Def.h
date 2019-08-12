#ifndef DS_NET_QOS_DEF_H
#define DS_NET_QOS_DEF_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Net_Def.h"

/**
  * ds Net QoS Def module.
  * This module groups all the QoS types and constants.
  */
namespace ds
{
   namespace Net
   {
      namespace QoSEvent
      {
         const ::ds::Net::EventType QDS_EV_MODIFY_RESULT = 0x106dee5;
         const ::ds::Net::EventType QDS_EV_INFO_CODE_UPDATED = 0x106dee8;
      };
      typedef int QoSInfoCodeType;
      namespace QoSInfoCode
      {
         const ::ds::Net::QoSInfoCodeType QDS_NOT_SPECIFIED = 0;
         const ::ds::Net::QoSInfoCodeType QDS_INVALID_PARAMS = 3;
         const ::ds::Net::QoSInfoCodeType QDS_INTERNAL_CALL_ENDED = 4;
         const ::ds::Net::QoSInfoCodeType QDS_INTERNAL_ERROR = 5;
         const ::ds::Net::QoSInfoCodeType QDS_INSUFFICIENT_LOCAL_RESOURCES = 6;
         const ::ds::Net::QoSInfoCodeType QDS_TIMED_OUT_OPERATION = 7;
         const ::ds::Net::QoSInfoCodeType QDS_INTERNAL_UNKNOWN_CAUSE_CODE = 8;
         const ::ds::Net::QoSInfoCodeType QDS_INTERNAL_MODIFY_IN_PROGRESS = 9;
         const ::ds::Net::QoSInfoCodeType QDS_NOT_SUPPORTED = 129;
         const ::ds::Net::QoSInfoCodeType QDS_NOT_AVAILABLE = 130;
         const ::ds::Net::QoSInfoCodeType QDS_NOT_GUARANTEED = 131;
         const ::ds::Net::QoSInfoCodeType QDS_INSUFFICIENT_NET_RESOURCES = 132;
         const ::ds::Net::QoSInfoCodeType QDS_AWARE_SYSTEM = 133;
         const ::ds::Net::QoSInfoCodeType QDS_UNAWARE_SYSTEM = 134;
         const ::ds::Net::QoSInfoCodeType QDS_REJECTED_OPERATION = 135;
         const ::ds::Net::QoSInfoCodeType QDS_WILL_GRANT_WHEN_RESUMED = 136;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_CALL_ENDED = 137;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_SVC_NOT_AVAILABLE = 138;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_L2_LINK_RELEASED = 139;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_L2_LINK_REESTAB_REJ = 140;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_L2_LINK_REESTAB_IND = 141;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_UNKNOWN_CAUSE_CODE = 142;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_DISJOINT_PROFILE_SET_SUGGESTED = 143;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_NULL_PROFILE_SUGGESTED = 144;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_UE_NOT_AUTHORIZED = 145;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_NOT_SPECIFIED = 201;
         const ::ds::Net::QoSInfoCodeType QDS_NETWORK_BUSY = 202;
      };
      typedef int QoSResultType;
      namespace QoSModifyResult
      {
         const ::ds::Net::QoSResultType QDS_ACCEPTED = 1;
         const ::ds::Net::QoSResultType QDS_REJECTED = 2;
      };
      typedef unsigned int QoSModifyMaskType;
      namespace QoSModifyMask
      {
         const ::ds::Net::QoSModifyMaskType QDS_NONE = 0x0;
         const ::ds::Net::QoSModifyMaskType QDS_RX_FLOW = 0x1;
         const ::ds::Net::QoSModifyMaskType QDS_RX_SPEC_ADD = 0x4;
         const ::ds::Net::QoSModifyMaskType QDS_TX_FLOW = 0x8;
         const ::ds::Net::QoSModifyMaskType QDS_RX_ALL = 0xf;
         const ::ds::Net::QoSModifyMaskType QDS_TX_SPEC_ADD = 0x20;
         const ::ds::Net::QoSModifyMaskType QDS_RX_FILTER = 0x40;
         const ::ds::Net::QoSModifyMaskType QDS_TX_FILTER = 0x80;
         const ::ds::Net::QoSModifyMaskType QDS_TX_ALL = 0xf0;
         const ::ds::Net::QoSModifyMaskType QDS_ALL = 0xff;
         const ::ds::Net::QoSModifyMaskType QDS_RX_SPEC_DELETE = 0x100;
         const ::ds::Net::QoSModifyMaskType QDS_TX_SPEC_DELETE = 0x200;
      };
      typedef int QoSRequestOpCodeType;
      namespace QoSRequestOpCode
      {
         const ::ds::Net::QoSRequestOpCodeType QDS_REQUEST = 0;
         const ::ds::Net::QoSRequestOpCodeType QDS_CONFIGURE = 1;
      };
      typedef int QoSProfileIdType;
      namespace QoSProfileIds
      {
         const int MAX_SUPPORTED_PROFILES = 255;
      };
      struct QoSTXQueueLevelType {
         int currentNewDataCnt; /**< Pending new data in the default watermark */
         int wmFreeCnt; /**< Empty space on the queue (which is the hi watermark
                          the pending new data in the queue) */
         int totalPendingCnt; /**< Total amount of data pending New data +
                                retransmission queue */
      };
   };
};
#endif //DS_NET_QOS_DEF_H
