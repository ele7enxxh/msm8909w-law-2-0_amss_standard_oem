/*===========================================================================
  FILE: ds_Sock_RecvTTLInfo.cpp

  OVERVIEW: This file provides implementation of the RecvTTLInfo class.

  DEPENDENCIES: None

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_RecvTTLInfo.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2015-03-02 ad Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "ds_Utils_StdErr.h"

#include "ds_Sock_RecvTTLInfo.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"


using namespace ds::Sock;
using namespace ds::Error;


/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
void * RecvTTLInfo::operator new
(
  unsigned int numBytes
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  void * buf = NULL;
  PS_SYSTEM_HEAP_MEM_ALLOC(buf, numBytes, void *);
  return buf; 
   
} /* RecvTTLInfo::operator new() */

void RecvTTLInfo::operator delete
(
  void *  bufPtr
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  if (0 == bufPtr)
  {
    DATA_ERR_FATAL("delete(): NULL ptr");
    return;
  }
  
  PS_SYSTEM_HEAP_MEM_FREE(bufPtr);
  return;

} /* RecvTTLInfo::operator delete() */


RecvTTLInfo::RecvTTLInfo
(
  unsigned char  _recvTTLHandle
) :
    recvTTLHandle( _recvTTLHandle)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) RecvTTLInfo::AddRef();
  return;

} /* RecvTTLInfo::RecvTTLInfo() */


ds::ErrorType CDECL RecvTTLInfo::GetAncID
(
  ds::Sock::AncDataIDType *  ancIDPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ancIDPtr)
  {
    LOG_MSG_INVALID_INPUT_0("RecvTTLInfo::GetAncID(): "
                            "NULL arg");
    return QDS_EFAULT;
  }

  *ancIDPtr = AncData::RECV_TTL_INFO;
  return AEE_SUCCESS;

} /* RecvTTLInfo::GetAncID() */


ds::ErrorType CDECL RecvTTLInfo::SetAncID
(
  ds::Sock::AncDataIDType  ancID
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return AEE_SUCCESS;

} /* RecvTTLInfo::SetAncID() */


ds::ErrorType CDECL RecvTTLInfo::GetRecvTTLHandle
(
  unsigned char *  recvTTLHandlePtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("RecvTTLInfo::GetRecvIFHandle(): "
                           "Obj 0x%x obj 0x%x", this, recvTTLHandlePtr);

  if (0 == recvTTLHandlePtr)
  {
    return QDS_EFAULT;
  }

  *recvTTLHandlePtr = recvTTLHandle;

  return AEE_SUCCESS;

} /* RecvTTLInfo::GetRecvIFHandle() */


ds::ErrorType CDECL RecvTTLInfo::QueryInterface
(
  AEEIID   iid,
  void **  objPtrPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("RecvTTLInfo::QueryInterface(): "
                           "Obj 0x%x iid %d  obj 0x%x", this, iid, objPtrPtr);

  if (0 == objPtrPtr)
  {
    return QDS_EFAULT;
  }

  switch (iid)
  {
    case AEEIID_IAncDataPriv:
    {
      *objPtrPtr = static_cast <IAncDataPriv *> ( this);
      (void) AddRef();
      break;
    }

    case AEEIID_IRecvTTLInfoPriv:
    {
      *objPtrPtr = this;
      (void) AddRef();
      break;
    }

    case AEEIID_IQI:
    {
      *objPtrPtr = static_cast <IQI *> ( this);
      (void) AddRef();
      break;
    }

    default:
    {
      *objPtrPtr = 0;

      return AEE_ECLASSNOTSUPPORT;
    }
  } /* switch */

  return AEE_SUCCESS;

} /* RecvTTLInfo::QueryInterface() */

