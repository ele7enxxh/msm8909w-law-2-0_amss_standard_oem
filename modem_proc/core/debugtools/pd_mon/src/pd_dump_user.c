/**
@file pd_dump_user.c
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
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_dump_user.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "pd_dump.h"

/**
API, pd_dump_init
@param[in] a1.ptr/const char* NULL Terminated Device Name
@return
PD_DUMP_HANDLE -- Opaque Device Handle
*/
PD_DUMP_HANDLE pd_dump_init(const char* name)
{
   PD_DUMP_HANDLE rc = -1;

   if (PD_DUMP_DEVICE_NAME_LEN > strnlen(name, PD_DUMP_DEVICE_NAME_LEN))
   {
      char device[PD_DUMP_DEVICE_PATH_LEN + PD_DUMP_DEVICE_NAME_LEN];

      strlcpy(device, PD_DUMP_DEVICE, sizeof(device));
      strlcat(device, name, sizeof(device));

      rc = qurt_qdi_open(device);
   }

   return rc;
}

/**
API, pd_dump_destroy
@param[in] PD_DUMP_HANDLE Opaque Device Handle
@return
PD_DUMP_STATUS -- Error Indication
*/
PD_DUMP_STATUS pd_dump_destroy(PD_DUMP_HANDLE handle)
{
   return (0 <= qurt_qdi_close(handle)) ? PD_DUMP_STATUS_SUCCESS : PD_DUMP_STATUS_ERROR;
}

/**
API, pd_dump_method_0
@param[in] PD_DUMP_HANDLE Opaque Device Handle
@return
PD_DUMP_STATUS -- Error Indication
*/
PD_DUMP_STATUS pd_dump_method_0(PD_DUMP_HANDLE handle)
{
   return (0 <= qurt_qdi_handle_invoke(handle, PD_DUMP_METHOD_0)) ? PD_DUMP_STATUS_SUCCESS : PD_DUMP_STATUS_ERROR;
}

/**
API, pd_dump_method_1
@param[in] PD_DUMP_HANDLE Opaque Device Handle
@return
PD_DUMP_STATUS -- Error Indication
*/
PD_DUMP_STATUS pd_dump_method_1(PD_DUMP_HANDLE handle)
{
   return (0 <= qurt_qdi_handle_invoke(handle, PD_DUMP_METHOD_1)) ? PD_DUMP_STATUS_SUCCESS : PD_DUMP_STATUS_ERROR;
}
