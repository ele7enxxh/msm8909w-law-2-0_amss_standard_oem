/*===========================================================================

                           E C A L L _ A P P . C

DESCRIPTION
  eCall task source file

REFERENCES
  3GPP TS 26.267

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/hlap/src/ecall_app.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_ECALL_APP

#include "ecall_app_task.h"
#include "ecall_app.h"
#include "ecall_app_i.h"
#include "ecall_efs.h"
#include "ecall_gps.h"
#include "ecall_diag.h"
#include "ecall_msd.h"
#include "err.h"

#include "ecall_sim.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

/*===========================================================================

                         I N T E R N A L    D A T A

===========================================================================*/

#define ECALL_NUM_OF_CALL_ID 1
#define ECALL_MANUAL_EMERG_SRV_CATEG 0x20 /* 6th bit set in emergency setup message */
#define ECALL_AUTO_EMERG_SRV_CATEG   0x40 /* 7th bit set */
#define ECALL_MAX_PBM_RECORDS           2 /* Records for TEST and RECONFIG number */
#define ECALL_MAX_REC_LEN            1500
#define ECALL_DIGITS_LEN_MAX         18
#define ECALL_APP_CM_CALL_ID_MAX     256

typedef enum
{
  ECALL_APP_CALL_STATUS_INACTIVE = 0,
  ECALL_APP_CALL_STATUS_EMERGENCY,
  ECALL_APP_CALL_STATUS_PS,
  ECALL_APP_CALL_STATUS_NON_EMERGENCY_PS
}ecall_app_call_status_e_type;

typedef struct
{

  uint8  num_dial[PBM_NUM_SIZE];
  uint16 num_len;

  /*SDN numbers for test and reconfig call
    in eCall NORMAL mode. Last two entries */
}ecall_pbm_list_type;

 /* FDN numbers for test and reconfig call
    in eCall ONLY mode. First two entries */
static ecall_pbm_list_type ecall_fdn_list[ECALL_MAX_PBM_RECORDS];

 /* SDN numbers for test and reconfig call
    in eCall NORMAL mode. Last two entries */
static ecall_pbm_list_type ecall_sdn_list[ECALL_MAX_PBM_RECORDS];

/* Stores all the call status of each call ID */
static ecall_app_call_status_e_type ecall_app_call_status[ECALL_APP_CM_CALL_ID_MAX];

/* static memory for ecall_app_struct; */
ecall_app_struct_type ecall_app_struct;

/*==========================================================================

  FUNCTION ECALL_APP_PROCESS_UPDATE_CALL_STATUS

  DESCRIPTION
    This API will update the eCall App call status table.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
===========================================================================*/
void ecall_app_process_update_call_status
(
  cm_call_event_e_type           call_event,
  cm_call_id_type                call_id,
  cm_call_type_e_type            call_type
)
{
  ECALL_MSG_3( ECALL_MED, "Received call_id %d, call_event %d, call_type %d",
                           call_id,
                           call_event,
                           call_type );
  if (CM_CALL_EVENT_CONNECT == call_event)
  {
    if (call_type == CM_CALL_TYPE_EMERGENCY)
    {
      ecall_app_call_status[call_id] = ECALL_APP_CALL_STATUS_EMERGENCY;
    }
    else if ( (call_type == CM_CALL_TYPE_PS_DATA) || (call_type == CM_CALL_TYPE_PS_DATA_IS707B) )
    {
      ecall_app_call_status[call_id] = ECALL_APP_CALL_STATUS_PS;
    }
    else
    {
      ecall_app_call_status[call_id] = ECALL_APP_CALL_STATUS_NON_EMERGENCY_PS;
    }
  }
  else if (CM_CALL_EVENT_END == call_event)
  {
    ecall_app_call_status[call_id] = ECALL_APP_CALL_STATUS_INACTIVE;
  }
  else
  {
    ECALL_MSG_1( ECALL_ERROR, "Unsupported call_event %d to update call_status table",
                               call_event );
  }
  ECALL_MSG_2( ECALL_MED, "Update call_status[%d] to status %d",
                           call_id,
                           ecall_app_call_status[call_id] );

} /* ECALL_APP_PROCESS_UPDATE_CALL_STATUS */

/*==========================================================================

  FUNCTION ECALL_APP_IS_EMERGENCY_CALL_ON_GOING

  DESCRIPTION
    This API will check is there any on-going emergency call.

  DEPENDENCIES
    None.

  RETURN VALUE
    TRUE  - One or more on-going emergency call(s)
    FALSE - No on-going emergency call

  SIDE EFFECTS
===========================================================================*/
boolean ecall_app_is_emergency_call_on_going
(
  void
)
{
  boolean ret = FALSE;
  uint16 i;
  
  for (i = 0; i < ECALL_APP_CM_CALL_ID_MAX; i++)
  {
    if (ecall_app_call_status[i] == ECALL_APP_CALL_STATUS_EMERGENCY)
    {
      ECALL_MSG_1( ECALL_HIGH, "call_id %d is an on-going emergency call",
                                i );
      ret = TRUE;
    }
  }
  return ret;
} /* ECALL_APP_IS_EMERGENCY_CALL_ON_GOING */

/*==========================================================================

  FUNCTION ECALL_APP_DUMP_NON_EMERGENCY_PS_CALL

  DESCRIPTION
    This API will check is there any on-going non-emergency and non-PS calls.
    If yes, eCall App will call cm_mm_call_cmd_end() to dump them.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
===========================================================================*/
void ecall_app_dump_non_emergency_ps_call
(
  void
)
{
  uint16 call_id;
  boolean ret = FALSE;
  cm_end_params_s_type  end_params[ECALL_NUM_OF_CALL_ID];

  for ( call_id = 0; call_id < ECALL_APP_CM_CALL_ID_MAX; call_id++ )
  {
    if ( ECALL_APP_CALL_STATUS_NON_EMERGENCY_PS == ecall_app_call_status[call_id] )
    {
      ECALL_MSG_1( ECALL_HIGH, "call_id %d is a non-emergency or non-PS call. eCall App ends it",
                                call_id );

      /* Terminate current eCall session if this non-emergency call is with eCall App */
      if ( ecall_get_call_id() == call_id )
      {
        ECALL_MSG_1( ECALL_HIGH, "call_id %d is an MO/MT eCall. De-register the client and abort Modem activity",
                                  call_id );
        ecall_app_task_deregister_modem_client_if_registered();
        ecall_abort_all();
      }

      /* End the non-emergency call */
      end_params[0].call_id = call_id;
      ret = cm_mm_call_cmd_end( NULL, NULL, ecall_app_struct.ecall_app_cm_client_id,
                                ECALL_NUM_OF_CALL_ID, end_params );

      if ( TRUE != ret )
      {
        ECALL_MSG_1( ECALL_ERROR, "CM has no buffer to send end call cmd for call_id %d",
                                   call_id );
      }
    }
  }

} /* ECALL_APP_DUMP_NON_EMERGENCY_PS_CALL */

/*==========================================================================

  FUNCTION ECALL_APP_PROCESS_SETUP_CALL

  DESCRIPTION
    This function will call CM API to setup the call.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_process_setup_call
(
  cm_call_id_type call_id
)
{
  boolean ret;
  cm_setup_res_params_s_type setup_params;
  

  memset ( &setup_params, 0, sizeof(setup_params) );
  setup_params.accept = TRUE;

#if ( !(defined FEATURE_ECALL_LCU45) && (defined FEATURE_ECALL_HAS_V1_HEADER) ) /* SCMM */
  /* For SCMM, AMR-WB is supported for regular voice call, but not supported for vocoder API *
   * with eCall transmission. We'll need to disable AMR-WB for eCall in SCMM                 */
  setup_params.is_ecall = TRUE;
#endif /* SCMM */

  ret = cm_mm_call_cmd_setup_res( NULL,
                                  NULL,
                                  ecall_app_struct.ecall_app_cm_client_id,
                                  call_id,
                                  &setup_params );
  if ( FALSE == ret )
  {
    ECALL_MSG_0( ECALL_ERROR, "No buffer to send this setup call response to CM" );
  }

} /* ECALL_APP_PROCESS_SETUP_CALL */

/*===========================================================================

FUNCTION REPORT_ECALL_EVENT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void report_ecall_event(event_id_enum_type event_id)
{
  byte event_payload[4];

  switch (event_id)
  {
    case EVENT_ECALL_START:
      event_payload[0] = (uint8) ecall_app_struct.ecall_config_params.type_of_call;
      /* 1 = Emergency call,        0 = Test call */

      event_payload[1] = (uint8) ecall_app_struct.ecall_config_params.efs_params.canned_msd;
      /* 1 = Send Canned MSD,       0 = Send Real MSD */

      event_payload[2] = (uint8) ecall_app_struct.ecall_config_params.activation_type;
      /* 1 = Auto initiated ecall , 0 = Manual initiated ecall */

      event_report_payload(EVENT_ECALL_START, 3, (void *)event_payload);

      break;

    case EVENT_ECALL_STOP:
      event_payload[0] = 0;  /* Manual call end */
      event_report_payload(EVENT_ECALL_STOP, 1, (void *)event_payload);
      break;

    case EVENT_ECALL_SESSION_START:
      event_payload[0] = 0;  /* Reserved */
      event_report_payload(EVENT_ECALL_SESSION_START , 1, (void *)event_payload);
      break;

    case EVENT_ECALL_SESSION_FAILURE:
      event_payload[0] = 1;  /* Session Timeout */
      event_report_payload(EVENT_ECALL_SESSION_FAILURE , 1, (void *)event_payload);
      break;

    default:
      ECALL_MSG_1( ECALL_ERROR, "Unknown ECALL event reported %d",
                                 event_id );
      break;
  }

} /* REPORT_ECALL_EVENT */

/*==========================================================================
  FUNCTION ECALL_END_CALL

  DESCRIPTION
    this function will END an current call.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None
===========================================================================*/
boolean ecall_end_call
(
  void
)
{
  boolean ecall_status = FALSE;
  cm_end_params_s_type  end_params[ECALL_NUM_OF_CALL_ID];

  end_params[0].call_id = ecall_get_call_id();

  ecall_status = cm_mm_call_cmd_end( NULL, NULL, ecall_app_struct.ecall_app_cm_client_id,
                                          ECALL_NUM_OF_CALL_ID, end_params );

  return ecall_status;

} /* ECALL_END_CALL */

/*==========================================================================

  FUNCTION ECALL_ORIGINATE_CALL

  DESCRIPTION

  DEPENDENCIES
    None.

  RETURN VALUE
    Boolean - 1 = Call Originated Successfully
              0 = Call Origination Failed

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_originate_call()
{
   /*lint -esym(603,alpha_buf)*/
  cm_num_s_type               calling_num;
  cm_orig_alpha_s_type        alpha_buf;
  cm_call_id_type             call_id = (uint8)-1;

  /* TODO: orig preference or w/e when CDMA & GW are supported! */
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
  cm_gw_cs_orig_params_s_type gw_orig_params;
  #endif /* FEATURE_GSM or FEATURE_WCDMA */

  cm_num_s_type       dial_num_buf;
  cm_call_type_e_type type_of_call = CM_CALL_TYPE_NONE;
  uint16 j =0;
  boolean retVal = FALSE;

  uint8 ecall_temp_buf[] = {
    0x31, 0x31, 0x32 /* 112 */
  };

  uint8 digits_len = 0;
  ecall_operating_mode_type ecall_operating_mode = ECALL_MODE_UNKNOWN;
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  /* Set up dialing number */
  cm_num_init( &calling_num );
  if ( TRUE == ecall_app_task_get_is_era_glonass_enabled() ) /* ERA-GLONASS eCall dialing number */
  {
    switch(ecall_app_struct.ecall_config_params.type_of_call)
    {
      case ECALL_EMERGENCY:
        ECALL_MSG_0( ECALL_HIGH, "ORIGINATE EMERGENCY eCALL, Dialing 112 " );
        cm_num_fill(&dial_num_buf, ecall_temp_buf, (uint8 )sizeof(ecall_temp_buf), CM_DIGIT_MODE_4BIT_DTMF);
        type_of_call = CM_CALL_TYPE_EMERGENCY; /* Set to Emergency Call */
        break;

      case ECALL_TEST:
      case ECALL_RECONFIG:
        /* For ECALL_TEST or ECALL_RECONFIG in ERA-GLONASS mode, dial DIGITS in the EFS configuration */
        for (j = 0; j < ECALL_DIGITS_LEN_MAX; j++)
        {
          if (ecall_app_struct.ecall_config_params.efs_params.digits[j] != '\0')
          {
            digits_len++;
          }
        }
        ECALL_MSG_1( ECALL_HIGH, "ECALL_TEST or RECALL_RECONFIG, Dial EFS number...len %d",
                                  digits_len );
        for (j = 0; j < digits_len; j++)
        {
          ECALL_MSG_2( ECALL_HIGH, "num[%d] - %x",
                                    j,
                                    ecall_app_struct.ecall_config_params.efs_params.digits[j] );
        }
        cm_num_fill(&dial_num_buf, ecall_app_struct.ecall_config_params.efs_params.digits,
                    digits_len,CM_DIGIT_MODE_4BIT_DTMF);
        type_of_call = CM_CALL_TYPE_VOICE;
        break;

      default:
          ECALL_MSG_1( ECALL_ERROR, "unknown type of Call %d",
                                     ecall_app_struct.ecall_config_params.type_of_call );
          return FALSE;
    }

  } /* ERA-GLONASS eCall dialing number */
  else /* Pan-EU eCall dialing number */
  {
    if (ecall_app_struct.ecall_config_params.efs_params.ecall_dial_num == ECALL_DIAL_NORMAL)
    {
      ecall_operating_mode = ecall_get_ecall_operating_mode();

      switch(ecall_app_struct.ecall_config_params.type_of_call)
      {
        case ECALL_EMERGENCY:
          ECALL_MSG_0( ECALL_HIGH, "ORIGINATE EMERGENCY eCALL, Dialing 112 " );
          cm_num_fill(&dial_num_buf, ecall_temp_buf, (uint8 )sizeof(ecall_temp_buf), CM_DIGIT_MODE_4BIT_DTMF);
          type_of_call = CM_CALL_TYPE_EMERGENCY; /* Set to Emergency Call */
          break;

        case ECALL_TEST:
          ECALL_MSG_0( ECALL_HIGH, "ORIGINATE TEST eCALL " );
          if(ecall_operating_mode == ECALL_MODE_ECALL_ONLY )
          {
            /* FDN Test number */
            ECALL_MSG_1( ECALL_HIGH, "ECALL_MODE_ECALL_ONLY, Dialing FDN number...len %d ",
                                      ecall_fdn_list[0].num_len );
            for (j = 0; j < ecall_fdn_list[0].num_len ; j++)
            {
              ECALL_MSG_2( ECALL_HIGH, "num[%d] - %x",
                                        j,
                                        ecall_fdn_list[0].num_dial[j] );
            }
            cm_num_fill(&dial_num_buf, ecall_fdn_list[0].num_dial, (uint8 )ecall_fdn_list[0].num_len, CM_DIGIT_MODE_4BIT_DTMF);
          }
          else if (ecall_operating_mode == ECALL_MODE_NORMAL )
          {
            /* SDN Test number */
            ECALL_MSG_1( ECALL_HIGH, "ECALL_MODE_NORMAL, Dialing SDN number...len %d",
                                      ecall_sdn_list[0].num_len );
            for (j = 0; j < ecall_sdn_list[0].num_len ; j++)
            {
              ECALL_MSG_2( ECALL_HIGH, "num[%d] - %x",
                                        j,
                                        ecall_sdn_list[0].num_dial[j] );
            }
            cm_num_fill(&dial_num_buf, ecall_sdn_list[0].num_dial, (uint8 )ecall_sdn_list[0].num_len, CM_DIGIT_MODE_4BIT_DTMF);
          }
          type_of_call = CM_CALL_TYPE_VOICE; /* Set to Regular Voice Call */
          break;

        case ECALL_RECONFIG:
          ECALL_MSG_0( ECALL_HIGH, "ORIGINATE Reconfiguration eCALL " );
          if(ecall_operating_mode == ECALL_MODE_ECALL_ONLY )
          {
            /* FDN Test number */
            ECALL_MSG_1( ECALL_HIGH, "ECALL_MODE_ECALL_ONLY, Dialing FDN number...len %d ",
                                      ecall_fdn_list[1].num_len );
            for (j = 0; j < ecall_fdn_list[1].num_len ; j++)
            {
              ECALL_MSG_2( ECALL_HIGH, "num[%d] - %x",
                                        j,
                                        ecall_fdn_list[1].num_dial[j] );
            }
            cm_num_fill(&dial_num_buf, ecall_fdn_list[1].num_dial, (uint8 )ecall_fdn_list[1].num_len, CM_DIGIT_MODE_4BIT_DTMF);
          }
          else if (ecall_operating_mode == ECALL_MODE_NORMAL )
          {
            /* SDN Test number */
            ECALL_MSG_1( ECALL_HIGH, "ECALL_MODE_NORMAL, Dial SDN number...len %d",
                                      ecall_sdn_list[1].num_len );
            for (j = 0; j < ecall_sdn_list[1].num_len ; j++)
            {
              ECALL_MSG_2( ECALL_HIGH, "num[%d] - %x",
                                        j,
                                        ecall_sdn_list[1].num_dial[j] );
            }
            cm_num_fill(&dial_num_buf, ecall_sdn_list[1].num_dial, (uint8 )ecall_sdn_list[1].num_len, CM_DIGIT_MODE_4BIT_DTMF);
          }
          type_of_call = CM_CALL_TYPE_VOICE;
          break;

        default:
          ECALL_MSG_1( ECALL_ERROR, "unknown type of Call %d",
                                     ecall_app_struct.ecall_config_params.type_of_call );
          return FALSE;
      } /*end of switch case*/
    } /* ecall_dial_num == ECALL_DIAL_NORMAL */
    else if ( ecall_app_struct.ecall_config_params.efs_params.ecall_dial_num == ECALL_DIAL_OVERRIDE )
    {
      /* For ECALL_DIAL_OVERRIDE, dial DIGITS in the EFS configuration */
      for (j = 0; j < ECALL_DIGITS_LEN_MAX; j++)
      {
        if (ecall_app_struct.ecall_config_params.efs_params.digits[j] != '\0')
        {
          digits_len++;
        }
      }
      ECALL_MSG_1( ECALL_HIGH, "ECALL_DIAL_OVERRIDE, Dial EFS number...len %d",
                                digits_len );
      for (j = 0; j < digits_len; j++)
      {
        ECALL_MSG_2( ECALL_HIGH, "num[%d] - %x",
                                  j,
                                  ecall_app_struct.ecall_config_params.efs_params.digits[j] );
      }
      cm_num_fill(&dial_num_buf, ecall_app_struct.ecall_config_params.efs_params.digits,
                  digits_len,CM_DIGIT_MODE_4BIT_DTMF);
      type_of_call = CM_CALL_TYPE_VOICE;
    } /* ecall_dial_num = ECALL_DIAL_OVERRIDE */
  } /* Pan-EU eCall dialing number */

  dial_num_buf.emerg_srv_categ = (ecall_app_struct.ecall_config_params.activation_type == ECALL_MANUAL_INITIATED) ?
                                  ECALL_MANUAL_EMERG_SRV_CATEG : ECALL_AUTO_EMERG_SRV_CATEG;

  ecall_app_struct.ecall_app_cm_client_id = ecall_get_cm_client_id();
  ECALL_MSG_1( ECALL_HIGH, "ecall_app_struct.ecall_app_cm_client_id = %d",
                            ecall_app_struct.ecall_app_cm_client_id );

#ifdef FEATURE_DUAL_SIM
  {
    ecall_prov_app_info_s_type *appInfo_ptr = NULL;
    mmgsdi_slot_id_enum_type    slot_id = (mmgsdi_slot_id_enum_type)0;
    sys_modem_as_id_e_type      as_id = SYS_MODEM_AS_ID_1;
    slot_id = ecall_app_task_get_ecall_usim_slot_id();

    if ((mmgsdi_slot_id_enum_type)0 == slot_id)
    {
      ECALL_MSG_1( ECALL_ERROR, "ecall_originate_call(): Invalid eCall usim slot_id %d",
                                 slot_id );
      goto Done;
    }

    appInfo_ptr = ecall_sim_get_appinfo_by_slot_id(slot_id);
    if (NULL == appInfo_ptr)
    {
      ECALL_MSG_0( ECALL_ERROR, "ecall_originate_call(): appInfo_ptr is NULL" );
      goto Done;
    }

    if (!ecall_app_task_is_asid_valid(appInfo_ptr->as_id))
    {
      ECALL_MSG_1( ECALL_ERROR, "ecall_originate_call(): invalid as_id %d",
                                 appInfo_ptr->as_id);
      goto Done;
    }
    else
    {
      /* Valid eCall as_id obtained from appInfo_ptr */
      as_id = appInfo_ptr->as_id;
    }

Done:
    ECALL_MSG_2( ECALL_HIGH, "Originating eCall with slot_id %d, as_id %d",
                              slot_id,
                              as_id );

	/* Use default call origination parameters */
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
    cm_util_default_gw_orig_params_per_sub(&gw_orig_params, as_id);
  #if ( !(defined FEATURE_ECALL_LCU45) && (defined FEATURE_ECALL_HAS_V1_HEADER) ) /* SCMM */
    /* For SCMM, AMR-WB is supported for regular voice call, but not supported for vocoder API *
     * with eCall transmission. We'll need to disable AMR-WB for eCall in SCMM                 */
    gw_orig_params.is_ecall = TRUE;
    gw_orig_params.bearer_capability_1.gsm_speech_ver_pref_0 = GSM_INVALID_SPEECH_VER;
  #endif /* SCMM */
    #endif /* FEATURE_GSM or FEATURE_WCDMA */

  #ifdef FEATURE_ECALL_HAS_ERA_GLONASS
    {
      cm_cc_generic_params_s_type cc_generic_params;
      memset(&cc_generic_params, 0, sizeof(cc_generic_params));
      if ( (ECALL_TEST == ecall_app_struct.ecall_config_params.type_of_call) ||
           (ECALL_RECONFIG == ecall_app_struct.ecall_config_params.type_of_call) )
      {
        /* For ERA-GLONASS Test or Reconfig eCall, eCall App needs to inform CM to abort *
         * FDN check since the number will be stored in EFS, not the FDN list.           */
        cc_generic_params.is_fdn_to_check = FALSE;
      }
      else
      {
        cc_generic_params.is_fdn_to_check = TRUE;
      }
    
      retVal = cm_mm_call_cmd_orig_exten5_per_subs(
                                   ecall_app_call_cmd_exten_cb_func,
                                   NULL,
                                   ecall_app_struct.ecall_app_cm_client_id,
                                   type_of_call,
                                   CM_SRV_TYPE_GSM_WCDMA,
                                   &calling_num,
                                   &dial_num_buf,
                                   &alpha_buf,
                                   NULL,
                                 #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
                                   &gw_orig_params,
                                 #else
                                   NULL,
                                 #endif
                                   NULL,
                                   NULL,
                                   CM_CALL_ID_INVALID,
                                   &cc_generic_params,
                                   CM_ORIG_CLIENT_CATEG_USER,
                                   TRUE,
                                   as_id,
                                   &call_id );
    }
  #else /* EU eCall */
    retVal = cm_mm_call_cmd_orig_cc_per_subs(
                               ecall_app_call_cmd_exten_cb_func,
                               NULL,
                               ecall_app_struct.ecall_app_cm_client_id,
                               type_of_call,
                               CM_SRV_TYPE_GSM_WCDMA,
                               &calling_num,
                               &dial_num_buf,
                               &alpha_buf,
                               NULL,
                             #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
                               &gw_orig_params,
                             #else
                               NULL,
                             #endif
                               NULL,
                               CM_CALL_ID_INVALID,
                               CM_ORIG_CLIENT_CATEG_USER,
                               as_id,
                               &call_id );
  #endif /* FEATURE_ECALL_HAS_ERA_GLONASS */

    if (retVal == FALSE)
    {
      ECALL_MSG_0( ECALL_ERROR, " ECALL ecall_originate_call returned FALSE " );
    }

    ecall_app_task_start_session_status_update(
                      ECALL_PASS,
                      call_id,
                      ecall_app_struct.ecall_app_cm_client_id,
                      as_id,
                      NULL );
  }
#else /* Single SIM case */
  {

    /* Use default call origination parameters */
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
    cm_util_default_gw_orig_params(&gw_orig_params);
  #if ( !(defined FEATURE_ECALL_LCU45) && (defined FEATURE_ECALL_HAS_V1_HEADER) ) /* SCMM */
    /* For SCMM, AMR-WB is supported for regular voice call, but not supported for vocoder API *
     * with eCall transmission. We'll need to disable AMR-WB for eCall in SCMM                 */
    gw_orig_params.is_ecall = TRUE;
    gw_orig_params.bearer_capability_1.gsm_speech_ver_pref_0 = GSM_INVALID_SPEECH_VER;
  #endif /* SCMM */
    #endif /* FEATURE_GSM or FEATURE_WCDMA */
  
  #ifdef FEATURE_ECALL_HAS_ERA_GLONASS
    {
      cm_cc_generic_params_s_type cc_generic_params;
      memset(&cc_generic_params, 0, sizeof(cc_generic_params));
      if ( (ECALL_TEST == ecall_app_struct.ecall_config_params.type_of_call) ||
           (ECALL_RECONFIG == ecall_app_struct.ecall_config_params.type_of_call) )
      {
        /* For ERA-GLONASS Test or Reconfig eCall, eCall App needs to inform CM to abort *
         * FDN check since the number will be stored in EFS, not the FDN list.           */
        cc_generic_params.is_fdn_to_check = FALSE;
      }
      else
      {
        cc_generic_params.is_fdn_to_check = TRUE;
      }

      retVal = cm_mm_call_cmd_orig_exten2(
                              ecall_app_call_cmd_cb_func,
                              NULL,
                              ecall_app_struct.ecall_app_cm_client_id,
                              type_of_call,
                              CM_SRV_TYPE_GSM_WCDMA,
                              &calling_num,
                              &dial_num_buf,
                              &alpha_buf,
                              NULL,
                            #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
                              &gw_orig_params,
                            #else
                              NULL,
                            #endif
                              NULL,
                              CM_CALL_ID_INVALID,
                              &cc_generic_params,
                              &call_id);
    }
  #else /* EU eCall */
    retVal = cm_mm_call_cmd_orig(ecall_app_call_cmd_cb_func,
                                 NULL,
                                 ecall_app_struct.ecall_app_cm_client_id,
                                 type_of_call,
                                 CM_SRV_TYPE_GSM_WCDMA,
                                 &calling_num,
                                 &dial_num_buf,
                                 &alpha_buf,
                                 NULL,
                               #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
                                 &gw_orig_params,
                               #else
                                 NULL,
                               #endif
                                 NULL,
                                 &call_id);
  #endif /* FEATURE_ECALL_HAS_ERA_GLONASS */

    if (retVal == FALSE)
    {
      ECALL_MSG_0( ECALL_ERROR, " ECALL ecall_originate_call returned FALSE " );
    }

    ecall_app_task_start_session_status_update(
                        ECALL_PASS,
                        call_id,
                        ecall_app_struct.ecall_app_cm_client_id,
                        SYS_MODEM_AS_ID_1,
                        NULL );
  }
#endif /* FEATURE_DUAL_SIM */

  ECALL_MSG_1( ECALL_MED, "ecall call_id = %d",
                           call_id );
  /* Queue up a command to the ecall_app_task to update call_id */
  cmd_ptr = ecall_app_task_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    cmd_ptr->cmd_id = ECALL_APP_CMD_UPDATE_ECALL_CALL_ID;
    cmd_ptr->data.call_id = call_id;
    ecall_app_task_put_cmd(cmd_ptr);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_originate_call()" );
    return FALSE;
  }

  return retVal;
}
/*lint +esym(603,alpha_buf)*/
/*===========================================================================

FUNCTION ECALL_APP_PBM_READ_RECORD

DESCRIPTION
  This function is used to read record from pbm.

DEPENDENCIES
  None

RETURN VALUE
  length of the phone number for the record read

SIDE EFFECTS
  None
===========================================================================*/
uint16 ecall_app_pbm_read_record
(
  pbm_record_id_type   pbm_record_id,
  pbm_record_s_type   *record_ptr,
  pbm_return_type     *pbm_return_ptr
)
{
  uint8             fields_buf[ECALL_MAX_REC_LEN] ={0};
  uint32            fields_ptr_size;
  uint16            category;
  pbm_field_s_type *fields_ptr;
  int               num_fields = 0;
  uint16            i,j;
  uint16            len_num_in_record=0;

  fields_ptr_size = (uint32)pbm_session_calculate_fields_size_from_id(pbm_record_id);

  fields_ptr = (pbm_field_s_type *)fields_buf;
  memset(fields_ptr, 0, (uint32)ECALL_MAX_REC_LEN);
  memset(record_ptr, 0, sizeof(pbm_record_s_type));
  if(fields_ptr_size == 0)
  {
    *pbm_return_ptr = pbm_session_record_read( pbm_record_id,
                                               &category,
                                               &num_fields,
                                               NULL,
                                               fields_ptr_size );
  }
  else
  {
    *pbm_return_ptr = pbm_session_record_read( pbm_record_id,
                                               &category,
                                               &num_fields,
                                               fields_buf,
                                               fields_ptr_size );

  }

  if(*pbm_return_ptr == PBM_SUCCESS)
  {
    /* Fill in the record with default values. */
    record_ptr->index = pbm_session_record_id_to_location(pbm_record_id);

    ECALL_MSG_3( ECALL_MED, "field_ptr_size %d of pbm_record_id %d num_fields",
                             fields_ptr_size,
                             pbm_record_id,
                             num_fields );
    /* Process the information from the buffer */
    for ( i = 0; (i < num_fields) &&
                 ( (i+1) < (ECALL_MAX_REC_LEN/sizeof(pbm_field_s_type) - 1) ) ; i++)
    {
      switch (fields_ptr[i].field_id)
      {
        case PBM_FIELD_HARDCODED_ECC:
        case PBM_FIELD_PHONE_GENERIC:
          ecall_memscpy ( record_ptr->number, sizeof(record_ptr->number), 
                          (const void *)pbm_get_field_data_ptr(fields_ptr,i), fields_ptr[i].data_len );
          len_num_in_record = fields_ptr[i].data_len-1;
          ECALL_MSG_2( ECALL_HIGH, "Num_len - %d index %d",
                                    len_num_in_record,
                                    record_ptr->index );
          for(j=0;j<fields_ptr[i].data_len-1;j++)
          {
            ECALL_MSG_2( ECALL_HIGH, "num[%d] - %x",
                                      j,
                                      record_ptr->number[j] );
          }
          if((i+1 < num_fields )&&
             ((fields_ptr[i+1].field_id ==  PBM_FIELD_GSM_TONNPI)||
             (fields_ptr[i+1].field_id ==  PBM_FIELD_NUMBER_TYPE))
            )
          {
            i++;
            ecall_memscpy ( &(record_ptr->type), sizeof(record_ptr->type),
                            (const void *)pbm_get_field_data_ptr(fields_ptr,(i)), fields_ptr[i].data_len );
          }
          break;
        case PBM_FIELD_NAME:
          ecall_memscpy ( record_ptr->text,
                          sizeof(record_ptr->text),
                          (const void *)pbm_get_field_data_ptr(fields_ptr,i),
                          fields_ptr[i].data_len );
          break;
        default:
          /* Rest of the fields are ignored, since not required for CPBR */
          break;
      }
    } /* end of for loop. each field in the record */
  }

  return len_num_in_record;
} /* ecall_app_pbm_read_record */

/*==========================================================================
  FUNCTION      ECALL_APP_PROCESS_FDN_SIM_FILE_INFO

  DESCRIPTION
  This function reads the test and reconfiguration numbers
  programmeed in the FDN of the USIM and stores in app global structure

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void ecall_app_process_fdn_sim_file_info
(
  pbm_phonebook_type pb_id,
  pbm_extended_fileinfo_s_type info
)
{
  uint8 index = 0;
  uint8 used_read = 0;
  int16 entry=0,len_num=0;
  pbm_record_s_type     record;
  pbm_return_T          pbm_return;
  pbm_record_id_type    pbm_record_id = 0;

  /* reset the stored fdn entries */
  for(index=0;index < ECALL_MAX_PBM_RECORDS; index++)
  {
    memset(ecall_fdn_list[index].num_dial,0,PBM_NUM_SIZE);
    ecall_fdn_list[index].num_len = 0;
  }

  /* For each index from current to last perform pbm_record_read */
  for(index=1; index <= info.number_of_records &&
               used_read < info.records_used; index++ )
  {
    pbm_record_id = pbm_session_location_to_record_id(pb_id, index);

    len_num = ecall_app_pbm_read_record(pbm_record_id, &record, &pbm_return);

    if(pbm_return == PBM_SUCCESS )
    {
      used_read++;
      if(entry < ECALL_MAX_PBM_RECORDS)
      {
        ecall_memscpy ( ecall_fdn_list[entry].num_dial, sizeof(ecall_fdn_list[entry].num_dial),
                        record.number, sizeof(record.number) );
        ecall_fdn_list[entry].num_len= len_num;
        entry++;
      }
    }
    else if(pbm_return != PBM_ERROR_NOT_FOUND)
    {
      ECALL_MSG_2( ECALL_ERROR, "retrieving record error %d index %d",
                                 pbm_return,
                                 index );
    }
   /* continue reading next index */
  }/* for(index )*/
}

/*==========================================================================
  FUNCTION      ECALL_APP_PROCESS_SDN_SIM_FILE_INFO

  DESCRIPTION
  This function reads the test and reconfiguration numbers in
  programmeed in the SDN of the USIM and stores in app global structure

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None
============================================================================*/
void ecall_app_process_sdn_sim_file_info
(
  pbm_phonebook_type pb_id,
  pbm_extended_fileinfo_s_type info
)
{
  uint8               index = 0;
  uint8               used_read = 0;
  int16               entry=0,len_num=0;
  pbm_record_s_type   record;
  pbm_return_T        pbm_return;
  pbm_record_id_type  pbm_record_id;

  /* reset the stored sdn entries */
  for(index=0;index < ECALL_MAX_PBM_RECORDS; index++)
  {
    memset(ecall_sdn_list[index].num_dial,0,PBM_NUM_SIZE);
    ecall_sdn_list[index].num_len = 0;
  }

  /* read the SDN Phone book in reverse as we need last two entries*/
  entry = 2; /* store in reverse order into global structure */
  index = info.number_of_records;

  /* For each index from last to first perform pbm_record_read.*/
  for( ;index >= 1 &&
        used_read < info.records_used; index--)
  {
    pbm_record_id = pbm_session_location_to_record_id(pb_id, index);

    len_num = ecall_app_pbm_read_record(pbm_record_id, &record, &pbm_return);

    if(pbm_return == PBM_SUCCESS )
    {
      used_read++;
      if(entry > 0)
      {
        entry--;
        ecall_memscpy ( ecall_sdn_list[entry].num_dial, sizeof(ecall_sdn_list[entry].num_dial),
                        record.number, sizeof(record.number) );
        ecall_sdn_list[entry].num_len = len_num;
      }
    }
    else if(pbm_return != PBM_ERROR_NOT_FOUND)
    {
      ECALL_MSG_2( ECALL_HIGH, "retrieving record error %d index %d",
                                pbm_return,
                                index );
    }
   /* continue reading next index */
  }/* for(index )*/
}

/*==========================================================================
  FUNCTION   ECALL_CHECK_PB_VALID

  DESCRIPTION
    validity of Phonebooks might change based on the inserted card type .
    if the inserted card is 2G -
       then FDN & SDN would come from DF-Telecom (PBM_GPB)
    if the inserted card is 3G -
       then FDN & SDN would come from ADF-USIM/CSIM (PBM_LPB)
    Hence this function would check whether a particular phonebook is valid or not
    based on the inserted card type.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None
===========================================================================*/
boolean ecall_check_pb_valid
(
  pbm_phonebook_type pb_id
)
{
  mmgsdi_session_info_query_type   session_info_query;
  mmgsdi_session_get_info_type     session_get_info;
  
  boolean pb_valid = TRUE;

  /* Initialize session_info_query */
  memset(&session_info_query, 0, sizeof(session_info_query));

  if (pb_id.pb_category == PBM_GPB)
  {
    ECALL_MSG_0( ECALL_HIGH, "check_pb_valid(): pb_category PBM_GPB" );
    switch (pb_id.device_type)
    {
      case PBM_FDN:
      case PBM_SDN:
        {
        #ifdef FEATURE_DUAL_SIM
          {
            mmgsdi_slot_id_enum_type     slot_id = (mmgsdi_slot_id_enum_type)0;
            ecall_prov_app_info_s_type  *appInfo_ptr = NULL;
            slot_id = ecall_app_task_get_ecall_usim_slot_id();
            if ((mmgsdi_slot_id_enum_type)0 == slot_id)
            {
              ECALL_MSG_1( ECALL_ERROR, "check_pb_valid(): Invalid eCall slot_id %d",
                                         slot_id );
              pb_valid = FALSE;
              break;
            }

            appInfo_ptr = ecall_sim_get_appinfo_by_slot_id(slot_id);
            if (NULL == appInfo_ptr)
            {
              ECALL_MSG_0( ECALL_ERROR, "check_pb_valid(): Null appInfo_ptr" );
              pb_valid = FALSE;
              break;
            }
            ECALL_MSG_2( ECALL_HIGH, "check_pb_valid(): Query for slot_id %d, session_type %d",
                                      slot_id,
                                      appInfo_ptr->session_type );
            session_info_query.query_key.session_type = appInfo_ptr->session_type;
            session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
            if (mmgsdi_session_get_info_sync(session_info_query,
                                             &session_get_info) == MMGSDI_SUCCESS)
            {
              if (session_get_info.app_data.app_type == MMGSDI_APP_SIM)
              {
                ECALL_MSG_1( ECALL_HIGH, "check_pb_valid(): session_type %d with MMGSDI_APP_SIM",
                                          appInfo_ptr->session_type );
                break;
              }
            }
          }
        #else
          /* Single SIM case */
          session_info_query.query_key.session_type = MMGSDI_GW_PROV_PRI_SESSION;
          session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
          
          if (mmgsdi_session_get_info_sync(session_info_query,
                                        &session_get_info) == MMGSDI_SUCCESS)
          {
            if (session_get_info.app_data.app_type == MMGSDI_APP_SIM)
            {
              ECALL_MSG_0( ECALL_HIGH, "check_pb_valid(): GW_PROV_PRI_SESSION with MMGSDI_APP_SIM" );
              break;
            }
          }
          session_info_query.query_key.session_type = MMGSDI_1X_PROV_PRI_SESSION;
          if (mmgsdi_session_get_info_sync(session_info_query,
                                        &session_get_info) == MMGSDI_SUCCESS)
          {
            if (session_get_info.app_data.app_type == MMGSDI_APP_RUIM)
            {
              ECALL_MSG_0( ECALL_HIGH, "check_pb_valid(): GW_1X_PROV_PRI_SESSION with MMGSDI_APP_RUIM" );
              break;
            }
          }
        #endif /* FEATURE_DUAL_SIM */
          //if we come here that means
          //this is 3G card and hence FDN and SDN should come from
          //provisioning sessions .
          //Hence this EVENT can be ignored by ecall
          //and wait for provision session events .
          ECALL_MSG_0( ECALL_HIGH, "check_pb_valid(): 3G Card, FDN & SDN should come from provisioning sessions" );
          pb_valid = FALSE;
        }
        break;

      default:
      ECALL_MSG_1( ECALL_HIGH, "check_pb_valid(): Invalid device_type: %d ",
                                pb_id.device_type );
      pb_valid = FALSE;
      break;
    }
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "check_pb_valid(): pb_category PBM_LPB" );
    switch (pb_id.device_type)
    {
      case PBM_FDN:
      case PBM_SDN:
        ECALL_MSG_0( ECALL_HIGH, "check_pb_valid(): device_type PBM_FDN or PBM_SDN" );
        break;

      default:
        ECALL_MSG_1( ECALL_HIGH, "check_pb_valid(): Invalid device_type: %d ",
                                  pb_id.device_type );
        pb_valid = FALSE;
        break;
    }
  }
  return pb_valid;

} /* ECALL_CHECK_PB_VALID */

/*==========================================================================
  FUNCTION   ECALL_APP_PROCESS_PBM_NOTIFY_UPDATE

  DESCRIPTION
    When PB is ready or if entries are added/deleted/updated/refresh,
    ecall app would update it's global variable for FDN/SDN device

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None
===========================================================================*/
void ecall_app_process_pbm_notify_update
(
  pbm_phonebook_type pb_id
)
{
  pbm_extended_fileinfo_s_type file_info;
  ecall_app_task_cmd_type *cmd_ptr = NULL;
  pbm_return_type ret;

  ECALL_MSG_4( ECALL_HIGH, "process_pbm_notify_update(): category %d, prov_type %d, slot_id %d, device_type %d",
                            pb_id.pb_category,
                            pb_id.prov_type,
                            pb_id.slot_id,
                            pb_id.device_type );

  if(ecall_check_pb_valid(pb_id) == TRUE)
  {
    ret = pbm_session_extended_file_info(pb_id, &file_info);

    if(ret != PBM_SUCCESS)
    {
      ECALL_MSG_2( ECALL_HIGH, "process_pbm_notify_update(): could not read file_info device_type %d error %d",
                                pb_id.device_type,
                                ret );
      return;
    }

    ECALL_MSG_3( ECALL_HIGH, "process_pbm_notify_update(): 3-PBM_FDN. 14-PBM_SDN dev %d num_of_records %d record_used %d",
                              pb_id.device_type,
                              file_info.number_of_records,
                              file_info.records_used );

    /* Post a command to the eCall App task to process this PBM SIM FILE */
    cmd_ptr = ecall_app_task_get_cmd_buf();
    if (cmd_ptr != NULL)
    {
      cmd_ptr->cmd_id = ECALL_APP_CMD_PBM_SIM_FILE_INFO;
      cmd_ptr->data.pbm.pb_id = pb_id;
      cmd_ptr->data.pbm.info = file_info;
      ecall_app_task_put_cmd(cmd_ptr);
    }
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "process_pbm_notify_update(): Failed at ecall_check_pb_valid()" );
    return;
  }
  
}


/*==========================================================================
  FUNCTION   ECALL_PBM_NOTIFY_EVENTS_CB

  DESCRIPTION
   callback function called from notify register. when PB is READY
   this function would post a command to get extended file info
   for FDN/SDN

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None
===========================================================================*/
void ecall_pbm_notify_events_cb
(
  void *user_data,
  pbm_notify_data_s_type *notify_data
)
{
  ecall_app_task_cmd_type    *cmd_ptr = NULL;
  pbm_device_type             device_type;

  device_type = notify_data->data.pb_id;

  /* we are interested in FDN or SDN entries */
  if(device_type == PBM_FDN || device_type == PBM_SDN)
  {
    ECALL_MSG_3( ECALL_HIGH, "PBM event for slot %d dev %d event %d",
                              notify_data->session_data.pb_id.slot_id,
                              device_type,
                              notify_data->event );

  #ifdef FEATURE_DUAL_SIM
    {
      /* Check if this PBM_FDN or PBM_SDN event belongs to eCall slot_id */
      ecall_prov_app_info_s_type *appInfo_ptr = NULL;
      mmgsdi_slot_id_enum_type    slot_id = (mmgsdi_slot_id_enum_type)0;
      slot_id = ecall_app_task_get_ecall_usim_slot_id();
      if ((mmgsdi_slot_id_enum_type)0 == slot_id)
      {
        ECALL_MSG_1( ECALL_ERROR, "pbm_notify_events_cb(): Invalid eCall usim slot_id %d",
                                   slot_id );
        return;
      }

      appInfo_ptr = ecall_sim_get_appinfo_by_slot_id(slot_id);
      if (NULL == appInfo_ptr)
      {
        ECALL_MSG_0( ECALL_ERROR, "pbm_notify_events_cb(): appInfo_ptr is NULL" );
        return;
      }

      if (notify_data->session_data.pb_id.slot_id != appInfo_ptr->slot_id)
      {
        ECALL_MSG_2( ECALL_HIGH, "pbm_notify_events_cb(): Not a eCall PBM event. eCall slot_id %d, PBM slot_id %d",
                                  appInfo_ptr->slot_id,
                                  notify_data->session_data.pb_id.slot_id );
        return;
      }
    }
  #endif /* FEATURE_DUAL_SIM */

    switch(notify_data->event)
    {
      case PBM_EVENT_PB_READY:
        ECALL_MSG_2( ECALL_HIGH, "PBM_EVENT_PB_READY device_type %d, prov_type %d",
                                  device_type,
                                  notify_data->session_data.pb_id.prov_type );
      case PBM_EVENT_REC_ADD:
      case PBM_EVENT_REC_UPDATE:
      case PBM_EVENT_REC_DELETE:
      case PBM_EVENT_PB_REFRESH_DONE:
        ECALL_MSG_0( ECALL_MED, "Update the ecall app stored table" );
        /* Post a command to the eCall App task */
        cmd_ptr = ecall_app_task_get_cmd_buf();
        if (cmd_ptr != NULL)
        {
          cmd_ptr->cmd_id = ECALL_APP_CMD_PBM_EVENT_UPDATE;
          cmd_ptr->data.pbm.pb_id = notify_data->session_data.pb_id;
          ecall_app_task_put_cmd(cmd_ptr);
        }
        break;

      /* refresh started */
      case PBM_EVENT_PB_REFRESH_START:
        ECALL_MSG_0( ECALL_MED, "PBM_EVENT_PB_REFRESH_START" );
        break;

      default:
        ECALL_MSG_1( ECALL_MED, "pbm event notification %d",
                                 notify_data->event );
        break;
    }
  }
}

/*==========================================================================
  FUNCTION   ECALL_APP_PBM_INIT

  DESCRIPTION
    Registers with pbm interface for notificaiton or events.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None
===========================================================================*/
void ecall_app_pbm_init
(
  void
)
{
  if(PBM_SUCCESS != pbm_notify_register(ecall_pbm_notify_events_cb, NULL))
  {
    ECALL_MSG_0( ECALL_ERROR, "registering for PBM notification failed" );
  }

} /* ECALL_APP_PBM_INIT */

/*==========================================================================
  FUNCTION   ECALL_APP_CALL_STATUS_INIT

  DESCRIPTION
    This API will initialize the eCall App call status table.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None
===========================================================================*/
void ecall_app_call_status_init
(
  void
)
{
  uint16 i;

  /* Initialize all ecall_app_call_status to ECALL_APP_CALL_STATUS_INACTIVE */
  for (i = 0; i < ECALL_APP_CM_CALL_ID_MAX; i++)
  {
    ecall_app_call_status[i] = ECALL_APP_CALL_STATUS_INACTIVE;
  }

} /* ECALL_APP_CALL_STATUS_INIT */

/*==========================================================================
  FUNCTION ECALL_CONSTRUCT_MSD

  DESCRIPTION
    This function Contrcuts the MSD (with/without GPS) and then originates an ecall
    It gets the ecall_cm_client_id that the ecall_app_task recevied thru cm_client_init()

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None
===========================================================================*/
void ecall_construct_msd
(
  ecall_msd_array_type ecall_app_msd_byte_array
)
{
  ecall_msd_structure_s_type ecall_msd;
  boolean efs_msd_read_status = FALSE;

  ECALL_MSG_0( ECALL_HIGH, "eCall app constructing MSD" );

  memset(&ecall_msd, 0, sizeof(ecall_msd_structure_s_type));

  /* Set MSD structure with Hard-Coded values first */
  ecall_construct_msd_struct(&ecall_msd); /* Construct Canned MSD from hardcoded values */

  if (ecall_app_struct.ecall_config_params.efs_params.canned_msd == TRUE)
  {
    ECALL_MSG_0( ECALL_HIGH, "ECALL - SEND CANNED from EFS " );
    efs_msd_read_status = ecall_app_efs_read_canned_msd(&ecall_msd);
  } /* Canned MSD */

  /* add the GPS fix position values, if any
  and hard code the rest of the Ecall MSD Structure */
  if( (efs_msd_read_status == FALSE) || (ecall_app_struct.ecall_config_params.efs_params.canned_msd == FALSE))
  {
    ecall_app_struct.ecall_msd_msg_id++;

    /* Check if UE got a valid (non-zero values) GPS position */
    #if ( (defined FEATURE_CGPS) || (defined FEATURE_ECALL_HAS_QMI_LOC) )
    ECALL_MSG_0( ECALL_MED, "Getting ecall GPS data to construct MSD" );
    ecall_getgpsdata(&ecall_msd); /* Get Position information from GPS module */
    #endif
  
    if (ecall_app_struct.ecall_config_params.type_of_call == ECALL_EMERGENCY)
    {
      /* EN15722: callType(0): Emergency Call */
      ecall_msd.ecall_control_info.callType = (ecall_type_of_call) 0;
    }
    else if (ecall_app_struct.ecall_config_params.type_of_call == ECALL_TEST)
    {
      /* EN15722: callType(1): Test Call */
      ecall_msd.ecall_control_info.callType = (ecall_type_of_call) 1;
    }
    else if (ecall_app_struct.ecall_config_params.type_of_call == ECALL_RECONFIG)
    {
      /* EN15722: callType(1): Test Call */
      ecall_msd.ecall_control_info.callType = (ecall_type_of_call) 1;
    }

    ecall_msd.ecall_control_info.activation = ecall_app_struct.ecall_config_params.activation_type;

    ecall_msd.ecall_messageidentifier       = ecall_app_struct.ecall_msd_msg_id;

  } /* (efs_msd_read_status == FALSE) || (ecall_config_params.canned_msd == FALSE) */

  /* Save the MSD in EFS */
  (void)ecall_save_msd(&ecall_msd);
  ecall_serialise_msd(&ecall_msd, ecall_app_msd_byte_array);

} /* ECALL_CONSTRUCT_MSD_WITH_GPS */

/*==========================================================================

  FUNCTION ECALL_APP_SET_ECALL_START_CONFIG

  DESCRIPTION
    This function will set type_of_call and activation_type to ecall_app_struct.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_set_ecall_start_config
(
  ecall_start_config_type *start_config_ptr
)
{
  ecall_app_struct.ecall_config_params.type_of_call = start_config_ptr->type_of_call;
  ecall_app_struct.ecall_config_params.activation_type = start_config_ptr->activation_type;
} /* ECALL_APP_SET_ECALL_START_CONFIG */



/*==========================================================================

  FUNCTION ECALL_APP_GET_EFS_PARAMS

  DESCRIPTION
    This function will be called by ecall_app_task when ecall_start_session
    is triggered. Will copy EFS data to ecall_app_struct

  DEPENDENCIES
    None.

  RETURN VALUE
    
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_get_efs_params
(
  ecall_efs_config_type *efs_config
)
{
 
  ecall_app_struct.ecall_config_params.efs_params.voc_config = efs_config->voc_config;
  ecall_app_struct.ecall_config_params.efs_params.ecall_dial_num = efs_config->ecall_dial_num;
  ecall_app_struct.ecall_config_params.efs_params.canned_msd = efs_config->canned_msd;
  ecall_memscpy ( ecall_app_struct.ecall_config_params.efs_params.digits, sizeof(ecall_app_struct.ecall_config_params.efs_params.digits),
                  efs_config->digits, sizeof(efs_config->digits) );
  ecall_app_struct.ecall_config_params.efs_params.gnss_update_time_ms = efs_config->gnss_update_time_ms;
  ecall_app_struct.ecall_config_params.efs_params.test_only_callback_timer_ms = efs_config->test_only_callback_timer_ms;
  ecall_app_struct.ecall_config_params.efs_params.test_only_call_cleardown_timer_ms = efs_config->test_only_call_cleardown_timer_ms;
  ecall_app_struct.ecall_config_params.efs_params.test_only_session_timer_ms = efs_config->test_only_session_timer_ms;
  ecall_app_struct.ecall_config_params.efs_params.ecall_usim_slot_id = efs_config->ecall_usim_slot_id;

} /* ECALL_APP_GET_EFS_PARAMS */

/*==========================================================================

  FUNCTION ECALL_SESSION_START

  DESCRIPTION
    Called from the cmlog.c/ ecall_diag.c, thru QXDM,
    this function will get the GPS and then proceed to making an ecall.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_session_start
(
  ecall_type_of_call type_of_call,
  ecall_activation_type activation_type,
  ecall_start_session_cb_type  ecall_start_session_cb_func
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  /* Queue up a command to the ecall_app_task to start ecall_session */
  cmd_ptr = ecall_app_task_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    ECALL_MSG_2( ECALL_MED, "SESSION_START: type_of_call %d, activation_type %d",
                             type_of_call,
                             activation_type );
    memset(&cmd_ptr->data.start_session_config, 0, sizeof(cmd_ptr->data.start_session_config));
    cmd_ptr->data.start_session_config.qmi_start = FALSE;
    cmd_ptr->data.start_session_config.start_config.type_of_call = type_of_call;
    cmd_ptr->data.start_session_config.start_config.activation_type = activation_type;
    cmd_ptr->data.start_session_config.start_session_cb = ecall_start_session_cb_func;
    cmd_ptr->data.start_session_config.call_orig_info_cb = NULL;
    cmd_ptr->data.start_session_config.call_orig_exten_info_cb = NULL;
    cmd_ptr->data.start_session_config.data_block_ptr = NULL;
    cmd_ptr->data.start_session_config.as_id = SYS_MODEM_AS_ID_1;
    /* Post a command to the eCall App task to process this session_start */
    cmd_ptr->cmd_id = ECALL_APP_CMD_SESSION_START;
    ecall_app_task_put_cmd(cmd_ptr);
    return TRUE; /* Command queued up successfully*/
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_session_start()" );
    ecall_start_session_cb_func(ECALL_FAILURE);
    return FALSE;
  }
  
} /* ecall_session_start */

/*==========================================================================

  FUNCTION ECALL_SESSION_QMI_START

  DESCRIPTION
    Called from the QMI_VOICE, thru OEM APP,
    this function will get the GPS and then proceed to making an ecall.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_session_qmi_start
(
  ecall_type_of_call type_of_call,
  ecall_activation_type activation_type,
  ecall_call_orig_info_cb_f_type call_orig_info_cb_func,
  void *data_block_ptr,
  ecall_call_orig_exten_info_s_type exten_info
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  /* Queue up a command to the ecall_app_task to start ecall_session */
  cmd_ptr = ecall_app_task_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    ECALL_MSG_3( ECALL_HIGH, "SESSION_QMI_START: type_of_call %d, activation_type %d, is_ecall_msd_available %d",
                              type_of_call,
                              activation_type,
                              exten_info.is_ecall_msd_available );
    memset(&cmd_ptr->data.start_session_config, 0, sizeof(cmd_ptr->data.start_session_config));
    cmd_ptr->data.start_session_config.qmi_start = TRUE;
    cmd_ptr->data.start_session_config.start_config.type_of_call = type_of_call;
    cmd_ptr->data.start_session_config.start_config.activation_type = activation_type;
    cmd_ptr->data.start_session_config.start_session_cb = NULL;
    cmd_ptr->data.start_session_config.call_orig_info_cb = call_orig_info_cb_func;
    cmd_ptr->data.start_session_config.call_orig_exten_info_cb = NULL;
    cmd_ptr->data.start_session_config.data_block_ptr = data_block_ptr;
    cmd_ptr->data.start_session_config.as_id = SYS_MODEM_AS_ID_1;
    if ( TRUE == exten_info.is_ecall_msd_available )
    {
      cmd_ptr->data.start_session_config.is_external_msd = TRUE;
      ecall_memscpy(cmd_ptr->data.start_session_config.external_msd, sizeof(cmd_ptr->data.start_session_config.external_msd),
                    exten_info.ecall_msd, sizeof(exten_info.ecall_msd));
    }
    /* Post a command to the eCall App task to process this session_start */
    cmd_ptr->cmd_id = ECALL_APP_CMD_SESSION_START;
    ecall_app_task_put_cmd(cmd_ptr);
    return TRUE; /* Command queued up successfully*/
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_session_start()" );
    call_orig_info_cb_func(data_block_ptr,
                           ECALL_FAILURE,
                           (cm_call_id_type)-1,
                           (cm_client_id_type)-1);
    return FALSE;
  }
  
} /* ECALL_SESSION_QMI_START */

/*==========================================================================

  FUNCTION ECALL_SESSION_QMI_START_PER_SUBS

  DESCRIPTION
    Called from the QMI_VOICE, thru OEM APP,
    this function will get the GPS and then proceed to making an ecall.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_session_qmi_start_per_subs
(
  ecall_type_of_call type_of_call,
  ecall_activation_type activation_type,
  ecall_call_orig_exten_info_cb_f_type call_orig_exten_info_cb_func,
  void *data_block_ptr,
  ecall_call_orig_exten_info_s_type exten_info
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;
  ecall_call_orig_exten_info_s_type call_back_exten_info;

  /* Queue up a command to the ecall_app_task to start ecall_session */
  cmd_ptr = ecall_app_task_get_cmd_buf();

  if (cmd_ptr != NULL)
  {
    ECALL_MSG_4( ECALL_HIGH, "SESSION_QMI_START_PER_SUBS: type_of_call %d, activation_type %d, as_id %d, is_ecall_msd_available %d",
                              type_of_call,
                              activation_type,
                              exten_info.as_id, 
                              exten_info.is_ecall_msd_available );
    memset(&cmd_ptr->data.start_session_config, 0, sizeof(cmd_ptr->data.start_session_config));
    cmd_ptr->data.start_session_config.qmi_start = TRUE;
    cmd_ptr->data.start_session_config.start_config.type_of_call = type_of_call;
    cmd_ptr->data.start_session_config.start_config.activation_type = activation_type;
    cmd_ptr->data.start_session_config.start_session_cb = NULL;
    cmd_ptr->data.start_session_config.call_orig_info_cb = NULL;
    cmd_ptr->data.start_session_config.call_orig_exten_info_cb = call_orig_exten_info_cb_func;
    cmd_ptr->data.start_session_config.data_block_ptr = data_block_ptr;
    cmd_ptr->data.start_session_config.as_id = exten_info.as_id;
    if ( TRUE == exten_info.is_ecall_msd_available )
    {
      cmd_ptr->data.start_session_config.is_external_msd = TRUE;
      ecall_memscpy(cmd_ptr->data.start_session_config.external_msd, sizeof(cmd_ptr->data.start_session_config.external_msd),
                    exten_info.ecall_msd, sizeof(exten_info.ecall_msd));
    }
    /* Post a command to the eCall App task to process this session_start */
    cmd_ptr->cmd_id = ECALL_APP_CMD_SESSION_START;
    ecall_app_task_put_cmd(cmd_ptr);
    return TRUE; /* Command queued up successfully*/
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_session_start()" );
    call_back_exten_info.is_as_id_available = TRUE;
    call_back_exten_info.as_id              = exten_info.as_id;
    call_orig_exten_info_cb_func(data_block_ptr,
                                 ECALL_FAILURE,
                                 (cm_call_id_type)-1,
                                 (cm_client_id_type)-1,
                                 call_back_exten_info);
    return FALSE;
  }
  
} /* ECALL_SESSION_QMI_START */

/*==========================================================================

  FUNCTION ECALL_SESSION_STOP

  DESCRIPTION
    Called from the cmlog.c/ call_diag.c, thru QXDM,
    this function will END an ecall.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_session_stop
(
  void
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  /* Queue up a command to the ecall_app_task to stop ecall session */
  cmd_ptr = ecall_app_task_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall App task to process this PCM frame */
    cmd_ptr->cmd_id = ECALL_APP_CMD_STOP_ECALL;
    ecall_app_task_put_cmd(cmd_ptr);
    return TRUE; /* Command queued up successfully*/
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_session_stop()" );
    return FALSE;
  }

} /* ECALL_SESSION_STOP */


/*==========================================================================

  FUNCTION ECALL_SESSION_GET_CALL_INFO
    This function will queue up a cmd to ecall_app_task and monitor the eCall
    session call info

  DESCRIPTION

  DEPENDENCIES
    None.

  RETURN VALUE
    Boolean - 1 = Command Queued Up Successfully
              0 = Command Queued Up Failed

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_session_get_call_info
(
  ecall_session_call_info_cb_type call_info_cb
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  if (call_info_cb == NULL)
  {
    ECALL_MSG_0( ECALL_ERROR, "Error: call_info_cb is NULL" );
    return FALSE;
  }

  /* Queue up a command to the ecall_app_task to get session call info */
  cmd_ptr = ecall_app_task_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall App task to process this PCM frame */
    cmd_ptr->cmd_id = ECALL_APP_CMD_GET_CALL_INFO;
    cmd_ptr->data.cmd_call_info_cb = call_info_cb;
    ecall_app_task_put_cmd(cmd_ptr);
    return TRUE; /* Command queued up successfully*/
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_session_get_call_info()" );
    return FALSE;
  }

} /* ECALL_SESSION_GET_CALL_INFO */

/*==========================================================================

  FUNCTION ECALL_SESSION_UPDATE_MSD_SETTING
    This function will queue up a cmd to ecall_app_task and update the MSD
    setting.

  DESCRIPTION

  DEPENDENCIES
    None.

  RETURN VALUE
    Boolean - 1 = Command Queued Up Successfully
              0 = Command Queued Up Failed

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_session_update_msd_setting
(
  ecall_session_update_msd_setting_s_type  msd_setting
)
{
  ecall_app_task_cmd_type *cmd_ptr = NULL;

  /* Queue up a command to the ecall_app_task to update the MSD setting */
  cmd_ptr = ecall_app_task_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    /* Post a command to the eCall App Task */
    cmd_ptr->cmd_id = ECALL_APP_CMD_UPDATE_MSD_SETTING;
    cmd_ptr->data.msd_setting = msd_setting;
    ecall_app_task_put_cmd(cmd_ptr);
    return TRUE; /* Command queued up successfully*/
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ecall_app_task_get_cmd_buf ran out of command buffers in ecall_session_update_msd_setting()" );
    return FALSE;
  }

} /* ECALL_SESSION_UPDATE_MSD_SETTING */




/*==========================================================================

  FUNCTION ECALL_PROCESS_SESSION_GET_CALL_INFO_CMD
    This function will call the passed in registered callback with current
    call info, such as type_of_call and activation_type

  DESCRIPTION

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_process_session_get_call_info_cmd
(
  ecall_session_call_info_cb_type call_info_cb
)
{
  ECALL_MSG_0( ECALL_MED, "ecall_process_session_get_call_info_cmd updates callback call_info_cb" );
  if(call_info_cb != NULL)
  {
    call_info_cb(ecall_app_struct.ecall_config_params.type_of_call,
                 ecall_app_struct.ecall_config_params.activation_type);
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "ERROR: call_info_cb is NULL" );
  }
} /* ECALL_PROCESS_SESSION_CALL_INFO_CMD */


/*==========================================================================

  FUNCTION ECALL_RESET_TYPE_OF_CALL
    This function will reset type_of_call to default value ECALL_NO_ACTIVE_CALL

  DESCRIPTION

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_reset_type_of_call
(
  void
)
{
  ECALL_MSG_0( ECALL_MED, "type_of_call reset to ECALL_NO_ACTIVE_CALL" );
  ecall_app_struct.ecall_config_params.type_of_call = ECALL_NO_ACTIVE_CALL;
} /* ECALL_RESET_TYPE_OF_CALL */

/*==========================================================================

  FUNCTION ECALL_APP_INIT

  DESCRIPTION
    This function will initialize ecall_app global structure variables

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_init
(
  void
)
{
  /* Initialize the whole ecall_app_struct to 0 first */
  memset(&ecall_app_struct, 0, sizeof(ecall_app_struct));

  /* Initialize type_of_call to ECALL_NO_ACTIVE_CALL */
  ecall_app_struct.ecall_config_params.type_of_call = ECALL_NO_ACTIVE_CALL;
}


#else

void ecall_app_dummy_func(void);

/*==========================================================================

  FUNCTION ECALL_SESSION_START

  DESCRIPTION
    Called from the cmlog.c/ ecall_diag.c, thru QXDM,
    this function will get the GPS and then proceed to making an ecall.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_session_start
(
  ecall_type_of_call type_of_call,
  ecall_activation_type activation_type,
  ecall_start_session_cb_type  ecall_start_session_cb_func
)
{
  ECALL_MSG_0( ECALL_HIGH, "ECALL FEATURE NOT DEFINED, Returning FALSE" );
  return FALSE;
} /* ecall_client_start */


/*==========================================================================

  FUNCTION ECALL_SESSION_STOP

  DESCRIPTION
    Called from the cmlog.c/ call_diag.c, thru QXDM,
    this function will END an ecall.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_session_stop
(
  void
)
{
  ECALL_MSG_0( ECALL_HIGH, "ECALL FEATURE NOT DEFINED, Returning FALSE" );
  return FALSE;
} /* ECALL_SESSION_STOP */

#endif //End Featurisation

