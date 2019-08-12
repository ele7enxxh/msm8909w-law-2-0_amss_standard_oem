/*===========================================================================
  FILE: NetworkUMTS.cpp

  OVERVIEW: This file provides implementation of the NetworkUMTS class.

  DEPENDENCIES: None

  Copyright (c) 2007 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_NetworkUMTS.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-04-06 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_NetworkUMTS.h"
#include "ds_Net_Platform.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CCritSect.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace NetPlatform;

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
NetworkUMTS::NetworkUMTS
(
  Network* pParent
)
: refCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkUMTS::NetworkUMTS(): Obj 0x%p", this);


  /* Store the parent, perform AddRefWeak() */
  ASSERT (NULL != pParent);

  mpParent = pParent;

  if(NULL != mpParent)
  {
    (void) mpParent->AddRefWeak();
  }
  else
  {
    LOG_MSG_ERROR_0 ("NetworkUMTS::NetworkUMTS(): "
                     "mpParent is NULL");
  }

  if (AEE_SUCCESS != DS_Utils_CreateInstance (NULL, 
                                              AEECLSID_CCritSect,
                                             (void **) &mpICritSect))
  {
    DATA_ERR_FATAL("NetworkUMTS::NetworkUMTS(): Cannot create critical section");
  }

} /* NetworkUMTS::NetworkUMTS() */

NetworkUMTS::~NetworkUMTS
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkUMTS::~NetworkUMTS(): Obj 0x%p", this);

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

  /*lint -save -e1550, -e1551 */
  DS_UTILS_RELEASEIF(mpICritSect);
  DS_UTILS_RELEASE_WEAKREF_IF(mpParent);
  /*lint -restore */

} /* ~NetworkUMTS() */


/*---------------------------------------------------------------------------
  Functions inherited from INetworkUMTS
---------------------------------------------------------------------------*/
ds::ErrorType NetworkUMTS::GetIMCNFlag
(
  UMTSIMCNFlagType* imcnFlag
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (mpParent->GetHandle(),
                               IFACE_IOCTL_UMTS_GET_IM_CN_FLAG,
                                 static_cast <void *> (imcnFlag));

  LOG_MSG_FUNCTION_EXIT_2 ("NetworkUMTS::GetIMCNFlag(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();

  return result;

} /* NetworkUMTS::GetIMCNFlag() */

ds::ErrorType NetworkUMTS::QueryInterface
(
  AEEIID iid,
  void **ppo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("NetworkUMTS::QueryInterface(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (iid)
  {
  case AEEIID_INetwork:
    case AEEIID_IQI:
    {
    *ppo = static_cast <INetwork *> (mpParent);
      result = AEE_SUCCESS;

    (void) mpParent->AddRef();
    break;
    }

  case AEEIID_INetworkUMTS:
    {
    *ppo = static_cast <INetworkUMTS *>(this);
      result = AEE_SUCCESS;

    (void) AddRef();
    break;
    }

  default:
    {
      result = AEE_ECLASSNOTSUPPORT;
      break;
  }
  }

  LOG_MSG_FUNCTION_EXIT_4 ("NetworkUMTS::QueryInterface(): "
                           "Result 0x%x, ppo 0x%p, obj 0x%p, iid 0x%x",
                           result, *ppo, this, iid);

  mpICritSect->Leave();

  return result;
} /* NetworkUMTS::QueryInterface() */
