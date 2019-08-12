#ifndef HALVMIDMTDATATYPES_H
#define HALVMIDMTDATATYPES_H
/**

@file   HALvmidmtDataTypes.h

@brief  This module provides the data types for VMIDMT.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/vmidmt/hal/inc/HALvmidmtDataTypes.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2011/11/21   sc      Initial revision

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

#endif // HALVMIDMTDATATYPES_H
