/**
@file pd_qurt_pimutex_user.c
@brief This file contains the API details for the Protection Domain Monitor
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_qurt_pimutex_user.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "pd_qurt.h"

/////////////////////////////////////////////////////////////////////
// Management of an QURT PIMUTEX within HOST Protection Domain
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Implementation Details
/////////////////////////////////////////////////////////////////////

/**
API, pd_qurt_pimutex_init
@param[in] a1.ptr/const char* NULL Terminated Device Name
@return
PD_QURT_HANDLE_PIMUTEX -- Opaque Device Handle
*/
PD_QURT_HANDLE_PIMUTEX pd_qurt_pimutex_init(const char* name)
{
   PD_QURT_HANDLE_PIMUTEX rc = -1;

   if (PD_QURT_DEVICE_NAME_LEN > strnlen(name, PD_QURT_DEVICE_NAME_LEN))
   {
      char device[PD_QURT_DEVICE_PATH_LEN + PD_QURT_DEVICE_NAME_LEN];

      strlcpy(device, PD_QURT_DEVICE_PIMUTEX, sizeof(device));
      strlcat(device, name, sizeof(device));

      rc = qurt_qdi_open(device, 0002, 0777); // path, flags (O_RDWR), permissions (ugo+rwx)
   }

   return rc;
}

/**
API, pd_qurt_pimutex_destroy
@param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
@return
PD_QURT_STATUS -- Error Indication
*/
PD_QURT_STATUS pd_qurt_pimutex_destroy(PD_QURT_HANDLE_PIMUTEX handle)
{
   return (0 <= qurt_qdi_close(handle)) ? PD_QURT_STATUS_SUCCESS : PD_QURT_STATUS_ERROR;
}

/**
API, pd_qurt_pimutex_lock
@param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
@return
PD_QURT_STATUS -- Error Indication
*/
PD_QURT_STATUS pd_qurt_pimutex_lock(PD_QURT_HANDLE_PIMUTEX handle)
{
   return (0 <= qurt_qdi_handle_invoke(handle, PD_QURT_METHOD_PIMUTEX_LOCK)) ? PD_QURT_STATUS_SUCCESS : PD_QURT_STATUS_ERROR;
}

/**
API, pd_qurt_pimutex_unlock
@param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
@return
PD_QURT_STATUS -- Error Indication
*/
PD_QURT_STATUS pd_qurt_pimutex_unlock(PD_QURT_HANDLE_PIMUTEX handle)
{
   return (0 <= qurt_qdi_handle_invoke(handle, PD_QURT_METHOD_PIMUTEX_UNLOCK)) ? PD_QURT_STATUS_SUCCESS : PD_QURT_STATUS_ERROR;
}

/**
API, pd_qurt_pimutex_try_lock
@param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
@param[in] int* Operation Result
@return
PD_QURT_STATUS -- Error Indication
*/
PD_QURT_STATUS pd_qurt_pimutex_try_lock(PD_QURT_HANDLE_PIMUTEX handle, int* out_result_p)
{
   return (0 <= qurt_qdi_handle_invoke(handle, PD_QURT_METHOD_PIMUTEX_TRY_LOCK, out_result_p)) ? PD_QURT_STATUS_SUCCESS : PD_QURT_STATUS_ERROR;
}
