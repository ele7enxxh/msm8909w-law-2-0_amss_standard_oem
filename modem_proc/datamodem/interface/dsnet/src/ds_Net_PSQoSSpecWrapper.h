#ifndef DS_NET_PSQOSSPECWRAPPER_H
#define DS_NET_PSQOSSPECWRAPPER_H
/*===========================================================================
@file ds_Net_PSQoSSpecWrapper.h

This file defines the class that is used as wrapper for the QoS Flow and 
QoS Fillter ps layer.

Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_PSQoSSpecWrapper.h#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2010-08-01 en  Created module.

===========================================================================*/
/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Utils_StdErr.h"
#include "ds_Net_Utils.h"
#include "ds_Net_Platform.h"
#include "ds_Net_MemManager.h"
#include "ds_Utils_ICritSect.h"

namespace ds
{
	namespace Net
	{
		/*lint -esym(1510, IQoSFlowPriv) */
		/*lint -esym(1510, IQI) */
		class PSQoSSpecWrapper
		{
		private:
			NetPlatform::PSFlowSpecType		    mPSQoSFlowSpec;
			NetPlatform::PSFilterSpecType     mPSIPFilterSpec;

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
      Destructor of the PSQoSSpecWrapper object.

      @details
      Destructor of the PSQoSSpecWrapper object. The destructor is private and 
      hence we cannot call "delete PSQoSSpecWrapper" etc. Use Release() method
      to release the pointer to this object.

      @param      None.
      @return     None.
      @see        IQI::Release()
      */
      virtual ~PSQoSSpecWrapper
      (
        void 
      ) 
      throw();
     /*!
      This is the critical section used by this object. DO NOT
      define multiple crit sects per object
     */
      ICritSect*                mpICritSect;

    public:
      /* Referance count defines*/
      DSIQI_IMPL_DEFAULTS_NO_QI();

      /* Constructor */
      PSQoSSpecWrapper();

      /* Clone method */
      ds::ErrorType Clone
      (
        const PSQoSSpecWrapper* from
      );

      /* QoS Flow functions*/
      virtual ds::ErrorType CDECL GetTrfClass (int* pTrfClass);
      virtual ds::ErrorType CDECL SetTrfClass (int TrfClass);
      virtual ds::ErrorType CDECL GetLatency (int* pLatency);
      virtual ds::ErrorType CDECL SetLatency (int Latency);
      virtual ds::ErrorType CDECL GetLatencyVar (int* pLatencyVar);
      virtual ds::ErrorType CDECL SetLatencyVar (int LatencyVar);
      virtual ds::ErrorType CDECL GetMinPolicedPktSize (int* pMinPolicedPktSize);
      virtual ds::ErrorType CDECL SetMinPolicedPktSize (int MinPolicedPktSize);
      virtual ds::ErrorType CDECL GetMaxAllowedPktSize (int* pMaxAllowedPktSize);
      virtual ds::ErrorType CDECL SetMaxAllowedPktSize (int MaxAllowedPktSize);
      virtual ds::ErrorType CDECL GetUmtsResBer (int* pUmtsResBer);
      virtual ds::ErrorType CDECL SetUmtsResBer (int UmtsResBer);
      virtual ds::ErrorType CDECL GetUmtsTrfPri (int* pUmtsTrfPri);
      virtual ds::ErrorType CDECL SetUmtsTrfPri (int UmtsTrfPri);
      virtual ds::ErrorType CDECL GetCdmaProfileID (unsigned short int* pCdmaProfileID);
      virtual ds::ErrorType CDECL SetCdmaProfileID (unsigned short int CdmaProfileID);
      virtual ds::ErrorType CDECL GetWlanUserPriority (int* pWlanUserPriority);
      virtual ds::ErrorType CDECL SetWlanUserPriority (int WlanUserPriority);
      virtual ds::ErrorType CDECL GetWlanMinServiceInterval (int* pWlanMinServiceInterval);
      virtual ds::ErrorType CDECL SetWlanMinServiceInterval (int WlanMinServiceInterval);
      virtual ds::ErrorType CDECL GetWlanMaxServiceInterval (int* pWlanMaxServiceInterval);
      virtual ds::ErrorType CDECL SetWlanMaxServiceInterval (int WlanMaxServiceInterval);
      virtual ds::ErrorType CDECL GetWlanInactivityInterval (int* pWlanInactivityInterval);
      virtual ds::ErrorType CDECL SetWlanInactivityInterval (int WlanInactivityInterval);
      virtual ds::ErrorType CDECL GetCdmaFlowPriority (unsigned char* pCdmaFlowPriority);
      virtual ds::ErrorType CDECL SetCdmaFlowPriority (unsigned char CdmaFlowPriority);
      virtual ds::ErrorType CDECL GetUmtsImCnFlag (boolean* pUmtsImCnFlag);
      virtual ds::ErrorType CDECL SetUmtsImCnFlag (boolean UmtsImCnFlag);
      virtual ds::ErrorType CDECL GetUmtsSigInd (boolean* pUmtsSigInd);
      virtual ds::ErrorType CDECL SetUmtsSigInd (boolean UmtsSigInd);
      virtual ds::ErrorType CDECL GetDataRateMinMax (int* pMaxRate, int* pGuaranteedRate);
      virtual ds::ErrorType CDECL SetDataRateMinMax (int maxRate, int guaranteedRate);
      virtual ds::ErrorType CDECL GetDataRateMinMaxEx (int64* pMaxRate, int64* pGuaranteedRate);
      virtual ds::ErrorType CDECL SetDataRateMinMaxEx (int64 maxRate, int64 guaranteedRate);
      virtual ds::ErrorType CDECL GetDataRateTokenBucket (int* pPeakRate, int* pTokenRate, int* pSize);
      virtual ds::ErrorType CDECL SetDataRateTokenBucket (int peakRate, int tokenRate, int size);
      virtual ds::ErrorType CDECL GetPktErrRate (unsigned short int* pMultiplier, unsigned short int* pExponent);
      virtual ds::ErrorType CDECL SetPktErrRate (unsigned short int multiplier, unsigned short int exponent);
      virtual ds::ErrorType CDECL GetNominalSDUSize (boolean* pFixed, int* pSize);
      virtual ds::ErrorType CDECL SetNominalSDUSize (boolean fixed, int size);

      /* IP Filter functions */
      virtual ds::ErrorType CDECL GetIPVsn (unsigned char* pIPVsn);
      virtual ds::ErrorType CDECL SetIPVsn (unsigned char IPVsn);
      virtual ds::ErrorType CDECL GetNextHdrProt (unsigned char* pNextHdrProt);
      virtual ds::ErrorType CDECL SetNextHdrProt (unsigned char NextHdrProt);
      virtual ds::ErrorType CDECL GetSrcPort (unsigned short int* pPort, unsigned short int* pRange);
      virtual ds::ErrorType CDECL SetSrcPort (unsigned short int port, unsigned short int range);
      virtual ds::ErrorType CDECL GetDstPort (unsigned short int* pPort, unsigned short int* pRange);
      virtual ds::ErrorType CDECL SetDstPort (unsigned short int port, unsigned short int range);
      virtual ds::ErrorType CDECL GetSrcV4 (unsigned int* pAddr, unsigned int* pSubnetMask);
      virtual ds::ErrorType CDECL SetSrcV4 (unsigned int addr, unsigned int subnetMask);
      virtual ds::ErrorType CDECL GetDstV4 (unsigned int* pAddr, unsigned int* pSubnetMask);
      virtual ds::ErrorType CDECL SetDstV4 (unsigned int addr, unsigned int subnetMask);
      virtual ds::ErrorType CDECL GetTos (unsigned char* pVal, unsigned char* pMask);
      virtual ds::ErrorType CDECL SetTos (unsigned char val, unsigned char mask);
      virtual ds::ErrorType CDECL GetFlowLabel (int* pFlowLabel);
      virtual ds::ErrorType CDECL SetFlowLabel (int flowLabel);
      virtual ds::ErrorType CDECL GetSrcV6 (ds::INAddr6Type* pAddr, unsigned char* pPrefixLen);
      virtual ds::ErrorType CDECL SetSrcV6 (const ds::INAddr6Type addr, unsigned char prefixLen);
      virtual ds::ErrorType CDECL GetDstV6 (ds::INAddr6Type* pAddr, unsigned char* pPrefixLen);
      virtual ds::ErrorType CDECL SetDstV6 (const ds::INAddr6Type addr, unsigned char prefixLen);
      virtual ds::ErrorType CDECL GetTrafficClass (unsigned char* pVal, unsigned char* pMask);
      virtual ds::ErrorType CDECL SetTrafficClass (unsigned char val, unsigned char mask);
      virtual ds::ErrorType CDECL GetEspSpi (int* pEspSpi);
      virtual ds::ErrorType CDECL SetEspSpi (int EspSpi);
      virtual ds::ErrorType CDECL GetAhSpi (int* pAhSpi);
      virtual ds::ErrorType CDECL SetAhSpi (int AhSpi);
      virtual ds::ErrorType CDECL GetICMPType (unsigned char* pICMPType);
      virtual ds::ErrorType CDECL SetICMPType (unsigned char ICMPType);     
      virtual ds::ErrorType CDECL GetICMPCode (unsigned char* pICMPCode);
      virtual ds::ErrorType CDECL SetICMPCode (unsigned char ICMPCode);
      virtual ds::ErrorType CDECL GetAuxInfo (unsigned short int* pFiId, unsigned short int* pFiPrecedence);

      /*-------------------------------------------------------------------------
      Internal methods.
      -------------------------------------------------------------------------*/
      virtual ds::ErrorType CDECL GetPSQoSFlowSpec (NetPlatform::PSFlowSpecType  *pPSFlowSpec);

      virtual ds::ErrorType CDECL UpdatePSQoSFlowSpec (NetPlatform::PSFlowSpecType  *pPSFlowSpec);

      virtual ds::ErrorType CDECL GetPSIPFilterSpec (NetPlatform::PSFilterSpecType *pPSIPFilterSpec);

      virtual ds::ErrorType CDECL UpdatePSIPFilterSpec (NetPlatform::PSFilterSpecType *pPSFilterSpec);
      /*-------------------------------------------------------------------------
        Methods to overload new/delete operators.
      -------------------------------------------------------------------------*/

     DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_PS_QOS_SPEC_WRAPPER);
	  };
  }
}

#endif //DS_NET_PSQOSSPECWRAPPER_H

