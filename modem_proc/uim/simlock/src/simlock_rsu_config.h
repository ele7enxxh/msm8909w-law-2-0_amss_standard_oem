#ifndef SIMLOCK_RSU_CONFIG_H
#define SIMLOCK_RSU_CONFIG_H
/*===========================================================================


            S I M   L O C K   R S U   C O N F I G   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_rsu_config.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/16   stv     Remote simlock support
08/19/15   stv     Restrict native simlock locking when rsu is enabled
05/15/15   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#if defined (FEATURE_SIMLOCK) && (defined (FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK))

#include "simlock.h"

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_SET_DATA

DESCRIPTION
  This function sets the simlock configuration data received in the request

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_rsu_config_set_data
(
  uint32           raw_data_len,
  const uint8   *  raw_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_RSU_CONFIG_IS_LOCK_ALLOWED

DESCRIPTION
  This function checks if the lock request is to be allowed or not.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_rsu_config_is_lock_allowed(
  const simlock_message_request_data_type *req_data_ptr
);

#endif /* FEATURE_SIMLOCK & (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) */

#endif /* SIMLOCK_RSU_CONFIG_H */
