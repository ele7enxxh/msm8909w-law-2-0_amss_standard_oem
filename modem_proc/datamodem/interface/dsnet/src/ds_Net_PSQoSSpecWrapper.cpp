/*===========================================================================
@file ds_Net_PSQoSSpecWrapper.h

OVERVIEW: This file provides implementation of the PSQoSSpecWrapper class.

Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_PSQoSSpecWrapper.cpp#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------ 
2011-09-30 kr  Free floating changes 
2010-08-01 en  Created module.

===========================================================================*/
/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include <stringl/stringl.h>
#include "ds_Net_PSQoSSpecWrapper.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_Def.h"
#include "ds_Net_CreateInstance.h"
#include "ds_Net_Platform.h"
#include "ds_Net_IIPFilterPriv.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CCritSect.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace NetPlatform;

/*===========================================================================

PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
CONSTRUCTOR/DESTRUCTOR
---------------------------------------------------------------------------*/
PSQoSSpecWrapper::PSQoSSpecWrapper()
 : refCnt (1)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("PSQoSSpecWrapper::PSQoSSpecWrapper(): "
                   "Creating object 0x%p", this);

  /* QoS Flow initialization */
  memset (&mPSQoSFlowSpec, 0, sizeof(mPSQoSFlowSpec));


  /* IP Filter initialization */
  
  memset(&mPSIPFilterSpec, 0, sizeof(mPSIPFilterSpec));

  /* Default value for IP version */
  mPSIPFilterSpec.ip_vsn = IP_V4;

  /* Default value for IP filter ID and precedence */
  mPSIPFilterSpec.ipfltr_aux_info.fi_id = PS_IFACE_IPFLTR_DEFAULT_ID;
  mPSIPFilterSpec.ipfltr_aux_info.fi_precedence = PS_IFACE_IPFLTR_DEFAULT_PRCD;

  if (AEE_SUCCESS != DS_Utils_CreateInstance (NULL, 
                                              AEECLSID_CCritSect,
                                              (void **) &mpICritSect))
  {
    DATA_ERR_FATAL("PSQoSSpecWrapper::PSQoSSpecWrapper(): Cannot create crit sect");
  }

} /* PSQoSSpecWrapper() */

PSQoSSpecWrapper::~PSQoSSpecWrapper()
throw()
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("PSQoSSpecWrapper::~PSQoSSpecWrapper(): "
                   "Deleting object 0x%p", this);

  /*lint -save -e1550, -e1551 */
  DS_UTILS_RELEASEIF(mpICritSect);
  /*lint -restore *///TODO see what we need to do here 
}

ds::ErrorType PSQoSSpecWrapper::Clone
(
  const PSQoSSpecWrapper* from
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(0 == from)
  {
    LOG_MSG_ERROR_0 ("PSQoSSpecWrapper::Clone(): "
                     "Inv. args");
    return QDS_EFAULT;
  }
  
  mPSIPFilterSpec = from->mPSIPFilterSpec;

  mPSQoSFlowSpec = from->mPSQoSFlowSpec;

  return AEE_SUCCESS;
}

int PSQoSSpecWrapper::GetParams
(
  void*     pDst,
  int       dstLen,
  void*     pSrc,
  uint32    paramName,
  uint32    fieldMask
 )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2 ("PSQoSSpecWrapper::GetParams(): "
                   "obj 0x%p, param 0x%x", this, paramName);

  if (NULL == pDst || 0 == dstLen)
  {
    LOG_MSG_ERROR_0 ("PSQoSSpecWrapper::GetParams(): "
                     "Inv. args");
    return QDS_EFAULT;
  }

  if (paramName & fieldMask)
  {
    (void) memscpy (pDst, dstLen, pSrc, dstLen);
  }
  else
  {
    LOG_MSG_INFO1_1 ("PSQoSSpecWrapper::GetParams(): "
                     "param %d not set", paramName);
    return QDS_EINVAL;
  }

  return AEE_SUCCESS;
} /* GetParams() */

int PSQoSSpecWrapper::SetParams
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
    LOG_MSG_ERROR_0 ("PSQoSSpecWrapper::SetParams(): "
                     "Inv. args");
    return QDS_EFAULT;
  }

  LOG_MSG_INFO2_2 ("PSQoSSpecWrapper::SetParams(): "
                   "obj 0x%p, param 0x%x", this, paramName);
  
  mpICritSect->Enter();
  (void) memscpy (pDst, dstLen, pSrc, dstLen);
  *pFieldMask |= paramName;
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* GetParams() */

int PSQoSSpecWrapper::GetTrfClass 
(
 int* pTrfClass
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return GetParams (pTrfClass, 
                    sizeof (mPSQoSFlowSpec.trf_class),
                    &mPSQoSFlowSpec.trf_class,
                    (uint32) IPFLOW_MASK_TRF_CLASS,
                    mPSQoSFlowSpec.field_mask);

} /* GetTrfClass() */


int PSQoSSpecWrapper::SetTrfClass 
(
 int TrfClass
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.trf_class,
                    sizeof (mPSQoSFlowSpec.trf_class),
                    &TrfClass,
                    (uint32) IPFLOW_MASK_TRF_CLASS,
                    &mPSQoSFlowSpec.field_mask);

} /* SetTrfClass() */

int PSQoSSpecWrapper::GetLatency 
(
 int* pLatency
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pLatency, 
                    sizeof (mPSQoSFlowSpec.latency),
                    &mPSQoSFlowSpec.latency,
                    (uint32) IPFLOW_MASK_LATENCY,
                    mPSQoSFlowSpec.field_mask);

} /* GetLatency() */

int PSQoSSpecWrapper::SetLatency 
(
 int Latency
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.latency,
                    sizeof (mPSQoSFlowSpec.latency),
                    &Latency,
                    (uint32) IPFLOW_MASK_LATENCY,
                    &mPSQoSFlowSpec.field_mask);

} /* SetLatency() */


int PSQoSSpecWrapper::GetLatencyVar 
(
 int* pLatencyVar
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pLatencyVar, 
                    sizeof (mPSQoSFlowSpec.latency_var),
                    &mPSQoSFlowSpec.latency_var,
                    (uint32) IPFLOW_MASK_LATENCY_VAR,
                    mPSQoSFlowSpec.field_mask);

} /* GetLatencyVar() */

int PSQoSSpecWrapper::SetLatencyVar 
(
 int LatencyVar
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.latency_var,
                    sizeof (mPSQoSFlowSpec.latency_var),
                    &LatencyVar,
                    (uint32) IPFLOW_MASK_LATENCY_VAR,
                    &mPSQoSFlowSpec.field_mask);

} /* SetLatencyVar() */

int PSQoSSpecWrapper::GetMinPolicedPktSize 
(
 int* pMinPolicedPktSize
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pMinPolicedPktSize, 
                    sizeof (mPSQoSFlowSpec.min_policed_pkt_size),
                    &mPSQoSFlowSpec.min_policed_pkt_size,
                    (uint32) IPFLOW_MASK_MIN_POLICED_PKT_SIZE,
                    mPSQoSFlowSpec.field_mask);

} /* GetMinPolicedPktSize() */


int PSQoSSpecWrapper::SetMinPolicedPktSize 
(
 int MinPolicedPktSize
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.min_policed_pkt_size,
                    sizeof (mPSQoSFlowSpec.min_policed_pkt_size),
                    &MinPolicedPktSize,
                    (uint32) IPFLOW_MASK_MIN_POLICED_PKT_SIZE,
                    &mPSQoSFlowSpec.field_mask);

} /* SetMinPolicedPktSize() */

int PSQoSSpecWrapper::GetMaxAllowedPktSize 
(
 int* pMaxAllowedPktSize
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pMaxAllowedPktSize, 
                    sizeof (mPSQoSFlowSpec.max_allowed_pkt_size),
                    &mPSQoSFlowSpec.max_allowed_pkt_size,
                    (uint32) IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE,
                    mPSQoSFlowSpec.field_mask);

} /* GetMaxAllowedPktSize() */

int PSQoSSpecWrapper::SetMaxAllowedPktSize 
(
 int MaxAllowedPktSize
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.max_allowed_pkt_size,
                    sizeof (mPSQoSFlowSpec.max_allowed_pkt_size),
                    &MaxAllowedPktSize,
                    (uint32) IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE,
                    &mPSQoSFlowSpec.field_mask);

} /* SetMaxAllowedPktSize() */

int PSQoSSpecWrapper::GetUmtsResBer 
(
 int* pUmtsResBer
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pUmtsResBer, 
                    sizeof (mPSQoSFlowSpec.umts_params.res_ber),
                    &(mPSQoSFlowSpec.umts_params.res_ber),
                    (uint32) IPFLOW_MASK_UMTS_RES_BER,
                    mPSQoSFlowSpec.field_mask);

} /* GetUmtsResBer() */

int PSQoSSpecWrapper::SetUmtsResBer 
(
 int UmtsResBer
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.umts_params.res_ber,
                    sizeof (mPSQoSFlowSpec.umts_params.res_ber),
                    &UmtsResBer,
                    (uint32) IPFLOW_MASK_UMTS_RES_BER,
                    &mPSQoSFlowSpec.field_mask);

} /* SetUmtsResBer() */


int PSQoSSpecWrapper::GetUmtsTrfPri 
(
 int* pUmtsTrfPri
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pUmtsTrfPri, 
                    sizeof (mPSQoSFlowSpec.umts_params.trf_pri),
                    &(mPSQoSFlowSpec.umts_params.trf_pri),
                    (uint32) IPFLOW_MASK_UMTS_TRF_PRI,
                    mPSQoSFlowSpec.field_mask);

} /* GetUmtsTrfPri() */

int PSQoSSpecWrapper::SetUmtsTrfPri 
(
 int UmtsTrfPri
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.umts_params.trf_pri,
                    sizeof (mPSQoSFlowSpec.umts_params.trf_pri),
                    &UmtsTrfPri,
                    (uint32) IPFLOW_MASK_UMTS_TRF_PRI,
                    &mPSQoSFlowSpec.field_mask);

} /* SetUmtsTrfPri() */

int PSQoSSpecWrapper::GetUmtsImCnFlag 
(
 boolean* pUmtsImCnFlag
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pUmtsImCnFlag, 
                    sizeof (mPSQoSFlowSpec.umts_params.im_cn_flag),
                    &(mPSQoSFlowSpec.umts_params.im_cn_flag),
                    (uint32) IPFLOW_MASK_UMTS_IM_CN_FLAG,
                    mPSQoSFlowSpec.field_mask);

} /* GetUmtsImCnFlag() */


int PSQoSSpecWrapper::SetUmtsImCnFlag 
(
 boolean UmtsImCnFlag
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.umts_params.im_cn_flag,
                    sizeof (mPSQoSFlowSpec.umts_params.im_cn_flag),
                    &UmtsImCnFlag,
                    (uint32) IPFLOW_MASK_UMTS_IM_CN_FLAG,
                    &mPSQoSFlowSpec.field_mask);

} /* SetUmtsImCnFlag() */

int PSQoSSpecWrapper::GetUmtsSigInd 
(
 boolean* pUmtsSigInd
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pUmtsSigInd, 
                    sizeof (mPSQoSFlowSpec.umts_params.sig_ind),
                    &(mPSQoSFlowSpec.umts_params.sig_ind),
                    (uint32) IPFLOW_MASK_UMTS_SIG_IND,
                    mPSQoSFlowSpec.field_mask);

} /* GetUmtsSigInd() */


int PSQoSSpecWrapper::SetUmtsSigInd 
(
 boolean UmtsSigInd
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.umts_params.sig_ind,
                    sizeof (mPSQoSFlowSpec.umts_params.sig_ind),
                    &UmtsSigInd,
                    (uint32) IPFLOW_MASK_UMTS_SIG_IND,
                    &mPSQoSFlowSpec.field_mask);

} /* SetUmtsSigInd() */


int PSQoSSpecWrapper::GetCdmaProfileID 
(
 unsigned short int* pCdmaProfileID
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pCdmaProfileID, 
                    sizeof (mPSQoSFlowSpec.cdma_params.profile_id),
                    &(mPSQoSFlowSpec.cdma_params.profile_id),
                    (uint32) IPFLOW_MASK_CDMA_PROFILE_ID,
                    mPSQoSFlowSpec.field_mask);

} /* GetCdmaProfileID() */

int PSQoSSpecWrapper::SetCdmaProfileID 
(
 unsigned short int CdmaProfileID
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.cdma_params.profile_id,
                    sizeof (mPSQoSFlowSpec.cdma_params.profile_id),
                    &CdmaProfileID,
                    (uint32) IPFLOW_MASK_CDMA_PROFILE_ID,
                    &mPSQoSFlowSpec.field_mask);

} /* SetCdmaProfileID() */


int PSQoSSpecWrapper::GetCdmaFlowPriority 
(
 unsigned char* pCdmaFlowPriority
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pCdmaFlowPriority, 
                    sizeof (mPSQoSFlowSpec.cdma_params.flow_priority),
                    &(mPSQoSFlowSpec.cdma_params.flow_priority),
                    (uint32) IPFLOW_MASK_CDMA_FLOW_PRIORITY,
                    mPSQoSFlowSpec.field_mask);

} /* GetCdmaFlowPriority() */


int PSQoSSpecWrapper::SetCdmaFlowPriority 
(
 unsigned char CdmaFlowPriority
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.cdma_params.flow_priority,
                    sizeof (mPSQoSFlowSpec.cdma_params.flow_priority),
                    &CdmaFlowPriority,
                    (uint32) IPFLOW_MASK_CDMA_FLOW_PRIORITY,
                    &mPSQoSFlowSpec.field_mask);

} /* SetCdmaFlowPriority() */


int PSQoSSpecWrapper::GetWlanUserPriority 
(
 int* pWlanUserPriority
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pWlanUserPriority, 
                    sizeof (mPSQoSFlowSpec.wlan_params.user_priority),
                    &(mPSQoSFlowSpec.wlan_params.user_priority),
                    (uint32) IPFLOW_MASK_WLAN_USER_PRI,
                    mPSQoSFlowSpec.field_mask);

} /* GetWlanUserPriority() */

int PSQoSSpecWrapper::SetWlanUserPriority 
(
 int WlanUserPriority
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.wlan_params.user_priority,
                    sizeof (mPSQoSFlowSpec.wlan_params.user_priority),
                    &WlanUserPriority,
                    (uint32) IPFLOW_MASK_WLAN_USER_PRI,
                    &mPSQoSFlowSpec.field_mask);

} /* SetWlanUserPriority() */


int PSQoSSpecWrapper::GetWlanMinServiceInterval 
(
 int* pWlanMinServiceInterval
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pWlanMinServiceInterval, 
                    sizeof (mPSQoSFlowSpec.wlan_params.min_service_interval),
                    &(mPSQoSFlowSpec.wlan_params.min_service_interval),
                    (uint32) IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL,
                    mPSQoSFlowSpec.field_mask);

} /* GetWlanMinServiceInterval() */

int PSQoSSpecWrapper::SetWlanMinServiceInterval 
(
 int WlanMinServiceInterval
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.wlan_params.min_service_interval,
                    sizeof (mPSQoSFlowSpec.wlan_params.min_service_interval),
                    &WlanMinServiceInterval,
                    (uint32) IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL,
                    &mPSQoSFlowSpec.field_mask);

} /* SetWlanMinServiceInterval() */

int PSQoSSpecWrapper::GetWlanMaxServiceInterval 
(
 int* pWlanMaxServiceInterval
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pWlanMaxServiceInterval, 
                    sizeof (mPSQoSFlowSpec.wlan_params.max_service_interval),
                    &(mPSQoSFlowSpec.wlan_params.max_service_interval),
                    (uint32) IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL,
                    mPSQoSFlowSpec.field_mask);

} /* GetWlanMaxServiceInterval() */


int PSQoSSpecWrapper::SetWlanMaxServiceInterval 
(
 int WlanMaxServiceInterval
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.wlan_params.max_service_interval,
                    sizeof (mPSQoSFlowSpec.wlan_params.max_service_interval),
                    &WlanMaxServiceInterval,
                    (uint32) IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL,
                    &mPSQoSFlowSpec.field_mask);

} /* SetWlanMaxServiceInterval() */


int PSQoSSpecWrapper::GetWlanInactivityInterval 
(
 int* pWlanInactivityInterval
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetParams (pWlanInactivityInterval, 
                    sizeof (mPSQoSFlowSpec.wlan_params.inactivity_interval),
                    &(mPSQoSFlowSpec.wlan_params.inactivity_interval),
                    (uint32) IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL,
                    mPSQoSFlowSpec.field_mask);

} /* GetWlanInactivityInterval() */

int PSQoSSpecWrapper::SetWlanInactivityInterval 
(
 int WlanInactivityInterval
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SetParams (&mPSQoSFlowSpec.wlan_params.inactivity_interval,
                    sizeof (mPSQoSFlowSpec.wlan_params.inactivity_interval),
                    &WlanInactivityInterval,
                    (uint32) IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL,
                    &mPSQoSFlowSpec.field_mask);

} /* SetWlanInactivityInterval() */


int PSQoSSpecWrapper::GetDataRateMinMax 
(
  int* pMaxRate,
  int* pGuaranteedRate
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pMaxRate || NULL == pGuaranteedRate)
  {
    LOG_MSG_ERROR_0 ("PSQoSSpecWrapper::GetDataRateMinMax(): "
                     "Inv. args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  if ((mPSQoSFlowSpec.field_mask & (uint32) IPFLOW_MASK_DATA_RATE) &&
    (DATA_RATE_FORMAT_MIN_MAX_TYPE == mPSQoSFlowSpec.data_rate.format_type))
  {
    *pMaxRate = mPSQoSFlowSpec.data_rate.format.min_max.max_rate;
    *pGuaranteedRate = mPSQoSFlowSpec.data_rate.format.min_max.guaranteed_rate;
  }
  else
  {
    mpICritSect->Leave();
    LOG_MSG_INFO1_1 ("PSQoSSpecWrapper::GetDataRateMinMax(): "
                     "param %d not set", IPFLOW_MASK_DATA_RATE);
    return QDS_EINVAL;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* GetDataRateMinMax() */

int PSQoSSpecWrapper::GetDataRateMinMaxEx 
(
  int64* pMaxRate,
  int64* pGuaranteedRate
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pMaxRate || NULL == pGuaranteedRate)
  {
    LOG_MSG_ERROR_0 ("PSQoSSpecWrapper::GetDataRateMinMaxEx(): "
                     "Inv. args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  if ((mPSQoSFlowSpec.field_mask & (uint32) IPFLOW_MASK_DATA_RATE) &&
    (DATA_RATE_FORMAT_MIN_MAX_EX_TYPE == mPSQoSFlowSpec.data_rate.format_type))
  {
    *pMaxRate = mPSQoSFlowSpec.data_rate.format.min_max_ex.max_rate;
    *pGuaranteedRate = mPSQoSFlowSpec.data_rate.format.min_max_ex.guaranteed_rate;
  }
  else
  {
    mpICritSect->Leave();
    LOG_MSG_INFO1_1 ("PSQoSSpecWrapper::GetDataRateMinMaxEx(): "
                     "param %d not set", IPFLOW_MASK_DATA_RATE);
    return QDS_EINVAL;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* GetDataRateMinMaxEx() */

int PSQoSSpecWrapper::SetDataRateMinMax 
(
  int maxRate,
  int guaranteedRate
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();
  mPSQoSFlowSpec.data_rate.format.min_max.max_rate = maxRate;
  mPSQoSFlowSpec.data_rate.format.min_max.guaranteed_rate = guaranteedRate;

  mPSQoSFlowSpec.field_mask |= IPFLOW_MASK_DATA_RATE;

  mPSQoSFlowSpec.data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* SetDataRateMinMax() */

int PSQoSSpecWrapper::SetDataRateMinMaxEx 
(
  int64 maxRate,
  int64 guaranteedRate
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();
  mPSQoSFlowSpec.data_rate.format.min_max_ex.max_rate = maxRate;
  mPSQoSFlowSpec.data_rate.format.min_max_ex.guaranteed_rate = guaranteedRate;

  mPSQoSFlowSpec.field_mask |= IPFLOW_MASK_DATA_RATE;

  mPSQoSFlowSpec.data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_EX_TYPE;
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* SetDataRateMinMaxEx() */

int PSQoSSpecWrapper::GetDataRateTokenBucket 
(
  int* pPeakRate,
  int* pTokenRate,
  int* pSize
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pPeakRate || NULL == pTokenRate || NULL == pSize)
  {
    LOG_MSG_ERROR_0 ("PSQoSSpecWrapper::GetDataRateTokenBucket(): "
                     "Inv. args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  if ((mPSQoSFlowSpec.field_mask & (uint32) IPFLOW_MASK_DATA_RATE) &&
      (DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE == mPSQoSFlowSpec.data_rate.format_type))
  {
    *pPeakRate = mPSQoSFlowSpec.data_rate.format.token_bucket.peak_rate;
    *pTokenRate = mPSQoSFlowSpec.data_rate.format.token_bucket.token_rate;
    *pSize = mPSQoSFlowSpec.data_rate.format.token_bucket.size;
  }
  else
  {
    mpICritSect->Leave();
    LOG_MSG_INFO1_1 ("PSQoSSpecWrapper::GetDataRateTokenBucket(): "
                     "param %d not set", IPFLOW_MASK_DATA_RATE);
    return QDS_EINVAL;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* GetDataRateTokenBucket() */

int PSQoSSpecWrapper::SetDataRateTokenBucket 
(
  int peakRate,
  int tokenRate,
  int size
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();
  mPSQoSFlowSpec.data_rate.format.token_bucket.peak_rate = peakRate;
  mPSQoSFlowSpec.data_rate.format.token_bucket.token_rate = tokenRate;
  mPSQoSFlowSpec.data_rate.format.token_bucket.size = size;

  mPSQoSFlowSpec.field_mask |= IPFLOW_MASK_DATA_RATE;

  mPSQoSFlowSpec.data_rate.format_type = DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE;
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* SetDataRateTokenBucket() */

int PSQoSSpecWrapper::GetPktErrRate 
(
  unsigned short int* pMultiplier,
  unsigned short int* pExponent
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pMultiplier || NULL == pExponent)
  {
    LOG_MSG_ERROR_0 ("PSQoSSpecWrapper::GetPktErrRate(): "
                     "Inv. args");
    return QDS_EFAULT;
  }
  
  mpICritSect->Enter();
  if (mPSQoSFlowSpec.field_mask & (uint32) IPFLOW_MASK_PKT_ERR_RATE)
  {
    *pMultiplier = mPSQoSFlowSpec.pkt_err_rate.multiplier;
    *pExponent = mPSQoSFlowSpec.pkt_err_rate.exponent;
  }
  else
  {
    mpICritSect->Leave();
    LOG_MSG_INFO1_1 ("PSQoSSpecWrapper::GetPktErrRate(): "
                     "param %d not set", IPFLOW_MASK_PKT_ERR_RATE);
    return QDS_EINVAL;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* GetWlanInactivityInterval() */

int PSQoSSpecWrapper::SetPktErrRate 
(
  unsigned short int multiplier,
  unsigned short int exponent
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();
  mPSQoSFlowSpec.pkt_err_rate.multiplier = multiplier;
  mPSQoSFlowSpec.pkt_err_rate.exponent = exponent;

  mPSQoSFlowSpec.field_mask |= IPFLOW_MASK_DATA_RATE;
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* SetPktErrRate() */


int PSQoSSpecWrapper::GetNominalSDUSize 
(
  boolean* pFixed,
  int*     pSize
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pFixed || NULL == pSize)
  {
    LOG_MSG_ERROR_0 ("PSQoSSpecWrapper::GetNominalSDUSize(): "
                     "Inv. args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  if (mPSQoSFlowSpec.field_mask & (uint32) IPFLOW_MASK_NOMINAL_SDU_SIZE)
  {
    *pFixed = mPSQoSFlowSpec.nominal_sdu_size.is_fixed;
    *pSize = mPSQoSFlowSpec.nominal_sdu_size.size;
  }
  else
  {
    mpICritSect->Leave();
    LOG_MSG_INFO1_1 ("PSQoSSpecWrapper::GetNominalSDUSize(): "
                     "param %d not set", IPFLOW_MASK_NOMINAL_SDU_SIZE);
    return QDS_EINVAL;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* GetNominalSDUSize() */

int PSQoSSpecWrapper::SetNominalSDUSize 
(
  boolean fixed,
  int     size
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();
  mPSQoSFlowSpec.nominal_sdu_size.is_fixed = fixed;
  mPSQoSFlowSpec.nominal_sdu_size.size = size;

  mPSQoSFlowSpec.field_mask |= IPFLOW_MASK_NOMINAL_SDU_SIZE;
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* SetNominalSDUSize() */

int PSQoSSpecWrapper::GetIPVsn
(
  unsigned char *pIPVersion
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pIPVersion)
  {
    result = QDS_EFAULT;
    goto bail;
  }
  mpICritSect->Enter();
  *pIPVersion = (unsigned char) mPSIPFilterSpec.ip_vsn;
  mpICritSect->Leave();

  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::GetIPVsn(): "
                   "IP version %d", *pIPVersion);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetIPVsn(): "
                   "GetIPVsn failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::SetIPVsn
(
  unsigned char ipVersion
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::SetIPVsn(): "
                   "IP vsn %d", ipVersion);

  mpICritSect->Enter();
  mPSIPFilterSpec.ip_vsn = (ip_version_enum_type) ipVersion;
  if (IP_V4 != mPSIPFilterSpec.ip_vsn && IP_V6 != mPSIPFilterSpec.ip_vsn)
  {
    mpICritSect->Leave();
    return QDS_EINVAL;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;
}

int PSQoSSpecWrapper::GetNextHdrProt
(
 unsigned char* pNextHdrProt
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pNextHdrProt)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpICritSect->Enter();
  if (IP_V4 == mPSIPFilterSpec.ip_vsn)
  {
    if ((uint32) IPFLTR_MASK_IP4_NEXT_HDR_PROT & 
      mPSIPFilterSpec.ip_hdr.v4.field_mask)
    {
      *pNextHdrProt = mPSIPFilterSpec.ip_hdr.v4.next_hdr_prot;
    }
    else
    {
      result = QDS_EINVAL;
      mpICritSect->Leave();
      goto bail;
    }
  }
  else
  {
    if ((uint32) IPFLTR_MASK_IP6_NEXT_HDR_PROT & 
      mPSIPFilterSpec.ip_hdr.v6.field_mask)
    {
      *pNextHdrProt = mPSIPFilterSpec.ip_hdr.v6.next_hdr_prot;
    }
    else
    {
      result = QDS_EINVAL;
      mpICritSect->Leave();
      goto bail;
    }
  }
  mpICritSect->Leave();

  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::GetNextHdrProt(): "
                   "Next Hdr proto %d", *pNextHdrProt);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetNextHdrProt(): "
                   "GetNextHdrProt failed, err %d", result);
  return result;
}


int PSQoSSpecWrapper::SetNextHdrProt
(
 unsigned char nextHdrProt
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::SetNextHdrProt(): "
                   "Next hdr %d", nextHdrProt);

  mpICritSect->Enter();
  if (IP_V4 == mPSIPFilterSpec.ip_vsn)
  {
    mPSIPFilterSpec.ip_hdr.v4.next_hdr_prot = nextHdrProt;
    mPSIPFilterSpec.ip_hdr.v4.field_mask |= 
      (uint32) IPFLTR_MASK_IP4_NEXT_HDR_PROT;
  }
  else
  {
    mPSIPFilterSpec.ip_hdr.v6.next_hdr_prot = nextHdrProt;
    mPSIPFilterSpec.ip_hdr.v6.field_mask |= 
      (uint32) IPFLTR_MASK_IP6_NEXT_HDR_PROT;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;
}


int PSQoSSpecWrapper::GetSrcPort
(
  unsigned short int* pPort,
  unsigned short int* pRange
)
{
  int result;
  unsigned char  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pPort || NULL == pRange)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

  if ((uint8) PS_IPPROTO_TCP == nextHdrProt)
  {
    if ((uint32) IPFLTR_MASK_TCP_SRC_PORT & 
      mPSIPFilterSpec.next_prot_hdr.tcp.field_mask)
    {
      *pPort = mPSIPFilterSpec.next_prot_hdr.tcp.src.port;
      *pRange = mPSIPFilterSpec.next_prot_hdr.tcp.src.range;
    }
    else
    {
      result = QDS_EINVAL;
      mpICritSect->Leave();
      goto bail;
    }
  }
  else if ((uint8) PS_IPPROTO_UDP == nextHdrProt)
  {
    if ((uint32) IPFLTR_MASK_UDP_SRC_PORT & 
      mPSIPFilterSpec.next_prot_hdr.udp.field_mask)
    {
      *pPort = mPSIPFilterSpec.next_prot_hdr.udp.src.port;
      *pRange = mPSIPFilterSpec.next_prot_hdr.udp.src.range;
    }
    else
    {
      result = QDS_EINVAL;
      mpICritSect->Leave();
      goto bail;
    }
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  LOG_MSG_INFO2_2("PSQoSSpecWrapper::GetSrcPort(): "
                  "Src port %d, range %d", *pPort, *pRange);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetSrcPort(): "
                   "GetSrcPort failed, err %d", result);
  return result;
}


int PSQoSSpecWrapper::SetSrcPort
(
  unsigned short int port,
  unsigned short int range
)
{
  int result;
  unsigned char  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("PSQoSSpecWrapper::SetSrcPort(): "
                  "Src port %d, range %d", port, range);
  
  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  if ((uint8) PS_IPPROTO_TCP == nextHdrProt)
  {
    mPSIPFilterSpec.next_prot_hdr.tcp.src.port = port;
    mPSIPFilterSpec.next_prot_hdr.tcp.src.range = range;
    
    mPSIPFilterSpec.next_prot_hdr.tcp.field_mask |= 
      (uint32) IPFLTR_MASK_TCP_SRC_PORT;
  }
  else if ((uint8) PS_IPPROTO_UDP == nextHdrProt)
  {
    mPSIPFilterSpec.next_prot_hdr.udp.src.port = port;
    mPSIPFilterSpec.next_prot_hdr.udp.src.range = range;

    mPSIPFilterSpec.next_prot_hdr.udp.field_mask |= 
      (uint32) IPFLTR_MASK_UDP_SRC_PORT;
  }
  else
  {
    result = QDS_EINVAL;
    goto bail;
  }
  
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  mpICritSect->Leave();
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetSrcPort(): "
                   "SetSrcPort failed, err %d", result);
  return result;
}


int PSQoSSpecWrapper::GetDstPort
(
  unsigned short int* pPort,
  unsigned short int* pRange
)
{
  int result;
  IPFilterIPNextProtocolType  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pPort || NULL == pRange)
  {
    result = QDS_EFAULT;
    goto bail;
  }
  
  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

  if ((uint8) PS_IPPROTO_TCP == nextHdrProt)
  {
    if ((uint32) IPFLTR_MASK_TCP_DST_PORT & 
      mPSIPFilterSpec.next_prot_hdr.tcp.field_mask)
    {
      *pPort = mPSIPFilterSpec.next_prot_hdr.tcp.dst.port;
      *pRange = mPSIPFilterSpec.next_prot_hdr.tcp.dst.range;
    }
    else
    {
      mpICritSect->Leave();
      result = QDS_EINVAL;
      goto bail;
    }
  }
  else if ((uint8) PS_IPPROTO_UDP == nextHdrProt)
  {
    if ((uint32) IPFLTR_MASK_UDP_DST_PORT & 
      mPSIPFilterSpec.next_prot_hdr.udp.field_mask)
    {
      *pPort = mPSIPFilterSpec.next_prot_hdr.udp.dst.port;
      *pRange = mPSIPFilterSpec.next_prot_hdr.udp.dst.range;
    }
    else
    {
      result = QDS_EINVAL;
      mpICritSect->Leave();
      goto bail;
    }
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  LOG_MSG_INFO2_2("PSQoSSpecWrapper::GetDstPort(): "
                  "Dst port %d, range %d", *pPort, *pRange);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetDstPort(): "
                   "GetDstPort failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::SetDstPort
(
  unsigned short int port,
  unsigned short int range
)
{
  int result;
  IPFilterIPNextProtocolType  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("PSQoSSpecWrapper::SetDstPort(): "
                  "Dst port %d, range %d", port, range);
  
  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  if ((uint8) PS_IPPROTO_TCP == nextHdrProt)
  {
    mPSIPFilterSpec.next_prot_hdr.tcp.dst.port = port;
    mPSIPFilterSpec.next_prot_hdr.tcp.dst.range = range;

    mPSIPFilterSpec.next_prot_hdr.tcp.field_mask |= 
      (uint32) IPFLTR_MASK_TCP_DST_PORT;
  }
  else if ((uint8) PS_IPPROTO_UDP == nextHdrProt)
  {
    mPSIPFilterSpec.next_prot_hdr.udp.dst.port = port;
    mPSIPFilterSpec.next_prot_hdr.udp.dst.range = range;

    mPSIPFilterSpec.next_prot_hdr.udp.field_mask |= 
      (uint32) IPFLTR_MASK_UDP_DST_PORT;
  }
  else
  {
    result = QDS_EINVAL;
    goto bail;
  }

  mpICritSect->Leave();
  return AEE_SUCCESS;

bail:
  mpICritSect->Leave();
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetDstPort(): "
                   "SetDstPort failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::GetSrcV4
(
  unsigned int*   pAddr,
  unsigned int*   pSubnetMask
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pAddr || NULL == pSubnetMask)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpICritSect->Enter();

  if (IP_V4 == mPSIPFilterSpec.ip_vsn &&
    (uint32) IPFLTR_MASK_IP4_SRC_ADDR & mPSIPFilterSpec.ip_hdr.v4.field_mask)
  {
    *pAddr = mPSIPFilterSpec.ip_hdr.v4.src.addr.ps_s_addr;
    *pSubnetMask = mPSIPFilterSpec.ip_hdr.v4.src.subnet_mask.ps_s_addr; 
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  LOG_MSG_INFO2_2("PSQoSSpecWrapper::GetSrcV4(): "
                  "IPv4 addr 0x%x, subnet mask 0x%x", *pAddr, *pSubnetMask);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetSrcV4(): "
                   "GetSrcV4 failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::SetSrcV4
(
  unsigned int   addr,
  unsigned int   subnetMask
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("PSQoSSpecWrapper::SetSrcV4(): "
                  "IPv4 addr 0x%x, subnet mask 0x%x",
                  (uint32)addr, (uint32)subnetMask);
  
  mpICritSect->Enter();
  if (IP_V4 == mPSIPFilterSpec.ip_vsn)
  {
    mPSIPFilterSpec.ip_hdr.v4.src.addr.ps_s_addr =  addr;
    mPSIPFilterSpec.ip_hdr.v4.src.subnet_mask.ps_s_addr = subnetMask;

    mPSIPFilterSpec.ip_hdr.v4.field_mask |= (uint32) IPFLTR_MASK_IP4_SRC_ADDR;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetSrcV4(): "
                   "SetSrcV4 failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::GetDstV4
(
  unsigned int*   pAddr,
  unsigned int*   pSubnetMask
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pAddr || NULL == pSubnetMask)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpICritSect->Enter();
  if (IP_V4 == mPSIPFilterSpec.ip_vsn &&
    (uint32) IPFLTR_MASK_IP4_DST_ADDR & mPSIPFilterSpec.ip_hdr.v4.field_mask)
  {
    *pAddr = mPSIPFilterSpec.ip_hdr.v4.dst.addr.ps_s_addr;
    *pSubnetMask = mPSIPFilterSpec.ip_hdr.v4.dst.subnet_mask.ps_s_addr; 
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  
  mpICritSect->Leave();
  LOG_MSG_INFO2_2("PSQoSSpecWrapper::GetDstV4(): "
                  "IPv4 addr 0x%x, subnet mask 0x%x", *pAddr, *pSubnetMask);

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetDstV4(): "
                   "GetDstV4 failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::SetDstV4
(
  unsigned int   addr,
  unsigned int   subnetMask
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("PSQoSSpecWrapper::SetDstV4(): "
                  "IPv4 addr 0x%x, subnet mask 0x%x", addr, subnetMask);

  mpICritSect->Enter();
  if (IP_V4 == mPSIPFilterSpec.ip_vsn)
  {
    mPSIPFilterSpec.ip_hdr.v4.dst.addr.ps_s_addr =  addr;
    mPSIPFilterSpec.ip_hdr.v4.dst.subnet_mask.ps_s_addr = subnetMask;

    mPSIPFilterSpec.ip_hdr.v4.field_mask |= (uint32) IPFLTR_MASK_IP4_DST_ADDR;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetDstV4(): "
                   "SetDstV4 failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::GetTos
(
  unsigned char* pVal,
  unsigned char* pMask
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pVal || NULL == pMask)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpICritSect->Enter();
  if (IP_V4 == mPSIPFilterSpec.ip_vsn &&
    (uint32) IPFLTR_MASK_IP4_TOS & mPSIPFilterSpec.ip_hdr.v4.field_mask)
  {
    *pVal = mPSIPFilterSpec.ip_hdr.v4.tos.val;
    *pMask = mPSIPFilterSpec.ip_hdr.v4.tos.mask;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  LOG_MSG_INFO2_2 ("PSQoSSpecWrapper::GetTos(): "
                   "V4 TOS val 0x%x, mask 0x%x", *pVal, *pMask);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetTos(): "
                   "GetTos failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::SetTos
(
  unsigned char val,
  unsigned char mask
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2 ("PSQoSSpecWrapper::SetTos(): "
                   "V4 TOS val 0x%x, mask 0x%x", val, mask);

  mpICritSect->Enter();
  if (IP_V4 == mPSIPFilterSpec.ip_vsn)
  {
    mPSIPFilterSpec.ip_hdr.v4.tos.val = val;
    mPSIPFilterSpec.ip_hdr.v4.tos.mask = mask;

    mPSIPFilterSpec.ip_hdr.v4.field_mask |= (uint32) IPFLTR_MASK_IP4_TOS;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetTos(): "
                   "SetTos failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::GetFlowLabel
(
  int* pFlowLabel
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pFlowLabel)
  {
    result = QDS_EFAULT;
    goto bail;
  }
  
  mpICritSect->Enter();
  if (IP_V6 == mPSIPFilterSpec.ip_vsn &&
    (uint32) IPFLTR_MASK_IP6_FLOW_LABEL & mPSIPFilterSpec.ip_hdr.v6.field_mask)
  {
    *pFlowLabel = mPSIPFilterSpec.ip_hdr.v6.flow_label;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::GetFlowLabel(): "
                   "V6 flow label 0x%x", *pFlowLabel);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetFlowLabel(): "
                   "GetFlowLabel failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::SetFlowLabel
(
 int flowLabel
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::SetFlowLabel(): "
                   "V6 flow label 0x%x", flowLabel);

  mpICritSect->Enter();
  if (IP_V6 == mPSIPFilterSpec.ip_vsn)
  {
    mPSIPFilterSpec.ip_hdr.v6.flow_label = flowLabel;
    mPSIPFilterSpec.ip_hdr.v6.field_mask |= (uint32) IPFLTR_MASK_IP6_FLOW_LABEL;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetFlowLabel(): "
                   "SetFlowLabel failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::GetSrcV6
(
  ds::INAddr6Type* pAddr,
  unsigned char* pPrefixLen
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pAddr || NULL == pPrefixLen)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpICritSect->Enter();
  if (IP_V6 == mPSIPFilterSpec.ip_vsn &&
    (uint32) IPFLTR_MASK_IP6_SRC_ADDR & mPSIPFilterSpec.ip_hdr.v6.field_mask)
  {
    (void) memscpy (*pAddr, sizeof(INAddr6Type),
      &mPSIPFilterSpec.ip_hdr.v6.src.addr,
      sizeof(INAddr6Type));
    *pPrefixLen = mPSIPFilterSpec.ip_hdr.v6.src.prefix_len;
    
    IPV6_ADDR_MSG(mPSIPFilterSpec.ip_hdr.v6.src.addr.ps_s6_addr64);
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetSrcV6(): "
                   "GetSrcV6 failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::SetSrcV6
(
  const ds::INAddr6Type addr,
  unsigned char prefixLen
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == addr)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpICritSect->Enter();
  if (IP_V6 == mPSIPFilterSpec.ip_vsn)
  {
    (void) memscpy (&mPSIPFilterSpec.ip_hdr.v6.src.addr, sizeof(INAddr6Type),
      addr, sizeof(INAddr6Type));
    mPSIPFilterSpec.ip_hdr.v6.src.prefix_len = prefixLen;

    mPSIPFilterSpec.ip_hdr.v6.field_mask |= (uint32) IPFLTR_MASK_IP6_SRC_ADDR;
    
    IPV6_ADDR_MSG(mPSIPFilterSpec.ip_hdr.v6.src.addr.ps_s6_addr64);
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetSrcV6(): "
                   "SetSrcV6 failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::GetDstV6
(
  ds::INAddr6Type* pAddr,
  unsigned char* pPrefixLen
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pAddr || NULL == pPrefixLen)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpICritSect->Enter();
  if (IP_V6 == mPSIPFilterSpec.ip_vsn &&
    (uint32) IPFLTR_MASK_IP6_DST_ADDR & mPSIPFilterSpec.ip_hdr.v6.field_mask)
  {
    (void) memscpy (*pAddr, sizeof(INAddr6Type),
      &mPSIPFilterSpec.ip_hdr.v6.dst.addr,
      sizeof(INAddr6Type));

    *pPrefixLen = mPSIPFilterSpec.ip_hdr.v6.dst.prefix_len;
    
    IPV6_ADDR_MSG(mPSIPFilterSpec.ip_hdr.v6.dst.addr.ps_s6_addr64);
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetDstV6(): "
                   "GetDstV6 failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::SetDstV6
(
  const ds::INAddr6Type addr,
  unsigned char prefixLen
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == addr)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpICritSect->Enter();
  if (IP_V6 == mPSIPFilterSpec.ip_vsn)
  {
    (void) memscpy (&mPSIPFilterSpec.ip_hdr.v6.dst.addr,
      sizeof(INAddr6Type),
      addr,
      sizeof(INAddr6Type));

    mPSIPFilterSpec.ip_hdr.v6.dst.prefix_len = prefixLen;

    mPSIPFilterSpec.ip_hdr.v6.field_mask |= (uint32) IPFLTR_MASK_IP6_DST_ADDR;
    
    IPV6_ADDR_MSG(mPSIPFilterSpec.ip_hdr.v6.dst.addr.ps_s6_addr64);
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetDstV6(): "
                   "SetDstV6 failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::GetTrafficClass
(
  unsigned char* pVal,
  unsigned char* pMask
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pVal || NULL == pMask)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpICritSect->Enter();
  if (IP_V6 == mPSIPFilterSpec.ip_vsn &&
    (uint32) IPFLTR_MASK_IP6_TRAFFIC_CLASS & mPSIPFilterSpec.ip_hdr.v6.field_mask)
  {
    *pVal = mPSIPFilterSpec.ip_hdr.v6.trf_cls.val;
    *pMask = mPSIPFilterSpec.ip_hdr.v6.trf_cls.mask;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  LOG_MSG_INFO2_2 ("PSQoSSpecWrapper::GetTrafficClass(): "
                   "V6 trf class val 0x%x, mask 0x%x", *pVal, *pMask);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetTrafficClass(): "
                   "GetTrafficClass failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::SetTrafficClass
(
  unsigned char val,
  unsigned char mask
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2 ("PSQoSSpecWrapper::SetTrafficClass(): "
                   "V6 trf class val 0x%x, mask 0x%x", val, mask);


  mpICritSect->Enter();
  if (IP_V6 == mPSIPFilterSpec.ip_vsn)
  {
    mPSIPFilterSpec.ip_hdr.v6.trf_cls.val = val;
    mPSIPFilterSpec.ip_hdr.v6.trf_cls.mask = mask;
    
    mPSIPFilterSpec.ip_hdr.v6.field_mask |= 
      (uint32) IPFLTR_MASK_IP6_TRAFFIC_CLASS;
  }
  else
  {
    mpICritSect->Leave();
    result = QDS_EINVAL;
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetTrafficClass(): "
                   "SetTrafficClass failed, err %d", result);
  return result;
}

int PSQoSSpecWrapper::GetAuxInfo
(
  unsigned short int* pFiId,
  unsigned short int* pFiPrecedence
)
{
  int result;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pFiId || NULL == pFiPrecedence)
  {
    result = QDS_EFAULT;
    goto bail;
  }
  mpICritSect->Enter();
  *pFiId = mPSIPFilterSpec.ipfltr_aux_info.fi_id;
  *pFiPrecedence = mPSIPFilterSpec.ipfltr_aux_info.fi_precedence;
  mpICritSect->Leave();

  LOG_MSG_INFO2_2 ("PSQoSSpecWrapper::GetAuxInfo(): "
                   " Filter ID 0x%x, Prcd 0x%x", *pFiId, *pFiPrecedence);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetAuxInfo(): "
                   "GetAuxInfo failed, err %d", result);
  return result;
}

ds::ErrorType CDECL PSQoSSpecWrapper::GetEspSpi 
(
 int* pEspSpi
)
{
  int result;
  IPFilterIPNextProtocolType  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pEspSpi)
  {
    result = QDS_EFAULT;
    goto bail;
  }
  
  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

  if ((uint8) PS_IPPROTO_ESP == nextHdrProt &&
    (uint32) IPFLTR_MASK_ESP_SPI & mPSIPFilterSpec.next_prot_hdr.esp.field_mask)
  {
    *pEspSpi = mPSIPFilterSpec.next_prot_hdr.esp.spi;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::GetEspSpi(): "
                   "ESP SPI 0x%x", *pEspSpi);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetEspSpi(): "
                   "GetEspSpi failed, err %d", result);
  return result;

} /* GetEspSpi() */

ds::ErrorType CDECL PSQoSSpecWrapper::GetAhSpi 
(
  int* pAhSpi
)
{
  int result;
  IPFilterIPNextProtocolType  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pAhSpi)
  {
    result = QDS_EFAULT;
    goto bail;
  }
  
  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

 if ((uint8) PS_IPPROTO_AH == nextHdrProt &&
    (uint32) IPFLTR_MASK_AH_SPI & mPSIPFilterSpec.next_prot_hdr.ah.field_mask)
  {
    *pAhSpi = mPSIPFilterSpec.next_prot_hdr.ah.spi;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::GetAhSpi(): "
                   "AH SPI 0x%x", *pAhSpi);
  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetAhSpi(): "
                   "GetAhSpi failed, err %d", result);
  return result;

} /* GetAhSpi() */



ds::ErrorType CDECL PSQoSSpecWrapper::GetICMPType 
(
 unsigned char* pICMPType
)
{
  int result;
  IPFilterIPNextProtocolType  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pICMPType)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::GetICMPType(): "
                   "GetICMPType %c", *pICMPType);

  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

  if ((uint8) PS_IPPROTO_ICMP == nextHdrProt &&
    ((uint32) IPFLTR_MASK_ICMP_MSG_TYPE & 
    mPSIPFilterSpec.next_prot_hdr.icmp.field_mask))
  {
    *pICMPType = mPSIPFilterSpec.next_prot_hdr.icmp.type;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetICMPType(): "
                   "GetICMPType failed, err %d", result);
  return result;

} /* GetICMPType() */



ds::ErrorType CDECL PSQoSSpecWrapper::GetICMPCode
(
 unsigned char* pICMPCode
)
{
  int result;
  IPFilterIPNextProtocolType  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pICMPCode)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::GetICMPCode(): "
                   "GetICMPCode %c", *pICMPCode);

  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

  if ((uint8) PS_IPPROTO_ICMP == nextHdrProt &&
    ((uint32) IPFLTR_MASK_ICMP_MSG_CODE & 
    mPSIPFilterSpec.next_prot_hdr.icmp.field_mask))
  {
    *pICMPCode = mPSIPFilterSpec.next_prot_hdr.icmp.code;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::GetICMPCode(): "
                   "GetICMPCode failed, err %d", result);
  return result;

} /* GetICMPCode() */



ds::ErrorType CDECL PSQoSSpecWrapper::SetEspSpi 
(
 int espSpi
)
{
  int result;
  IPFilterIPNextProtocolType  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::SetEspSpi(): "
                   "ESP SPI 0x%x", espSpi);

  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

  if ((uint8) PS_IPPROTO_ESP == nextHdrProt)
  {
    mPSIPFilterSpec.next_prot_hdr.esp.spi = espSpi;
    mPSIPFilterSpec.next_prot_hdr.esp.field_mask |= 
      (uint32) IPFLTR_MASK_ESP_SPI;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetEspSpi(): "
                   "SetEspSpi failed, err %d", result);
  return result;

} /* SetEspSpi() */


ds::ErrorType CDECL PSQoSSpecWrapper::SetAhSpi 
(
 int AhSpi
)
{
  int result = AEE_SUCCESS;
  IPFilterIPNextProtocolType  nextHdrProt = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::SetAhSpi(): "
                   "AH SPI 0x%x", AhSpi);

  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

  if ((uint8) PS_IPPROTO_AH == nextHdrProt)
  {
    mPSIPFilterSpec.next_prot_hdr.ah.spi = AhSpi;
    mPSIPFilterSpec.next_prot_hdr.ah.field_mask |= 
    (uint32) IPFLTR_MASK_AH_SPI;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetAhSpi(): "
                   "SetAhSpi failed, err %d", result);
  return result;

} /* SetAhSpi() */


ds::ErrorType CDECL PSQoSSpecWrapper::SetICMPType
( 
 unsigned char ICMPType
)
{
  int result;
  IPFilterIPNextProtocolType  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::SetICMPType(): "
                   "SetICMPType %c", ICMPType);

  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

  if ((uint8) PS_IPPROTO_ICMP == nextHdrProt)
  {
    mPSIPFilterSpec.next_prot_hdr.icmp.type = ICMPType;
    mPSIPFilterSpec.next_prot_hdr.icmp.field_mask |= 
      (uint32) IPFLTR_MASK_ICMP_MSG_TYPE;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetICMPType(): "
                   "SetICMPType failed, err %d", result);
  return result;

} /* SetICMPType() */



ds::ErrorType CDECL PSQoSSpecWrapper::SetICMPCode
( 
 unsigned char ICMPCode
)
{
  int result;
  IPFilterIPNextProtocolType  nextHdrProt;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1 ("PSQoSSpecWrapper::SetICMPCode(): "
                   "SetICMPCode %c", ICMPCode);

  mpICritSect->Enter();
  result = GetNextHdrProt (&nextHdrProt);
  if (AEE_SUCCESS != result)
  { 
    mpICritSect->Leave();
    goto bail;
  }

  if ((uint8) PS_IPPROTO_ICMP == nextHdrProt)
  {
    mPSIPFilterSpec.next_prot_hdr.icmp.code = ICMPCode;
    mPSIPFilterSpec.next_prot_hdr.icmp.field_mask |= 
      (uint32) IPFLTR_MASK_ICMP_MSG_CODE;
  }
  else
  {
    result = QDS_EINVAL;
    mpICritSect->Leave();
    goto bail;
  }
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("PSQoSSpecWrapper::SetICMPCode(): "
                   "SetICMPCode failed, err %d", result);
  return result;

} /* SetICMPCode() */

ds::ErrorType PSQoSSpecWrapper::GetPSQoSFlowSpec
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

  mpICritSect->Enter();
  (void) memscpy (pPSFlowSpec, sizeof (mPSQoSFlowSpec),
    &mPSQoSFlowSpec, sizeof (mPSQoSFlowSpec));
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* GetPSQoSFlowSpec() */

ds::ErrorType PSQoSSpecWrapper::UpdatePSQoSFlowSpec
( 
 NetPlatform::PSFlowSpecType  *pPSFlowSpec
 )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == pPSFlowSpec)
  {
    return QDS_EINVAL;
  }

  mpICritSect->Enter();
  (void) memscpy (&mPSQoSFlowSpec, sizeof (mPSQoSFlowSpec),
    pPSFlowSpec, sizeof (mPSQoSFlowSpec));
  mpICritSect->Leave();
  
  return AEE_SUCCESS;

} /* GetPSQoSFlowSpec() */


ds::ErrorType PSQoSSpecWrapper::GetPSIPFilterSpec
(
 NetPlatform::PSFilterSpecType *pPSIPFilterSpec
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pPSIPFilterSpec)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  (void) memscpy (pPSIPFilterSpec,
    sizeof(NetPlatform::PSFilterSpecType),
    &mPSIPFilterSpec, 
    sizeof(NetPlatform::PSFilterSpecType));
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* GetPSIPFilterSpec() */

ds::ErrorType PSQoSSpecWrapper::UpdatePSIPFilterSpec
(
 NetPlatform::PSFilterSpecType *pPSIPFilterSpec
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pPSIPFilterSpec)
  {
    return QDS_EFAULT;
  }
  mpICritSect->Enter();
  (void) memscpy (&mPSIPFilterSpec,
    sizeof(NetPlatform::PSFilterSpecType),
    pPSIPFilterSpec, 
    sizeof(NetPlatform::PSFilterSpecType));
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* UpdateSpec() */

