/*===========================================================================
  FILE: ds_Utils_Conversion.cpp

  OVERVIEW: This file provides conversion utilities.

  DEPENDENCIES: None

  Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are signaled in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_Conversion.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-01-10 sa  Added appropriate log messages before ASSERT(0).  
  2010-01-11 ts  Added PutCMsg utility for ancillary data.
  2009-03-01 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "ds_Utils_StdErr.h"

#include "dserrno.h"
#include "amssassert.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_Conversion.h"
#include "ps_in.h"
#include "stringl.h"

using namespace ds::Error;


/*===========================================================================

                       PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
ds::ErrorType ds::Utils::Conversion::PSErrnoGetDSErrno
(
  int16  psErrno
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Map psErrno to ds::ErrorType
  -------------------------------------------------------------------------*/
  switch (psErrno)
  {
    case DS_EEOF:
    {
      return QDS_EEOF;
    }

    case DS_EINTR:
    {
      return QDS_EINTR;
    }

    case DS_EBADF:
    {
      return QDS_EBADF;
    }

    case DS_EFAULT:
    {
      return QDS_EFAULT;
    }

    case DS_EWOULDBLOCK:
    {
      return AEE_EWOULDBLOCK;
    }

    case DS_EAFNOSUPPORT:
    {
      return QDS_EAFNOSUPPORT;
    }

    case DS_EPROTOTYPE:
    {
      return QDS_EPROTOTYPE;
    }

    case DS_ESOCKNOSUPPORT:
    {
      return QDS_ESOCKNOSUPPORT;
    }

    case DS_EPROTONOSUPPORT:
    {
      return QDS_EPROTONOSUPPORT;
    }

    case DS_EMFILE:
    {
      return QDS_EMFILE;
    }

    case DS_EOPNOTSUPP:
    {
      return QDS_EOPNOTSUPP;
    }

    case DS_EADDRINUSE:
    {
      return QDS_EADDRINUSE;
    }

    case DS_EADDRREQ:
    {
      return QDS_EADDRREQ;
    }

    case DS_EINPROGRESS:
    {
      return QDS_EINPROGRESS;
    }

    case DS_EISCONN:
    {
      return QDS_ENETISCONN;
    }

    case DS_EIPADDRCHANGED:
    {
      return QDS_EIPADDRCHANGED;
    }

    case DS_ENOTCONN:
    {
      return QDS_ENOTCONN;
    }

    case DS_ECONNREFUSED:
    {
      return QDS_ECONNREFUSED;
    }

    case DS_ETIMEDOUT:
    {
      return QDS_ETIMEDOUT;
    }

    case DS_ECONNRESET:
    {
      return QDS_ECONNRESET;
    }

    case DS_ECONNABORTED:
    {
      return QDS_ECONNABORTED;
    }

    case DS_EPIPE:
    {
      return QDS_EPIPE;
    }

    case DS_ENETDOWN:
    {
      return QDS_ENETDOWN;
    }

    case DS_EMAPP:
    {
      return QDS_EMAPP;
    }

    case DS_EBADAPP:
    {
      return QDS_EBADAPP;
    }

    case DS_SOCKEXIST:
    {
      return QDS_ESOCKEXIST;
    }

    case DS_EINVAL:
    {
      return QDS_EINVAL;
    }

    case DS_NAMEERR:
    {
      return QDS_ENAMEERR;
    }

    case DS_NAMEMISS:
    {
      return QDS_ENAMEERR;
    }

    case DS_NETERR:
    {
      return QDS_ENETNONET;
    }

    case DS_INUSE:
    {
      return QDS_EADDRINUSE;
    }

    case DS_ENOMEM:
    {
      return AEE_ENOMEMORY;
    }

    case DS_EMSGSIZE:
    {
      return QDS_EMSGSIZE;
    }

    case DS_ENOPROTOOPT:
    {
      return QDS_ENOPROTOOPT;
    }

    case DS_ENORECOVERY:
    {
      return AEE_EFAILED;
    }

    case DS_ENOADDRESS:
    {
      return QDS_EHOSTUNREACH;
    }

    case DS_EMSGTRUNC:
    {
      return QDS_EMSGTRUNC;
    }

    case DS_ENOROUTE:
    {
      return QDS_ENOROUTE;
    }

    case DS_ESHUTDOWN:
    {
      return QDS_ESHUTDOWN;
    }

    case DS_ENETUNREACH:
    {
      return QDS_ENETUNREACH;
    }

    case DS_EHOSTUNREACH:
    {
      return QDS_EHOSTUNREACH;
    }

    case DS_EHOSTDOWN:
    {
      return QDS_EHOSTDOWN;
    }

    case DS_ENONET:
    {
      return QDS_ENONET;
    }

    case DS_EPROTO:
    {
      return QDS_EPROTO;
    }

    case DS_EACCES:
    {
      return QDS_EACCES;
    }

    case DS_EQOSUNAWARE:
    {
      return QDS_EQOSUNAWARE;
    }

    case DS_ENOSRCADDR:
    {
      return QDS_ENOSRCADDR;
    }

    case DS_ENETISCONN:
    {
      return QDS_ENETISCONN;
    }

    case DS_ENETINPROGRESS:
    {
      return QDS_ENETINPROGRESS;
    }

    case DS_ENETNONET:
    {
      return QDS_ENETNONET;
    }

    case DS_ENETCLOSEINPROGRESS:
    {
      return QDS_ENETCLOSEINPROGRESS;
    }

    case DS_ENETEXIST:
    {
      return QDS_ENETEXIST;
    }

    case DS_ENETGOINGDORMANT:
    {
      return QDS_ENETGOINGDORMANT;
    }

    case DS_ETRUNCATED:
    {
      return QDS_EMSGTRUNC;
    }

    case DS_ESYSTEM:
    {
      return AEE_EFAILED;
    }

    case DS_EMAXQUERIES:
    {
      return QDS_EMAPP;
    }

    case DS_ENOERR:
    {
      return AEE_SUCCESS;
    }

    case DS_EDBOVERFLOW:
    {
      return QDS_BCMCSDB_OVERFLOW;
    }

    case DS_EDBNOOVERWRITE:
    {
      return QDS_BCMCSDB_NO_OVERWRITE;
    }

    default:
    {
      LOG_MSG_ERROR_1("ds::Utils::Conversion::PSErrnoGetDSErrno(): "
                      "Unknown ps err %d", psErrno);
      return AEE_EFAILED;
    }
  }
} /* PSErrnoGetDSErrno() */


ds::ErrorType ds::Utils::Conversion::ProcessLenReq
(
  int         bufLen,
  int *       pBufLenReq,
  int         srcBufLen,
  int         *pNumItems
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pNumItems)
  {
    DATA_ERR_FATAL("ds::Utils::Conversion::ProcessLenReq(): ProcessLenReq(): pNumItems ptr is NULL");
    return QDS_EFAULT;
  }
  if (bufLen < 0)
  {
    return QDS_EFAULT;
  }
  /*-------------------------------------------------------------------------
    Initialize lenReq argument to the available buffer length.
  -------------------------------------------------------------------------*/
  if (NULL != pBufLenReq)
  {
    *pBufLenReq = srcBufLen;
  }
  
  *pNumItems = MIN (srcBufLen, bufLen);

  return AEE_SUCCESS;

} /* ProcessLenReq() */


/**
  @function
  PutCMsg

  @brief Fills the passed cmsg structure with the passed data

  This function fills the passed cmsg structure with the passed data after
  aligning it.

  @param[in] msg a msghdr structure that contains the control len of cmsg
  @param[out] cmsg the control message header
  @param[in] level Conrol message level (e.g IPPROTO_IP)
  @param[in] type Control message type (e.g IP_RECVIF)
  @param[in] len  Length of the data buffer.
  @param[in] data The data buffer that need to be put in the cmsg.
  @param[inout] cmsgTotalLen Adds to the input value
                the total length of the cmsghdr +required padding+data
                The actual cmsg formed may be smaller than this value.
  @return     ds Format errno value (see ds_Errors_Def.h)
*/
ds::ErrorType ds::Utils::Conversion::PutCMsg
(
  struct dss_msghdr  *msg,
  struct dss_cmsghdr *cmsg,
  int16               level,
  int16               type,
  uint16              len,
  void               *data,
  uint16             *cmsgTotalLen
)
  {
  uint16 cmlen = DSS_CMSG_LEN(len);
  ds::ErrorType err = AEE_SUCCESS;

  if ( NULL == msg || NULL == cmsg || 
       NULL == data || NULL == cmsgTotalLen )
  {
    return QDS_EFAULT;
  }
  
  if((((unsigned char *)msg->msg_control) + msg->msg_controllen) <
    (((unsigned char *)cmsg) + cmlen)) {
      /*--------------------------------------------------------------------
        Message truncated.
      --------------------------------------------------------------------*/
      err = QDS_EMSGTRUNC;
      cmlen = ((unsigned char*)msg->msg_control) + msg->msg_controllen -
        ((unsigned char*)cmsg);
  }

  cmsg->cmsg_level = dss_htoles((uint16)level);
  cmsg->cmsg_type  = dss_htoles((uint16)type);
  cmsg->cmsg_len   = dss_htoles(cmlen);

  memscpy(DSS_CMSG_DATA(cmsg), cmlen - sizeof(struct dss_cmsghdr),
          data, cmlen - sizeof(struct dss_cmsghdr));
  (*cmsgTotalLen) += DSS_CMSG_SPACE(len);
  return err;
} /* PutCMsg */
