#ifndef __DSS_COMMON_H__
#define __DSS_COMMON_H__

/*===================================================

FILE:  DSS_Common.h

SERVICES:
   Common functions and macros for DSS_.

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_Common.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-13 en  History added.

===========================================================================*/

#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "DSS_Conversion.h"
#include "ds_sig_svc.h"

// Call the function f, which returns AEEResult.
// If it fails, convert the result to a ds error code, assign it to *dss_errno
// and return DSS_ERROR.
// dss_errno should be defined in the scope, and be of type sint15.
#define IDS_ERR_RET_ERRNO(f)                                \
   do {                                                     \
      AEEResult nRes = f;                                   \
      if (AEE_SUCCESS != nRes) {                            \
         *dss_errno = DSSConversion::IDS2DSErrorCode(nRes); \
         return DSS_ERROR;                                  \
      }                                                     \
   } while (0)

#define IDS_ERR_RET(f)                                      \
   do {                                                     \
      AEEResult nRes = f;                                   \
      if (AEE_SUCCESS != nRes) {                            \
         return nRes;                                       \
      }                                                     \
   } while (0)

// Call the function f, which returns AEEResult.
// If it fails, convert the result to a ds error code, assign it to *dss_errno,
// and goto bail to free any specific resources.
// dss_errno should be defined in the scope, and be of type sint15.
// nRet should be defined in the scope
// bail label is required in the function
#define BAIL_ERRNO(f)                                       \
   do {                                                     \
      AEEResult res = f;                                    \
      if (AEE_SUCCESS != res) {                             \
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);  \
         nRet = DSS_ERROR;                                  \
         goto bail;                                         \
      }                                                     \
   } while (0)


// Call the function f, which returns AEEResult.
// If it fails, goto bail to free any specific resources.
// nRet should be defined in the scope
// bail label is required in the function
#define BAIL_ERR(f)                                         \
   do {                                                     \
      nRet = f;                                             \
      if (AEE_SUCCESS != nRet) {                            \
         goto bail;                                         \
      }                                                     \
   } while (0)

// This C macro is called from all DS API routines
// In case this macro is called from within DS_SIG task
// the DS API routine returns an error.
#define DSS_DS_API_CALLED_RET_ERRNO()                             \
   do {                                                           \
      if (ds_sig_is_current_task()) {                             \
         LOG_MSG_ERROR_0("ERROR: DS API called from DS_SIG task");\
         *dss_errno = DS_EINVAL;                                  \
         return DSS_ERROR;                                        \
      }                                                           \
   } while (0)

// This C macro is called from all DS API routines
// In case this macro is called from within DS_SIG task
// the DS API routine returns the parameter to the macro as error.
#define DSS_DS_API_CALLED_RET_ERRNO_PARAM(ret)                    \
   do {                                                           \
      if (ds_sig_is_current_task()) {                             \
         LOG_MSG_ERROR_0("ERROR: DS API called from DS_SIG task");\
         *dss_errno = DS_EINVAL;                                  \
         return ret;                                              \
      }                                                           \
   } while (0)

// This C macro is called from all DS API routines
// In case this macro is called from within DS_SIG task
// the DS API routine returns an error. dss_errno is not set.
#define DSS_DS_API_CALLED_RET()                                   \
   do {                                                           \
      if (ds_sig_is_current_task()) {                             \
         LOG_MSG_ERROR_0("ERROR: DS API called from DS_SIG task");  \
         return DSS_ERROR;                                        \
      }                                                           \
   } while (0)

// This C macro is called from all DS API routines
// In case this macro is called from within DS_SIG task
// the DS API routine returns the parameter to the macro as error.
#define DSS_DS_API_CALLED_RET_PARAM(ret)                          \
  do {                                                            \
     if (ds_sig_is_current_task()) {                              \
        LOG_MSG_ERROR_0("ERROR: DS API called from DS_SIG task");   \
        return ret;                                               \
      }                                                           \
   } while (0)

namespace DSSCommon
{
   inline void ReleaseIf(IQI** iqi) throw()
   {
      if (0 != *iqi) {
         // The cast to void is done because we do not
         // need to check the return value which is 
         // the decremented reference count for the object.
         // The cast is done to avoid lint error.
         (void)(*iqi)->Release();
         *iqi = NULL;
      }
   }
}

#endif // __DSS_COMMON_H__
