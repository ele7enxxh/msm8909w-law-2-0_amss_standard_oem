/*===========================================================================
  FILE: MTPDReg.cpp

  OVERVIEW: This file provides implementation of the MTPDReg class.

  DEPENDENCIES: None

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MTPDReg.cpp#1 $
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
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_TechUMTSFactory.h"
#include "ds_Net_MTPDReg.h"
#include "ds_Net_Platform.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_EventManager.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalBus.h"

using namespace ds::Net;
using namespace NetPlatform;

MTPDReg::MTPDReg
(
  int32 ifaceHandle,
  int32 mtpdHandle
)
throw()
: Handle (ifaceHandle),
  mMTPDHandle (mtpdHandle),
  mRegObj(NULL),
  refCnt(1),
  weakRefCnt(1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_3 ("MTPDReg::MTPDReg(): "
                   "Obj 0x%p, iface 0x%x, MTPD handle 0x%x",
                   this, ifaceHandle, mMTPDHandle);

  /* Create signal bus to hold event registration info */
  (void) DS_Utils_CreateInstance(0, AEECLSID_CSignalBus,
                                 (void **) &mpSigBusMTPD);

  if (AEE_SUCCESS != Handle::Init(EventManager::mtpdObjList))
  {
    LOG_MSG_ERROR_3 ("MTPDReg::MTPDReg "
                     "Failed to init the Handle   "
                     "obj 0x%p, if handle 0x%x, mMTPDHandle 0x%x",
                     this, ifaceHandle, mMTPDHandle);
    return;
  }

} /* MTPDReg() */

void MTPDReg::Destructor
(
  void
)
throw()
{
  MTDeRegCBType   mtDeRegInfo;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset (&mtDeRegInfo, 0, sizeof(mtDeRegInfo));
  mtDeRegInfo.handle = mMTPDHandle;

  (void) IfaceIoctl (GetHandle(),
                     IFACE_IOCTL_MT_DEREG_CB,
                     &mtDeRegInfo);

  DS_UTILS_RELEASEIF (mpSigBusMTPD);

  Handle::Destructor();

} /* MTPDReg::Destructor() */

MTPDReg::~MTPDReg
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mRegObj = NULL;

} /* ~MTPDReg() */


boolean MTPDReg::Process
(
  void* userDataPtr
)
{
  EventInfoType*       eventInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == userDataPtr)
  {
    LOG_MSG_ERROR_0 ("MTPDReg::Process(): "
                     "NULL ev info");
    return FALSE;
  }

  eventInfoPtr = (EventInfoType *)userDataPtr;
  if (eventInfoPtr->userHandle == mMTPDHandle)
  {
    return Handle::Process(userDataPtr);
  }

  return TRUE;

} /* Process() */

ds::ErrorType MTPDReg::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ppISigBus)
  {
    DATA_ERR_FATAL("MTPDReg::GetSignalBus(): NULL args");
    return AEE_EBADPARM;
  }

  mpICritSect->Enter();

  switch (eventID)
  {
    case  TechUMTSFactory::QDS_EV_MTPD:
      *ppISigBus = mpSigBusMTPD;
      (void)(*ppISigBus)->AddRef();
      mpICritSect->Leave();
      return AEE_SUCCESS;

    default:
      *ppISigBus = NULL;
      mpICritSect->Leave();
      return AEE_EBADPARM;
  }

} /* GetSignalBus() */

