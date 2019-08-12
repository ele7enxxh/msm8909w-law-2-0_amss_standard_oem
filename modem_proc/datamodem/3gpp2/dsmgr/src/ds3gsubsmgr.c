/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             3G  DATA  SERVICES  SUBSCRIPTION  MANAGER

GENERAL DESCRIPTION
  This software unit manages the subscriptions for DS. It provides APIs for
  subscription related properties.
  Will map CM subs id value to DS internal subs id index values.

Copyright (c) 2013-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gsubsmgr.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/13   rs      Fixed KW errors
04/25/13   ss      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_profile.h"
#include "ds3gsubsmgr.h"
#include "data_msg.h"
#include "ds3gutil.h"

#include "nv_items.h"
#include "modem_mem.h"
#include "queue.h"

#include "ps_sys.h"

/*===========================================================================

                        LOCAL DATA STRUCTURES

===========================================================================*/
typedef struct
{
  ds3gsubsmgr_client_type       client_id;
  ds3gsubsmgr_event_type        event_id;
}ds3gsubsmgr_client_reg_info;

typedef struct
{
  q_link_type                   link;
  ds3gsubsmgr_client_type       client_id;
  ds3gsubsmgr_event_type        event_id;
  ds3gsubsmgr_notify_func_type  func_ptr;
  void                         *data_ptr;
}ds3gsubsmgr_event_data_block;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Array for info about each subscription
---------------------------------------------------------------------------*/
static ds3gsubsmgr_subs_type ds3gsubsmgr_subs[DS3GSUBSMGR_SUBS_ID_MAX] = {{0}};

/*---------------------------------------------------------------------------
  Standby preference of the phone
---------------------------------------------------------------------------*/
static sys_modem_dual_standby_pref_e_type ds3gsubsmgr_standby_pref =
                                                     SYS_MODEM_DS_PREF_NONE;

/*---------------------------------------------------------------------------
  Device operation mode - single/dual sim, default=single sim
---------------------------------------------------------------------------*/
static sys_modem_device_mode_e_type ds3gsubsmgr_device_mode =
                                            SYS_MODEM_DEVICE_MODE_SINGLE_SIM;

/*---------------------------------------------------------------------------
  Cached values.
  Corresponding values are present in ds3gsubsmgr_subs
---------------------------------------------------------------------------*/
static ds3gsubsmgr_subs_id_e_type ds3gsubsmgr_def_data_subs_index =
                                                     DS3GSUBSMGR_SUBS_ID_MIN;
static ds3gsubsmgr_subs_id_e_type ds3gsubsmgr_def_voice_subs_index =
                                                     DS3GSUBSMGR_SUBS_ID_MIN;

/*---------------------------------------------------------------------------
  Queue of registered events
---------------------------------------------------------------------------*/
static q_type ds3gsubsmgr_event_q = {{NULL}};

/*---------------------------------------------------------------------------
  Client id counter
---------------------------------------------------------------------------*/
static ds3gsubsmgr_client_type ds3gsubsmgr_client_id_ctr = 0;

#define SYS_SYS_MODE_MASK_ALL      (SYS_SYS_MODE_MASK_CDMA   | \
                                    SYS_SYS_MODE_MASK_HDR    | \
                                    SYS_SYS_MODE_MASK_GSM    | \
                                    SYS_SYS_MODE_MASK_WCDMA  | \
                                    SYS_SYS_MODE_MASK_WLAN   | \
                                    SYS_SYS_MODE_MASK_LTE    | \
                                    SYS_SYS_MODE_MASK_TDS  )

/*===========================================================================

                    INTERNAL FUNCTIONS

===========================================================================*/

/*=========================================================================== 
FUNCTION      ds3gsubsmgr_compare_function

DESCRIPTION   Utility function to compare two items.

DEPENDENCIES  None.

RETURN VALUE  1: if the items are equal
              0: if itemsa are not equal

SIDE EFFECTS  None.
===========================================================================*/
int ds3gsubsmgr_compare_function
(
  void  *item_ptr,
  void  *target_val
)
{
  ds3gsubsmgr_event_data_block  *data_block;
  ds3gsubsmgr_client_reg_info   *to_compare;
  boolean                       ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((item_ptr == NULL) ||(target_val == NULL))
  {
    return FALSE;
  }
  data_block = (ds3gsubsmgr_event_data_block *)item_ptr;
  to_compare = (ds3gsubsmgr_client_reg_info *)target_val;

  if((data_block->client_id == to_compare->client_id) &&
     (data_block->event_id == to_compare->event_id))
  {
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
  }
  return ret_val;
}/* ds3gsubsmgr_compare_function()*/

/*===========================================================================
FUNCTION      ds3gsubsmgr_event_notify

DESCRIPTION   Function to notify registered clients.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gsubsmgr_event_notify
(
  ds3gsubsmgr_event_type        event_id,
  ds3gsubsmgr_event_info_type  *event_info
)
{
  uint8                          q_count = 0;
  ds3gsubsmgr_event_data_block  *event_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "ds3gsubsmgr_event_notify event_id:%d",
                  event_id);

  q_count = q_cnt(&ds3gsubsmgr_event_q);
  while(q_count--)
  {
    event_block = (ds3gsubsmgr_event_data_block*)q_get(&ds3gsubsmgr_event_q);
    if(event_block)
    {
      if(event_id == event_block->event_id)
      {
        if(event_block->func_ptr != NULL)
        {
          event_block->func_ptr(event_id, event_info, event_block->data_ptr);
        }
      }
      q_put(&ds3gsubsmgr_event_q, &(event_block->link));
    }
  }
  return;
}/* ds3gsubsmgr_notify_event() */

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
void ds3gsubsmgr_init( void )
{
  ds3gsubsmgr_subs_id_e_type  subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
  sys_modem_as_id_e_type      cm_subs_id;
  nv_mode_enum_type           nv_mode_pref;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize event queues for event registration and notification */
  q_init(&ds3gsubsmgr_event_q);
  ds3gsubsmgr_client_id_ctr = 0;

  memset(ds3gsubsmgr_subs, 0,
         DS3GSUBSMGR_SUBS_ID_MAX*sizeof(ds3gsubsmgr_subs_type));

  /* Initialize subsmgr data, default data and default voice are on subs 1 */
  for( subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
       subs_index < DS3GSUBSMGR_SUBS_ID_COUNT; subs_index++ )
  {
    cm_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(subs_index);
    ds3gsubsmgr_subs[subs_index].as_id = cm_subs_id;

    if( subs_index == DS3GSUBSMGR_SUBS_ID_1)
    {
      ds3gsubsmgr_subs[subs_index].is_default_data_subs = TRUE;
      ds3gsubsmgr_subs[subs_index].is_default_voice_subs = TRUE;
      ds3gsubsmgr_def_data_subs_index  = DS3GSUBSMGR_SUBS_ID_1;
      ds3gsubsmgr_def_voice_subs_index = DS3GSUBSMGR_SUBS_ID_1;
    }
    else
    {
      ds3gsubsmgr_subs[subs_index].is_default_data_subs  = FALSE;
      ds3gsubsmgr_subs[subs_index].is_default_voice_subs = FALSE;
    }

    ds3gsubsmgr_subs[subs_index].mode_pref = CM_MODE_PREF_AUTOMATIC;

    /* Read Mode preference NV for subscription id */
    nv_mode_pref = ds3g_get_nv_mode_pref( cm_subs_id );

    ds3gsubsmgr_subs[subs_index].mode_pref = 
      ds3gsubsmgr_convert_nv_mode_to_cm_mode(nv_mode_pref);

    ds3gsubsmgr_subs[subs_index].subs_capability = SYS_SYS_MODE_MASK_ALL;    
  }

  ds3gsubsmgr_standby_pref = SYS_MODEM_DS_PREF_NONE;
  ds3gsubsmgr_device_mode  = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;

  return;
}/* ds3gsubsmgr_init() */

/*===========================================================================
FUNCTION      DS3GSUBSMGR_EVENT_REG

DESCRIPTION   Called by clients to register to get DS3GSUBSMGR event.

DEPENDENCIES  None.

RETURN VALUE  TRUE: If registration successful.
              FALSE: If registration unsuccessful.

SIDE EFFECTS  Registered client is entered on a queue.
===========================================================================*/
boolean ds3gsubsmgr_event_reg
(
  ds3gsubsmgr_event_type        event_id,
  ds3gsubsmgr_notify_func_type  cb_func_ptr,
  void                         *cb_data_ptr,
  ds3gsubsmgr_client_type      *client_id
)
{
  boolean                        ret_val = FALSE;
  ds3gsubsmgr_event_data_block  *event_block_to_insert = NULL;
  ds3gsubsmgr_event_info_type    event_info;
  ds3gsubsmgr_subs_id_e_type     subs_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "ds3gsubsmgr_event_reg event:%d", event_id);

  if(client_id == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL ptr passed for client_id");
    return FALSE;
  }

  /*----------------------------------------------------------------------- 
    Allocate memory on heap
  -----------------------------------------------------------------------*/
  event_block_to_insert = (ds3gsubsmgr_event_data_block*)modem_mem_alloc(
                                          sizeof(ds3gsubsmgr_event_data_block),
                                          MODEM_MEM_CLIENT_DATA);

  if(event_block_to_insert != NULL)
  {
    memset(event_block_to_insert, 0 ,sizeof(ds3gsubsmgr_event_data_block));
    event_block_to_insert->event_id = event_id;
    event_block_to_insert->func_ptr = cb_func_ptr;
    event_block_to_insert->data_ptr = cb_data_ptr;
    event_block_to_insert->client_id = ds3gsubsmgr_client_id_ctr++;

    q_link((void*)event_block_to_insert, &(event_block_to_insert->link));
    q_put(&ds3gsubsmgr_event_q, &(event_block_to_insert->link));

    *client_id = event_block_to_insert->client_id;
    ret_val = TRUE;
  }

  memset(&event_info, 0, sizeof(ds3gsubsmgr_event_info_type));
  switch(event_id)
  {
    case DS3GSUBSMGR_DEVICE_MODE_EV:
      event_info.device_mode.curr = ds3gsubsmgr_device_mode;
      event_info.device_mode.prev = SYS_MODEM_DEVICE_MODE_NONE;

      if(cb_func_ptr != NULL)
      {
        cb_func_ptr(DS3GSUBSMGR_DEVICE_MODE_EV,
                    &event_info,
                    cb_data_ptr);
      }
      break;

    case DS3GSUBSMGR_DDS_CHANGE_EV:
      event_info.def_data_subs.curr = ds3gsubsmgr_def_data_subs_index;
      event_info.def_data_subs.prev = DS3GSUBSMGR_SUBS_ID_MIN;

      if(cb_func_ptr != NULL)
      {
        cb_func_ptr(DS3GSUBSMGR_DDS_CHANGE_EV,
                    &event_info,
                    cb_data_ptr);
      }
      break;

    case DS3GSUBSMGR_DVS_CHANGE_EV:
      event_info.def_voice_subs.curr = ds3gsubsmgr_def_voice_subs_index;
      event_info.def_voice_subs.prev = DS3GSUBSMGR_SUBS_ID_MIN;

      if(cb_func_ptr != NULL)
      {
        cb_func_ptr(DS3GSUBSMGR_DVS_CHANGE_EV,
                    &event_info,
                    cb_data_ptr);
      }
      break;

    case DS3GSUBSMGR_MODE_PREF_CHG_EV:
      for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN; 
          subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT; 
          subs_indx++)
      {
        event_info.mode_pref.curr = ds3gsubsmgr_subs[subs_indx].mode_pref;
        event_info.mode_pref.prev = CM_MODE_PREF_AUTOMATIC;
        event_info.mode_pref.subs_id = subs_indx;

        if(cb_func_ptr != NULL)
        {
          cb_func_ptr(DS3GSUBSMGR_MODE_PREF_CHG_EV,
                      &event_info,
                      cb_data_ptr);
        }
      }
      break;

    case DS3GSUBSMGR_SUBS_CAPABILITY_CHG_EV:
      for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN; 
          subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT; 
          subs_indx++)
      {
        event_info.subs_capability.curr = ds3gsubsmgr_subs[subs_indx].subs_capability;
        event_info.subs_capability.prev = SYS_SYS_MODE_MASK_ALL;
        event_info.subs_capability.subs_id = subs_indx;

        if(cb_func_ptr != NULL)
        {
          cb_func_ptr(DS3GSUBSMGR_SUBS_CAPABILITY_CHG_EV,
                      &event_info,
                      cb_data_ptr);
        }
      }
      break;    

    default:
      break;
  }

  return ret_val;
}/* ds3gsubsmgr_event_reg() */

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
)
{
  boolean                        ret_val = FALSE;
  ds3gsubsmgr_event_data_block  *event_block_deleted = NULL;
  ds3gsubsmgr_client_reg_info    client_event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3gsubsmgr_event_dereg() client_id: %d event: %d",
                  client_id, event_id);

  memset(&client_event_info, 0, sizeof(ds3gsubsmgr_client_reg_info));
  client_event_info.client_id = client_id;
  client_event_info.event_id = event_id;

  event_block_deleted = q_linear_delete_new(&ds3gsubsmgr_event_q,
                                            ds3gsubsmgr_compare_function,
                                            (void *)&client_event_info,
                                            NULL,
                                            NULL );
  if(event_block_deleted != NULL)
  {
    /* free allocated memory  */
    modem_mem_free(event_block_deleted, MODEM_MEM_CLIENT_DATA);
    ret_val = TRUE;
  }

  return ret_val;
}/* ds3gsubsmgr_event_dereg() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif

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
)
{
  cm_mode_pref_e_type cm_mode_pref;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(nv_mode_pref)
  {
    case NV_MODE_DIGITAL_PREF:
    case NV_MODE_DIGITAL_ONLY:
      cm_mode_pref = CM_MODE_PREF_DIGITAL_ONLY;
      break;

    case NV_MODE_ANALOG_PREF:
    case NV_MODE_ANALOG_ONLY:
      cm_mode_pref = CM_MODE_PREF_AMPS_ONLY;
      break;

    case NV_MODE_AUTOMATIC:
      cm_mode_pref = CM_MODE_PREF_AUTOMATIC;
      break;

    case NV_MODE_E911:
      cm_mode_pref = CM_MODE_PREF_EMERGENCY;
      break;

    case NV_MODE_PCS_CDMA_ONLY:
    case NV_MODE_CELL_CDMA_ONLY:
    case NV_MODE_CDMA_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_ONLY;
      break;

    case NV_MODE_HDR_ONLY:
      cm_mode_pref = CM_MODE_PREF_HDR_ONLY;
      break;

    case NV_MODE_CDMA_AMPS_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_AMPS_ONLY;
      break;

    case NV_MODE_GPS_ONLY:
      cm_mode_pref = CM_MODE_PREF_GPS_ONLY;
      break;

    case NV_MODE_GSM_ONLY:
      cm_mode_pref = CM_MODE_PREF_GSM_ONLY;
      break;

    case NV_MODE_WCDMA_ONLY:
      cm_mode_pref = CM_MODE_PREF_WCDMA_ONLY;
      break;

    case NV_MODE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_WLAN_ONLY;
      break;

    case NV_MODE_ANY_BUT_HDR:
      cm_mode_pref = CM_MODE_PREF_ANY_BUT_HDR;
      break;

    case NV_MODE_GSM_WCDMA_ONLY:
      cm_mode_pref = CM_MODE_PREF_GSM_WCDMA_ONLY;
      break;

    case NV_MODE_DIGITAL_LESS_HDR_ONLY:
      cm_mode_pref = CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY;
      break;

    case NV_MODE_CDMA_HDR_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_HDR_ONLY;
      break;

    case NV_MODE_CDMA_AMPS_HDR_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_AMPS_HDR_ONLY;
      break;

    case NV_MODE_CDMA_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_WLAN;
      break;

    case NV_MODE_HDR_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_HDR_WLAN;
      break;

    case NV_MODE_CDMA_HDR_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_HDR_WLAN;
      break;

    case NV_MODE_GSM_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_GSM_WLAN;
      break;

    case NV_MODE_WCDMA_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_WCDMA_WLAN;
      break;

    case NV_MODE_GW_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_GW_WLAN;
      break;

    case NV_MODE_CDMA_AMPS_HDR_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY;
      break;

    case NV_MODE_CDMA_AMPS_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY;
      break;

    case NV_MODE_ANY_BUT_HDR_WLAN:
      cm_mode_pref = CM_MODE_PREF_ANY_BUT_HDR_WLAN;
      break;

    case NV_MODE_LTE_ONLY:
      cm_mode_pref = CM_MODE_PREF_LTE_ONLY;
      break;

    case NV_MODE_GWL:
      cm_mode_pref = CM_MODE_PREF_GWL;
      break;

    case NV_MODE_CDMA_LTE_ONLY:
    case NV_MODE_CDMA_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_LTE_ONLY;
      break;

    case NV_MODE_HDR_LTE_ONLY:
    case NV_MODE_HDR_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_HDR_LTE_ONLY;
      break;

    case NV_MODE_GSM_LTE_ONLY:
    case NV_MODE_GSM_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_GSM_LTE;
      break;

    case NV_MODE_WCDMA_LTE_ONLY:
    case NV_MODE_WCDMA_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_WCDMA_LTE;
      break;

    case NV_MODE_CDMA_HDR_LTE_ONLY:
    case NV_MODE_CDMA_HDR_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_HDR_LTE_ONLY;
      break;

    case NV_MODE_CDMA_GSM_LTE_ONLY:
    case NV_MODE_CDMA_GSM_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_GSM_LTE;
      break;

    case NV_MODE_CDMA_WCDMA_LTE_ONLY:
    case NV_MODE_CDMA_WCDMA_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_WCDMA_LTE;
      break;

    case NV_MODE_HDR_GSM_LTE_ONLY:
    case NV_MODE_HDR_GSM_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_HDR_GSM_LTE;
      break;

    case NV_MODE_HDR_WCDMA_LTE_ONLY:
    case NV_MODE_HDR_WCDMA_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_HDR_WCDMA_LTE;
      break;

    case NV_MODE_ANY_BUT_WLAN:
      cm_mode_pref = CM_MODE_PREF_ANY_BUT_WLAN;
      break;

    case NV_MODE_CDMA_GSM_WCDMA:
      cm_mode_pref = CM_MODE_PREF_CDMA_GW;
      break;

    case NV_MODE_CDMA_HDR_GSM_WCDMA:
      cm_mode_pref = CM_MODE_PREF_CDMA_HDR_GW;
      break;

    default:
      cm_mode_pref = CM_MODE_PREF_AUTOMATIC;
      break;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "ds3gsubsmgr_convert_nv_mode_to_cm_mode "
                  "nv_mode:%d cm_mode:%d",
                  nv_mode_pref, cm_mode_pref);
  return cm_mode_pref;
}/* ds3gsubsmgr_convert_nv_mode_to_cm_mode() */

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
)
{
  boolean retval;

  retval = (subs_id == ds3gsubsmgr_get_def_data_subs_id());
  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3gsubsmgr_is_def_data_subs_id subs_id:%d retval:%d",
                  subs_id, retval);

  return retval;
} /* ds3gsubsmgr_is_def_data_subs_id() */

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_DEF_DATA_SUBS_ID

DESCRIPTION   This function fetches the default data subscription id.

DEPENDENCIES  None.

RETURN VALUE  Default data subscription id.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_as_id_e_type ds3gsubsmgr_get_def_data_subs_id(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Validate cached subs index value */
  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3gsubsmgr_def_data_subs_index))
  {
    return SYS_MODEM_AS_ID_NONE;
  }

  return ds3gsubsmgr_subs[ds3gsubsmgr_def_data_subs_index].as_id;
}/* ds3gsubsmgr_get_def_data_subs_id() */

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_DEF_VOICE_SUBS_ID

DESCRIPTION   This function fetches the default voice subscription id.

DEPENDENCIES  None.

RETURN VALUE  Default voice subscription id.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_as_id_e_type ds3gsubsmgr_get_def_voice_subs_id(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Validate cached subs index value */
  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3gsubsmgr_def_voice_subs_index))
  {
    return SYS_MODEM_AS_ID_NONE;
  }

  return ds3gsubsmgr_subs[ds3gsubsmgr_def_voice_subs_index].as_id;
} /* ds3gsubsmgr_get_def_voice_subs_id() */

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_STANDBY_PREF

DESCRIPTION   This function fetches the standby preference.

DEPENDENCIES  None.

RETURN VALUE  Standby preference type.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_dual_standby_pref_e_type ds3gsubsmgr_get_standby_pref(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3gsubsmgr_standby_pref;
}/* ds3gsubsmgr_get_standby_pref() */

/*===========================================================================
FUNCTION      DS3GSUBSMGR_GET_DEVICE_MODE

DESCRIPTION   This function fetches the current device mode, i.e. single sim,
              dsds, dsda

DEPENDENCIES  None.

RETURN VALUE  Current device mode.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_device_mode_e_type ds3gsubsmgr_get_device_mode(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate cached device mode value */
  if( ds3gsubsmgr_device_mode <= SYS_MODEM_DEVICE_MODE_NONE ||
      ds3gsubsmgr_device_mode >= SYS_MODEM_DEVICE_MODE_MAX )
  {
    
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid cached device mode %d",
                    ds3gsubsmgr_device_mode);
    return SYS_MODEM_DEVICE_MODE_NONE;
  }

  return ds3gsubsmgr_device_mode;
}/* ds3gsubsmgr_get_device_mode() */

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Validate cached subs index value */
  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
  {
    return CM_MODE_PREF_NONE;
  }

  return ds3gsubsmgr_subs[subs_indx].mode_pref;
}/* ds3gsubsmgr_get_mode_pref_for_subs_index() */

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
)
{
  ds3gsubsmgr_subs_id_e_type subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  return ds3gsubsmgr_get_mode_pref_for_subs_index(subs_index);
}/* ds3gsubsmgr_get_mode_pref_for_subs_id() */

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Validate cached subs index value */
  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx))
  {
    return SYS_SYS_MODE_MASK_NONE;
  }

  return ds3gsubsmgr_subs[subs_indx].subs_capability;
}/* ds3gsubsmgr_get_subs_capability_for_subs_index() */

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
)
{
  ds3gsubsmgr_subs_id_e_type subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  return ds3gsubsmgr_get_subs_capability_for_subs_index(subs_index);
}/* ds3gsubsmgr_get_subs_capability_for_subs_id() */


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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3gsubsmgr_set_def_data_subs_id(active_subs_id);
  ds3gsubsmgr_set_def_voice_subs_id(active_subs_id);

  return;
} /* ds3gsubsmgr_set_single_standby_active_subs() */

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SET_DEFAULT_DATA_SUBS_INDEX

DESCRIPTION   This function updates the default data subscription id in 
              ds3g subs mgr state info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  If the incoming subs id is not valid, update will not happen.
===========================================================================*/ 
void ds3gsubsmgr_set_default_data_subs_index
(
  ds3gsubsmgr_subs_id_e_type subs_index
)
{
  ds3gsubsmgr_subs_id_e_type   index = DS3GSUBSMGR_SUBS_ID_MIN;
  ds3gsubsmgr_event_info_type  event_info;
  ds3gsubsmgr_subs_id_e_type   old_default_data_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate */
  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(subs_index) )
  {
    return;
  }

  /* Update cache */
  old_default_data_subs_id = ds3gsubsmgr_def_data_subs_index;
  ds3gsubsmgr_def_data_subs_index = subs_index;

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "ds3gsubsmgr_set_default_data_subs_index "
                  "old DDS:%d new DDS:%d",
                  old_default_data_subs_id, ds3gsubsmgr_def_data_subs_index);

  /* Update state info */
  for( index = DS3GSUBSMGR_SUBS_ID_MIN; 
       index < DS3GSUBSMGR_SUBS_ID_COUNT; index++ )
  {
    if( index == subs_index )
    {
      ds3gsubsmgr_subs[index].is_default_data_subs = TRUE; 
    }
    else  
    {
      ds3gsubsmgr_subs[index].is_default_data_subs = FALSE;
    }
  }

  /* Notify all registered clients */
  if(old_default_data_subs_id != ds3gsubsmgr_def_data_subs_index)
  {
    memset(&event_info, 0, sizeof(ds3gsubsmgr_event_info_type));
    event_info.def_data_subs.curr = ds3gsubsmgr_def_data_subs_index;
    event_info.def_data_subs.prev = old_default_data_subs_id;
    ds3gsubsmgr_event_notify(DS3GSUBSMGR_DDS_CHANGE_EV, &event_info);
  }

  return;
}/* ds3gsubsmgr_set_default_data_subs_index() */

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
)
{
  ds3gsubsmgr_subs_id_e_type    subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
  ds_sys_subscription_enum_type ds_subs_index = DS_SYS_DEFAULT_SUBS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );
  ds3gsubsmgr_set_default_data_subs_index(subs_index);

  /* update ps sys about default subs id if valid */
  if( ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    ds_subs_index = ds3gsubsmgr_subs_id_cm_to_ds(subs_id);
    ps_sys_set_default_data_subscription(
       (ps_sys_subscription_enum_type)ds_subs_index);
  }

  return;
}/* ds3gsubsmgr_set_def_data_subs_id() */

/*===========================================================================
FUNCTION      DS3GSUBSMGR_SET_DEFAULT_VOICE_SUBS_INDEX

DESCRIPTION   This function updates the default voice subscription id in 
              ds3g subs mgr state info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/ 
void ds3gsubsmgr_set_default_voice_subs_index
(
  ds3gsubsmgr_subs_id_e_type subs_index
)
{
  ds3gsubsmgr_subs_id_e_type   index = DS3GSUBSMGR_SUBS_ID_MIN;
  ds3gsubsmgr_event_info_type  event_info;
  ds3gsubsmgr_subs_id_e_type   old_default_voice_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate */
  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(subs_index) )
  {
    return;
  }

  /* Update cache */
  old_default_voice_subs_id = ds3gsubsmgr_def_voice_subs_index;
  ds3gsubsmgr_def_voice_subs_index = subs_index;

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "ds3gsubsmgr_set_default_voice_subs_index "
                  "old DDS:%d new DDS:%d",
                  old_default_voice_subs_id, ds3gsubsmgr_def_voice_subs_index);

  /* Update state info */
  for( index = DS3GSUBSMGR_SUBS_ID_MIN; 
         index < DS3GSUBSMGR_SUBS_ID_COUNT; index++ )
  {
    if( index == ds3gsubsmgr_def_voice_subs_index )
    {
      ds3gsubsmgr_subs[index].is_default_voice_subs = TRUE; 
    }
    else
    {
      ds3gsubsmgr_subs[index].is_default_voice_subs = FALSE;
    }
  }

  /* Notify all registered clients */
  if(old_default_voice_subs_id != ds3gsubsmgr_def_voice_subs_index)
  {
    memset(&event_info, 0, sizeof(ds3gsubsmgr_event_info_type));
    event_info.def_voice_subs.curr = ds3gsubsmgr_def_voice_subs_index;
    event_info.def_voice_subs.prev = old_default_voice_subs_id;
    ds3gsubsmgr_event_notify(DS3GSUBSMGR_DVS_CHANGE_EV, &event_info);
  }

  return;
}/* ds3gsubsmgr_set_default_voice_subs_index() */

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
)
{
  ds3gsubsmgr_subs_id_e_type subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );
  ds3gsubsmgr_set_default_voice_subs_index(subs_index);

  return;
}/* ds3gsubsmgr_set_def_voice_subs_id() */

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
)
{
  ds3gsubsmgr_subs_id_e_type   subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
  ds3gsubsmgr_event_info_type  event_info;
  cm_mode_pref_e_type          old_mode_pref;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* convert CM subs id to DS subs id */
  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  /* Validate subs index */
  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(subs_index) )
  {
    return;
  }

  /* Update cache */
  old_mode_pref = ds3gsubsmgr_subs[subs_index].mode_pref;
  ds3gsubsmgr_subs[subs_index].mode_pref = mode_pref;

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "ds3gsubsmgr_set_mode_pref subs_id:%d old mode_pref:%d "
                  "new mode_pref:%d",
                  subs_id, old_mode_pref, mode_pref);

  /* Notify all registered clients */
  if(old_mode_pref != mode_pref)
  {
    memset(&event_info, 0, sizeof(ds3gsubsmgr_event_info_type));
    event_info.mode_pref.subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
    event_info.mode_pref.curr = mode_pref;
    event_info.mode_pref.prev = old_mode_pref;
    ds3gsubsmgr_event_notify(DS3GSUBSMGR_MODE_PREF_CHG_EV, &event_info);
  }
  return;
}/* ds3gsubsmgr_set_mode_pref() */

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
)
{
  ds3gsubsmgr_subs_id_e_type   subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
  ds3gsubsmgr_event_info_type  event_info;
  sys_sys_mode_mask_e_type     old_subs_capability;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* convert CM subs id to DS subs id */
  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  /* Validate subs index */
  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(subs_index) )
  {
    return;
  }

  /* Update cache */
  old_subs_capability = ds3gsubsmgr_subs[subs_index].subs_capability;
  ds3gsubsmgr_subs[subs_index].subs_capability = subs_capability;

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "ds3gsubsmgr_set_subs_capability subs_id:%d "
                  "old subs_capability:%d new subs_capability:%d",
                  subs_id, old_subs_capability, subs_capability);

  /* Notify all registered clients */
  if(old_subs_capability != subs_capability)
  {
    memset(&event_info, 0, sizeof(ds3gsubsmgr_event_info_type));
    event_info.subs_capability.subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
    event_info.subs_capability.curr = subs_capability;
    event_info.subs_capability.prev = old_subs_capability;
    ds3gsubsmgr_event_notify(DS3GSUBSMGR_SUBS_CAPABILITY_CHG_EV, &event_info);
  }

  return;
}/* ds3gsubsmgr_set_subs_capability() */

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3gsubsmgr_standby_pref = pref;
  return;
}/* ds3gsubsmgr_set_standby_pref() */

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
)
{
  ds3gsubsmgr_event_info_type   event_info;
  sys_modem_device_mode_e_type  old_device_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate input device mode value */
  if( mode <= SYS_MODEM_DEVICE_MODE_NONE || 
      mode >= SYS_MODEM_DEVICE_MODE_MAX )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Cannot set invalid device mode %d",
                    mode);
    return;
  }

  /* Update cache */
  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, "Update device mode %d to %d",
                    ds3gsubsmgr_device_mode,mode);
  old_device_mode = ds3gsubsmgr_device_mode;
  ds3gsubsmgr_device_mode = mode;

  /* Notify all registered clients */
  if(old_device_mode != ds3gsubsmgr_device_mode)
  {
    memset(&event_info, 0, sizeof(ds3gsubsmgr_event_info_type));
    event_info.device_mode.curr = ds3gsubsmgr_device_mode;
    event_info.device_mode.prev = old_device_mode;
    ds3gsubsmgr_event_notify(DS3GSUBSMGR_DEVICE_MODE_EV, &event_info);
  }

  return;
}/* ds3gsubsmgr_set_device_mode() */

/*===========================================================================
FUNCTION      DS3GSUBSMGR_IS_DEVICE_MODE_SINGLE_SIM

DESCRIPTION   This function returns TRUE if the device mode is single sim.

DEPENDENCIES  None.

RETURN VALUE  TRUE:     the device mode is single sim 
              FALSE:    the device mode is not single sim

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gsubsmgr_is_device_mode_single_sim(void)
{
  boolean retval;
  retval = (ds3gsubsmgr_device_mode == SYS_MODEM_DEVICE_MODE_SINGLE_SIM);

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                  "ds3gsubsmgr_is_device_mode_single_sim: %d",
                  retval);
  return retval;
} /* ds3gsubsmgr_is_device_mode_single_sim() */

/*===========================================================================
FUNCTION      DS3GSUBSMGR_IS_DEVICE_MODE_MULTI_SIM_STANDBY

DESCRIPTION   This function returns TRUE if the device mode is DSDS or TSTS.

DEPENDENCIES  None.

RETURN VALUE  TRUE:     device mode is multi standby 
              FALSE:    device mode is not multi standby

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gsubsmgr_is_device_mode_multi_sim_standby(void)
{
  boolean retval;

  //TODO: Rework needed work dual data DSDA */
  retval 
    = ((ds3gsubsmgr_device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY) ||
       (ds3gsubsmgr_device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE)
#if defined(FEATURE_TRIPLE_SIM)
    || (ds3gsubsmgr_device_mode == SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY)
#endif /*FEATURE_TRIPLE_SIM*/
     );

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                  "ds3gsubsmgr_is_device_mode_multi_sim_standby. device_mode:%d",
                  ds3gsubsmgr_device_mode);

  return retval;
} /* ds3gsubsmgr_is_device_mode_multi_sim_standby() */

/*===========================================================================
FUNCTION      DS3GSUBSMGR_IS_DEVICE_MODE_MULTI_SIM_ACTIVE

DESCRIPTION   This function returns TRUE if the device mode is DSDA .

DEPENDENCIES  None.

RETURN VALUE  TRUE:     device mode is multi standby 
              FALSE:    device mode is not multi standby

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gsubsmgr_is_device_mode_multi_sim_active(void)
{
  boolean retval;

  retval 
    =  (ds3gsubsmgr_device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE);

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                  "ds3gsubsmgr_is_device_mode_multi_sim_active. device_mode:%d",
                  ds3gsubsmgr_device_mode);

  return retval;
} /* ds3gsubsmgr_is_device_mode_multi_sim_active() */


/*===========================================================================
FUNCTION      DS3GSUBSMGR_SUBS_ID_CM_TO_DS

DESCRIPTION   This function converts the input CM subscription id to 
              PS Subscription Id

DEPENDENCIES  None.

RETURN VALUE  DS subscrption ID

SIDE EFFECTS  None.
===========================================================================*/
ds_sys_subscription_enum_type ds3gsubsmgr_subs_id_cm_to_ds
(
  sys_modem_as_id_e_type cm_subs_id
)
{
  ds_sys_subscription_enum_type ds_subs_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cm_subs_id )
  {
    case SYS_MODEM_AS_ID_1:
      ds_subs_id = DS_SYS_PRIMARY_SUBS;
      break;

    case SYS_MODEM_AS_ID_2:
      ds_subs_id = DS_SYS_SECONDARY_SUBS;
      break;

#if defined(FEATURE_TRIPLE_SIM)
    case SYS_MODEM_AS_ID_3:
      ds_subs_id = DS_SYS_TERTIARY_SUBS;
      break;
#endif /*defined(FEATURE_TRIPLE_SIM)*/

    /* No change is not allowed for this conversion */
    case SYS_MODEM_AS_ID_NO_CHANGE:
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "subs_id_cm_to_ds(): Unknown cm subs id %d",
                      cm_subs_id);
      ds_subs_id = DS_SYS_DEFAULT_SUBS;
      break;
  }

  return ds_subs_id;
} /* ds3gsubsmgr_subs_id_cm_to_ds() */

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
)
{
  ds3gsubsmgr_subs_id_e_type ds_subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cm_subs_id )
  {
    case SYS_MODEM_AS_ID_1:
      ds_subs_id = DS3GSUBSMGR_SUBS_ID_1;
      break;

    case SYS_MODEM_AS_ID_2:
      ds_subs_id = DS3GSUBSMGR_SUBS_ID_2;
      break;

#if defined(FEATURE_TRIPLE_SIM)
    case SYS_MODEM_AS_ID_3:
      ds_subs_id = DS3GSUBSMGR_SUBS_ID_3;
      break;
#endif /*defined(FEATURE_TRIPLE_SIM)*/

    /* No change is not allowed for this conversion */
    case SYS_MODEM_AS_ID_NO_CHANGE:
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "subs_id_cm_to_ds3g(): Invalid cm subs id %d",
                      cm_subs_id);
      ds_subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
      break;
  }

  return ds_subs_id;
}/* ds3gsubsmgr_subs_id_cm_to_ds3g() */

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
)
{
  sys_modem_as_id_e_type cm_subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( subs_index )
  {
    case DS3GSUBSMGR_SUBS_ID_1:
      cm_subs_id = SYS_MODEM_AS_ID_1;
      break;
  
    case DS3GSUBSMGR_SUBS_ID_2:
      cm_subs_id = SYS_MODEM_AS_ID_2;
      break;

#if defined(FEATURE_TRIPLE_SIM)
    case DS3GSUBSMGR_SUBS_ID_3:
      cm_subs_id = SYS_MODEM_AS_ID_3;
      break;
#endif /*defined(FEATURE_TRIPLE_SIM)*/

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "subs_id_ds3g_to_cm(): Invalid ds subs id %d", subs_index);
      cm_subs_id = SYS_MODEM_AS_ID_NONE;
      break;
  }

  return cm_subs_id;
}/* ds3gsubsmgr_subs_id_ds3g_to_cm() */

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
)
{
  ds_sys_subscription_enum_type ds_subs_id = DS_SYS_DEFAULT_SUBS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( subs_index )
  {
    case DS3GSUBSMGR_SUBS_ID_1:
      ds_subs_id = DS_SYS_PRIMARY_SUBS;
      break;
  
    case DS3GSUBSMGR_SUBS_ID_2:
      ds_subs_id = DS_SYS_SECONDARY_SUBS;
      break;

#if defined(FEATURE_TRIPLE_SIM)
    case DS3GSUBSMGR_SUBS_ID_3:
      ds_subs_id = DS_SYS_TERTIARY_SUBS;
      break;
#endif /*defined(FEATURE_TRIPLE_SIM)*/

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "subs_id_ds3g_to_ds(): Invalid ds subs id %d", subs_index);
      ds_subs_id = DS_SYS_DEFAULT_SUBS;
      break;
  }

  return ds_subs_id;
}/* ds3gsubsmgr_subs_id_ds3g_to_ds() */

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
)
{
  ds3gsubsmgr_subs_id_e_type subs_index = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( subs_id )
  {
    case DS_SYS_PRIMARY_SUBS:
      subs_index = DS3GSUBSMGR_SUBS_ID_1;
      break;
  
    case DS_SYS_SECONDARY_SUBS:
      subs_index = DS3GSUBSMGR_SUBS_ID_2;
      break;

#if defined(FEATURE_TRIPLE_SIM)
    case DS_SYS_TERTIARY_SUBS:
      subs_index = DS3GSUBSMGR_SUBS_ID_3;
      break;
#endif /*defined(FEATURE_TRIPLE_SIM)*/

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "subs_id_ds_to_ds3g(): Invalid ds subs id %d", subs_id);
      subs_index = DS3GSUBSMGR_SUBS_ID_INVALID;
      break;
  }

  return subs_index;
}/* ds3gsubsmgr_subs_id_ds_to_ds3g() */

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
)
{
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ds_subs_id )
  {
    case DS_SYS_PRIMARY_SUBS:
      subs_id = SYS_MODEM_AS_ID_1;
      break;
  
    case DS_SYS_SECONDARY_SUBS:
      subs_id = SYS_MODEM_AS_ID_2;
      break;

#if defined(FEATURE_TRIPLE_SIM)
    case DS_SYS_TERTIARY_SUBS:
      subs_id = SYS_MODEM_AS_ID_3;
      break;
#endif /*defined(FEATURE_TRIPLE_SIM)*/

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "subs_id_ds_to_cm(): Invalid ds subs id %d", ds_subs_id);
      subs_id = SYS_MODEM_AS_ID_NONE;
      break;
  }

  return subs_id;
}/* ds3gsubsmgr_subs_id_ds_to_cm() */

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
)
{
  ds_profile_subs_etype profile_subs_id = DS_PROFILE_ACTIVE_SUBSCRIPTION_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( cm_subs_id )
  {
    case SYS_MODEM_AS_ID_1:
      profile_subs_id = DS_PROFILE_ACTIVE_SUBSCRIPTION_1;
      break;
  
    case SYS_MODEM_AS_ID_2:
      profile_subs_id = DS_PROFILE_ACTIVE_SUBSCRIPTION_2;
      break;

#if defined(FEATURE_TRIPLE_SIM)
    case SYS_MODEM_AS_ID_3:
      profile_subs_id = DS_PROFILE_ACTIVE_SUBSCRIPTION_3;
      break;
#endif /*defined(FEATURE_TRIPLE_SIM)*/

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "subs_id_cm_to_dsprofile(): Invalid ds subs id %d",
                      profile_subs_id);
      profile_subs_id = DS_PROFILE_ACTIVE_SUBSCRIPTION_NONE;
      break;
  }

  return profile_subs_id;
} /* ds3gsubsmgr_subs_id_cm_to_dsprofile() */

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
)
{
  ds3gsubsmgr_subs_id_e_type subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  /* Range check */
  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(subs_index) )
  {
    return FALSE;
  }

  return TRUE;
} /* ds3gsubsmgr_is_subs_id_valid() */

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Range check */
  if( subs_index <= DS3GSUBSMGR_SUBS_ID_INVALID ||
      subs_index >= DS3GSUBSMGR_SUBS_ID_MAX )
  {
    DATA_3GMGR_MSG2( MSG_LEGACY_ERROR, "subs id invalid: %d, device mode %d",
                     subs_index, ds3gsubsmgr_device_mode);
    return FALSE;
  }

  return TRUE;
}/* ds3gsubsmgr_is_ds3g_subs_index_valid() */

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
)
{
  sys_sys_mode_e_type  sys_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(mode_pref)
  {
    case CM_MODE_PREF_CDMA_ONLY:
    case CM_MODE_PREF_CDMA_AMPS_ONLY:
    case CM_MODE_PREF_CDMA_WLAN:
    case CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY:
    case CM_MODE_PREF_CDMA_GSM:
    case CM_MODE_PREF_CDMA_GW_TDS:
    case CM_MODE_PREF_CDMA_GSM_AMPS:
    case CM_MODE_PREF_CDMA_GSM_GPS_AMPS:
      sys_mode = SYS_SYS_MODE_CDMA;
      break;

    case CM_MODE_PREF_HDR_ONLY:
    case CM_MODE_PREF_CDMA_HDR_ONLY:
    case CM_MODE_PREF_CDMA_AMPS_HDR_ONLY:
    case CM_MODE_PREF_HDR_WLAN:
    case CM_MODE_PREF_CDMA_HDR_WLAN:
    case CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY:
    case CM_MODE_PREF_CDMA_HDR_GW_TDS:
    case CM_MODE_PREF_CDMA_HDR_GSM_AMPS:
    case CM_MODE_PREF_CDMA_HDR_GSM_GPS_AMPS:
    case CM_MODE_PREF_HDR_GSM:
      sys_mode = SYS_SYS_MODE_HDR;
      break;

    case CM_MODE_PREF_GSM_ONLY:
    case CM_MODE_PREF_GSM_WLAN:
    case CM_MODE_PREF_GSM_GPS:
      sys_mode = SYS_SYS_MODE_GSM;
      break;

    case CM_MODE_PREF_WCDMA_ONLY:
    case CM_MODE_PREF_WCDMA_WLAN:
    case CM_MODE_PREF_GSM_WCDMA_ONLY:
    case CM_MODE_PREF_GW_WLAN:
    case CM_MODE_PREF_WCDMA_GPS:
    case CM_MODE_PREF_GW_GPS:
      sys_mode = SYS_SYS_MODE_WCDMA;
      break;

#ifdef FEATURE_TDSCDMA
    case CM_MODE_PREF_TDS_ONLY:
    case CM_MODE_PREF_TDS_GSM:
    case CM_MODE_PREF_TDS_GSM_WCDMA:
    case CM_MODE_PREF_TDS_WCDMA:
      sys_mode = SYS_SYS_MODE_TDS;
      break;
#endif /*FEATURE_TDSCDMA*/

#ifdef FEATURE_DATA_LTE
    case CM_MODE_PREF_LTE_ONLY:
    case CM_MODE_PREF_GWL:
    case CM_MODE_PREF_CDMA_LTE_ONLY:
    case CM_MODE_PREF_HDR_LTE_ONLY:
    case CM_MODE_PREF_GSM_LTE:
    case CM_MODE_PREF_WCDMA_LTE:
    case CM_MODE_PREF_CDMA_HDR_LTE_ONLY:
    case CM_MODE_PREF_CDMA_GSM_LTE:
    case CM_MODE_PREF_CDMA_WCDMA_LTE:
    case CM_MODE_PREF_HDR_GSM_LTE:
    case CM_MODE_PREF_HDR_WCDMA_LTE:
    case CM_MODE_PREF_TDS_GSM_LTE:
    case CM_MODE_PREF_TDS_GSM_WCDMA_LTE:
    case CM_MODE_PREF_TDS_LTE:
    case CM_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE:
    case CM_MODE_PREF_CDMA_GSM_WCDMA_LTE:
    case CM_MODE_PREF_CDMA_HDR_GSM_TDS_LTE:
    case CM_MODE_PREF_TDS_WCDMA_LTE:
      sys_mode = SYS_SYS_MODE_LTE;
      break;
#endif /* FEATURE_DATA_LTE */

    case CM_MODE_PREF_AUTOMATIC:
    default:
#if defined(FEATURE_DATA_LTE)
      sys_mode = SYS_SYS_MODE_LTE;
#elif defined(FEATURE_HDR)
      sys_mode = SYS_SYS_MODE_HDR;
#elif defined(FEATURE_DATA_IS707)
      sys_mode = SYS_SYS_MODE_CDMA;
#elif defined(FEATURE_WCDMA)
      sys_mode = SYS_SYS_MODE_WCDMA;
#elif defined(FEATURE_GSM)
      sys_mode = SYS_SYS_MODE_GSM;
#endif
      break;
  }

  return sys_mode;
} /* ds3gsubsmgr_mode_pref_to_sys_mode() */

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
)
{
  boolean retval = FALSE;

  /*-------------------------------------------------------------------
    Check if device mode is Single Sim or multi-sim standby, if so 
    return true is passed subs_id is DDS. If device mode is not one of 
    above return FALSE. 

    For true DSDA, this always return TRUE as PS call is allowed on any subs_id at 
    any point of time.
  --------------------------------------------------------------------*/
  if (TRUE == ds3gsubsmgr_is_device_mode_multi_sim_active())
  {
    retval = TRUE;
  }
    /*  TODO these checks can be removed, Comparing with DDS Subs is suffice */
  else if( (TRUE == ds3gsubsmgr_is_device_mode_multi_sim_standby()) ||
       (TRUE == ds3gsubsmgr_is_device_mode_single_sim()) )
  {
    retval = (subs_id == ds3gsubsmgr_get_def_data_subs_id());
  }
  
  DATA_3GMGR_MSG3(MSG_LEGACY_LOW,
                  "subs_id: %d, device mode: %d, ps_call_allowed: %d",
                  subs_id, ds3gsubsmgr_device_mode, retval);

  return retval; 
} /* ds3gsubsmgr_is_ps_call_allowed_on_subs */

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
)
{
  boolean                      retval = FALSE;
  sys_modem_device_mode_e_type device_mode = SYS_MODEM_DEVICE_MODE_NONE;
  /* ------------------------------------------------------------------------ */
  device_mode = ds3gsubsmgr_get_device_mode();
  
  if((subs_id != DS3GSUBSMGR_SUBS_ID_INVALID) &&
     ((device_mode == SYS_MODEM_DEVICE_MODE_SINGLE_SIM &&
      subs_id <= DS3GSUBSMGR_SUBS_ID_1 ) ||
     ((device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY||
      device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE)&&
      subs_id <= DS3GSUBSMGR_SUBS_ID_2 ) 
#if defined(FEATURE_TRIPLE_SIM)
        ||
     (device_mode == SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY && 
      subs_id <= DS3GSUBSMGR_SUBS_ID_3)
#endif /* FEATURE_TRIPLE_SIM */
    ))
  {
    retval = TRUE;
  }

  if( !retval )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
              "subs_id:%d invalid in device_mode:%d", subs_id, device_mode);
  }

  return retval;
}/* ds3gsubsmgr_is_subs_valid_in_current_config */

