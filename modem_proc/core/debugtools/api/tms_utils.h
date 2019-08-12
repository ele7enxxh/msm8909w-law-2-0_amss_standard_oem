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
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/api/tms_utils.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "ctype.h"
#include "stdarg.h"
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"

#if defined(__cplusplus)
extern "C" {
#endif
typedef char tms_chr;                                                            /**< NULL Terminated Buffer */

/**
API, NULL Terminated Buffer Append, Result Buffer Always NULL Terminated

if (sizeof(buf) > tms_utils_chr_append(buf, sizeof(buf), ...)) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_append(buf, sizeof(buf), ...)) buffer_overflow;

@param[in]     tms_chr*                      Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3)))
#endif
size_t tms_utils_chr_append(tms_chr* out_buf_p, size_t out_buf_sz, tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Compare

if (0 == tms_utils_chr_compare(buf, sizeof(buf), ...)) match;
if (0 > tms_utils_chr_compare(buf, sizeof(buf), ...)) lexical_compare;
if (0 < tms_utils_chr_compare(buf, sizeof(buf), ...)) lexical_compare;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        int32_t                       Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3)))
#endif
int32_t tms_utils_chr_compare(tms_chr const* in_buf_p, size_t in_buf_sz, tms_chr const* in_buf2_p, size_t in_buf2_sz);

/**
API, NULL Terminated Buffer Copy, Result Buffer Always NULL Terminated

if (sizeof(buf) > tms_utils_chr_copy(buf, sizeof(buf), ...)) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_copy(buf, sizeof(buf), ...)) buffer_overflow;

@param[in]     tms_chr*                      Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1,3)))
#endif
size_t tms_utils_chr_copy(tms_chr* out_buf_p, size_t out_buf_sz, tms_chr const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Search, Last Occur of Param

if (NULL != tms_utils_chr_last(buf, sizeof(buf), 'r')) found;
if (NULL == tms_utils_chr_last(buf, sizeof(buf), 'r')) not_found;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     tms_chr                       Input Character
@return        tms_chr*                      Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
tms_chr const* tms_utils_chr_last(tms_chr const* in_buf_p, size_t in_buf_sz, const tms_chr chr);

/**
API, NULL Terminated Buffer Length

if (sizeof(buf) > tms_utils_chr_length(buf, sizeof(buf))) buffer_underflow;
if (sizeof(buf) <= tms_utils_chr_length(buf, sizeof(buf))) buffer_overflow;

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
size_t tms_utils_chr_length(tms_chr const* in_buf_p, size_t in_buf_sz);

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

/** @addtogroup tms_api @{ */

/**
API, Typedefs and Enumerations
*/
typedef char TMS_UTILS_CHR_T;
typedef char* TMS_UTILS_BUF_P;
typedef const int TMS_UTILS_BUF_SZ;
typedef const char* TMS_UTILS_FMT_P;
typedef union { unsigned long ul; long l; unsigned u; int i; } TMS_UTILS_ARG_T;
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
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((format(printf,3,4), warn_unused_result))
#endif
TMS_UTILS_RET_T tms_utils_fmt(TMS_UTILS_BUF_P out_buf_p, TMS_UTILS_BUF_SZ out_buf_sz, TMS_UTILS_FMT_P fmt_str, ...);

/**
API, Formatted Output to a Sized Buffer
NOTE: This routine treats string fmt (%s) as an integer (%x)
@param[in]  TMS_UTILS_BUF_P   Output Buffer Pointer (NULL Terminated on Output)
@param[in]  TMS_UTILS_BUF_SZ  Output Buffer Size
@param[in]  TMS_UTILS_FMT_P   Format Template Pointer (NULL Terminated)
@param[in]  ...               Format Template Expansion Arguments
@return     TMS_UTILS_RET_T   Byte Count Output into Buffer Pointer (>=0) w/ NULL Terminator, Error Indications (<0)
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((format(printf,3,4), warn_unused_result))
#endif
TMS_UTILS_RET_T tms_utils_fmt_nostring(TMS_UTILS_BUF_P out_buf_p, TMS_UTILS_BUF_SZ out_buf_sz, TMS_UTILS_FMT_P fmt_tpl, ...);

/**
API, Scan Input and Return Numeric Representation Decimal
@param[in]  TMS_UTILS_BUF_P   Input Buffer Pointer
@param[in]  TMS_UTILS_BUF_SZ  Input Buffer Size
@return     long              Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((nonnull(1), warn_unused_result))
#endif
long tms_utils_num_l(TMS_UTILS_BUF_P in_buf_p, TMS_UTILS_BUF_SZ in_buf_sz);

/**
API, Scan Input and Return Numeric Representation Hex
@param[in]  TMS_UTILS_BUF_P   Input Buffer Pointer
@param[in]  TMS_UTILS_BUF_SZ  Input Buffer Size
@return     unsigned long     Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((nonnull(1), warn_unused_result))
#endif
unsigned long tms_utils_num_lx(TMS_UTILS_BUF_P in_buf_p, TMS_UTILS_BUF_SZ in_buf_sz);

/**
API, Scan Input and Return Numeric Representation Decimal
@param[in]  TMS_UTILS_BUF_P   Input Buffer Pointer
@param[in]  TMS_UTILS_BUF_SZ  Input Buffer Size
@return     int               Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((nonnull(1), warn_unused_result))
#endif
int tms_utils_num_i(TMS_UTILS_BUF_P in_buf_p, TMS_UTILS_BUF_SZ in_buf_sz);

/**
API, Scan Input and Return Numeric Representation Hex
@param[in]  TMS_UTILS_BUF_P   Input Buffer Pointer
@param[in]  TMS_UTILS_BUF_SZ  Input Buffer Size
@return     unsigned int      Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((nonnull(1), warn_unused_result))
#endif
unsigned int tms_utils_num_ix(TMS_UTILS_BUF_P in_buf_p, TMS_UTILS_BUF_SZ in_buf_sz);

/**
API, Utility Hash Function

tms_chr const buf[] = "string initializer";
uint32_t hash = tms_utils_hash(buf, sizeof(buf));

@param[in]     tms_chr*                      Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        uint32_t                      Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
uint32_t tms_utils_hash(tms_chr const* in_buf_p, size_t in_buf_sz);

/** @} end_addtogroup tms_api */

#if defined(__cplusplus)
}
#endif

#endif
