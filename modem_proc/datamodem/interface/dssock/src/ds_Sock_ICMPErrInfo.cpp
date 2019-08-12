/*===========================================================================
  FILE: ds_Sock_ICMPErrInfo.cpp

  OVERVIEW: This file provides implementation of the ICMPErrInfo class.

  DEPENDENCIES: None

  Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_ICMPErrInfo.cpp#1 $
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

#include "ds_Sock_ICMPErrInfo.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_mem.h"

#include "stringl.h"

using namespace ds::Sock;
using namespace ds::Error;


/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
void * ICMPErrInfo::operator new
(
  unsigned int numBytes
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_mem_get_buf( PS_MEM_ICMP_ERR_INFO_TYPE);

} /* ICMPErrInfo::operator new() */


void ICMPErrInfo::operator delete
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

} /* ICMPErrInfo::operator delete() */


ICMPErrInfo::ICMPErrInfo
(
  struct ps_sock_extended_err  psExtendedErrInfo,
  ds::SockAddrInternalType*    _addr
) :
    addr(*_addr)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  extendedErrInfo.error_number = psExtendedErrInfo.ee_errno;
  extendedErrInfo.origin       = psExtendedErrInfo.ee_origin;
  extendedErrInfo.type         = psExtendedErrInfo.ee_type;
  extendedErrInfo.code         = psExtendedErrInfo.ee_code;
  extendedErrInfo.info         = psExtendedErrInfo.ee_info;

  /*-------------------------------------------------------------------------
    Using explicit scope to shut up lint. It complains because AddRef() is
    declared as virtual to shut up compiler
  -------------------------------------------------------------------------*/
  (void) ICMPErrInfo::AddRef();
  return;

} /* ICMPErrInfo::ICMPErrInfo() */


ds::ErrorType CDECL ICMPErrInfo::GetAncID
(
  ds::Sock::AncDataIDType *  ancIDPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ancIDPtr)
  {
    LOG_MSG_INVALID_INPUT_0("ICMPErrInfo::GetAncID(): "
                            "NULL arg");
    return QDS_EFAULT;
  }

  *ancIDPtr = AncData::ICMP_ERROR_INFO;
  return AEE_SUCCESS;

} /* ICMPErrInfo::GetAncID() */


ds::ErrorType CDECL ICMPErrInfo::SetAncID
(
  ds::Sock::AncDataIDType  ancID
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return AEE_SUCCESS;

} /* ICMPErrInfo::SetAncID() */


ds::ErrorType CDECL ICMPErrInfo::GetExtendedErr
(
  ExtendedErrType *  extendedErrInfoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == extendedErrInfoPtr)
  {
    LOG_MSG_INVALID_INPUT_1("ICMPErrInfo::GetExtendedErr(): "
                            "NULL arg, obj 0x%x", this);
    return QDS_EFAULT;
  }

  *extendedErrInfoPtr = extendedErrInfo;

  LOG_MSG_FUNCTION_EXIT_1("ICMPErrInfo::GetExtendedErr(): "
                          "Success, obj 0x%x", this);
  return AEE_SUCCESS;

} /* ICMPErrInfo::GetExtendedErr() */


ds::ErrorType CDECL ICMPErrInfo::GetAddr
(
  ds::SockAddrStorageType _addr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == _addr)
  {
    LOG_MSG_INVALID_INPUT_1("ICMPErrInfo::GetAddr(): "
                            "NULL arg, obj 0x%x", this);
    return QDS_EFAULT;
  }

  memscpy( _addr, sizeof(ds::SockAddrStorageType),
           &addr, sizeof(ds::SockAddrStorageType));

  LOG_MSG_FUNCTION_EXIT_1("ICMPErrInfo::GetAddr(): "
                          "Success, obj 0x%x", this);
  return AEE_SUCCESS;

} /* ICMPErrInfo::GetAddr() */


ds::ErrorType CDECL ICMPErrInfo::QueryInterface
(
  AEEIID   iid,
  void **  objPtrPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == objPtrPtr)
  {
    LOG_MSG_INVALID_INPUT_1("ICMPErrInfo::QueryInterface(): "
                            "NULL arg, obj 0x%x", this);
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

    case AEEIID_IICMPErrInfoPriv:
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
      LOG_MSG_INVALID_INPUT_2("ICMPErrInfo::QueryInterface(): "
                              "Unknown iid %d, obj 0x%x", iid, this);
      return AEE_ECLASSNOTSUPPORT;
    }
  } /* switch */

  LOG_MSG_FUNCTION_EXIT_2("ICMPErrInfo::QueryInterface(): "
                          "Success, obj 0x%x  iid %d", this, iid);
  return AEE_SUCCESS;

} /* ICMPErrInfo::QueryInterface() */

