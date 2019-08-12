#ifndef SIMLOCK_VALIDITY_H
#define SIMLOCK_VALIDITY_H
/*===========================================================================


            S I M L O C K   V A L I D I T Y   H E A D E R


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_validity.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/08/15   bcho    Free memory on reception of task stop sig
07/06/15   bcho    Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_SIMLOCK

#include "simlock_common.h"


/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_VALIDITY_INIT

DESCRIPTION
  This function initializes critical section for valid IMSI list

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_validity_init
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_VALIDITY_CHECK_IMSI

DESCRIPTION
  This function checks whether subscription related to IMSI is valid or not.
  If this API is invoked before SIMLOCK algo run, it would return FALSE as
  it is not concluded by SIMLOCK module whether subscription is valid or not.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If subscription, related to IMSI, is valid.
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_validity_check_imsi
(
  simlock_imsi_type imsi
);

/*===========================================================================
FUNCTION SIMLOCK_VALIDITY_UPDATE_IMSI_INFO

DESCRIPTION
  This function updates IMSI validity info

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_validity_update_imsi_info
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  const simlock_run_algorithm_msg_resp_type  * resp_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_VALIDITY_DEINIT

DESCRIPTION
  This function frees memory allocated to store IMSI list

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_validity_deinit
(
  void
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_VALIDITY_H */
