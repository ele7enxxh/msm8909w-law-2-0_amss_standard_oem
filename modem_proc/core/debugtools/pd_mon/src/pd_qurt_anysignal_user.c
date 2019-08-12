/**
@file pd_qurt_anysignal_user.c
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
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_qurt_anysignal_user.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "pd_qurt.h"

/////////////////////////////////////////////////////////////////////
// Management of an QURT ANYSIGNAL within HOST Protection Domain
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
API, pd_qurt_anysignal_init
@param[in] a1.ptr/const char* NULL Terminated Device Name
@return
PD_QURT_HANDLE_ANYSIGNAL -- Opaque Device Handle
*/
PD_QURT_HANDLE_ANYSIGNAL pd_qurt_anysignal_init(const char* name)
{
   PD_QURT_HANDLE_ANYSIGNAL rc = -1;

   if (PD_QURT_DEVICE_NAME_LEN > strnlen(name, PD_QURT_DEVICE_NAME_LEN))
   {
      char device[PD_QURT_DEVICE_PATH_LEN + PD_QURT_DEVICE_NAME_LEN];

      strlcpy(device, PD_QURT_DEVICE_ANYSIGNAL, sizeof(device));
      strlcat(device, name, sizeof(device));

      rc = qurt_qdi_open(device, 0002, 0777); // path, flags (O_RDWR), permissions (ugo+rwx)
   }

   return rc;
}

/**
API, pd_qurt_anysignal_destroy
@param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
@return
PD_QURT_STATUS -- Error Indication
*/
PD_QURT_STATUS pd_qurt_anysignal_destroy(PD_QURT_HANDLE_ANYSIGNAL handle)
{
   return (0 <= qurt_qdi_close(handle)) ? PD_QURT_STATUS_SUCCESS : PD_QURT_STATUS_ERROR;
}

/**
API, pd_qurt_anysignal_wait
@param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
@param[in] unsigned int Operation Argument
@param[in] unsigned int* Operation Result
@return
PD_QURT_STATUS -- Error Indication
*/
PD_QURT_STATUS pd_qurt_anysignal_wait(PD_QURT_HANDLE_ANYSIGNAL handle, unsigned int in_mask, unsigned int* out_mask_p)
{
   return (0 <= qurt_qdi_handle_invoke(handle, PD_QURT_METHOD_ANYSIGNAL_WAIT, in_mask, out_mask_p)) ? PD_QURT_STATUS_SUCCESS : PD_QURT_STATUS_ERROR;
}

/**
API, pd_qurt_anysignal_set
@param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
@param[in] unsigned int Operation Argument
@param[in] unsigned int* Operation Result
@return
PD_QURT_STATUS -- Error Indication
*/
PD_QURT_STATUS pd_qurt_anysignal_set(PD_QURT_HANDLE_ANYSIGNAL handle, unsigned int in_mask, unsigned int* out_mask_p)
{
   return (0 <= qurt_qdi_handle_invoke(handle, PD_QURT_METHOD_ANYSIGNAL_SET, in_mask, out_mask_p)) ? PD_QURT_STATUS_SUCCESS : PD_QURT_STATUS_ERROR;
}

/**
API, pd_qurt_anysignal_get
@param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
@param[in] unsigned int* Operation Result
@return
PD_QURT_STATUS -- Error Indication
*/
PD_QURT_STATUS pd_qurt_anysignal_get(PD_QURT_HANDLE_ANYSIGNAL handle, unsigned int* out_mask_p)
{
   return (0 <= qurt_qdi_handle_invoke(handle, PD_QURT_METHOD_ANYSIGNAL_GET, out_mask_p)) ? PD_QURT_STATUS_SUCCESS : PD_QURT_STATUS_ERROR;
}

/**
API, pd_qurt_anysignal_clear
@param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
@param[in] unsigned int Operation Argument
@param[in] unsigned int* Operation Result
@return
PD_QURT_STATUS -- Error Indication
*/
PD_QURT_STATUS pd_qurt_anysignal_clear(PD_QURT_HANDLE_ANYSIGNAL handle, unsigned int in_mask, unsigned int* out_mask_p)
{
   return (0 <= qurt_qdi_handle_invoke(handle, PD_QURT_METHOD_ANYSIGNAL_CLEAR, in_mask, out_mask_p)) ? PD_QURT_STATUS_SUCCESS : PD_QURT_STATUS_ERROR;
}
