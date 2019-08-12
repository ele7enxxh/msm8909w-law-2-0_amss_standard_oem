/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( M O B I L E  E Q U I P M E N T  C O M M A N D S )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the 
  Mobile Equipment commands.

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatetsime_init_me
    ME initialization function. This function does the following:
    - Initializes ATCOP as one of the clients to CM.
    - Registers the CM client call back functions.
    - Initializes the unsolicited response event queue.
    - Sets initial phone functionality level.

  dsatetsime_change_indicator_state
    This function updates the specified indicator to the specified
    state.  It provides a wrapper for indicator state tracking.  The
    state of the indicators is reported by +CIND.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsime_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk/pg   Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


/* this file should be included only in GSM or WCDMA mode */
#ifdef FEATURE_DSAT_ETSI_MODE

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "dsati.h"
#include "dsatcmif.h"
#include "dsatdl.h"
#include "dsatetsicmif.h"
#include "dsatetsime.h"
#include "dsatetsictab.h"
#include "dsatctab.h"
#include "dsatparm.h"
#include "dsatclienti.h"
#include "vbatt.h"
#include "cm.h"
#include "amssassert.h"
#include "msg.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include "dsm.h"
#include "time_svc.h"
#include "time_tod.h"

#include "dsatvend.h"
#ifdef FEATURE_DATA_PS_EAP
#include "intconv.h"
#include "ps_eap.h"
#include "ps_eap_sim_aka.h"
#include "ps_eap_aka.h"
#include "ps_eap_sim.h"
#endif /* FEATURE_DATA_PS_EAP */
#ifdef FEATURE_MMGSDI_ATCSIM
#include "uim.h"
#endif /* FEATURE_MMGSDI_ATCIM */

#include "dsatme.h"
#include "dsatetsicall.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#include "sys_v.h"

#include "uim_v.h"
#include "ps_utils.h"

boolean dsatetsime_set_time_zone_into_nv(sys_time_zone_type time_zone);

#ifdef FEATURE_DATA_PS_EAP
rex_timer_type dsatetsime_ceap_report_timer;
rex_timer_type dsatetsime_ceap_eap_process_timer;
#endif /* FEATURE_DATA_PS_EAP */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
            Function Definitions: Public functions
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSATETSIME_CHANGE_INDICATOR_STATE

DESCRIPTION
  This function updates the specified indicator to the specified
  state.  It provides a wrapper for indicator state tracking.  The
  state of the indicators is reported by +CIND.  Unsolicited response
  code generation is not suported but could be added to this function
  in future release.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :  if there was any problem in executing the command
    DSAT_OK :     if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_change_indicator_state
(
  uint8                  indicator,
  dsat_ind_val_type      value,
  sys_modem_as_id_e_type subs_id
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type current_cind_val;

  if (!(IS_VALID_SUBS_ID(subs_id)))
  {
    DS_AT_MSG1_ERROR("Invalid subscription specified: %d",subs_id);
    return DSAT_ASYNC_EVENT;
  }

  if (MAX_CIND_INDICATORS > indicator)
  {
    /* Check for state change */
    current_cind_val = (dsat_num_item_type)dsatutil_get_val(
                   DSATETSI_EXT_ACT_CIND_ETSI_IDX,subs_id,indicator,NUM_TYPE);
    if ( current_cind_val != value )
    {
      DS_AT_MSG3_MED("Changing indicator[%d] value: old=%d new=%d",
               indicator, current_cind_val, value);
      
      /* Set indicator state */
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CIND_ETSI_IDX,subs_id,indicator,0,value,NUM_TYPE)

      if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatetsime_report_ciev_result_fp))
      {
        dsatdl_vtable.dsatetsime_report_ciev_result_fp(indicator, value, subs_id);
      }
    }
  }
  else
  {
    /* Report error */
    DS_AT_MSG1_ERROR("Unsupported indicator specified: %d",indicator);
    result = DSAT_ERROR;
  }
  
  return result;
} /* dsatetsime_change_indicator_state */


/*===========================================================================

FUNCTION DSATETSIME_GET_INDICATOR_STATE

DESCRIPTION
  This function returns the current value of the specified indicator.
  This is the value displayed for +CIND read syntax.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns indicator value as integer type.

SIDE EFFECTS
  None

===========================================================================*/
dsat_ind_val_type  dsatetsime_get_indicator_state
(
  uint8                   indicator,
  sys_modem_as_id_e_type  subs_id
)
{
  dsat_num_item_type  ind_state;
#ifdef FEATURE_DUAL_SIM
  ind_state = (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CIND_ETSI_IDX,subs_id,indicator,NUM_TYPE);
#else
  ind_state = (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CIND_ETSI_IDX,0,indicator,NUM_TYPE);
#endif /* FEATURE_DUAL_SIM */   
  return (dsat_ind_val_type)ind_state;
}

/*--------               INITIALIZATION FUNCTIONS                --------*/

/*===========================================================================

FUNCTION  DSATETSIME_INIT_ME

DESCRIPTION
  ME initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to CM.
  - Registers the CM client call back functions.
  - Initializes the unsolicited response event queue.
  - Sets initial phone functionality level.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_init_me ( void )
{
  dsat_memory_subs_type i;

  /* Assume phone initializes with unknown functionality. */
  /* This is required as Call Manager does not send    */
  /* an operating mode event during initialization.    */
  DSATUTIL_SET_VAL(DSAT_EXT_CFUN_IDX,0,0,0,DSAT_ME_FUNC_MAX,MIX_NUM_TYPE) 
  /* Clear ME indicators */
  for (i = DSAT_MS_FIRST_SUBS; i < DSAT_MS_MAX_SUBS; i++)
  {
    memset((void*)dsatutil_get_val(DSATETSI_EXT_ACT_CIND_ETSI_IDX,i,0,STR_TYPE), 
                 0x0,
                 MAX_CIND_INDICATORS * sizeof(dsat_num_item_type));
  }

  for (i = DSAT_MS_FIRST_SUBS; i < DSAT_MS_MAX_SUBS; i++)
  {
#ifdef FEATURE_DATA_ETSI_PIN
    dsatutil_reset_password(dsatutil_get_val(
                DSATETSI_EXT_ACT_CPIN_ETSI_IDX,i,0,MIX_STR_TYPE), PIN_MAX_LEN+1);
    dsatutil_reset_password(dsatutil_get_val(
                DSATETSI_EXT_ACT_CPIN_ETSI_IDX,i,1,MIX_STR_TYPE), PIN_MAX_LEN+1);
#endif /* FEATURE_DATA_ETSI_PIN */
    dsat_sysinfo_cmd.sim_valid[i] = DSAT_SIM_VALID_NOT_PRESENT;
  }
   dsatetsime_init_auto_nitz_setting_from_nv();
  return;
}/* dsatetsime_init_me */

/*===========================================================================

FUNCTION  DSATETSIME_CONVERT_RSSI

DESCRIPTION
  This function converts the RSSI values from Call Manager into a
  value used by AT commands.  The RSSI value and scale factor are
  input parameters.  The output value will represent the signal level
  as a fraction of the scale value.

  Both +CSQ and +CIND utilize this function.

DEPENDENCIES
  None

RETURN VALUE
  Signal level as integer, scaled based on parameter.

SIDE EFFECTS
  None

===========================================================================*/
uint8 dsatetsime_convert_rssi
(
  uint16    rssi,    /* Call Manager RSSI as positive integer */
  uint8     scale    /* ATCOP scaling factor */
)
{
  uint8 siglvl = 0;

  if(dsatcmdp_get_current_mode() == DSAT_MODE_TDS)
  {
    /* Note: RSSI dbm is really a negative value        */
    if ((RSSI_TDS_MIN < rssi) &&
      (RSSI_TDS_MAX > rssi))
    {
    /* Round to nearest integer on scale */
      siglvl = (uint8)floor(((rssi * RSSI_TDS_SLOPE + RSSI_TDS_OFFSET) *
                            scale)/100 + 0.5);
    }
    else if ((RSSI_TDS_MAX <= rssi) && 
             (RSSI_TDS_NO_SIGNAL != rssi)) 
    {
      siglvl = RSSI_TOOLO_CODE;
    }
    else if (RSSI_TDS_MIN >= rssi)
    {
      siglvl = RSSI_TDS_TOOHI_CODE;  /* Capped maximum */
    }
    else
    {
      DS_AT_MSG1_MED("RSSI outside signal range: %d", rssi);
      siglvl = RSSI_TDS_UNKNOWN_CODE;
    }
  }
  else
  {
  /* Note: RSSI dbm is really a negative value        */
  if ((RSSI_MIN < rssi) &&
      (RSSI_MAX > rssi))
  {
    /* Round to nearest integer on scale */
    siglvl = (uint8)floor(((rssi * RSSI_SLOPE + RSSI_OFFSET) *
                            scale)/100 + 0.5);
  }
  else if ((RSSI_MAX <= rssi) && 
           (RSSI_NO_SIGNAL != rssi)) 
  {
    siglvl = RSSI_TOOLO_CODE;
  }
  else if (RSSI_MIN >= rssi)
  {
    siglvl = RSSI_TOOHI_CODE;  /* Capped maximum */
  }
  else
  {
    DS_AT_MSG1_MED("RSSI outside signal range: %d", rssi);
    siglvl = RSSI_UNKNOWN_CODE;
  }
  }    
  return siglvl;
} /* dsatetsime_convert_rssi */

#ifdef FEATURE_DATA_ETSI_PIN
/*===========================================================================

FUNCTION  DSAT_PIN_MMGSDI_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the GSDI events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat_pin_mmgsdi_event_handler_ex
(
  ds_at_mmgsdi_event_type *mmgsdi_event_info /* MMGSDI event info pointer */
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_EVENT;
  dsat_apps_id_e_type    apps_id;
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_events_enum_type mmgsdi_event;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
    dsatme_mmgsdi_state_ms_slot_info  *me_ms_slot_val = NULL;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;

  apps_id = mmgsdi_event_info->apps_id;
  mmgsdi_event = mmgsdi_event_info->mmgsdi_event;

  /* Convert the detailed error to general error. If it is not an error
     then the event remains the same */
  if( mmgsdi_event != MMGSDI_CARD_INSERTED_EVT &&
      mmgsdi_event != MMGSDI_CARD_REMOVED_EVT &&
      mmgsdi_event != MMGSDI_CARD_ERROR_EVT )
  {
    me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE);
    if (NULL == me_ms_val)
    {
      return DSAT_ASYNC_EVENT;
    }
  }
  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, TRUE);

  if (NULL == me_ss_val)
  {
    return DSAT_ASYNC_EVENT;
  }
  switch (mmgsdi_event)
  {
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      /*Update the flag to indicate SIM INIT COMPLETED*/
      if (!IS_VALID_GW_APPS_ID(apps_id))
      {
         return DSAT_ERROR;
      }

      me_ms_slot_val = dsat_get_base_addr_per_slot(mmgsdi_event_info->slot_id, TRUE);
      if (NULL == me_ms_slot_val)
      {
        return DSAT_ASYNC_EVENT;
      }

      if (me_ms_slot_val->dsat_qcsimstat_curr != DSAT_QCSIMSTAT_SIM_INIT_COMPLETED)
      {
        me_ms_slot_val->dsat_qcsimstat_curr = DSAT_QCSIMSTAT_SIM_INIT_COMPLETED;
        me_ms_slot_val->is_qcsimstat_updated = TRUE;
      }

#ifdef FEATURE_DATA_PS_EAP
      if(me_ss_val->dsat_eap_info.is_initialized == FALSE)
      {
        eap_aka_init();
        eap_sim_init();
        me_ss_val->dsat_eap_info.is_initialized = TRUE;
      }
#endif /* FEATURE_DATA_PS_EAP */
#if defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)
       /*ATcop might miss session changed event  due to late registration. Update it here*/
      me_ss_val->dsat_apps_provisioned = DSAT_MMGSDI_ACTIVE;
#endif /* defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION) */
      /* Query MMGSDI for all PIN status as ATCoP might miss to process PIN events before
         subscription ready event at boot up due to timing issue on few of the targets. */
      GET_PIN_STATUS( dsat_qcsimapp_info[apps_id].session_id, CPIN_MMGSDI_ALL_PIN_STATUS );
      if ( MMGSDI_SUCCESS != mmgsdi_status )
      {
        DS_AT_MSG0_ERROR(" Initial status query for all PINs failed");
      }
    break;

  case MMGSDI_SESSION_CHANGED_EVT:
     	
#if defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)
    if (apps_id == DSAT_APPS_ID_GW_PRIMARY)
    {
      if(TRUE == mmgsdi_event_info->activated)
      {
        me_ss_val->dsat_apps_provisioned = DSAT_MMGSDI_ACTIVE;
        /* mapping slot info with subscription*/
        /* In DSDA every information stored at index 0*/
        /* Index 0 will have provisened slot info*/
        /* Swap slots if required */

        if(dsat_qcsimapp_info[apps_id].slot != mmgsdi_event_info->slot_id)
        {
          mmgsdi_slot_id_enum_type sec_slot;
          
          sec_slot = dsat_qcsimapp_info[DSAT_APPS_ID_GW_PRIMARY].slot;
          dsat_qcsimapp_info[DSAT_APPS_ID_GW_PRIMARY].slot = mmgsdi_event_info->slot_id;
          dsat_qcsimapp_info[DSAT_APPS_ID_GW_SECONDARY].slot = sec_slot;
        }
      }
      else
      { /* Erase mapping */
        /* Dont reset to INACTIVE if last issued command is CFUN = 0 or CFUN =1 */
        
        if( DSAT_MMGSDI_DOWN != me_ss_val->dsat_apps_provisioned &&
            DSAT_MMGSDI_UP   !=   me_ss_val->dsat_apps_provisioned)
        {
          me_ss_val->dsat_apps_provisioned = DSAT_MMGSDI_INACTIVE;
        }
      }
    }
#endif /*defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION) */
    dsatcmdp_set_restricted_mode( TRUE,apps_id );
    /* Set SIM state waiting */
    
    /*Check to not move SIM into BUSY when SIM is INVALID or REMOVED*/
    if (TRUE == mmgsdi_event_info->activated)
    {
      me_ms_val->dsat_pin_error = DSAT_CME_SIM_BUSY;
    }
            /* Query MMGSDI for initial PIN status */
    GET_PIN_STATUS( dsat_qcsimapp_info[apps_id].session_id, CPIN_MMGSDI_ALL_PIN_STATUS );
    if ( MMGSDI_SUCCESS != mmgsdi_status )
    {
      DS_AT_MSG0_ERROR(" Initial status query for all PINs failed");
    }
    break;

  default:
    DS_AT_MSG1_LOW("Unsupported MMGSDI event: %d", mmgsdi_event);
    break;
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsat_pin_mmgsdi_event_handler_fp))
  {
    result = dsatdl_vtable.dsat_pin_mmgsdi_event_handler_fp(mmgsdi_event_info);
  }

  return result;
} /* dsat_pin_mmgsdi_event_handler_ex */


/*===========================================================================

FUNCTION CPIN_TRACK_PIN_STATUS

DESCRIPTION
  This function updates internal state variables to track the current 
  PIN status event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD if the request is sent to gsdi to verify pin
                 or sent to pbm to set the storage type.
  DSAT_ERROR if the set storage request is refused by PBM.
             It should not happen.  A system ERR is submitted.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cpin_track_pin_status
(
  mmgsdi_events_enum_type     event,
  mmgsdi_pin_status_enum_type pin_status,
  dsat_apps_id_e_type         apps_id
)
{
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  

  DS_AT_MSG1_HIGH("cpin_track_pin_status: pin_status = %d",pin_status);

  me_ms_val = dsat_get_base_addr_per_session(apps_id, FALSE);

  switch (event)
  {
  case MMGSDI_PIN1_EVT:
    switch(pin_status)
    {
      case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        me_ms_val->dsat_pin_error = DSAT_CME_NO_ERROR;
        me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsatcmdp_set_restricted_mode( TRUE, apps_id );
        dsat_sysinfo_cmd.sim_valid[apps_id] = DSAT_SIM_VALID_NOT_PRESENT;
        DS_AT_MSG1_MED("Restricted Mode set, SIM PIN1 enabled: %d",event);
        break;

      case MMGSDI_PIN_BLOCKED:
        me_ms_val->dsat_pin_error = DSAT_CME_NO_ERROR;
        me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        me_ms_val->dsat_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsatcmdp_set_restricted_mode( TRUE, apps_id );
        DS_AT_MSG1_MED("Restricted Mode set, SIM PIN1 blocked: %d",event);
        dsat_sysinfo_cmd.sim_valid[apps_id] = DSAT_SIM_VALID_NOT_PRESENT;
        break;

      case MMGSDI_PIN_PERM_BLOCKED:
        me_ms_val->dsat_pin_error = DSAT_CME_SIM_FAILURE;
        me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        me_ms_val->dsat_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsatcmdp_set_restricted_mode(TRUE, apps_id); /*For compilation put zero*/
        DS_AT_MSG1_MED("Restricted Mode set, SIM PIN1 permanently blocked: %d",event);
        dsat_sysinfo_cmd.sim_valid[apps_id] = DSAT_SIM_VALID_NOT_PRESENT;
        break;

      case MMGSDI_PIN_DISABLED:
      case MMGSDI_PIN_ENABLED_VERIFIED:
        me_ms_val->dsat_pin_error = DSAT_CME_NO_ERROR;
        me_ms_val->dsat_pin_required &= ~(uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        me_ms_val->dsat_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat_sysinfo_cmd.sim_valid[apps_id] = DSAT_SIM_VALID_PRESENT;
        break;

      case MMGSDI_PIN_UNBLOCKED:
        me_ms_val->dsat_pin_error = DSAT_CME_NO_ERROR;
        me_ms_val->dsat_pin_required &= ~(uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        me_ms_val->dsat_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat_sysinfo_cmd.sim_valid[apps_id] = DSAT_SIM_VALID_PRESENT;
   /* 
      If CPIN is Cleared and if sim init is completed prior to,
      then clear Restricted Mode.
   */
      if(me_ms_val->dsat_sim_init_comp)
      {
         DS_AT_MSG0_HIGH("Clearing Restricted mode after CPIN clearing");
          dsatcmdp_set_restricted_mode( me_is_restricting_pin_puk_set(apps_id) , apps_id);
      }
      break;

     default:
       break;
    }
    break;
      
  case MMGSDI_SUBSCRIPTION_READY_EVT:   /* Compatibility with UI */
    DS_AT_MSG1_MED("Restricted Mode clr %d",event);
    me_ms_val->dsat_sim_init_comp = TRUE;
    me_ms_val->dsat_pin_error = DSAT_CME_NO_ERROR;
    /* Check for disabling restricted command mode */
    dsatcmdp_set_restricted_mode( me_is_restricting_pin_puk_set(apps_id) , apps_id);
    break;

  case MMGSDI_PIN2_EVT:
    switch(pin_status)
    {
      case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        me_ms_val->dsat_pin_error = DSAT_CME_NO_ERROR;
        me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        DS_AT_MSG1_MED("SIM PIN2 enabled: %d",event);
        break;

      case MMGSDI_PIN_BLOCKED:
        me_ms_val->dsat_pin_error = DSAT_CME_NO_ERROR;
        me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        me_ms_val->dsat_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        DS_AT_MSG1_MED("SIM PIN2 blocked: %d",event);
        break;

      case MMGSDI_PIN_DISABLED:
      case MMGSDI_PIN_ENABLED_VERIFIED:
      case MMGSDI_PIN_UNBLOCKED:
        me_ms_val->dsat_pin_error = DSAT_CME_NO_ERROR;
        me_ms_val->dsat_pin_required &= ~(uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        me_ms_val->dsat_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        DS_AT_MSG1_MED("SIM PIN2 cleared: %d",event);
        break;

      default:
        break;
    }
    break;

  default:
    DS_AT_MSG1_MED("Invalid PIN Event: %d",event);
    return DSAT_ERROR;
  }

  return DSAT_OK;
} /* cpin_track_pin_status */

#endif /* FEATURE_DATA_ETSI_PIN */

/*===========================================================================
FUNCTION ETSIME_MAP_CMD_TO_FILE

DESCRIPTION
  This function maps a command call back used as a client refernce to the 
  exact MMGSDI file name.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/
void etsime_map_cmd_to_file 
(
  mmgsdi_file_enum_type *filename, 
  cb_cmd_e_type          cb_cmd,
  dsat_apps_id_e_type    apps_id
)
{
  cb_cmd_e_type        temp_cb_cmd;
  mmgsdi_app_enum_type app_type = dsat_qcsimapp_info[apps_id].app_type;

/* Macro to set the file name based on the apps_enabled */
#define SET_FILENAME(app_type, file_gsm, file_umts)\
    if (app_type == MMGSDI_APP_SIM)  \
    {\
      *filename = file_gsm;\
    }\
    else if (app_type == MMGSDI_APP_USIM ) \
    {\
      *filename = file_umts;\
    }\
    else \
    {\
      DS_AT_MSG0_ERROR("NO APPS AVAILABLE");\
      /* Default to USIM IMSI */\
      *filename = file_umts;\
    }

   ASSERT(filename != NULL ); 

  /* Remove the Refresh Mask if any  */
  temp_cb_cmd = (cb_cmd_e_type )((int32)cb_cmd & (int32)(~DSAT_MMGSDI_REFRESH_MASK_CB));
  switch(temp_cb_cmd)
  {

  case CPUC_MMGSDI_SIM_READ:
    SET_FILENAME(app_type, MMGSDI_GSM_PUCT, MMGSDI_USIM_PUCT);
    break;
  case DSAT_MMGSDI_IMSI_READ:
    SET_FILENAME(app_type, MMGSDI_GSM_IMSI, MMGSDI_USIM_IMSI);
    break;

  case DSAT_MMGSDI_CSP_READ:
    SET_FILENAME(app_type, MMGSDI_GSM_CSP, MMGSDI_USIM_CSP);
    break;

  case DSAT_MMGSDI_SPN_READ:
    SET_FILENAME(app_type, MMGSDI_GSM_SPN, MMGSDI_USIM_SPN);
    break;

  case ICCID_MMGSDI_SIM_READ:
    *filename = MMGSDI_ICCID;
    break;

  case CUAD_MMGSDI_SIM_FILE_ATTR:
    *filename = MMGSDI_DIR;
    break;
	
  default:
    DS_AT_MSG0_ERROR("UNKNOWN SIM FILE");
    *filename = MMGSDI_NO_FILE_ENUM;
    break;
  }
}/* etsime_map_cmd_to_file */

#ifdef FEATURE_DATA_ETSI_PIN

/*===========================================================================

FUNCTION  ME_IS_RESTRICTING_PIN_PUK_SET

DESCRIPTION
  Determines from PIN/PUK state if AT comands should be restricted.

DEPENDENCIES
  None

RETURN VALUE
  FALSE : Don't restrict
  TRUE  : Restrict

SIDE EFFECTS
  None

===========================================================================*/
boolean me_is_restricting_pin_puk_set
(
  dsat_apps_id_e_type apps_id
)
{
  boolean restrict_pin = FALSE;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE);
  if (NULL == me_ms_val)
  {
    return restrict_pin;
  }

  restrict_pin = ( me_ms_val->dsat_pin_required & 
    ( (uint32)DSAT_CPIN_SIM_PIN_REQUIRED 
#ifdef FEATURE_MMGSDI_PERSONALIZATION      
    | (uint32)DSAT_CPIN_NET_PIN_REQUIRED
    | (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED
    | (uint32)DSAT_CPIN_SP_PIN_REQUIRED
    | (uint32)DSAT_CPIN_CORP_PIN_REQUIRED 
    | (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED 
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
    ) ) ||

    ( me_ms_val->dsat_pin_blocked &
      ( (uint32)DSAT_CPIN_SIM_PIN_REQUIRED
#ifdef FEATURE_MMGSDI_PERSONALIZATION      
      | (uint32)DSAT_CPIN_NET_PIN_REQUIRED
      | (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED
      | (uint32)DSAT_CPIN_SP_PIN_REQUIRED
      | (uint32)DSAT_CPIN_CORP_PIN_REQUIRED
      | (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
    ) );
   
  return restrict_pin;
} /* me_is_restricting_pin_puk_set */
#endif /* FEATURE_DATA_ETSI_PIN */

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION: DSATETSIME_SET_TIME_ZONE_INTO_NV

DESCRIPTION
  This function set time zone value into NV items.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatetsime_set_time_zone_into_nv(sys_time_zone_type time_zone)
{
  nv_item_type nv_item;
  nv_stat_enum_type  nv_stat = NV_DONE_S;
  memset ((void*)&nv_item, 0, sizeof(nv_item));
  nv_item.timezone = (nv_timezone_enum_type) time_zone;
  nv_stat = dsatutil_put_nv_item(NV_TIMEZONE_I, &nv_item);
  if (nv_stat != NV_DONE_S)
  {
    return FALSE;
  }
  return TRUE;
} /* dsatetsime_set_time_zone_into_nv */

/*===========================================================================
FUNCTION: DSATETSIME_INIT_AUTO_NITZ_SETTING_FROM_NV

DESCRIPTION
  This function initializes the auto NITZ update from NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_init_auto_nitz_setting_from_nv(void)
{
  nv_item_type    nv_item;

  if(dsatutil_get_nv_item(NV_SET_AUTO_TIME_I, 
                                         &nv_item) == NV_DONE_S)
  {
    if(nv_item.set_auto_time == FALSE)
    {
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CTZU_ETSI_IDX,0,0,0,0,NUM_TYPE)
    }
    else
    {
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CTZU_ETSI_IDX,0,0,0,1,NUM_TYPE)
    }
  }
} /* dsatetsime_init_auto_nitz_setting_from_nv */
#endif /* FEATURE_MMGSDI */
#endif /* FEATURE_DSAT_ETSI_MODE */
