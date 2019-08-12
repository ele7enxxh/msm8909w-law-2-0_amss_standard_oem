#ifndef SIMLOCK_POLICY_H
#define SIMLOCK_POLICY_H
/*===========================================================================


            S I M L O C K   P O L I C Y  H E A D E R


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_policy.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/08/16   bcho    Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_common.h"
#include "simlock_config.h"
#include "simlock_platform.h"

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_APPLY_CONFIG_POLICY

DESCRIPTION
  This function updates the validity of a subscription on a card, based on
  the slot and the subscription policy set in the SIMLock feature.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_apply_config_policy
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  simlock_config_data_type                   * config_data_ptr,
  simlock_run_algorithm_msg_resp_type        * result_msg_ptr
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_POLICY_H */

