#ifndef AKAALGOBIT_H
#define AKAALGOBIT_H

/*===========================================================================

DESCRIPTION
  This file contains bit manupulation functions

                                                        
Copyright (c) 2004-2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/akaalgo/src/AKAAlgoBit.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
                
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/* <EJECT> */


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* AA -   AKAAlgo*/

typedef unsigned long AAword[2];
typedef unsigned long AA_qword[2];


#define AA_bitsize(type) (sizeof(type) * 8)

/*===========================================================================
MACRO AA_copymask

DESCRIPTION
   Creates a mask of bits sized to the number of bits in the given type.
===========================================================================*/
#define AA_copymask(type) ((0xffffffff) >> (32 - AA_bitsize(type)))

/*===========================================================================
MACRO MASK

DESCRIPTION
   Masks the bits in data at the given offset for given number of width bits.
===========================================================================*/
#define AA_MASK(width, offset, data) \
    /*lint -e701 shift left  of signed quantity  */  \
    /*lint -e702 shift right of signed quantity  */  \
    /*lint -e572 Excessive shift value           */  \
    /*lint -e573 Signed-unsigned mix with divide */  \
    /*lint -e506 Constant value boolean          */  \
    /*lint -e649 Sign fill during constant shift */  \
                                                     \
   (((width) == AA_bitsize(data)) ? (data) :   \
   ((((AA_copymask(data) << (AA_bitsize(data) - ((width) % AA_bitsize(data)))) & AA_copymask(data)) >>  (offset)) & (data))) \
                     \
    /*lint +e701 */  \
    /*lint +e702 */  \
    /*lint +e572 */  \
    /*lint +e573 */  \
    /*lint +e506 */  \
    /*lint +e649 */  

/*===========================================================================
MACRO AA_MASK_AND_SHIFT

DESCRIPTION
   Same as the macro MASK except also shifts the data in the result by the
   given number of shift bits.
===========================================================================*/
#define AA_MASK_AND_SHIFT(width, offset, shift, data)  \
    /*lint -e504 Unusual shifter value */  \
                  ((((signed) (shift)) < 0) ?       \
                    AA_MASK((width), (offset), (data)) << -(shift) :  \
                    AA_MASK((width), (offset), (data)) >>  (((unsigned) (shift)))) \
    /*lint +e504 */

/*===========================================================================
MACRO AA_MASK_AND_SHIFT_RIGHT

DESCRIPTION
   Same as the macro MASK except also shifts the data in the result right
   by the given number of shift bits.
===========================================================================*/
#define AA_MASK_AND_SHIFT_RIGHT(width, offset, shift, data)   \
                    (AA_MASK((width), (offset), (data)) >>  ((unsigned)(shift)))

/*===========================================================================
MACRO AA_MASK_AND_SHIFT_LEFT

DESCRIPTION
   Same as the macro MASK except also shifts the data in the result left
   by the given number of shift bits.
===========================================================================*/
#define AA_MASK_AND_SHIFT_LEFT(width, offset, shift, data)    \
                    (AA_MASK((width), (offset), (data)) << ((unsigned)(shift)))

/*===========================================================================
MACRO AA_MASK_B

DESCRIPTION
   Masks the number of bits give by length starting at the given offset.
   Unlike MASK and AA_MASK_AND_SHIFT, this macro only creates that mask, it
   does not operate on the data buffer.
===========================================================================*/
#define AA_MASK_B(offset, len) \
  ((0xff >> offset) & (0xff << (8 - (offset + len))))

#define AA_QW_ALIGNED(qw) (!(((int) qw) << 30))
#define AA_QW_CVT_N2Q(p_n64) ((unsigned long *)(p_n64))


uint32 AA_qw_hi
(
  AA_qword qw
    /* qword to return the high 32 bits from */
);

uint32 AA_qw_lo
(
  AA_qword qw
   /* qword to return the low 32 bits from  */
);



void AA_b_packq(
   AAword src, 
   byte dst[], 
   word pos, 
   word len 
);

void AA_b_unpackq(
   byte src[], 
   word pos, 
   word len, 
   AAword qw
);

void AA_b_packw(
   word src, 
   byte dst[], 
   word pos, 
   word len 
);

word AA_b_unpackw(
   byte src[], 
   word pos, 
   word len
);

void AA_qw_set
(
  AA_qword qw,
    /* the qword to set                */
  uint32 hi,
    /* the value for the high 32 bits */
  uint32 lo
    /* the value for the low 32 bits  */
);


#ifdef __cplusplus
}
#endif // __cplusplus


#endif /* AKAALGOBIT_H */

