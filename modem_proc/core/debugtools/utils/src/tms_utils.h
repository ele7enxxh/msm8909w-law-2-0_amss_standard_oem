#ifndef TMS_UTILS_H
#define TMS_UTILS_H
/**
@file tms_utils.h
@brief This file contains the API for the TMS Utilities API 0.x
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 QUALCOMM Technologies Incorporated.
All rights reserved.
QUALCOMM Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/utils/src/tms_utils.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "stdarg.h"

#if defined(__cplusplus)
extern "C" {
#endif

// Supported Template Format Expansions
//
// %[-][0][ww][type]
//
// [-] left justification within field
// [0] zero pad numeric output
// [www] field width
// [type] field type
// c - character argument
// s - null terminated buffer pointer argument
// b - binary int argument
// d - decimal signed int argument
// o - octal int argument
// x - hex unsigned int argument
// ld - decimal signed long argument
// lx - hex unsigned long argument
// X - hex unsigned int argument (caps expansion)
// lX - hex unsigned long argument (caps expansion)

// Use Example
//
// TMS_UTILS_RET_T count;
// TMS_UTILS_CHR_T buffer[256];
//
// if (TMS_UTILS_BUF_SZ_ZERO < (count = tms_utils_fmt(buffer, sizeof(buffer), "%d", 12345)))
// {
//  PRINT("%d, %d == strlen(\"%s\")\n", count, strlen(buffer), buffer);
// }
// else
// {
//  ERROR("no output\n");
// }

/** @addtogroup err_api @{ */

/**
API, Typedefs and Enumerations
*/
typedef char TMS_UTILS_CHR_T;
typedef char* TMS_UTILS_BUF_P;
typedef const int TMS_UTILS_BUF_SZ;
typedef const char* TMS_UTILS_FMT_P;
typedef union { unsigned long ul; long l; } TMS_UTILS_ARG_T;
typedef int TMS_UTILS_RET_T;

#define TMS_UTILS_BUF_SZ_ZERO       0
#define TMS_UTILS_RET_SUCCESS       0
#define TMS_UTILS_RET_ERROR         -1
#define TMS_UTILS_NULL              (void*)0

/**
API, Formatted Output to a Sized Buffer
NOTE: Caller of routine supplies NHLOS protections to Output Buffer if
required. The routine itself does not carry any specific NHLOS details.
@param[in]  TMS_UTILS_BUF_P   Output Buffer Pointer (NULL Terminated on Output)
@param[in]  TMS_UTILS_BUF_SZ  Output Buffer Size
@param[in]  TMS_UTILS_FMT_P   Format Template Pointer (NULL Terminated)
@param[in]  ...               Format Template Expansion Arguments
@return     TMS_UTILS_RET_T   Byte Count Output into Buffer Pointer (>=0) w/ NULL Terminator, Error Indications (<0)
*/
#if __GNUC__ > 3
// tech areas fix their software warnings; unused return result is potential buffer overflow
__attribute__((format(printf,3,4), warn_unused_result))
#endif
TMS_UTILS_RET_T tms_utils_fmt(TMS_UTILS_BUF_P out_buf_p, TMS_UTILS_BUF_SZ out_buf_sz, TMS_UTILS_FMT_P fmt_str, ...);

/** @} end_addtogroup err_api */

#if defined(__cplusplus)
}
#endif

#endif
