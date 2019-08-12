/*===========================================================================

               D A T A   S E R V I C E S   D S D  I N T E R F A C E

GENERAL DESCRIPTION
  Interface between DSD and Data Services

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2010 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gdsdif.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/23/13   rs      Added support for additional WLAN information
05/24/13   sd      TSTS changes for DS3G.
01/24/12   ss      Fix to report correct rat_mask when data subscription is
                   SUB2.
10/17/12   dvk     Merged fix to tear down 3gpp2 call when moving from 1x/HDR
                   to 3gpp system if handoff is not allowed.
03/11/12   jee     To fix aborting partial context duirng  handoff
02/27/12   var     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#include "rex.h"
#include "timer.h"
#include "sys.h"
#include "modem_mem.h"

#include "data_msg.h"
#include "dstask_v.h"
#include "ps_iface.h"
#include "ps_sys.h"
#include "ds3gmgr.h"
#include "ds3gutil.h"
#include "ds3gdsdif.h"
#include "ds3gsubsmgr.h"
#include "ds3gflowmgr.h"
#include "ds3geventmgr.h"
#include "dsumtspdpreg.h"
#include "queue.h"
#include "msgr.h"
#include "msgr_ds.h"
#include "lte_rrc_ext_msg.h"
#include "dsmsgr.h"

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "dsutil.h"
#include "ps_sys.h"
#include "ps_sys_event.h"
#include "ds_wlan_util.h"
#include "ps_sys_conf.h"
#endif /*FEATURE_DATA_WLAN_MAPCON*/


#ifdef FEATURE_DATA_PKT_CLASSIFIER
#include "ds707_pkt_classifier.h"
#endif/* FEATURE_DATA_PKT_CLASSIFIER */

#ifdef FEATURE_DATA_ANDSF
#include "ds_andsf_api.h"
#endif /*FEATURE_DATA_ANDSF*/
/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

#define DS3GDSDIF_MT_CSFB_ESTAB_HYST_TIMER_VAL  12000

/*---------------------------------------------------------------------------
  SS event complete cmd info structure
---------------------------------------------------------------------------*/
typedef struct
{
  ds3gsubsmgr_subs_id_e_type  subs_id;
}ds3gdsdif_ss_event_done_cmd_type;

/*===========================================================================

                    GLOBAL VARIABLES

===========================================================================*/

/*-------------------------------------------------------------------------
  Radio status information.
-------------------------------------------------------------------------*/
static ds3gdsdif_radio_info_type
         *ds3gdsdif_radio_info[DS3GSUBSMGR_SUBS_ID_COUNT] = {NULL};

/*---------------------------------------------------------------------------
   LTE attach in progress struture
---------------------------------------------------------------------------*/
static ds3geventmgr_attach_info_type
         ds3gdsdif_lte_attach_in_progress[DS3GSUBSMGR_SUBS_ID_COUNT] = {{0}};

/*---------------------------------------------------------------------------
   Voice call/ CS data call active
---------------------------------------------------------------------------*/
static ds3gdsdif_voice_call_info_type*
         ds3gdsdif_voice_call_info[DS3GSUBSMGR_SUBS_ID_COUNT][DS3GDSDIF_VOICE_CALL_COUNT] = {{NULL}};

/*---------------------------------------------------------------------------
   Emergency call
---------------------------------------------------------------------------*/
static ds3gdsdif_voice_call_info_type*
         ds3gdsdif_emerg_call_info[DS3GSUBSMGR_SUBS_ID_COUNT][DS3GDSDIF_EMERG_CALL_COUNT] = {{NULL}};

/*---------------------------------------------------------------------------
    Critical section
---------------------------------------------------------------------------*/
static rex_crit_sect_type ds3gdsdif_crit_sect = {{0}};

/*---------------------------------------------------------------------------
  Queue to keep track of registrations by external clients for attach info
---------------------------------------------------------------------------*/
static q_type ds3gdsdif_ext_event_q = {{NULL}};

/*---------------------------------------------------------------------------
  Timer objects for MT CSFB establishment hysteresis
---------------------------------------------------------------------------*/
static timer_type ds3gdsdif_mt_csfb_estab_hyst_timer[DS3GSUBSMGR_SUBS_ID_MAX];

/*---------------------------------------------------------------------------
  Flag to keep status of WLAN notification
---------------------------------------------------------------------------*/
static boolean ds3gdsdif_wlan_notified = FALSE;

/*===========================================================================

                    INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_RADIO_INFO_PTR

DESCRIPTION   This utility function will return a pointer to the radio info
              for the passed sub id

PARAMETERS    subs_id: ds3g data sub id.

DEPENDENCIES  None.

RETURN VALUE  pointer to ds3gdsdif_radio_info[subs_id]
              NULL pointer if subs_id is invalid

SIDE EFFECTS  None.
===========================================================================*/
ds3gdsdif_radio_info_type* ds3gdsdif_get_radio_info_ptr
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    return NULL;
  }

  return ds3gdsdif_radio_info[subs_id];
}/* ds3gdsdif_get_radio_info_ptr() */

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_SYSTEM_MODE_INDEX

DESCRIPTION   This utility function will return index in radio info given
              the system mode.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  TRUE:  system_mode found
              FALSE: system_mode not found

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_get_system_mode_index
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  sys_sys_mode_e_type         system_mode,
  uint8                      *index
)
{
  boolean                     ret_val = FALSE;
  ds3g_radio_index_type       i;
  ds3gdsdif_radio_info_type  *radio_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (index == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "invalid system mode index");
    return FALSE;
  }

  radio_info_ptr = ds3gdsdif_get_radio_info_ptr(subs_id);

  if(radio_info_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_LOW,
              "radio info instance not present for subs_id: %d", subs_id);
    *index = DS3G_RADIO_INDEX_MAX;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Loop though all radios and check if system_mode matches
  -------------------------------------------------------------------------*/
  for(i = DS3G_RADIO_INDEX_MIN; i < DS3G_RADIO_INDEX_MAX; i++)
  {
    if(radio_info_ptr->radio_info[i].system_mode == system_mode)
    {
      ret_val = TRUE;
      break;
    }
  }

  *index = i;
  return ret_val;
}/* ds3gdsdif_get_system_mode_index() */

/*===========================================================================
FUNCTION      DS3GDSDIF_CREATE_RADIO_INFO_INSTANCE

DESCRIPTION   This function creates radio info instance for subs_id.

PARAMETERS    subs_id: subscription id of the instance to be created.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds3gdsdif_radio_info_type* ds3gdsdif_create_radio_info_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds3g_radio_index_type       i;
  ds3gdsdif_radio_info_type  *radio_info_instance = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  radio_info_instance = (ds3gdsdif_radio_info_type*)
                          modem_mem_alloc(sizeof(ds3gdsdif_radio_info_type),
                                          MODEM_MEM_CLIENT_DATA);

  if(radio_info_instance != NULL)
  {
    /*-------------------------------------------------------------------------
      Initialize system mode for all radios
    -------------------------------------------------------------------------*/
    memset(radio_info_instance, 0 , sizeof(ds3gdsdif_radio_info_type));
    for(i = DS3G_RADIO_INDEX_MIN; i < DS3G_RADIO_INDEX_MAX; i++)
    {
      radio_info_instance->subs_id = subs_id;
      radio_info_instance->radio_info[i].system_mode
        = ds3gstkproc_convert_radio_index_to_sys_mode(i);
      radio_info_instance->radio_info[i].rat_mask= PS_SYS_RAT_3GPP_NULL_BEARER;
    }

    ds3gdsdif_radio_info[subs_id] = radio_info_instance;
  }
  return radio_info_instance;
}/* ds3gdsdif_create_radio_info_instance() */

/*===========================================================================
FUNCTION      DS3GDSDIF_HANDLE_DEVICE_MODE_CHANGE

DESCRIPTION   This function handles device mode change.

PARAMETERS    device_mode: device mode from ds3gsubsmgr

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_handle_device_mode_change
(
  sys_modem_device_mode_e_type  device_mode
)
{
  boolean                     create_subs_instance[DS3GSUBSMGR_SUBS_ID_COUNT];
  ds3gsubsmgr_subs_id_e_type  subs_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&create_subs_instance, 0, sizeof(boolean)*DS3GSUBSMGR_SUBS_ID_COUNT);
  switch(device_mode)
  {
    case SYS_MODEM_DEVICE_MODE_SINGLE_SIM:
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_1] = TRUE;
      break;

    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY:
    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE:
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_1] = TRUE;
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_2] = TRUE;
      break;

#if defined(FEATURE_TRIPLE_SIM)
    case SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY:
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_1] = TRUE;
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_2] = TRUE;
      create_subs_instance[DS3GSUBSMGR_SUBS_ID_3] = TRUE;
      break;
#endif /*defined(FEATURE_TRIPLE_SIM)*/

    default:
      break;
  }

  /* create DSDIF radio info instance based on device mode */
  for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
      subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT;
      subs_indx++)
  {
    if(create_subs_instance[subs_indx] == TRUE)
    {
      if(ds3gdsdif_radio_info[subs_indx] == NULL)
      {
        if(ds3gdsdif_create_radio_info_instance(subs_indx) == NULL)
        {
          DATA_ERR_FATAL("Error while creating radio info instance. return.");
        }
        else
        {
          DATA_MSG1(MSG_LEGACY_LOW,
                    "create radio info instance success for subs_id:%d.",
                    subs_indx);
        }
      }
    }
  }

  /* Notify device mode to DSD */
  ds_dsd_ext_notify_device_mode(device_mode);

  /* Notify mode preference for all subs_id */
  for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
      subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT;
      subs_indx++)
  {
    if(create_subs_instance[subs_indx] == TRUE)
    {
      ds_dsd_ext_notify_mode_pref_chg(
         ds3gsubsmgr_get_mode_pref_for_subs_index(subs_indx),
         subs_indx);
    }
  }

  return;
}/* ds3gdsdif_handle_device_mode_change() */

/*===========================================================================
FUNCTION      DS3GDSDIF_SS_EVENT_COMPLETE_HDLR_CB

DESCRIPTION   This callback is called by DSD when SS event processing is
              complete.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_ss_event_complete_hdlr_cb
(
  ds3gsubsmgr_subs_id_e_type subs_id
)
{
  ds_cmd_type                       *ds3g_cmd_ptr = NULL;
  ds3gdsdif_ss_event_done_cmd_type  *data_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Post a cmd to DS3G to handle pref sys change
  -------------------------------------------------------------------------*/
  ds3g_cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3gdsdif_ss_event_done_cmd_type));
  if( (ds3g_cmd_ptr == NULL) || (ds3g_cmd_ptr->cmd_payload_ptr == NULL) )
  {
    modem_mem_free(data_block_ptr, MODEM_MEM_CLIENT_DATA);
    ASSERT(0);
    return;
  }

  DATA_MSG0(MSG_LEGACY_LOW,
            "Issuing DS_CMD_DS3GDSDIF_SS_EVENT_PROC_DONE to DS3G");

  data_block_ptr
    = (ds3gdsdif_ss_event_done_cmd_type *)ds3g_cmd_ptr->cmd_payload_ptr;

  ds3g_cmd_ptr->hdr.cmd_id = DS_CMD_DS3GDSDIF_SS_EVENT_PROC_DONE;
  data_block_ptr->subs_id = subs_id;
  ds_put_cmd(ds3g_cmd_ptr);

  return;
}/* ds3gdsdif_ss_event_complete_hdlr_cb() */

/*===========================================================================
FUNCTION      DS3GDSDIF_PREF_SYS_CHG_HDLR_CB

DESCRIPTION   This callback is called by DSD when preferred system changes.
              It posts a command to DS3G to handle the pref sys change.

PARAMETERS    ds_dsd_event_info_ptr: event info containing new system info
              cb_data_ptr: not used

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_pref_sys_chg_hdlr_cb
(
  ds_dsd_ext_event_type        event,
  ds_dsd_ext_event_info_type  *ds_dsd_event_info_ptr,
  void                        *cb_data_ptr
)
{
  ds_cmd_type                 *ds3g_cmd_ptr=NULL;
  ds_dsd_ext_event_info_type  *data_block_ptr=NULL;
  uint8                        pref_sys_index = 0;
  uint8                        old_pref_sys_index = 0;
  sys_sys_mode_e_type          old_pref_sys = SYS_SYS_MODE_NO_SRV;
  ds3gsubsmgr_subs_id_e_type   subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
  sys_sys_mode_e_type          new_pref_sys = SYS_SYS_MODE_NO_SRV;
  sys_srv_status_e_type        new_pref_srv_status = SYS_SRV_STATUS_NO_SRV;
  ds3gdsdif_radio_info_type   *radio_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if callback pointer is NULL
  -------------------------------------------------------------------------*/
  if(ds_dsd_event_info_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "NULL event info pointer");
    return;
  }

  /*-------------------------------------------------------------------------
    Check if event == PREF_SYS_CHANGED_EV
  -------------------------------------------------------------------------*/
  if(event != DS_DSD_EXT_PREF_SYS_CHANGED_EV)
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Incorrect event: %d, cannot be handled here",
              event);
    return;
  }

  subs_id = ds3gsubsmgr_subs_id_ds_to_ds3g(ds_dsd_event_info_ptr->subs_id);
  pref_sys_index = ds_dsd_event_info_ptr->pref_index;

  radio_info_ptr = ds3gdsdif_get_radio_info_ptr(subs_id);

  if (radio_info_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_LOW,
              "radio info instance not present for subs_id: %d", subs_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Check if the pref system is remote or local. If preferred system is
    remote, then no need to notify the mode handlers.
  -------------------------------------------------------------------------*/
  if(ds_dsd_event_info_ptr->dsd_radio_info[pref_sys_index].remote_flag
     == TRUE)
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Pref system is remote for subs_id:%d.",
              subs_id);
  }
  else
  {
    DATA_MSG1(MSG_LEGACY_LOW,"Pref system is local for subs_id:%d", subs_id);
  }

  /*-------------------------------------------------------------------------
    Keep a copy of old pref sys and new pref sys
  -------------------------------------------------------------------------*/
  new_pref_sys
    = ds_dsd_event_info_ptr->dsd_radio_info[pref_sys_index].system_mode;
  new_pref_srv_status
    = ds_dsd_event_info_ptr->dsd_radio_info[pref_sys_index].srv_status;

  if(radio_info_ptr != NULL)
  {
    old_pref_sys_index = radio_info_ptr->pref_index;
    old_pref_sys = radio_info_ptr->radio_info[old_pref_sys_index].system_mode;
  }

  DATA_MSG4(MSG_LEGACY_HIGH,
            "ds3gdsdif_pref_sys_chg_hdlr_cb subs_id:%d old_pref_sys:%d, "
            "new_pref_sys:%d, mode_pref_change:%d",
            subs_id, old_pref_sys, new_pref_sys,
            ds_dsd_event_info_ptr->mode_pref_notify);

  /*-------------------------------------------------------------------------
    Check if new pref sys is valid
  -------------------------------------------------------------------------*/
  if((new_pref_sys >= SYS_SYS_MODE_MAX) || (new_pref_sys == SYS_SYS_MODE_NONE))
  {
    DATA_MSG2(MSG_LEGACY_LOW, "Invalid sys change new sys:%d, status:%d ",
              new_pref_sys,
              new_pref_srv_status);
    return;
  }

  /*-------------------------------------------------------------------------
    Post a cmd to DS3G to handle pref sys change
  -------------------------------------------------------------------------*/
  ds3g_cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_dsd_ext_event_info_type));
  if( (ds3g_cmd_ptr == NULL) || (ds3g_cmd_ptr->cmd_payload_ptr == NULL ) )
  {
    modem_mem_free( data_block_ptr, MODEM_MEM_CLIENT_DATA );
    ASSERT(0);
    return;
  }

  DATA_MSG0(MSG_LEGACY_LOW,
            "Issuing DS_CMD_DS3GDSDIF_PREF_SYS_CHANGED to DS3G");

  ds3g_cmd_ptr->hdr.cmd_id = DS_CMD_DS3GDSDIF_PREF_SYS_CHANGED;

  data_block_ptr = (ds_dsd_ext_event_info_type*)ds3g_cmd_ptr->cmd_payload_ptr;

  memscpy(data_block_ptr,
          sizeof(ds_dsd_ext_event_info_type),
          ds_dsd_event_info_ptr,
          sizeof(ds_dsd_ext_event_info_type));

  ds_put_cmd(ds3g_cmd_ptr);

  return;
}/*ds3gdsdif_pref_sys_chg_hdlr_cb()*/

/*===========================================================================
FUNCTION       DS3GDSDIF_NOTIFY_WLAN_STATUS

DESCRIPTION    This function notifies wlan available or not available to DSD

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds3gdsdif_notify_wlan_status
(
   ps_sys_wlan_status_type  *wlan_info,
   boolean                   wlan_available
)
{
  ds_dsd_event_info_type  dsd_event_info = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( wlan_available )
  {
    dsd_event_info.data.wlan_param_info.wqe_supported_flag = TRUE;
    dsd_event_info.data.wlan_param_info.wqe_supported = wlan_info->wqe_status;
    dsd_event_info.event = DS_DSD_CLIENT_RADIO_ACQUIRED_EV;
    dsd_event_info.data.wlan_param_info.is_wlan_available = TRUE;
  
    memscpy(&(dsd_event_info.data.wlan_param_info.ipv4_addr),
            sizeof(ps_ip_addr_type),
            &(wlan_info->ipv4_addr),
            sizeof(ps_ip_addr_type));
  
    memscpy(&(dsd_event_info.data.wlan_param_info.ipv6_addr),
            sizeof(ps_ip_addr_type),
            &(wlan_info->ipv6_addr),
            sizeof(ps_ip_addr_type));
  
    dsd_event_info.data.wlan_param_info.ipv6_prefix_len = wlan_info->
                                                          ipv6_prefix_len;
  
    memscpy(dsd_event_info.data.wlan_param_info.ap_mac_addr,
            PS_SYS_MAC_ADDR_LEN,
            wlan_info->ap_mac_addr,
            PS_SYS_MAC_ADDR_LEN);
  
    ds3gdsdif_cpy_ps_sys_wlan_addr(&(dsd_event_info.data.wlan_param_info.dns_addr),
                                   wlan_info->dns_ipv4_addr1,
                                   wlan_info->dns_ipv4_addr2,
                                   wlan_info->dns_ipv6_addr1,
                                   wlan_info->dns_ipv6_addr2);
  
    ds3gdsdif_cpy_ps_sys_wlan_addr(&(dsd_event_info.data.wlan_param_info.epdg_addr),
                                   wlan_info->epdg_ipv4_addr1,
                                   wlan_info->epdg_ipv4_addr2,
                                   wlan_info->epdg_ipv6_addr1,
                                   wlan_info->epdg_ipv6_addr2);
  
    strlcpy(dsd_event_info.data.wlan_param_info.ssid,
            wlan_info->ssid,
            PS_SYS_MAX_SSID_LEN);
  
    DATA_3GMGR_MSG6(MSG_LEGACY_LOW,
                    "WLAN mac addr: %x, %x, %x %x, %x, %x",
                    dsd_event_info.data.wlan_param_info.ap_mac_addr[0],
                    dsd_event_info.data.wlan_param_info.ap_mac_addr[1],
                    dsd_event_info.data.wlan_param_info.ap_mac_addr[2],
                    dsd_event_info.data.wlan_param_info.ap_mac_addr[3],
                    dsd_event_info.data.wlan_param_info.ap_mac_addr[4],
                    dsd_event_info.data.wlan_param_info.ap_mac_addr[5]);
  
    DATA_3GMGR_MSG_SPRINTF_1(MSG_LEGACY_LOW,
                             "SSID: %d",
                             dsd_event_info.data.wlan_param_info.ssid);
  
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Notify wifi acquired to DSD");
    ds_dsd_ext_event_notify(DS_DSD_RADIO_WLAN, &dsd_event_info);
  }
  else
  {
    dsd_event_info.data.wlan_param_info.wqe_supported_flag = TRUE;
    dsd_event_info.data.wlan_param_info.wqe_supported = wlan_info->wqe_status;
    dsd_event_info.event = DS_DSD_CLIENT_RADIO_LOST_EV;
    dsd_event_info.data.wlan_param_info.is_wlan_available = FALSE;

    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Notify wifi lost to DSD");
    ds_dsd_ext_event_notify(DS_DSD_RADIO_WLAN, &dsd_event_info);
  }

  return;
} /* ds3gdsdif_notify_wlan_status */

/*===========================================================================
FUNCTION       DS3GDSDIF_CHECK_AND_NOTIFY_WLAN_AVAILABLE

DESCRIPTION    This function checks if wlan available is to be notified
                       to the clients or not

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds3gdsdif_check_and_notify_wlan_available
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
)
{
  ps_sys_wlan_status_type  *wlan_info = NULL;
  int16                     ps_errno = -1;
  sys_modem_as_id_e_type    cm_subs_id = SYS_MODEM_AS_ID_NONE;
  sys_sys_mode_mask_e_type  subs_capability = SYS_SYS_MODE_MASK_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (ps_sys_conf_get_ex (PS_SYS_TECH_WLAN,
                          PS_SYS_CONF_WLAN_STATUS,
                          PS_SYS_PRIMARY_SUBS,
                          (void*)wlan_info,
                          &ps_errno ) != 0 )
  {
     DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "PS_SYS_CONF_GET_EX failed on PS_SYS_CONF_WLAN_STATUS, ");
     return;
  }

  /* if WLAN is not available return */
  if ( (NULL == wlan_info) || (wlan_info->is_wlan_available == FALSE) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "WLAN not available");
    return;
  }

  cm_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id);
  subs_capability = ds3gsubsmgr_get_subs_capability_for_subs_id(cm_subs_id);

  /*-------------------------------------------------------------------------
      Send out the wlan notification only when wlan is available and make sure that
      duplicate notifications are not sent
  -------------------------------------------------------------------------*/

  if ( ( subs_capability & SYS_SYS_MODE_MASK_LTE ) &&
       ( ds3gdsdif_wlan_notified == FALSE ))
  {
    ds3gdsdif_notify_wlan_status(wlan_info, TRUE);
    ds3gdsdif_wlan_notified = TRUE;
  }

  if ( !( subs_capability & SYS_SYS_MODE_MASK_LTE ) &&
       ( ds3gdsdif_wlan_notified == TRUE ))
  {
    ds3gdsdif_notify_wlan_status(wlan_info, FALSE);
    ds3gdsdif_wlan_notified = FALSE;
  }
  
  return;
}/* ds3gdsdif_check_and_notify_wlan_available */


/*===========================================================================
FUNCTION      DS3GDSDIF_SUBSMGR_EVENT_CB

DESCRIPTION   This callback is called by DS3GSUBSMGR to notify events.

PARAMETERS    event_id: DS3GSUBSMGR event id.
              event_info_ptr: event info containing new system info
              cb_data_ptr: not used

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_subsmgr_event_cb
(
  ds3gsubsmgr_event_type        event_id,
  ds3gsubsmgr_event_info_type  *event_info_ptr,
  void                         *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check if callback pointer is NULL
  -------------------------------------------------------------------------*/
  if(event_info_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "NULL event info pointer");
    return;
  }

  DATA_MSG1(MSG_LEGACY_HIGH, "ds3gdsdif_subsmgr_event_cb event_id:%d",
            event_id);

  switch(event_id)
  {
    case DS3GSUBSMGR_DEVICE_MODE_EV:
      ds3gdsdif_handle_device_mode_change(event_info_ptr->device_mode.curr);
      break;

    case DS3GSUBSMGR_DDS_CHANGE_EV:
      ds_dsd_ext_notify_dds_change(event_info_ptr->def_data_subs.curr);
      break;

    case DS3GSUBSMGR_MODE_PREF_CHG_EV:
      /* Notify mode preference for subs_id */
      ds_dsd_ext_notify_mode_pref_chg(
         event_info_ptr->mode_pref.curr,
         event_info_ptr->mode_pref.subs_id);
      break;

    case DS3GSUBSMGR_SUBS_CAPABILITY_CHG_EV:
      if ( event_info_ptr->subs_capability.subs_id 
            == DS3GSUBSMGR_SUBS_ID_1 )
      {
        ds3gdsdif_check_and_notify_wlan_available(DS3GSUBSMGR_SUBS_ID_1);
      }
      break;      

    default:
      break;
  }

  return;
}/* ds3gdsdif_subsmgr_event_cb() */

/*===========================================================================
FUNCTION     DS3GDSDIF_PROCESS_GSM_VOICE_CALL_EVENT

DESCRIPTION  This function process the GSM voice call event.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gdsdif_process_gsm_voice_call_event
(
  sys_modem_as_id_e_type  asubs_id,
  cm_call_event_e_type    call_event
)
{
  uint8                        gsm_stack;
  ds3g_cm_ss_stack_info_type  *gsm_stack_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the stack serving GSM */
  gsm_stack = ds3gstkproc_get_stack_id_from_radio(asubs_id, SYS_SYS_MODE_GSM);
  if(gsm_stack >= CM_NO_STACKS)
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Invalid stack_id: %d", gsm_stack);
    return;
  }

  /* Get GSM stack info */
  ds3gstkproc_get_stack_info_from_stack_id(
     ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id), gsm_stack, &gsm_stack_info_ptr);

  if(NULL == gsm_stack_info_ptr)
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Invalid stack info for stack:%d", gsm_stack);
    return;
  }

  if((call_event == CM_CALL_EVENT_CONNECT) ||
     (call_event == CM_CALL_EVENT_HO_COMPLETE) ||
     (call_event == CM_CALL_EVENT_END))
  {
    /* check if DTM is supported */
    if(gsm_stack_info_ptr->cell_srv_ind.dtm_supp == SYS_DTM_SUPPORT_NOT_AVAIL)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                      "process gsm voice call, DTM:%d. Process cached stack",
                      gsm_stack_info_ptr->cell_srv_ind.dtm_supp);

      /*-----------------------------------------------------------------------
         This is to handle voice call connected/HO complete/end event. If
         voice call connected/ended and DTM not supported, parse cached
         stack info.
      -----------------------------------------------------------------------*/
      ds3gstkproc_process_cached_stack_info(asubs_id);
    }
  }
  return;
}/* ds3gdsdif_process_gsm_voice_call_event() */

/*===========================================================================
FUNCTION      DS3GDSDIF_INVOKE_EXT_CBACKS

DESCRIPTION   This function invokes the cback for the given event.

PARAMETERS    dsdif_ext_event: Event for which cback has to be invoked
              event_info:      Event information to be passed to cback func

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_invoke_ext_cbacks
(
   ds3gdsdif_ext_event_type   dsdif_ext_event,
   void                      *event_info
)
{
  ds3gdsdif_ext_event_data_block  *event_data_block = NULL;
/*---------------------------------------------------------------------------*/
  DATA_MSG1(MSG_LEGACY_LOW,
            "ds3gdsdif_invoke_ext_cbacks: Cback to be invoked for event: %d",
            dsdif_ext_event);

  if (dsdif_ext_event <= DS3GDSDIF_EXT_INVALID_EV ||
      dsdif_ext_event > DS3GDSDIF_EXT_MAX_EV)
  {
    DATA_MSG1(MSG_LEGACY_LOW,
              "Trying to invoke cback for an invalid event %d ",
              dsdif_ext_event);
    return;
  }

  if(event_info == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW,
              "Event Info passed is invalid: No cback invoked");
    return;
  }

  event_data_block
    = (ds3gdsdif_ext_event_data_block *)q_check(&ds3gdsdif_ext_event_q);

  /*---------------------------------------------------------------------------
    Loop through the event queue and find the clients that have registered
    for attach init information and then invoke their cback functions
  ---------------------------------------------------------------------------*/
  while(event_data_block != NULL)
  {
    if(event_data_block->event_id == dsdif_ext_event)
    {
      if(event_data_block->func_ptr != NULL)
      {
        event_data_block->func_ptr (event_data_block->event_id,
                                    event_info,
                                    event_data_block->data_ptr);
      }
      else
      {
        DATA_MSG0(MSG_LEGACY_LOW, "Cback function pointer is invalid");
      }
    }
    event_data_block = q_next(&ds3gdsdif_ext_event_q,
                              &(event_data_block->link));
  }
  return;
} /* ds3gdsdif_invoke_ext_cbacks */

/*===========================================================================
FUNCTION      DS3GDSDIF_EXT_COMPARE_FUNCTION

DESCRIPTION   Utility function to compare two items.

DEPENDENCIES  None.

RETURN VALUE  1: if the items are equal
              0: if itemsa are not equal

SIDE EFFECTS  None.
===========================================================================*/
int ds3gdsdif_ext_compare_function
(
  void  *item_ptr,
  void  *target_val
)
{
  ds3gdsdif_ext_event_data_block  *data_block = NULL;
  ds3gdsdif_ext_client_reg_info   *to_compare = NULL;
  boolean                          ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((item_ptr == NULL) ||(target_val == NULL))
  {
    return ret_val;
  }

  data_block = (ds3gdsdif_ext_event_data_block *)item_ptr;
  to_compare = (ds3gdsdif_ext_client_reg_info *)target_val;

  if((data_block->client_id == to_compare->dsdif_ext_client) &&
     (data_block->event_id == to_compare->dsdif_ext_event))
  {
    ret_val = TRUE;
  }

  return ret_val;
}/* ds3gdsdif_ext_compare_function()*/

/*===========================================================================
FUNCTION       ds3gdsdif_get_voice_call_instance

DESCRIPTION    This function returns voice call instance for a given
               subs_id and call_id

DEPENDENCIES   None.

RETURN VALUE   Valid pointer: voice call instance found
               NULL: voice call instance not found

SIDE EFFECTS   None.
===========================================================================*/
ds3gdsdif_voice_call_info_type* ds3gdsdif_get_voice_call_instance
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint8                       call_id
)
{
  ds3gdsdif_voice_call_info_type  *voice_call_info_p = NULL;
  uint8                            index = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id) )
  {
    return NULL;
  }

  for( index=0; index < DS3GDSDIF_VOICE_CALL_COUNT; index++)
  {
    if( (ds3gdsdif_voice_call_info[ds3g_subs_id][index] != NULL) &&
        (ds3gdsdif_voice_call_info[ds3g_subs_id][index]->call_id == call_id) )
    {
      voice_call_info_p = ds3gdsdif_voice_call_info[ds3g_subs_id][index];
      break;
    }
  }

  return voice_call_info_p;
} /* ds3gdsdif_get_voice_call_instance() */

/*===========================================================================
FUNCTION       ds3gdsdif_is_voice_call_active_with_call_id

DESCRIPTION    This function checks if voice call instance for a given
               subs_id and call_id is present or not

DEPENDENCIES   None.

RETURN VALUE   TRUE: voice call instance found
                        FALSE: voice call instance not found

SIDE EFFECTS   None.
===========================================================================*/
boolean ds3gdsdif_is_voice_call_active_with_call_id
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint8   call_id
)
{
  boolean                          ret_val = FALSE;
  ds3gdsdif_voice_call_info_type  *voice_call_info_p = NULL;
  /* ----------------------------------------------------  */
  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id) )
  {
    return ret_val;
  }
  voice_call_info_p = ds3gdsdif_get_voice_call_instance(ds3g_subs_id, call_id);
  if (voice_call_info_p != NULL)
  {
    ret_val = TRUE;
  }
  
  DATA_MSG2(MSG_LEGACY_MED,"ds3gdsdif_is_voice_call_active_with_call_id:%d, ret_val:%d", 
                                                                      call_id, ret_val);
  return ret_val;
}/* ds3gdsdif_is_voice_call_active_with_call_id */

/*===========================================================================
FUNCTION       DS3GDSDIF_CREATE_VOICE_CALL_INSTANCE

DESCRIPTION    This function creates a voice call instance for a given
               subs_id and call_id

DEPENDENCIES   None.

RETURN VALUE   Valid pointer: voice call instance create successful
               NULL: voice call instance create failed

SIDE EFFECTS   None.
===========================================================================*/
ds3gdsdif_voice_call_info_type* ds3gdsdif_create_voice_call_instance
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint8                       call_id
)
{
  uint8                            index = 0;
  ds3gdsdif_voice_call_info_type  *voice_call_inst_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id) )
  {

    return NULL;
  }

  /* check if there is already an entry with same call_id */
  voice_call_inst_p = ds3gdsdif_get_voice_call_instance(ds3g_subs_id, call_id);
  if( voice_call_inst_p != NULL )
  {
    DATA_MSG0(MSG_LEGACY_MED,"ds3gdsdif_create_voice_call_instance: already exists");
    return NULL;
  }

  /* find first free index */
  for( index=0; index < DS3GDSDIF_VOICE_CALL_COUNT; index++)
  {
    if( ds3gdsdif_voice_call_info[ds3g_subs_id][index] == NULL )
    {
      break;
    }
  }
  if( index >= DS3GDSDIF_VOICE_CALL_COUNT )
  {
    return NULL;
  }

  voice_call_inst_p = modem_mem_alloc(sizeof(ds3gdsdif_voice_call_info_type),
                                      MODEM_MEM_CLIENT_DATA);
  if( voice_call_inst_p != NULL )
  {
    voice_call_inst_p->call_id = call_id;
    voice_call_inst_p->radio = SYS_SYS_MODE_NO_SRV;
    voice_call_inst_p->state = DS3GDSDIF_VOICE_CALL_INACTIVE;
    voice_call_inst_p->voice_ho_type = SYS_VOICE_HO_NONE;

    ds3gdsdif_voice_call_info[ds3g_subs_id][index] = voice_call_inst_p;
  }

  return voice_call_inst_p;
} /* ds3gdsdif_create_voice_call_instance() */

/*===========================================================================
FUNCTION       DS3GDSDIF_DELETE_VOICE_CALL_INSTANCE

DESCRIPTION    This function deletes a voice call instance for a given
               subs_id and call_id

DEPENDENCIES   None.

RETURN VALUE   TRUE  voice call instance delete successful
               False voice call instance delete failed

SIDE EFFECTS   None.
===========================================================================*/
boolean ds3gdsdif_delete_voice_call_instance
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint8                       call_id
)
{
  uint8                            index = 0;
  ds3gdsdif_voice_call_info_type  *voice_call_info_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id) )
  {
    return FALSE;
  }
  
  for( index=0; index < DS3GDSDIF_VOICE_CALL_COUNT; index++)
  {
    if( (ds3gdsdif_voice_call_info[ds3g_subs_id][index] != NULL) &&
        (ds3gdsdif_voice_call_info[ds3g_subs_id][index]->call_id == call_id) )
    {
      voice_call_info_p = ds3gdsdif_voice_call_info[ds3g_subs_id][index];
      break;
    }
  }
  
  if(voice_call_info_p != NULL)
  {
    modem_mem_free(voice_call_info_p, MODEM_MEM_CLIENT_DATA);
    ds3gdsdif_voice_call_info[ds3g_subs_id][index] = NULL;
  }

  return TRUE;
} /* ds3gdsdif_delete_voice_call_instance() */

/*===========================================================================
FUNCTION       DS3GDSDIF_GET_EMERG_CALL_INSTANCE

DESCRIPTION    This function returns emergency call instance for a given
               subs_id and call_id

DEPENDENCIES   None.

RETURN VALUE   Valid pointer: emergency call instance found
               NULL: emergency call instance not found

SIDE EFFECTS   None.
===========================================================================*/
ds3gdsdif_voice_call_info_type* ds3gdsdif_get_emerg_call_instance
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint8                       call_id
)
{
  ds3gdsdif_voice_call_info_type  *emerg_call_info_p = NULL;
  uint8                            index = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id) )
  {
    return NULL;
  }

  for( index=0; index < DS3GDSDIF_EMERG_CALL_COUNT; index++)
  {
    if( (ds3gdsdif_emerg_call_info[ds3g_subs_id][index] != NULL) &&
        (ds3gdsdif_emerg_call_info[ds3g_subs_id][index]->call_id == call_id) )
    {
      emerg_call_info_p = ds3gdsdif_emerg_call_info[ds3g_subs_id][index];
      break;
    }
  }

  return emerg_call_info_p;
} /* ds3gdsdif_get_emerg_call_instance() */

/*===========================================================================
FUNCTION       DS3GDSDIF_CREATE_EMERG_CALL_INSTANCE

DESCRIPTION    This function creates a emergency call instance for a given
               subs_id and call_id

DEPENDENCIES   None.

RETURN VALUE   Valid pointer: emergency call instance create successful
               NULL: emergency call instance create failed

SIDE EFFECTS   None.
===========================================================================*/
ds3gdsdif_voice_call_info_type* ds3gdsdif_create_emerg_call_instance
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint8                       call_id
)
{
  uint8                            index = 0;
  ds3gdsdif_voice_call_info_type  *emerg_call_inst_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id) )
  {
    return NULL;
  }

  /* check if there is already an entry with same call_id */
  emerg_call_inst_p = ds3gdsdif_get_emerg_call_instance(ds3g_subs_id, call_id);
  if( emerg_call_inst_p != NULL )
  {
    return NULL;
  }

  /* find first free index */
  for( index=0; index < DS3GDSDIF_EMERG_CALL_COUNT; index++)
  {
    if( ds3gdsdif_emerg_call_info[ds3g_subs_id][index] == NULL )
    {
      break;
    }
  }
  if( index >= DS3GDSDIF_EMERG_CALL_COUNT )
  {
    return NULL;
  }

  emerg_call_inst_p = modem_mem_alloc(sizeof(ds3gdsdif_voice_call_info_type),
                                      MODEM_MEM_CLIENT_DATA);
  if( emerg_call_inst_p != NULL )
  {
    emerg_call_inst_p->call_id = call_id;
    emerg_call_inst_p->radio = SYS_SYS_MODE_NO_SRV;
    emerg_call_inst_p->state = DS3GDSDIF_VOICE_CALL_INACTIVE;
    emerg_call_inst_p->voice_ho_type = SYS_VOICE_HO_NONE;

    ds3gdsdif_emerg_call_info[ds3g_subs_id][index] = emerg_call_inst_p;
  }

  return emerg_call_inst_p;
} /* ds3gdsdif_emerg_voice_call_instance() */

/*===========================================================================
FUNCTION       DS3GDSDIF_DELETE_EMERG_CALL_INSTANCE

DESCRIPTION    This function deletes a emergency call instance for a given
               subs_id and call_id

DEPENDENCIES   None.

RETURN VALUE   TRUE  emergency call instance delete successful
               False emergency call instance delete failed

SIDE EFFECTS   None.
===========================================================================*/
boolean ds3gdsdif_delete_emerg_call_instance
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint8                       call_id
)
{
  uint8                            inst_index = 0;
  ds3gdsdif_voice_call_info_type  *emerg_call_inst_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  emerg_call_inst_p = ds3gdsdif_get_emerg_call_instance(ds3g_subs_id, call_id);
  if( emerg_call_inst_p == NULL )
  {
    return FALSE;
  }
  
  emerg_call_inst_p = ds3gdsdif_emerg_call_info[ds3g_subs_id][inst_index];
  modem_mem_free(emerg_call_inst_p, MODEM_MEM_CLIENT_DATA);
  emerg_call_inst_p = NULL;
  ds3gdsdif_emerg_call_info[ds3g_subs_id][inst_index] = NULL;

  return TRUE;
} /* ds3gdsdif_delete_voice_call_instance() */

/*===========================================================================
FUNCTION       DS3GDSDIF_CSFB_CALL_STATUS_IND_HDLR

DESCRIPTION    This function is used to process LTE_RRC_CSFB_CALL_STATUS_IND
               message.

DEPENDENCIES   None.

RETURN VALUE   TRUE is processing was successful
               False otherwise

SIDE EFFECTS   None.
===========================================================================*/
boolean ds3gdsdif_csfb_call_status_ind_hdlr
(
  msgr_umid_type               msgrtype,
  const msgr_hdr_struct_type  *dsmsg_ptr
)
{
  lte_rrc_csfb_call_status_ind_s  *msg_ptr = NULL;
  ds3gsubsmgr_subs_id_e_type       subs_index;
  ds3gdsdif_voice_call_state_type  voice_call_state;
  ds3gdsdif_voice_call_info_type  *voice_call_inst_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (msgrtype != LTE_RRC_CSFB_CALL_STATUS_IND) || (dsmsg_ptr == NULL) )
  {
    return FALSE;
  }

  msg_ptr = (lte_rrc_csfb_call_status_ind_s*)dsmsg_ptr;
  subs_index =  ds3gsubsmgr_subs_id_cm_to_ds3g(
                  INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id) );
  voice_call_state = ds3gdsdif_get_voice_call_state(subs_index);
  DATA_3GMGR_MSG3( MSG_LEGACY_MED,
                   "ds3gdsdif_csfb_call_status_ind_hdlr subs_id:%d "
                   "voice_call_active:%d csfb_status:%d",
                   subs_index,
                   voice_call_state.is_call_active,
                   msg_ptr->call_status_e );

  switch( msg_ptr->call_status_e )
  {
    case LTE_RRC_CSFB_CALL_STARTED:
      /* Handle CSFB Call started only if there is no voice call already
         (MO CSFB or SRVCC) */
      if( voice_call_state.is_call_active == FALSE )
      {
        voice_call_inst_p
          = ds3gdsdif_create_voice_call_instance(subs_index, DS3G_CALL_ID_MAX);
        if( voice_call_inst_p )
        {
          voice_call_inst_p->state = DS3GDSDIF_VOICE_CALL_MT_CSFB_PAGED;

        /* Start MT_CSFB_ESTAB_HYSTERESIS timer */
         (void)rex_set_timer( &ds3gdsdif_mt_csfb_estab_hyst_timer[subs_index], 
                            DS3GDSDIF_MT_CSFB_ESTAB_HYST_TIMER_VAL);
        }
      } 
      break;

    default:
      break;
  }

  return TRUE;
} /* ds3gdsdif_csfb_call_status_ind_hdlr() */

/*===========================================================================
FUNCTION       DS3GDSDIF_MT_CSFB_ESTAB_HYST_TIMER_CB

DESCRIPTION    This callback function is called when MT_CSFB_ESTAB_HYSTERESIS 
               timer expires.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds3gdsdif_mt_csfb_estab_hyst_timer_cb
(
  unsigned long  subs_idx
)
{
  ds_cmd_type                 *cmd_ptr = NULL;
  ds3gsubsmgr_subs_id_e_type   ds3g_subs_id;
  ds3gsubsmgr_subs_id_e_type  *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_subs_id = (ds3gsubsmgr_subs_id_e_type)subs_idx;

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                  "ds3gdsdif_mt_csfb_estab_hyst_timer_cb subs_id:%d",
                  ds3g_subs_id);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3gsubsmgr_subs_id_e_type));
  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  cmd_ptr->hdr.cmd_id = DS_CMD_DS3GDSDIF_MT_CSFB_HYST_TIMER_EXP;
  data_ptr = (ds3gsubsmgr_subs_id_e_type*)cmd_ptr->cmd_payload_ptr;
  *data_ptr = ds3g_subs_id;
  ds_put_cmd(cmd_ptr);

  return;
} /* ds3gdsdif_mt_csfb_estab_hyst_timer_cb() */

/*===========================================================================

                    EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GDSDIF_INIT

DESCRIPTION   This function regsiters DS as a client with DSD and registers
              for preferred system change event.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_init(void)
{
  ds3gsubsmgr_subs_id_e_type    subs_indx;
  ds3gsubsmgr_client_type       client_id;
#ifdef FEATURE_DATA_WLAN_MAPCON
  int16                       ps_errno;
#endif /*FEATURE_DATA_WLAN_MAPCON*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MSG0(MSG_LEGACY_LOW, "ds3gdsdif_init");

  /*-------------------------------------------------------------------------
    Initialize global variables
  -------------------------------------------------------------------------*/
  rex_init_crit_sect(&ds3gdsdif_crit_sect);
  q_init(&ds3gdsdif_ext_event_q);
  memset (&ds3gdsdif_lte_attach_in_progress, 0,
          sizeof(ds3geventmgr_attach_info_type)*DS3GSUBSMGR_SUBS_ID_COUNT);

  /*-------------------------------------------------------------------------
   Initialize the DS3GDSDIF cache of radio information
  -------------------------------------------------------------------------*/
  for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
      subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT;
      subs_indx++)
  {
    ds3gdsdif_radio_info[subs_indx] = NULL;
    ds3gdsdif_lte_attach_in_progress[subs_indx].in_progress = FALSE;
  }

  /*-------------------------------------------------------------------------
    Register for subsmgr events
  -------------------------------------------------------------------------*/
  if(ds3gsubsmgr_event_reg(DS3GSUBSMGR_DEVICE_MODE_EV,
                           ds3gdsdif_subsmgr_event_cb,
                           NULL,
                           &client_id) == FALSE)
  {
    DATA_ERR_FATAL("Device mode ev registration failed.");
    return;
  }

  if(ds3gsubsmgr_event_reg(DS3GSUBSMGR_DDS_CHANGE_EV,
                           ds3gdsdif_subsmgr_event_cb,
                           NULL,
                           &client_id) == FALSE)
  {
    DATA_ERR_FATAL("DDS change ev registration failed.");
    return;
  }

  if(ds3gsubsmgr_event_reg(DS3GSUBSMGR_MODE_PREF_CHG_EV,
                           ds3gdsdif_subsmgr_event_cb,
                           NULL,
                           &client_id) == FALSE)
  {
    DATA_ERR_FATAL("mode pref change ev registration failed.");
    return;
  }

  if(ds3gsubsmgr_event_reg(DS3GSUBSMGR_SUBS_CAPABILITY_CHG_EV,
                           ds3gdsdif_subsmgr_event_cb,
                           NULL,
                           &client_id) == FALSE)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "Subs capability change ev registration failed.");
    ASSERT(0);
    return;
  }  

  /*-------------------------------------------------------------------------
    Register with DSD module only if the DSD module is present.
  -------------------------------------------------------------------------*/
  if( ds_dsd_ext_event_reg_client(DS_DSD_EXT_CLIENT_DATA,
                                  DS_DSD_EXT_PREF_SYS_CHANGED_EV,
                                  ds3gdsdif_pref_sys_chg_hdlr_cb,
                                  NULL) == FALSE)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Could not register with DS DSD for pref sys "
              "change ind");
  }

  if(ds_dsd_ext_reg_ss_event_process_complete(
       ds3gdsdif_ss_event_complete_hdlr_cb)== FALSE)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,
              "Could not register with DSD for SS event complete");
  }

#ifdef FEATURE_DATA_WLAN_MAPCON
  /*-------------------------------------------------------------------------
  Register WLAN related info with PS framework
  -------------------------------------------------------------------------*/
  if ( 0 != ps_sys_event_reg(PS_SYS_TECH_WLAN,
                             PS_SYS_EVENT_WLAN_STATUS_CHANGE,
                             ds3gdsdif_wlan_srv_chg_cb,
                             NULL,
                             &ps_errno))
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Register for WLAN status change event failed");
  }
#endif /*FEATURE_DATA_WLAN_MAPCON*/


  /*-------------------------------------------------------------------------
    Register MT CSFB establishment hysteresis timer
  -------------------------------------------------------------------------*/
  for(subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
      subs_indx < DS3GSUBSMGR_SUBS_ID_COUNT;
      subs_indx++)
  {
    memset(&ds3gdsdif_mt_csfb_estab_hyst_timer[subs_indx], 0, sizeof(timer_type));
    rex_def_timer_ex( &ds3gdsdif_mt_csfb_estab_hyst_timer[subs_indx],
                       ds3gdsdif_mt_csfb_estab_hyst_timer_cb,
                       (unsigned long)subs_indx );
  }


  /*-------------------------------------------------------------------------
    Register for CSFB indications from NAS
  -------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(LTE_RRC_CSFB_CALL_STATUS_IND,
                   ds3gdsdif_csfb_call_status_ind_hdlr);

  return;
} /* ds3gdsdif_init() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_PREFERRED_SYS

DESCRIPTION   This function returns the current preferred data system.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  Returns current preferred data system.

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_e_type ds3gdsdif_get_preferred_sys
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8                      pref_index = 0xFF;
  sys_sys_mode_e_type        pref_sys = SYS_SYS_MODE_NO_SRV;
  ds3gdsdif_radio_info_type  *radio_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  radio_info_ptr = ds3gdsdif_get_radio_info_ptr(subs_id);

  if(radio_info_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_LOW,
              "radio info instance not present for subs_id: %d", subs_id);
    return SYS_SYS_MODE_NO_SRV;
  }

  pref_index = radio_info_ptr->pref_index;
  pref_sys = radio_info_ptr->radio_info[pref_index].system_mode;

  if(pref_sys >= SYS_SYS_MODE_MAX)
  {
    pref_sys = SYS_SYS_MODE_NO_SRV;
  }
  /*-------------------------------------------------------------------------
    Check if the pref system is remote or local. If preferred system is
    remote, return SYS_SYS_MODE_NO_SRV
  -------------------------------------------------------------------------*/
  if(radio_info_ptr->radio_info[pref_index].remote_flag == TRUE)
  {
    pref_sys = SYS_SYS_MODE_NO_SRV;
  }

  DATA_MSG2(MSG_LEGACY_LOW,
            "ds3gdsdif_get_preferred_sys, subs_id:%d pref sys: %d",
             subs_id,
            pref_sys);

  return pref_sys;
}/* ds3gdsdif_get_preferred_sys() */

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_PREF_RADIO_INFO

DESCRIPTION   This function returns the current preferred radio information.
              The memory for pref_radio_info has to be allocated by the caller.

PARAMETERS    pref_radio_info:  returns preferred radio information

DEPENDENCIES  Memory for the out parameters needs to be provided by the caller.

RETURN VALUE  TRUE:  pref_radio_info has valid value
              FALSE: pref_radio_info has invalid value

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_get_pref_radio_info
(
  ds3gsubsmgr_subs_id_e_type   subs_id,
  ds_dsd_ext_radio_info_type  *pref_radio_info
)
{
  uint8                       pref_index;
  ds3gdsdif_radio_info_type   *radio_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pref_radio_info == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "NULL pref_radio_info ptr passed");
    return FALSE;
  }

  radio_info_ptr = ds3gdsdif_get_radio_info_ptr(subs_id);

  if(radio_info_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_LOW,
              "radio info instance not present for subs_id: %d", subs_id);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    clear the passed structure
  -------------------------------------------------------------------------*/
  memset(pref_radio_info, 0, sizeof(ds_dsd_ext_radio_info_type));

  pref_index = radio_info_ptr->pref_index;
  /*-------------------------------------------------------------------------
    Check if the pref system is remote or local. If preferred system is
    remote, return false and fill radio info with SYS_SRV_STATUS_NO_SRV
  -------------------------------------------------------------------------*/
  if(radio_info_ptr->radio_info[pref_index].remote_flag == TRUE)
  {
    pref_radio_info->system_mode = SYS_SYS_MODE_NO_SRV;
    pref_radio_info->srv_status  = SYS_SRV_STATUS_NO_SRV;
    pref_radio_info->rat_mask    = PS_SYS_RAT_3GPP_NULL_BEARER;
    pref_radio_info->so_mask     = 0;

    DATA_MSG0(MSG_LEGACY_LOW, "Preferred system is remote");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If preferred system is local, return true and fill radio info with info
    from local copy
  -------------------------------------------------------------------------*/
  memscpy(pref_radio_info,
          sizeof(ds_dsd_ext_radio_info_type),
          &(radio_info_ptr->radio_info[pref_index]),
          sizeof(ds_dsd_ext_radio_info_type));

  return TRUE;
}/* ds3gdsdif_get_pref_radio_info() */

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_RADIO_INFO

DESCRIPTION   This function returns the radio information for the queried
              system_mode. The memory for the radio_info has to be allocated
              by the caller.

PARAMETERS    system_mode:    mode for which the radio info is being queried
              radio_info:     returns radio information

DEPENDENCIES  Memory for the out parameters needs to be provided by the caller.

RETURN VALUE  TRUE:  radio_info has valid value
              FALSE: radio_info has invalid value

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_get_radio_info
(
  ds3gsubsmgr_subs_id_e_type   subs_id,
  sys_sys_mode_e_type          system_mode,
  ds_dsd_ext_radio_info_type  *radio_info
)
{
  uint8                        index;
  ds3gdsdif_radio_info_type   *radio_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(radio_info == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "NULL pointer passed");
    return FALSE;
  }

  radio_info_ptr = ds3gdsdif_get_radio_info_ptr(subs_id);

  if(radio_info_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_LOW,
              "radio info instance not present for subs_id: %d", subs_id);
    return FALSE;
  }

  if((system_mode >= SYS_SYS_MODE_MAX) || (system_mode == SYS_SYS_MODE_NONE))
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Invalid system :  %d",system_mode);
    return FALSE;
  }

  if(FALSE == ds3gdsdif_get_system_mode_index(subs_id, system_mode, &index))
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Invalid sys: sys mode: %d",
              system_mode);
    return FALSE;
  }

  if(index < DS3G_RADIO_INDEX_MIN || index >= DS3G_RADIO_INDEX_MAX)
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Invalid radio index: index: %d",
              index);
    return FALSE;
  }

  memscpy(radio_info,
          sizeof(ds_dsd_ext_radio_info_type),
          &(radio_info_ptr->radio_info[index]),
          sizeof(ds_dsd_ext_radio_info_type));

  return TRUE;
}/* ds3gdsdif_get_radio_info() */

/*===========================================================================
FUNCTION      DS3GDSDIF_IS_BSR_IN_PROGRESS

DESCRIPTION   This function returns if BSR is in progress or not.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  True:  BSR in progress
              False: BSR not in progress

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_is_bsr_in_progress
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(ds3gstkproc_get_bsr_state(subs_id) == DS3G_STKPROC_BSR_SET)
  {
    ret_val = TRUE;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "ds3gdsdif_is_bsr_in_progress:%d",ret_val);
  return ret_val;
}/* ds3gdsdif_is_bsr_in_progress() */

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_RESEL_TARGET_RAT

DESCRIPTION   return the reselection target rat variable cached as part of
              CM SS event processing.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_e_type ds3gdsdif_get_resel_target_rat
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds3g_ss_event_info_type     *ss_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get SS event info pointer */
  ss_event_info_ptr = ds3gstkproc_get_ss_event_info_instance(subs_id);
  if(ss_event_info_ptr == NULL)
  {
    return SYS_SYS_MODE_NO_SRV;
  }
  return ss_event_info_ptr->target_rat;
}/* ds3gdsdif_get_resel_target_rat() */

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_PROF_INFO

DESCRIPTION   This function validates the profile when Attach is in progres
              and extracts the APN name and Emergency call supported info
              to pass it to MPPM

PARAMETERS    ds3g_subs_id: Current active Subscription for DS3G
              attach_in_progress: Flag that tells attach in progress status
              attach_prof_info: Struct containing APN and emergency call
                                support info

DEPENDENCIES  None.

RETURN VALUE  TRUE: If profile contents are valid during attach in progress
              FALSE: If input params are invalid or profile is invalid

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_set_attach_apn_info
(
  ds3gsubsmgr_subs_id_e_type          ds3g_subs_id,
  boolean                             attach_in_progress,
  ds3geventmgr_attach_prof_info_type *attach_prof_info
)
{
  boolean      ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return ret_val;
  }

  if(TRUE == attach_in_progress)
  {
    if(NULL != attach_prof_info)
    {
      memset(&ds3gdsdif_lte_attach_in_progress[ds3g_subs_id], 0,
             sizeof(ds3geventmgr_attach_info_type));

      ds3gdsdif_lte_attach_in_progress[ds3g_subs_id].prof_info.
        emergency_calls_supported = attach_prof_info->emergency_calls_supported;

      strlcpy(ds3gdsdif_lte_attach_in_progress[ds3g_subs_id].prof_info.apn_name,
              (const char*)attach_prof_info->apn_name,
              DS_SYS_MAX_APN_LEN);
      ret_val = TRUE;
    }
  }
  else
  {
    ret_val = TRUE;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION      DS3GDSDIF_IS_LTE_ATTACH_IN_PROGRESS

DESCRIPTION   This function returns if LTE attach is in progress or not.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  True:  LTE attach is in progress
              False: LTE attach is not in progress

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_is_lte_attach_in_progress
(
  sys_modem_as_id_e_type  subs_id
)
{
  boolean                     ret_val = FALSE;
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return FALSE;
  }

  ret_val = ds3gdsdif_lte_attach_in_progress[ds3g_subs_id].in_progress;
  DATA_MSG2(MSG_LEGACY_LOW,
            "ds3gdsdif_is_lte_attach_in_progress on subs_id:%d : %d",
            subs_id,
            ret_val);

  return ret_val;
}/* ds3gdsdif_is_lte_attach_in_progress() */

/*===========================================================================
FUNCTION      DS3GDSDIF_SET_LTE_ATTACH_IN_PROGRESS

DESCRIPTION   This function returns if LTE attach is in progress or not.

PARAMETERS    lte_attach_in_progress

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_set_lte_attach_in_progress
(
  sys_modem_as_id_e_type  subs_id,
  boolean                 attach_in_progress,
  ds3geventmgr_attach_prof_info_type *attach_prof_info
)
{
  uint8                        stack_id;
  ds3g_cm_ss_stack_info_type  *stack_info_ptr = NULL;
  boolean                      lte_avail_on_stack = FALSE;
  ds3gsubsmgr_subs_id_e_type   ds3g_subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
  ds3geventmgr_filter_type       filter_info;
  ds3geventmgr_event_info_type  *event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }

  if(FALSE == ds3gdsdif_set_attach_apn_info(ds3g_subs_id,
                                            attach_in_progress,
                                            attach_prof_info))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Invalid Input Params Subs: %d Attach Prof Info: 0x%x",
                    ds3g_subs_id, attach_prof_info);
    return;
  }

  ds3gdsdif_lte_attach_in_progress[ds3g_subs_id].in_progress
    = attach_in_progress;

  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));

  filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
                                        ds3gsubsmgr_subs_id_cm_to_ds(subs_id);
  filter_info.tech =  PS_SYS_TECH_3GPP;
  /*-------------------------------------------------------------------------
    Notify DSD that ATTACH is going on in progress: radio acq and emm attach
  -------------------------------------------------------------------------*/
  if(attach_in_progress)
  {
    /* Check if LTE is available on any stack and PS domain registered */
    for(stack_id = 0; stack_id < CM_NO_STACKS; stack_id++)
    {
      ds3gstkproc_get_stack_info_from_stack_id(ds3g_subs_id, stack_id,
                                               &stack_info_ptr);

      if(stack_info_ptr != NULL)
      {
        if((stack_info_ptr->sys_mode == SYS_SYS_MODE_LTE) &&
           (ds3gstkproc_is_srv_domain_ps_supported(stack_info_ptr->srv_domain)))
        {
          lte_avail_on_stack = TRUE;
          break;
        }
      }
    }

    /*--------------------------------------------------------------------
      Indicate LTE acquired only if CM has not reported LTE acquired
    ----------------------------------------------------------------------*/
    if(lte_avail_on_stack == FALSE)
    {
      /*----------------------------------------------------------------------
        1). Indicate LTE radio acquired with limited service.
            When the indication reaches here, LTE is factually attaching, and
            there is no issue to indicatte LTE radio acq with limited srv.
        2). Because of timing, LTE Attach Ind like here and CM SS event come in
            different orders. It is possible that MH may indicate LTE attach that
            reaches here before before CM SS event for LTE radio acq reaches DSD.
            So we need to go ahead to indicate the fact that LTE is acquired and
            is in attching.
            CM may later in SS event info indicate no srv or limited srv. DSD
            may indicate the srv status change but the pref system remains LTE.
      -----------------------------------------------------------------------*/
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Notify DSD LTE Radio Acq in attaching");

      ds3gdsdif_notify_dsd_acq_lost_event(
         DS_DSD_CLIENT_RADIO_ACQUIRED_EV,
         ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id),
         SYS_SYS_MODE_LTE,
         DS_SYS_RAT_3GPP_NULL_BEARER,
         0,
         SYS_SRV_STATUS_LIMITED,
         SYS_SRV_DOMAIN_CAMPED);
      /*LTE does not care about domain info*/
    }

    /*--------------------------------------------------------------------
      Indicate LTE attaching
    ----------------------------------------------------------------------*/
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Notify DSD LTE Attaching");
    ds3gdsdif_notify_dsd_lte_attach_event(subs_id,
                                          DS_DSD_CLIENT_RADIO_EMMATTACHING_EV,
                                          (ds_dsd_attach_complete_type)0,
                                          PS_SYS_RAT_3GPP_NULL_BEARER);
  }
  event_info_ptr = (ds3geventmgr_event_info_type *)modem_mem_alloc(
                  sizeof(ds3geventmgr_event_info_type), MODEM_MEM_CLIENT_DATA);


  if(event_info_ptr != NULL)
  {
    memset(event_info_ptr, 0, sizeof(ds3geventmgr_event_info_type));

    memscpy(event_info_ptr, sizeof(ds3geventmgr_attach_info_type),
            &ds3gdsdif_lte_attach_in_progress[ds3g_subs_id],
            sizeof(ds3geventmgr_attach_info_type));
    if(ds3geventmgr_set_event_info(DS3GEVENTMGR_LTE_ATTACH_INIT_EV,
                                (void *)event_info_ptr,
                                &filter_info) == FALSE)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Unable to provide LTE attach Init info to reg. clients");
    }
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "Heap Memory allocation failure, LTE attach Init"
                    " notification not done to MPPM");
  }

  if(event_info_ptr != NULL)
  {
    modem_mem_free(event_info_ptr, MODEM_MEM_CLIENT_DATA);
    event_info_ptr = NULL;
  }
  return;
} /* ds3gdsdif_set_lte_attach_in_progress() */

/*===========================================================================
FUNCTION      DS3GDSDIF_SET_LTE_EMERGENCY_CALL_IN_PROG

DESCRIPTION   This function notifies DSD if LTE emergency call is in progress.

DEPENDENCIES  None
 
PARAMETERS    subs_id: subscription id 
              in_prog: TRUE - emergency call starts
                       FALSE - emergency call finishes
RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gdsdif_set_lte_emergency_call_in_prog
(
  sys_modem_as_id_e_type               subs_id,
  boolean                              in_prog
)
{
  ds3gsubsmgr_subs_id_e_type          ds3g_subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }

  /*--------------------------------------------------------------------
  Notify DSD
  ----------------------------------------------------------------------*/
  ds_dsd_ext_notify_lte_emerg_call_in_prog(ds3g_subs_id, in_prog);

  return;
} /* ds3gdsdif_set_lte_emergency_call_in_prog() */

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_VOICE_CALL_STATE

DESCRIPTION   Returns the voice call state for the given subs_id

PARAMETERS    ds3g_subs_id: subscription id.

DEPENDENCIES  None.

RETURN VALUE  ds3gdsdif_voice_call_state_type

SIDE EFFECTS  None.
===========================================================================*/
ds3gdsdif_voice_call_state_type ds3gdsdif_get_voice_call_state
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
)
{
  uint8                              index = 0;
  ds3gdsdif_voice_call_info_type    *voice_call_info_p = NULL;
  ds3gdsdif_voice_call_state_type    voice_call_state;
  boolean                            any_call_connected = FALSE;
  uint8                              conn_idx = 0;
  boolean                            any_call_orig = FALSE;
  uint8                              orig_idx = 0;
  boolean                            any_mt_csfb_paged = FALSE;
  uint8                              mt_csfb_idx = 0;
  boolean                            any_call_inactive_pending = FALSE;
  uint8                              pending_idx = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&voice_call_state, 0, sizeof(ds3gdsdif_voice_call_state_type));

  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id) )
  {
    return voice_call_state;
  }

  for( index=0; index < DS3GDSDIF_VOICE_CALL_COUNT; index++)
  {
    voice_call_info_p = ds3gdsdif_voice_call_info[ds3g_subs_id][index];
    if( voice_call_info_p != NULL )
    {
      switch( voice_call_info_p->state )
      {
        case DS3GDSDIF_VOICE_CALL_ORIG:
          any_call_orig = TRUE;
          orig_idx = index;
          break;

        case DS3GDSDIF_VOICE_CALL_CONNECTED:
          any_call_connected = TRUE;
          conn_idx = index;
          break;

        case DS3GDSDIF_VOICE_CALL_MT_CSFB_PAGED:
          any_mt_csfb_paged = TRUE;
          mt_csfb_idx = index;
          break;
       
        case DS3GDSDIF_VOICE_CALL_INACTIVE_PENDING:
          any_call_inactive_pending = TRUE;
          pending_idx = index;
          break;

        default:
          break;
      }
    }
  }

  if( any_call_connected )
  {
    voice_call_state.is_call_active = TRUE;
    voice_call_state.info_p
      = ds3gdsdif_voice_call_info[ds3g_subs_id][conn_idx];
  }
  else if( any_call_orig )
  {
    voice_call_state.is_call_active = TRUE;
    voice_call_state.info_p
      = ds3gdsdif_voice_call_info[ds3g_subs_id][orig_idx];
  }
  else if( any_mt_csfb_paged )
  {
    voice_call_state.is_call_active = TRUE;
    voice_call_state.info_p
      = ds3gdsdif_voice_call_info[ds3g_subs_id][mt_csfb_idx];
  }
  else if( any_call_inactive_pending )
  {
    voice_call_state.is_call_active = FALSE;
    voice_call_state.info_p
      = ds3gdsdif_voice_call_info[ds3g_subs_id][pending_idx];
  }
  else
  {
    voice_call_state.is_call_active = FALSE;
    voice_call_state.info_p = NULL;
  }

  return voice_call_state;
}/* ds3gdsdif_get_voice_call_state() */

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_EMERG_CALL_STATE

DESCRIPTION   returns the emergency call state for a given subs_id

PARAMETERS    subs_id: subscription id.

DEPENDENCIES  None.

RETURN VALUE  ds3gdsdif_voice_call_state_type

SIDE EFFECTS  None.
===========================================================================*/
ds3gdsdif_voice_call_state_type ds3gdsdif_get_emerg_call_state
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
)
{
  uint8                              index = 0;
  ds3gdsdif_voice_call_info_type    *emerg_call_info_p = NULL;
  ds3gdsdif_voice_call_state_type    emerg_call_state;
  boolean                            any_call_connected = FALSE;
  uint8                              conn_idx = 0;
  boolean                            any_call_orig = FALSE;
  uint8                              orig_idx = 0;
  boolean                            any_call_inactive_pending = FALSE;
  uint8                              pending_idx = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&emerg_call_state, 0, sizeof(ds3gdsdif_voice_call_state_type));

  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id) )
  {
    return emerg_call_state;
  }

  for( index=0; index < DS3GDSDIF_EMERG_CALL_COUNT; index++)
  {
    emerg_call_info_p = ds3gdsdif_emerg_call_info[ds3g_subs_id][index];
    if( emerg_call_info_p != NULL )
    {
      switch( emerg_call_info_p->state )
      {
        case DS3GDSDIF_VOICE_CALL_ORIG:
          any_call_orig = TRUE;
          orig_idx = index;
          break;

        case DS3GDSDIF_VOICE_CALL_CONNECTED:
          any_call_connected = TRUE;
          conn_idx = index;
          break;

        case DS3GDSDIF_VOICE_CALL_INACTIVE_PENDING:
          any_call_inactive_pending = TRUE;
          pending_idx = index;
          break;

        default:
          break;
      }
    }
  }

  if( any_call_connected )
  {
    emerg_call_state.is_call_active = TRUE;
    emerg_call_state.info_p
      = ds3gdsdif_emerg_call_info[ds3g_subs_id][conn_idx];
  }
  else if( any_call_orig )
  {
    emerg_call_state.is_call_active = TRUE;
    emerg_call_state.info_p
      = ds3gdsdif_emerg_call_info[ds3g_subs_id][orig_idx];
  }
  else if( any_call_inactive_pending )
  {
    emerg_call_state.is_call_active = FALSE;
    emerg_call_state.info_p
      = ds3gdsdif_emerg_call_info[ds3g_subs_id][pending_idx];
  }
  else
  {
    emerg_call_state.is_call_active = FALSE;
    emerg_call_state.info_p = NULL;
  }

  return emerg_call_state;
}/* ds3gdsdif_get_emerg_call_state() */

/*===========================================================================
FUNCTION      DS3GDSDIF_EHRPD_IS_PREF_SYSTEM

DESCRIPTION   Checks whether EHRPD is the preferred system.

PARAMETERS    subs_id: cm data sub id.

DEPENDENCIES  None.

RETURN VALUE  TRUE: if EHRPD is the preferred system
              FALSE: otherwise

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gdsdif_ehprd_is_pref_system
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_dsd_ext_radio_info_type pref_radio = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DATA_PKT_CLASSIFIER
  if(ds707_is_pkt_classifier_enabled())
  {
    return ds707_pkt_classifier_is_ehrpd_mode();
  }
#endif/* FEATURE_DATA_PKT_CLASSIFIER */

  if (ds3gdsdif_get_pref_radio_info(ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id),
                                    &pref_radio))
  {
    if( (pref_radio.rat_mask == PS_IFACE_CDMA_EVDO_REVA &&
         pref_radio.so_mask == PS_IFACE_CDMA_EVDO_EMPA_EHRPD) ||
        (pref_radio.rat_mask == PS_IFACE_CDMA_EVDO_REVB &&
         pref_radio.so_mask == PS_IFACE_CDMA_EVDO_MMPA_EHRPD) )
    {
      return TRUE;
    }
  }

  return FALSE;
}/* ds3gdsdif_ehprd_is_pref_system() */

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_EVENT

DESCRIPTION   This function notifies DSD about radio acquired or lost event.

PARAMETERS    event_id: DSD event id
              radio: system mode associated with event
              rat_mask: rat mask
              so_mask: so mask
              srv_status: service status
              srv_domain: service domain

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_acq_lost_event
(
  ds_dsd_client_radio_event_type  event_id,
  sys_modem_as_id_e_type          as_id,
  sys_sys_mode_e_type             radio,
  uint32                          rat_mask,
  uint32                          so_mask,
  sys_srv_status_e_type           srv_status,
  sys_srv_domain_e_type           srv_domain
)
{
  ds_dsd_event_info_type          dsd_event_info;
  dsd_radio_type                  dsd_radio;
  ds3gsubsmgr_subs_id_e_type      ds3g_subs_id;
  ds3gsubsmgr_subs_id_e_type      other_subs_indx;
  ds3g_ss_event_info_type        *ss_event_info_ptr = NULL;
  ds3gdsdif_voice_call_state_type    voice_call_state;
  ds3gdsdif_voice_call_state_type    emerg_call_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MSG7(MSG_LEGACY_MED,
            "ds3gdsdif_notify_dsd_acq_lost_event: event_id:%d subs_id:%d "
            "radio:%d rat_mask:%d so_mask:%d srv_status:%d, srv_dom:%d",
            event_id, as_id, radio, rat_mask,so_mask, srv_status,srv_domain);

  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(as_id);

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }

  /* clear the event info structure */
  memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));

  dsd_event_info.event   = event_id;
  dsd_event_info.subs_id = ds3g_subs_id;
  dsd_event_info.data.attach_radio.rat_mask   = rat_mask;
  dsd_event_info.data.attach_radio.so_mask    = so_mask;
  dsd_event_info.data.attach_radio.srv_status = srv_status;
  dsd_event_info.data.attach_radio.is_remote  = 0;
  dsd_event_info.data.attach_radio.srv_domain = srv_domain;

  dsd_radio = ds_dsd_ext_translate_ss_to_dsd_radio(radio);

  /* Get SS event info pointer */
  ss_event_info_ptr = ds3gstkproc_get_ss_event_info_instance(ds3g_subs_id);
  if(ss_event_info_ptr == NULL)
  {
    return;
  }

  if(dsd_radio != DS_DSD_RADIO_INVALID)
  {
    if((ss_event_info_ptr->hdr_actv_ho_in_prog == TRUE) &&
       ((dsd_radio == DS_DSD_RADIO_LTE) ||
        (dsd_radio == DS_DSD_RADIO_HDR)))
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                      "Not notifying. HDR active HO in progress.");
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,"Notify DSD. radio=%d event_id:%d",
                      radio,event_id);
      ds_dsd_ext_event_notify(dsd_radio, &dsd_event_info);

      /* set dsd notified by SS event */
      ds3gstkproc_set_dsd_notified_by_ss_event(TRUE);

      voice_call_state = ds3gdsdif_get_voice_call_state(ds3g_subs_id);
      emerg_call_state = ds3gdsdif_get_emerg_call_state(ds3g_subs_id);

      /*---------------------------------------------------------------------
        If HDR/LTE lost event and 1x voice call active then send 1x voice
        call connected event to DSD.
        This is to handle cases where HDR/LTE loss is reported after 1x voice
        connected so that DSD stops HDR/LTE OOS hyst timer.
      ---------------------------------------------------------------------*/
      if((event_id == DS_DSD_CLIENT_RADIO_LOST_EV) &&
         ((radio == SYS_SYS_MODE_HDR) || (radio == SYS_SYS_MODE_LTE)) &&
          ((voice_call_state.is_call_active == TRUE) &&
           (voice_call_state.info_p != NULL) &&
           ((voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG) ||
            ((voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED) &&
             (voice_call_state.info_p->radio == SYS_SYS_MODE_CDMA)))) )
      {
        memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));
        if( voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG )
        {
          dsd_event_info.event = DS_DSD_CLIENT_RADIO_VOICE_CALL_ORIG_EV;
        }
        else if( voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED )
        {
          dsd_event_info.event = DS_DSD_CLIENT_RADIO_VOICE_CALL_CONNECTED_EV;
        }
        dsd_event_info.subs_id = ds3g_subs_id;
        ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
      }

      /*---------------------------------------------------------------------
        If HDR/LTE lost event and 1x emergency call active then send 1x
        emergency call connected event to DSD.
        This is to handle cases where HDR/LTE loss is reported after 1x emergency
        connected so that DSD stops HDR/LTE OOS hyst timer.
      ---------------------------------------------------------------------*/
      if((event_id == DS_DSD_CLIENT_RADIO_LOST_EV) &&
         ((radio == SYS_SYS_MODE_HDR) || (radio == SYS_SYS_MODE_LTE)) &&
          ((emerg_call_state.is_call_active == TRUE) &&
           (emerg_call_state.info_p != NULL) &&
           ((emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG) ||
            ((emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED) &&
             (emerg_call_state.info_p->radio == SYS_SYS_MODE_CDMA)))) )
      {
        memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));
        if( emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG )
        {
          dsd_event_info.event = DS_DSD_CLIENT_RADIO_EMERG_CALL_ORIG_EV;
        }
        else if( emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED )
        {
          dsd_event_info.event = DS_DSD_CLIENT_RADIO_EMERG_CALL_STARTED_EV;
        }
        dsd_event_info.subs_id  = ds3g_subs_id;
        ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
      }

      /*---------------------------------------------------------------------
        If UE is in DSDS mode only and If HDR/LTE lost event and call active
        on other sub then send call connected on other sub event to DSD.
        This is to handle cases where HDR/LTE loss is reported after voice
        call connected on other sub so that DSD stops HDR/LTE OOS hyst timer.
      ---------------------------------------------------------------------*/
      if( ds3gsubsmgr_get_device_mode() ==
          SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY )
      {
        other_subs_indx = DS3GDSDIF_GET_OTHER_SUBS_INDX(ds3g_subs_id);
        voice_call_state = ds3gdsdif_get_voice_call_state(other_subs_indx);
        emerg_call_state = ds3gdsdif_get_emerg_call_state(other_subs_indx);

        if((event_id == DS_DSD_CLIENT_RADIO_LOST_EV) &&
           ((radio == SYS_SYS_MODE_HDR) || (radio == SYS_SYS_MODE_LTE)) &&
           ((voice_call_state.is_call_active == TRUE) ||
            (emerg_call_state.is_call_active == TRUE)))
        {
          memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));
          dsd_event_info.event = DS_DSD_CLIENT_RADIO_VOICE_CALL_UP_ON_OTHER_SUB_EV;
          dsd_event_info.subs_id  = ds3g_subs_id;
          ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
        }
      }
    }
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid DSD radio. radio=%d", radio);
  }
  return;
}/* ds3gdsdif_notify_dsd_acq_lost_event() */

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_LOST_EVENT

DESCRIPTION   This function fills the dsd event info and notifies DSD.

PARAMETERS    sys_mode: system mode associated with event

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_lost_event
(
  sys_sys_mode_e_type     sys_mode,
  sys_modem_as_id_e_type  as_id
)
{
  uint32                      rat_mask = DS_SYS_RAT_3GPP_NULL_BEARER;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG1(MSG_LEGACY_LOW,
            "ds3gdsdif_notify_dsd_lost_event: sys_mode:%d",sys_mode);

  if((sys_mode == SYS_SYS_MODE_CDMA) || (sys_mode == SYS_SYS_MODE_HDR))
  {
    rat_mask = DS_SYS_RAT_3GPP2_NULL_BEARER;
  }

  ds3gdsdif_notify_dsd_acq_lost_event(DS_DSD_CLIENT_RADIO_LOST_EV,
                                      as_id,
                                      sys_mode,
                                      rat_mask,
                                      0,
                                      SYS_SRV_STATUS_NO_SRV,
                                      SYS_SRV_DOMAIN_NO_SRV);
  return;
}/* ds3gdsdif_notify_dsd_lost_event() */

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_HDR_SESSION_EVENT

DESCRIPTION   This function notifies DSD about following HDR attach events
               - Session Open
               - Session Close

PARAMETERS    dsd_event: dsd event
              rat_mask: rat mask
              so_mask: so mask

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_hdr_session_event
(
  sys_modem_as_id_e_type          as_id,
  ds_dsd_client_radio_event_type  dsd_event,
  uint32                          rat_mask,
  uint32                          so_mask
)
{
  ds_dsd_event_info_type          dsd_event_info;
  ds3gdsdif_voice_call_state_type  voice_call_state;
  ds3gdsdif_voice_call_state_type  emerg_call_state;
  ds3gsubsmgr_subs_id_e_type      ds3g_subs_id;
  ds3gsubsmgr_subs_id_e_type      other_subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(as_id);

  memset(&dsd_event_info, 0, sizeof(ds_dsd_event_info_type));
  dsd_event_info.event   = dsd_event;
  dsd_event_info.subs_id = ds3g_subs_id;
  dsd_event_info.data.hdr_session.rat_mask = rat_mask;
  dsd_event_info.data.hdr_session.so_mask = so_mask;
  ds_dsd_ext_event_notify(DS_DSD_RADIO_HDR, &dsd_event_info);

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }

  voice_call_state = ds3gdsdif_get_voice_call_state(ds3g_subs_id);
  emerg_call_state = ds3gdsdif_get_emerg_call_state(ds3g_subs_id);

  /*-------------------------------------------------------------------------
    If HDR session close event and 1x voice call active then send 1x voice
    call connected event to DSD.
    This is to handle cases where HDR session close is reported after 1x
    voice connected so that DSD stops HDR detach hyst timer.
  -------------------------------------------------------------------------*/
  if( (dsd_event == DS_DSD_CLIENT_RADIO_HDRSESSIONCLOSED_EV) &&
      ((voice_call_state.is_call_active == TRUE) &&
       (voice_call_state.info_p != NULL) &&
       ((voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG) ||
        ((voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED) &&
         (voice_call_state.info_p->radio == SYS_SYS_MODE_CDMA)))) )
  {
    memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));
    if( voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG )
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_VOICE_CALL_ORIG_EV;
    }
    else if( voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED )
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_VOICE_CALL_CONNECTED_EV;
    }
    dsd_event_info.subs_id = ds3g_subs_id;
    ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
  }

  /*-------------------------------------------------------------------------
    If HDR session close event and 1x emergency call active then send 1x
    emergency call connected event to DSD.
    This is to handle cases where HDR session close is reported after 1x
    emergency call connected so that DSD stops HDR detach hyst timer.
  -------------------------------------------------------------------------*/
  if( (dsd_event == DS_DSD_CLIENT_RADIO_HDRSESSIONCLOSED_EV) &&
      ((emerg_call_state.is_call_active == TRUE) &&
       (emerg_call_state.info_p != NULL) &&
       ((emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG) ||
        ((emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED) &&
         (emerg_call_state.info_p->radio == SYS_SYS_MODE_CDMA)))) )
  {
    memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));
    if( emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG )
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_EMERG_CALL_ORIG_EV;
    }
    else if( emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED )
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_EMERG_CALL_STARTED_EV;
    }
    dsd_event_info.subs_id = ds3g_subs_id;
    ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
  }

  /*---------------------------------------------------------------------
    If UE is in DSDS mode only and if HDR session event and call active
    on other sub then send call connected on other sub event to DSD.
    This is to handle cases where HDR session closeloss is reported after
    voice call connected on other sub so that DSD stops HDR detach hyst timer.
  ---------------------------------------------------------------------*/
  if( ds3gsubsmgr_get_device_mode() ==
      SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY )
  {
    other_subs_indx = DS3GDSDIF_GET_OTHER_SUBS_INDX(ds3g_subs_id);
    voice_call_state = ds3gdsdif_get_voice_call_state(other_subs_indx);
    emerg_call_state = ds3gdsdif_get_emerg_call_state(other_subs_indx);

    if((dsd_event == DS_DSD_CLIENT_RADIO_HDRSESSIONCLOSED_EV) &&
       ((voice_call_state.is_call_active == TRUE) ||
        (emerg_call_state.is_call_active == TRUE)))
    {
      memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_VOICE_CALL_UP_ON_OTHER_SUB_EV;
      dsd_event_info.subs_id  = ds3g_subs_id;
      ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
    }
  }

  return;
}/* ds3gdsdif_notify_dsd_hdr_session_event() */

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_LTE_ATTACH_EVENT

DESCRIPTION   This function notifies DSD about following LTE attach events
               - Attach Init
               - Attach Complete
               - Detached
               - EPS Context Tx

PARAMETERS    as_id: subs id
              dsd_event: dsd event
              attach_type: IRAT or Normal Attach
              rat_mask: rat mask

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_lte_attach_event
(
  sys_modem_as_id_e_type          as_id,
  ds_dsd_client_radio_event_type  dsd_event,
  ds_dsd_attach_complete_type     attach_type,
  uint32                          rat_mask
)
{
  ds_dsd_event_info_type          dsd_event_info;
  ds3gdsdif_voice_call_state_type  voice_call_state;
  ds3gdsdif_voice_call_state_type  emerg_call_state;
  ds3gsubsmgr_subs_id_e_type      ds3g_subs_id;
  ds3gsubsmgr_subs_id_e_type      other_subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(as_id);

  memset(&dsd_event_info, 0, sizeof(ds_dsd_event_info_type));
  dsd_event_info.event   = dsd_event;
  dsd_event_info.subs_id = ds3g_subs_id;
  if(dsd_event == DS_DSD_CLIENT_RADIO_EMMATTACHCOMPLETED_EV)
  {
    dsd_event_info.data.emm_attach.attach_type = attach_type;
  }
  dsd_event_info.data.emm_attach.rat_mask = rat_mask;
  ds_dsd_ext_event_notify(DS_DSD_RADIO_LTE , &dsd_event_info);

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return;
  }

  voice_call_state = ds3gdsdif_get_voice_call_state(ds3g_subs_id);
  emerg_call_state = ds3gdsdif_get_emerg_call_state(ds3g_subs_id);

/*-------------------------------------------------------------------------
    If LTE detach event and 1x voice call active then send 1x voice
    call connected event to DSD.
    This is to handle cases where LTE detach is reported after 1x
    voice connected so that DSD stops LTE detach hyst timer.
  -------------------------------------------------------------------------*/
  if( (dsd_event == DS_DSD_CLIENT_RADIO_EMMDETACHCOMPLETED_EV) &&
      ((voice_call_state.is_call_active == TRUE) &&
       (voice_call_state.info_p != NULL) &&
       ((voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG) ||
        ((voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED) &&
         (voice_call_state.info_p->radio == SYS_SYS_MODE_CDMA)))) )
  {
    memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));
    if( voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG )
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_VOICE_CALL_ORIG_EV;
    }
    else if( voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED )
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_VOICE_CALL_CONNECTED_EV;
    }
    dsd_event_info.subs_id = ds3g_subs_id;
    ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
  }

  /*-------------------------------------------------------------------------
    If LTE detach event and 1x emergency call active then send 1x
    emergency call connected event to DSD.
    This is to handle cases where LTE detach is reported after 1x
    emergency call connected so that DSD stops LTE detach hyst timer.
  -------------------------------------------------------------------------*/
  if( (dsd_event == DS_DSD_CLIENT_RADIO_EMMDETACHCOMPLETED_EV) &&
      ((emerg_call_state.is_call_active == TRUE) &&
       (emerg_call_state.info_p != NULL) &&
       ((emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG) ||
        ((emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED) &&
         (emerg_call_state.info_p->radio == SYS_SYS_MODE_CDMA)))) )
  {
    memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));
    if( emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_ORIG )
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_EMERG_CALL_ORIG_EV;
    }
    else if( emerg_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_CONNECTED )
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_EMERG_CALL_STARTED_EV;
    }
    dsd_event_info.subs_id = ds3g_subs_id;
    ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
  }

  /*---------------------------------------------------------------------
    If UE is in DSDS mode only and If HDR/LTE lost event and call active
    on other sub then send call connected on other sub event to DSD.
    This is to handle cases where LTE detachloss is reported after voice
    call connected on other sub so that DSD stops LTE detach hyst timer.
  ---------------------------------------------------------------------*/
  if( ds3gsubsmgr_get_device_mode() ==
      SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY )
  {
    other_subs_indx = DS3GDSDIF_GET_OTHER_SUBS_INDX(ds3g_subs_id);
    voice_call_state = ds3gdsdif_get_voice_call_state(other_subs_indx);
    emerg_call_state = ds3gdsdif_get_emerg_call_state(other_subs_indx);

    if((dsd_event == DS_DSD_CLIENT_RADIO_EMMDETACHCOMPLETED_EV) &&
       ((voice_call_state.is_call_active == TRUE) ||
        (emerg_call_state.is_call_active == TRUE)))
    {
      memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_VOICE_CALL_UP_ON_OTHER_SUB_EV;
      dsd_event_info.subs_id  = ds3g_subs_id;
      ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
    }
  }

  return;
}/* ds3gdsdif_notify_dsd_lte_attach_event() */

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_GWT_PARAM_CHG_EVENT

DESCRIPTION   This function notifies DSD about G/W/TD param change event.

PARAMETERS    sys_mode: system mode associated with event

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_gwt_param_chg_event
(
  sys_sys_mode_e_type      sys_mode,
  sys_modem_as_id_e_type   as_id,
  cm_cell_srv_ind_s_type  *cell_srv_ind_ptr
)
{
  ds_dsd_event_info_type  dsd_event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cell_srv_ind_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Null pointer passed");
    return;
  }

  memset(&dsd_event_info,0,sizeof(dsd_event_info));

  dsd_event_info.event = DS_DSD_CLIENT_RADIO_NETWORK_PARM_CHANGED_EV;
  dsd_event_info.subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(as_id);

  memscpy(&(dsd_event_info.data.cell_srv_info.cell_srv_ind),
          sizeof(cm_cell_srv_ind_s_type),
          cell_srv_ind_ptr,
          sizeof(cm_cell_srv_ind_s_type));

  ds_dsd_ext_event_notify(ds_dsd_ext_translate_ss_to_dsd_radio(sys_mode),
                          &dsd_event_info);

  /* set dsd notified by SS event */
  ds3gstkproc_set_dsd_notified_by_ss_event(TRUE);

  return;
}/* ds3gdsdif_notify_dsd_gwt_param_chg_event() */

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_LTE_DIV_DUPLEX_CHG_EVENT

DESCRIPTION   This function notifies DSD about LTE div duplex change event.

PARAMETERS    as_id: subs_id on which to notify
              div_duplex: LTE div duplex

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_lte_div_duplex_chg_event
(
  sys_modem_as_id_e_type  as_id,
  sys_div_duplex_e_type   div_duplex
)
{
  ds_dsd_event_info_type  dsd_event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&dsd_event_info,0,sizeof(dsd_event_info));

  dsd_event_info.event = DS_DSD_CLIENT_RADIO_LTE_EX_SO_CHANGED_EV;
  dsd_event_info.subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(as_id);
  dsd_event_info.data.lte_ex_so_info.lte_so_ex_bit_msk = 
                                              DS_DSD_LTE_EX_SO_DIV_DUPLEX_MSK;
  dsd_event_info.data.lte_ex_so_info.div_duplex = div_duplex;

  ds_dsd_ext_event_notify(DS_DSD_RADIO_LTE, &dsd_event_info);

  /* set dsd notified by SS event */
  ds3gstkproc_set_dsd_notified_by_ss_event(TRUE);

  return;
}/* ds3gdsdif_notify_dsd_lte_div_duplex_chg_event() */

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_LTE_CA_CHG_EVENT

DESCRIPTION   This function notifies DSD about LTE CA so chg events
  
PARAMETERS    as_id: Subs_id
              ca_dl_available: ca dl is available
              ca_ul_available: ca ul is available

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_lte_ca_chg_event
(
  sys_modem_as_id_e_type  as_id,
  boolean                 ca_dl_available,
  boolean                 ca_ul_available
)
{
  ds_dsd_event_info_type  dsd_event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&dsd_event_info,0,sizeof(dsd_event_info));

  dsd_event_info.event = DS_DSD_CLIENT_RADIO_LTE_EX_SO_CHANGED_EV;
  dsd_event_info.subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(as_id);
  dsd_event_info.data.lte_ex_so_info.lte_so_ex_bit_msk =DS_DSD_LTE_EX_SO_CA_MSK;
  dsd_event_info.data.lte_ex_so_info.ca_dl_available = ca_dl_available;
  dsd_event_info.data.lte_ex_so_info.ca_ul_available = ca_ul_available;

  ds_dsd_ext_event_notify(DS_DSD_RADIO_LTE, &dsd_event_info);
} /* ds3gdsdif_notify_dsd_lte_ca_chg_event */

/*===========================================================================
FUNCTION      DS3GDSDIF_NOTIFY_DSD_ROAM_STATUS_CHG_EVENT  

DESCRIPTION   This function notifies DSD about roaming status change.
  
PARAMETERS    sys_mode:       system mode
              as_id:          Subs_id
              roaming_status: roaming status

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_notify_dsd_roam_status_chg_event
(
  sys_sys_mode_e_type     sys_mode,
  sys_modem_as_id_e_type  as_id,
  boolean                 roaming_status
)
{
  ds_dsd_event_info_type  dsd_event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&dsd_event_info,0,sizeof(dsd_event_info));

  dsd_event_info.event = DS_DSD_CLIENT_RADIO_ROAM_STATUS_CHANGED_EV;
  dsd_event_info.subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(as_id);
  dsd_event_info.data.roaming_status_info.roaming_status = roaming_status;

  ds_dsd_ext_event_notify(ds_dsd_ext_translate_ss_to_dsd_radio(sys_mode),
                          &dsd_event_info);

  return;
}/*ds3gdsdif_notify_dsd_roam_status_chg_event*/

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_PS_SUSPEND

DESCRIPTION   This function process the PS suspend

PARAMETERS    subs_id: ds3g sub id.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_process_ps_suspend
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  sys_sys_mode_e_type          dsd_pref_sys = SYS_SYS_MODE_NO_SRV;
  uint8                        dsd_pref_cm_stack = CM_NO_STACKS;
  ds3g_cm_ss_stack_info_type  *dsd_pref_cm_stack_info_ptr = NULL;
  ds3g_ss_event_info_type     *cached_ss_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    return;
  }

  /* Get cached SS event info for subs_id */
  cached_ss_event_info_ptr = ds3gstkproc_get_ss_event_info_instance(subs_id);
  if(cached_ss_event_info_ptr == NULL)
  {
    return;
  }

  /* Get preferred system stack */
  dsd_pref_sys = ds3gdsdif_get_preferred_sys(subs_id);
  dsd_pref_cm_stack = ds3gstkproc_get_stack_id_from_radio(
                        ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id),
                        dsd_pref_sys);
  if(dsd_pref_cm_stack > CM_NO_STACKS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "pref_cm_stack invalid :%d",
                    dsd_pref_cm_stack);
    return;
  }

  /* Get preferred system stack information */
  ds3gstkproc_get_stack_info_from_stack_id(subs_id,
                                           dsd_pref_cm_stack,
                                           &dsd_pref_cm_stack_info_ptr);
  if(dsd_pref_cm_stack_info_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "NULL stack info ptr");
    return;
  }

  /* Notify PS data suspend or resume to Mode handlers */
  if(cached_ss_event_info_ptr->ps_data_suspend == TRUE)
  {
    ds3g_process_data_suspend_ex(ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id));
  }
  else
  {
    /* Send resume only if preferred stack has PS domain registered */
    if(ds3gstkproc_is_srv_domain_ps_supported(
         dsd_pref_cm_stack_info_ptr->srv_domain) == TRUE)
    {
      ds3g_process_data_resume_ex(ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id));
    }
  }
  return;
}/* ds3gdsdif_process_ps_suspend() */

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_DSD_PREF_SYS_CHG_CMD

DESCRIPTION   This function processes the DS_DSD_PREF_SYS_CHG_CMD.  This
              command is posted to DS3G when preferred system changes.
              This function will call into the MH's sys change handlers
              and then call into MH's pref sys change handlers.

PARAMETERS    cmd_ptr: command pointer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_process_dsd_pref_sys_chg_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  uint8                          old_pref_index = 0;
  sys_sys_mode_e_type            old_pref_sys = SYS_SYS_MODE_NO_SRV;
  ds3gsubsmgr_subs_id_e_type     subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
  uint8                          new_pref_index = 0;
  sys_sys_mode_e_type            new_pref_sys = SYS_SYS_MODE_NO_SRV;
  sys_srv_status_e_type          new_pref_srv_status = SYS_SRV_STATUS_NO_SRV;
  uint32                         new_pref_changed_fields = 0;
  ds3g_radio_index_type          radio_indx;
  ds_dsd_ext_event_info_type    *new_event_info_p = NULL;
  ds_cmd_type                   *ds_cmd_ptr = NULL;
  ds3g_sys_chg_ss_info_type  *sys_chg_ss_info_ptr = NULL;
  ds3gdsdif_radio_info_type     *radio_info_ptr;
  sys_modem_as_id_e_type         cm_subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if cmd_ptr is valid
  -------------------------------------------------------------------------*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Typecast cmd payload and check if it is valid
  -------------------------------------------------------------------------*/
  new_event_info_p = (ds_dsd_ext_event_info_type*)cmd_ptr->cmd_payload_ptr;

  subs_id = ds3gsubsmgr_subs_id_ds_to_ds3g(new_event_info_p->subs_id);
  new_pref_index   = new_event_info_p->pref_index;

  radio_info_ptr = ds3gdsdif_get_radio_info_ptr(subs_id);

  /* create radio info instance if not present */
  if(radio_info_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_LOW,
              "radio info instance not present for subs_id:%d. return.",
              subs_id);
    ASSERT(0);
  }
  else
  {
    old_pref_index = radio_info_ptr->pref_index;
  }

  /*-------------------------------------------------------------------------
    Copy old_pref_sys and new_pref_sys for sys change handlers
  -------------------------------------------------------------------------*/
  old_pref_sys = radio_info_ptr->radio_info[old_pref_index].system_mode;
  new_pref_sys
    = new_event_info_p->dsd_radio_info[new_pref_index].system_mode;
  new_pref_srv_status
    = new_event_info_p->dsd_radio_info[new_pref_index].srv_status;
  new_pref_changed_fields
    = new_event_info_p->dsd_radio_info[new_pref_index].changed_fields;

  DATA_MSG8(MSG_LEGACY_HIGH,
            "ds3gdsdif_process_dsd_pref_sys_chg_cmd: subs_id:%d "
            "old_pref_sys: %d new_pref_sys:%d pref_changed_fields: 0x%x"
            "new_pref_srv_status:%d new_pref_srv_domain:%d "
            "new_pref_rat_mask:%d new_pref_so_mask:%d",
            subs_id, old_pref_sys, new_pref_sys,
            new_event_info_p->dsd_radio_info[new_pref_index].changed_fields,
            new_event_info_p->dsd_radio_info[new_pref_index].srv_status,
            new_event_info_p->dsd_radio_info[new_pref_index].srv_domain,
            new_event_info_p->dsd_radio_info[new_pref_index].rat_mask,
            new_event_info_p->dsd_radio_info[new_pref_index].so_mask);

  /*-------------------------------------------------------------------------
   Copy the new radio info into local cache
  -------------------------------------------------------------------------*/
  rex_enter_crit_sect(&ds3gdsdif_crit_sect);
  radio_info_ptr->subs_id = ds3gsubsmgr_subs_id_ds_to_ds3g(new_event_info_p->
                                                                     subs_id);
  radio_info_ptr->pref_index = new_event_info_p->pref_index;
  radio_info_ptr->changed_radio_mask = new_event_info_p->changed_radio_mask;

  for(radio_indx = DS3G_RADIO_INDEX_MIN;
      radio_indx < DS3G_RADIO_INDEX_MAX;
      radio_indx++)
  {
    memscpy(&(radio_info_ptr->radio_info[radio_indx]),
            sizeof(ds_dsd_ext_radio_info_type),
            &(new_event_info_p->dsd_radio_info[radio_indx]),
            sizeof(ds_dsd_ext_radio_info_type));
  }
  rex_leave_crit_sect(&ds3gdsdif_crit_sect);

  /*-------------------------------------------------------------------------
    !!!!!!! Do Not changed the notification order !!!!!!!
    1. Sys change handlers
    2. ATCOP
    3. Cell srv indication
    4. PS data suspend resume handlers
  -------------------------------------------------------------------------*/
  cm_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id);

  if((new_pref_sys != old_pref_sys) &&
     (new_event_info_p->mode_pref_notify == TRUE))
  {
    ds3g_dispatch_mode_pref_change_handler(old_pref_sys,new_pref_sys,cm_subs_id);
  }

  /*-------------------------------------------------------------------------
    1. Call MH's sys change handlers with new pref mode.
  -------------------------------------------------------------------------*/
  if((ds3gdsdif_lte_attach_in_progress[subs_id].in_progress == TRUE) &&
     (new_pref_sys != SYS_SYS_MODE_LTE))
  {
    /*-----------------------------------------------------------------------
      In case of LTE attach in progress, DSDIF should not call sys change
      handler for any non-LTE radio
    -----------------------------------------------------------------------*/
    DATA_MSG3(MSG_LEGACY_HIGH,
              "LTE attach in progress:%d on subs_id:%d, "
              "New pref_sys:%d != LTE. Not calling sys_chg_hdlr",
              subs_id,
              ds3gdsdif_lte_attach_in_progress[subs_id].in_progress,
              new_pref_sys);
  }
  else
  {
    /*-------------------------------------------------------------------------
      If new preferred system is CDMA or HDR, check the srv_status.
        If srv_status = No service, call sys_chg_hdlr with new_pref_sys = OOS
          This is required by QoS module since Qos also listens to sys_chg_hdlr
        Else, call sys_chg_hdlr with same new_pref_sys passed by DSD
      Else, call sys_chg_hdlr with same new_pref_sys passed by DSD
    -------------------------------------------------------------------------*/
    if((new_pref_sys == SYS_SYS_MODE_CDMA) ||
       (new_pref_sys == SYS_SYS_MODE_HDR))
    {
      if(new_pref_srv_status == SYS_SRV_STATUS_NO_SRV)
      {
        ds3g_dispatch_sys_change_handler_ex(
           old_pref_sys, SYS_SYS_MODE_NO_SRV,
           cm_subs_id);
      }
      else
      {
        ds3g_dispatch_sys_change_handler_ex(
           old_pref_sys, new_pref_sys,
           cm_subs_id);
      }
    }
    else
    {
      ds3g_dispatch_sys_change_handler_ex(
         old_pref_sys, new_pref_sys, cm_subs_id);
    }
  }

  /*-------------------------------------------------------------------------
    2. Call ATCOP
  -------------------------------------------------------------------------*/
  ds3g_dsat_update_service_mode(cm_subs_id);

  /*-------------------------------------------------------------------------
    3. Cell service indication posting to MH
  -------------------------------------------------------------------------*/
  ds_cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_sys_chg_ss_info_type));
  if( (ds_cmd_ptr == NULL) || (ds_cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  else
  {
    /*---------------------------------------------------------------------
      Fill in the command parameters and send the command to indicate that
      the system mode has changed and/or the suspend state has changed.
    ---------------------------------------------------------------------*/
    ds_cmd_ptr->hdr.cmd_id =  DS_CMD_3GPP_SYS_CHG_SS_INFO;
    sys_chg_ss_info_ptr
      = (ds3g_sys_chg_ss_info_type*)ds_cmd_ptr->cmd_payload_ptr;

    sys_chg_ss_info_ptr->subs_id = cm_subs_id;
    sys_chg_ss_info_ptr->sys_mode =
      new_event_info_p->dsd_radio_info[new_event_info_p->pref_index].system_mode;
    sys_chg_ss_info_ptr->cell_srv_ind =
      new_event_info_p->dsd_radio_info[new_event_info_p->pref_index].cell_srv_ind;
#ifdef FEATURE_DATA_LTE
    sys_chg_ss_info_ptr->div_duplex =
      ds3g_get_div_duplex_from_so_mask
      (
         new_event_info_p->dsd_radio_info[new_event_info_p->pref_index].so_mask
      );
#endif /* FEATURE_DATA_LTE */

    ds_put_cmd(ds_cmd_ptr);
  }

  /*-------------------------------------------------------------------------
    4. Notify IRAT on any subscription to DS3G Flow mgr
  -------------------------------------------------------------------------*/
  if( (new_pref_sys != old_pref_sys) &&
      (new_pref_srv_status == SYS_SRV_STATUS_SRV) )
  {
    ds3gflowmgr_notify_irat_ind();
  }

  return;
}/* ds3gdsdif_process_dsd_pref_sys_chg_cmd() */

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_DSD_SS_EVENT_PROC_DONE_CMD

DESCRIPTION   This function processes the DS_DSD_SS_EVENT_PROC_DONE_CMD. This
              command is posted to DS3GDSDIF when DSD calls SS event processes
              complete.

PARAMETERS    cmd_ptr: command pointer

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_process_dsd_ss_event_proc_done_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3gdsdif_ss_event_done_cmd_type  *ss_event_done_cmd_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
    ASSERT(0);
    return;
  }

  ss_event_done_cmd_info_ptr
    = (ds3gdsdif_ss_event_done_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_MSG1(MSG_LEGACY_LOW,
            "ds3gdsdif_process_dsd_ss_event_proc_done_cmd subs_id:%d",
            ss_event_done_cmd_info_ptr->subs_id);

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(ss_event_done_cmd_info_ptr->subs_id))
  {
    return;
  }

  if(cmd_ptr->hdr.cmd_id != DS_CMD_DS3GDSDIF_SS_EVENT_PROC_DONE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Command %d: not for DS3GDSDIF return",
                    cmd_ptr->hdr.cmd_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Handle BSR flag if DSD notified by SS event.
    Else, it was already handled during SS event stack processing.
  -------------------------------------------------------------------------*/
  /* Check if any notification is pending */
  if(DS3G_STKPROC_BSR_RESET_PENDING
     == ds3gstkproc_get_bsr_state(ss_event_done_cmd_info_ptr->subs_id))
  {
    ds3gstkproc_set_bsr_state(ss_event_done_cmd_info_ptr->subs_id,
                              DS3G_STKPROC_BSR_RESET);

    /* notify mode handlers */
    ds3g_dispatch_bsr_reset_handler(
       ds3gsubsmgr_subs_id_ds3g_to_cm(ss_event_done_cmd_info_ptr->subs_id));
  }

  /*-------------------------------------------------------------------------
    Handle PS Suspend flag if DSD notified by SS event.
    Else, it was already handled during SS event stack processing.
  -------------------------------------------------------------------------*/
  ds3gdsdif_process_ps_suspend(ss_event_done_cmd_info_ptr->subs_id);


  /* No need to reset the dsd notified by SS event flag.
     It will be reset during the next SS event processing. */

  return;
}/* ds3gdsdif_process_dsd_ss_event_proc_done_cmd() */

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_VOICE_CALL_EVENT_CMD

DESCRIPTION   This function process the CM voice call event command

PARAMETERS    cmd_ptr: command pointer.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void  ds3gdsdif_process_voice_call_event_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3gdsdif_call_info_cmd_type    *call_info_cmd_ptr = NULL;
  ds3gsubsmgr_subs_id_e_type       subs_indx;
  ds_dsd_event_info_type           dsd_event_info;
  boolean                          notify_dsd = FALSE;
  ds3gdsdif_voice_call_info_type  *call_info_ptr = NULL;
  boolean                          emergency_call = FALSE;
  ds3gdsdif_voice_call_state_type  voice_call_state;
  sys_modem_as_id_e_type           other_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmd_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  if((cmd_ptr->hdr.cmd_id != DS_CMD_DS3GDSDIF_VOICE_CALL_EVENT) ||
     (cmd_ptr->cmd_payload_ptr == NULL))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Unsupported cmd id %d or NULL user data ptr passed",
                    cmd_ptr->hdr.cmd_id);
    return;
  }

  /*-------------------------------------------------------------------------
   Get call info and subs_index from command pointer
  -------------------------------------------------------------------------*/
  call_info_cmd_ptr = (ds3gdsdif_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(call_info_cmd_ptr->subs_id);
  memset(&dsd_event_info, 0, sizeof(ds_dsd_event_info_type));

  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx) )
  {
    return;
  }

  DATA_MSG6(MSG_LEGACY_HIGH,
            "ds3gdsdif_process_voice_call_event_cmd: "
            "event:%d call_type:%d call_id:%d sys_mode:%d "
            "voice_ho_type:%d subs_id:%d ",
            call_info_cmd_ptr->call_event,
            call_info_cmd_ptr->call_type,
            call_info_cmd_ptr->call_id,
            call_info_cmd_ptr->call_sys_mode,
            call_info_cmd_ptr->call_voice_ho_type,
            call_info_cmd_ptr->subs_id);

  /*-------------------------------------------------------------------------
    Get call info pointer based on call type for the same subscription
  -------------------------------------------------------------------------*/
  switch(call_info_cmd_ptr->call_type)
  {
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_CS_DATA:
    case CM_CALL_TYPE_VT:
      if( (call_info_cmd_ptr->call_event == CM_CALL_EVENT_ORIG) ||
          (call_info_cmd_ptr->call_event == CM_CALL_EVENT_INCOM) )
      {
        voice_call_state = ds3gdsdif_get_voice_call_state(subs_indx);
        if( (voice_call_state.is_call_active == TRUE) &&
            (voice_call_state.info_p != NULL) &&
            (voice_call_state.info_p->state
               == DS3GDSDIF_VOICE_CALL_MT_CSFB_PAGED) )
        {
          call_info_ptr = voice_call_state.info_p;
        }
        else
        {
          call_info_ptr
            = ds3gdsdif_create_voice_call_instance(subs_indx,
                                                   call_info_cmd_ptr->call_id);
        }
      }
      else if ((call_info_cmd_ptr->call_event == CM_CALL_EVENT_HO_COMPLETE) &&
               (!ds3gdsdif_is_voice_call_active_with_call_id(subs_indx,
                                                            call_info_cmd_ptr->call_id)))
      {
         call_info_ptr
            = ds3gdsdif_create_voice_call_instance(subs_indx,
                                                   call_info_cmd_ptr->call_id);         
      }
      else
      {
        call_info_ptr
          = ds3gdsdif_get_voice_call_instance(subs_indx,
                                              call_info_cmd_ptr->call_id);
      }
      emergency_call = FALSE;
      break;

    case CM_CALL_TYPE_EMERGENCY:
      if( (call_info_cmd_ptr->call_event == CM_CALL_EVENT_ORIG) ||
          (call_info_cmd_ptr->call_event == CM_CALL_EVENT_INCOM) )
      {
        call_info_ptr
          = ds3gdsdif_create_emerg_call_instance(subs_indx,
                                                 call_info_cmd_ptr->call_id);
      }
      else
      {
        call_info_ptr
          = ds3gdsdif_get_emerg_call_instance(subs_indx,
                                              call_info_cmd_ptr->call_id);
      }
      emergency_call = TRUE;
      break;

    default:
      break;
  }

  /* Return if call_info_ptr is NULL i.e. unhandled call_type */
  if(call_info_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_LOW, "Unhandled call_type:%d returning",
              call_info_cmd_ptr->call_type);
    return;
  }

  /*-----------------------------------------------------------------------
    Handle call events on the same subscription
  -----------------------------------------------------------------------*/
  switch(call_info_cmd_ptr->call_event)
  {
    case CM_CALL_EVENT_ORIG:
    case CM_CALL_EVENT_INCOM:
      /* cache call info (call id and state) */
      call_info_ptr->call_id = call_info_cmd_ptr->call_id;
      call_info_ptr->state   = DS3GDSDIF_VOICE_CALL_ORIG;

      /* reset call_info_ptr->radio for new voice/emergency call*/
      call_info_ptr->radio = SYS_SYS_MODE_NO_SRV;
      call_info_ptr->voice_ho_type = SYS_VOICE_HO_NONE;

      /*---------------------------------------------------------------------
        Send voice call orig / emergency call orig events to DSD
      ---------------------------------------------------------------------*/
      dsd_event_info.event
        = (emergency_call ?
           DS_DSD_CLIENT_RADIO_EMERG_CALL_ORIG_EV :
           DS_DSD_CLIENT_RADIO_VOICE_CALL_ORIG_EV);
      dsd_event_info.subs_id = subs_indx;
      notify_dsd = TRUE;
      break;

    case CM_CALL_EVENT_CONNECT:
      if(call_info_ptr->call_id == call_info_cmd_ptr->call_id)
      {
        /* cache call info (radio and state) */
        call_info_ptr->state = DS3GDSDIF_VOICE_CALL_CONNECTED;
        call_info_ptr->radio = call_info_cmd_ptr->call_sys_mode;

        /*-------------------------------------------------------------------
          If voice call/ emergency  not on 1x
            Send voice call end / emergency call end event to DSD
          Else (call on 1x)
            Send voice call connect / emergency call started event to DSD
        -------------------------------------------------------------------*/
        if(call_info_cmd_ptr->call_sys_mode != SYS_SYS_MODE_CDMA)
        {
          dsd_event_info.event
            = (emergency_call ?
               DS_DSD_CLIENT_RADIO_EMERG_CALL_ENDED_EV :
               DS_DSD_CLIENT_RADIO_VOICE_CALL_ENDED_EV);
          dsd_event_info.subs_id = subs_indx;
          notify_dsd = TRUE;
        }
        else
        {
          dsd_event_info.event
            = (emergency_call ?
               DS_DSD_CLIENT_RADIO_EMERG_CALL_STARTED_EV :
               DS_DSD_CLIENT_RADIO_VOICE_CALL_CONNECTED_EV);
          dsd_event_info.subs_id = subs_indx;
          notify_dsd = TRUE;
        }

        /*-------------------------------------------------------------------
          If voice call/emergency call is on GSM, process cached SS event.
          This would check if GSM is on a non-DTM cell or DTM cell and is
          automatically handled.
        -------------------------------------------------------------------*/
        if(call_info_cmd_ptr->call_sys_mode == SYS_SYS_MODE_GSM)
        {
          ds3gdsdif_process_gsm_voice_call_event(
                             ds3gsubsmgr_subs_id_ds3g_to_cm(subs_indx),
                             call_info_cmd_ptr->call_event);
        }
      }
      break;

    /*-----------------------------------------------------------------------
     This is to handle SRVCC to GSM calls
     In case of VOLTE calls originated from 3rd party IMS, CALL_ORIG,
     CALL_CONNECTED events are not received by DS.
     No harm in handling this event for QC-IMS as well.
    -----------------------------------------------------------------------*/
    case CM_CALL_EVENT_HO_COMPLETE:
      if((call_info_cmd_ptr->call_sys_mode == SYS_SYS_MODE_GSM) &&
         (call_info_cmd_ptr->call_voice_ho_type == SYS_VOICE_HO_SRVCC_L_2_G))
      {
        /* Store call_id since CALL_ORIG or CALL_INCOM is not received
           for SRVCC */
        call_info_ptr->call_id = call_info_cmd_ptr->call_id;
        call_info_ptr->state = DS3GDSDIF_VOICE_CALL_CONNECTED;
        call_info_ptr->radio = call_info_cmd_ptr->call_sys_mode;
        call_info_ptr->voice_ho_type = call_info_cmd_ptr->call_voice_ho_type;

        ds3gdsdif_process_gsm_voice_call_event(
                             ds3gsubsmgr_subs_id_ds3g_to_cm(subs_indx),
                             call_info_cmd_ptr->call_event);
      }
      break;

    case CM_CALL_EVENT_END:
      if(call_info_ptr->call_id == call_info_cmd_ptr->call_id)
      {
        /* cache call info (radio and state) */
        call_info_ptr->state = DS3GDSDIF_VOICE_CALL_INACTIVE_PENDING;

        /*-------------------------------------------------------------------
          If voice call/emergency call is on GSM, process cached SS event.
          This would check if GSM is on a non-DTM cell or DTM cell and is
          automatically handled.
        -------------------------------------------------------------------*/
        if((call_info_cmd_ptr->call_sys_mode == SYS_SYS_MODE_GSM) ||
           (ds3gdsdif_get_preferred_sys(subs_indx) == SYS_SYS_MODE_GSM))
        {
          /* Update cached call_info radio since call may have landed to
             G via SRVCC or W2G etc */
          call_info_ptr->radio = call_info_cmd_ptr->call_sys_mode;

          ds3gdsdif_process_gsm_voice_call_event(
                         ds3gsubsmgr_subs_id_ds3g_to_cm(subs_indx),
                         call_info_cmd_ptr->call_event);
        }

        /*-------------------------------------------------------------------
          If voice/CS Data/VT call ends or emergency calls ends and UE not
          in ECBM, Send voice call end / emergency call end event to DSD
        -------------------------------------------------------------------*/
        if((call_info_cmd_ptr->call_type == CM_CALL_TYPE_VOICE) ||
           (call_info_cmd_ptr->call_type == CM_CALL_TYPE_CS_DATA) ||
           (call_info_cmd_ptr->call_type == CM_CALL_TYPE_VT)      ||
           ((emergency_call && ds3g_is_ph_state_emergency_cb_mode() == FALSE)))
        {
          dsd_event_info.event = (emergency_call ?
                                  DS_DSD_CLIENT_RADIO_EMERG_CALL_ENDED_EV :
                                  DS_DSD_CLIENT_RADIO_VOICE_CALL_ENDED_EV);

          dsd_event_info.subs_id = subs_indx;
          call_info_ptr->state = DS3GDSDIF_VOICE_CALL_INACTIVE;

          /* Do not reset cached call_info radio. This info will be used to check
             if GSM call ended and ps_suspend flag is set i.e RAU/TAU
             procedures ongoing.
             This flag will be reset during CALL_ORIG or CALL_INCOMING event */
          //call_info_ptr->radio = SYS_SYS_MODE_NO_SRV;

          if(call_info_cmd_ptr->call_sys_mode != SYS_SYS_MODE_GSM)
          {
            notify_dsd = TRUE;
          }
        }
        /* free the call instance if call event is call_end */
        if (!emergency_call)
        {
          ds3gdsdif_delete_voice_call_instance(subs_indx, call_info_cmd_ptr->call_id);
        }
        else if((ds3g_is_ph_state_emergency_cb_mode() == FALSE))
        {
          ds3gdsdif_delete_emerg_call_instance(subs_indx, call_info_cmd_ptr->call_id);
        }
      }
      break;

    default:
      break;
  }
  if(notify_dsd == TRUE)
  {
    /*-----------------------------------------------------------------------
      As system mode is not available at the time of call orig, 1x is used
      as default sys mode to satisfy the function declaration. It is just a
      dummy here.
    -----------------------------------------------------------------------*/
    ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
  }

  /*-----------------------------------------------------------------------
    Handle call events on the other subscription
  -----------------------------------------------------------------------*/
  /* check if device mode is DSDS */
  if( ds3gsubsmgr_get_device_mode() ==
      SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  "device_mode:DSDS. Notify voice call event on other sub");

    memset(&dsd_event_info, 0, sizeof(ds_dsd_event_info_type));
    switch(call_info_cmd_ptr->call_event)
    {
      case CM_CALL_EVENT_ORIG:
      case CM_CALL_EVENT_INCOM:
      case CM_CALL_EVENT_CONNECT:
        dsd_event_info.event
          = DS_DSD_CLIENT_RADIO_VOICE_CALL_UP_ON_OTHER_SUB_EV;
        break;

      case CM_CALL_EVENT_END:
        dsd_event_info.event
          = DS_DSD_CLIENT_RADIO_VOICE_CALL_DOWN_ON_OTHER_SUB_EV;
        break;

      default:
        break;
    }

    other_subs_id = (sys_modem_as_id_e_type)DS3GDSDIF_GET_OTHER_SUBS_INDX(subs_indx);
    ds3gstkproc_process_cached_stack_info(other_subs_id);

    dsd_event_info.subs_id = (ds3gsubsmgr_subs_id_e_type)other_subs_id;

  /*-------------------------------------------------------------------------
      As system mode is not available at the time of call orig, 1x is used
      as default sys mode to satisfy the function declaration. It is just a
      dummy here.
  -------------------------------------------------------------------------*/
    ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
  }

  return;
}/* ds3gdsdif_process_voice_call_event_cmd() */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS3GDSDIF_WLAN_SVC_CHG_CB

DESCRIPTION   This callback is called to notify WLAN status change.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_wlan_srv_chg_cb
(
  ps_sys_tech_enum_type     tech_type,
  ps_sys_event_enum_type    event_name,
  void                      *event_info_ptr,
  void                      *user_data_ptr
)
{
  ds_cmd_type              *cmd_ptr = NULL;
  ps_sys_wlan_status_type  *data_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "ds3gdsdif_wlan_srv_chg_cb: tech:%d, evnt:%d, evnt_ptr:0x%x",
                  tech_type, event_name, event_info_ptr);

  /* validate event_id and pointer */
  if( (event_name != PS_SYS_EVENT_WLAN_STATUS_CHANGE) ||
      (event_info_ptr == NULL) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Un-registered event or event_info_ptr is NULL");
    return;
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ps_sys_wlan_status_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_DS3GDSDIF_WLAN_SYS_CHG;
  data_block_ptr = (ps_sys_wlan_status_type*)cmd_ptr->cmd_payload_ptr;

  /* copy contents to command */
  memset(data_block_ptr, 0, sizeof(ps_sys_wlan_status_type));
  memscpy(data_block_ptr,
          sizeof(ps_sys_wlan_status_type),
          event_info_ptr,
          sizeof(ps_sys_wlan_status_type));

  ds_put_cmd(cmd_ptr);

  return;
}/* ds3gdsdif_wlan_srv_chg_cb() */

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_WLAN_SVC_CHG_CMD

DESCRIPTION   This function processes the DS3GDSDIF_WLAN_SYS_CHG_CMD.

PARAMETERS    cmd_ptr: command pointer.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_process_wlan_svc_chg_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ps_sys_wlan_status_type  *wlan_info = NULL;
  sys_sys_mode_mask_e_type  subs_capability;
  boolean wlan_notif_status = FALSE;                  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  wlan_info = (ps_sys_wlan_status_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "ds3gdsdif_process_wlan_svc_chg_cmd: wlan_avail:%d, "
                  "wqe supported:%d",
                  wlan_info->is_wlan_available, wlan_info->wqe_status);

  if ( wlan_info->is_wlan_available && 
       wlan_info->wlan_assoc_type == PS_SYS_WLAN_ASSOC_STA ) 
  {
    subs_capability = ds3gsubsmgr_get_subs_capability_for_subs_id(SYS_MODEM_AS_ID_1);
    if ( subs_capability & SYS_SYS_MODE_MASK_LTE )
    {
      wlan_notif_status = TRUE;
    }

    DATA_3GMGR_MSG1 (MSG_LEGACY_HIGH,
                     "ds3gdsdif_process_wlan_svc_chg_cmd: wlan_notif_status:%d, ",
                     wlan_notif_status);
    ds3gdsdif_notify_wlan_status(wlan_info,wlan_notif_status);
    ds3gdsdif_wlan_notified = wlan_notif_status;
  }
  else
  {
    ds3gdsdif_notify_wlan_status(wlan_info,wlan_notif_status);
    ds3gdsdif_wlan_notified = wlan_notif_status;
  }

#ifdef FEATURE_DATA_ANDSF
  ds_andsf_notify_wlan_info(wlan_info);
#endif /*FEATURE_DATA_ANDSF*/


  return;
}/* ds3gdsdif_process_wlan_svc_chg_cmd() */

/*===========================================================================
FUNCTION     DS3GDSDIF_CPY_PS_SYS_WLAN_ADDR

DESCRIPTION  This utility function is used to cpy ps_sys wlan info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_cpy_ps_sys_wlan_addr
(
  wlan_addr_type   *wlan_addr_dest,
  ps_ip_addr_type   ipv4_addr1_src,
  ps_ip_addr_type   ipv4_addr2_src,
  ps_ip_addr_type   ipv6_addr1_src,
  ps_ip_addr_type   ipv6_addr2_src
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memscpy(&(wlan_addr_dest->wlan_ipv4_addr1),
          sizeof(ps_ip_addr_type),
          &(ipv4_addr1_src),
          sizeof(ps_ip_addr_type));

  memscpy(&(wlan_addr_dest->wlan_ipv4_addr2),
          sizeof(ps_ip_addr_type),
          &(ipv4_addr2_src),
          sizeof(ps_ip_addr_type));

  memscpy(&(wlan_addr_dest->wlan_ipv6_addr1),
          sizeof(ps_ip_addr_type),
          &(ipv6_addr1_src),
          sizeof(ps_ip_addr_type));

  memscpy(&(wlan_addr_dest->wlan_ipv6_addr2),
          sizeof(ps_ip_addr_type),
          &(ipv6_addr2_src),
          sizeof(ps_ip_addr_type));

  return;
}
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION     DS3GDSDIF_NOTIFY_EMERGERGENCY_CB_MODE

DESCRIPTION  This function is used emergency callback mode

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gdsdif_notify_emergergency_cb_mode
(
  sys_modem_as_id_e_type  asubs_id,
  boolean                 emergency_cb_mode
)
{
  ds_dsd_event_info_type      dsd_event_info;
  ds3gsubsmgr_subs_id_e_type  ds3g_call_subs_id;
  ds3gdsdif_voice_call_state_type  emerg_call_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG1(MSG_LEGACY_HIGH,
            "ds3gdsdif_notify_emergergency_cb_mode: emergency_cb_mode:%d",
            emergency_cb_mode);

  ds3g_call_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id);

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_call_subs_id))
  {
    return;
  }

  memset(&dsd_event_info,0,sizeof(ds_dsd_event_info_type));

  /* If emergency callback exits, notify DSD if notification still pending */
  if(emergency_cb_mode == FALSE)
  {
    emerg_call_state = ds3gdsdif_get_emerg_call_state(ds3g_call_subs_id);

    if( (emerg_call_state.is_call_active == FALSE) &&
        (emerg_call_state.info_p != NULL) &&
        (emerg_call_state.info_p->state == 
           DS3GDSDIF_VOICE_CALL_INACTIVE_PENDING) )
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_EMERG_CALL_ENDED_EV;
      dsd_event_info.subs_id = ds3g_call_subs_id;
      ds_dsd_ext_event_notify(
         ds_dsd_ext_translate_ss_to_dsd_radio(
            emerg_call_state.info_p->radio),
         &dsd_event_info);

      emerg_call_state.info_p->state
        = DS3GDSDIF_VOICE_CALL_INACTIVE;

      /* Do Not reset call_info_ptr->radio. This flag will be reset during
         CALL_ORIG or CALL_INCOMING event */
      //ds3gdsdif_emergency_call_info[ds3g_call_subs_id].radio
      //  = SYS_SYS_MODE_NO_SRV;
      
      ds3gdsdif_delete_emerg_call_instance(ds3g_call_subs_id, 
                                            emerg_call_state.info_p->call_id);
    }
  }
  return;
}/* ds3gdsdif_notify_emergergency_cb_mode() */

/*===========================================================================
FUNCTION      DS3GDSDIF_GET_SRV_STATUS_FROM_DS3G_RADIO_CACHE

DESCRIPTION   Utility function to get the srv status from ds3g stk table.

DEPENDENCIES  None.

RETURN VALUE  The srv status of the pref radio from the ds3g radio cache

SIDE EFFECTS  None.
===========================================================================*/
sys_srv_status_e_type dsd3gif_get_srv_status_from_ds3g_radio_cache
(
  sys_modem_as_id_e_type       asubs_id,
  ds_dsd_ext_radio_info_type  *radio_ptr
)
{
  uint8                        stack_id = CM_NO_STACKS;
  ds3g_cm_ss_stack_info_type  *cached_stack_info_ptr = NULL;
/*-------------------------------------------------------------------------*/
  if (radio_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "radio_ptr NULL");
    return SYS_SRV_STATUS_NO_SRV;
  }

  stack_id = ds3gstkproc_get_stack_id_from_radio(asubs_id,
                                                 radio_ptr->system_mode);

  if(stack_id >= CM_NO_STACKS)
  {
    return SYS_SRV_STATUS_NO_SRV;
  }

  ds3gstkproc_get_stack_info_from_stack_id(
     ds3gsubsmgr_subs_id_cm_to_ds3g(asubs_id),
     stack_id,
     &cached_stack_info_ptr);

  if(cached_stack_info_ptr == NULL)
  {
    DATA_MSG1(MSG_LEGACY_LOW,
              "Invalid cached_stack_info_ptr for stack_id: %d", stack_id);
    return SYS_SRV_STATUS_NO_SRV;
  }

  /*-----------------------------------------------------------------------
    If the preferred radio is no longer available, retrun no service
  -----------------------------------------------------------------------*/
  if((cached_stack_info_ptr->sys_mode != radio_ptr->system_mode) &&
     ((cached_stack_info_ptr->sys_mode > SYS_SYS_MODE_NO_SRV)
      && (cached_stack_info_ptr->sys_mode < SYS_SYS_MODE_MAX)))
  {
    DATA_MSG2(MSG_LEGACY_LOW,
              "pref radio sys_mode:%d not found on stack%d, return no service",
              cached_stack_info_ptr->sys_mode,
              stack_id);
    return SYS_SRV_STATUS_NO_SRV;
  }

  return cached_stack_info_ptr->srv_status;
} /* dsd3gif_get_sys_mode_from_ds3g_radio_cache() */

/*===========================================================================
FUNCTION      DS3GDSDIF_PROCESS_MT_CSFB_HYST_TIMER_EXP_CMD

DESCRIPTION   This function processes DS_CMD_DS3GDSDIF_MT_CSFB_HYST_TIMER_EXP 
              command.

PARAMETERS    cmd_ptr

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gdsdif_process_mt_csfb_hyst_timer_exp_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id;
  ds3gdsdif_voice_call_state_type  voice_call_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ( cmd_ptr == NULL ) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  if(cmd_ptr->hdr.cmd_id != DS_CMD_DS3GDSDIF_MT_CSFB_HYST_TIMER_EXP)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid command:%d",
                    cmd_ptr->hdr.cmd_id);
    return;
  }

  ds3g_subs_id = *((ds3gsubsmgr_subs_id_e_type*)cmd_ptr->cmd_payload_ptr);
  voice_call_state = ds3gdsdif_get_voice_call_state(ds3g_subs_id);

  /*-----------------------------------------------------------------------
    Handle MT CSFB hysteresis timer expired only if current voice state is
    MT_CSFB_PAGED
  -----------------------------------------------------------------------*/
  if( (voice_call_state.is_call_active == TRUE) &&
      (voice_call_state.info_p != NULL) &&
      (voice_call_state.info_p->state == DS3GDSDIF_VOICE_CALL_MT_CSFB_PAGED)
    )
  {
    voice_call_state.info_p->state = DS3GDSDIF_VOICE_CALL_INACTIVE_PENDING;

    /* It MT_CSFB_ESTAB_HYSTERESIS timer expires, process it as GSM voice
       call end event */
    ds3gdsdif_process_gsm_voice_call_event(
       ds3gsubsmgr_subs_id_ds3g_to_cm(ds3g_subs_id),
       CM_CALL_EVENT_END);

    voice_call_state.info_p->state = DS3GDSDIF_VOICE_CALL_INACTIVE;

    ds3gdsdif_delete_voice_call_instance(ds3g_subs_id, voice_call_state.info_p->call_id);
  }

  return;
} /* ds3gdsdif_process_mt_csfb_hyst_timer_exp_cmd() */
