#ifndef DS3GDEVMGR_H
#define DS3GDEVMGR_H
/*===========================================================================

            DS3G DEVICE MANAGER FILE

===========================================================================*/
/*!
  @file
  ds3gdevmgr.h

  @brief
  This header file externalizes DS3G Device manager functionality.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/        
/*==============================================================================

  Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gdevmgr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
9/08/14    vs      Initial File
==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ps_sys.h"
#include "ps_sys_event.h"
#include "ps_sys_conf.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef struct
{
  boolean                   data_setting;
  boolean                   roaming_setting;
  ds_sys_apn_info_type      apn_info;
  sys_modem_as_id_e_type    subs_id;
} ds3gdevmgr_device_settings_info_type;

/*===========================================================================
FUNCTION      DS3GDEVMGR_INIT

DESCRIPTION   This function initializes the DS3G Device Manager module.
 
PARAMETERS    None.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdevmgr_init( void );

/*===========================================================================
FUNCTION      DS3GDEVMGR_GET_DATA_SETTINGS

DESCRIPTION   This function fetches the Data Settings
 
PARAMETERS    Subscription Id
 
DEPENDENCIES  None.

RETURN VALUE  Data Setting

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdevmgr_get_data_settings
(
  boolean                 *data_setting_p,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GDEVMGR_GET_ROAMING_SETTINGS

DESCRIPTION   This function fetches the Data Roaming Settings
 
PARAMETERS    Subscription Id
 
DEPENDENCIES  None.

RETURN VALUE  Roaming Setting

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdevmgr_get_roaming_settings
(
  boolean                 *roaming_setting_p,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GDEVMGR_GET_APN_SETTINGS

DESCRIPTION   This function fetches the APN Settings
 
PARAMETERS    Subscription Id
 
DEPENDENCIES  None.

RETURN VALUE  APN Settings

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdevmgr_get_apn_settings
(
  ds_sys_apn_info_type   *apn_info_p,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GDEVMGR_GET_DEVICE_SETTINGS

DESCRIPTION   This function fetches all the device Settings
 
PARAMETERS    Subscription Id
 
DEPENDENCIES  None.

RETURN VALUE  Device Settings

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdevmgr_get_device_settings
(
  ds3gdevmgr_device_settings_info_type* device_settings_p,
  sys_modem_as_id_e_type                subs_id
);

/*===========================================================================
FUNCTION      DS3GDEVMGR_PROCESS_CMDS

DESCRIPTION   This function is used to process all commands posted to this 
              module 
 
PARAMETERS    Command Ptr
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gdevmgr_process_cmds
(
  const ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GDEVMGR_PROCESS_CMDS

DESCRIPTION   This callback is invoked when any device event is received
 
PARAMETERS    1. Tech Type 
              2. Event name
              3. Subs id
              4. Event info
              5. User Data
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gdevmgr_events_cb
(
  ps_sys_tech_enum_type                    tech_type,
  ps_sys_event_enum_type                   event_name,
  ps_sys_subscription_enum_type            subscription_id,
  void                                   * event_info_ptr,
  void                                   * user_data_ptr
);  

#endif /* DS3G_DEVICE_EVT_HDLR_H */




