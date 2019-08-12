#ifndef PD_DUMP_H
#define PD_DUMP_H
/**
@file pd_dump.h
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
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_dump.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "qurt.h"
#include "qurt_qdi_driver.h"

#include "msg.h"
#include "tms_utils.h"
#include "rfsa_client.h"

#include "stdlib.h"
#include "stringl/stringl.h"

#if defined(__cplusplus)
extern "C" {
#endif

// COMMON

#define PD_DUMP_DEVICE_PATH_LEN     32
#define PD_DUMP_DEVICE_NAME_LEN     (QURT_MAX_NAME_LEN*2)
#define PD_DUMP_DEVICE_ASID_MAX     32 /*QURT_MAX_ASIDS*/

typedef enum { PD_DUMP_STATUS_SUCCESS = 0, PD_DUMP_STATUS_ERROR = -1 } PD_DUMP_STATUS;

typedef int PD_DUMP_HANDLE;

#define PD_DUMP_DEVICE "/pd_dump/"

#define PD_DUMP_METHOD_0                           (QDI_PRIVATE+0)
#define PD_DUMP_METHOD_1                           (QDI_PRIVATE+1)

/**
API, pd_dump_init
@param[in] a1.ptr/const char* NULL Terminated Device Name
@return
PD_DUMP_HANDLE -- Opaque Device Handle
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
PD_DUMP_HANDLE pd_dump_init(const char* name);

/**
API, pd_dump_destroy
@param[in] PD_DUMP_HANDLE Opaque Device Handle
@return
PD_DUMP_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
PD_DUMP_STATUS pd_dump_destroy(PD_DUMP_HANDLE handle);

/**
API, pd_dump_method_0
@param[in] PD_DUMP_HANDLE Opaque Device Handle
@return
PD_DUMP_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
PD_DUMP_STATUS pd_dump_method_0(PD_DUMP_HANDLE handle);

/**
API, pd_dump_method_1
@param[in] PD_DUMP_HANDLE Opaque Device Handle
@return
PD_DUMP_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
PD_DUMP_STATUS pd_dump_method_1(PD_DUMP_HANDLE handle);

/**
API, Initialization of service prior to use
@return
None.
*/
void pd_dump_driver_init(void);

/**
Termination of service following use
@return
None.
*/
void pd_dump_driver_term(void);

#if defined(__cplusplus)
}
#endif

#endif
