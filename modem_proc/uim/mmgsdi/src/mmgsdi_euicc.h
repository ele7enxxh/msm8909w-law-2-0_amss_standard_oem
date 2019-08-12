#ifndef MMGSDI_EUICC_H
#define MMGSDI_EUICC_H
/*===========================================================================


           M M G S D I   E U I C C   F U N C T I O N   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_euicc.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/06/16   av      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/* ============================================================================
FUNCTION MMGSDI_EUICC_IS_ISDR_FOUND

DESCRIPTION
  This function attemmpts to select ISDR (which is present on an eUICC, but not
  on a non-eUICC).

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_euicc_is_isdr_found(
  mmgsdi_slot_id_enum_type           slot_id,
  boolean                            *is_isdr_found_ptr
);

#endif /* MMGSDI_EUICC_H */
