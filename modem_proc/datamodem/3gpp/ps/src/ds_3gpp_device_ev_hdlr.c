/*!
  @file
  ds_3gpp_device_ev_hdlr.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_device_ev_hdlr.c#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
10/01/14     vs      Initial File
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "ds_3gpp_device_ev_hdlr.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_roaming_hdlr.h"
#include "ds3geventmgr.h"
#include "ds3gutil.h"
#include "ds3gsubsmgr.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_dsd_apm_ext_i.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
  

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_PROCESS_APN_SETTINGS

DESCRIPTION   This function processes the APN settings
 
PARAMETERS    Device Settings
              Boolean Pointer set to True if the attach apn is unblocked
 
DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
static void ds3gpp_device_ev_hdlr_process_apn_settings
(
  ds3gdevmgr_device_settings_info_type *device_settings_p,
  boolean*                               is_attach_apn_ptr
);

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_CB

DESCRIPTION   This callback is invoked when device events are received
 
PARAMETERS    1. Event Id 
              2. Event info
              3. User Dats
 
DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
static void ds3gpp_device_ev_hdlr_cb
(
  ds3geventmgr_event_type       event_id,
  ds3geventmgr_filter_type      *ps_filter_info,
  void                          *event_info_ptr,
  void                          *data_ptr
);

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_ADVERTISE_THROTTLING_INFO

DESCRIPTION   This function is used to advertise throttling information
 
PARAMETERS    1. Status indicating data enabled / disabled 
              2. Subscription Id 
              3. boolean- True if the attach apn is unblocked
 
DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
static void ds3gpp_device_ev_hdlr_advertise_throttling_info
(
  boolean                status,
  sys_modem_as_id_e_type subs_id,
  boolean                is_attach_apn
);

/*===========================================================================


                               FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_INIT

DESCRIPTION   This function initializes the device event handler module
 
PARAMETERS    None
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gpp_device_ev_hdlr_init
(
  void
)
{
  ds3gsubsmgr_subs_id_e_type subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
  ds3geventmgr_filter_type   filter_info;
  /*-----------------------------------------------------------------------*/

  DS_3GPP_MSG0_MED("ds3gpp_device_ev_hdlr_init");

  filter_info.tech = PS_SYS_TECH_ALL;

  for (subs_id = DS3GSUBSMGR_SUBS_ID_MIN;
        subs_id < DS3GSUBSMGR_SUBS_ID_MAX; subs_id++)
  { 
    filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
                               ds3gsubsmgr_subs_id_ds3g_to_ds(subs_id);

    if (ds3geventmgr_event_reg(DS3GEVENTMGR_DATA_CHANGED_EV,
                               DS3GEVENTMGR_CLIENT_ID_3GPP,
                               &filter_info,
                               ds3gpp_device_ev_hdlr_cb,
                               NULL) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Registration for DS3GEVENTMGR_DATA_CHANGED_EV"
                         " failed !");
    }

    if (ds3geventmgr_event_reg(DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV,
                               DS3GEVENTMGR_CLIENT_ID_3GPP,
                               &filter_info,
                               ds3gpp_device_ev_hdlr_cb,
                               NULL) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Registration for DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV"
                         " failed !");
    }

    if (ds3geventmgr_event_reg(DS3GEVENTMGR_APN_CHANGED_EV,
                               DS3GEVENTMGR_CLIENT_ID_3GPP,
                               &filter_info,
                               ds3gpp_device_ev_hdlr_cb,
                               NULL) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Registration for DS3GEVENTMGR_APN_CHANGED_EV"
                         " failed !");
    }
  }
} /* ds3gpp_device_ev_hdlr_init */

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_INIT

DESCRIPTION   This function initializes device event handler parameters 
              after power up is complete 
 
PARAMETERS    None
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gpp_device_ev_hdlr_init_after_powerup
(
  void
)
{
  ds3gdevmgr_device_settings_info_type  device_settings_info;
  int                                   subs_id = 0;
  boolean                               data_is_disabled = FALSE;
  boolean                               is_attach_apn = FALSE;
  /*------------------------------------------------------------------------*/

  DS_3GPP_MSG0_MED("ds3gpp_device_ev_hdlr_init_after_powerup");

  for (subs_id = 0; subs_id < DS3GSUBSMGR_SUBS_ID_MAX; subs_id++)
  {
    memset((void*)&device_settings_info, 0, 
           sizeof(ds3gdevmgr_device_settings_info_type));

    if (ds3gdevmgr_get_device_settings(&device_settings_info, subs_id))
    {
      data_is_disabled = 
        ds3gpp_device_ev_hdlr_process_data_settings(&device_settings_info,&is_attach_apn);

      ds3gpp_device_ev_hdlr_advertise_throttling_info(data_is_disabled, 
                                                      subs_id,
                                                      is_attach_apn);
    }
    is_attach_apn = FALSE;
  }
} /* ds3gpp_device_ev_hdlr_init_after_powerup*/

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_CB

DESCRIPTION   This callback is invoked when device events are received
 
PARAMETERS    1. Event Id 
              2. PS filter info 
              2. Event info
              3. User Dats
 
DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
static void ds3gpp_device_ev_hdlr_cb
(
  ds3geventmgr_event_type       event_id,
  ds3geventmgr_filter_type      *ps_filter_info_p,
  void                          *event_info_ptr,
  void                          *data_ptr
)
{
  ds3gdevmgr_device_settings_info_type   device_settings_info;
  boolean                                data_is_disabled = FALSE;
  sys_modem_as_id_e_type                 subs_id = SYS_MODEM_AS_ID_1;
  boolean                                is_attach_apn = FALSE;
  boolean                                is_attach_apn_apn_settings = FALSE;
  boolean                                is_attach_apn_data_settings = FALSE;
  /*----------------------------------------------------------------------*/
  do
  {

    DS_3GPP_MSG1_HIGH("Processing ds3gpp_device_ev_hdlr_cb. Event: %d", 
                      event_id);

    memset((void*)&device_settings_info,0,
           sizeof(ds3gdevmgr_device_settings_info_type));

    if (event_id != DS3GEVENTMGR_DATA_CHANGED_EV &&
        event_id != DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV &&
        event_id != DS3GEVENTMGR_APN_CHANGED_EV)
    {
      DS_3GPP_MSG1_ERROR("Invalid Event: %d received !",event_id);
      ASSERT(0);
      break;
    }

    if (event_info_ptr == NULL)
    {
      DATA_ERR_FATAL("event_info_ptr is NULL !");
      break;
    }

    if (ps_filter_info_p == NULL)
    {
      DATA_ERR_FATAL("ps_filter_info_p is NULL !");
      break;
    }

    subs_id = ds3gsubsmgr_subs_id_ds_to_cm
                ((ds_sys_subscription_enum_type)ps_filter_info_p->ps_subs_id);

    ds3gdevmgr_get_device_settings(&device_settings_info, subs_id);
    /* is_attach_apn_apn_settings will be set to TRUE; attach apn was enabled */
    ds3gpp_device_ev_hdlr_process_apn_settings(&device_settings_info,
                                               &is_attach_apn_apn_settings);

    /* is_attach_apn_data_settings will be set to TRUE; attach apn was enabled */

    data_is_disabled = 
      ds3gpp_device_ev_hdlr_process_data_settings(&device_settings_info,
                                                  &is_attach_apn_data_settings);

    is_attach_apn = (is_attach_apn_apn_settings || is_attach_apn_data_settings);
    ds3gpp_device_ev_hdlr_advertise_throttling_info
      (data_is_disabled, device_settings_info.subs_id,is_attach_apn);

  } while (0); 

  return;
} /* ds3gpp_device_ev_hdlr_cb */

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_PROCESS_DATA_SETTINGS

DESCRIPTION   This function processes the data settings
 
PARAMETERS    Device Settings
              Boolean Pointer set to True if the attach apn is unblocked
 
DEPENDENCIES  None

RETURN VALUE  TRUE, if Data is disabled after processing the settings 
              FALSE, if Data is enabled 

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gpp_device_ev_hdlr_process_data_settings
(
  ds3gdevmgr_device_settings_info_type  *device_settings_info_p,
  boolean*                              is_attach_apn_ptr
)
{
  byte                           internet_apn[DS_SYS_MAX_APN_LEN];
  boolean                        data_is_disabled = FALSE;
  ps_iface_net_down_reason_type  tear_down_reason = 
                                   PS_NET_DOWN_REASON_NOT_SPECIFIED;
  /*----------------------------------------------------------------------*/
  
  memset((void*)internet_apn, 0, sizeof(internet_apn));
        
  do
  {
    if (device_settings_info_p == NULL)
    {
      DATA_ERR_FATAL("device_settings_p is NULL !");
      break;
    }

    if (is_attach_apn_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("is_attach_apn_ptr is NULL !");
      ASSERT(0);
      break;
    }

    /*--------------------------------------------------------------
      If NV is set to TRUE, ignore data settings
    ---------------------------------------------------------------*/
    if (ds_3gpp_nv_manager_get_ignore_ui_data_throttling
         (device_settings_info_p->subs_id) == TRUE)
    {
      DS_3GPP_MSG1_HIGH("Ignoring data settings for CM Subs id : %d", 
                        device_settings_info_p->subs_id);
      break;
    }

    if (device_settings_info_p->apn_info.curr_apn_info
        [DS_SYS_APN_TYPE_INTERNET].is_configured == FALSE)
    {
      DS_3GPP_MSG0_HIGH("Internet APN is not configured. Ignoring Data Settings");
      break;
    }

    strlcpy((char*)internet_apn,
            (char*)device_settings_info_p->apn_info.curr_apn_info
            [DS_SYS_APN_TYPE_INTERNET].apn_name,
            sizeof(internet_apn));

    DS_3GPP_MSG2_HIGH("Data Setting: %d, Data Roaming Setting: %d",
                      device_settings_info_p->data_setting,
                      device_settings_info_p->roaming_setting);

    if (device_settings_info_p->data_setting == TRUE)
    {
      if (device_settings_info_p->roaming_setting == TRUE || 
          ds_3gpp_roaming_get_status(device_settings_info_p->subs_id) == FALSE)
      {
        ds_3gpp_pdn_throt_unblock_ui_data_disabled_throttling
          ((byte*)internet_apn, device_settings_info_p->subs_id);

        if (ds_dsd_apm_is_apn_in_attach_pdn_list(device_settings_info_p->subs_id,
				                 (char*)internet_apn))
        {
          *is_attach_apn_ptr = TRUE;
        }
        break;
      }
      else
      {
        tear_down_reason = PS_NET_DOWN_REASON_DATA_ROAMING_SETTINGS_DISABLED;
      }
    }
    else
    {
      tear_down_reason = PS_NET_DOWN_REASON_DATA_SETTINGS_DISABLED;
    }

    data_is_disabled = 
      ds_3gpp_pdn_throt_perform_ui_data_disabled_throttling
        ((byte*)internet_apn, device_settings_info_p->subs_id);

    ds_3gpp_pdn_cntx_teardown_apn((char*)internet_apn, 
                                  device_settings_info_p->subs_id,
                                  tear_down_reason);

  } while (0); 

  return data_is_disabled;

} /* ds3gpp_device_ev_hdlr_process_device_settings */

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_PROCESS_APN_SETTINGS

DESCRIPTION   This function processes the APN settings
 
PARAMETERS    Device Settings
              Boolean Pointer set to True if the attach apn is unblocked
 
DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
static void ds3gpp_device_ev_hdlr_process_apn_settings
(
  ds3gdevmgr_device_settings_info_type *device_settings_info_p,
  boolean*                              is_attach_apn_ptr
)
{
  byte                                  internet_apn[DS_SYS_MAX_APN_LEN];
  byte                                  old_internet_apn[DS_SYS_MAX_APN_LEN];
  /*----------------------------------------------------------------------*/

  memset((void*)internet_apn, 0, sizeof(internet_apn));
  memset((void*)old_internet_apn, 0, sizeof(old_internet_apn));

  do
  {
    if (device_settings_info_p == NULL)
    {
      DATA_ERR_FATAL("device_settings_info_p is NULL !");
      break;
    }

    if (is_attach_apn_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("is_attach_apn_ptr is NULL !");
      ASSERT(0);
      break;
    }

    /*--------------------------------------------------------------
      If NV is set to TRUE, ignore data settings
    ---------------------------------------------------------------*/
    if (ds_3gpp_nv_manager_get_ignore_ui_data_throttling 
          (device_settings_info_p->subs_id) == TRUE)
    {
      DS_3GPP_MSG1_HIGH("Ignoring data settings for CM Subs id : %d", 
                        device_settings_info_p->subs_id);
      break;
    }

    strlcpy((char*)internet_apn,
            (char*)device_settings_info_p->apn_info.
              curr_apn_info[DS_SYS_APN_TYPE_INTERNET].apn_name,
            sizeof(internet_apn));

    strlcpy((char*)old_internet_apn, 
            (char*)device_settings_info_p->apn_info.
              prev_apn_info[DS_SYS_APN_TYPE_INTERNET].apn_name,
            sizeof(old_internet_apn));

    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"internet APN: %s",internet_apn);

    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"old internet APN: %s",
                            old_internet_apn);

    if (strncasecmp((const char*)internet_apn, 
                    (const char*)old_internet_apn, 
                    DS_SYS_MAX_APN_LEN) != 0)
    {
      DS_3GPP_MSG0_HIGH("Internet APN Name has changed ! ");

      if (device_settings_info_p->apn_info.prev_apn_info
          [DS_SYS_APN_TYPE_INTERNET].is_configured == TRUE)
      {
        ds_3gpp_pdn_throt_unblock_ui_data_disabled_throttling
          ((byte*)old_internet_apn, device_settings_info_p->subs_id);

        if (ds_dsd_apm_is_apn_in_attach_pdn_list(device_settings_info_p->subs_id,
				                 (char*)old_internet_apn))
        {
          *is_attach_apn_ptr = TRUE;
        }

      }
      else
      {
        DS_3GPP_MSG0_HIGH("Old Internet APN was not configured. "
                          "So no need to unthrottle");
      }
    }

  } while (0); 

  return;
} /* ds3gpp_device_ev_hdlr_process_apn_settings */

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_ADVERTISE_THROTTLING_INFO

DESCRIPTION   This function is used to advertise throttling information
 
PARAMETERS    1. Flag indicating data enabled / disabled 
              2. Subscription Id 
              3. Boolean Pointer set to True if the attach apn is unblocked
 
DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
static void ds3gpp_device_ev_hdlr_advertise_throttling_info
(
  boolean                data_disabled,
  sys_modem_as_id_e_type subs_id,
  boolean                is_attach_apn
)
{
  do
  {
    if (ds_3gpp_nv_manager_get_ignore_ui_data_throttling(subs_id) == TRUE)
    {
      DS_3GPP_MSG1_HIGH("Ignoring data settings for CM Subs id : %d", 
                        subs_id);
      break;
    }

    ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);

#ifdef FEATURE_DATA_LTE
    if (data_disabled == TRUE)
    {
      ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable(subs_id);
    }
    else
    {
      if (is_attach_apn)
      {
        ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available(subs_id);
      }
    }
#endif /* #ifdef FEATURE_DATA_LTE */
  } while (0); 

  return;
} /* ds3gpp_device_ev_hdlr_advertise_throttling_info */

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_IS_APN_ALLOWED

DESCRIPTION   This function checks whether PDN can be brought up for this APN
 
PARAMETERS    1. APN name 
              2. Subs id 
 
DEPENDENCIES  None

RETURN VALUE  TRUE, if PDN can be brought up
              FALSE, otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gpp_device_ev_hdlr_is_apn_allowed
(
  char                            *apn_p,
  sys_modem_as_id_e_type           subs_id,
  ps_iface_net_down_reason_type   *down_reason_p
)
{
  boolean                apn_is_allowed = TRUE;
  boolean                data_setting = FALSE;
  boolean                roaming_setting = FALSE;
  ds_sys_apn_info_type   apn_info;
  /*--------------------------------------------------------------*/

  do
  {
    /*--------------------------------------------------------------
      If NV is set to TRUE, ignore data settings
    ---------------------------------------------------------------*/
    if (ds_3gpp_nv_manager_get_ignore_ui_data_throttling(subs_id) == TRUE)
    {
      break;
    }

    ds3gdevmgr_get_apn_settings(&apn_info, subs_id);

    /*--------------------------------------------------------------
      If Internet APN is not configured, ignore data settings
    ---------------------------------------------------------------*/
    if (apn_info.curr_apn_info[DS_SYS_APN_TYPE_INTERNET].is_configured == FALSE)
    {
      break;
    }

    /*--------------------------------------------------------------
      If the given APN is not the internet APN, allow the call
    ---------------------------------------------------------------*/
    if (strncasecmp((const char*)apn_info.curr_apn_info
                      [DS_SYS_APN_TYPE_INTERNET].apn_name,
                    (const char*)apn_p, 
                    DS_SYS_MAX_APN_LEN) != 0)
    {
      break;
    }

    if (down_reason_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("Down Reason ptr is NULL");
      break;
    }

    /*--------------------------------------------------------------
      If Data is Disabled or Data Roaming is Disabled and UE is
      Roaming, disallow the call
    ---------------------------------------------------------------*/

    ds3gdevmgr_get_data_settings(&data_setting, subs_id);
    if (data_setting == FALSE)
    {
      *down_reason_p = PS_NET_DOWN_REASON_DATA_SETTINGS_DISABLED;
      DS_3GPP_MSG0_ERROR("UI Data Disabled");
      apn_is_allowed = FALSE;
      break;
    }

    ds3gdevmgr_get_roaming_settings(&roaming_setting, subs_id);
    if ((roaming_setting == FALSE) && (ds_3gpp_roaming_get_status(subs_id)))
    {
      *down_reason_p = PS_NET_DOWN_REASON_DATA_ROAMING_SETTINGS_DISABLED;
      DS_3GPP_MSG0_ERROR("UI Data Roaming Disabled and UE is Roaming");
      apn_is_allowed = FALSE;
      break;
    }
  }

  while (0); 

  return apn_is_allowed;
} /* ds3gpp_device_ev_hdlr_is_apn_allowed */
