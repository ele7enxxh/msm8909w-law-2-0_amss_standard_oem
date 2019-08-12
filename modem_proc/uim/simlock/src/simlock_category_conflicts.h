#ifndef SIMLOCK_CATEGORY_CONFLICTS_H
#define SIMLOCK_CATEGORY_CONFLICTS_H
/*===========================================================================


            S I M L O C K   C A T E G O R Y   C O N F L I C T S

                              H E A D E R


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_category_conflicts.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_SIMLOCK

#include "simlock.h"
#include "simlock_config.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_VERIFY_CODES_FOR_CONFLICTS

DESCRIPTION
  This function verifies if the lock codes being added for a category do not
  have any conflicts with the already enabled codes.
  For example, if the 3GPP NW category is enabled only for mcc1-mnc1
  then the 3GPP SP category can be enabled for mcc1-mnc1-sp1 but not
  for mcc2-mnc2-sp2
 
DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_verify_codes_for_conflicts
(
  const simlock_category_data_type  * category_data_ptr,
  const simlock_config_data_type    * config_data_ptr,
  simlock_slot_enum_type              slot
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_CATEGORY_CONFLICTS_H */

