#ifndef DS_DSD_WLANSM_H
#define DS_DSD_WLANSM_H
/*===========================================================================

                    D S _ D S D _ W L A N S M . H

DESCRIPTION


EXTERNALIZED FUNCTIONS

 Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_wlanSm.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
17/10/12    sk    Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ds_dsd_common.h"
#include "ds_dsd_ext_i.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_GET_WLAN_AVAILABLE

DESCRIPTION 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  
===========================================================================*/
boolean ds_dsd_get_wlan_available
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_WLANSM_GET_ADDRESS

DESCRIPTION   This is the function registered with common handler table.

DEPENDENCIES  None.

RETURN VALUE  WLAN address.

SIDE EFFECTS  NONE
===========================================================================*/
boolean ds_dsd_wlansm_get_address
(
  ds_dsd_ext_wlan_addr_type  *addr,
  boolean                     previous,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_WLANSM_COMPARE_ADDRESS

DESCRIPTION   This is the util function to compare WLAN address.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  NONE
===========================================================================*/
boolean ds_dsd_wlansm_compare_address
(
  ds_dsd_ext_wlan_addr_type *addr1,
  ds_dsd_ext_wlan_addr_type *addr2,
  boolean                   *result
);

/*===========================================================================
FUNCTION      DS_DSD_WLANSM_TIMER_CB

DESCRIPTION   The is the function to process timer expiry events of this
              state machine.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds_dsd_wlanSm_timer_cb
(
  unsigned long               timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_WLANSM_CREATE_SM_INSTANCE

DESCRIPTION   This function creates WLAN SM instance corresponding to subs id

DEPENDENCIES  None.

RETURN VALUE  If sucess, statemachine pointer.
              Else, NULL.

SIDE EFFECTS  1. Creates SM instance
              2. Variables are initialized to default value.
===========================================================================*/
ds_dsd_sm_type* ds_dsd_wlanSm_create_sm_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS_DSD_WLANSM_INIT

DESCRIPTION   This function initializes WLAN state machine

DEPENDENCIES  Event Dispatcher module is initialized already.

RETURN VALUE  None.

SIDE EFFECTS  Registered with event dispatcher module to get notification
===========================================================================*/
void ds_dsd_wlanSm_init(void);

#endif /* FEATURE_DATA_WLAN_MAPCON */

#endif /* DS_DSD_WLANSM_H */
