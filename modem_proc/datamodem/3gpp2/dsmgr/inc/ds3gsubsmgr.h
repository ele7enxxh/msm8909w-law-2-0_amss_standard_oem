#ifndef DS3GSUBSMGR_H
#define DS3GSUBSMGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             3G  DATA  SERVICES  SUBSCRIPTION  MANAGER

GENERAL DESCRIPTION
  This software unit manages the subscriptions for DS. It provides APIs for
  subscription related properties.
  Will map CM subs id value to DS internal subs id index values.

Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gsubsmgr.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/25/13   ss      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_profile.h"
#include "dstask_v.h"
#include "sys.h"
#include "cm.h"
#include "ds_sys.h"
#include "ds_profile.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
  DS3G Subs Mgr internal subscription index type. 
  DS internally should use DS3GSUBSMGR_SUBS_ID_MAX instead of 
  DS3G_MAX_SUBS.
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GSUBSMGR_SUBS_ID_INVALID = -1,
  DS3GSUBSMGR_SUBS_ID_MIN     = 0,
  DS3GSUBSMGR_SUBS_ID_1       = DS3GSUBSMGR_SUBS_ID_MIN,
  DS3GSUBSMGR_SUBS_ID_2       = 1,
#if defined(FEATURE_TRIPLE_SIM)
  DS3GSUBSMGR_SUBS_ID_3       = 2,
#endif /*defined(FEATURE_TRIPLE_SIM)*/
  DS3GSUBSMGR_SUBS_ID_MAX,
  DS3GSUBSMGR_SUBS_ID_COUNT   = DS3GSUBSMGR_SUBS_ID_MAX
} ds3gsubsmgr_subs_id_e_type;

/*-------------------------------------------------------------------------
  Data type for DS3G Subs Mgr maintaining subscription info
-------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type  as_id;
  boolean                 is_default_data_subs;
  boolean                 is_default_voice_subs;
  cm_mode_pref_e_type     mode_pref;
  sys_sys_mode_mask_e_type  subs_capability;  
} ds3gsubsmgr_subs_type;

/*-------------------------------------------------------------------------
  DS3G External Events Enum Type for device mode and DDS change
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GSUBSMGR_INVALID_EV         = -1,
  DS3GSUBSMGR_MIN_EV,
  DS3GSUBSMGR_DEVICE_MODE_EV     = DS3GSUBSMGR_MIN_EV,
  DS3GSUBSMGR_DDS_CHANGE_EV,
  DS3GSUBSMGR_DVS_CHANGE_EV,
  DS3GSUBSMGR_MODE_PREF_CHG_EV,
  DS3GSUBSMGR_SUBS_CAPABILITY_CHG_EV,
  DS3GSUBSMGR_MAX_EV             = DS3GSUBSMGR_SUBS_CAPABILITY_CHG_EV
}ds3gsubsmgr_event_type;

/*-------------------------------------------------------------------------
  Structure to notify events
-------------------------------------------------------------------------*/
typedef struct
{
  struct
  {
    sys_modem_device_mode_e_type  curr;
    sys_modem_device_mode_e_type  prev;
  } device_mode;

  struct
  {
    ds3gsubsmgr_subs_id_e_type    curr;
    ds3gsubsmgr_subs_id_e_type    prev;
  } def_data_subs;

  struct
  {
    ds3gsubsmgr_subs_id_e_type    curr;
    ds3gsubsmgr_subs_id_e_type    prev;
  } def_voice_subs;

  struct
  {
    ds3gsubsmgr_subs_id_e_type    subs_id;
    cm_mode_pref_e_type           curr;
    cm_mode_pref_e_type           prev;
  } mode_pref;

  struct
  {
    ds3gsubsmgr_subs_id_e_type    subs_id;
    sys_sys_mode_mask_e_type      curr;
    sys_sys_mode_mask_e_type      prev;
  } subs_capability;  
}ds3gsubsmgr_event_info_type;

/*------------------------------------------------------------------------
   Clients Registers a callback function with DS3GSUBSMGR module
   for events.
-------------------------------------------------------------------------*/
typedef void (*ds3gsubsmgr_notify_func_type)
               (
                 ds3gsubsmgr_event_type        event_id,
                 ds3gsubsmgr_event_info_type  *event_info_ptr,
                 void                         *data_ptr
               );

/*------------------------------------------------------------------------
   Client id type
-------------------------------------------------------------------------*/
typedef uint16 ds3gsubsmgr_client_type;

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GSUBSMGR_INIT

DESCRIPTION   This function initializes the DS3G Subs Mgr module. This would 
              be invoked from ds3gi_cm_init() which is the initialization
              routine for DSMgr.

              The subs id 1 is the default subs id for DS. The device mode is
              default to SINGLE SIM mode. Other ds subs id structures are
              also populated.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gsubsmgr_init( void );

/*===========================================================================
FUNCTION      DS3GSUBSMGR_EVENT_REG

DESCRIPTION   Called by clients to register to get DS3GSUBSMGR event.

DEPENDENCIES  None.

RETURN VALUE  TRUE: If registration successful.
                    If client previously registgered.

              FALSE: If registration unsuccessful.

SIDE EFFECTS  Registered client is entered on a queue.
===========================================================================*/
boolean ds3gsubsmgr_event_reg
(
  ds3gsubsmgr_event_type        event_id,
  ds3gsubsmgr_notify_func_type  cb_func_ptr,
  void                         *cb_data_ptr,
  ds3gsubsmgr_client_type      *client_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_EVENT_DEREG

DESCRIPTION   Called by clients to de-register DS3GSUBSMGR event notification.

DEPENDENCIES  None.

RETURN VALUE  TRUE: If de-registration successful.
                    If client previously de-registgered.

              FALSE: If de-registration unsuccessful.

SIDE EFFECTS  De-registered client is dequeued.
===========================================================================*/
boolean ds3gsubsmgr_event_dereg
(
  ds3gsubsmgr_client_type       client_id,
  ds3gsubsmgr_event_type        event_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_CONVERT_NV_MODE_TO_CM_MODE

DESCRIPTION   This utility function translates the NV mode enum type to 
              CM mode enum.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
cm_mode_pref_e_type ds3gsubsmgr_convert_nv_mode_to_cm_mode
(
  nv_mode_enum_type nv_mode_pref
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_IS_DEF_DATA_SUBS_ID

DESCRIPTION   This function checks if passed subs id is default data
              subscription id.

DEPENDENCIES  None.

RETURN VALUE  TRUE:  subs_id is data subs id
              FALSE: subs_id is not data subs id

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gsubsmgr_is_def_data_subs_id
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_DEF_DATA_SUBS_ID

DESCRIPTION   This function fetches the default data subscription id.

DEPENDENCIES  None.

RETURN VALUE  Default data subscription id.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_as_id_e_type ds3gsubsmgr_get_def_data_subs_id(void);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_DEF_VOICE_SUBS_ID

DESCRIPTION   This function fetches the default voice subscription id.

DEPENDENCIES  None.

RETURN VALUE  Default voice subscription id.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_as_id_e_type ds3gsubsmgr_get_def_voice_subs_id(void);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_STANDBY_PREF

DESCRIPTION   This function fetches the standby preference.

DEPENDENCIES  None.

RETURN VALUE  Standby preference type.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_dual_standby_pref_e_type ds3gsubsmgr_get_standby_pref(void);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_DEVICE_MODE

DESCRIPTION   This function fetches the current device mode, i.e. single sim,
              dsds, dsda

DEPENDENCIES  None.

RETURN VALUE  Current device mode.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_device_mode_e_type ds3gsubsmgr_get_device_mode(void);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_MODE_PREF_FOR_SUBS_INDEX

DESCRIPTION   This function fetches the mode preference for DS SUBS index

DEPENDENCIES  None.

RETURN VALUE  mode preference

SIDE EFFECTS  None.
===========================================================================*/
cm_mode_pref_e_type ds3gsubsmgr_get_mode_pref_for_subs_index
(
  ds3gsubsmgr_subs_id_e_type  subs_indx
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_MODE_PREF_FOR_SUBS_ID

DESCRIPTION   This function fetches the mode preference for CM SUBS id

DEPENDENCIES  None.

RETURN VALUE  mode preference

SIDE EFFECTS  None.
===========================================================================*/
cm_mode_pref_e_type ds3gsubsmgr_get_mode_pref_for_subs_id
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_SUBS_CAPABILITY_FOR_SUBS_INDEX

DESCRIPTION   This function fetches the subs capability for DS SUBS index

DEPENDENCIES  None.

RETURN VALUE  subs capability

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_mask_e_type ds3gsubsmgr_get_subs_capability_for_subs_index
(
  ds3gsubsmgr_subs_id_e_type  subs_indx
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_SUBS_CAPABILITY_FOR_SUBS_ID

DESCRIPTION   This function fetches the subs capability for CM SUBS id

DEPENDENCIES  None.

RETURN VALUE  subs capability

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_mask_e_type ds3gsubsmgr_get_subs_capability_for_subs_id
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SET_SINGLE_STANDBY_ACTIVE_SUBS

DESCRIPTION   This function updates the active subscription in case of singe 
              standby mode of operation.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  If the incoming subs id is not valid, update will not happen.
===========================================================================*/
void ds3gsubsmgr_set_single_standby_active_subs
(
  sys_modem_as_id_e_type active_subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SET_DEF_DATA_SUBS_ID

DESCRIPTION   This function updates the default data subscription id in 
              ds3g subs mgr state info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gsubsmgr_set_def_data_subs_id
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SET_DEF_VOICE_SUBS_ID

DESCRIPTION   This function updates the default voice subscription id in 
              ds3g subs mgr state info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gsubsmgr_set_def_voice_subs_id
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SET_MODE_PREF

DESCRIPTION   This function updates the mode preference in ds3g subs mgr 
              state info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gsubsmgr_set_mode_pref
(
  sys_modem_as_id_e_type subs_id,
  cm_mode_pref_e_type    mode_pref
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SET_SUBS_CAPABILITY

DESCRIPTION   This function updates the subs capability in ds3g subs mgr 
              state info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gsubsmgr_set_subs_capability
(
  sys_modem_as_id_e_type       subs_id,
  sys_sys_mode_mask_e_type     subs_capability
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SET_STANDBY_PREF

DESCRIPTION   This function updates the standby preference in ds3g subs mgr 
              state info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gsubsmgr_set_standby_pref
(
  sys_modem_dual_standby_pref_e_type pref
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SET_DEVICE_MODE

DESCRIPTION   This function updates the current device mode, i.e. single sim, 
              dsds, dsda in ds3g subs mgr state info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gsubsmgr_set_device_mode
(
  sys_modem_device_mode_e_type mode
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_IS_DEVICE_MODE_SINGLE_SIM

DESCRIPTION   This function returns TRUE if the device mode is single sim.

DEPENDENCIES  None.

RETURN VALUE  TRUE:     the device mode is single sim 
              FALSE:    the device mode is not single sim

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gsubsmgr_is_device_mode_single_sim (void);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_IS_DEVICE_MODE_MULTI_SIM_STANDBY

DESCRIPTION   This function returns TRUE if the device mode is DSDS or TSTS.

DEPENDENCIES  None.

RETURN VALUE  TRUE:     device mode is multi standby 
              FALSE:    device mode is not multi standby

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gsubsmgr_is_device_mode_multi_sim_standby (void);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SUBS_ID_CM_TO_DS3G

DESCRIPTION   This function converts the input CM subscription id to 
              DS3GSubsMgr specific subscription index value.

DEPENDENCIES  None.

RETURN VALUE  DS3GSubsMgr specific subscription index value.

SIDE EFFECTS  None.
===========================================================================*/
ds3gsubsmgr_subs_id_e_type ds3gsubsmgr_subs_id_cm_to_ds3g
(
  sys_modem_as_id_e_type cm_subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SUBS_ID_DS3G_TO_CM

DESCRIPTION   This function converts the input DS3GSubsMgr specific 
              subscription index value to CM subscription id.

DEPENDENCIES  None.

RETURN VALUE  CM subscription id.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_as_id_e_type ds3gsubsmgr_subs_id_ds3g_to_cm
(
  ds3gsubsmgr_subs_id_e_type subs_index
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SUBS_ID_DS3G_TO_DS

DESCRIPTION   This function converts the input DS3GSubsMgr specific 
              subscription index value to DS subscription id.

DEPENDENCIES  None.

RETURN VALUE  DS subscription id.

SIDE EFFECTS  None.
===========================================================================*/
ds_sys_subscription_enum_type ds3gsubsmgr_subs_id_ds3g_to_ds
(
  ds3gsubsmgr_subs_id_e_type subs_index
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SUBS_ID_DS_TO_DS3G

DESCRIPTION   This function converts the input DS subscription id to 
              DS3GSubsMgr specific subscription index value.

DEPENDENCIES  None.

RETURN VALUE  DS3G subscription index.

SIDE EFFECTS  None.
===========================================================================*/
ds3gsubsmgr_subs_id_e_type ds3gsubsmgr_subs_id_ds_to_ds3g
(
  ds_sys_subscription_enum_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SUBS_ID_DS_TO_CM

DESCRIPTION   This function converts the input DS subscription id to
              CM subscription id.

DEPENDENCIES  None.

RETURN VALUE  DS3G subscription index.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_as_id_e_type ds3gsubsmgr_subs_id_ds_to_cm
(
  ds_sys_subscription_enum_type  ds_subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SUBS_ID_CM_TO_DSPROFILE

DESCRIPTION   This function converts the input DS Profile subscription id to
              CM subscription id.

DEPENDENCIES  None.

RETURN VALUE  DS3G subscription index.

SIDE EFFECTS  None.
===========================================================================*/
ds_profile_subs_etype ds3gsubsmgr_subs_id_cm_to_dsprofile
(
  sys_modem_as_id_e_type  cm_subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_IS_SUBS_ID_VALID

DESCRIPTION   This function validates the subscription id is valid.

DEPENDENCIES  None.

RETURN VALUE  TRUE or FALSE based on whether subs_index is valid or not.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gsubsmgr_is_subs_id_valid
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_IS_DS3G_SUBS_INDEX_VALID

DESCRIPTION   This function validates the subscription index value based on 
              the current device mode.

DEPENDENCIES  None.

RETURN VALUE  TRUE or FALSE based on whether subs_index is valid or not.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gsubsmgr_is_ds3g_subs_index_valid
(
  ds3gsubsmgr_subs_id_e_type subs_index
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_MODE_PREF_TO_SYS_MODE

DESCRIPTION   This function returns the preferred sys mode from cm mode pref

DEPENDENCIES  None.

RETURN VALUE  sys mode.

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_e_type ds3gsubsmgr_mode_pref_to_sys_mode
(
  cm_mode_pref_e_type mode_pref
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SUBS_ID_CM_TO_DS

DESCRIPTION   This function converts the input CM subscription id to 
              DS Subscription Id

DEPENDENCIES  None.

RETURN VALUE  DS subscrption ID

SIDE EFFECTS  None.
===========================================================================*/
ds_sys_subscription_enum_type ds3gsubsmgr_subs_id_cm_to_ds
(
  sys_modem_as_id_e_type   cm_subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_IS_PS_CALL_ALLOWED_ON_SUBS

DESCRIPTION   This function checks whether PS call is allowed on the subs_id.

DEPENDENCIES  None.

RETURN VALUE  boolean.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gsubsmgr_is_ps_call_allowed_on_subs
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_IS_SUBS_VALID_IN_CURRENT_CONFIG

DESCRIPTION   This function checks whether subs_id passed is valid or not 
              for present device config.

DEPENDENCIES  None.

RETURN VALUE  boolean.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gsubsmgr_is_subs_valid_in_current_config
(
  ds3gsubsmgr_subs_id_e_type subs_id
);

#endif /* DS3GSUBSMGR_H */
