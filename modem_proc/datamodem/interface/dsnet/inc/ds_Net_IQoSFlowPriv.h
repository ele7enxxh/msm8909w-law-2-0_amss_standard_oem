#ifndef DS_NET_IQOSFLOWPRIV_H
#define DS_NET_IQOSFLOWPRIV_H

/*============================================================================
  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
namespace ds
{
   namespace Net
   {
      typedef unsigned int QoSFlowOptIDType;
      namespace QoSFlowOptID
      {
         const ::ds::Net::QoSFlowOptIDType QDS_IP_TRAFFIC_CLASS = 0x1;
         const ::ds::Net::QoSFlowOptIDType QDS_DATA_RATE_TOKEN_BUCKET = 0x2;
         const ::ds::Net::QoSFlowOptIDType QDS_LATENCY = 0x4;
         const ::ds::Net::QoSFlowOptIDType QDS_LATENCY_VARIANCE = 0x8;
         const ::ds::Net::QoSFlowOptIDType QDS_PACKET_ERROR_RATE = 0x10;
         const ::ds::Net::QoSFlowOptIDType QDS_MIN_POLICED_PACKET_SIZE = 0x20;
         const ::ds::Net::QoSFlowOptIDType QDS_MAX_ALLOWED_PACKET_SIZE = 0x40;
         const ::ds::Net::QoSFlowOptIDType QDS_UMTS_RESIDUAL_BIT_ERROR_RATE = 0x80;
         const ::ds::Net::QoSFlowOptIDType QDS_UMTS_TRAFFIC_PRIORITY = 0x100;
         const ::ds::Net::QoSFlowOptIDType QDS_CDMA_PROFILE_ID = 0x200;
         const ::ds::Net::QoSFlowOptIDType QDS_WLAN_USER_PRI = 0x400;
         const ::ds::Net::QoSFlowOptIDType QDS_WLAN_MIN_SERVICE_INTERVAL = 0x800;
         const ::ds::Net::QoSFlowOptIDType QDS_WLAN_MAX_SERVICE_INTERVAL = 0x1000;
         const ::ds::Net::QoSFlowOptIDType QDS_WLAN_INACTIVITY_INTERVAL = 0x2000;
         const ::ds::Net::QoSFlowOptIDType QDS_NOMINAL_SDU_SIZE = 0x4000;
         const ::ds::Net::QoSFlowOptIDType QDS_CDMA_FLOW_PRI = 0x8000;
         const ::ds::Net::QoSFlowOptIDType QDS_UMTS_IMS_SIGNALING_CONTEXT = 0x10000;
         const ::ds::Net::QoSFlowOptIDType QDS_UMTS_HIGH_PRIORITY_DATA = 0x20000;
         const ::ds::Net::QoSFlowOptIDType QDS_DATA_RATE_MIN_MAX = 0x40000;
         const ::ds::Net::QoSFlowOptIDType QDS_DATA_RATE_MIN_MAX_EX = 0x80000;
      };
      struct _SeqQoSFlowOptIDType__seq_unsignedLong {
         QoSFlowOptIDType* data;
         int dataLen;
         int dataLenReq;
      };
      typedef _SeqQoSFlowOptIDType__seq_unsignedLong SeqQoSFlowOptIDType;
      typedef int QoSFlowOptIPTrafficClassType;
      namespace QoSFlowOptIPTrafficClass
      {
         const ::ds::Net::QoSFlowOptIPTrafficClassType QDS_CONVERSATIONAL = 0;
         const ::ds::Net::QoSFlowOptIPTrafficClassType QDS_STREAMING = 1;
         const ::ds::Net::QoSFlowOptIPTrafficClassType QDS_INTERACTIVE = 2;
         const ::ds::Net::QoSFlowOptIPTrafficClassType QDS_BACKGROUND = 3;
      };
      typedef int QoSFlowOptLatencyType;
      typedef int QoSFlowOptLatencyVarianceType;
      typedef int QoSFlowOptMinPolicedPacketSizeType;
      typedef int QoSFlowOptMaxAllowedPacketSizeType;
      typedef int QoSFlowOptUMTSResidualBitErrorRateType;
      namespace QoSFlowOptUMTSResidualBitErrorRate
      {
         const ::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType QDS_RATE1 = 0;
         const ::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType QDS_RATE2 = 1;
         const ::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType QDS_RATE3 = 2;
         const ::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType QDS_RATE4 = 3;
         const ::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType QDS_RATE5 = 4;
         const ::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType QDS_RATE6 = 5;
         const ::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType QDS_RATE7 = 6;
         const ::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType QDS_RATE8 = 7;
         const ::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType QDS_RATE9 = 8;
         const ::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType QDS_RATE10 = 9;
      };
      typedef int QoSFlowOptUMTSTrafficPriorityType;
      namespace QoSFlowOptUMTSTrafficPriority
      {
         const ::ds::Net::QoSFlowOptUMTSTrafficPriorityType QDS_PRI1 = 0;
         const ::ds::Net::QoSFlowOptUMTSTrafficPriorityType QDS_PRI2 = 1;
         const ::ds::Net::QoSFlowOptUMTSTrafficPriorityType QDS_PRI3 = 2;
      };
      typedef unsigned short QoSFlowOptCDMAProfileIDType;
      typedef unsigned char QoSFlowOptCDMAFlowPriorityType;
      typedef int QoSFlowOptWLANUserPriorityType;
      namespace QoSFlowOptWLANUserPriority
      {
         const ::ds::Net::QoSFlowOptWLANUserPriorityType QDS_BEST_EFFORT = 0;
         const ::ds::Net::QoSFlowOptWLANUserPriorityType QDS_BACKGROUND = 1;
         const ::ds::Net::QoSFlowOptWLANUserPriorityType QDS_RESERVED = 2;
         const ::ds::Net::QoSFlowOptWLANUserPriorityType QDS_EXCELLENT_EFFORT = 3;
         const ::ds::Net::QoSFlowOptWLANUserPriorityType QDS_CONTROLLED_LOAD = 4;
         const ::ds::Net::QoSFlowOptWLANUserPriorityType QDS_VIDEO = 5;
         const ::ds::Net::QoSFlowOptWLANUserPriorityType QDS_VOICE = 6;
         const ::ds::Net::QoSFlowOptWLANUserPriorityType QDS_NETWORK_CONTROL = 7;
      };
      typedef int QoSFlowOptWLANMinServiceIntervalType;
      typedef int QoSFlowOptWLANMaxServiceIntervalType;
      typedef int QoSFlowOptWLANInactivityIntervalType;
      typedef boolean QoSFlowOptUMTSImsSignalingContextType;
      typedef boolean QoSFlowOptUMTSHighPriorityDataType;
      struct QoSFlowOptDataRateMinMaxType {
         int maxRate;
         int guaranteedRate;
      };
      struct QoSFlowOptDataRateMinMaxExType {
         int64 maxRate;
         int64 guaranteedRate;
      };
      struct QoSFlowOptDataRateTokenBucketType {
         int peakRate;
         int tokenRate;
         int size;
      };
      struct QoSFlowOptPacketErrorRateType {
         unsigned short multiplier;
         unsigned short exponent;
      };
      struct QoSFlowOptNominalSDUSizeType {
         boolean fixed;
         AEEINTERFACE_PADMEMBERS(size, 3)
         int size;
      };
      const ::AEEIID AEEIID_IQoSFlowPriv = 0x106cd47;
      
      /** @interface IQoSFlowPriv
        * 
        * ds Net QoS Flow interface.
        */
      struct IQoSFlowPriv : public ::IQI
      {
         
         /**
           * This function creates an identical copy of the IQoSFlowPriv.
           * @param flow The created IQoSFlowPriv.
           * @retval ds::SUCCESS IQoSFlowPriv cloned successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Clone(IQoSFlowPriv** flow) = 0;
         
         /**
           * This attribute represents the valid options - if an option was set,
           * its ID will be in this list.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetValidOptions(::ds::Net::QoSFlowOptIDType* value, int valueLen, int* valueLenReq) = 0;
         
         /**
           * This attribute represents a list of erroneous options into
           * the IQoSFlowPriv object.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetErroneousOptions(::ds::Net::QoSFlowOptIDType* value, int valueLen, int* valueLenReq) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetTrfClass(::ds::Net::QoSFlowOptIPTrafficClassType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetTrfClass(::ds::Net::QoSFlowOptIPTrafficClassType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetLatency(::ds::Net::QoSFlowOptLatencyType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetLatency(::ds::Net::QoSFlowOptLatencyType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetLatencyVar(::ds::Net::QoSFlowOptLatencyVarianceType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetLatencyVar(::ds::Net::QoSFlowOptLatencyVarianceType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetMinPolicedPktSize(::ds::Net::QoSFlowOptMinPolicedPacketSizeType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetMinPolicedPktSize(::ds::Net::QoSFlowOptMinPolicedPacketSizeType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetMaxAllowedPktSize(::ds::Net::QoSFlowOptMaxAllowedPacketSizeType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetMaxAllowedPktSize(::ds::Net::QoSFlowOptMaxAllowedPacketSizeType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetUmtsResBer(::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetUmtsResBer(::ds::Net::QoSFlowOptUMTSResidualBitErrorRateType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetUmtsTrfPri(::ds::Net::QoSFlowOptUMTSTrafficPriorityType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetUmtsTrfPri(::ds::Net::QoSFlowOptUMTSTrafficPriorityType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetCdmaProfileID(::ds::Net::QoSFlowOptCDMAProfileIDType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetCdmaProfileID(::ds::Net::QoSFlowOptCDMAProfileIDType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetWlanUserPriority(::ds::Net::QoSFlowOptWLANUserPriorityType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetWlanUserPriority(::ds::Net::QoSFlowOptWLANUserPriorityType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetWlanMinServiceInterval(::ds::Net::QoSFlowOptWLANMinServiceIntervalType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetWlanMinServiceInterval(::ds::Net::QoSFlowOptWLANMinServiceIntervalType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetWlanMaxServiceInterval(::ds::Net::QoSFlowOptWLANMaxServiceIntervalType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetWlanMaxServiceInterval(::ds::Net::QoSFlowOptWLANMaxServiceIntervalType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetWlanInactivityInterval(::ds::Net::QoSFlowOptWLANInactivityIntervalType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetWlanInactivityInterval(::ds::Net::QoSFlowOptWLANInactivityIntervalType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetCdmaFlowPriority(::ds::Net::QoSFlowOptCDMAFlowPriorityType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetCdmaFlowPriority(::ds::Net::QoSFlowOptCDMAFlowPriorityType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetUmtsImCnFlag(::ds::Net::QoSFlowOptUMTSImsSignalingContextType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetUmtsImCnFlag(::ds::Net::QoSFlowOptUMTSImsSignalingContextType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetUmtsSigInd(::ds::Net::QoSFlowOptUMTSHighPriorityDataType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetUmtsSigInd(::ds::Net::QoSFlowOptUMTSHighPriorityDataType value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetDataRateMinMax(::ds::Net::QoSFlowOptDataRateMinMaxType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetDataRateMinMax(const ::ds::Net::QoSFlowOptDataRateMinMaxType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetDataRateMinMaxEx(::ds::Net::QoSFlowOptDataRateMinMaxExType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetDataRateMinMaxEx(const ::ds::Net::QoSFlowOptDataRateMinMaxExType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetDataRateTokenBucket(::ds::Net::QoSFlowOptDataRateTokenBucketType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetDataRateTokenBucket(const ::ds::Net::QoSFlowOptDataRateTokenBucketType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetPktErrRate(::ds::Net::QoSFlowOptPacketErrorRateType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetPktErrRate(const ::ds::Net::QoSFlowOptPacketErrorRateType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetNominalSDUSize(::ds::Net::QoSFlowOptNominalSDUSizeType* value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL SetNominalSDUSize(const ::ds::Net::QoSFlowOptNominalSDUSizeType* value) = 0;
      };
   };
};
#endif //DS_NET_IQOSFLOWPRIV_H
