#ifndef QOS_FLOW_SPEC_H
#define QOS_FLOW_SPEC_H
/*===========================================================================
  @file QoSFlowSpec.h

  This file defines the class that implements the IQoSFlowSpec interface.

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSFlowSpec.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-08-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_IQoSFlowPriv.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_PSQoSSpecWrapper.h"

namespace ds
{
namespace Net
{
   /*lint -esym(1510, IQoSFlowPriv) */
   /*lint -esym(1510, IQI) */
class QoSFlowSpec : public IQoSFlowPriv
{
private:
  /* To suppress lint error 429 */
  QoSFlowSpec*                  mpQoSFlowSpecClone;
  PSQoSSpecWrapper*             mpPSQoSSpecWrapper;

  /* Auxillary internal functions */
  virtual ds::ErrorType CDECL GetOptionsInternal
  (
    QoSFlowOptIDType* pOpts,
    int optsLen,
    int* pOptsLenReq,
    boolean isErrOpt
  );

  virtual ds::ErrorType CDECL GetParams
  (
    void*     pDst,
    int       dstLen,
    void*     pSrc,
    uint32    paramName,
    uint32    fieldMask
  );

  virtual ds::ErrorType CDECL SetParams
  (
    void*           pDst,
    int             dstLen,
    const void*     pSrc,
    uint32          paramName,
    uint32*         pFieldMask
  );


  /*!
  @brief
  Destructor of the QoSFlowSpec object.

  @details
  Destructor of the QoSFlowSpec object. The destructor is private and
  hence we cannot call "delete pQoSFlowSpec" etc. Use Release() method
  to release the pointer to this object.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  virtual ~QoSFlowSpec
  (
    void
  )
  throw();

public:
  /* Constructor */
  QoSFlowSpec(NetPlatform::PSFlowSpecType* pPSFlowSpec = NULL);

  /* Inherited from IQoSFlowPriv */
  virtual ds::ErrorType CDECL Clone (IQoSFlowPriv** flowObj);
  virtual ds::ErrorType CDECL GetValidOptions (QoSFlowOptIDType* ValidOptions, int ValidOptionsLen, int* ValidOptionsLenReq);
  virtual ds::ErrorType CDECL GetErroneousOptions (QoSFlowOptIDType* ErroneousOptions, int ErroneousOptionsLen, int* ErroneousOptionsLenReq);
  virtual ds::ErrorType CDECL GetTrfClass (QoSFlowOptIPTrafficClassType* TrfClass);
  virtual ds::ErrorType CDECL SetTrfClass (QoSFlowOptIPTrafficClassType TrfClass);
  virtual ds::ErrorType CDECL GetLatency (QoSFlowOptLatencyType* Latency);
  virtual ds::ErrorType CDECL SetLatency (QoSFlowOptLatencyType Latency);
  virtual ds::ErrorType CDECL GetLatencyVar (QoSFlowOptLatencyVarianceType* LatencyVar);
  virtual ds::ErrorType CDECL SetLatencyVar (QoSFlowOptLatencyVarianceType LatencyVar);
  virtual ds::ErrorType CDECL GetMinPolicedPktSize (QoSFlowOptMinPolicedPacketSizeType* MinPolicedPktSize);
  virtual ds::ErrorType CDECL SetMinPolicedPktSize (QoSFlowOptMinPolicedPacketSizeType MinPolicedPktSize);
  virtual ds::ErrorType CDECL GetMaxAllowedPktSize (QoSFlowOptMaxAllowedPacketSizeType* MaxAllowedPktSize);
  virtual ds::ErrorType CDECL SetMaxAllowedPktSize (QoSFlowOptMaxAllowedPacketSizeType MaxAllowedPktSize);
  virtual ds::ErrorType CDECL GetUmtsResBer (QoSFlowOptUMTSResidualBitErrorRateType* UmtsResBer);
  virtual ds::ErrorType CDECL SetUmtsResBer (QoSFlowOptUMTSResidualBitErrorRateType UmtsResBer);
  virtual ds::ErrorType CDECL GetUmtsTrfPri (QoSFlowOptUMTSTrafficPriorityType* UmtsTrfPri);
  virtual ds::ErrorType CDECL SetUmtsTrfPri (QoSFlowOptUMTSTrafficPriorityType UmtsTrfPri);
  virtual ds::ErrorType CDECL GetCdmaProfileID (QoSFlowOptCDMAProfileIDType* CdmaProfileID);
  virtual ds::ErrorType CDECL SetCdmaProfileID (QoSFlowOptCDMAProfileIDType CdmaProfileID);
  virtual ds::ErrorType CDECL GetWlanUserPriority (QoSFlowOptWLANUserPriorityType* WlanUserPriority);
  virtual ds::ErrorType CDECL SetWlanUserPriority (QoSFlowOptWLANUserPriorityType WlanUserPriority);
  virtual ds::ErrorType CDECL GetWlanMinServiceInterval (QoSFlowOptWLANMinServiceIntervalType* WlanMinServiceInterval);
  virtual ds::ErrorType CDECL SetWlanMinServiceInterval (QoSFlowOptWLANMinServiceIntervalType WlanMinServiceInterval);
  virtual ds::ErrorType CDECL GetWlanMaxServiceInterval (QoSFlowOptWLANMaxServiceIntervalType* WlanMaxServiceInterval);
  virtual ds::ErrorType CDECL SetWlanMaxServiceInterval (QoSFlowOptWLANMaxServiceIntervalType WlanMaxServiceInterval);
  virtual ds::ErrorType CDECL GetWlanInactivityInterval (QoSFlowOptWLANInactivityIntervalType* WlanInactivityInterval);
  virtual ds::ErrorType CDECL SetWlanInactivityInterval (QoSFlowOptWLANInactivityIntervalType WlanInactivityInterval);
  virtual ds::ErrorType CDECL GetCdmaFlowPriority (QoSFlowOptCDMAFlowPriorityType* CdmaFlowPriority);
  virtual ds::ErrorType CDECL SetCdmaFlowPriority (QoSFlowOptCDMAFlowPriorityType CdmaFlowPriority);
  virtual ds::ErrorType CDECL GetUmtsImCnFlag (QoSFlowOptUMTSImsSignalingContextType* UmtsImCnFlag);
  virtual ds::ErrorType CDECL SetUmtsImCnFlag (QoSFlowOptUMTSImsSignalingContextType UmtsImCnFlag);
  virtual ds::ErrorType CDECL GetUmtsSigInd (QoSFlowOptUMTSHighPriorityDataType* UmtsSigInd);
  virtual ds::ErrorType CDECL SetUmtsSigInd (QoSFlowOptUMTSHighPriorityDataType UmtsSigInd);
  virtual ds::ErrorType CDECL GetDataRateMinMax (QoSFlowOptDataRateMinMaxType* DataRateMinMax);
  virtual ds::ErrorType CDECL SetDataRateMinMax (const QoSFlowOptDataRateMinMaxType* DataRateMinMax);
  virtual ds::ErrorType CDECL GetDataRateMinMaxEx (QoSFlowOptDataRateMinMaxExType* DataRateMinMaxEx);
  virtual ds::ErrorType CDECL SetDataRateMinMaxEx (const QoSFlowOptDataRateMinMaxExType* DataRateMinMaxEx);
  virtual ds::ErrorType CDECL GetDataRateTokenBucket (QoSFlowOptDataRateTokenBucketType* DataRateTokenBucket);
  virtual ds::ErrorType CDECL SetDataRateTokenBucket (const QoSFlowOptDataRateTokenBucketType* DataRateTokenBucket);
  virtual ds::ErrorType CDECL GetPktErrRate (QoSFlowOptPacketErrorRateType* PktErrRate);
  virtual ds::ErrorType CDECL SetPktErrRate (const QoSFlowOptPacketErrorRateType* PktErrRate);
  virtual ds::ErrorType CDECL GetNominalSDUSize (QoSFlowOptNominalSDUSizeType* NominalSDUSize);
  virtual ds::ErrorType CDECL SetNominalSDUSize (const QoSFlowOptNominalSDUSizeType* NominalSDUSize);


  /* Functions inherited from IQI interface */
  DSIQI_IMPL_DEFAULTS(IQoSFlowPriv)

  /*-------------------------------------------------------------------------
    Internal methods.
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL GetPSQoSFlowSpec
  (
    NetPlatform::PSFlowSpecType  *pPSFlowSpec
  );

  virtual ds::ErrorType CDECL UpdateSpec
  (
    NetPlatform::PSFlowSpecType  *pPSFlowSpec
  );

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_QOS_FLOW_SPEC)


}; /* class QoSFlowSpec */

} /* namespace Net */
} /* namespace ds */

#endif /* QOS_FLOW_SPEC_H */

