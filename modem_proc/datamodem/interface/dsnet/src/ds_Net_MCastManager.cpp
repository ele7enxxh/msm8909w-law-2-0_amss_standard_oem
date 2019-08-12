/*===========================================================================
  FILE: MCastManager.cpp

  OVERVIEW: This file provides implementation of the MCastManager class.

  DEPENDENCIES: None

  Copyright (c) 2007-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MCastManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-09-30 kr  Free floating changes
  2008-12-22 hm  Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATUTE_DATA_PS_MCAST
#include "comdef.h"
#include <stringl/stringl.h>
#include "ds_Utils_StdErr.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MCastManager.h"
#include "ds_Net_MCastManagerPriv.h"
#include "ds_Net_MCastSession.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Conversion.h"
#include "ds_Utils_CCritSect.h"
#include "ds_Net_MBMSSpec.h"
#include "ds_Net_MCastMBMSCtrl.h"
#include "ds_Net_MCastSessionsOutput.h"
#include "ds_Net_MCastSessionsInput.h"
#include "ps_system_heap.h"

using namespace ds::Error;
using namespace ds::Net;
using namespace ds::Net::Conversion;
using namespace NetPlatform;


MCastManager::MCastManager
(
  int32            ifaceHandle,
  NetworkModeType  networkMode
)
: mIfaceHandle (ifaceHandle),
  mpMCastManagerPriv(0),
  mNetworkMode(networkMode),
  refCnt(1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("MCastManager::MCastManager(): "
                            "Obj 0x%p, iface handle 0x%x, mode %d",
                            this, ifaceHandle, networkMode);

  //TODO Move out to Init function. Go through constructors of all dsnet/dss
  //objects and fix this
  if (AEE_SUCCESS != DS_Utils_CreateInstance (0,
                                              AEECLSID_CCritSect,
                                              (void **) &mpICritSect))
  {
    LOG_MSG_ERROR_1 ("MCastManager::MCastManager(): "
                     "Failed to alloc CritSect, obj 0x%p", this);
    return;
  }
} /* MCastManager::MCastManager() */

MCastManager::~MCastManager
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mIfaceHandle = 0;

  DS_UTILS_RELEASEIF(mpMCastManagerPriv);

  /*lint -save -e1550, -e1551 */
  DS_UTILS_RELEASEIF(mpICritSect);
  /*lint -restore */

} /* MCastManager::~MCastManager() */


//TODO Need to undo IFACE_IOCTL_MCAST_JOIN in case of error in below function
ds::ErrorType MCastManager::Join
(
  const ::ds::SockAddrStorageType addr,
  const char *                    mcastSpec,
  IMCastSession **                session,
  char *                          errSpec,
  int                             errSpecLen,
  int *                           errSpecLenReq
)
{
  MCastJoinType      mcastJoinInfo;
  ds::ErrorType      result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == session || 0 == addr)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::Join(): NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    JSON related arguments are for the future use
  -------------------------------------------------------------------------*/
  (void) mcastSpec;
  (void) errSpec;
  (void) errSpecLen;
  (void) errSpecLenReq;

  /*-------------------------------------------------------------------------
    Validate MCast join info and create IOCTL arguments to perform
    iface level IOCTL to join the multicast group.
  -------------------------------------------------------------------------*/
  memset (&mcastJoinInfo, 0, sizeof (MCastJoinType));
  result = DS2PSMCastJoinSpec (addr, &mcastJoinInfo);
  if (AEE_SUCCESS != result)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::Join(): Failed to convert "
                             "MCast spec from DS to PS format, obj 0x%p",
                             this);
    return result;
  }

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_ERROR_2 ("MCastManager::Join(): Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Perform Mcast join IOCTL on the iface handle to join the multicast group.
      Create a MCastSession object for the session after the IOCTL is issued.
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (mIfaceHandle,
                         IFACE_IOCTL_MCAST_JOIN,
                         static_cast <void *> (&mcastJoinInfo));
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("MCastManager::Join(): "
                       "IFACE_IOCTL_MCAST_JOIN failed, obj 0x%p", this);
      break;
    }

    /*-----------------------------------------------------------------------
      Use the Mcast handle obtained from the IOCTL to create a MCast
      session object.
    -----------------------------------------------------------------------*/
    *session = new MCastSession
               (
                 mIfaceHandle,
                 mcastJoinInfo.handle,
                 MCastSessionCreation::MCAST_SESSION_CREATED_BY_JOIN,
                 MCastJoinFlags::REG_SETUP_ALLOWED
               );
    if (0 == *session)
    {
      LOG_MSG_ERROR_1 ("MCastManager::Join(): "
                       "Failed to alloc MCastSession, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

  } while (0);

  LOG_MSG_FUNCTION_EXIT_3( "MCastManager::Join(): "
                           "Result 0x%x, MCast session 0x%p, obj 0x%p",
                           result, *session, this);
  mpICritSect->Leave();

  return result;
} /* MCastManager::Join() */


ds::ErrorType MCastManager::LeaveBundle
(
  IMCastSessionsInput * sessions
)
{
  MCastSession *        mcastSessionObj = 0;
  MCastSessionsInput *  sessionsInput;
  MCastLeaveExType      leaveExInfo;
  ds::ErrorType         result;
  int32                 index;
  int32                 sessionsLen;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == sessions)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::LeaveBundle(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("MCastManager::LeaveBundle(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Construct arguments for MCAST_LEAVE_EX IOCTL
    -----------------------------------------------------------------------*/
    sessionsInput = static_cast <MCastSessionsInput *> (sessions);

    result = sessionsInput->GetNumOfSessions(&sessionsLen);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("MCastManager::LeaveBundle(): "
                       "Failed to get # of sessions, obj 0x%p", this);
      break;
    }

    memset (&leaveExInfo, 0, sizeof (MCastLeaveExType));
    leaveExInfo.num_flows = (uint8) sessionsLen;

    for (index = 0; index < sessionsLen; index++)
    {
      /*---------------------------------------------------------------------
        Get MCast session and its handle. Release the session afterwards as
        it is no longer needed
      ---------------------------------------------------------------------*/
      result =
        sessionsInput->GetNth(index,
                              (IMCastSession **)(&mcastSessionObj));
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_INFO3_1 ("MCastManager::Join(): "
                         "Failed to get nth MCast session, obj 0x%p", this);
        break;
      }

      leaveExInfo.handle[index] = mcastSessionObj->GetMCastHandle();
      DS_UTILS_RELEASEIF( mcastSessionObj);
    }

    /*-----------------------------------------------------------------------
      Call bundled leave IOCTL on underlying iface to leave the mcast group
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (mIfaceHandle,
                         IFACE_IOCTL_MCAST_LEAVE_EX,
                         static_cast <void *> (&leaveExInfo));
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2( "MCastManager::LeaveBundle(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* MCastManager::LeaveBundle() */


//TODO Need to undo IFACE_IOCTL_MCAST_REGISTER_EX in case of error in below function
ds::ErrorType MCastManager::RegisterBundle
(
  IMCastSessionsInput *  sessions
)
{
  MCastSession *            mcastSessionObj = 0;
  MCastSessionsInput *      sessionsInput = 0;
  MCastRegisterExType       regExInfo;
  ds::ErrorType             result;
  int32                     index;
  int32                     sessionsLen;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == sessions)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::RegisterBundle(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_ERROR_2 ("MCastManager::RegisterBundle(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Construct MCAST_REG_EX IOCTL arguments.
      Register can be called only on sessions group created from
      call to JoinBundle with all regFlags == REG_SETUP_NOT_ALLOWED
    -----------------------------------------------------------------------*/
    sessionsInput = static_cast <MCastSessionsInput *> (sessions);

    result = sessionsInput->GetNumOfSessions(&sessionsLen);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("MCastManager::LeaveBundle(): "
                       "Failed to get # of sessions, obj 0x%p", this);
      break;
    }

    memset (&regExInfo, 0, sizeof (MCastRegisterExType));
    regExInfo.num_flows = (uint8) sessionsLen;

    for (index = 0; index < sessionsLen; index++)
    {
      /*---------------------------------------------------------------------
        Get MCast session and its handle. Release the session afterwards as
        it is no longer needed
      ---------------------------------------------------------------------*/
      result =
        sessionsInput->GetNth(index,
                              (IMCastSession **)(&mcastSessionObj));
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_INFO3_1 ("MCastManager::RegisterBundle(): "
                         "Failed to get nth MCast session, obj 0x%p", this);
        break;
      }

      if (MCastJoinFlags::REG_SETUP_NOT_ALLOWED !=
            mcastSessionObj->GetRegFlag())
      {
        LOG_MSG_INVALID_INPUT_2 ("MCastManager::RegisterBundle(): "
                                 "Registration is not allowed on "
                                 "MCast session obj 0x%p, obj 0x%p",
                                 mcastSessionObj, this);
        result = QDS_EINVAL;
        break;
      }

      regExInfo.handle[index] = mcastSessionObj->GetMCastHandle();
      DS_UTILS_RELEASEIF( mcastSessionObj);
    }

    /*-----------------------------------------------------------------------
      Call bundled REG_EX IOCTL on underlying iface.
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (mIfaceHandle,
                         IFACE_IOCTL_MCAST_REGISTER_EX,
                         static_cast <void *> (&regExInfo));
  } while (0);

  /*-------------------------------------------------------------------------
    Release mcastSessionObj in case of an error. For example, if a session
    object had MCastJoinFlags::REG_SETUP_NOT_ALLOWED, then code will reach here
    without calling Release().

    If there was no error, calling DS_UTILS_RELEASEIF will not cause any harm
    as mcastSessionObj will have been set to 0 previously.
  -------------------------------------------------------------------------*/
  DS_UTILS_RELEASEIF( mcastSessionObj);

  LOG_MSG_FUNCTION_EXIT_2( "MCastManager::RegisterBundle(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* MCastManager::RegisterBundle() */


//TODO Need to undo IFACE_IOCTL_MCAST_JOIN_EX in case of error in below function
ds::ErrorType CDECL MCastManager::JoinBundle
(
  const ::ds::SockAddrStorageType * addrSeq,
  int                               addrSeqLen,
  const char *                      mcastSpecs,
  IMCastSessionsOutput **           sessions,
  const MCastJoinFlagsType *        mcastJoinFlagsSeq,
  int                               mcastJoinFlagsSeqLen,
  char *                            errSpec,
  int                               errSpecLen,
  int *                             errSpecLenReq
)
{
  MCastSession **             mcastSessionArr = 0;
  MCastSessionsOutput *        mcastSessionOutputPtr = 0;
  MCastJoinExType             mcastJoinExInfo;
  ds::ErrorType               result;
  uint32                       index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == addrSeq || 0 == sessions || 0 == mcastJoinFlagsSeq)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::JoinBundle(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  if (0 >= addrSeqLen || addrSeqLen != mcastJoinFlagsSeqLen)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::JoinBundle(): Invalid arg, obj 0x%p",
                             this);
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    JSON related arguments are for the future use
  -------------------------------------------------------------------------*/
  (void) mcastSpecs;
  (void) errSpec;
  (void) errSpecLen;
  (void) errSpecLenReq;

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("MCastManager::JoinBundle(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Construct JOIN_EX IOCTL args.
    -----------------------------------------------------------------------*/
    memset (&mcastJoinExInfo, 0, sizeof (MCastJoinExType));
    result = DS2PSMCastJoinExSpec (addrSeq,
                                   addrSeqLen,
                                   &mcastJoinExInfo,
                                   mcastJoinFlagsSeq);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INVALID_INPUT_1 ("MCastManager::JoinBundle(): Failed to convert "
                               "MCast spec from DS to PS format, obj 0x%p",
                               this);
      break;
    }

    /*-----------------------------------------------------------------------
      Issue a MCAST_JOIN_EX iface IOCTL to request for a bundle MCAST join
      IOCTLS. Create corresponding Mcast session objects (MCastSession
      objects) associated with these MCast Handles.
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (mIfaceHandle,
                         IFACE_IOCTL_MCAST_JOIN_EX,
                         static_cast <void *> (&mcastJoinExInfo));
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("MCastManager::JoinBundle(): "
                       "IFACE_IOCTL_MCAST_JOIN_EX failed, obj 0x%p", this);
      break;
    }

    /*-----------------------------------------------------------------------
      Allocate memory for the array of MCast Session objects pointers:
    -----------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(
      mcastSessionArr,
      sizeof (MCastSession *) * mcastJoinExInfo.num_flows,
      MCastSession**);
    if (0 == mcastSessionArr)
    {
      result = AEE_ENOMEMORY;
      break;
    }

    memset (mcastSessionArr,
            0,
            sizeof (MCastSession *) * mcastJoinExInfo.num_flows);

    /*-----------------------------------------------------------------------
      Create MCastSession objects for all of these MCast handles.
    -----------------------------------------------------------------------*/
    for (index = 0; index < mcastJoinExInfo.num_flows; index++)
    {
      mcastSessionArr[index] =
        new MCastSession
        (
          mIfaceHandle,
          mcastJoinExInfo.handle[index],
          MCastSessionCreation::MCAST_SESSION_CREATED_BY_JOIN_BUNDLE,
          mcastJoinFlagsSeq[index]
        );
      if (0 == mcastSessionArr[index])
      {
        LOG_MSG_ERROR_1 ("MCastManager::JoinBundle(): "
                         "Failed to alloc MCastSession, obj 0x%p", this);
        result = AEE_ENOMEMORY;
        break;
      }
    }

    /*-----------------------------------------------------------------------
      Create IMCastSessionOutput and populate it with retrieved sessions.
    -----------------------------------------------------------------------*/
    *sessions = mcastSessionOutputPtr = new MCastSessionsOutput();
    if (0 == mcastSessionOutputPtr)
    {
      LOG_MSG_ERROR_1 ("MCastManager::JoinBundle(): "
                       "Failed to alloc MCastSessionsOutput, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    for (index = 0; index < mcastJoinExInfo.num_flows; index++)
    {
      result = mcastSessionOutputPtr->AddMCastSession (mcastSessionArr[index]);
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_INFO3_1 ("MCastManager::JoinBundle(): "
                         "Failed to add MCastSession, obj 0x%p", this);
        break;
      }
    }
  } while (0);

  /*-------------------------------------------------------------------------
    Release array of MCastSession objects. Since they are added to
    MCastSessionsOutputobject, there is no need to maintain additional
    reference
  -------------------------------------------------------------------------*/
  if (0 != mcastSessionArr)
  {
    for (index = 0; index < mcastJoinExInfo.num_flows; index++)
    {
      /*---------------------------------------------------------------------
        MCastSessionsOutput object holds the reference to MCastSessions we
        created, so here we can release them.
      ---------------------------------------------------------------------*/
      DS_UTILS_RELEASEIF(mcastSessionArr[index]);
    }

    PS_SYSTEM_HEAP_MEM_FREE (mcastSessionArr);
  }

  /*-------------------------------------------------------------------------
    Free MCastSessionsOutputobject in case of failure
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result && 0 != mcastSessionOutputPtr)
  {
    DS_UTILS_RELEASEIF(mcastSessionOutputPtr);
  }

  LOG_MSG_FUNCTION_EXIT_2( "MCastManager::JoinBundle(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* MCastManager::JoinBundle() */

ds::ErrorType MCastManager::CreateMCastSessionsInput
(
  IMCastSessionsInput **  newMCastSessionsInput
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == newMCastSessionsInput)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::CreateMCastSessionsInput(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("MCastManager::CreateMCastSessionsInput(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    *newMCastSessionsInput =
      static_cast <IMCastSessionsInput *> (new MCastSessionsInput());
    if (0 == *newMCastSessionsInput)
    {
      LOG_MSG_ERROR_1 ("MCastManager::CreateMCastSessionsInput(): "
                       "Failed to alloc MCastSessionsInput, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    result = AEE_SUCCESS;
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2( "MCastManager::CreateMCastSessionsInput(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* MCastManager::CreateMCastSessionsInput() */

ds::ErrorType MCastManager::GetTechObject
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::GetTechObject(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (iid)
  {
    case AEEIID_IMCastManager:
    case AEEIID_IMCastManagerMBMSPriv:
    case AEEIID_IMCastManagerBCMCS:
    case AEEIID_IMCastManagerExt:
    case AEEIID_IQI:
    {
      result = QueryInterface(iid, ppo);
      break;
    }

    case AEEIID_IMCastManagerPriv:
    {
      /*---------------------------------------------------------------------
        Return cached MCastManagerPriv object, create and cache if not created
      ---------------------------------------------------------------------*/
      if (0 == mpMCastManagerPriv)
      {
        mpMCastManagerPriv = new MCastManagerPriv(mIfaceHandle);
        if (0 == mpMCastManagerPriv)
        {
          LOG_MSG_ERROR_1 ("MCastManager::GetTechObject(): "
                           "Failed to alloc MCastManagerPriv, obj 0x%p", this);
          result = AEE_ENOMEMORY;
          break;
        }
      }

      (void) mpMCastManagerPriv->AddRef();

      *ppo   = mpMCastManagerPriv;
      result = AEE_SUCCESS;

      break;
    }

    default:
    {
      result = AEE_ECLASSNOTSUPPORT;
      break;
    }
  }

  LOG_MSG_FUNCTION_EXIT_4( "MCastManager::CreateMCastSessionsInput(): "
                           "Result 0x%x, ppo 0x%x, obj 0x%p, iid 0x%x",
                           result, *ppo, this, iid);
  mpICritSect->Leave();

  return result;
} /* MCastManager::GetTechObject() */

ds::ErrorType MCastManager::QueryInterface
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::QueryInterface(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (iid)
  {
    case AEEIID_IMCastManager:
    case AEEIID_IQI:
    {
      *ppo   = static_cast <IMCastManager *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_IMCastManagerExt:
    {
      *ppo   = static_cast <IMCastManagerExt *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_IMCastManagerMBMSPriv:
    {
      *ppo   = static_cast <IMCastManagerMBMSPriv *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_IMCastManagerBCMCS:
    {
      *ppo   = static_cast <IMCastManagerBCMCS *> (this);
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

  LOG_MSG_FUNCTION_EXIT_4 ("MCastManager::QueryInterface(): "
                           "Result 0x%x, ppo 0x%p, obj 0x%p, iid 0x%x",
                           result, *ppo, this, iid);
  mpICritSect->Leave();

  return result;
} /* MCastManager::QueryInterface() */


/*---------------------------------------------------------------------------
  Inherited functions from IMCastManagerMBMSPriv.
---------------------------------------------------------------------------*/
//TODO Need to undo IFACE_IOCTL_MBMS_MCAST_CONTEXT_ACTIVATE in case of error in below function
ds::ErrorType MCastManager::Activate
(
  const ds::IPAddrType *  addr,
  int                     pdpNumber,
  IMCastMBMSCtrlPriv **   ppMCastMBMSCtrl
)
{
  /*lint -esym(429, pMCastMBMSCtrl) */
  MCastMBMSCtrl *      pMCastMBMSCtrl;
  MBMSContextActType   activateInfo;
  ds::ErrorType        result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == addr || 0 == ppMCastMBMSCtrl)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::Activate(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Construct the MBMS_CONTEXT_ACT IOCTL arg.
  -------------------------------------------------------------------------*/
  memset (&activateInfo, 0, sizeof(activateInfo));
  activateInfo.profile_id = pdpNumber;

  memscpy (&activateInfo.ip_addr, sizeof(ip_addr_type),
    addr, sizeof(ip_addr_type));

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("MCastManager::Activate(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Perform iface ioctl to activate MBMS multicast context.
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (mIfaceHandle,
                         IFACE_IOCTL_MBMS_MCAST_CONTEXT_ACTIVATE,
                         static_cast <void *> (&activateInfo));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Construct a MCastMBMSCtrl object and return as out-arg
    -----------------------------------------------------------------------*/
    pMCastMBMSCtrl = new MCastMBMSCtrl(mIfaceHandle, activateInfo.handle);
    if (0 == pMCastMBMSCtrl)
    {
      LOG_MSG_ERROR_1 ("MCastManager::Activate(): "
                       "Failed to alloc MCastMBMSCtrl, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    *ppMCastMBMSCtrl = static_cast <IMCastMBMSCtrlPriv *> (pMCastMBMSCtrl);
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("MCastManager::Activate(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* MCastManager::Activate() */

/*---------------------------------------------------------------------------
  Functions inherited from IMCastManagerBCMCS
---------------------------------------------------------------------------*/
ds::ErrorType CDECL MCastManager::UpdateDB
(
  const BCMCSDBSpecType *  updateParam
)
{
  NetPlatform::PSBCMCSDbUpdateType  ioctlArg;
  ds::ErrorType                     result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == updateParam)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::UpdateDB(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  memset(&ioctlArg, 0, sizeof(NetPlatform::PSBCMCSDbUpdateType));

  result = Conversion::DS2PSBCMCSSpec(updateParam, &ioctlArg);
  if (AEE_SUCCESS != result)
  {
    LOG_MSG_INFO3_1 ("MCastManager::UpdateDB(): Failed to convert "
                     "BCMCS DB spec from DS to PS format, obj 0x%p", this);
    return result;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Perform iface ioctl to update database for BCMCS.
  -------------------------------------------------------------------------*/
  result = IfaceIoctl(mIfaceHandle,
                      IFACE_IOCTL_BCMCS_DB_UPDATE,
                      static_cast <void *> (&ioctlArg));

  LOG_MSG_FUNCTION_EXIT_2 ("MCastManager::UpdateDB(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* MCastManager::UpdateDB() */

ds::ErrorType MCastManager::RegisterUsingHandoffOpt
(
  const ::ds::SockAddrStorageType *  addrSeq,
  int                                addrSeqLen
)
{
  MCastRegisterUsingHandoffOptType  mcastRegInfo;
  ds::ErrorType                     result;
  int                               i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == addrSeq)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::RegisterUsingHandoffOpt(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  if (0 > addrSeqLen)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastManager::RegisterUsingHandoffOpt(): "
                             "Invalid arg, obj 0x%p", this);
    return QDS_EINVAL;
  }

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("MCastManager::RegisterUsingHandoffOpt(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Validate MCast RegisterUsingHandoffOpt info and create IOCTL arguments
      to perform iface level IOCTL to register using handoff optimization.
    -----------------------------------------------------------------------*/
    memset (&mcastRegInfo, 0, sizeof (MCastRegisterUsingHandoffOptType));
    mcastRegInfo.num_mcast_addr = (uint8) addrSeqLen;

    for (i = 0; i < addrSeqLen; i++)
    {
      result = DS2PSMCastAddrInfo (addrSeq[i],
                                   &mcastRegInfo.mcast_addr_info[i]);
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_INFO3_1 ("MCastManager::RegisterUsingHandoffOpt(): Failed to "
                         "convert MCast addr from DS to PS format, obj 0x%p",
                         this);
        break;
      }
    }

    /*-----------------------------------------------------------------------
      Call bundled REGISTER_USING_HANDOFF IOCTL on underlying iface.
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (mIfaceHandle,
                         IFACE_IOCTL_BCMCS_REGISTER_USING_HANDOFF,
                         static_cast <void *> (&mcastRegInfo));
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("MCastManager::RegisterUsingHandoffOpt(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* MCastManager::RegisterUsingHandoffOpt() */
#endif // FEATUTE_DATA_PS_MCAST
