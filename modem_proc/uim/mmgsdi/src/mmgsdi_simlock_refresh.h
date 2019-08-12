#ifndef MMGSDI_SIMLOCK_REFRESH_H
#define MMGSDI_SIMLOCK_REFRESH_H
/*===========================================================================


                 M M G S D I   S I M L O C K   R E F R E S H   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     -----------------------------------------------------------
08/28/14   vv      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#include "mmgsdi.h"
#include "mmgsdilib_common.h"

#ifdef FEATURE_SIMLOCK

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_SIMLOCK_REFRESH_PROCESS_UNLOCK

DESCRIPTION
  This function reads the EF-DCK file and extracts the control key. If a
  valid control key exists, then it sends an unlock request to the SIMLock.
  It then updates the EF-DCK file with the default control key

DEPENDENCIES
  The SIM Lock task and command queue must be up and running

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_simlock_refresh_process_unlock
(
  mmgsdi_session_id_type     session_id
);

#endif /* FEATURE_SIMLOCK */
#endif /* MMGSDI_SIMLOCK_REFRESH_H */

