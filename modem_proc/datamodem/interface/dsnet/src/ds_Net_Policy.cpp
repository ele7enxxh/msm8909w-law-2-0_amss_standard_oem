/*===========================================================================
  FILE: Policy.cpp

  OVERVIEW: This file provides implementation of the Policy class.

  DEPENDENCIES: None

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_Policy.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-09-30 kr  Free floating changes
  2010-03-31 ss  Added appType get/set functions.
  2008-03-10 hm Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "comdef.h"

#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Net_Handle.h"
#include "ds_Net_Policy.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Platform.h"
#include "ds_Utils_CCritSect.h"
#include <stringl/stringl.h>

using namespace ds::Net;
using namespace ds::Error;

/*---------------------------------------------------------------------------
  CONSTRUCTOR/DESTRUCTOR
---------------------------------------------------------------------------*/
Policy::Policy
(
  void
)
: refCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("Policy::Policy(): "
                   "Creating object 0x%p", this);

  //TODO: What should we use for default value? GRP_ANY_DEFAULT is defined only for the group in IDL.
  //  ifaceName     = ds::Net::Type::DEFAULT_NET_TYPE;

  ipFamily    = ds::AddrFamily::QDS_AF_INET;
  isRouteable = FALSE;
  policyFlag  = PolicyFlag::QDS_ANY;
  ifaceName   = 0;
  ifaceGroup  = IfaceGroup::GRP_ANY_DEFAULT;
  ifaceId     = 0;
  umtsProfileNum = 0;
  cdmaProfileNum = 0;
  umtsAPNName = NULL;
  umtsAPNNameLen = 0;
  appType = 0;
  subsId = 0;
  apnType = 0;

  if (AEE_SUCCESS != DS_Utils_CreateInstance (NULL,
                                              AEECLSID_CCritSect,
                                              (void **) &mpICritSect))
  {
    DATA_ERR_FATAL("Policy::Policy(): Cannot create crit sect");
  }

}

Policy::~Policy
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("Policy::~Policy(): "
                   "Deleting object 0x%p", this);


  if (NULL != umtsAPNName)
  {
    PS_SYSTEM_HEAP_MEM_FREE(umtsAPNName);
  }

  /*lint -save -e1550, -e1551 */
  DS_UTILS_RELEASEIF(mpICritSect);
  /*lint -restore */

}

/*---------------------------------------------------------------------------
  Public function definitions.
---------------------------------------------------------------------------*/
int Policy::GetPolicyFlag
(
  PolicyFlagType* argPolicyFlag
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == argPolicyFlag)
  {
    LOG_MSG_ERROR_0 ("Policy::GetPolicyFlag(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *argPolicyFlag = policyFlag;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}

int Policy::SetPolicyFlag
(
  PolicyFlagType argPolicyFlag
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2  ("Policy::SetPolicyFlag(): "
                             "Obj 0x%p, flag %d", this, argPolicyFlag);

  if (PolicyFlag::QDS_UP_PREFERRED != argPolicyFlag &&
      PolicyFlag::QDS_UP_ONLY       != argPolicyFlag &&
      PolicyFlag::QDS_ANY           != argPolicyFlag)
  {
    LOG_MSG_ERROR_1 ("Policy::SetPolicyFlag(): "
                     "Invalid policy flag %d", (int)argPolicyFlag);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  policyFlag = argPolicyFlag;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}

int Policy::GetAddressFamily
(
  ::ds::AddrFamilyType* argAddressFamily
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == argAddressFamily)
  {
    LOG_MSG_ERROR_0 ("Policy::GetAddressFamily(): "
                     "NULL args");
    return QDS_EFAULT;
  }
  
  mpICritSect->Enter();
  *argAddressFamily = ipFamily;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}

int Policy::SetAddressFamily
(
  ::ds::AddrFamilyType argAddressFamily
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Policy::SetAddressFamily(): "
                           "Obj 0x%p, family %d", this, argAddressFamily);

  if (ds::AddrFamily::QDS_AF_INET   != argAddressFamily &&
      ds::AddrFamily::QDS_AF_INET6  != argAddressFamily &&
      ds::AddrFamily::QDS_AF_UNSPEC != argAddressFamily)
  {
    LOG_MSG_ERROR_1 ("Policy::SetAddressFamily(): "
                     "Invalid argument to set addr family %d",
                     (int)argAddressFamily);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  ipFamily = argAddressFamily;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}

int Policy::GetRouteable
(
  boolean* argRouteable
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == argRouteable)
  {
    LOG_MSG_ERROR_0 ("Policy::GetRouteable(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *argRouteable = isRouteable;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}
int Policy::SetRouteable
(
  boolean argRouteable
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Policy::SetRouteable(): "
                           "Obj 0x%p, routeable %d", this, argRouteable);

  mpICritSect->Enter();
  isRouteable = argRouteable;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}

int Policy::GetIfaceId
(
  IfaceIdType*  argIfaceId
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == argIfaceId)
  {
    LOG_MSG_ERROR_0 ("Policy::GetIfaceId(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *argIfaceId = ifaceId;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}

int Policy::SetIfaceId
(
  IfaceIdType  argIfaceId
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Policy::SetIfaceId(): "
                           "Obj 0x%p, iface id 0x%x", this, argIfaceId);

  //TODO: Validate iface id?

  mpICritSect->Enter();
  ifaceId = argIfaceId;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}


int Policy::GetIfaceName
(
  ds::Net::IfaceNameType* argIfaceName
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == argIfaceName)
  {
    LOG_MSG_ERROR_0 ("Policy::GetIfaceName(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *argIfaceName = ifaceName;
  mpICritSect->Leave();


  return AEE_SUCCESS;

} /* GetIfaceName() */

int Policy::SetIfaceName
(
  ds::Net::IfaceNameType argIfaceName
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Policy::SetIfaceName(): "
                           "Obj 0x%p, if name 0x%x", this, argIfaceName);

  mpICritSect->Enter();
  ifaceName = argIfaceName;
  mpICritSect->Leave();
  return AEE_SUCCESS;

} /* SetIfaceName() */


int Policy::GetIfaceGroup
(
  IfaceGroupType* argIfaceGroup
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == argIfaceGroup)
  {
    LOG_MSG_ERROR_0 ("Policy::GetIfaceGroup(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *argIfaceGroup = ifaceGroup;
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* GetIfaceGroup() */


int Policy::SetIfaceGroup
(
  IfaceGroupType argIfaceGroup
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Policy::SetIfaceGroup(): "
                           "Obj 0x%p, iface grp 0x%x", this, argIfaceGroup);

  mpICritSect->Enter();
  ifaceGroup = argIfaceGroup;
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* SetIfaceGroup() */

ds::ErrorType Policy::GetCDMAProfileNumber
(
  int* argCDMAProfileNumber
)
{
  if (NULL == argCDMAProfileNumber)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *argCDMAProfileNumber = cdmaProfileNum;
  mpICritSect->Leave();


  return AEE_SUCCESS;

} /* SetCDMAProfileNumber() */

ds::ErrorType Policy::SetCDMAProfileNumber
(
  int argCDMAProfileNumber
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Policy::SetCDMAProfileNumber(): "
                           "Obj 0x%p, CDMA prof num %d",
                           this, argCDMAProfileNumber);

  mpICritSect->Enter();
  cdmaProfileNum = argCDMAProfileNumber;
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* SetCDMAProfileNumber() */

ds::ErrorType Policy::GetUMTSProfileNumber
(
  int* argUMTSProfileNumber
)
{
  if (NULL == argUMTSProfileNumber)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *argUMTSProfileNumber = umtsProfileNum;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}

ds::ErrorType Policy::SetUMTSProfileNumber
(
  int argUMTSProfileNumber
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Policy::SetUMTSProfileNumber(): "
                           "Obj 0x%p, UMTS prof num %d",
                           this, argUMTSProfileNumber);

  mpICritSect->Enter();
  umtsProfileNum = argUMTSProfileNumber;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}

ds::ErrorType Policy::QueryInterface
(
  AEEIID iid,
  void **ppo
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_3  ("Policy::QueryInterface(): "
                    "Obj 0x%p, ref cnt %d, iid 0x%x", this, refCnt, iid);
  if (NULL == ppo)
  {
    LOG_MSG_ERROR_0 ("Policy::QueryInterface(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  switch (iid)
  {
    case AEEIID_IPolicy:
      *ppo = reinterpret_cast <IPolicy *> (this);
      (void) AddRef ();
      break;

    case AEEIID_IPolicyPriv:
      *ppo = static_cast <IPolicyPriv *> (this);
      (void) AddRef ();
      break;

    case AEEIID_IQI:
      *ppo = reinterpret_cast <IQI *> (this);
      (void) AddRef ();
      break;

    default:
      return AEE_ECLASSNOTSUPPORT;
  }

  return AEE_SUCCESS;
}

ds::ErrorType Policy::GetUMTSAPNName
(
  char *  pName,
  int     len,
  int*    pLenReq
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  if (pLenReq != NULL)
  {
    *pLenReq = umtsAPNNameLen;
  }

  if (NULL == pName && 0 != len)
  {
    mpICritSect->Leave();
    return QDS_EFAULT;
  }

  if (NULL == umtsAPNName)
  {
    pName   = NULL;
    len     = 0;
    pLenReq = 0;
    mpICritSect->Leave();
    return AEE_SUCCESS;
  }

  (void) strlcpy (pName,
                      (const char *)umtsAPNName,
                      len);
  mpICritSect->Leave();

  return AEE_SUCCESS;

} /* GetUMTSAPNName() */


ds::ErrorType Policy::SetUMTSAPNName
(
  const char *  pName
)
{
  int len = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("Policy::SetUMTSAPNName(): "
                            "Obj 0x%p, apn name 0x%p", this, pName);

  len = strlen(pName);

  /*-------------------------------------------------------------------------
    Max supported domain name is 255
  -------------------------------------------------------------------------*/
  if (len > 255 || len < 0)
  {
    LOG_MSG_INVALID_INPUT_2("Policy::SetUMTSAPNName(): "
                            "Incorrect apn len %d, obj 0x%p", len, this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  if ( 0 != len )
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(umtsAPNName, len + 1, char*);
    if (NULL == umtsAPNName)
    {
      mpICritSect->Leave();
      return AEE_ENOMEMORY;
    }

    (void) strlcpy (umtsAPNName, pName, len + 1);
    umtsAPNNameLen = len;
  }
  mpICritSect->Leave();

  LOG_MSG_FUNCTION_EXIT_3("Policy::SetUMTSAPNName(): "
                          "Success Obj 0x%p, apn len %d, apn name %s",
                          this, umtsAPNNameLen, umtsAPNName);

  return AEE_SUCCESS;

} /* SetUMTSAPNName() */

ds::ErrorType Policy::GetAppType
(
  int* AppType
)
{
  if (NULL == AppType)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *AppType = appType;
  mpICritSect->Leave();

  return AEE_SUCCESS;
} /* GetAppType() */

ds::ErrorType Policy::SetAppType
(
  int AppType
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Policy::SetAppType(): "
                           "Obj 0x%p, appType %d", this, AppType);
  mpICritSect->Enter();
  appType = AppType;
  mpICritSect->Leave();

  return AEE_SUCCESS;
} /* SetAppType() */

int Policy::GetSubsId
(
  SubsInfoType*  argSubsId
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == argSubsId)
  {
    LOG_MSG_ERROR_0 ("Policy::GetSubsId(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *argSubsId = subsId;
  mpICritSect->Leave();


  return AEE_SUCCESS;
} /* GetSubsId() */

int Policy::SetSubsId
(
  SubsInfoType  argSubsId
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Policy::SetSubsId(): "
                           "Obj 0x%p, subs ID %d", this, argSubsId);

  if (ds::Net::SubsInfo::TERTIARY_SUBS  < argSubsId)
  {
    LOG_MSG_ERROR_1 ("Policy::SetSubsId(): "
                     "Invalid argument to set subs id %d",
                     argSubsId);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  subsId = argSubsId;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}/* SetSubsId() */

int Policy::GetApnType
(
  ApnInfoType*  argApnType
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == argApnType)
  {
    LOG_MSG_ERROR_0 ("Policy::GetApnType(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *argApnType = apnType;
  mpICritSect->Leave();


  return AEE_SUCCESS;
} /* GetApnType() */

int Policy::SetApnType
(
  ApnInfoType  argApnType
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Policy::SetApnType(): "
                           "Obj 0x%p, apn type %d", this, argApnType);

  if (ds::Net::ApnInfo::IMS_APN  < argApnType)
  {
    LOG_MSG_ERROR_1 ("Policy::SetApnType(): "
                     "Invalid argument to set apn type %d",
                     argApnType);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  apnType = argApnType;
  mpICritSect->Leave();

  return AEE_SUCCESS;
}/* SetApnType() */



