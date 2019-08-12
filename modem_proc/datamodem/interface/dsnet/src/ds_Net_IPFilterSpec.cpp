/*===========================================================================
  FILE: IPFilterSpec.cpp

  OVERVIEW: This file provides implementation of the IPFilterSpec class.

  DEPENDENCIES: None

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_IPFilterSpec.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-08-10 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include <stringl/stringl.h>
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_IPFilterSpec.h"
#include "ds_Net_IIPFilterPriv.h"
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
IPFilterSpec::IPFilterSpec
(
  void
): mpIPFilterSpecClone(NULL),
   mpPSQoSSpecWrapper(NULL),
   refCnt(1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("IPFilterSpec::IPFilterSpec(): "
                           "obj 0x%x", this);

  mpPSQoSSpecWrapper = new PSQoSSpecWrapper();
}

IPFilterSpec::~IPFilterSpec()
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("IPFilterSpec::~IPFilterSpec(): "
                           "obj 0x%x, mpIPFilterSpecClone 0x%x, mpPSQoSSpecWrapper 0x%x",
                           this, mpIPFilterSpecClone, mpPSQoSSpecWrapper);

  DS_UTILS_RELEASEIF(mpIPFilterSpecClone);
  DS_UTILS_RELEASEIF(mpPSQoSSpecWrapper);
}

int IPFilterSpec::Clone
(
  IIPFilterPriv** filterObj
)
{
  int result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("IPFilterSpec::Clone(): "
                            "Obj 0x%p", this);

  if (NULL == filterObj)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  mpIPFilterSpecClone = new IPFilterSpec();
  if ((NULL == mpIPFilterSpecClone) ||
      (NULL == mpIPFilterSpecClone->mpPSQoSSpecWrapper))
  {
    result = AEE_ENOMEMORY;
    goto bail;
  }

  result = mpIPFilterSpecClone->mpPSQoSSpecWrapper->Clone(mpPSQoSSpecWrapper);
  if(AEE_SUCCESS != result)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  *filterObj = static_cast <IIPFilterPriv *> (mpIPFilterSpecClone);

  (void) (*filterObj)->AddRef();

  return AEE_SUCCESS;

bail:

  LOG_MSG_ERROR_1 ("IPFilterSpec::Clone(): "
                   "Clone failed, err %d", result);

  DS_UTILS_RELEASEIF(mpIPFilterSpecClone);

  return result;
}

int IPFilterSpec::GetOptionsInternal
(
  IPFilterIDType*  pOpts,
  int              optsLen,
  int *            pOptsLenReq,
  boolean          isErrMask
)
{
  int                             result;
  int                             index;
  uint32                          mask;
  uint8                           nextHdrProto;
  NetPlatform::PSFilterSpecType   localPSIPFilterSpec;

  #define LOCAL_ARRAY_SIZE       (20)

  /* Change the array size to current max num of options supported */
  IPFilterIDType   localOptsArr[LOCAL_ARRAY_SIZE] = {0,};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  LOG_MSG_INFO2_1 ("IPFilterSpec::GetOptionsInternal(): "
                   "obj 0x%p", this);

  if(LOCAL_ARRAY_SIZE <= optsLen)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  result = mpPSQoSSpecWrapper->GetPSIPFilterSpec(&localPSIPFilterSpec);

  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  nextHdrProto = (uint8) PS_NO_NEXT_HDR; 
  index = 0;

  if (IP_V4 == localPSIPFilterSpec.ip_vsn)
  {
    if ((uint32)IPFLTR_MASK_IP4_NEXT_HDR_PROT & 
        localPSIPFilterSpec.ip_hdr.v4.field_mask)
    {
      nextHdrProto = localPSIPFilterSpec.ip_hdr.v4.next_hdr_prot;
    }

    if (TRUE == isErrMask)
    {
      mask = localPSIPFilterSpec.ip_hdr.v4.err_mask;
    }
    else
    {
      mask = localPSIPFilterSpec.ip_hdr.v4.field_mask;
    }

    if ((uint32)IPFLTR_MASK_IP4_SRC_ADDR & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_SRC_ADDR;
    }

    if ((uint32)IPFLTR_MASK_IP4_DST_ADDR & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_DST_ADDR;
    }

    if ((uint32)IPFLTR_MASK_IP4_NEXT_HDR_PROT & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_NEXT_HDR_PROTO;
    }

    if ((uint32)IPFLTR_MASK_IP4_TOS & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_IPV4_TOS;
    }
  }
  else if (IP_V6 == localPSIPFilterSpec.ip_vsn)
  {
    if ((uint32)IPFLTR_MASK_IP6_NEXT_HDR_PROT & 
        localPSIPFilterSpec.ip_hdr.v6.field_mask)
    {
      nextHdrProto = localPSIPFilterSpec.ip_hdr.v6.next_hdr_prot;
    }

    if (TRUE == isErrMask)
    {
      mask = localPSIPFilterSpec.ip_hdr.v6.err_mask;
    }
    else
    {
      mask = localPSIPFilterSpec.ip_hdr.v6.field_mask;
    }

    if ((uint32)IPFLTR_MASK_IP6_SRC_ADDR & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_SRC_ADDR;
    }

    if ((uint32)IPFLTR_MASK_IP6_DST_ADDR & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_DST_ADDR;
    }

    if ((uint32)IPFLTR_MASK_IP6_NEXT_HDR_PROT & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_NEXT_HDR_PROTO;
    }

    if ((uint32)IPFLTR_MASK_IP6_TRAFFIC_CLASS & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_IPV6_TRF_CLASS;
    }

    if ((uint32)IPFLTR_MASK_IP6_FLOW_LABEL & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_IPV6_FLOW_LABEL;
    }
  }


  if ((uint8) PS_IPPROTO_TCP == nextHdrProto)
  {
    /* Return valid TCP options */
    if (TRUE == isErrMask)
    {
      mask = localPSIPFilterSpec.next_prot_hdr.tcp.err_mask;
    }
    else
    {
      mask = localPSIPFilterSpec.next_prot_hdr.tcp.field_mask;
    }

    if ((uint32)IPFLTR_MASK_TCP_SRC_PORT & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_TCP_SRC_PORT;
    }

    if ((uint32)IPFLTR_MASK_TCP_DST_PORT & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_TCP_DST_PORT;
    }
  }
  else if ((uint8) PS_IPPROTO_UDP == nextHdrProto)
  {
    /* Return valid UDP options */
    if (TRUE == isErrMask)
    {
      mask = localPSIPFilterSpec.next_prot_hdr.udp.err_mask;
    }
    else
    {
      mask = localPSIPFilterSpec.next_prot_hdr.udp.field_mask;
    }

    if ((uint32)IPFLTR_MASK_UDP_SRC_PORT & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_UDP_SRC_PORT;
    }

    if ((uint32)IPFLTR_MASK_UDP_DST_PORT & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_UDP_DST_PORT;
    }
  }
  else if ((uint8) PS_IPPROTO_ESP == nextHdrProto)
  {
    /* Return valid ESP options */
    if (TRUE == isErrMask)
    {
      mask = localPSIPFilterSpec.next_prot_hdr.esp.err_mask;
    }
    else
    {
      mask = localPSIPFilterSpec.next_prot_hdr.esp.field_mask;
    }

    if ((uint32)IPFLTR_MASK_ESP_SPI & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_ESP_SPI;
    }
  }
  else if ((uint8) PS_IPPROTO_AH == nextHdrProto)
  {
    /* Return valid AH options */
    if (TRUE == isErrMask)
    {
      mask = localPSIPFilterSpec.next_prot_hdr.ah.err_mask;
    }
    else
    {
      mask = localPSIPFilterSpec.next_prot_hdr.ah.field_mask;
    }

    if ((uint32)IPFLTR_MASK_AH_SPI & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_AH_SPI;
    }
  }
  else if ((uint8) PS_IPPROTO_ICMP == nextHdrProto)
  {
    /* Return valid ICMP options */
    if (TRUE == isErrMask)
    {
      mask = localPSIPFilterSpec.next_prot_hdr.icmp.err_mask;
    }
    else
    {
      mask = localPSIPFilterSpec.next_prot_hdr.icmp.field_mask;
    }

    if ((uint32)IPFLTR_MASK_ICMP_MSG_TYPE & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_ICMP_MTYPE;
    }

    if ((uint32)IPFLTR_MASK_ICMP_MSG_CODE & mask)
    {
      localOptsArr[index++] = IPFilterID::QDS_ICMP_MCODE;
    }
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
                    optsLen * sizeof (IPFilterIDType),
                    localOptsArr,
                    optsLen * sizeof (IPFilterIDType));
  }
  else
  {
    (void) memscpy (pOpts,
                    index * sizeof (IPFilterIDType),
                    localOptsArr,
                    index * sizeof (IPFilterIDType));
  }

  return AEE_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("IPFilterSpec::GetOptionsInternal(): "
                   "GetOptionsInternal failed, err %d", result);
  return result;


} /* GetOptionsInternal() */

int IPFilterSpec::GetValidOptions
(
  IPFilterIDType *pValidOptions,
  int             validOptionsLen,
  int             *pValidOptionsLenReq
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return GetOptionsInternal (pValidOptions,
                             validOptionsLen,
                             pValidOptionsLenReq,
                             FALSE);

} /* GetValidOptions() */


int IPFilterSpec::GetErroneousOptions
(
  IPFilterIDType *pErrOpts,
  int            errOptsLen,
  int            *pErrOptsLenReq
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return GetOptionsInternal (pErrOpts,
                             errOptsLen,
                             pErrOptsLenReq,
                             TRUE);

} /* GetErroneousOptions() */


int IPFilterSpec::GetIPVsn
(
  IPFilterIPVersionType *pIPVersion
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetIPVsn((unsigned char *)pIPVersion);
}

int IPFilterSpec::SetIPVsn
(
  IPFilterIPVersionType ipVersion
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetIPVsn(ipVersion);

}

int IPFilterSpec::GetNextHdrProt
(
  IPFilterIPNextProtocolType* pNextHdrProt
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return mpPSQoSSpecWrapper->GetNextHdrProt((unsigned char *)pNextHdrProt);
}


int IPFilterSpec::SetNextHdrProt
(
  IPFilterIPNextProtocolType nextHdrProt
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return mpPSQoSSpecWrapper->SetNextHdrProt(nextHdrProt);
}


int IPFilterSpec::GetSrcPort
(
  IPFilterPortType* pSrcPort
)
{
  unsigned short int localPort = 0;
  int                res;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  res = mpPSQoSSpecWrapper->GetSrcPort(&localPort, &(pSrcPort->range));

  if(AEE_SUCCESS == res)
  {
    memscpy((void *)pSrcPort->port, 
      sizeof(INPortType),
           (const void *)&localPort, 
           sizeof(INPortType));
  }

  return res;
}


int IPFilterSpec::SetSrcPort
(
  const IPFilterPortType* pSrcPort
)
{
  unsigned short int localPort = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memscpy((void *)&localPort,
    sizeof(unsigned short int),
    (const void *)pSrcPort->port,
    sizeof(unsigned short int));

  return mpPSQoSSpecWrapper->SetSrcPort(localPort, pSrcPort->range);
}


int IPFilterSpec::GetDstPort
(
  IPFilterPortType* pDstPort
)
{
  unsigned short int localPort = 0;
  int                res;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  res = mpPSQoSSpecWrapper->GetDstPort(&localPort, &(pDstPort->range));

  if(AEE_SUCCESS == res)
  {
    memscpy((void *)pDstPort->port,
      sizeof(INPortType),
      (const void *)&localPort,
      sizeof(INPortType));
  }

  return res;
}

int IPFilterSpec::SetDstPort
(
  const IPFilterPortType* pDstPort
)
{
  unsigned short int localPort = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  memscpy((void *)&localPort,
    sizeof(unsigned short int),
    (const void *)pDstPort->port,
    sizeof(unsigned short int));

  return mpPSQoSSpecWrapper->SetDstPort(localPort, pDstPort->range);
  
}

int IPFilterSpec::GetSrcV4
(
  IPFilterIPv4AddrType* pSrcV4
)
{
  unsigned int addr = 0;
  unsigned int subnet = 0;
  int          res;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  res = mpPSQoSSpecWrapper->GetSrcV4(&addr, &subnet);
  if(AEE_SUCCESS != res)
  {
    return res;
  }

  memscpy((void *)pSrcV4->addr, sizeof(INAddrType),
    (const void *)&addr, sizeof(INAddrType));

  memscpy((void *)pSrcV4->subnetMask, sizeof(INAddrType),
    (const void *)&subnet, sizeof(INAddrType));
  
  return AEE_SUCCESS;

}

int IPFilterSpec::SetSrcV4
(
  const IPFilterIPv4AddrType* pSrcV4
)
{
  unsigned int addr = 0;
  unsigned int subnet = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memscpy((void *)&addr, sizeof(INAddrType),
    (const void *)pSrcV4->addr, sizeof(INAddrType));
  memscpy((void *)&subnet, sizeof(INAddrType),
    (const void *)pSrcV4->subnetMask, sizeof(INAddrType));
  return mpPSQoSSpecWrapper->SetSrcV4(addr, subnet);

}

int IPFilterSpec::GetDstV4
(
  IPFilterIPv4AddrType* pDstV4
)
{
  unsigned int addr = 0;
  unsigned int subnet = 0;
  int          res;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  res = mpPSQoSSpecWrapper->GetDstV4(&addr, &subnet);
  if(AEE_SUCCESS != res)
  {
    return res;
  }

  memscpy((void *)pDstV4->addr, sizeof(INAddrType),
    (const void *)&addr, sizeof(INAddrType));

  memscpy((void *)pDstV4->subnetMask, sizeof(INAddrType),
    (const void *)&subnet, sizeof(INAddrType));
  
  return AEE_SUCCESS;
}

int IPFilterSpec::SetDstV4
(
  const IPFilterIPv4AddrType* pDstV4
)
{
  unsigned int addr = 0;
  unsigned int subnet = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memscpy((void *)&addr, sizeof(INAddrType),
    (const void *)pDstV4->addr, sizeof(INAddrType));
  memscpy((void *)&subnet, sizeof(INAddrType),
    (const void *)pDstV4->subnetMask, sizeof(INAddrType));

  return mpPSQoSSpecWrapper->SetDstV4(addr, subnet);

}

int IPFilterSpec::GetTos
(
  IPFilterTOSType* pToS
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetTos(&(pToS->val), &(pToS->mask));

}

int IPFilterSpec::SetTos
(
  const IPFilterTOSType* pToS
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  return mpPSQoSSpecWrapper->SetTos(pToS->val, pToS->mask);

}

int IPFilterSpec::GetFlowLabel
(
  IPFilterIPv6FlowLabelType* pFlowLabel
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetFlowLabel((int *) pFlowLabel);

}

int IPFilterSpec::SetFlowLabel
(
  IPFilterIPv6FlowLabelType flowLabel
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetFlowLabel(flowLabel);

}

int IPFilterSpec::GetSrcV6
(
  IPFilterIPv6AddrType* pSrcV6
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  INAddr6Type* localAddr = (INAddr6Type *)(pSrcV6->addr);
  return mpPSQoSSpecWrapper->GetSrcV6(localAddr, &(pSrcV6->prefixLen));

}

int IPFilterSpec::SetSrcV6
(
  const IPFilterIPv6AddrType* pSrcV6
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetSrcV6(pSrcV6->addr, pSrcV6->prefixLen);

}

int IPFilterSpec::GetDstV6
(
  IPFilterIPv6AddrType* pDstV6
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  INAddr6Type* localAddr = (INAddr6Type*)(pDstV6->addr);
  return mpPSQoSSpecWrapper->GetDstV6(localAddr, &(pDstV6->prefixLen));

}

int IPFilterSpec::SetDstV6
(
  const IPFilterIPv6AddrType* pDstV6
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetDstV6(pDstV6->addr, pDstV6->prefixLen);

}


int IPFilterSpec::GetTrafficClass
(
  IPFilterIPv6TrafficClassType* pTrafficClass
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  return mpPSQoSSpecWrapper->GetTrafficClass(&(pTrafficClass->val), &(pTrafficClass->mask));

}

int IPFilterSpec::SetTrafficClass
(
  const IPFilterIPv6TrafficClassType* pTrafficClass
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return mpPSQoSSpecWrapper->SetTrafficClass(pTrafficClass->val, pTrafficClass->mask);
}

int IPFilterSpec::GetAuxInfo
(
  IPFilterAuxInfoType* pAuxInfo
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetAuxInfo(&(pAuxInfo->fi_id), &(pAuxInfo->fi_precedence));

}

ds::ErrorType CDECL IPFilterSpec::GetEspSpi 
(
  int* pEspSpi
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  return mpPSQoSSpecWrapper->GetEspSpi(pEspSpi);
  
} /* GetEspSpi() */

ds::ErrorType CDECL IPFilterSpec::GetAhSpi 
(
  int* pAhSpi
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  return mpPSQoSSpecWrapper->GetAhSpi(pAhSpi);
  
} /* GetAhSpi() */


ds::ErrorType CDECL IPFilterSpec::GetICMPType 
(
  unsigned char* ICMPType
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetICMPType(ICMPType);
  
} /* GetICMPType() */



ds::ErrorType CDECL IPFilterSpec::GetICMPCode
(
  unsigned char* ICMPCode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->GetICMPCode(ICMPCode);
  
} /* GetICMPCode() */



ds::ErrorType CDECL IPFilterSpec::SetEspSpi 
(
 int espSpi
)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetEspSpi(espSpi);
  
} /* SetEspSpi() */

ds::ErrorType CDECL IPFilterSpec::SetAhSpi 
(
  int ahSpi
)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetAhSpi(ahSpi);
  
} /* SetAhSpi() */


ds::ErrorType CDECL IPFilterSpec::SetICMPType
( 
  unsigned char ICMPType
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  return mpPSQoSSpecWrapper->SetICMPType(ICMPType);
  
} /* SetICMPType() */



ds::ErrorType CDECL IPFilterSpec::SetICMPCode
( 
  unsigned char ICMPCode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return mpPSQoSSpecWrapper->SetICMPCode(ICMPCode);
  
} /* SetICMPCode() */




ds::ErrorType IPFilterSpec::GetPSIPFilterSpec
(
  NetPlatform::PSFilterSpecType *pPSIPFilterSpec
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pPSIPFilterSpec)
  {
    return QDS_EFAULT;
  }

  return mpPSQoSSpecWrapper->GetPSIPFilterSpec(pPSIPFilterSpec);

} /* GetPSIPFilterSpec() */

ds::ErrorType IPFilterSpec::UpdateSpec
(
  NetPlatform::PSFilterSpecType *pPSIPFilterSpec
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pPSIPFilterSpec)
  {
    return QDS_EFAULT;
  }
  
  return mpPSQoSSpecWrapper->UpdatePSIPFilterSpec(pPSIPFilterSpec);

} /* UpdateSpec() */

ds::ErrorType IPFilterSpec::QueryInterface
(
  AEEIID iid,
  void **ppo
)
{
  int result = AEE_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_4  ("IPFilterSpec::QueryInterface(): "
                    "Obj 0x%p, ref cnt %d, iid 0x%x , ppo 0x%x",
                     this, refCnt, iid, ppo);

  if (NULL == ppo)
  {
    return QDS_EFAULT;
  }

  *ppo = NULL;

  switch (iid)
  {
    case AEEIID_IIPFilterPriv:
      *ppo = static_cast <IIPFilterPriv *>(this);
      (void) AddRef(); 
      break;

    case AEEIID_IQI:
      *ppo = reinterpret_cast <IQI *> (this);
      (void) AddRef();
      break;

    default:
      return AEE_ECLASSNOTSUPPORT;
   }

   return result;
}
