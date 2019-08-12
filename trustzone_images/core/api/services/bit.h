#ifndef BIT_H
#define BIT_H
/*===========================================================================

              B I T    M A N I P U L A T I O N    S E R V I C E S

                            D E C L A R A T I O N S

DESCRIPTION
  The following declarations are for use with the Bit Manipulation Services.

Copyright (c) 1991-2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/bit.h_v   1.1   03 Oct 2001 10:18:34   rajeevg  $
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/bit.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/01    gr     Moved bit manipulation macros from bit.c into bit.h.
03/31/99    ms     Added prototypes for b_copy_ltobw_s2d0 and b_copy_ltobw_s2d0
12/15/98   jct     Removed 80186 support
11/20/98    ms     Added prototype for function b_copy_litend_to_bigend_word.
08/28/98   jct     Added updates from ARM port, added comments, moved debug
                   definitions to this header
12/20/96   rdh     Changed functions to Pascal calling convention.
07/07/92   ip      Release to DMSS PVCS.
04/29/92   gb      Large model version.
04/23/92   ip      Initial porting of file from Brassboard to DMSS.
01/23/91   rdb     Created first cut of file.

===========================================================================*/

#include "comdef.h"
#include "qw.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                            MACRO DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO bitsize

DESCRIPTION
   Computes size in bits of the specified data type.
===========================================================================*/
#define bitsize(type) (sizeof(type) * 8)

/*===========================================================================
MACRO copymask

DESCRIPTION
   Creates a mask of bits sized to the number of bits in the given type.
===========================================================================*/
#define copymask(type) ((0xffffffff) >> (32 - bitsize(type)))

/*===========================================================================
MACRO MASK

DESCRIPTION
   Masks the bits in data at the given offset for given number of width bits.
===========================================================================*/
#define MASK(width, offset, data) \
    /*lint -e701 shift left  of signed quantity  */  \
    /*lint -e702 shift right of signed quantity  */  \
    /*lint -e572 Excessive shift value           */  \
    /*lint -e573 Signed-unsigned mix with divide */  \
    /*lint -e506 Constant value boolean          */  \
    /*lint -e649 Sign fill during constant shift */  \
                                                     \
   (((width) == bitsize(data)) ? (data) :   \
   ((((copymask(data) << (bitsize(data) - ((width) % bitsize(data)))) & copymask(data)) >>  (offset)) & (data))) \
                     \
    /*lint +e701 */  \
    /*lint +e702 */  \
    /*lint +e572 */  \
    /*lint +e573 */  \
    /*lint +e506 */  \
    /*lint +e649 */  

/*===========================================================================
MACRO MASK_AND_SHIFT

DESCRIPTION
   Same as the macro MASK except also shifts the data in the result by the
   given number of shift bits.
===========================================================================*/
#define MASK_AND_SHIFT(width, offset, shift, data)  \
    /*lint -e504 Unusual shifter value */  \
                  ((((signed) (shift)) < 0) ?       \
                    MASK((width), (offset), (data)) << -(shift) :  \
                    MASK((width), (offset), (data)) >>  (((unsigned) (shift)))) \
    /*lint +e504 */

/*===========================================================================
MACRO MASK_AND_SHIFT_RIGHT

DESCRIPTION
   Same as the macro MASK except also shifts the data in the result right
   by the given number of shift bits.
===========================================================================*/
#define MASK_AND_SHIFT_RIGHT(width, offset, shift, data)   \
                    (MASK((width), (offset), (data)) >>  ((unsigned)(shift)))

/*===========================================================================
MACRO MASK_AND_SHIFT_LEFT

DESCRIPTION
   Same as the macro MASK except also shifts the data in the result left
   by the given number of shift bits.
===========================================================================*/
#define MASK_AND_SHIFT_LEFT(width, offset, shift, data)    \
                    (MASK((width), (offset), (data)) << ((unsigned)(shift)))

/*===========================================================================
MACRO MASK_B

DESCRIPTION
   Masks the number of bits give by length starting at the given offset.
   Unlike MASK and MASK_AND_SHIFT, this macro only creates that mask, it
   does not operate on the data buffer.
===========================================================================*/
#define MASK_B(offset, len) \
  ((0xff >> offset) & (0xff << (8 - (offset + len))))

/*===========================================================================
MACRO LITLEND_PTR_ADD

DESCRIPTION
   This macro adds offsets to byte pointers operating on little_endian word
   buffers. The pointer p to a word buffer should initially have been
   exclusive_ORed with 1. Further add operations on such a pointer using
   this macro will lead to the correct offset in a little endian word buffer.
===========================================================================*/
#define LITLEND_PTR_ADD(p,i)     (((((dword)(p))^1)+(i))^1)

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern void  b_packb ( byte  val, byte buf[ ], word pos, word len );
extern void  b_packw ( word  val, byte buf[ ], word pos, word len );
extern void  b_packd ( dword val, byte buf[ ], word pos, word len );
extern void  b_packq ( qword val, byte buf[ ], word pos, word len );

extern byte   b_unpackb ( byte buf[ ], word pos, word len );
extern word   b_unpackw ( byte buf[ ], word pos, word len );
extern dword  b_unpackd ( byte buf[ ], word pos, word len );
extern void   b_unpackq ( byte buf[ ], word pos, word len, qword qw );


extern void  b_copy ( 
                   void  *sbuf,
                   word  spos,
                   void  *dbuf,
                   word  dpos,
                   word  len      );

extern void  b_copy_litend_to_bigend_word ( 
                   word  *sbuf,
                   word  spos,
                   byte  *dbuf,
                   word  dpos,
                   word  len      );

extern void  b_copy_ltobw_s1d0 ( 
                   word  *sbuf,
                   byte  *dbuf,
                   word  len      );

extern void  b_copy_ltobw_s2d0 ( 
                   word  *sbuf,
                   byte  *dbuf,
                   word  len      );

#endif /* BIT_H */
