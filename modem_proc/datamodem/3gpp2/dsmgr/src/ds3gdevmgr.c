/*!
  @file
  ds3gdevmgr.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gdevmgr.c#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
09/08/14     vs      Initial File
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "data_msg.h"
#include "dstask_v.h"
#include "ds3gdevmgr.h"
#include "ds3geventmgr.h"
#include "ds3gsubsmgr.h"
#include "ds3gutil.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef struct
{
  ps_sys_event_enum_type            event_name;
  ps_sys_subscription_enum_type     subscription_id;
} ds3gdevmgr_events_cb_info_type;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GDEVMGR_PROCESS_DEVICE_SETTINGS

DESCRIPTION   This function is used to process all device settings

PARAMETERS    Callback Info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds3gdevmgr_process_device_settings
(
  ds3gdevmgr_events_cb_info_type *cb_info_p
);


/*===========================================================================


                               FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GDEVMGR_INIT

DESCRIPTION   This function initializes the DS3G Device Manager module.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdevmgr_init( void )
{
  ps_sys_subscription_enum_type  ps_subs_id = PS_SYS_DEFAULT_SUBS;
  int16                          ret_val = 0;
  int16                          ps_errno = 0;
  ds3gsubsmgr_subs_id_e_type     ds3g_subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (ds3g_subs_id = DS3GSUBSMGR_SUBS_ID_MIN;
         ds3g_subs_id < DS3GSUBSMGR_SUBS_ID_MAX; ds3g_subs_id++)
  {
    ps_subs_id = (ps_sys_subscription_enum_type)
                    ds3gsubsmgr_subs_id_ds3g_to_ds(ds3g_subs_id);

    ret_val = ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                                  PS_SYS_EVENT_DATA_SERVICE_SETTING,
                                  ps_subs_id,
                                  ds3gdevmgr_events_cb,
                                  NULL,
                                  &ps_errno);
    if (ret_val < 0)
    {
      DATA_3GMGR_MSG3 (MSG_LEGACY_ERROR,
                       "ds_3gpp_device_ev_hdlr_init: Error while "
                       "registering for PS_SYS_EVENT_DATA_SERVICE_SETTING event,"
                       "return value %d and error %d, PS subs index: %d ",
                        ret_val, ps_errno, ps_subs_id);
    }

    ret_val = ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                                  PS_SYS_EVENT_DATA_SERVICE_ROAMING_SETTING,
                                  ps_subs_id,
                                  ds3gdevmgr_events_cb,
                                  NULL,
                                  &ps_errno);
    if (ret_val < 0)
    {
      DATA_3GMGR_MSG3 (MSG_LEGACY_ERROR,
                       "ds_3gpp_device_ev_hdlr_init: Error while "
                       "registering for PS_SYS_EVENT_DATA_SERVICE_ROAMING_SETTING event,"
                       "return value %d and error %d, PS subs index: %d",
                        ret_val, ps_errno, ps_subs_id);
    }

    ret_val = ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                                  PS_SYS_EVENT_APN_INFO,
                                  ps_subs_id,
                                  ds3gdevmgr_events_cb,
                                  NULL,
                                  &ps_errno);
    if (ret_val < 0)
    {
      DATA_3GMGR_MSG3 (MSG_LEGACY_ERROR,
                       "ds_3gpp_device_ev_hdlr_init: Error while "
                       "registering for PS_SYS_EVENT_DATA_SERVICE_SETTING event,"
                       "return value %d and error %d, PS subs index: %d",
                        ret_val, ps_errno, ps_subs_id);
    }
  }
} /* ds3gdevmgr_init */

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
  ps_sys_tech_enum_type           tech_type,
  ps_sys_event_enum_type          event_name,
  ps_sys_subscription_enum_type   subscription_id,
  void                           *event_info_ptr,
  void                           *user_data_ptr
)
{
  ds_cmd_type                     *cmd_ptr = NULL;
  ds3gdevmgr_events_cb_info_type  *device_ev_info_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3gdevmgr_events_cb_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                    "Device Event: %d received on PS Subs Id: %d",
                    event_name, subscription_id);

    cmd_ptr->hdr.cmd_id = DS_CMD_PROCESS_DEVICE_SETTINGS;
    device_ev_info_p = (ds3gdevmgr_events_cb_info_type*)cmd_ptr->cmd_payload_ptr;

    device_ev_info_p->event_name = event_name;
    device_ev_info_p->subscription_id = subscription_id;
    ds_put_cmd_ext(cmd_ptr);
  }

  return;
} /* ds3gdevmgr_events_cb */

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  switch (cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_PROCESS_DEVICE_SETTINGS:
    {
      ds3gdevmgr_process_device_settings
      (
        (ds3gdevmgr_events_cb_info_type*)cmd_ptr->cmd_payload_ptr
      );
      break;
    }

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Unrecognized command: %d",
                      cmd_ptr->hdr.cmd_id );
      break;
  }
} /* ds3gdevmgr_process_cmds */

/*===========================================================================
FUNCTION      DS3GDEVMGR_PROCESS_DEVICE_SETTINGS

DESCRIPTION   This function is used to process all device settings

PARAMETERS    Callback Info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds3gdevmgr_process_device_settings
(
  ds3gdevmgr_events_cb_info_type  *cb_info_p
)
{
  boolean                       data_setting = FALSE;
  boolean                       roaming_setting = FALSE;
  ds_sys_apn_info_type          apn_info;
  ds3geventmgr_event_info_type  event_info;
  sys_modem_as_id_e_type        cm_subs_id = SYS_MODEM_AS_ID_NONE;
  ds3geventmgr_filter_type      filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cb_info_p != NULL);

  memset(&event_info, 0, sizeof(ds3geventmgr_event_info_type));
  memset(&apn_info, 0, sizeof(ds_sys_apn_info_type));
  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));

  cm_subs_id =
    ds3gsubsmgr_subs_id_ds_to_cm
      ((ds_sys_subscription_enum_type)cb_info_p->subscription_id);

  DATA_3GMGR_MSG1(MSG_LEGACY_MED, "Processing PS Sys Event: %d",
                  cb_info_p->event_name);

  filter_info.ps_subs_id = cb_info_p->subscription_id;
  filter_info.tech = PS_SYS_TECH_ALL;

  switch (cb_info_p->event_name)
  {
    case PS_SYS_EVENT_DATA_SERVICE_SETTING:
    {
      if (ds3gdevmgr_get_data_settings(&data_setting,cm_subs_id))
      {
        event_info.event_payload_p = (void*)&data_setting;

        if(ds3geventmgr_set_event_info(DS3GEVENTMGR_DATA_CHANGED_EV,
                                       (void*)&event_info,
                                       &filter_info) == FALSE)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                         "Unable to send DS3GEVENTMGR_DATA_CHANGED_EV");
        }
      }
      break;
    }

    case PS_SYS_EVENT_DATA_SERVICE_ROAMING_SETTING:
    {
      if (ds3gdevmgr_get_roaming_settings(&roaming_setting,cm_subs_id))
      {
        event_info.event_payload_p = (void*)&roaming_setting;

        if(ds3geventmgr_set_event_info(DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV,
                                       (void*)&event_info,
                                       &filter_info) == FALSE)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                         "Unable to send DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV");
        }
      }
      break;
    }

    case PS_SYS_EVENT_APN_INFO:
    {
      if (ds3gdevmgr_get_apn_settings(&apn_info,cm_subs_id))
      {
        event_info.event_payload_p = (void*)&apn_info;
        if(ds3geventmgr_set_event_info(DS3GEVENTMGR_APN_CHANGED_EV,
                                       (void*)&event_info,
                                       &filter_info) == FALSE)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                         "Unable to send DS3GEVENTMGR_APN_CHANGED_EV");
        }
      }
      break;
    }

   default:
   {
     DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Unknown PS Sys event: %d",
                                       cb_info_p->event_name);
     break;
   }
 }
} /* ds3gdevmgr_process_device_settings */

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
  sys_modem_as_id_e_type   subs_id
)
{
  ps_sys_subscription_enum_type   ps_subs_id = PS_SYS_DEFAULT_SUBS;
  boolean                         ret_val = TRUE;
  int16                           ps_errno = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if(data_setting_p == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Data setting ptr is NULL");
      ret_val = FALSE;
      break;
    }

    ps_subs_id = (ps_sys_subscription_enum_type)
                   ds3gsubsmgr_subs_id_cm_to_ds(subs_id);

    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Fetching Data settings for CM Subs Id: %d", subs_id);

    if ( ps_sys_conf_get_ex( PS_SYS_TECH_ALL,
                             PS_SYS_CONF_DATA_SERVICE_SETTING,
                             ps_subs_id,
                             (void*)data_setting_p,
                             &ps_errno ) != 0 )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "PS_SYS_CONF_GET_EX failed on PS_SYS_CONF_DATA_SERVICE_SETTING, "
                      "return");
      ret_val = FALSE;
      break;
    }
  } while (0);

  return ret_val;
} /* ds3gdevmgr_get_data_settings */

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
)
{
  ps_sys_subscription_enum_type   ps_subs_id = PS_SYS_DEFAULT_SUBS;
  boolean                         ret_val = TRUE;
  int16                           ps_errno = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if(roaming_setting_p == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Roaming setting ptr is NULL");
      ret_val = FALSE;
      break;
    }

    ps_subs_id = (ps_sys_subscription_enum_type)
                   ds3gsubsmgr_subs_id_cm_to_ds(subs_id);

    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Fetching Roaming settings for CM Subs Id: %d", subs_id);

    if ( ps_sys_conf_get_ex( PS_SYS_TECH_ALL,
                             PS_SYS_CONF_DATA_SERVICE_ROAMING_SETTING,
                             ps_subs_id,
                             (void*)roaming_setting_p,
                             &ps_errno ) != 0 )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "PS_SYS_CONF_GET_EX failed on "
                      "PS_SYS_CONF_DATA_SERVICE_ROAMING_SETTING, return");
      ret_val = FALSE;
      break;
    }
  } while (0);

  return ret_val;
} /* ds3gdevmgr_get_roaming_settings */

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
)
{
  ps_sys_subscription_enum_type   ps_subs_id = PS_SYS_DEFAULT_SUBS;
  boolean                         ret_val = TRUE;
  int16                           ps_errno = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if(apn_info_p == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "apn_info_p is NULL");
      ret_val = FALSE;
      break;
    }

    ps_subs_id = (ps_sys_subscription_enum_type)
                   ds3gsubsmgr_subs_id_cm_to_ds(subs_id);

    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Fetching APN settings for CM Subs Id: %d", subs_id);

    if ( ps_sys_conf_get_ex( PS_SYS_TECH_ALL,
                             PS_SYS_CONF_APN_INFO,
                             ps_subs_id,
                             (void*)apn_info_p,
                             &ps_errno ) != 0 )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "PS_SYS_CONF_GET_EX failed on "
                      "PS_SYS_CONF_APN_INFO, return");
      ret_val = FALSE;
      break;
    }
  } while (0);

  return ret_val;
} /* ds3gdevmgr_get_apn_settings */

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
  ds3gdevmgr_device_settings_info_type  *device_settings_p,
  sys_modem_as_id_e_type                 subs_id
)
{
  boolean  ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  device_settings_p->subs_id = subs_id;

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                  "Fetching Device settings for CM Subs Id: %d", subs_id);
  do
  {
    if (!ds3gdevmgr_get_data_settings
          (&(device_settings_p->data_setting),subs_id) ||
        !ds3gdevmgr_get_roaming_settings
          (&(device_settings_p->roaming_setting), subs_id) ||
        !ds3gdevmgr_get_apn_settings
          (&(device_settings_p->apn_info), subs_id))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                     "Error Fetching some Device settings for CM Subs Id: %d",
                      subs_id);
      break;
    }

    ret_val = TRUE;

  } while (0);

  return ret_val;
} /* ds3gdevmgr_get_device_settings */

