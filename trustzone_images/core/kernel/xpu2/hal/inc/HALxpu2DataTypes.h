#ifndef HALXPU2DATATYPES_H
#define HALXPU2DATATYPES_H
/**

@file   HALxpu2DataTypes.h

@brief  This module provides the data types for XPU2.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/xpu2/hal/inc/HALxpu2DataTypes.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2012/11/08   sc      Updated with uint64
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

typedef  unsigned long long uint64;      /* Unsigned 64 bit value */
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
typedef  signed long int    int32;       /* Signed 32 bit value */
typedef  unsigned long int  bool32;      /* boolean, 32 bit (TRUE or FALSE) */
typedef  unsigned char      boolean;     /* Boolean value type. */

#ifdef __cplusplus
}
#endif

#endif /* !_ARM_ASM_ */

#endif // HALXPU2DATATYPES_H
