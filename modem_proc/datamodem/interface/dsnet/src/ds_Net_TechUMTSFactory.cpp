/*===========================================================================
  FILE: TechUMTSFactory.cpp

  OVERVIEW: This file provides implementation of the TechUMTSFactory class.

  DEPENDENCIES: None

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_TechUMTSFactory.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-10-23 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Net_Utils.h"
#include "ds_Net_TechUMTSFactory.h"
#include "ds_Net_Platform.h"
#include "ds_Net_MTPDReg.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_Policy.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CCritSect.h"


using namespace ds::Net;
using namespace ds::Error;
using namespace NetPlatform;

TechUMTSFactory::TechUMTSFactory
(
  void
)
throw()
: refCnt (1)
{

  LOG_MSG_INFO1_1 ("TechUMTSFactory::TechUMTSFactory(): "
                   "Creating object 0x%p", this);
  if (AEE_SUCCESS != DS_Utils_CreateInstance (NULL, 
                                              AEECLSID_CCritSect,
                                              (void **) &mpICritSect))
  {
    DATA_ERR_FATAL("TechUMTSFactory::TechUMTSFactory(): Cannot create crit sect");
  }

} /* TechUMTSFactory() */

TechUMTSFactory::~TechUMTSFactory
(
  void 
)
throw()
{

  LOG_MSG_INFO1_1 ("TechUMTSFactory::~TechUMTSFactory(): "
                   "Deleting object 0x%p", this);

  /*lint -save -e1550, -e1551 */
  DS_UTILS_RELEASEIF(mpICritSect);
  /*lint -restore */

} /* ~TechUMTSFactory() */

/*---------------------------------------------------------------------------
  Public Function Definitions
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
  Functions inherited from ITechUMTS interface
-------------------------------------------------------------------------*/
ds::ErrorType TechUMTSFactory::RegMTPD
(
  IPolicy* argPolicy, 
  ISignal* signalObj, 
  IQI**    mtpdRegObj
)
{
  MTPDReg*            pMTPDReg = NULL;
  int32               ifaceHandle; 
  int                 result;
  MTRegCBType         mtRegInfo;
  ACLPolicyInfoType   aclPolicy;
  IQI*                tmpRegObj;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1  ("TechUMTSFactory::RegMTPD(): "
                             "Obj 0x%p", this);

  if (NULL == argPolicy || NULL == signalObj || NULL == mtpdRegObj)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  /*-------------------------------------------------------------------------
    Perform a network lookup before issuing the IOCLTL.
  -------------------------------------------------------------------------*/
  result = IfaceLookUpByPolicy (argPolicy, &ifaceHandle);
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Get the ACL policy for the given IPolicy.
  -------------------------------------------------------------------------*/
  result = NetPlatform::GenerateAclPolicy(argPolicy, &aclPolicy);
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  memset (&mtRegInfo, 0, sizeof(mtRegInfo));
  mtRegInfo.acl_pol_ptr = &aclPolicy;
  mtRegInfo.handle = 0;

  result = IfaceIoctl (ifaceHandle, 
                       IFACE_IOCTL_MT_REG_CB,
                       &mtRegInfo);
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Create the MTPDReg object depending on the MT handle returned.
  -------------------------------------------------------------------------*/
  pMTPDReg = new MTPDReg(ifaceHandle, (int32)mtRegInfo.handle);
  if (NULL == pMTPDReg)
  {
    result = AEE_ENOMEMORY;
    goto bail;
  }

  *mtpdRegObj = reinterpret_cast <IQI *> (pMTPDReg);

  /*-------------------------------------------------------------------------
    Register for MT events on the object created.
  -------------------------------------------------------------------------*/
  
  tmpRegObj = pMTPDReg->GetRegObj();

  result = pMTPDReg->OnStateChange (signalObj, QDS_EV_MTPD, &tmpRegObj);
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  pMTPDReg->SetRegObj(tmpRegObj);
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  mpICritSect->Leave();
  if (NULL != pMTPDReg)
  {
    (void) pMTPDReg->Release();
    pMTPDReg = NULL;
    *mtpdRegObj = NULL;
  }
  LOG_MSG_ERROR_1 ("TechUMTSFactory::RegMTPD(): "
                   "Err %d reg for MTPD", result);
  return result;

} /* RegMTPD() */
