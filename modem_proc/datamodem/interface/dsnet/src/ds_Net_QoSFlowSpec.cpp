/*===========================================================================
  FILE: QoSFlowSpec.cpp

  OVERVIEW: This file provides implementation of the QoSFlowSpec class.

  DEPENDENCIES: None

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSFlowSpec.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-09-30 kr  Free floating changes
  2008-08-10 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include <stringl/stringl.h>
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_QoSFlowSpec.h"
#include "ds_Net_CreateInstance.h"
#include "ds_Net_Platform.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace NetPlatform;


/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  CONSTRUCTOR/DESTRUCTOR
---------------------------------------------------------------------------*/
QoSFlowSpec::QoSFlowSpec
(
  NetPlatform::PSFlowSpecType* pPSFlowSpec
)
: mpQoSFlowSpecClone(NULL),
  mpPSQoSSpecWrapper(NULL),
  refCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("QoSFlowSpec::QoSFlowSpec(): "
                   "Creating object 0x%p", this);

  mpPSQoSSpecWrapper = new PSQoSSpecWrapper();
  if (NULL != mpPSQoSSpecWrapper)
  {
    (void) mpPSQoSSpecWrapper->UpdatePSQoSFlowSpec(pPSFlowSpec);
  }

} /* QoSFlowSpec() */

QoSFlowSpec::~QoSFlowSpec()
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("QoSFlowSpec::~QoSFlowSpec(): "
                   "Deleting object 0x%p", this);

  DS_UTILS_RELEASEIF (mpQoSFlowSpecClone);
  DS_UTILS_RELEASEIF (mpPSQoSSpecWrapper);
}


int QoSFlowSpec::Clone
(
  IQoSFlowPriv** flowObj
)
{
  int result;
  NetPlatform::PSFlowSpecType   localPSQoSFlowSpec;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSFlowSpec::Clone(): "
                            "Obj 0x%p", this);

  if (NULL == flowObj)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  result = mpPSQoSSpecWrapper->GetPSQoSFlowSpec(&localPSQoSFlowSpec);

  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  mpQoSFlowSpecClone = new QoSFlowSpec (&localPSQoSFlowSpec);
  if (NULL == mpQoSFlowSpecClone)
  {
    result = AEE_ENOMEMORY;
    goto bail;
  }

  *flowObj = static_cast <IQoSFlowPriv *> (mpQoSFlowSpecClone);
  (void) (*flowObj)->AddRef();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("QoSFlowSpec::Clone(): "
                   "err %d", result);
  return result;
}

int QoSFlowSpec::GetOptionsInternal
(
  QoSFlowOptIDType*    pOpts,
  int                             optsLen,
  int*                            pOptsLenReq,
  boolean                         isErrMask
)
{
  int                             result;
  int                             index;
  uint32                          mask;
  NetPlatform::PSFlowSpecType     localPSQoSFlowSpec;

#define LOCAL_ARRAY_SIZE       (30)

  /* Change the array size to current max num of options supported */
  QoSFlowOptIDType     localOptsArr[LOCAL_ARRAY_SIZE] = {0,};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(LOCAL_ARRAY_SIZE <= optsLen)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  result = mpPSQoSSpecWrapper->GetPSQoSFlowSpec(&localPSQoSFlowSpec);

  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  if (TRUE == isErrMask)
  {
    mask = localPSQoSFlowSpec.err_mask;
  }
  else
  {
    mask = localPSQoSFlowSpec.field_mask;
  }

  /* Start with index 0 */
  index = 0;

  if (mask & (uint32) IPFLOW_MASK_TRF_CLASS)
  {
    localOptsArr[index++] = QoSFlowOptID::QDS_IP_TRAFFIC_CLASS;
  }

  if (mask & (uint32) IPFLOW_MASK_DATA_RATE)
  {
    if (DATA_RATE_FORMAT_MIN_MAX_TYPE ==
        localPSQoSFlowSpec.data_rate.format_type)
    {
      localOptsArr[index++] = QoSFlowOptID::QDS_DATA_RATE_MIN_MAX;
    }
    else if (DATA_RATE_FORMAT_MIN_MAX_EX_TYPE ==
        localPSQoSFlowSpec.data_rate.format_type)
    {
      localOptsArr[index++] = QoSFlowOptID::QDS_DATA_RATE_MIN_MAX_EX;
    }
    else if (DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE ==
             localPSQoSFlowSpec.data_rate.format_type)
    {
      localOptsArr[index++] = QoSFlowOptID::QDS_DATA_RATE_TOKEN_BUCKET;
    }
  }

  if (mask & (uint32) IPFLOW_MASK_LATENCY)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_LATENCY;
  }

  if (mask & (uint32) IPFLOW_MASK_LATENCY_VAR)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_LATENCY_VARIANCE;
  }

  if (mask & (uint32) IPFLOW_MASK_PKT_ERR_RATE)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_PACKET_ERROR_RATE;
  }

  if (mask & (uint32) IPFLOW_MASK_MIN_POLICED_PKT_SIZE)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_MIN_POLICED_PACKET_SIZE;
  }

  if (mask & (uint32) IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_MAX_ALLOWED_PACKET_SIZE;
  }

  if (mask & (uint32) IPFLOW_MASK_UMTS_RES_BER)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_UMTS_RESIDUAL_BIT_ERROR_RATE;
  }

  if (mask & (uint32) IPFLOW_MASK_UMTS_TRF_PRI)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_UMTS_TRAFFIC_PRIORITY;
  }

  if (mask & (uint32) IPFLOW_MASK_CDMA_PROFILE_ID)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_CDMA_PROFILE_ID;
  }

  if (mask & (uint32) IPFLOW_MASK_WLAN_USER_PRI)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_WLAN_USER_PRI;
  }

  if (mask & (uint32) IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_WLAN_MIN_SERVICE_INTERVAL;
  }

  if (mask & (uint32) IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_WLAN_MAX_SERVICE_INTERVAL;
  }

  if (mask & (uint32) IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_WLAN_INACTIVITY_INTERVAL;
  }

  if (mask & (uint32) IPFLOW_MASK_NOMINAL_SDU_SIZE)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_NOMINAL_SDU_SIZE;
  }

  if (mask & (uint32) IPFLOW_MASK_CDMA_FLOW_PRIORITY)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_CDMA_FLOW_PRI;
  }

  if (mask & (uint32) IPFLOW_MASK_UMTS_IM_CN_FLAG)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_UMTS_IMS_SIGNALING_CONTEXT;
  }

  if (mask & (uint32) IPFLOW_MASK_UMTS_SIG_IND)
  {
    localOptsArr[index++] =
      QoSFlowOptID::QDS_UMTS_HIGH_PRIORITY_DATA;
  }

  if(NULL != pOptsLenReq)
  {
     *pOptsLenReq = index;
  }
  if (NULL == pOpts)
  {
     if(0 != optsLen)
     {
        result = QDS_EFAULT;
        goto bail;
     }
     return AEE_SUCCESS;
  }
  if(0 == optsLen)
  {
    return AEE_SUCCESS;
  }
  if (optsLen < index)
  {
    (void) memscpy (pOpts,
      index * sizeof (QoSFlowOptIDType*),
      localOptsArr,
      optsLen * sizeof (QoSFlowOptIDType*));
  }
  else
  {
    (void) memscpy (pOpts,
      index * sizeof (QoSFlowOptIDType*),
      localOptsArr,
      index * sizeof (QoSFlowOptIDType*));
  }

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("QoSFlowSpec::GetOptionsInternal(): "
                   "err %d", result);
  return result;
} /* GetOptionsInternal() */

int QoSFlowSpec::GetValidOptions
(
  QoSFlowOptIDType* pValidOptions,
  int validOptionsLen,
  int* pValidOptionsLenReq
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return GetOptionsInternal (pValidOptions,
                             validOptionsLen,
                             pValidOptionsLenReq,
                             FALSE);

} /* GetValidOptions() */

int QoSFlowSpec::GetErroneousOptions
(
  QoSFlowOptIDType* pErroneousOptions,
  int erroneousOptionsLen,
  int* pErroneousOptionsLenReq
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetOptionsInternal (pErroneousOptions,
                             erroneousOptionsLen,
                             pErroneousOptionsLenReq,
                             TRUE);

} /* GetErroneousOptions() */

int QoSFlowSpec::GetParams
(
  void*     pDst,
  int       dstLen,
  void*     pSrc,
  uint32    paramName,
  uint32    fieldMask
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2 ("QoSFlowSpec::GetParams(): "
                   "obj 0x%p, param 0x%x", this, paramName);

  if (NULL == pDst || 0 == dstLen)
  {
    LOG_MSG_ERROR_0 ("QoSFlowSpec::GetParams(): "
                     "Inv. args");
    return QDS_EFAULT;
  }

  if (paramName & fieldMask)
  {
    (void) memscpy (pDst, dstLen, pSrc, dstLen);
  }
  else
  {
    LOG_MSG_INFO1_1 ("QoSFlowSpec::GetParams(): "
                     "param %d not set", paramName);
    return QDS_EINVAL;
  }

  return AEE_SUCCESS;
} /* GetParams() */

int QoSFlowSpec::SetParams
(
  void*           pDst,
  int             dstLen,
  const void*     pSrc,
  uint32          paramName,
  uint32*         pFieldMask
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pDst || 0 == dstLen || NULL == pFieldMask)
  {
    LOG_MSG_ERROR_0 ("QoSFlowSpec::SetParams(): "
                     "Inv. args");
    return QDS_EFAULT;
  }

  LOG_MSG_INFO2_2 ("QoSFlowSpec::SetParams(): "
                   "obj 0x%p, param 0x%x", this, paramName);

  (void) memscpy (pDst, dstLen, pSrc, dstLen);
  *pFieldMask |= paramName;

  return AEE_SUCCESS;

} /* GetParams() */

int QoSFlowSpec::GetTrfClass
(
  QoSFlowOptIPTrafficClassType* pTrfClass
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return mpPSQoSSpecWrapper->GetTrfClass((int *) pTrfClass);

} /* GetTrfClass() */


int QoSFlowSpec::SetTrfClass
(
  QoSFlowOptIPTrafficClassType TrfClass
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetTrfClass(TrfClass);

} /* SetTrfClass() */

int QoSFlowSpec::GetLatency
(
  QoSFlowOptLatencyType* pLatency
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetLatency((int *) pLatency);

} /* GetLatency() */

int QoSFlowSpec::SetLatency
(
  QoSFlowOptLatencyType Latency
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetLatency(Latency);

} /* SetLatency() */


int QoSFlowSpec::GetLatencyVar
(
  QoSFlowOptLatencyVarianceType* pLatencyVar
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetLatencyVar((int *) pLatencyVar);

} /* GetLatencyVar() */

int QoSFlowSpec::SetLatencyVar
(
  QoSFlowOptLatencyVarianceType LatencyVar
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetLatencyVar(LatencyVar);

} /* SetLatencyVar() */

int QoSFlowSpec::GetMinPolicedPktSize
(
  QoSFlowOptMinPolicedPacketSizeType* pMinPolicedPktSize
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetMinPolicedPktSize((int *)pMinPolicedPktSize);

} /* GetMinPolicedPktSize() */


int QoSFlowSpec::SetMinPolicedPktSize
(
  QoSFlowOptMinPolicedPacketSizeType MinPolicedPktSize
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetMinPolicedPktSize(MinPolicedPktSize);

} /* SetMinPolicedPktSize() */

int QoSFlowSpec::GetMaxAllowedPktSize
(
  QoSFlowOptMaxAllowedPacketSizeType* pMaxAllowedPktSize
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetMaxAllowedPktSize((int *) pMaxAllowedPktSize);

} /* GetMaxAllowedPktSize() */

int QoSFlowSpec::SetMaxAllowedPktSize
(
  QoSFlowOptMaxAllowedPacketSizeType MaxAllowedPktSize
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetMaxAllowedPktSize(MaxAllowedPktSize);

} /* SetMaxAllowedPktSize() */

int QoSFlowSpec::GetUmtsResBer
(
  QoSFlowOptUMTSResidualBitErrorRateType* pUmtsResBer
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetUmtsResBer((int *) pUmtsResBer);

} /* GetUmtsResBer() */

int QoSFlowSpec::SetUmtsResBer
(
  QoSFlowOptUMTSResidualBitErrorRateType UmtsResBer
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetUmtsResBer(UmtsResBer);

} /* SetUmtsResBer() */


int QoSFlowSpec::GetUmtsTrfPri
(
  QoSFlowOptUMTSTrafficPriorityType* pUmtsTrfPri
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetUmtsTrfPri((int *)pUmtsTrfPri);

} /* GetUmtsTrfPri() */

int QoSFlowSpec::SetUmtsTrfPri
(
  QoSFlowOptUMTSTrafficPriorityType UmtsTrfPri
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetUmtsTrfPri(UmtsTrfPri);

} /* SetUmtsTrfPri() */

int QoSFlowSpec::GetUmtsImCnFlag
(
  QoSFlowOptUMTSImsSignalingContextType* pUmtsImCnFlag
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetUmtsImCnFlag((boolean *) pUmtsImCnFlag);

} /* GetUmtsImCnFlag() */


int QoSFlowSpec::SetUmtsImCnFlag
(
  QoSFlowOptUMTSImsSignalingContextType UmtsImCnFlag
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetUmtsImCnFlag(UmtsImCnFlag);

} /* SetUmtsImCnFlag() */

int QoSFlowSpec::GetUmtsSigInd
(
  QoSFlowOptUMTSHighPriorityDataType* pUmtsSigInd
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetUmtsSigInd((boolean *)pUmtsSigInd);

} /* GetUmtsSigInd() */


int QoSFlowSpec::SetUmtsSigInd
(
  QoSFlowOptUMTSHighPriorityDataType UmtsSigInd
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetUmtsSigInd(UmtsSigInd);

} /* SetUmtsSigInd() */


int QoSFlowSpec::GetCdmaProfileID
(
  QoSFlowOptCDMAProfileIDType* pCdmaProfileID
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetCdmaProfileID((unsigned short int *)pCdmaProfileID);

} /* GetCdmaProfileID() */

int QoSFlowSpec::SetCdmaProfileID
(
  QoSFlowOptCDMAProfileIDType CdmaProfileID
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetCdmaProfileID(CdmaProfileID);

} /* SetCdmaProfileID() */


int QoSFlowSpec::GetCdmaFlowPriority
(
  QoSFlowOptCDMAFlowPriorityType* pCdmaFlowPriority
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return mpPSQoSSpecWrapper->GetCdmaFlowPriority((unsigned char *)pCdmaFlowPriority);

} /* GetCdmaFlowPriority() */


int QoSFlowSpec::SetCdmaFlowPriority
(
  QoSFlowOptCDMAFlowPriorityType CdmaFlowPriority
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetCdmaFlowPriority(CdmaFlowPriority);

} /* SetCdmaFlowPriority() */


int QoSFlowSpec::GetWlanUserPriority
(
  QoSFlowOptWLANUserPriorityType* pWlanUserPriority
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetWlanUserPriority((int *) pWlanUserPriority);

} /* GetWlanUserPriority() */

int QoSFlowSpec::SetWlanUserPriority
(
  QoSFlowOptWLANUserPriorityType WlanUserPriority
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetWlanUserPriority(WlanUserPriority);

} /* SetWlanUserPriority() */


int QoSFlowSpec::GetWlanMinServiceInterval
(
  QoSFlowOptWLANMinServiceIntervalType* pWlanMinServiceInterval
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetWlanMinServiceInterval((int *)pWlanMinServiceInterval);

} /* GetWlanMinServiceInterval() */

int QoSFlowSpec::SetWlanMinServiceInterval
(
  QoSFlowOptWLANMinServiceIntervalType WlanMinServiceInterval
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetWlanMinServiceInterval(WlanMinServiceInterval);

} /* SetWlanMinServiceInterval() */

int QoSFlowSpec::GetWlanMaxServiceInterval
(
  QoSFlowOptWLANMaxServiceIntervalType* pWlanMaxServiceInterval
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetWlanMaxServiceInterval((int *) pWlanMaxServiceInterval);

} /* GetWlanMaxServiceInterval() */


int QoSFlowSpec::SetWlanMaxServiceInterval
(
  QoSFlowOptWLANMaxServiceIntervalType WlanMaxServiceInterval
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetWlanMaxServiceInterval(WlanMaxServiceInterval);

} /* SetWlanMaxServiceInterval() */


int QoSFlowSpec::GetWlanInactivityInterval
(
  QoSFlowOptWLANInactivityIntervalType* pWlanInactivityInterval
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetWlanInactivityInterval((int *) pWlanInactivityInterval);

} /* GetWlanInactivityInterval() */

int QoSFlowSpec::SetWlanInactivityInterval
(
  QoSFlowOptWLANInactivityIntervalType WlanInactivityInterval
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetWlanInactivityInterval(WlanInactivityInterval);

} /* SetWlanInactivityInterval() */


int QoSFlowSpec::GetDataRateMinMax
(
  QoSFlowOptDataRateMinMaxType* pDataRateMinMax
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetDataRateMinMax(&(pDataRateMinMax->maxRate), &(pDataRateMinMax->guaranteedRate));

} /* GetDataRateMinMax() */

int QoSFlowSpec::GetDataRateMinMaxEx
(
  QoSFlowOptDataRateMinMaxExType* pDataRateMinMaxEx
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetDataRateMinMaxEx(&(pDataRateMinMaxEx->maxRate), &(pDataRateMinMaxEx->guaranteedRate));

} /* GetDataRateMinMaxEx() */

int QoSFlowSpec::SetDataRateMinMax
(
  const QoSFlowOptDataRateMinMaxType* pDataRateMinMax
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetDataRateMinMax(pDataRateMinMax->maxRate, pDataRateMinMax->guaranteedRate);

} /* SetDataRateMinMax() */

int QoSFlowSpec::SetDataRateMinMaxEx
(
  const QoSFlowOptDataRateMinMaxExType* pDataRateMinMaxEx
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetDataRateMinMaxEx(pDataRateMinMaxEx->maxRate, pDataRateMinMaxEx->guaranteedRate);

} /* SetDataRateMinMaxEx() */

int QoSFlowSpec::GetDataRateTokenBucket
(
  QoSFlowOptDataRateTokenBucketType* pDataRateTokenBucket
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetDataRateTokenBucket(&(pDataRateTokenBucket->peakRate), &(pDataRateTokenBucket->tokenRate), &(pDataRateTokenBucket->size));

} /* GetDataRateTokenBucket() */

int QoSFlowSpec::SetDataRateTokenBucket
(
  const QoSFlowOptDataRateTokenBucketType* pDataRateTokenBucket
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetDataRateTokenBucket(pDataRateTokenBucket->peakRate, pDataRateTokenBucket->tokenRate, pDataRateTokenBucket->size);

} /* SetDataRateTokenBucket() */

int QoSFlowSpec::GetPktErrRate
(
  QoSFlowOptPacketErrorRateType* pPktErrRate
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetPktErrRate(&(pPktErrRate->multiplier), &(pPktErrRate->exponent));

} /* GetWlanInactivityInterval() */

int QoSFlowSpec::SetPktErrRate
(
  const QoSFlowOptPacketErrorRateType* pPktErrRate
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetPktErrRate(pPktErrRate->multiplier, pPktErrRate->exponent);

} /* SetPktErrRate() */


int QoSFlowSpec::GetNominalSDUSize
(
  QoSFlowOptNominalSDUSizeType* pNominalSDUSize
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetNominalSDUSize(&(pNominalSDUSize->fixed), &(pNominalSDUSize->size));

} /* GetNominalSDUSize() */

int QoSFlowSpec::SetNominalSDUSize
(
  const QoSFlowOptNominalSDUSizeType* pNominalSDUSize
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetNominalSDUSize(pNominalSDUSize->fixed, pNominalSDUSize->size);

} /* SetNominalSDUSize() */

ds::ErrorType QoSFlowSpec::GetPSQoSFlowSpec
(
  NetPlatform::PSFlowSpecType  *pPSFlowSpec
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pPSFlowSpec)
  {
    ASSERT (0);
    return QDS_EINVAL;
  }

  return mpPSQoSSpecWrapper->GetPSQoSFlowSpec(pPSFlowSpec);

} /* GetPSQoSFlowSpec() */

ds::ErrorType QoSFlowSpec::UpdateSpec
(
  NetPlatform::PSFlowSpecType  *pPSFlowSpec
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pPSFlowSpec)
  {
    return QDS_EINVAL;
  }

  return mpPSQoSSpecWrapper->UpdatePSQoSFlowSpec(pPSFlowSpec);

} /* GetPSQoSFlowSpec() */


