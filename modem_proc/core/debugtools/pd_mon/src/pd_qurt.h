#ifndef PD_QURT_H
#define PD_QURT_H
/**
@file pd_qurt.h
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
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_qurt.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "msg.h"

#include "qurt.h"
#include "qurt_qdi_driver.h"

#include "stdarg.h"
#include "stdlib.h"
#include "stringl/stringl.h"

#if defined(__cplusplus)
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////
// Common
/////////////////////////////////////////////////////////////////////

#define PD_QURT_DEVICE_PATH_LEN     32
#define PD_QURT_DEVICE_NAME_LEN     (QURT_MAX_NAME_LEN*2)
#define PD_QURT_DEVICE_ASID_MAX     32 /*QURT_MAX_ASIDS*/

typedef enum { PD_QURT_STATUS_SUCCESS = 0, PD_QURT_STATUS_ERROR = -1 } PD_QURT_STATUS;

/////////////////////////////////////////////////////////////////////
// Management of an QURT ANYSIGNAL within HOST Protection Domain
/////////////////////////////////////////////////////////////////////

typedef int PD_QURT_HANDLE_ANYSIGNAL;

#define PD_QURT_DEVICE_ANYSIGNAL "/pd_anysignal/"

#define PD_QURT_METHOD_ANYSIGNAL_WAIT              (QDI_PRIVATE+0)
#define PD_QURT_METHOD_ANYSIGNAL_SET               (QDI_PRIVATE+1)
#define PD_QURT_METHOD_ANYSIGNAL_GET               (QDI_PRIVATE+2)
#define PD_QURT_METHOD_ANYSIGNAL_CLEAR             (QDI_PRIVATE+3)

/**
API, pd_qurt_anysignal_init
@param[in] a1.ptr/const char* NULL Terminated Device Name
@return
PD_QURT_HANDLE_ANYSIGNAL -- Opaque Device Handle
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
PD_QURT_HANDLE_ANYSIGNAL pd_qurt_anysignal_init(const char* name);

/**
API, pd_qurt_anysignal_destroy
@param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
@return
PD_QURT_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
PD_QURT_STATUS pd_qurt_anysignal_destroy(PD_QURT_HANDLE_ANYSIGNAL handle);

/**
API, pd_qurt_anysignal_wait
@param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
@param[in] unsigned int Operation Argument
@param[in] unsigned int* Operation Result
@return
PD_QURT_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(3)))
#endif
PD_QURT_STATUS pd_qurt_anysignal_wait(PD_QURT_HANDLE_ANYSIGNAL handle, unsigned int in_mask, unsigned int* out_mask_p);

/**
API, pd_qurt_anysignal_set
@param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
@param[in] unsigned int Operation Argument
@param[in] unsigned int* Operation Result
@return
PD_QURT_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(3)))
#endif
PD_QURT_STATUS pd_qurt_anysignal_set(PD_QURT_HANDLE_ANYSIGNAL handle, unsigned int in_mask, unsigned int* out_mask_p);

/**
API, pd_qurt_anysignal_get
@param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
@param[in] unsigned int* Operation Result
@return
PD_QURT_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(2)))
#endif
PD_QURT_STATUS pd_qurt_anysignal_get(PD_QURT_HANDLE_ANYSIGNAL handle, unsigned int* out_mask_p);

/**
API, pd_qurt_anysignal_clear
@param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
@param[in] unsigned int Operation Argument
@param[in] unsigned int* Operation Result
@return
PD_QURT_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(3)))
#endif
PD_QURT_STATUS pd_qurt_anysignal_clear(PD_QURT_HANDLE_ANYSIGNAL handle, unsigned int in_mask, unsigned int* out_mask_p);

/**
API, Initialization of service prior to use
@return
None.
*/
void pd_qurt_anysignal_driver_init(void);

/**
Termination of service following use
@return
None.
*/
void pd_qurt_anysignal_driver_term(void);

/////////////////////////////////////////////////////////////////////
// Management of an QURT PIMUTEX within HOST Protection Domain
/////////////////////////////////////////////////////////////////////

typedef int PD_QURT_HANDLE_PIMUTEX;

#define PD_QURT_DEVICE_PIMUTEX "/pd_pimutex/"

#define PD_QURT_METHOD_PIMUTEX_LOCK                (QDI_PRIVATE+0)
#define PD_QURT_METHOD_PIMUTEX_UNLOCK              (QDI_PRIVATE+1)
#define PD_QURT_METHOD_PIMUTEX_TRY_LOCK            (QDI_PRIVATE+2)

/**
API, pd_qurt_pimutex_init
@param[in] a1.ptr/const char* NULL Terminated Device Name
@return
PD_QURT_HANDLE_PIMUTEX -- Opaque Device Handle
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
PD_QURT_HANDLE_PIMUTEX pd_qurt_pimutex_init(const char* name);

/**
API, pd_qurt_pimutex_destroy
@param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
@return
PD_QURT_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
PD_QURT_STATUS pd_qurt_pimutex_destroy(PD_QURT_HANDLE_PIMUTEX handle);

/**
API, pd_qurt_pimutex_lock
@param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
@return
PD_QURT_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
PD_QURT_STATUS pd_qurt_pimutex_lock(PD_QURT_HANDLE_PIMUTEX handle);

/**
API, pd_qurt_pimutex_unlock
@param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
@return
PD_QURT_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
PD_QURT_STATUS pd_qurt_pimutex_unlock(PD_QURT_HANDLE_PIMUTEX handle);

/**
API, pd_qurt_pimutex_try_lock
@param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
@param[in] int* Operation Result
@return
PD_QURT_STATUS -- Error Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(2)))
#endif
PD_QURT_STATUS pd_qurt_pimutex_try_lock(PD_QURT_HANDLE_PIMUTEX handle, int* out_result_p);

/**
API, Initialization of service prior to use
@return
None.
*/
void pd_qurt_pimutex_driver_init(void);

/**
Termination of service following use
@return
None.
*/
void pd_qurt_pimutex_driver_term(void);

#if defined(__cplusplus)
}
#endif

#endif
