#ifndef GBA_TMPI_H
#define GBA_TMPI_H
/*===========================================================================


            G B A   T M P I   H E A D E R


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
06/10/15   tkl      Add support of IPv6 BSF address
03/12/15   tkl      Initial revision
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
FUNCTION GBA_TMPI_GET_BSF_ID

DESCRIPTION
  This function computes BSF ID from BSF address and NAF ID

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  update bsf_id data
===========================================================================*/
gba_result_enum_type gba_tmpi_get_bsf_id (
  const gba_bsf_address_type           * realm_ptr,
  gba_naf_id_type                      * bsf_id_ptr
);

/*===========================================================================
FUNCTION GBA_TMPI_CALCULATE_TMPI

DESCRIPTION
  This function construct the TMPI string from NAF

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  update bsf_id data
===========================================================================*/
gba_result_enum_type gba_tmpi_calculate_tmpi (
  const gba_ks_naf_type           *naf_ptr,
  gba_impi_type                   *tmpi_ptr
);

/*===========================================================================
FUNCTION GBA_TMPI_COMPUTE_FOR_GBA_ME

DESCRIPTION
  This function computes the TMPI for GBA_ME

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_tmpi_compute_for_gba_me(
  const gba_ks_type                       * ks_ptr,
  const gba_impi_type                     * impi_ptr,
  const gba_rand_type                     * rand_ptr,
  const gba_naf_id_type                   * bsf_id_ptr,
  gba_impi_type                           * tmpi_ptr
);

/*===========================================================================
FUNCTION GBA_TMPI_CLEAR_TMPI_STORAGE

DESCRIPTION
  This function clear the TMPI from the storage

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_tmpi_clear_tmpi_storage(
  gba_session_type                session_type
);

/*===========================================================================
FUNCTION GBA_TMPI_GET_IMPI_TMPI_PAIR

DESCRIPTION
  This function gets the TMPI from storage

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_tmpi_get_impi_tmpi_pair(
  gba_session_type                session_type,
  const gba_impi_type            *impi_ptr,
  gba_impi_type                  *tmpi_ptr
);

#endif /* GBA_TMPI_H */

