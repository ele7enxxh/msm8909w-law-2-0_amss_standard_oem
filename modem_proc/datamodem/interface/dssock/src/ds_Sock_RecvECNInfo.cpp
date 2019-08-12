/*===========================================================================
  FILE: ds_Sock_RecvECNInfo.cpp

  OVERVIEW: This file provides implementation of the RecvECNInfo class.

  DEPENDENCIES: None

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_RecvECNInfo.cpp#1 $
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

#include "ds_Sock_RecvECNInfo.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"


using namespace ds::Sock;
using namespace ds::Error;


/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
void * RecvECNInfo::operator new
(
  unsigned int numBytes
)
throw()
{

  void * buf = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_SYSTEM_HEAP_MEM_ALLOC(buf, numBytes, void *);
  return buf; 
   
} /* RecvECNInfo::operator new() */

void RecvECNInfo::operator delete
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
} /* RecvECNInfo::operator delete() */


RecvECNInfo::RecvECNInfo
(
  unsigned char  _recvECNHandle
) :
    recvECNHandle( _recvECNHandle)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) RecvECNInfo::AddRef();
  return;

} /* RecvECNInfo::RecvECNInfo() */


ds::ErrorType CDECL RecvECNInfo::GetAncID
(
  ds::Sock::AncDataIDType *  ancIDPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ancIDPtr)
  {
    LOG_MSG_INVALID_INPUT_0("RecvECNInfo::GetAncID(): "
                            "NULL arg");
    return QDS_EFAULT;
  }

  *ancIDPtr = AncData::RECV_ECN_INFO;
  return AEE_SUCCESS;

} /* RecvECNInfo::GetAncID() */


ds::ErrorType CDECL RecvECNInfo::SetAncID
(
  ds::Sock::AncDataIDType  ancID
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return AEE_SUCCESS;

} /* RecvECNInfo::SetAncID() */


ds::ErrorType CDECL RecvECNInfo::GetRecvECNHandle
(
  unsigned char *  recvECNHandlePtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("RecvECNInfo::GetRecvIFHandle(): "
                           "Obj 0x%x ECNHandle obj 0x%x ", this, recvECNHandlePtr);

  if (0 == recvECNHandlePtr)
  {
    return QDS_EFAULT;
  }

  *recvECNHandlePtr = recvECNHandle;

  return AEE_SUCCESS;

} /* RecvECNInfo::GetRecvIFHandle() */


ds::ErrorType CDECL RecvECNInfo::QueryInterface
(
  AEEIID   iid,
  void **  objPtrPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("RecvECNInfo::QueryInterface(): "
                           "Obj 0x%x iid %d obj 0x%x", this, iid, objPtrPtr);

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

    case AEEIID_IRecvECNInfoPriv:
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

} /* RecvECNInfo::QueryInterface() */

