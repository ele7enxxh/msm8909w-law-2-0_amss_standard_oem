#ifndef QURT_ELITE_TYPES_H
#define QURT_ELITE_TYPES_H

/*======================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */
/**
@file qurt_elite_types.h

@brief This file contains basic types and pre processor macros.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_types.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
07/09/10   mwc     Created file.
========================================================================== */

#include "mmdefs.h"
#include "adsp_error_codes.h"

#if defined(COMDEF_H) /* guards against a known re-definer */
#define _BOOLEAN_DEFINED
#define _UINT32_DEFINED
#define _UINT16_DEFINED
#define _UINT8_DEFINED
#define _INT32_DEFINED
#define _INT16_DEFINED
#define _INT8_DEFINED
#define _UINT64_DEFINED
#define _INT64_DEFINED
#define _BYTE_DEFINED
#endif /* #if !defined(COMDEF_H) */

/* -----------------------------------------------------------------------
** Standard Types
** ----------------------------------------------------------------------- */

/* The following definitions are the same accross platforms.  This first
** group are the sanctioned types.
*/
#ifndef boolean
#ifndef _BOOLEAN_DEFINED
typedef  bool_t      boolean;    /* Boolean value type. */
#define _BOOLEAN_DEFINED         /* Boolean value type flag. */
#endif
#endif

#ifndef uint32
#ifndef _UINT32_DEFINED
typedef  uint32_t  uint32;       /* Unsigned 32-bit value. */
#define _UINT32_DEFINED          /* Unsigned 32-bit value flag. */
#endif
#endif

#ifndef uint16
#ifndef _UINT16_DEFINED
typedef  uint16_t     uint16;    /* Unsigned 16-bit value. */
#define _UINT16_DEFINED          /* Unsigned 16-bit value flag. */
#endif
#endif

#ifndef uint8
#ifndef _UINT8_DEFINED
typedef  uint8_t      uint8;     /* Unsigned 8-bit value. */
#define _UINT8_DEFINED           /* Unsigned 8-bit value flag. */
#endif
#endif

#ifndef int32
#ifndef _INT32_DEFINED
typedef  int32_t    int32;       /* Signed 32-bit value */
#define _INT32_DEFINED           /* Signed 32-bit value flag. */
#endif
#endif

#ifndef int16
#ifndef _INT16_DEFINED
typedef  int16_t       int16;    /* Signed 16-bit value. */
#define _INT16_DEFINED           /* Signed 16-bit value flag. */
#endif
#endif

#ifndef int8
#ifndef _INT8_DEFINED
typedef  int8_t        int8;     /* Signed 8-bit value. */
#define _INT8_DEFINED            /* Signed 8-bit value flag. */
#endif
#endif

#ifndef uint64
#ifndef _UINT64_DEFINED
typedef  uint64_t    uint64;     /* Unsigned 64-bit value. */
#define _UINT64_DEFINED          /* Unsigned 64-bit value flag. */
#endif
#endif

#ifndef int64
#ifndef _INT64_DEFINED
typedef  int64_t            int64;  /* Signed 64-bit value. */
#define _INT64_DEFINED              /* Signed 64-bit value flag. */
#endif
#endif

#ifndef byte
#ifndef _BYTE_DEFINED
typedef  uint8_t      byte;      /* Byte type. */
#define  _BYTE_DEFINED           /* Byte type flag. */
#endif
#endif

/* -----------------------------------------------------------------------
** Function Calling Conventions
** ----------------------------------------------------------------------- */

#ifndef CDECL
#ifdef _MSC_VER
#define CDECL __cdecl
#else
#define CDECL
#endif /* _MSC_VER */
#endif /* CDECL */


/** @addtogroup qurt_elite_types
@{ */

/* -----------------------------------------------------------------------
** Preprocessor helpers
** ----------------------------------------------------------------------- */
#define __STR__(x) #x
  /**< Macro wrapper for the preprocessing operator number. */

#define __TOSTR__(x) __STR__(x)
  /**< Macro wrapper for converting a number to a string. */

#define __FILE_LINE__ __FILE__ ":" __TOSTR__(__LINE__)
  /**< Macro wrapper for generating a file name and line. */

#if defined(__GNUC__) || defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 220000)
#define QURT_ELITE_ALIGN(t, a)    t __attribute__((aligned(a)))
  /**< Macro for compiler pragma align. */

#elif defined(_MSC_VER)
#define QURT_ELITE_ALIGN(t, a)    __declspec(align(a)) t
  /**< Macro for compiler pragma align. */

#elif defined(__ARMCC_VERSION)
#define QURT_ELITE_ALIGN(t, a)    __align(a) t
  /**< Macro for compiler pragma align. */

#else
#error Unknown compiler
#endif

#define QURT_ELITE_ARRAY_SIZE(a)             ((int)((sizeof((a))/sizeof((a)[0]))))
  /**< Macro for getting the array size. */

#define QURT_ELITE_MIN(a,b)   ((a)<(b)?(a):(b))
  /**< Macro for getting the minimum of two numbers. */

#define QURT_ELITE_MAX(a,b)   ((a)>(b)?(a):(b))
  /**< Macro for getting the maximum of two numbers. */

#define QURT_ELITE_ZEROAT(p)  memset((p), 0, sizeof(*p))
  /**< Macro for setting the buffer to be all zeros. */ //lint -emacro(545,STD_ZEROAT)

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* =======================================================================
 Global definitions/forward declarations
========================================================================== */
/*
Define this keyword for GNU C.
*/
#ifdef __GNUC__
#define restrict __restrict__   /**< Macro for a compiler pragma restriction. */
#else
#define restrict                /**< Macro for a compiler pragma restriction. */
#endif


/** Error code test macro. */
#ifdef ADSP_SUCCEEDED
#undef ADSP_SUCCEEDED
#endif
#define ADSP_SUCCEEDED(x) (ADSP_EOK == (x))

/** Error code test macro. */
#ifdef ADSP_FAILED
#undef ADSP_FAILED
#endif
#define ADSP_FAILED(x) (ADSP_EOK != (x))

/**
  Macro that rounds a number to the nearest multiple of 4 (approaching infinity).

  @return
  None.

  @dependencies
  None.
*/
static inline uint32_t QURT_ELITE_ROUNDUP_MULTIPLE4(uint32_t x)
{
   return  ((x + 3 ) & -4);
}

/**
  Macro that rounds a number to the nearest multiple of 8 (approaching infinity).

  @return
  None.

  @dependencies
  None.
*/
static inline uint32_t QURT_ELITE_ROUNDUP_MULTIPLE8(uint32_t x)
{
   return  ((x + 7 ) & -8);
}


/** @} */ /* end_addtogroup qurt_elite_types */

#ifdef __cplusplus
}
#endif //__cplusplus


#endif /* #ifndef QURT_ELITE_TYPES_H */

