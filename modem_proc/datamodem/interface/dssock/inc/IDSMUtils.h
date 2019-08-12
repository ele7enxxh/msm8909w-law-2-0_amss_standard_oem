#ifndef DS_IDSMUTIL_H
#define DS_IDSMUTIL_H

/*===========================================================================
@file DSMUtils.h

This file provides declarations for DSM utilities.
It is used only in DSS simulation environment. Target ds code
shall define a parallel header for DSM functions

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

#include "datamodem_variation.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_Interface.h"
#include "dsm.h"
#include "ds_Utils_IQI.h"
/*lint -esym(1510, IDSMUtils) */
namespace ds {
   const ::AEEIID AEEIID_IDSMUtils = 0x0107dd5f;

   class IDSMUtils : public IQI
   {
      public:
         virtual ::AEEResult CDECL ReadDSMChain(dsm_item_type** item_ptr, int32* pnBytesRead) = 0;
         virtual ::AEEResult CDECL WriteDSMChain(dsm_item_type** item_ptr, int32* pnBytesWritten) = 0;
         virtual ::AEEResult CDECL SendToDSMChain(dsm_item_type** item_ptr_ptr, const ::ds::SockAddrStorageType remoteAddr, unsigned int flags, int32* pnBytesSent) = 0;
         virtual ::AEEResult CDECL RecvFromDSMChain(dsm_item_type** item_ptr_ptr, ds::SockAddrStorageType remoteAddr, unsigned int flags, int32* pnBytesRecvd) = 0;
   };
}
#endif /* #ifndef DS_IDSMUTIL_H */
