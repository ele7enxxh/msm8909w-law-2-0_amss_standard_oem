#ifndef SIMLOCK_MODEM_OTA_H
#define SIMLOCK_MODEM_OTA_H
/*===========================================================================


            S I M L O C K   M O D E M   O T A   H E A D E R


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_modem_ota.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/03/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_common.h"
#include "simlock_modem_lib.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_MODEM_OTA_PROCESS_UNLOCK

DESCRIPTION
  This function process the OTA unlock request. It first gets the IMEI of
  the device and compares with that in the OTA request. If they match,
  then it retrieves the CK from the request data and sends an unlock
  request to the SIMLOCK engine

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_modem_ota_process_unlock(
  const simlock_ota_unlock_data_type * ota_unlock_data_ptr,
  simlock_slot_enum_type               slot,
  simlock_ota_unlock_resp_type         unlock_resp_data
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_MODEM_OTA_H */

