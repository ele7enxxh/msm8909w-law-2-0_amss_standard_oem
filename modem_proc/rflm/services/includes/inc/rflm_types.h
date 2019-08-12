/*!
  @file
  rflm_types.h

  @brief
  RFLM type definitions.
  This file should not be included standalone - use rflm.h instead,
  which includes this file.

*/

/*===========================================================================

  Copyright (c) 2008-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/includes/inc/rflm_types.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2014-02-18   kg      Added typedef for dword
2013-07-18   JC      Port from modem_fw_types.h

===========================================================================*/

/* This file should only be included as part of rflm.h. Enforce the 
   order (it's important that this be before the #define RFLM_TYPES_H). */
#include "rflm.h"

#ifndef RFLM_TYPES_H
#define RFLM_TYPES_H

#if (RFLM_FEATURE_BUILD_MODE != RFLM_FEATURE_BUILD_NORMAL)
typedef unsigned long dword;
#endif


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#ifdef __cplusplus
// This shouldn't matter, as there are no function prototypes in this file.
// But Lint gets confused when mixing C and C++ otherwise.
extern "C" {
#endif

/*! @brief Standard types.
*/
#ifndef _BOOLEAN_DEFINED
#define _BOOLEAN_DEFINED
typedef  unsigned char      boolean;     /* Boolean value type. */
typedef  boolean            Boolean;
#endif

#ifndef _INT8_DEFINED
#define _INT8_DEFINED
typedef  signed char        int8;        /* Signed 8  bit value */
#endif

#ifndef _UINT8_DEFINED
#define _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#endif

#ifndef _INT16_DEFINED
#define _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
#endif

#ifndef _UINT16_DEFINED
#define _UINT16_DEFINED
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#endif

#ifndef _INT32_DEFINED
#define _INT32_DEFINED
typedef  signed long        int32;       /* Signed 32 bit value */
#endif

#ifndef _UINT32_DEFINED
#define _UINT32_DEFINED
typedef  unsigned long      uint32;      /* Unsigned 32 bit value */
#endif

#ifndef _INT64_DEFINED
#define _INT64_DEFINED
typedef  signed long long   int64;       /* Signed 64 bit value */
#endif

#ifndef _UINT64_DEFINED
#define _UINT64_DEFINED
typedef  unsigned long long uint64;      /* Unsigned 64 bit value */
#endif

//#define _BYTE_DEFINED

/*! @brief Range of standard types.
*/
#define MAX_INT8            (0x7F)
#define MIN_INT8            (-0x80)
#define MAX_UINT8           (0xFF)
#define MAX_INT16           (0x7FFF)
#define MIN_INT16           (-0x8000)
#define MAX_UINT16          (0xFFFF)
#define MAX_INT32           (0x7FFFFFFF)
#define MIN_INT32           (-0x80000000)
#define MAX_UINT32          (0xFFFFFFFF)
#define MAX_INT64           (0x7FFFFFFFFFFFFFFFLL)
#define MIN_INT64           (-0x8000000000000000uLL)
#define MAX_UINT64          (0xFFFFFFFFFFFFFFFFuLL)


/*! @brief QDSP6 32-bit vector type.
*/
typedef union ALIGN(4) {
  int8    b[4];
  uint8  ub[4];
  int16   h[2];
  uint16 uh[2];
  int32   w;
  uint32 uw;
  /* Aliases for halfwords */
  struct {
    int16 I;
    int16 Q;
  }; 
  struct {
    int16 re;
    int16 im;
  };
  struct {
    int16 real;
    int16 imag;
  };
  struct {
    int16 Real;
    int16 Imag;
  };
} vect32;


/*! @brief vect32 arrays are often accessed as vect64, but functions taking
    (vect32 *) don't have alignment information. Use this type to provide that.

    Do NOT use this as the type of your array, or padding will be inserted
    between each vect32. 
    e.g.
    // aligned_vect32 myarray[16];  // WRONG! Will pad between array items
    vect32 myarray[16] ALIGN(8);  // Correct - align only start of array.
    func((aligned_vect32 *) myarray);  // Cast on call

    void func(aligned_vect32 *buf32) {
       vect64 *buf = (vect64 *) buf;  // Cast before using
       *buf++ = ...;
    }
*/
typedef vect32 aligned_vect32 ALIGN(8);


/*! @brief QDSP6 64-bit vector type.
*/
typedef union ALIGN(8) {
  int8    b[8];
  uint8  ub[8];
  int16   h[4];
  uint16 uh[4];
  int32   w[2];
  uint32 uw[2];
  int64   d;
  uint64 ud;
  vect32  v[2]; /* Access each half as a 32-bit vector */
  /* Aliases for words */
  struct {
    int32 I;
    int32 Q;
  };
  struct {
    int32 re;
    int32 im;
  };
  struct {
    int32 real;
    int32 imag;
  };
  struct {
    int32 Real;
    int32 Imag;
  };
} vect64;


/*! @brief Styles from the QDSP6 programmer's guide.
    Let's avoid using the following types
*/
#if 0
typedef  boolean            Boolean;
typedef  int8               Word8;
typedef  uint8              UWord8;
typedef  int16              Word16;
typedef  uint16             UWord16;
typedef  int32              Word32;
typedef  uint32             UWord32;
typedef  int64              Word64;
typedef  uint64             UWord64;
typedef  vect32             Vect32;
typedef  vect64             Vect64;
typedef  uint8              byte;
typedef  uint16             word;
typedef  uint32             dword;
#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_TYPES_H */
