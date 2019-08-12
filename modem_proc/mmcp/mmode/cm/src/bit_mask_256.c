/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   B I T M A S K 2 5 6   M O D U L E

GENERAL DESCRIPTION
  This module provides the APIs to manipulate 256 bit bitmask data structure.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1998 - 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.
Qualcomm Confidential and Proprietary

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.


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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/bit_mask_256.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/16   rm      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "bit_mask_256.h"


#define BIT_MASK_256_OP_IS     0
#define BIT_MASK_256_OP_SET    1
#define BIT_MASK_256_OP_CLEAR  2

/*===========================================================================
=============================================================================
============================ BITMASK APIS ===================================
=============================================================================
===========================================================================*/

/*===========================================================================
FUNCTION bit_mask_256_bit_operation

DESCRIPTION
  Perform the selected operation on the bitmask.
  - BIT_MASK_256_OP_IS
  - BIT_MASK_256_OP_SET
  - BIT_MASK_256_OP_CLEAR

DEPENDENCIES
  None

RETURN VALUE
  TRUE is operation successful.

SIDE EFFECTS
  None

===========================================================================*/
boolean bit_mask_256_bit_operation(
  bit_mask_256_t      *bit_mask_ptr,
  size_t               bit_num,
  int                  op
)
{
  size_t              mask_offset; /* 0-3  */
  size_t              mask_bit;    /* 0-63 */
  uint64             *mask_entry;
  uint64              mask;

  mask_entry  = &bit_mask_ptr->bits_1_64;
  mask_offset = bit_num / 64;
  mask_bit    = bit_num % 64;

  mask        = ((uint64)1 << mask_bit);
  mask_entry += mask_offset;

  if (BIT_MASK_256_OP_CLEAR == op)
  {
    *mask_entry &= ~mask;
    return TRUE;
  }
  else if (BIT_MASK_256_OP_SET == op)
  {
    *mask_entry |= mask;
    return TRUE;
  }
  else if (BIT_MASK_256_OP_IS == op)
  {
    return ((*mask_entry & mask) != 0);
  }

  return FALSE;
}

/*===========================================================================
FUNCTION bit_mask_256_bit_is_present

DESCRIPTION
  Check if the bit bit_num is set in the bitmask.

DEPENDENCIES
  None

RETURN VALUE
  TRUE is operation successful.

SIDE EFFECTS
  None

===========================================================================*/
boolean bit_mask_256_bit_is_present(
  bit_mask_256_t      *bit_mask_ptr,
  size_t               bit_num
)
{
  return bit_mask_256_bit_operation( bit_mask_ptr,
                                     bit_num,
                                     BIT_MASK_256_OP_IS );
}

/*===========================================================================
FUNCTION bit_mask_256_set_bit

DESCRIPTION
  Set the bit bit_num in the bitmask.

DEPENDENCIES
  None

RETURN VALUE
  TRUE is operation successful.

SIDE EFFECTS
  None

===========================================================================*/
boolean bit_mask_256_set_bit(
  bit_mask_256_t      *bit_mask_ptr,
  size_t               bit_num
)
{
  return bit_mask_256_bit_operation( bit_mask_ptr,
                                     bit_num,
                                     BIT_MASK_256_OP_SET );
}

/*===========================================================================
FUNCTION bit_mask_256_clear_bit

DESCRIPTION
  Reset the bit bit_num in the bitmask.

DEPENDENCIES
  None

RETURN VALUE
  TRUE is operation successful.

SIDE EFFECTS
  None

===========================================================================*/
boolean bit_mask_256_clear_bit(
  bit_mask_256_t      *bit_mask_ptr,
  size_t               bit_num
)
{
  return bit_mask_256_bit_operation( bit_mask_ptr,
                                     bit_num,
                                     BIT_MASK_256_OP_CLEAR );
}

/*===========================================================================
FUNCTION bit_mask_256_masks_are_equal

DESCRIPTION
  Check if the same bits are set in both the bitmasks.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if bitmasks equal.

SIDE EFFECTS
  None

===========================================================================*/
boolean bit_mask_256_masks_are_equal(
  bit_mask_256_t const *bit_mask_ptr_A,
  bit_mask_256_t const *bit_mask_ptr_B
)
{
  return (   (bit_mask_ptr_A->bits_1_64    == bit_mask_ptr_B->bits_1_64)
          && (bit_mask_ptr_A->bits_65_128  == bit_mask_ptr_B->bits_65_128)
          && (bit_mask_ptr_A->bits_129_192 == bit_mask_ptr_B->bits_129_192)
          && (bit_mask_ptr_A->bits_193_256 == bit_mask_ptr_B->bits_193_256) );
}

/*===========================================================================
FUNCTION bit_mask_256_masks_overlap

DESCRIPTION
  Check if any of the same bits are set in both the bitmasks.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if bitmasks overlap

SIDE EFFECTS
  None

===========================================================================*/
boolean bit_mask_256_masks_overlap(
  bit_mask_256_t const *bit_mask_ptr_A,
  bit_mask_256_t const *bit_mask_ptr_B
)
{
  bit_mask_256_t common_bits = bit_mask_256_masks_common(bit_mask_ptr_A,bit_mask_ptr_B);
  return ( !bit_mask_256_mask_is_empty(&common_bits) );
}

/*===========================================================================
FUNCTION bit_mask_256_mask_is_subset

DESCRIPTION
  Check if all the bits set in bit_mask_ptr_B are also set in bit_mask_ptr_A.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if subset.

SIDE EFFECTS
  None

===========================================================================*/
boolean bit_mask_256_mask_is_subset(
  bit_mask_256_t const  *bit_mask_ptr_A,
  bit_mask_256_t const  *bit_mask_ptr_B
)
{
  return (   ((bit_mask_ptr_A->bits_1_64    & bit_mask_ptr_B->bits_1_64)    == bit_mask_ptr_B->bits_1_64    )
          && ((bit_mask_ptr_A->bits_65_128  & bit_mask_ptr_B->bits_65_128)  == bit_mask_ptr_B->bits_65_128  )
          && ((bit_mask_ptr_A->bits_129_192 & bit_mask_ptr_B->bits_129_192) == bit_mask_ptr_B->bits_129_192 )
          && ((bit_mask_ptr_A->bits_193_256 & bit_mask_ptr_B->bits_193_256) == bit_mask_ptr_B->bits_193_256 ) );
}

/*===========================================================================
FUNCTION bit_mask_256_mask_is_empty

DESCRIPTION
  Check if all the bits in the bitmask are reset.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if mask empty.

SIDE EFFECTS
  None

===========================================================================*/
boolean bit_mask_256_mask_is_empty(
  bit_mask_256_t const *bit_mask_ptr
)
{
  return (   (bit_mask_ptr->bits_1_64    == 0)
          && (bit_mask_ptr->bits_65_128  == 0)
          && (bit_mask_ptr->bits_129_192 == 0)
          && (bit_mask_ptr->bits_193_256 == 0) );
}

/*===========================================================================
FUNCTION bit_mask_256_set_all

DESCRIPTION
  Set all the bits in the bitmask .

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void bit_mask_256_set_all(
  bit_mask_256_t  *bit_mask_ptr
)
{
  /* ULL designates it as Unsigned Long Long (ULL) to get rid of compiler
  ** warnings "literally treated as unsigned long long".**
  */
  bit_mask_ptr->bits_1_64    = 0xFFFFFFFFFFFFFFFFULL;
  bit_mask_ptr->bits_65_128  = 0xFFFFFFFFFFFFFFFFULL;
  bit_mask_ptr->bits_129_192 = 0xFFFFFFFFFFFFFFFFULL;
  bit_mask_ptr->bits_193_256 = 0xFFFFFFFFFFFFFFFFULL;
}

/*===========================================================================
FUNCTION bit_mask_256_clear_all

DESCRIPTION
  Reset all the bits in the bitmask .

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void bit_mask_256_clear_all(
  bit_mask_256_t  *bit_mask_ptr
)
{
  bit_mask_ptr->bits_1_64    = 0;
  bit_mask_ptr->bits_65_128  = 0;
  bit_mask_ptr->bits_129_192 = 0;
  bit_mask_ptr->bits_193_256 = 0;
}

/*===========================================================================
FUNCTION bit_mask_256_masks_common

DESCRIPTION
  Find the bits set in both the input bitmasks .

DEPENDENCIES
  None

RETURN VALUE
  bit_mask_256_t.

SIDE EFFECTS
  None

===========================================================================*/
bit_mask_256_t bit_mask_256_masks_common(
  bit_mask_256_t const     *bit_mask_ptr_A,
  bit_mask_256_t const     *bit_mask_ptr_B
)
{
  bit_mask_256_t local_bit_mask;

  local_bit_mask.bits_1_64    = (bit_mask_ptr_A->bits_1_64    & bit_mask_ptr_B->bits_1_64);
  local_bit_mask.bits_65_128  = (bit_mask_ptr_A->bits_65_128  & bit_mask_ptr_B->bits_65_128);
  local_bit_mask.bits_129_192 = (bit_mask_ptr_A->bits_129_192 & bit_mask_ptr_B->bits_129_192);
  local_bit_mask.bits_193_256 = (bit_mask_ptr_A->bits_193_256 & bit_mask_ptr_B->bits_193_256);

  return local_bit_mask;
}

/*===========================================================================
FUNCTION bit_mask_256_add_mask

DESCRIPTION
  Set all the bits set in bit_mask_ptr_B in bit_mask_ptr_A as well.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void bit_mask_256_add_mask(
  bit_mask_256_t       *bit_mask_ptr_A,
  bit_mask_256_t const *bit_mask_ptr_B
)
{
  bit_mask_ptr_A->bits_1_64    |= bit_mask_ptr_B->bits_1_64;
  bit_mask_ptr_A->bits_65_128  |= bit_mask_ptr_B->bits_65_128;
  bit_mask_ptr_A->bits_129_192 |= bit_mask_ptr_B->bits_129_192;
  bit_mask_ptr_A->bits_193_256 |= bit_mask_ptr_B->bits_193_256;
}

/*===========================================================================
FUNCTION bit_mask_256_remove_mask

DESCRIPTION
  Reset all the bits set in bit_mask_ptr_B in bit_mask_ptr_A.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  bit_mask_256_remove_mask(
  bit_mask_256_t       *bit_mask_ptr_A,
  bit_mask_256_t const *bit_mask_ptr_B
)
{
  bit_mask_ptr_A->bits_1_64    &= ~(bit_mask_ptr_B->bits_1_64);
  bit_mask_ptr_A->bits_65_128  &= ~(bit_mask_ptr_B->bits_65_128);
  bit_mask_ptr_A->bits_129_192 &= ~(bit_mask_ptr_B->bits_129_192);
  bit_mask_ptr_A->bits_193_256 &= ~(bit_mask_ptr_B->bits_193_256);
}

/*===========================================================================
FUNCTION bit_mask_256_mask_xor_masks

DESCRIPTION
  Perform the bitwise XOR operation on the 2 input bitmasks and return the
  result.

DEPENDENCIES
  None

RETURN VALUE
  bit_mask_256_t.

SIDE EFFECTS
  None

===========================================================================*/
bit_mask_256_t bit_mask_256_mask_xor_masks(
  bit_mask_256_t const     *bit_mask_ptr_A,
  bit_mask_256_t const     *bit_mask_ptr_B
)
{
  bit_mask_256_t local_bit_mask;

  local_bit_mask.bits_1_64    = (bit_mask_ptr_A->bits_1_64    ^ bit_mask_ptr_B->bits_1_64);
  local_bit_mask.bits_65_128  = (bit_mask_ptr_A->bits_65_128  ^ bit_mask_ptr_B->bits_65_128);
  local_bit_mask.bits_129_192 = (bit_mask_ptr_A->bits_129_192 ^ bit_mask_ptr_B->bits_129_192);
  local_bit_mask.bits_193_256 = (bit_mask_ptr_A->bits_193_256 ^ bit_mask_ptr_B->bits_193_256);

  return local_bit_mask;
}

/*===========================================================================
FUNCTION bit_mask_256_inv_all

DESCRIPTION
  Toggle all the bits in the bitmask .

DEPENDENCIES
  None

RETURN VALUE
  bit_mask_256_t.

SIDE EFFECTS
  None

===========================================================================*/
bit_mask_256_t bit_mask_256_inv_all(
  bit_mask_256_t      *bit_mask_ptr
)
{
  bit_mask_256_t local_bit_mask;

  local_bit_mask.bits_1_64    = ~bit_mask_ptr->bits_1_64;
  local_bit_mask.bits_65_128  = ~bit_mask_ptr->bits_65_128;
  local_bit_mask.bits_129_192 = ~bit_mask_ptr->bits_129_192;
  local_bit_mask.bits_193_256 = ~bit_mask_ptr->bits_193_256;

  return local_bit_mask;
}
