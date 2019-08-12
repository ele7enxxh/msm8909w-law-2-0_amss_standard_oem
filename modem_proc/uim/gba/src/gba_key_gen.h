#ifndef GBA_KEY_GEN_H
#define GBA_KEY_GEN_H
/*===========================================================================


            G B A   K E Y  G E N E R A T I O N   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/15   lxu     Fixed various issues
11/31/14   tkl     Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gba.h"
#include "gba_lib.h"

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION GBA_KEY_GEN_KS

DESCRIPTION
  This function takes the IK and CK as input, and generates Ks.
  Caller takes care of the memory allocation for Ks pointer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_key_gen_ks (
  const gba_ik_type                * ikey_ptr,
  const gba_ck_type                * ckey_ptr,
  gba_ks_type                      * ks_ptr
);

/*===========================================================================
FUNCTION GBA_KEY_GEN_KSNAF

DESCRIPTION
  This function takes Ks, Rand, IMPI, Naf ID as input, and generates KsNAF.
  Caller takes care of the memory allocation for KsNAF pointer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_key_gen_ksnaf (
  const gba_ks_type                * ks_ptr,
  const gba_rand_type              * rand_ptr,
  const gba_impi_type              * impi_ptr,
  const gba_naf_id_type            * naf_id_ptr,
  gba_ks_naf_type                  * ksnaf_ptr
);

#endif /* GBA_KEY_GEN_H */

