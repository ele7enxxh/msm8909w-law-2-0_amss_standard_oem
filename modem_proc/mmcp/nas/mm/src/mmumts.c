/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmumts.c_v   1.19   01 Jul 2002 12:04:54   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmumts.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/01   jca     Rewrote RRC<->MM PLMN type conversion functions.
05/14/01   jca     Added mm_ prefix to MM/GMM shared variables.
05/16/01   sbk     Fixed bugs found in service_ind conversion routine
05/29/01   sbk     Fixed a bug in get_lai_from_service_info
05/31/01   sbk     Simplified memcpy in mm_send_nas_msg_to_rrc
06/26/01   sbk     Got rid of convert_lai_list_cnf_to_rr
09/18/01   sbk     Channel rrc commands to send_cmd_to_rrc function
03/13/02   ks      Changed the establish_cause for CS DATA to
                   RRC_EST_ORIG_CONV_CALL and corrected the SMS & SS causes.
04/15/02   ks      Setting the data_req.priority to LOW if the variable
                   mm_sig_low_priority is set TRUE in mm_send_ota_message.
04/24/02   jca     Removed redundant function mm_send_nas_msg_to_rrc().
05/02/02   jca     Fixed bug in function get_imei_as_digits() where last
                   digit of IMEI was not being written to return buffer.
06/05/02   ks      Added support for Dual mode operation.
06/26/02   ks      Added support for NAS message sequence numbering.
08/05/02   mks     Fixed mm_get_initial_ue_id() to pick PTMSI as ue_id if
                   TMSI is not available and PTMSI is available.
09/05/02   mks     Removed the extra characters at the EOL
09/05/02   mks     Implemented MM integrity checking procedures for CS domain
11/14/02   cd      In mm_send_nas_msg_to_rrc_by_domain() added ability to overwrite
                   the FOR bit in case it has been sent and the LOCATION_UPDATING_REQ
                   msg has not ben sent out yet
01/07/03   ks      Setting the 3rd digit MNC to 0xF in rrc_plmn_id primitive if LA Id
                   has 2digit MNC in mmumts_convert_nas_plmn_id_to_rrc_plmn_id().
01/10/03   mks     Updated mm_check_for_integrity_protection () to validate the
                   protocol descriminator while performing integrity checking of
                   MM signalling messages.
04/07/03   cd      Added call to start T3210 during a LU procedure just before the
                   LU_REQ message is sent
05/08/03   cd      Added check for PD_MM when processing a LU_REQ msg
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
07/14/03   ks      settting the integrity_check_passed to TRUE if the PD type is
                   PD_TEST as TC messages should be processed without integrity.
09/11/03   ks      Checking the Revision NV parameters before updating the
                   Sequence numbering.
12/04/03   jca     Deleted unused function get_lai_from_service_info().
01/21/05   ks      Added function mm_waiting_for_nw_command() which returns TRUE
                   when MM is in state 9 used by RRC for connection management.
02/02/05   ks      Return TRUE in function get_lai_from_service_info() when the
                   mm_cnm_rel_req_received flag is set to TRUE.
04/04/05   ks      If MM is IDLE, return TRUE in mm_waiting_for_nw_command().
07/29/05   abe     Addin the falg is_emergency_call in rrc_data_req for the feature 
                   FEATURE_OUT_OF_SERVICE_ENHANCEMENT
09/08/05   abe     Removing the feature falg in the previous fix.
12/20/05   up      Modified 'mm_get_initial_ue_id' function to validate LAI & 
                   RAI before giving to RRC. 
07/14/06   nr      Adding changes for SMS optimization CR 93099 
02/20/08   jbk     Remove GCF NV Item Dependence on Send Sequence Number
09/26/11   rajesh  Added code to send L3 NAS message to RRC instead of RR, if UE is in WRLF state even
                   when active rat is GSM.
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mm_v.h"
#include "rrcmmif.h"
#include "rrccmd.h"
#include "mm_rr.h"
#include "mm_umts_v.h"
#include "err.h"
#include "log.h"
#include "naslog_v.h"
#include "gmm_int_v.h"
#include "mm_sim.h"
#include "mm_iei.h"
#include "cm.h"
#include <stringl/stringl.h>
#ifdef FEATURE_LTE
#include "emm_database.h"
#endif

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
#ifdef FEATURE_MODEM_HEAP
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern serving_plmn_info_T *mm_wrlf_serving_plmn_ptr_sim[MAX_NAS_STACKS];
#define mm_wrlf_serving_plmn_ptr mm_wrlf_serving_plmn_ptr_sim[mm_as_id]
#else
extern serving_plmn_info_T *mm_wrlf_serving_plmn_ptr;
#endif
#else
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern serving_plmn_info_T mm_wrlf_serving_plmn_sim[MAX_NAS_STACKS];
#define mm_wrlf_serving_plmn mm_wrlf_serving_plmn_sim[mm_as_id]
#else
extern serving_plmn_info_T mm_wrlf_serving_plmn;
#endif
#endif
#endif
#ifdef FEATURE_SEGMENT_LOADING
#include "mcpm_api.h"
#include "mcfg_seg_load.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#include "ITDSCDMA.h"
#endif

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION HAS_VALID_LAI

DESCRIPTION
  This function determines the validity of stored LAI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_has_valid_lai(void)
{
  sys_plmn_id_s_type plmn_id;

  /* PLMN id = first three octets of the LAI */
  memscpy( &plmn_id,sizeof(sys_plmn_id_s_type), mm_location_information.lai, PLMN_SIZE );

  if ((sys_plmn_id_is_undefined(plmn_id)) ||
    (!sys_plmn_id_is_valid(plmn_id)))
  {
    /* invalid PLMN */
    return FALSE;
  }

  /* per 24.008 , sec 10.5.1.3, LAC is deleted if LAC is 0xFFFE or 0x0000 */
  if (((mm_location_information.lai[PLMN_SIZE] == 0xFF) &&
    (mm_location_information.lai[PLMN_SIZE + 1] == 0xFE)) ||
    ((mm_location_information.lai[PLMN_SIZE] == 0x00) &&
    (mm_location_information.lai[PLMN_SIZE + 1] == 0x00)))
  {
    /* Invalid LAC */
    return FALSE;
  }

  return TRUE;

}


/*===========================================================================

FUNCTION HAS_VALID_RAI

DESCRIPTION
  This function determines the validity of stored RAI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean has_valid_rai(void)
{
  if ((sys_plmn_id_is_undefined(gmm_stored_rai.plmn_id)) ||
    !sys_plmn_id_is_valid(gmm_stored_rai.plmn_id))
  {
    /* invalid PLMN */
    return FALSE;
  }

  /* per 24.008 , sec 10.5.5.15, LAC is deleted if LAC is 0xFFFE or 0x0000 */
  if (((gmm_stored_rai.location_area_code.lac[0] == 0xFF) &&
    (gmm_stored_rai.location_area_code.lac[1] == 0xFE)) ||
    ((gmm_stored_rai.location_area_code.lac[0] == 0x00) &&
    (gmm_stored_rai.location_area_code.lac[1] == 0x00)))
  {
    /* Invalid LAC */
    return FALSE;
  }

  if (gmm_stored_rai.routing_area_code == 0xFF)
  {
    /* Invalid RAC */
    return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION MM_CHECK_FOR_INTEGRITY_PROTECTION

DESCRIPTION
  This function performs the integrity checking of LU messages per
  section 4.1.1.1.1 of TS24.008.

DEPENDENCIES
  None

RETURN VALUE
  This function returns
    TRUE  - if the integrity check succeeds
    FALSE - if the integrity check fails

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_check_lu_for_integrity_protection( mm_cmd_type *msg_p )
{
  boolean integrity_check_passed = mm_integrity_protected;
  
#ifdef FEATURE_NAS_REL10
     dword                           new_T3212_timer_value = 0; /* Default - Should always be reset*/
     boolean                         per_ms_3212_avail = FALSE;
#endif

#ifdef FEATURE_TDSCDMA
  if (is_tdscdma_mode() && !gmm_anite_gcf_flag)
  {
    integrity_check_passed = TRUE;
  }
#endif /* FEATURE_TDSCDMA */

  if ( mm_integrity_protected == FALSE &&
       is_OTA_message(msg_p) &&
       get_OTA_msg_type(msg_p) == LOCATION_UPDATING_ACCEPT &&
       mm_get_lu_cause () == MM_LU_CAUSE_PERIODIC )
  {
    byte lu_lai[LAI_SIZE];
    byte lu_tmsi[TMSI_SIZE];
    boolean tmsi_avail = FALSE;
    byte msg_length = 0;
    /* Message index pointing to the Optional IEs in the Location Update Accept message*/
    byte msg_index  = 7;

    /*-------------------------------------------------------------
     * Store the LAI received in the Location Update Accept message
     *------------------------------------------------------------*/
    memscpy (lu_lai,LAI_SIZE, msg_p->cmd.rr_data_ind.layer3_message + 2, LAI_SIZE);

    /*-----------------------------------------------------------------
     * Store the TMSI, if present in the Location Update Accept message
     *----------------------------------------------------------------*/
    memset(lu_tmsi, 0x00, TMSI_SIZE);

    /*------------------------------------------------------------
     * Store the message length received in the Location Update Accept message  */
    msg_length = MIN(msg_p->cmd.rr_data_ind.message_header.message_len_lsb,(byte)gmm_max_L3_msg_length(MM_CS_DOMAIN));

    while (msg_index < msg_length)
    {
      if ((msg_p->cmd.rr_data_ind.layer3_message[msg_index] == IEI_MOBILE_ID) && (msg_index + 1 < msg_length))
       {
         if ((msg_index + 2 < msg_length) &&
             (( (byte)(msg_p->cmd.rr_data_ind.layer3_message[msg_index+2]) & (byte)(0x07) ) == (byte)(0x04)) &&
             ((msg_index + MIN(msg_p->cmd.rr_data_ind.layer3_message[msg_index+1],TMSI_SIZE) + 2) <= msg_length))
          {
            tmsi_avail = TRUE;
            memscpy(lu_tmsi,TMSI_SIZE ,msg_p->cmd.rr_data_ind.layer3_message+msg_index+2,MIN(msg_p->cmd.rr_data_ind.layer3_message[msg_index+1],TMSI_SIZE));
          }
          msg_index = msg_index + 2 + msg_p->cmd.rr_data_ind.layer3_message[msg_index+1];
       }
      else if ((msg_p->cmd.rr_data_ind.layer3_message[msg_index] == IEI_FOLLOW_ON_PROCEED) &&
               ((msg_index + 1) <= msg_length))
       {
         msg_index = msg_index + 1;
       }
      else if ((msg_p->cmd.rr_data_ind.layer3_message[msg_index] == IEI_EQUIV_PLMN) &&
               (msg_index + 1 < msg_length) &&
               ((msg_index + msg_p->cmd.rr_data_ind.layer3_message[msg_index+1] + 2) <= msg_length))
       {
         msg_index = msg_index + msg_p->cmd.rr_data_ind.layer3_message[msg_index+1] + 2;
       }
      else if ((msg_p->cmd.rr_data_ind.layer3_message[msg_index] == IEI_EMERGENCY_NUM_LIST) &&
               (msg_index + 1 < msg_length) &&
               ((msg_index + msg_p->cmd.rr_data_ind.layer3_message[msg_index+1] + 2) <= msg_length))
       {
         msg_index = msg_index + msg_p->cmd.rr_data_ind.layer3_message[msg_index+1] + 2;
       }
       /*
         ** TS 24.008 4.1.1.1.1 V10.7.0 Integrity Protection for Per MS 3212
         ** UE shall not accept a non-integrity protected LAU Accept in response to periodic request, 
         ** if the LAU accept contains a changed Per MS T3212 value.   
            */
#ifdef FEATURE_NAS_REL10
      else if ((msg_p->cmd.rr_data_ind.layer3_message[msg_index] == IEI_PER_MS_3212) && (msg_index + 2 < msg_length))
       {
         per_ms_3212_avail = TRUE;
         new_T3212_timer_value = gmm_convert_timer3_value_ie_to_msecs(
                                  (msg_p->cmd.rr_data_ind.layer3_message[msg_index+2]));
         msg_index = msg_index + msg_p->cmd.rr_data_ind.layer3_message[msg_index+1] + 2;
       }
#endif
      else
       {
         msg_index = msg_index + 1;
       }
    }
     /*-----------------------------------------------------------------
     * LU Accept message is processed without integrity protection only
     * for PERIODIC LU with no change to LAI ,TMSI and Per MS 3212
     *----------------------------------------------------------------*/
    if ( memcmp(lu_lai,  mm_location_information.lai,  LAI_SIZE) == 0 )
    {
      if (tmsi_avail == FALSE)
      {
        integrity_check_passed = TRUE;
      }
      else if (memcmp(lu_tmsi, mm_location_information.tmsi, TMSI_SIZE) == 0 )
      {
        integrity_check_passed = TRUE;
      }
#ifdef FEATURE_NAS_REL10
      if ((per_ms_3212_avail == TRUE) && 
                    (new_T3212_timer_value != mm_stored_per_ms_t3212_value))
      {
        integrity_check_passed = mm_integrity_protected;
      }
#endif

    }

  }
  return integrity_check_passed;
}


/*===========================================================================

FUNCTION mm_check_lu_rej_for_integrity_protection

DESCRIPTION
  This function performs the integrity checking of LU reject messages 


DEPENDENCIES
  None

RETURN VALUE
  This function returns
    TRUE  - if the integrity check succeeds
    FALSE - if the integrity check fails

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_check_lu_rej_for_integrity_protection( mm_cmd_type *msg_p )
{
  boolean integrity_check_passed = TRUE;

#ifdef FEATURE_FEMTO_CSG
  byte reject_cause;
#endif 

#ifdef FEATURE_FEMTO_CSG
  if ( mm_integrity_protected == FALSE &&
       is_OTA_message(msg_p) &&
       get_OTA_msg_type(msg_p) == LOCATION_UPDATING_REJECT)
  {
    reject_cause = msg_p->cmd.rr_data_ind.layer3_message[2];     
    if(reject_cause == CSG_NOT_AUTHORIZED && mm_serving_csg_id != SYS_CSG_ID_INVALID)
    {
      integrity_check_passed = FALSE;
    }
  }
#endif 
return integrity_check_passed;
}

/*===========================================================================

FUNCTION mm_check_service_rej_for_integrity_protection

DESCRIPTION
  This function performs the integrity checking of LU reject messages 


DEPENDENCIES
  None

RETURN VALUE
  This function returns
    TRUE  - if the integrity check succeeds
    FALSE - if the integrity check fails

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_check_service_rej_for_integrity_protection( mm_cmd_type *msg_p )
{
  boolean integrity_check_passed = TRUE;

#ifdef FEATURE_FEMTO_CSG
  byte reject_cause;
#endif 

#ifdef FEATURE_FEMTO_CSG
  if ( mm_integrity_protected == FALSE &&
       is_OTA_message(msg_p) &&
       get_OTA_msg_type(msg_p) == CM_SERVICE_REJECT)
  {
    reject_cause = msg_p->cmd.rr_data_ind.layer3_message[2];     
    if(reject_cause == CSG_NOT_AUTHORIZED && mm_serving_csg_id != SYS_CSG_ID_INVALID)
    {
      integrity_check_passed = FALSE;
    }
  }
#endif 
  return integrity_check_passed;
}


/*===========================================================================

FUNCTION MM_CHECK_FOR_INTEGRITY_PROTECTION

DESCRIPTION
  This function performs the integrity checking of signaling messages per
  section 4.1.1.1.1 of TS24.008.

DEPENDENCIES
  None

RETURN VALUE
  This function returns
    TRUE  - if the integrity check succeeds
    FALSE - if the integrity check fails

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_check_for_integrity_protection( mm_cmd_type *msg_p )
{
  boolean integrity_check_passed = mm_integrity_protected;
  byte protocol_descriminator =
       (byte)GET_PD(msg_p->cmd.rr_data_ind.layer3_message[0]);

#ifdef FEATURE_TDSCDMA
  if (is_tdscdma_mode() && !gmm_anite_gcf_flag)
  {
    integrity_check_passed = TRUE;
  }
#endif /* FEATURE_TDSCDMA */

  if (protocol_descriminator == PD_MM)
  {
    switch (get_OTA_msg_type(msg_p))
    {
      case AUTHENTICATION_REQUEST:
      case AUTHENTICATION_REJECT:
      case IDENTITY_REQUEST:
      case ABORT:
        integrity_check_passed = TRUE;
        break;

     case LOCATION_UPDATING_REJECT:
       integrity_check_passed = mm_check_lu_rej_for_integrity_protection(msg_p);
       break;

      case LOCATION_UPDATING_ACCEPT:
        integrity_check_passed = mm_check_lu_for_integrity_protection(msg_p);
        break;

      case CM_SERVICE_ACCEPT:
      /*-------------------------------------------------------------------
       * CM_SERVICE_ACCEPT message is processed without integrity protection
       * only during emergency call establishment
       *------------------------------------------------------------------*/
        if (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION &&
            mm_cnm_is_active &&
            mm_cnm_srv_est_cause == MM_RR_EMERGENCY_CALL)
        {
          integrity_check_passed = TRUE;
        }
        break;

      case CM_SERVICE_REJECT:
         integrity_check_passed = mm_check_service_rej_for_integrity_protection(msg_p);
        break;

      case TMSI_REALLOCATION_COMMAND:
#ifdef FEATURE_DUAL_SIM
       if ((cm_get_operator_info((sys_modem_as_id_e_type)mm_sub_id) == OPERATOR_CMCC) || (cm_get_operator_info((sys_modem_as_id_e_type)mm_sub_id) == OPERATOR_CU))
#else
       if ((cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CMCC) || (cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CU))
#endif
        {
          if ((!integrity_check_passed) && 
              (UPDATED == mm_location_information.location_update_status) && 
             ((MM_WAIT_FOR_OUTGOING_MM_CONNECTION == mm_state) ||
              (MM_CONNECTION_ACTIVE == mm_state))&& 
              (MM_RR_EMERGENCY_CALL == mm_cnm_srv_est_cause))
          { /* in case Emergency call is setting up and UPDATED means UE may send *
           * CM SERVICE REQUEST with valid TMSI  */
            integrity_check_passed = TRUE;
          }  
        }
      break;
     
      default:
        break;

    }
  }
  /*-----------------------------------------------
  ** Check CC messages for integrity protection
  **----------------------------------------------*/
  else if (protocol_descriminator == PD_CC &&
    mm_state == MM_CONNECTION_ACTIVE &&
    mm_connection_type == EMERGENCY_MM_CONNECTION)
  {
    integrity_check_passed = TRUE;
  }

  else if (protocol_descriminator == PD_TEST)
  {
    integrity_check_passed = TRUE;
  }
  return integrity_check_passed;
}


/*===========================================================================

FUNCTION MM_GET_INITIAL_UE_ID

DESCRIPTION
  This function supplies the initial UE ID used in RRC_EST _REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

rrc_initial_ue_id_type mm_get_initial_ue_id(rrc_initial_ue_id_choice_e_type *choice )
{
   rrc_initial_ue_id_type ue_id;
   sys_plmn_id_s_type    plmn_id;
#if defined(FEATURE_LTE)
   emm_ctrl_data_type *emm_ctrl_data_ptr = emm_db_get_ctrl_data(); 
   gmm_ptmsi_type   ptmsi;
   ASSERT(emm_ctrl_data_ptr != NULL);
#endif

   /* Per 8.5.1 of TS25.331, TMSI shall be chosen as UE ID type if available */
   /* and if LAI is valid */
   if( has_valid_tmsi() && mm_has_valid_lai())
   {
      *choice = RRC_TMSI_LAI_UE_ID;
      memscpy(ue_id.tmsi_lai.tmsi.value,RRC_MAX_GSMMAP_TMSI_LENGTH, mm_location_information.tmsi, TMSI_SIZE);

      /* PLMN id = first three octets of the LAI */
      memscpy( &plmn_id,sizeof(sys_plmn_id_s_type), mm_location_information.lai, PLMN_SIZE );
      ue_id.tmsi_lai.lai.plmn = mmumts_convert_nas_plmn_id_to_rrc_plmn_id( plmn_id );

      /* Location Area Code = the 2 octets following PLMN id */
      ue_id.tmsi_lai.lai.lac[0] = mm_location_information.lai[3];
      ue_id.tmsi_lai.lai.lac[1] = mm_location_information.lai[4];
   }

#if defined(FEATURE_LTE)
   /*If TIN is GUTI then UE id will be mapped identity*/
   else if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI && 
       emm_ctrl_data_ptr->emm_guti_valid == TRUE

#ifdef FEATURE_SGLTE
                            &&     MM_IS_SGLTE_ON_PS_STACK_ID)
#else
                                    )
#endif
       )
   {
     *choice = RRC_PTMSI_RAI_UE_ID;
     /*Copy PTMSI*/
     emm_map_guti_to_ptmsi(&ptmsi, emm_ctrl_data_ptr);
     memscpy(ue_id.ptmsi_rai.ptmsi.value, RRC_MAX_GSMMAP_PTMSI_LENGTH ,ptmsi.digit, PTMSI_SIZE);
       
     /* Copy the PLMN id */
     ue_id.ptmsi_rai.rai.lai.plmn = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->emm_guti.plmn);
     
     /* Set the RAC */
     ue_id.ptmsi_rai.rai.rac[0] = emm_ctrl_data_ptr->emm_guti.mme_code;
     
     /* Set the LAC */ 
     ue_id.ptmsi_rai.rai.lai.lac[0] = emm_ctrl_data_ptr->emm_guti.mme_group_id[1];
     ue_id.ptmsi_rai.rai.lai.lac[1] = emm_ctrl_data_ptr->emm_guti.mme_group_id[0];
   }
#endif
   /* PTMSI chosen as UE ID type if TMSI not available & PTMSI is available */
   /* and if RAI is valid */ 
   else if(mm_has_valid_ptmsi() && has_valid_rai())
   {
      *choice = RRC_PTMSI_RAI_UE_ID;

      /* Copy the PTMSI */
      memscpy(ue_id.ptmsi_rai.ptmsi.value, RRC_MAX_GSMMAP_PTMSI_LENGTH, gmm_stored_ptmsi.digit, PTMSI_SIZE);

      /* Set the RAC */
      ue_id.ptmsi_rai.rai.rac[0] = gmm_stored_rai.routing_area_code;

      /* Copy the PLMN id */
      ue_id.ptmsi_rai.rai.lai.plmn = mmumts_convert_nas_plmn_id_to_rrc_plmn_id( gmm_stored_rai.plmn_id);

      /* Set the LAC */
      memscpy (ue_id.ptmsi_rai.rai.lai.lac, RRC_MAX_GSMMAP_LAC_LENGTH, gmm_stored_rai.location_area_code.lac, LAC_SIZE);
   }
   /* IMSI shall be chosen as UE ID type if TMSI & PTMSI not available & IMSI is available */
   else if( mm_stored_imsi.length != 0)
   {
      /* use IMSI as UE ID */
      *choice = RRC_IMSI_UE_ID;
      ue_id.imsi = convert_imsi_to_digits(mm_stored_imsi);
   }
   /* IMEI shall be chosen as UE ID type if TMSI, PTMSI, IMSI are not available */
   else
   {
      *choice = RRC_IMEI_UE_ID;
      ue_id.imei = get_imei_as_digits();
   }

   return ue_id;
}

/*===========================================================================

FUNCTION CONVERT_EST_CAUSE_TO_RRC

DESCRIPTION
  This function converts the NAS establishment cause to the RRC establisment
  cause type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

rrc_establish_cause_e_type convert_est_cause_to_rrc(establishment_cause_T est_cause)
{
   rrc_establish_cause_e_type rrc_cause;
   switch(est_cause)
   {
      case MM_RR_MO_FULL_SPEECH:
      case MM_RR_MO_HALF_SPEECH:
         rrc_cause = RRC_EST_ORIG_CONV_CALL;
         break;

      case MM_RR_MO_DATA_9600:
      case MM_RR_MO_DATA_4800:
      case MM_RR_MO_DATA_2400:
         rrc_cause = RRC_EST_ORIG_CONV_CALL;
         break;

      case MM_RR_SHORT_MESSAGE:
         rrc_cause = RRC_EST_LOW_PRI_SIGNALLING;
         break;

      case MM_RR_SUPPLEMENTARY_SERVICE:
         rrc_cause = RRC_EST_HI_PRI_SIGNALLING;
         break;

      case MM_RR_LOCATION_UPDATE:
         rrc_cause = RRC_EST_REGISTRATION;
         break;

      case MM_RR_IMSI_DETACH:
         rrc_cause = RRC_EST_DETACH;
         break;

      case MM_RR_CALL_REEST:
         rrc_cause = RRC_EST_CALL_RE_ESTABLISH;
         break;

      case MM_RR_EMERGENCY_CALL:
         rrc_cause = RRC_EST_EMERGENCY_CALL;
         break;

      default:
         MSG_ERROR_DS(MM_SUB, "=MM= Unknown establishment cause to convert to RRC", 0,0,0);
         rrc_cause = RRC_EST_ORIG_CONV_CALL;
         break;
   }

   return rrc_cause;
}


/*===========================================================================

FUNCTION CONVERT_LAI_LIST_TO_RR

DESCRIPTION
  This function onverts PLMN list from RRC format to intertask LAI format of RR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void convert_lai_list_to_rr(rrc_lai_list_type *rrc_list, inter_task_lai_list_T *rr_list)
{
   byte count;
   sys_plmn_id_s_type plmn_id;

   for(count =0; count < rrc_list->num_lais; count++)
   {
      plmn_id = mmumts_convert_rrc_plmn_id_to_nas_plmn_id( rrc_list->lai[count].plmn );
      rr_list->location_area_identification[count].PLMN_id.identity[0] = plmn_id.identity[0];
      rr_list->location_area_identification[count].PLMN_id.identity[1] = plmn_id.identity[1];
      rr_list->location_area_identification[count].PLMN_id.identity[2] = plmn_id.identity[2];

      rr_list->location_area_identification[count].location_area_code[0] =
        rrc_list->lai[count].lac[0];
      rr_list->location_area_identification[count].location_area_code[1] =
        rrc_list->lai[count].lac[1];
   }

   rr_list->no_of_entries = (byte)rrc_list->num_lais;

}


/*===========================================================================

FUNCTION MMCONVERT_RRC_LAI_TYPE_TO_INTER_TASK_LAI

DESCRIPTION
  This function converts given RRC LAI type to that of RR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmconvert_rrc_lai_type_to_inter_task_lai
(
   inter_task_lai_T *rr_lai, rrc_lai_type * rrc_lai
)
{
   sys_plmn_id_s_type plmn_id;


   plmn_id = mmumts_convert_rrc_plmn_id_to_nas_plmn_id( rrc_lai->plmn );
   rr_lai->PLMN_id.identity[0] = plmn_id.identity[0];
   rr_lai->PLMN_id.identity[1] = plmn_id.identity[1];
   rr_lai->PLMN_id.identity[2] = plmn_id.identity[2];

   rr_lai->location_area_code[0] = rrc_lai->lac[0];
   rr_lai->location_area_code[1] = rrc_lai->lac[1];
}


/*===========================================================================

FUNCTION MM_SEND_OTA_MSG

DESCRIPTION
  This function sends an L3 NAS message to existing RRC connection to the
  Circuit-switched CN domain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_ota_message( byte *nas_msg_p, dword length )
{
  byte pd = 0;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t * ptr_W = NULL;

  ptr_W = get_wcdma_interface();
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
  if(mm_wcdma_rlf_state == WCDMA_RLF_SERVICE_CNF_PENDING)
  {
    MSG_HIGH_DS_0(MM_SUB,"Hard abort already sent to RRC, ignoring L3 NAS message");
  }
  else
#endif
   if (is_umts_mode()
#ifdef FEATURE_WRLF_SYSTEM_SEL
      || (mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
#endif
     )
   {
#ifdef FEATURE_WCDMA
      rrc_cmd_type* rrc_cmd_p = NULL;

      if (is_umts_mode())
      {
      rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
      }
#ifdef FEATURE_WRLF_SYSTEM_SEL
      else if(mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
      {
        rrc_cmd_p = mm_get_rrc_cmd_buf( mm_wrlf_serving_plmn_ptr->info.active_rat  );
      }
#endif /* FEATURE_WRLF_SYSTEM_SEL */


      if (rrc_cmd_p == NULL)
      {
         MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
      }   

      rrc_cmd_p->cmd_hdr.cmd_id = RRC_DATA_REQ;


      if (mm_sig_low_priority)
      {
         rrc_cmd_p->cmd.data_req.priority = RRC_NAS_MSG_LOW_PRIO;
      }
      else
      {
         rrc_cmd_p->cmd.data_req.priority = RRC_NAS_MSG_HIGH_PRIO;
      }

      rrc_cmd_p->cmd.data_req.cn_domain_id = RRC_CS_DOMAIN_CN_ID;

      rrc_cmd_p->cmd.data_req.nas_msg.length = length;
#ifndef FEATURE_MODEM_HEAP
      rrc_cmd_p->cmd.data_req.nas_msg.value = (uint8*) mm_mem_alloc(length);
#else
      if (is_wcdma_mode() 
#ifdef FEATURE_WRLF_SYSTEM_SEL
         || ((mm_wcdma_rlf_state != WCDMA_RLF_IDLE) &&
             (mm_wrlf_serving_plmn_ptr->info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS))
#endif
         )
      {  
#ifdef FEATURE_SEGMENT_LOADING
        rrc_cmd_p->cmd.data_req.nas_msg.value = (uint8*) IWCDMA_rrc_malloc_for_external_cmd(ptr_W, length);
#else
        rrc_cmd_p->cmd.data_req.nas_msg.value = (uint8*) rrc_malloc_for_external_cmd(length);
#endif
      }
#ifdef FEATURE_TDSCDMA
      else if (is_tdscdma_mode()
#ifdef FEATURE_WRLF_SYSTEM_SEL
         || ((mm_wcdma_rlf_state != WCDMA_RLF_IDLE) &&
             (mm_wrlf_serving_plmn_ptr->info.active_rat == SYS_RAT_TDS_RADIO_ACCESS))
#endif
         )
      {
        rrc_cmd_p->cmd.data_req.nas_msg.value =
          mm_mem_alloc( rrc_cmd_p->cmd.data_req.nas_msg.length );
      }
#endif /* FEATURE_TDSCDMA */
#endif
      if (rrc_cmd_p->cmd.data_req.nas_msg.value == NULL)
      {
         MSG_FATAL_DS(MM_SUB, "=MM= Memory allocation failed", 0,0,0);
      }

      /* Copy the L3 message contents */
      memscpy((void *)rrc_cmd_p->cmd.data_req.nas_msg.value,
                          (byte)gmm_max_L3_msg_length(MM_CS_DOMAIN),(void *)(nas_msg_p + sizeof(IMH_T)),
                                           length);
      rrc_cmd_p->cmd.data_req.is_emergency_call = FALSE;
      
      /* Incase of emergency call set is_emergency_call to TRUE as 
         pasr of this feature requirement
      */
      if (rrc_cmd_p->cmd.data_req.nas_msg.value[1] == CM_SERVICE_REQUEST)
      {
        if (mm_cnm_srv_est_cause == MM_RR_EMERGENCY_CALL)
        {
          rrc_cmd_p->cmd.data_req.is_emergency_call = TRUE;
        }
#ifdef FEATURE_NAS_ECALL
        if(mm_call_est_cause == NO_CNM_SERVICE)
        {
          mm_call_est_cause = (rrc_cmd_p->cmd.data_req.nas_msg.value[2]) & 0x0F;
        }
        if (mm_call_est_cause == EMERGENCY_CALL_ESTABLISHMENT)
        {
          mm_stop_timer(TIMER_T3242);
          mm_stop_timer(TIMER_T3243);
        }
#endif
      }
#ifdef FEATURE_WRLF_SYSTEM_SEL 
      if (mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
      {
#ifdef FEATURE_MODEM_HEAP
        send_cmd_to_rrc( rrc_cmd_p, mm_wrlf_serving_plmn_ptr->info.active_rat  );
#else
        send_cmd_to_rrc( rrc_cmd_p, mm_wrlf_serving_plmn.info.active_rat  );
#endif
      }
      else
#endif /* FEATURE_WRLF_SYSTEM_SEL */
      if (is_umts_mode())
      {
        send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
      }
#endif /* FEATURE_WCDMA */
   }
   else if (is_gsm_mode())
   {

     /* two new fields, calback_fn and message_length are added  to rr_data_req_T structure 
        for optimization of SMS data during handovers, hence nas_msg_p is replaced with
        rr_data_req_T structure, the old contents are copied to the new structure */

     rr_data_req_T  rr_data_req;
     IMH_T *rr_msg = (IMH_T *)(&rr_data_req);
     memset (&rr_data_req, 0x00, sizeof(rr_data_req_T));
     
     memscpy(&rr_data_req.message_header,sizeof(IMH_T), nas_msg_p, sizeof(IMH_T));
     rr_data_req.l3_message_length = (byte) length;
     rr_data_req.message_header.message_set = MS_MM_RR;
     rr_data_req.message_header.message_id  = (int) RR_DATA_REQ;

     {
       dword payload_size;
       payload_size = sizeof(rr_data_req_T)-sizeof(IMH_T);
       payload_size -= (byte)gmm_max_L3_msg_length(MM_CS_DOMAIN) ;
       payload_size += length;
       PUT_IMH_LEN(payload_size, &rr_data_req.message_header);
     }

            /* PD is the lower nibble */
            pd = nas_msg_p[4] & 0x0F;

            /* Send seq num is only applicable to MM, CC, SS msgs */
            if( pd == PD_MM || pd == PD_CC || pd == PD_SS)
            {
               /*
                * The send seq number in a message type is 0 until it is changed
                * No need to update here as the send seq number is always 0 on
                * the first MM/CC/SS L3 message to the network
                */

               /* Increment the send sequence number
                * and do modulo 4 arithmetic for the next run
                */

               if ((mm_system_information.mscr == 0x0)||
                  (mm_nv_force_mscr_r99 == NV_FORCE_UE_REL97))
               {
                 /* Use only 1bit for Sequence numbering */
                 send_sequence_number_mm_cc_ss = send_sequence_number_mm_cc_ss % 2;
               }
               else if (mm_system_information.mscr == 0x1)
               {
                 send_sequence_number_mm_cc_ss = send_sequence_number_mm_cc_ss % 4;
               }
               else
               {
                 MSG_ERROR_DS( MM_SUB, "=MM= Wrong MSCR information from RR",0,0,0);
               }

               nas_msg_p[5] |= (byte)(send_sequence_number_mm_cc_ss << 6);

               ++send_sequence_number_mm_cc_ss;
             }

     memscpy(&rr_data_req.layer3_message[0],gmm_max_L3_msg_length(MM_CS_DOMAIN), nas_msg_p + sizeof(IMH_T), length);

/* this code is to query cnm whether data is for SMS or not and depending
    on that we set call back function */

     rr_data_req.callback_fn=NULL;

     if(pd==PD_SMS)
     {
       if(is_cp_data(nas_msg_p + sizeof(IMH_T)) == TRUE )
       {
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
         if(mm_sub_id == MM_AS_ID_1)
         {
            rr_data_req.callback_fn=CNM_sms_cp_data_l2_ack;
         }
         else
         {
            rr_data_req.callback_fn=CNM_sms_cp_data_l2_ack_subs_2;
         }
#else
         rr_data_req.callback_fn=CNM_sms_cp_data_l2_ack;
#endif
       }
     }
#ifdef FEATURE_NAS_ECALL
     if ((rr_data_req.layer3_message[1] & 0x3F) == CM_SERVICE_REQUEST)
     {
       if(mm_call_est_cause == NO_CNM_SERVICE)
       {
         mm_call_est_cause =  (rr_data_req.layer3_message[2]) & 0x0F;
       }
       if(mm_call_est_cause == EMERGENCY_CALL_ESTABLISHMENT)
       {
         mm_stop_timer(TIMER_T3242);
         mm_stop_timer(TIMER_T3243);
       }
     }
#endif
     /* Log in GSM mode too */
     mm_send_ota_msg_log_packet(UE_TO_CN_DIRECTION,
                                                  length,
                                                      (byte *)(rr_data_req.layer3_message));



     MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_DATA_REQ");


     mm_send_message( rr_msg, GS_QUEUE_RR );

   }
   else
   {
      MSG_FATAL_DS(MM_SUB, "=MM= Trying to send an OTA message to unknown destination", 0,0,0);
   }
}


/*===========================================================================

FUNCTION MM_SEND_NAS_MSG_TO_RRC_BY_DOMAIN

DESCRIPTION
  This function sends an L3 NAS message to existing RRC connection
  based on the given CN domain ID

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_nas_msg_to_rrc_by_domain(byte *nas_msg_p,
                                      rrc_cn_domain_identity_e_type cn_domain)
{
#ifdef FEATURE_WCDMA

   rrc_cmd_type* rrc_cmd_p = NULL;
   uint32 length;
   uint8   *write_ptr = NULL;
   byte    protocol_discriminator;
   uint16 mm_max_L3_msg_length;
#ifdef FEATURE_SEGMENT_LOADING
   interface_t* ptr_W = NULL;

   ptr_W = get_wcdma_interface();
#endif

   rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );


   if (rrc_cmd_p == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }   
   rrc_cmd_p->cmd_hdr.cmd_id = RRC_DATA_REQ;

   if (mm_sig_low_priority)
   {
      rrc_cmd_p->cmd.data_req.priority = RRC_NAS_MSG_LOW_PRIO;
   }
   else
   {
      rrc_cmd_p->cmd.data_req.priority = RRC_NAS_MSG_HIGH_PRIO;
   }

   rrc_cmd_p->cmd.data_req.cn_domain_id = cn_domain;
   if( cn_domain == RRC_CS_DOMAIN_CN_ID)
   {
      mm_max_L3_msg_length = (byte)gmm_max_L3_msg_length(MM_CS_DOMAIN);
   }
   else
   {
      mm_max_L3_msg_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);
   }

   /* Get message length */
   GET_IMH_LEN( length, (IMH_T*)nas_msg_p);

   rrc_cmd_p->cmd.data_req.nas_msg.length = length;
#ifndef FEATURE_MODEM_HEAP
   rrc_cmd_p->cmd.data_req.nas_msg.value = (uint8*) mm_mem_alloc(length);
#else
   if (is_wcdma_mode())
   {  
#ifdef FEATURE_SEGMENT_LOADING
      rrc_cmd_p->cmd.data_req.nas_msg.value = (uint8*) IWCDMA_rrc_malloc_for_external_cmd(ptr_W, length);
#else
      rrc_cmd_p->cmd.data_req.nas_msg.value = (uint8*) rrc_malloc_for_external_cmd(length);
#endif
   }
#ifdef FEATURE_TDSCDMA
   else if (is_tdscdma_mode())
   {
      rrc_cmd_p->cmd.data_req.nas_msg.value =
        mm_mem_alloc( rrc_cmd_p->cmd.data_req.nas_msg.length );
   }
#endif /* FEATURE_TDSCDMA */
#endif

   if (rrc_cmd_p->cmd.data_req.nas_msg.value == NULL)
   {
      MSG_FATAL_DS(MM_SUB, "=MM= Memory allocation failed", 0,0,0);
   }

   memscpy((void *)rrc_cmd_p->cmd.data_req.nas_msg.value,
                   mm_max_L3_msg_length, (void *)(nas_msg_p + sizeof(IMH_T)),
                                        length);
    rrc_cmd_p->cmd.data_req.is_emergency_call = FALSE;
       
    /* Incase of emergency call set is_emergency_call to TRUE as 
       pasr of this feature requirement
    */
    if ((rrc_cmd_p->cmd.data_req.nas_msg.value[1] == CM_SERVICE_REQUEST) && 
        (mm_cnm_srv_est_cause == MM_RR_EMERGENCY_CALL))
    {
      rrc_cmd_p->cmd.data_req.is_emergency_call = TRUE;
    }

   /*
   ** If the message type is a LOCATION_UPDATE_REQUEST,  overwrite the FOR bit
   ** This is an optimization in case the FOR has been set to TRUE, and the LU_REQUEST
   ** message is not sent yet
   */
   /* move the write pointer to the second byte, which contains the message type*/
   write_ptr = rrc_cmd_p->cmd.data_req.nas_msg.value;
   protocol_discriminator = rrc_cmd_p->cmd.data_req.nas_msg.value[0];
   write_ptr++;

   if ((protocol_discriminator == PD_MM) && (*write_ptr == LOCATION_UPDATING_REQUEST))
   {
       /* move the write pointer to the third byte, which contains the
       ** ciphering key sequence number, the location update type and the FOR
       */
       write_ptr++;
       /*
       ** overwrite the follow-on-request bit
       */
       if (mm_follow_on_request == TRUE)
       {
           *write_ptr |= 0x08;
           MSG_HIGH_DS_0(MM_SUB, "=MM= FOR is 1 in LOCATION_UPDATING_REQUEST");
       }
       /*
       ** Starting T3210 for LU procedure for an existing RRC connection
       */
       mm_start_timer( TIMER_T3210, DEFAULT_TIMEOUT );
   }

#ifdef FEATURE_GTOW_PS_HANDOVER
   if ((cn_domain == RRC_PS_DOMAIN_CN_ID)
#ifdef FEATURE_NAS_REL11
       && ((rrc_cmd_p->cmd.data_req.nas_msg.value[1] == GMM_ATTACH_REQUEST) ||
           (rrc_cmd_p->cmd.data_req.nas_msg.value[1] == GMM_ROUTING_AREA_UPDATE_REQUEST)||
           (rrc_cmd_p->cmd.data_req.nas_msg.value[1] == GMM_DETACH_REQUEST)||
           (rrc_cmd_p->cmd.data_req.nas_msg.value[1] == GMM_SERVICE_REQUEST))
#endif         
   )   
   {
     /* In GtoW PSHO case, we do not have to send an IDT before sending RAU procedure because the session is already open. So, this is one of a case 
     where RRC_DATA_REQ can be sent without RRC_OPEN_SESS_REQ and hence this variable would not be properly updated. So, doing it here.*/
     gmm_last_signalling_sent_rat = mm_serving_plmn.info.active_rat;
   }
#endif


   send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
#endif /* FEATURE_WCDMA */

}


/*===========================================================================

FUNCTION HAS_VALID_TMSI

DESCRIPTION
  This function determines the validity of stored TMSI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean has_valid_tmsi(void)
{
   dword count = 0;
   for ( count = 0; count < TMSI_SIZE; count++ )
   {
      if (mm_location_information.tmsi[count] != 0xff )
      {
         return TRUE;
      }
   }
   return FALSE;
}


/*===========================================================================

FUNCTION GET_TMSI_AS_NUMBER

DESCRIPTION
  This function converts TMSI to a decimal number to be used on certain
  RRC interfaces that expects TMSI as a number

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

uint32 get_tmsi_as_number(void)
{
   byte *tmsi = mm_location_information.tmsi;
   if( has_valid_tmsi() == FALSE)
      return 0;

   return (uint32)
              (tmsi[0] & 0x0F)         * 10000000
            + ((tmsi[0] & 0xF0) >> 4)  * 1000000
            + (tmsi[1] & 0x0F)         * 100000
            + ((tmsi[1] & 0xF0) >> 4)  * 10000
            + (tmsi[2] & 0x0F)         * 1000
            + ((tmsi[2] & 0xF0) >> 4)  * 100
            + (tmsi[3] & 0x0F)         * 10
            + ((tmsi[3] & 0xF0) >> 4)  * 1;
}


/*===========================================================================

FUNCTION CONVERT_IMSI_TO_DIGITS

DESCRIPTION
  This function converts IMSI to BCD digit array as expected by RRC layer

DEPENDENCIES
  None

RETURN VALUE
  IMSI of type rrc_imsi_gsm_map_type

SIDE EFFECTS
  None

===========================================================================*/

rrc_imsi_gsm_map_type convert_imsi_to_digits(imsi_data_T imsi)
{
   byte odd_digits;
   byte rrc_index = 0;
   byte index = 0;
   rrc_imsi_gsm_map_type rrc_imsi_local = {0};

   if(imsi.length == 0)
   {
      MSG_ERROR_DS(MM_SUB, "=MM= No valid IMSI present to convert", 0,0,0);
      rrc_imsi_local.num_digits = 0;
      return rrc_imsi_local;
   }

   odd_digits = imsi.data[0] & 0x08;

   if( odd_digits)
      rrc_imsi_local.num_digits = imsi.length *2 - 1;
   else
      rrc_imsi_local.num_digits = imsi.length *2 - 2;


   rrc_imsi_local.value[rrc_index++] = UPPER_BCD(imsi.data[0]);

   /* start from the 2nd byte of IMSI value */
   for(index = 1; index < imsi.length; index++)
   {
      if((rrc_index >= (RRC_MAX_GSMMAP_IMSI_DIGITS-1)) ||
         (index >= IMSI_LEN_IN_BYTES))/*size of imsi.data array is IMSI_LEN_IN_BYTES */
      {
         rrc_imsi_local.num_digits = 0;
         MSG_ERROR_DS(MM_SUB,"=MM= Index out of bound and buffer overflow, rrc_index %d and index %d",rrc_index,index,0);
         break;
      }
      else if( index == imsi.length -1) /* last byte */
      {
         rrc_imsi_local.value[rrc_index++] = LOWER_BCD( imsi.data[index] );
         if(odd_digits) /* count the last one if odd digits */
            rrc_imsi_local.value[rrc_index++] = UPPER_BCD( imsi.data[index] );
      }
      else
      {
         rrc_imsi_local.value[rrc_index++] = LOWER_BCD( imsi.data[index] );
         rrc_imsi_local.value[rrc_index++] = UPPER_BCD( imsi.data[index] );
      }
   }

   return rrc_imsi_local;
}


/*===========================================================================

FUNCTION GET_IMEI_AS_DIGITS

DESCRIPTION
  This function converts IMEI to RRC IMEI digit array type

DEPENDENCIES
  None

RETURN VALUE
  IMEI of the type rrc_imei_gsm_map_type

SIDE EFFECTS
  None

===========================================================================*/

rrc_imei_gsm_map_type get_imei_as_digits(void)
{
   rrc_imei_gsm_map_type rrc_imei;
   byte imei[MAX_IMEI_LEN+1];
   byte index = 0;
   byte rrc_index = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   mm_read_imei( imei );

   if (imei[0] != MAX_IMEI_LEN)
   {
      MSG_ERROR_DS( MM_SUB, "=MM= IMEI length must be 8", 0,0,0 );

      memset(rrc_imei.value, 0, RRC_NUM_IMEI_DIGITS);
      return rrc_imei;
   }

   rrc_imei.value[rrc_index++] = UPPER_BCD(imei[1]);

   /* Start from the 2nd byte of IMEI value */
   for (index = 2; index < MAX_IMEI_LEN+1 && rrc_index < RRC_NUM_IMEI_DIGITS - 1; index++)
   {
      rrc_imei.value[rrc_index++] = LOWER_BCD( imei[index] );
      rrc_imei.value[rrc_index++] = UPPER_BCD( imei[index] );
   }

   return rrc_imei;
}


/*===========================================================================

FUNCTION MMUMTS_CONVERT_RRC_PLMN_ID_TO_NAS_PLMN_ID

DESCRIPTION
  This function converts from the RRC PLMN ID format (1 BCD digit per byte)
  to the NAS PLMN_id_T format (2 BCD digits per byte).

DEPENDENCIES
  None

RETURN VALUE
  PLMN ID converted to PLMN_id_T format (3 byte array - 2 BCD digits/byte)

SIDE EFFECTS
  None

===========================================================================*/

sys_plmn_id_s_type mmumts_convert_rrc_plmn_id_to_nas_plmn_id( rrc_plmn_identity_type plmn_id )
{
   sys_plmn_id_s_type nas_plmn_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if (plmn_id.num_mnc_digits == 2)
   {
      plmn_id.mnc[2] = 0x0F;
   }

   /* ----------------------------------------
   ** PLMN octet 1 = MCC digit 2 | MCC digit 1
   ** ---------------------------------------- */
   nas_plmn_id.identity[0] = (byte) ((plmn_id.mcc[1] << 4) + plmn_id.mcc[0]);

   /* ----------------------------------------
   ** PLMN octet 2 = MNC digit 3 | MCC digit 3
   ** ---------------------------------------- */
   nas_plmn_id.identity[1] = (byte) ((plmn_id.mnc[2] << 4) + plmn_id.mcc[2]);

   /* ----------------------------------------
   ** PLMN octet 3 = MNC digit 2 | MNC digit 1
   ** ---------------------------------------- */
   nas_plmn_id.identity[2] = (byte) ((plmn_id.mnc[1] << 4) + plmn_id.mnc[0]);

   return nas_plmn_id;

} /* end mmumts_convert_rrc_plmn_id_to_nas_plmn_id() */

/* <EJECT> */
/*===========================================================================

FUNCTION MMUMTS_CONVERT_NAS_PLMN_ID_TO_RRC_PLMN_ID

DESCRIPTION
  This function converts from the NAS PLMN ID format (2 BCD digits per byte)
  to the RRC PLMN_id_T format (1 BCD digit per byte).

DEPENDENCIES
  None

RETURN VALUE
  PLMN ID converted to rrc_plmn_id_type format

SIDE EFFECTS
  None

===========================================================================*/

rrc_plmn_identity_type mmumts_convert_nas_plmn_id_to_rrc_plmn_id( sys_plmn_id_s_type plmn_id )
{
   rrc_plmn_identity_type rrc_plmn_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* ----------------------------------------
   ** PLMN octet 1 = MCC digit 2 | MCC digit 1
   ** ---------------------------------------- */
   rrc_plmn_id.mcc[0] = plmn_id.identity[0] & 0x0F;
   rrc_plmn_id.mcc[1] = (plmn_id.identity[0] & 0xF0) >> 4;

   /* ----------------------------------------
   ** PLMN octet 2 = MNC digit 3 | MCC digit 3
   ** ---------------------------------------- */
   rrc_plmn_id.mcc[2] = plmn_id.identity[1] & 0x0F;

   if ((plmn_id.identity[1] & 0xF0) == 0xF0)
   {
      rrc_plmn_id.num_mnc_digits = 2;
      rrc_plmn_id.mnc[2] = 0x0F; /* Set the 3rd digit to 0xF as per the spec */
   }
   else
   {
      rrc_plmn_id.num_mnc_digits = 3;
      rrc_plmn_id.mnc[2] = (plmn_id.identity[1] & 0xF0) >> 4;
   }

   /* ----------------------------------------
   ** PLMN octet 3 = MNC digit 2 | MNC digit 1
   ** ---------------------------------------- */
   rrc_plmn_id.mnc[0] = plmn_id.identity[2] & 0x0F;
   rrc_plmn_id.mnc[1] = (plmn_id.identity[2] & 0xF0) >> 4;

   return rrc_plmn_id;

} /* end mmumts_convert_rrc_plmn_id_to_nas_plmn_id() */


/*===========================================================================

FUNCTION MM_EXTRACT_LAC_FROM_SIB_INFO

DESCRIPTION
  This function extracts LAC into rrc_lai_type from the NAS common info

DEPENDENCIES
  None

RETURN VALUE
  TRUE on success
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean  mm_extract_lac_from_sib_info(rrc_lai_type *lai,
                                      rrc_gsm_map_nas_sys_info_type *nas_common_info)
{
   if( nas_common_info->length < 2)
   {
      return FALSE;
   }

   /* Copy the first two bytes which is LAC */
   lai->lac[0] = nas_common_info->value[0];
   lai->lac[1] = nas_common_info->value[1];

   return TRUE;
}

/*===========================================================================

FUNCTION MM_WAITING_FOR_NW_COMMAND

DESCRIPTION
  This function is called by RRC to see if the MM is in state 9 and decide 
  to release the RRC connection if a Radio link failure is detected and may
  abort Cell update procedures to quickly return the UE to Idle state so that
  pages can be received during a quick MT page calls.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  when mm_state == MM_WAIT_FOR_NETWORK_COMMAND OR MM_IDLE
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_waiting_for_nw_command()
{

#ifdef FEATURE_SGLTE
  return mm_per_subs_waiting_for_nw_command((sys_modem_as_id_e_type)mm_ps_stack_id);
#else
  if (((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) && mm_cnm_rel_req_received) ||
       (mm_state == MM_IDLE))       
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }  
#endif
}

/*===========================================================================

FUNCTION MM_PER_NAS_STACKS_WAITING_FOR_NW_COMMAND

DESCRIPTION
  This function uses the stack_id_nas to check if current mm state is 9

DEPENDENCIES
  None

RETURN VALUE
  TRUE  when mm_state == MM_WAIT_FOR_NETWORK_COMMAND OR MM_IDLE
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_per_nas_stacks_waiting_for_nw_command
(
  sys_modem_as_id_e_type stack_id_nas
)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if(IS_NOT_VALID_STACK_ID(stack_id_nas))
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", stack_id_nas+1,0,0);
    return FALSE;
  }
  if (((mm_state_sim[stack_id_nas] == MM_WAIT_FOR_NETWORK_COMMAND) && mm_cnm_rel_req_received_sim[stack_id_nas]) ||
       (mm_state_sim[stack_id_nas] == MM_IDLE))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }  
#else
  (void)stack_id_nas;
  return mm_waiting_for_nw_command();
#endif
}

/*===========================================================================

FUNCTION MM_PER_SUBS_WAITING_FOR_NW_COMMAND

DESCRIPTION
  This function is called by RRC to see if the MM is in state 9 and decide 
  to release the RRC connection if a Radio link failure is detected and may
  abort Cell update procedures to quickly return the UE to Idle state so that
  pages can be received during a quick MT page calls.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  when mm_state == MM_WAIT_FOR_NETWORK_COMMAND OR MM_IDLE
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_per_subs_waiting_for_nw_command
(
  sys_modem_as_id_e_type sub_id
)
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if(IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Invalid SUB ID:%d", sub_id+1,0,0);
    return FALSE;
  }
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif
  return mm_per_nas_stacks_waiting_for_nw_command(stack_id_nas);
}

/*===========================================================================

FUNCTION MM_READ_EMERGENCY_IMSI
DESCRIPTION
  This function will give imsi even if SIM is NOT AVAILABLE, and emergency call is started
DEPENDENCIES
  None

RETURN VALUE
  Returns IMSI VALUE

SIDE EFFECTS
  None

===========================================================================*/
 
 imsi_data_T mm_read_emergency_imsi()
 {
    mmgsdi_session_info_query_type  session_info_query;
    mmgsdi_session_get_info_type        session_get_info_ptr;
    mmgsdi_file_enum_type    sim_file_name_imsi = MMGSDI_NO_FILE_ENUM;
    imsi_data_T         imsi_data;
      
    rrc_imsi_gsm_map_type  imsi_data_digits;
    byte index=0;
    mmgsdi_app_enum_type  sim_app_type = MMGSDI_APP_NONE;
    session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
    session_info_query.query_key.session_type = mm_sim_mmgsdi_info.session_type;
    if (MMGSDI_SUCCESS == mmgsdi_session_get_info_sync(session_info_query, &session_get_info_ptr))
    {
      sim_app_type = session_get_info_ptr.app_data.app_type;
    }
    if( sim_app_type == MMGSDI_APP_SIM)
    {
     sim_file_name_imsi = MMGSDI_GSM_IMSI;
    }
    else if( sim_app_type == MMGSDI_APP_USIM)
    {
    sim_file_name_imsi = MMGSDI_USIM_IMSI;
    }
    memset( (void *)&imsi_data, 0, sizeof(imsi_data_T));
    if( (sim_file_name_imsi == MMGSDI_USIM_IMSI || sim_file_name_imsi == MMGSDI_GSM_IMSI ) &&
        (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_NOT_AVAILABLE))
    {
      if(session_get_info_ptr.app_state == MMGSDI_APP_STATE_ILLEGAL)
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM= SIM is illegal so sending IMEI");
      }
      else
      {
        if( mm_sim_read( sim_file_name_imsi,
                      (byte *) &imsi_data,
                      MMGSDI_IMSI_LEN
                      ))
        {
          imsi_data_digits = convert_imsi_to_digits(imsi_data);
          /*    Validate only MCC & MNC of HPLMN here   */
          if(imsi_data.length > IMSI_LEN_IN_BYTES)
          {
            imsi_data.length = 0;
            MSG_HIGH_DS_0(MM_SUB,"=MM= IMSI is illegal so sending IMEI");
          }
          for(;index < imsi_data_digits.num_digits;index++)
          {
            if(imsi_data_digits.value[index] >= 0x0A )
            {
              imsi_data.length = 0;
              MSG_HIGH_DS_0(MM_SUB,"=MM= IMSI is illegal so sending IMEI");
              break;
            }
            
          }
        }
      }
    }

    return imsi_data;
}
