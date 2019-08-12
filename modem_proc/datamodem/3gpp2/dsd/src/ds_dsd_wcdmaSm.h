#ifndef DS_DSD_WCDMASM_H
#define DS_DSD_WCDMASM_H
/*===========================================================================

                    D S _ D S D _ W C D M A S M . H

DESCRIPTION


EXTERNALIZED FUNCTIONS

 Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_wcdmaSm.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/11    sk    Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ds_dsd_common.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_WCDMASM_CREATE_SM_INSTANCE

DESCRIPTION   This function creates WCDMA SM instance corresponding to subs id

DEPENDENCIES  None.

RETURN VALUE  If sucess, statemachine pointer.
              Else, NULL.

SIDE EFFECTS  1. Creates SM instance
              2. Variables are initialized to default value.
===========================================================================*/
ds_dsd_sm_type* ds_dsd_wcdmaSm_create_sm_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_WCDMASM_INIT

DESCRIPTION   This function initializes WCDMA state machine

DEPENDENCIES  Event Dispatcher module is initialized already.

RETURN VALUE  None.

SIDE EFFECTS  Registered with event dispatcher module to get notification
===========================================================================*/
void ds_dsd_wcdmaSm_init(void);

#endif /* DS_DSD_WCDMASM_H */
