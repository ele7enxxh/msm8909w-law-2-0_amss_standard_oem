#ifndef HALXPU2DATATYPES_H
#define HALXPU2DATATYPES_H
/**

@file   HALxpu2DataTypes.h

@brief  This module provides the data types for XPU2.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/kernel/xpu2/hal/inc/HALxpu2DataTypes.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2012/11/12   sc      Updated with uint64
2011/11/22   sc      Initial revision

===========================================================================
Copyright (c) 2011-2012
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================
*/
/*
 * Assembly wrapper
 */
#ifndef _ARM_ASM_

/*
 * C++ wrapper
 */
#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE   1   /* Boolean true value. */
#define FALSE  0   /* Boolean false value. */

#ifndef NULL
  #define NULL  0
#endif

/* -----------------------------------------------------------------------
** Standard Types
** ----------------------------------------------------------------------- */

#ifndef _UINT64_DEFINED
typedef  unsigned long long  uint64;     /* Unsigned 64 bit value */
#define _UINT64_DEFINED
#endif
#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif
#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif
#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif
#ifndef _INT32_DEFINED
typedef  signed long int    int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif
#ifndef _BOOL32_DEFINED
typedef  unsigned long int  bool32;      /* boolean, 32 bit (TRUE or FALSE) */
#define _BOOL32_DEFINED
#endif
#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;     /* Boolean value type. */
#define _BOOLEAN_DEFINED
#endif


#ifdef __cplusplus
}
#endif

#endif /* !_ARM_ASM_ */

#endif // HALXPU2DATATYPES_H
