/*=========================================================================*/
/*!
  @file
  BearerTechEx.cpp

  @brief
  This file provides implementation of the ds::Net::BearerTechEx class.

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_BearerTechEx.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-02-13 da  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Net_BearerTechEx.h"
#include "ds_Net_Utils.h"
#include "ds_Utils_CCritSect.h"
#include <stringl/stringl.h>

using namespace ds::Net;
using namespace ds::Error;

/*===========================================================================

                     PRIVATE FUNCTION DEFINITIONS

===========================================================================*/
ds::ErrorType BearerTechEx::GetParams
(
  ParamsType paramName,
  void *     pOut,
  void *     pIn,
  int        len
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pOut)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  memscpy (pOut, len, pIn, len);
  mpICritSect->Leave();

  LOG_MSG_INFO1_3 ("BearerTechEx::GetParams(): "
                   "Obj 0x%p, param %d, val %d",
                   this, paramName, *((int *)pOut));

  return AEE_SUCCESS;

} /* GetParams() */

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  CONSTRUCTOR/DESTRUCTOR
---------------------------------------------------------------------------*/
BearerTechEx::BearerTechEx
(
  uint32   technology,
  uint32   rat,
  uint32   soMask
)
: mTechnology(technology),
  mRatValue(rat),
  mSoMask(soMask),
  mpICritSect(0),
  refCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("BearerTechEx::GetParams(): "
                   "Obj 0x%p", this);

  if (AEE_SUCCESS != DS_Utils_CreateInstance (NULL,
                                              AEECLSID_CCritSect,
                                              (void **) &mpICritSect))
  {
    DATA_ERR_FATAL("BearerTechEx::GetParams(): Cannot create crit sect");
  }

} /* BearerTechEx() */

BearerTechEx::~BearerTechEx
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -save -e1550, -e1551 */
  DS_UTILS_RELEASEIF(mpICritSect);
  /*lint -restore */

} /* ~BearerTechEx() */

/*---------------------------------------------------------------------------
  Functions inherited from IBearerInfo
---------------------------------------------------------------------------*/

AEEResult BearerTechEx::GetTechnology(unsigned int* value)
{
  return GetParams(DS_NET_BEARER_TECH_EX_PARAMS_NETWORK_TYPE,
                   value,
                   &mTechnology,
                   sizeof(mTechnology));
}
         
AEEResult BearerTechEx::GetRatValue(unsigned int* value)
{
  return GetParams(DS_NET_BEARER_TECH_EX_PARAMS_RAT_VALUE,
    value,
    &mRatValue,
    sizeof(mRatValue));
}
         
AEEResult BearerTechEx::GetSoMask(unsigned int* value)
{
  return GetParams(DS_NET_BEARER_TECH_EX_PARAMS_SO_MASK,
    value,
    &mSoMask,
    sizeof(mSoMask));
}

