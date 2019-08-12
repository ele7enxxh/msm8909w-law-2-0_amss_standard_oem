/*===========================================================================
  FILE: ds_Sock_RecvIFInfo.cpp

  OVERVIEW: This file provides implementation of the RecvIFInfo class.

  DEPENDENCIES: None

  Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_RecvIFInfo.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2009-03-02 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "ds_Utils_StdErr.h"

#include "ds_Sock_RecvIFInfo.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_mem.h"

using namespace ds::Sock;
using namespace ds::Error;


/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
void * RecvIFInfo::operator new
(
  unsigned int numBytes
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_mem_get_buf( PS_MEM_RECV_IF_INFO_TYPE);

} /* RecvIFInfo::operator new() */


void RecvIFInfo::operator delete
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

  PS_MEM_FREE( bufPtr);
  return;

} /* RecvIFInfo::operator delete() */


RecvIFInfo::RecvIFInfo
(
  unsigned int  _recvIFHandle
) :
    recvIFHandle( _recvIFHandle)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Using explicit scope to shut up lint. It complains because AddRef() is
    declared as virtual to shut up compiler
  -------------------------------------------------------------------------*/
  (void) RecvIFInfo::AddRef();
  return;

} /* RecvIFInfo::RecvIFInfo() */


ds::ErrorType CDECL RecvIFInfo::GetAncID
(
  ds::Sock::AncDataIDType *  ancIDPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ancIDPtr)
  {
    LOG_MSG_INVALID_INPUT_0("RecvIFInfo::GetAncID(): "
                            "NULL arg");
    return QDS_EFAULT;
  }

  *ancIDPtr = AncData::RECV_IF_INFO;
  return AEE_SUCCESS;

} /* RecvIFInfo::GetAncID() */


ds::ErrorType CDECL RecvIFInfo::SetAncID
(
  ds::Sock::AncDataIDType  ancID
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return AEE_SUCCESS;

} /* RecvIFInfo::SetAncID() */


ds::ErrorType CDECL RecvIFInfo::GetRecvIFHandle
(
  unsigned int *  recvIFHandlePtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("RecvIFInfo::GetRecvIFHandle(): "
                           "Obj 0x%x obj 0x%x ", this, recvIFHandlePtr);

  if (0 == recvIFHandlePtr)
  {
    return QDS_EFAULT;
  }

  *recvIFHandlePtr = recvIFHandle;

  return AEE_SUCCESS;

} /* RecvIFInfo::GetRecvIFHandle() */


ds::ErrorType CDECL RecvIFInfo::QueryInterface
(
  AEEIID   iid,
  void **  objPtrPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("RecvIFInfo::QueryInterface(): "
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

    case AEEIID_IRecvIFInfoPriv:
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

} /* RecvIFInfo::QueryInterface() */

