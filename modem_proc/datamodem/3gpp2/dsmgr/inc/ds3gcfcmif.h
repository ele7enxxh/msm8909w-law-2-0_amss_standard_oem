#ifndef DS3GCFCMIF_H
#define DS3GCFCMIF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             3G  DATA  SERVICES  CFCM INTERFACE

GENERAL DESCRIPTION
  This software unit acts as interface to the MCS CFCM module.

 Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gcfcmif.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/24/14   vm      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "sys.h"
#include "dstask_v.h"

#include "cfcm.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------
  Enum for ds3gcfcmif mitigation level state type
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GCFCMIF_ML_STATE_MIN = 0,
  DS3GCFCMIF_ML_STATE_ML0 = DS3GCFCMIF_ML_STATE_MIN,
  DS3GCFCMIF_ML_STATE_ML1,
  DS3GCFCMIF_ML_STATE_ML2,
  DS3GCFCMIF_ML_STATE_ML3,
  DS3GCFCMIF_ML_STATE_MAX = DS3GCFCMIF_ML_STATE_ML3
}ds3gcfcmif_ml_state_type;


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GCFCMIF_INIT

DESCRIPTION   This function initializes the DS3G CFCM IF module. This would
              be invoked during dstask init.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcfcmif_init( void );

/*===========================================================================
FUNCTION      DS3GCFCMIF_GET_ML_SM_STATE

DESCRIPTION   This function returns the state of Thermal Runaway Mitigation 
              level state machine

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds3gcfcmif_ml_state_type ds3gcfcmif_get_ml_sm_state( void );

/*===========================================================================
FUNCTION      DS3GCFCMIF_PROCESS_THERMAL_RUNAWAY_IND_CMD

DESCRIPTION   This function processes the thermal runaway indication command.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcfcmif_process_thermal_runaway_ind_cmd
(
  ds_cmd_type  *cmd_ptr
);


#endif /* DS3GCFCMIF_H */

