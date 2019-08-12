#ifndef DS_UTILS_CONVERSION_H
#define DS_UTILS_CONVERSION_H
/*=========================================================================*/
/*! 
  @file 
  ds_Utils_Conversion.h

  @brief
  This file provides conversion from errors defined in dserrno.h to
  ds::ErrorType.

  @see dserrno.h
  @see DSErrors.h

  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/
/*=========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"
#include "dssocket_defs.h"

/*===========================================================================

                       PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Utils
{
namespace Conversion
{

/*!
  @function
  PSErrnoGetDSErrno

  @brief
  Converts errno from PS to ds format. 

  @param[in]  Legacy DSS errno (see dserrno.h)
  @return     ds Format errno value (see ds_Errors_Def.h)
*/
ds::ErrorType PSErrnoGetDSErrno
(
  int16  psErrno
) 
throw();


/*!
  @function
  ProcessLenReq

  @param[in]  bufLen - length of buffer allocated by user
              This is the 'len' field provided by user. This can 
              be 0 if the user just wants to retrieve the number
              of items. 

  @param[out] pBufLenReq - available length to be provided to user
              This is the 'lenReq' field provided by user. If it is
              non-null, then it should be populated by the total 
              number of items available. 

  @param[in]  srcBufLen - available number of items at the lower layers. 
              If 'lenReq' field is non-null, it should be populated
              which this field.
  
  @param[out] pNumItems - This is a internal variable. This would contain
              how many items we can populate in the buffer provided by 
              user. It is calculated within ProcessLenReq() function as:
              MIN (srcBufLen, bufLen). Since it is passed by internal
              clients of ProcessLenReq(), it must be non-null.

  @return     ds Format errno value (see ds_Errors_Def.h)

  @brief
  Performs required processing for returning lenReq sequences.

  @details 
  LenReq parameter is populated by srcBufLen, 
  pNumItems represents number of bytes which
  should be copied by user to result buffer(min(bufLen, srcBufLen))
*/
ds::ErrorType ProcessLenReq
(
  int         bufLen,
  int         *pBufLenReq,
  int         srcBufLen,
  int         *pNumItems
);

/**
  @function
  PutCMsg

  @brief Fills the passed cmsg structure with the passed data

  This function fills the passed cmsg structure with the passed data after
  aligning it. The CMSG header is using a little-endian order. The data part
  is written as is without changing the byte order.
  
  @param[in]    msg a msghdr structure that contains the control len of cmsg
  @param[out]   cmsg the control message header
  @param[in]    level Conrol message level (e.g IPPROTO_IP)
  @param[in]    type Control message type (e.g IP_RECVIF)
  @param[in]    len  Length of the data buffer.
  @param[in]    data The data buffer that need to be put in the cmsg.
  @param[inout] cmsgTotalLen Adds to the input value
                the total length of the cmsghdr +required padding+data
                The actual cmsg formed may be smaller than this value.
  @return     ds Format errno value (see ds_Errors_Def.h)
*/
ds::ErrorType PutCMsg
(
  struct dss_msghdr  *msg,
  struct dss_cmsghdr *cmsg,
  int16               level,
  int16               type,
  uint16              len,
  void               *data,
  uint16             *cmsgTotalLen
);


/**
  @macro
  htolel

  @brief Converts host-to-little endian long integer

  Converts host-to-little endian long integer.  Handles potential byte order
  differences between different computer architectures.

  @param[in]    x     unsigned long integer value to be converted.
  @return       The little endian byte-ordered value.
  */
#define dss_htolel(x) \
   (x)

/**
  @macro
  dss_htoles

  @brief Converts host-to-little endian short

  Converts host-to-little endian short integer.  Handles potential byte order
  differences between different computer architectures.

  @param[in]    x     unsigned short integer value to be converted.
  @return       The little endian byte-ordered value.
  */
#define dss_htoles(x) \
   (x)


} /* namespace Conversion */
} /* namespace Utils */
} /* namespace ds */

#endif /* DS_UTILS_CONVERSION_H */

