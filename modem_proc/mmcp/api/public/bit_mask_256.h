#ifndef BIT_MAP_256_H
#define BIT_MAP_256_H
/*==========================================================================

              C A L L   M A N A G E R   H E A D E R   F I L E

===========================================================================*/

/**
  @file bit_map_256.h

  Definitions for a 256-bit bitmask type implementation
*/
/*===========================================================================
Copyright (c) 2002 - 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/bit_mask_256.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/16   rm      Created
===========================================================================*/

#include "comdef.h"
#include "stdlib.h"

typedef struct
{
  uint64  bits_1_64;
  uint64  bits_65_128;
  uint64  bits_129_192;
  uint64  bits_193_256;
} bit_mask_256_t;

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
);

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
);

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
);

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
  bit_mask_256_t  const    *bit_mask_ptr_A,
  bit_mask_256_t  const    *bit_mask_ptr_B
);

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
  bit_mask_256_t  const   *bit_mask_ptr_A,
  bit_mask_256_t  const   *bit_mask_ptr_B
);

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
  bit_mask_256_t  const    *bit_mask_ptr
);

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
  bit_mask_256_t      *bit_mask_ptr
);

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
  bit_mask_256_t      *bit_mask_ptr
);

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
  bit_mask_256_t  const    *bit_mask_ptr_A,
  bit_mask_256_t  const    *bit_mask_ptr_B
);

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
);

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
);

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
);

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
  bit_mask_256_t  const   *bit_mask_ptr_A,
  bit_mask_256_t  const   *bit_mask_ptr_B
);

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
);

#endif /* #ifndef BIT_MAP_256_H */
